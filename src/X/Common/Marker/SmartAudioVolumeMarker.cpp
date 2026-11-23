// Original file: X/Common/Marker/SmartAudioVolumeMarker.cpp

#include "SmartAudioVolumeMarker.h"
#include "../GlobalSNO.h"
#include "../InactiveMarker.h"
#include "../TagMaps/MarkerTags.h"
#include "../TagMaps/Tags.h"
#include "../World.h"
#include "Marker.h"
#include "../../Client/CWorld.h"
#include "../../Client/Gfx/GfxPrimitives.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Shape.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/RGBAColor.h"









SNO AudioVolumeMarker::GetActorForEdit(SNO snoSource)
{
    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_SOUND);
}






void AudioVolumeMarker::GetAABB(
    const Marker & tMarker,
    AABB & aabbBounds)
{
    PRTransform transform = gtransformMarkerToScene * tMarker.transform;
    Shape shape;

    sGetShape(tMarker, transform, shape);
    switch (shape.eType)
    {
    case Shape::SPHERE:
        aabbBounds.wp = shape.sphere.wpCenter;
        aabbBounds.wvExt.Set(shape.sphere.wdRadius, shape.sphere.wdRadius, shape.sphere.wdRadius);
        break;
    case Shape::BOX:
        aabbBounds.wp = shape.box.transform.wp;
        aabbBounds.wvExt = shape.box.wvExt;
        break;
    default:
        break;
    }


}




















void AudioVolumeMarker::sGetShape(
    const Marker & tMarker,
    const PRTransform & transform,
    Shape & shape)
{
    shape.eType = (Shape::Type)TagMapGetInt(tMarker.hTagMap, TAG_MARKER_AUDIOVOLUME_SHAPE);
    switch (shape.eType)
    {
    case Shape::SPHERE:
    {
        shape.sphere.Set(transform.wp, TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_AUDIOVOLUME_RADIUS));
        break;
    }
    case Shape::BOX:
        shape.box.transform = transform;
        shape.box.wvExt.x = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_AUDIOVOLUME_OBB_X);
        shape.box.wvExt.y = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_AUDIOVOLUME_OBB_Y);
        shape.box.wvExt.z = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_AUDIOVOLUME_OBB_Z);
        break;
    default:
        break;
    }

}






void AudioVolumeMarker::GetMarkerBounds(
    const Marker & tMarker,
    PRTransform & transform,
    Sphere & wsBounds)
{
    Shape shape;
    sGetShape(tMarker, transform, shape);
    switch (shape.eType)
    {
    case Shape::SPHERE:
        wsBounds = shape.sphere;
        break;
    case Shape::BOX:
        wsBounds.wpCenter = shape.box.transform.wp;

        wsBounds.wdRadius = Math::Sqrt(shape.box.wvExt.x * shape.box.wvExt.x + shape.box.wvExt.y * shape.box.wvExt.y + shape.box.wvExt.z * shape.box.wvExt.z);
        break;
    default:
        break;
    }


}




DataID AudioVolumeMarker::CreateForClientGame(
    const SWorldID idSWorld,
    const SSceneID,
    const PRTransform & transform,
    const Marker & tMarker,
    const InactiveMarkerOwner &)
{
    if (!WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(idSWorld)))
    {



        AudioSettings tSettings;
        tSettings.snoMusic = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_AUDIOVOLUME_MUSIC);
        tSettings.snoAmbient = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_AUDIOVOLUME_AMBIENT);
        tSettings.snoReverb = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_AUDIOVOLUME_REVERB);

        Shape shape;
        sGetShape(tMarker, transform, shape);

        SNO snoOnEntryEffectGroup = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_AUDIOVOLUME_EFFECTGROUP_ON_ENTRY);
        CWorldAddAudioVolume(shape, tMarker, tSettings, snoOnEntryEffectGroup);
    }

    return -1;
}



void AudioVolumeMarker::Draw(
    const Marker & tMarker,
    const PRTransform & transform,
    const BOOL bSelected)
{
    if (bSelected)
    {
        Shape shape;
        sGetShape(tMarker, transform, shape);

        if (shape.eType == Shape::SPHERE)
        {

            GfxSphereDrawDeferred(shape.sphere, RGBAColor(0xff, 0, 0, 0xff), GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
        }
        else if (shape.eType == Shape::BOX)
        {
            GfxOBB3DDrawDeferred(shape.box, RGBAColor(0xff, 0, 0, 0xff), GLOBALSNO_SHADER_3D_PRIMS, GFX_LINE_STYLE_SOLID, RENDERCONTEXT_DEFAULT);
        }
    }
}
