#include <iostream>
#include <mutex>
#include <thread>


/*!
 * \note thread-safe lazy initialisation
 *       - use mutex
 *       - use double-check locking with C++17 compiler or later
 *       - std::call_once
 *       - singleton with static local variable (see c35.cpp)
 *       The latter two are recommended
 */

class Test
{
public:
    Test() { std::cout << "Test constructed" << std::endl; }
    ~Test() { std::cout << "Test destructed" << std::endl; }
    void func() { std::cout << "func called" << std::endl; }
};


Test* ptest{nullptr};
std::once_flag f;

// the new statement (`ptest = new Test`) involves 3 operations:
//   - allocate memory
//   - construct object in the memory
//   - store object address in pointer
// the order of this initialisation is undefined until C++17
void process()
{
    // call_once: the provided function is guaranteed to be called only once
    // the thread cannot be interrupted until the call completes
    std::call_once(f, []() { ptest = new Test; });
    ptest->func();
}

int main()
{
    std::thread t1{process};
    std::thread t2{process};
    t1.join();
    t2.join();

    delete ptest;
    return 0;
}
