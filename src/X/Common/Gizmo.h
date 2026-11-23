// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Gizmo.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"

enum {
    GIZMOSTATE_FULL = 0,
    GIZMOSTATE_EMPTY = 1,
    GIZMOSTATE_HALF = 2,
    GIZMOSTATE_ACTIVE = 0,
    GIZMOSTATE_INACTIVE = 1,
    GIZMOSTATE_DOOR_CLOSED = 0,
    GIZMOSTATE_DOOR_OPENED = 1,
    GIZMO_STATE_DESTROYED = 1,
    GIZMO_STATE_SECRET_PORTAL_CLOSED = -1,
    GIZMO_STATE_SECRET_PORTAL_OPENING = 0,
    GIZMO_STATE_SECRET_PORTAL_OPEN = 1,
};

typedef int32 SpawnerTriggerType;


class ActorCommonData;
class CRefString;
typedef int32 SNO;  // X/Common/SNO/SNOFiles.h
typedef int32 GBID;  // X/Common/GameBalance.h
typedef int32 GizmoType;  // X/Common/GizmoSwig.h

// Declared here from their definitions in Gizmo.cpp; the DWARF does not record which header
// declared them.
BOOL GizmoPlayerCanCloseDoor(const ActorCommonData & tACD);  // Gizmo.cpp:496
BOOL ACDGizmoCanOperate(const ActorCommonData & tACDGizmo, const ActorCommonData & tACDOperator,
                        BOOL bIgnoreCantStartDisplayedPowers);  // Gizmo.cpp:505
GBID GizmoGetItemToUse(SNO snoActor);  // Gizmo.cpp:824
BOOL GizmoIsWreckable(const GizmoType eGizmoType);  // Gizmo.cpp:836
BOOL GizmoIsDisabled(const ActorCommonData & tACD);  // Gizmo.cpp:853
BOOL GizmoChestIsOpen(const ActorCommonData & tACD);  // Gizmo.cpp:864
BOOL GizmoSwitchIsOn(const ActorCommonData & tACD);  // Gizmo.cpp:883
BOOL GizmoDoorIsOpen(const ActorCommonData & tACD);  // Gizmo.cpp:899
BOOL GizmoPowerUpIsInactive(const ActorCommonData & tACD);  // Gizmo.cpp:917
BOOL GizmoHealingWellIsEmpty(const ActorCommonData & tACD);  // Gizmo.cpp:933
BOOL GizmoObjectiveFinderIsUsed(const ActorCommonData & tACD);  // Gizmo.cpp:949
BOOL GizmoLockedByQuestRange(const ActorCommonData & tACD);  // Gizmo.cpp:965
BOOL GizmoDoorIsLocked(const ActorCommonData & tACD);  // Gizmo.cpp:1026
BOOL GizmoHasContentRestrictions(const ActorCommonData & tACD);  // Gizmo.cpp:1052
SNO ACDGizmoGetOperationPower(const ActorCommonData & tACD);  // Gizmo.cpp:1060
uint32 ACDGizmoGetAnimation(const ActorCommonData & tACD);  // Gizmo.cpp:1107
GizmoType ActorGetGizmoType(SNO snoActor);  // Gizmo.cpp:1129
BOOL GizmoSupportsProximityDetection(const GizmoType eGizmoType);  // Gizmo.cpp:1145
BOOL GizmoIsNonMoving(GizmoType eGizmoType);  // Gizmo.cpp:1167
BOOL GizmoGetUINameFromLabel(SNO snoActor, CRefString & usText);  // Gizmo.cpp:1176
BOOL GizmoGetUIName(const ActorCommonData & tACD, CRefString & usText);  // Gizmo.cpp:1189
void GizmoGetOperationRadius(const ActorCommonData & tACDGizmo, const ActorCommonData * ptACDOperator,
                             float & flOperationRadius);  // Gizmo.cpp:1429
BOOL GizmoShouldFaceToOperate(const ActorCommonData & tACDGizmo);  // Gizmo.cpp:1486
void LoreGizmoRestoreOpenState(ActorCommonData & tACD);  // Gizmo.cpp:1514
BOOL GizmoIsPortal(GizmoType eGizmoType);  // Gizmo.cpp:1542
BOOL GizmoUsesStaticMinimapPing(GizmoType eGizmoType);  // Gizmo.cpp:1555
