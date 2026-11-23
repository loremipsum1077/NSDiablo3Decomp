// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdArray.h.
// Subset reconstructed from the bdAddr callers in bdCommonAddr.cpp.
// Other element types and the unobserved API still need their own caller proofs.
#pragma once
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"
template <typename T> class bdArray {  // size 0x10
public:
    BD_CLASS_ALLOCATOR(bdArray);  // line 29
    bdArray(const bdUInt capacity = 0);
    bdArray(const bdUInt size, const T & value);
    bdArray(const bdArray &);
    ~bdArray();
    void operator=(const bdArray & a);
    bdBool isEmpty() const;
    T & operator[](const bdUInt i);
    const T & operator[](const bdUInt i) const;
    bdUInt getSize() const;
    bdUInt getCapacity() const;
    inline void increaseCapacity(const bdUInt increase);  // inferred: in-class `inline` gives clang 5 the inline hint (out-of-line `inline` on a class-template member does not); needed for pushBack<bdAchievementState>/<bdString> inlining as in the original
    inline void decreaseCapacity(const bdUInt decrease);  // inferred, same reason: the original inlines it into operator=<bdAddr>/<bdInetAddr>/<bdString>/<bdJSONValue> and bdAddressMap::unregisterAddr (DWARF: inlined into 5 functions)
    void popBack();
    bdBool rangeCheck(const bdUInt i) const;
    void removeAt(const bdUInt i);
    void clear();
    inline void pushBack(const T & value);  // inferred, same reason (bdNetStartParams ctor inlines it four times)
    void ensureCapacity(const bdUInt capacity);
    void setGrow(const bdUInt i, const T & value); bdBool findFirst(const T & value, bdUInt & i) const;  // findFirst: DWARF bdArray.h:238-239
protected:
    void copyConstructArrayObject(T * dest, const T & src, const bdUInt n) const;
    void copyConstructArrayArray(T * dest, const T * src, const bdUInt n) const;
    void copyConstructObjectObject(T * dest, const T & src) const;
    void defaultConstructArray(T * dest, const bdUInt n) const;
    void destruct(T * src, const bdUInt n) const;
    T * uninitializedCopy(const bdArray & a) const;
    T * m_data;  // 0x00
    bdUInt m_capacity;  // 0x08
    bdUInt m_size;  // 0x0c
};

#include "bdArray.inl"
