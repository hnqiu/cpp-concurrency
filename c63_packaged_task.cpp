#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>


/*!
 * \brief c62 note: synchronisation in asynchronous programming
 * \note
 * Blocking synchronisation is done via mutexes or atomic operations.
 * Non-blocking synchronisation can be achieved by concurrent message queues.
 * 
 */


/*!
 * \brief c63: packaged_task
 * \note a higher level of abstraction of std::promise.
 * It encapsulates a callable object and a promise for the result of the task.
 * 
 * Unlike std::thread which will start a new thread as soon as it's constructed
 * packaged_task doesn't start until the operator() is called. Therefore, we
 * can manage packaged tasks with a container, while the tasks are yet to run.
*/

int main()
{
    std::packaged_task<int(int, int)> ptask(
        [](int a, int b)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return a+b;
        }
    );
    auto fut = ptask.get_future();

    std::thread t{std::move(ptask), 6, 7};
    std::cout << "Waiting for result...\n";

    // the main thread will be blocked on fut.get()
    std::cout << "6 + 7 = " << fut.get() << std::endl;
    t.join();

    return 0;
}
