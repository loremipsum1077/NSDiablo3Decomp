// Original file: X/Client/CPlayerUtil.h
#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"
#include "../Common/CommonTypes.h"

typedef int32 GameError;

enum PlayerClickIntersectType {
    PLAYER_CLICK_INTERSECT_APPEARANCE = 0,
    PLAYER_CLICK_INTERSECT_NAV_MESH = 1,
    PLAYER_CLICK_INTERSECT_WALKING = 2,
    PLAYER_CLICK_INTERSECT_GROUND_PLANE = 3,
    PLAYER_CLICK_INTERSECT_CONTACT_OFFSET_PLANE_FROM_NAV_MESH = 4,
    PLAYER_CLICK_INTERSECT_CONTACT_OFFSET_PLANE_FROM_GROUND_PLANE = 5,
};

struct Player;
struct RActor;
class Target;
struct ResolvedClick;
struct IVector2D;
struct Vector3D;

typedef int32 SNO;  // X/Common/SNO/SNOFiles.h
typedef float WorldDistance;  // X/Sigma/Math/WorldTypes.h

void CPlayerUtilPlayerClick(const Player * ptPlayer, const IVector2D & p, const PlayerClickIntersectType eIntersectType, Vector3D & wpIntersect, const float flPlaneOffsetDist);
RActorID CPlayerUtilGetNearbyStickyTargetedActorFromList(const Player * ptPlayer, const Vector3D & vBaseTargetPos, const SNO snoPower);
WorldDistance CPlayerUtilGetRangeFromDest(const ACDID idACDPlayer, const SNO snoPower, const Target & tActorTarget);
BOOL CPlayerUtilNeedsToGetCloser(const RActor * ptPlayerRActor, SNO snoPower, const Target & tActorTarget, WorldDistance wdRange, WorldDistance wdLongReach);
BOOL CPlayerUtilPlayerSpecifiedDontMove(const Player * ptPlayer, BOOL bPrimaryActionKey, BOOL bHasActorTarget);
GameError CPlayerUtilResolveConsoleButton(const SNO snoPowerOnButton, ResolvedClick & tResolvedClick, const Player * ptPlayer, BOOL bHeldButton, BOOL bCheckForHostiles, BOOL bCheckForOperatableGizmos);
RActorID CPlayerUtilPickRActorForPower(const Player * ptPlayer, const SNO snoPowerToUse);
BOOL CPlayerUtilCanInterruptComboNow();
void CPlayerUtilSpawnClickEffect(const Target & tTarget);
void CPlayerUtilSpawnPowerClickEffect(SNO snoPower, const Target & tTarget);
