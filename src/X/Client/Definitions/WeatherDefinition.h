// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/WeatherDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../InterpolationTrack.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;

enum {
    WEATHER_CAUSES_FROST_BREATH_BIT = 0,
    WEATHER_CAUSES_RIPPLES_BIT = 1,
};

enum {
    WEATHER_GROUP_NONE = -1,
    WEATHER_GROUP_TRISTRAM = 0,
    WEATHER_GROUP_HIGHLANDS = 1,
    WEATHER_GROUP_CALDEUM = 2,
    WEATHER_GROUP_BONEYARDS = 3,
    WEATHER_GROUP_BASTIONS_KEEP = 4,
    WEATHER_GROUP_HEAVEN = 5,
    WEATHER_GROUP_WESTMARCH = 6,
    WEATHER_GROUP_BOG = 7,
    WEATHER_GROUP_PANDEMONIUM = 8,
    WEATHER_GROUP_BRIDGE = 9,
    WEATHER_GROUP_FESTERING_WOODS = 10,
    WEATHER_GROUP_OASIS = 11,
    WEATHER_GROUP_CRATER = 12,
    WEATHER_GROUP_FORTRESS = 13,
};

typedef int32 WeatherGroup;

struct WeatherStateParams {  // size 0x4c
    BOOL fIsFogged;  // 0x00
    RGBAColor rgbaFogColor;  // 0x04
    float flFogStart;  // 0x08
    float flFogEnd;  // 0x0c
    float flHeightFogStart;  // 0x10
    float flHeightFogEnd;  // 0x14
    RGBAColor rgbaShadowColor;  // 0x18
    float flAOFactor;  // 0x1c
    float flAOShadowFactor;  // 0x20
    float flNormalMapFactor;  // 0x24
    SNO snoSunLightScene;  // 0x28
    SNO snoSunLightActor;  // 0x2c
    float flSunAltitude;  // 0x30
    float flSunAzimuth;  // 0x34
    SNO snoPlayerLightScene;  // 0x38
    SNO snoPlayerLightActor;  // 0x3c
    SNO snoLocalPlayerHeadLight;  // 0x40
    float flZoomFogStart;  // 0x44
    float flZoomFogEnd;  // 0x48
};
static_assert(sizeof(WeatherStateParams) == 0x4c, "WeatherStateParams");

struct ColorCorrectionParams {  // size 0x10
    SNO snoTexSaturationVignette;  // 0x00
    SNO snoTexCorrectionA;  // 0x04
    SNO snoTexCorrectionB;  // 0x08
    BOOL fRetro;  // 0x0c
};
static_assert(sizeof(ColorCorrectionParams) == 0x10, "ColorCorrectionParams");

struct WeatherDefinition {  // size 0x1b0
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    uint32 dwWindDuration;  // 0x10
    VectorPath pathWind;  // 0x18
    VectorPath pathWindLocal;  // 0x48
    uint32 dwWeatherDuration;  // 0x78
    FloatPath pathWeatherIntensity;  // 0x80
    SNO snoWeatherParticle;  // 0xb0
    SNO snoFogParticle;  // 0xb4
    SNO snoSound;  // 0xb8
    SNO snoSkyboxActor;  // 0xbc
    float flLightningThreshold;  // 0xc0
    uint32 dwLightningDelayMin;  // 0xc4
    uint32 dwLightningDelayDelta;  // 0xc8
    uint32 dwThunderDelayMax;  // 0xcc
    SNO snoLightning;  // 0xd0
    SNO snoLightningEffectGroup;  // 0xd4
    SNO snoThunder;  // 0xd8
    float flDepthOfFieldNear;  // 0xdc
    float flDepthOfFieldFar;  // 0xe0
    float flDepthOfFieldMax;  // 0xe4
    WeatherStateParams tParamsNotRaining;  // 0xe8
    WeatherStateParams tParamsWhenRaining;  // 0x134
    float flBlendApproachSpeed;  // 0x180
    ColorCorrectionParams tColorCorrection;  // 0x184
    XPtr64<int> arRandomWeatherSNOs;  // 0x198
    SerializeData serRandomWeatherSNOs;  // 0x1a0
    WeatherGroup eWeatherGroup;  // 0x1a8
    uint32 dwPadding;  // 0x1ac
};
static_assert(sizeof(WeatherDefinition) == 0x1b0, "WeatherDefinition");

class WeatherGroupDef : public GroupDefinition {  // size 0xa0
public:
    WeatherGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 134; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 137
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_CLIENTDATA; }  // vtable slot 7, line 139
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(WeatherGroupDef) == 0xa0, "WeatherGroupDef");

extern WeatherGroupDef * gptGroupDefWeather;  // WeatherDefinition.cpp:136

typedef ScopedDefinitionLockR<WeatherDefinition> ScopedWeatherDefinitionLockR;

// Line 167. Its callers call LockRFast and have nothing from an unlock, so
// DefinitionUnlockR<WeatherDefinition> is taken to be empty, as for FlagSetDefinition.
template <> inline const WeatherDefinition * DefinitionLockR<WeatherDefinition>(SNO sno) {
    return (const WeatherDefinition *)gptGroupDefWeather->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<WeatherDefinition>(const WeatherDefinition *) {}

// Functions defined in this file (bodies are not in the DWARF):
//   line   129  WeatherGroupDef::~WeatherGroupDef()  [0x71001c4890]
//   line   134  void * WeatherGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x71001c48c0 (+1 more)]
//   line   134  void WeatherGroupDef::MemFree(void *& ptr) const  [0x71001c4910 (+1 more)]
//   line   137  BOOL WeatherGroupDef::CanReferenceOtherSNOs() const  [0x71001c49a0]
//   line   139  SNOPackType WeatherGroupDef::GetSNOPackDestination(const void *) const  [0x71001c4990]
//   line   167  const WeatherDefinition * DefinitionLockR<WeatherDefinition>(SNO sno)  [inlined into 5 functions]
