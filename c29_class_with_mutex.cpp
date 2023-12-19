#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Vector
{
public:
    Vector() = default;
    ~Vector() = default;

    void push_back(int d)
    {
        _m.lock();
        _data.push_back(d);
        _m.unlock();
    }
    void print()
    {
        // comment out the mutex lock to see what happens
        _m.lock();
        for (const auto d : _data)
        {
            std::cout << d << " ";
        }
        std::cout << std::endl;
        _m.unlock();
    }
private:
    std::vector<int> _data;
    std::mutex _m;
};


void pushToVector(Vector& v)
{
    for (int i = 0; i < 10; ++i)
    {
        v.push_back(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        v.print();
    }
}

int main()
{
    Vector v;
    std::thread t1{pushToVector, std::ref(v)};
    std::thread t2{pushToVector, std::ref(v)};

    t1.join();
    t2.join();
    return 0;
}
