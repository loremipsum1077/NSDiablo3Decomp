// Original file: Contrib/Contrib/DemonWare/bdJSON/bdJSONSerializer.cpp

#include "Contrib/Contrib/DemonWare/bdJSON/bdJSONSerializer.h"
#include "Contrib/Contrib/DemonWare/bdJSON/bdJSONUtility.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformString/bdPlatformString.inl"

// Blank lines keep the original line numbers (DWARF line table).
// Log channel and format strings are invented: the release bdLog macros drop them.
// Functions the linker stripped from the original (default ctor, setBuffer, length,
// the array writers, the other writeNumeric/getFormat instantiations) are not written.







static const bdUInt BD_JSON_SEPARATOR_SIZE = 2;



static const bdUInt BD_JSON_OBJECT_MISC_SIZE = 3;













bdJSONSerializer::bdJSONSerializer(bdNChar8 *buffer,
                                   bdUInt32 bufferSize)
    : m_buffer(buffer)
    , m_bufferSize(bufferSize)
    , m_bufferIndex(0)
    , m_stackDepth(0)
{
    m_stack[0].m_type = BD_JSON_NULL;
}

bdJSONSerializer::~bdJSONSerializer()
{
    m_buffer = 0;
}











inline bdBool bdJSONSerializer::checkBuffer(bdUInt32 size) const
{
    bdBool result = true;
    if ((m_buffer == NULL) || ((m_bufferIndex + size) > m_bufferSize))
    {
        bdLogWarn("bdJSON/serializer", "Buffer too small: need %u bytes, %u left", size, m_bufferSize - m_bufferIndex);
        result = false;
    }
    return result;
}


bdBool bdJSONSerializer::validate() const
{
    bdBool ok = (m_stackDepth == 0) && (m_bufferIndex != 0);
    return ok;
}







bdBool bdJSONSerializer::checkObject() const
{
    bdBool result = false;
    if ((m_stackDepth > 0) &&
        (m_stack[m_stackDepth - 1].m_type == BD_JSON_OBJECT))
    {
        result = true;
    }
    else
    {
        bdLogWarn("bdJSON/serializer", "Not inside an object");
    }
    return result;
}

bdBool bdJSONSerializer::writeBeginObject()
{
    const bdBool isArray = checkArray();
    const bdUInt32 valueSize = bdJSON::BD_JSON_BEGIN_OBJECT_SIZE;
    const bdUInt32 separatorSize = getSeparatorSize(isArray);
    const bdUInt32 outSize = valueSize + separatorSize;
    bdBool ok = checkBuffer(outSize);
    if (ok)
    {
        if (m_stackDepth < 32)  // m_stack has 32 entries; no named limit is in the DWARF
        {
            if (isArray)
            {
                writeArraySeparator();
            }
            bdJSONTypeFirst &current = m_stack[m_stackDepth];
            current.m_type = BD_JSON_OBJECT;
            current.m_first = true;
            ++m_stackDepth;

            m_buffer[m_bufferIndex++] = bdJSON::BD_JSON_BEGIN_OBJECT;
            m_buffer[m_bufferIndex] = bdJSON::BD_JSON_NULL_CHAR;
        }
        else
        {
            bdLogWarn("bdJSON/serializer", "Maximum nesting depth %u reached", 32);
            ok = false;
        }
    }

    return ok;
}

bdBool bdJSONSerializer::writeEndObject()
{
    bdBool ok = checkBuffer(bdJSON::BD_JSON_END_OBJECT_SIZE);
    if (ok)
    {
        if (checkObject())
        {
            --m_stackDepth;
            m_buffer[m_bufferIndex++] = bdJSON::BD_JSON_END_OBJECT;
            m_buffer[m_bufferIndex] = bdJSON::BD_JSON_NULL_CHAR;
        }
        else
        {
            bdLogWarn("bdJSON/serializer", "End of object without a matching begin");
            ok = false;
        }
    }
    return ok;
}

void bdJSONSerializer::writeObjectKey(const bdNChar8 *key)
{




    writeArraySeparator();


    m_buffer[m_bufferIndex++] = bdJSON::BD_JSON_QUOTATION_CHAR;
    m_bufferIndex += bdJSONUtility::escapeString(key, m_buffer + m_bufferIndex, m_bufferSize - m_bufferIndex);
    m_buffer[m_bufferIndex++] = bdJSON::BD_JSON_QUOTATION_CHAR;


    writeKeySeparator();
}

bdBool bdJSONSerializer::writeObjectKeyValue(const bdNChar8 *key,
                                             const bdNChar8 *value,
                                             bdBool asString)
{
    writeObjectKey(key);
    const bdNChar8 *format = asString ? "\"%s\"" : "%s";
    const bdUInt32 bufferRemaining = m_bufferSize - m_bufferIndex;
    const bdInt bytesNeeded = bdSnprintf(m_buffer + m_bufferIndex, bufferRemaining, format, value);



    bdBool ok = (bytesNeeded >= 0) && (bytesNeeded < static_cast<bdInt>(bufferRemaining));
    if (ok)
    {
        m_bufferIndex += bytesNeeded;
    }
    return ok;
}












bdBool bdJSONSerializer::checkArray() const
{
    bdBool result = false;
    if ((m_stackDepth > 0) &&
        (m_stack[m_stackDepth - 1].m_type == BD_JSON_ARRAY))
    {
        result = true;
    }
    return result;
}










































































inline bdUInt32 bdJSONSerializer::getSeparatorSize(bdBool inArrayOrObject)
{
    bdUInt32 size = 0;
    if (inArrayOrObject && !m_stack[m_stackDepth - 1].m_first)
    {
        size = BD_JSON_SEPARATOR_SIZE;
    }
    return size;
}

inline void bdJSONSerializer::writeKeySeparator()
{

    m_buffer[m_bufferIndex++] = bdJSON::BD_JSON_NAME_SEPARATOR;

    m_buffer[m_bufferIndex++] = bdJSON::BD_JSON_SPACE_CHAR;
}


inline void bdJSONSerializer::writeArraySeparator()
{
    if (m_stackDepth > 0)
    {


        bdBool &first = m_stack[m_stackDepth - 1].m_first;
        if (!first)
        {
            m_buffer[m_bufferIndex++] = bdJSON::BD_JSON_VALUE_SEPARATOR;

            m_buffer[m_bufferIndex++] = bdJSON::BD_JSON_SPACE_CHAR;
        }

        first = false;
    }
}



























bdBool bdJSONSerializer::writeBoolean(const bdNChar8 *key,
                                      const bdBool value)
{
    bdBool ok = false;
    if (checkObject())
    {
        const bdInt32 keySize = bdJSONUtility::escapeString(key, NULL, 0) + bdJSON::BD_JSON_STRING_OVERHEAD_SIZE;
        const bdUInt32 valueSize = value ? bdJSON::BD_JSON_TRUE_SIZE : bdJSON::BD_JSON_FALSE_SIZE;
        const bdUInt32 separatorSize = getSeparatorSize(true);

        if (keySize > bdJSON::BD_JSON_ESCAPED_ERROR_SIZE)
        {
            const bdUInt32 outSize = keySize + valueSize + separatorSize + BD_JSON_OBJECT_MISC_SIZE;
            if (checkBuffer(outSize))
            {
                ok = writeObjectKeyValue(key, value ? bdJSON::BD_JSON_TRUE : bdJSON::BD_JSON_FALSE, false);
            }
        }
    }
    return ok;
}

























bdBool bdJSONSerializer::writeString(const bdNChar8 *key,
                                     const bdNChar8 *value)
{
    bdBool ok = false;
    if (checkObject())
    {
        const bdNChar8 *safeValue = value ? value : "";
        const bdInt32 keySize = bdJSONUtility::escapeString(key, NULL, 0) + bdJSON::BD_JSON_STRING_OVERHEAD_SIZE;
        const bdInt32 valueSize = bdJSONUtility::escapeString(safeValue, NULL, 0) + bdJSON::BD_JSON_STRING_OVERHEAD_SIZE;
        const bdUInt32 separatorSize = getSeparatorSize(true);

        if ((keySize > bdJSON::BD_JSON_ESCAPED_ERROR_SIZE) && (valueSize > bdJSON::BD_JSON_ESCAPED_ERROR_SIZE))
        {
            const bdInt outSize = keySize + valueSize + separatorSize + BD_JSON_OBJECT_MISC_SIZE;
            if (checkBuffer(outSize))
            {
                writeObjectKey(key);
                m_buffer[m_bufferIndex++] = bdJSON::BD_JSON_QUOTATION_CHAR;
                m_bufferIndex += bdJSONUtility::escapeString(safeValue, m_buffer + m_bufferIndex, m_bufferSize - m_bufferIndex);
                m_buffer[m_bufferIndex++] = bdJSON::BD_JSON_QUOTATION_CHAR;
                m_buffer[m_bufferIndex] = bdJSON::BD_JSON_NULL_CHAR;
                ok = true;
            }
        }
    }
    return ok;
}




































































template <typename T>
bdBool bdJSONSerializer::writeNumeric(const bdNChar8 *key,
                                      T value,
                                      bdBool asString)
{
    bdBool ok = false;
    if (checkObject())
    {
        bdNChar8 stringRepr[32];
        const bdNChar8 *format = getFormat<T>();
        const bdInt32 keySize = bdJSONUtility::escapeString(key, NULL, 0) + bdJSON::BD_JSON_STRING_OVERHEAD_SIZE;
        const bdInt valueSize = bdSnprintf(stringRepr, sizeof(stringRepr), format, value) + (asString ? bdJSON::BD_JSON_STRING_OVERHEAD_SIZE : 0);
        const bdUInt32 separatorSize = getSeparatorSize(true);

        if (keySize > bdJSON::BD_JSON_ESCAPED_ERROR_SIZE)
        {
            const bdInt outSize = keySize + valueSize + separatorSize + BD_JSON_OBJECT_MISC_SIZE;
            if (checkBuffer(outSize))
            {
                ok = writeObjectKeyValue(key, stringRepr, asString);
            }
        }
    }
    return ok;
}




























































































bdBool bdJSONSerializer::writeUInt64(const bdNChar8 *key,
                                     const bdUInt64 value,
                                     bdBool asString)
{
    return writeNumeric<const bdUInt64>(key, value, asString);
}


































































































































template <>
const bdNChar8 *bdJSONSerializer::getFormat<const bdUInt64>() const
{
    return "%llu";
}
