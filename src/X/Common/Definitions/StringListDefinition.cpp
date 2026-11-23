// Original file: X/Common/Definitions/StringListDefinition.cpp

#include "StringListDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/LanguageRules.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/StringManip.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <new>
#include <stdarg.h>
#include <string.h>

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/Game.cpp; no adopted header declares it yet.
XMemoryAllocatorInterface * AppGetCurrentMemoryInterface();

// X/Common/Game.h:412.







// X/Common/Game.h:543 and Game.cpp:412. AppGlobals is 0x23a8 bytes of mostly unadopted types;
// only the string debug mode is read here.
#include "../Game.h"




extern AppGlobals sgtAppGlobals;

// X/Common/FileIO/FileIO.h:36. Only the members this file calls are declared.
namespace FileIO {
class File {  // size 0x10
public:
    File(XLocale);
    ~File();
    bool Open(const char * szFilename);
    int64 GetFileSize() const;
    bool ReadFile(void * buffer, uint64 dwBytesToRead, uint64 * pdwBytesRead);
private:
    XLocale m_eLocale;  // 0x00
    void * m_Impl;  // 0x08 (FileIO::File::FileImpl *)
};
}  // namespace FileIO

enum {
    STRINGLIST_SNO_COMMON_VERSION = 42,
    STRINGLIST_XML_VERSION = 3,
    STRING_TABLE_DEFAULT_HASH_SIZE = 1024,
    STRINGLIST_SNO_PC_VERSION = 44,
    STRINGLIST_SNO_CONSOLE_VERSION = 48,
    STRINGLIST_SNO_AXE_VERSION = 44,
};

typedef XGrowableMap<int, const int &, int, const int &> StringlistOverrideMap;

static StringlistOverrideMap stStringlistOverrideMap(16, NULL);  // line 61

// The field names are "" in the binary.
static const TypeDescriptorField StringTableEntry_Fields[5] = {  // line 76
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(StringTableEntry, serText), NULL, 0),
    CStringTypeDescriptorField("", offsetof(StringTableEntry, szText), 0x21,
                               (int32)offsetof(StringTableEntry, serText) - (int32)offsetof(StringTableEntry, szText)),
    TypeDescriptorField("", DT_INT, offsetof(StringTableEntry, hLabel), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(StringTableEntry, dwPad), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(StringTableEntry), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_StringTableEntry("StringTableEntry", 0, StringTableEntry_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 88
const XTypeDescriptor * DT_STRING_TABLE_ENTRY = &TypeDescriptor_StringTableEntry;

static const TypeDescriptorField StringListDefinition_Fields[3] = {  // line 90
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(StringListDefinition, serStrings), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_STRING_TABLE_ENTRY, offsetof(StringListDefinition, arStrings), 0x21,
                                     (int32)offsetof(StringListDefinition, serStrings) - (int32)offsetof(StringListDefinition, arStrings)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(StringListDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_StringListDefinition("StringList", STRINGLIST_XML_VERSION, StringListDefinition_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 92
const XTypeDescriptor * DT_STRINGLIST = &TypeDescriptor_StringListDefinition;

StringListGroupDef * gptGroupDefStringList;  // line 100

static const LPCSTR uszMissingString = "!!Missing!!";  // line 106
static const LPCSTR uszEmptyString = "";  // line 107

// line 156
static StringTableMap * sAllocMapStringTable(int32 nStringCount) {
    if (nStringCount <= 16) {
        nStringCount = 16;
    }

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    void * pMem = SigmaMemoryNew(sizeof(StringTableMap), 4, ptMemoryInterface, FALSE);
    StringTableMap * ptMap = new (pMem) StringTableMap(16, ptMemoryInterface);
    ptMap->SetHashTableSize_Tracked(nStringCount / 2);
    ptMap->SetBlockCount_Tracked(nStringCount);
    return ptMap;
}

// line 178
static void sFreeMapStringTable(StringTableMap * ptMap) {
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    if (ptMap) {
        ptMap->~StringTableMap();
        SigmaMemoryFree(ptMap, ptMemoryInterface);
    }
}

// line 194
StringListGroupDef::StringListGroupDef()
    : GroupDefinition("StringList", STRINGLIST_SNO,
                      SNOVersions(STRINGLIST_SNO_PC_VERSION, 46, STRINGLIST_SNO_CONSOLE_VERSION,
                                  STRINGLIST_SNO_CONSOLE_VERSION, 49),
                      SNODATATYPE_TITLE, sizeof(StringListDefinition), DT_STRINGLIST, 0x4441, 0x501e) {
}

// line 223
uint32 StringListGroupDef::GetMaxCount() {
    return 128;
}

// line 470
BOOL StringListGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    DisplayInternalError(NULL, "", 476, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// line 482
void StringListGroupDef::PurgeDefinitionAndSNO(const SNO sno) {
    GroupDefinition::PurgeDefinitionAndSNO(sno);
}

// line 491
BOOL StringListGroupDef::AllowMultipleCheckout(SNO snoHandle) {
    DisplayInternalError(NULL, "", 495, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// line 504
void StringListGroupDef::Delete(void * lpDefinition) const {
    StringListDefinition * ptDefinition = (StringListDefinition *)lpDefinition;

    sFreeMapStringTable(ptDefinition->ptMapStringTable);
    ptDefinition->ptMapStringTable = NULL;
    ptDefinition->dwStringCount = ptDefinition->serStrings.dwSize / sizeof(StringTableEntry);

    GroupDefinition::Delete(lpDefinition);
}

static void sParseForStringTable(StringListDefinition * ptStringListDef);

// line 519
void StringListGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    StringListDefinition * ptDefinition = (StringListDefinition *)lpDefinition;
    ptDefinition->dwStringCount = ptDefinition->serStrings.ElementCount(sizeof(StringTableEntry));

    sParseForStringTable(ptDefinition);


}

// line 607
StringLabelHandle sHashLabel(LPCSTR pszName) {
    return XHashKey<LPCSTR>(pszName);
}

// line 616
StringTableEntry * StringListGetEntry(const StringListDefinition * pdefStringList, LPCSTR szLabel) {
    StringTableEntry * ptTableEntry = NULL;

    if (pdefStringList->ptMapStringTable == NULL) {
        return NULL;
    }

    pdefStringList->ptMapStringTable->Lookup(sHashLabel(szLabel), ptTableEntry);
    return ptTableEntry;
}

// line 635
static LPCSTR sStringListFetchByLabelInternal(const SNO snoStringList, StringLabelHandle hLabel, BOOL & fFound) {
    static CHAR szDebugStr[256];

    LPCSTR usString = NULL;
    SNO snoOverride;
    ScopedStringListDefinitionLockR pdefStringList(snoStringList);
    StringTableEntry * ptTableEntry = NULL;

    switch (sgtAppGlobals.eStringDebugMode) {
        case STRINGDEBUG_SNO_AND_LABEL:
            Blizzard::String::Format(szDebugStr, sizeof(szDebugStr), "%s:%d",
                                     SNOToString(STRINGLIST_SNO, snoStringList, FALSE).str(), hLabel);
            usString = szDebugStr;
            break;
        case STRINGDEBUG_SNO:
            Blizzard::String::Copy(szDebugStr, SNOToString(STRINGLIST_SNO, snoStringList, FALSE).str(), sizeof(szDebugStr));
            usString = szDebugStr;
            break;
        case STRINGDEBUG_LABEL:
            Blizzard::String::Format(szDebugStr, sizeof(szDebugStr), "%d", hLabel);
            usString = szDebugStr;
            break;
        default:
            fFound = FALSE;

            if (stStringlistOverrideMap.Lookup(snoStringList, snoOverride)) {
                ScopedStringListDefinitionLockR pdefStringListOverride(snoOverride);
                if (pdefStringListOverride->ptMapStringTable &&
                    pdefStringListOverride->ptMapStringTable->Lookup(hLabel, ptTableEntry)) {
                    usString = ptTableEntry->szText;
                    fFound = TRUE;
                }
            }

            if (ptTableEntry == NULL && pdefStringList->ptMapStringTable &&
                pdefStringList->ptMapStringTable->Lookup(hLabel, ptTableEntry)) {
                usString = ptTableEntry->szText;
                fFound = TRUE;
            }

            if (!fFound) {
                Blizzard::String::Format(szDebugStr, sizeof(szDebugStr), "%s - %d:%d", uszMissingString, snoStringList, hLabel);
                usString = szDebugStr;
            }
            break;
    }

    return usString;
}

// line 760
static LPCSTR sStringListFetchInternal(const SNO snoStringList, const CHAR * szLabel, BOOL & fFound) {
    static CHAR szDebugStr[256];

    SNO snoOverride;
    StringTableEntry * ptTableEntry = NULL;
    LPCSTR usString;
    ScopedStringListDefinitionLockR pdefStringList(snoStringList);

    switch (sgtAppGlobals.eStringDebugMode) {
        case STRINGDEBUG_SNO_AND_LABEL:
            Blizzard::String::Format(szDebugStr, sizeof(szDebugStr), "%s:%s",
                                     SNOToString(STRINGLIST_SNO, snoStringList, FALSE).str(), szLabel);
            usString = szDebugStr;
            break;
        case STRINGDEBUG_SNO:
            Blizzard::String::Copy(szDebugStr, SNOToString(STRINGLIST_SNO, snoStringList, FALSE).str(), sizeof(szDebugStr));
            usString = szDebugStr;
            break;
        case STRINGDEBUG_LABEL:
            Blizzard::String::Copy(szDebugStr, szLabel, sizeof(szDebugStr));
            usString = szDebugStr;
            break;
        default:
            if (stStringlistOverrideMap.Lookup(snoStringList, snoOverride)) {
                ScopedStringListDefinitionLockR pdefStringListOverride(snoOverride);
                ptTableEntry = StringListGetEntry(pdefStringListOverride, szLabel);
            }

            if (ptTableEntry == NULL) {
                ptTableEntry = StringListGetEntry(pdefStringList, szLabel);
            }

            fFound = FALSE;
            if (ptTableEntry) {
                usString = ptTableEntry->szText;
                fFound = TRUE;
            } else {
                Blizzard::String::Format(szDebugStr, sizeof(szDebugStr), "%s - %d:%s", uszMissingString, snoStringList, szLabel);
                usString = szDebugStr;
            }
            break;
    }

    return usString;
}

// line 870
CRefString StringListFetch(const SNO snoStringList, LPCSTR szLabel) {
    BOOL fSuccess;
    return CRefString(sStringListFetchInternal(snoStringList, szLabel, fSuccess));
}

// line 881
LPCSTR StringListFetchLPCSTR(const SNO snoStringList, LPCSTR szLabel) {
    BOOL fSuccess;
    return sStringListFetchInternal(snoStringList, szLabel, fSuccess);
}

// line 893
BOOL StringListFetch(const SNO snoStringList, LPCSTR szLabel, CRefString & usReturnString) {
    BOOL fSuccess;
    usReturnString = sStringListFetchInternal(snoStringList, szLabel, fSuccess);
    return fSuccess;
}

// line 924
void StringListDecodeDecoratedLabel(LPCSTR szSnoDecoratedLabel, SNO & snoStringList, LPSTR szLabel) {
    CHAR szSno[128];

    snoStringList = SNO_NONE;
    if (SplitDecoratedLabel(szSnoDecoratedLabel, szSno, szLabel)) {
        snoStringList = SNOFileLookupGUID(STRINGLIST_SNO, szSno);
    }
}

// line 939
CRefString StringListFetch(LPCSTR szSnoDecoratedLabel, BOOL & bSuccess) {
    // The DWARF lists szLabel first, but the original's stack layout (szLabel at sp+8) needs usString
    // declared first.
    CRefString usString;
    CHAR szLabel[1024];
    SNO snoStringList;

    bSuccess = FALSE;
    if (szSnoDecoratedLabel[0] == '\0') {
        return CRefString(uszEmptyString);
    }

    StringListDecodeDecoratedLabel(szSnoDecoratedLabel, snoStringList, szLabel);
    if (snoStringList != SNO_NONE && strlen(szLabel) != 0) {
        return CRefString(sStringListFetchInternal(snoStringList, szLabel, bSuccess));
    }

    usString.Format("%s - %s", uszMissingString, szSnoDecoratedLabel);
    return usString;
}

// line 973
CRefString StringListFetch(LPCSTR szSnoDecoratedLabel) {
    BOOL bSuccess = FALSE;
    return StringListFetch(szSnoDecoratedLabel, bSuccess);
}

// line 983
CRefString StringListFetchByLabel(const SNO snoStringList, const StringLabelHandle hLabel) {
    BOOL fFound = FALSE;
    CRefString sText(sStringListFetchByLabelInternal(snoStringList, hLabel, fFound));

    if (!fFound) {
        return CRefString(uszEmptyString);
    }

    return sText;
}

// line 998
BOOL StringListFetchByLabel(const SNO snoStringList, const StringLabelHandle hLabel, CRefString & usReturn) {
    BOOL fFound = FALSE;
    usReturn = sStringListFetchByLabelInternal(snoStringList, hLabel, fFound);
    return fFound;
}

// line 1040
static void sParseForStringTable(StringListDefinition * ptStringListDef) {
    if (ptStringListDef->dwStringCount == 0)
        return;
    sFreeMapStringTable(ptStringListDef->ptMapStringTable);
    ptStringListDef->ptMapStringTable = sAllocMapStringTable(ptStringListDef->dwStringCount);

    for (uint32 i = 0; i < ptStringListDef->dwStringCount; i++) {
        StringTableEntry * ptTableEntry = &((StringTableEntry *)ptStringListDef->arStrings)[i];
        StringTableEntry * ptExisting;

        if (ptStringListDef->ptMapStringTable->Lookup(ptTableEntry->hLabel, ptExisting)) {
            continue;
        }

        ptStringListDef->ptMapStringTable->SetAt_Tracked(ptTableEntry->hLabel, ptTableEntry);
    }
}

// line 1580
StringLabelHandle StringListGetLabelHandle(LPCSTR szLabel) {
    return sHashLabel(szLabel);
}

// line 1589
// Relaid on DWARF lines 1589-1624: uszEscapePosition (1598) is declared before nIterations (1599); that order
// fixed the loop-test order. nEscapeSequenceLen 1602, nLeftLen 1603, uszRetval 1605 inside the loop.
#line 1589
CRefString StringListEntryReplace(
    LPCSTR uszFormatString,
    LPCSTR uszEscapeSequence,
    LPCSTR uszReplacement,
    int32 nMaxReplacements)
{
    CRefString usRetval(uszFormatString);
    LPCSTR uszWorkingFormatString = usRetval;

    LPCSTR uszEscapePosition = Blizzard::String::FindI(uszWorkingFormatString, uszEscapeSequence);
    int32 nIterations = 0;
    while (uszEscapePosition && nIterations++ < nMaxReplacements)
    {
        size_t nEscapeSequenceLen = strlen(uszEscapeSequence);
        size_t nLeftLen = uszEscapePosition - uszWorkingFormatString;

        CHAR uszRetval[8192];




        strncpy(uszRetval, uszWorkingFormatString, nLeftLen);
        uszRetval[nLeftLen] = '\0';

        strcat_safe(uszRetval, sizeof(uszRetval), uszReplacement);
        strcat_safe(uszRetval, sizeof(uszRetval), uszEscapePosition + nEscapeSequenceLen);

        usRetval = uszRetval;
        uszWorkingFormatString = usRetval;

        uszEscapePosition = Blizzard::String::FindI(uszWorkingFormatString, uszEscapeSequence);
    }


    return usRetval;
}
#line 425

// line 1629
BOOL SplitDecoratedLabel(LPCSTR szDecoratedLabel, LPSTR pszLabelA, LPSTR pszLabelB) {
    pszLabelA[0] = '\0';
    if (pszLabelB) {
        pszLabelB[0] = '\0';
    }

    if (szDecoratedLabel == NULL) {
        return FALSE;
    }

    LPCSTR pszSeparator = strchr(szDecoratedLabel, ':');
    if (pszSeparator == NULL) {
        return FALSE;
    }

    int32 nCount = (int32)(pszSeparator - szDecoratedLabel) + 1;
    Blizzard::String::Copy(pszLabelA, szDecoratedLabel, nCount);
    if (pszLabelB && nCount < (int32)strlen(szDecoratedLabel)) {
        strcpy(pszLabelB, szDecoratedLabel + nCount);
    }

    return TRUE;
}

// line 1668
CRefString LocalizedSprintf(LPCSTR cpFmtStr, ...) {
    CRefString sResult(cpFmtStr);
    int32 i = 1;
    char * szParam;
    va_list tArgList;

    va_start(tArgList, cpFmtStr);
    szParam = va_arg(tArgList, char *);
    while (szParam) {
        CRefString sStringToReplace(CRefString().Format("{s%d}", i));
        sResult = StringListEntryReplace(sResult, sStringToReplace.str(), szParam, 1);
        szParam = va_arg(tArgList, char *);
        i++;
    }
    va_end(tArgList);

    CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(sResult.str()));
    return sAfterLanguageRules;
}

// line 1983
// Inference: early returns, because DWARF puts pSNOs (2018) and nItemCount (2019) at function
// scope, and the original's SigmaMemoryFree has no line row (tails merged from several lines).
void StringListApplyHotfix(const char * szHotfixName) {
    uint8 * pData;
    uint32 nDataSize;
    FileIO::File tFile(1);
    CRefString sHotfixSLOFilename;

    sHotfixSLOFilename.Format("Misc\\Hotfixes\\%s.slo", szHotfixName);
    if (!tFile.Open(sHotfixSLOFilename.str())) {
        return;
    }

    nDataSize = (uint32)tFile.GetFileSize();
    pData = (uint8 *)SigmaMemoryNew(nDataSize, 4, NULL, FALSE);

    if (!tFile.ReadFile(pData, nDataSize, NULL)) {
        SigmaMemoryFree(pData, NULL);
        return;
    }

    if (*(uint32 *)pData != 0xACBF0004) {
        SigmaMemoryFree(pData, NULL);
        return;
    }

    SNO * pSNOs = (SNO *)(pData + sizeof(uint32));
    int32 nItemCount = (nDataSize - sizeof(uint32)) / (2 * sizeof(SNO));
    for (int32 i = 0; i < nItemCount; i++) {
        stStringlistOverrideMap.SetAt_Tracked(pSNOs[i * 2], pSNOs[i * 2 + 1]);
    }

    SigmaMemoryFree(pData, NULL);
}
