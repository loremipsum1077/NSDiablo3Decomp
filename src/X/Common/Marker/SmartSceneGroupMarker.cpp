// X/Common/Marker/SmartSceneGroupMarker.cpp
//
// Reconstructed from the DWARF and the line-attributed listing of the
// Diablo III 2.6.2 (Switch, clang 5.0.1) build.
//
// Inventory of this file (DWARF source order):
//   line 24  SNO    SceneGroupMarker::GetActorForEdit(SNO)                  [0x710064bc20]
//   line 31  void   SceneGroupMarker::GetAABB(const Marker&, AABB&)         [0x710064bc30]
//   line 59  DataID SceneGroupMarker::CreateForServerGame(...)              [0x710064bef0]
//
// IsForClient, IsSpecialServerMarker and ~SceneGroupMarker are defined inline
// in SmartSceneGroupMarker.h (lines 28, 34 and 13); they are emitted through
// the class vtable in this compile unit.

#include "SmartSceneGroupMarker.h"

#include "../Definitions/Definitions.h"
#include "../Definitions/SceneDefinition.h"
#include "../Definitions/SceneGroupDefinition.h"
#include "../GlobalSNO.h"
#include "../Hyperlink.h"
#include "../InactiveMarker.h"
#include "../SceneCommon.h"
#include "../SNO/SNOFiles.h"
#include "../SNO/SNOName.h"
#include "Marker.h"  // Marker (transform at 0x0c, snoname at 0x28); also declares gtransformMarkerToScene
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RefString.h"

// SmartSceneGroupMarker.cpp:24 (main.elf 0x710064bc20, vtable slot 3).
//   mov w0, #0x3c9 ; b GlobalSNOGet
// 0x3c9 == GLOBALSNO_MARKER_ACTOR_SCENE (GlobalSNO.h:49).
SNO SceneGroupMarker::GetActorForEdit(SNO snoSource)
{
    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_SCENE);
}

// SmartSceneGroupMarker.cpp:31 (main.elf 0x710064bc30, vtable slot 5, 700 bytes).
//
// Per-line reconstruction from the listing:
//   line 35  aabbBounds.Set(cgwpNull, 1.0f)             (Rect.inl:361, inlined)
//   line 36  PRTransform transform = gtransformMarkerToScene * tMarker.transform
//                                                       (Transform.h:67, inlined)
//   line 38  ScopedSceneGroupDefinitionLockR pdefSceneGroup(tMarker.snoname.snoHandle)
//                                                       (Definitions.h:990, inlined)
//   line 40  for (int32 i = 0; i < pdefSceneGroup->nSceneCnt; ++i)
//   line 42  SNO snoScene = pdefSceneGroup->arScenes[i].snoScene
//                                                       (XPtr64.h:68, inlined)
//   line 43  ScopedSceneDefinitionLockR pdefScene(snoScene)
//                                                       (Definitions.h:990, inlined)
//   line 47  aabbBounds = pdefScene->aabbBounds          (Rect.h:309, inlined)
//   line 51  AABB::Union(aabbBounds, pdefScene->aabbBounds, aabbBounds)
//                                                       (Rect.inl:446, inlined)
//   line 55  aabbBounds.wp += transform.wp              (Vector.inl:546, inlined)
void SceneGroupMarker::GetAABB(const Marker & tMarker, AABB & aabbBounds)
{
    // line 35.  aabbBounds.Set(cgwpNull, 1.0f) -- Rect.inl:361, i.e.
    // AABB::Set(const Vector3D wp0, const float wdSize), now defined in its
    // DWARF owning file (Sigma/Math/Rect.inl).
    aabbBounds.Set(cgwpNull, 1.0f);

    // line 36.  gtransformMarkerToScene is declared at Marker.h:53 (Marker.cpp:2763).
    PRTransform transform = gtransformMarkerToScene * tMarker.transform;

    // line 38.  DefinitionLockR<SceneGroupDefinition> -> GroupDefinition::LockRFast.
    ScopedSceneGroupDefinitionLockR pdefSceneGroup(tMarker.snoname.snoHandle);

    // line 40.
    for (int32 i = 0; i < pdefSceneGroup->nSceneCnt; ++i)
    {
        // line 42.  arScenes is an XPtr64<SceneGroupItem>; XPtr64.h:68 converts.
        SNO snoScene = pdefSceneGroup->arScenes[i].snoScene;

        // line 43.  DefinitionLockR<SceneDefinition> -> GroupDefinition::LockR(sno, FALSE).
        ScopedSceneDefinitionLockR pdefScene(snoScene);

        if (i == 0)
        {
            // line 47.
            aabbBounds = pdefScene->aabbBounds;
        }
        else
        {
            // line 51.  AABB::Union(aabbBounds, pdefScene->aabbBounds, aabbBounds) --
            // Rect.inl:446, now defined in its DWARF owning file (Sigma/Math/Rect.inl).
            AABB::Union(aabbBounds, pdefScene->aabbBounds, aabbBounds);
        }
    }

    // line 55.
    aabbBounds.wp += transform.wp;
}

// SmartSceneGroupMarker.cpp:59 (main.elf 0x710064bef0, vtable slot 13, 176 bytes).
//
// Per-line reconstruction from the listing:
//   line 65/66  SceneCommonDataTryToGet(idSScene)                       (SceneCommon.h)
//   line 68     if (ptSCD)
//   line 70     BOOL bAllNone = TRUE
//   line 71-73  for (int i = 0; i < 4; ++i) if (tSceneSpec.arSnoLevelAreas[i] != -1) ...
//                                                    (unrolled to four cmn/b.ne)
//   line 83     HyperlinkCreateFromSNO(SNOName(MARKERSET_SNO, tMarkerOwner.snoMarkerSet), NULL)
//   line 84     HyperlinkCreateFromSNO(SNOName(SCENE_SNO, ptSCD->snoScene), NULL)
//   line 88     ~CRefString / ~CRefString
//   line 92     return -1
// MARKERSET_SNO == 24 and SCENE_SNO == 33 (SNOFiles.h:130).  The -1 level-area
// sentinel is SNO_NONE (SNOFiles.h:124, value -1).
DataID SceneGroupMarker::CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner)
{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);

    if (ptSCD)
    {
        BOOL bAllNone = TRUE;

        for (int i = 0; i < 4; ++i)
        {
            if (ptSCD->tSceneSpec.arSnoLevelAreas[i] != SNO_NONE)
            {
                bAllNone = FALSE;
                break;
            }
        }

        if (bAllNone)
        {
            CRefString sMarkerSet = HyperlinkCreateFromSNO(SNOName(MARKERSET_SNO, tMarkerOwner.snoMarkerSet), NULL);
            CRefString sScene = HyperlinkCreateFromSNO(SNOName(SCENE_SNO, ptSCD->snoScene), NULL);
        }
    }

    return -1;
}
