#include <chrono>
#include <iostream>
#include <shared_mutex>
#include <thread>
#include <vector>


using Mutex = std::shared_timed_mutex;
using ReadLock = std::shared_lock<Mutex>;
using WriteLock = std::unique_lock<Mutex>;

Mutex m;
int x{0};

void read(size_t i)
{
    // a shared lock can acquire a mutex when there're no exclusive locks
    ReadLock l{m};
    // do something
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "read thead #" << i << ": x = " << x << std::endl;
}

void write()
{
    // an exclusive lock can only acquire a mutex when the mutex is unlocked.
    // this thread will wait until all other locks (including shared locks) are
    // released.
    WriteLock l{m};
    ++x;
    std::cout << "write thead: x = " << x << std::endl;
}

int main()
{
    std::vector<std::thread> threads;
    size_t i{0};
    for (; i < 10; ++i)
    {
        threads.push_back(std::thread{read, i});
    }
    threads.push_back(std::thread{write});
    threads.push_back(std::thread{write});
    for (; i < 20; ++i)
    {
        threads.push_back(std::thread{read, i});
    }

    for (auto& t : threads)
    {
        t.join();
    }

    return 0;
}
