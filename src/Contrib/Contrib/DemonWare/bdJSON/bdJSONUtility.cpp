// Original file: Contrib/Contrib/DemonWare/bdJSON/bdJSONUtility.cpp

#include "bdJSONUtility.h"

#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../bdPlatform/bdPlatformString/bdPlatformString.inl"
#include <stdlib.h>

// Blank lines keep the original line numbers (DWARF line table).
// Log channel and format strings are invented: the release bdLog macros drop them.






const bdNChar8 * bdJSONUtility::skipSeparator(const bdNChar8 * start, const bdNChar8 separator)
{

    const bdNChar8 * ptr = skipWhitespace(start);
    if (*ptr == separator)
    {
        ++ptr;
    }
    return ptr;
}

const bdNChar8 * bdJSONUtility::skipString(const bdNChar8 * start)
{
    const bdNChar8 * ptr = skipWhitespace(start);
    if (*ptr == bdJSON::BD_JSON_QUOTATION_CHAR)
        ++ptr;
    for (;;)
    {
        if (*ptr == bdJSON::BD_JSON_ESCAPE_CHAR)
        {
            ++ptr;
        }
        else if (*ptr == bdJSON::BD_JSON_QUOTATION_CHAR)
        {
            ++ptr;
            break;
        }
        else if (*ptr == bdJSON::BD_JSON_NULL_CHAR)
            break;
        ++ptr;
    }
    return ptr;
}


const bdNChar8 * bdJSONUtility::skipArray(const bdNChar8 * start, bdUInt32 * count)
{
    const bdNChar8 * ptr = skipWhitespace(start);
    if (*ptr == bdJSON::BD_JSON_BEGIN_ARRAY)
    {
        ++ptr;
        if (count != NULL)
        {
            *count = 0;
        }
        while (*ptr != bdJSON::BD_JSON_NULL_CHAR)
        {
            switch (*ptr)
            {
            case bdJSON::BD_JSON_QUOTATION_CHAR:

                ptr = skipString(ptr);
                break;
            case bdJSON::BD_JSON_BEGIN_ARRAY:

                ptr = skipArray(ptr, NULL);
                break;
            case bdJSON::BD_JSON_BEGIN_OBJECT:

                ptr = skipObject(ptr, NULL);
                break;
            case bdJSON::BD_JSON_END_ARRAY:

                ++ptr;
                if (count != NULL && *(ptr - 2) != bdJSON::BD_JSON_BEGIN_ARRAY)
                {
                    (*count)++;
                }
                return ptr;
            default:

                ++ptr;
                if (count != NULL && *(ptr - 1) == bdJSON::BD_JSON_VALUE_SEPARATOR)
                {
                    (*count)++;
                }
                break;
            }
        }
    }
    return ptr;
}





const bdNChar8 * bdJSONUtility::skipObject(const bdNChar8 * start, bdUInt32 * count)
{
    const bdNChar8 * ptr = skipWhitespace(start);
    if (*ptr == bdJSON::BD_JSON_BEGIN_OBJECT)
    {
        ++ptr;
        if (count != NULL)
        {
            *count = 0;
        }
        bdBool done = false;
        while (!done)
        {
            switch (*ptr)
            {
            case bdJSON::BD_JSON_NULL_CHAR:
                done = true;
                break;
            case bdJSON::BD_JSON_QUOTATION_CHAR:
                ptr = skipString(ptr);
                break;
            case bdJSON::BD_JSON_BEGIN_ARRAY:
                ptr = skipArray(ptr, NULL);
                break;
            case bdJSON::BD_JSON_BEGIN_OBJECT:
                ptr = skipObject(ptr, NULL);
                break;
            case bdJSON::BD_JSON_END_OBJECT:
                ++ptr;
                done = true;
                break;
            case bdJSON::BD_JSON_NAME_SEPARATOR:
                if (count != NULL)
                {
                    (*count)++;
                }
                ++ptr;
                break;
            default:
                ++ptr;
                break;
            }
        }
    }
    return ptr;
}


const bdNChar8 * bdJSONUtility::skipLiteral(const bdNChar8 * start)
{
    const bdNChar8 * ptr = start;
    if (!bdMemcmp(ptr, "true", 4) || !bdMemcmp(ptr, "null", 4))
    {
        ptr += 4;
    }
    else if (!bdMemcmp(ptr, "false", 5))
    {
        ptr += 5;
    }
    else
    {
        bdLogWarn("bdJSON/utility", "Failed to skip literal: %s", ptr);
    }
    return ptr;
}

const bdNChar8 * bdJSONUtility::skipNumber(const bdNChar8 * start, bdBool * isFloatingPoint)
{
    const bdNChar8 * ptr = start;




    if (*ptr == '-')
    {
        ptr++;
    }



    if (*ptr == '0')
    {
        ptr++;
    }
    else if (*ptr >= '1' && *ptr <= '9')
    {
        ptr++;
        while (*ptr >= '0' && *ptr <= '9')
        {
            ptr++;
        }
    }
    if (*ptr == '.')
    {
        if (isFloatingPoint != NULL)
        {
            *isFloatingPoint = true;
        }
        ptr++;
        while (*ptr >= '0' && *ptr <= '9')
        {
            ptr++;
        }
    }
    else
    {
        if (isFloatingPoint != NULL)
        {
            *isFloatingPoint = false;
        }
    }
    if (*ptr == 'e' || *ptr == 'E')
    {
        ptr++;
        if (*ptr == '-' || *ptr == '+')
        {
            ptr++;
        }
        while (*ptr >= '0' && *ptr <= '9')
        {
            ptr++;
        }
    }
    return ptr;
}

const bdNChar8 * bdJSONUtility::skipValue(const bdNChar8 * start, bdBool logErrors)
{
    const bdNChar8 * ptr = skipWhitespace(start);
    switch (*ptr)
    {
    case bdJSON::BD_JSON_BEGIN_OBJECT:
        ptr = skipObject(ptr, NULL);
        break;
    case bdJSON::BD_JSON_BEGIN_ARRAY:
        ptr = skipArray(ptr, NULL);
        break;
    case bdJSON::BD_JSON_QUOTATION_CHAR:
        ptr = skipString(ptr);
        break;
    case '-':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        ptr = skipNumber(ptr, NULL);
        break;
    case 't':
    case 'f':
    case 'n':
        ptr = skipLiteral(ptr);
        break;
    default:
        if (logErrors)
        {
            bdLogError("bdJSON/utility", "Failed to skip value: %s", start);
        }
        break;
    }
    return ptr;
}
bdInt32 bdJSONUtility::escapeString(const bdNChar8 *const input, bdNChar8 *const outBuffer,
                                    const bdUInt32 outBufferSz)
{
    const bdNChar8 * ptr = input;
    bdInt32 resultSize = BD_JSON_ESCAPE_ERROR;
    bdUInt32 numEscapeChars = 0;
    const bdUInt32 length = bdStrlen(input);
    bdBool valid = true;



    for (bdUInt32 i = 0; i < length && valid; ++i)
    {
        if (ptr[i] == bdJSON::BD_JSON_QUOTATION_CHAR || ptr[i] == bdJSON::BD_JSON_ESCAPE_CHAR
            || ptr[i] == bdJSON::BD_JSON_BACK_CHAR || ptr[i] == bdJSON::BD_JSON_FF_CHAR
            || ptr[i] == bdJSON::BD_JSON_LF_CHAR || ptr[i] == bdJSON::BD_JSON_CR_CHAR
            || ptr[i] == bdJSON::BD_JSON_TAB_CHAR)
        {



            ++numEscapeChars;
        }
        else if (static_cast<bdUByte8>(ptr[i]) < 0x20)
        {


            numEscapeChars += bdJSON::BD_JSON_ESCAPED_UNICODE_SIZE - 1;
        }
        else if (static_cast<bdUByte8>(ptr[i]) >= 0x80)
        {
            const bdJSONCodePoint codePoint = getCodePointFromUTF8(&ptr[i]);
            if (codePoint.m_valid)
            {
                numEscapeChars -= codePoint.m_numRead - 1;
                numEscapeChars += (codePoint.m_codePoint < 0x10000 ? bdJSON::BD_JSON_ESCAPED_UNICODE_SIZE
                                                                   : bdJSON::BD_JSON_ESCAPED_SURROGATE_PAIR_SIZE) - 1;
                i += codePoint.m_numRead - 1;
            }
            else
            {
                bdLogError("bdJSON/utility", "Invalid UTF-8 sequence in string");
                valid = false;
            }
        }
    }

    if (valid)
    {


        resultSize = length + numEscapeChars;



        if (outBuffer != NULL && resultSize < outBufferSz)
        {
            bdInt32 bytesWritten = 0;
            if (numEscapeChars == 0)
            {

                bytesWritten = bdSnprintf(outBuffer, resultSize + 1, "%s", input);
            }
            else
            {
                bdNChar8 * outPtr = outBuffer;

                outBuffer[resultSize] = bdJSON::BD_JSON_NULL_CHAR;


                for (bdUInt32 i = 0; i < length; ++i)
                {
                    const bdUByte8 & chr = reinterpret_cast<const bdUByte8 &>(ptr[i]);
                    if (chr == bdJSON::BD_JSON_ESCAPE_CHAR || chr == bdJSON::BD_JSON_QUOTATION_CHAR)
                    {


                        *outPtr++ = bdJSON::BD_JSON_ESCAPE_CHAR;
                        *outPtr++ = chr;
                    }
                    else if (chr >= 0x20 && chr < 0x80)
                    {
                        *outPtr++ = chr;
                    }
                    else
                    {
                        switch (chr)
                        {
                        case bdJSON::BD_JSON_CR_CHAR:
                            *outPtr++ = bdJSON::BD_JSON_ESCAPE_CHAR;
                            *outPtr++ = 'r';
                            break;
                        case bdJSON::BD_JSON_LF_CHAR:
                            *outPtr++ = bdJSON::BD_JSON_ESCAPE_CHAR;
                            *outPtr++ = 'n';
                            break;
                        case bdJSON::BD_JSON_TAB_CHAR:
                            *outPtr++ = bdJSON::BD_JSON_ESCAPE_CHAR;
                            *outPtr++ = 't';
                            break;
                        case bdJSON::BD_JSON_FF_CHAR:
                            *outPtr++ = bdJSON::BD_JSON_ESCAPE_CHAR;
                            *outPtr++ = 'f';
                            break;
                        case bdJSON::BD_JSON_BACK_CHAR:
                            *outPtr++ = bdJSON::BD_JSON_ESCAPE_CHAR;
                            *outPtr++ = 'b';
                            break;

                        default:
                        {
                            const bdJSONCodePoint codePoint = getCodePointFromUTF8(reinterpret_cast<const bdNChar8 *>(&chr));
                            if (codePoint.m_codePoint < 0x10000)
                            {
                                bdSnprintf(outPtr, bdJSON::BD_JSON_ESCAPED_UNICODE_SIZE + 1, "\\u%04x", codePoint.m_codePoint);
                                outPtr += bdJSON::BD_JSON_ESCAPED_UNICODE_SIZE;
                            }
                            else
                            {
                                const bdUInt32 point = codePoint.m_codePoint - 0x10000;
                                const bdUInt32 hiPoint = 0xD800 | ((point >> 10) & 0x3FF);
                                const bdUInt32 loPoint = 0xDC00 | (point & 0x3FF);
                                bdSnprintf(outPtr, bdJSON::BD_JSON_ESCAPED_UNICODE_SIZE + 1, "\\u%04x", hiPoint);
                                bdSnprintf(outPtr + bdJSON::BD_JSON_ESCAPED_UNICODE_SIZE, bdJSON::BD_JSON_ESCAPED_UNICODE_SIZE + 1, "\\u%04x", loPoint);
                                outPtr += bdJSON::BD_JSON_ESCAPED_SURROGATE_PAIR_SIZE;
                            }

                            i += codePoint.m_numReprBytes - 1;
                        }
                        break;
                        }
                    }
                }
                bytesWritten = outPtr - outBuffer;
            }
            if (bytesWritten != resultSize)
            {
                resultSize = BD_JSON_ESCAPE_ERROR;
            }
        }
    }


    return resultSize;
}


bdInt32 bdJSONUtility::deescapeString(const bdNChar8 *const input, bdNChar8 *const outBuffer,
                                      const bdUInt32 outBufferSz)
{

    const bdInt32 length = bdStrlen(input);
    return deescapeString(input, length, outBuffer, outBufferSz);
}

bdInt32 bdJSONUtility::deescapeString(const bdNChar8 *const input, const bdUInt32 inputSz,
                                      bdNChar8 *const outBuffer, const bdUInt32 outBufferSz)
{
    const bdUInt32 length = inputSz;
    const bdNChar8 * ptr = input;
    bdUInt32 numEscapeChars = 0;
    bdUInt32 i = 0;
    bdInt32 resultSize = BD_JSON_ESCAPE_ERROR;
    bdBool valid = true;




    while (i < length && valid)
    {
        if (ptr[i] == bdJSON::BD_JSON_ESCAPE_CHAR)
        {
            ++numEscapeChars;
            ++i;
            if (ptr[i] == bdJSON::BD_JSON_UNICODE_CHAR)
            {

                if (i + 4 > length)
                {
                    bdLogError("bdJSON/utility", "Truncated unicode escape");
                    valid = false;
                }
                else
                {
                    const bdJSONCodePoint codePoint = getCodePointFromJSON(&ptr[i + 1]);
                    if (!codePoint.m_valid)
                    {
                        valid = false;
                    }
                    else
                    {



                        i += codePoint.m_numRead;
                        numEscapeChars += bdJSON::BD_JSON_ESCAPED_UNICODE_SIZE - 1 - codePoint.m_numReprBytes;
                        if (codePoint.m_codePoint > 0xFFFF)
                        {

                            numEscapeChars += bdJSON::BD_JSON_ESCAPED_UNICODE_SIZE;
                        }
                    }
                }
            }
        }
        ++i;
    }
    if (valid)
    {
        resultSize = length - numEscapeChars;


        if (outBuffer != NULL && resultSize < outBufferSz)
        {
            if (numEscapeChars == 0)
            {

                bdMemcpy(outBuffer, input, resultSize);
                outBuffer[resultSize] = bdJSON::BD_JSON_NULL_CHAR;
            }
            else
            {
                bdNChar8 * outPtr = outBuffer;
                outBuffer[resultSize] = bdJSON::BD_JSON_NULL_CHAR;

                for (i = 0; i < length; ++i)
                {
                    if (ptr[i] == bdJSON::BD_JSON_ESCAPE_CHAR)
                    {
                        ++i;
                        switch (ptr[i])
                        {
                        case 'r': *outPtr++ = bdJSON::BD_JSON_CR_CHAR; break;
                        case 'n': *outPtr++ = bdJSON::BD_JSON_LF_CHAR; break;
                        case 't': *outPtr++ = bdJSON::BD_JSON_TAB_CHAR; break;
                        case 'f': *outPtr++ = bdJSON::BD_JSON_FF_CHAR; break;
                        case 'b': *outPtr++ = bdJSON::BD_JSON_BACK_CHAR; break;
                        case bdJSON::BD_JSON_UNICODE_CHAR:
                        {

                            const bdJSONCodePoint cp = getCodePointFromJSON(&ptr[i + 1]);
                            if (cp.m_codePoint < 0x80)
                            {

                                *outPtr++ = static_cast<bdNChar8>(cp.m_codePoint);
                            }
                            else if (cp.m_codePoint < 0x800)
                            {
                                *outPtr++ = static_cast<bdNChar8>(0xC0 | ((cp.m_codePoint >> 6) & 0x1F));
                                *outPtr++ = static_cast<bdNChar8>(0x80 | (cp.m_codePoint & 0x3F));
                            }
                            else if (cp.m_codePoint < 0x10000)
                            {
                                *outPtr++ = static_cast<bdNChar8>(0xE0 | ((cp.m_codePoint >> 12) & 0x0F));
                                *outPtr++ = static_cast<bdNChar8>(0x80 | ((cp.m_codePoint >> 6) & 0x3F));
                                *outPtr++ = static_cast<bdNChar8>(0x80 | (cp.m_codePoint & 0x3F));
                            }
                            else
                            {

                                *outPtr++ = static_cast<bdNChar8>(0xF0 | ((cp.m_codePoint >> 18) & 0x07));
                                *outPtr++ = static_cast<bdNChar8>(0x80 | ((cp.m_codePoint >> 12) & 0x3F));
                                *outPtr++ = static_cast<bdNChar8>(0x80 | ((cp.m_codePoint >> 6) & 0x3F));
                                *outPtr++ = static_cast<bdNChar8>(0x80 | (cp.m_codePoint & 0x3F));
                            }

                            i += cp.m_numRead;
                        }
                        break;
                        default:
                            *outPtr++ = ptr[i];
                            break;
                        }
                    }
                    else
                    {
                        *outPtr++ = ptr[i];
                    }
                }
            }
        }
    }


    return resultSize;
}
// NON_MATCHING: 9 of 90; only the order the result fields are ORed into x0 at the return. Locals
// and the 584 test are on their DWARF lines/columns; init forms and m_valid placement tried.
bdJSONUtility::bdJSONCodePoint bdJSONUtility::getCodePointFromJSON(const bdNChar8 * ptr)
{

    bdJSONCodePoint result = {0, 0, 0, false};




    bdNChar8 * endPtr;
    bdNChar8 hex[8] = {ptr[0], ptr[1], ptr[2], ptr[3]};

    result.m_codePoint = static_cast<bdUInt32>(strtol(hex, &endPtr, 16));
    if (endPtr != hex + 4)
    {
        bdLogError("bdJSON/utility", "Invalid unicode escape: %s", hex);
        result.m_codePoint = 0;
        return result;
    }
    result.m_numRead = 4;






    if ((result.m_codePoint & 0xFC00) == 0xD800)
    {
        if (ptr[4] == bdJSON::BD_JSON_ESCAPE_CHAR && ptr[5] == bdJSON::BD_JSON_UNICODE_CHAR)
        {

            bdMemcpy(hex, ptr + 6, 4);
            const bdUInt32 loPoint = strtol(hex, &endPtr, 16);
            if ((endPtr == hex + 4) && ((loPoint & 0xFC00) == 0xDC00))
            {
                result.m_codePoint = (((result.m_codePoint & 0x3FF) << 10) | (loPoint & 0x3FF)) + 0x10000;
                result.m_numRead = 10;
            }
            else
            {
                bdLogError("bdJSON/utility", "Invalid unicode escape: %s", hex);
                result.m_codePoint = 0;
                result.m_numRead = 0;
                return result;
            }
        }
        else
        {

            bdLogError("bdJSON/utility", "Missing low surrogate");
            return result;
        }
    }




    if (result.m_codePoint < 0x80)
    {
        result.m_numReprBytes = 1;
    }
    else if (result.m_codePoint < 0x800)
    {
        result.m_numReprBytes = 2;
    }
    else if (result.m_codePoint < 0x10000)
    {
        result.m_numReprBytes = 3;
    }
    else
    {
        result.m_numReprBytes = 4;
    }
    result.m_valid = true;


    return result;
}


bdJSONUtility::bdJSONCodePoint bdJSONUtility::getCodePointFromUTF8(const bdNChar8 * ptr)
{
    bdJSONCodePoint result = {0, 0, 0, false};
    const bdUByte8 ptrVal0 = static_cast<bdUByte8>(ptr[0]);

    if (ptrVal0 < 0x80)
    {
        result.m_codePoint = ptrVal0;
        result.m_numReprBytes = 1;
        result.m_numRead = 1;
        result.m_valid = true;
    }
    else if ((ptrVal0 & 0xE0) == 0xC0)
    {
        const bdUByte8 ptrVal1 = static_cast<bdUByte8>(ptr[1]);
        if ((ptrVal1 & 0xC0) == 0x80)
        {


            result.m_codePoint = ((ptrVal0 & 0x1F) << 6) | (ptrVal1 & 0x3F);
            result.m_numReprBytes = 2;
            result.m_numRead = 2;
            result.m_valid = true;
        }
    }
    else if ((ptrVal0 & 0xF0) == 0xE0)
    {
        const bdUByte8 ptrVal1 = static_cast<bdUByte8>(ptr[1]);
        const bdUByte8 ptrVal2 = static_cast<bdUByte8>(ptr[2]);
        if ((ptrVal1 & 0xC0) == 0x80 && (ptrVal2 & 0xC0) == 0x80)
        {


            result.m_codePoint = ((ptrVal0 & 0x0F) << 12) | ((ptrVal1 & 0x3F) << 6) | (ptrVal2 & 0x3F);
            result.m_numReprBytes = 3;
            result.m_numRead = 3;
            result.m_valid = true;
        }
    }
    else if ((ptrVal0 & 0xF8) == 0xF0)
    {
        const bdUByte8 ptrVal1 = static_cast<bdUByte8>(ptr[1]);
        const bdUByte8 ptrVal2 = static_cast<bdUByte8>(ptr[2]);
        const bdUByte8 ptrVal3 = static_cast<bdUByte8>(ptr[3]);
        if ((ptrVal1 & 0xC0) == 0x80 && (ptrVal2 & 0xC0) == 0x80 && (ptrVal3 & 0xC0) == 0x80)
        {


            result.m_codePoint = ((ptrVal0 & 0x07) << 18) | ((ptrVal1 & 0x3F) << 12) | ((ptrVal2 & 0x3F) << 6)
                                 | (ptrVal3 & 0x3F);
            result.m_numReprBytes = 4;
            result.m_numRead = 4;
            result.m_valid = true;
        }
    }

    if (!result.m_valid)
    {

        bdLogError("bdJSON/utility", "Invalid UTF-8 sequence: %s", ptr);
    }

    return result;
}
