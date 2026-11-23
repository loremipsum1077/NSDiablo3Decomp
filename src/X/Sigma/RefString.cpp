#include "RefString.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "DataStructures/XChainedFixedBlockAllocator.h"
#include "Memory/SigmaMemory.h"
#include "Sigma.h"
#include "UnicodeText.h"

// X/Common/Shell/MainNX64.cpp; the platform's _vscprintf prints into it.
extern char g_avscprintfScratch[1];
#define _vscprintf(szFormat, arglist) vsnprintf(g_avscprintfScratch, 1, szFormat, arglist)

enum CRefStringBufferLocation {
    BUFFER_LOCATION_UNKNOWN = 0,
    BUFFER_LOCATION_LITTLE_POOL = 1,
    BUFFER_LOCATION_BIG_POOL = 2,
    BUFFER_LOCATION_SYSTEM_MEMORY = 3,
};

class CRefStringData {
public:
    CRefStringData();
    ~CRefStringData();
    LPSTR data();

    short m_nReferenceCount;
    short m_eBufferLocation;
    size_t m_nDataLength;
    size_t m_nAllocatedStringLength;
    RefStringMemPool* m_ptRefStringMemPool;
    clock_t m_nCreationTime;

    static uint32 s_nCount;
};

static const int32 CSTRING_SIZE = 128;

struct TRefStringDataBuffer {
    TRefStringDataBuffer() { m_szString[0] = '\0'; }

    CRefStringData m_data;
    CHAR m_szString[CSTRING_SIZE + 1];
};

static const int32 BIG_CSTRING_SIZE = 4096;

static TRefStringDataBuffer gRefStringDataBufferNil;
static CRefStringData* gRefStringDataNil = &gRefStringDataBufferNil.m_data;

uint32 CRefStringData::s_nCount;

inline CRefStringData::CRefStringData()
    : m_nReferenceCount(-1), m_eBufferLocation(BUFFER_LOCATION_UNKNOWN), m_nDataLength(0),
      m_nAllocatedStringLength(0), m_ptRefStringMemPool(NULL) {
    m_nCreationTime = clock();
    s_nCount++;
}

inline CRefStringData::~CRefStringData() {
    s_nCount--;
}

inline LPSTR CRefStringData::data() {
    return (LPSTR)(this + 1);
}

struct TBigRefStringDataBuffer {
    TBigRefStringDataBuffer() { m_szString[0] = '\0'; }

    CRefStringData m_data;
    CHAR m_szString[BIG_CSTRING_SIZE + 1];
};

typedef XChainedFixedBlockAllocator<TRefStringDataBuffer> TRefStringDataAllocator;
typedef XChainedFixedBlockAllocator<TBigRefStringDataBuffer> TBigRefStringDataAllocator;

struct RefStringMemPool {
    TRefStringDataAllocator* ptRefStringDataAllocator;
    TBigRefStringDataAllocator* ptBigRefStringDataAllocator;
};

struct RefStringGlobals {
    RefStringMemPool tMemPool;
    UpCallForRefStringMemPool fpUpcallForMemPool;
};

static BOOL s_bInitialized;
static Blizzard::Lock::RMutex* s_ptMutex;

static void sRefStringInitializeMemPool(RefStringMemPool* ptRefStringMemPool, BOOL bCreateBigRefStringAllocator,
                                        XMemoryAllocatorInterface* ptMemoryInterface) {
    ptRefStringMemPool->ptRefStringDataAllocator =
        (TRefStringDataAllocator*)SigmaMemoryNew(sizeof(TRefStringDataAllocator), 4, ptMemoryInterface, FALSE);
    ptRefStringMemPool->ptRefStringDataAllocator->Initialize();
    ptRefStringMemPool->ptRefStringDataAllocator->SetMemoryInterface(ptMemoryInterface);
    ptRefStringMemPool->ptRefStringDataAllocator->SetBlockCount_Tracked(1000);

    if (bCreateBigRefStringAllocator) {
        ptRefStringMemPool->ptBigRefStringDataAllocator =
            (TBigRefStringDataAllocator*)SigmaMemoryNew(sizeof(TBigRefStringDataAllocator), 4, ptMemoryInterface, FALSE);
        ptRefStringMemPool->ptBigRefStringDataAllocator->Initialize();
        ptRefStringMemPool->ptBigRefStringDataAllocator->SetMemoryInterface(ptMemoryInterface);
        ptRefStringMemPool->ptBigRefStringDataAllocator->SetBlockCount_Tracked(4);
    } else {
        ptRefStringMemPool->ptBigRefStringDataAllocator = NULL;
    }
}

static void sRefStringDisposeMemPool(RefStringMemPool* ptRefStringMemPool, XMemoryAllocatorInterface* ptMemoryInterface) {
    ptRefStringMemPool->ptRefStringDataAllocator->Validate();
    if (ptRefStringMemPool->ptBigRefStringDataAllocator != NULL) {
        ptRefStringMemPool->ptBigRefStringDataAllocator->Validate();
    }

    if (ptRefStringMemPool->ptRefStringDataAllocator->GetUsedBlockCount() != 0 ||
        (ptRefStringMemPool->ptBigRefStringDataAllocator != NULL &&
         ptRefStringMemPool->ptBigRefStringDataAllocator->GetUsedBlockCount() != 0)) {
    }

    ptRefStringMemPool->ptRefStringDataAllocator->Dispose();
    XFREE(ptRefStringMemPool->ptRefStringDataAllocator, ptMemoryInterface);

    if (ptRefStringMemPool->ptBigRefStringDataAllocator != NULL) {
        ptRefStringMemPool->ptBigRefStringDataAllocator->Dispose();
        XFREE(ptRefStringMemPool->ptBigRefStringDataAllocator, ptMemoryInterface);
    }
}

void RefStringInitializeForApp() {
    SigmaGlobals& tSigmaGlobals = gtSigmaGlobals;
    RefStringGlobals*& ptRefStringGlobals = tSigmaGlobals.ptRefStringGlobals;

    UnicodeText::Initialize();

    s_ptMutex = new Blizzard::Lock::RMutex;

    ptRefStringGlobals = (RefStringGlobals*)SigmaMemoryNew(sizeof(RefStringGlobals), 4, NULL, TRUE);

    sRefStringInitializeMemPool(&ptRefStringGlobals->tMemPool, TRUE, NULL);

    s_bInitialized = TRUE;
}

void RefStringDisposeForApp() {
    SigmaGlobals& tSigmaGlobals = gtSigmaGlobals;
    RefStringGlobals*& ptRefStringGlobals = tSigmaGlobals.ptRefStringGlobals;

    s_bInitialized = FALSE;

    sRefStringDisposeMemPool(&ptRefStringGlobals->tMemPool, NULL);
    XFREE(ptRefStringGlobals, NULL);

    delete s_ptMutex;

    UnicodeText::Dispose();
}

// LINKER-REMOVED: (DWARF: line 292, low_pc 0, 12 bytes). Its read keeps s_bInitialized
// and both stores alive, as a byte, through the late GlobalOpt (docs/late_globalopt.md).
BOOL RefStringIsInitialized() {
    return s_bInitialized;
}

void RefStringSetUpcallForMemoryPool(UpCallForRefStringMemPool fpUpcallForMemPool) {
    SigmaGlobals& tSigmaGlobals = gtSigmaGlobals;
    RefStringGlobals* ptRefStringGlobals = tSigmaGlobals.ptRefStringGlobals;
    ptRefStringGlobals->fpUpcallForMemPool = fpUpcallForMemPool;
}

RefStringMemPool* RefStringMemPoolCreate(XMemoryAllocatorInterface* ptMemoryInterface, BOOL bCreateBigRefStringAllocator) {
    RefStringMemPool* ptRefStringMemPool =
        (RefStringMemPool*)SigmaMemoryNew(sizeof(RefStringMemPool), 4, ptMemoryInterface, TRUE);
    sRefStringInitializeMemPool(ptRefStringMemPool, bCreateBigRefStringAllocator, ptMemoryInterface);
    return ptRefStringMemPool;
}

void RefStringMemPoolDelete(RefStringMemPool*& ptRefStringMemPool, XMemoryAllocatorInterface* ptMemoryInterface) {
    sRefStringDisposeMemPool(ptRefStringMemPool, ptMemoryInterface);
    XFREE(ptRefStringMemPool, ptMemoryInterface);
}

inline RefStringMemPool* sRefStringGetActiveMemoryPool(BOOL bForceAppLevelMemoryPool) {
    SigmaGlobals& tSigmaGlobals = gtSigmaGlobals;
    RefStringGlobals* ptRefStringGlobals = tSigmaGlobals.ptRefStringGlobals;

    if (!bForceAppLevelMemoryPool && ptRefStringGlobals->fpUpcallForMemPool != NULL) {
        RefStringMemPool* ptMemPool = ptRefStringGlobals->fpUpcallForMemPool();
        if (ptMemPool != NULL) {
            return ptMemPool;
        }
    }

    return &ptRefStringGlobals->tMemPool;
}

CRefString::CRefString() {
    m_bForceAppLevelMemoryPool = FALSE;
    _Init();
}

CRefString::CRefString(BOOL bForceAppLevelMemoryPool) {
    m_bForceAppLevelMemoryPool = bForceAppLevelMemoryPool;
    _Init();
}

CRefString::CRefString(const CRefString& szString) {
    m_bForceAppLevelMemoryPool = FALSE;
    _AssignByReference(szString);
}

CRefString::CRefString(LPCSTR szString) {
    m_bForceAppLevelMemoryPool = FALSE;
    _AssignByCopy(szString);
}

void CRefString::_CommonCtorBody(LPCSTR szString, int32 nLength) {
    m_bForceAppLevelMemoryPool = FALSE;

    if (szString == NULL || nLength == 0 || szString[0] == '\0') {
        _Init();
    } else {
        _AssignByCopyLength(szString, nLength);
    }
}

CRefString::CRefString(LPCSTR szString, int32 nLength) {
    _CommonCtorBody(szString, nLength);
}

CRefString::CRefString(LPCSTR szString, uint32 nLength) {
    _CommonCtorBody(szString, nLength);
}

CRefString::CRefString(LPCSTR szString, int64 nLength) {
    _CommonCtorBody(szString, (int32)nLength);
}

inline void CRefString::_Init() {
    m_pData = gRefStringDataNil;
    m_pszString = gRefStringDataNil->data();
}

void CRefString::_ReAllocate(size_t nLength, BOOL bSaveData) {
    if (nLength <= m_pData->m_nAllocatedStringLength) {
        return;
    }

    s_ptMutex->Enter();

    size_t nNewAllocatedLength;
    CRefStringBufferLocation eBufferLocation;
    CRefStringData* pNewData = NULL;
    short nNewReferenceCount = (m_pData->m_nAllocatedStringLength == 0) ? 1 : m_pData->m_nReferenceCount;

    RefStringMemPool* ptRefStringMemPool = sRefStringGetActiveMemoryPool(m_bForceAppLevelMemoryPool);

    if (nLength <= CSTRING_SIZE) {
        pNewData = &ptRefStringMemPool->ptRefStringDataAllocator->Alloc_Tracked()->m_data;
        nNewAllocatedLength = CSTRING_SIZE;
        eBufferLocation = BUFFER_LOCATION_LITTLE_POOL;
    } else if (nLength <= BIG_CSTRING_SIZE && ptRefStringMemPool->ptBigRefStringDataAllocator != NULL) {
        pNewData = &ptRefStringMemPool->ptBigRefStringDataAllocator->Alloc_Tracked()->m_data;
        nNewAllocatedLength = BIG_CSTRING_SIZE;
        eBufferLocation = BUFFER_LOCATION_BIG_POOL;
    }

    if (pNewData == NULL) {
        void* pRawBuffer = SigmaMemoryNew(sizeof(CRefStringData) + nLength + 1, 4, NULL, FALSE);
        pNewData = new (pRawBuffer) CRefStringData;
        nNewAllocatedLength = nLength;
        eBufferLocation = BUFFER_LOCATION_SYSTEM_MEMORY;
    }

    pNewData->m_nReferenceCount = nNewReferenceCount;
    pNewData->m_eBufferLocation = eBufferLocation;
    pNewData->m_nAllocatedStringLength = nNewAllocatedLength;
    pNewData->m_ptRefStringMemPool = ptRefStringMemPool;

    if (bSaveData == TRUE) {
        SigmaMemoryCopy(pNewData->data(), m_pData->data(), m_pData->m_nDataLength + 1);
        pNewData->m_nDataLength = m_pData->m_nDataLength;
    } else {
        pNewData->data()[0] = '\0';
        pNewData->m_nDataLength = 0;
    }

    s_ptMutex->Leave();

    _Free();

    m_pData = pNewData;
    m_pszString = pNewData->data();
}

void CRefString::_Allocate(size_t nLength) {
    s_ptMutex->Enter();

    CRefStringBufferLocation eBufferLocation;
    CRefStringData* pData = NULL;
    RefStringMemPool* ptRefStringMemPool = sRefStringGetActiveMemoryPool(m_bForceAppLevelMemoryPool);

    if (nLength <= CSTRING_SIZE) {
        pData = &ptRefStringMemPool->ptRefStringDataAllocator->Alloc_Tracked()->m_data;
        pData->m_nAllocatedStringLength = CSTRING_SIZE;
        eBufferLocation = BUFFER_LOCATION_LITTLE_POOL;
    } else if (nLength <= BIG_CSTRING_SIZE && ptRefStringMemPool->ptBigRefStringDataAllocator != NULL) {
        pData = &ptRefStringMemPool->ptBigRefStringDataAllocator->Alloc_Tracked()->m_data;
        pData->m_nAllocatedStringLength = BIG_CSTRING_SIZE;
        eBufferLocation = BUFFER_LOCATION_BIG_POOL;
    }

    if (pData == NULL) {
        void* pRawBuffer = SigmaMemoryNew(sizeof(CRefStringData) + nLength + 1, 4, NULL, FALSE);
        pData = new (pRawBuffer) CRefStringData;
        pData->m_nAllocatedStringLength = nLength;
        eBufferLocation = BUFFER_LOCATION_SYSTEM_MEMORY;
    }

    pData->m_nReferenceCount = 1;
    pData->m_eBufferLocation = eBufferLocation;
    pData->m_ptRefStringMemPool = ptRefStringMemPool;

    m_pData = pData;
    m_pszString = pData->data();

    s_ptMutex->Leave();
}

void CRefString::_Free() {
    s_ptMutex->Enter();

    if (m_pData->m_nAllocatedStringLength != 0) {
        if (m_pData->m_eBufferLocation == BUFFER_LOCATION_SYSTEM_MEMORY) {
            SigmaMemoryFree(m_pData, NULL);
        } else {
            SigmaGlobals& tSigmaGlobals = gtSigmaGlobals;
            RefStringGlobals* ptRefStringGlobals = tSigmaGlobals.ptRefStringGlobals;

            if (ptRefStringGlobals != NULL) {
                RefStringMemPool* ptRefStringMemPool = sRefStringGetActiveMemoryPool(m_bForceAppLevelMemoryPool);

                if (m_pData->m_eBufferLocation == BUFFER_LOCATION_LITTLE_POOL) {
                    ptRefStringMemPool->ptRefStringDataAllocator->Free((TRefStringDataBuffer*)m_pData);
                } else {
                    ptRefStringMemPool->ptBigRefStringDataAllocator->Free((TBigRefStringDataBuffer*)m_pData);
                }
            }
        }
    }

    m_pData = NULL;

    s_ptMutex->Leave();
}

inline BOOL CRefString::_IsNilString() const {
    return m_pData == gRefStringDataNil;
}

inline void CRefString::_AddRef() {
    if (!_IsNilString()) {
        m_pData->m_nReferenceCount++;
    }
}

inline void CRefString::_Release() {
    if (!_IsNilString()) {
        if (m_pData->m_nReferenceCount == 1) {
            _Free();
        } else {
            m_pData->m_nReferenceCount--;
        }

        _Init();
    }
}

CRefString::~CRefString() {
    _Release();
}

CRefString::operator const char*() const {
    return m_pszString;
}

LPCSTR CRefString::str() const {
    return m_pszString;
}

size_t CRefString::Length() const {
    return m_pData->m_nDataLength;
}

LPCSTR CRefString::Format(LPCSTR szFormat, ...) {
    va_list arglist;

    va_start(arglist, szFormat);
    int32 nLength = _vscprintf(szFormat, arglist);
    va_end(arglist);

    if (nLength == 0) {
        _Release();
        return m_pszString;
    }

    CRefString szNewString;
    szNewString.m_bForceAppLevelMemoryPool = m_bForceAppLevelMemoryPool;
    szNewString._Allocate(nLength);
    LPSTR pszData = szNewString.m_pszString;

    va_start(arglist, szFormat);
    int32 nRetval = Blizzard::String::VFormat(pszData, nLength + 1, szFormat, arglist);
    (void)nRetval;
    va_end(arglist);

    pszData[nLength] = '\0';
    szNewString._SetDataLength(nLength);

    _Release();
    _AssignByReference(szNewString);

    return pszData;
}

BOOL CRefString::operator==(LPCSTR szString) const {
    return _CompareData(szString);
}

BOOL CRefString::operator==(const CRefString& szString) const {
    if (m_pData == szString.m_pData) {
        return TRUE;
    }

    if (Length() != szString.Length()) {
        return FALSE;
    }

    return _CompareData(szString);
}

inline BOOL CRefString::_CompareData(LPCSTR szString) const {
    return strcmp(m_pszString, szString) == 0;
}

CRefString& CRefString::operator=(const CRefString& szString) {
    if (m_pData == NULL) {
        _Init();
    }

    if (m_pData != szString.m_pData) {
        _Release();
        _AssignByReference(szString);
    }

    return *this;
}

CRefString& CRefString::operator=(LPCSTR szString) {
    if (m_pData == NULL) {
        _Init();
    }

    if (szString >= m_pszString && szString <= m_pszString + Length() && Length() != 0) {
        return *this;
    }

    _Release();
    _AssignByCopy(szString);

    return *this;
}

CRefString& CRefString::operator+=(LPCSTR szString) {
    if (_IsNilString()) {
        if (m_pszString != szString) {
            _AssignByCopy(szString);
        }
    } else {
        _Append(szString);
    }

    return *this;
}

CRefString operator+(const CRefString& lhs, const CRefString& rhs) {
    CRefString sTotalString("");

    if (lhs._IsNilString()) {
        sTotalString._AssignByCopy(rhs);
    } else {
        sTotalString._AssignByCopy(lhs);
        if (!rhs._IsNilString()) {
            sTotalString._Append(rhs);
        }
    }

    return sTotalString;
}

inline void CRefString::_AssignByReference(const CRefString& szString) {
    m_pData = szString.m_pData;
    m_pszString = m_pData->data();
    _AddRef();
}

inline void CRefString::_AssignByCopy(LPCSTR szString) {
    size_t nLength = strlen(szString);

    if (nLength == 0) {
        _Init();
    } else {
        _Allocate(nLength);
        SigmaMemoryMove(m_pszString, (void*)szString, nLength);
        m_pszString[nLength] = '\0';
        _SetDataLength(nLength);
    }
}

inline void CRefString::_AssignByCopyLength(LPCSTR szString, int32 nMaxLength) {
    size_t nLength = nMaxLength;

    _Allocate(nLength);
    strncpy(m_pszString, szString, nLength);
    m_pszString[nLength] = '\0';

    nLength = strlen(m_pszString);
    if (nLength == 0) {
        _Release();
        _Init();
    } else {
        _SetDataLength(nLength);
    }
}

void CRefString::_Append(LPCSTR szString) {
    size_t nAppendLength = strlen(szString);
    if (nAppendLength == 0) {
        return;
    }

    size_t nCurLength = Length();
    size_t nLength = nCurLength + nAppendLength;

    if (m_pData->m_nReferenceCount > 1) {
        LPCSTR szOldString = m_pszString;
        _Release();
        _Allocate(nLength);
        strncpy(m_pszString, szOldString, nCurLength);
    } else {
        _ReAllocate(nLength, TRUE);
    }

    LPSTR pszData = m_pszString;
    SigmaMemoryCopy(pszData + nCurLength, szString, nAppendLength);
    pszData[nLength] = '\0';
    _SetDataLength(nLength);
}

inline void CRefString::_SetDataLength(size_t nLength) {
    m_pData->m_nDataLength = nLength;
}

template <>
uint32 XHashKey<const CRefString&>(const CRefString& szString) {
    return XHashKey<LPCSTR>((LPCSTR)szString);
}

template <>
uint32 XHashKey<LPCSTR>(LPCSTR szString) {
    uint32 nHash = 0;
    while (*szString != '\0') {
        nHash = nHash * 33 + *szString++;
    }
    return nHash;
}

CRefString FormatRefStringArgList(LPCSTR szFormat, va_list tArgList) {
    int32 nLength = _vscprintf(szFormat, tArgList);
    CHAR* pBuffer = (CHAR*)__builtin_alloca(nLength + 1);
    int32 nRetval = Blizzard::String::VFormat(pBuffer, nLength + 1, szFormat, tArgList);
    (void)nRetval;
    pBuffer[nLength] = '\0';

    CRefString sResult(pBuffer);
    return sResult;
}

CRefString FormatRefString(LPCSTR szFormat, ...) {
    va_list tArgList;
    va_start(tArgList, szFormat);
    CRefString sResult = FormatRefStringArgList(szFormat, tArgList);
    va_end(tArgList);
    return sResult;
}
