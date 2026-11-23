// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/EffectGroupDefinition.h
// Subset: the definition structs, the group definition class and its global.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/AttachmentDefinition.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"
#include "../../../X/Sigma/DataStructures/XPtr64.h"
#include "../../../X/Sigma/Files/XSerializeData.h"

class XMemoryAllocatorInterface;

enum EffectWeightMethod {
    EFFECT_WEIGHT_PICK_ONE = 0,
    EFFECT_WEIGHT_PICK_SOME = 1,
    EFFECT_WEIGHT_PICK_ALL = 2,
    EFFECT_WEIGHT_BY_RESOURCE_LEVEL = 3,
    EFFECT_WEIGHT_BY_TEAM = 5,
    EFFECT_WEIGHT_BY_RARITY = 7,
    EFFECT_WEIGHT_BY_SHADER_HW_CLASS = 8,
    EFFECT_WEIGHT_BY_WEAPON_DAMAGE = 9,
    EFFECT_WEIGHT_BY_LOOK = 10,
    EFFECT_WEIGHT_BY_WEAPON_DAMAGE_TYPE = 11,
    EFFECT_WEIGHT_BY_TOPOLOGY = 12,
    EFFECT_WEIGHT_BY_GENDER = 13,
    EFFECT_WEIGHT_BY_FRIENDLY_STATUS = 14,
    EFFECT_WEIGHT_BY_RUNE = 15,
    EFFECT_WEIGHT_BY_CLASS = 16,
    EFFECT_WEIGHT_BY_GAMETYPE = 17,
};

struct EffectItem {  // size 0xc8
    uint32 dwWeight;  // 0x00
    uint32 dwHash;  // 0x04
    MsgTriggeredEvent tMsgTriggeredEvents;  // 0x08
};
static_assert(sizeof(EffectItem) == 0xc8, "EffectItem");

struct EffectGroupDefinition {  // size 0x78
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwEffectFlags;  // 0x0c
    SerializeData serEffects;  // 0x10
    int32 nEffectCount;  // 0x18
    XPtr64<EffectItem> ptEffects;  // 0x20
    int32 nMinToPick;  // 0x28
    int32 nMaxToPick;  // 0x2c
    EffectWeightMethod nEffectWeightMethod;  // 0x30
    SNO snoPower;  // 0x34
    uint32 fPlayedBit[16];  // 0x38
};
static_assert(sizeof(EffectGroupDefinition) == 0x78, "EffectGroupDefinition");

class EffectGroupGroupDef : public GroupDefinition {  // size 0xa0
public:
    EffectGroupGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 102
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_CLIENTDATA; }  // vtable slot 7, line 104
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(EffectGroupGroupDef) == 0xa0, "EffectGroupGroupDef");

extern EffectGroupGroupDef * gptGroupDefEffectGroup;  // EffectGroupDefinition.cpp

typedef ScopedDefinitionLockR<EffectGroupDefinition> ScopedEffectGroupDefinitionLockR;

template <> inline const EffectGroupDefinition * DefinitionLockR<EffectGroupDefinition>(SNO sno) {  // line 134
    return (const EffectGroupDefinition *)gptGroupDefEffectGroup->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<EffectGroupDefinition>(const EffectGroupDefinition * pdef) {  // line 135
    void * lpDef = (void *)pdef;
    gptGroupDefEffectGroup->UnlockR(lpDef);
}
