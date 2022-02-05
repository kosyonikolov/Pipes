#ifndef MTL_LBS_PIPE_H
#define MTL_LBS_PIPE_H

#include <concepts>
#include <mutex>

#include <mt/csemaphore.h>

namespace mt
{
    template<class T, class Container, mt::semaphore Sem>
    requires requires (Container c, const T inVal, int cap)
    {
        {Container(cap)};
        {c.empty()} -> std::same_as<bool>;
        {c.front()} -> std::convertible_to<T>;
        {c.pop_front()};
        {c.push_back(inVal)};
    }
    class lbs_pipe
    {
    private:
        Container q;
        std::mutex mQ;
        Sem items, spaces;
        bool closedByWriter = false;

    public:
        lbs_pipe(const int cap) : items(0), spaces(cap), q(cap) {}

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

        void close_write()
        {
            closedByWriter = true;
            items.signal();
        }

        void close_read()
        {
            // TODO
        }
    };
}

#endif