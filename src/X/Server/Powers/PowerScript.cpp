#include "PowerScript.h"

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/Stopwatch.h"
#include "../AI/AIStates.h"
#include "../AI/ActorAI.h"
#include "../Actors/ActorPowers.h"
#include "../Actors/Projectile.h"
#include "../LuaScript/LuaScript.h"
#include "../LuaScript/LuaScriptUtil.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SGameConstants.h"
#include "PowerAPI.h"
#include "PowerHelper.h"
#include "PowerTrigger.h"
#include "Powers.h"
#include "SpellEffect.h"

// X/Common/Attributes/FastAttribute.cpp
void FastAttribSetValueFloat(AttribGroupID idAttribGroup, FastAttribKey tKey, float flValue);

void sScriptedAnimationPower_Contact(Power * ptPower);





const char * CREATE_FUNCTION_NAME = "OnCreate";
const char * START_FUNCTION_NAME = "OnStart";

const char * CONTACT_0_FUNCTION_NAME = "OnContact0";
const char * CONTACT_1_FUNCTION_NAME = "OnContact1";
const char * CONTACT_2_FUNCTION_NAME = "OnContact2";
const char * CONTACT_3_FUNCTION_NAME = "OnContact3";
const char * PROJECTILE_IMPACT_FUNCTION_NAME = "OnProjectileImpact";
const char * PROJECTILE_WALL_BOUNCE_FUNCTION_NAME = "OnProjectileWallBounce";
const char * PROJECTILE_TOAD_IDLE_FUNCTION_NAME = "OnProjectileToadIdle";
const char * END_FUNCTION_NAME = "OnEnd";
const char * INTERRUPT_FUNCTION_NAME = "OnInterrupt";
const char * BOSS_ENCOUNTER_CLEANUP_FUNCTION_NAME = "OnBossEncounterCleanup";




enum BuffFunctionType {
    PERIODIC_EFFECT = 0,
    ON_DO_DAMAGE_EFFECT = 1,
    ON_HIT_EFFECT = 2,
    ON_FIRST_HIT_ATTEMPT_EFFECT = 3,
    ON_GET_HIT_EFFECT = 4,
    ON_ATTACK_EFFECT = 5,
    ON_DODGE_EFFECT = 6,
    ON_COST_EFFECT = 7,
    ON_HEAL_FROM_SKILL_EFFECT = 8,
    ON_KILL_EFFECT = 9,
    ON_ASSIST_EFFECT = 10,
    ON_ITEM_PICKUP_EFFECT = 11,
    DAMAGE_SHIELD_EFFECT = 12,
    ON_EXPIRATION_EFFECT = 13,
    ON_REMOVAL_EFFECT = 14,
    ON_DEATH_EFFECT = 15,
    ON_REFLECT_EFFECT = 16,
    ON_PROJECTILE_CREATE_EFFECT = 17,
    ON_KNOCKBACKLAND_EFFECT = 18,
    ON_PET_ADD_EFFECT = 19,
    ON_MESSAGE_EFFECT = 20,
    ON_RESOURCE_GAIN_EFFECT = 21,
    ON_GET_HIT_WITH_CROWD_CONTROL_EFFECT = 22,
    ON_HIT_WITH_CROWD_CONTROL_EFFECT = 23,
    ON_EQUIPMENT_CHANGE_EFFECT = 24,
    ON_SKILL_CHANGE_EFFECT = 25,
    ON_CALLOUT_EVENT_EFFECT = 26,
    ON_GIZMO_OPERATED_EFFECT = 27,
    ON_WRECKABLE_DESTROYED_EFFECT = 28,
    ON_TELEPORTED_EFFECT = 29,
    ON_POWER_DAMAGE_INIT_EFFECT = 30,
    ON_LOOT_DROP_EFFECT = 31,
    ON_ITEM_SOLD_EFFECT = 32,
    ON_LEVEL_EFFECT = 33,
    ON_CORPSE_CONSUMED_EFFECT = 34,
    ON_PET_DISMISS_EFFECT = 35,
    NUM_BUFF_FUNCTION_NAMES = 36,
};

// The name of every buff callback, for each of the 32 buff slots. The macro name is invented;
// the table is one row per buff slot.
#define BUFF_FUNCTION_NAMES(n) { \
    "Buff" #n "PeriodicEffect", "Buff" #n "OnDoDamageEffect", "Buff" #n "OnHitEffect", \
    "Buff" #n "OnFirstHitAttemptEffect", "Buff" #n "OnGetHitEffect", "Buff" #n "OnAttackEffect", \
    "Buff" #n "OnDodgeEffect", "Buff" #n "OnCostEffect", "Buff" #n "OnHealFromSkillEffect", \
    "Buff" #n "OnKillEffect", "Buff" #n "OnAssistEffect", "Buff" #n "OnItemPickupEffect", \
    "Buff" #n "DamageShieldEffect", "Buff" #n "OnExpirationEffect", "Buff" #n "OnRemovalEffect", \
    "Buff" #n "OnDeathEffect", "Buff" #n "OnReflectEffect", "Buff" #n "OnProjectileCreateEffect", \
    "Buff" #n "OnKnockbackLandEffect", "Buff" #n "OnPetAddEffect", "Buff" #n "OnMessageEffect", \
    "Buff" #n "OnResourceGainEffect", "Buff" #n "OnGetHitWithCrowdControlEffect", \
    "Buff" #n "OnHitWithCrowdControlEffect", "Buff" #n "OnEquipmentChangeEffect", \
    "Buff" #n "OnSkillChangeEffect", "Buff" #n "OnCalloutEventEffect", \
    "Buff" #n "OnGizmoOperatedEffect", "Buff" #n "OnWreckableDestroyedEffect", \
    "Buff" #n "OnTeleportedEffect", "Buff" #n "OnPowerDamageInitEffect", "Buff" #n "OnLootDropEffect", \
    "Buff" #n "OnItemSoldEffect", "Buff" #n "OnLevelEffect", "Buff" #n "OnCorpseConsumedEffect", \
    "Buff" #n "OnPetDismissEffect", \
}





























































const char * cgarBuffFunctionNames[32][NUM_BUFF_FUNCTION_NAMES] = {
    BUFF_FUNCTION_NAMES(0), BUFF_FUNCTION_NAMES(1), BUFF_FUNCTION_NAMES(2), BUFF_FUNCTION_NAMES(3),
    BUFF_FUNCTION_NAMES(4), BUFF_FUNCTION_NAMES(5), BUFF_FUNCTION_NAMES(6), BUFF_FUNCTION_NAMES(7),
    BUFF_FUNCTION_NAMES(8), BUFF_FUNCTION_NAMES(9), BUFF_FUNCTION_NAMES(10), BUFF_FUNCTION_NAMES(11),
    BUFF_FUNCTION_NAMES(12), BUFF_FUNCTION_NAMES(13), BUFF_FUNCTION_NAMES(14), BUFF_FUNCTION_NAMES(15),
    BUFF_FUNCTION_NAMES(16), BUFF_FUNCTION_NAMES(17), BUFF_FUNCTION_NAMES(18), BUFF_FUNCTION_NAMES(19),
    BUFF_FUNCTION_NAMES(20), BUFF_FUNCTION_NAMES(21), BUFF_FUNCTION_NAMES(22), BUFF_FUNCTION_NAMES(23),
    BUFF_FUNCTION_NAMES(24), BUFF_FUNCTION_NAMES(25), BUFF_FUNCTION_NAMES(26), BUFF_FUNCTION_NAMES(27),
    BUFF_FUNCTION_NAMES(28), BUFF_FUNCTION_NAMES(29), BUFF_FUNCTION_NAMES(30), BUFF_FUNCTION_NAMES(31),
};


























static const char * sGetBuffFunctionName(BuffFunctionType eFunctionType, int32 nBuffIndex)
{


    if (nBuffIndex < 0 || nBuffIndex >= 32)
        return NULL;



    return cgarBuffFunctionNames[nBuffIndex][eFunctionType];
}

static BOOL sCheckLuaStack()
{


    return LuaScriptGetExecutingStackDepth() < 7;
}

enum ScriptParamType {
    SCRIPT_PARAM_INT = 0,
    SCRIPT_PARAM_FLOAT = 1,
    SCRIPT_PARAM_STRING = 2,
    SCRIPT_PARAM_NIL = 3,
};










struct ScriptParam {
    ScriptParamType eType;
    int32 nValue;
    float flValue;
    CRefString sValue;
};


static void sScriptedPowerPopulateAttributes(Power * ptPower, AttribGroupID idFastAttrib)


{
    if (ptPower == NULL)
        return;




    SNO snoPower = ptPower->snoPower;

    ActorCommonData * ptACDOwner = ACDTryToGet(ptPower->idACD);

    float flProjectileSpeed = PowerGetFormulaValue(ptACDOwner, AutoPowerDefinitionR(snoPower), TAG_POWER_PROJECTILE_SPEED, -1, -1, 0);
    FastAttribSetValueFloat(idFastAttrib, ATTRIBUTE_PROJECTILE_SPEED, flProjectileSpeed);
}



static ScriptInstanceID sPowerScriptStartScript(Power * ptPower, const char * szFunctionName, ScriptParam * ptParams, int32 nParams)




{

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHUTTING_DOWN))
        return SCRIPTINSTANCEID_INVALID;

    Stopwatch tStopwatch;
    float flTime;


    SNO snoPower = ptPower->snoPower;

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }





    if (!LuaScriptHasFunction(ptPower->ptLuaState, szFunctionName))
    {
        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }
        return SCRIPTINSTANCEID_INVALID;
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }



    const PowerDefinition * pdefPower = (const PowerDefinition *)gptGroupDefPower->LockR(ptPower->snoPower, FALSE);
    ScriptInstanceID idScriptInstance = LuaScriptNewInstanceWithScriptID(ptPower->idScript, szFunctionName, ptPower->ptLuaState, pdefPower->snoScript, ptPower->id, SCRIPT_ATTACH_POWER);
    gptGroupDefPower->UnlockR((void *&)pdefPower);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (idScriptInstance == SCRIPTINSTANCEID_INVALID)
        return SCRIPTINSTANCEID_INVALID;
    lua_State * ptLuaState = LuaScriptGetInstanceState(idScriptInstance);






    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (LuaScriptGetFunction(ptLuaState, szFunctionName))
    {
        LuaStackChecker tStackCheck(ptLuaState, PowerReuseScriptState(AutoPowerDefinitionR(snoPower)) ? 1 : 0);
        int32 nNumParams = 0;
































        if (ptParams != NULL)
        {
            for (int32 i = 0; i < nParams; i++)
            {
                switch (ptParams[i].eType)
                {
                    case SCRIPT_PARAM_INT:
                        lua_pushinteger(ptLuaState, ptParams[i].nValue);
                        break;

                    case SCRIPT_PARAM_FLOAT:
                        lua_pushnumber(ptLuaState, ptParams[i].flValue);
                        break;

                    case SCRIPT_PARAM_STRING:
                        lua_pushstring(ptLuaState, ptParams[i].sValue.str());
                        break;

                    case SCRIPT_PARAM_NIL:
                        lua_pushnil(ptLuaState);
                        break;

                    default:
                        DisplayInternalError(NULL, "", 408, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                }
                nNumParams++;
            }
        }
        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }

        ptPower->nScriptCalls++;


        LuaScriptSetInstanceResumeTick(idScriptInstance, GameGetActiveGameCommonData().dwServerTime, nNumParams);

        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }

        LuaTickScriptInstanceId(idScriptInstance);

        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }
    }

    lua_State * ptLuaCheckState = LuaScriptTryToGetLuaStateWithScriptInstanceID(idScriptInstance);
    if (ptLuaCheckState != NULL)
        ptPower->listScriptInstanceIDs.AddTail_Tracked(idScriptInstance);

    return idScriptInstance;
}




void sScriptedPower_Create(Power * ptPower)
{

    if (ptPower == NULL)
        return;




    Stopwatch tStopwatch;
    float flTime;
    SNO snoPower = ptPower->snoPower;


    const PowerDefinition * pdefPower = (const PowerDefinition *)gptGroupDefPower->LockR(snoPower, FALSE);
    ptPower->snoScript = pdefPower->snoScript;
    gptGroupDefPower->UnlockR((void *&)pdefPower);

    if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }


    if (PowerReuseScriptState(AutoPowerDefinitionR(snoPower)))
    {
        PowerGlobals *& ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
        ScriptID idScript;

        if (!ptPowerGlobals->tScriptIDs.Lookup(ptPower->snoScript, idScript))
        {
            idScript = LuaScriptCreateState(ptPower->snoScript, 0xffff, NULL);
            ptPowerGlobals->tScriptIDs.SetAt_Tracked(ptPower->snoScript, idScript);
        }

        ptPower->idScript = idScript;
    }
    else
    {



        ptPower->idScript = LuaScriptCreateState(ptPower->snoScript, 0xffff, NULL);
    }







    if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    lua_State * ptLuaState = LuaScriptTryToGetLuaStateWithScriptID(ptPower->idScript);
    if (ptLuaState == NULL)
        return;






    ptPower->ptLuaState = ptLuaState;

    if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }







    ptPower->tTargetCriteria.eLifeStatus = TCLS_ALIVE_ONLY;
    ptPower->tTargetCriteria.dwTeamRelationshipFlags = (1 << TCTR_IGNORES_WRECKABLES);

    if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }


    sPowerScriptStartScript(ptPower, CREATE_FUNCTION_NAME, NULL, 0);

    if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

}



void sScriptedPower_Start(Power * ptPower, void * pParam)
{


    if (ptPower == NULL)
        return;




    ScriptParam scriptParams[1];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = (int32)(intptr_t)pParam;


    sPowerScriptStartScript(ptPower, START_FUNCTION_NAME, scriptParams, 1);

    PowerHelper_HitSoundInit(ptPower);
}



void sScriptedAnimationPower_Start(Power * ptPower)
{

    if (ptPower == NULL)
        return;



    sScriptedPower_Start(ptPower, NULL);


    if (ptPower->bDoesntAnimate)
    {

        PowerHelper_SetDefaultContactAnimations(ptPower->id, ptPower->dwAnimationTag);

        sScriptedAnimationPower_Contact(ptPower);
        return;
    }

    PowerAPIAnimation tAnimation;
    PowerHelper_AnimationInit(ptPower->id, tAnimation);
    tAnimation.tPreplayedAnimData = ptPower->tPreplayedAnimData;

    ActorCommonData & tACDMainActor = ACDGet(ptPower->idACD);
    BOOL bInfAnimTiming = PowerHasInfiniteAnimationTiming(tACDMainActor, AutoPowerDefinitionR(ptPower->snoPower));
    if (bInfAnimTiming || PowerIsChannelled(tACDMainActor, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        ptPower->dwFlags |= (1 << PF_MANUALLY_FINISH);
    }
    if (bInfAnimTiming)
    {
        tAnimation.dwDuration = (uint32)-2;
    }


    PowerAPI_DoAnimation(tAnimation);



    if (PowerCanMoveForwardBeforeContactFrame(AutoPowerDefinitionR(ptPower->snoPower)) &&
        (ptPower->dwFlags & (1 << PF_CONSOLE_MOVE_TOWARDS_TARGET)))
    {
        AnimationPower_CloseOnTarget(ptPower->id);
    }

}



void sScriptedAnimationPower_Contact(Power * ptPower)
{

    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);

    if (ptACDOwner == NULL)
        return;



    if (!ptPower->tTarget.IsNone() &&
        ptPower->tTarget.IsInvalidForSource(ptACDOwner))
        return;



    if (!PowerHasInfiniteAnimationTiming(*ptACDOwner, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        AIQueueCheckActionCompleted((ActorID)ptACDOwner->idOwner, ptPower->snoPower, QUEUED_ACTION_POWER);
    }

    switch (ptPower->dwCurrentContactFrame)
    {
        case 0: sPowerScriptStartScript(ptPower, CONTACT_0_FUNCTION_NAME, NULL, 0); break;
        case 1: sPowerScriptStartScript(ptPower, CONTACT_1_FUNCTION_NAME, NULL, 0); break;
        case 2: sPowerScriptStartScript(ptPower, CONTACT_2_FUNCTION_NAME, NULL, 0); break;
        case 3: sPowerScriptStartScript(ptPower, CONTACT_3_FUNCTION_NAME, NULL, 0); break;
        default:
            DisplayInternalError(NULL, "", 660, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    AIStateActivateCooldown(*ptPower);
}



void sScriptedPower_Interrupt(Power * ptPower)
{

    sPowerScriptStartScript(ptPower, INTERRUPT_FUNCTION_NAME, NULL, 0);

    AIStateActivateCooldown(*ptPower);
}



void sScriptedPower_BossEncounterCleanup(Power * ptPower)
{

    sPowerScriptStartScript(ptPower, BOSS_ENCOUNTER_CLEANUP_FUNCTION_NAME, NULL, 0);
}




void sScriptedPower_End(Power * ptPower)
{

    sPowerScriptStartScript(ptPower, END_FUNCTION_NAME, NULL, 0);
}



void sScriptedAnimationPower_OnImpact(
    Power * ptPower,
    void * pParam)
{
    if (ptPower == NULL)
        return;



    ImpactData * ptImpact = (ImpactData *)pParam;





    if (SpellEffect_ProjectileTryReflect(ptPower->id, ptImpact))
        return;



    Target tImpactTarget;
    SpellEffect_GetProjectileImpactData(ptImpact->idACDProjectile,
                                        ptImpact->idACDIntersected, tImpactTarget);






    if (ProjectileIsReflected(ptImpact->idACDProjectile))
    {
        ptPower->dwFlags |= (1 << PF_WAS_REFLECTED);
        ptPower->idACDReflectedBy = ProjectileGetReflectedByACD(ptImpact->idACDProjectile);
    }

    if (ACDTryToGet(ptImpact->idACDIntersected) != NULL)
    {
        PowerAddAffectedACD(ptPower->id, ptImpact->idACDIntersected, ptImpact->idACDProjectile);
    }



    int32 nTargetsHit = ProjectileActorGetHitListCount(ptImpact->idACDProjectile);
    int32 nMaxTargetsHit = PowerGetMaxProjectileImpactCount();

    if (nTargetsHit < nMaxTargetsHit || ptImpact->idACDIntersected == ACDID_INVALID)
    {

        ScriptParam scriptParams[2];
        scriptParams[0].eType = SCRIPT_PARAM_INT;
        scriptParams[0].nValue = ptImpact->idACDProjectile;
        scriptParams[1].eType = SCRIPT_PARAM_INT;
        scriptParams[1].nValue = ptImpact->idACDIntersected;

        sPowerScriptStartScript(ptPower, PROJECTILE_IMPACT_FUNCTION_NAME, scriptParams, 2);
    }

    BOOL fDoPierce = SpellEffect_CalculatePierce(ptPower->id, ptImpact->idACDProjectile, ptImpact->idACDIntersected);



    BOOL fDestroyProjectile = !fDoPierce && ProjectileDestroyOnImpact(ptImpact->idACDProjectile);



    if (fDestroyProjectile)
    {
        PowerAPIDestroyProjectile tProjectile;

        if (nTargetsHit >= nMaxTargetsHit)
        {
            ActorCommonData * ptACDProjectile = ACDTryToGet(ptImpact->idACDProjectile);

            CRefString sError;
            sError.Format("sScriptedAnimationPower_OnImpact: ProjectileSNO (%s) from PowerSNO (%s) Power ID (ID - %d) collided with %d targets (capped at %d)!\n",
                ptACDProjectile ? SNOToString(POWER_SNO, ptACDProjectile->snoActor, FALSE).str() : "[unknown]",
                SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str(),
                ptPower->id, nTargetsHit, nMaxTargetsHit);




        }


        PowerHelper_StopProjectileInit(ptPower->id, ptImpact->idACDProjectile, ptImpact->idACDIntersected, tProjectile);
        PowerAPI_DestroyProjectile(tProjectile);
    }
    else
    {

        ptImpact->bStop = FALSE;
    }

    ptPower->dwFlags &= ~(1 << PF_WAS_REFLECTED);
}



void sScriptedAnimationPower_OnWallBounce(Power * ptPower, void * pParam)
{


    if (ptPower == NULL)
        return;




    ScriptParam scriptParams[1];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = (int32)(intptr_t)pParam;

    sPowerScriptStartScript(ptPower, PROJECTILE_WALL_BOUNCE_FUNCTION_NAME, scriptParams, 1);
}



void sScriptedAnimationPower_OnToadIdle(Power * ptPower, void * pParam)
{


    if (ptPower == NULL)
        return;




    ScriptParam scriptParams[1];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = (int32)(intptr_t)pParam;

    sPowerScriptStartScript(ptPower, PROJECTILE_TOAD_IDLE_FUNCTION_NAME, scriptParams, 1);
}



void ScriptedAnimationPowerMessageHandler(
    Power * ptPower,
    PowerMessage eMessage,
    void * pParam)
{








    switch (eMessage)
    {
        case POWER_POPULATE_ATTRIBUTES:
            sScriptedPowerPopulateAttributes(ptPower, (AttribGroupID)(intptr_t)pParam);
            break;



        case POWER_CREATE:
            sScriptedPower_Create(ptPower);
            break;
        case POWER_START:
            sScriptedAnimationPower_Start(ptPower);
            break;
        case POWER_CONTACT:
            sScriptedAnimationPower_Contact(ptPower);
            break;
        case POWER_IMPACT:
            sScriptedAnimationPower_OnImpact(ptPower, pParam);
            break;



        case POWER_INTERRUPT:
            sScriptedPower_Interrupt(ptPower);
            break;
        case POWER_END:
            sScriptedPower_End(ptPower);
            break;
        case POWER_WALL_BOUNCE:
            sScriptedAnimationPower_OnWallBounce(ptPower, pParam);
            break;
        case POWER_TOAD_IDLE:
            sScriptedAnimationPower_OnToadIdle(ptPower, pParam);
            break;
        case POWER_BOSS_ENCOUNTER_CLEANUP:
            sScriptedPower_BossEncounterCleanup(ptPower);
            break;
    }









}



void ScriptedNonAnimationPowerMessageHandler(
    Power * ptPower,
    PowerMessage eMessage,
    void * pParam)
{
    switch (eMessage)
    {
        case POWER_POPULATE_ATTRIBUTES:
            sScriptedPowerPopulateAttributes(ptPower, (AttribGroupID)(intptr_t)pParam);
            break;
        case POWER_CREATE:
            sScriptedPower_Create(ptPower);
            break;
        case POWER_START:
            sScriptedPower_Start(ptPower, pParam);
            // A non-animating power makes contact as soon as it starts.
            sScriptedAnimationPower_Contact(ptPower);
            break;
        case POWER_IMPACT:
            sScriptedAnimationPower_OnImpact(ptPower, pParam);
            break;
        case POWER_INTERRUPT:
            sScriptedPower_Interrupt(ptPower);
            break;




        case POWER_WALL_BOUNCE:
            sScriptedAnimationPower_OnWallBounce(ptPower, pParam);
            break;
        case POWER_TOAD_IDLE:
            sScriptedAnimationPower_OnToadIdle(ptPower, pParam);
            break;
        case POWER_BOSS_ENCOUNTER_CLEANUP:
            sScriptedPower_BossEncounterCleanup(ptPower);
            break;
    }
}









void ScriptedSpecialWalkPowerMessageHandler(
    Power * ptPower,
    PowerMessage eMessage,
    void * pParam)
{
    switch (eMessage)
    {
        case POWER_POPULATE_ATTRIBUTES:
            PowerHelper_PopulateCommonAttributes(ptPower->id, (AttribGroupID)(intptr_t)pParam);
            break;
        case POWER_UI_DAMAGE_NUMBERS:
            PowerHelper_AttackPayloadInit_Standard(ptPower->id, *(PowerAPIAttackPayload *)pParam);
            break;
        case POWER_CREATE:
            sScriptedPower_Create(ptPower);
            break;
        case POWER_START:
        {

            ActorCommonData * ptACD = PowerGetOwner(ptPower);
            ActorID idActor = ptACD ? (ActorID)ptACD->idOwner : ACTORID_INVALID;

            PowerID idPrimaryPower = ActorPowersGetPrimary(idActor);






            if (SpecialWalkPower_Start(ptPower->id, pParam))
            {
                idPrimaryPower = ActorPowersGetPrimary(idActor);
                if (ptPower->id == idPrimaryPower)
                {






                    sScriptedPower_Start(ptPower, NULL);
                    break;
                }
            }

            PowerInterrupt(ptPower->id, FALSE);
        }
        break;

        case POWER_IMPACT:
            sScriptedAnimationPower_OnImpact(ptPower, pParam);
            break;
        case POWER_ARRIVED:
            if (!SpecialWalkPower_End(ptPower->id, pParam))
                break;
        case POWER_END:
            sScriptedPower_End(ptPower);
            break;
        case POWER_UPDATE_TARGET:
            PowerHelper_UpdateTargetStandard(ptPower->id, (const UpdateTargetData *)pParam);
            break;
        case POWER_INTERRUPT:
            sScriptedPower_Interrupt(ptPower);
            break;







        case POWER_CONTACT:
            switch (ptPower->dwCurrentContactFrame)
            {
                case 0: sPowerScriptStartScript(ptPower, CONTACT_0_FUNCTION_NAME, NULL, 0); break;
                case 1: sPowerScriptStartScript(ptPower, CONTACT_1_FUNCTION_NAME, NULL, 0); break;
                case 2: sPowerScriptStartScript(ptPower, CONTACT_2_FUNCTION_NAME, NULL, 0); break;
                case 3: sPowerScriptStartScript(ptPower, CONTACT_3_FUNCTION_NAME, NULL, 0); break;
                default:
                    DisplayInternalError(NULL, "", 1025, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
            break;

        case POWER_WALL_BOUNCE:
            sScriptedAnimationPower_OnWallBounce(ptPower, pParam);
            break;
        case POWER_TOAD_IDLE:
            sScriptedAnimationPower_OnToadIdle(ptPower, pParam);
            break;
        case POWER_BOSS_ENCOUNTER_CLEANUP:
            sScriptedPower_BossEncounterCleanup(ptPower);
            break;
    }
}





void ScriptedLoopingAnimationPowerMessageHandler(
    Power * ptPower,
    PowerMessage eMessage,
    void * pParam)
{
    switch (eMessage)
    {
        case POWER_CREATE:
            sScriptedPower_Create(ptPower);
            break;
        case POWER_POPULATE_ATTRIBUTES:
            sScriptedPowerPopulateAttributes(ptPower, (AttribGroupID)(intptr_t)pParam);
            break;




        case POWER_START:
            SpellEffectRunBeginEffect(ptPower->id, pParam);
            if (ptPower != NULL && !(ptPower->dwFlags & (1 << PF_STOPPING)))
            {
                float flSeconds = PowerGetFormulaValue(ptPower->id, TAG_POWER_LOOPING_ANIMATION_TIME);
                LoopingAnimationPower_StartTimer(ptPower->id, flSeconds, TRUE, TRUE);
            }
            break;

        case POWER_CONTACT:
            sScriptedAnimationPower_Contact(ptPower);
            PowerForceStop(ptPower->id, TRUE);
            break;
        case POWER_IMPACT:
            sScriptedAnimationPower_OnImpact(ptPower, pParam);
            break;
        case POWER_INTERRUPT:
            LoopingAnimationPower_RemoveBuff(ptPower->id);
            if (PowerGetInterruptedEffect(ptPower->id) != SPELL_INVALID)
            {
                SpellEffectRunInterruptedEffect(ptPower->id, NULL);
            }
            AIStateActivateCooldown(*ptPower);
            break;
        case POWER_UPDATE_TARGET:
            PowerHelper_UpdateTargetStandard(ptPower->id, (const UpdateTargetData *)pParam);
            break;
        case POWER_END:
            LoopingAnimationPower_RemoveBuff(ptPower->id);
            ptPower->dwFlags &= ~(1 << PF_MANUALLY_FINISH);
            sScriptedPower_End(ptPower);
            break;
        case POWER_WALL_BOUNCE:
            sScriptedAnimationPower_OnWallBounce(ptPower, pParam);
            break;
        case POWER_TOAD_IDLE:
            sScriptedAnimationPower_OnToadIdle(ptPower, pParam);
            break;
    }
}





void ScriptedPower_BuffPeriodicSpellEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void *)
{
    PowerID idPower = ptBuff->idPower;
    Power * ptPower = PowerGet(idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }


    ScriptParam scriptParams[1];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = ptBuff->idACDAffected;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(PERIODIC_EFFECT, ptBuff->nBuffIndex), scriptParams, 1);
    ptPower->listActiveCallbackBuffs.RemoveHead();

}



void ScriptedPower_BuffOnDoDamageEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    PowerAPIDamage & tPowerDamage,
    float * flDamageDone,
    ACDID idACDAttacker,
    ACDID idACDTarget)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    uint32 dwOldPowerFlags = ptPower->dwFlags;

    ptPower->dwFlags |= (1 << PF_DISALLOW_SCRIPT_WAIT) | (1 << PF_DISALLOW_PAYLOAD_APPLICATION);

    ptPower->listPowerAPIDamage.AddHead_Tracked(&tPowerDamage);
    ptPower->listDamage.AddHead_Tracked(flDamageDone);

    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDTarget;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_DO_DAMAGE_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();

    ptPower->dwFlags = dwOldPowerFlags;
    ptPower->listPowerAPIDamage.RemoveHead();
    ptPower->listDamage.RemoveHead();
}



void ScriptedPower_BuffOnGetHitEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    PowerAPIDamage & tPowerDamage,
    float flHitpointDamage,
    ACDID idACDAttacker,
    ACDID idACDTarget)
{


    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[4];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDTarget;
    scriptParams[2].eType = SCRIPT_PARAM_FLOAT;
    scriptParams[2].flValue = flHitpointDamage;
    scriptParams[3].eType = SCRIPT_PARAM_INT;
    scriptParams[3].nValue = tPowerDamage.eDeliveryMechanism;

    ptPower->listPowerAPIDamage.AddHead_Tracked(&tPowerDamage);

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_GET_HIT_EFFECT, ptBuff->nBuffIndex), scriptParams, 4);
    ptPower->listActiveCallbackBuffs.RemoveHead();

    ptPower->listPowerAPIDamage.RemoveHead();
}



void ScriptedPower_BuffOnHitEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    PowerAPIDamage & tPowerDamage,
    float flHitpointDamage,
    float * flDamageDone,
    ACDID idACDAttacker,
    ACDID idACDTarget,
    ACDID idACDFromPet)
{


    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[5];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDTarget;
    scriptParams[2].eType = SCRIPT_PARAM_FLOAT;
    scriptParams[2].flValue = flHitpointDamage;
    scriptParams[3].eType = SCRIPT_PARAM_INT;
    scriptParams[3].nValue = tPowerDamage.eDeliveryMechanism;
    scriptParams[4].eType = SCRIPT_PARAM_INT;
    scriptParams[4].nValue = idACDFromPet;

    ptPower->listPowerAPIDamage.AddHead_Tracked(&tPowerDamage);
    ptPower->listDamage.AddHead_Tracked(flDamageDone);

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_HIT_EFFECT, ptBuff->nBuffIndex), scriptParams, 5);
    ptPower->listActiveCallbackBuffs.RemoveHead();

    ptPower->listPowerAPIDamage.RemoveHead();
    ptPower->listDamage.RemoveHead();
}



void ScriptedPower_BuffOnFirstHitAttemptEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    ACDID idACDAttacker,
    ACDID idACDTarget,
    PowerID idFirstHitPower,
    SNO snoFirstHitPower)
{


    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[4];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDTarget;
    scriptParams[2].eType = SCRIPT_PARAM_INT;
    scriptParams[2].nValue = idFirstHitPower;
    scriptParams[3].eType = SCRIPT_PARAM_INT;
    scriptParams[3].nValue = snoFirstHitPower;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_FIRST_HIT_ATTEMPT_EFFECT, ptBuff->nBuffIndex), scriptParams, 4);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnAttackEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    ACDID idACDAttacker,
    ACDID idACDTarget,
    PowerID idPowerFrom,
    SNO snoPower)
{






    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[4];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDTarget;
    scriptParams[2].eType = SCRIPT_PARAM_INT;
    scriptParams[2].nValue = snoPower;
    scriptParams[3].eType = SCRIPT_PARAM_INT;
    scriptParams[3].nValue = idPowerFrom;

    PowerAPIDamage tDamage;
    PowerHelper_DamageInit(tDamage, idPowerFrom, idACDAttacker);
    ptPower->listPowerAPIDamage.AddHead_Tracked(&tDamage);

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_ATTACK_EFFECT, ptBuff->nBuffIndex), scriptParams, 4);
    ptPower->listActiveCallbackBuffs.RemoveHead();

    ptPower->listPowerAPIDamage.RemoveHead();
}



void ScriptedPower_BuffOnDodgeEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    ACDID idACDAttacker,
    ACDID idACDTarget,
    const PowerAPIAttackPayload & tPayload)
{






    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDTarget;

    PowerAPIDamage tDamage;
    PowerHelper_DamageInit(tDamage, tPayload.idPower, idACDTarget);
    tDamage.eDeliveryMechanism = tPayload.eDeliveryMechanism;
    tDamage.flProcScalar = tPayload.flProcScalar;
    ptPower->listPowerAPIDamage.AddHead_Tracked(&tDamage);

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_DODGE_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();

    ptPower->listPowerAPIDamage.RemoveHead();
}



void ScriptedPower_BuffOnCostEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    ACDID idACD,
    PowerID idPowerFrom,
    SNO snoPower,
    float flResourceCost,
    ResourceType eResourceType,
    float flHealthCost,
    BOOL bCostsWereCharged,
    float flHealthCostUndiscounted)
{





    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[8];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACD;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = snoPower;
    scriptParams[2].eType = SCRIPT_PARAM_FLOAT;
    scriptParams[2].flValue = flResourceCost;
    scriptParams[3].eType = SCRIPT_PARAM_INT;
    scriptParams[3].nValue = eResourceType;
    scriptParams[4].eType = SCRIPT_PARAM_FLOAT;
    scriptParams[4].flValue = flHealthCost;
    scriptParams[5].eType = SCRIPT_PARAM_INT;
    scriptParams[5].nValue = idPowerFrom;
    scriptParams[6].eType = SCRIPT_PARAM_INT;
    scriptParams[6].nValue = bCostsWereCharged;
    scriptParams[7].eType = SCRIPT_PARAM_FLOAT;
    scriptParams[7].flValue = flHealthCostUndiscounted;



    PowerAPIDamage tDamage;
    PowerHelper_DamageInit(tDamage, idPowerFrom, idACD);
    ptPower->listPowerAPIDamage.AddHead_Tracked(&tDamage);

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_COST_EFFECT, ptBuff->nBuffIndex), scriptParams, 8);
    ptPower->listActiveCallbackBuffs.RemoveHead();

    ptPower->listPowerAPIDamage.RemoveHead();
}



void ScriptedPower_BuffOnHealFromSkillEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACD,
    PowerID idPowerFrom,
    SNO snoPower,
    float flHealAmount,
    float flOverhealAmount,
    int32 nNumberOfHeals)
{


    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[5];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACD;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = snoPower;
    scriptParams[2].eType = SCRIPT_PARAM_FLOAT;
    scriptParams[2].flValue = flHealAmount;
    scriptParams[3].eType = SCRIPT_PARAM_FLOAT;
    scriptParams[3].flValue = flOverhealAmount;
    scriptParams[4].eType = SCRIPT_PARAM_INT;
    scriptParams[4].nValue = nNumberOfHeals;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_HEAL_FROM_SKILL_EFFECT, ptBuff->nBuffIndex), scriptParams, 5);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffDamageShieldEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDTarget,
    float * flDamageDone,
    PowerAPIDamage & tPowerDamage,
    void * pData)
{


    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDTarget;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = tPowerDamage.idACDSource;

    ptPower->listPowerAPIDamage.AddHead_Tracked(&tPowerDamage);
    ptPower->listDamage.AddHead_Tracked(flDamageDone);

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(DAMAGE_SHIELD_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();

    ptPower->listPowerAPIDamage.RemoveHead();
    ptPower->listDamage.RemoveHead();
}



void ScriptedPower_BuffOnKillEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    float * flDamageDone,
    PowerAPIDamage * ptPowerDamage,
    ACDID idACDAttacker,
    ACDID idACDKilled)
{





    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDKilled;

    if (ptPowerDamage != NULL)
    {
        ptPower->listPowerAPIDamage.AddHead_Tracked(ptPowerDamage);
    }
    else
    {
        PowerAPIDamage tDamage;  // the list keeps &tDamage past this block, as in the original
        PowerHelper_DamageInit(tDamage, POWERID_INVALID, idACDKilled);
        ptPower->listPowerAPIDamage.AddHead_Tracked(&tDamage);
    }

    if (flDamageDone != NULL)
    {
        ptPower->listDamage.AddHead_Tracked(flDamageDone);
    }
    else
    {
        float flZeroDamageDone[NUM_DAMAGE_TYPES];
        SigmaMemoryClear(flZeroDamageDone, sizeof(flZeroDamageDone));
        ptPower->listDamage.AddHead_Tracked(flZeroDamageDone);
    }

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_KILL_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();

    ptPower->listPowerAPIDamage.RemoveHead();
    ptPower->listDamage.RemoveHead();
}




void ScriptedPower_BuffOnAssistEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    ACDID idACDAttacker,
    ACDID idACDKilled)
{





    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDKilled;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_ASSIST_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnItemPickUpEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    ACDID idACD,
    ACDID idACDItem,
    PickupAction ePickupAction)
{



    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACD;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDItem;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_ITEM_PICKUP_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnDeathEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    ACDID idACDTarget,
    ACDID idACDKiller)
{


    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDTarget;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDKiller;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_DEATH_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();

}



void ScriptedPower_BuffOnExpirationEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDTarget,
    void * pData)
{


    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[1];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDTarget;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_EXPIRATION_EFFECT, ptBuff->nBuffIndex), scriptParams, 1);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnRemovalEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    BuffRemovalReason eBuffRemovalReason,
    void * pData)
{


    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = ptBuff->idACDAffected;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = eBuffRemovalReason;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_REMOVAL_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



BOOL ScriptedPower_BuffOnReflectEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    ImpactData * ptImpact,
    void * pData)
{


    Power * ptPower = PowerGet(ptBuff->idPower);

    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return FALSE;
    }



    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = ptBuff->idACDAffected;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = ptImpact->idACDProjectile;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_REFLECT_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();


    return TRUE;

}



void ScriptedPower_BuffOnProjectileCreateEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    ACDID idACDAttacker,
    ACDID idACDProjectile)
{


    Power * ptPower = PowerGet(ptBuff->idPower);

    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDProjectile;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_PROJECTILE_CREATE_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnKnockbackLandEffectCallback(
    PowerAPIBuffInstance * ptBuff)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[1];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = ptBuff->idACDAffected;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_KNOCKBACKLAND_EFFECT, ptBuff->nBuffIndex), scriptParams, 1);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnPetAddEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDSummoner,
    ACDID idACDPet,
    PetType ePetType,
    SNO snoPet)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }



    ScriptParam scriptParams[4];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDSummoner;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDPet;

    scriptParams[2].eType = SCRIPT_PARAM_INT;
    scriptParams[2].nValue = ePetType;

    scriptParams[3].eType = SCRIPT_PARAM_INT;
    scriptParams[3].nValue = snoPet;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_PET_ADD_EFFECT, ptBuff->nBuffIndex), scriptParams, 4);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnResourceGainEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    float flResourceAdded)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = ptBuff->idACDAffected;

    scriptParams[1].eType = SCRIPT_PARAM_FLOAT;
    scriptParams[1].flValue = flResourceAdded;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_RESOURCE_GAIN_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnGetHitWithCrowdControlEffectCallback(
    PowerID idPowerFrom,
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDAttacker,
    ACDID idACDTarget,
    SNO snoPower,
    CrowdControlType eCCtype,
    float flDuration,
    float flProcScalar)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[5];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDTarget;

    scriptParams[2].eType = SCRIPT_PARAM_INT;
    scriptParams[2].nValue = snoPower;

    scriptParams[3].eType = SCRIPT_PARAM_INT;
    scriptParams[3].nValue = eCCtype;

    scriptParams[4].eType = SCRIPT_PARAM_FLOAT;
    scriptParams[4].flValue = flDuration;

    PowerAPIDamage tDamage;
    PowerHelper_DamageInit(tDamage, idPowerFrom, idACDAttacker);
    tDamage.flProcScalar = flProcScalar;
    ptPower->listPowerAPIDamage.AddHead_Tracked(&tDamage);

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_GET_HIT_WITH_CROWD_CONTROL_EFFECT, ptBuff->nBuffIndex), scriptParams, 5);
    ptPower->listActiveCallbackBuffs.RemoveHead();

    ptPower->listPowerAPIDamage.RemoveHead();
}



void ScriptedPower_BuffOnHitWithCrowdControlEffectCallback(
    PowerID idPowerFrom,
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDAttacker,
    ACDID idACDTarget,
    SNO snoPower,
    CrowdControlType eCCtype,
    float flDuration,
    float flProcScalar)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[5];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDTarget;

    scriptParams[2].eType = SCRIPT_PARAM_INT;
    scriptParams[2].nValue = snoPower;

    scriptParams[3].eType = SCRIPT_PARAM_INT;
    scriptParams[3].nValue = eCCtype;

    scriptParams[4].eType = SCRIPT_PARAM_FLOAT;
    scriptParams[4].flValue = flDuration;

    PowerAPIDamage tDamage;
    PowerHelper_DamageInit(tDamage, idPowerFrom, idACDAttacker);
    tDamage.flProcScalar = flProcScalar;
    ptPower->listPowerAPIDamage.AddHead_Tracked(&tDamage);

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_HIT_WITH_CROWD_CONTROL_EFFECT, ptBuff->nBuffIndex), scriptParams, 5);
    ptPower->listActiveCallbackBuffs.RemoveHead();

    ptPower->listPowerAPIDamage.RemoveHead();
}



void ScriptedPower_BuffOnEquipmentChangeEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDActor,
    InventorySlotType eSlot,
    EquipmentChangeReason eReason)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[3];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDActor;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = eSlot;

    scriptParams[2].eType = SCRIPT_PARAM_INT;
    scriptParams[2].nValue = eReason;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_EQUIPMENT_CHANGE_EFFECT, ptBuff->nBuffIndex), scriptParams, 3);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnSkillChangeEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDActor)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[1];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDActor;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_SKILL_CHANGE_EFFECT, ptBuff->nBuffIndex), scriptParams, 1);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnCalloutEventCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDActor,
    CalloutEventType eEvent)
{
    Power * ptPower = PowerGet(ptBuff->idPower);

    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDActor;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = eEvent;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_CALLOUT_EVENT_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnGizmoOperatedCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDActor,
    ActorCommonData & tACDGizmo)
{
    Power * ptPower = PowerGet(ptBuff->idPower);

    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[3];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDActor;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = tACDGizmo.id;

    scriptParams[2].eType = SCRIPT_PARAM_INT;
    scriptParams[2].nValue = tACDGizmo.GetGizmoType();

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_GIZMO_OPERATED_EFFECT, ptBuff->nBuffIndex), scriptParams, 3);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnWreckableDestroyedCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDAttacker,
    ACDID idACDKilled)
{
    Power * ptPower = PowerGet(ptBuff->idPower);

    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAttacker;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDKilled;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_WRECKABLE_DESTROYED_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnTeleportedCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDActor)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[1];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDActor;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_TELEPORTED_EFFECT, ptBuff->nBuffIndex), scriptParams, 1);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnPowerDamageInitCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDSource,
    ACDID idACDTarget,
    PowerAPIDamage & tPowerDamage)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[2];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDSource;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDTarget;

    ptPower->listPowerAPIDamage.AddHead_Tracked(&tPowerDamage);

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_POWER_DAMAGE_INIT_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();

    ptPower->listPowerAPIDamage.RemoveHead();
}



void ScriptedPower_BuffOnLootDropCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDPlayer,
    ActorCommonData & tACDItem,
    LootContext eLootContext)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[4];

    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDPlayer;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = tACDItem.id;

    scriptParams[2].eType = SCRIPT_PARAM_INT;
    scriptParams[2].nValue = tACDItem.hGB.gbid;

    scriptParams[3].eType = SCRIPT_PARAM_INT;
    scriptParams[3].nValue = eLootContext;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_LOOT_DROP_EFFECT, ptBuff->nBuffIndex), scriptParams, 4);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}




void ScriptedPower_BuffOnItemSoldCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDPlayer,
    ActorCommonData & tACDItem)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[3];

    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDPlayer;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = tACDItem.id;

    scriptParams[2].eType = SCRIPT_PARAM_INT;
    scriptParams[2].nValue = tACDItem.hGB.gbid;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_ITEM_SOLD_EFFECT, ptBuff->nBuffIndex), scriptParams, 3);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnLevelCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACD)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[1];

    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACD;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_LEVEL_EFFECT, ptBuff->nBuffIndex), scriptParams, 1);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnCorpseConsumedCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACD,
    ACDID idACDCorpse)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[2];

    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACD;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDCorpse;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_CORPSE_CONSUMED_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnPetDismissCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACD,
    ACDID idACDPet)
{
    Power * ptPower = PowerGet(ptBuff->idPower);


    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }

    if (ACDTryToGet(ptBuff->idACDAffected) == NULL)
        return;






    ScriptParam scriptParams[2];

    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACD;
    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = idACDPet;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_PET_DISMISS_EFFECT, ptBuff->nBuffIndex), scriptParams, 2);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}



void ScriptedPower_BuffOnMessageEffectCallback(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDAffected,
    int32 dwParam0,
    int32 dwParam1,
    float flParam2)
{
    Power * ptPower = PowerGet(ptBuff->idPower);

    if (!sCheckLuaStack())
    {

        LuaScriptPrintExecutingStackString();
        return;
    }




    ScriptParam scriptParams[4];
    scriptParams[0].eType = SCRIPT_PARAM_INT;
    scriptParams[0].nValue = idACDAffected;

    scriptParams[1].eType = SCRIPT_PARAM_INT;
    scriptParams[1].nValue = dwParam0;

    scriptParams[2].eType = SCRIPT_PARAM_INT;
    scriptParams[2].nValue = dwParam1;

    scriptParams[3].eType = SCRIPT_PARAM_FLOAT;
    scriptParams[3].flValue = flParam2;

    ptPower->listActiveCallbackBuffs.AddHead_Tracked(ptBuff->id);
    sPowerScriptStartScript(ptPower, sGetBuffFunctionName(ON_MESSAGE_EFFECT, ptBuff->nBuffIndex), scriptParams, 4);
    ptPower->listActiveCallbackBuffs.RemoveHead();
}

