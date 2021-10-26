#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <cmath>

#include "cpp_semaphore.h"
#include "pipe.h"

void source(write_pipe<int> & out)
{
    for (int i = 0; i <= 100; i++)
    {
        out.write(i);
        // std::cout << "push " << i << "\n";
        // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    out.closeWrite();
}

void stage1(read_pipe<int> & in, write_pipe<double> & out)
{
    static int cnt = 0;
    static std::mutex m;

    m.lock();
    cnt++;
    m.unlock();

    int inVal;
    while (in.read(inVal))
    {
        const double val = inVal;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
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
    double sum = 0;
    double inVal;
    while (in.read(inVal))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << inVal << "\n";
        sum += inVal;
    }

    std::cout << "Sum = " << sum << "\n";
}

int main(int, char**) 
{
    pipe<int> pipe1(10);
    pipe<double> pipe2(10);

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
