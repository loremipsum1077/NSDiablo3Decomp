// X/Client/Wind.cpp
//
// Global and local (player scene) wind. Each function sits on its DWARF line: the name line,
// the parameter line (the DWARF puts each parameter one line below the name) and the brace
// that carries the prologue two lines below.
#include "X/Client/Wind.h"
#include "X/Client/CGame.h"
#include "X/Client/CPlayer.h"
#include "X/Client/InterpolationTrack.h"
#include "X/Client/Definitions/WeatherDefinition.h"
#include "X/Client/RActors/RActors.h"
#include "X/Common/Game.h"
#include "X/Common/GameConstants.h"
#include "X/Common/SceneCommon.h"
#include "X/Sigma/Math/MathGeneral.h"
#include "X/Sigma/Math/Vector.h"
#include "X/Sigma/Memory/SigmaMemory.h"
#include "X/Sigma/Random.h"









struct EnvironmentGlobals
{
    WindState tWindStateGlobal;  // 0x00
    WindState tWindStateLocal;  // 0x20
};









void WindState::Update(
    float flDeltaTime)
{
    RNG & rng = CGameGlobalsGet().rngLocal;
    static float sflMinAmplitude = 0.75f;  // never written: folded (DWARF: float, no location)
    static float sflMaxAmplitude = 1.25f;
    static float sflMinFrequency = 0.5f;
    static float sflMaxFrequency = 1.5f;

    // About once every 128 updates pick a new amplitude to approach.
    if ((RandGetDWord(rng) % 128) == 0)
    {
        m_flAmplitudeTarget = Math::Lerp(sflMinAmplitude, sflMaxAmplitude, RandFloat_Internal(rng));
    }

    // Approach the target amplitude.
    m_flAmplitude += 2.0f * flDeltaTime * (m_flAmplitudeTarget - m_flAmplitude);

    // And about once every 256 updates a new frequency.
    if ((RandGetDWord(rng) % 256) == 0)
    {
        // Lerp of [0.5, 1.5]: the multiply by the unit range folds away.
        m_flFrequencyTarget = Math::Lerp(sflMinFrequency, sflMaxFrequency, RandFloat_Internal(rng));
    }

    // Approach the target frequency.
    m_flFrequency += 2.0f * flDeltaTime * (m_flFrequencyTarget - m_flFrequency);

    // Advance the oscillation, wrapping once past a full turn.
    float flOmega = 2.0f * PI * m_flFrequency;
    m_flAngle += flOmega * flDeltaTime;
    if (m_flAngle > 2.0f * PI)
    {
        m_flAngle -= 2.0f * PI;
    }
}















void WindState::GetOscillatoryParams(
    Vector3D & wvWind,
    float & flAmplitude,
    float & flAngle) const
{
    wvWind = m_wvWind;
    flAmplitude = m_flAmplitude;
    flAngle = m_flAngle;
}




Vector3D WindState::GetPrevailing(
    float flPhaseShift) const
{
    // The prevailing wind swells by up to a quarter with the oscillation.

    static float sflAmplitudeScale = 0.25f;
    float flScale = 1.0f + sflAmplitudeScale * m_flAmplitude * Math::Cos(m_flAngle + flPhaseShift);
    return flScale * m_wvWind;
}




void Wind::InitializeForGame()
// Allocates the environment globals (zeroed) for the game.
{
    EnvironmentGlobals *& ptEnvGlobals = CGameGet().ptEnvGlobals;
    ptEnvGlobals = (EnvironmentGlobals *)SigmaMemoryNew(sizeof(EnvironmentGlobals), 4, NULL, TRUE);
}




void Wind::DisposeForGame()
// Frees the environment globals (the free and the reset share line 133 in the line table).
{
    EnvironmentGlobals *& ptEnvGlobals = CGameGet().ptEnvGlobals;
    SigmaMemoryFree(ptEnvGlobals, NULL); ptEnvGlobals = NULL;
}




void Wind::InitializeForWorld()
// Both wind states start calm in a new world.
{
    EnvironmentGlobals * ptEnvGlobals = CGameGet().ptEnvGlobals;

    ptEnvGlobals->tWindStateGlobal.Init();
    ptEnvGlobals->tWindStateLocal.Init();
}




void Wind::DisposeForWorld()
// Nothing to release per world.
{
}




void Wind::Update(
    float flDeltaTime)
{
    SceneCommonData * ptSCD = NULL;

    if (GameIsStartUp())
    {
        // The front end has no player: use the first scene.
        SceneCommonDataIterator tIterator;
        SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes);
    }
    else
    {
        RActorID idRActor = LocalPlayerRActorGet();
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor == NULL)
        {
            return;
        }

        SSceneID idSScene = ptRActor->idSScene;
        ptSCD = SceneCommonDataTryToGet(idSScene);
    }

    if (ptSCD == NULL)
    {
        return;
    }

    if (ptSCD->tSceneSpec.snoWeather == SNO_NONE)
    {
        return;
    }

    ScopedWeatherDefinitionLockR pdefWeather(ptSCD->tSceneSpec.snoWeather);
    EnvironmentGlobals * ptEnvGlobals = CGameGet().ptEnvGlobals;

    InterpTime tInterpTime;
    tInterpTime.Init();
    tInterpTime.nTimeType = TICK_TIME_LOOPING;
    tInterpTime.flTime = CGameGetClientTime();
    tInterpTime.flScale = pdefWeather->dwWindDuration * OO_TICKS_PER_SECOND;

    {
        Vector3D wvInterp;
        pdefWeather->pathWind.tHeader.GetInterpolantVectorFromDistributionFunction(CGameGlobalsGet().rngLocal, wvInterp);
        pdefWeather->pathWind.GetInterpolatedValueByTime(wvInterp, tInterpTime, ptEnvGlobals->tWindStateGlobal.m_wvWind);
        ptEnvGlobals->tWindStateGlobal.m_wvWind = ptEnvGlobals->tWindStateGlobal.m_wvWind * TICKS_PER_SECOND;
    }
    {
        Vector3D wvInterp;
        pdefWeather->pathWindLocal.tHeader.GetInterpolantVectorFromDistributionFunction(CGameGlobalsGet().rngLocal, wvInterp);
        pdefWeather->pathWindLocal.GetInterpolatedValueByTime(wvInterp, tInterpTime, ptEnvGlobals->tWindStateLocal.m_wvWind);
        ptEnvGlobals->tWindStateLocal.m_wvWind = ptEnvGlobals->tWindStateLocal.m_wvWind * TICKS_PER_SECOND;
    }

    ptEnvGlobals->tWindStateGlobal.Update(flDeltaTime);
    ptEnvGlobals->tWindStateLocal.Update(flDeltaTime);
}



WindState & Wind::GetGlobalWindState()
// The scene-independent wind state.
{
    return CGameGet().ptEnvGlobals->tWindStateGlobal;
}
