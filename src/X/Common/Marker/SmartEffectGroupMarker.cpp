// X/Common/Marker/SmartEffectGroupMarker.cpp
//
// Byte-matching decompilation of Diablo III 2.6.2 (Switch, clang 5.0.1).
// Evidence for every body below is the DWARF record plus the line-attributed
// listing in the packet (file/line columns cited per instruction).
//
// Function order follows the original addresses/DWARF decl lines:
//   line  29  SNO      EffectGroupMarker::GetActorForEdit(SNO snoSource)            0x71006bed90
//   line  36  void     EffectGroupMarker::GetAABB(const Marker&, AABB&)            0x71006beda0
//   line  57  BOOL     EffectGroupMarker::IsConsideredInMarkerSetBounds(const Marker&) 0x71006bee90
//   line  83  DataID   EffectGroupMarker::CreateForClientGame(...)                 0x71006beea0
//   line 147  void     EffectGroupMarker::Draw(...)                                0x71006befb0
// EffectGroupMarker::IsForClient (0x71006befc0) and ~EffectGroupMarker (0x71006befd0)
// belong to SmartEffectGroupMarker.h (DWARF decl file) and are handled there.

#include "SmartEffectGroupMarker.h"

#include "../../Client/CMarker.h"
#include "../../Client/RActors/RActors.h"
#include "../GlobalSNO.h"
#include "../InactiveMarker.h"
#include "Marker.h"                // also declares gtransformMarkerToScene (Marker.cpp:2763)
#include "../../Sigma/Math/Rect.h"  // AABB (Rect.h:103)

// SmartEffectGroupMarker.cpp:29; 0x71006bed90. 8 bytes:
//   mov w0,#0x3c7 ; b _Z12GlobalSNOGeti
// 0x3c7 = 967 = GLOBALSNO_MARKER_ACTOR_PARTICLE (X/Common/GlobalSNO.h:49).
SNO EffectGroupMarker::GetActorForEdit(SNO snoSource)
{
    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_PARTICLE);
}

// SmartEffectGroupMarker.cpp:36; 0x71006beda0. 236 bytes.
// DWARF inlines: PRTransform operator* at line 40 (Transform.h:67), Quaternion::Rotate
// (Quaternion.inl:144) and Vector3D operator+ (Vector.inl:535) inside it,
// Vector3D::operator= at line 41 (Vector.inl:502) and Vector3D::Set at line 42
// (Vector.inl:637). transformResult.q is dead in the original (its DWARF location
// is `DW_OP_piece 0x10` with no q pieces), so only wp is written out.
void EffectGroupMarker::GetAABB(const Marker & tMarker, AABB & aabbBounds)
{
    PRTransform transform = gtransformMarkerToScene * tMarker.transform;
    aabbBounds.wp = transform.wp;
    aabbBounds.wvExt.Set(1.5f, 1.5f, 1.5f);
}

// SmartEffectGroupMarker.cpp:57; 0x71006bee90. 8 bytes: `mov w0,wzr ; ret` (F0:60).
BOOL EffectGroupMarker::IsConsideredInMarkerSetBounds(const Marker & tMarker)
{
    return FALSE;
}

// SmartEffectGroupMarker.cpp:83; 0x71006beea0. 260 bytes.
//  F0:120 vtable slot 3 call with w1 = -1  -> GetActorForEdit(SNO_NONE)
//  F0:119 RActorCreate(snoActor, 1.0f, -1.0f, transform, 0, RENDERCONTEXT_DEFAULT, -1)
//  F0:126 `cbz w22` (idRActor == 0 skips) and the inlined RActorTryToGet INVALID check
//  F0:129 ptRActor->idSScene, F0:130 ptRActor->tMarkerOwner, F0:131 MarkerAddAttachments...
DataID EffectGroupMarker::CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner)
{
    RActorID idRActor = RActorCreate(GetActorForEdit(SNO_NONE), 1.0f, -1.0f, transform, 0, RENDERCONTEXT_DEFAULT, -1);

    if (idRActor)
    {
        RActor * ptRActor = RActorTryToGet(idRActor);
        ptRActor->idSScene = idSScene;
        ptRActor->tMarkerOwner = tMarkerOwner;
        MarkerAddAttachmentsToPlaceholder(idRActor, MARKERTYPE_EFFECTGROUP, tMarker.snoname.snoHandle);
    }

    return idRActor;
}

// SmartEffectGroupMarker.cpp:147; 0x71006befb0. 4 bytes: `ret` (F0:184).
void EffectGroupMarker::Draw(const Marker & tMarker, const PRTransform & transform, const BOOL bSelected)
{
}
