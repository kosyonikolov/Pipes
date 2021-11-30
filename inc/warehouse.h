#ifndef PIPES_WAREHOUSE_H
#define PIPES_WAREHOUSE_H

#include <memory>

#include "ring.h"

template<typename T>
class warehouse
{
public:
    struct slot
    {
        int id;
        T * const obj;
    };

    slot acquire()
    {

    }

    void release(const slot & s)
    {

    }
};

#endif