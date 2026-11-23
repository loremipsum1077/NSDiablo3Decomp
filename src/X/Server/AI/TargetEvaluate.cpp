// Original file: X/Server/Ai/TargetEvaluate.cpp
#include "./TargetEvaluate.h"
#include "../Actors/Actors.h"
#include "./ActorAI.h"
#include "./BrainComponents.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Powers/Powers.h"
#include "../Powers/PowerAPI.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Player.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Common/Target.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Random.h"
#include <float.h>

// Declared here until their headers carry them.
BOOL ACDCanAttackTarget(const ActorCommonData & tACD, const Target & tTarget, AutoPowerDefinitionR pdefPower, uint32 * pdwFailureReasons);  // ActorAI.cpp:1116
BOOL ACDInSameWorld(const ActorCommonData & tACD1, const ActorCommonData & tACD2);  // X/Common/ActorCommonData.cpp:2503
BOOL PowerActorCanBeKnockedBack(ActorID idActor, PowerID idPower);  // X/Server/ActorIsAffected.cpp:430














BOOL sEvaluateTarget(ActorCommonData & tACD, SNO snoPower, const ActorCommonData & tACDTarget, float & flDistSqr, BOOL & bPreferred, const BOOL bAffectedOnly, const BOOL bForcedTarget)
{







    Target tTarget;
    tTarget.InitializeFromACD(tACDTarget.id, FALSE, bForcedTarget);


    if (!ACDCanAttackTarget(tACD, tTarget, AutoPowerDefinitionR(snoPower), NULL))
    {
        return FALSE;
    }

    if (!PowerCanTarget(AutoPowerDefinitionR(snoPower), tACD.id, &tTarget))
    {
        return FALSE;
    }

    ScopedPowerDefinitionLockR pdefPower(snoPower);


    BOOL bPlayersOnly = PowerGeneralTagGetInt(pdefPower, TAG_POWER_CUSTOM_TARGET_PLAYERS_ONLY);
    if (bPlayersOnly && !ACDIsPlayer(tACDTarget.id))
    {
        return FALSE;
    }

    SNO snoBuffPower = PowerGeneralTagGetSNO(pdefPower, TAG_POWER_CUSTOM_TARGET_BUFF_SNO);
    if (snoBuffPower == SNO_NONE)
    {
        snoBuffPower = snoPower;
    }


    BOOL bUnaffectedOnly = PowerGeneralTagGetInt(pdefPower, TAG_POWER_CUSTOM_TARGET_UNAFFECTED_ONLY);
    if (bUnaffectedOnly || bAffectedOnly)
    {
        int32 nBuffIndex = PowerGeneralTagGetInt(pdefPower, TAG_POWER_CUSTOM_TARGET_BUFF_SNO_BUFF_INDEX);
        BuffID idExistingBuff = PowerAPI_BuffFindOnACD(snoBuffPower, nBuffIndex, tACDTarget, ACDID_INVALID, FALSE);

        if (bUnaffectedOnly && idExistingBuff != BUFFID_INVALID)
        {
            return FALSE;
        }

        if (bAffectedOnly && idExistingBuff == BUFFID_INVALID)
        {
            return FALSE;
        }
    }


    snoBuffPower = PowerGeneralTagGetSNO(pdefPower, TAG_POWER_CUSTOM_TARGET_BUFF_SNO_2);
    if (snoBuffPower != SNO_NONE)
    {
        bUnaffectedOnly = PowerGeneralTagGetInt(pdefPower, TAG_POWER_CUSTOM_TARGET_UNAFFECTED_ONLY_2);
        if (bUnaffectedOnly || bAffectedOnly)
        {
            int32 nBuffIndex = PowerGeneralTagGetInt(pdefPower, TAG_POWER_CUSTOM_TARGET_BUFF_SNO_BUFF_INDEX_2);
            BuffID idExistingBuff = PowerAPI_BuffFindOnACD(snoBuffPower, nBuffIndex, tACDTarget, ACDID_INVALID, FALSE);

            if (bUnaffectedOnly && idExistingBuff != BUFFID_INVALID)
            {
                return FALSE;
            }

            if (bAffectedOnly && idExistingBuff == BUFFID_INVALID)
            {
                return FALSE;
            }
        }
    }


    if (!ACDInSameWorld(tACD, tACDTarget))
    {
        return FALSE;
    }

    flDistSqr = ACDGetDistanceSquared(tACD, tACDTarget, FALSE);
    bPreferred = TRUE;

    BOOL bPreferredOnly = PowerGeneralTagGetInt(pdefPower, TAG_POWER_CUSTOM_TARGET_PREFERRED_RANGE);


    float nMinRange = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_CUSTOM_TARGET_MIN_RANGE, -1, -1, 0);
    if (nMinRange > 0.0f && flDistSqr < nMinRange * nMinRange)
    {
        bPreferred = FALSE;
        if (!bPreferredOnly)
        {
            return FALSE;
        }
    }


    float nMaxRange = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_CUSTOM_TARGET_MAX_RANGE, -1, -1, 0);
    if (nMaxRange > 0.0f && flDistSqr > nMaxRange * nMaxRange)
    {
        bPreferred = FALSE;
        if (!bPreferredOnly)
        {
            return FALSE;
        }
    }

    return TRUE;
}



void sGetTarget(Target & tTarget, ActorID idActor, SNO snoPower, TargetEvaluate eTargetType)




{
    BOOL bBestPreferred = FALSE; int32 nCount = 0; float flBestDistSqr = (eTargetType == TARGETEVALUATE_FARTHEST_TARGET) ? 0.0f : FLT_MAX;


    uint32 uNow = GameGetServerTime();

    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        const TTargetList * ppList[2]; ActorCommonData & tACD = ACDGet(idACD);

        BrainAPIUpdateTargets(idActor, FALSE);
        ppList[0] = NULL; ppList[1] = NULL;

        if (PowerGeneralTagGetBool(AutoPowerDefinitionR(snoPower), TAG_POWER_CAST_TARGET_ALLIES))
        {
            ppList[0] = BrainAPIGetFriendlyTargetList(idActor);
        }

        if (PowerGeneralTagGetBool(AutoPowerDefinitionR(snoPower), TAG_POWER_CAST_TARGET_ENEMIES))
        {
            ppList[1] = BrainAPIGetEnemyTargetList(idActor);
        }

        for (int32 i = 0; i < 2; i++)
        {
            const TTargetList * pList = ppList[i];
            if (pList == NULL)
            {
                continue;
            }


            POS pos = pList->GetHeadPosition();
            while (pos)
            {
                const TargetInfo & tActorTarget = pList->GetNext(pos);
                BOOL bForcedTarget = tActorTarget.dwLastSeenTime == 0;
                const Actor * ptTargetActor = ActorTryToGet(tActorTarget.idTarget);
                if (ptTargetActor == NULL)
                {
                    continue;
                }

                if (uNow < tActorTarget.dwActiveTime)
                {
                    continue;
                }


                ACDID idACDTarget = ptTargetActor->idACD;
                ActorCommonData & tACDTarget = ACDGet(idACDTarget);
                float flDistSqr = 0.0f;
                BOOL bPreferred = FALSE;

                if (!sEvaluateTarget(tACD, snoPower, tACDTarget, flDistSqr, bPreferred, eTargetType == TARGETEVALUATE_HAS_POWER_BUFF, bForcedTarget))
                {
                    continue;
                }



                if (!bPreferred && bBestPreferred)
                {
                    continue;
                }


                switch (eTargetType)
                {
                case TARGETEVALUATE_RANDOM_TARGET_NOT_LARGE_OR_KNOCKBACKABLE:
                    if (!tACDTarget.IsMonster() ||
                        tACDTarget.IsLargeMonster() ||
                        !PowerActorCanBeKnockedBack((ActorID)ACDGetOwner(idACDTarget), POWER_NONE))
                    {
                        continue;
                    }
                    break;

                case TARGETEVALUATE_CLOSEST_PLAYER_OR_PET_NEEDS_HEAL:
                    {
                        float flNeedsHealHPPercent = PowerGeneralTagGetFloat(AutoPowerDefinitionR(snoPower), TAG_POWER_CUSTOM_TARGET_NEEDS_HEAL_HP_PERCENT);
                        if (tACDTarget.GetHitpointPercent() > flNeedsHealHPPercent)
                        {
                            continue;
                        }
                    }
                    break;

                case TARGETEVALUATE_CLOSEST_TARGET_IN_PIE:
                    {
                        Vector3D vecToTarget = tACDTarget.GetPlace().wp - tACD.GetPlace().wp;
                        vecToTarget.Normalize();

                        Vector3D vecForward(cgwvXAxis);
                        Angle aYaw = tACD.GetOrientation().ToYaw();
                        Vector3D::Rotate(cgwvZAxis, aYaw, vecForward);

                        Angle aAngleBetween = Math::ACos(Vector3D::DotProduct(vecToTarget, vecForward));
                        Angle flMaxHalfAngle = PowerGeneralTagGetFloat(AutoPowerDefinitionR(snoPower), TAG_POWER_CUSTOM_TARGET_CLOSEST_IN_PIE_ANGLE);
                        flMaxHalfAngle = Math::DegToAngle(flMaxHalfAngle) * 0.5f;
                        if (aAngleBetween > flMaxHalfAngle + 0.000001f)
                        {
                            continue;
                        }
                    }
                    break;
                }

                switch (eTargetType)
                {
                case TARGETEVALUATE_CLOSEST_TARGET:
                case TARGETEVALUATE_CLOSEST_TARGET_IN_PIE:
                    if (bPreferred == bBestPreferred && flDistSqr >= flBestDistSqr)
                    {
                        continue;
                    }
                    break;

                case TARGETEVALUATE_FARTHEST_TARGET:
                    if (bPreferred == bBestPreferred && flDistSqr <= flBestDistSqr)
                    {
                        continue;
                    }
                    break;

                case TARGETEVALUATE_RANDOM_TARGET:
                case TARGETEVALUATE_HAS_POWER_BUFF:
                case TARGETEVALUATE_RANDOM_TARGET_NOT_LARGE_OR_KNOCKBACKABLE:
                    if (bPreferred && !bBestPreferred)
                    {
                        nCount = 0;
                    }

                    nCount++;
                    if (Rand_Internal(SGameGlobalsGet().rng, nCount - 1) != 0)
                    {
                        continue;
                    }
                    break;

                case TARGETEVALUATE_CLOSEST_PLAYER_OR_PET_NEEDS_HEAL:
                    {
                        ActorCommonData * ptACDWinner = ACDTryToGet(tTarget.GetACD());
                        if (ptACDWinner)
                        {
                            BOOL bSameType;

                            BOOL bWinnerIsPlayer = ACDIsPlayer(tTarget.GetACD());
                            BOOL bTargetIsPlayer = ACDIsPlayer(idACDTarget);
                            BOOL bWinnerIsPet = ptACDWinner->IsPet();
                            BOOL bTargetIsPet = tACDTarget.IsPet();

                            bSameType = (bWinnerIsPlayer && bTargetIsPlayer) ||
                                        (bWinnerIsPet && bTargetIsPet);
                            if (bSameType)
                            {
                                if (flDistSqr >= flBestDistSqr)
                                {
                                    continue;
                                }
                            }
                            else
                            {
                                if (bWinnerIsPlayer && bTargetIsPet)
                                {
                                    continue;
                                }
                            }
                        }
                    }
                    break;
                }

                tTarget.InitializeFromACD(idACDTarget, FALSE, bForcedTarget);
                flBestDistSqr = flDistSqr;
                bBestPreferred = bPreferred;
            }
        }
    }
}








BOOL TargetEvaluateIsOnTargetTeam(ACDID idACD, ACDID idACDTarget, TargetTeam eTargetTeam, BOOL bIgnoreAlive)




{
    ActorCommonData & tACD = ACDGet(idACD);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);


    if (ptACDTarget == NULL ||
        (!bIgnoreAlive && !ptACDTarget->IsAlive()))
    {
        return FALSE;
    }

    BOOL bCharmedAlliesAreEnemies = !ActorIsUsingAIQueue((ActorID)ACDGetOwner(idACD));

    if (eTargetTeam == TARGET_TEAM_FRIENDLY)
    {

        if (bCharmedAlliesAreEnemies && tACD.IsCharmed())
        {
            return ACDIsHostile(tACD, *ptACDTarget, FALSE);
        }
        else
        {
            return ACDIsFriendly(tACD, *ptACDTarget, bCharmedAlliesAreEnemies);
        }
    }
    else
    {
        if (bCharmedAlliesAreEnemies && tACD.IsCharmed())
        {
            return ACDIsFriendly(tACD, *ptACDTarget, FALSE);
        }
        else
        {
            return ACDIsHostile(tACD, *ptACDTarget, bCharmedAlliesAreEnemies);
        }
    }
}







void TargetEvaluateCustom(Target & tTarget, ActorID idActor, SNO snoPower)



{
    tTarget.Clear();

    if (SGameGlobalsGet().dwFlags & 0x400)
    {
        return;
    }
    ActorID idTarget;
    if (snoPower != SNO_NONE)
    {
        TargetEvaluate eTargetEvaluate = PowerGeneralTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_CUSTOM_TARGET_FUNC);
        if (eTargetEvaluate != TARGETEVALUATE_NONE)
        {
            sGetTarget(tTarget, idActor, snoPower, eTargetEvaluate);
            return;
        }
    }

    idTarget = BrainAPIActorChooseTarget(idActor, snoPower, FALSE, NULL);
    if (idTarget != ACTORID_INVALID)
    {
        ACDID idACDTarget = ACDFindByActor(idTarget);
        tTarget.InitializeFromACD(idACDTarget, FALSE, FALSE);
    }
}
