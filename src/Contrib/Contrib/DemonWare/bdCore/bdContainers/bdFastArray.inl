// Original bdFastArray.inl; names and operations evidenced by bdBitBuffer callers.
#pragma once
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
template<typename T> inline bdFastArray<T>::bdFastArray(const bdUInt capacity)
    : m_data(0), m_capacity(capacity), m_size(0) {
    if (capacity) m_data = bdAllocate<T>(capacity);
}
template<typename T> inline bdFastArray<T>::~bdFastArray()
{
    clear();
}
template<typename T> inline T& bdFastArray<T>::operator[](const bdUInt i)
{
    return m_data[i];
}
template<typename T> inline bdUInt bdFastArray<T>::getSize() const
{
    return m_size;
}
template<typename T> inline void bdFastArray<T>::increaseCapacity(const bdUInt increase) {
    const bdUInt newCapacity = m_capacity + (m_capacity < increase ? increase : m_capacity);
    T* newData = 0;
    if (newCapacity) {
        newData = bdAllocate<T>(newCapacity);
        copyArrayArray(newData, m_data, m_size);
    }
    bdDeallocate(m_data);
    m_data = newData;
    m_capacity = newCapacity;
}
template<typename T> inline void bdFastArray<T>::ensureCapacity(const bdUInt capacity) {
    if (capacity > m_capacity) {
        const bdUInt increase = capacity - m_capacity;
        increaseCapacity(increase);
    }
}
template<typename T> inline void bdFastArray<T>::setGrow(const bdUInt i, const T& value) {
    if (i >= m_size) {
        ensureCapacity(i + 1);
        m_size = i + 1;
    }
    m_data[i] = value;
}
template<typename T> inline void bdFastArray<T>::pushBack(const T* value, const bdUInt n) {
    ensureCapacity(m_size + n);
    copyArrayArray(m_data + m_size, value, n);
    m_size += n;
}
template<typename T> inline bdBool bdFastArray<T>::rangeCheck(const bdUInt i) const
{
    return i < m_size;
}
template<typename T> inline void bdFastArray<T>::clear() {
    bdDeallocate(m_data);
    m_data = 0;
    m_capacity = 0;
    m_size = 0;
}
template<typename T> inline T* bdFastArray<T>::begin()
{
    return m_data;
}
// line 371
template<typename T> inline T* bdFastArray<T>::end()
{
    return m_data + m_size;
}
template<typename T> inline void bdFastArray<T>::copyArrayArray(T* dest, const T* src, const bdUInt n) const {
    if (n) bdMemcpy(dest, src, n * sizeof(T));
}

// Original line 39: copy constructor used by connection listener snapshots.
template<typename T> inline bdFastArray<T>::bdFastArray(const bdFastArray & a)
 : m_capacity(a.m_capacity), m_size(a.m_size) { m_data = uninitializedCopy(a); }
// line 86
template<typename T> inline const T & bdFastArray<T>::operator[](bdUInt i) const { return m_data[i]; }
// line 131
template<typename T> inline void bdFastArray<T>::decreaseCapacity(bdUInt decrease) {
    const bdUInt possibleDecrease = m_capacity >> 1;
    if (decrease > possibleDecrease) decrease = possibleDecrease;
    m_capacity -= decrease;
    T * newData = 0;
    if (m_capacity) newData = bdReallocate(m_data, m_capacity);
    m_data = newData;
}
// line 218
template<typename T> inline void bdFastArray<T>::pushBack(const T & value) {
    if (m_size == m_capacity) increaseCapacity(1);
    copyObjectObject(m_data + m_size, value); ++m_size;
}
// line 269
template<typename T> inline bdBool bdFastArray<T>::findFirst(const T & value, bdUInt & i) const {
    for (bdUInt j = 0; j < m_size; ++j) if (value == m_data[j]) { i = j; return true; }
    return false;
}
// line 296
template<typename T> inline void bdFastArray<T>::removeSection(bdUInt begin, bdUInt end) {
    if (end <= m_size) {
        T * dest = m_data + begin;
        bdMemmove(dest, m_data + end, (m_size - end) * sizeof(T));
        m_size -= end - begin;
        if (m_capacity > m_size * 4) decreaseCapacity(m_capacity >> 1);
    }
}
// line 238
template<typename T> inline void bdFastArray<T>::popBack() {
    if (m_size > 0) {
        --m_size;
        if (m_capacity > m_size * 4) decreaseCapacity(m_capacity >> 1);
    }
}
// line 285
template<typename T> inline void bdFastArray<T>::removeAt(const bdUInt i) {
    if (rangeCheck(i)) { m_data[i] = m_data[m_size - 1]; popBack(); }
}
// line 314
template<typename T> inline void bdFastArray<T>::removeAll(const T value) {
    for (bdUInt i = 0; i < m_size; ++i) { if (m_data[i] == value) { removeAt(i); --i; } }
}
// line 329
template<typename T> inline void bdFastArray<T>::removeAllKeepOrder(const T value) {
    for (bdUInt i = 0; i < m_size; ++i) { if (m_data[i] == value) { removeSection(i, i + 1); --i; } }
}
// line 412
template<typename T> inline void bdFastArray<T>::copyObjectObject(T * dest, const T & src) const { bdMemcpy(dest, &src, sizeof(T)); }
// line 422
template<typename T> inline T * bdFastArray<T>::uninitializedCopy(const bdFastArray & a) const {
    T * data = 0;
    if (a.m_capacity) { data = bdAllocate<T>(a.m_capacity); copyArrayArray(data, a.m_data, a.m_size); }
    return data;
}
