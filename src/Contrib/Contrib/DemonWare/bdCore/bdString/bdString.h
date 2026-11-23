// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdString/bdString.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

// The bookkeeping header that sits immediately before a bdString's characters.
class bdStringData {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdStringData);

    // line 29
    bdNChar8* getString()
    {
        return reinterpret_cast<bdNChar8*>(this + 1);
    }

    bdUInt m_referenceCount;  // 0x00
    bdUWord m_length;  // 0x08
    bdUWord m_capacity;  // 0x10
};
static_assert(sizeof(bdStringData) == 0x18, "bdStringData");

class bdString {  // size 0x8
public:
    // line 52
    BD_CLASS_ALLOCATOR(bdString);

    bdString();
    bdString(const bdString &);
    bdString(const bdNChar8 *const);
    bdString(const bdNChar8 *const, const bdUWord);
    ~bdString();
    bdString & operator=(const bdString & s);
    bdString & operator=(const bdNChar8 *const s);
    bdBool operator==(const bdString & s) const;
    bdBool operator==(const bdNChar8 *const s) const;
    bdBool operator!=(const bdString &) const;
    bdBool operator!=(const bdNChar8 *const s) const;
    bdString operator+(const bdString &) const;
    bdString operator+(const bdNChar8 *const) const;
    bdString & operator+=(const bdString &);
    bdString & operator+=(const bdNChar8 *const);
    bdString & operator+=(const bdNChar8);
    bdBool findFirst(const bdNChar8, bdUWord &) const;
    bdString getSection(const bdUWord, bdUWord) const;
    bdUWord getLength() const;
    const bdNChar8 * getBuffer() const;
protected:
    bdStringData * getStringData() const;
    void initialize();
    void addReference(bdStringData * stringData) const;
    void removeReference(bdStringData * stringData) const;
    void allocateBuffer(const bdUWord length);
    void freeBuffer(bdStringData * stringData) const;
    bdBool enoughCapacity(const bdUWord length) const;
    bdNChar8 * m_string;  // 0x00
};
static_assert(sizeof(bdString) == 0x8, "bdString");
