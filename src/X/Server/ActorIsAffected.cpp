// Original file: X/Server/ActorIsAffected.cpp

#include "../Common/ActorCommonData.h"
#include "../Common/Attributes/FastAttribute.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/Definitions/PowerDefinition.h"
#include "../Common/Game.h"
#include "../Common/GameTemplate.h"
#include "../Common/Movement/ActorMovement.h"
#include "../Common/Player.h"
#include "../Common/Powers/PowersCommon.h"
#include "../Sigma/Math/MathGeneral.h"
#include "AI/ActorAI.h"
#include "AI/BrainStance.h"
#include "Actors/ActorPowers.h"
#include "Actors/Actors.h"
#include "Powers/Powers.h"

// Declared here because their headers do not carry them yet.
BOOL BrainStanceIsInWalkingStance(ActorID idActor);  // X/Server/AI/BrainStance.cpp:674
BOOL BrainStanceNeedToPlayWalkCoolDown(ActorID idActor);  // X/Server/AI/BrainStance.cpp:617
BOOL ActorPowersPrimaryIsNotInterruptable(ActorID idActor);  // X/Server/Actors/ActorPowers.cpp:253
BOOL ActorPowersPrimaryIsInterruptableByWalking(ActorID idActor);  // X/Server/Actors/ActorPowers.cpp:274





BOOL PowerACDCanTakeDamage(ActorCommonData & tACD)

{
    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_INVULNERABLE)) > 0 ||
        tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_NO_DAMAGE)) > 0)
    {
        return FALSE;
    }

    if (tACD.IsGizmo() && !tACD.IsWreckableObject())
    {
        return FALSE;
    }


    return TRUE;
}



static BOOL sPowerActorCanInterruptHimself(ActorID idActor, BOOL bAllowPlayingAnimInterrupt)

{

    if (ActorPowersPrimaryIsNotInterruptable(idActor))
    {
        return FALSE;
    }

    if (BrainStanceNeedToPlayWalkCoolDown(idActor))
    {
        return FALSE;
    }

    EStanceType eStance = BrainGetStanceType(idActor);
    switch (eStance)
    {
    case STANCE_GETHIT:
        {
            Actor * ptActor = ActorGet(idActor);
            if (ACDIsPlayer(ptActor->idACD))
            {
                return FALSE;
            }
            return FALSE;
        }

    case STANCE_KNOCK_BACK:
    case STANCE_STUNNED:
    case STANCE_DYING:
    case STANCE_DEAD:
    case STANCE_WALK_WARM_UP:
    case STANCE_WALK_COOL_DOWN:
    case STANCE_FEARED:
    case STANCE_ATTACHED:
    case STANCE_FROZEN:
    case STANCE_FORCE_GRIPPED:
        return FALSE;

    case STANCE_PLAY_ANIMATION:






        if (bAllowPlayingAnimInterrupt) return TRUE;
        return FALSE;
    }

    return TRUE;
}











BOOL PowerActorCanWalk(ActorID idActor)

{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL)
    {
        return FALSE;
    }

    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    if (tACD.IsGizmo())
    {
        return FALSE;
    }

    if (tACD.IsAttached() ||
        tACD.IsImmobilized() ||
        tACD.IsStunned())
    {
        return FALSE;
    }

    PowerID idPower = ActorPowersGetPrimary(idActor);
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        if (ActorPowersPrimaryIsInterruptableByWalking(idActor) ||
            PowerShouldUseClientWalking(tACD, AutoPowerDefinitionR(ptPower->snoPower)))
        {
            return TRUE;
        }
    }

    if (BrainStanceIsInWalkingStance(idActor))
    {
        return TRUE;
    }
    if (!sPowerActorCanInterruptHimself(idActor, FALSE))
    {
        return FALSE;
    }
    return TRUE;
}




BOOL PowerActorCanPlayAnimation(ActorID idActor, BOOL bAllowPlayingAnimInterrupt)


{
    return sPowerActorCanInterruptHimself(idActor, bAllowPlayingAnimInterrupt);
}




BOOL PowerActorCanDoAnything(ActorID idActor)

{
    return sPowerActorCanInterruptHimself(idActor, FALSE);
}




BOOL PowerActorCanGracefullyStopSkill(ActorID idActor)

{
    EStanceType eStance = BrainGetStanceType(idActor);

    if (eStance == STANCE_SKILL)
    {
        return TRUE;
    }


    return FALSE;
}



BOOL PowerActorCanDoGetHit(ActorID idActor)

{
    ACDID idACD = ActorGet(idActor)->idACD; ActorCommonData & tACD = ACDGet(idACD);

    if (tACD.IsWreckableObject())
    {
        return FALSE;
    }

    SNO snoPower = ActorPowersGetPrimaryPower(idActor);
    if (snoPower != SNO_NONE && PowerIsRecoilImmune(&tACD, AutoPowerDefinitionR(snoPower)))
    {
        return FALSE;
    }

    EStanceType eStance = BrainGetStanceType(idActor);

    switch (eStance)
    {
    case STANCE_GETHIT:
    case STANCE_KNOCK_BACK:
    case STANCE_STUNNED:
    case STANCE_DYING:
    case STANCE_DEAD:
    case STANCE_PLAY_ANIMATION:
    case STANCE_FROZEN:
    case STANCE_FORCE_GRIPPED:
        return FALSE;

    case STANCE_FEARED:
        return !ACDIsPlayer(idACD);
    }
















    return TRUE;
}


static BOOL sActorIsInKnockdownMovement(ActorID idActor)

{
    ActorCommonData * ptACD = ACDTryToGet(ACDFindByActor(idActor));
    if (ptACD)
    {

        ActorMovement * ptMovement = ptACD->GetActorMovement();
        if (ptMovement)
        {
            if (ptMovement->IsMoving() && (ptMovement->m_uMoveFlags & 0x10000))
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}




BOOL PowerActorCanDo(ActorID idActor, SNO snoPower)


{


    if (ActorPowersPrimaryIsNotInterruptable(idActor))
    {
        return FALSE;
    }

    if (BrainStanceNeedToPlayWalkCoolDown(idActor))
    {
        return FALSE;
    }

    if (sActorIsInKnockdownMovement(idActor))
    {
        return FALSE;
    }


    const ActorCommonData * ptACD = ACDTryToGet(ACDFindByActor(idActor));

    EStanceType eStance = BrainGetStanceType(idActor);
    switch (eStance)
    {
    case STANCE_GETHIT:
        return GameGetGameType() == GameTypeIsPvP();

    case STANCE_KNOCK_BACK:
    case STANCE_SKILL:
    case STANCE_DYING:
    case STANCE_DEAD:
    case STANCE_WALK_WARM_UP:
    case STANCE_WALK_COOL_DOWN:
    case STANCE_PLAY_ANIMATION:
    case STANCE_ATTACHED:
        return FALSE;






    case STANCE_FEARED:
        return PowerBreaksFear(ptACD, AutoPowerDefinitionR(snoPower)) != FALSE;








    case STANCE_STUNNED:
    case STANCE_FROZEN:
    case STANCE_FORCE_GRIPPED:
        return PowerBreaksStun(ptACD, AutoPowerDefinitionR(snoPower)) != FALSE;
    }















    return TRUE;
}




BOOL PowerActorCanDie(ActorID idActor)

{
    if (ActorHasBeenKilled(idActor))
    {
        return FALSE;
    }



    return TRUE;
}



BOOL PowerActorCanResurrect(ActorID idActor)

{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL)
    {
        return FALSE;
    }

    if (!ActorHasBrain(idActor))
    {
        return FALSE;
    }


    EStanceType eStance = BrainGetStanceType(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    BOOL bIsPlayer = ACDIsPlayer(idACD);
    if (!bIsPlayer && !ACDIsHireling(tACD))
    {
        if (eStance == STANCE_DYING)
            return FALSE;
    }

    if (bIsPlayer)
    {
        Player * ptPlayer = PlayerGetByACD(idACD);
        if (ptPlayer && !GameTypeIsPvP() &&
            (ptPlayer->tHeroStateData.uPlayerFlags & 1) &&

            !tACD.IsDueling())
        {
            return FALSE;
        }
    }

    if (!ActorHasBeenKilled(idActor))
    {
        return FALSE;
    }


    return TRUE;
}



BOOL PowerActorCanBeDead(ActorID idActor)

{
    EStanceType eStance = BrainGetStanceType(idActor);

    if (eStance == STANCE_DEAD)
    {
        return FALSE;
    }


    return TRUE;
}



BOOL PowerActorCanBeKnockedBack(ActorID idActor, PowerID idPower)


{
    ActorCommonData & tACD = ACDGet(ActorGet(idActor)->idACD);

    if (tACD.IsWreckableObject())
    {
        return FALSE;
    }

    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_IMMUNE_TO_KNOCKBACK)))
    {
        return FALSE;
    }

    if (tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_KNOCKBACK_WEIGHT)) < EPSILON)
    {
        return FALSE;
    }

    if (idPower == POWERID_INVALID || tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LAST_CONTINUOUS_KNOCKBACK_POWER_ID)) != idPower)
    {

        if (tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_CROWD_CONTROL_RESISTANCE)) >= 0.65f)
        {
            return FALSE;
        }
    }


    SNO snoPower = ActorPowersGetPrimaryPower(idActor);
    if (snoPower != SNO_NONE && PowerIsKnockbackImmune(&tACD, AutoPowerDefinitionR(snoPower)))
    {
        return FALSE;
    }

    EStanceType eStance = BrainGetStanceType(idActor);

    switch (eStance)
    {
    case STANCE_DYING:
    case STANCE_DEAD:
    case STANCE_PLAY_ANIMATION:
        return FALSE;
    }

    return TRUE;
}
