#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>


std::string sdata{"Empty"};
std::mutex mut;
std::condition_variable cond;
bool condition{false};

void reader(size_t i)
{
    std::cout << "Reader thread " << i << " has started..." << std::endl;
    std::unique_lock<std::mutex> lock{mut};

    std::cout << "Reader waiting for data" << std::endl;
    cond.wait(lock, [](){ return condition; });

    std::cout << "Reader " << i << " reading data: " << sdata << std::endl;
}

void writer()
{
    std::cout << "Writer thread has started..." << std::endl;
    {
        std::lock_guard<std::mutex> lock{mut};
        sdata = "some data";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        condition = true;
    }
    std::cout << "Writer thread sending notification..." << std::endl;
    cond.notify_all();
    // or
    // for (size_t i = 0; i < 3; ++i)
    // {
    //     cond.notify_one();
    // }
    
}

int main()
{
    std::cout << "Initial data is '" << sdata << "'" << std::endl;

    std::vector<std::thread> threads;
    for (size_t i = 0; i < 3; ++i)
    {
        threads.push_back(std::thread{reader, i});
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    threads.push_back(std::thread{writer});

    for (auto& t : threads)
    {
        t.join();
    }
    return 0;
}
