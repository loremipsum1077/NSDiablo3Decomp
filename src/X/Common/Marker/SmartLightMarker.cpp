// Original file: X/Common/Marker/SmartLightMarker.cpp

#include "X/Common/Marker/SmartLightMarker.h"

#include "X/Client/CGame.h"
#include "X/Client/Camera/CameraManager.h"
#include "X/Client/Definitions/LightDefinition.h"
#include "X/Client/Gfx/GfxPrimitives.h"
#include "X/Client/Lights.h"
#include "X/Client/RWindow.h"
#include "X/Common/GameCommonData.h"
#include "X/Common/GlobalSNO.h"
#include "X/Common/Marker/Marker.h"
#include "X/Common/TagMaps/MarkerTags.h"
#include "X/Common/TagMaps/Tags.h"
#include "X/Common/World.h"
#include "X/Sigma/Math/Rect.h"
#include "X/Sigma/Math/Sphere.h"
#include "X/Sigma/Math/Transform.h"
#include "X/Sigma/RGBAColor.h"

// X/Client/Lights.cpp:1104, :1925 and :1992; X/Client/Gfx/GfxPrimitives.cpp:486.
LightID LightAdd(const uint32 dwFlags, const SNO sno, const PRTransform & transform, const int32 nPriorityBoost, const RenderContext eContext);
WorldDistance LightComputeCutoffRadius(const float flDesiredIntensityAtCutoff, const float flIntensity, const float flC0, const float flC1, const float flC2);
float LightGetBoundingRadius(SNO snoLight);
void GfxSimpleConeDrawDeferred(const Vector3D & wpTip, const Vector3D & vDir, const float flRadius, const Angle aConeAngle, const Camera & tCamera, const RGBAColor & rgbaColor, const int32 nNumDivisions, const int32 nNumSections, GlobalSNO snoShader, const RenderContext eContext);




SNO LightMarker::GetActorForEdit(SNO snoSource)
{

    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_LIGHT);
}


void LightMarker::GetAABB(const Marker & tMarker, AABB & aabbBounds)
{
    float flRadius = 0.0f;
    PRTransform transform = gtransformMarkerToScene * tMarker.transform;








    flRadius *= TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_LIGHT_RADIUS_SCALAR);
    aabbBounds.wp = transform.wp;
    aabbBounds.wvExt.Set(flRadius, flRadius, flRadius);
}

BOOL LightMarker::IsConsideredInMarkerSetBounds(const Marker & tMarker)
{
    return FALSE;
}





























void LightMarker::GetMarkerBounds(const Marker & tMarker, PRTransform & transform, Sphere & wsBounds)

{


    float flRadius = LightGetBoundingRadius(tMarker.snoname.snoHandle);
    flRadius *= TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_LIGHT_RADIUS_SCALAR);
    wsBounds.Set(transform.wp, flRadius);
}

DataID LightMarker::CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner)
{




    DataID idObject = DATAID_INVALID;
    ScopedLightDefinitionLockR pdefLight(tMarker.snoname.snoHandle);

    if ((pdefLight->dwFlags & (1 << 10)) &&

        GameIsOpenWorld())
    {
        return DATAID_INVALID;
    }


    if ((pdefLight->dwFlags & (1 << 11)) &&

        WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(idSWorld)))
    {
        return DATAID_INVALID;
    }


    idObject = LightAdd(0, tMarker.snoname.snoHandle, transform, 0, RENDERCONTEXT_DEFAULT);
    if (idObject != DATAID_INVALID)
    {
        Light * ptLight = LightGet(idObject);

        ptLight->idSScene = idSScene;
        ptLight->tMarkerOwner = tMarkerOwner;
        LightSetScale(ptLight->id, TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_LIGHT_RADIUS_SCALAR));
    }
    return idObject;
}



void LightMarker::Draw(const Marker & tMarker, const PRTransform & transform, const BOOL bSelected)

{
    if (bSelected)
    {
        ScopedLightDefinitionLockR pdefLightDef(tMarker.snoname.snoHandle);

        if (pdefLightDef->eType == 2)
        {
            const LightStateParams & tStateA = pdefLightDef->tStateA; int32 nCameraIndex = GetRWindowMgr()->GetCurrentRWindow().GetCameraIndex();
            const Camera & cam = CameraManagerGetCamera(nCameraIndex);

            float flLightRadiusScalar = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_LIGHT_RADIUS_SCALAR);


            float wdRadiusCutoff = tStateA.pathRadius.ptArrayNodes->tValueMin * flLightRadiusScalar;
            Sphere wsCutoff;
            wsCutoff.Set(transform.wp, wdRadiusCutoff);
            Vector3D wv = transform.Rotate(cgwvZAxis);
            Angle aFalloff = tStateA.pathFalloff.ptArrayNodes->tValueMin;
            Angle aHotspot = tStateA.pathHotspot.ptArrayNodes->tValueMin;
            GfxSimpleConeDrawDeferred(wsCutoff.wpCenter, wv, wdRadiusCutoff, aFalloff, cam, crgbaRed, 16, 1, GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
            GfxSimpleConeDrawDeferred(wsCutoff.wpCenter, wv, wdRadiusCutoff, aHotspot, cam, crgbaWhite, 16, 1, GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
        }
        else
        {



            const LightStateParams & tStateA = pdefLightDef->tStateA;
            WorldDistance wdRadiusCutoff = tStateA.pathRadius.ptArrayNodes->tValueMin;
            float flIntensity = tStateA.pathIntensity.ptArrayNodes->tValueMin;
            float flC0 = tStateA.pathAttenuation[0].ptArrayNodes->tValueMin;
            float flC1 = tStateA.pathAttenuation[1].ptArrayNodes->tValueMin;
            float flC2 = tStateA.pathAttenuation[2].ptArrayNodes->tValueMin;

            const WorldDistance wdIntensityAtRadius = 25.0f / 255.0f;
            WorldDistance wdRadiusFalloff = LightComputeCutoffRadius(wdIntensityAtRadius, flIntensity, flC0, flC1, flC2);

            if (wdRadiusFalloff > 0.000001f)
            {
                Sphere wsFalloff;
                wsFalloff.Set(transform.wp, wdRadiusFalloff);
                GfxSphereDrawDeferred(wsFalloff, RGBAColor(255, 0, 255, 255), GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
            }

            Sphere wsCutoff;
            wsCutoff.Set(transform.wp, wdRadiusCutoff);
            GfxSphereDrawDeferred(wsCutoff, RGBAColor(0, 255, 255, 255), GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
        }
    }
}
