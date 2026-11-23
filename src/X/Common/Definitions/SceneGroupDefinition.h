// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/SceneGroupDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../GameBalance.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;
class XTypeDescriptor;

struct SceneGroupItem {  // size 0xc
    SNO snoScene;  // 0x00
    int32 nWeight;  // 0x04
    GBID gbidLabel;  // 0x08
};
static_assert(sizeof(SceneGroupItem) == 0xc, "SceneGroupItem");

struct SceneGroupDefinition {  // size 0x28
    RequiredDefinitionHeader tHeader;  // 0x00
    int32 nSceneCnt;  // 0x0c
    SerializeData serScenes;  // 0x10
    XPtr64<SceneGroupItem> arScenes;  // 0x18
    uint32 dwFlags;  // 0x20
};
static_assert(sizeof(SceneGroupDefinition) == 0x28, "SceneGroupDefinition");

class SceneGroupGroupDef : public GroupDefinition {  // size 0xa0, line 48
public:
    SceneGroupGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 53; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 56
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 58
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(SceneGroupGroupDef) == 0xa0, "SceneGroupGroupDef");

extern const XTypeDescriptor * DT_SCENEGROUPITEM;  // SceneGroupDefinition.cpp:38
extern const XTypeDescriptor * DT_SCENEGROUP;  // SceneGroupDefinition.cpp:44
extern SceneGroupGroupDef * gptGroupDefSceneGroup;  // SceneGroupDefinition.cpp:50

typedef ScopedDefinitionLockR<SceneGroupDefinition> ScopedSceneGroupDefinitionLockR;

// Line 86. As for FlagSetDefinition.h, the unlock is taken to be empty.
template <> inline const SceneGroupDefinition * DefinitionLockR<SceneGroupDefinition>(SNO sno) {
    return (const SceneGroupDefinition *)gptGroupDefSceneGroup->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<SceneGroupDefinition>(const SceneGroupDefinition *) {}

// Functions defined in this file (bodies are not in the DWARF):
//   line    48  SceneGroupGroupDef::~SceneGroupGroupDef()  [0x7100641bd0]
//   line    53  void * SceneGroupGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x7100641c00 (+1 more)]
//   line    53  void SceneGroupGroupDef::MemFree(void *& ptr) const  [0x7100641c50 (+1 more)]
//   line    56  BOOL SceneGroupGroupDef::CanReferenceOtherSNOs() const  [0x7100641ce0]
//   line    58  SNOPackType SceneGroupGroupDef::GetSNOPackDestination(const void *) const  [0x7100641cd0]
//   line    86  const SceneGroupDefinition * DefinitionLockR<SceneGroupDefinition>(SNO sno)  [inlined into 9 functions]
