// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/ActDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "StringListDefinition.h"
#include "../ImageHandle.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;

enum {
    MAX_ENTRANCE_NUMBER = 128,
    MAX_MAIN_QUESTS = 64,
    MAX_START_LOCATION_OVERRIDES = 6,
};

struct ResolvedPortalDestination {  // size 0xc
    SNO snoWorld;  // 0x00
    uint32 uEntranceGUID;  // 0x04
    SNO snoDestLevelArea;  // 0x08
    // Line 39. Body written from the copy inlined into Minimap::GetMarkerNameFromPortal.
    ResolvedPortalDestination() {
        snoWorld = SNO_NONE;
        uEntranceGUID = (uint32)-1;
        snoDestLevelArea = SNO_NONE;
    }
    BOOL IsValid() const;
};
static_assert(sizeof(ResolvedPortalDestination) == 0xc, "ResolvedPortalDestination");

// Line 47; written from the copy inlined into ActorCommonData::SendPortalSpecifier.
inline BOOL ResolvedPortalDestination::IsValid() const {
    return snoWorld != SNO_NONE && uEntranceGUID != (uint32)-1;
}

enum WaypointFlags {
    WAYPOINT_USABLE_IN_STORY_MODE_BIT = 0,
    WAYPOINT_USABLE_IN_ADVENTURE_MODE_BIT = 1,
};

struct WaypointInfo {  // size 0x28
    uint32 dwFlags;  // 0x00
    SNO snoWorld;  // 0x04
    SNO snoLevelArea;  // 0x08
    ImageHandle hWaypointButtonNormal;  // 0x0c
    ImageHandle hWaypointButtonMouseOver;  // 0x10
    ImageHandle hWaypointButtonPushed;  // 0x14
    SNO snoQuestRange;  // 0x18
    StringLabelHandle hZoneLabel;  // 0x1c
    IVector2D pointMapLocation;  // 0x20
    static void GetDefaultData(WaypointInfo & tInfo);
};
static_assert(sizeof(WaypointInfo) == 0x28, "WaypointInfo");

struct ActQuestInfo {  // size 0x4
    SNO snoQuest;  // 0x00
};
static_assert(sizeof(ActQuestInfo) == 0x4, "ActQuestInfo");

struct ActStartLocOverride {  // size 0x18
    ResolvedPortalDestination tStartLocation;  // 0x00
    SNO snoQuestRange;  // 0x0c
    SNO snoHubWorld;  // 0x10
    BOOL bDisallowReturnToTown;  // 0x14
};
static_assert(sizeof(ActStartLocOverride) == 0x18, "ActStartLocOverride");

struct ActDefinition {  // size 0x1060
    RequiredDefinitionHeader tHeader;  // 0x00
    SerializeData serQuests;  // 0x0c
    XPtr64<ActQuestInfo> arQuests;  // 0x18
    WaypointInfo aWaypointInfo[100];  // 0x20
    ResolvedPortalDestination tStartLocDefault;  // 0xfc0
    ActStartLocOverride aStartLocOverride[MAX_START_LOCATION_OVERRIDES];  // 0xfcc
};
static_assert(sizeof(ActDefinition) == 0x1060, "ActDefinition");

class ActGroupDef : public GroupDefinition {  // size 0xa0, line 106
public:
    ActGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 111; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 114
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 116
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(ActGroupDef) == 0xa0, "ActGroupDef");

extern const XTypeDescriptor * DT_RESOLVED_PORTAL_DESTINATION;  // ActDefinition.cpp:47
extern const XTypeDescriptor * DT_WAYPOINTINFO;  // ActDefinition.cpp:59
extern const XTypeDescriptor * DT_ACT_QUEST_INFO;  // ActDefinition.cpp:63
extern const XTypeDescriptor * DT_ACT_START_LOC_OVERRIDE;  // ActDefinition.cpp:70
extern const XTypeDescriptor * DT_ACT_DEFINITION;  // ActDefinition.cpp:77
extern ActGroupDef * gptGroupDefAct;  // ActDefinition.cpp:82

typedef ScopedDefinitionLockR<ActDefinition> ScopedActDefinitionLockR;

// Line 163; the fast immutable definition lock inlined into
// UIGameSettings::Console::IsQuestSelectable and DetermineActCompletionProgress.
template <> inline const ActDefinition * DefinitionLockR<ActDefinition>(SNO sno) {
    return (const ActDefinition *)gptGroupDefAct->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<ActDefinition>(const ActDefinition *) {}
