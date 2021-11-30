#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <cmath>
#include <mutex>
#include <random>

#include "cpp_semaphore.h"
#include "cpp20_semaphore.h"
#include "read_pipe.h"
#include "write_pipe.h"
#include "ring_pipe.h"

void source(write_pipe<int> & out)
{
    for (int i = 0; i < 1 << 22; i++)
    {
        out.write(i);
    }
    out.closeWrite();
}

void sink(read_pipe<int> & in)
{
    int64_t sum = 0;
    int inVal;
    while (in.read(inVal))
    {
        sum += inVal;
    }

    std::cout << "Sum = " << sum << "\n";
}

template<typename Pipe>
double timePipe(Pipe & p)
{
    auto start = std::chrono::steady_clock::now();

    std::thread consumer([&]() { sink(p); });
    source(p);
    consumer.join();

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;
}

int main(int, char**) 
{
    ring_pipe<int, semaphore> p1(256);
    ring_pipe<int, semaphore_cpp20> p2(256);

    const auto t1 = timePipe(p1);
    const auto t2 = timePipe(p2);

    std::cout << "p1 = " << t1 << " ms\n";
    std::cout << "p2 = " << t2 << " ms\n";
    // std::cout << "p3 = " << t3 << " ms\n";
    // std::cout << "p4 = " << t4 << " ms\n";
    // std::cout << "p5 = " << t5 << " ms\n";
    // std::cout << "p6 = " << t6 << " ms\n";

    return 0;
}
