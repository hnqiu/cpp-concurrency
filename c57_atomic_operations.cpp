#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>


/*!
 * \brief spin lock with atomic_flag
 * \note atomic_flag is processor-intensive and ideally the critical section
 * should only be accessed by one thread, otherwise if multiple threads are
 * interrupting each other the performance is heavily impacted.
 * atomic_flag is usually used in systems and libraries.
 * 
 * atomic_flag is often used to implement a hybrid mutex. It starts with a spin
 * lock with a time out. If the thread sets the flag in time, it enters the
 * critical section. This gives better performance than using a mutex. If the
 * thread cannot set the flag in time, it will then use the normal mutex
 * implementation.
 */

std::atomic_flag lock_cout{ATOMIC_FLAG_INIT};

void foo(int i)
{
    // wait until lock_cout is not set by other threads
    while (lock_cout.test_and_set()) { }
    // start critical section
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Thread " << i << " gets into critical section." << std::endl;
    lock_cout.clear();
}

int main()
{
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 10; ++i)
    {
        threads.push_back(std::thread{foo, i});
    }

    for (auto& t : threads)
    {
        t.join();
    }

    return 0;
}
