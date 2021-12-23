#ifndef MTL_POSIX_SEMAPHORE_H
#define MTL_POSIX_SEMAPHORE_H

#include <semaphore.h>

namespace mt
{
    class posix_semaphore
    {
    private:
        sem_t sem;

    public:
        posix_semaphore();
        posix_semaphore(int value);
        void wait();
        void signal();
        int querry();
    };
}

#endif