// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdSequenceNumber.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdSequenceNumber {  // size 0x4
public:
    BD_CLASS_ALLOCATOR(bdSequenceNumber);
    bdSequenceNumber(const bdInt);
    bdSequenceNumber(const bdSequenceNumber &, const bdUInt, const bdUInt);
    void set(const bdSequenceNumber & last, const bdUInt seqNumber, const bdUInt bits);
    bdInt32 getValue() const;
    bdSequenceNumber operator+(const bdSequenceNumber & other) const;
    bdSequenceNumber & operator+=(const bdSequenceNumber &);
    bdSequenceNumber & operator++();
    bdSequenceNumber operator++(bdInt);
    bdSequenceNumber operator-(const bdSequenceNumber & other) const;
    bdBool operator>(const bdSequenceNumber & other) const;
    bdBool operator<(const bdSequenceNumber & other) const;
    bdBool operator<=(const bdSequenceNumber & other) const;
    bdBool operator>=(const bdSequenceNumber &) const;
    bdBool operator==(const bdSequenceNumber & other) const;
    bdBool operator!=(const bdSequenceNumber & other) const;
protected:
    bdInt m_seqNum;  // 0x00
};
static_assert(sizeof(bdSequenceNumber) == 0x4, "bdSequenceNumber");

