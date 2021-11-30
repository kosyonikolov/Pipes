#include "cpp20_semaphore.h"

#include <semaphore>

semaphore_cpp20::semaphore_cpp20() : sem(0)
{
}

semaphore_cpp20::semaphore_cpp20(int value) : sem(value)
{
}

void semaphore_cpp20::wait()
{
    sem.acquire();
}

void semaphore_cpp20::signal()
{
    sem.release();
}

int semaphore_cpp20::querry()
{
    return -1;
}