// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdJSON/bdJSONValue.h

#pragma once

#include "../bdCore/bdMemory/bdMemory-std.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdJSONArray;
class bdJSONObject;
class bdString;

enum bdJSONType {
    BD_JSON_NULL = 0,
    BD_JSON_STRING = 1,
    BD_JSON_NUMBER = 2,
    BD_JSON_BOOLEAN = 3,
    BD_JSON_ARRAY = 4,
    BD_JSON_OBJECT = 5,
};

class bdJSONValue {  // size 0x10
public:
    BD_CLASS_ALLOCATOR(bdJSONValue);
    bdJSONValue();
    ~bdJSONValue();
    bdJSONValue(bdJSONType);
    bdJSONValue(const bdJSONValue &);
    bdJSONValue(const bdInt64);
    bdJSONValue(const bdUInt64);
    bdJSONValue(const bdFloat64);
    bdJSONValue(const bdBool);
    bdJSONValue(const bdString &);
    bdJSONValue(const bdNChar8 *);
    bdJSONValue(const bdJSONObject &);
    bdJSONValue(const bdJSONArray &);
    bdJSONValue & operator=(const bdInt64);
    bdJSONValue & operator=(const bdUInt64 number);
    bdJSONValue & operator=(const bdFloat64);
    bdJSONValue & operator=(const bdBool);
    bdJSONValue & operator=(const bdString &);
    bdJSONValue & operator=(const bdNChar8 *);
    bdJSONValue & operator=(const bdJSONValue & other);
    bdInt32 serialize(bdNChar8 *, const bdUInt32) const;
    bdString serialize() const;
    bdBool parse(const bdNChar8 *const jsonStr, const bdNChar8 ** outPtr);
    bdBool parse(const bdString &);
    bdJSONType m_type;  // 0x00
    bdJSONType getType() const;
    bdBool getBool(bdBool &);
    bdBool getInt64(bdInt64 &);
    bdBool getUInt64(bdUInt64 &);
    bdBool getFloat64(bdFloat64 &);
    bdBool getString(bdString &);
    bdJSONObject * getObject() const;
    bdJSONArray * getArray() const;
    bdString * getString() const;
    void cleanup();
protected:
    union {  // size 0x8
        bdString * m_string;  // 0x00
        bdJSONObject * m_object;  // 0x00
        bdJSONArray * m_array;  // 0x00
    };  // 0x08
    static bdBool parseValue(const bdNChar8 * jsonPtr, const bdNChar8 ** outPtr, bdJSONValue & target);
    static bdBool parseNumber(const bdNChar8 *& json, bdJSONValue & target);
    static bdBool parseLiteral(const bdNChar8 * jsonPtr, const bdNChar8 ** outPtr, bdJSONValue & target);
    static bdBool parseString(const bdNChar8 *& json, bdJSONValue & target);
    bdBool assignNumber(const bdInt64 number);
    bdBool assignNumber(const bdUInt64 number);
    bdBool assignNumber(const bdFloat64 number);
};
static_assert(sizeof(bdJSONValue) == 0x10, "bdJSONValue");


// Variables defined in this file:
//   line    74  static const bdNChar8 *const bdJSONTypeString[6]
