#ifndef DEFERREDARRAY_HPP
#define DEFERREDARRAY_HPP

#include <cstddef>

template<class T, int N> class DeferredArray
{
    char _data[N * sizeof(T)];
    int _n = 0;

public:
    DeferredArray() = default;
    DeferredArray(const DeferredArray&) = delete;
    DeferredArray(DeferredArray&&) = delete;

    inline ~DeferredArray()
    {
        auto t = (T*)_data;
        auto n = _n;
        while (n > 0) t[--n].~T();
    }

    DeferredArray& operator=(const DeferredArray&) = delete;
    DeferredArray& operator=(DeferredArray&&) = delete;

    inline T* begin() { return (T*)_data; }
    inline T* end() { return begin() + _n; }

    constexpr int capacity() const { return N; }
    inline int size() const { return _n; }
    inline bool hasRoom() const { return _n < N; }
    inline bool isFull() const { return _n >= N; }

    inline void* allocate() { return begin() + _n++; }
    inline T& operator[](ptrdiff_t n) { return begin()[n]; }
    inline const T& operator[](ptrdiff_t n) const { return begin()[n]; }

    template<typename... A> inline T& push(A&&... args)
    {
        auto block = allocate();
        return *(new (block) T(args...));
    }
};

#endif

