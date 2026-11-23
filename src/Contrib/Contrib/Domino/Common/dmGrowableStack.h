// Original file: Contrib/Contrib/Domino/Common/dmGrowableStack.h
// Written by hand from the DWARF (class at line 21) and from the copies that
// dmDynamicTree::RayCast inlines. The #line directive puts every member on its
// original line; Push records the original header path and line 49.

#pragma once

#include "../Include/dmSerialize.h"
#include "../Include/dmSettings.h"
#include <string.h>

#line 20 "../../..\\Domino/Common/dmGrowableStack.h"
template <typename T, int N>
class dmGrowableStack
{
public:

    dmGrowableStack()
    {
        m_pStack = m_array;
        m_count = 0;
        m_capacity = N;
    }


    ~dmGrowableStack()
    {
        if (m_pStack != m_array)
        {
            dmFree(m_pStack);
            m_pStack = NULL;
        }
    }


    void Push(const T & element)
    {
        if (m_count == m_capacity)
        {
            T * old = m_pStack;
            m_capacity *= 2;
            m_pStack = (T *)dmAlloc(m_capacity * sizeof(T));
            memcpy(m_pStack, old, m_count * sizeof(T));
            if (old != m_array)
            {
                dmFree(old);
            }
        }

        m_pStack[m_count] = element;
        ++m_count;
    }


    T Pop()
    {

        --m_count;
        return m_pStack[m_count];
    }


    dm_int32 GetCount()
    {
        return m_count;
    }

private:
    T * m_pStack;
    T m_array[N];
    dm_int32 m_count;
    dm_int32 m_capacity;
};
// The DWARF has one instantiation: dmGrowableStack<int, 128>, size 0x210.
static_assert(sizeof(dmGrowableStack<int, 128>) == 0x210, "dmGrowableStack<int, 128>");
