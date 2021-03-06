#ifndef PIPES_RING_PIPE_H
#define PIPES_RING_PIPE_H

#include <memory>
#include <mutex>

#include "read_pipe.h"
#include "write_pipe.h"

template<typename T, typename semaphore>
class ring_pipe : public read_pipe<T>, public write_pipe<T>
{
private:
    std::unique_ptr<T[]> data;
    // Invariant: valid data is between iBeg, ... cap - 1, 0, 1, ... iEnd - 1
    // iBeg and iEnd are always valid indices and roll around when updated
    // No bounds checking will happen on this one because the semaphores take care of it
    int iBeg, iEnd;
    // Necessary for empty() => iEnd == iBeg can mean both empty and full
    // Otherwise we'd have to sample the items semaphore and keep a separate flag 
    // to keep track if the writer has closed the pipe
    int size;
    const int capacity;
    std::mutex mQ;
    semaphore items, spaces;
    bool closedByWriter = false;

    bool empty() const
    {
        return size == 0;
    }

    T front() const
    {
        return data.get()[iBeg];
    }

    void pop_front()
    {
        iBeg++;
        if (iBeg >= capacity) iBeg = 0;
        size--;
    }

    void push_back(const T & val)
    {
        data.get()[iEnd] = val;
        iEnd++;
        if (iEnd >= capacity) iEnd = 0;
        size++;
    }

public:
    ring_pipe(const int cap) : capacity(cap), iBeg(0), iEnd(0), size(0), items(0), spaces(cap)
    {
        data = std::unique_ptr<T[]>(new T[cap]);
    }

    bool read(T & outVal)
    {
        bool retVal = true;

        items.wait();
        mQ.lock();

        if (empty() && closedByWriter)
        {
            retVal = false;
            items.signal(); // Everyone else will also get it
        } 
        else
        {
            outVal = front();
            pop_front();
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
        else push_back(val);

        mQ.unlock();
        if (retVal) items.signal();
        return retVal;
    }

    void closeWrite()
    {
        closedByWriter = true;
        items.signal();
    }

    void closeRead()
    {
        // TODO
    }
};

#endif