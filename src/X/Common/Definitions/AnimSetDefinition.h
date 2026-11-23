// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/AnimSetDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class CTagMap;  // X/Common/TagMaps/Tags.h; only held through XPtr64 here
class XMemoryAllocatorInterface;

struct AnimSetTagMap {  // size 0x10
    SerializeData serTagMap;  // 0x00
    XPtr64<CTagMap> hTagMap;  // 0x08
};
static_assert(sizeof(AnimSetTagMap) == 0x10, "AnimSetTagMap");

struct AnimSetDefinition {  // size 0x1e0
    RequiredDefinitionHeader tHeader;  // 0x00
    SNO snoParentAnimSet;  // 0x0c
    SerializeData serTagMapAnimDefault;  // 0x10
    XPtr64<CTagMap> hTagMapAnimDefault;  // 0x18
    AnimSetTagMap tWeaponAnimTagMap[28];  // 0x20
};
static_assert(sizeof(AnimSetDefinition) == 0x1e0, "AnimSetDefinition");

class AnimSetGroupDef : public GroupDefinition {  // size 0xa0, line 48
public:
    AnimSetGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 53; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 56
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 58
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(AnimSetGroupDef) == 0xa0, "AnimSetGroupDef");

extern AnimSetGroupDef * gptGroupDefAnimSet;  // AnimSetDefinition.cpp:51

typedef ScopedDefinitionLockR<AnimSetDefinition> ScopedAnimSetDefinitionLockR;

template <> inline const AnimSetDefinition * DefinitionLockR<AnimSetDefinition>(SNO sno) {  // line 79
    return (const AnimSetDefinition *)gptGroupDefAnimSet->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<AnimSetDefinition>(const AnimSetDefinition * pdef) {  // line 80
    void * lpDef = (void *)pdef;
    gptGroupDefAnimSet->UnlockR(lpDef);
}


// Functions defined in this file (bodies are not in the DWARF):
//   line    48  AnimSetGroupDef::~AnimSetGroupDef()  [0x710060c900]
//   line    53  void * AnimSetGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x710060c930 (+1 more)]
//   line    53  void AnimSetGroupDef::MemFree(void *& ptr) const  [0x710060c980 (+1 more)]
//   line    56  BOOL AnimSetGroupDef::CanReferenceOtherSNOs() const  [0x710060ca10]
//   line    58  SNOPackType AnimSetGroupDef::GetSNOPackDestination(const void *) const  [0x710060ca00]
//   line    79  const AnimSetDefinition * DefinitionLockR<AnimSetDefinition>(SNO sno)  [inlined into 6 functions]
//   line    80  void DefinitionUnlockR<AnimSetDefinition>(const AnimSetDefinition * pdef)  [inlined into 6 functions]
