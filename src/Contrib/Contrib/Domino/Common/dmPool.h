// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/Domino/Common/dmPool.h

#pragma once

#include "../Include/dmSerialize.h"

struct dmBody;
struct dmContact;
struct dmConvexContact;
struct dmDistanceJoint;
struct dmFixture;
struct dmHashEdge;
struct dmJoint;
struct dmMeshContact;
struct dmMouseJoint;
struct dmPhantom;
struct dmPrismaticJoint;
struct dmRevoluteJoint;
struct dmShoulderJoint;
struct dmSphericalJoint;
struct dmTempEdge;
struct dmWeldJoint;

// Read-only subset reconstructed through dmFixture and dmContact. Allocation remains declaration-only.
template <typename T> class dmPool {
public:
    dmPool();
    ~dmPool();
    void Create(dm_int32);
    void Destroy();
    dm_int32 Allocate();
    void Free(dm_int32);
    bool IsValid(dm_int32 index) const {
        return index >= 0 && index < m_capacity && m_pData[index].m_index == index;
    }
    dm_int32 GetCount() const;
    dm_int32 GetCapacity() const;
    dm_int32 GetCeiling() const;
    const T * Get(dm_int32 index) const;
    T * Get(dm_int32 index) { return m_pData + index; }
    const T & operator[](dm_int32 index) const;
    T & operator[](dm_int32 index) { return m_pData[index]; }
    T * GetPointer();
    bool IsValid() const;
    void SortFreeList();
private:
    T * m_pData;
    dm_int32 m_count;
    dm_int32 m_capacity;
    dm_int32 m_ceiling;
    dm_int32 m_freeList;
    dm_int32 m_sortedHead;
};

// dmPool.h:42, written from dmWorld_GetFirstBody.
template <typename T> inline dm_int32 dmPool<T>::GetCount() const
{
    return m_count;
}

// dmPool.h:53, written from dmWorld_GetNextBody.
template <typename T> inline dm_int32 dmPool<T>::GetCeiling() const
{
    return m_ceiling;
}

// dmPool.h:47.
template <typename T> inline dm_int32 dmPool<T>::GetCapacity() const
{
    return m_capacity;
}

// dmPool.h:212, written from dmJoint::Destroy, which inlines it.
template <typename T> inline void dmPool<T>::Free(dm_int32 index)
{
    m_pData[index].m_next = m_freeList;
    m_freeList = index;
    m_sortedHead = dmMax(m_sortedHead, index + 1);
    --m_count;
}

// dmPool.h:166, written from its out-of-line copies in dmJoint.cpp. Its
// allocation records the original header path and line.
#include "../Include/dmSettings.h"
#include <string.h>

// dmPool.h:123, written from the thirteen copies inlined into dmWorld::Create (dmWorld.cpp:97-115).
#line 123 "../../..\\Domino/Common/dmPool.h"
template <typename T> void dmPool<T>::Create(dm_int32 capacity)
{
    if (capacity == 0)
    {

        m_pData = NULL; m_count = 0; m_capacity = 0; m_ceiling = 0;


        m_freeList = -1;
        m_sortedHead = -1;
        return;
    }

    m_pData = (T *)dmAlloc(capacity * sizeof(T));
    memset(m_pData, 0, capacity * sizeof(T));
    m_capacity = capacity;
    m_count = 0;
    m_ceiling = 0;
    m_freeList = 0;


    for (dm_int32 i = 0; i < m_capacity - 1; ++i)
    {
        m_pData[i].m_next = i + 1;
    }
    m_pData[m_capacity - 1].m_next = -1;
    m_sortedHead = 0;
}

#line 154
template <typename T> void dmPool<T>::Destroy()
{
    dmFree(m_pData);

    m_pData = NULL;
    m_count = m_capacity = m_ceiling = 0;

    m_freeList = -1;
}

#line 166 "../../..\\Domino/Common/dmPool.h"
template <typename T> dm_int32 dmPool<T>::Allocate()
{
    if (m_freeList == -1)
    {


        // Grow the pool by half.
        dm_int32 newCapacity = dmMax(2, m_capacity + (m_capacity >> 1));
        T * pNewData = (T *)dmAlloc(newCapacity * sizeof(T));
        memset(pNewData, 0, newCapacity * sizeof(T));


        memcpy(pNewData, m_pData, m_capacity * sizeof(T));


        m_freeList = m_capacity;
        for (dm_int32 i = m_capacity; i < newCapacity - 1; ++i)
        {
            pNewData[i].m_next = i + 1;
        }
        pNewData[newCapacity - 1].m_next = -1;
        m_sortedHead = m_capacity;


        dmFree(m_pData);
        m_pData = pNewData;
        m_capacity = newCapacity;
    }


    dm_int32 index = m_freeList;
    m_freeList = m_pData[index].m_next;


    m_pData[index].m_index = index;
    ++m_count;

    m_ceiling = dmMax(m_ceiling, index + 1);
    m_sortedHead = dmMax(m_sortedHead, index + 1);
    return index;
}

// dmPool.h:245, written from its fourteen out-of-line copies in dmWorld.cpp.
#line 245 "../../..\\Domino/Common/dmPool.h"
template <typename T> void dmPool<T>::SortFreeList()
{
    if (m_freeList == m_sortedHead)
    {
        return;
    }


    if (m_freeList > m_sortedHead)
    {
        m_sortedHead = m_freeList;
        return;
    }

    if (m_freeList == -1)
    {
        return;
    }

    dm_int32 sortedHead = m_sortedHead;
    dm_int32 ceiling = dmMin(sortedHead, m_ceiling);
    for (dm_int32 i = 0; i < ceiling; ++i)
    {
        if (m_pData[i].m_index != i)
        {
            m_freeList = i;
            break;
        }
    }

    dm_int32 prevFree = m_freeList;

    for (dm_int32 i = prevFree + 1; i < ceiling; ++i)
    {
        if (m_pData[i].m_index != i)
        {
            m_pData[prevFree].m_next = i;
            prevFree = i;
        }
    }

    dm_int32 capacity = dmMin(sortedHead, m_capacity);
    for (dm_int32 i = ceiling; i < capacity; ++i)
    {
        m_pData[prevFree].m_next = i;
        prevFree = i;
    }


    for (dm_int32 i = sortedHead; i < m_capacity; ++i)
    {
        if (m_pData[i].m_index != i)
        {
            m_pData[prevFree].m_next = i;

            sortedHead = i;
            break;
        }
    }

    if (m_pData[prevFree].m_next != sortedHead)
    {
        m_pData[prevFree].m_next = -1;
    }

    m_sortedHead = m_freeList;
}

// Recovered from the eleven GetPointer inline sites in dmIsland::Create.
// No out-of-line original body is present: its body is not independently checkable.
// DW_INL_inlined only: no explicit inline keyword.
#line 82 "../../..\\Domino/Common/dmPool.h"
template <typename T> T * dmPool<T>::GetPointer()
{
    return m_pData;
}
