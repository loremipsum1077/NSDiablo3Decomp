// Original file: BlizzardCore/Include/Blizzard/Util.h
// Buffer and BufferData are templates, so the DWARF has only their instantiated layouts. The
// bodies are not in the DWARF; they are written from the Buffer<0, char> copy inlined into
// System_File::PreallocateSlow.

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Blizzard {
namespace Util {
namespace BufferPrivate {
// Util.h:54
template <typename T, size_t SIZE>
struct BufferData {
    size_t elements;  // 0x00
    T * data;  // 0x08
    T local[SIZE];  // 0x10
    T * Local() { return local; }  // line 60
};

// Util.h:63
template <typename T>
struct BufferData<T, 0> {
    size_t elements;  // 0x00
    T * data;  // 0x08
    T * Local() { return NULL; }
};
}  // namespace BufferPrivate

// Util.h:73
template <size_t SIZE, typename T>
class Buffer : public BufferPrivate::BufferData<T, SIZE> {
public:
    // line 82
    Buffer(size_t n)
    {
        this->elements = n;
        if (n <= SIZE)
        {
            this->data = this->Local();
        }
        else
        {
            this->data = new T[n];
        }
    }
    // line 91
    ~Buffer()
    {
        if (this->data != this->Local())
        {
            delete[] this->data;
        }
    }
    operator T *() { return this->data; }  // line 98
    operator const T *() const { return this->data; }
    size_t Size() const { return this->elements; }

private:
    Buffer(const Buffer &);
    Buffer & operator=(const Buffer &);
};

}  // namespace Util
}  // namespace Blizzard
