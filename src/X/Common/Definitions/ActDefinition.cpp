// Original file: X/Common/Definitions/ActDefinition.cpp

#include "ActDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;

enum {
    ACT_COMMON_SNO_VERSION = 42,
    ACT_DEFINITION_VERSION = 0,
    ACT_PC_SNO_VERSION = 42,
    ACT_CONSOLE_SNO_VERSION = 45,
    ACT_AXE_SNO_VERSION = 44,
    ACT_START_LOC_OVERRIDE_VERSION = 1,
};

// line 36. Only the value 1 is recorded (the WaypointInfo dwFlags field default and the
// GetDefaultData store); how the original spelled it is not.
static const int32 scnDefaultWaypointFlags = (1 << WAYPOINT_USABLE_IN_STORY_MODE_BIT);

// The field names are "" in the binary.
static const TypeDescriptorField ResolvedPortalDestination_Fields[4] = {  // line 43
    FilterTypeDescriptorField("", DT_SNO, offsetof(ResolvedPortalDestination, snoWorld), "", WORLD_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(ResolvedPortalDestination, uEntranceGUID), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ResolvedPortalDestination, snoDestLevelArea), "", LEVELAREA_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ResolvedPortalDestination), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ResolvedPortalDestination("ResolvedPortalDestination", 0, ResolvedPortalDestination_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 47
const XTypeDescriptor * DT_RESOLVED_PORTAL_DESTINATION = &TypeDescriptor_ResolvedPortalDestination;

static const TypeDescriptorField WaypointInfo_Fields[10] = {  // line 49
    TypeDescriptorField("", DT_INT, offsetof(WaypointInfo, dwFlags), (const void *)scnDefaultWaypointFlags, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WaypointInfo, snoWorld), "", WORLD_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WaypointInfo, snoLevelArea), "", LEVELAREA_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(WaypointInfo, hWaypointButtonNormal), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WaypointInfo, hWaypointButtonMouseOver), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WaypointInfo, hWaypointButtonPushed), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WaypointInfo, snoQuestRange), "", QUESTRANGE_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(WaypointInfo, hZoneLabel), (const void *)-1, 1),
    TypeDescriptorField("", DT_IVECTOR2D, offsetof(WaypointInfo, pointMapLocation), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WaypointInfo), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WaypointInfo("WaypointInfo", 0, WaypointInfo_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 59
const XTypeDescriptor * DT_WAYPOINTINFO = &TypeDescriptor_WaypointInfo;

static const TypeDescriptorField ActQuestInfo_Fields[2] = {  // line 61
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActQuestInfo, snoQuest), "", QUEST_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActQuestInfo), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActQuestInfo("ActQuestInfo", 0, ActQuestInfo_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 63
const XTypeDescriptor * DT_ACT_QUEST_INFO = &TypeDescriptor_ActQuestInfo;

static const TypeDescriptorField ActStartLocOverride_Fields[5] = {  // line 65
    TypeDescriptorField("", DT_RESOLVED_PORTAL_DESTINATION, offsetof(ActStartLocOverride, tStartLocation), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActStartLocOverride, snoQuestRange), "", QUESTRANGE_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActStartLocOverride, snoHubWorld), "", WORLD_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(ActStartLocOverride, bDisallowReturnToTown), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActStartLocOverride), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActStartLocOverride("ActStartLocOverride", ACT_START_LOC_OVERRIDE_VERSION, ActStartLocOverride_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 70
const XTypeDescriptor * DT_ACT_START_LOC_OVERRIDE = &TypeDescriptor_ActStartLocOverride;

static const TypeDescriptorField ActDefinition_Fields[6] = {  // line 72
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ActDefinition, serQuests), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ACT_QUEST_INFO, offsetof(ActDefinition, arQuests), 0x21,
                                     (int32)offsetof(ActDefinition, serQuests) - (int32)offsetof(ActDefinition, arQuests)),
    FixedArrayTypeDescriptorField("", DT_WAYPOINTINFO, offsetof(ActDefinition, aWaypointInfo), NULL, 1, 100, 0),
    TypeDescriptorField("", DT_RESOLVED_PORTAL_DESTINATION, offsetof(ActDefinition, tStartLocDefault), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_ACT_START_LOC_OVERRIDE, offsetof(ActDefinition, aStartLocOverride), NULL, 1,
                                  MAX_START_LOCATION_OVERRIDES, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActDefinition("Act", ACT_DEFINITION_VERSION, ActDefinition_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 77
const XTypeDescriptor * DT_ACT_DEFINITION = &TypeDescriptor_ActDefinition;

ActGroupDef * gptGroupDefAct;  // line 82

// line 87
ActGroupDef::ActGroupDef()
    : GroupDefinition("Act", ACT_SNO,
                      SNOVersions(ACT_PC_SNO_VERSION, 46, ACT_CONSOLE_SNO_VERSION, ACT_CONSOLE_SNO_VERSION, 46),
                      SNODATATYPE_TITLE, sizeof(ActDefinition), DT_ACT_DEFINITION, 0x4005, 14) {
}

// line 107
uint32 ActGroupDef::GetMaxCount() {
    return 6;
}

// line 115
void WaypointInfo::GetDefaultData(WaypointInfo & tInfo) {
    tInfo.dwFlags = scnDefaultWaypointFlags;
    tInfo.snoWorld = SNO_NONE;
    tInfo.snoLevelArea = SNO_NONE;
    tInfo.snoQuestRange = SNO_NONE;
    tInfo.hZoneLabel = (StringLabelHandle)-1;
    tInfo.hWaypointButtonNormal = 0;
    tInfo.hWaypointButtonMouseOver = 0;
    tInfo.hWaypointButtonPushed = 0;
    tInfo.pointMapLocation.Set(0, 0);
}

// line 131
void ActGroupDef::GetDefaultData(void * ptDefaultDef) const {
    ActDefinition * pdef = (ActDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdef, sizeof(ActDefinition));

    for (int32 i = 0; i < 100; i++) {
        WaypointInfo & tInfo = pdef->aWaypointInfo[i];
        WaypointInfo::GetDefaultData(tInfo);
    }
}
