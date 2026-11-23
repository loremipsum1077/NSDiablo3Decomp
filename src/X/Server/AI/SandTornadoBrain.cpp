// Original file: X/Server/AI/SandTornadoBrain.cpp
#include "../Actors/Actors.h"
#include "../Actors/Projectile.h"
#include "./ActorAI.h"
#include "./BrainComponents.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Movement/SActorMovement.h"
#include "../Powers/Powers.h"
#include "../Powers/PowerAPI.h"
#include "../Powers/PowerHelper.h"
#include "../Powers/PowerTrigger.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorCommonCollision.h"
#include "../../Common/AI/ActorAISwig.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../../Common/Nav/NavCellHandle.h"
#include "../../Common/Nav/NavZone.h"
#include "../../Common/Player.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Powers/PowersStruct.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Common/Target.h"
#include "../../Common/TeamCommon.h"
#include "../../Common/WorldCommonCollision.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/Random.h"
// Declared here until their headers are in src: ActorCollision.cpp:349, WorldCommonCollision.cpp:147, NavigationMesh.cpp:3536, PowerAPI.cpp:6247, Projectile.cpp:1080 and :1127.
void ACDsCylinderIntersectSphere(const WorldPlace & placeCenter, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags); BOOL WorldCollSweptCircle(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult); BOOL NavMeshCheckLOSToACD(NavCellHandle hNavCellStart, const WorldPlace & placeStart, ACDID idACDDest, const PassabilityType ePassabilityType, BOOL bWantProjectileLOS);
void PowerAPI_DoFindAllPossibleTargets(const PowerAPIChooseTargets & tTargetingInfo, XPooledDataIDList & listTargets); const ActorCommonData * ProjectileGetOwnerACD(ACDID idACDProjectile); BOOL ProjectileReflect(ACDID idACDProjectile, ACDID idACDReflector, BOOL bReflectYawOnly, Angle aYawOffset, Team eTeam, float flSpeedMult);
struct SandTornadoBrainData {  // size 0x38
    Vector3D vTargetLoc;  // 0x00
    uint32 dwNextSwitchTime;  // 0x0c
    float flMoveSpeed;  // 0x10
    BOOL bInitialized;  // 0x14
    float flUnitDamageRadius;  // 0x18
    float flWanderRadiusMin;  // 0x1c
    float flWanderRadiusDelta;  // 0x20
    float flRadiusGrowthRatePerSwitch;  // 0x24
    uint32 dwSwitchTicksMin;  // 0x28
    uint32 dwSwitchTicksDelta;  // 0x2c
    uint32 dwTickGrowthRatePerSwitch;  // 0x30
    uint32 dwTicksToChasePlayerCentroid;  // 0x34
};


SandTornadoBrainData * SandTornadoGetBrainData(const ActorID idActor)
{



    if (idActor != ACTORID_INVALID)
    {
        ActorAI * ptAI = ActorTryToGetAI(idActor);
        if (ptAI)
        {
            if (ptAI->nBrainType == ACTORBRAIN_SAND_TORNADO)
            {
                return (SandTornadoBrainData *)ptAI->pbNewBrainDataBuf;
            }
        }
    }
    return NULL;
}


static const int32 TORNADO_UPDATE_TICK_INTERVAL = 15;

static void sSandTornadoSwitchDirections(SandTornadoBrainData * pBrainData, ActorID idActor)


{
    if (pBrainData == NULL)
    {
        return;
    }

    ACDID idACD = ACDFindByActor(idActor); Actor * ptActor;
    ActorCommonData & tACD = ACDGet(idACD);

    pBrainData->dwNextSwitchTime = GameGetServerTime() + RandRange_Internal(SGameGlobalsGet().rng, pBrainData->dwSwitchTicksMin, pBrainData->dwSwitchTicksDelta);
    pBrainData->dwSwitchTicksMin += pBrainData->dwTickGrowthRatePerSwitch;
    pBrainData->dwSwitchTicksDelta += pBrainData->dwTickGrowthRatePerSwitch;

    ptActor = ActorGet(idActor);
    if (ptActor->ptSActorMovement == NULL)
    {
        return;
    }


    WorldPlace placeCurr = tACD.GetPlace();
    WorldPlace placeDest(placeCurr);

    if (pBrainData->dwTicksToChasePlayerCentroid == 0)
    {
        RNG & rng = SGameGlobalsGet().rng;
        float flRandomX = RandRangeFloat_Internal(rng, pBrainData->flWanderRadiusMin, pBrainData->flWanderRadiusDelta);
        float flRandomY = RandRangeFloat_Internal(rng, pBrainData->flWanderRadiusMin, pBrainData->flWanderRadiusDelta);
        uint32 nNegRandomBits = Rand_Internal(rng, 0xFFFE);
        placeDest.wp.x += (nNegRandomBits & 1) ? flRandomX : -flRandomX;
        placeDest.wp.y += (nNegRandomBits & 8) ? flRandomY : -flRandomY;
    }
    else
    {  // dwNumPlayers is declared ahead of fNavCellInitialized (the DWARF lists it after listTargets): this order gives the original's loop-entry register setup.
        POS pos; uint32 dwNumPlayers; BOOL fNavCellInitialized;
        pBrainData->dwTicksToChasePlayerCentroid--;

        XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
        ACDsCylinderIntersectSphere(placeDest,

            Math::Max(pBrainData->flWanderRadiusMin + pBrainData->flWanderRadiusDelta, 100.0f),
            idACD, listTargets, 7);



        pBrainData->vTargetLoc = Vector3D(0.0f, 0.0f, 0.0f);
        dwNumPlayers = 0; fNavCellInitialized = FALSE;
        NavCellHandle hNavCellStart = { SSCENEID_INVALID, 0 };

        pos = listTargets.GetHeadPosition();
        while (pos)
        {
            ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);

            if (ACDIsPlayer(idACDTarget))
            {
                if (!fNavCellInitialized)
                {
                    NavCellHandleFromWorldPlace(placeDest, hNavCellStart); fNavCellInitialized = TRUE;
                }

                if (NavMeshCheckLOSToACD(hNavCellStart, placeDest, idACDTarget, 2, FALSE))
                {
                    ActorCommonData * ptTargetACD = ACDTryToGet(idACDTarget);
                    if (ptTargetACD)
                    {
                        pBrainData->vTargetLoc += ptTargetACD->GetPlace().wp;
                        dwNumPlayers++;
                    }
                }
            }
        }

        if (dwNumPlayers > 0)
        {
            placeDest.wp = pBrainData->vTargetLoc * (1.0f / dwNumPlayers);
        }
        else
        {
            RNG & rng = SGameGlobalsGet().rng;
            float flRandomX = RandRangeFloat_Internal(rng, pBrainData->flWanderRadiusMin, pBrainData->flWanderRadiusDelta);
            float flRandomY = RandRangeFloat_Internal(rng, pBrainData->flWanderRadiusMin, pBrainData->flWanderRadiusDelta);
            uint32 nNegRandomBits = Rand_Internal(rng, 0xFFFE);
            placeDest.wp.x += (nNegRandomBits & 1) ? flRandomX : -flRandomX;
            placeDest.wp.y += (nNegRandomBits & 8) ? flRandomY : -flRandomY;
        }
    }

    NavCellHandle tNavCellHandleCurr;
    if (NavCellHandleFromWorldPlace(placeCurr, tNavCellHandleCurr))
    {




        Circle circleStart;
        circleStart.Set((const Vector2D &)placeCurr.wp, tACD.GetRadius());

        const ACDCollSettings & tACDCollSettings = tACD.GetCollSettings();
        WorldCollParams tCollParams(tACDCollSettings);
        WorldCollResult tCollResult;
        WorldCollSweptCircle(placeCurr.idSWorld, tNavCellHandleCurr, circleStart, (const Vector2D &)placeDest.wp, tCollParams, 3, 1,
            tACD.id, tCollResult);

        placeDest.wp = tCollResult.wpEnd;

        pBrainData->vTargetLoc = placeDest.wp;


        uint32 uMoveFlags = 0x807;
        ptActor->ptSActorMovement->MoveTo(placeDest, pBrainData->flMoveSpeed, 0.0f, (ActorAnimationTag)-1, uMoveFlags, 0.0f, ACDID_INVALID, SNO_NONE, 0, -1, 0.0f, NULL);
    }
}





void SandTornadoBrainInitialize(const ActorID idActor)
{

    SandTornadoBrainData * pBrainData = SandTornadoGetBrainData(idActor);

    SigmaMemoryClear(pBrainData, sizeof(SandTornadoBrainData));
}


void SandTornadoBrainUpdate(ActorID idActor)

{
    SandTornadoBrainData * pBrainData = SandTornadoGetBrainData(idActor);
    if (pBrainData == NULL)
    {
        return;
    }

    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI == NULL)
    {
        return;
    }

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);

    uint32 dwNow = GameGetServerTime();
    if (!pBrainData->bInitialized)
    {

        pBrainData->flMoveSpeed = Math::Max(pBrainData->flMoveSpeed, 0.07f);
        pBrainData->bInitialized = TRUE;
        pBrainData->vTargetLoc = tACD.GetPlace().wp;
        pBrainData->flUnitDamageRadius = 1.0f;
        pBrainData->dwTicksToChasePlayerCentroid = RandRange_Internal(SGameGlobalsGet().rng, 1, 4);

        Target tTarget;
        tTarget.InitializeFromPlace(tACD.GetPlace());
        PowerID idPower = PowerStart(idACD, GlobalSNOGet(GLOBALSNO_POWERS_SAND_TORNADO_ONSPAWN), tTarget);




        pBrainData->dwSwitchTicksMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_TIME_BETWEEN_DIR_CHANGE_MIN) * 60.0f);
        pBrainData->dwSwitchTicksDelta = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_TIME_BETWEEN_DIR_CHANGE_DELTA) * 60.0f);
        pBrainData->dwTickGrowthRatePerSwitch = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_TIME_BETWEEN_DIR_CHANGE_GROWTH) * 60.0f);
        pBrainData->flWanderRadiusMin = PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_WANDER_RADIUS_MIN) * 60.0f;
        pBrainData->flWanderRadiusDelta = PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_WANDER_RADIUS_DELTA) * 60.0f;
        pBrainData->flRadiusGrowthRatePerSwitch = PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_WANDER_RADIUS_GROWTH) * 60.0f;

        pBrainData->dwNextSwitchTime = GameGetServerTime() +
            RandRange_Internal(SGameGlobalsGet().rng, pBrainData->dwSwitchTicksMin, pBrainData->dwSwitchTicksDelta);
        sSandTornadoSwitchDirections(pBrainData, idActor);
    }

    else if (dwNow >= pBrainData->dwNextSwitchTime)
    {
        sSandTornadoSwitchDirections(pBrainData, idActor);
    }
    else
    {

        ACDID idACDTemp = ACDFindByActor(idActor);
        ActorCommonData & tACDTemp = ACDGet(idACDTemp);

        float flDistToTarget = Vector3D::Distance2DSquared(tACDTemp.GetPlace().wp, pBrainData->vTargetLoc);

        if (flDistToTarget <= pBrainData->flWanderRadiusMin * 0.05f)
        {
            sSandTornadoSwitchDirections(pBrainData, idActor);
        }
    }

    ptAI->dwNextBrainUpdateTime = dwNow + TORNADO_UPDATE_TICK_INTERVAL;
}



static BOOL sSandTornado_DeflectionTest(PowerAPIBuffInstance *, ImpactData *, void *)
{



    return TRUE;
}




static void sSandTornado_DeflectProjectile(ActorCommonData & tACDProjectile, ActorCommonData & tACDTornado, PowerID idPower)



{

    if (ACDIsProjectile(tACDProjectile.id))
    {




        float flSpeedIncrease = RandRangeFloat_Internal(SGameGlobalsGet().rng, 2.0f, 2.0f);
        ProjectileSetSpeedMultiplier(tACDProjectile.id, flSpeedIncrease, 15, TRUE, TRUE, FALSE);

        WorldPlace wpOwner = tACDTornado.GetPlace();
        WorldPlace wpProjectile = tACDProjectile.GetPlace();
        Vector3D wvToTarget;
        WorldPlaceGetVectorBetween(wpProjectile, wpOwner, wvToTarget);

        ActorMovement * ptMove = tACDProjectile.GetActorMovement();

        Vector3D wvCurrentVelocity = ptMove->GetVelocity();


        Angle aDifference = Vector3D::AngleBetween(wvToTarget, wvCurrentVelocity);
        if (aDifference > 1.5707964f && aDifference < 4.712389f)
        {
            return;
        }

        Vector3D wvFacing(cgwvXAxis);

        Angle aFacing = tACDTornado.GetOrientation().ToYaw();
        Vector3D::Rotate(cgwvZAxis, aFacing, wvFacing);


        Angle aReflectionOffset = RandRangeFloat_Internal(SGameGlobalsGet().rng, 1.5707964f, 2.3561945f);

        ProjectileReflect(tACDProjectile.id, tACDTornado.id, TRUE, -aReflectionOffset, PowerGetTeam(idPower), 1.0f);

    }
}

static BOOL sDuneDervish_ReflectEffect(PowerAPIBuffInstance * ptBuff, ImpactData * ptImpact, void *)



{

    PowerID idPower = ptBuff->idPower;

    ACDID idACDTarget = ptImpact->idACDProjectile;

    ActorCommonData * ptACDCenter = PowerGetOwner(idPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDCenter == NULL || ptACDTarget == NULL)
    {
        return FALSE;
    }

    BOOL bTeamsAreAllied = FALSE;
    Team eTeamPower = PowerGetTeam(idPower);



    const ActorCommonData * ptACDProjectileOwner = ProjectileGetOwnerACD(idACDTarget);
    if (ptACDProjectileOwner)
    {
        Team eTeamTarget = ptACDProjectileOwner->GetTeam();
        bTeamsAreAllied = TeamsAreFriendly(eTeamTarget, eTeamPower);
    }

    if (bTeamsAreAllied && !ProjectileIsReflected(idACDTarget))
    {
        return FALSE;
    }

    sSandTornado_DeflectProjectile(*ptACDTarget, *ptACDCenter, idPower);

    return TRUE;
}




static void sSandTornado_DeflectPulse(PowerID idPower, void *)


{
    ActorCommonData * ptACDCenter = PowerGetOwner(idPower);
    if (ptACDCenter == NULL)
    {
        return;
    }


    POS pos;
    XPooledDataIDList listProjectileTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;


    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInit(idPower, tChooseTargets);
    PowerHelper_ChooseTargetsInitGeometrySphere(
        ptACDCenter->GetPlace(),
        PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_RADIUS) * 1.1f,
        tChooseTargets.tGeometry);

    PowerAPI_DoFindAllPossibleTargets(tChooseTargets, listProjectileTargets);

    Team eTeamPower = PowerGetTeam(idPower);
    pos = listProjectileTargets.GetHeadPosition();
    while (pos)
    {
        BOOL bTeamsAreAllied = FALSE;
        ACDID idACDTarget = (ACDID)listProjectileTargets.GetNext(pos);




        if (!ACDIsProjectile(idACDTarget))
        {
            continue;
        }

        if (ProjectileGetReflectedByACD(idACDTarget) == ptACDCenter->id)
        {
            continue;
        }


        const ActorCommonData * ptACDProjectileOwner = ProjectileGetOwnerACD(idACDTarget);
        if (ptACDProjectileOwner)
        {
            Team eTeamTarget = ptACDProjectileOwner->GetTeam();
            bTeamsAreAllied = TeamsAreFriendly(eTeamTarget, eTeamPower);
        }

        if (bTeamsAreAllied && !ProjectileIsReflected(idACDTarget))
        {
            continue;
        }

        ActorCommonData * ptACDProjectile = ACDTryToGet(idACDTarget);
        if (ptACDProjectile)
        {

            sSandTornado_DeflectProjectile(*ptACDProjectile, *ptACDCenter, idPower);
        }
    }

    PowerTriggerAddTimerTrigger(idPower, TORNADO_UPDATE_TICK_INTERVAL, sSandTornado_DeflectPulse, NULL);
}



void Sand_Tornado_AOE(PowerAPIBuffInstance * ptBuff, void *)


{
    PowerID idPower = ptBuff->idPower;
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ActorID idActor = (ActorID)ptACD->idOwner;
        SandTornadoBrainData * pBrainData = SandTornadoGetBrainData(idActor);
        if (pBrainData)
        {

            XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
            PowerAPIChooseTargets tTargets;
            PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);

            tTargets.tGeometry.tSphere.wdRadius = PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_RADIUS);
            tTargets.tGeometry.tSphere.wdRadius *= pBrainData->flUnitDamageRadius;
            PowerAPI_DoChooseTargets(tTargets, listTargets);


            PowerAPIAttackPayload tPayload;
            PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
            PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
        }
    }
}



void Sand_Tornado_OnDeath(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void *)
{



    if (ptBuff)
    {



        ActorCommonData * ptACD = PowerGetOwner(ptBuff->idPower);
        if (ptACD)
        {
            ActorID idActor = (ActorID)ptACD->idOwner;
            BrainAPIDoDeath(idActor, idActor, TRUE, TRUE, TRUE, NULL, NULL);
        }
    }
}




void Sand_Tornado_OnSpawn(const PowerID idPower, void *)


{

    ActorCommonData * ptACD = PowerGetOwner(idPower);
    ActorID idActor = ptACD ? (ActorID)ptACD->idOwner : ACTORID_INVALID;

    float flDurationMin = PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_LIFE_DURATION_MIN);
    float flDurationDelta = PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_LIFE_DURATION_DELTA);
    float flDuration = RandRangeFloat_Internal(SGameGlobalsGet().rng, flDurationMin, flDurationDelta) * 60.0f;
    flDuration = Math::Max(flDuration, 0.0f);
    uint32 dwDuration = Math::FloatToInt(flDuration);
    ActorDeleteSoon(idActor, dwDuration + 1);

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.pPeriodicSpellEffect = Sand_Tornado_AOE;
    tBuffStartData.pRemovalEffect = Sand_Tornado_OnDeath;
    tBuffStartData.dwRefreshRate = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_DAMAGE_PULSE_RATE) * 60.0f);
    tBuffStartData.bInfinite = TRUE;

    tBuffStartData.pShouldReflectTest = sSandTornado_DeflectionTest;
    tBuffStartData.pReflectEffect = sDuneDervish_ReflectEffect;

    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);

    PowerTriggerAddTimerTrigger(idPower, TORNADO_UPDATE_TICK_INTERVAL, sSandTornado_DeflectPulse, NULL);

    SandTornadoBrainData * pBrainData = SandTornadoGetBrainData(idActor);
    if (pBrainData)
    {

        pBrainData->flMoveSpeed = PowerGetFormulaValue(idPower, TAG_POWER_TORNADO_MOVE_SPEED);
    }
}
