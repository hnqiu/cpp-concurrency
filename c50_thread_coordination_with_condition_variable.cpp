#include <chrono>
#include <condition_variable>
#include <iostream>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>


/*!
 * \brief simulate a download program
 * \note a program with 3 threads:
 * - a download thread,
 * - a progress bar display thread, and
 * - a data process thread after the download completes
 * 
 * This is an improvement on c47.cpp
 */

using Mutex = std::shared_timed_mutex;
using ReadLock = std::shared_lock<Mutex>;
using WriteLock = std::unique_lock<Mutex>;

Mutex data_mutex;
std::string sdata;
bool update_progress{false};
std::condition_variable_any data_cond;

Mutex completed_mutex;
bool completed{false};
std::condition_variable_any completed_cond;

void fetch_data()
{
    for (size_t i = 0; i < 5; ++i)
    {
        std::cout << "Fetching data...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));

        WriteLock data_lock{data_mutex};
        sdata += "Block" + std::to_string(i);
        std::cout << "sdata: " << sdata << std::endl;
        update_progress = true;
        data_lock.unlock();
        data_cond.notify_all();
    }

    WriteLock completed_lock{completed_mutex};
    completed = true;
    completed_lock.unlock();
    completed_cond.notify_all();
    std::cout << "Data fetch thread ends" << std::endl;
}

void progress_bar()
{
    size_t len{0};
    while (true)
    {
        std::cout << "Progress bar displaying...\n";

        ReadLock data_lock{data_mutex};
        data_cond.wait(data_lock, [](){ return update_progress; });
        len = sdata.size();
        update_progress = false;
        data_lock.unlock();
        std::cout << "Received " << len << " bytes of data so far" << std::endl;

        ReadLock completed_lock{completed_mutex};
        if (completed_cond.wait_for(completed_lock,
                                    std::chrono::milliseconds(10),
                                    [](){ return completed; }))
        {
            std::cout << "Progress bar thread ends" << std::endl;
            break;
        }
    }
}

void process_data()
{
    std::cout << "Processing thread waiting for data...\n";

    ReadLock completed_lock{completed_mutex};
    completed_cond.wait(completed_lock, [](){ return completed; });
    completed_lock.unlock();

    ReadLock data_lock{data_mutex};
    std::cout << "Processing data " << sdata << std::endl;
}


int main()
{
    std::vector<std::thread> threads;
    threads.push_back(std::thread{fetch_data});
    threads.push_back(std::thread{progress_bar});
    threads.push_back(std::thread{process_data});

    for (auto& t : threads)
    {
        t.join();
    }

    return 0;
}
