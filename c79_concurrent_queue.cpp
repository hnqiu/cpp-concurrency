#include <chrono>
#include <condition_variable>
#include <iostream>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>


/*!
 * \brief An concurrent queue implementation
 */

namespace concurrency
{
class ConcurrentQueueException : public std::runtime_error
{
public:
    ConcurrentQueueException() : std::runtime_error("Empty queue") { }
    ConcurrentQueueException(const char* s) : std::runtime_error(s) { }
};


template<typename T>
class ConcurrentQueue
{
public:
    ConcurrentQueue() = default;
    ~ConcurrentQueue() = default;

    void push(T value)
    {
        std::unique_lock<std::mutex> lock{_m};
        _data.push(value);
        lock.unlock();
        _cond.notify_one();
    }

    void pop(T& value)
    {
        std::unique_lock<std::mutex> lock{_m};
        // if read before write, an exception will be thrown,
        // if (_data.empty())
        // {
        //     throw ConcurrentQueueException();
        // }
        // or better wait until the queue has elements again
        _cond.wait(lock, [this](){ return !_data.empty(); });
        value = _data.front();
        _data.pop();
    }
private:
    std::queue<T> _data;
    std::mutex _m;
    std::condition_variable _cond;
};
}


concurrency::ConcurrentQueue<std::string> queue;

void reader()
{
    std::cout << "Reading queue..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::string data;
    try
    {
        queue.pop(data);
        std::cout << "Read data: '" << data << "'\n";
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }
}

void writer()
{
    std::cout << "Writing queue..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    queue.push("some data");
}

int main()
{
    auto w = std::async(std::launch::async, writer);
    auto r = std::async(std::launch::async, reader);

    w.wait();
    r.wait();

    return 0;
}
