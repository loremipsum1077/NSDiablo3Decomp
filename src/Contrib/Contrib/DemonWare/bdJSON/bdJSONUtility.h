// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdJSON/bdJSONUtility.h

#pragma once

#include "../bdCore/bdMemory/bdMemory-std.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

// Values from the DWARF const_value where recorded (lines 21-61). The sizes
// marked "unrecorded" have no DWARF value and no use in the binary; they are
// the obvious lengths. The STRING_OVERHEAD, NULL_TERMINATOR, BEGIN/END_ARRAY,
// BEGIN/END_OBJECT and ESCAPED_ERROR sizes are recorded in the
// bdJSONSerializer.cpp unit.
namespace bdJSON
{
static const bdNChar8 BD_JSON_NULL_CHAR = '\0';
static const bdNChar8 BD_JSON_SPACE_CHAR = ' ';
static const bdNChar8 BD_JSON_TAB_CHAR = '\t';
static const bdNChar8 BD_JSON_CR_CHAR = '\r';
static const bdNChar8 BD_JSON_LF_CHAR = '\n';
static const bdNChar8 BD_JSON_FF_CHAR = '\f';
static const bdNChar8 BD_JSON_BACK_CHAR = '\b';
static const bdNChar8 BD_JSON_QUOTATION_CHAR = '"';
static const bdNChar8 BD_JSON_ESCAPE_CHAR = '\\';
static const bdNChar8 BD_JSON_UNICODE_CHAR = 'u';
static const bdUInt BD_JSON_ESCAPED_UNICODE_SIZE = 6;
static const bdUInt BD_JSON_ESCAPED_SURROGATE_PAIR_SIZE = 12;
static const bdUInt BD_JSON_STRING_OVERHEAD_SIZE = 2;
static const bdUInt BD_JSON_NULL_TERMINATOR_SIZE = 1;
static const bdUInt BD_JSON_BEGIN_ARRAY_SIZE = 2;
static const bdUInt BD_JSON_END_ARRAY_SIZE = 2;
static const bdUInt BD_JSON_BEGIN_OBJECT_SIZE = 2;
static const bdUInt BD_JSON_END_OBJECT_SIZE = 2;
static const bdInt BD_JSON_ESCAPE_ERROR = -1;  // unrecorded
static const bdInt BD_JSON_ESCAPED_ERROR_SIZE = 1;
static const bdNChar8 BD_JSON_BEGIN_ARRAY = '[';
static const bdNChar8 BD_JSON_END_ARRAY = ']';
static const bdNChar8 BD_JSON_BEGIN_OBJECT = '{';
static const bdNChar8 BD_JSON_END_OBJECT = '}';
static const bdNChar8 BD_JSON_NAME_SEPARATOR = ':';
static const bdNChar8 BD_JSON_VALUE_SEPARATOR = ',';
static const bdNChar8 BD_JSON_TRUE[5] = "true";
static const bdNChar8 BD_JSON_FALSE[6] = "false";
static const bdNChar8 BD_JSON_NULLSTR[5] = "null";
static const bdUInt BD_JSON_TRUE_SIZE = 4;
static const bdUInt BD_JSON_FALSE_SIZE = 5;
static const bdUInt BD_JSON_NULLSTR_SIZE = 4;
}

class bdJSONUtility {  // size 0x1
public:
    BD_CLASS_ALLOCATOR(bdJSONUtility);
    struct bdJSONCodePoint {  // size 0x8
        bdUInt32 m_codePoint;  // 0x00
        bdByte8 m_numReprBytes;  // 0x04
        bdByte8 m_numRead;  // 0x05
        bdBool m_valid;  // 0x06
    };
    // line 82
    static const bdInt32 BD_JSON_ESCAPE_ERROR = -1;
    static bdJSONUtility::bdJSONCodePoint getCodePointFromJSON(const bdNChar8 * ptr);
    static bdJSONUtility::bdJSONCodePoint getCodePointFromUTF8(const bdNChar8 * ptr);
    static bdInt32 escapeString(const bdNChar8 *const input, bdNChar8 *const outBuffer, const bdUInt32 outBufferSz);
    static bdInt32 deescapeString(const bdNChar8 *const input, bdNChar8 *const outBuffer, const bdUInt32 outBufferSz);
    static bdInt32 deescapeString(const bdNChar8 *const input, const bdUInt32 inputSz, bdNChar8 *const outBuffer, const bdUInt32 outBufferSz);
    static const bdNChar8 * skipWhitespace(const bdNChar8 * ptr);
    static const bdNChar8 * skipSeparator(const bdNChar8 * start, const bdNChar8 separator);
    static const bdNChar8 * skipString(const bdNChar8 * start);
    static const bdNChar8 * skipArray(const bdNChar8 * start, bdUInt32 * count);
    static const bdNChar8 * skipObject(const bdNChar8 * start, bdUInt32 * count);
    static const bdNChar8 * skipLiteral(const bdNChar8 * start);
    static const bdNChar8 * skipNumber(const bdNChar8 * start, bdBool * isFloatingPoint);
    static const bdNChar8 * skipValue(const bdNChar8 * start, bdBool logErrors);
};
static_assert(sizeof(bdJSONUtility) == 0x1, "bdJSONUtility");

// line 126
inline const bdNChar8 * bdJSONUtility::skipWhitespace(const bdNChar8 * ptr)
{
    if (ptr == NULL)
    {
        return NULL;
    }
    while (*ptr == bdJSON::BD_JSON_SPACE_CHAR || *ptr == bdJSON::BD_JSON_TAB_CHAR
           || *ptr == bdJSON::BD_JSON_CR_CHAR || *ptr == bdJSON::BD_JSON_LF_CHAR)
    {
        ++ptr;
    }
    return ptr;
}
