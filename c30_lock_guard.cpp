#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

std::mutex m;

void foo(std::string str)
{
    try
    {
        // std::lock_guard<std::mutex> l(m);
        // use unique lock instead so that the mutex can be unlocked
        // immediately after the critical section
        std::unique_lock<std::mutex> l(m);
        // more: https://en.cppreference.com/w/cpp/thread/lock_tag
        std::cout << str[0] << " " << str[1] << " " << str[2] << std::endl;
        l.unlock();
        // throw std::exception();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception occurred: " << e.what() << '\n';
    }
}

int main()
{
    std::thread t1{foo, "noweg"};
    std::thread t2{foo, "dgoyw"};

    t1.join();
    t2.join();
    return 0;
}
