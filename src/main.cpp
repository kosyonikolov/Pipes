#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <cmath>

#include "cpp_semaphore.h"
#include "pipe.h"

void source(Pipe<int> & writeHead)
{
    for (int i = 0; i <= 100; i++)
    {
        writeHead.write(i);
        // std::cout << "push " << i << "\n";
        // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    writeHead.closeWrite();
}

void stage1(Pipe<int> & readHead, Pipe<double> & writeHead)
{
    int inVal;
    while (readHead.read(inVal))
    {
        const double val = inVal;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // std::cout << "pass " << val << "\n";
        writeHead.write(std::sqrt(val));
    }
    writeHead.closeWrite();
}

void sink(Pipe<double> & readHead)
{
    double sum = 0;
    double inVal;
    while (readHead.read(inVal))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << inVal << "\n";
        sum += inVal;
    }

    std::cout << "Sum = " << sum << "\n";
}

int main(int, char**) 
{
    Pipe<int> pipe1(10);
    Pipe<double> pipe2(10);

    std::thread tSrc([&]() { source(pipe1); });
    std::thread tStage1([&]() { stage1(pipe1, pipe2);});
    std::thread tSink([&]() { sink(pipe2); });

    tSrc.join();
    tStage1.join();
    tSink.join();

    return 0;
}
