// Original file: Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h
// Hand-written: the DWARF only has instantiations of this template. Member names, order and
// declaration lines are the DWARF's; T must derive from bdReferencable (addRef / releaseRef /
// virtual destructor).

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"
#include <stddef.h>

// line 24
template <typename T>
class bdReference {  // size 0x8
public:
    // line 28
    BD_CLASS_ALLOCATOR(bdReference);

    // line 31
    bdReference();
    // line 36
    bdReference(T* p);
    // line 40
    bdReference(const bdReference<T>& other);
    template <typename U>
    bdReference(const bdReference<U>& other);
    // line 48
    ~bdReference();
    // line 52
    void operator=(T* p);
    // line 58
    bdReference<T>& operator=(const bdReference<T>& other);
    template <typename U>
    bdReference<T>& operator=(const bdReference<U>& other);
    // line 74
    template <typename U>
    bdBool operator==(const bdReference<U>& other) const;
    template <typename U>
    bdBool operator!=(const bdReference<U>& other) const;
    // line 91
    bdBool operator!() const;
    // line 100
    bdBool isNull() const;
    // line 109
    bdBool notNull() const;
    // line 113
    operator T*() const;
    // line 117
    T* operator->();
    // line 121
    T* operator->() const;
    // line 125
    T& operator*();
    // line 129
    const T& operator*() const;
    // line 140
    bdBool operator<(const bdReference<T>& other) const;
    // line 151
    bdBool operator>(const bdReference<T>& other) const;

protected:
    // line 156
    T* m_ptr;  // 0x00
};

#include "bdReference.inl"

// line 175. The inline tree shows the non-const operator-> called from line 177.
template <typename T, typename U>
inline T* reference_cast(bdReference<U> referenceClass)
{
    return static_cast<T*>(referenceClass.operator->());
}
