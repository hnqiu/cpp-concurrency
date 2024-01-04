#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>


/*!
 * \brief atomic
 * \note Although integer operations could be single instructions (e.g. ++i is
 * a single instruction), at processor level, it could involve 3 operations:
 * - pre-fetch the value
 * - increment the value in the processor core's register
 * - publish the new value to the cache system
 * Other threads can interleave between these operations.
 * 
 * The parameter of atomics must be trivially copyable. Usually it's integer
 * types or pointers.
 * Atomic types cannot be dereferenced (. or ->). It should be copied to a
 * non-atomic pointer to be accessed.
 */

// int x{0};
std::atomic<int> x{0};

void foo()
{
    for (size_t i = 0; i < 100'000; ++i)
    {
        ++x;
    }
}

int main()
{
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 10; ++i)
    {
        threads.push_back(std::thread{foo});
    }

    for (auto& t : threads)
    {
        t.join();
    }
    std::cout << "x = " << x << std::endl;

    return 0;
}
