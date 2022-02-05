#ifndef MTL_LBS_PIPE_H
#define MTL_LBS_PIPE_H

#include <concepts>
#include <cstddef>
#include <mutex>
#include <deque>
#include <cassert>

#include <mt/csemaphore.h>

namespace mt
{
    template<class T, class SequentialContainer, mt::semaphore Sem>
    requires requires (SequentialContainer c, const T value, size_t k)
    {
        {c.data()} -> std::convertible_to<T>;
        {c.size()} -> std::convertible_to<size_t>;
    }
    class lbs_pipe
    {
    private:
        using guard = std::lock_guard<std::mutex>;

        SequentialContainer & data;
                
        std::deque<size_t> free, ready;
        Sem semFree, semReady;
        std::mutex mFree, mReady;

        bool closedByWriter = false;
        bool closedByReader = false;

    public:
        lbs_pipe(SequentialContainer & data) : data(data), semFree(data.size()) 
        {
            for (size_t i = 0; i < data.size(); i++) free.push_back(i);
        }

        const T * begin_read()
        {
            semReady.wait();
            guard readyGuard(mReady);

            if (ready.empty())
            {
                assert(closedByWriter);

                // Signal but no data -> closed by writer
                // Repeat the signal so that every user of the pipe gets it
                semReady.signal();
                return nullptr;
            }

            const size_t id = ready.front();
            ready.pop_front();

            return data.data() + id;
        }

        void finish_read(const T * value)
        {
            const ptrdiff_t id = value - data.size();
            assert(id >= 0 && id < data.size());

            mFree.lock();
            free.push_back(id);
            mFree.unlock();
            semFree.signal();
        }

        T * begin_write()
        {
            if (closedByReader) return nullptr;

            semFree.wait();
            guard freeGuard(mFree);

            if (free.empty())
            {
                semFree.signal();
                return nullptr;
            }

            const size_t id = free.front();
            free.pop_front();

            return data.data() + id;
        }

        void finish_write(const T * value)
        {
            const ptrdiff_t id = value - data.size();
            assert(id >= 0 && id < data.size());

            mReady.lock();
            ready.push_back(id);
            mReady.unlock();
            semReady.signal();
        }

        void close_write()
        {
            closedByWriter = true;
            semReady.signal();
        }

        void close_read()
        {
            closedByReader = true;
            semFree.signal();
        }
    };
}

#endif