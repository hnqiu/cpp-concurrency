#include <atomic>
#include <mutex>
#include <thread>

class some_type {
    // ...
public:
    void do_it() { /*...*/ }
};

std::atomic<some_type*> ptr{nullptr};            // Variable to be lazily initialized
std::mutex process_mutex;

void process() {
    if (!ptr) {                     // First check of ptr
        std::lock_guard<std::mutex> lk(process_mutex);

        if (!ptr)                  // Second check of ptr
            ptr = new some_type;   // Initialize ptr
    }
    some_type* p = ptr;
    p->do_it();
}

int main()
{
    std::thread t1{process};
    std::thread t2{process};

    t1.join();
    t2.join();
    return 0;
}
