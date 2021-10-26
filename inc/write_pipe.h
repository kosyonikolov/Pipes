#ifndef PIPES_WRITE_PIPE_H
#define PIPES_WRITE_PIPE_H

template<typename T>
class write_pipe
{
public:
    virtual bool write(const T & val) = 0;
    virtual void closeWrite() = 0;
    virtual ~write_pipe() {};
};

#endif