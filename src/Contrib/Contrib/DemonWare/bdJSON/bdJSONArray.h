// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdJSON/bdJSONArray.h

#pragma once

#include "../bdCore/bdContainers/bdArray.h"
#include "../bdCore/bdMemory/bdMemory-std.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdJSONValue.h"
#include <stddef.h>
#include <stdint.h>

class bdJSONArray {  // size 0x10
    // bdJSONValue owns its arrays (new bdJSONArray, copy, parse, serialize).
    friend class bdJSONValue;
protected:
    // line 35
    BD_CLASS_ALLOCATOR(bdJSONArray);

    // line 38
    bdJSONArray() {}
    // line 39
    ~bdJSONArray() {}
    bdJSONArray(const bdJSONArray &);
    bdJSONArray & operator=(const bdJSONArray &);
public:
    // line 45
    void pushBack(const bdJSONValue & value)
    {
        m_array.pushBack(value);
    }
    bdUInt32 size();
    bdJSONValue & operator[](bdUInt32);
    typedef const bdJSONValue * jsonArrayIterator;
    bdJSONArray::jsonArrayIterator begin();
    bdJSONArray::jsonArrayIterator end();
protected:
    bdArray<bdJSONValue> m_array;  // 0x00
    bdBool parse(const bdNChar8 * jsonPtr, const bdNChar8 ** outPtr);
    bdInt32 serialize(bdNChar8 *, bdUInt32) const;
};
static_assert(sizeof(bdJSONArray) == 0x10, "bdJSONArray");
