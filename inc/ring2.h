#ifndef PIPES_ring2_H
#define PIPES_ring2_H

#include <memory>
#include <thread>
#include <atomic>

template<typename T>
class ring2
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
    std::atomic<int> count;
    const int capacity;

public:
    ring2(const int cap) : capacity(cap), iBeg(0), iEnd(0), count(0)
    {
        data = std::unique_ptr<T[]>(new T[cap]);
    }

    ring2(ring2 & other) = delete;

    bool empty() const
    {
        return count == 0;
    }

    bool full() const
    {
        return count == capacity;
    }

    int size() const
    {
        return count;
    }

    T front() const
    {
        return data.get()[iBeg];
    }

    T & front()
    {
        return data.get()[iBeg];
    }

    T back() const
    {
        int iLast = iEnd - 1;
        if (iLast < 0) iLast = capacity - 1;
        return data.get()[iLast];
    }

    T & back()
    {
        int iLast = iEnd - 1;
        if (iLast < 0) iLast = capacity - 1;
        return data.get()[iLast];
    }

    void pop_front()
    {
        iBeg++;
        if (iBeg >= capacity) iBeg = 0;
        count--;
    }

    void pop_back()
    {
        iEnd--;
        if (iEnd < 0) iEnd = capacity - 1;
        count--;
    }

    void push_front(const T & val)
    {
        int iBegNew = iBeg - 1;
        if (iBegNew < 0) iBegNew = capacity - 1;
        data.get()[iBegNew] = val;
        iBeg = iBegNew;
        count++;
    }

    void push_back(const T & val)
    {
        data.get()[iEnd] = val;
        iEnd++;
        if (iEnd >= capacity) iEnd = 0;
        count++;
    }
};

#endif