#ifndef PIPES_SEMAPHORE_20_H
#define PIPES_SEMAPHORE_20_H

#include <semaphore>

class semaphore_cpp20
{
private:
    std::counting_semaphore<1024> sem;

public:
    semaphore_cpp20();
    semaphore_cpp20(int value);
    void wait();
    void signal();
    int querry();
};

#endif