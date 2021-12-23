#ifndef MTL_CPIPE_H
#define MTL_CPIPE_H

#include <concepts>

namespace mt 
{
    template<typename P, typename T>
    concept read_pipe = requires(P pipe, T & val)
    {
        {pipe.read(val)} -> std::same_as<bool>;
        {pipe.close_read()};
    };

    template<typename P, typename T>
    concept write_pipe = requires(P pipe, const T val)
    {
        {pipe.write(val)} -> std::same_as<bool>;
        {pipe.close_write()};
    };

    template<typename P, typename T>
    concept pipe = read_pipe<P, T> && write_pipe<P, T>;
}

#endif