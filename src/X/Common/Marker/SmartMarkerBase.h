// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartMarkerBase.h
// Members sit on their original lines (DWARF decl_line); the two in-class bodies are the
// out-of-line copies at 0x710064b880 (IsGizmoLocationMarker) and 0x710064b890 (~MarkerBase).

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "../Definitions/AttachmentDefinition.h"
#include "MarkerType.h"
typedef int32 AttachmentType;

#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"

struct AABB;
class CRefString;
struct InactiveMarkerOwner;
struct Marker;
struct MarkerSetDefinition;
struct PRTransform;
struct Quaternion;
struct Sphere;
struct WorldPlace;


class MarkerBase {  // size 0x18
    // vtable pointer
public:





    virtual SNOGroup GetSNOGroup() const;  // vtable slot 0

    virtual MarkerType GetMarkerType() const;  // vtable slot 1

    virtual CRefString GetMarkerTypeString(const Marker * ptMarker) const;  // vtable slot 2


    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3


    virtual void GetMarkerBounds(const Marker & tMarker, PRTransform & transform, Sphere & wsBounds);  // vtable slot 4




    virtual void GetAABB(const Marker & tMarker, AABB & aabbBounds);  // vtable slot 5



    virtual BOOL IsForClient(const Marker & tMarker);  // vtable slot 6


    virtual BOOL IsForPTR(const Marker & tMarker);  // vtable slot 7


    virtual BOOL IsSpecialServerMarker(const Marker & tMarker);  // vtable slot 8


    virtual BOOL IsGizmoLocationMarker() { return FALSE; }  // vtable slot 9


    virtual BOOL IsPathMarker(const Marker & tMarker);  // vtable slot 10


    virtual BOOL IsConsideredInMarkerSetBounds(const Marker & tMarker);  // vtable slot 11

















    virtual AttachmentType GetAttachmentType(SNO snoMarkerSource);  // vtable slot 12








    virtual DataID CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 13








    virtual DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 14






















    virtual void Draw(const Marker & tMarker, const PRTransform & transform, const BOOL bSelected);  // vtable slot 15














    virtual void VerifyMarkerBounds(const Marker & tMarker, const MarkerSetDefinition & tdefMarkerSet, SNO snoScene);  // vtable slot 16






    MarkerBase(SNOGroup snoGroup, MarkerType eMarkerType, const char * szDisplayString);





    MarkerBase();


    virtual ~MarkerBase() {}  // vtable slot 0

protected:

    void GetActorAABB(SNO snoActor, const PRTransform & transform, float flScale, AABB & aabbBounds);







    SNOGroup m_snoGroup;  // 0x08
    MarkerType m_eMarkerType;  // 0x0c
    const char * m_szDisplayString;  // 0x10
};
static_assert(sizeof(MarkerBase) == 0x18, "MarkerBase");
