#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>


/*!
 * \brief std::future & std::promise
 * \note A promise stores a result; a future gets the result. Together they
 * create a shared state.
 * 
 * std::future objects are usually created by std::promise, or an asynchronous
 * operation.
 * std::promise::set_exception() can share exceptions inter-threads.
 * 
 * std::future assumes exclusive read access to the data, so it's used with
 * single consumer thread. It is move only.
 */

void produce(std::promise<int>& promise)
{
    try
    {
        int x{42};
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // throw an exception under a certain condition
        if (0) // or 1
        {
            throw std::out_of_range("Error: out of range!");
        }
        std::cout << "Promise sets shared state to " << x << std::endl;
        promise.set_value(x);
    }
    catch(const std::exception& e)
    {
        promise.set_exception(std::current_exception());
    }
}

void consume(std::future<int>& future)
{
    std::cout << "Consumer getting future value...\n";
    try
    {
        int x = future.get();
        std::cout << "Consumer has got value " << x << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

// a cleaner way to throw exceptions
void produce_alternative(std::promise<int>& promise)
{
    int x{42};
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (1)
    {
        promise.set_exception(
            std::make_exception_ptr(std::out_of_range("Out of range!")));
        return;
    }
    std::cout << "Promise sets shared state to " << x << std::endl;
    promise.set_value(x);
}

int main()
{
    {
        std::promise<int> prom;
        std::future<int> fut = prom.get_future();
        std::thread t1{produce, std::ref(prom)};
        std::thread t2{consume, std::ref(fut)};

        t1.join();
        t2.join();
    }
    std::cout << std::endl;

    {
        std::promise<int> prom;
        std::future<int> fut = prom.get_future();
        std::thread t1{produce_alternative, std::ref(prom)};
        std::thread t2{consume, std::ref(fut)};

        t1.join();
        t2.join();
    }

    return 0;
}
