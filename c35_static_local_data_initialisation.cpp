#include <iostream>
#include <thread>
#include <vector>

/*!
 * \brief test thread-safe initialisation of static local variables
 * \note static local variable initialisation in modern C++ is well-defined.
 *       only one thread can initialise the variable.
 *       the other threads that reach the declaration will be blocked
 *       and wait until the first thread has finished the initialisation.
 */

class Singleton
{
public:
    static Singleton* getSingleton()
    {
        // this method is not thread-safe. There will be data race.
        if (single == nullptr)
        {
            single = new Singleton;
        }
        return single;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
private:
    Singleton() { std::cout << "Initialising Singleton" << std::endl; }
    static Singleton* single;
};

// this is thread-safe
// Singleton& getSingleton()
// {
//    static Singleton single;
//    return single;
// }


void foo()
{
    // The Singleton::getSingleton() method is not thread-safe.
    // There will be multiple instances of Singleton.
    // Instead, we can define a non-member function (see L35-40)
    // and use it to get the Singleton instance.
    // (the Singleton constructor (L31) needs to become public.
    // Line 58 is not needed as well.)
    // This is guaranteed to be thread-safe
    // Singleton& single = getSingleton();
    Singleton* single = Singleton::getSingleton();
    std::cout << "singleton address: " << single << std::endl;
}


Singleton* Singleton::single = nullptr;

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

    return 0;
}
