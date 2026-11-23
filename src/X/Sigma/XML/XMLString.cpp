// XMLString.cpp
//
// Helpers for moving a string between plain text and its XML-escaped form.
// The escaped form replaces the five XML metacharacters with their entities.
// Both conversions write into one shared scratch buffer.

#include <string.h>

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Unicode.h"

static const int32 MAX_XML_STRING_SIZE = 16384;
static CHAR gszXMLConvertedStringBuffer[MAX_XML_STRING_SIZE];

LPCSTR XMLStringConvertToXMLString( LPCSTR szString )
{
    const CHAR * pSrc = szString;
    CHAR * pDest = gszXMLConvertedStringBuffer;
    size_t nLen;

    while ( *pSrc != '\0' )
    {
        switch ( *pSrc )
        {
        case '"':
            memcpy( pDest, "&quot;", 7 );
            pDest += 5;
            break;
        case '&':
            memcpy( pDest, "&amp;", 6 );
            pDest += 4;
            break;
        case '\'':
            memcpy( pDest, "&apos;", 7 );
            pDest += 5;
            break;
        case '<':
            memcpy( pDest, "&lt;", 5 );
            pDest += 3;
            break;
        case '>':
            memcpy( pDest, "&gt;", 5 );
            pDest += 3;
            break;
        default:
            *pDest = *pSrc;
            break;
        }
        pDest++;
        pSrc++;
    }

    *pDest = '\0';
    return gszXMLConvertedStringBuffer;
}

LPCSTR XMLStringConvertFromXMLString( LPCSTR szString )
{
    CHAR * pSrc = (CHAR*)szString;
    CHAR * pDest = gszXMLConvertedStringBuffer;
    size_t nLen;

    while ( *pSrc != '\0' )
    {
        if ( *pSrc == '&' )
        {
            if ( strncmp( pSrc, "&amp;", 5 ) == 0 )
            {
                *pDest = '&';
                pSrc += 4;
            }
            else if ( strncmp( pSrc, "&lt;", 4 ) == 0 )
            {
                *pDest = '<';
                pSrc += 3;
            }
            else if ( strncmp( pSrc, "&gt;", 4 ) == 0 )
            {
                *pDest = '>';
                pSrc += 3;
            }
            else if ( strncmp( pSrc, "&quot;", 5 ) == 0 )
            {
                *pDest = '"';
                pSrc += 5;
            }
            else if ( strncmp( pSrc, "&apos;", 4 ) == 0 )
            {
                *pDest = '\'';
                pSrc += 5;
            }
            else
            {
                *pDest = '&';
            }
        }
        else
        {
            *pDest = *pSrc;
        }
        ++pDest;
        ++pSrc;
    }

    *pDest = '\0';
    return gszXMLConvertedStringBuffer;
}
