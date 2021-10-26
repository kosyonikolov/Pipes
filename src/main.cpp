#include <iostream>
#include <thread>
#include <chrono>
#include <queue>

#include "cpp_semaphore.h"

int main(int, char**) 
{
    semaphore s(0);
    std::queue<int> q;

    auto producer = [&]()
    {
        for (int i = 0; i < 10; i++)
        {
            q.push(i);
            s.signal();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    };

    auto consumer = [&]()
    {
        int cntRecv = 0;
        while (cntRecv < 10)
        {
            s.wait();
            int k = q.front();
            q.pop();
            std::cout << k << "\n";
            cntRecv++;
        }
    };

    std::thread tc(consumer);
    std::thread tp(producer);

    tc.join();
    tp.join();

    return 0;
}
