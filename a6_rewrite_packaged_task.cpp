#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>


// this is to rewrite c63.cpp using std::promise

void add(std::promise<int>& result, int a, int b)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    result.set_value(a + b);
}

void print(std::future<int>& fut)
{
    std::cout << "Waiting for result...\n";
    std::cout << "6 + 7 = " << fut.get() << std::endl;
}

int main()
{
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();
    std::thread t1{add, std::ref(prom), 6, 7};
    std::thread t2{print, std::ref(fut)};

    t1.join();
    t2.join();

    return 0;
}
