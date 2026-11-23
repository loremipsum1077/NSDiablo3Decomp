// Original file: X/Common/Marker/SmartMarkerBase.cpp

#include "SmartMarkerBase.h"
#include "Marker.h"
#include "../Definitions/ActorDefinition.h"
#include "../Definitions/MarkerSetDefinition.h"
#include "../Definitions/SceneDefinition.h"
#include "../SNO/SNOName.h"
#include "../TagMaps/MarkerTags.h"
#include "../TagMaps/Tags.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/RefString.h"
CRefString SNOFileGetName(const SNOName & snoname);  // SNOFiles.cpp:9375, no adopted header carries it

namespace {











const RGBAColor NO_SPAWN_AREA_FILL_COLOR(0xff, 0, 0, 0x40);
const RGBAColor NO_SPAWN_AREA_BORDER_COLOR(0xff, 0, 0, 0xff);
}


MarkerBase::MarkerBase(SNOGroup snoGroup, MarkerType eMarkerType, const char * szDisplayString)
    : m_snoGroup(snoGroup)
    , m_eMarkerType(eMarkerType)
    , m_szDisplayString(szDisplayString)
{
}




MarkerBase::MarkerBase()
    : m_snoGroup(SNOGROUP_NONE)
    , m_eMarkerType(MARKERTYPE_NONE)
    , m_szDisplayString("null")
{
}

SNOGroup MarkerBase::GetSNOGroup() const
{
    return m_snoGroup;
}


MarkerType MarkerBase::GetMarkerType() const
{
    return m_eMarkerType;
}


CRefString MarkerBase::GetMarkerTypeString(const Marker *) const
{
    return m_szDisplayString;
}


SNO MarkerBase::GetActorForEdit(SNO snoSource)
{
    return SNO_NONE;
}



void MarkerBase::GetMarkerBounds(
    const Marker & tMarker,
    PRTransform & transform,
    Sphere & wsBounds)
{
    wsBounds.Set(transform.wp, 30.0f);
}


void MarkerBase::GetAABB(
    const Marker & tMarker,
    AABB & aabbBounds)
{
    aabbBounds.wp = gtransformMarkerToScene * tMarker.transform.wp;
    aabbBounds.wvExt.Set(0.5f, 0.5f, 0.5f);
}


BOOL MarkerBase::IsForClient(const Marker & tMarker)
{

    XASSERT(FALSE);
    return FALSE;
}


BOOL MarkerBase::IsForPTR(const Marker & tMarker)
{
    return TagMapGetInt(tMarker.hTagMap, TAG_MARKER_PTR_ONLY) != 0;
}







BOOL MarkerBase::IsSpecialServerMarker(const Marker & tMarker)
{
    return FALSE;
}



BOOL MarkerBase::IsConsideredInMarkerSetBounds(const Marker & tMarker)
{
    return TRUE;
}



BOOL MarkerBase::IsPathMarker(const Marker & tMarker)
{
    return FALSE;
}




DataID MarkerBase::CreateForServerGame(
    const SSceneID idSScene,
    const WorldPlace & placePosition,
    const Quaternion & qOrientation,
    const Marker & tMarker,
    const InactiveMarkerOwner & tMarkerOwner)
{
    XASSERT(FALSE);
    return -1;
}







void MarkerBase::VerifyMarkerBounds(
    const Marker & tMarker,
    const MarkerSetDefinition & tdefMarkerSet,
    SNO snoScene)
{


    if (tdefMarkerSet.dwMarkerSetFlags & 2)
    {
        return;
    }


    CRefString sMarkerSetName = SNOFileGetName(SNOName(MARKERSET_SNO, tdefMarkerSet.tHeader.snoHandle));

    AABB aabbMarker;
    GetAABB(tMarker, aabbMarker);

    if (snoScene != SNO_NONE)
    {


















        ScopedSceneDefinitionLockR pdefScene(snoScene);
        AABB aabbScene = pdefScene->aabbBounds;
        aabbScene.wvExt *= 1.4f;

        Vector3D wpMin;
        Vector3D wpMax;


        aabbScene.ComputeBounds(wpMin, wpMax);
    }
}























































AttachmentType MarkerBase::GetAttachmentType(SNO snoMarkerSource)
{
    return ATTACHMENTTYPE_INVALID;
}



DataID MarkerBase::CreateForClientGame(
    const SWorldID idSWorld,
    const SSceneID idSScene,
    const PRTransform & transform,
    const Marker & tMarker,
    const InactiveMarkerOwner & tMarkerOwner)
{
    XASSERT(FALSE);
    return -1;
}




















































void MarkerBase::Draw(
    const Marker & tMarker,
    const PRTransform & transform,
    const BOOL bSelected)
{
}

















void MarkerBase::GetActorAABB(
    SNO snoActor,
    const PRTransform & transform,
    float flScale,
    AABB & aabbBounds)
{
    ScopedActorDefinitionLockR ptDefActor(snoActor);

    OBB obbBounds;
    obbBounds.transform.wp = ptDefActor->aabbBounds.wp;
    obbBounds.transform.q = cgqIdentity;
    obbBounds.wvExt = ptDefActor->aabbBounds.wvExt * flScale;
    obbBounds.transform *= transform;

    aabbBounds.Set(obbBounds);
}
