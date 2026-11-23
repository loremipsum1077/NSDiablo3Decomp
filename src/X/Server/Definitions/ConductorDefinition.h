// Adopted from the DWARF skeleton (build/dwarf/headers/X/Server/Definitions/ConductorDefinition.h);
// hand-maintained from here on. Original file: X/Server/Definitions/ConductorDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
class XMemoryAllocatorInterface;
// Used only through XPtr64<ScriptMessageMap> (a pointer); the owning header is not adopted.
struct ScriptMessageMap;

class CRefString;  // returned by the ConductorGet*Label helpers below

struct ConductorSequence {  // size 0x20
    BOOL bRepeatable;  // 0x00
    XPtr64<ScriptMessageMap> arScriptMessageMap;  // 0x08
    SerializeData serMessageMap;  // 0x10
    int32 nMessageMapCount;  // 0x18
};
static_assert(sizeof(ConductorSequence) == 0x20, "ConductorSequence");

struct ConductorPhase {  // size 0x20
    uint32 dwPhaseHash;  // 0x00
    uint32 dwPhaseHashPadding;  // 0x04
    XPtr64<ConductorSequence> arSequences;  // 0x08
    SerializeData serSequences;  // 0x10
    uint32 nNumSequences;  // 0x18
};
static_assert(sizeof(ConductorPhase) == 0x20, "ConductorPhase");

struct ConductorDefinition {  // size 0x48
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 nNumPhases;  // 0x0c
    BOOL bRunOnlyOnce;  // 0x10
    XPtr64<ConductorPhase> arPhases;  // 0x18
    SerializeData serPhases;  // 0x20
    uint32 dwScriptMachineTime;  // 0x28
    XPtr64<unsigned char> pCompiledScript;  // 0x30
    SerializeData serCompiledScript;  // 0x38
    SNO snoScript;  // 0x40
};
static_assert(sizeof(ConductorDefinition) == 0x48, "ConductorDefinition");

class ConductorGroupDef : public GroupDefinition {  // size 0xa0
public:
    ConductorGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5, line 70; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 73
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_SERVERDATA; }  // vtable slot 7, line 75
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
};
static_assert(sizeof(ConductorGroupDef) == 0xa0, "ConductorGroupDef");

extern ConductorGroupDef * gptGroupDefConductor;  // ConductorDefinition.cpp

typedef ScopedDefinitionLockR<ConductorDefinition> ScopedConductorDefinitionLockR;

// Declared here because their definitions live in X/Server/Definitions/ConductorDefinition.cpp
// and LuaScriptDebug.cpp relocates against them.
BOOL ConductorGetPhaseFromInitFunctionName(LPCSTR szFunctionName, int32 & nPhaseIndex);
BOOL ConductorGetPhaseAndSequenceFromStartFunctionName(LPCSTR szFunctionName, int32 & nPhaseIndex, int32 & nSequenceIndex);
CRefString ConductorGetPhaseLabel(SNO snoConductor, uint32 nPhaseIndex);
CRefString ConductorGetSequenceLabel(SNO snoConductor, uint32 nPhaseIndex, uint32 nSequenceIndex);

// Functions defined in this file (bodies are not in the DWARF):
//   line    65  ConductorGroupDef::~ConductorGroupDef()  [0x71007d6e50]
//   line    70  void * ConductorGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x71007d6e80 (+1 more)]
//   line    70  void ConductorGroupDef::MemFree(void *& ptr) const  [0x71007d6ed0 (+1 more)]
//   line    73  BOOL ConductorGroupDef::CanReferenceOtherSNOs() const  [0x71007d6f60]
//   line    75  SNOPackType ConductorGroupDef::GetSNOPackDestination(const void *) const  [0x71007d6f50]
//   line   137  const ConductorDefinition * DefinitionLockR<ConductorDefinition>(SNO sno)  [inlined into 3 functions]
//   line   138  void DefinitionUnlockR<ConductorDefinition>(const ConductorDefinition * pdef)  [inlined into 3 functions]

CRefString ConductorGetSequenceStartFunctionName(int32 nPhaseIndex, int32 nSequenceIndex);  // ConductorDefinition.cpp:575

// ConductorDefinition.h:137-138, original scoped readers use LockR(FALSE).
template <> inline const ConductorDefinition * DefinitionLockR<ConductorDefinition>(SNO sno) { return (const ConductorDefinition *)gptGroupDefConductor->LockR(sno, FALSE); }
template <> inline void DefinitionUnlockR<ConductorDefinition>(const ConductorDefinition * pdef) { gptGroupDefConductor->UnlockR((void *&)pdef); }
