#include <iostream>
#include <string>
#include <thread>


int main()
{
    auto fizzbuzz = [](size_t epoch) -> void {
        const size_t num{4};
        const std::string names[num] = {"Abdul", "Bart", "Claudia", "Divya"};

        for (size_t i = 1; i <= epoch; ++i)
        {
            std::cout << names[(i-1) % num] << " says ";
            if (i % 3 == 0 && i % 5 == 0)
            {
                std::cout << "fizzbuzz!";
            }
            else if (i % 3 == 0)
            {
                std::cout << "fizz!";
            }
            else if (i % 5 == 0)
            {
                std::cout << "buzz!";
            }
            else
            {
                std::cout << i;
            }
            std::cout << std::endl;
        }
    };

    std::thread t{fizzbuzz, 100};
    t.join();

    return 0;
}

