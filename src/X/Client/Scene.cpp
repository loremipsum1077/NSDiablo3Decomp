// Original file: X/Client/Scene.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "Scene.h"
#include "CGame.h"
#include "SNOPreload.h"
#include "Definitions/AppearanceDefinition.h"
#include "../Common/Definitions/WorldDefinition.h"
#include "../Sigma/DataStructures/XGrowableArray.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/Math/MathGeneral.h"
#include "Camera/CameraManager.h"
#include "../Sigma/MemoryPools.h"
#include "../Common/World.h"
#include "SceneAttachments.h"
#include "Physics/DominoClient.h"
#include "../Common/Definitions/SceneDefinition.h"
#include "../Sigma/Math/Intersect.h"
// Declared here from their definitions elsewhere (headers not adopted yet).
void RActorOnSceneSwap(SWorldID idSWorld, SSceneID idSSceneOld, SSceneID idSSceneNew);  // X/Client/RActors/RActors.cpp:10888

BOOL CameraComputeAABBVisibility(const Camera & cam, const AABB & aabb);  // X/Client/Camera/Camera.cpp
void SceneGetMarkerSetAABBInWorldSpace(const SceneCommonData & tSCD, AABB & aabb);  // X/Common/SceneCommon.cpp:1097

void RActorCollisionInitialize(RActor * ptRActor);  // X/Client/RActors
void RActorCollisionDispose(RActor * ptRActor);  // X/Client/RActors
void MaterialFreeTexAnimState(TexAnimState *& ptTexAnimState);  // X/Client/Gfx/Material

void ParticleSystemDelete(const ParticleSystemID idParticleSystem);  // X/Client/Particle.cpp

void SceneCommonInitializeForGame();  // X/Common/SceneCommon.cpp
void SceneCommonDisposeForGame();  // X/Common/SceneCommon.cpp
void CScenesCollisionInitializeForGame();  // X/Client/CSceneCollision.cpp
void CScenesCollisionDisposeForGame();  // X/Client/CSceneCollision.cpp

RActorIDListPooled & RActorGetMainList();  // X/Client/RActors/RActors.cpp:11792
void SceneCommonDataDispose(SceneCommonData & tSCD);  // X/Common/SceneCommon.cpp:1129

void WorldCacheSceneValues(SNO snoScene, SceneCachedValues & tCachedValues);  // X/Common/World.cpp

void SceneCommonDataInitialize(SceneCommonData & tSCD, const SSceneID idSScene, const SceneSpecification & tSceneSpec, const SWorldID idSWorld, const SNO snoScene, const PRTransform & transform, const SSceneID idSSceneParent, const SNO snoSceneGroup, const SNO snoMarkerSetSrc, int32 nMarkerSetSrcIndex, const BOOL bIsAttachment, const BOOL bIsVista);  // X/Common/SceneCommon.cpp:489
void SceneCommonDataInitializeNavMesh(SSceneID idSScene);  // X/Common/SceneCommon.cpp:357

// Defined later in this file.
static void sCSceneSubobjectsInitialize(CScene & tCScene);
static void sCSceneInitializeClutter(const SNO snoAppearance, CScene * ptCScene);
static void sPreloadAllSubObjectTextures(CScene * ptCScene, const AppearanceDefinition * pdefAppearance);
static void sCSceneSubobjectsDispose(CScene & tCScene);
static void sCSceneDisposeClutter(CScene * ptCScene);
static void sCSceneDeleteRActors(CScene * ptCScene);
static void sUnloadAllSubObjectTextures(CScene * ptCScene, const AppearanceDefinition * pdefAppearance);
void sSceneAdd(CSceneID idCScene, const SNO snoScene, const SSceneID idSScene, const SceneSpecification & tSceneSpec, const SWorldID idSWorld, const PRTransform & transform, const RenderContext eContext, const SSceneID idSSceneParent, const SNO snoSceneGroup, BOOL bIsAttachment, BOOL bIsVista, const SNO * snoActors, uint32 uActorCount);
static BOOL sGetInterpolatedHeight(CScene * ptCScene, const Vector3D & wp, float & flHeight);
static BOOL sGetInterpolatedColor(CScene * ptCScene, const Vector3D & wp, RGBAColor & rgbaColor);
static BOOL sGetInterpolatedSurface(CScene * ptCScene, const Vector3D & wp, SNO & snoSurface);
static BOOL sGetInterpolatedNormal(CScene * ptCScene, const Vector3D & wp, Vector3D & wvFloorNormal);
CScene * CSceneGetFromSSceneID(SSceneID idSScene);
BOOL CSceneDataArrayIterate(TDataArrayIterator<CScene> & tIterator, const CGame & tCGame, CSceneDataArrayIterateMode eIterateMode);
void CSceneDelete(const CSceneID idCScene);
void sSceneCleanUp(CScene * ptCScene);
static void sCSceneFlagForDeletion(const CSceneID idCScene);
static void sCSceneInclusionsOnLeave();
static float sSceneRActorGetFade(RActor * ptRActor);
// Inference: declaration placement is local; signatures are DWARF SubObject.cpp:1217/1274/1400/1236/1304/1350.
int32 SubObjectGetNumSegments(const SubObject & tSubObject);
int32 SubObjectGetNumSegmentVertices(const SubObject & tSubObject, int32 nSegmentIndex);
void SubObjectGetSegmentVertexPos(const SubObject & tSubObject, int32 nSegmentIndex, uint16 wVertexIndex, Vector3D & wpPos);
int32 SubObjectGetNumSegmentTriangles(const SubObject & tSubObject, int32 nSegmentIndex);

enum {
    MAX_PROJECTION_SURFACES = 10240,
    MAX_FADE_GROUPS = 128,
    MAX_CLUTTER_SYSTEMS = 8,
};

void SubObjectGetSegmentIndexListWalkInfo(const SubObject & tSubObject, int32 nSegmentIndex, int32 & nIndexListIndexCount, int32 & nIndexListIndexIncrement);
void SubObjectGetSegmentTriangle(const SubObject & tSubObject, int32 nSegmentIndex, int32 nIndexListIndex, uint16 & wIndex0, uint16 & wIndex1, uint16 & wIndex2);
// Inference: local declaration from DWARF SceneCommon.cpp:1084; original declaration header unavailable.
void SceneObjectAABBToWorldAABB(const PRTransform & transformSceneToWorld, const AABB & aabbToTransform, AABB & aabbWorldSpace);


static const float ALPHA_FADE_OUT_RATE = 4.0f;
static const float ALPHA_FADE_IN_RATE = 4.0f;



static const float DISTANCE_BETWEEN_CLUTTER_SAMPLES = 5.0f;
static const int32 SAMPLES_PER_GRID = 8;
static const float SUBGRID_WIDTH = 60.0f;

struct ClutterSystem {
    ParticleSystemID idParticleSystem;
    SNO snoParticleSystem;
};
// Inference: declarations use DWARF CPlayer.cpp:5204/1612, PostFX.cpp:2219, WorldDefinition.cpp:622.
BOOL PlayerIsLocal(const Player * ptPlayer);
Player * LocalPlayerGet();
void PostFXResetReflectionPlanes();
void SceneSpecInitialize(SceneSpecification & tSceneSpec);

enum {
    CLUTTER_SUBGRID_POPULATED = 0,
    CLUTTER_SUBGRID_SAMPLED = 1,
};

struct ClutterSubgrid {
    Vector2D wpBase;
    AABB aabb;
    int32 nClutterListCount;
    ClutterSystem tClutterList[MAX_CLUTTER_SYSTEMS];
};

typedef XGrowableMap<int, const int &, int, const int &> TSSceneIDToCSceneIDMap;

struct CSceneGlobals {
    BOOL fFadeGroup[MAX_FADE_GROUPS];
    BOOL fFadeGroupRActors[MAX_FADE_GROUPS];
    int32 nNumFadeGroups;
    TSSceneIDToCSceneIDMap tSSceneIDToCSceneIDMap;
    int32 nDRLGDebugDrawCmdId;
    SSceneID idCurrentScene;
    XGrowableArray<int, int> arAddedScenes;
    XPooledDataIDList listScenesFlaggedForDeletion;
    XGrowableArray<unsigned char, unsigned char> arClipCodes;
};
static_assert(sizeof(CSceneGlobals) == 0x528, "CSceneGlobals");

struct ThreadedSubObjectInfo {
    int32 nLightCount;
    const LightState * pArrayLights[16];
};

struct ThreadedSceneInfo {
    CScene * ptScene;
    const AppearanceDefinition * pdefAppearance;
    ThreadedSubObjectInfo * pSubObjectInfo;
};

class SceneRenderUpdateJob {
public:
    static Frustum * sptFrustum;
    static const Camera * sptCam;
    static float sflDeltaTime;
    static float sflClientTime;
    SceneRenderUpdateJob(ThreadedSceneInfo * ptSceneInfo, int32 nStartingIndex, int32 nCount);
    void Execute();
    static void JobFcn(void * pParam);
private:
    ThreadedSceneInfo * m_ptSceneInfo;
    int32 m_nStartingIndex;
    int32 m_nCount;
};
#include "RWindow.h"
#include "../Common/GlobalSNO.h"
#include "Definitions/SurfaceDefinition.h"
#include "Definitions/ShaderDefinition.h"
#include "../Common/JobManager/JobManager.h"
#include <new>
#include "PostFX.h"
#include "../Common/TagMaps/AppearanceTags.h"
#include "Definitions/PreferencesDefinition.h"
#include "../Common/ActorCommonData.h"
#include "../Sigma/Random.h"
// Inference: local declarations use DWARF CPlayer.cpp:1686, Player.cpp:2181, ParticleRenderUpdate.cpp:33.
ACDID LocalPlayerACDGet();
int32 PlayerGetNumPlayers();
float ParticleGetClutterDistance();
#include "Camera/Camera.h"
// Same-file forward to the genuine definition at3480.
static BOOL sSceneIsVisible(const Camera & cam, CScene * ptCScene);
#include "../Common/TagMaps/Tags.h"
// Inference: local declaration from DWARF TexAnimation.cpp:1124; owning header unknown.
TexAnimState * TexAnimRenderAsyncCopy(int32 nCount, const TexAnimState * ptTexAnimState, const UberMaterial * ptUberMaterial, ShaderTex eOverlayShaderTex);
// Inference: local declarations from DWARF CPlayer.cpp:1668, RActors.cpp:6513, SubObjectGfx.cpp:3241, Lights.cpp:629.
RActorID LocalPlayerRActorGet();
void RActorGetCenterPoint(const RActor * ptRActor, Vector3D & wpCenter);
struct ObjectTexAnimParam;
void SubObjectStateUpdate(const SubObjectAppearance & tSOA, SubObjectState & tSubObjectState, const ObjectTexAnimParam * ptObjTexAnimParam, const float flDeltaTime, const float flClientTime);
int32 LightsGetDynamic(const RenderContext eContext, const Sphere & ws, const uint32 dwFilterFlags, const int32 nMaxLightCount, const LightState ** pArrayLights, const AABB * ptAABB, SNO * snoArrayLights);
#include "Definitions/ShaderMapDefinition.h"
// Inference: local declarations from DWARF CAttachmentDefinition.cpp:72, AppearanceDefinition.cpp:4244, SubObjectGfx.cpp:3220.
int32 LookIndexFromHash(const AppearanceDefinition * pdefAppearance, uint32 nHash);
SubObjectAppearance & AppearanceGetSubObjectAppearance(const AppearanceDefinition * ptDefinition, int32 nSubObjectLook, const int32 nSubObject, const GeoSetType eGeoSetType);
void SubObjectStateInitFade(SubObjectState & tSubObjectState);
// Inference: source-local declaration of DWARF SubObjectGfx.h:55 enumerator, avoiding a shared-header edit.
enum { SUBOBJECTSTATE_IS_REFLECTIVE_BIT = 2 };



static void sCSceneCreatePhysics(CScene * ptCScene)
{




    GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
    if (eGameMode == GAMEMODE_PLAYING || eGameMode == GAMEMODE_HOLODECK || eGameMode == GAMEMODE_MENU)
    {







        DominoWorld::s_currentScene = ptCScene->tSCD.snoScene;
        ScopedSceneDefinitionLockR pdefScene(ptCScene->tSCD.snoScene);
        SNO snoPhysMesh = pdefScene->snoPhysMesh;
        if (snoPhysMesh != SNO_NONE)
        {




            CRefString name = SNOToString(SCENE_SNO, ptCScene->tSCD.snoScene, FALSE);
            const PRTransform & transform = ptCScene->tSCD.transformWorld;

            ptCScene->ptStaticRigidBody = gptPhysicsAPIImpl->CreateBodyFromPhysMesh(snoPhysMesh, transform, 1.0f, name.str());
            DominoWorld::s_currentScene = SNO_NONE;
        }
    }
}
static void sCSceneDeletePhysics(CScene * ptCScene)
{

    if (ptCScene->ptStaticRigidBody)
    {

        gptPhysicsAPIImpl->DestroyBody(ptCScene->ptStaticRigidBody);
        ptCScene->ptStaticRigidBody = NULL;
    }
}


void CScenesInitializeForGame()
{

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    CGame & tCGame = CGameGet();
    tCGame.ptCScenes = TDataArrayCreateTracked<CScene, CSceneID>("Scenes", ptMemoryInterface, 5120);

    CSceneGlobals *& ptCSceneGlobals = tCGame.ptCSceneGlobals;
    ptCSceneGlobals = (CSceneGlobals *)SigmaMemoryNew(sizeof(CSceneGlobals), 4, ptMemoryInterface, TRUE);

    ptCSceneGlobals->tSSceneIDToCSceneIDMap.Initialize(16, FALSE, ptMemoryInterface);
    ptCSceneGlobals->tSSceneIDToCSceneIDMap.SetHashTableSize_Tracked(64);

    ptCSceneGlobals->arAddedScenes.Initialize();
    ptCSceneGlobals->arAddedScenes.SetMemoryInterface(ptMemoryInterface);

    ptCSceneGlobals->listScenesFlaggedForDeletion.Initialize(MemoryPoolsGetDATAIDListPool());

    ptCSceneGlobals->arClipCodes.Initialize();
    ptCSceneGlobals->arClipCodes.SetMemoryInterface(ptMemoryInterface);

    SceneCommonInitializeForGame();
    CScenesCollisionInitializeForGame();
}





void CScenesDisposeForGame()
{

    CGame & tCGame = CGameGet();


    CScenesCollisionDisposeForGame();

    TDataArrayIterator<CScene> tIt;
    while (CSceneDataArrayIterate(tIt, tCGame, IncludeHolodeckScenes))
    {
        CSceneDelete(tIt.ptElement->id);
    }

    TDataArrayDispose(tCGame.ptCScenes);

    SceneCommonDisposeForGame();
    CSceneGlobals *& ptCSceneGlobals = tCGame.ptCSceneGlobals;
    ptCSceneGlobals->listScenesFlaggedForDeletion.Dispose();
    ptCSceneGlobals->arAddedScenes.Dispose();
    ptCSceneGlobals->arClipCodes.Dispose();

    ptCSceneGlobals->tSSceneIDToCSceneIDMap.Dispose();

    XFREE(ptCSceneGlobals, AppGetCurrentMemoryInterface());
}




void CScenesInitializeForWorld(SWorldID idSWorld)
{

    sCSceneInclusionsOnLeave();

    CGame & tCGame = CGameGet();
    CSceneGlobals * ptCSceneGlobals = tCGame.ptCSceneGlobals;
    ptCSceneGlobals->nNumFadeGroups = 0;
    ptCSceneGlobals->nDRLGDebugDrawCmdId = -1;
    ptCSceneGlobals->idCurrentScene = -1;
    ptCSceneGlobals->arAddedScenes.EmptyArray();
}






void CScenesDisposeForWorld(SWorldID idSWorld)
{

    CGame & tCGame = CGameGet();

    TDataArrayIterator<CScene> tIt;
    while (CSceneDataArrayIterate(tIt, tCGame, IncludeHolodeckScenes))
    {

        CScene * ptCScene = tIt.ptElement;
        if (ptCScene->tSCD.idSWorld == idSWorld)
        {
            CSceneDelete(ptCScene->id);
        }
    }
}


// Linker-dropped in the original (concrete DIE at low_pc 0, Scene.cpp:333); second real caller of the clutter helpers.
// Inference: Scene.cpp:338 holds an early-out test whose condition is not recoverable (code dropped); omitted.
void CScenesRepopulateClutter(
    void)
{
    CGame & tCGame = CGameGet();





    // Inference: iterate mode leaves no code (CSceneDataArrayIterate ignores it); IncludeHolodeckScenes as in siblings. '{' on 335 per line table; 334 inferred as the parameter line.
    TDataArrayIterator<CScene> tIt;
    while (CSceneDataArrayIterate(tIt, tCGame, IncludeHolodeckScenes))
    {
        CScene * ptCScene = tIt.ptElement;

        sCSceneDisposeClutter(ptCScene);



        // Inference: snoAppearance (DWARF local, no location) taken from CScene::snoAppearance.
        SNO snoAppearance = ptCScene->snoAppearance;

        sCSceneInitializeClutter(snoAppearance, ptCScene);
    }
}

























void sSceneAdd(CSceneID idCScene, const SNO snoScene, const SSceneID idSScene, const SceneSpecification & tSceneSpec, const SWorldID idSWorld, const PRTransform & transform, const RenderContext eContext, const SSceneID idSSceneParent, const SNO snoSceneGroup, BOOL bIsAttachment, BOOL bIsVista, const SNO * snoActors, uint32 uActorCount)
{













    SNOPreload::AddClientSceneBySNO(snoScene);

    SNOPreload::AddClientWeatherBySNO(tSceneSpec.snoWeather);

    CGame & tCGame = CGameGet();
    GameCommonData & tGCD = CGameGetGameCommonData();

    SSceneID idSSceneToUse = (idSScene == SSCENEID_INVALID) ? idCScene : idSScene;





    CSceneGlobals * ptCSceneGlobals = tCGame.ptCSceneGlobals;
    ptCSceneGlobals->tSSceneIDToCSceneIDMap.SetAt_Tracked(idSSceneToUse, idCScene);

    CScene * ptCScene = CSceneGet(idCScene);


    for (uint32 i = 0; i < uActorCount; i++)
    {
        SNO snoActor = snoActors[i];
        ptCScene->snoActors[i] = snoActor;
        SNOPreload::AddClientActorBySNO(snoActor);
    }

    ptCScene->uActorCount = uActorCount;

    SceneCommonDataInitialize(ptCScene->tSCD, idSSceneToUse, tSceneSpec, idSWorld, snoScene, transform, idSSceneParent, snoSceneGroup, SNO_NONE, -1, bIsAttachment, bIsVista);

    if (bIsVista) ptCScene->dwFlags |= (1 << CSCENE_IS_VISTA_BIT); else ptCScene->dwFlags &= ~(1 << CSCENE_IS_VISTA_BIT);
    ptCScene->dwFlags &= ~(1 << CSCENE_LAST_VISIBLE_BIT);




    ptCScene->eContext = eContext;

    ScopedSceneDefinitionLockR pdefScene(snoScene);














    if (pdefScene->dwFlags & (1 << SCENE_CONSOLE_ZOOMED_OUT_CAMERA_BIT)) ptCScene->dwFlags |= (1 << CSCENE_CONSOLE_ZOOMED_OUT_CAMERA_BIT); else ptCScene->dwFlags &= ~(1 << CSCENE_CONSOLE_ZOOMED_OUT_CAMERA_BIT);

    CSceneAttachmentsInitialize(ptCScene);

    sCSceneCreatePhysics(ptCScene);

    ptCScene->tParentData.Init();

    if (!bIsVista)
    {
        SceneCommonDataInitializeNavMesh(idSSceneToUse);
    }

    sCSceneSubobjectsInitialize(*ptCScene);

    if (tGCD.GameModeGet() == GAMEMODE_PLAYING)
    {
        sCSceneInitializeClutter(pdefScene->GetAppearance(), ptCScene);
    }

    CSceneAttachmentsOnMsg(ptCScene, CSCENE_MSGKEY_CREATE);

    ptCScene->listSceneRActors.SetPool(MemoryPoolsGetDATAIDListPool());












}




CSceneID CSceneAdd(const SNO snoScene, const SSceneID idSScene, const SceneSpecification & tSceneSpec, const SWorldID idSWorld, const PRTransform & transform, const SSceneID idSSceneParent, const SNO snoSceneGroup, BOOL bIsAttachment, BOOL bIsVista, const RenderContext eContext, const SNO * snoActors, uint32 uActorCount)
{












    CGame & tCGame = CGameGet();
    CSceneID idCScene = tCGame.ptCScenes->ElementNew();
    if (idCScene != CSCENEID_INVALID)
    {



        sSceneAdd(idCScene, snoScene, idSScene, tSceneSpec, idSWorld, transform, eContext, idSSceneParent, snoSceneGroup, bIsAttachment, bIsVista, snoActors, uActorCount);
    }













    return idCScene;
}


void sSceneCleanUp(CScene * ptCScene)
{
    if (!(ptCScene->dwFlags & (1 << CSCENE_DELETE_AT_END_OF_UPDATE_BIT)))
    {

        SNOPreload::RemoveClientSceneBySNO(ptCScene->tSCD.snoScene);

        SNOPreload::RemoveClientWeatherBySNO(ptCScene->tSCD.tSceneSpec.snoWeather);

        for (uint32 i = 0; i < ptCScene->uActorCount; i++)
        {
            SNO snoActor = ptCScene->snoActors[i];
            SNOPreload::RemoveClientActorBySNO(snoActor);
        }
    }





    CSceneAttachmentsOnMsg(ptCScene, CSCENE_MSGKEY_DELETE);

    SSceneID idSScene = ptCScene->tSCD.idSScene;

    CSceneAttachmentsDispose(ptCScene);

    sCSceneSubobjectsDispose(*ptCScene);

    sCSceneDeletePhysics(ptCScene);




    SceneCommonDataDispose(ptCScene->tSCD);

    sCSceneDisposeClutter(ptCScene);

    sCSceneDeleteRActors(ptCScene);

    ptCScene->listSceneRActors.RemoveAll();

    CGame & tCGame = CGameGet();
    CSceneGlobals * ptCSceneGlobals = tCGame.ptCSceneGlobals;

    if (idSScene != SSCENEID_INVALID)
    {
        ptCSceneGlobals->tSSceneIDToCSceneIDMap.RemoveAt(idSScene);
    }
}



void CSceneDelete(const CSceneID idCScene)
{

    CScene * ptCScene = CSceneTryToGet(idCScene);
    if (ptCScene)
    {




        sSceneCleanUp(ptCScene);

        CGame & tCGame = CGameGet();
        tCGame.ptCScenes->ElementDeleteByID(idCScene);
    }
}

// Linker-dropped in the original (concrete DIE at low_pc 0, Scene.cpp:602); written from its DWARF and line table.
// Inference: the 606 test (col 14 ptCScenes, col 6 '!') and 615 context compare follow the line table; iterate mode leaves no code, IncludeHolodeckScenes as in siblings.
void CSceneDeleteAll(
    RenderContext eContext)
{
    CGame & tCGame = CGameGet();
    if (!tCGame.ptCScenes)
    {
        return;
    }

    TDataArrayIterator<CScene> tIt;
    while (CSceneDataArrayIterate(tIt, tCGame, IncludeHolodeckScenes))
    {
        CScene * ptCScene = tIt.ptElement;
        if (ptCScene->eContext == eContext)
        {
            CSceneDelete(ptCScene->id);
        }
    }
}



















































void CSceneSetParent(const CSceneID idCScene, const ParentData & tParentData)
{


    CScene * ptCScene = CSceneTryToGet(idCScene);
    if (!ptCScene)
    {
        return;
    }

    ptCScene->tParentData = tParentData;
}




















static void sPreloadAllSubObjectTextures(CScene * ptCScene, const AppearanceDefinition * pdefAppearance)
{


    const Structure & tStructure = pdefAppearance->tStructure;
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];

    SNONameArray tTextures;
    for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; nSubObjectIndex++)
    {

        const SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
        CSceneSubObject & tCSceneSubObject = ptCScene->pSceneSubobjects[nSubObjectIndex];
        SubObjectState & tSubObjectState = tCSceneSubObject.tSubObjectState;

        const SubObjectAppearance & tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLook];
        int32 nCount = tSOA.tCompositeUberMaterial.serMatTexList.dwSize / sizeof(MaterialTextureEntry);
        MaterialTextureEntry * ptMatTexList = tSOA.tCompositeUberMaterial.ptMatTexList;
        for (int32 i = 0; i < nCount; i++)
        {

            SNO snoTex = ptMatTexList[i].tMatTex.snoTex;
            if (snoTex != SNO_NONE)
            {
                tTextures.push_back(SNOName(TEXTURE_SNO, snoTex));
            }
        }
    }




    if (tTextures.size())
    {

        SNOPreload::AddClientTextures(tTextures);
    }
}


static void sUnloadAllSubObjectTextures(CScene * ptCScene, const AppearanceDefinition * pdefAppearance)
{


    const Structure & tStructure = pdefAppearance->tStructure;
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];

    SNONameArray tTextures;
    for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; nSubObjectIndex++)
    {

        const SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
        CSceneSubObject & tCSceneSubObject = ptCScene->pSceneSubobjects[nSubObjectIndex];
        SubObjectState & tSubObjectState = tCSceneSubObject.tSubObjectState;

        const SubObjectAppearance & tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLook];
        int32 nCount = tSOA.tCompositeUberMaterial.serMatTexList.dwSize / sizeof(MaterialTextureEntry);
        MaterialTextureEntry * ptMatTexList = tSOA.tCompositeUberMaterial.ptMatTexList;
        for (int32 i = 0; i < nCount; i++)
        {

            SNO snoTex = ptMatTexList[i].tMatTex.snoTex;
            if (snoTex != SNO_NONE)
            {
                tTextures.push_back(SNOName(TEXTURE_SNO, snoTex));
            }
        }
    }




    if (tTextures.size())
    {

        SNOPreload::RemoveClientTextures(tTextures);
    }
}


static void sCSceneFindBestReflectionPlane(
    CScene & tCScene,
    const SubObject & tSubObject,
    Plane & wpPlane)
{
    float fBestNormalZ = 0.0f;
    // Inference: retain only upward-facing triangles, following original zero baseline and strict greater test.
    for (int nSegment = 0; nSegment < SubObjectGetNumSegments(tSubObject); ++nSegment)
    {
        for (int nTri = 0; nTri < SubObjectGetNumSegmentTriangles(tSubObject, nSegment); ++nTri)
        {
            uint16 wIndex0, wIndex1, wIndex2;
            SubObjectGetSegmentTriangle(tSubObject, nSegment, nTri, wIndex0, wIndex1, wIndex2);

            Vector3D wpPos0, wpPos1, wpPos2;
            SubObjectGetSegmentVertexPos(tSubObject, nSegment, wIndex0, wpPos0);
            SubObjectGetSegmentVertexPos(tSubObject, nSegment, wIndex1, wpPos1);
            SubObjectGetSegmentVertexPos(tSubObject, nSegment, wIndex2, wpPos2);

            Vector3D wp0 = tCScene.tSCD.transformWorld * wpPos0;
            Vector3D wp1 = tCScene.tSCD.transformWorld * wpPos1;
            Vector3D wp2 = tCScene.tSCD.transformWorld * wpPos2;

            Plane wpTriPlane;
            wpTriPlane.Set(wp0, wp1, wp2);
            if (wpTriPlane.wvNormal.z > fBestNormalZ)
            {
                fBestNormalZ = wpTriPlane.wvNormal.z;
                wpPlane = wpTriPlane;
            }
        }
    }
}





static void sCSceneSubobjectsInitialize(
    CScene & tCScene)
{
    ScopedSceneDefinitionLockR pdefScene(tCScene.tSCD.snoScene);
    ScopedAppearanceDefinitionLockR pdefAppearance(pdefScene->GetAppearance());
    ScopedAppearanceBufferContentRetrieval tAppearanceBufferContentRetrieval(pdefAppearance);

    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];

    float flLookSwitchTime = CGameGetClientTime();
    int32 nSubObjectLook;


    nSubObjectLook = LookIndexFromHash(pdefAppearance, pdefScene->tLookLink.dwLookHash);




    if (nSubObjectLook == -1)
        nSubObjectLook = 0;


    // Inference: original allocation requests cleared storage with alignment 4.
    tCScene.nNumSceneSubobjects = tGeoSet.nSubObjectCount;
    tCScene.pSceneSubobjects = (CSceneSubObject *)SigmaMemoryNew(tGeoSet.nSubObjectCount * sizeof(CSceneSubObject), 4, NULL, TRUE);
    BOOL fContainsFadeGroup = FALSE;
    BOOL fContainsReflection = FALSE;
    for (int32 nSubObject = 0; nSubObject < tGeoSet.nSubObjectCount; ++nSubObject)
    {
        const SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObject];
        CSceneSubObject & tCSceneSubObject = tCScene.pSceneSubobjects[nSubObject];
        SubObjectState & tSubObjectState = tCSceneSubObject.tSubObjectState;

        tCSceneSubObject.flAlpha = 1.0f;
        tCSceneSubObject.idSurfaceEffect = -1;

        SubObjectAppearance & tSOA = AppearanceGetSubObjectAppearance(pdefAppearance, nSubObjectLook, nSubObject, eGeoSetType);

        tSubObjectState.arTexAnimStates = MaterialAllocTexAnimState(tSOA.tCompositeUberMaterial, tSubObjectState.nTexAnimStateCount);

        TexAnimInitialize(tSOA.tCompositeUberMaterial, tSubObjectState.arTexAnimStates, NULL);


        tSubObjectState.nSubObjectLook = nSubObjectLook;
        tSubObjectState.flLookSwitchTime = flLookSwitchTime;
        BOOL fCanBeRendered = (tSOA.dwFlags & (1 << SUBOBJECTAPPEARANCE_RENDER_BIT)) != 0;
        if (fCanBeRendered) tSubObjectState.dwFlags |= (1 << SUBOBJECTSTATE_CAN_BE_RENDERED_BIT); else tSubObjectState.dwFlags &= ~(1 << SUBOBJECTSTATE_CAN_BE_RENDERED_BIT);

        SubObjectStateInitFade(tSubObjectState);


        if (tSOA.tCompositeUberMaterial.snoShaderMap != SNO_NONE)
        {
            ScopedShaderMapDefinitionLockR pdefShaderMap(tSOA.tCompositeUberMaterial.snoShaderMap);
            tSubObjectState.snoDefaultShader = ShaderMapGetDefaultShader(tSOA.tCompositeUberMaterial.snoShaderMap, TRUE, pdefShaderMap);
            tSubObjectState.snoFadeShader = ShaderMapGetFadeShader(tSOA.tCompositeUberMaterial.snoShaderMap, pdefShaderMap);
        }


        tSubObjectState.idCloth = -1;
        // Inference: valid shared fade groups are 1..127, matching original unsigned (group-1)<127.
        if (tSubObject.nFadeGroup > 0 && tSubObject.nFadeGroup < MAX_FADE_GROUPS)
            fContainsFadeGroup = TRUE;




        if (tSOA.tCompositeUberMaterial.snoShaderMap != SNO_NONE &&
            ShaderMapIsReflective(tSOA.tCompositeUberMaterial.snoShaderMap))
        {

            sCSceneFindBestReflectionPlane(tCScene, tSubObject, tSubObjectState.wpReflectionPlane);
            fContainsReflection = TRUE; tSubObjectState.dwFlags |= (1 << SUBOBJECTSTATE_IS_REFLECTIVE_BIT);
        }
        else
        {
            tSubObjectState.dwFlags &= ~(1 << SUBOBJECTSTATE_IS_REFLECTIVE_BIT);
        }
    }

    if (fContainsFadeGroup) tCScene.dwFlags |= (1 << CSCENE_CONTAINS_FADE_GROUP_BIT); else tCScene.dwFlags &= ~(1 << CSCENE_CONTAINS_FADE_GROUP_BIT);
    if (fContainsReflection) tCScene.dwFlags |= (1 << CSCENE_CONTAINS_REFLECTION_BIT); else tCScene.dwFlags &= ~(1 << CSCENE_CONTAINS_REFLECTION_BIT);
    if (pdefScene->dwFlags & (1 << SCENE_IS_DRLG_BORDER_TILE)) tCScene.dwFlags |= (1 << CSCENE_IS_BORDER_BIT); else tCScene.dwFlags &= ~(1 << CSCENE_IS_BORDER_BIT);


    tCScene.nSubgridsX = pdefAppearance->nSubgridsX;
    tCScene.nSubgridCount = pdefAppearance->nSubgridsX * pdefAppearance->nSubgridsY;
    tCScene.tSubgridAABB = pdefAppearance->tSubgridsAABB;
    tCScene.tSubgridAABB.wp += tCScene.tSCD.transformWorld.wp;
    tCScene.nLookIndex = nSubObjectLook;


    tCScene.snoAppearance = pdefScene->snoAppearance;

    sPreloadAllSubObjectTextures(&tCScene, pdefAppearance);
}




static void sCSceneSubobjectsDispose(CScene & tCScene)
{

    if (!(tCScene.dwFlags & (1 << CSCENE_DELETE_AT_END_OF_UPDATE_BIT)))
    {
        ScopedAppearanceDefinitionLockR pdefAppearance(tCScene.snoAppearance);
        sUnloadAllSubObjectTextures(&tCScene, pdefAppearance);
    }

    for (int32 i = 0; i < tCScene.nNumSceneSubobjects; i++)
    {
        SubObjectState & tSubObjectState = tCScene.pSceneSubobjects[i].tSubObjectState;

        MaterialFreeTexAnimState(tSubObjectState.arTexAnimStates);
    }

    XFREE(tCScene.pSceneSubobjects, NULL);
}



static void sCSceneResetForAppearanceChange(
    CScene & tCScene)
{
    sCSceneSubobjectsDispose(tCScene);
    sCSceneSubobjectsInitialize(tCScene);
}
























// Inference: the Switch path retains the quality query but density is always 0.25 before player scaling.
static float sGetClutterDensity()
{

    float flDensity = 0.25f;
    Prefs::GetVideoPrefs().GetClutterQuality();
















    // Inference: the original subtracts player count from four, divides by three, then scales density.
    flDensity *= (4 - PlayerGetNumPlayers()) / 3.0f;
    return flDensity;
}

























































































































































static ParticleSystemID sGetClutterParticleSystem(
    ClutterSubgrid & tSubgrid,
    const SNO snoParticleSystem)
{
    for (int32 i = 0; i < tSubgrid.nClutterListCount; i++)
    {
        ClutterSystem & tSystem = tSubgrid.tClutterList[i];
        if (tSystem.snoParticleSystem == snoParticleSystem)
            return tSystem.idParticleSystem;
    }

    // Inference: the original bound is eight systems and the failure sentinel is -1.
    if (tSubgrid.nClutterListCount >= MAX_CLUTTER_SYSTEMS)
        return PARTICLESYSTEMID_INVALID;




    PRTransform transform = cgtPRTransformIdentity;
    transform.wp = tSubgrid.aabb.wp;

    ParticleSystemAddParams tParams(snoParticleSystem, transform);
    ClutterSystem tSystem;
    tSystem.idParticleSystem = ParticleSystemAdd(tParams);
    tSystem.snoParticleSystem = snoParticleSystem;
    if (tSystem.idParticleSystem != PARTICLESYSTEMID_INVALID)
        tSubgrid.tClutterList[tSubgrid.nClutterListCount++] = tSystem;

    return tSystem.idParticleSystem;
}







static void sCSceneInitializeSubgrid(ClutterSubgrid & tSubgrid, AABB & aabbSubgrid)
{








    tSubgrid.wpBase.Set(
        aabbSubgrid.wp.x - aabbSubgrid.wvExt.x, aabbSubgrid.wp.y - aabbSubgrid.wvExt.y);


    tSubgrid.aabb = aabbSubgrid;
}






































































































































static void sCreateParticlesForSubgrid(
    ClutterSubgrid & tSubgrid,
    uint32 & dwFlags,
    int32 nSubgridIndex,
    CScene * ptCScene)
{



    dwFlags |= (1 << CLUTTER_SUBGRID_POPULATED);


    float flClutterDensity = sGetClutterDensity();
    ScopedAppearanceDefinitionLockR pdefAppearance(ptCScene->snoAppearance);
    int32 nSamplesY = pdefAppearance->ptSubgrids[nSubgridIndex].nSamplesY;
    int32 nSamplesX = pdefAppearance->ptSubgrids[nSubgridIndex].nSamplesX;
    AppearanceSample * ptSamples = pdefAppearance->ptSubgrids[nSubgridIndex].ptSamples;
    if (!ptSamples) return; else ptSamples += ptCScene->nLookIndex * nSamplesX * nSamplesY;
    float flZToAdd = ptCScene->tSCD.transformWorld.wp.z;














    // Inference: all samples participate; density is packed in four bits and scaled by 1/16.
    for (int32 y = 0; y < nSamplesY; y++)
    {
        for (int32 x = 0; x < nSamplesX; x++)
        {
            AppearanceSample & tSample = ptSamples[y * nSamplesX + x];

            if (flClutterDensity * (tSample.tClutterDensityMaskAndColor.dwClutterDensity * (1.0f / 16.0f)) < 0.000001f)
                continue;


            // Inference: the original excludes the signed 24-bit SNO_NONE particle sentinel.
            if (tSample.tClutterZFractionAndParticle.snoParticle == SNO_NONE)
                continue;



            int32 nXRight = Math::Min(x + 1, nSamplesX - 1);
            int32 nYDown = Math::Min(y + 1, nSamplesY - 1);
            AppearanceSample & tSample01 = ptSamples[y * nSamplesX + nXRight];
            AppearanceSample & tSample10 = ptSamples[nYDown * nSamplesX + x];
            AppearanceSample & tSample11 = ptSamples[nYDown * nSamplesX + nXRight];

            // Inference: each of the four density samples must be nonzero before interpolating.
            if (!tSample.tClutterDensityMaskAndColor.dwClutterDensity ||
                !tSample01.tClutterDensityMaskAndColor.dwClutterDensity ||
                !tSample10.tClutterDensityMaskAndColor.dwClutterDensity ||
                !tSample11.tClutterDensityMaskAndColor.dwClutterDensity)
                continue;













            // Inference: eight random attempts per sample follow the original loop bound; packed color is 7/7/7/3.
            for (int32 i = 0; i < SAMPLES_PER_GRID; i++)
            {
                float flFracX = RandFloat_Internal(CGameGlobalsGet().rngLocal);
                float flFracY = RandFloat_Internal(CGameGlobalsGet().rngLocal);
                float flDensity = flClutterDensity * Math::Lerp(Math::Lerp((tSample.tClutterDensityMaskAndColor.dwClutterDensity * (1.0f / 16.0f)), (tSample01.tClutterDensityMaskAndColor.dwClutterDensity * (1.0f / 16.0f)), flFracX), Math::Lerp((tSample10.tClutterDensityMaskAndColor.dwClutterDensity * (1.0f / 16.0f)), (tSample11.tClutterDensityMaskAndColor.dwClutterDensity * (1.0f / 16.0f)), flFracX), flFracY);
                RGBAColor rgbaColor = Math::Lerp(Math::Lerp(RGBAColor(((tSample.tClutterDensityMaskAndColor.dwColor >> 14) & 127) << 1, ((tSample.tClutterDensityMaskAndColor.dwColor >> 7) & 127) << 1, (tSample.tClutterDensityMaskAndColor.dwColor & 127) << 1, (tSample.tClutterDensityMaskAndColor.dwColor >> 21) << 5), RGBAColor(((tSample01.tClutterDensityMaskAndColor.dwColor >> 14) & 127) << 1, ((tSample01.tClutterDensityMaskAndColor.dwColor >> 7) & 127) << 1, (tSample01.tClutterDensityMaskAndColor.dwColor & 127) << 1, (tSample01.tClutterDensityMaskAndColor.dwColor >> 21) << 5), flFracX), Math::Lerp(RGBAColor(((tSample10.tClutterDensityMaskAndColor.dwColor >> 14) & 127) << 1, ((tSample10.tClutterDensityMaskAndColor.dwColor >> 7) & 127) << 1, (tSample10.tClutterDensityMaskAndColor.dwColor & 127) << 1, (tSample10.tClutterDensityMaskAndColor.dwColor >> 21) << 5), RGBAColor(((tSample11.tClutterDensityMaskAndColor.dwColor >> 14) & 127) << 1, ((tSample11.tClutterDensityMaskAndColor.dwColor >> 7) & 127) << 1, (tSample11.tClutterDensityMaskAndColor.dwColor & 127) << 1, (tSample11.tClutterDensityMaskAndColor.dwColor >> 21) << 5), flFracX), flFracY);

                if (RandFloat_Internal(CGameGlobalsGet().rngLocal) < flDensity)
                {
                    ParticleSystemID idParticleSystem = sGetClutterParticleSystem(tSubgrid, tSample.tClutterZFractionAndParticle.snoParticle);
                    if (idParticleSystem != PARTICLESYSTEMID_INVALID)
                    {
                        ParticleSystem * ptParticleSystem = ParticleSystemGet(idParticleSystem);
                        ScopedParticleSystemLock tLock(ptParticleSystem);
                        float flZ = Math::Lerp(Math::Lerp((pdefAppearance->flMinZ + (pdefAppearance->flMaxZ - pdefAppearance->flMinZ) * (tSample.tClutterZFractionAndParticle.dwZFraction / 255.0f)), (pdefAppearance->flMinZ + (pdefAppearance->flMaxZ - pdefAppearance->flMinZ) * (tSample01.tClutterZFractionAndParticle.dwZFraction / 255.0f)), flFracX), Math::Lerp((pdefAppearance->flMinZ + (pdefAppearance->flMaxZ - pdefAppearance->flMinZ) * (tSample10.tClutterZFractionAndParticle.dwZFraction / 255.0f)), (pdefAppearance->flMinZ + (pdefAppearance->flMaxZ - pdefAppearance->flMinZ) * (tSample11.tClutterZFractionAndParticle.dwZFraction / 255.0f)), flFracX), flFracY);

                        Vector3D wpPos;
                        wpPos.x = tSubgrid.wpBase.x + (x + flFracX) * DISTANCE_BETWEEN_CLUTTER_SAMPLES;
                        wpPos.y = tSubgrid.wpBase.y + (y + flFracY) * DISTANCE_BETWEEN_CLUTTER_SAMPLES;
                        wpPos.z = flZToAdd + flZ;

                        // Inference: positions are relative to subgrid center; red/alpha encode the original particle color pair.
                        wpPos -= tSubgrid.aabb.wp;

                        ParticleCreateParams tParams;
                        tParams.rgbaColor.Set(rgbaColor.r, rgbaColor.a, 255, 255);
                        tParams.flDensity = flDensity;
                        tParams.fIgnoreGroundHug = TRUE;

                        ptParticleSystem->CreateParticle(wpPos, &tParams);
                    }
                }
            }
        }
    }


    for (int32 i = 0; i < tSubgrid.nClutterListCount; i++)
    {
        ClutterSystem & tSystem = tSubgrid.tClutterList[i];
        ParticleSystem * ptParticleSystem = ParticleSystemGet(tSystem.idParticleSystem);

        ptParticleSystem->SortParticles();
    }
}




static void sDeleteParticlesFromSubgrid(ClutterSubgrid & tSubgrid, uint32 & dwFlags)
{


    dwFlags &= ~(1 << CLUTTER_SUBGRID_POPULATED);
    for (int32 i = 0; i < tSubgrid.nClutterListCount; i++)
    { // Inference: original 1475 indexed load identifies abstract-DIE tSystem1474 as this list element.
        ClutterSystem & tSystem = tSubgrid.tClutterList[i];
        ParticleSystemDelete(tSystem.idParticleSystem);
    }
    tSubgrid.nClutterListCount = 0;
}




static void sCSceneInitializeClutter(const SNO snoAppearance, CScene * ptCScene)
{



















    if (ptCScene->nSubgridCount == 0)
    {
        return;
    }



    if (ptCScene->ptSubgrids)
    {
        return;
    }



    AABB aabb(ptCScene->tSubgridAABB);
    int32 nSubgridsX = ptCScene->nSubgridsX;
    int32 nSubgridsY = ptCScene->nSubgridCount / nSubgridsX;





































    ptCScene->ptSubgrids = (ClutterSubgrid *)SigmaMemoryNew(ptCScene->nSubgridCount * sizeof(ClutterSubgrid), 4, NULL, TRUE);
    ptCScene->ptSubgridFlags = (uint32 *)SigmaMemoryNew(ptCScene->nSubgridCount * sizeof(uint32), 4, NULL, TRUE);

    Vector3D wpAABBMin = aabb.wp - aabb.wvExt;
    Vector3D wpAABBMax = aabb.wp + aabb.wvExt;

    for (int32 y = 0; y < nSubgridsY; y++)
    {
        for (int32 x = 0; x < nSubgridsX; x++)
        {
            int32 nIndex = y * nSubgridsX + x;

            Vector3D wpMin;
            wpMin.x = wpAABBMin.x + x * SUBGRID_WIDTH;
            wpMin.y = wpAABBMin.y + y * SUBGRID_WIDTH;
            wpMin.z = wpAABBMin.z;

            Vector3D wpMax;
            wpMax.x = Math::Min(wpAABBMin.x + (x + 1) * SUBGRID_WIDTH, wpAABBMax.x);
            wpMax.y = Math::Min(wpAABBMin.y + (y + 1) * SUBGRID_WIDTH, wpAABBMax.y);
            wpMax.z = wpAABBMax.z;

            AABB aabbSubgrid;
            aabbSubgrid.Set(wpMin, wpMax);

            sCSceneInitializeSubgrid(ptCScene->ptSubgrids[nIndex], aabbSubgrid);
        }
    }
}





























































































































// NON_MATCHING: 2352/2448 bytes; original frame is 0x20 larger and the inlined clutter creation schedules differently.
static void sCSceneUpdateSubgrids(
    const Camera & cam,
    CScene * ptCScene,
    BOOL fSceneVisible)
{
    if (!ptCScene->ptSubgrids)
        return;



    ACDID idACDPlayer = LocalPlayerACDGet();
    if (idACDPlayer == ACDID_INVALID)
        return;












    Vector3D wpPlayer = ACDGet(idACDPlayer).GetWorldLocation().transform.wp;

    float flClutterDistanceMax = ParticleGetClutterDistance();

    const Frustum & tFrustum = cam.tFrustum;


    int32 nSubgridsX = ptCScene->nSubgridsX;
    int32 nSubgridsY = ptCScene->nSubgridCount / nSubgridsX;
    for (int32 y = 0; y < nSubgridsY; y++)
    {
        for (int32 x = 0; x < nSubgridsX; x++)
        {
            uint32 & dwFlags = ptCScene->ptSubgridFlags[y * nSubgridsX + x];
            ClutterSubgrid & tSubgrid = ptCScene->ptSubgrids[y * nSubgridsX + x]; // Inference: FALSE default below follows out-of-bounds deletion path.
            BOOL bBoundsOk = FALSE;

            if (fSceneVisible &&
                Math::Abs(tSubgrid.aabb.wp.x - wpPlayer.x) < flClutterDistanceMax + tSubgrid.aabb.wvExt.x &&
                Math::Abs(tSubgrid.aabb.wp.y - wpPlayer.y) < flClutterDistanceMax + tSubgrid.aabb.wvExt.y)
            {
                if (!(dwFlags & (1 << CLUTTER_SUBGRID_SAMPLED)))
                {
                    dwFlags |= (1 << CLUTTER_SUBGRID_SAMPLED);
                }




                // Inference: sampled-bit update has no surviving sample operation on this platform.
                bBoundsOk = Intersect::FrustumAndAABB(tFrustum, tSubgrid.aabb);
            }
            if (bBoundsOk && !(dwFlags & (1 << CLUTTER_SUBGRID_POPULATED)))
            {
                sCreateParticlesForSubgrid(tSubgrid, dwFlags, y * nSubgridsX + x, ptCScene);
            }
            else if (!bBoundsOk)
            {


                if (dwFlags & (1 << CLUTTER_SUBGRID_POPULATED))
                {
                    sDeleteParticlesFromSubgrid(tSubgrid, dwFlags);
                }
            }
        }
    }
}




















































static void sCSceneDisposeClutter(CScene * ptCScene)
{

    if (ptCScene->ptSubgrids)
    {
        for (int32 i = 0; i < ptCScene->nSubgridCount; i++)
        {
            ClutterSubgrid & tSubgrid = ptCScene->ptSubgrids[i];
            uint32 & dwFlags = ptCScene->ptSubgridFlags[i];
            if (dwFlags & (1 << CLUTTER_SUBGRID_POPULATED))
            {
                sDeleteParticlesFromSubgrid(tSubgrid, dwFlags);
            }
        }




        XFREE(ptCScene->ptSubgrids, NULL);
        XFREE(ptCScene->ptSubgridFlags, NULL);
    }
}



static void sCSceneDeleteRActors(CScene * ptCScene)
{

    CGame & tCGame = CGameGet();
    if (!tCGame.ptRActors)
    {
        return;
    }

    RActorIDListPooled & listRActors = RActorGetMainList();
    POS pos = listRActors.GetHeadPosition();
    while (pos)
    {
        POS posCurrent = pos;
        RActorID idRActor = (RActorID)listRActors.GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {

            if (ptRActor->idSScene == ptCScene->tSCD.idSScene)
            {

                RActorFlagForDeletion(ptRActor);
            }
        }
        else
        {


            listRActors.RemoveAt(posCurrent);
        }
    }


    pos = ptCScene->listSceneRActors.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)ptCScene->listSceneRActors.GetNext(pos);
        RActor * ptRActor = RActorGet(idRActor);

        RActorFlagForDeletion(ptRActor);
    }
}



























BOOL sTestAllVerticesAgainstPlanes(
    const AppearanceDefinition * pdefAppearance,
    const Plane * wTransformedPlanes,
    const SubObject & tSubObject)
{
    ScopedAppearanceBufferContentRetrieval tAppearanceBufferContentRetrieval(pdefAppearance);

    // Inference: six-plane loop and TRUE initial containment state follow the unrolled original comparisons.
    if (!(tSubObject.dwFlags & (1 << SUBOBJECT_COARSE_GEO_FADE_TEST_BIT)))
    {
        int32 nNumSegments = SubObjectGetNumSegments(tSubObject);
        for (int32 nSegment = 0; nSegment < nNumSegments; ++nSegment)
        {
            int32 nVertCount = SubObjectGetNumSegmentVertices(tSubObject, nSegment);
            for (int32 i = 0; i < nVertCount; ++i)
            {
                Vector3D wpPos;
                SubObjectGetSegmentVertexPos(tSubObject, nSegment, i, wpPos);
                BOOL fResult = TRUE;
                for (int32 nPlaneIndex = 0; nPlaneIndex < 6; ++nPlaneIndex)
                {
                    if (wTransformedPlanes[nPlaneIndex].ContainsPoint(wpPos) > 0.0f)
                    {
                        fResult = FALSE;
                        break;
                    }
                }

                if (fResult)
                {
                    return TRUE;
                }
            }
        }




    }
    else
    {
        int32 nNumSegments = SubObjectGetNumSegments(tSubObject);
        XGrowableArray<unsigned char, unsigned char> & arClipCodes = CGameGet().ptCSceneGlobals->arClipCodes;

        for (int32 nSegment = 0; nSegment < nNumSegments; ++nSegment)
        {
            int32 nVertCount = SubObjectGetNumSegmentVertices(tSubObject, nSegment);
            arClipCodes.SetArraySize_Tracked(nVertCount);

            // Inference: zero clip accumulator and shift-left packing follow original six-plane bit assembly.
            for (int32 i = 0; i < nVertCount; ++i)
            {
                Vector3D wpPos;
                SubObjectGetSegmentVertexPos(tSubObject, nSegment, i, wpPos);
                uint8 clipCode = 0;
                for (int32 nPlaneIndex = 0; nPlaneIndex < 6; ++nPlaneIndex)
                {
                    clipCode <<= 1;
                    if (wTransformedPlanes[nPlaneIndex].ContainsPoint(wpPos) > 0.0f)
                        clipCode |= 1;
                }

                if (!clipCode)
                    return TRUE;


                arClipCodes[i] = clipCode;
            }

            int32 nTris = SubObjectGetNumSegmentTriangles(tSubObject, nSegment);
            int32 nIndexListIndex = 0;
            int32 nIndexListIndexCount, nIndexListIndexIncrement;
            SubObjectGetSegmentIndexListWalkInfo(tSubObject, nSegment, nIndexListIndexCount, nIndexListIndexIncrement);


            // Inference: descending triangle count follows original decrement/nonzero loop.
            for (int32 i = nTris; i != 0; --i)
            {
                uint16 ndx0, ndx1, ndx2;
                SubObjectGetSegmentTriangle(tSubObject, nSegment, nIndexListIndex, ndx0, ndx1, ndx2);
                if (!(arClipCodes[ndx0] & arClipCodes[ndx1] & arClipCodes[ndx2]))
                    return TRUE;

                nIndexListIndex += nIndexListIndexIncrement;
            }
        }
    }

    return FALSE;
}




static BOOL sFrustumIntersectSubObject(
    const AppearanceDefinition * pdefAppearance,
    const CScene * ptCScene,
    Frustum & tFrustum,
    const SubObject & tSubObject)
{
    AABB aabb;
    SceneObjectAABBToWorldAABB(ptCScene->tSCD.transformWorld, tSubObject.aabbBounds, aabb);

    Sphere ws;
    aabb.GetMinimalBoundingSphere(ws);

    if (!Intersect::FrustumAndSphere(tFrustum, ws))
        return FALSE;



    if (!Intersect::FrustumAndAABB(tFrustum, aabb))
        return FALSE;



    // Inference: GetPlanes() supplies the original private plane-array loads without an emitted call.
    Plane wTransformedPlanes[6];
    for (uint32 i = 0; i < 6; ++i)
    {
        Vector3D vecPointOnPlane = tFrustum.GetPlanes()[i].wvNormal * tFrustum.GetPlanes()[i].wdD;
        vecPointOnPlane = ptCScene->tSCD.transformWorld * vecPointOnPlane;
        PRTransform trNoTrans = ptCScene->tSCD.transformWorld;
        trNoTrans.wp.Set(0.0f, 0.0f, 0.0f);
        wTransformedPlanes[i].wvNormal = trNoTrans * tFrustum.GetPlanes()[i].wvNormal;
        wTransformedPlanes[i].wdD = Vector3D::DotProduct(wTransformedPlanes[i].wvNormal, vecPointOnPlane);
    }

    return sTestAllVerticesAgainstPlanes(pdefAppearance, wTransformedPlanes, tSubObject);
}




static BOOL sSubObjectShouldFade(
    const AppearanceDefinition * pdefAppearance,
    CScene * ptCScene,
    Frustum ** ptFrustums,
    uint32 nFrustumCount,
    const SubObject & tSubObject,
    SubObjectState & tSubObjectState)
{
    // Inference: 0.3f check interval is the original constant at 0xe3bedc.
    float flCurrentTime = CGameGetClientTime();
    if (tSubObjectState.flLastFadeCheckTime + 0.3f > flCurrentTime)
    {
        return tSubObjectState.fLastFadeCheckResult;
    }

    float flTimeSlice = flCurrentTime - tSubObjectState.flLastFadeCheckTime;
    float flTrunc = Math::FloatTrunc(flTimeSlice / 0.3f);
    tSubObjectState.flLastFadeCheckTime = flCurrentTime - (flTimeSlice - flTrunc * 0.3f);
    // Inference: both eDebugFadeMode compares carry line 2086 (one switch); case order follows the original block layout.
    switch (sgtAppGlobals.eDebugFadeMode)
    {
    case DFM_FADE_ALL:
        tSubObjectState.fLastFadeCheckResult = TRUE;
        return TRUE;
    case DFM_FADE_NEVER:
        tSubObjectState.fLastFadeCheckResult = FALSE;
        return FALSE;
    default:
        break;
    }

    if (tSubObject.dwFlags & (1 << SUBOBJECT_FADE_CHILD_BIT))
    {
        tSubObjectState.fLastFadeCheckResult = FALSE;
        return FALSE;
    }




    if (ptFrustums)
    {

        for (uint32 i = 0; i < nFrustumCount; ++i)
        {
            if (ptFrustums[i] && sFrustumIntersectSubObject(pdefAppearance, ptCScene, *ptFrustums[i], tSubObject))
            {
                tSubObjectState.fLastFadeCheckResult = TRUE;
                return TRUE;
            }
        }
    }

    tSubObjectState.fLastFadeCheckResult = FALSE;
    return FALSE;
}



static BOOL sSubObjectShouldFade(
    const AppearanceDefinition * pdefAppearance,
    const Matrix4x4 & mWorld,
    const Frustum & tFrustum,
    const SubObject & tSubObject,
    SubObjectState & tSubObjectState)
{
    // Inference: same 0.3f interval follows the original constant and time arithmetic.
    float flCurrentTime = CGameGetClientTime();
    if (tSubObjectState.flLastFadeCheckTime + 0.3f > flCurrentTime)
    {
        return tSubObjectState.fLastFadeCheckResult;
    }

    float flTimeSlice = flCurrentTime - tSubObjectState.flLastFadeCheckTime;
    float flTrunc = Math::FloatTrunc(flTimeSlice / 0.3f);
    tSubObjectState.flLastFadeCheckTime = flCurrentTime - (flTimeSlice - flTrunc * 0.3f);

    // Inference: GetPlanes() supplies original plane loads; inverse transform follows original operations.
    Matrix4x4 mInvWorld;
    mInvWorld = mWorld.Inverse();
    Plane wTransformedPlanes[6];
    for (uint32 i = 0; i < 6; ++i)
    {
        Vector3D vecPointOnPlane = tFrustum.GetPlanes()[i].wvNormal * -tFrustum.GetPlanes()[i].wdD;
        vecPointOnPlane = vecPointOnPlane * mInvWorld;
        wTransformedPlanes[i].wvNormal = Matrix4x4::Transform3x3(tFrustum.GetPlanes()[i].wvNormal, mInvWorld);
        wTransformedPlanes[i].wdD = -Vector3D::DotProduct(wTransformedPlanes[i].wvNormal, vecPointOnPlane);
    }

    BOOL fFadeCheckResult = sTestAllVerticesAgainstPlanes(pdefAppearance, wTransformedPlanes, tSubObject);
    tSubObjectState.fLastFadeCheckResult = fFadeCheckResult;
    return fFadeCheckResult;
}



static void sUpdateSurfaceEffects(
    const CScene * ptCScene,
    CSceneSubObject & tCSceneSubObject,
    const int32 nSubObjectIndex,
    const SNO snoSurface)
{
    const SurfaceDefinition * pdefSurface = (const SurfaceDefinition *)gptGroupDefSurface->LockRFast(snoSurface);

    // Inference: resident surface definitions have no unlock operation; only missing effects are created.
    if (pdefSurface->snoParticleSystem != SNO_NONE &&
        tCSceneSubObject.idSurfaceEffect == PARTICLESYSTEMID_INVALID)
    {
        ParticleSystemAddParams tParams(pdefSurface->snoParticleSystem, PRTransform(cgqIdentity, tCSceneSubObject.wsBounds.wpCenter));
        tCSceneSubObject.idSurfaceEffect = ParticleSystemAdd(tParams);

        ParentData tParentData;
        tParentData.SetHardpointIndex(4, ptCScene->id, -1);
        // Inference: parent type4, no hardpoint, subobject index and FALSE are original arguments.
        ParticleSystem * ptParticleSystem = ParticleSystemGet(tCSceneSubObject.idSurfaceEffect);
        ptParticleSystem->SetParent(tParentData, nSubObjectIndex, FALSE);
    }
}






static void sFadeSubObject(
    const AppearanceDefinition * pdefAppearance,
    CScene * ptCScene,
    Frustum * ptFrustum,
    CSceneSubObject & tCSceneSubObject,
    const SubObject & tSubObject,
    const float flDeltaTime)
{
    float flAlphaGoal = 1.0f;
    switch (tSubObject.nFadeGroup)
    {
    case FADE_INDIVIDUAL:
        if (sSubObjectShouldFade(pdefAppearance, ptCScene, &ptFrustum, 1, tSubObject, tCSceneSubObject.tSubObjectState))
            flAlphaGoal = 0.0f;
        break;


    case FADE_BELOW_PLAYER:
    {

        RActorID idPlayerRActor = LocalPlayerRActorGet();
        RActor * ptRActorPlayer = RActorTryToGet(idPlayerRActor);
        if (ptRActorPlayer)
        {
            AABB aabb;
            SceneObjectAABBToWorldAABB(ptCScene->tSCD.transformWorld, tSubObject.aabbBounds, aabb);

            Vector3D wpActor;
            RActorGetCenterPoint(ptRActorPlayer, wpActor);

            Vector3D wv;
            wv.x = aabb.wp.x - wpActor.x;
            wv.y = aabb.wp.y - wpActor.y;
            wv.z = 0.0f;
            wv.Normalize();
            // Inference: direction (0.7071,0.7071,0) and dot threshold 0.5 follow original constants.
            const Vector3D wvDir(0.7071f, 0.7071f, 0.0f);
            float flDotProduct = Vector3D::DotProduct(wv, wvDir);
            if (flDotProduct > 0.5f)
            {

                flAlphaGoal = 1.0f - flDotProduct;
            }
        }
        break;
    }
    default:
    {
        CSceneGlobals * ptCSceneGlobals = CGameGet().ptCSceneGlobals;

        if (tSubObject.nFadeGroup < MAX_FADE_GROUPS)
        {
            BOOL fFade = ptCSceneGlobals->fFadeGroup[tSubObject.nFadeGroup] ||
                ptCSceneGlobals->fFadeGroupRActors[tSubObject.nFadeGroup];

            if (fFade)
                flAlphaGoal = 0.0f;
        }
        break;
    }
    }





    // Inference: named fade rates scale by delta time and alpha clamps to authored floor and 1.
    if (flAlphaGoal < tCSceneSubObject.flAlpha)
    {
        tCSceneSubObject.flAlpha = Math::Max(tCSceneSubObject.flAlpha - ALPHA_FADE_OUT_RATE * flDeltaTime, flAlphaGoal);
    }
    else if (tCSceneSubObject.flAlpha < flAlphaGoal)
    {
        tCSceneSubObject.flAlpha = Math::Min(tCSceneSubObject.flAlpha + ALPHA_FADE_IN_RATE * flDeltaTime, flAlphaGoal);
    }

    tCSceneSubObject.flAlpha = Math::Pin(tCSceneSubObject.flAlpha, tSubObject.flFadeAlpha, 1.0f);
}




static SNO sSelectShader(
    const SubObjectAppearance & tSOA,
    const SubObject & tSubObject,
    const float flAlpha,
    CScene * ptCScene,
    const SubObjectState & tSubObjectState,
    BOOL & fUsingFadeShader)
{




    SNO snoShader;






























    // Inference: code only on 2319/2323. Row 2319 columns (!= 28, && 46, < 57) fit a tab-indented
    // `if (tSubObject.nFadeGroup != FADE_DONT_FADE && flAlpha < 0.999999f)`; the TRUE/FALSE stores leave no rows.

    if (tSubObject.nFadeGroup != FADE_DONT_FADE && flAlpha < 0.999999f)
        fUsingFadeShader = TRUE;
    else
        fUsingFadeShader = FALSE;
    snoShader = fUsingFadeShader ? tSubObjectState.snoFadeShader : tSubObjectState.snoDefaultShader;

    return snoShader;
}




























































































































































static BOOL sSelectPrepassShader(
    const SubObject & tSubObject,
    const float flAlpha,
    const ShaderDefinition * pdefShader)
{
    if ((flAlpha >= 0.999999f || tSubObject.nFadeGroup == FADE_DONT_FADE) &&

        pdefShader->nPassCount == 1)
    {
        RenderPass * ptPass = pdefShader->ptPasses;
        if (!ptPass->tRenderParams.fAlphaTestEnable &&
            !ptPass->tRenderParams.fAlphaBlendEnable &&
            ptPass->tRenderParams.fZBufferWriteEnable == TRUE &&
            ptPass->tRenderParams.eCullMode == CULLMODE_CW &&

            (ptPass->eRenderLayer == RENDERLAYER_OPAQUE || ptPass->eRenderLayer == RENDERLAYER_EARLY_OPAQUE || ptPass->eRenderLayer == RENDERLAYER_FLOOR))
        {
            return TRUE;
        }
    }
    return FALSE;
}




































static int32 sSubObjectUpdate(
    const AppearanceDefinition * pdefAppearance,
    CScene * ptCScene,
    const Camera & cam,
    Frustum * ptFrustum,
    const SubObject & tSubObject,
    CSceneSubObject & tCSceneSubObject,
    const float flDeltaTime,
    const float flClientTime,
    const int32 nMaxLightCount,
    const LightState ** pArrayLights,
    RenderContext eRenderContext)
{
    SubObjectState & tSubObjectState = tCSceneSubObject.tSubObjectState;
    // Inference: fDoRender's BOOL normalization is source spelling for the original bit-zero test.
    BOOL fDoRender = (tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_CAN_BE_RENDERED_BIT)) != 0;
    if (!fDoRender)
    {
        tCSceneSubObject.fVisible = tCSceneSubObject.fCastsShadow = FALSE;
        return 0;
    }


    tCSceneSubObject.fVisible = TRUE;
    tCSceneSubObject.fCastsShadow = (tSubObject.dwFlags & (1 << SUBOBJECT_CASTS_SHADOW)) != 0;



    if (tSubObject.nFadeGroup != FADE_DONT_FADE)
    {
        sFadeSubObject(pdefAppearance, ptCScene, ptFrustum, tCSceneSubObject, tSubObject, flDeltaTime);
    }

    AABB aabb;
    SceneObjectAABBToWorldAABB(ptCScene->tSCD.transformWorld, tSubObject.aabbBounds, aabb);

    if (!CameraComputeAABBVisibility(cam, aabb))
        tCSceneSubObject.fVisible = FALSE;



    if (tCSceneSubObject.fCastsShadow && !CameraComputeAABBVisibility(CameraManagerGetCamera(1), aabb))
        tCSceneSubObject.fCastsShadow = FALSE;



    if (!tCSceneSubObject.fVisible && !tCSceneSubObject.fCastsShadow)
        return 0;




    WorldDistance wdDist = aabb.wvExt.Magnitude();
    tCSceneSubObject.aabbBounds = aabb;
    tCSceneSubObject.wsBounds.Set(aabb.wp, wdDist);

    tCSceneSubObject.rect2DWorld.Set(
        aabb.wp.x - aabb.wvExt.x,
        aabb.wp.y - aabb.wvExt.y,
        aabb.wp.x + aabb.wvExt.x,
        aabb.wp.y + aabb.wvExt.y);

    const SubObjectAppearance & tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLook];
    SubObjectStateUpdate(tSOA, tSubObjectState, NULL, flDeltaTime, flClientTime);

    uint32 dwLightFilterFlags;
    dwLightFilterFlags = (tSubObject.dwFlags & (1 << SUBOBJECT_INTERIOR_SCENE_BIT)) ? 0x12 : 0x18;







    // Inference: 0x12/0x18 filters and NULL optional SNO array are the original call arguments.
    return LightsGetDynamic(eRenderContext, tCSceneSubObject.wsBounds, dwLightFilterFlags, nMaxLightCount, pArrayLights, &tCSceneSubObject.aabbBounds, NULL);
}




static void sRenderSubmitStencilBounds(
    DisplayList & dispList,
    Renderable & tRenderable,
    int32 nSubObjectIndex)
{
    RenderableDetails & tDetails = tRenderable.details;


    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = tDetails.tAppearance.pdefAppearance->tStructure.tGeoSets[eGeoSetType];
    const SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
    AABB aabbBounds;
    Vector3D wpMin;
    Vector3D wpMax;


    wpMin = tSubObject.aabbBounds.wp - tSubObject.aabbBounds.wvExt;
    wpMax = tSubObject.aabbBounds.wp + tSubObject.aabbBounds.wvExt;

    // Inference (2645/2646): DWARF inlines Vector3D::operator= into the float[3] stencil bounds.
    const float flInflate = 1.5f;
    aabbBounds.Set(wpMin, wpMax);
    aabbBounds.Inflate(flInflate);
    (Vector3D &)tDetails.tStencilBound.wp = aabbBounds.wp;
    (Vector3D &)tDetails.tStencilBound.wvExt = aabbBounds.wvExt;

    dispList.SubmitRenderable(tRenderable);
}




static void sCScenesRenderSubObject(
    DisplayList & dispList,
    DisplayList & dispListProjected,
    CScene * ptCScene,
    const Camera & cam,
    CSceneSubObject & tCSceneSubObject,
    const AppearanceDefinition * pdefAppearance,
    const int32 nSubObjectIndex,
    int32 nLightCount,
    const LightState ** pArrayLights,
    const ShaderDefinition * pdefShaderPrepass,
    const ShaderDefinition * pdefShaderProjectedLayer,
    const ShaderDefinition * pdefShaderCookie,
    const ShaderDefinition * pdefShaderShadowStencilBounds,
    const ShaderDefinition * pdefShaderSceneFadeZPass)
{


    SubObjectState & tSubObjectState = tCSceneSubObject.tSubObjectState;
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];
    const SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
    const SubObjectAppearance & tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLook];
    const BOOL bAppPrepassFlagEnabled = AppDrawFlagIsSet(APP_DRAW_DEPTH_PREPASS_BIT);

    PackedVBEntry * ptEntry;
    PackedIBEntry * ptEntryIB;





























    if (AppDrawFlagIsSet(APP_DRAW_WALKABLE_SUBOJBECTS_ONLY_BIT) &&
        ((tSubObject.dwFlags & (1 << SUBOBJECT_NOT_WALKABLE_BIT)) || TagMapGetInt(tSOA.hTagMapMasterMaterial, TAG_APPEARANCE_FORCE_NONWALKABLE)))
        return;



    BOOL fUsingFadeShader;
    SNO snoShader = sSelectShader(tSOA, tSubObject, tCSceneSubObject.flAlpha, ptCScene, tSubObjectState, fUsingFadeShader);








    if (snoShader == SNO_NONE)
        return;




    ScopedShaderDefinitionLockR pdefShader(snoShader);
    {
        uint32 dwRenderLayers = pdefShader->dwRenderLayers;
        if (dwRenderLayers & (1 << RENDERLAYER_POSTFX_DISTORTION))
        {
            if (!AppDrawFlagIsSet(APP_DRAW_HWCLASS_DISTORTIONS_BIT))
                return;


            PostFXSetFrameFlag(POSTFX_FRAME_HAS_SOMETHING_DISTORTING, TRUE);
        }
    }






    Renderable tRenderable(
        RENDERABLETYPE_SCENE,
        ptCScene->id,
        PERFBIN_BACKGROUND,
        pdefShader,
        tSOA.tCompositeUberMaterial.mat);
    RenderableHeader & tHeader = tRenderable.header;
    RenderableDetails & tDetails = tRenderable.details;

    ptCScene->tSCD.transformWorld.ToMatrix(tDetails.tCommon.mWorld);



    if (snoShader != GlobalSNOGet(GLOBALSNO_SHADER_SCENE_FOGPLANES) &&
        snoShader != GlobalSNOGet(GLOBALSNO_SHADER_SCENE_INTERACTIVE_FOGPLANES) &&
        snoShader != GlobalSNOGet(GLOBALSNO_SHADER_SCENE_INTERACTIVE_FOGPLANES_VERTALPHA) &&
        snoShader != GlobalSNOGet(GLOBALSNO_SHADER_SCENE_INTERACTIVE_CORRUPTION) &&
        snoShader != GlobalSNOGet(GLOBALSNO_SHADER_SCENE_INTERACTIVE_CORRUPTION_VERTALPHA))
    {
        tDetails.tCommon.mWorld = cmSceneScaleTweak * tDetails.tCommon.mWorld;
    }

    tDetails.tAppearance.pdefAppearance = pdefAppearance;
    tDetails.tAppearance.nSubObjectIndex = nSubObjectIndex;
    tDetails.tAppearance.flLookSwitchTime = tSubObjectState.flLookSwitchTime;
    tDetails.tAppearance.idCloth = tSubObjectState.idCloth;
    tDetails.tAppearance.eDyeType = tSubObjectState.eDyeType;
    tDetails.tAppearance.nTexAnimStateCount = tSubObjectState.nTexAnimStateCount;

    tDetails.tAppearance.arTexAnimStates = TexAnimRenderAsyncCopy(tSubObjectState.nTexAnimStateCount, tSubObjectState.arTexAnimStates, &tSOA.tCompositeUberMaterial, SHADERTEX_NONE);




    tDetails.tAppearance.ptSOA = &tSOA;
    tDetails.tAppearance.ptSubObject = &tSubObject;

    if (!(tSubObject.dwFlags & ((1 << SUBOBJECT_EDGE_GEOMETRY_BIT) | (1 << SUBOBJECT_PACK_GEOMETRY_BIT)))) {

        ptEntry = &tSubObject.ptPostprocessed->tVBEntry;
        ptEntryIB = &tSubObject.ptPostprocessed->tIBEntry;
        tDetails.tAppearance.ptEntry = ptEntry;
        tDetails.tAppearance.ptEntryIB = ptEntryIB;




    } else if ((tSubObject.dwFlags & ((1 << SUBOBJECT_PACK_GEOMETRY_BIT) | (1 << SUBOBJECT_PACK_SKIN_BIT))) == (1 << SUBOBJECT_PACK_GEOMETRY_BIT)) {


        ptEntry = &tSubObject.ptPostprocessed->tPackVBEntry;
        ptEntryIB = &tSubObject.ptPostprocessed->tIBEntry;
        tDetails.tAppearance.ptEntry = ptEntry; tDetails.tAppearance.ptEntryIB = ptEntryIB;
    } else if (tSubObject.dwFlags & (1 << SUBOBJECT_PACK_SKIN_BIT)) {
        ptEntry = &tSubObject.ptPostprocessed->tVBEntry;
        ptEntryIB = &tSubObject.ptPostprocessed->tIBEntry;
        tDetails.tAppearance.ptEntry = ptEntry;
        tDetails.tAppearance.ptEntryIB = ptEntryIB;
    }
    tDetails.tAppearance.snoAppearance = pdefAppearance->tHeader.snoHandle;

    tDetails.tAppearance.eOverlayType = 0;

    tHeader.rgbaColor = crgbaWhite;
    uint8 bAlpha;  // 2815 columns: nFadeGroup 17, && 46 (tab indent): fade-group test first
    if (tSubObject.nFadeGroup != FADE_DONT_FADE && fUsingFadeShader)
    {
        bAlpha = (uint8)Math::FloatRoundToInt(tCSceneSubObject.flAlpha * 255.0f);

        tHeader.rgbaColor.a = bAlpha;
    }

    // Inference: the original shifts the sign-extended fade group left by one (z-pass group + 1 below).
    tHeader.nGroup = tSubObject.nFadeGroup * 2;


    float flDist = Vector3D::DistanceSquared(tCSceneSubObject.wsBounds.wpCenter, cam.cmState.transform.wp);
    tHeader.SetDist(flDist);
    tHeader.fIsWall = TRUE;
    tHeader.SetRect(tCSceneSubObject.rect2DWorld);

    tDetails.tCommon.nLightCount = nLightCount;
    if (nLightCount)
    {
        SigmaMemoryCopy(tDetails.tCommon.pArrayLights, pArrayLights, sizeof(tDetails.tCommon.pArrayLights));
    }

    int32 nDefaultRenderableIndex = dispList.SubmitRenderableDetails(tDetails);

    const float flScenePrepassClipDist = tCSceneSubObject.wsBounds.wdRadius + 600.0f;
    const float flScenePrepassClipDistSqrd = flScenePrepassClipDist * flScenePrepassClipDist;
    if (flDist < flScenePrepassClipDistSqrd &&

        sSelectPrepassShader(tSubObject, tCSceneSubObject.flAlpha, tHeader.pdefShader))

        tHeader.fInPrepass = TRUE;




    if (tCSceneSubObject.flAlpha >= 0.000001f && tCSceneSubObject.fVisible)
    {
        if ((tSubObject.nFadeGroup != FADE_DONT_FADE && fUsingFadeShader) || !(tSubObject.dwFlags & (1 << SUBOBJECT_RECEIVES_SHADOW)))
        {

            tHeader.fSetShadowLevel = FALSE;
        }
        else
        {
            tHeader.fSetShadowLevel = TRUE;
        }

        dispList.SubmitRenderableHeader(tHeader, nDefaultRenderableIndex);

        const GeoSet & tGeoSetLocal = pdefAppearance->tStructure.tGeoSets[eGeoSetType];
        if (tGeoSetLocal.ptSubObjects[tDetails.tAppearance.nSubObjectIndex].dwFlags & (1 << SUBOBJECT_RECEIVES_FX_BIT))
        {

            tHeader.pdefShader = pdefShaderProjectedLayer;
            dispListProjected.SubmitRenderable(tRenderable);
        }
    }

    if (tCSceneSubObject.fCastsShadow)
    {
        RGBAColor colorSave(tHeader.rgbaColor);
        tHeader.pdefShader = pdefShaderCookie;
        tHeader.rgbaColor = RGBAColor(0, 0, 0, 255);
        dispList.SubmitRenderableHeader(tHeader, nDefaultRenderableIndex);
        tHeader.rgbaColor = colorSave;
    }



    if (tCSceneSubObject.fCastsShadow)
    {
        Material mat;
        SigmaMemoryClear(&mat, sizeof(mat));

        Renderable tStencilRenderable(
            RENDERABLETYPE_STENCIL_BOUNDS,
            ptCScene->id,
            PERFBIN_BACKGROUND,
            pdefShaderShadowStencilBounds,
            mat);
        RenderableDetails & tStencilDetails = tStencilRenderable.details;

        tStencilDetails.tCommon.mWorld = tDetails.tCommon.mWorld;

        tStencilDetails.tAppearance.pdefAppearance = pdefAppearance;

        sRenderSubmitStencilBounds(dispList, tStencilRenderable, nSubObjectIndex);
    }



    if (tCSceneSubObject.fVisible && fUsingFadeShader && tSubObject.nFadeGroup != FADE_DONT_FADE &&

        tCSceneSubObject.flAlpha >= 0.000001f)
    {



        tHeader.nGroup++;

        tHeader.pdefShader = pdefShaderSceneFadeZPass;
        dispList.SubmitRenderableHeader(tHeader, nDefaultRenderableIndex);
    }

    if (bAppPrepassFlagEnabled && tHeader.fInPrepass)
    {
        tHeader.pdefShader = pdefShaderPrepass;
        dispList.SubmitRenderableHeader(tHeader, nDefaultRenderableIndex);
    }
}
























static void sCSceneInclusionsOnEnter(
    SceneCommonData & tSCD)
{
    CSceneGlobals * ptCSceneGlobals = CGameGet().ptCSceneGlobals;
    ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);

    // Inference: placeholder worlds suppress inclusions, preserving the original early return.
    if (WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(tSCD.idSWorld)))
        return;



    if (pdefScene->arInclusions.GetArraySize())
    {
        PRTransform tPRTransform = cgtPRTransformIdentity;
        const WorldCommonData & tWCD = WorldGetCommonData(CGameGetCurrentSWorldID());
        tPRTransform.wp = Vector3D(-tWCD.pointWorldOrigin.x, -tWCD.pointWorldOrigin.y, 0);

        ptCSceneGlobals->arAddedScenes.SetArraySize_Tracked(pdefScene->arInclusions.GetArraySize());
        for (int32 i = 0; i != pdefScene->arInclusions.GetArraySize(); i++)
        {
            SNO snoInclusion = pdefScene->arInclusions[i];
            SceneSpecification tSceneSpec;
            SceneSpecInitialize(tSceneSpec);
            WorldCacheSceneValues(snoInclusion, tSceneSpec.tCachedValues);
            CSceneID idAddedScene = CSceneAdd(
                snoInclusion,
                CSCENEID_INVALID,
                tSceneSpec,
                CGameGetCurrentSWorldID(),
                tPRTransform,
                CSCENEID_INVALID, SNO_NONE, FALSE, TRUE, RENDERCONTEXT_DEFAULT, NULL, 0);
            // Inference: invalid source/parent IDs, vista TRUE/default context match original arguments.
            ptCSceneGlobals->arAddedScenes[i] = idAddedScene;
        }
    }
}








static void sCSceneInclusionsOnLeave()
{

    CSceneGlobals * ptCSceneGlobals = CGameGet().ptCSceneGlobals;
    int32 nNumAddedScenes = ptCSceneGlobals->arAddedScenes.GetArraySize();
    if (nNumAddedScenes)
    {
        for (int32 i = 0; i != nNumAddedScenes; i++)
        {

            sCSceneFlagForDeletion(ptCSceneGlobals->arAddedScenes[i]);
        }

        ptCSceneGlobals->arAddedScenes.EmptyArray();
    }
}



static void sCSceneOnSceneChange()
{
    Player * ptPlayer = LocalPlayerGet();
    if (!ptPlayer)
        return;


    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return;



    CSceneGlobals * ptCSceneGlobals = CGameGet().ptCSceneGlobals;
    SSceneID idScene = ACDGetCurrentSSceneID(*ptACDPlayer);


    if (idScene != ptCSceneGlobals->idCurrentScene)
    {
        sCSceneInclusionsOnLeave();

        ptCSceneGlobals->idCurrentScene = idScene;

        SceneCommonData * ptSCD = SceneCommonDataTryToGet(idScene);
        if (ptSCD)
        {
            sCSceneInclusionsOnEnter(*ptSCD);
        }
    }
}























// Inference: BSS symbols at 0x1772e90/98/a0/a4 supply zero-initialized job statics; declaration lines from DWARF.
Frustum * SceneRenderUpdateJob::sptFrustum;
const Camera * SceneRenderUpdateJob::sptCam;
float SceneRenderUpdateJob::sflDeltaTime;
float SceneRenderUpdateJob::sflClientTime;

inline SceneRenderUpdateJob::SceneRenderUpdateJob(  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    ThreadedSceneInfo * ptSceneInfo,
    int32 nStartingIndex,
    int32 nCount)
    : m_ptSceneInfo(ptSceneInfo), m_nStartingIndex(nStartingIndex), m_nCount(nCount)
{
}





void SceneRenderUpdateJob::Execute()
{
    GameSetActiveGameCommonData(&CGameGet().tGCD, TRUE);

    int32 nStartingIndex = m_nStartingIndex;
    int32 nCount = m_nCount + nStartingIndex;
    CScene * ptScene = m_ptSceneInfo->ptScene;
    const AppearanceDefinition * pdefAppearance = m_ptSceneInfo->pdefAppearance;
    float flDeltaTime = sflDeltaTime;
    float flClientTime = sflClientTime;
    Frustum * ptFrustum = sptFrustum;
    const Camera & cam = *sptCam;
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];
    const SubObject * ptSubObjects = tGeoSet.ptSubObjects;
    CSceneSubObject * ptSceneSubObjects = ptScene->pSceneSubobjects;
    // Inference: original immediate light limit 15 and default render context; no extra synthetic callers.
    for (int32 nSubObjectIndex = nStartingIndex; nSubObjectIndex < nCount; ++nSubObjectIndex)
    {
        ThreadedSubObjectInfo & info = m_ptSceneInfo->pSubObjectInfo[nSubObjectIndex];
        const SubObject & tSubObject = ptSubObjects[nSubObjectIndex];
        CSceneSubObject & tCSceneSubObject = ptSceneSubObjects[nSubObjectIndex];
        info.nLightCount = sSubObjectUpdate(pdefAppearance, ptScene, cam, ptFrustum, tSubObject, tCSceneSubObject, flDeltaTime, flClientTime, 15, info.pArrayLights, RENDERCONTEXT_DEFAULT);
    }
}

void SceneRenderUpdateJob::JobFcn(void * pParam)
{

    SceneRenderUpdateJob * pThis = (SceneRenderUpdateJob *)pParam;
    pThis->Execute();
}















template <bool fMultiThreaded>
static void sCScenesUpdate(
    RWindow & tRWindow,
    const float flDeltaTime,
    const float flClientTime)
{
    AsyncRWindowData & tAsyncRVData = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(0)];

    CGame & tCGame = CGameGet();

    const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());
    Frustum * ptFrustums[4];
    SigmaMemoryFill(ptFrustums, 0, sizeof(ptFrustums));
    Frustum tFrustums[4];

    uint32 nLocalPlayerCount = 0;

    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer))
        {
            RActor * ptRActorPlayer = RActorTryToGet(ptPlayer->idPlayerRActor);

            if (ptRActorPlayer)
            {

                Vector3D wpActor;
                RActorGetCenterPoint(ptRActorPlayer, wpActor);
                tFrustums[nLocalPlayerCount].Set(wpActor, cam.cmState.transform.wp, cgwvZAxis, 0.6981317f, 0.75f, 0.1f, 100.0f);
                ptFrustums[nLocalPlayerCount] = &tFrustums[nLocalPlayerCount];
                nLocalPlayerCount++;
            }
        }
    }

    CSceneGlobals * ptCSceneGlobals = tCGame.ptCSceneGlobals;

    SigmaMemoryClear(ptCSceneGlobals->fFadeGroup, sizeof(ptCSceneGlobals->fFadeGroup));

    sCSceneOnSceneChange();

    PostFXResetReflectionPlanes();

    ThreadedSceneInfo arThreadedSceneInfo[1280];
    ThreadedSubObjectInfo arThreadedSubObjectInfo[2560];
    int32 nVisibleSceneCount = 0;
    int32 nVisibleSubObjectCount = 0;



    TDataArrayIterator<CScene> tIt;
    while (CSceneDataArrayIterate(tIt, tCGame, IncludeHolodeckScenes))
    {
        CScene * ptCScene = tIt.ptElement;

        if (ptCScene->dwFlags & (1 << CSCENE_DELETE_AT_END_OF_UPDATE_BIT))
            continue;



        if (tAsyncRVData.GetRenderContext() != ptCScene->eContext)
            continue;




        if (!sSceneIsVisible(cam, ptCScene))
        {
            sCSceneUpdateSubgrids(cam, ptCScene, FALSE);
            continue;
        }

        sCSceneUpdateSubgrids(cam, ptCScene, TRUE);

        SNO snoAppearance;
        BOOL bIsBorderScene;








        snoAppearance = ptCScene->snoAppearance;
        bIsBorderScene = ptCScene->dwFlags & (1 << CSCENE_IS_BORDER_BIT);



        if (AppDrawFlagIsSet(APP_DRAW_NO_DRLG_BORDER_SCENES_BIT) && bIsBorderScene)
            continue;




        const AppearanceDefinition * pdefAppearance = (const AppearanceDefinition *)gptGroupDefAppearance->LockR(snoAppearance, FALSE);
        const Structure & tStructure = pdefAppearance->tStructure;
        const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
        const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];

        if (ptCScene->nNumSceneSubobjects != tGeoSet.nSubObjectCount)
        {


            sCSceneResetForAppearanceChange(*ptCScene);
        }







        BOOL fContainsFadeGroup = (ptCScene->dwFlags & (1 << CSCENE_CONTAINS_FADE_GROUP_BIT)) != 0;
        BOOL fContainsReflection = (ptCScene->dwFlags & (1 << CSCENE_CONTAINS_REFLECTION_BIT)) != 0;
        if (fContainsFadeGroup || fContainsReflection)
        {

            for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; nSubObjectIndex++)
            {
                const SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
                SubObjectState & tSubObjectState = ptCScene->pSceneSubobjects[nSubObjectIndex].tSubObjectState;









                // Inference: the abstract DIE keeps PackedVBEntry & tEntry (3268) and PackedIBEntry &
                // tEntryIB (3269) in a nested block with no instructions in this instance. The bindings
                // follow the member names (SubObjectPostData::tVBEntry/tIBEntry); whatever used them
                // leaves no code here and is not recoverable.

                {
                    PackedVBEntry & tEntry = tSubObject.ptPostprocessed->tVBEntry;
                    PackedIBEntry & tEntryIB = tSubObject.ptPostprocessed->tIBEntry;
                }




                BOOL fDoRender = (tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_CAN_BE_RENDERED_BIT)) != 0;
                if (fDoRender)
                {
                    if (tSubObject.nFadeGroup > FADE_INDIVIDUAL && tSubObject.nFadeGroup < MAX_FADE_GROUPS)
                    {

                        if (!ptCSceneGlobals->fFadeGroup[tSubObject.nFadeGroup])
                        {
                            ptCSceneGlobals->fFadeGroup[tSubObject.nFadeGroup] = sSubObjectShouldFade(pdefAppearance, ptCScene, ptFrustums, nLocalPlayerCount, tSubObject, tSubObjectState);
                        }
                    }
                }
            }
        }


















        ThreadedSceneInfo & info = arThreadedSceneInfo[nVisibleSceneCount++];
        info.ptScene = ptCScene;
        info.pdefAppearance = pdefAppearance;



        info.pSubObjectInfo = &arThreadedSubObjectInfo[nVisibleSubObjectCount];
        nVisibleSubObjectCount += tGeoSet.nSubObjectCount;

        if (nVisibleSubObjectCount >= 2560)
        {
            DisplayInternalError(NULL, "", 3318, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }




    CameraManagerUpdateChildCameras(tRWindow.GetCameraIndex());
    // Inference (3328/3368): the only compare (3328) branches straight to the epilogue, skipping the
    // render-block shader locks; abstract DIE has separate job (3335) and render (3370) blocks.
    if (fMultiThreaded && nVisibleSceneCount > 0)
    {
        SceneRenderUpdateJob::sptCam = &cam;
        SceneRenderUpdateJob::sptFrustum = ptFrustums[0];
        SceneRenderUpdateJob::sflDeltaTime = flDeltaTime;
        SceneRenderUpdateJob::sflClientTime = flClientTime;

        char arJobBuffer[20480];
        int32 nMaxJobsPerScene = 1280 / nVisibleSceneCount;

        int32 nJobIndex = 0;
        for (int32 i = 0; i < nVisibleSceneCount; i++)
        {
            ThreadedSceneInfo & info = arThreadedSceneInfo[i];
            const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
            const GeoSet & tGeoSet = info.pdefAppearance->tStructure.tGeoSets[eGeoSetType];
            int32 nCount = tGeoSet.nSubObjectCount;
            int32 nIncrement = Math::Max(nCount / nMaxJobsPerScene + 1, 16);

            for (int32 j = 0; j < nCount; j += nIncrement, nJobIndex++)
            {
                void * pBuffer = &arJobBuffer[nJobIndex * sizeof(SceneRenderUpdateJob)];
                SceneRenderUpdateJob * ptJob = new (pBuffer) SceneRenderUpdateJob(&info, j, Math::Min(nIncrement, nCount - j));








                JobManager::Get()->PushJob(SceneRenderUpdateJob::JobFcn, ptJob);
            }
        }


        JobManager::Get()->WaitForIdle();
    }


    if (nVisibleSceneCount > 0)
    {
        ScopedShaderDefinitionLockR pdefShaderProjectedLayer(GlobalSNOGet(GLOBALSNO_SUBOBJECT_PROJECTED_LAYER_SHADER));
        ScopedShaderDefinitionLockR pdefShaderCookie(GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_COOKIE));
        ScopedShaderDefinitionLockR pdefShaderPrepass(GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_PREPASS));

        ScopedShaderDefinitionLockR pdefShaderShadowStencilBounds(GlobalSNOGet(GLOBALSNO_SHADER_SHADOW_STENCIL_BOUNDS));



        ScopedShaderDefinitionLockR pdefShaderSceneFadeZPass(GlobalSNOGet(GLOBALSNO_SCENE_FADE_ZPASS));

        for (int32 i = 0; i < nVisibleSceneCount; i++)
        {
            ThreadedSceneInfo & info = arThreadedSceneInfo[i];
            const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
            const GeoSet & tGeoSet = info.pdefAppearance->tStructure.tGeoSets[eGeoSetType];
            const SubObject * ptSubObjects = tGeoSet.ptSubObjects;
            CSceneSubObject * ptSceneSubObjects = info.ptScene->pSceneSubobjects;

            for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; nSubObjectIndex++)
            {
                // Inference: stackInfo (3392) and the unused ptSubObjects/ptSceneSubObjects (3385/3386)
                // have no instructions; their single-threaded use is not recoverable from this instance.
                ThreadedSubObjectInfo stackInfo;










                ThreadedSubObjectInfo & subObjectInfo = info.pSubObjectInfo[nSubObjectIndex];

                CSceneSubObject & tCSceneSubObject = info.ptScene->pSceneSubobjects[nSubObjectIndex];
                if (!tCSceneSubObject.fVisible)
                {
                    if (tCSceneSubObject.idSurfaceEffect != PARTICLESYSTEMID_INVALID)
                    {
                        ParticleSystemDelete(tCSceneSubObject.idSurfaceEffect);
                        tCSceneSubObject.idSurfaceEffect = PARTICLESYSTEMID_INVALID;
                    }
                }
                else
                {
                    const SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
                    if (tSubObject.snoSurface != SNO_NONE)
                    {
                        sUpdateSurfaceEffects(info.ptScene, tCSceneSubObject, nSubObjectIndex, tSubObject.snoSurface);
                    }
                }

                if (tCSceneSubObject.fVisible || tCSceneSubObject.fCastsShadow)
                {
                    sCScenesRenderSubObject(
                        tAsyncRVData.dispList,
                        tAsyncRVData.dispListProjected,
                        info.ptScene,
                        cam,
                        tCSceneSubObject,
                        info.pdefAppearance,
                        nSubObjectIndex,
                        subObjectInfo.nLightCount,
                        subObjectInfo.pArrayLights,
                        pdefShaderPrepass,
                        pdefShaderProjectedLayer,
                        pdefShaderCookie,
                        pdefShaderShadowStencilBounds,
                        pdefShaderSceneFadeZPass);
                }
            }

            gptGroupDefAppearance->UnlockR((void *&)info.pdefAppearance);
        }
    }
}















// NON_MATCHING: 4688/4696 bytes; fade tests now fade-group-first (2319/2815/2852 columns); 2906 still bitwise in ours (original: fVisible, alpha, saved fade group) and subobjects indexed by offset.
void CScenesRenderUpdate(
    RWindow & tRWindow,
    const float flDeltaTime,
    const float flClientTime)
{



    sCScenesUpdate<true>(tRWindow, flDeltaTime, flClientTime);



}




static BOOL sSceneIsVisible(const Camera & cam, CScene * ptCScene)
{


    SWorldID idSWorld = CGameGetCurrentSWorldID();

    if (ptCScene->tSCD.idSWorld != idSWorld)
    {
        return FALSE;
    }

    AABB aabbBounds;
    SceneGetMarkerSetAABBInWorldSpace(ptCScene->tSCD, aabbBounds);

    return CameraComputeAABBVisibility(cam, aabbBounds);
}




BOOL SceneIsVisible(CSceneID idCScene)
{

    CScene * ptCScene = CSceneGet(idCScene);
    const Camera & cam = CameraManagerGetCamera(CAMERA_CURRENT);
    return sSceneIsVisible(cam, ptCScene);
}































BOOL CSceneDataArrayIterate(TDataArrayIterator<CScene> & tIterator, const CGame & tCGame, CSceneDataArrayIterateMode eIterateMode)
{






    if (!tCGame.ptCScenes)
    {
        return FALSE;
    }




    CScene * ptCScene = tCGame.ptCScenes->Iterate(tIterator);










    return ptCScene != NULL;
}

CScene * CSceneGetFromSSceneID(SSceneID idSScene)
{

    CGame & tCGame = CGameGet();

    CSceneGlobals * ptCSceneGlobals = tCGame.ptCSceneGlobals;

    if (!ptCSceneGlobals)
    {
        return NULL;
    }


    CSceneID idCScene;
    BOOL bResult = ptCSceneGlobals->tSSceneIDToCSceneIDMap.Lookup(idSScene, idCScene);
    if (!bResult)
    {
        return NULL;
    }



    CScene * ptCScene = CSceneGet(idCScene);


    return ptCScene;
}











































void CSceneReveal(const RevealSceneMessage & tRevealSceneMessage)
{

    if (CGameLoadingScreenIsEnabled())
    {

        SNOPreload::WaitForClientScene(tRevealSceneMessage.snoScene);
    }




    WorldCommonData * ptWCD = WorldTryGetCommonData(tRevealSceneMessage.idSWorld);
    if (!ptWCD)
    {
        return;
    }







    CScene * ptCScene = CSceneGetFromSSceneID(tRevealSceneMessage.idSScene);
    if (!ptCScene)
    {



        WorldCacheSceneValues(tRevealSceneMessage.snoScene, const_cast<SceneCachedValues &>(tRevealSceneMessage.tSceneSpec.tCachedValues));

        CSceneID idCScene = CSceneAdd(
            tRevealSceneMessage.snoScene,
            tRevealSceneMessage.idSScene,
            tRevealSceneMessage.tSceneSpec,
            tRevealSceneMessage.idSWorld,
            tRevealSceneMessage.transform,
            tRevealSceneMessage.idSSceneParent,
            tRevealSceneMessage.snoSceneGroup,
            FALSE,
            tRevealSceneMessage.bIsVista,
            RENDERCONTEXT_DEFAULT,
            tRevealSceneMessage.snoActors,
            tRevealSceneMessage.uActorCount);

        CScene * ptNewCScene = CSceneTryToGet(idCScene);
        if (ptNewCScene)
        {


            ptNewCScene->nRefCount = 1;



            for (uint32 i = 0; i < tRevealSceneMessage.uLabelCount; i++)
            {
                ptNewCScene->tSCD.listAppliedLabels.AddTail_Tracked(tRevealSceneMessage.arAppliedLabels[i]);
            }
        }
    }
    else
    {

        ptCScene->nRefCount++;

        if (ptCScene->dwFlags & (1 << CSCENE_DELETE_AT_END_OF_UPDATE_BIT))
        {





            ptCScene->dwFlags &= ~(1 << CSCENE_DELETE_AT_END_OF_UPDATE_BIT);

            CSceneGlobals * ptCSceneGlobals = CGameGet().ptCSceneGlobals;
            POS pos = ptCSceneGlobals->listScenesFlaggedForDeletion.Find(ptCScene->id);
            if (pos)
            {
                ptCSceneGlobals->listScenesFlaggedForDeletion.RemoveAt(pos);
            }



            SNOPreload::AddClientSceneBySNO(ptCScene->tSCD.snoScene);

            SNOPreload::AddClientWeatherBySNO(ptCScene->tSCD.tSceneSpec.snoWeather);

            for (uint32 i = 0; i < ptCScene->uActorCount; i++)
            {
                SNO snoActor = ptCScene->snoActors[i];
                SNOPreload::AddClientActorBySNO(snoActor);
            }



            ScopedAppearanceDefinitionLockR pdefAppearance(ptCScene->snoAppearance);
            sPreloadAllSubObjectTextures(ptCScene, pdefAppearance);
        }
    }




}




static void sCSceneFlagForDeletion(const CSceneID idCScene)
{

    CScene * ptCScene = CSceneTryToGet(idCScene);
    CSceneGlobals * ptCSceneGlobals = CGameGet().ptCSceneGlobals;
    if (ptCScene)
    {

        ptCSceneGlobals->listScenesFlaggedForDeletion.AddTail_Tracked(idCScene);
        ptCScene->dwFlags |= (1 << CSCENE_DELETE_AT_END_OF_UPDATE_BIT);

        ScopedAppearanceDefinitionLockR pdefAppearance(ptCScene->snoAppearance);
        sUnloadAllSubObjectTextures(ptCScene, pdefAppearance);


        for (uint32 i = 0; i < ptCScene->uActorCount; i++)
        {
            SNO snoActor = ptCScene->snoActors[i];
            SNOPreload::RemoveClientActorBySNO(snoActor);
        }

        SNOPreload::RemoveClientSceneBySNO(ptCScene->tSCD.snoScene);

        SNOPreload::RemoveClientWeatherBySNO(ptCScene->tSCD.tSceneSpec.snoWeather);

        for (uint32 i = 0; i < ptCScene->uActorCount; i++)
        {
            SNO snoActor = ptCScene->snoActors[i];
            SNOPreload::RemoveClientActorBySNO(snoActor);
        }
    }
}



void CScenesDeleteAllFlagged()
{

    CSceneGlobals * ptCSceneGlobals = CGameGet().ptCSceneGlobals;

    while (!ptCSceneGlobals->listScenesFlaggedForDeletion.IsEmpty())
    {
        CSceneID idCScene = ptCSceneGlobals->listScenesFlaggedForDeletion.GetHead();
        ptCSceneGlobals->listScenesFlaggedForDeletion.RemoveHead();

        CSceneDelete(idCScene);
    }
}



void CSceneDestroy(const DestroySceneMessage & tDestroySceneMessage)
{


    CScene * ptCScene = CSceneGetFromSSceneID(tDestroySceneMessage.idSScene);
    if (ptCScene)
    {


        ptCScene->nRefCount--;
        if (ptCScene->nRefCount > 0)
        {
            return;
        }




        sCSceneFlagForDeletion(ptCScene->id);
    }
}


void CSceneSwap(const SwapSceneMessage & tSwapSceneMessage)
{


    RActorOnSceneSwap(tSwapSceneMessage.idSWorld, tSwapSceneMessage.idSSceneOld, tSwapSceneMessage.idSSceneNew);
}



static float sSceneRActorGetFade(
    RActor * ptRActor)
{
    float flAlphaGoal = 1.0f;
    // Inference: default alpha 1 and first intersecting subobject alpha 0 follow original branches.
    if (sgtAppGlobals.eDebugFadeMode == DFM_FADE_ALL)
    {
        flAlphaGoal = 0.0f;
    }
    else if (sgtAppGlobals.eDebugFadeMode == DFM_DEFAULT && CGameGlobalsGet().bFadeFrustumValid)
    {
        const Frustum & tFrustum = CGameGlobalsGet().tFadeFrustum;
        const WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
        if (Intersect::FrustumAndSphere(tFrustum, loc.wsBounds))
        {
            ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
            const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[GEOSET_HIGHPOLY];
            RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[GEOSET_HIGHPOLY];

            for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
            {
                const SubObject & tSubObject = tGeoSet.ptSubObjects[i];
                SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[i];

                if (sSubObjectShouldFade(pdefAppearance, ptRActor->mWorld, tFrustum, tSubObject, tSubObjectState))
                {
                    flAlphaGoal = 0.0f;
                    break;
                }
            }
        }
    }

    return flAlphaGoal;
}















float SceneSubObjectGetFade(RActor * ptRActor, SSceneID idSScene, int32 nSubObjectIndex)
{


    CScene * ptCScene = CSceneGetFromSSceneID(idSScene);

    if (!ptCScene)
    {
        return 1.0f;
    }


    nSubObjectIndex -= FADE_SUBOBJECT_BIAS;

    if (nSubObjectIndex < 0 ||
        nSubObjectIndex >= ptCScene->nNumSceneSubobjects)
    {
        return 1.0f;
    }



    CSceneSubObject & tCSceneSubObject = ptCScene->pSceneSubobjects[nSubObjectIndex];







    if (ptRActor)
    {





        SNO snoAppearance = ptCScene->snoAppearance;

        ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
        const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
        const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];
        const SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
        CSceneGlobals * ptCSceneGlobals = CGameGet().ptCSceneGlobals;
        if (!ptCSceneGlobals->fFadeGroup[tSubObject.nFadeGroup] &&
            !ptCSceneGlobals->fFadeGroupRActors[tSubObject.nFadeGroup])
        {
            float flAlphaGoal = sSceneRActorGetFade(ptRActor);
            if (flAlphaGoal != 1.0f)
            {
                ptCSceneGlobals->fFadeGroupRActors[tSubObject.nFadeGroup] = TRUE;
            }
        }
    }

    return tCSceneSubObject.flAlpha;
}




float SceneRActorGetFade(RActor * ptRActor)
{

    float flAlphaGoal = sSceneRActorGetFade(ptRActor);

    if (flAlphaGoal < ptRActor->flAlpha)
    {
        flAlphaGoal = Math::Max(ptRActor->flAlpha - ALPHA_FADE_OUT_RATE, flAlphaGoal);
    }
    else if (ptRActor->flAlpha < flAlphaGoal)
    {
        flAlphaGoal = Math::Min(ptRActor->flAlpha + ALPHA_FADE_IN_RATE, flAlphaGoal);
    }

    return flAlphaGoal;
}




int32 SceneGetASubObjectIndexFromFadeGroup(SSceneID idSScene, int32 nFadeGroup)
{


    if (nFadeGroup == FADE_DONT_FADE)
    {
        return -1;
    }





    CScene * ptCScene = CSceneGetFromSSceneID(idSScene);
    if (!ptCScene)
    {
        return -1;
    }








    SNO snoAppearance = ptCScene->snoAppearance;

    ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];

    int32 nSubObjectIndex = -1;
    for (int32 nSubObject = 0; nSubObject < tGeoSet.nSubObjectCount; nSubObject++)
    {
        CSceneSubObject & tCSceneSubObject = ptCScene->pSceneSubobjects[nSubObject];
        SubObjectState & tSubObjectState = tCSceneSubObject.tSubObjectState;


        BOOL fDoRender = tSubObjectState.dwFlags & 1;
        if (!fDoRender)
        {
            continue;
        }

        const SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObject];
        if (tSubObject.nFadeGroup == nFadeGroup)
        {
            nSubObjectIndex = nSubObject + FADE_SUBOBJECT_BIAS;
            break;
        }
    }
    return nSubObjectIndex;
}

static BOOL sGetInterpolatedHeight(
    CScene * ptCScene,
    const Vector3D & wp,
    float & flHeight)
{
    if (!ptCScene->ptSubgrids)
        return FALSE;

    // check that the point is inside this scene's subgrids

    AABB aabb = ptCScene->tSubgridAABB;
    int32 nSubgridsX = ptCScene->nSubgridsX;

    Vector3D wpAABBMin = aabb.wp - aabb.wvExt;
    Vector3D wpAABBMax = aabb.wp + aabb.wvExt;


    if (wp.x < wpAABBMin.x ||
        wp.y < wpAABBMin.y || wp.x >= wpAABBMax.x || wp.y >= wpAABBMax.y)
        return FALSE;

    // check the child scenes first






    const SceneChildIDListPooled & tChildSceneIDs = ptCScene->tSCD.tChildSceneIDs;
    if (tChildSceneIDs.Count() > 0)
    {
        POS pos = tChildSceneIDs.GetHeadPosition();
        while (pos)
        {
            SSceneID idChildScene = tChildSceneIDs.GetNext(pos);

            CScene * ptChildScene = CSceneGetFromSSceneID(idChildScene);
            if (ptChildScene)
            {
                if (sGetInterpolatedHeight(ptChildScene, wp, flHeight))
                    return TRUE;
            }
        }
    }

    // find the subgrid
    int32 nSubgridX = (int32)((wp.x - wpAABBMin.x) / SUBGRID_WIDTH);
    int32 nSubgridY = (int32)((wp.y - wpAABBMin.y) / SUBGRID_WIDTH);

    ClutterSubgrid & tSceneSubgrid = ptCScene->ptSubgrids[nSubgridX + nSubgridY * nSubgridsX];
    ScopedAppearanceDefinitionLockR pdefAppearance(ptCScene->snoAppearance);
    if (!pdefAppearance->ptSubgrids)
        return FALSE;



    AppearanceSamplingSubgrid & tSubgrid = pdefAppearance->ptSubgrids[nSubgridX + nSubgridY * nSubgridsX];
    if (!tSubgrid.ptSamples)
        return FALSE;

    // find the sample


    float fSubgridSampleX = (wp.x - tSceneSubgrid.wpBase.x) / DISTANCE_BETWEEN_CLUTTER_SAMPLES;
    float fSubgridSampleY = (wp.y - tSceneSubgrid.wpBase.y) / DISTANCE_BETWEEN_CLUTTER_SAMPLES;

    int32 nSampleX = (int32)fSubgridSampleX;
    int32 nSampleY = (int32)fSubgridSampleY;

    float fSampleXPercent = fSubgridSampleX - nSampleX;
    float fSampleYPercent = fSubgridSampleY - nSampleY;

    float fFirstWeightY = 1.0f - fabsf(fSampleYPercent - 0.5f);
    int32 nFirstYSample = nSampleY;
    if (fSampleYPercent < 0.5f)
    {
        // Inference: 1.0f - weight is row 4093 col 30 (indent 8); the ++ leaves no row of its own.
        fFirstWeightY = 1.0f - fFirstWeightY;
    }
    else nFirstYSample++;
    float fFirstWeightX = 1.0f - fabsf(fSampleXPercent - 0.5f);
    int32 nFirstXSample = nSampleX;
    if (fSampleXPercent < 0.5f)
    {
        // Inference: as above, row 4101 col 30.
        fFirstWeightX = 1.0f - fFirstWeightX;
    }
    else nFirstXSample++;
    float flZ = 0.0f;
    int32 nValidCount = 0;
    float flTotalWeight = 0.0f;
    // Inference: nSamplesY before nSamplesX from the original factoring (PinY + nLookIndex*nSamplesY) * nSamplesX; columns cannot tell.
    int32 nLookOffset = ptCScene->nLookIndex * tSubgrid.nSamplesY * tSubgrid.nSamplesX;

    float flMinZ = pdefAppearance->flMinZ;
    float flRangeZ = pdefAppearance->flMaxZ - flMinZ;

    float fWeightY = fFirstWeightY;
    for (int32 dwYDelta = 0; dwYDelta < 2; dwYDelta++)
    {
        int32 ysamp; // decl 4116 precedes nSubSampleY 4117 in the DWARF; the - 1 before dwYDelta keeps the original's unfolded n-1 < 0 Pin test (inference)
        int32 nSubSampleY = Math::Pin(nFirstYSample - 1 + dwYDelta, 0, tSubgrid.nSamplesY - 1);
        ysamp = nLookOffset + nSubSampleY * tSubgrid.nSamplesX;
        float fWeightX = fFirstWeightX;
        for (int32 dwXDelta = 0; dwXDelta < 2; dwXDelta++)
        {
            int32 xsamp;
            int32 nSubSampleX = Math::Pin(nFirstXSample - 1 + dwXDelta, 0, tSubgrid.nSamplesX - 1);
            xsamp = ysamp + nSubSampleX;
            AppearanceSample & tSample = tSubgrid.ptSamples[xsamp];
            float fWeight = fWeightY * fWeightX;

            if (tSample.tClutterDensityMaskAndColor.dwMask & (1 << APPEARANCE_SAMPLE_SURFACE_IS_VALID_BIT))
            {
                nValidCount++;
                flZ += fWeight * (flMinZ + flRangeZ * (tSample.tWalkableZFractionAndSurface.dwZFraction / 255.0f));
                flTotalWeight += fWeight;
            }

            fWeightX = 1.0f - fWeightX;
        }

        fWeightY = 1.0f - fWeightY;
    }

    if (nValidCount > 0)
    {
        if (nValidCount != 4)
        {

            flZ /= flTotalWeight;
        }
        // Columns 4143:13/4146:17/4149:34/4154:22 and the flTempHeight lexical block put this tail inside if (nValidCount > 0).
        float flTempHeight = flZ + ptCScene->tSCD.transformWorld.wp.z;

        // Original compares with wp.z + 100.0f first (fcmp mi, then fccmp gt); columns 31/33/42/65 fit either order.
        if (flTempHeight < wp.z + 100.0f && flTempHeight > wp.z - 100.0f)
        {
            flHeight = flTempHeight;
            return TRUE;
        }
    }

    return FALSE;
}

static BOOL sGetInterpolatedColor(
    CScene * ptCScene,
    const Vector3D & wp,
    RGBAColor & rgbaColor)
{
    if (!ptCScene->ptSubgrids)
        return FALSE;



    AABB aabb = ptCScene->tSubgridAABB;
    int32 nSubgridsX = ptCScene->nSubgridsX;

    Vector3D wpAABBMin = aabb.wp - aabb.wvExt;
    Vector3D wpAABBMax = aabb.wp + aabb.wvExt;


    if (wp.x < wpAABBMin.x || wp.y < wpAABBMin.y ||
        wp.x >= wpAABBMax.x || wp.y >= wpAABBMax.y)
        return FALSE;






    const SceneChildIDListPooled & tChildSceneIDs = ptCScene->tSCD.tChildSceneIDs;
    if (tChildSceneIDs.Count() > 0)
    {
        POS pos = tChildSceneIDs.GetHeadPosition();
        while (pos)
        {
            SSceneID idChildScene = tChildSceneIDs.GetNext(pos);

            CScene * ptChildScene = CSceneGetFromSSceneID(idChildScene);
            if (ptChildScene && sGetInterpolatedColor(ptChildScene, wp, rgbaColor))
                return TRUE;
        }
    }







    int32 nSubgridX = (int32)((wp.x - wpAABBMin.x) / SUBGRID_WIDTH);
    int32 nSubgridY = (int32)((wp.y - wpAABBMin.y) / SUBGRID_WIDTH);

    ClutterSubgrid & tSceneSubgrid = ptCScene->ptSubgrids[nSubgridX + nSubgridY * nSubgridsX];
    ScopedAppearanceDefinitionLockR pdefAppearance(ptCScene->snoAppearance);
    if (!pdefAppearance->ptSubgrids)
        return FALSE;



    AppearanceSamplingSubgrid & tSubgrid = pdefAppearance->ptSubgrids[nSubgridX + nSubgridY * nSubgridsX];
    if (!tSubgrid.ptSamples)
        return FALSE;






    float fSubgridSampleX = (wp.x - tSceneSubgrid.wpBase.x) / DISTANCE_BETWEEN_CLUTTER_SAMPLES;
    float fSubgridSampleY = (wp.y - tSceneSubgrid.wpBase.y) / DISTANCE_BETWEEN_CLUTTER_SAMPLES;

    int32 nSampleX = (int32)fSubgridSampleX;
    int32 nSampleY = (int32)fSubgridSampleY;

    float fSampleXPercent = fSubgridSampleX - nSampleX;
    float fSampleYPercent = fSubgridSampleY - nSampleY;

    float fFirstWeightY = 1.0f - Math::Abs(fSampleYPercent - 0.5f);
    int32 nFirstYSample = nSampleY;
    if (fSampleYPercent < 0.5f)
        fFirstWeightY = 1.0f - fFirstWeightY;
    else
        ++nFirstYSample;


    float fFirstWeightX = 1.0f - Math::Abs(fSampleXPercent - 0.5f);
    int32 nFirstXSample = nSampleX;
    if (fSampleXPercent < 0.5f)
        fFirstWeightX = 1.0f - fFirstWeightX;
    else
        ++nFirstXSample;

    // Inference: 8-bit fixed-point weights and packed 7/7/7/3 color channels follow original shifts; original converts with fcvtzu (unsigned).
    uint32 dwFirstWeightX = (uint32)(fFirstWeightX * 256.0f);
    uint32 dwFirstWeightY = (uint32)(fFirstWeightY * 256.0f);

    uint32 a = 0;
    uint32 r = 0;
    uint32 g = 0;
    uint32 b = 0;
    int32 nValidCount = 0;
    uint32 dwTotalWeight = 0;

    int32 nLookOffset = ptCScene->nLookIndex * tSubgrid.nSamplesY * tSubgrid.nSamplesX;

    uint32 dwWeightY = dwFirstWeightY;
    for (int32 dwYDelta = 0; dwYDelta < 2; ++dwYDelta)
    {
        int32 ysamp; // Inference: - 1 before dwYDelta keeps the original unfolded n-1 < 0 Pin test, as in sGetInterpolatedHeight.
        int32 nSubSampleY = Math::Pin(nFirstYSample - 1 + dwYDelta, 0, tSubgrid.nSamplesY - 1);
        ysamp = nLookOffset + nSubSampleY * tSubgrid.nSamplesX;
        uint32 dwWeightX = dwFirstWeightX;
        for (int32 dwXDelta = 0; dwXDelta < 2; ++dwXDelta)
        {
            int32 xsamp;
            int32 nSubSampleX = Math::Pin(nFirstXSample - 1 + dwXDelta, 0, tSubgrid.nSamplesX - 1);
            xsamp = ysamp + nSubSampleX;
            AppearanceSample & tSample = tSubgrid.ptSamples[xsamp];
            uint32 dwWeight = dwWeightY * dwWeightX;

            if (tSample.tClutterDensityMaskAndColor.dwMask & (1 << APPEARANCE_SAMPLE_COLOR_IS_VALID_BIT))
            {
                ++nValidCount;
                a += dwWeight * ((tSample.tClutterDensityMaskAndColor.dwColor >> 21) << 5);
                r += dwWeight * (((tSample.tClutterDensityMaskAndColor.dwColor >> 14) & 127) << 1);
                g += dwWeight * (((tSample.tClutterDensityMaskAndColor.dwColor >> 7) & 127) << 1);
                b += dwWeight * ((tSample.tClutterDensityMaskAndColor.dwColor & 127) << 1);
                dwTotalWeight += dwWeight;
            }

            dwWeightX = 256 - dwWeightX;
        }

        dwWeightY = 256 - dwWeightY;
    }

    if (!dwTotalWeight)
        return FALSE;
    if (nValidCount != 4)
    {

        a /= dwTotalWeight;
        r /= dwTotalWeight;
        g /= dwTotalWeight;
        b /= dwTotalWeight;
    }
    else
    {
        a >>= 16;
        r >>= 16;
        g >>= 16;
        b >>= 16;
    }

    rgbaColor.a = Math::Min(a, 255u);
    rgbaColor.r = Math::Min(r, 255u);
    rgbaColor.g = Math::Min(g, 255u);
    rgbaColor.b = Math::Min(b, 255u);
    return TRUE;
}




static BOOL sGetInterpolatedSurface(
    CScene * ptCScene,
    const Vector3D & wp,
    SNO & snoSurface)
{
    if (!ptCScene->ptSubgrids)
        return FALSE;

    // check that the point is inside this scene's subgrids

    AABB aabb = ptCScene->tSubgridAABB;
    int32 nSubgridsX = ptCScene->nSubgridsX;

    Vector3D wpAABBMin = aabb.wp - aabb.wvExt;
    Vector3D wpAABBMax = aabb.wp + aabb.wvExt;


    if (wp.x < wpAABBMin.x ||
        wp.y < wpAABBMin.y || wp.x >= wpAABBMax.x || wp.y >= wpAABBMax.y)
        return FALSE;

    // check the child scenes first






    const SceneChildIDListPooled & tChildSceneIDs = ptCScene->tSCD.tChildSceneIDs;
    if (tChildSceneIDs.Count() > 0)
    {
        POS pos = tChildSceneIDs.GetHeadPosition();
        while (pos)
        {
            SSceneID idChildScene = tChildSceneIDs.GetNext(pos);

            CScene * ptChildScene = CSceneGetFromSSceneID(idChildScene);
            if (ptChildScene)
            {
                if (sGetInterpolatedSurface(ptChildScene, wp, snoSurface))
                    return TRUE;
            }
        }
    }

    // find the subgrid
    // ...

    int32 nSubgridX = (int32)((wp.x - wpAABBMin.x) / SUBGRID_WIDTH);
    int32 nSubgridY = (int32)((wp.y - wpAABBMin.y) / SUBGRID_WIDTH);

    ClutterSubgrid & tSceneSubgrid = ptCScene->ptSubgrids[nSubgridX + nSubgridY * nSubgridsX];
    ScopedAppearanceDefinitionLockR pdefAppearance(ptCScene->snoAppearance);
    if (!pdefAppearance->ptSubgrids)
        return FALSE;

    AppearanceSamplingSubgrid & tSubgrid = pdefAppearance->ptSubgrids[nSubgridX + nSubgridY * nSubgridsX];

    if (!tSubgrid.ptSamples)
        return FALSE;

    // find the sample
    // ...
    // ...

    float fSubgridSampleX = (wp.x - tSceneSubgrid.wpBase.x) / DISTANCE_BETWEEN_CLUTTER_SAMPLES;
    float fSubgridSampleY = (wp.y - tSceneSubgrid.wpBase.y) / DISTANCE_BETWEEN_CLUTTER_SAMPLES;

    int32 nSampleX = (int32)fSubgridSampleX;
    int32 nSampleY = (int32)fSubgridSampleY;

    int32 nLookOffset = ptCScene->nLookIndex * tSubgrid.nSamplesX * tSubgrid.nSamplesY;

    const AppearanceSample & tSample = tSubgrid.ptSamples[nLookOffset + nSampleY * tSubgrid.nSamplesX + nSampleX];
    if (!(tSample.tClutterDensityMaskAndColor.dwMask & (1 << APPEARANCE_SAMPLE_SURFACE_IS_VALID_BIT)))
        return FALSE;

    snoSurface = tSample.tWalkableZFractionAndSurface.snoSurface;
    return snoSurface != SNO_NONE;
}

static BOOL sGetInterpolatedNormal(
    CScene * ptCScene,
    const Vector3D & wp,
    Vector3D & wvFloorNormal)
{
    if (!ptCScene->ptSubgrids)
        return FALSE;



    AABB aabb = ptCScene->tSubgridAABB;
    int32 nSubgridsX = ptCScene->nSubgridsX;

    Vector3D wpAABBMin = aabb.wp - aabb.wvExt;
    Vector3D wpAABBMax = aabb.wp + aabb.wvExt;


    if (wp.x < wpAABBMin.x || wp.y < wpAABBMin.y ||
        wp.x >= wpAABBMax.x || wp.y >= wpAABBMax.y)
        return FALSE;






    const SceneChildIDListPooled & tChildSceneIDs = ptCScene->tSCD.tChildSceneIDs;
    if (tChildSceneIDs.Count() > 0)
    {
        POS pos = tChildSceneIDs.GetHeadPosition();
        while (pos)
        {
            SSceneID idChildScene = tChildSceneIDs.GetNext(pos);

            CScene * ptChildScene = CSceneGetFromSSceneID(idChildScene);
            if (ptChildScene && sGetInterpolatedNormal(ptChildScene, wp, wvFloorNormal))
                return TRUE;
        }
    }







    int32 nSubgridX = (int32)((wp.x - wpAABBMin.x) / SUBGRID_WIDTH);
    int32 nSubgridY = (int32)((wp.y - wpAABBMin.y) / SUBGRID_WIDTH);

    ClutterSubgrid & tSceneSubgrid = ptCScene->ptSubgrids[nSubgridX + nSubgridY * nSubgridsX];
    ScopedAppearanceDefinitionLockR pdefAppearance(ptCScene->snoAppearance);
    if (!pdefAppearance->ptSubgrids)
        return FALSE;



    AppearanceSamplingSubgrid & tSubgrid = pdefAppearance->ptSubgrids[nSubgridX + nSubgridY * nSubgridsX];
    if (!tSubgrid.ptSamples)
        return FALSE;




    float fSubgridSampleX = (wp.x - tSceneSubgrid.wpBase.x) / DISTANCE_BETWEEN_CLUTTER_SAMPLES;
    float fSubgridSampleY = (wp.y - tSceneSubgrid.wpBase.y) / DISTANCE_BETWEEN_CLUTTER_SAMPLES;

    int32 nSampleX = (int32)fSubgridSampleX;
    int32 nSampleY = (int32)fSubgridSampleY;

    float fSampleXPercent = fSubgridSampleX - nSampleX;
    float fSampleYPercent = fSubgridSampleY - nSampleY;

    int32 nBaseIndex;
    int32 nRightIndex;
    int32 nAtIndex;
    Vector3D vRight;
    Vector3D vAt;

    // Inference: triangle split and +/- sample-width directions follow original arithmetic.
    if (fSampleXPercent + fSampleYPercent < 1.0f)
    {

        nBaseIndex = nSampleY * tSubgrid.nSamplesX + nSampleX;
        nRightIndex = Math::Min(nSampleY + 1, tSubgrid.nSamplesY - 1) * tSubgrid.nSamplesX + nSampleX;
        nAtIndex = nSampleY * tSubgrid.nSamplesX + Math::Min(nSampleX + 1, tSubgrid.nSamplesX - 1);
        vRight.Set(0.0f, -DISTANCE_BETWEEN_CLUTTER_SAMPLES, 0.0f);
        vAt.Set(DISTANCE_BETWEEN_CLUTTER_SAMPLES, 0.0f, 0.0f);

    }
    else
    {

        nBaseIndex = Math::Min(nSampleY + 1, tSubgrid.nSamplesY - 1) * tSubgrid.nSamplesX + Math::Min(nSampleX + 1, tSubgrid.nSamplesX - 1);
        nRightIndex = Math::Min(nSampleY + 1, tSubgrid.nSamplesY - 1) * tSubgrid.nSamplesX + nSampleX;
        nAtIndex = nSampleY * tSubgrid.nSamplesX + Math::Min(nSampleX + 1, tSubgrid.nSamplesX - 1);
        vRight.Set(0.0f, DISTANCE_BETWEEN_CLUTTER_SAMPLES, 0.0f);
        vAt.Set(-DISTANCE_BETWEEN_CLUTTER_SAMPLES, 0.0f, 0.0f);
    }


    int32 nLookOffset = ptCScene->nLookIndex * tSubgrid.nSamplesY * tSubgrid.nSamplesX;

    float flMinZ = pdefAppearance->flMinZ;
    float flRangeZ = pdefAppearance->flMaxZ - flMinZ;

    const AppearanceSample & tBaseSample = tSubgrid.ptSamples[nLookOffset + nBaseIndex];
    const AppearanceSample & tRightSample = tSubgrid.ptSamples[nLookOffset + nRightIndex];
    const AppearanceSample & tAtSample = tSubgrid.ptSamples[nLookOffset + nAtIndex];

    float flBaseZ = flMinZ + flRangeZ * (tBaseSample.tWalkableZFractionAndSurface.dwZFraction / 255.0f);
    float flRightZ = flMinZ + flRangeZ * (tRightSample.tWalkableZFractionAndSurface.dwZFraction / 255.0f);
    float flAtZ = flMinZ + flRangeZ * (tAtSample.tWalkableZFractionAndSurface.dwZFraction / 255.0f);
    // Inference: original tests each flag as (word >> 5) & 1 (tst w8=1, wN, lsr #5), i.e. shift-and-1, not a != 0 mask test.
    BOOL fBaseValid = (tBaseSample.tClutterDensityMaskAndColor.dwMask >> APPEARANCE_SAMPLE_SURFACE_IS_VALID_BIT) & 1;
    BOOL fRightValid = (tRightSample.tClutterDensityMaskAndColor.dwMask >> APPEARANCE_SAMPLE_SURFACE_IS_VALID_BIT) & 1;
    BOOL fAtValid = (tAtSample.tClutterDensityMaskAndColor.dwMask >> APPEARANCE_SAMPLE_SURFACE_IS_VALID_BIT) & 1;

    if (!fBaseValid && !fRightValid && !fAtValid)
        return FALSE;


    // Inference: invalid samples inherit first valid height, following original conditional selects.
    float flValidZ = fBaseValid ? flBaseZ : (fRightValid ? flRightZ : flAtZ);

    if (!fRightValid)
        flRightZ = flValidZ;



    if (!fAtValid)
        flAtZ = flValidZ;



    if (!fBaseValid)
        flBaseZ = flValidZ;




    vRight.z = flRightZ - flBaseZ;
    vAt.z = flAtZ - flBaseZ;
    vRight.Normalize();
    vAt.Normalize();

    wvFloorNormal = Vector3D::CrossProduct(vRight, vAt);
    return TRUE;
}

BOOL CScenesGetFloorHeight(SWorldID idSWorld, const Vector3D & wp, float & fFloorHeight)
{



    TDataArrayIterator<CScene> tIterator;
    while (CSceneDataArrayIterate(tIterator, CGameGet(), SkipHolodeckScenes))
    {
        CScene * ptCScene = tIterator.ptElement;
        if (ptCScene->tSCD.idSWorld != idSWorld)
        {
            continue;
        }


        if (sGetInterpolatedHeight(ptCScene, wp, fFloorHeight))
        {
            return TRUE;
        }
    }

    return FALSE;
}


BOOL CScenesGetPositionColor(SWorldID idSWorld, const Vector3D & wp, RGBAColor & rgbaColor)
{



    BOOL fValid = FALSE;
    TDataArrayIterator<CScene> tIterator;
    while (CSceneDataArrayIterate(tIterator, CGameGet(), SkipHolodeckScenes))
    {
        CScene * ptCScene = tIterator.ptElement;
        if (ptCScene->tSCD.idSWorld != idSWorld)
        {
            continue;
        }


        if (sGetInterpolatedColor(ptCScene, wp, rgbaColor))
        {
            fValid = TRUE;
            if (rgbaColor.r + rgbaColor.g + rgbaColor.b != 0)
            {
                break;
            }
        }
    }

    return fValid;
}


BOOL CScenesGetSurface(SWorldID idSWorld, const Vector3D & wp, SNO & snoSurface)
{



    TDataArrayIterator<CScene> tIterator;
    while (CSceneDataArrayIterate(tIterator, CGameGet(), SkipHolodeckScenes))
    {
        CScene * ptCScene = tIterator.ptElement;
        if (ptCScene->tSCD.idSWorld != idSWorld)
        {
            continue;
        }


        if (sGetInterpolatedSurface(ptCScene, wp, snoSurface))
        {
            return TRUE;
        }
    }

    return FALSE;
}


BOOL CScenesGetFloorNormal(SWorldID idSWorld, const Vector3D & wp, Vector3D & wvFloorNormal)
{



    TDataArrayIterator<CScene> tIterator;
    while (CSceneDataArrayIterate(tIterator, CGameGet(), SkipHolodeckScenes))
    {
        CScene * ptCScene = tIterator.ptElement;
        if (ptCScene->tSCD.idSWorld != idSWorld)
        {
            continue;
        }


        if (sGetInterpolatedNormal(ptCScene, wp, wvFloorNormal))
        {
            return TRUE;
        }
    }

    return FALSE;
}





















BOOL CSceneGetPositionColor(SSceneID idSScene, const Vector3D & wp, RGBAColor & rgbaColor)
{




    CScene * ptCScene = CSceneGetFromSSceneID(idSScene);
    if (!ptCScene)
    {
        return FALSE;
    }


    return sGetInterpolatedColor(ptCScene, wp, rgbaColor) != FALSE;
}




BOOL CSceneGetSurface(SSceneID idSScene, const Vector3D & wp, SNO & snoSurface)
{




    CScene * ptCScene = CSceneGetFromSSceneID(idSScene);
    if (!ptCScene)
    {
        return FALSE;
    }


    return sGetInterpolatedSurface(ptCScene, wp, snoSurface) != FALSE;
}



























BOOL CSceneGetMeshLighting(const Vector3D & wp, RGBAColor & rgbaColor)
{











    return CScenesGetPositionColor(CGameGetCurrentSWorldID(), wp, rgbaColor);
}




















































































// NON_MATCHING: line 4855 select (0x71000d5288: and, orr, csel eq). Negated if/else and the !bIsVisible ternary get the and/orr order but csel ne; bIsVisible ternary keeps orr first; == FALSE ternary stops the select forming.
void CScenesUpdateEnvironmentRActors()
{

    if (CGameGetGameCommonData().GameModeGet() != GAMEMODE_PLAYING)
    {
        return;
    }

    const Camera & cam = CameraManagerGetCamera(CAMERA_CURRENT);

    CGame & tCGame = CGameGet();
    TDataArrayIterator<CScene> tIt;
    while (CSceneDataArrayIterate(tIt, tCGame, SkipHolodeckScenes))
    {
        CScene * ptCScene = tIt.ptElement;
        BOOL bWasVisible = ptCScene->dwFlags & (1 << CSCENE_LAST_VISIBLE_BIT);
        BOOL bIsVisible = sSceneIsVisible(cam, ptCScene);
        if (bIsVisible) ptCScene->dwFlags |= (1 << CSCENE_LAST_VISIBLE_BIT); else ptCScene->dwFlags &= ~(1 << CSCENE_LAST_VISIBLE_BIT);

        if (!bWasVisible && bIsVisible)
        {
            POS pos = ptCScene->listSceneRActors.GetHeadPosition();
            while (pos)
            {
                RActorID idRActor = (RActorID)ptCScene->listSceneRActors.GetNext(pos);
                RActor * ptRActor = RActorGet(idRActor);
                RActorCollisionInitialize(ptRActor);
            }
        }
        else if (bWasVisible && !bIsVisible)
        {
            POS pos = ptCScene->listSceneRActors.GetHeadPosition();
            while (pos)
            {
                RActorID idRActor = (RActorID)ptCScene->listSceneRActors.GetNext(pos);
                RActor * ptRActor = RActorGet(idRActor);
                RActorCollisionDispose(ptRActor);
            }
        }
    }
}



void SceneResetRActorFadeGroups()
{

    CSceneGlobals * ptCSceneGlobals = CGameGet().ptCSceneGlobals;
    SigmaMemoryClear(ptCSceneGlobals->fFadeGroupRActors, sizeof(ptCSceneGlobals->fFadeGroupRActors));
}
