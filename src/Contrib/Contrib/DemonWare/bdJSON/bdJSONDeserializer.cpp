#include "Contrib/Contrib/DemonWare/bdJSON/bdJSONDeserializer.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "Contrib/Contrib/DemonWare/bdJSON/bdJSONUtility.h"
#include <errno.h>
bdJSONDeserializer::bdJSONDeserializer()
    : m_type(BD_JSON_NULL)
    , m_ptr(0)
    , m_end(0)
    , m_isFloatingPoint(false)
    , m_parsed(false)
    , m_count(0)
{
}


bdJSONDeserializer::~bdJSONDeserializer()
{
    m_ptr = 0;
    m_end = 0;
}
// Log text and format strings are invented: the release bdLog macros drop them.



#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformString/bdPlatformString.inl"
static const bdNChar8 *const bdJSONTypeString[6] = {"BD_JSON_NULL", "BD_JSON_STRING", "BD_JSON_NUMBER", "BD_JSON_BOOLEAN", "BD_JSON_ARRAY", "BD_JSON_OBJECT"};  // DWARF bdJSONValue.h:74 (the header lacks it)
// DWARF bdPlatformString-unix.inl:45 and :92 (not yet in the header)
inline bdInt32 bdStrtoi32(const bdNChar8 *const s, bdNChar8 ** endptr, const bdUInt radix)
{
    long result = strtol(s, endptr, radix);
    if (result > 0x7fffffffL) { errno = ERANGE; result = 0x7fffffffL; }
    else if (result < -0x80000000L) { errno = ERANGE; result = -0x80000000L; }
    return static_cast<bdInt32>(result);
}
inline bdUInt64 bdStrtoui64(const bdNChar8 *const s, bdNChar8 ** endptr, const bdUInt32 radix)
{
    return strtoull(s, endptr, radix);
}
bdBool bdJSONDeserializer::parse(const bdNChar8 * buffer)
{
    return m_parsed = parse(buffer, *this);
}




































bdBool bdJSONDeserializer::getUInt16(const bdNChar8 *const key,
                                     bdUInt16 & value) const
{

    bdJSONDeserializer field;
    bdBool ok = getFieldByKey(key, field, true) && field.getUInt16(value);
    return ok;
}
bdBool bdJSONDeserializer::getInt32(const bdNChar8 *const key,
                                    bdInt32 & value) const
{

    bdJSONDeserializer field;
    bdBool ok = getFieldByKey(key, field, true) && field.getInt32(value);
    return ok;
}
















bdBool bdJSONDeserializer::getUInt64(const bdNChar8 *const key,
                                     bdUInt64 & value) const
{

    bdJSONDeserializer field;
    bdBool ok = getFieldByKey(key, field, true) && field.getUInt64(value);
    return ok;
}
















bdBool bdJSONDeserializer::getString(const bdNChar8 *const key,
                                     bdNChar8 *const value,
                                     const bdUInt32 size) const
{
    bdJSONDeserializer field;
    bdBool ok = getFieldByKey(key, field, true) && field.getString(value, size);
    return ok;
}






















































































































































template <typename T>
bdBool bdJSONDeserializer::getNumeric(T & value) const
{
    bdBool ok = false;
    if (m_type == BD_JSON_STRING || m_type == BD_JSON_NUMBER)
    {
        errno = 0;  // 0xb6685c: __errno_location called afresh at every errno use

        const bdNChar8 * ptr = (m_type == BD_JSON_STRING) ? m_ptr + 1 : m_ptr;
        bdNChar8 * endptr = NULL;
        T readValue = strToNumber<T>(ptr, &endptr);
        if (checkStringToNumericValueError(ptr, endptr, getTypename<T>()))
        {
            value = readValue; ok = true;
        }
    }
    else


        bdLogError("bdJSON/deserializer", "Cannot get numeric from type %s", bdJSONTypeString[m_type]);
    return ok;
}

















bdBool bdJSONDeserializer::getUInt16(bdUInt16 & value) const
{

    return getNumeric<bdUInt16>(value);
}
bdBool bdJSONDeserializer::getInt32(bdInt32 & value) const
{

    return getNumeric<bdInt32>(value);
}










bdBool bdJSONDeserializer::getUInt64(bdUInt64 & value) const
{

    return getNumeric<bdUInt64>(value);
}





































bdBool bdJSONDeserializer::getString(bdNChar8 *const value, const bdUInt32 size) const
{

    bdBool ok = false;
    if (m_type == BD_JSON_STRING)
    {

        const bdUInt32 strLength = bdJSONUtility::skipString(m_ptr) - m_ptr;
        const bdInt32 sizeNeeded = bdJSONUtility::deescapeString(m_ptr + 1, strLength - bdJSON::BD_JSON_STRING_OVERHEAD_SIZE, NULL, 0);
        if (sizeNeeded == bdJSONUtility::BD_JSON_ESCAPE_ERROR)
        {

            bdLogError("bdJSON/deserializer", "Failed to deescape string");
        }
        else if (sizeNeeded >= static_cast<bdInt32>(size))  // INFERENCE: signed compare (b.ge at 0xb666b8)
        {
            bdLogError("bdJSON/deserializer", "Buffer too small: %u < %d", size, sizeNeeded);
        }
        else
        {
            bdJSONUtility::deescapeString(m_ptr + 1, strLength - bdJSON::BD_JSON_STRING_OVERHEAD_SIZE, value, size);
            ok = true;
        }
    }
    else
    {
        bdLogError("bdJSON/deserializer", "Not a string: %s", bdJSONTypeString[m_type]);
    }
    return ok;
}











































































bdBool bdJSONDeserializer::isObject() const
{

    return m_type == BD_JSON_OBJECT;
}







bdBool bdJSONDeserializer::getFieldByKey(const bdNChar8 *const key,
                                         bdJSONDeserializer & value,
                                         bdBool logErrors) const
{
    bdBool ok = true;  // INFERENCE: the original reaches the loop exits with w0=1 (orr w0,wzr,#1 at 0xb66428)
    if (m_type == BD_JSON_OBJECT && m_ptr != NULL)
    {

        bdNChar8 escapedKey[256];
        escapedKey[0] = bdJSON::BD_JSON_QUOTATION_CHAR;
        bdInt32 escapedKeyLen = bdJSONUtility::escapeString(key, &escapedKey[1], sizeof(escapedKey) - 3);


        if (escapedKeyLen != bdJSONUtility::BD_JSON_ESCAPE_ERROR)
        {

            if (static_cast<bdUWord>(escapedKeyLen + 2) < sizeof(escapedKey)  /* INFERENCE: 64-bit len+2 reused as strncmp length and index (0xb66398-0xb663a0) */)
            {
                escapedKey[escapedKeyLen + 1] = bdJSON::BD_JSON_QUOTATION_CHAR;
                escapedKey[escapedKeyLen + 2] = bdJSON::BD_JSON_NULL_CHAR;


                const bdNChar8 * ptr = m_ptr + 1;
                for (;;)
                {
                    ptr = bdJSONUtility::skipWhitespace(ptr);
                    if (!(ptr < m_end && *ptr != bdJSON::BD_JSON_NULL_CHAR && *ptr != bdJSON::BD_JSON_END_OBJECT)) break;

                    if (bdStrncmp(ptr, escapedKey, escapedKeyLen + 2) == 0)
                    {
                        ptr = bdJSONUtility::skipWhitespace(bdJSONUtility::skipString(ptr));
                        ptr = bdJSONUtility::skipSeparator(ptr, bdJSON::BD_JSON_NAME_SEPARATOR);
                        ok = parse(ptr, value);
                        break;
                    }
                    else
                    {
                        ptr = bdJSONUtility::skipString(ptr);

                        ptr = bdJSONUtility::skipSeparator(ptr, bdJSON::BD_JSON_NAME_SEPARATOR);
                        ptr = bdJSONUtility::skipValue(ptr, logErrors);
                        ptr = bdJSONUtility::skipSeparator(ptr, bdJSON::BD_JSON_VALUE_SEPARATOR);
                    }
                }

                if (ptr >= m_end || *ptr == bdJSON::BD_JSON_END_OBJECT || *ptr == bdJSON::BD_JSON_NULL_CHAR)
                {
                    if (logErrors) { bdLogError("bdJSON/deserializer", "Key not found: %s", key); } ok = false;
                }
            }
            else
            {
                bdLogError("bdJSON/deserializer", "Key too long: %d > %d", escapedKeyLen + 3, sizeof(escapedKey));
                ok = false;
            }
        }
        else
        {
            bdLogError("bdJSON/deserializer", "Cannot escape key: %s", key);
            ok = false;
        }
    }
    else
    {
        bdLogError("bdJSON/deserializer", "Not an object: %s", key);
        ok = false;
    }
    return ok;
}






































bdBool bdJSONDeserializer::hasKey(const bdNChar8 * key) const
{
    bdJSONDeserializer field;
    return getFieldByKey(key, field, false);
}












































































































inline bdBool bdJSONDeserializer::checkStringToNumericValueError(const bdNChar8 * ptr, bdNChar8 * endptr, const bdNChar8 * typeName) const  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    bdBool result = true;
    if (endptr == ptr)
    {
        bdLogError("bdJSON/deserializer", "Invalid %s: %s", typeName, ptr);
        result = false;
    }
    else if (errno == ERANGE)
    {
        errno = 0;
        bdLogError("bdJSON/deserializer", "%s out of range: %s", typeName, ptr);
        result = false;
    }
    return result;
}



bdBool bdJSONDeserializer::parse(const bdNChar8 * pos,
                                 bdJSONDeserializer & value)
{
    bdBool ok = true;  // INFERENCE: ok starts true, failures clear it; the original returns the success tail with w0=1 and the failure tail separately
    if (pos != NULL)
    {
        const bdNChar8 * ptr = bdJSONUtility::skipWhitespace(pos);
        switch (*ptr)
        {
        case bdJSON::BD_JSON_BEGIN_OBJECT:
            value.m_type = BD_JSON_OBJECT;
            value.m_ptr = ptr;
            value.m_end = bdJSONUtility::skipObject(ptr, &value.m_count);
            value.m_isFloatingPoint = false;
            break;
        case bdJSON::BD_JSON_BEGIN_ARRAY:
            value.m_type = BD_JSON_ARRAY;
            value.m_ptr = ptr;
            value.m_end = bdJSONUtility::skipArray(ptr, &value.m_count);
            value.m_isFloatingPoint = false;
            break;
        case bdJSON::BD_JSON_QUOTATION_CHAR:
            value.m_type = BD_JSON_STRING;
            value.m_ptr = ptr;
            value.m_end = bdJSONUtility::skipString(ptr);
            value.m_isFloatingPoint = false;
            value.m_count = 1;
            break;
        case '-':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            value.m_type = BD_JSON_NUMBER;
            value.m_ptr = ptr;
            value.m_end = bdJSONUtility::skipNumber(ptr, &value.m_isFloatingPoint);
            value.m_count = 1;
            break;
        case 't':
        case 'f':
        case 'n':
            if (bdMemcmp(ptr, bdJSON::BD_JSON_TRUE, bdJSON::BD_JSON_TRUE_SIZE) == 0
                || bdMemcmp(ptr, bdJSON::BD_JSON_FALSE, bdJSON::BD_JSON_FALSE_SIZE) == 0)
            {
                value.m_type = BD_JSON_BOOLEAN;
                value.m_ptr = ptr;
                value.m_end = bdJSONUtility::skipLiteral(ptr);
                value.m_isFloatingPoint = false;
                value.m_count = 1;
            }
            else if (bdMemcmp(ptr, bdJSON::BD_JSON_NULLSTR, bdJSON::BD_JSON_NULLSTR_SIZE) == 0)
            {
                value.m_type = BD_JSON_NULL;
                value.m_ptr = ptr;
                value.m_end = bdJSONUtility::skipLiteral(ptr);
                value.m_isFloatingPoint = false;
                value.m_count = 1;
            }
            else
            {
                bdLogError("bdJSON/deserializer", "Invalid literal: %s", ptr);
                ok = false;
            }
            break;
        default:
            bdLogError("bdJSON/deserializer", "Invalid value: %c", *ptr);  // INFERENCE: *ptr is the log argument (the byte is already in w0 at the call)
            ok = false;
            break;
        }
    }
    else
    {
        bdLogError("bdJSON/deserializer", "Null pointer");
        ok = false;
    }
    if (ok)
    {
        value.m_parsed = true;
    }
    else
    {
        value.m_type = BD_JSON_NULL;  // INFERENCE: reset of type/flags/ptr/end (not count) from stores at 0xb661f0-0xb661f8; statement order picked so the compiler keeps the stores unsunk
        value.m_isFloatingPoint = false;
        value.m_parsed = false;
        value.m_ptr = NULL;
        value.m_end = NULL;
    }
    return ok;
}



















































template <> bdUInt16 bdJSONDeserializer::strToNumber<bdUInt16>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const
{


    bdUInt32 readValue = bdStrtoui32(ptr, endptr, 10);
    if (readValue > 0xFFFF)
    {

        errno = ERANGE; readValue = 0xFFFF;
    }
    return static_cast<bdUInt16>(readValue);
}
template <> const bdNChar8 * bdJSONDeserializer::getTypename<bdUInt16>() const
{
    return "bdUInt16";
}
template <> bdInt32 bdJSONDeserializer::strToNumber<bdInt32>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const
{

    return bdStrtoi32(ptr, endptr, 10);
}
template <> const bdNChar8 * bdJSONDeserializer::getTypename<bdInt32>() const
{
    return "bdInt32";
}


















template <> bdUInt64 bdJSONDeserializer::strToNumber<bdUInt64>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const
{

    return bdStrtoui64(ptr, endptr, 10);
}
template <> const bdNChar8 * bdJSONDeserializer::getTypename<bdUInt64>() const
{
    return "bdUInt64";
}
