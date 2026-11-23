// Original file: X/Common/Target.cpp

#include "Target.h"
#include "../Client/Gfx/GfxVB.h"

#include "ActorCommonData.h"
#include "Nav/NavigationMesh.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Memory/SigmaMemory.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through ActorCommonData.h's
// include chain. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"

// Defined in X/Common/Powers/PowersCommon.cpp:5129 and :5266 and X/Common/Nav/NavigationMesh.cpp:2411;
// no adopted header declares them yet.
void PowerGetNavPlaceFromACD(const ActorCommonData * ptACD, NavPlace & tNavPlace);
BOOL PowerPlaceToNavMeshHandle(const WorldPlace & tPlace, NavMeshHandle & tNavMeshHandle);
BOOL NavMeshHandleIsNone(const NavMeshHandle & tNavMeshHandle);

// _GLOBAL__sub_I_Target.cpp also runs the header statics of the includes above (cgplaceNull,
// the four RGBAColor normals of X/Client/Gfx/GfxVB.h:351-354).
const Target cgTargetNone;

void Target::Clear() {
    eType = TT_NONE;
    idACDTarget = ACDID_INVALID;
    SigmaMemoryClear(&tPlace, sizeof(tPlace));
    bForcedActorTarget = FALSE;
}






BOOL Target::IsNone() const {
    return eType == TT_NONE;
}



// NON_MATCHING: 116 vs 120 bytes. The original sets w0 = 1 before the first null test and gives
// the IsInWorld/GetPlace failures their own `mov w0, #1`; ours sends every failure to one block.
BOOL Target::IsInvalidForSource(const ActorCommonData * ptACDSource) const

{
    if (ptACDSource == NULL || eType == TT_NONE)
    {
        return TRUE;
    }






    // row 59 has columns 9/10 (deeper indentation in the original; what encloses it is unknown)
    if (!IsInWorld())
    {
        return TRUE;
    }

    WorldPlace placeSource = ptACDSource->GetPlace();
    WorldPlace placeTarget;
    BOOL bValidPlace = GetPlace(placeTarget);
    if (!bValidPlace)
    {
        return TRUE;
    }

    if (placeSource.idSWorld != placeTarget.idSWorld)
    {
        return TRUE;
    }

    return FALSE;

}





BOOL Target::IsInvalid() const {
    if (eType == TT_NONE) {
        return TRUE;
    }

    if (eType == TT_ACTOR) {
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget == NULL || !ptACDTarget->IsInWorld()) {
            return TRUE;
        }
    }

    if (eType == TT_ACTOR_WITH_FALLBACK_POINT) {
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget == NULL || !ptACDTarget->IsInWorld()) {
            if (tPlace.idSWorld == SWORLDID_INVALID) {
                return TRUE;
            }
        }
    }

    return FALSE;
}













BOOL Target::IsTargetingACD() const {
    BOOL bTargetingACD = FALSE;

    switch (eType) {
    case TT_ACTOR:
    case TT_ACTOR_WITH_FALLBACK_POINT:
        bTargetingACD = TRUE;
        break;
    }

    return bTargetingACD;
}











void Target::ValidateTargetACD() {
    if (IsTargetingACD()) {
        if (ACDTryToGet(GetACD()) == NULL) {
            if (eType == TT_ACTOR_WITH_FALLBACK_POINT) {
                eType = TT_POINT;
                idACDTarget = ACDID_INVALID;
                bForcedActorTarget = FALSE;
            } else {
                Clear();
            }
        }
    }
}

















void Target::InitializeFromPlace(const WorldPlace & _tPlace) {
    eType = TT_POINT;
    tPlace = _tPlace;
    idACDTarget = ACDID_INVALID;
    bForcedActorTarget = FALSE;
}






void Target::InitializeFromACD(const ACDID idACD, const BOOL bWantFallbackPoint, const BOOL bForcedTarget) {
    ActorCommonData * ptACDTarget = ACDTryToGet(idACD);
    if (ptACDTarget != NULL) {
        eType = TT_ACTOR;
        idACDTarget = idACD;
        if (bWantFallbackPoint) {
            tPlace = ptACDTarget->GetPlace();
            if (ptACDTarget->IsAlive()) {
                eType = TT_ACTOR_WITH_FALLBACK_POINT;
            } else {
                idACDTarget = ACDID_INVALID;
                eType = TT_POINT;
            }
        }
        bForcedActorTarget = bForcedTarget;
    } else {
        Clear();
    }
}




















BOOL Target::GetPlace(WorldPlace & _tPlace) const {
    switch (eType) {
    case TT_POINT:
        _tPlace = tPlace;
        return TRUE;

    case TT_ACTOR: {
        ActorCommonData * ptACD = ACDTryToGet(idACDTarget);
        if (ptACD != NULL && ptACD->tLocation.eActorLocation == AL_WORLD) {
            _tPlace = ptACD->GetPlace();
        } else {
            _tPlace = cgplaceNull;
        }
        return TRUE;
    }

    case TT_ACTOR_WITH_FALLBACK_POINT: {
        ActorCommonData * ptACD = ACDTryToGet(idACDTarget);
        if (ptACD != NULL && ptACD->tLocation.eActorLocation == AL_WORLD) {
            _tPlace = ptACD->GetPlace();
        } else {
            _tPlace = tPlace;
        }
        return TRUE;
    }

    default:
        return FALSE;
    }
}

















BOOL Target::IsInWorld() const {
    BOOL bInWorld = FALSE;

    switch (eType) {
    case TT_POINT:
        bInWorld = TRUE;
        break;

    case TT_ACTOR: {
        ActorCommonData * ptACD = ACDTryToGet(idACDTarget);
        if (ptACD != NULL) {
            bInWorld = ptACD->GetLocationType() == AL_WORLD;
        }
        break;
    }

    case TT_ACTOR_WITH_FALLBACK_POINT: {
        ActorCommonData * ptACD = ACDTryToGet(idACDTarget);
        if (ptACD != NULL) {
            ActorLocationType eActorLocationType = ptACD->GetLocationType();
            bInWorld = eActorLocationType == AL_WORLD;
        } else {
            bInWorld = TRUE;
        }
        break;
    }
    }

    return bInWorld;
}















// NON_MATCHING: 700 vs 704 bytes. In the shared PowerPlaceToNavMeshHandle tail the original keeps
// &tPlace in x21 across the call and loads tPlace.wp.x through it; ours rematerialises the
// address and loads from this + 8.
BOOL Target::GetNavPlace(NavPlace & tNavPlace) const {
    switch (eType) {
    case TT_POINT:
        if (!PowerPlaceToNavMeshHandle(tPlace, tNavPlace.tNavMeshHandle)) {
            return FALSE;
        }
        tNavPlace.tPlace = tPlace;
        return TRUE;

    case TT_ACTOR: {
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget != NULL && ACDGetOwner(idACDTarget) != DATAID_INVALID) {
            PowerGetNavPlaceFromACD(ptACDTarget, tNavPlace);
            if (!NavMeshHandleIsNone(tNavPlace.tNavMeshHandle)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    case TT_ACTOR_WITH_FALLBACK_POINT: {
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        BOOL bSuccess = FALSE;
        if (ptACDTarget != NULL && ACDGetOwner(idACDTarget) != DATAID_INVALID) {
            PowerGetNavPlaceFromACD(ptACDTarget, tNavPlace);
            bSuccess = !NavMeshHandleIsNone(tNavPlace.tNavMeshHandle);
        }
        if (bSuccess) {
            return TRUE;
        }

        if (!PowerPlaceToNavMeshHandle(tPlace, tNavPlace.tNavMeshHandle)) {
            return FALSE;
        }
        tNavPlace.tPlace = tPlace;
        return TRUE;
    }

    default:
        return FALSE;
    }
}






























ACDID Target::GetACD() const {
    ACDID idACD = ACDID_INVALID;

    switch (eType) {
    case TT_NONE:
    case TT_POINT:
        break;

    case TT_ACTOR:
    case TT_ACTOR_WITH_FALLBACK_POINT:
        idACD = idACDTarget;
        break;

    default:
        DisplayInternalError(NULL, "", 406, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return idACD;
}



BOOL Target::GetAttackPosition(WorldPlace & placeAttack) const {
    if (eType == TT_ACTOR) {
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget == NULL || !ptACDTarget->IsInWorld()) {
            return FALSE;
        }
    }

    GetPlace(placeAttack);
    return TRUE;
}
