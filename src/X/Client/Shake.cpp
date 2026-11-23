// Original file: X/Client/Shake.cpp
// Functions sit on their original line numbers (2.6.2 DWARF).
// Not written: the linker-removed function whose locals the DWARF records at lines 502-518
// (tCGame, ptShakeGlobals, eContext, tIt, ptShake, pdefShake, wcyl).
// ShakeTryToGet, ShakeSetPosition and ShakeUpdatePropertiesFromParent have no out-of-line copy in
// the original; their bodies are checked only through the functions that inline them.
#include "Shake.h"
#include "CGame.h"
#include "CPlayer.h"
#include "RWindow.h"
#include "RActors/RActors.h"
#include "Definitions/PreferencesDefinition.h"
#include "Definitions/ShakeDefinition.h"
#include "../Common/GameConstants.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Memory/SigmaMemory.h"

// X/Client/DebugTextDraw.cpp:84; no header in src declares it yet.
void DebugTextSubmit(LPCSTR szText, const Vector3D & wp, const RWindow & tRWindow);

enum {
    MAX_SHAKES = 32,
};

struct ShakeRWindowData {  // size 0x1c
    Vector3D wvCameraOffset;  // 0x00
    Quaternion qCameraRotation;  // 0x0c
};

struct ShakeGlobals {  // size 0x8
    TDataArray<Shake, ShakeID> * ptShakes;  // 0x00
};







Shake * ShakeGet(ShakeID id)
{

    ShakeGlobals * ptShakeGlobals = CGameGet().ptShakeGlobals;
    return ptShakeGlobals->ptShakes->ElementGet(id);
}




Shake * ShakeTryToGet(ShakeID id)
{

    ShakeGlobals * ptShakeGlobals = CGameGet().ptShakeGlobals;
    return ptShakeGlobals->ptShakes->ElementTryToGet(id);
}




BOOL ShakeExists(ShakeID idShake)
{

    return ShakeTryToGet(idShake) != NULL;
}











void ShakeInitializeForGame()

{
    ShakeGlobals *& ptShakeGlobals = CGameGet().ptShakeGlobals;
    ptShakeGlobals = (ShakeGlobals *)SigmaMemoryNew(sizeof(ShakeGlobals), 4, AppGetCurrentMemoryInterface(), TRUE);

    ptShakeGlobals->ptShakes = TDataArrayCreateTracked<Shake, ShakeID>("Shake", AppGetCurrentMemoryInterface(), MAX_SHAKES);
}




void ShakeDisposeForGame()

{
    ShakeDeleteAll(RENDERCONTEXT_INVALID);

    ShakeGlobals *& ptShakeGlobals = CGameGet().ptShakeGlobals;
    if (ptShakeGlobals->ptShakes)
    {
        TDataArrayDispose(ptShakeGlobals->ptShakes);
    }

    SigmaMemoryFree(ptShakeGlobals, AppGetCurrentMemoryInterface()); ptShakeGlobals = NULL;
}




ShakeRWindowData * ShakeInitializeForRWindow()

{
    ShakeRWindowData * ptShakeRWindowData = new (SigmaMemoryNew(sizeof(ShakeRWindowData), 4, NULL, TRUE)) ShakeRWindowData();
    ptShakeRWindowData->wvCameraOffset = cgwvNull;
    ptShakeRWindowData->qCameraRotation = cgqIdentity;

    return ptShakeRWindowData;
}




void ShakeDisposeForRWindow(ShakeRWindowData * ptData)

{
    delete ptData;
}




void ShakeDeleteAll(const RenderContext eContext)
{

    ShakeGlobals * ptShakeGlobals = CGameGet().ptShakeGlobals;
    if (!ptShakeGlobals || !ptShakeGlobals->ptShakes)
    {
        return;
    }

    TDataArrayIterator<Shake> tIt;
    while (ptShakeGlobals->ptShakes->Iterate(tIt))
    {
        Shake * ptShake = tIt.ptElement;
        if (eContext == RENDERCONTEXT_INVALID || ptShake->eContext == eContext)
        {

            ShakeDelete(ptShake->id);
        }
    }
}




ShakeID ShakeAdd(
    SNO snoShake,
    const PRTransform & transform,
    const RenderContext eContext)
{
    CGame & tCGame = CGameGet();
    ShakeGlobals * ptShakeGlobals = tCGame.ptShakeGlobals;

    ShakeID idShake = ptShakeGlobals->ptShakes->ElementNew();

    if (idShake == SHAKEID_INVALID)
    {
        return SHAKEID_INVALID;
    }

    Shake * ptShake = ShakeGet(idShake);

    ptShake->snoShake = snoShake;
    ptShake->eContext = eContext;
    ptShake->flTime = 0.0f;
    ptShake->bOutro = FALSE;
    ptShake->flOutroTime = 0.0f;
    ptShake->transform = transform;

    RNG & rng = CGameGlobalsGet().rngLocal;

    ScopedShakeDefinitionLockR pdefShake(snoShake);
    pdefShake->pathCameraPositionAmplitude.tHeader.GetInterpolantVectorFromDistributionFunction(rng, ptShake->wvCameraPositionAmplitudeInterp);
    pdefShake->pathCameraPositionFrequency.tHeader.GetInterpolantVectorFromDistributionFunction(rng, ptShake->wvCameraPositionFrequencyInterp);
    ptShake->flCameraRollAmplitudeInterp = pdefShake->pathCameraRollAmplitude.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptShake->flCameraRollFrequencyInterp = pdefShake->pathCameraRollFrequency.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptShake->flCameraPitchAmplitudeInterp = pdefShake->pathCameraPitchAmplitude.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptShake->flCameraPitchFrequencyInterp = pdefShake->pathCameraPitchFrequency.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptShake->flCameraYawAmplitudeInterp = pdefShake->pathCameraYawAmplitude.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptShake->flCameraYawFrequencyInterp = pdefShake->pathCameraYawFrequency.tHeader.GetInterpolantFromDistributionFunction(rng);

    ptShake->tParentData.Init();


    return idShake;
}



void ShakeDelete(ShakeID idShake)
{

    Shake * ptShake = ShakeTryToGet(idShake);
    if (!ptShake)
    {
        return;
    }

    CGame & tCGame = CGameGet();
    ShakeGlobals * ptShakeGlobals = tCGame.ptShakeGlobals;
    ptShakeGlobals->ptShakes->ElementDeleteByID(idShake);
}





void ShakePlayOutro(ShakeID idShake)
{

    Shake * ptShake = ShakeTryToGet(idShake);
    if (!ptShake)
    {
        return;
    }

    ptShake->bOutro = TRUE;
}





void ShakeSetPosition(ShakeID idShake, const Vector3D & wp)
{


    Shake * ptShake = ShakeTryToGet(idShake);
    if (!ptShake)
    {
        return;
    }

    ptShake->transform.wp = wp;
}






void ShakeSetParent(const ShakeID idShake, const ParentData & tParentData)
{


    Shake * ptShake = ShakeTryToGet(idShake);
    if (!ptShake)
    {
        return;
    }

    ptShake->tParentData = tParentData;
}




void ShakeUpdatePropertiesFromParent(Shake * ptShake)
{

    if (!ptShake->tParentData.ParentIsValid())
    {
        return;
    }

    PRTransform transform;
    ptShake->tParentData.GetTransform(transform);

    ShakeSetPosition(ptShake->id, transform.wp);
}




static void sShakeUpdateOne(
    RWindow & tRWindow,
    ShakeRWindowData & tShakeRWindowData,
    Shake * ptShake,
    const ShakeDefinition * pdefShake,
    float flDeltaTime)
{
    if (ptShake->tParentData.m_eParentType != PARENT_NONE)
    {

        ShakeUpdatePropertiesFromParent(ptShake);
    }

    float flShakeFactor = 0.0f;







    {
        RActorID idRActor = LocalPlayerRActorGet();
        const RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            WorldDistance wdSeparation = Vector3D::Distance(ptRActor->wpWalkingPos, ptShake->transform.wp);

            if (wdSeparation < pdefShake->wdMinRange)
            {
                flShakeFactor = 1.0f;
            }
            else if (wdSeparation > pdefShake->wdMaxRange)
            {
                flShakeFactor = 0.0f;
            }
            else
            {
                float flRange = pdefShake->wdMaxRange - pdefShake->wdMinRange;

                if (flRange < EPSILON)
                {
                    flShakeFactor = 1.0f;
                }
                else
                {

                    flShakeFactor = 1.0f - (wdSeparation - pdefShake->wdMinRange) / flRange;
                }
            }
        }
    }




    ptShake->flTime += flDeltaTime;

    InterpTime tInterpTime;
    tInterpTime.Init();

    tInterpTime.flTime = ptShake->flTime;
    tInterpTime.flScale = pdefShake->tSystemDuration * OO_TICKS_PER_SECOND;

    if (ptShake->bOutro)
    {
        ptShake->flOutroTime += flDeltaTime;

        ptShake->flOutroCrossFade = ptShake->flOutroTime / (pdefShake->tOutroDuration * OO_TICKS_PER_SECOND);
        ptShake->flOutroCrossFade = Math::Pin(ptShake->flOutroCrossFade, 0.0f, 1.0f);
    }
    else
    {
        ptShake->flOutroCrossFade = 0.0f;
    }

    if (pdefShake->dwShakeFlags & (1 << SHAKE_DEF_CONTINUOUS))
    {
        if (ptShake->bOutro)
        {
            if (ptShake->flOutroTime >= pdefShake->tOutroDuration * OO_TICKS_PER_SECOND)
            {
                ShakeDelete(ptShake->id);
                return;
            }

            tInterpTime.flRatio = ptShake->flOutroCrossFade;
            tInterpTime.flOutro = ptShake->flOutroCrossFade;
            tInterpTime.nTimeType = TICK_TIME_OUTRO_CROSSFADE;
        }
        else
        {
            tInterpTime.nTimeType = TICK_TIME_LOOPING;
        }
    }
    else
    {
        if (ptShake->flTime >= pdefShake->tSystemDuration * OO_TICKS_PER_SECOND)
        {
            ShakeDelete(ptShake->id);
            return;
        }
        tInterpTime.nTimeType = TICK_TIME_NON_LOOPING;
    }



    if (AppDrawFlagIsSet(APP_DRAW_DISPLAY_SHAKE_NAMES_BIT))
    {
        Vector3D wp(ptShake->transform.wp);
        DebugTextSubmit(SNOToString(SHAKE_SNO, ptShake->snoShake, FALSE).str(), wp, tRWindow);
    }





    if (flShakeFactor > EPSILON)
    {
        float flTime = ptShake->flTime * FULL_CIRCLE;

        Vector3D wvCameraPositionAmplitude;
        pdefShake->pathCameraPositionAmplitude.GetInterpolatedValueByTime(ptShake->wvCameraPositionAmplitudeInterp, tInterpTime, wvCameraPositionAmplitude);

        Vector3D wvCameraPositionFrequency;
        pdefShake->pathCameraPositionFrequency.GetInterpolatedValueByTime(ptShake->wvCameraPositionFrequencyInterp, tInterpTime, wvCameraPositionFrequency);




        Vector3D vAngle(flTime * wvCameraPositionFrequency.x,
                        flTime * wvCameraPositionFrequency.y,
                        flTime * wvCameraPositionFrequency.z);
        vAngle.x = Math::PinWrap(vAngle.x, 0.0f, FULL_CIRCLE);
        vAngle.y = Math::PinWrap(vAngle.y, 0.0f, FULL_CIRCLE);
        vAngle.z = Math::PinWrap(vAngle.z, 0.0f, FULL_CIRCLE);

        Vector3D wvCameraPosition(
            Math::Sin(vAngle.x) * wvCameraPositionAmplitude.x,
            Math::Sin(vAngle.y) * wvCameraPositionAmplitude.y,
            Math::Sin(vAngle.z) * wvCameraPositionAmplitude.z);

        if (pdefShake->dwShakeFlags & (1 << SHAKE_DEF_RESPECT_ORIENTATION))
        {
            wvCameraPosition = ptShake->transform.q.Rotate(wvCameraPosition);
        }
        else
        {

            Vector3D::Rotate(cgwvZAxis, EIGHTH_CIRCLE, wvCameraPosition);
        }

        Angle aCameraRollAmplitude;
        pdefShake->pathCameraRollAmplitude.GetInterpolatedValueByTime(ptShake->flCameraRollAmplitudeInterp, tInterpTime, aCameraRollAmplitude);

        float flCameraRollFrequency;
        pdefShake->pathCameraRollFrequency.GetInterpolatedValueByTime(ptShake->flCameraRollFrequencyInterp, tInterpTime, flCameraRollFrequency);

        Angle aCameraPitchAmplitude;
        pdefShake->pathCameraPitchAmplitude.GetInterpolatedValueByTime(ptShake->flCameraPitchAmplitudeInterp, tInterpTime, aCameraPitchAmplitude);

        float flCameraPitchFrequency;
        pdefShake->pathCameraPitchFrequency.GetInterpolatedValueByTime(ptShake->flCameraPitchFrequencyInterp, tInterpTime, flCameraPitchFrequency);

        Angle aCameraYawAmplitude;
        pdefShake->pathCameraYawAmplitude.GetInterpolatedValueByTime(ptShake->flCameraYawAmplitudeInterp, tInterpTime, aCameraYawAmplitude);

        float flCameraYawFrequency;
        pdefShake->pathCameraYawFrequency.GetInterpolatedValueByTime(ptShake->flCameraYawFrequencyInterp, tInterpTime, flCameraYawFrequency);

        Angle aCameraRoll = Math::Sin(Math::PinWrap(flTime * flCameraRollFrequency, 0.0f, FULL_CIRCLE)) * aCameraRollAmplitude * flShakeFactor;
        Angle aCameraPitch = Math::Sin(Math::PinWrap(flTime * flCameraPitchFrequency, 0.0f, FULL_CIRCLE)) * aCameraPitchAmplitude * flShakeFactor;
        Angle aCameraYaw = Math::Sin(Math::PinWrap(flTime * flCameraYawFrequency, 0.0f, FULL_CIRCLE)) * aCameraYawAmplitude * flShakeFactor;

        Quaternion qCamera;

        qCamera.FromYPR(aCameraYaw, aCameraPitch, aCameraRoll);

        tShakeRWindowData.wvCameraOffset += wvCameraPosition * flShakeFactor;
        tShakeRWindowData.qCameraRotation = tShakeRWindowData.qCameraRotation * qCamera;
    }
}




void ShakeUpdate(
    RWindow & tRWindow,
    float flDeltaTime)
{
    RenderContext eContext = tRWindow.GetRenderContext();
    CGame & tCGame = CGameGet();
    ShakeGlobals * ptShakeGlobals = tCGame.ptShakeGlobals;

    ShakeRWindowData & tShakeRWindowData = tRWindow.GetShakeRWindowData();
    tShakeRWindowData.wvCameraOffset = cgwvNull;
    tShakeRWindowData.qCameraRotation = cgqIdentity;

    TDataArrayIterator<Shake> tIt;
    while (ptShakeGlobals->ptShakes->Iterate(tIt))
    {
        Shake * ptShake = tIt.ptElement;

        if (ptShake->eContext == eContext)
        {
            ScopedShakeDefinitionLockR pdefShake(ptShake->snoShake);
            sShakeUpdateOne(tRWindow, tShakeRWindowData, ptShake, pdefShake, flDeltaTime);
        }
    }
}


































// NON_MATCHING: 2 of 47 instructions. In the disabled branch the original computes the address of
// cgqIdentity.wdReal (add x8, x8, #0xc) before the store of wvImag.z; ours computes it after.
void ShakeGetCameraOffset(
    const ShakeRWindowData & tShakeRWindowData,
    Vector3D & wvCameraOffset,
    Quaternion & qCameraRotation)
{
    if (Prefs::GetVideoPrefs().GetDisableScreenShake())
    {
        wvCameraOffset = cgwvNull;
        qCameraRotation = cgqIdentity;
    }
    else
    {
        wvCameraOffset = tShakeRWindowData.wvCameraOffset;
        qCameraRotation = tShakeRWindowData.qCameraRotation;
    }
}
