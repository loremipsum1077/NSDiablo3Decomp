#include "../Actors/Actors.h"
#include "ActorAI.h"
#include "BrainComponents.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Movement/SActorMovement.h"
#include "../Powers/Powers.h"
#include "../Powers/PowerAPI.h"
#include "../Actors/ActorEffect.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/AI/ActorAISwig.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/Target.h"
#include "../../Common/Marker/Marker.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"
#include <math.h>

#include "../Actors/Actors.h"
#include "ActorAI.h"
#include "BrainComponents.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Movement/SActorMovement.h"
#include "../Powers/Powers.h"
#include "../Powers/PowerAPI.h"
#include "../Actors/ActorEffect.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/AI/ActorAISwig.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/Target.h"
#include "../../Common/Marker/Marker.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"
#include <math.h>

// Local layouts from PowerBrains.h DWARF.
struct WizardTornadoBrainData {  // size 0x38
    Angle aOriginalYaw;  // 0x00
    Angle aYaw;  // 0x04
    float flAngleVel;  // 0x08
    float flAngleAcc;  // 0x0c
    uint32 dwNextSwitchTime;  // 0x10
    float flMoveSpeed;  // 0x14
    BOOL bInitialized;  // 0x18
    float flUnitDamageRadius;  // 0x1c
    BOOL bTrackTarget;  // 0x20
    uint32 dwNextLockOnTime;  // 0x24
    ACDID idACDTarget;  // 0x28
    Vector2D vLastPos;  // 0x2c
    float flYawLimit;  // 0x34
};
static_assert(sizeof(WizardTornadoBrainData) == 0x38, "WizardTornadoBrainData");

struct SentryBrainData {  // size 0xc
    uint32 dwTime;  // 0x00
    uint32 dwTimeLastSummon;  // 0x04
    ACDID idACDSeeker;  // 0x08
};
static_assert(sizeof(SentryBrainData) == 0xc, "SentryBrainData");

ACDID BrainAPIFindClosestEnemy(const ActorID idActor, const WorldDistance wdRadius);

WizardTornadoBrainData* WizardTornadoGetBrainData(const ActorID idActor)
{
    if (idActor != ACTORID_INVALID) {
        ActorAI* ptAI = ActorTryToGetAI(idActor);
        if (ptAI && ptAI->nBrainType == ACTORBRAIN_WIZARD_TORNADO)
            return (WizardTornadoBrainData*)ptAI->pbNewBrainDataBuf;
    }
    return 0;
}
static SentryBrainData* sSentryGetBrainData(const ActorID idActor)
{
    if (idActor != ACTORID_INVALID) {
        ActorAI* ptAI = ActorTryToGetAI(idActor);
        if (ptAI && ptAI->nBrainType == ACTORBRAIN_SENTRY)
            return (SentryBrainData*)ptAI->pbNewBrainDataBuf;
    }
    return 0;
}
static void sWizardTornadoSwitchDirections(ActorCommonData& tACD, WizardTornadoBrainData* pBrainData)
{
    uint32 dwNow = GameGetServerTime();
    if (pBrainData->bTrackTarget) {
        pBrainData->dwNextSwitchTime = dwNow + RandRange_Internal(SGameGlobalsGet().rng, 10, 15);
        ActorCommonData* ptACD = ACDTryToGet(pBrainData->idACDTarget);
        if (!ptACD)  // Inference: invalid-target store at row 77 precedes the 83-92 block, so the NULL case comes first.
            pBrainData->idACDTarget = ACDID_INVALID;
        else {
            WorldPlace placeMe = tACD.GetPlace();
            WorldPlace placeTarget = ptACD->GetPlace();
            Vector3D wvToTarget;
            WorldPlaceGetVectorBetween(placeMe, placeTarget, wvToTarget);
            Angle targetYaw = wvToTarget.As2D().ToYaw();  // DWARF: Vector2D::ToYaw inlined at original line 87 (As2D leaves no code)
            Angle aDifference = Math::AngularDifference(pBrainData->aOriginalYaw, targetYaw);
            if (aDifference < pBrainData->flYawLimit)
                pBrainData->aOriginalYaw = targetYaw;
        }
        Angle aDifference = Math::SignedAngularDifference(pBrainData->aYaw, pBrainData->aOriginalYaw);
        pBrainData->flAngleAcc = aDifference < 0.0f ? -1.0f : 1.0f;
        if (pBrainData->flAngleVel * pBrainData->flAngleAcc < 0.0f)
            pBrainData->flAngleVel = 0.0f;
    }
    else {
        pBrainData->dwNextSwitchTime = dwNow + RandRange_Internal(SGameGlobalsGet().rng, 20, 30);
        BOOL bNegDirection = TRUE;  // Inference: TRUE default + else-if FALSE; the original's late orr #1/csel le (row 124) needs a three-way phi, a ternary folds to cset.
        if (pBrainData->flAngleAcc == 0.0f)
        {
            uint32 dwRand = Rand_Internal(SGameGlobalsGet().rng, 31);
            bNegDirection = (dwRand & 16) == 0;
        }
        else if (pBrainData->flAngleAcc > 0.0f)
            bNegDirection = FALSE;
        pBrainData->flAngleAcc = RandRangeFloat_Internal(SGameGlobalsGet().rng, 1.0f, 4.0f);
        if (!bNegDirection)
            pBrainData->flAngleAcc = -pBrainData->flAngleAcc;
    }
}
static void sWizardTornadoReflect(WizardTornadoBrainData& tBrainData)
{
    // Statement order from the original rows (140, 141, then the zero stores at 143).
    tBrainData.aYaw += HALF_CIRCLE;
    tBrainData.aOriginalYaw = tBrainData.aYaw;
    tBrainData.flAngleVel = 0.0f;
    tBrainData.flAngleAcc = 0.0f;
}
#line 146 "X/Server/Ai/PowerBrains.cpp"
void WizardTornadoBrainInitialize(const ActorID idActor)
{
    WizardTornadoBrainData* pBrainData = WizardTornadoGetBrainData(idActor);
    SigmaMemoryClear(pBrainData, sizeof(*pBrainData));
    pBrainData->flMoveSpeed = 0.18f;
    ActorCommonData& tACD = ACDGet(ACDFindByActor(idActor));
    pBrainData->aOriginalYaw = tACD.GetOrientation().ToYaw();
}
#line 157 "X/Server/Ai/PowerBrains.cpp"
void WizardTornadoBrainDispose(const ActorID idActor)
{
}
static BOOL sTornadoHasMoved(const ActorCommonData& tACD, const Vector2D& vLastPos)
{
    WorldPlace placeCurr = tACD.GetPlace();
    return !(Vector2D::DistanceSquared(vLastPos, placeCurr.wp.As2D()) < 1e-12f);
}
#line 175 "X/Server/Ai/PowerBrains.cpp"

void WizardTornadoBrainUpdate(
    ActorID idActor)
{
    WizardTornadoBrainData * pBrainData = WizardTornadoGetBrainData(idActor);
    if (!pBrainData)
    {
        return;
    }

    ActorAI* ptAI = ActorTryToGetAI(idActor);
    if (!ptAI)
    {
        return;
    }

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);

    uint32 dwNow = GameGetServerTime();
    if (!pBrainData->bInitialized)
    {

        pBrainData->flAngleVel = 0.0f;
        pBrainData->flAngleAcc = 0.0f;
        pBrainData->aYaw = pBrainData->aOriginalYaw;
        pBrainData->dwNextSwitchTime = GameGetServerTime() +
            RandRange_Internal(SGameGlobalsGet().rng, 10, 20);
        pBrainData->flMoveSpeed = Math::Max(pBrainData->flMoveSpeed, 0.001f);
        pBrainData->bInitialized = TRUE;
        pBrainData->bTrackTarget = FALSE;
        pBrainData->dwNextLockOnTime = 0;
        pBrainData->idACDTarget = ACDID_INVALID;
        pBrainData->vLastPos.Set(0.0f, 0.0f);
        pBrainData->flYawLimit = QUARTER_CIRCLE;


        SNO snoPower = GlobalSNOGet(0x554);
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, &tACD, ACDID_INVALID);
        PowerAPIBuffInstance* ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff)
        {
            Power* ptPower = PowerGet(ptBuff->idPower);
            if (ptPower)
            {
                if (FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(0x2bb, snoPower)) > 0)
                {
                    pBrainData->bTrackTarget = TRUE;
                    pBrainData->idACDTarget = ptPower->tTarget.GetACD();
                }

                if (FastAttribGetValueInt(ptPower->idFastAttrib, (FastAttribKey)0xfffff568))
                {
                    pBrainData->flYawLimit = 0.0f;
                }
            }
        }
    }
    else if (!sTornadoHasMoved(tACD, pBrainData->vLastPos))
    {
        sWizardTornadoReflect(*pBrainData);
    }
    else if (dwNow >= pBrainData->dwNextSwitchTime)
    {
        sWizardTornadoSwitchDirections(tACD, pBrainData);
    }

    pBrainData->flAngleVel += pBrainData->flAngleAcc * (24.0f * (1.0f / 60.0f));
    pBrainData->flAngleVel = Math::Pin(pBrainData->flAngleVel, -HALF_CIRCLE, HALF_CIRCLE);
    pBrainData->aYaw += pBrainData->flAngleVel * (24.0f * (1.0f / 60.0f));


    float flLimitMin = pBrainData->aOriginalYaw - pBrainData->flYawLimit;
    float flLimitMax = pBrainData->aOriginalYaw + pBrainData->flYawLimit;
    if (pBrainData->aYaw <= flLimitMin)
    {
        pBrainData->aYaw = flLimitMin;
        pBrainData->flAngleVel = 0.0f;
        pBrainData->flAngleAcc = 5.0f;
        pBrainData->dwNextSwitchTime = dwNow + 50;
    }
    else if (pBrainData->aYaw >= flLimitMax)
    {
        pBrainData->aYaw = flLimitMax;
        pBrainData->flAngleVel = 0.0f;
        pBrainData->flAngleAcc = -5.0f;
        pBrainData->dwNextSwitchTime = dwNow + 50;
    }
    pBrainData->aYaw = Math::Pin(pBrainData->aYaw, flLimitMin, flLimitMax);



    Vector3D vVel(Math::Cos(pBrainData->aYaw), Math::Sin(pBrainData->aYaw), 0.0f);

    Actor* ptActor = ActorGet(idActor);
    if (!ptActor->ptSActorMovement)
    {
        return;
    }


    WorldPlace placeDest;
    placeDest = tACD.GetPlace();
    pBrainData->vLastPos = placeDest.wp.As2D();
    placeDest.wp += vVel * (60.0f * (24.0f * (1.0f / 60.0f)));
    ptActor->ptSActorMovement->MoveTo(placeDest, pBrainData->flMoveSpeed, 0.0f, 0x11000, 0, 0.0f, ACDID_INVALID, -1, 0, -1, 0.0f, 0);

    pBrainData->flUnitDamageRadius += 30.0f * (24.0f * (1.0f / 60.0f));
    pBrainData->flUnitDamageRadius = Math::Min(pBrainData->flUnitDamageRadius, 1.0f);








    ptAI->dwNextBrainUpdateTime = dwNow + 24;
}
#line 339 "X/Server/Ai/PowerBrains.cpp"
void SentryBrainInitialize(ActorID idActor)
{
    SentryBrainData* pBrainData = sSentryGetBrainData(idActor);
    SigmaMemoryClear(pBrainData, sizeof(*pBrainData));
    pBrainData->dwTime = GameGetServerTime();
    pBrainData->idACDSeeker = ACDID_INVALID;
}
#line 352 "X/Server/Ai/PowerBrains.cpp"
void SentryBrainUpdate(ActorID idActor)
{
    Actor* ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData& tACD = ACDGet(idACD);
    SNO snoPowerProxySeeker = GlobalSNOGet(0x66c);
    SentryBrainData* pBrainData = sSentryGetBrainData(idActor);
    if (pBrainData->idACDSeeker != ACDID_INVALID && !ACDTryToGet(pBrainData->idACDSeeker))
        pBrainData->idACDSeeker = ACDID_INVALID;
    if (pBrainData->idACDSeeker != ACDID_INVALID) {
        if (pBrainData->dwTimeLastSummon + 30.0f < GameGetServerTime()) {
            ACDID idACDTarget = BrainAPIFindClosestEnemy(ActorID(ACDGetOwner(pBrainData->idACDSeeker)), 0.5f);
            if (idACDTarget != ACDID_INVALID) {
                ActorCommonData& tACDTarget = ACDGet(idACDTarget);
                if (tACDTarget.IsAlive()) {
                    Target tTarget;
                    tTarget.InitializeFromACD(idACDTarget, 0, 0);
                    PowerStart(idACD, GlobalSNOGet(0x5e1), tTarget);
                    BrainAPIDoDeath(ActorID(ACDGetOwner(pBrainData->idACDSeeker)), ACTORID_INVALID, TRUE, TRUE, TRUE, 0, 0);
                    pBrainData->idACDSeeker = ACDID_INVALID;
                    return;
                }
            }
        }
    }
    if (pBrainData->idACDSeeker != ACDID_INVALID && pBrainData->dwTimeLastSummon + 300 < GameGetServerTime()) {
        BrainAPIDoDeath(ActorID(ACDGetOwner(pBrainData->idACDSeeker)), ACTORID_INVALID, TRUE, TRUE, TRUE, 0, 0);
        pBrainData->idACDSeeker = ACDID_INVALID;
    }
    if (pBrainData->idACDSeeker != ACDID_INVALID) return;
    if (pBrainData->dwTime + 120 < GameGetServerTime() && pBrainData->dwTimeLastSummon + 480 < GameGetServerTime()) {
        pBrainData->dwTimeLastSummon = GameGetServerTime();
        int32 nNumSummoned = ACDCountNearbySummonedUnits(tACD, 80.0f);
        if (nNumSummoned < 3) {
            WorldPlace tSummonLocation = tACD.GetPlace();
            ActorAddStruct tActorAdd;
            tActorAdd.dwActorAddFlags = 0xc0;
            tActorAdd.eTeam = tACD.GetTeam();
            tActorAdd.tSNOName.snoGroup = 1;
            tActorAdd.tSNOName.snoHandle = snoPowerProxySeeker;
            tActorAdd.tMarkersName = MarkerStringHash("Power Proxy Seeker");
            tActorAdd.tPlace = tSummonLocation;
            tActorAdd.qOrientation = tACD.GetOrientation();
            ScenesCommonNavMeshNodeFromPlace(tActorAdd.tPlace, &tActorAdd.tNavMeshHandle, FALSE, FALSE);
            ActorID idSeeker = GBAdd(tActorAdd);
            if (idSeeker != ACTORID_INVALID) {
                pBrainData->idACDSeeker = ACDFindByActor(idSeeker);
                SNO snoRope = GlobalSNOGet(0x23a);
                ACDPlayEffectRopeACDToACD(snoRope, idACD, 1, pBrainData->idACDSeeker, 1, FALSE);
            }
        }
    }
}
#line 446 "X/Server/Ai/PowerBrains.cpp"
BOOL SentryBrainOnDeath(const ActorID idActor, const ActorID idKillerActor)
{
    SentryBrainData* pBrainData = sSentryGetBrainData(idActor);
    if (ACDTryToGet(pBrainData->idACDSeeker)) {
        BrainAPIDoDeath(ActorID(ACDGetOwner(pBrainData->idACDSeeker)), ACTORID_INVALID, TRUE, TRUE, TRUE, 0, 0);
        pBrainData->idACDSeeker = ACDID_INVALID;
    }
    return FALSE;
}
