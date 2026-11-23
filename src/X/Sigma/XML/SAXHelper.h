// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/XML/SAXHelper.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Files/XFiles.h"
#include "../SigmaTypes.h"
#include "../Unicode.h"

class CRefString;

static const int32 MAX_BUFFER_SIZE = 4096;

class SAX {  // size 0x1010
    // vtable pointer
public:
    SAX();
    void WriteBeginElement(LPCSTR szName, LPCSTR szAttribute);
    void WriteEndElement(LPCSTR szName);
    void WriteIntElement(LPCSTR szName, int32 nValue, LPCSTR szAttribute);
    void WriteInt64Element(LPCSTR, int64, LPCSTR);
    void WriteFloatElement(LPCSTR, float, LPCSTR);
    void WriteStringElement(LPCSTR szName, LPCSTR uszValue, LPCSTR szAttribute);
    static const LPCSTR FLOAT_STR_FORMAT;
    static const LPCSTR INT32_STR_FORMAT;
    static const LPCSTR INT64_STR_FORMAT;
protected:
    virtual void Flush() = 0;  // vtable slot 0
    void Indent();
    virtual void WriteIndent();  // vtable slot 1
    void Outdent();
    virtual void WriteBeginTag(LPCSTR szName, LPCSTR szAttribute);  // vtable slot 2
    virtual void WriteEndTag(LPCSTR szName);  // vtable slot 3
    virtual void WriteLine();  // vtable slot 4
    void WriteString(LPCSTR uszValue);
    void WriteChar(CHAR cValue);
    int32 m_nIndent;  // 0x08
    char m_sBuffer[MAX_BUFFER_SIZE];  // 0x0c
    int32 m_nBufferOffset;  // 0x100c
};
static_assert(sizeof(SAX) == 0x1010, "SAX");

class SAXFile : public SAX {  // size 0x2020
public:
    BOOL Create(LPCSTR);
    void WriteEOF();
    void Dispose();
    virtual void Flush();  // vtable slot 0
private:
    FileReference m_tFileRef;  // 0x1010
};
static_assert(sizeof(SAXFile) == 0x2020, "SAXFile");

class SAXString : public SAX {  // size 0x1018
public:
    SAXString(CRefString *);
    ~SAXString();
    virtual void Flush();  // vtable slot 0
private:
    CRefString * m_pszDestBuffer;  // 0x1010
};
static_assert(sizeof(SAXString) == 0x1018, "SAXString");

class SAXLogString : public SAX {  // size 0x1018
public:
    // SAXHelper.h:127-137. Inline in the original; the bodies are the copies inlined into
    // sTraceMessage (X/Server/Network/ServerMessageReceive.cpp).
    SAXLogString(CRefString * pszDestBuffer) : m_pszDestBuffer(pszDestBuffer) {}
    ~SAXLogString() { Flush(); }
    virtual void Flush();  // vtable slot 0
    virtual void WriteBeginTag(LPCSTR szName, LPCSTR szAttribute);  // vtable slot 2
    virtual void WriteEndTag(LPCSTR szName);  // vtable slot 3
    virtual void WriteLine();  // vtable slot 4
    virtual void WriteIndent();  // vtable slot 1
private:
    CRefString * m_pszDestBuffer;  // 0x1010
};
static_assert(sizeof(SAXLogString) == 0x1018, "SAXLogString");

