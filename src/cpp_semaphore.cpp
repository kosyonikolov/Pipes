#include "cpp_semaphore.h"
#include <semaphore.h>

semaphore::semaphore(int value)
{
    sem_init(&sem, 0, value);
}

void semaphore::wait()
{
    sem_wait(&sem);
}

void semaphore::signal()
{
    sem_post(&sem);
}