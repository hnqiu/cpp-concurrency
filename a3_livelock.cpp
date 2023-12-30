#include <mutex>
#include <thread>

int x{0};
std::mutex m;

void func() {
    std::lock_guard<std::mutex> l{m};
    while (x == 0) {
        x = 1 - x;
    }
}

int main()
{
    std::thread thr1{ func };
    std::thread thr2{ func };

    thr1.join();
    thr2.join();
}
