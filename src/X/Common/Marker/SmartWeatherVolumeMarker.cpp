// X/Common/Marker/SmartWeatherVolumeMarker.cpp: MARKERTYPE_WEATHERVOLUME's MarkerBase
// implementation. Members sit on their original DWARF lines.

#include "SmartWeatherVolumeMarker.h"
#include "../GlobalSNO.h"
#include "../InactiveMarker.h"
#include "../TagMaps/MarkerTags.h"
#include "../TagMaps/Tags.h"
#include "../World.h"
#include "Marker.h"
#include "../../Client/RWindow.h"
#include "../../Client/Weather.h"
#include "../../Client/Gfx/GfxPrimitives.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Shape.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/RGBAColor.h"






SNO WeatherVolumeMarker::GetActorForEdit(SNO snoSource)
{
    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_LIGHT);  // 0x3c4: weather volumes reuse the light marker actor
}






















void WeatherVolumeMarker::sGetShape(
    const Marker & tMarker,
    const PRTransform & transform,
    Shape & shape)
{
    shape.eType = (Shape::Type)TagMapGetInt(tMarker.hTagMap, TAG_MARKER_WEATHERVOLUME_SHAPE);
    switch (shape.eType)
    {
    case Shape::AXIAL_CYLINDER:
    {
        const float DEFAULT_HEIGHT = 15.0f;
        shape.axialCylinder.Set(transform.wp, DEFAULT_HEIGHT, TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_WEATHERVOLUME_RADIUS));
        break;
    }
    case Shape::BOX:
        shape.box.transform = transform;
        shape.box.wvExt.x = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_WEATHERVOLUME_OBB_X);
        shape.box.wvExt.y = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_WEATHERVOLUME_OBB_Y);
        shape.box.wvExt.z = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_WEATHERVOLUME_OBB_Z);
        break;
    default:
        break;
    }
}







void WeatherVolumeMarker::GetAABB(
    const Marker & tMarker,
    AABB & aabbBounds)
{
    PRTransform transform = gtransformMarkerToScene * tMarker.transform;
    Shape shape;

    sGetShape(tMarker, transform, shape);
    switch (shape.eType)
    {
    case Shape::AXIAL_CYLINDER:
        aabbBounds.wp = shape.axialCylinder.wpBase;
        aabbBounds.wvExt.Set(shape.axialCylinder.wdRadius, shape.axialCylinder.wdRadius, shape.axialCylinder.wdHeight);
        break;
    case Shape::BOX:
        aabbBounds.wp = shape.box.transform.wp;
        aabbBounds.wvExt = shape.box.wvExt;
        break;
    default:
        break;
    }
}





void WeatherVolumeMarker::GetMarkerBounds(
    const Marker & tMarker,
    PRTransform & transform,
    Sphere & wsBounds)
{
    Shape shape;

    sGetShape(tMarker, transform, shape);
    switch (shape.eType)
    {
    case Shape::AXIAL_CYLINDER:
        wsBounds.Set(transform.wp, shape.axialCylinder.wdRadius);
        break;
    case Shape::BOX:
        wsBounds.wpCenter = shape.box.transform.wp;
        wsBounds.wdRadius = Math::Sqrt(shape.box.wvExt.x * shape.box.wvExt.x + shape.box.wvExt.y * shape.box.wvExt.y + shape.box.wvExt.z * shape.box.wvExt.z);
        break;
    default:
        break;
    }
}




DataID WeatherVolumeMarker::CreateForClientGame(
    const SWorldID idSWorld,
    const SSceneID idSScene,
    const PRTransform & transform,
    const Marker & tMarker,
    const InactiveMarkerOwner & tMarkerOwner)
{
    if (!WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(idSWorld)))
    {
        Shape shape;
        sGetShape(tMarker, transform, shape);
        if (shape.eType == Shape::AXIAL_CYLINDER)
        {
            Weather::AddWeatherVolumeCylinder(
                GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData(),
                shape.axialCylinder,
                tMarker.snoname.snoHandle,
                TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_QUESTRANGE));
        }
        else if (shape.eType == Shape::BOX)
        {
            Weather::AddWeatherVolumeOBB(
                GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData(),
                shape.box,
                tMarker.snoname.snoHandle,
                TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_QUESTRANGE));
        }
    }

    return -1;
}





void WeatherVolumeMarker::Draw(
    const Marker & tMarker,
    const PRTransform & transform,
    const BOOL bSelected)
{
    if (bSelected)
    {
        Shape shape;
        sGetShape(tMarker, transform, shape);
        if (shape.eType == Shape::AXIAL_CYLINDER)
            GfxAxialCylinderDrawDeferred(shape.axialCylinder, RGBAColor(0xff, 0, 0, 0xff), GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
        else if (shape.eType == Shape::BOX)
            GfxOBB3DDrawDeferred(shape.box, RGBAColor(0xff, 0, 0, 0xff), GLOBALSNO_SHADER_3D_PRIMS, GFX_LINE_STYLE_SOLID, RENDERCONTEXT_DEFAULT);
    }
}
