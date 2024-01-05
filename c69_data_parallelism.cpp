#include <algorithm>
#include <future>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>


static std::mt19937 mt;
std::uniform_real_distribution<double> dist(0.0, 100.0);

double accum(double* begin, double* end)
{
    return std::accumulate(begin, end, 0.0);
}

// use std::async to perform parallel computing
double add_async(std::vector<double>& vec)
{
    auto vec0 = &vec[0];
    auto size = vec.size();

    auto fut1 = std::async(std::launch::async, accum, vec0, vec0 + size/4);
    auto fut2 = std::async(std::launch::async, accum, vec0 + size/4, vec0 + 2*size/4);
    auto fut3 = std::async(std::launch::async, accum, vec0 + 2*size/4, vec0 + 3*size/4);
    auto fut4 = std::async(std::launch::async, accum, vec0 + 3*size/4, vec0 + size);

    return fut1.get() + fut2.get() + fut3.get() + fut4.get();
}

// use std::packaged_task to perform parallel computing
double add_packaged_task(std::vector<double>& vec)
{
    using task_type = double(double*, double*);
    std::packaged_task<task_type> p1{accum};
    std::packaged_task<task_type> p2{accum};
    std::packaged_task<task_type> p3{accum};
    std::packaged_task<task_type> p4{accum};

    auto fut1 = p1.get_future();
    auto fut2 = p2.get_future();
    auto fut3 = p3.get_future();
    auto fut4 = p4.get_future();

    auto vec0 = &vec[0];
    auto size = vec.size();

    std::vector<std::thread> threads;
    threads.push_back(std::thread{std::move(p1), vec0, vec0 + size/4});
    threads.push_back(std::thread{std::move(p2), vec0 + size/4, vec0 + 2*size/4});
    threads.push_back(std::thread{std::move(p3), vec0 + 2*size/4, vec0 + 3*size/4});
    threads.push_back(std::thread{std::move(p4), vec0 + 3*size/4, vec0 + size});

    for (auto& t : threads)
    {
        t.join();
    }

    return fut1.get() + fut2.get() + fut3.get() + fut4.get();
}

int main()
{
    std::vector<double> vec(16);
    std::iota(vec.begin(), vec.end(), 1.0);

    std::vector<double> vrand(10'000);
    std::generate(vrand.begin(), vrand.end(), [&vrand](){ return dist(mt); });

    std::cout << "Parallel computing using std::async\n";
    std::cout << "Sum of first 16 int is " << add_async(vec) << std::endl;
    std::cout << "Sum of 10'000 random is " << add_async(vrand) << std::endl;

    std::cout << "\nParallel computing using std::packaged_task\n";
    std::cout << "Sum of first 16 int is " << add_packaged_task(vec) << std::endl;
    std::cout << "Sum of 10'000 random is " << add_packaged_task(vrand) << std::endl;

    return 0;
}
