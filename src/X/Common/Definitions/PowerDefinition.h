// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/PowerDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../Powers/PowersStruct.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;

struct ScriptFormulaDetails {  // size 0x308
    ScriptFormulaDetails();
    CHAR szFormulaName[256];  // 0x00
    CHAR szNote[512];  // 0x100
    uint32 uRunes;  // 0x300
    int32 nCustomSortIndex;  // 0x304
};
static_assert(sizeof(ScriptFormulaDetails) == 0x308, "ScriptFormulaDetails");

struct PowerDefinition {  // size 0x310
    RequiredDefinitionHeader tHeader;  // 0x00
    PowerDef tPower;  // 0x10
    BOOL bScripted;  // 0x2e8
    BOOL bUseSecondaryResource;  // 0x2ec
    int32 nFormulaCount;  // 0x2f0
    XPtr64<unsigned char> pCompiledScript;  // 0x2f8
    SerializeData serCompiledScript;  // 0x300
    SNO snoQuestMetaData;  // 0x308
    SNO snoScript;  // 0x30c
};
static_assert(sizeof(PowerDefinition) == 0x310, "PowerDefinition");

class PowerGroupDef : public GroupDefinition {  // size 0xa0
public:
    PowerGroupDef();
    virtual ~PowerGroupDef() {}  // vtable slot 0, line 80
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 85; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 88
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 90
        return SNOPACKTYPE_SERVERDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual BOOL CacheManagerCanPurgeElement(const DefDataElement & tDefElement, BOOL fIgnoreDoNotPurgeBit) const;  // vtable slot 29
};
static_assert(sizeof(PowerGroupDef) == 0xa0, "PowerGroupDef");

typedef ScopedDefinitionLockR<PowerDefinition> ScopedPowerDefinitionLockR;

typedef AutoDefinitionR<PowerDefinition> AutoPowerDefinitionR;

extern const XTypeDescriptor * DT_POWER_DEFINITION;  // PowerDefinition.cpp:98
extern PowerGroupDef * gptGroupDefPower;  // PowerDefinition.cpp:104

// Lines 161-162. The lock helpers every ScopedPowerDefinitionLockR and
// AutoPowerDefinitionR goes through, explicitly specialized for PowerDefinition
// (Definitions.h only declares the primaries; each definition type specializes
// them in its own header, see StringListDefinition.h:65-70). _Z15DefinitionLockRI15PowerDefinitionEPKT_i
// and _Z17DefinitionUnlockRI15PowerDefinitionEvPKT_, both inlined into callers.
template <> inline const PowerDefinition * DefinitionLockR<PowerDefinition>(SNO sno) {
    return (const PowerDefinition *)gptGroupDefPower->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<PowerDefinition>(const PowerDefinition * _pdef) {
    gptGroupDefPower->UnlockR((void *&)_pdef);
}

// Lines 170-171. Bodies written from the copies inlined into UISkills::Console::GetSkillList:
// a plain LockR that may not fail, and an UnlockR through a local copy of the pointer.
template <> inline const PowerDefinition * AutoDefinitionR<PowerDefinition>::AutoDefinitionLockR(SNO sno) {
    return (const PowerDefinition *)gptGroupDefPower->LockR(sno, FALSE);
}
template <> inline void AutoDefinitionR<PowerDefinition>::AutoDefinitionUnlockR(const PowerDefinition * _pdef) {
    void * pdef = (void *)_pdef;
    gptGroupDefPower->UnlockR(pdef);
}

BOOL PowerIsContactFrameEnabled(const PowerDefinition * pdefPower, int32 nContactFrame);


// Functions defined in this file (bodies are not in the DWARF):
//   line    80  PowerGroupDef::~PowerGroupDef()  [0x710063cd30]
//   line    85  void * PowerGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x710063cd60 (+1 more)]
//   line    85  void PowerGroupDef::MemFree(void *& ptr) const  [0x710063cdb0 (+1 more)]
//   line    88  BOOL PowerGroupDef::CanReferenceOtherSNOs() const  [0x710063ce40]
//   line    90  SNOPackType PowerGroupDef::GetSNOPackDestination(const void *) const  [0x710063ce30]
//   line   161  const PowerDefinition * DefinitionLockR<PowerDefinition>(SNO sno)  [inlined into 70 functions]
//   line   162  void DefinitionUnlockR<PowerDefinition>(const PowerDefinition * _pdef)  [inlined into 70 functions]
//   line   170  const PowerDefinition * AutoDefinitionR<PowerDefinition>::AutoDefinitionLockR(SNO sno)  [inlined into 292 functions]
//   line   171  void AutoDefinitionR<PowerDefinition>::AutoDefinitionUnlockR(const PowerDefinition * _pdef)  [inlined into 329 functions]
