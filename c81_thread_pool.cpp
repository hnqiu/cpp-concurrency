#include <chrono>
#include <condition_variable>
#include <iostream>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>


/*!
 * \brief A thread pool implementation using the concurrent queue from c79.cpp
 */


std::mutex cout_mutex; // mutex for cout

namespace concurrency
{
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
        _cond.wait(lock, [this]() {
            std::lock_guard<std::mutex> l{cout_mutex};
            std::cout << "waiting\n";
            return !_data.empty();
        });
        value = _data.front();
        _data.pop();
    }

private:
    std::queue<T> _data;
    std::mutex _m;
    std::condition_variable _cond;
};

class ThreadPool
{
public:
    ThreadPool()
    {
        for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
        // for (size_t i = 0; i < 2; ++i)
        {
            std::unique_lock<std::mutex> l{cout_mutex};
            std::cout << "Creating thread\n";
            l.unlock();
            _threads.push_back(std::thread{[this]() {
                while (true)
                {
                    std::function<void()> task;
                    _tasks.pop(task);
                    task();
                }
            }});
        }
    }
    ~ThreadPool()
    {
        for (auto& t : _threads)
        {
            t.join();
        }
    }

    void submit(std::function<void()> task)
    {
        _tasks.push(task);
    }

private:
    std::vector<std::thread> _threads;
    ConcurrentQueue<std::function<void()>> _tasks;
};

}


int main()
{
    std::cout << "Num of hardware threads: " 
              << std::thread::hardware_concurrency << std::endl;

    concurrency::ThreadPool pool;
    for (size_t i = 0; i < 12; ++i)
    {
        pool.submit([]() {
            std::unique_lock<std::mutex> l{cout_mutex};
            std::cout << "thread #" << std::this_thread::get_id()
                      << " executing" << std::endl;
            l.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            return;
        });
    }

    return 0;
}
