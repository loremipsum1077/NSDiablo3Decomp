// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdJSON/bdJSONObject.h

#pragma once

#include "../bdCore/bdContainers/bdArray.h"
#include "../bdCore/bdContainers/bdHashMap.h"
#include "../bdCore/bdMemory/bdMemory-std.h"
#include "../bdCore/bdString/bdString.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdJSONValue.h"
#include <stddef.h>
#include <stdint.h>

class bdJSONObject {  // size 0x20
    // bdJSONValue owns its objects (new bdJSONObject, copy, parse, serialize).
    friend class bdJSONValue;
protected:
    // line 36
    BD_CLASS_ALLOCATOR(bdJSONObject);

    // line 40
    bdJSONObject() : m_map(4, 0.75f) {}
    ~bdJSONObject();
    bdJSONObject(const bdJSONObject &);
    bdJSONObject & operator=(const bdJSONObject &);
    typedef bdHashMap<bdString, bdJSONValue, bdHashingClass> bdJSONObjectMap;
public:
    typedef bdJSONObjectMap::Iterator jsonObjectIterator;
    bdBool containsKey(const bdString &);
    bdBool containsKey(const bdNChar8 *);
    bdJSONValue & operator[](const bdString &);
    bdJSONValue & operator[](const bdNChar8 * key);
    bdBool removeKey(const bdNChar8 *);
    void getAllKeys(bdArray<bdString> &) const;
protected:
    bdJSONObject::bdJSONObjectMap & getMap();
    bdJSONObject::bdJSONObjectMap m_map;  // 0x00
    bdBool parse(const bdNChar8 * jsonPtr, const bdNChar8 ** outPtr);
    bdInt32 serialize(bdNChar8 *, bdUInt32) const;
};
static_assert(sizeof(bdJSONObject) == 0x20, "bdJSONObject");
