#include <MtLib/posix_semaphore.h>

namespace mt 
{
    posix_semaphore::posix_semaphore()
    {
        sem_init(&sem, 0, 0);
    }

    posix_semaphore::posix_semaphore(int value)
    {
        sem_init(&sem, 0, value);
    }

    void posix_semaphore::wait()
    {
        sem_wait(&sem);
    }

    void posix_semaphore::signal()
    {
        sem_post(&sem);
    }

    int posix_semaphore::querry()
    {
        int val;
        sem_getvalue(&sem, &val);
        return val;
    }
}