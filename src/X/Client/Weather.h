// Minimal Weather.h: only the declarations adopted so far (owner X/Client/Weather.cpp).
#pragma once

#include "../Sigma/SigmaTypes.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/CommonTypes.h"

struct AxialCylinder;
struct OBB;
struct WeatherRWindowData;
struct ColorCorrectionParams;
struct WeatherRenderData;
struct RGBAColor;
class RWindow;

typedef int32 LightID;

namespace Weather {
void InitializeForGame();  // Weather.cpp:220
void DisposeForGame();  // Weather.cpp:230
void InitializeForWorld(RWindow & tRWindow);  // Weather.cpp:240
WeatherRWindowData * InitializeForRWindow();  // Weather.cpp:268
void DisposeForRWindow(WeatherRWindowData * ptData);  // Weather.cpp:345
RActorID GetLitPlayer(const WeatherRWindowData & tData);  // Weather.cpp:407
void AddPlayerLights(WeatherRWindowData & tData, const RActorID idRActor);  // Weather.cpp:416
void DisposeForWorld(RWindow & tRWindow);  // Weather.cpp:466
SNO GetWeatherToUse(const RWindow & tRWindow);  // Weather.cpp:1189
void Update(RWindow & tRWindow, float flDeltaTime, const BOOL fDeleteInsteadOfOutro);  // Weather.cpp:1386
float GetIntensity(const WeatherRWindowData & tData);  // Weather.cpp:1570
void Warped(RWindow & tRWindow);  // Weather.cpp:1600
void TriggerLightning(WeatherRWindowData & tData);  // Weather.cpp:1624
LightID GetLocalPlayerHeadlight(const WeatherRWindowData & tData);  // Weather.cpp:1633
void GetShadowColor(const WeatherRWindowData & tData, float & flShadowFactor, float & flAOFactor, float & flAOShadowFactor);  // Weather.cpp:1658
float GetNormalMapFactor(const WeatherRWindowData & tData);  // Weather.cpp:1672
BOOL GetFogColor(const WeatherRWindowData & tData, RGBAColor & rgbaFogColor);  // Weather.cpp:1683
void PrepareRender(WeatherRWindowData & tData);  // Weather.cpp:1760
const WeatherRenderData * GetRenderData(const WeatherRWindowData & tData);  // Weather.cpp:1771
LightID GetSunLightScene(const WeatherRWindowData & tData);  // Weather.cpp:1642
void AddWeatherVolumeCylinder(WeatherRWindowData & tData, const AxialCylinder & cyl, const SNO snoSource, const SNO snoQuestRange);  // Weather.cpp:1726
void AddWeatherVolumeOBB(WeatherRWindowData & tData, const OBB & obb, const SNO snoSource, const SNO snoQuestRange);  // Weather.cpp:1743
void GetDepthOfField(const WeatherRWindowData & tData, float & flDepthOfFieldNear, float & flDepthOfFieldFar, float & flDepthOfFieldMax);  // Weather.cpp:1698
void GetColorCorrection(WeatherRWindowData & tData, ColorCorrectionParams ** pptParams, ColorCorrectionParams ** pptParamsPrev, ColorCorrectionParams ** pptParamsPrev2);  // Weather.cpp:1712
}
