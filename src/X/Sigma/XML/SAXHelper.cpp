#include "SAXHelper.h"

#include <stdio.h>

#include "../Memory/SigmaMemory.h"
#include "../RefString.h"

const LPCSTR SAX::FLOAT_STR_FORMAT = "%.10g";
const LPCSTR SAX::INT32_STR_FORMAT = "%d";
const LPCSTR SAX::INT64_STR_FORMAT = "%lld";

SAX::SAX()
{
    m_nIndent = 0;
    m_nBufferOffset = 0;
    SigmaMemoryClear(m_sBuffer, sizeof(m_sBuffer));
}

void SAX::WriteChar(CHAR cValue)
{
    m_sBuffer[m_nBufferOffset] = cValue;
    m_nBufferOffset++;
    if (m_nBufferOffset == MAX_BUFFER_SIZE)
        Flush();
}

void SAX::WriteString(LPCSTR uszValue)
{
    for (LPCSTR lpszCurrentChar = uszValue; *lpszCurrentChar; lpszCurrentChar++)
        WriteChar(*lpszCurrentChar);
}

void SAX::Indent()
{
    m_nIndent++;
}

void SAX::WriteIndent()
{
    for (int32 i = 0; i < m_nIndent; i++)
        WriteChar('\t');
}

void SAX::Outdent()
{
    m_nIndent--;
}

void SAX::WriteBeginTag(LPCSTR szName, LPCSTR szAttribute)
{
    WriteString("<");
    WriteString(szName);
    if (szAttribute)
    {
        WriteChar(' ');
        WriteString(szAttribute);
    }
    WriteString(">");
}

void SAX::WriteEndTag(LPCSTR szName)
{
    WriteString("</");
    WriteString(szName);
    WriteString(">");
}

void SAX::WriteLine()
{
    WriteString("\r\n");
}

void SAX::WriteBeginElement(LPCSTR szName, LPCSTR szAttribute)
{
    WriteIndent();
    WriteBeginTag(szName, szAttribute);
    WriteLine();
    Indent();
}

void SAX::WriteEndElement(LPCSTR szName)
{
    Outdent();
    WriteIndent();
    WriteEndTag(szName);
    WriteLine();
}

void SAX::WriteIntElement(LPCSTR szName, int32 nValue, LPCSTR szAttribute)
{
    CHAR sz[128];
    sprintf(sz, INT32_STR_FORMAT, nValue);
    WriteStringElement(szName, sz, szAttribute);
}

void SAX::WriteStringElement(LPCSTR szName, LPCSTR uszValue, LPCSTR szAttribute)
{
    WriteIndent();
    WriteBeginTag(szName, szAttribute);
    for (LPCSTR lpszCurrentChar = uszValue; *lpszCurrentChar; lpszCurrentChar++)
        WriteChar(*lpszCurrentChar);
    WriteEndTag(szName);
    WriteLine();
}

void SAXLogString::Flush()
{
    *m_pszDestBuffer += m_sBuffer;
    m_nBufferOffset = 0;
}

void SAXLogString::WriteBeginTag(LPCSTR szName, LPCSTR szAttribute)
{
    WriteString(szName);
    if (szAttribute)
    {
        WriteChar(' ');
        WriteString(szAttribute);
    }
    WriteString(":");
}

void SAXLogString::WriteEndTag(LPCSTR szName)
{
}

void SAXLogString::WriteLine()
{
}

void SAXLogString::WriteIndent()
{
    WriteString(" ");
}
