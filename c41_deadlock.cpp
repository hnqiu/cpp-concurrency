#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>


// scoped_lock since C++17: locks mutexes simultaneously
// std::scoped_lock l(mut1, mut2);
// before C++17 we can use std::lock(mut1, mut2);
// rule of thumb: lock mutexes in one operation
// if mutexes cannot be locked in a single operation,
// organise the mutexes in hierarchical order for the threads to acquire

std::mutex mut1;
std::mutex mut2;

void fooA()
{
    std::cout << "fooA trying to lock mutexes 1 and 2\n";
    std::lock(mut1, mut2);
    std::cout << "fooA has locked mutexes 1 and 2\n";

    std::unique_lock<std::mutex> lck1(mut1, std::adopt_lock);
    std::unique_lock<std::mutex> lck2(mut2, std::adopt_lock);
    // alternatively, use
    // std::unique_lock<std::mutex> lck1(mut1, std::defer_lock);
    // std::unique_lock<std::mutex> lck2(mut2, std::defer_lock);
    // first, then call
    // std::lock(mut1, mut2);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "fooA releasing mutexes 1 and 2\n";
}

void fooB()
{
    std::cout << "fooB trying to lock mutexes 1 and 2\n";
    std::lock(mut2, mut1); // try to acquire mutexes in reverse order
    std::cout << "fooB has locked mutexes 1 and 2\n";

    std::unique_lock<std::mutex> lck1(mut1, std::adopt_lock);
    std::unique_lock<std::mutex> lck2(mut2, std::adopt_lock);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "fooB releasing mutexes 1 and 2\n";
}


int main()
{
    std::thread t1{fooA};
    std::thread t2{fooB};
    t1.join();
    t2.join();

    return 0;
}
