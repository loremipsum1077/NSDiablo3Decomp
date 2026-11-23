// Original file: X/Common/Marker/SmartAmbientSoundMarker.cpp

#include "X/Common/Marker/SmartAmbientSoundMarker.h"

#include "X/Client/CGame.h"
#include "X/Client/Definitions/AmbientSoundDefinition.h"
#include "X/Client/Definitions/SoundDefinition.h"
#include "X/Client/Gfx/GfxPrimitives.h"
#include "X/Client/Sound/AmbientSound.h"
#include "X/Common/Definitions/MarkerSetDefinition.h"
#include "X/Common/GlobalSNO.h"
#include "X/Common/Marker/Marker.h"
#include "X/Common/TagMaps/MarkerTags.h"
#include "X/Common/TagMaps/Tags.h"
#include "X/Sigma/Math/Cylinder.h"
#include "X/Sigma/Math/Rect.h"
#include "X/Sigma/Math/Sphere.h"
#include "X/Sigma/Math/Transform.h"
#include "X/Sigma/RGBAColor.h"

// X/Client/Gfx/GfxPrimitives.cpp:1153.
void GfxCapsuleDrawDeferred(const Capsule & c, const PRTransform & transform, const RGBAColor & rgbaColor, GlobalSNO snoShader, const RenderContext eContext);











SNO AmbientSoundMarker::GetActorForEdit(SNO snoSource)
{

    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_SOUND);
}





void AmbientSoundMarker::GetAABB(const Marker & tMarker, AABB & aabbBounds)
{
    float flRadius = 0.0f;

    PRTransform transform = gtransformMarkerToScene * tMarker.transform;







    aabbBounds.wp = transform.wp;
    aabbBounds.wvExt.Set(flRadius, flRadius, flRadius);
}






























void AmbientSoundMarker::GetMarkerBounds(const Marker & tMarker, PRTransform & transform, Sphere & wsBounds)

{


    float flRadius = AmbientSound::GetRadius(tMarker.snoname.snoHandle);
    wsBounds.Set(transform.wp, flRadius);
}




static void sResolveLinkToAudioVolume(AmbientSound * ptAmbientSound, const InactiveMarkerOwner & tMarkerOwner, uint32 tName)




{
    ScopedMarkerSetDefinitionLockR pdefMarkerSet(tMarkerOwner.snoMarkerSet);
    for (int32 nMarkerIndex = 0; nMarkerIndex < pdefMarkerSet->tMarkerSet.GetArraySize(); nMarkerIndex++)
    {
        const Marker & tMarker = pdefMarkerSet->tMarkerSet[nMarkerIndex];
        if (tMarker.eType == MARKERTYPE_AUDIOVOLUME)
        {
            for (int32 i = 0; i < tMarker.nLinkCount; i++)
            {

                if (tMarker.ptMarkerLinks[i].dwHash == tName)
                {
                    ptAmbientSound->m_fLinkedToAudioVolume = TRUE;
                    ptAmbientSound->m_tMarkerName = tMarker.dwHash;
                    return;
                }
            }
        }
    }

    ptAmbientSound->m_fLinkedToAudioVolume = FALSE;
}




DataID AmbientSoundMarker::CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner)




{
    float flCapsuleLength = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_AMBIENTSOUND_CAPSULELENGTH);
    DataID idObject = AmbientSound::Add(tMarker.snoname.snoHandle, transform, RENDERCONTEXT_DEFAULT, flCapsuleLength, FALSE);
    if (idObject != DATAID_INVALID)
    {
        AmbientSound * ptAmbientSound = AmbientSoundGet(idObject);

        ptAmbientSound->m_idSScene = idSScene;
        ptAmbientSound->m_idSWorld = idSWorld;
        ptAmbientSound->m_tMarkerOwner = tMarkerOwner;
        sResolveLinkToAudioVolume(ptAmbientSound, tMarkerOwner, tMarker.dwHash);


        ptAmbientSound->m_snoQuestRange = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_QUESTRANGE);
    }

    return idObject;
}





void AmbientSoundMarker::Draw(const Marker & tMarker, const PRTransform & transform, const BOOL bSelected)



{
    AppGlobals & tAppGlobals = sgtAppGlobals;

    if (!bSelected &&
        tAppGlobals.eDebugDisplayMode != DDM_SOUND_STATUS)
        return;
    float flCapsuleLength;
    BOOL fDrewSomething = FALSE;
    ScopedAmbientSoundDefinitionLockR pdefAmbSound(tMarker.snoname.snoHandle);
    flCapsuleLength = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_AMBIENTSOUND_CAPSULELENGTH);

    if (pdefAmbSound->snoBackground != SNO_NONE)
    {
        ScopedSoundDefinitionLockR pdefSound(pdefAmbSound->snoBackground);
        Capsule capsuleFalloffInner;
        Capsule capsuleFalloffOuter;
        capsuleFalloffInner.Set(transform, pdefSound->wdFalloffRadiusMin, flCapsuleLength);
        capsuleFalloffOuter.Set(transform, pdefSound->wdFalloffRadiusMin + pdefSound->wdFalloffRadiusDelta, flCapsuleLength);

        GfxCapsuleDrawDeferred(capsuleFalloffInner, cgtPRTransformIdentity, crgbaBlue, GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
        GfxCapsuleDrawDeferred(capsuleFalloffOuter, cgtPRTransformIdentity, crgbaBlue, GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
        fDrewSomething = TRUE;
    }
    else
    {
        for (int32 i = 0; i < 2; i++)
        {
            const RandomAmbientSoundParams & tParams = pdefAmbSound->tRandomSounds[i];
            if (tParams.snoSound != SNO_NONE)
            {
                Capsule capsuleFalloffInner;
                Capsule capsuleFalloffOuter;
                capsuleFalloffInner.Set(transform, tParams.flRadiusMin, flCapsuleLength);
                capsuleFalloffOuter.Set(transform, tParams.flRadiusMin + tParams.flRadiusDelta, flCapsuleLength);
                GfxCapsuleDrawDeferred(capsuleFalloffInner, cgtPRTransformIdentity, crgbaGreen, GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
                GfxCapsuleDrawDeferred(capsuleFalloffOuter, cgtPRTransformIdentity, crgbaGreen, GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
                fDrewSomething = TRUE;
            }
        }
    }
    if (!fDrewSomething)
    {
        if (pdefAmbSound->snoFixedIntervalSound != SNO_NONE)
        {
            ScopedSoundDefinitionLockR pdefSound(pdefAmbSound->snoFixedIntervalSound);
            Capsule capsuleFalloffInner;
            Capsule capsuleFalloffOuter;
            capsuleFalloffInner.Set(transform, pdefSound->wdFalloffRadiusMin, flCapsuleLength);
            capsuleFalloffOuter.Set(transform, pdefSound->wdFalloffRadiusMin + pdefSound->wdFalloffRadiusDelta, flCapsuleLength);
            GfxCapsuleDrawDeferred(capsuleFalloffInner, cgtPRTransformIdentity, crgbaRed, GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
            GfxCapsuleDrawDeferred(capsuleFalloffOuter, cgtPRTransformIdentity, crgbaRed, GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
        }
    }
}
