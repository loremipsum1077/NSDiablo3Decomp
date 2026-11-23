// Local processing types and declarations recovered from this CU's DWARF.
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../Contrib/Contrib/fmodstudiosrc11005/lowlevel_api/src/fmod.h"
#include "../../../Contrib/Contrib/fmodstudiosrc11005/lowlevel_api/src/fmod_dsp.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_seq.h"
#include "../../Sigma/Memory/SigmaMemory.h"

extern "C" FMOD_RESULT FMOD_DSP_GetSystemObject(FMOD_DSP*, FMOD_SYSTEM**);

namespace SignalProcessingUtils {
struct CheapLimiter {
    // DWARF: inline constructor (CheapAmpModelFModDSP.cpp:30) calling Update at line 32.
    CheapLimiter(float flThreshold, float flMargin)
    {
        Update(flThreshold, flMargin);
    }
    // DWARF: inline Update (CheapAmpModelFModDSP.cpp:35); store rows 37:21 and 39:15 give the field order.
    void Update(float flThreshold, float flMargin)
    {
        m_flInvThreshold = 1.0f / flThreshold;
        m_flThreshold = flThreshold;
        m_flMargin = flMargin;
    }
    float m_flInvThreshold, m_flThreshold, m_flMargin;
    // DWARF: inline (no out-of-line copy in the original). Inference: expression shapes from rows 51:24, 52:23/32/43/55, 53:20/22.
    float Process(float flSample)
    {
        const float flNum1 = 0.97556883f;
        const float flNum2 = -0.21852411f;
        flSample = flSample * m_flInvThreshold;
        flSample = (flNum1 + flNum2 * flSample * flSample) * flSample;
        return flSample * m_flMargin;
    }
};

struct WaveShaper {
    // DWARF: inline constructor (CheapAmpModelFModDSP.cpp:59) calling Update at line 63.
    WaveShaper(float flAmount, float flThreshold)
        : m_flAmount(flAmount)
        , m_flThreshold(flThreshold)
    {
        Update(flAmount, flThreshold);
    }
    // DWARF: inline Update (CheapAmpModelFModDSP.cpp:66) with local flMid. Inference: the formulas
    // reproduce the original's folded constants bit for bit (stores at rows 73:10, 74:10, 78:15, 79:15, 80:20).
    void Update(float flAmount, float flThreshold)
    {
        const float flMid = flThreshold * 0.5f;
        m_flX = flMid - flAmount;
        m_flY = flMid + flAmount;
        m_flSlope1 = m_flY / m_flX;
        m_flSlope2 = (flThreshold - m_flY) / (flThreshold - m_flX);
        m_flYIntersect2 = m_flY - m_flSlope2 * m_flX;
        m_flAmount = flAmount;
        m_flThreshold = flThreshold;
    }
    float m_flAmount, m_flThreshold, m_flX, m_flY, m_flSlope1, m_flSlope2, m_flYIntersect2;
    // DWARF: inline (no out-of-line copy in the original). Inference: `&&` split over 94/95 and early returns from rows 94:29, 95:13, 98:22, 100:35, 102:35.
    float Process(float flSample)
    {
        float flAbs = __builtin_fabsf(flSample);
        if (m_flAmount > 0.0001f &&
            flAbs < m_flThreshold)
        {
            if (flAbs < m_flX)
                return flSample * m_flSlope1;
            else if (flSample > 0.0f)
                return flSample * m_flSlope2 + m_flYIntersect2;
            else
                return flSample * m_flSlope2 - m_flYIntersect2;
        }
        return flSample;
    }
};

struct ThreePoleFilter {
    float m_flB1, m_flB2, m_flB3, m_flB4, m_flA2, m_flA3, m_flA4;
    float m_In1, m_In2, m_In3, m_Out1, m_Out2, m_Out3;
    // DWARF: inline, lines 117 and 122; parameter names invented. The bodies follow t3Pole_Default
    // (b1 = 1, the rest 0) and the preset rows (seven coefficients, zero history).
    ThreePoleFilter()
        : m_flB1(1.0f), m_flB2(0.0f), m_flB3(0.0f), m_flB4(0.0f), m_flA2(0.0f), m_flA3(0.0f), m_flA4(0.0f),
          m_In1(0.0f), m_In2(0.0f), m_In3(0.0f), m_Out1(0.0f), m_Out2(0.0f), m_Out3(0.0f) {
    }
    ThreePoleFilter(float flB1, float flB2, float flB3, float flB4, float flA2, float flA3, float flA4)
        : m_flB1(flB1), m_flB2(flB2), m_flB3(flB3), m_flB4(flB4), m_flA2(flA2), m_flA3(flA3), m_flA4(flA4),
          m_In1(0.0f), m_In2(0.0f), m_In3(0.0f), m_Out1(0.0f), m_Out2(0.0f), m_Out3(0.0f) {
    }
    // DWARF: inline (no out-of-line copies in the original). Process sum order from rows 138:30/40/48/55/63/70/78 and 139:21-61.
    void Reset()
    {
        m_In1 = m_In2 = m_In3 = m_Out1 = m_Out2 = m_Out3 = 0.0f;
    }
    float Process(float flSample)
    {
        const float flOut = m_flB1 * flSample + m_flB2 * m_In1 + m_flB3 * m_In2 + m_flB4 * m_In3
            - m_flA2 * m_Out1 - m_flA3 * m_Out2 - m_flA4 * m_Out3;
        m_In3 = m_In2;
        m_In2 = m_In1;
        m_In1 = flSample;
        m_Out3 = m_Out2;
        m_Out2 = m_Out1;
        m_Out1 = flOut;
        return flOut;
    }
};

namespace Presets {
ThreePoleFilter t3Pole_Default;  // line 155
ThreePoleFilter t3Pole22_44[3] = {  // line 158
    ThreePoleFilter(0.1321407f, 0.3964221f, 0.3964221f, 0.1321407f, -0.3431932f, 0.6043935f, -0.2040747f),
    ThreePoleFilter(0.07359709f, 0.22079127f, 0.22079127f, 0.07359709f, -0.9761275f, 0.8567618f, -0.2918576f),
    ThreePoleFilter(0.05546638f, 0.16639914f, 0.16639914f, 0.05546638f, -1.2235872f, 1.0378946f, -0.3705763f),
};
ThreePoleFilter t3Pole22_48[3] = {  // line 170
    ThreePoleFilter(0.1058063f, 0.3174189f, 0.3174189f, 0.1058063f, -0.6028559f, 0.6878319f, -0.2385256f),
    ThreePoleFilter(0.05888811f, 0.17666434f, 0.17666434f, 0.05888811f, -1.1764156f, 0.9711591f, -0.3236386f),
    ThreePoleFilter(0.04407065f, 0.13221194f, 0.13221194f, 0.04407065f, -1.4080652f, 1.1626776f, -0.4020473f),
};
ThreePoleFilter t3Pole22_96[3] = {  // line 182
    ThreePoleFilter(0.01674918f, 0.05024754f, 0.05024754f, 0.01674918f, -1.1764156f, 0.9711591f, -0.3236386f),
    ThreePoleFilter(0.009086207f, 0.027258622f, 0.027258622f, 0.009086207f, -2.2223058f, 1.8621825f, -0.5671871f),
    ThreePoleFilter(0.006502257f, 0.01950677f, 0.01950677f, 0.006502257f, -2.3441677f, 2.0283835f, -0.6321979f),
};
}
}
namespace CheapAmpModelDSP {
static const uint32 MAX_CHANNELS = 8;
struct CheapAmpModel {
    // DWARF: inline constructor (CheapAmpModelFModDSP.cpp:217): WaveShaper at 221, t3PoleFilter copied from
    // Presets::t3Pole_Default at 222 (memcpy of 0x34 bytes from that symbol), CheapLimiter at 226.
    // Inference: argument values are the original's stored constants (1/240, 0.1463, 0.5308, 0.563).
    CheapAmpModel()
        : tWaveShaper(1.0f / 240.0f, 0.1463f)
        , t3PoleFilter(SignalProcessingUtils::Presets::t3Pole_Default)
        , tLimiter(0.5308f, 0.563f)
    {
    }
    float Process(float flSample);
    SignalProcessingUtils::WaveShaper tWaveShaper;
    SignalProcessingUtils::ThreePoleFilter t3PoleFilter;
    SignalProcessingUtils::CheapLimiter tLimiter;
};
static_assert(sizeof(CheapAmpModel) == 0x5c, "DWARF size");
FMOD_RESULT Create(FMOD_DSP_STATE*);
FMOD_RESULT Release(FMOD_DSP_STATE*);
FMOD_RESULT Reset(FMOD_DSP_STATE*);
FMOD_RESULT Process(FMOD_DSP_STATE*, float*, float*, unsigned int, int, int*);
FMOD_RESULT SetParameter(FMOD_DSP_STATE*, int, float);
// Store order create, reset, release, read, setparameterfloat from the line rows 260-265.
#line 254 "X/Client/Sound/CheapAmpModelFModDSP.cpp"
BOOL gGetCheapAmpModelDesc(FMOD_DSP_DESCRIPTION* ptFmodDSPDescription)
{
    SigmaMemoryFill(ptFmodDSPDescription, 0, sizeof(ptFmodDSPDescription));
    Blizzard::String::Copy(ptFmodDSPDescription->name, "CheapAmpModel", 32);
    ptFmodDSPDescription->create = Create;
    ptFmodDSPDescription->reset = Reset;
    ptFmodDSPDescription->release = Release;
    ptFmodDSPDescription->read = Process;
    ptFmodDSPDescription->setparameterfloat = SetParameter;
    return 1;
}
#line 286 "X/Client/Sound/CheapAmpModelFModDSP.cpp"
FMOD_RESULT Create(FMOD_DSP_STATE* dsp_state)
{
    CheapAmpModel* ptDSPs = new CheapAmpModel[MAX_CHANNELS];
    blz::fill_n(ptDSPs, MAX_CHANNELS, CheapAmpModel());
    dsp_state->plugindata = ptDSPs;
    return Reset(dsp_state);
}
#line 296 "X/Client/Sound/CheapAmpModelFModDSP.cpp"
FMOD_RESULT Release(FMOD_DSP_STATE* dsp_state)
{
    delete[] static_cast<CheapAmpModel*>(dsp_state->plugindata);
    return FMOD_OK;
}
#line 302 "X/Client/Sound/CheapAmpModelFModDSP.cpp"
FMOD_RESULT Reset(FMOD_DSP_STATE* dsp_state)
{
    int nSampleRate = 0;
    {
        FMOD_SYSTEM* ptFModSystem;
        if (FMOD_DSP_GetSystemObject(static_cast<FMOD_DSP*>(dsp_state->instance), &ptFModSystem) != FMOD_OK)
            return FMOD_ERR_PLUGIN;
        if (FMOD_System_GetSoftwareFormat(ptFModSystem, &nSampleRate, 0, 0) != FMOD_OK)
            return FMOD_ERR_PLUGIN;
    }
    CheapAmpModel* ptDSPs = static_cast<CheapAmpModel*>(dsp_state->plugindata);
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        SignalProcessingUtils::ThreePoleFilter& t3Pole = ptDSPs[i].t3PoleFilter;
        switch (nSampleRate) {
        case 44100: t3Pole = SignalProcessingUtils::Presets::t3Pole22_44[2]; break;
        case 48000: t3Pole = SignalProcessingUtils::Presets::t3Pole22_48[2]; break;
        case 96000: t3Pole = SignalProcessingUtils::Presets::t3Pole22_96[2]; break;
        default: t3Pole = SignalProcessingUtils::Presets::t3Pole_Default; break;
        }
        t3Pole.Reset();
    }
    return FMOD_OK;
}
#line 349 "X/Client/Sound/CheapAmpModelFModDSP.cpp"
FMOD_RESULT Process(FMOD_DSP_STATE* dsp_state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int* pOutchannels)
{
    int outchannels = *pOutchannels;
    if (outchannels != inchannels)
        return FMOD_ERR_PLUGIN;
    CheapAmpModel* ptDSPs = static_cast<CheapAmpModel*>(dsp_state->plugindata);
    CheapAmpModel* ptDSPEnd = ptDSPs + inchannels;
    for (unsigned int count = 0; count < length; ++count) {
        for (CheapAmpModel* ptDSP = ptDSPs; ptDSP != ptDSPEnd; ++ptDSP)
            *outbuffer++ = ptDSP->Process(*inbuffer++);
    }
    return FMOD_OK;
}
#line 376 "X/Client/Sound/CheapAmpModelFModDSP.cpp"
FMOD_RESULT SetParameter(FMOD_DSP_STATE* dsp_state, int index, float value)
{
    return FMOD_OK;
}
#line 229 "X/Client/Sound/CheapAmpModelFModDSP.cpp"
float CheapAmpModel::Process(float flSample)
{
    if (flSample > 1.0f)
        flSample = 0.999999f;
    else if (flSample < -1.0f)
        flSample = -0.999999f;
    float flOut = tWaveShaper.Process(flSample);
    flOut = tLimiter.Process(flOut);
    flOut = t3PoleFilter.Process(flOut);
    return flOut;
}
}
