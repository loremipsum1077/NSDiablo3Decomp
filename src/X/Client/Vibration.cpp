// Original file: X/Client/Vibration.cpp

#include "Vibration.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/GameConstants.h"
#include "../Common/Player.h"
#include "CGame.h"
#include "CPlayer.h"
#include "Definitions/VibrationDefinition.h"
#include "InterpolationTrack.h"
#include "RActors/RActors.h"

// Declared here because its header does not carry it yet.
void InputSetVibration(const int32 nControllerIndex, const float flSpeed);  // X/Client/Input/InputNX64.cpp:1159
BOOL SetMotorSpeed(float flSpeed, int32 nControllerIndex, BOOL bForceSet);

namespace {
struct VibrationGlobals {  // size 0x48
    TDataArray<Vibration::VibrationInfo, int> * ptVibrations;  // 0x00
    float flControllerMagnitude[4];  // 0x08
    float flPreviousMagnitude[4];  // 0x18
    float flHighestPriority[4];  // 0x28
    Blizzard::Time::Millisecond msLastClearedVibration[4];  // 0x38
};





















static VibrationGlobals sgtVibrationGlobals;
}

namespace Vibration {






void InitializeForGame()
{
    SigmaMemoryClear(&sgtVibrationGlobals, sizeof(sgtVibrationGlobals));
    sgtVibrationGlobals.ptVibrations = TDataArrayCreateTracked<VibrationInfo, int>("Vibration", AppGetCurrentMemoryInterface(), 32);
}


void DisposeForGame()
{
    DeleteAll();
    TDataArrayDispose(sgtVibrationGlobals.ptVibrations);
}


void DeleteAll()

{
    if (sgtVibrationGlobals.ptVibrations == NULL)
    {
        return;
    }

    TDataArrayIterator<VibrationInfo> tIt;
    while (sgtVibrationGlobals.ptVibrations->Iterate(tIt))
    {

        Delete(tIt->id);
    }

    if (sgtVibrationGlobals.ptVibrations)
    {
        sgtVibrationGlobals.ptVibrations->DeleteAll();
    }



    for (int32 i = 0; i < 4; i++)
    {
        SetMotorSpeed(0.0f, i, TRUE);
    }
}


void DeleteAllForController(int32 nControllerIndex)

{
    if (sgtVibrationGlobals.ptVibrations == NULL)
    {
        return;
    }

    TDataArrayIterator<VibrationInfo> tIt;
    while (sgtVibrationGlobals.ptVibrations->Iterate(tIt))
    {

        if (tIt->nControllerIndex == nControllerIndex)
        {
            Delete(tIt->id);
        }
    }


    SetMotorSpeed(0.0f, nControllerIndex, TRUE);
}

VibrationInfo * Get(VibrationID id)

{
    return sgtVibrationGlobals.ptVibrations->ElementGet(id);
}


VibrationInfo * TryToGet(VibrationID id)

{
    return sgtVibrationGlobals.ptVibrations->ElementTryToGet(id);
}


BOOL Exists(VibrationID idVibration)

{
    return TryToGet(idVibration) != NULL;
}


void SetParent(const VibrationID idVibration, const ParentData & tParentData)

{



    VibrationInfo * ptVibration = TryToGet(idVibration);

    if (ptVibration)
        ptVibration->tParentData = tParentData;
}



VibrationID Add(SNO snoVibration, const PRTransform & transform, int32 nPlayerIndex)


{
    VibrationID idVibration = sgtVibrationGlobals.ptVibrations->ElementNew();

    if (idVibration == VIBRATIONID_INVALID)
    {
        return VIBRATIONID_INVALID;
    }

    if (!PlayerIsLocal(nPlayerIndex))
    {
        return VIBRATIONID_INVALID;
    }


    VibrationInfo * ptVibration = Get(idVibration);

    ptVibration->snoVibration = snoVibration;
    ptVibration->flTime = 0.0f; ptVibration->flOutroTime = 0.0f;
    ptVibration->bOutro = FALSE;
    ptVibration->transform = transform;

    RNG & rng = CGameGlobalsGet().rngLocal;
    ScopedVibrationDefinitionLockR pdefVibration(snoVibration);


    ptVibration->flMainVibrationInterp = pdefVibration->primaryVibrationIntensityCurve.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptVibration->flOutroVibrationInterp = pdefVibration->outroVibrationIntensityCurve.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptVibration->nPlayerIndex = nPlayerIndex;
    ptVibration->nControllerIndex = PlayerGetByPlayerIndex(nPlayerIndex)->nControllerIndex;
    ptVibration->tParentData.Init();










    return idVibration;
}

void UpdatePropertiesFromParent(VibrationInfo * ptVibration)

{
    if (ptVibration->tParentData.ParentIsValid())
    {


        PRTransform transform;
        ptVibration->tParentData.GetTransform(transform);

        SetPosition(ptVibration->id, transform.wp);
    }
}


void SetPosition(VibrationID idVibration, const Vector3D & wp)

{


    VibrationInfo * ptVibration = TryToGet(idVibration);



    if (ptVibration)
    {
        ptVibration->transform.wp = wp;
    }
}

void Delete(VibrationID idVibration)

{
    if (!Exists(idVibration)) return;
    sgtVibrationGlobals.ptVibrations->ElementDelete(TryToGet(idVibration));



}

void PlayOutro(VibrationID idVibration)

{


    VibrationInfo * ptVibration = TryToGet(idVibration);

    if (ptVibration)
    {

        ptVibration->bOutro = TRUE;
    }
}
}  // namespace Vibration





















































BOOL SetMotorSpeed(float flSpeed, int32 nControllerIndex, BOOL bForceSet)

{


    if (bForceSet || sgtVibrationGlobals.flPreviousMagnitude[nControllerIndex] != flSpeed)
    {



        sgtVibrationGlobals.flPreviousMagnitude[nControllerIndex] = flSpeed;

        InputSetVibration(nControllerIndex, flSpeed);
        return TRUE;
    }

    return FALSE;
}

namespace Vibration {
void Update()

{
    if (sgtVibrationGlobals.ptVibrations == NULL)
        return;

    TDataArrayIterator<VibrationInfo> tIt; while (sgtVibrationGlobals.ptVibrations->Iterate(tIt))
    {


        tIt->Update();

        if (tIt->bDeleted)
        {
            Delete(tIt->id);
        }
    }



    Blizzard::Time::Millisecond msNow = Blizzard::Time::Milliseconds();

    for (int32 i = 0; i < 4; i++)
    {
        if (sgtVibrationGlobals.flControllerMagnitude[i] > EPSILON)
        {
            if (Blizzard::Time::GetTimeElapsed(sgtVibrationGlobals.msLastClearedVibration[i], msNow) > 2000)
            {


                sgtVibrationGlobals.msLastClearedVibration[i] = msNow;
                sgtVibrationGlobals.flControllerMagnitude[i] = 0.0f;
                DeleteAllForController(i);
            }
        }
        else
        {
            sgtVibrationGlobals.msLastClearedVibration[i] = msNow;
        }


        SetMotorSpeed(sgtVibrationGlobals.flControllerMagnitude[i], i, FALSE);
        sgtVibrationGlobals.flControllerMagnitude[i] = 0.0f;
        sgtVibrationGlobals.flHighestPriority[i] = -1.0f;
    }
}


void FlagForDelete(VibrationID idVibration)

{
    VibrationInfo * ptVibration = TryToGet(idVibration);

    if (ptVibration)
        ptVibration->bDeleted = TRUE;
}



void VibrationInfo::Update()
{
    if (tParentData.m_eParentType != -1) UpdatePropertiesFromParent(this);




    ScopedVibrationDefinitionLockR pdefVibration(snoVibration);

    float flDistanceScale = 1.0f;


    if (GameGetActiveGameCommonData().GameModeGet() != GAMEMODE_HOLODECK)
    {




        Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
        if (ptPlayer == NULL) return;
        RActor * ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
        flDistanceScale = 0.0f; if (ptRActor)
        {
            float flDistance = Vector3D::Distance(ptRActor->wpWalkingPos, transform.wp);

            if (flDistance < pdefVibration->wdMinRange)
            {
                flDistanceScale = 1.0f;
            }
            else if (flDistance > pdefVibration->wdMaxRange)
            {
                flDistanceScale = 0.0f;
            }
            else
            {
                float flRange = pdefVibration->wdMaxRange - pdefVibration->wdMinRange;

                if (flRange < EPSILON)
                {
                    flDistanceScale = 1.0f;
                }
                else
                {
                    flDistanceScale = 1.0f - (flDistance - pdefVibration->wdMinRange) / flRange;
                }
            }
        }
    }






    float flDeltaTime = CGameGetRealDeltaTime(); flTime += flDeltaTime;
    InterpTime tInterpTime;

    tInterpTime.Init();

    tInterpTime.flTime = flTime;
    tInterpTime.flScale = (float)pdefVibration->tSystemDuration * OO_TICKS_PER_SECOND;

    if (bOutro)
    {
        flOutroTime += flDeltaTime;

        flOutroCrossFade = flOutroTime / ((float)pdefVibration->tOutroDuration * OO_TICKS_PER_SECOND); flOutroCrossFade = Math::Pin(flOutroCrossFade, 0.0f, 1.0f);
    }
    else
    {
        flOutroCrossFade = 0.0f;
    }


    TickTimeType eTimeType; if (pdefVibration->dwFlags & 1)
    {
        if (bOutro)
        {
            if (flOutroTime >= (float)pdefVibration->tOutroDuration * OO_TICKS_PER_SECOND)
            {
                FlagForDelete(id); return;
            }
            eTimeType = TICK_TIME_OUTRO_CROSSFADE;
            tInterpTime.flRatio = flOutroCrossFade;
            tInterpTime.flOutro = flOutroCrossFade;
        }
        else
        {
            eTimeType = TICK_TIME_LOOPING;
        }
    }
    else
    {


        if (tInterpTime.flTime >= (float)pdefVibration->tSystemDuration * OO_TICKS_PER_SECOND)
        {
            FlagForDelete(id);
            return;
        }

        eTimeType = TICK_TIME_NON_LOOPING;
    } tInterpTime.nTimeType = eTimeType;


    if (flDistanceScale > EPSILON)
    {
        float flMainIntensity; float flOutroIntensity;
        pdefVibration->primaryVibrationIntensityCurve.GetInterpolatedValueByTime(flMainVibrationInterp, tInterpTime, flMainIntensity);


        pdefVibration->outroVibrationIntensityCurve.GetInterpolatedValueByTime(flOutroVibrationInterp, tInterpTime, flOutroIntensity);

        if (pdefVibration->flPriority > sgtVibrationGlobals.flHighestPriority[nControllerIndex])
        {

            sgtVibrationGlobals.flControllerMagnitude[nControllerIndex] = flDistanceScale * (bOutro ? flOutroIntensity : flMainIntensity);

            sgtVibrationGlobals.flHighestPriority[nControllerIndex] = pdefVibration->flPriority;
        }
    }
}
}  // namespace Vibration
