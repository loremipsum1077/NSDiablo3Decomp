// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Scene.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "AttachmentParent.h"
#include "CGame.h"
#include "Gfx/GfxCommon.h"
#include "Particle.h"
#include "Physics/DominoBody.h"
#include "RActors/RActors.h"
#include "RMessage.h"
#include "SubObjectGfx.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/SceneCommon.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/SigmaTypes.h"

struct ClutterSubgrid;

typedef int32 ParticleSystemID;

struct CSceneSubObject {  // size 0x90
    SubObjectState tSubObjectState;  // 0x00
    BOOL fVisible;  // 0x48
    BOOL fCastsShadow;  // 0x4c
    float flAlpha;  // 0x50
    ParticleSystemID idSurfaceEffect;  // 0x54
    Sphere wsBounds;  // 0x58
    AABB aabbBounds;  // 0x68
    Rect2D rect2DWorld;  // 0x80
};
static_assert(sizeof(CSceneSubObject) == 0x90, "CSceneSubObject");

enum {
    FADE_DONT_FADE = -1,
    FADE_INDIVIDUAL = 0,
    FADE_BELOW_PLAYER = 1000,
    FADE_SUBOBJECT_BIAS = 2000,
};

enum {
    CSCENEID_INVALID = -1,
};

typedef int32 CSceneID;

enum {
    CSCENE_IS_VISTA_BIT = 0,
    CSCENE_DELETE_AT_END_OF_UPDATE_BIT = 1,
    CSCENE_LAST_VISIBLE_BIT = 2,
    CSCENE_CLUTTER_TEXTURES_LOADED_BIT = 3,
    CSCENE_CONTAINS_FADE_GROUP_BIT = 4,
    CSCENE_CONTAINS_REFLECTION_BIT = 5,
    CSCENE_IS_BORDER_BIT = 6,
    CSCENE_CONSOLE_ZOOMED_OUT_CAMERA_BIT = 7,
};

struct CScene {  // size 0x7d8
    CSceneID id;  // 0x00
    SceneCommonData tSCD;  // 0x08
    uint32 dwFlags;  // 0x2c0
    int32 nRefCount;  // 0x2c4
    RenderContext eContext;  // 0x2c8
    int32 nNumSceneSubobjects;  // 0x2cc
    CSceneSubObject * pSceneSubobjects;  // 0x2d0
    RMessageList tRMsgList;  // 0x2d8
    XPooledDataIDList tAttachmentList;  // 0x310
    RigidBody * ptStaticRigidBody;  // 0x338
    int32 nSubgridCount;  // 0x340
    ClutterSubgrid * ptSubgrids;  // 0x348
    uint32 * ptSubgridFlags;  // 0x350
    SNO snoAppearance;  // 0x358
    AABB tSubgridAABB;  // 0x35c
    int32 nSubgridsX;  // 0x374
    int32 nLookIndex;  // 0x378
    SNO snoActors[256];  // 0x37c
    uint32 uActorCount;  // 0x77c
    ParentData tParentData;  // 0x780
    RActorIDListPooled listSceneRActors;  // 0x7b0
};
static_assert(sizeof(CScene) == 0x7d8, "CScene");


// Functions defined in this file (bodies are not in the DWARF):
//   line    70  CScene::CScene()  [inlined into 1 functions]
//   line    70  CScene::~CScene()  [inlined into 1 functions]
//   line   130  CScene * CSceneGet(CSceneID id)  [inlined into 17 functions]
//   line   136  CScene * CSceneTryToGet(CSceneID id)  [inlined into 16 functions]





































inline CScene * CSceneGet(CSceneID id)
{

    return CGameGet().ptCScenes->ElementGet(id);
}

inline CScene * CSceneTryToGet(CSceneID id)
{

    return CGameGet().ptCScenes->ElementTryToGet(id);
}

// Scene.cpp:3961
int32 SceneGetASubObjectIndexFromFadeGroup(const SSceneID idSScene, const int32 nFadeGroup);

CScene * CSceneGetFromSSceneID(SSceneID idSScene);  // Scene.cpp:3568

#include "../Common/SceneCommon.h"
BOOL CSceneDataArrayIterate(TDataArrayIterator<CScene> & tIterator, const CGame & tCGame, CSceneDataArrayIterateMode eIterateMode);  // Scene.cpp:3538
BOOL SceneIsVisible(CSceneID idCScene);  // Scene.cpp:3500
