// Original file: X/Client/Selection.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Particle.h"
#include "../Common/CommonTypes.h"
#include "../Sigma/Math/Ray.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Math/Vector.h"

// X/Client/RActors/RActorCollision.h:33-39 (that header is not adopted yet).
struct RActorIntersection {  // size 0x8
    RActorID idRActor;  // 0x00
    float flT;  // 0x04
};
static_assert(sizeof(RActorIntersection) == 0x8, "RActorIntersection");

typedef blz::vector<RActorIntersection, blz::allocator<RActorIntersection> > RActorIntersectArray;

typedef int32 MouseButton;

enum SelState {
    SELSTATE_NONE = 0,
    SELSTATE_RUBBERBAND = 1,
};

struct SelectionGlobals {  // size 0xf0
    RActorIDListPooled listSelectedRActors;  // 0x00
    RActorIDListPooled listRubberbandedRActors;  // 0x28
    RActorIntersectArray listRActorsHit;  // 0x50
    RActorIntersectArray listRActorsHitFallback;  // 0x68
    RActorID idNearestNecroCorpseRActor;  // 0x80
    blz::vector<RActorID, blz::allocator<RActorID> > listSelectedCorpses;  // 0x88
    MouseButton eMouseButtonUsedToPick;  // 0xa0
    SelState eState;  // 0xa4
    IRect2D rectRubberBand;  // 0xa8
    OBB obbSelectedRActors;  // 0xb8
    Vector3D wvSelectionOffset;  // 0xe0
    float flStereoDepth;  // 0xec
};
static_assert(sizeof(SelectionGlobals) == 0xf0, "SelectionGlobals");

// X/Client/Selection.cpp:160-1113.
void SelectionInitializeForWorld();
void SelectionDisposeForWorld();
RActorIntersectArray & SelectionGetPickList();
RActorIntersectArray & SelectionGetFallbackPickList();
RActorID SelectionGetTargetedCorpse();
blz::vector<RActorID, blz::allocator<RActorID> > & SelectionGetTargetedCorpses();
BOOL ScreenPickGround(const IVector2D & p, Vector3D & wpIntersect, uint32 dwIntersectFlags, SSceneID idSScene, SSceneID * pidSSceneResult, SNO * psnoAppearance, int32 * pnSubobject, WorldDistance * pwdIntersect);
BOOL ScreenPickGroundWithOffset(const IVector2D & p, const Vector3D & wvOffset, Vector3D & wpIntersect, uint32 dwIntersectFlags, SSceneID idSScene, SSceneID * pidSSceneResult, SNO * psnoAppearance, int32 * pnSubobject, WorldDistance * pwdIntersect);
BOOL IntersectGround(const LineSeg3D & wline, Vector3D & wpIntersect, uint32 dwIntersectFlags, SSceneID idSScene, SSceneID * pidSSceneResult, SNO * psnoAppearance, int32 * pnSubobject, WorldDistance * pwdIntersect);
