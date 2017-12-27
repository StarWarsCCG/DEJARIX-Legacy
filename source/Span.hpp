#ifndef SPAN_H
#define SPAN_H

template<typename T> struct Span
{
    T* data;
    int count;

    T* begin() const { return data; }
    T* end() const { return data + count; }
};

#endif
