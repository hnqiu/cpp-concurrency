#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>


void produce(std::promise<int>& promise)
{
    int x{42};
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "Promise sets shared state to " << x << std::endl;
    promise.set_value(x);
}

void consume(std::shared_future<int>& future)
{
    std::cout << "Consumer getting future value...\n";
    int x = future.get();
    std::cout << "Consumer " << std::this_thread::get_id()
              << " has got value " << x << std::endl;
}

int main()
{
    std::promise<int> prom;
    std::shared_future<int> fut1 = prom.get_future();
    auto fut2 = fut1;
    std::thread t1{produce, std::ref(prom)};
    std::thread t2{consume, std::ref(fut1)};
    std::thread t3{consume, std::ref(fut2)};

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
