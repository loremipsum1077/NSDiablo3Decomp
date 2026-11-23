// Original bdFastArray.h. Minimal template surface from DWARF instantiations.
#pragma once
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory.inl"
template<typename T> class bdFastArray {
public:
    bdFastArray(const bdUInt capacity = 0);
    bdFastArray(const bdFastArray & a);
    ~bdFastArray();
    const T & operator[](bdUInt i) const;
    void pushBack(const T & value);
    void decreaseCapacity(bdUInt decrease);
    void removeSection(bdUInt begin, bdUInt end);
    void removeAllKeepOrder(const T value);
    void popBack();
    void removeAt(const bdUInt i);
    void removeAll(const T value);
    bdBool findFirst(const T & value, bdUInt & i) const;
    T& operator[](const bdUInt i);
    bdUInt getSize() const;
    bdBool rangeCheck(const bdUInt i) const;
    void increaseCapacity(const bdUInt increase);
    void ensureCapacity(const bdUInt capacity);
    void setGrow(const bdUInt i, const T& value);
    void pushBack(const T* value, const bdUInt n);
    void clear();
    T* begin();
    T* end();
protected:
    void copyArrayArray(T* dest, const T* src, const bdUInt n) const;
    void copyObjectObject(T * dest, const T & src) const;
    T * uninitializedCopy(const bdFastArray & a) const;
    T* m_data;
    bdUInt m_capacity;
    bdUInt m_size;
};
#include "bdFastArray.inl"
