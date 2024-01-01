#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>


/*!
 * \brief condition variable with predicate
 * \note A predicate usually uses a shared boolean. It can deal with
 * - lost wakeup, and
 * - spurious wakeup
 */

std::string sdata{"Empty"};
std::mutex mut;
std::condition_variable cond;
bool condition{false};

void reader()
{
    std::cout << "Reader thread has started..." << std::endl;
    std::unique_lock<std::mutex> lock{mut};

    std::cout << "Reader waiting for data" << std::endl;
    cond.wait(lock, [](){ return condition; });
    // this is equivalent to
    // while (!condition)
    // {
    //     cond.wait(lock);
    // }
    // 'condition' is set by the writer when it completes.
    // The reader will call the predicate fist.
    // It will call wait() if it returns false.
    // If 'condition' is true, wait() won't be called because there's already a
    // notification.

    std::cout << "Reader reading data: " << sdata << std::endl;
}

void writer()
{
    std::cout << "Writer thread has started..." << std::endl;
    {
        std::lock_guard<std::mutex> lock{mut};
        sdata = "some data";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // set condition to true while mutex is still locked
        condition = true;
    }
    std::cout << "Writer thread sending notification..." << std::endl;
    cond.notify_one();
}

int main()
{
    std::cout << "Initial data is '" << sdata << "'" << std::endl;

    // start writer before reader. notification is still not lost.
    std::thread t2{writer};
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::thread t1{reader};

    t1.join();
    t2.join();
    return 0;
}
