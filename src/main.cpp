#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <cmath>
#include <mutex>
#include <random>

#include "cpp_semaphore.h"
#include "read_pipe.h"
#include "write_pipe.h"
#include "ring_pipe.h"

void source(write_pipe<int> & out)
{
    std::uniform_int_distribution<int> dist(0, 10);
    std::default_random_engine rng(std::random_device{}());

    for (int i = 0; i <= 100; i++)
    {
        out.write(i);
        // std::cout << "push " << i << "\n";
        // std::this_thread::sleep_for(std::chrono::milliseconds(5 + dist(rng)));
    }
    out.closeWrite();
}

void dumbSink(read_pipe<int> & in)
{
    int cnt = 0;
    int val;
    while (in.read(val))
    {
        std::cout << val << "\n";
        cnt++;
    }

    std::cout << "Count = " << cnt << "\n";
}

void stage1(read_pipe<int> & in, write_pipe<double> & out)
{
    static int cnt = 0;
    static std::mutex m;

    std::uniform_int_distribution<int> dist(0, 50);
    std::default_random_engine rng(std::random_device{}());

    m.lock();
    cnt++;
    m.unlock();

    int inVal;
    while (in.read(inVal))
    {
        const double val = inVal;
        std::this_thread::sleep_for(std::chrono::milliseconds(100 + dist(rng)));
        // std::cout << "pass " << val << "\n";
        out.write(std::sqrt(val));
    }

    m.lock();
    cnt--;
    if (cnt <= 0) out.closeWrite();
    m.unlock();
}

void sink(read_pipe<double> & in)
{
    std::uniform_int_distribution<int> dist(0, 30);
    std::default_random_engine rng(std::random_device{}());

    double sum = 0;
    double inVal;
    while (in.read(inVal))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(30 + dist(rng)));
        std::cout << inVal << "\n";
        sum += inVal;
    }

    std::cout << "Sum = " << sum << "\n";
}

int main(int, char**) 
{
    ring_pipe<int> pipe1(10);
    ring_pipe<double> pipe2(10);

    auto tStart = std::chrono::steady_clock::now();

    std::thread tSrc([&]() { source(pipe1); });
    std::thread tStage1_1([&]() { stage1(pipe1, pipe2);});
    std::thread tStage1_2([&]() { stage1(pipe1, pipe2);});
    std::thread tStage1_3([&]() { stage1(pipe1, pipe2);});
    std::thread tSink([&]() { sink(pipe2); });

    tSrc.join();
    tStage1_1.join();
    tStage1_2.join();
    tStage1_3.join();
    tSink.join();

    auto tEnd = std::chrono::steady_clock::now();
    const auto tMs = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count();

    std::cout << "Elapsed time: " << tMs << " ms\n";

    return 0;
}
