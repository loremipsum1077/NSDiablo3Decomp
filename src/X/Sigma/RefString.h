#pragma once

#include <stdarg.h>

#include "DataStructures/XBaseMap.h"
#include "SigmaTypes.h"
#include "Unicode.h"

class XMemoryAllocatorInterface;

class CRefStringData;
struct RefStringMemPool;

typedef RefStringMemPool* (*UpCallForRefStringMemPool)();

class CRefString {
public:
    CRefString();
    CRefString(BOOL bForceAppLevelMemoryPool);
    CRefString(const CRefString& szString);
    CRefString(LPCSTR szString);
    CRefString(LPCSTR szString, int32 nLength);
    CRefString(LPCSTR szString, uint32 nLength);
    CRefString(LPCSTR szString, uint64 nLength);
    CRefString(LPCSTR szString, int64 nLength);
    ~CRefString();

    LPCSTR str() const;
    operator const char*() const;

    BOOL operator==(LPCSTR szString) const;
    BOOL operator==(const CRefString& szString) const;
    BOOL operator!=(LPCSTR szString) const { return !(*this == szString); }
    BOOL operator!=(const CRefString& szString) const { return !(*this == szString); }

    CRefString& operator=(const CRefString& szString);
    CRefString& operator=(LPCSTR szString);
    CRefString& operator+=(LPCSTR szString);

    size_t Length() const;
    LPCSTR Format(LPCSTR szFormat, ...);
    CRefString ToLower();
    CRefString ToUpper();

    friend CRefString operator+(const CRefString& lhs, const CRefString& rhs);

private:
    BOOL _IsNilString() const;
    void _AssignByReference(const CRefString& szString);
    void _AssignByCopy(LPCSTR szString);
    void _AssignByCopyLength(LPCSTR szString, int32 nMaxLength);
    void _Append(LPCSTR szString);
    void _Init();
    void _Allocate(size_t nLength);
    void _ReAllocate(size_t nLength, BOOL bSaveData);
    void _Free();
    void _AddRef();
    void _Release();
    BOOL _CompareData(LPCSTR szString) const;
    void _SetDataLength(size_t nLength);
    void _CommonCtorBody(LPCSTR szString, int32 nLength);

    CRefStringData* m_pData;
    LPSTR m_pszString;
    BOOL m_bForceAppLevelMemoryPool;
};

// XStringList (XGrowableList<CRefString, const CRefString&>) and XStringArray
// (blz::vector<CRefString>) are typedef'd here in the original; their headers are in other areas.

void RefStringInitializeForApp();
void RefStringDisposeForApp();
BOOL RefStringIsInitialized();
void RefStringSetUpcallForMemoryPool(UpCallForRefStringMemPool fpUpcallForMemPool);
RefStringMemPool* RefStringMemPoolCreate(XMemoryAllocatorInterface* ptMemoryInterface, BOOL bCreateBigRefStringAllocator);
void RefStringMemPoolDelete(RefStringMemPool*& ptRefStringMemPool, XMemoryAllocatorInterface* ptMemoryInterface);

CRefString operator+(const CRefString& lhs, const CRefString& rhs);

template <>
uint32 XHashKey<const CRefString&>(const CRefString& szString);
template <>
uint32 XHashKey<LPCSTR>(LPCSTR szString);

CRefString FormatRefStringArgList(LPCSTR szFormat, va_list tArgList);
CRefString FormatRefString(LPCSTR szFormat, ...);

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(CRefString) == 0x18, "CRefString");
