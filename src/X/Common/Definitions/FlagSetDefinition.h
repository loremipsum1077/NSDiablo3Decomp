// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/FlagSetDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
class XMemoryAllocatorInterface;

enum {
    FLAGSET_MAX_LABEL = 128,
    FLAGSET_MAX_DESCRIPTION = 256,
};

struct FlagLookup {  // size 0x184
    int32 nIndex;  // 0x00
    CHAR szLabel[128];  // 0x04
    CHAR szDescription[256];  // 0x84
};
static_assert(sizeof(FlagLookup) == 0x184, "FlagLookup");

struct FlagSetDefinition {  // size 0x20
    RequiredDefinitionHeader tHeader;  // 0x00
    XPtr64<FlagLookup> arFlagLookups;  // 0x10
    SerializeData serFlagLookups;  // 0x18
};
static_assert(sizeof(FlagSetDefinition) == 0x20, "FlagSetDefinition");

class FlagSetGroupDef : public GroupDefinition {  // size 0xa0, line 39
public:
    FlagSetGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 44; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 47
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 49
        return SNOPACKTYPE_SERVERDATA;
    }
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
};
static_assert(sizeof(FlagSetGroupDef) == 0xa0, "FlagSetGroupDef");

extern FlagSetGroupDef * gptGroupDefFlagSet;  // FlagSetDefinition.cpp:33

typedef ScopedDefinitionLockR<FlagSetDefinition> ScopedFlagSetDefinitionLockR;

// Line 77. Its one caller (GameCommonData::Initialize) calls LockRFast and has nothing from an
// unlock, so DefinitionUnlockR<FlagSetDefinition> is taken to be empty; where and how the
// original spells that empty unlock is not in the DWARF.
template <> inline const FlagSetDefinition * DefinitionLockR<FlagSetDefinition>(SNO sno) {
    return (const FlagSetDefinition *)gptGroupDefFlagSet->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<FlagSetDefinition>(const FlagSetDefinition *) {}

int32 FlagSetLookup(SNO snoFlagSet, LPCSTR szFlagLabel);
BOOL FlagSetGetFlag(const uint8 * arFlags, const int32 nBitArraySize, const int32 nFlagIndex);
void FlagSetSetFlag(uint8 * arFlags, const int32 nBitArraySize, const int32 nFlagIndex, const BOOL bValue);

// Functions defined in this file (bodies are not in the DWARF):
//   line    39  FlagSetGroupDef::~FlagSetGroupDef()  [0x710061c150]
//   line    44  void * FlagSetGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x710061c180 (+1 more)]
//   line    44  void FlagSetGroupDef::MemFree(void *& ptr) const  [0x710061c1d0 (+1 more)]
//   line    47  BOOL FlagSetGroupDef::CanReferenceOtherSNOs() const  [0x710061c260]
//   line    49  SNOPackType FlagSetGroupDef::GetSNOPackDestination(const void *) const  [0x710061c250]
//   line    77  const FlagSetDefinition * DefinitionLockR<FlagSetDefinition>(SNO sno)  [inlined into 1 functions]
