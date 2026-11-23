// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdArray.inl
#pragma once
#include "../bdMemory/bdMemory.inl"
#include <new>
// On the original's lines: member names and DWARF locals on their DWARF lines (build/try/h32/ar_locals.txt).
// get (222), pushBack(const T*, n) (299), begin/end (420/432) are not written here.
template <typename T>
// Inferred initializer spelling from dispatchAll: zero pointer at line 14, capacity test at 18, allocation at 20.




bdArray<T>::bdArray(const bdUInt capacity)
    : m_data(0),
      m_capacity(capacity),
      m_size(0)
{
    if (m_capacity)
    {
        m_data = bdAllocate<T>(m_capacity);
    }
}

template <typename T>
inline bdArray<T>::bdArray(const bdUInt size, const T & value)
    : m_data(0), m_capacity(size), m_size(size)
{
    if (m_capacity) {
        m_data = bdAllocate<T>(m_capacity);
        copyConstructArrayObject(m_data, value, m_capacity);
    }
}





template <typename T>
inline bdArray<T>::bdArray(const bdArray & a)
    : m_capacity(a.getCapacity()), m_size(a.getSize()) { m_data = uninitializedCopy(a); }





template <typename T>
inline bdArray<T>::~bdArray() { clear(); }
// Statement order from the line table of the bdQoSProbeEntryWrapper instance
// (0x7100b517b0): 56 self check, 58 newSize, 59 capacity test, 62-64 reallocate,


template <typename T>
inline void bdArray<T>::operator=(const bdArray & a)
{

    if (this != &a) {

        const bdUInt newSize = a.getSize();
        if (newSize > m_capacity) {


            clear();
            m_data = uninitializedCopy(a);
            m_capacity = a.m_capacity;
            m_size = newSize;

        } else if (newSize > m_size) {





            for (bdUInt i = 0; i < m_size; ++i) m_data[i] = a[i];





            copyConstructArrayArray(m_data + m_size, a.m_data + m_size, newSize - m_size);
            m_size = newSize;
        } else {





            for (bdUInt i = 0; i < newSize; ++i) m_data[i] = a[i];




            destruct(m_data + newSize, m_size - newSize);

            m_size = newSize;
            decreaseCapacity(0);
        }
    }
}


template <typename T>
inline T & bdArray<T>::operator[](const bdUInt i) { return m_data[i]; }






template <typename T>
inline const T & bdArray<T>::operator[](const bdUInt i) const { return m_data[i]; }






template <typename T>
inline bdBool bdArray<T>::isEmpty() const { return m_size == 0; }




template <typename T>
inline bdUInt bdArray<T>::getSize() const { return m_size; }




template <typename T>
inline bdUInt bdArray<T>::getCapacity() const { return m_capacity; }




template <typename T>
inline void bdArray<T>::increaseCapacity(const bdUInt increase)
{

    const bdUInt newCapacity = m_capacity + increase;

    T * newData = 0;
    if (newCapacity) {
        newData = bdAllocate<T>(newCapacity);
        copyConstructArrayArray(newData, m_data, m_size);
    }
    destruct(m_data, m_size);
    bdDeallocate(m_data);
    m_data = newData;
    m_capacity = newCapacity;
}

















template <typename T>
inline void bdArray<T>::decreaseCapacity(const bdUInt decrease)
{
    if (m_capacity > m_size * 4) {

        const bdUInt spare = m_capacity - m_size;
        const bdUInt possibleDecrease = spare < decrease ? spare : decrease;

        m_capacity -= possibleDecrease > (m_capacity >> 1) ? possibleDecrease : (m_capacity >> 1);








        T* newData = 0;
        if (m_capacity) {
            newData = bdAllocate<T>(m_capacity);
            copyConstructArrayArray(newData, m_data, m_size);
        }
        destruct(m_data, m_size);
        bdDeallocate(m_data);
        m_data = newData;
    }
}
// Original lines 200, 267 and 479, reconstructed from bdAntiCheatChallenge::deserialize.



template <typename T>
inline void bdArray<T>::ensureCapacity(const bdUInt capacity)
{
    if (m_capacity < capacity)
    {
        const bdUInt increase = capacity - m_capacity;  // The DWARF puts the max below inside increaseCapacity (139); moving it there stops increaseCapacity inlining in bdAddressMap::getAddrHandle, which matches as written.
        increaseCapacity(m_capacity < increase ? increase : m_capacity);
    }
}

template <typename T>
inline void bdArray<T>::clear()
{
    destruct(m_data, m_size);
    bdDeallocate(m_data);
    m_data = 0;
    m_capacity = 0;
    m_size = 0;
}
















































template <typename T>
inline void bdArray<T>::setGrow(const bdUInt i, const T & value)
{

    const bdBool inRange = rangeCheck(i);
    if (inRange)
    {
        m_data[i] = value;
    }
    else
    {
        ensureCapacity(i + 1);
        defaultConstructArray(m_data + m_size, i - m_size);
        copyConstructObjectObject(m_data + i, value);
        m_size = i + 1;
    }
}



template <typename T>
inline void bdArray<T>::pushBack(const T & value)
{
    if (m_size == m_capacity) increaseCapacity(m_capacity ? m_capacity : 1);
    copyConstructObjectObject(m_data + m_size, value);
    ++m_size;
}



















template <typename T>
inline void bdArray<T>::popBack()
{
    if (m_size) {

        --m_size;
        destruct(m_data + m_size, 1);
        decreaseCapacity(0);
    }
}


















template <typename T>
inline bdBool bdArray<T>::rangeCheck(const bdUInt i) const { return i < m_size; }
// Rows (bdAddressMap::unregisterAddr): 366 if; 368 one unguarded copy (cols 15/28: `m_data[i] = m_data[m_size-1]`,
// the only compare is bdReference::operator='s self test), then popBack.

// DWARF bdArray.inl:347 (params value/i, local j at 350); body from the copy inlined into bdNetImpl::pump: rows 350 cols 2/25/34 (`j++`), 352 cols 7/13/16. The `i = j` store leaves no row there (index is dead in pump), same shape as bdFastArray::findFirst.
template <typename T>
inline bdBool bdArray<T>::findFirst(const T & value, bdUInt & i) const
{

    for (bdUInt j = 0; j < m_size; j++)
    {
        if (value == m_data[j])
        {
            i = j;
            return true;
        }
    }
    return false;
}


template <typename T>
inline void bdArray<T>::removeAt(const bdUInt i)
{
    const bdBool inRange = rangeCheck(i);
    if (inRange) {

        m_data[i] = m_data[m_size-1];
        popBack();
    }
}


















































































template <typename T>
inline void bdArray<T>::copyConstructArrayArray(T * dest, const T * src, const bdUInt n) const
{


    for (bdUInt i = 0; i < n; ++i) ::new (dest + i) T(src[i]);
}






template <typename T>
inline void bdArray<T>::copyConstructArrayObject(T * dest, const T & src, const bdUInt n) const
{


    for (bdUInt i = 0; i < n; ++i) ::new (dest + i) T(src);
}




template <typename T>
inline void bdArray<T>::defaultConstructArray(T * dest, const bdUInt n) const
{

    for (bdUInt i = 0; i < n; ++i)
    {
        new (dest + i) T();
    }
}

template <typename T>
inline void bdArray<T>::copyConstructObjectObject(T * dest, const T & src) const
{
    ::new (dest) T(src);
}


template <typename T>
inline void bdArray<T>::destruct(T * src, const bdUInt n) const
{





    for (bdUInt i = 0; i < n; ++i) src[i].~T();
}




template <typename T>
inline T * bdArray<T>::uninitializedCopy(const bdArray & a) const
{
    T * data = 0;
    if (a.m_capacity) {
        data = bdAllocate<T>(a.m_capacity);
        copyConstructArrayArray(data, a.m_data, a.m_size);
    }
    return data;
}
