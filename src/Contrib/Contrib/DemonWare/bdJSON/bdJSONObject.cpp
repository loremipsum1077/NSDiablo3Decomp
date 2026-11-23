// Original file: Contrib/Contrib/DemonWare/bdJSON/bdJSONObject.cpp

#include "bdJSONObject.h"

#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "bdJSONUtility.h"

// Blank lines keep the original line numbers (DWARF line table).
// Log channel and format strings are invented: the release bdLog macros drop them.









bdJSONObject::bdJSONObject(const bdJSONObject & other) : m_map(4, 0.75f)
{

    m_map.clear();
    for (jsonObjectIterator it = other.m_map.getIterator(); it != NULL; other.m_map.next(it))
    {

        m_map.put(other.m_map.getKey(it), other.m_map.getValue(it));
    }


}

bdJSONObject::~bdJSONObject()
{
    m_map.clear();



}































bdJSONValue & bdJSONObject::operator[](const bdNChar8 * key)
{
    if (!m_map.containsKey(bdString(key)))
    {
        m_map.put(bdString(key), bdJSONValue());
    }
    jsonObjectIterator it = m_map.getIterator(bdString(key));
    bdJSONValue & result = m_map.getValue(it);


    return result;
}



























































































































bdBool bdJSONObject::parse(const bdNChar8 * jsonPtr,
                           const bdNChar8 ** outPtr)
{
    bdBool success = false;
    const bdNChar8 * ptr = bdJSONUtility::skipWhitespace(jsonPtr);
    if (*ptr == bdJSON::BD_JSON_BEGIN_OBJECT)
    {
        ptr = bdJSONUtility::skipWhitespace(++ptr);
        bdBool parsing = true;
        while (parsing)
        {
            switch (*ptr)
            {
            case bdJSON::BD_JSON_QUOTATION_CHAR:
            {
                ++ptr;

                bdNChar8 key[256] = {0};
                bdNChar8 * keyPtr = key;
                bdBool keyReadSuccess = false;




                for (;;)
                {
                    if (*ptr == bdJSON::BD_JSON_ESCAPE_CHAR)
                    {

                        *keyPtr++ = *ptr++;
                    }
                    else if (*ptr == bdJSON::BD_JSON_NULL_CHAR || *ptr == bdJSON::BD_JSON_QUOTATION_CHAR)
                        break;
                    *keyPtr++ = *ptr++;
                }

                *keyPtr = bdJSON::BD_JSON_NULL_CHAR;

                ptr = bdJSONUtility::skipSeparator(++ptr, bdJSON::BD_JSON_NAME_SEPARATOR);
                keyReadSuccess = true;


                if (keyReadSuccess)
                {
                    bdJSONValue value;
                    if (value.parse(ptr, &ptr))
                    {
                        bdNChar8 dequotedKey[256];
                        const bdInt deescapedSz = bdJSONUtility::deescapeString(key, dequotedKey, sizeof(dequotedKey));
                        if (deescapedSz != bdJSONUtility::BD_JSON_ESCAPE_ERROR)
                        {

                            m_map.put(bdString(dequotedKey), value);
                        }
                        else
                        {
                            parsing = false;
                        }
                    }
                    else
                    {
                        parsing = false;
                    }
                }
            }
            break;
            case bdJSON::BD_JSON_VALUE_SEPARATOR:
                ++ptr;
                break;
            case bdJSON::BD_JSON_SPACE_CHAR:
            case bdJSON::BD_JSON_TAB_CHAR:
            case bdJSON::BD_JSON_CR_CHAR:
            case bdJSON::BD_JSON_LF_CHAR:
                ptr = bdJSONUtility::skipWhitespace(ptr);
                break;

            case bdJSON::BD_JSON_END_OBJECT:
                ++ptr;
                success = true;
                parsing = false;
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
        bdLogError("bdJSON/object", "Failed to parse object: %s", jsonPtr);
    }
    return success;
}
