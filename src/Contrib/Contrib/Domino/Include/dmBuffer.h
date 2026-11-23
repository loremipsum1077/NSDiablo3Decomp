// Written by hand from the DWARF (Contrib/Contrib/Domino/Include/dmBuffer.h).
// Only the members some adopted caller inlines have bodies.

#pragma once

#include "dmSerialize.h"
#include "dmSettings.h"

// dmBuffer.h:27: a fixed inline array that spills to the heap.
template <typename T, int N>
class dmBuffer {
public:
    dmBuffer();
    ~dmBuffer();
    dm_int32 GetCount() const;
    T & operator[](dm_int32 index);
    const T & operator[](dm_int32 index) const;
    T * GetPointer();
    void PushBack(const T & item);
    void SetCount(dm_int32 count);
private:
    T m_array[N];
    T * m_pData;
    dm_int32 m_capacity;
    dm_int32 m_count;
};

template <typename T, int N>
inline dmBuffer<T, N>::dmBuffer()
{
    m_pData = m_array;
    m_capacity = N;
    m_count = 0;
}

template <typename T, int N>
inline dmBuffer<T, N>::~dmBuffer()
{
    if (m_pData != m_array)
    {
        dmFree(m_pData);
    }
}

template <typename T, int N>
inline dm_int32 dmBuffer<T, N>::GetCount() const
{
    return m_count;
}

template <typename T, int N>
inline T & dmBuffer<T, N>::operator[](dm_int32 index)
{
    return m_pData[index];
}

template <typename T, int N>
inline const T & dmBuffer<T, N>::operator[](dm_int32 index) const
{
    return m_pData[index];
}

template <typename T, int N>
inline T * dmBuffer<T, N>::GetPointer()
{
    return m_pData;
}

// dmBuffer.h:147, written from dmBody::BuildIsland. Its allocation records the
// original header path and line, as dmPool<T>::Allocate does.
#include <string.h>

#line 147 "../../..\\Domino/Include/dmBuffer.h"
template <typename T, int N> void dmBuffer<T, N>::PushBack(const T & item)
{
    if (m_count == m_capacity)
    {
        m_capacity *= 2;
        T * pNewData = (T *)dmAlloc(m_capacity * sizeof(T));
        memcpy(pNewData, m_pData, m_count * sizeof(T));
        if (m_pData != m_array)
        {
            dmFree(m_pData);
        }

        m_pData = pNewData;
    }

    m_pData[m_count] = item;
    ++m_count;
}

// dmBuffer.h:103, written from the dmSimpleHull.cpp callers (Vertex::Remove,
// Vertex::Deactivate). It grows to exactly the requested count.
#line 103 "../../..\\Domino/Include/dmBuffer.h"
template <typename T, int N> void dmBuffer<T, N>::SetCount(dm_int32 count)
{
    m_count = count;
    if (m_count > m_capacity)
    {



        m_capacity = m_count;
        if (m_pData != m_array)
        {
            dmFree(m_pData);
        }
        m_pData = (T *)dmAlloc(m_capacity * sizeof(T));
    }
}
