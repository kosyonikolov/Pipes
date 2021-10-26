#ifndef PIPES_SEMAPHORE_H
#define PIPES_SEMAPHORE_H

#include <semaphore.h>

class semaphore
{
private:
    sem_t sem;

public:
    semaphore();
    semaphore(int value);
    void wait();
    void signal();
};

#endif