// Original file: X/Server/Powers/Power_Gizmo.cpp
// Complete.
#include "./Powers.h"
#include "./PowerAPI.h"
#include "./PowerHelper.h"
#include "./PowerTrigger.h"
#include "../AI/AIStates.h"
#include "../SNPC.h"
#include "../Actors/ActorEffect.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribConstants.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Hyperlink.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Common/Target.h"
#include "../../Common/World.h"
#include "../../Common/Player.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/RefString.h"

int32 PowerContactTagGetInt(PowerID idPower, uint32 dwTag);  // X/Server/Powers/Powers.cpp:4218
BOOL PortalUse(ACDID idACDPortal, ActorCommonData & tACDOperator, const PlayerWarpReason eWarpReason);  // X/Server/TownPortal.cpp:296

// s_varIgrPerkXP (X/Common/CommunityBuffs.cpp in 2.7.7) and the community buff table.
#include "../../Common/CommunityBuffs.h"

void NPCOperate(ActorCommonData & tACDNPC, ActorCommonData & tACDPlayer, const NPCMenu ePreferredMenu, const BOOL fFromPower, const BOOL fSkipImportantConversations, const BOOL fForceOperateVendor);  // X/Server/SNPC.cpp:650
void GizmoOperate(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator, BOOL bForceOperation, BOOL bAlreadyConfirmed);  // X/Server/SGizmo.cpp:2007

void sShrine_GenericBuffs(const AttributeBuffType eABT, const PowerID idPower);
void Community_Event_Buff(PowerID idPower, void *);

//@ 50
static void sOperate(SNO snoPower, ACDID idACDOwner, ACDID idACDGizmo)



{
    ActorCommonData * ptACDGizmo = ACDTryToGet(idACDGizmo);
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (ptACDGizmo && ptACDOwner)
    {
        if (GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_NPC) == snoPower)
        {
            NPCOperate(*ptACDGizmo, *ptACDOwner, 0, TRUE, FALSE, FALSE);
        }
        else
        {
            GizmoOperate(*ptACDGizmo, ptACDOwner, FALSE, FALSE);
        }
    }
}

//@ 73
void GizmoPower(Power * ptPower, PowerMessage eMessage, void * pParam)



{
    if (ptPower->tTarget.eType != 1 /* actor target */)
    {
        return;
    }


    switch (eMessage)
    {

    case POWER_START:
    {
        PowerHelper_DoFirstTimeCosts(ptPower->id, TRUE);

        ACDID idACDGizmo = ptPower->tTarget.idACDTarget;
        ActorCommonData * ptACDGizmo = ACDTryToGet(idACDGizmo);
        if (ptACDGizmo == NULL)
        {
            PowerForceStop(ptPower->id, TRUE);
            return;
        }











        SNO snoOnClickEffectGroup = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_ON_CLICK_EFFECT, ptACDGizmo->snoActor);
        if (snoOnClickEffectGroup != SNO_NONE)
        {
            ACDPlayEffect(idACDGizmo, EFFECT_PLAY_EFFECT_GROUP, snoOnClickEffectGroup, ACDID_INVALID);
        }

        if (ptPower->dwAnimationTag != (uint32)-1)
        {

            float flUseTimeSeconds = ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_GIZMO_USE_TIME, ptACDGizmo->snoActor);
            if (flUseTimeSeconds > 0.0f)
            {
                BOOL bShowProgressBar = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_SHOW_PROGRESS_BAR, ptACDGizmo->snoActor);
                LoopingAnimationPower_StartTimer(ptPower->id, flUseTimeSeconds, bShowProgressBar, TRUE);
            }
            else
            {

                PowerAPIAnimation tAnimation;
                SigmaMemoryClear(&tAnimation, sizeof(tAnimation));

                tAnimation.idPower = ptPower->id;
                tAnimation.idACDSource = ptPower->idACD;
                tAnimation.tTarget = ptPower->tTarget;
                tAnimation.nCooldown = 0;
                tAnimation.bSetAimTarget = FALSE;
                tAnimation.bTurnToFaceTarget = FALSE;
                tAnimation.flSpeedScalar = PowerGetAttackSpeed(ptPower->id);
                tAnimation.dwDuration = (uint32)-1;
                tAnimation.dwAnimationTag = ptPower->dwAnimationTag;

                PowerAPI_DoAnimation(tAnimation);
            }
        }
        else
        {

            sOperate(ptPower->snoPower, ptPower->idACD, idACDGizmo);
            ptPower->dwFlags |= (1 << PF_INTERRUPTABLE);
            PowerDetachFromActor(ptPower->id, FALSE);
        }
        break;
    }


    case POWER_CONTACT:
    {
        ACDID idACDGizmo = ptPower->tTarget.idACDTarget;
        ActorCommonData * ptACDGizmo = ACDTryToGet(idACDGizmo);
        if (ptACDGizmo != NULL)
        {
            sOperate(ptPower->snoPower, ptPower->idACD, idACDGizmo);
        }

        ptPower->dwFlags |= (1 << PF_INTERRUPTABLE);
        PowerForceStop(ptPower->id, TRUE);
        AIStateActivateCooldown(*ptPower);
        break;
    }




    case POWER_INTERRUPT:
    case POWER_END:
        LoopingAnimationPower(ptPower, eMessage, pParam);
        break;
    }
}

//@ 178
void Shrine_Buff(PowerID idPower, void *)


{
    Target tTarget;

    PowerGetTarget(idPower, tTarget);

    PowerAPIBuffStartData tBuffStartData(idPower);

    float flBuff = PowerGetFormulaValue(idPower, TAG_POWER_SHRINE_BUFF_BONUS);
    Attrib eAttrib = PowerContactTagGetInt(idPower, TAG_POWER_SHRINE_BUFF_ATTRIBUTE);

    if (eAttrib > 0 && eAttrib < NUM_ATTRIBUTES)
    {
        ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.idACDTarget);
        if (ptACDTarget && ptACDTarget->AttributesGetInt(FastAttribKey(ATTRIBUTE_HAS_INFINITE_SHRINE_BUFFS)))
        {
            tBuffStartData.dwDurationMin = 36000;
            tBuffStartData.dwDurationDelta = 0;
        }

        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, FastAttribKey(eAttrib), flBuff);
        PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(tTarget.idACDTarget));
    }
    else
    {
        CRefString sPower = HyperlinkCreateFromSNO(SNOName(POWER_SNO, PowerGetSNO(idPower)), NULL);

    }
}

//@ 213
static void sApplyBuffToAllies(PowerAPIBuffStartData & tBuffStartData, const ACDID idACDUser)



{
    ActorCommonData & tACDUser = ACDGet(idACDUser);
    if (PowerGetFormulaValue(tBuffStartData.idPower, TAG_POWER_SHRINE_BUFF_ALLIES) != 0.0f)
    {

        WorldPlace wpUser = tACDUser.GetPlace();
        SNO snoUserLevelArea = ACDGetLevelArea(tACDUser);

        float fRadius = PowerGetFormulaValue(tBuffStartData.idPower, TAG_POWER_SHRINE_BUFF_RADIUS);
        float fRadiusSq = fRadius * fRadius;


        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (ptPlayer->idPlayerACD != ACDID_INVALID)
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDPlayer &&
                    ACDIsFriendly(*ptACDPlayer, tACDUser, FALSE) &&
                    ptACDPlayer->IsAlive() &&
                    !ptACDPlayer->IsObserver())
                {

                    WorldDistance wdDistSqBetweenPlayers = 0.0f;
                    if (ptPlayer->idPlayerACD != idACDUser)
                    {

                        WorldPlace wpPlayer = ptACDPlayer->GetPlace();
                        if (wpPlayer.idSWorld != wpUser.idSWorld)
                        {
                            continue;
                        }

                        wdDistSqBetweenPlayers = WorldPlaceDistanceSquared(wpUser, wpPlayer);
                    }


                    if (wdDistSqBetweenPlayers <= fRadiusSq ||
                        (snoUserLevelArea != SNO_NONE &&
                         ACDGetLevelArea(*ptACDPlayer) == snoUserLevelArea))
                    {
                        if (ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_HAS_INFINITE_SHRINE_BUFFS)))
                        {
                            uint32 dwDuration = tBuffStartData.dwDurationMin;
                            tBuffStartData.dwDurationMin = 36000;
                            PowerAPI_BuffAddToACD(tBuffStartData, *ptACDPlayer);
                            tBuffStartData.dwDurationMin = dwDuration;
                        }
                        else
                        {
                            PowerAPI_BuffAddToACD(tBuffStartData, *ptACDPlayer);
                        }
                    }
                }
            }
        }
    }
    else
    {
        PowerAPI_BuffAddToACD(tBuffStartData, tACDUser);
    }
}

//@ 282
void sShrine_GenericBuffs(const AttributeBuffType eABT, const PowerID idPower)


{
    const int32 MAX_GENERIC_BUFF_ATTRIBUTES = 4;
    Target tTarget;
    PowerGetTarget(idPower, tTarget);

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bPositiveEffect = true;

    Attrib eAttribs[MAX_GENERIC_BUFF_ATTRIBUTES] =
    {
        PowerContactTagGetInt(idPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_0),
        PowerContactTagGetInt(idPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_1),
        PowerContactTagGetInt(idPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_2),
        PowerContactTagGetInt(idPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_3),
    };

    float flAttributeValues[MAX_GENERIC_BUFF_ATTRIBUTES] =
    {
        PowerGetFormulaValue(idPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_FORMULA_0),
        PowerGetFormulaValue(idPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_FORMULA_1),
        PowerGetFormulaValue(idPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_FORMULA_2),
        PowerGetFormulaValue(idPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_FORMULA_3),
    };

    BOOL bSuccess = TRUE;
    int nAttributes = 0;
    for (int i = 0; i < MAX_GENERIC_BUFF_ATTRIBUTES; i++)
    {
        Attrib eAttrib = eAttribs[i];
        float flAttributeValue = flAttributeValues[i];

        if (eAttrib > 0 && eAttrib < NUM_ATTRIBUTES)
        {
            if (KeyGetDataType(FastAttribKey(eAttrib)) == 0 /* float */)
            {
                FastAttribSetValueFloat(tBuffStartData.idFastAttrib, FastAttribKey(eAttrib), flAttributeValue);
                nAttributes++;
            }
            else
            {
                FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(eAttrib), Math::FloatToInt(flAttributeValue));
            }
        }
        else if (eAttrib != -1)
        {




            CRefString sPower = HyperlinkCreateFromSNO(SNOName(POWER_SNO, PowerGetSNO(idPower)), NULL);


            bSuccess = FALSE;
        }
    }
    if (nAttributes && bSuccess)
    {
        tBuffStartData.nAppearanceParameter = eABT;
        sApplyBuffToAllies(tBuffStartData, tTarget.idACDTarget);
    }
}

//@ 350
void Shrine_Desecrated_Blessed(PowerID idPower, void *)


{
    sShrine_GenericBuffs(ABT_SHRINE_DESECRATED, idPower);
}

//@ 360
void Shrine_Desecrated_Enlightened(PowerID idPower, void *)


{
    sShrine_GenericBuffs(ABT_SHRINE_DESECRATED, idPower);
}

//@ 370
void Shrine_Desecrated_Fortune(PowerID idPower, void *)


{
    sShrine_GenericBuffs(ABT_SHRINE_DESECRATED, idPower);
}

//@ 380
void Shrine_Desecrated_Frenzied(PowerID idPower, void *)


{
    sShrine_GenericBuffs(ABT_SHRINE_DESECRATED, idPower);
}

//@ 389
void IGR_Buff(const PowerID idPower, void *)


{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL)
    {
        return;
    }

    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (ptACDOwner == NULL)
    {
        return;
    }

    float flBonusXP = s_varIgrPerkXP.Get();
    if (flBonusXP < EPSILON)
    {
        return;
    }

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = true;
    tBuffStartData.bPositiveEffect = true;

    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_EXPERIENCE_BONUS_PCT_IGR_BUFF), flBonusXP);

    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
}

//@ 422
void Community_Event_Buff(PowerID idPower, void *)


{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL)
    {
        return;
    }

    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (ptACDOwner == NULL)
    {
        return;
    }

    // 2.7.7: a temporary copy of the buff table, loaded from the Perk cvars, adds buff index 0
    // and one buff index per active community buff (CommunityBuffs::Apply).
    CommunityBuffs tCommunityBuffs;
    tCommunityBuffs.LoadFromCvars();

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = true;
    tBuffStartData.bPositiveEffect = true;
    tBuffStartData.nMaxStackCount = 1;
    tCommunityBuffs.Apply(idPower);
}

//@ 528
void Anniversary_Buff(const PowerID idPower, void *)


{
    Community_Event_Buff(idPower, NULL);
}

//@ 537
void GizmoPortalOperate(PowerID idPower, void *)


{
    ActorCommonData * ptACDOperator = PowerGetOwner(idPower);
    ACDID idACDPortal = PowerGetTargetACD(idPower);
    PortalUse(idACDPortal, *ptACDOperator, 9);
}
