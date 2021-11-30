#ifndef PIPES_RING2_PIPE_H
#define PIPES_RING2_PIPE_H

#include <memory>
#include <mutex>
#include <thread>

#include "ring2.h"

#include "read_pipe.h"
#include "write_pipe.h"

template<typename T>
class ring2_pipe : public read_pipe<T>, public write_pipe<T>
{
private:
    ring2<T> ring;
    bool closedByWriter = false;

public:
    ring2_pipe(const int cap) : ring(cap)
    {
    }

    bool read(T & outVal)
    {
        if (!ring.empty())
        {
            outVal = ring.front();
            ring.pop_front();
            return true;
        }

        while(ring.empty() && !closedByWriter) std::this_thread::yield();
        if (closedByWriter) return false;

        outVal = ring.front();
        ring.pop_front();
        return true;
    }

    bool write(const T & val)
    {
        if (closedByWriter) return false;

        while (ring.full()) std::this_thread::yield();
        ring.push_back(val);       
        return true;
    }

    void closeWrite()
    {
        closedByWriter = true;
    }

    void closeRead()
    {
        // TODO
    }
};

#endif