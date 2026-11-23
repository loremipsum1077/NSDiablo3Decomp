// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/StringListDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
class XMemoryAllocatorInterface;
class XTypeDescriptor;

typedef uint32 StringLabelHandle;

struct StringTableEntry {  // size 0x18
    XPtr64<char> szText;  // 0x00
    SerializeData serText;  // 0x08
    StringLabelHandle hLabel;  // 0x10
    uint32 dwPad;  // 0x14
};
static_assert(sizeof(StringTableEntry) == 0x18, "StringTableEntry");

typedef XGrowableMap<unsigned int, unsigned int, StringTableEntry *, StringTableEntry *> StringTableMap;

struct StringListDefinition {  // size 0x28
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwStringCount;  // 0x0c
    XPtr64<StringTableEntry> arStrings;  // 0x10
    SerializeData serStrings;  // 0x18
    StringTableMap * ptMapStringTable;  // 0x20
};
static_assert(sizeof(StringListDefinition) == 0x28, "StringListDefinition");

class StringListGroupDef : public GroupDefinition {  // size 0xa0, line 86
public:
    StringListGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 91; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 94
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 96
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void PurgeDefinitionAndSNO(const SNO sno);  // vtable slot 25
    virtual BOOL AllowMultipleCheckout(SNO snoHandle);  // vtable slot 28
};
static_assert(sizeof(StringListGroupDef) == 0xa0, "StringListGroupDef");

extern const XTypeDescriptor * DT_STRING_TABLE_ENTRY;  // StringListDefinition.cpp:88
extern const XTypeDescriptor * DT_STRINGLIST;  // StringListDefinition.cpp:92
extern StringListGroupDef * gptGroupDefStringList;  // StringListDefinition.cpp:100

// Lines 142 and 143. The read lock reports failure (LockR with fLockCanFail FALSE) and the unlock
// is a real call.
template <> inline const StringListDefinition * DefinitionLockR<StringListDefinition>(SNO sno) {
    return (const StringListDefinition *)gptGroupDefStringList->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<StringListDefinition>(const StringListDefinition * pdef) {
    gptGroupDefStringList->UnlockR((void *&)pdef);
}

typedef ScopedDefinitionLockR<StringListDefinition> ScopedStringListDefinitionLockR;

StringTableEntry * StringListGetEntry(const StringListDefinition * pdefStringList, LPCSTR szLabel);
CRefString StringListFetch(const SNO snoStringList, LPCSTR szLabel);
LPCSTR StringListFetchLPCSTR(const SNO snoStringList, LPCSTR szLabel);
BOOL StringListFetch(const SNO snoStringList, LPCSTR szLabel, CRefString & usReturnString);
CRefString StringListFetch(LPCSTR szSnoDecoratedLabel, BOOL & bSuccess);
CRefString StringListFetch(LPCSTR szSnoDecoratedLabel);
CRefString StringListFetchByLabel(const SNO snoStringList, const StringLabelHandle hLabel);
BOOL StringListFetchByLabel(const SNO snoStringList, const StringLabelHandle hLabel, CRefString & usReturn);
StringLabelHandle StringListGetLabelHandle(LPCSTR szLabel);
CRefString StringListEntryReplace(LPCSTR uszFormatString, LPCSTR uszEscapeSequence, LPCSTR uszReplacement,
                                  int32 nMaxReplacements);
BOOL SplitDecoratedLabel(LPCSTR szDecoratedLabel, LPSTR pszLabelA, LPSTR pszLabelB);
CRefString LocalizedSprintf(LPCSTR cpFmtStr, ...);
void StringListApplyHotfix(const char * szHotfixName);


// Functions defined in this file (bodies are not in the DWARF):
//   line    86  StringListGroupDef::~StringListGroupDef()  [0x7100643ba0]
//   line    91  void * StringListGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x7100643bd0 (+1 more)]
//   line    91  void StringListGroupDef::MemFree(void *& ptr) const  [0x7100643c20 (+1 more)]
//   line    94  BOOL StringListGroupDef::CanReferenceOtherSNOs() const  [0x7100643cb0]
//   line    96  SNOPackType StringListGroupDef::GetSNOPackDestination(const void *) const  [0x7100643ca0]
//   line   142  const StringListDefinition * DefinitionLockR<StringListDefinition>(SNO sno)  [inlined into 14 functions]
//   line   143  void DefinitionUnlockR<StringListDefinition>(const StringListDefinition * pdef)  [inlined into 14 functions]
