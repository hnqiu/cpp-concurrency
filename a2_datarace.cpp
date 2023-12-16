#include <iostream>
#include <thread>

int data{0};

void foo(int& d)
{
    for (size_t i = 0; i < 100000; ++i)
    {
        ++d;
    }
}


int main()
{
    // data race
    // data is shared between all threads and there is no synchronisation
    // a thread can be interrupted between reading the old value and writing the new value
    // which causes the final value of the data is much lower than expected
    std::thread t1{foo, std::ref(data)};
    std::thread t2{foo, std::ref(data)};
    std::thread t3{foo, std::ref(data)};

    t1.join();
    t2.join();
    t3.join();
    std::cout << "Result: " << data << std::endl;

    return 0;
}
