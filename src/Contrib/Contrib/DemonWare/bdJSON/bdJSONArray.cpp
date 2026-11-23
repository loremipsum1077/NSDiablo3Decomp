// Original file: Contrib/Contrib/DemonWare/bdJSON/bdJSONArray.cpp

#include "bdJSONArray.h"

#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "bdJSONUtility.h"

// Blank lines keep the original line numbers (DWARF line table).

// Log channel and format strings are invented: the release bdLog macros drop them.







bdJSONArray::bdJSONArray(const bdJSONArray & other)
{
    m_array = other.m_array;
}





































































bdBool bdJSONArray::parse(const bdNChar8 * jsonPtr,
                          const bdNChar8 ** outPtr)
{
    bdBool success = false;
    const bdNChar8 * ptr = bdJSONUtility::skipWhitespace(jsonPtr);
    if (*ptr == bdJSON::BD_JSON_BEGIN_ARRAY)
    {
        ++ptr;
        bdBool parsing = true;
        while (parsing)
        {
            switch (*ptr)
            {
            case bdJSON::BD_JSON_SPACE_CHAR:
            case bdJSON::BD_JSON_TAB_CHAR:
            case bdJSON::BD_JSON_CR_CHAR:
            case bdJSON::BD_JSON_LF_CHAR:
            case bdJSON::BD_JSON_VALUE_SEPARATOR:
                ++ptr;
                break;
            case bdJSON::BD_JSON_END_ARRAY:
                ++ptr;
                success = true;
                parsing = false;
                break;
            case bdJSON::BD_JSON_BEGIN_OBJECT:
            case bdJSON::BD_JSON_BEGIN_ARRAY:
            {
                bdJSONValue value;
                if (value.parse(ptr, &ptr)) pushBack(value); else parsing = false;
            }
            break;
            case bdJSON::BD_JSON_QUOTATION_CHAR:
            {
                bdJSONValue value;
                if (value.parse(ptr, &ptr)) pushBack(value); else parsing = false;
            }
            break;
            case '-': case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7': case '8': case '9':
            {
                bdJSONValue value;
                if (value.parse(ptr, &ptr)) pushBack(value); else parsing = false;
            }
            break;
            case 't': case 'f': case 'n':
            {
                bdJSONValue value;
                if (value.parse(ptr, &ptr)) pushBack(value); else parsing = false;
            }
            break;
            default:
                parsing = false;
                break;
            }
        }
    }





    if (success)
    {
        if (outPtr != NULL)
        {
            *outPtr = ptr;
        }
    }
    else
    {
        bdLogError("bdJSON/array", "Failed to parse array: %s", jsonPtr);
        m_array.clear();
    }
    return success;
}
