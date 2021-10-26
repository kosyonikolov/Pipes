#ifndef PIPES_PIPE_H
#define PIPES_PIPE_H

#include <deque>
#include <mutex>

#include "cpp_semaphore.h"

template<typename T>
class Pipe
{
private:
    std::deque<T> q;
    std::mutex mQ;
    semaphore items, spaces;
    int capacity;
    bool closedByWriter = false;

public:
    Pipe(const int cap) : capacity(cap), items(0), spaces(cap) {}

    bool read(T & outVal)
    {
        bool retVal = true;

        items.wait();
        mQ.lock();

        if (q.empty() && closedByWriter)
        {
            retVal = false;
            items.signal(); // Everyone else will also get it
        } 
        else
        {
            outVal = q.front();
            q.pop_front();
        }

        mQ.unlock();
        if (retVal) spaces.signal();
        return retVal;
    }

    bool write(const T & val)
    {
        if (closedByWriter) return false;
        bool retVal = true;

        spaces.wait();
        mQ.lock();

        if (closedByWriter) retVal = false;
        else q.push_back(val);

        mQ.unlock();
        if (retVal) items.signal();
        return retVal;
    }

    void closeWrite()
    {
        closedByWriter = true;
        items.signal();
    }
};

#endif