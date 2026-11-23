// Original file: Contrib/Contrib/Domino/Common/dmArray.h
// Supported storage operations reconstructed from dmBroadPhase original callers.
#pragma once
#include "../Include/dmSettings.h"
#include "../Include/dmMath.h"
#include <string.h>
template <typename T> class dmArray {
public:
    dmArray();
    ~dmArray();
    void Create() { m_pData = 0; m_capacity = 0; m_count = 0; }
    void Destroy() { dmFree(m_pData); m_pData = 0; m_capacity = 0; m_count = 0; }
    void SetCapacity(dm_int32 capacity) {
        if (capacity > m_capacity) {
            m_capacity = capacity;
            dmFree(m_pData);
            m_pData = static_cast<T *>(dmAllocFunction(sizeof(T) * m_capacity, 16,
                "../../..\\Domino/Common/dmArray.h", 106, "SetCapacity"));
        }
    }
    // dmArray.h:118; the allocation records original line 128.
    void SetCount(dm_int32 count) {
        m_count = count;
        if (m_capacity < count) {
            m_capacity = count;
            dmFree(m_pData);
            m_pData = static_cast<T *>(dmAllocFunction(sizeof(T) * m_capacity, 16,
                "../../..\\Domino/Common/dmArray.h", 128, "SetCount"));
        }
    }
    dm_int32 GetCount() const { return m_count; }
    T * GetPointer() { return m_pData; }  // dmArray.h:156
    void PushBack(const T & item) {
        if (m_count == m_capacity) ExpandCapacity();
        m_pData[m_count] = item;
        ++m_count;
    }
    T & operator[](dm_int32 index) { return m_pData[index]; }
private:
    void ExpandCapacity() {
        m_capacity = dmMax(2 * m_count, 2);
        T * pNewData = static_cast<T *>(dmAllocFunction(sizeof(T) * m_capacity, 16,
            "../../..\\Domino/Common/dmArray.h", 202, "ExpandCapacity"));
        memcpy(pNewData, m_pData, sizeof(T) * m_count);
        dmFree(m_pData);
        m_pData = pNewData;
    }
    T * m_pData;
    dm_int32 m_capacity;
    dm_int32 m_count;
};
