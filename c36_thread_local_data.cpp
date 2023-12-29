#include <cassert>
#include <iostream>
#include <random>
#include <thread>

/*!
 * \brief test thread-local variables
 */

thread_local std::mt19937 mt;
constexpr size_t NUM{8};

void foo(double* array)
{
    std::uniform_real_distribution<double> dist(0, 1);
    for (size_t i = 0; i < NUM; ++i)
    {
        array[i] = dist(mt);
        std::cout << array[i] << "\t";
    }
    std::cout << std::endl;
}


int main()
{
    double a1[NUM];
    double a2[NUM];
    std::thread t1{foo, a1};
    t1.join();

    std::thread t2{foo, a2};
    t2.join();

    for (size_t i = 0; i < NUM; ++i)
    {
        assert(a1[i] == a2[i]);
    }

    return 0;
}
