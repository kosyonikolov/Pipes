#ifndef PIPES_READ_PIPE_H
#define PIPES_READ_PIPE_H

template<typename T>
class read_pipe
{
public:
    virtual bool read(T & outVal) = 0;
    virtual void closeRead() = 0;
    virtual ~read_pipe() {};
};

#endif