#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>


/*!
 * \brief std::async
 * \note a higher level of abstraction of std::thread and similar to
 * std::packaged_task to some extent.
 * 
 * std::async() returns an std::future object.
 * 
 * Launch policy includes std::launch::async | std::launch::deferred
 * https://en.cppreference.com/w/cpp/thread/launch
 * - std::launch::async: task is executed on a new thread immediately. With
 * this policy, the task is implicitly joined when we leave the scope. The
 * returned future's destructor will block until the task completes.
 * - std::launch::deferred: task is executed on the first calling thread when
 * the result is requested (i.e. calling get()). This is called lazy execution.
 * 
 * By default both flags are set, the impl decides whether to start a new
 * thread, so it's unclear which thread it runs on and when it runs. It will be
 * problematic if thread-local data is used.
 */

unsigned long long fibonacci(unsigned long n)
{
    if (n <= 1)
    {
        return 1;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main()
{
    unsigned long n{40};
    std::cout << "calling fibonacci(" << n << ")" << std::endl;

    auto result_future = std::async(fibonacci, n);

    using namespace std::literals;
    // calling wait() won't execute the async call if the policy is deferred
    while (result_future.wait_for(100ms) != std::future_status::ready)
    {
        std::cout << "Waiting for result...\n";
    }
    std::cout << "result is " << result_future.get() << std::endl;

    // similar to promise and future, we can use a try-catch block to capture
    // exceptions thrown by the async call:
    /*
    try
    {
        int x = result_future.get();
        // something else
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    */

    return 0;
}
