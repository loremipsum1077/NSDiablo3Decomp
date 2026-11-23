// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Definitions/ScriptDefinition.h
// Subset: the group definition class and its global, which X/Common/Game.cpp registers
// (GroupDefinition::Create<ScriptGroupDef>). The definition struct itself is not adopted yet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"

#include "../LuaScript/ScriptEventParams.h"
#include "../LuaScript/ScriptEventSwig.h"
typedef int32 ScriptEventType;
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Common/SNO/SNOName.h"
struct ScriptEventVariantParamString_Console {  // size 0x10
    SerializeData serString;  // 0x00
    XPtr64<char> szString;  // 0x08
};
static_assert(sizeof(ScriptEventVariantParamString_Console) == 0x10, "ScriptEventVariantParamString_Console");

struct ScriptMessageMap {  // size 0xc0
    ScriptMessageMap();
    ScriptMessageMap(const ScriptMessageMap &);
    ScriptEventType eEventType;  // 0x00
    uint32 dwUniqueID;  // 0x04
    ScriptEventVariantParamLW arMessageFilters[4];  // 0x08
    ScriptEventVariantParamString_Console arMessageFilters_Strings[4];  // 0x68
    SerializeData serHandlerFunction_Console;  // 0xa8
    XPtr64<char> szHandlerFunction;  // 0xb0
    uint32 dwListenerFlags;  // 0xb8
    uint32 nEventCount;  // 0xbc
    void PostProcess();
};
static_assert(sizeof(ScriptMessageMap) == 0xc0, "ScriptMessageMap");

struct ScriptDefinition {  // size 0x48
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwMachineTime;  // 0x0c
    SerializeData serCompiledData;  // 0x10
    XPtr64<unsigned char> pCompiledData;  // 0x18
    SerializeData serMessageMap;  // 0x20
    XPtr64<ScriptMessageMap> arScriptMessageMap;  // 0x28
    int32 nMessageMapCount;  // 0x30
    uint32 dwScriptUID;  // 0x34
    SNO snoQuestMetaData;  // 0x38
    SNOName snoNameDynamic;  // 0x3c
};
static_assert(sizeof(ScriptDefinition) == 0x48, "ScriptDefinition");

class XMemoryAllocatorInterface;

class ScriptGroupDef : public GroupDefinition {  // size 0xa0
public:
    ScriptGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 132
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_SERVERDATA; }  // vtable slot 7, line 134
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual uint32 GetPrecacheDependencyCount();  // vtable slot 8
};
static_assert(sizeof(ScriptGroupDef) == 0xa0, "ScriptGroupDef");

extern ScriptGroupDef * gptGroupDefScript;  // ScriptDefinition.cpp

typedef ScopedDefinitionLockR<ScriptDefinition> ScopedScriptDefinitionLockR;
template <> inline const ScriptDefinition * DefinitionLockR<ScriptDefinition>(SNO sno) { return (const ScriptDefinition *)gptGroupDefScript->LockR(sno, FALSE); }
template <> inline void DefinitionUnlockR<ScriptDefinition>(const ScriptDefinition * pdef) { gptGroupDefScript->UnlockR((void *&)pdef); }
