#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

// compare this with c28_mutex.cpp

using namespace std::literals;

std::timed_mutex m;

void foo1()
{
    std::cout << "foo1 locking mutex" << std::endl;
    m.lock();
    std::cout << "foo1 locked mutex" << std::endl;
    std::this_thread::sleep_for(5s);
    std::cout << "foo1 unlocking mutex" << std::endl;
    m.unlock();
}

void foo2()
{
    std::this_thread::sleep_for(100ms);
    std::cout << "foo2 locking mutex" << std::endl;
    while (!m.try_lock_for(1s))
    {
        std::cout << "foo2 cannot acquire mutex" << std::endl;
    }
    std::cout << "foo2 locked mutex" << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "foo2 unlocking mutex" << std::endl;
    m.unlock();
}

// --------------------------- //

void goo1()
{
    std::cout << "goo1 locking mutex" << std::endl;
    std::unique_lock<std::timed_mutex> l(m);
    std::cout << "goo1 locked mutex" << std::endl;
    std::this_thread::sleep_for(5s);
    std::cout << "goo1 unlocking mutex" << std::endl;
}

void goo2()
{
    std::this_thread::sleep_for(100ms);
    std::cout << "goo2 locking mutex" << std::endl;

    std::unique_lock<std::timed_mutex> l(m, std::defer_lock);
    while (!l.try_lock_for(1s))
    {
        std::cout << "goo2 cannot acquire mutex" << std::endl;
    }
    std::cout << "goo2 locked mutex" << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "goo2 unlocking mutex" << std::endl;
}

// --------------------------- //

int main()
{
    std::thread t1{foo1};
    std::thread t2{foo2};

    t1.join();
    t2.join();

    std::cout << "\n\n";

    std::thread t3{goo1};
    std::thread t4{goo2};

    t3.join();
    t4.join();
    return 0;
}
