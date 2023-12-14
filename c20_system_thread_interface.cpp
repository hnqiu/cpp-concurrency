#include <chrono>
#include <iostream>
#include <thread>


void foo()
{
    using namespace std::literals;
    std::this_thread::sleep_for(0.1s);
    std::cout << "ID of thread foo: " << std::this_thread::get_id << std::endl;
}

int main()
{
    std::cout << "ID of main thread: " << std::this_thread::get_id << std::endl;

    std::thread t{foo};
    std::cout << "ID of thread t: " << t.get_id() << std::endl;
    std::cout << "handle of thread t: " << t.native_handle() << std::endl;
    t.join();

    std::cout << "\nAfter thread t has joined:\n";
    std::cout << "ID of thread t: " << t.get_id() << std::endl;
    std::cout << "handle of thread t: " << t.native_handle() << std::endl;

    return 0;
}
