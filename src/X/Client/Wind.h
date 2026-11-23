// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Wind.h
//
// Lines follow the DWARF: WindState at 13, Init defined in the class at 15 (its inlined
// stores come from lines 18-22), the method declarations at 26-34 and the members at 39-44.

#pragma once

#include "../Sigma/Math/Vector.h"



struct WindState
{
    void Init()
    {
        // Calm: no wind vector, angle and amplitude at rest, unit frequency.
        m_wvWind = cgwvNull;
        m_flAngle = 0.0f;
        m_flAmplitudeTarget = 0.0f;
        m_flAmplitude = 0.0f;
        m_flFrequencyTarget = m_flFrequency = 1.0f;
    }


    void Update(float flDeltaTime);


    Vector3D GetOscillatory(float flPhaseShift) const;

    Vector3D GetPrevailing(float flPhaseShift) const;


    void GetOscillatoryParams(Vector3D & wvWind, float & flAmplitude, float & flAngle) const;




    float m_flAngle;  // 0x00
    float m_flAmplitudeTarget;  // 0x04
    float m_flAmplitude;  // 0x08
    float m_flFrequencyTarget;  // 0x0c
    float m_flFrequency;  // 0x10
    Vector3D m_wvWind;  // 0x14
};
static_assert(sizeof(WindState) == 0x20, "WindState");

// Defined in Wind.cpp (lines 119-229); CGame.cpp declares the ones it calls itself.
namespace Wind {
void InitializeForGame();
void DisposeForGame();
void InitializeForWorld();
void DisposeForWorld();
void Update(float flDeltaTime);
WindState & GetGlobalWindState();
}  // namespace Wind
