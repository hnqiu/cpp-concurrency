#include <iostream>
#include <thread>

class ThreadGuard
{
public:
    explicit ThreadGuard(std::thread&& t) : _t(std::move(t)) { }
    ~ThreadGuard()
    {
        if (_t.joinable())
        {
            _t.join();
        }
    }

    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
private:
    std::thread _t;
};

int main()
{
    auto foo = [](){std::cout << "foo\n";};
    try
    {
        std::thread t{foo};
        ThreadGuard g{std::move(t)};
        throw std::exception();
    }
    catch(const std::exception& e)
    {
        std::cerr << "exception occurred: " << e.what() << '\n';
    }
    return 0;
}
