#pragma once

#include "../SigmaTypes.h"

#include <stddef.h>









template <typename TYPE, typename ARG_TYPE, int CAPACITY>
class XRingBuffer {
public:

    inline XRingBuffer(BOOL bDequeueIfAtCapacity)

        : m_bDequeueIfAtCapacity(bDequeueIfAtCapacity) { Empty(); }

    inline void Empty() { m_uHead = m_uTail = m_uCount = 0; }
    inline size_t Count() { return m_uCount; }
    inline size_t Capacity() { return CAPACITY; }

    inline void Enqueue(ARG_TYPE tData) {

        if (m_bDequeueIfAtCapacity && Count() == CAPACITY) {

            Dequeue();
        }



        m_tElements[m_uTail] = tData;

        m_uTail++;
        if (m_uTail == CAPACITY + 1) {
            m_uTail = 0;
        }


        m_uCount++;
    }

    inline TYPE& Dequeue() {



        size_t uRetElementIndex = m_uHead;

        m_uHead++;
        if (m_uHead == CAPACITY + 1) {
            m_uHead = 0;
        }

        m_uCount--;

        return m_tElements[uRetElementIndex];
    }

    inline TYPE& operator[](const size_t index) {




        size_t normalizedIndex = m_uHead + index;

        if (normalizedIndex > CAPACITY) {
            normalizedIndex -= CAPACITY + 1;
        }

        return m_tElements[normalizedIndex];
    }

protected:
    BOOL m_bDequeueIfAtCapacity;
    size_t m_uHead;
    size_t m_uTail;
    size_t m_uCount;
    TYPE m_tElements[CAPACITY + 1];
};
