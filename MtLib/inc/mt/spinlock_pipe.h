#ifndef MTL_SPINLOCK_PIPE_H
#define MTL_SPINLOCK_PIPE_H

#include <concepts>
#include <thread>

namespace mt 
{
    template<class T, class Ring>
    requires requires (Ring c, const T inVal, int cap)
    {
        {Ring(cap)};
        {c.empty()} -> std::same_as<bool>;
        {c.full()} -> std::same_as<bool>;
        {c.front()} -> std::convertible_to<T>;
        {c.pop_front()};
        {c.push_back(inVal)};
    }
    class spinlock_pipe
    {
    private:
        Ring ring;
        bool closedByWriter = false;

    public:
        spinlock_pipe(const int cap) : ring(cap)
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

        void close_write()
        {
            closedByWriter = true;
        }

        void close_read()
        {
            // TODO
        }
    };
}

#endif