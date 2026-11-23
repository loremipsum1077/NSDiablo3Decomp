// Original file: X/Client/Definitions/WeatherDefinition.cpp

#include "WeatherDefinition.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Common/GlobalSNO.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

extern const XTypeDescriptor * DT_SNO; extern const XTypeDescriptor * DT_VELOCITYVECTOR_PATH;  // D3DescribedType.cpp, InterpolationTrack.cpp

enum {
    WEATHER_COMMON_SNO_VERSION = 52,
    WEATHER_XML_VERSION = 5,
    WEATHER_STATE_PARAMS_XML_VERSION = 2,
    WEATHER_PC_SNO_VERSION = 52,
    WEATHER_CONSOLE_SNO_VERSION = 54,
    WEATHER_AXE_SNO_VERSION = 54,
    WEATHER_XBOXONE_SNO_VERSION = 54,
    WEATHER_PS4_SNO_VERSION = 54,
};












static const float DEFAULT_LIGHTNING_THRESHOLD = 0.8f;
static const float DEFAULT_BLEND_APPROACH_SPEED = 0.005f;

static const float DEFAULT_DOF_NEAR = 100.0f;
static const float DEFAULT_DOF_FAR = 250.0f;
static const float DEFAULT_DOF_MAX = 60.0f;

// Float defaults in the tables below are passed as their bit patterns, as the binary has them.













static const RGBAColor cgrgbaDefaultShadowColor(0x88, 0x88, 0x88, 0xff);

static const TypeDescriptorField WeatherStateParams_Fields[20] = {
    TypeDescriptorField("", DT_INT, offsetof(WeatherStateParams, fIsFogged), NULL, 1), TypeDescriptorField("", DT_RGBACOLOR, offsetof(WeatherStateParams, rgbaFogColor), &crgbaBlack, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeatherStateParams, flFogStart), NULL, 1), TypeDescriptorField("", DT_FLOAT, offsetof(WeatherStateParams, flFogEnd), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeatherStateParams, flHeightFogStart), NULL, 1), TypeDescriptorField("", DT_FLOAT, offsetof(WeatherStateParams, flHeightFogEnd), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(WeatherStateParams, rgbaShadowColor), &cgrgbaDefaultShadowColor, 1), TypeDescriptorField("", DT_FLOAT, offsetof(WeatherStateParams, flAOFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeatherStateParams, flAOShadowFactor), (const void *)0x3f800000, 1), TypeDescriptorField("", DT_FLOAT, offsetof(WeatherStateParams, flNormalMapFactor), (const void *)0x3f800000, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherStateParams, snoSunLightScene), "", LIGHT_SNO, 0, -1), FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherStateParams, snoSunLightActor), "", LIGHT_SNO, 0, -1),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeatherStateParams, flSunAltitude), (const void *)0x42340000, 1), TypeDescriptorField("", DT_FLOAT, offsetof(WeatherStateParams, flSunAzimuth), (const void *)0x42480000, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherStateParams, snoPlayerLightScene), "", LIGHT_SNO, 0, -1), FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherStateParams, snoPlayerLightActor), "", LIGHT_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherStateParams, snoLocalPlayerHeadLight), "", LIGHT_SNO, 0, -1), TypeDescriptorField("", DT_FLOAT, offsetof(WeatherStateParams, flZoomFogStart), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeatherStateParams, flZoomFogEnd), NULL, 0x700001), TypeDescriptorField(NULL, DT_NULL, sizeof(WeatherStateParams), NULL, 0),
};








ComplexTypeDescriptor TypeDescriptor_WeatherStateParams("WeatherStateParams", WEATHER_STATE_PARAMS_XML_VERSION, WeatherStateParams_Fields, 20, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_WEATHER_STATE_PARAMS = &TypeDescriptor_WeatherStateParams;

static const TypeDescriptorField ColorCorrectionParams_Fields[5] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(ColorCorrectionParams, snoTexSaturationVignette), "", TEXTURE_SNO, 0, -1), FilterTypeDescriptorField("", DT_SNO, offsetof(ColorCorrectionParams, snoTexCorrectionA), "", TEXTURE_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ColorCorrectionParams, snoTexCorrectionB), "", TEXTURE_SNO, 0, -1), TypeDescriptorField("", DT_INT, offsetof(ColorCorrectionParams, fRetro), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ColorCorrectionParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ColorCorrectionParams("ColorCorrectionParams", 0, ColorCorrectionParams_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_COLOR_CORRECTION_PARAMS = &TypeDescriptor_ColorCorrectionParams;

static const TypeDescriptorField WeatherDefinition_Fields[29] = {
    TypeDescriptorField("", DT_INT, offsetof(WeatherDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_TIME, offsetof(WeatherDefinition, dwWindDuration), (const void *)18000, 1),
    TypeDescriptorField("", DT_VELOCITYVECTOR_PATH, offsetof(WeatherDefinition, pathWind), &cgpathNullVectorPath, 1),
    TypeDescriptorField("", DT_VELOCITYVECTOR_PATH, offsetof(WeatherDefinition, pathWindLocal), &cgpathNullVectorPath, 1),
    TypeDescriptorField("", DT_TIME, offsetof(WeatherDefinition, dwWeatherDuration), (const void *)18000, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(WeatherDefinition, pathWeatherIntensity), &cgpathNullFloatPath, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherDefinition, snoWeatherParticle), "", PARTICLE_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherDefinition, snoFogParticle), "", PARTICLE_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherDefinition, snoSound), "", SOUND_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherDefinition, snoSkyboxActor), "", ACTOR_SNO, 0, -1),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeatherDefinition, flLightningThreshold), (const void *)0x3f4ccccd, 1),
    TypeDescriptorField("", DT_TIME, offsetof(WeatherDefinition, dwLightningDelayMin), (const void *)600, 1),
    TypeDescriptorField("", DT_TIME, offsetof(WeatherDefinition, dwLightningDelayDelta), (const void *)1500, 1),
    TypeDescriptorField("", DT_TIME, offsetof(WeatherDefinition, dwThunderDelayMax), (const void *)240, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherDefinition, snoLightning), "", LIGHT_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherDefinition, snoLightningEffectGroup), "", EFFECTGROUP_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeatherDefinition, snoThunder), "", SOUND_SNO, 0, -1),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeatherDefinition, flDepthOfFieldNear), (const void *)0x42c80000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeatherDefinition, flDepthOfFieldFar), (const void *)0x437a0000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeatherDefinition, flDepthOfFieldMax), (const void *)0x42700000, 1),
    TypeDescriptorField("", DT_WEATHER_STATE_PARAMS, offsetof(WeatherDefinition, tParamsNotRaining), NULL, 1),
    TypeDescriptorField("", DT_WEATHER_STATE_PARAMS, offsetof(WeatherDefinition, tParamsWhenRaining), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeatherDefinition, flBlendApproachSpeed), (const void *)0x3ba3d70a, 1),
    TypeDescriptorField("", DT_COLOR_CORRECTION_PARAMS, offsetof(WeatherDefinition, tColorCorrection), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(WeatherDefinition, serRandomWeatherSNOs), NULL, 0),
    FilteredVariableArrayTypeDescriptorField("", DT_SNO, offsetof(WeatherDefinition, arRandomWeatherSNOs), 0x21, (int32)offsetof(WeatherDefinition, serRandomWeatherSNOs) - (int32)offsetof(WeatherDefinition, arRandomWeatherSNOs), WEATHER_SNO),
    TypeDescriptorField("", DT_INT, offsetof(WeatherDefinition, eWeatherGroup), (const void *)WEATHER_GROUP_NONE, 1),
    TypeDescriptorField("", DT_INT, offsetof(WeatherDefinition, dwPadding), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WeatherDefinition), NULL, 0),
};






ComplexTypeDescriptor TypeDescriptor_WeatherDefinition("Weather", WEATHER_XML_VERSION, WeatherDefinition_Fields, 29, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_WEATHER = &TypeDescriptor_WeatherDefinition;





WeatherGroupDef * gptGroupDefWeather;





WeatherGroupDef::WeatherGroupDef()
    : GroupDefinition("Weather", WEATHER_SNO,
                      SNOVersions(WEATHER_PC_SNO_VERSION,
                                  WEATHER_AXE_SNO_VERSION + AXE_BASE_SNO_VERSION,
                                  WEATHER_XBOXONE_SNO_VERSION,
                                  WEATHER_PS4_SNO_VERSION,
                                  WEATHER_CONSOLE_SNO_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(WeatherDefinition), DT_WEATHER, 0x2005, 8)
{
}







uint32 WeatherGroupDef::GetMaxCount() {
    return 150;
}









static void sSetWeatherStateParamsToDefaults(WeatherStateParams & tParams)
{

    tParams.fIsFogged = FALSE;
    tParams.rgbaFogColor = crgbaBlack;
    tParams.flFogStart = 0.0f;
    tParams.flFogEnd = 0.0f;
    tParams.flHeightFogStart = 0.0f;
    tParams.flHeightFogEnd = 0.0f;
    tParams.rgbaShadowColor = cgrgbaDefaultShadowColor;
    tParams.flAOFactor = 1.0f;
    tParams.flAOShadowFactor = 1.0f;
    tParams.flNormalMapFactor = 1.0f;
    tParams.snoSunLightScene = SNO_NONE;
    tParams.snoSunLightActor = SNO_NONE;
    tParams.snoPlayerLightScene = GlobalSNOGet(GLOBALSNO_LIGHT_PLAYERLIGHT_SCENE);
    tParams.snoPlayerLightActor = GlobalSNOGet(GLOBALSNO_LIGHT_PLAYERLIGHT_ACTOR);
    tParams.snoLocalPlayerHeadLight = SNO_NONE;
    tParams.flSunAltitude = 45.0f;
    tParams.flSunAzimuth = 50.0f;

    tParams.flZoomFogStart = tParams.flFogStart;
    tParams.flZoomFogEnd = tParams.flFogEnd;
}





static void sSetColorCorrectionParamsToDefaults(ColorCorrectionParams & tParams)
{
    tParams.snoTexSaturationVignette = GlobalSNOGet(GLOBALSNO_POSTFX_SATURATION_VIGNETTE_TEXTURE);
    tParams.snoTexCorrectionA = GlobalSNOGet(GLOBALSNO_POSTFX_COLOR_CORRECTION_A_TEXTURE);
    tParams.snoTexCorrectionB = GlobalSNOGet(GLOBALSNO_POSTFX_COLOR_CORRECTION_B_TEXTURE);
    tParams.fRetro = FALSE;
}





void WeatherGroupDef::GetDefaultData(void * ptDefaultDef) const
{
    SigmaMemoryClear(ptDefaultDef, sizeof(WeatherDefinition));
    WeatherDefinition * pdef = (WeatherDefinition *)ptDefaultDef;

    pdef->dwFlags = 0;
    pdef->dwWindDuration = 18000;
    pdef->pathWind = cgpathNullVectorPath;
    pdef->pathWindLocal = cgpathNullVectorPath;
    pdef->dwWeatherDuration = 18000;
    pdef->pathWeatherIntensity = cgpathNullFloatPath;
    pdef->snoWeatherParticle = SNO_NONE;
    pdef->snoFogParticle = SNO_NONE;
    pdef->snoSound = SNO_NONE;
    pdef->flLightningThreshold = DEFAULT_LIGHTNING_THRESHOLD;
    pdef->dwLightningDelayMin = 600;
    pdef->dwLightningDelayDelta = 1500;
    pdef->dwThunderDelayMax = 240;
    pdef->snoLightning = SNO_NONE;
    pdef->snoLightningEffectGroup = SNO_NONE;
    pdef->snoThunder = SNO_NONE;
    pdef->flDepthOfFieldNear = DEFAULT_DOF_NEAR;
    pdef->flDepthOfFieldFar = DEFAULT_DOF_FAR;
    pdef->flDepthOfFieldMax = DEFAULT_DOF_MAX;
    sSetWeatherStateParamsToDefaults(pdef->tParamsNotRaining);
    sSetWeatherStateParamsToDefaults(pdef->tParamsWhenRaining);
    pdef->flBlendApproachSpeed = DEFAULT_BLEND_APPROACH_SPEED;
    sSetColorCorrectionParamsToDefaults(pdef->tColorCorrection);
}
