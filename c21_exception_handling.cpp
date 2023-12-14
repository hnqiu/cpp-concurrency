#include <iostream>
#include <thread>
#include <exception>

void foo()
{
    // this is wrong because exception should be handled in the same thread
    throw std::exception();
    std::cout << "foo" << std::endl;
}

int main()
{
    // exceptions cannot be caught by other threads
    // the program will terminate as the thrown exception is not properly handled
    try
    {
        std::thread t{foo};
        t.join();
    }
    catch(const std::exception& e)
    {
        std::cerr << "exception caught: " << e.what() << '\n';
    }

    return 0;
}
