// X/Client/EffectGroup.h
// Subset: only the entry point X/Client/CMarker.cpp calls. The DWARF does not record where
// EffectGroupAttachmentAdd was declared; this is its owning file's header.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"

struct TriggerEvent;

typedef int32 ParentType;

// X/Client/EffectGroup.cpp:1152
AttachmentID EffectGroupAttachmentAdd(const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType, const TriggerEvent & tEffectEvent);

enum {
    EFFECTGROUPID_INVALID = -1,
};

typedef int32 EffectGroupID;

// X/Client/EffectGroup.cpp:1368
void EffectGroupDelete(const EffectGroupID idEffectGroup);

typedef int32 RopePosition;

struct EffectGroup {  // size 0xc
    EffectGroupID id;  // 0x00
    RActorID idRActorOwner;  // 0x04
    uint32 dwTrackingID;  // 0x08
};
static_assert(sizeof(EffectGroup) == 0xc, "EffectGroup");

// X/Client/EffectGroup.cpp:83, 91, 100, 122, 1399 (the Create functions take an SNO, which this
// header does not have in scope; they are declared where they are used)
EffectGroup & EffectGroupGet(const EffectGroupID idEffectGroup);
EffectGroup * EffectGroupTryToGet(const EffectGroupID idEffectGroup);
void EffectGroupInitializeForGame();
void EffectGroupDisposeForGame();
void EffectGroupOutro(const EffectGroupID idEffectGroup);
