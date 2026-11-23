// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartMarker.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Client/RActors/RActors.h"
#include "../CommonTypes.h"
#include "../Definitions/AttachmentDefinition.h"
#include "MarkerType.h"
#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"


struct AABB;
class CRefString;
class CTagMap;
class GameCommonData;
struct InactiveMarkerOwner;
struct Marker;
class MarkerBase;
struct MarkerSetDefinition;
struct PRTransform;
struct Quaternion;
struct Sphere;
struct WorldPlace;

class SmartMarker {  // size 0x10
public:
    SmartMarker(Marker &);
    SmartMarker(const Marker &);
    SmartMarker(Marker *);
    SmartMarker(SNO, int32);
    SmartMarker(const MarkerSetDefinition *, int32);
    SmartMarker(GameCommonData &, MarkerID);
    Marker & MarkerGet();
    MarkerBase * InterfaceGet();
    SNOGroup GetSNOGroup() const;
    SNO GetSNOHandle() const;
    MarkerType GetMarkerType() const;
    CRefString GetDisplayString() const;
    static CRefString GetGenericDisplayString(MarkerType eType);
    SNO GetActorForEdit(SNO) const;
    static SNO GetActorForEdit(MarkerType, SNO);
    void GetMarkerBounds(PRTransform & transform, Sphere & wsBounds) const;
    void GetAABB(AABB &) const;
    const CTagMap * GetTagMap() const;
    const CHAR * GetMarkerName() const;
    const PRTransform & GetTransform() const;
    BOOL IsForClient() const;
    BOOL IsForPTR() const;
    BOOL IsSpecialServerMarker() const;
    BOOL IsGizmoLocationMarker() const;
    BOOL IsPathMarker() const;
    BOOL IsConsideredInMarkerSetBounds() const;
    AttachmentType GetAttachmentType(SNO snoMarkerSource) const;
    static AttachmentType GetAttachmentType(MarkerType eType, SNO snoMarkerSource);
    DataID CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const InactiveMarkerOwner & tMarkerOwner) const;
    DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const InactiveMarkerOwner & tMarkerOwner) const;
    void Draw(const PRTransform &, const BOOL) const;
    void VerifyMarkerBounds(const MarkerSetDefinition &);
    void VerifyMarkerBounds(const MarkerSetDefinition & tdefMarkerSet, SNO snoScene);
protected:
    static MarkerBase * GetMarkerObjectFromType(MarkerType eType);
    SmartMarker(MarkerType);
    // MarkerGet(SmartMarker &) (SmartMarker.cpp:217) reads m_pMarker directly.
    friend Marker & MarkerGet(SmartMarker & sMarker);
    friend const Marker & MarkerGet(const SmartMarker & sMarker);
    Marker * m_pMarker;  // 0x00
    MarkerBase * m_pInterface;  // 0x08
};

// SmartMarker.cpp:217 and :222.
Marker & MarkerGet(SmartMarker & sMarker);
const Marker & MarkerGet(const SmartMarker & sMarker);
static_assert(sizeof(SmartMarker) == 0x10, "SmartMarker");

// Inline members X/Client/CMarker.cpp inlines (SmartMarker.h:67, :69, :116, :119, :136). They
// call through m_pInterface, so the MarkerBase class and the Marker layout are needed here.
#include "SmartMarkerBase.h"
#include "Marker.h"

inline SNOGroup SmartMarker::GetSNOGroup() const {  // line 67
    return m_pInterface ? m_pInterface->GetSNOGroup() : SNOGROUP_NONE;
}

inline SNO SmartMarker::GetSNOHandle() const {  // line 69
    return m_pMarker->snoname.snoHandle;
}

inline AttachmentType SmartMarker::GetAttachmentType(SNO snoMarkerSource) const {  // line 116
    return m_pInterface->GetAttachmentType(snoMarkerSource);
}

inline AttachmentType SmartMarker::GetAttachmentType(MarkerType eType, SNO snoMarkerSource) {  // line 119
    return SmartMarker(eType).GetAttachmentType(snoMarkerSource);
}

inline DataID SmartMarker::CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const InactiveMarkerOwner & tMarkerOwner) const {  // line 128
    return m_pInterface->CreateForServerGame(idSScene, placePosition, qOrientation, *m_pMarker, tMarkerOwner);
}

inline DataID SmartMarker::CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const InactiveMarkerOwner & tMarkerOwner) const {  // line 136
    return m_pInterface->CreateForClientGame(idSWorld, idSScene, transform, *m_pMarker, tMarkerOwner);
}


// Functions defined in this file (bodies are not in the DWARF):
//   line    53  MarkerBase * SmartMarker::InterfaceGet()  [inlined into 1 functions]
//   line    67  SNOGroup SmartMarker::GetSNOGroup() const  [inlined into 2 functions]
//   line    69  SNO SmartMarker::GetSNOHandle() const  [inlined into 2 functions]
//   line    70  MarkerType SmartMarker::GetMarkerType() const  [inlined into 2 functions]
//   line    71  CRefString SmartMarker::GetDisplayString() const  [inline]
//   line    72  CRefString SmartMarker::GetGenericDisplayString(MarkerType eType)  [inline]
//   line    81  void SmartMarker::GetMarkerBounds(PRTransform & transform, Sphere & wsBounds) const  [inlined into 1 functions]
//   line    88  const CTagMap * SmartMarker::GetTagMap() const  [inlined into 2 functions]
//   line    90  const PRTransform & SmartMarker::GetTransform() const  [inlined into 3 functions]
//   line    92  BOOL SmartMarker::IsForClient() const  [inlined into 1 functions]
//   line    93  BOOL SmartMarker::IsForPTR() const  [inlined into 1 functions]
//   line    94  BOOL SmartMarker::IsSpecialServerMarker() const  [inline]
//   line    95  BOOL SmartMarker::IsGizmoLocationMarker() const  [inlined into 2 functions]
//   line    96  BOOL SmartMarker::IsPathMarker() const  [inlined into 1 functions]
//   line   116  AttachmentType SmartMarker::GetAttachmentType(SNO snoMarkerSource) const  [inlined into 1 functions]
//   line   119  AttachmentType SmartMarker::GetAttachmentType(MarkerType eType, SNO snoMarkerSource)  [inlined into 1 functions]
//   line   128  DataID SmartMarker::CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const InactiveMarkerOwner & tMarkerOwner) const  [inlined into 1 functions]
//   line   136  DataID SmartMarker::CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const InactiveMarkerOwner & tMarkerOwner) const  [inlined into 1 functions]
//   line   170  void SmartMarker::VerifyMarkerBounds(const MarkerSetDefinition & tdefMarkerSet, SNO snoScene)  [inlined into 1 functions]

// Original definitions are within SmartMarker's class extent (lines33–186).
#line 70 "X/Common/Marker/SmartMarker.h"
inline MarkerType SmartMarker::GetMarkerType() const { return m_pInterface->GetMarkerType(); }
#line 90 "X/Common/Marker/SmartMarker.h"
inline const PRTransform & SmartMarker::GetTransform() const { return m_pMarker->transform; }
#line 95 "X/Common/Marker/SmartMarker.h"
inline BOOL SmartMarker::IsGizmoLocationMarker() const { return m_pInterface->IsGizmoLocationMarker(); }
// DRLGEncounters.cpp:1731 and :1696/1708/1714 inline these (InterfaceGet's load is attributed
// to line 56, so its body spans 53-57).
#line 53 "X/Common/Marker/SmartMarker.h"
inline MarkerBase * SmartMarker::InterfaceGet()
{

    return m_pInterface;
}
#line 88 "X/Common/Marker/SmartMarker.h"
inline const CTagMap * SmartMarker::GetTagMap() const { return m_pMarker->hTagMap; }
// GameTesting.cpp:1460 (sCheckMarkerBounds) inlines this: ldp m_pMarker/m_pInterface, blr [vtbl+0x80]
// (MarkerBase::VerifyMarkerBounds, vtable slot 16), all on SmartMarker.h:172.
#line 170 "X/Common/Marker/SmartMarker.h"
inline void SmartMarker::VerifyMarkerBounds(const MarkerSetDefinition & tdefMarkerSet, SNO snoScene)
{

    m_pInterface->VerifyMarkerBounds(*m_pMarker, tdefMarkerSet, snoScene);
}
