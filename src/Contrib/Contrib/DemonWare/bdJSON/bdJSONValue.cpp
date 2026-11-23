// Original file: Contrib/Contrib/DemonWare/bdJSON/bdJSONValue.cpp

#include "bdJSONValue.h"

#include "../bdCore/bdMemory/bdMemory.inl"
#include "../bdCore/bdString/bdString.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdPlatform/bdPlatformString/bdPlatformString.inl"
#include "bdJSONArray.h"
#include "bdJSONObject.h"
#include "bdJSONUtility.h"

// Blank lines keep the original line numbers (DWARF line table).
// Log channel and format strings are invented: the release bdLog macros drop them.






bdJSONValue::bdJSONValue()
    : m_type(BD_JSON_NULL),
      m_string(NULL)
{
}

bdJSONValue::bdJSONValue(bdJSONType type)
    : m_type(type)
{
    switch (type)
    {
    case BD_JSON_STRING:
    case BD_JSON_NUMBER:
    case BD_JSON_BOOLEAN:

        m_string = new bdString();
        break;
    case BD_JSON_ARRAY:
        m_array = new bdJSONArray();
        break;
    case BD_JSON_OBJECT:
        m_object = new bdJSONObject();
        break;
    default:
        break;
    }










}

bdJSONValue::~bdJSONValue()
{











    cleanup();
}


void bdJSONValue::cleanup()
{
    switch (m_type)
    {
    case BD_JSON_STRING:
    case BD_JSON_NUMBER:
    case BD_JSON_BOOLEAN:
        delete m_string;
        m_string = NULL;
        break;
    case BD_JSON_ARRAY:
        delete m_array;
        m_array = NULL;
        break;
    case BD_JSON_OBJECT:
        delete m_object;
        m_object = NULL;
        break;
    default:
        break;
    }
    m_type = BD_JSON_NULL;
}







bdJSONValue & bdJSONValue::operator=(const bdUInt64 number)
{
    cleanup();
    assignNumber(number);
    return *this;
}

inline bdBool bdJSONValue::assignNumber(const bdUInt64 number)
{
    bdBool result = false;
    bdNChar8 buffer[32] = {0};
    if (bdSnprintf(buffer, sizeof(buffer), "%llu", number) > 0)
    {
        m_type = BD_JSON_NUMBER;
        m_string = new bdString(buffer);
        result = true;
    }
    return result;
}



































































































bdJSONValue::bdJSONValue(const bdJSONObject & object)
{
    m_type = BD_JSON_OBJECT;
    m_object = new bdJSONObject(object);
}









bdJSONValue::bdJSONValue(const bdJSONValue & other)
{
    switch (other.m_type)
    {
    case BD_JSON_NULL:
        break;
    case BD_JSON_ARRAY:
        m_array = new bdJSONArray(*other.getArray());
        break;
    case BD_JSON_OBJECT:

        m_object = new bdJSONObject(*other.getObject());
        break;
    default:

        m_string = new bdString(other.m_string->getBuffer());
        break;
    }
    m_type = other.m_type;
}

bdJSONValue & bdJSONValue::operator=(const bdJSONValue & other)
{
    cleanup();
    switch (other.m_type)
    {
    case BD_JSON_NULL:
        break;
    case BD_JSON_ARRAY:
        m_array = new bdJSONArray(*other.getArray());
        break;
    case BD_JSON_OBJECT:

        m_object = new bdJSONObject(*other.getObject());
        break;
    default:

        m_string = new bdString(other.m_string->getBuffer());
        break;
    }
    m_type = other.m_type;
    return *this;
}


bdJSONObject * bdJSONValue::getObject() const
{
    bdJSONObject * result = NULL;
    if (m_type == BD_JSON_OBJECT && m_object != NULL)
    {
        result = m_object;
    }
    else
    {
        bdLogWarn("bdJSON/value", "Value is not an object, type is %u", m_type);
    }
    return result;
}


inline bdJSONArray * bdJSONValue::getArray() const
{
    bdJSONArray * result = NULL;
    if (m_type == BD_JSON_ARRAY && m_array != NULL)
    {
        result = m_array;
    }
    else
    {
        bdLogWarn("bdJSON/value", "Value is not an array, type is %u", m_type);
    }
    return result;
}



















































































































































































































bdBool bdJSONValue::parse(const bdNChar8 *const jsonStr,
                          const bdNChar8 ** outPtr)
{

    const bdBool success = parseValue(jsonStr, outPtr, *this);
    if (!success)
    {
        cleanup();
    }
    return success;
}

















bdBool bdJSONValue::parseValue(const bdNChar8 * jsonPtr, const bdNChar8 ** outPtr,
                               bdJSONValue & target)
{
    bdBool success = false;
    target.cleanup();
    if (jsonPtr == NULL)
    {
        bdLogError("bdJSON/value", "Null pointer passed to parseValue");
    }
    else
    {
        const bdNChar8 * ptr = bdJSONUtility::skipWhitespace(jsonPtr);
        switch (*ptr)
        {
        case bdJSON::BD_JSON_BEGIN_OBJECT:
            target.m_object = new bdJSONObject();
            target.m_type = BD_JSON_OBJECT;
            success = target.m_object->parse(ptr, &ptr);
            break;
        case bdJSON::BD_JSON_BEGIN_ARRAY:
            target.m_array = new bdJSONArray();
            target.m_type = BD_JSON_ARRAY;
            success = target.m_array->parse(ptr, &ptr);
            break;
        case bdJSON::BD_JSON_QUOTATION_CHAR:
            success = parseString(ptr, target);
            break;
        case '-':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            success = parseNumber(ptr, target);
            break;
        case 't':
        case 'f':
        case 'n':
            success = parseLiteral(ptr, &ptr, target);
            break;
        default:
            break;
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
            target.cleanup();
            bdLogError("bdJSON/value", "Failed to parse value: %s", jsonPtr);
        }
    }
    return success;
}



bdBool bdJSONValue::parseLiteral(const bdNChar8 * jsonPtr, const bdNChar8 ** outPtr,
                                 bdJSONValue & target)
{
    bdBool result = false;
    const bdNChar8 * ptr = bdJSONUtility::skipWhitespace(jsonPtr);

    target.cleanup();
    if (ptr != NULL)
    {
        if (!bdStrncmp(ptr, bdJSON::BD_JSON_TRUE, bdJSON::BD_JSON_TRUE_SIZE))
        {
            target.m_type = BD_JSON_BOOLEAN;
            target.m_string = new bdString(bdJSON::BD_JSON_TRUE);
            if (outPtr != NULL)
            {
                *outPtr = ptr + bdJSON::BD_JSON_TRUE_SIZE;
            }
            result = true;
        }
        else if (!bdStrncmp(ptr, bdJSON::BD_JSON_FALSE, bdJSON::BD_JSON_FALSE_SIZE))
        {
            target.m_type = BD_JSON_BOOLEAN;
            target.m_string = new bdString(bdJSON::BD_JSON_FALSE);
            if (outPtr != NULL)
            {
                *outPtr = ptr + bdJSON::BD_JSON_FALSE_SIZE;
            }
            result = true;
        }
        else if (!bdStrncmp(ptr, bdJSON::BD_JSON_NULLSTR, bdJSON::BD_JSON_NULLSTR_SIZE))
        {
            target.m_type = BD_JSON_NULL;
            if (outPtr != NULL)
            {
                *outPtr = ptr + bdJSON::BD_JSON_NULLSTR_SIZE;
            }
            result = true;
        }
        else
        {
            bdLogError("bdJSON/value", "Failed to parse literal: %s", jsonPtr);
        }
    }
    else
    {
        bdLogError("bdJSON/value", "Null pointer passed to parseLiteral");
    }
    return result;
}


inline bdBool bdJSONValue::parseNumber(const bdNChar8 *& json, bdJSONValue & target)
{

    const bdNChar8 * start = bdJSONUtility::skipWhitespace(json);
    const bdNChar8 * ptr = start;
    while (*ptr == '0' || *ptr == '1' || *ptr == '2' || *ptr == '3' || *ptr == '4' || *ptr == '5' || *ptr == '6'
           || *ptr == '7' || *ptr == '8' || *ptr == '9' || *ptr == '-' || *ptr == '+' || *ptr == '.' || *ptr == 'e'
           || *ptr == 'E')
    {
        ++ptr;
    }



    target.cleanup();
    target.m_string = new bdString(start, ptr - start);
    target.m_type = BD_JSON_NUMBER;
    json = ptr;
    return true;
}


bdBool bdJSONValue::parseString(const bdNChar8 *& json, bdJSONValue & target)
{
    bdBool result = false;

    const bdNChar8 * start = bdJSONUtility::skipWhitespace(json);
    if (*start == bdJSON::BD_JSON_QUOTATION_CHAR)
    {
        ++start;

        const bdNChar8 * ptr = start;
        while (*ptr != bdJSON::BD_JSON_NULL_CHAR && *ptr != bdJSON::BD_JSON_QUOTATION_CHAR)
        {

            if (*ptr == bdJSON::BD_JSON_ESCAPE_CHAR)
            {
                ++ptr;
            }
            ++ptr;
        }

        if (*ptr == bdJSON::BD_JSON_QUOTATION_CHAR)
        {
            target.cleanup();
            bdString dequoted(start, ptr - start);
            const bdUInt32 len = ptr - start;
            bdNChar8 * deescaped = bdAllocate<bdNChar8>(len + 1);
            const bdInt32 deescapedSz = bdJSONUtility::deescapeString(dequoted.getBuffer(), deescaped, len + 1);
            if (deescapedSz != bdJSONUtility::BD_JSON_ESCAPE_ERROR)
            {
                target.m_string = new bdString(deescaped);
                target.m_type = BD_JSON_STRING;
                bdDeallocate(deescaped);
                result = true;

                json = ptr + 1;
            }
        }
    }

    return result;
}
