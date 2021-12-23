#ifndef MTL_CSEMAPHORE_H
#define MTL_CSEMAPHORE_H

namespace mt
{
    template<typename T>
    concept semaphore = requires(T v, int k)
    {
        {T()};
        {T(k)};
        {v.signal()};
        {v.wait()};
    };
}

#endif