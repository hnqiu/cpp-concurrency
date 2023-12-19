#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

std::mutex m;

void foo(std::string str)
{
    try
    {
        std::lock_guard<std::mutex> l(m);
        std::cout << str[0] << " " << str[1] << " " << str[2] << std::endl;
        throw std::exception();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception occurred: " << e.what() << '\n';
    }
}

int main()
{
    std::thread t1{foo, "noweg"};
    std::thread t2{foo, "dgoyw"};

    t1.join();
    t2.join();
    return 0;
}
