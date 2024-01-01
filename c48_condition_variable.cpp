#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>


std::string sdata{"Empty"};
std::mutex mut;
std::condition_variable cond;
// std::condition_variable only works with standard std::mutex
// there is also std::condition_variable_any to work with any mutex-like object

void reader()
{
    std::cout << "Reader thread has started..." << std::endl;
    std::unique_lock<std::mutex> lock{mut};

    std::cout << "Reader waiting for data" << std::endl;
    cond.wait(lock);

    std::cout << "Reader reading data: " << sdata << std::endl;
}

void writer()
{
    std::cout << "Writer thread has started..." << std::endl;
    {
        std::lock_guard<std::mutex> lock{mut};
        sdata = "some data";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Writer thread sending notification..." << std::endl;
    cond.notify_one();
}

int main()
{
    std::cout << "Initial data is '" << sdata << "'" << std::endl;

    // reader has to start before writer, otherwise notification will be lost.
    // this is called "lost wakeup".
    std::thread t1{reader};
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::thread t2{writer};

    t1.join();
    t2.join();
    return 0;
}
