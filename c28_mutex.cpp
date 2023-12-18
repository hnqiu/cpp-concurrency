#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std::literals;
std::mutex m;

void foo1()
{
    std::cout << "foo1 locking mutex" << std::endl;
    m.lock();
    std::cout << "foo1 locked mutex" << std::endl;
    std::this_thread::sleep_for(500ms);
    std::cout << "foo1 unlocking mutex" << std::endl;
    m.unlock();
}

void foo2()
{
    std::this_thread::sleep_for(100ms);
    std::cout << "foo2 locking mutex" << std::endl;
    while (!m.try_lock())
    {
        std::cout << "foo2 cannot acquire mutex" << std::endl;
        std::this_thread::sleep_for(100ms);
    }
    std::cout << "foo2 locked mutex" << std::endl;
    std::this_thread::sleep_for(100ms);
    std::cout << "foo2 unlocking mutex" << std::endl;
    m.unlock();
}

int main()
{
    std::thread t1{foo1};
    std::thread t2{foo2};

    t1.join();
    t2.join();
    return 0;
}
