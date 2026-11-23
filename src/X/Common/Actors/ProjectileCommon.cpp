// Original file: X/Common/Actors/ProjectileCommon.cpp
//
// Projectile helpers shared by the client and server. Functions are in their original order:
// ProjectileGetTicksTillDetonate (line 30) and ProjectileValidateMaxDuration (line 68), with
// MAX_PROJECTILE_DURATION_TICKS between them (line 66).

#include "../ActorCommonData.h"
#include "../Attributes/FastAttributeStructs.h"
#include "../CommonTypes.h"
#include "../Game.h"
#include "../../Client/CGame.h"
#include "../../Server/Actors/Actors.h"
#include "../../Sigma/Math/MathGeneral.h"

// Declared locally until their headers are adopted; the comment names the defining file.
uint32 GameGetServerTime();  // X/Common/GameCommonData.cpp:292
BOOL GameActiveGameCommonDataIsServer();  // X/Common/Game.cpp:923
BOOL ACDIsProjectile(ACDID idACD);  // X/Common/ActorCommonData.cpp:5494
void ActorFlagForDeletion(ActorID idActor);  // X/Server/Actors/Actors.cpp:4407

int32 ProjectileGetTicksTillDetonate(ACDID idACDProjectile)
{
    int32 nTicksTillDetonate = -1;
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (ptACDProjectile != NULL)
    {
        uint32 dwDetonateTime = ptACDProjectile->AttributesGetInt(ATTRIBUTE_PROJECTILE_DETONATE_TIME);
        if (dwDetonateTime != 0)
        {
            uint32 dwNow = GameActiveGameCommonDataIsServer() ? GameGetServerTime() : CGameGetPredictedServerTime();
            int32 nTimeTillFixedTime = (int32)(dwDetonateTime - dwNow);
            nTicksTillDetonate = Math::Max(0, nTimeTillFixedTime);
        }
    }

    return nTicksTillDetonate;
}


static const uint32 MAX_PROJECTILE_DURATION_TICKS = 1800;

BOOL ProjectileValidateMaxDuration(ACDID idACDProjectile)
{
    if (!GameActiveGameCommonDataIsServer())
    {
        return TRUE;
    }

    if (!ACDIsProjectile(idACDProjectile))
    {
        return TRUE;
    }

    ActorID idActor = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL)
    {
        return TRUE;
    }

    if (GameGetServerTime() <= ptActor->dwTimeCreated)
    {
        return TRUE;
    }

    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    uint32 dwTimeProjLifetime = GameGetServerTime() - ptActor->dwTimeCreated;
    if (dwTimeProjLifetime > MAX_PROJECTILE_DURATION_TICKS &&
        (ptACDProjectile == NULL ||
         ptACDProjectile->AttributesGetInt(ATTRIBUTE_PROJECTILE_UNCAPPED_LIFETIME) == 0))
    {
        ActorFlagForDeletion(idActor);
        return FALSE;
    }

    return TRUE;
}
