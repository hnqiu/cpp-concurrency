#include <chrono>
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
 * This is not a great implementation as there're too many explicit locking
 * and unlocking, and sleeps. A better solution is to use condition variables
 * for inter-thread signaling.
 */

using Mutex = std::shared_timed_mutex;
using ReadLock = std::shared_lock<Mutex>;
using WriteLock = std::unique_lock<Mutex>;

Mutex data_mutex;
std::string sdata;
bool update_progress{false};

Mutex completed_mutex;
bool completed{false};

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
    }

    WriteLock completed_lock{completed_mutex};
    completed = true;
    completed_lock.unlock();
    std::cout << "Data fetch thread ends" << std::endl;
}

void progress_bar()
{
    size_t len{0};
    while (true)
    {
        std::cout << "Progress bar displaying...\n";

        ReadLock data_lock{data_mutex};
        while (!update_progress)
        {
            data_lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            data_lock.lock();
        }
        len = sdata.size();
        update_progress = false;
        data_lock.unlock();
        std::cout << "Received " << len << " bytes of data so far" << std::endl;

        ReadLock completed_lock{completed_mutex};
        if (completed)
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
    while (!completed)
    {
        completed_lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        completed_lock.lock();
    }
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
