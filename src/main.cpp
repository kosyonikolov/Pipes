#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <cmath>
#include <mutex>
#include <random>
#include <array>

#include <MtLib/csemaphore.h>
#include <MtLib/cpipe.h>

#include <MtLib/posix_semaphore.h>
#include <MtLib/lbs_pipe.h>
#include <NsLib/ring.h>

template<typename P>
requires mt::write_pipe<P, int>
void source(P & out)
{
    for (int i = 1; i <= 1 << 25; i++)
    {
        out.write(i);
    }
    out.close_write();
}

template<typename P>
requires mt::read_pipe<P, int>
void sink(P & in)
{
    int64_t sum = 0;
    int inVal;
    while (in.read(inVal))
    {
        // std::cout << inVal << "\n";
        sum += inVal;
    }

    std::cout << "Sum = " << sum << "\n";
}

template<typename Pipe>
requires mt::read_pipe<Pipe, int> && mt::write_pipe<Pipe, int>
double timePipe(Pipe & p)
{
    auto start = std::chrono::steady_clock::now();

    std::thread consumer([&]() { sink(p); });
    source(p);
    consumer.join();

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;
}

template<mt::semaphore T>
void testSem()
{
    T sem(3);
    for (int i = 0; i < 3; i++) sem.wait();
    std::cout << "OK\n";
}


int main(int, char**) 
{
    testSem<mt::posix_semaphore>();

    mt::lbs_pipe<int, ns::ring<int>, mt::posix_semaphore> p1(1024);

    const auto t1 = timePipe(p1);

    // const auto t3 = timePipe(p3);
    // const auto t2 = timePipe(p2);
    
    std::cout << "p1 = " << t1 << " ms\n";
    // std::cout << "p2 = " << t2 << " ms\n";
    // std::cout << "p3 = " << t3 << " ms\n";
    // std::cout << "p4 = " << t4 << " ms\n";
    // std::cout << "p5 = " << t5 << " ms\n";
    // std::cout << "p6 = " << t6 << " ms\n";

    return 0;
}
