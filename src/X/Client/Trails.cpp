#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Trails.h"
#include "TrailUpdate.h"
#include "TexAnimation.h"
#include "AttachmentParent.h"
#include "CGame.h"
#include "InterpolationTrack.h"
#include "RActors/RActors.h"
#include "Definitions/TrailDefinition.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxIB.h"
#include "Gfx/GfxVB.h"
#include "../Common/Game.h"
#include "../Common/GameConstants.h"
#include "../Sigma/Random.h"
#include "../Common/JobManager/JobManager.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include <new>

// X/Common/Game.cpp:1938
XMemoryAllocatorInterface * AppGetCurrentMemoryInterface();
extern VBFormatInfo cgtVBFormatInfo[NUM_VBFORMATS];  // Gfx/GfxVB.cpp
VBID GfxVBAddRef(const VBID vbid);  // Gfx/GfxVB.cpp
typedef uint16 GfxNdx;  // X/Client/Gfx/GfxAPI.h:56
BOOL GfxIBAllocTracked(const ResourceUsage eUsage, const uint32 dwCount, const BOOL fPurgeIfNecessary,
                       const GfxNdx * lpInitialData, IBID & ibid, LPCSTR szFile, int32 nLine, LPCSTR szDebugString);  // Gfx/GfxIB.cpp
void GfxIBFree(const IBID ibid);  // Gfx/GfxIB.cpp
void GfxStripIBAlloc(const ResourceUsage eUsage, const int32 nLength, const BOOL fPurgeIfNecessary, int32 & nIndexCount, IBID & ibid);  // Gfx/GfxIB.cpp

int32 MaterialGetTextureCount(const UberMaterial & tUberMaterial);  // Gfx/GfxMaterial.cpp
void TexAnimInitialize(const UberMaterial & tUberMaterial, TexAnimState * ptTexAnimStateList, RActor * ptRActor);  // TexAnimation.cpp

enum {
    TRAIL_IGNORE_OWNER_ALPHA = 0,
    TRAIL_PENDING_DELETION = 1,
    TRAIL_ALL_TEXTURE_LOADED = 2,
    TRAIL_EDGE_ALPHA = 3,
};

enum {
    MAX_TRAILS = 96,
    MAX_LINE_STRIP_LENGTH = 1024,
    MAX_CIRCLE_SEGMENTS = 12,
    MAX_CIRCLE_STRIP_LENGTH = 6656,
};

struct TrailGlobals {  // size 0x10
    VBID vbid;  // 0x00
    IBID ibid;  // 0x04
    TDataArray<Trail, int> * ptTrails;  // 0x08
};
static_assert(sizeof(TrailGlobals) == 0x10, "TrailGlobals");
static const float TRAIL_EPSILON = 0.0001f;
class TrailUpdateJob {  // size 0x18
public:
    TrailUpdateJob(TDataArrayIterator<Trail> tIt, int32 nCount);
    void Execute();
    static void JobFcn(void * pParam);
private:
    TDataArrayIterator<Trail> m_tIt;  // 0x00
    int32 m_nCount;  // 0x10
};
static_assert(sizeof(TrailUpdateJob) == 0x18, "TrailUpdateJob");

void TrailsDeleteAll();
void TrailDelete(TrailID id);
Trail * TrailTryToGet(TrailID id);
static void sTrailGfxInit(Trail * ptTrail);
static void sTrailGfxDispose(Trail * ptTrail);


#include "SubObjectGfx.h"
#include "RWindow.h"
#include "Camera/CameraManager.h"
#include "Definitions/PreferencesDefinition.h"
#include "Definitions/ShaderDefinition.h"
#include "Prefetch.h"
#include "Gfx/GfxShader.h"
#include "Gfx/GfxAPI.h"
static BOOL sTrailGetAlong(Trail *, int32, Vector3D &);

#include "Definitions/ShaderMapDefinition.h"
// Original indexed rendering APIs; definitions belong to Gfx, TexAnimation, and PostFX.
void * GfxVBLock(VBID, uint32, uint32 &);
void GfxVBUnlock(VBID);
void GfxWorldMatrixSet(const Matrix4x4 &);
void GfxShaderInit(const ShaderDefinition *, int32);
void GfxShaderSetTextureFactor(const RGBAColor &);
void GfxShaderSetIB(IBID, uint32);
void GfxShaderSetStream(StreamIndex, uint32, VBID);
void GfxShaderSetTextures(const UberMaterial &, const TexAnimState *, const char *, int32);
void GfxShaderDraw(DrawMode, uint32, uint32, uint32, uint32, uint32, PerfBin);
TexAnimState * TexAnimRenderAsyncCopy(int32, const TexAnimState *, const UberMaterial *, ShaderTex);
int32 PostFXGetNumReflectionPlanes();
UByte4 Vector3DNormalToUByte4(const Vector3D &);

static void sTrailGetPerpendicular(Trail *, int32, Vector3D &);
#include "Camera/Camera.h"


inline void TrailInterpState::Init(
    BOOL _bInfiniteDuration,
    float _flSystemDuration,
    float _flOutroDuration)
{
    flTime = 0.0f;
    bOutro = FALSE;
    flOutroTime = 0.0f;
    flOutroCrossFade = 0.0f;
    bInfiniteDuration = _bInfiniteDuration;
    flSystemDuration = Math::Max(_flSystemDuration, OO_TICKS_PER_SECOND);
    flOutroDuration = _flOutroDuration;
}




inline BOOL TrailInterpState::UpdateAndGetTime(
    InterpTime & tInterpTime,
    const float flDeltaTime)
{
    tInterpTime.Init();

    flTime = Math::Min(flTime + flDeltaTime, flSystemDuration);

    tInterpTime.flTime = flTime;
    tInterpTime.flScale = flSystemDuration;

    if (bOutro)
    {
        flOutroTime = Math::Min(flOutroTime + flDeltaTime, flOutroDuration);
        flOutroCrossFade = flOutroTime / flOutroDuration;
        flOutroCrossFade = Math::Pin(flOutroCrossFade, 0.0f, 1.0f);
    }
    else
    {
        flOutroCrossFade = 0.0f;
    }

    if (bInfiniteDuration)
    {
        if (bOutro)
        {
            if (flOutroTime >= flOutroDuration)
            {
                return FALSE;
            }

            tInterpTime.nTimeType = TICK_TIME_OUTRO_CROSSFADE;
            tInterpTime.flRatio = flOutroCrossFade;
            tInterpTime.flOutro = flOutroCrossFade;
        }
        else
        {
            tInterpTime.nTimeType = TICK_TIME_LOOPING;
        }
    }
    else
    {
        if (flTime >= flSystemDuration)
        {
            return FALSE;
        }
        tInterpTime.nTimeType = TICK_TIME_NON_LOOPING;
    }

    return TRUE;
}






void TrailsInitializeForGame() {
    CGame & tCGame = CGameGet();
    tCGame.ptTrailGlobals = (TrailGlobals *)SigmaMemoryNew(sizeof(TrailGlobals), 4, NULL, TRUE);
    tCGame.ptTrailGlobals->ptTrails = TDataArrayCreateTracked<Trail, int>("Trail", AppGetCurrentMemoryInterface(), MAX_TRAILS);
}









void TrailsDisposeForGame() {
    TrailsDeleteAll();

    TrailGlobals *& ptTrailGlobals = CGameGet().ptTrailGlobals;
    if (ptTrailGlobals->ptTrails) {
        TDataArrayDispose(ptTrailGlobals->ptTrails);
    }
    SigmaMemoryFree(ptTrailGlobals, NULL);
    ptTrailGlobals = NULL;
}









void TrailsGfxInit() {
    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    ptTrailGlobals->vbid = GfxVBAddRef(cgtVBFormatInfo[VBFORMAT_1TEX_L].vbidPooled);
    int32 nIndexCount;
    GfxStripIBAlloc(RESOURCEUSAGE_IMMUTABLE, MAX_LINE_STRIP_LENGTH, FALSE, nIndexCount, ptTrailGlobals->ibid);

    TDataArrayIterator<Trail> tIt;
    while (ptTrailGlobals->ptTrails->Iterate(tIt)) {
        Trail * ptTrail = tIt.ptElement;
        if (!(ptTrail->dwTrailFlags & (1 << TRAIL_PENDING_DELETION))) {
            sTrailGfxInit(ptTrail);
        }
    }
}

















void TrailsGfxDispose() {
    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    GfxVBRelease(ptTrailGlobals->vbid);

    TDataArrayIterator<Trail> tIt;
    while (ptTrailGlobals->ptTrails->Iterate(tIt)) {
        Trail * ptTrail = tIt.ptElement;
        if (!(ptTrail->dwTrailFlags & (1 << TRAIL_PENDING_DELETION))) {
            sTrailGfxDispose(ptTrail);
        }
    }

    GfxIBRelease(ptTrailGlobals->ibid);
}






























static void sTrailGfxInit(
    Trail * ptTrail)
{







    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;

    uint16 wordarray[MAX_CIRCLE_STRIP_LENGTH];

    switch (ptTrail->nTrailType)
    {
    case TRAIL_LINE:
        {
            ptTrail->ibid = ptTrailGlobals->ibid;
            break;
        }
    case TRAIL_CROSS:
        {
            int32 nIndexCount = ptTrail->nCurvePoints * 4 + 2;


            int32 nLinePoints = ptTrail->nCurvePoints * 2;
            for (int32 i = 0; i < nLinePoints; ++i)
            {

                wordarray[i] = (uint16)i;

                wordarray[nLinePoints + i + 2] = (uint16)(nLinePoints + i);
            }

            wordarray[nLinePoints] = (uint16)(nLinePoints - 1);
            wordarray[nLinePoints + 1] = (uint16)nLinePoints;

            GfxIBAllocTracked(RESOURCEUSAGE_IMMUTABLE, nIndexCount, FALSE, wordarray, ptTrail->ibid, "", 0, "");
            break;
        }

    case TRAIL_CIRCLE:
        {
            int32 nIndexCount = (ptTrail->nCurvePoints - 1) * (ptTrail->nCircleSegments + 1) * 2;


            int32 nArrayIndex = 0;
            for (int32 i = 0; i < ptTrail->nCurvePoints - 1; ++i)
            {
                int32 nOffsetIndex0 = i * ptTrail->nCircleSegments;
                int32 nOffsetIndex1 = (i + 1) * ptTrail->nCircleSegments;
                for (int32 j = 0; j < ptTrail->nCircleSegments; ++j)
                {
                    wordarray[nArrayIndex] = (uint16)(nOffsetIndex0 + j);
                    ++nArrayIndex;
                    wordarray[nArrayIndex] = (uint16)(nOffsetIndex1 + j);
                    ++nArrayIndex;
                }


                wordarray[nArrayIndex] = (uint16)nOffsetIndex0;
                ++nArrayIndex;
                wordarray[nArrayIndex] = (uint16)nOffsetIndex1;
                ++nArrayIndex;
            }

            GfxIBAllocTracked(RESOURCEUSAGE_IMMUTABLE, nIndexCount, FALSE, wordarray, ptTrail->ibid, "", 0, "");
            break;
        }
    default:
        {
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }
}




static void sTrailGfxDispose(Trail * ptTrail) {
    switch (ptTrail->nTrailType) {
    case TRAIL_LINE:
        break;
    case TRAIL_CROSS:
    case TRAIL_CIRCLE:
        GfxIBFree(ptTrail->ibid);
        break;
    default:
        DisplayInternalError(NULL, "", 379, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}






inline Trail * TrailGet(TrailID id) {
    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    return ptTrailGlobals->ptTrails->ElementGet(id);
}






Trail * TrailTryToGet(TrailID id) {
    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    return ptTrailGlobals->ptTrails->ElementTryToGet(id);
}






void TrailsDeleteAll() {
    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    if (ptTrailGlobals->ptTrails) {
        TDataArrayIterator<Trail> tIt;
        while (ptTrailGlobals->ptTrails->Iterate(tIt)) {
        Trail * ptTrail = tIt.ptElement;
            if (!(ptTrail->dwTrailFlags & (1 << TRAIL_PENDING_DELETION))) {
                TrailDelete(ptTrail->id);
            }
        }

        ptTrailGlobals->ptTrails->DeleteAll();
    }
}

















TrailID TrailAdd(
    const SNO snoTrail,
    const RActorID idRActor,
    const HardpointLink * ptHardpointLinks,
    const BOOL fIgnoreOwnerAlpha,
    const RenderContext eContext)
{
    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    TrailID id = ptTrailGlobals->ptTrails->ElementNew();
    if (id == TRAILID_INVALID)
    {
        return id;
    }

    Trail * ptTrail = TrailGet(id);


    ptTrail->dwLastFrameSubmitted = CGameAsyncGetDrawFrameCount() - 1;


    ScopedTrailDefinitionLockR pdefTrail(snoTrail);

    RNG & rng = CGameGlobalsGet().rngLocal;

    float flSystemDuration = (float)RandRange_Internal(rng, pdefTrail->tSystemDurationMin, pdefTrail->tSystemDurationDelta) * OO_TICKS_PER_SECOND;
    float flOutroDuration = (float)RandRange_Internal(rng, pdefTrail->tOutroDurationMin, pdefTrail->tOutroDurationDelta) * OO_TICKS_PER_SECOND;
    BOOL bInfiniteDuration = (pdefTrail->dwTrailDefFlags & (1 << TRAILDEF_INFINITE_DURATION_BIT)) != 0;

    ptTrail->tInterpState.Init(bInfiniteDuration, flSystemDuration, flOutroDuration);

    ptTrail->snoTrail = snoTrail;
    ptTrail->eContext = eContext;
    ptTrail->nLength = RandRange_Internal(rng, pdefTrail->nLength, pdefTrail->nLengthDelta);
    ptTrail->nLength = Math::Pin(ptTrail->nLength, 3, (int32)MAX_TRAIL_LENGTH);
    ptTrail->nCurvePoints = (ptTrail->nLength - 1) * 4;


    ptTrail->tTrailKnotPoints = new TrailKnotPoint[ptTrail->nLength];
    ptTrail->tTrailCurvePoints = new TrailCurvePoint[ptTrail->nCurvePoints];
    ptTrail->tTexAnimStateList = new TexAnimState[MaterialGetTextureCount(pdefTrail->tUberMaterial)];








    ptTrail->dwTrailFlags = 0;
    ptTrail->nTrailType = pdefTrail->nTrailType;
    ptTrail->nCircleSegments = Math::Pin(pdefTrail->nCircleSegments, 3, (int32)MAX_CIRCLE_SEGMENTS);
    ptTrail->wvLastAlong = cgwvZAxis;
    ptTrail->wvLastForward = cgwvZAxis;

    ptTrail->flColorOverTimeInterp = pdefTrail->pathColorOverTime.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptTrail->flColorOverLengthInterp = pdefTrail->pathColorOverLength.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptTrail->flAlphaOverTimeInterp = pdefTrail->pathAlphaOverTime.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptTrail->flAlphaOverLengthInterp = pdefTrail->pathAlphaOverLength.tHeader.GetInterpolantFromDistributionFunction(rng);

    if (fIgnoreOwnerAlpha)
    {
        ptTrail->dwTrailFlags |= (1 << TRAIL_IGNORE_OWNER_ALPHA);
    }

    if (pdefTrail->dwTrailDefFlags & (1 << TRAILDEF_EDGE_ALPHA)) ptTrail->dwTrailFlags |= (1 << TRAIL_EDGE_ALPHA);
    else ptTrail->dwTrailFlags &= ~(1 << TRAIL_EDGE_ALPHA);
    ptTrail->flCreateTime = CGameGetClientTime();
    ptTrail->flLastTimeBumped = ptTrail->flCreateTime - OO_TICKS_PER_SECOND;

    sTrailGfxInit(ptTrail);

    for (int32 i = 0; i < MAX_TRAIL_ATTACH_POINTS; i++)
    {
        ptTrail->tParentData[i].SetHardpoint(PARENT_RACTOR, idRActor, ptHardpointLinks[i].tHash);
    }

    TexAnimInitialize(pdefTrail->tUberMaterial, ptTrail->tTexAnimStateList, NULL);

    ptTrail->flTimeParentWarped = 0.0f;

    return id;
}





































static void sDeleteAsyncTrail(void * pParam, void *) {
    TrailDelete((TrailID)(intptr_t)pParam);
}







void TrailDelete(TrailID id) {
    Trail * ptTrail = TrailTryToGet(id);
    if (ptTrail == NULL) {
        return;
    }

    if (ptTrail->dwLastFrameSubmitted == CGameAsyncGetDrawFrameCount() &&
        CGameAsyncRenderRegisterDelayedCallback(sDeleteAsyncTrail, (void *)(intptr_t)id, NULL)) {
        ptTrail->dwTrailFlags |= (1 << TRAIL_PENDING_DELETION);
        return;
    }

    delete [] ptTrail->tTrailKnotPoints;
    delete [] ptTrail->tTrailCurvePoints;
    delete [] ptTrail->tTexAnimStateList;

    sTrailGfxDispose(ptTrail);

    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    ptTrailGlobals->ptTrails->ElementDelete(ptTrail);
}




















void TrailUpdatePropertiesFromParents(Trail * ptTrail) {
    Vector3D wp[MAX_TRAIL_ATTACH_POINTS];
    for (int32 i = 0; i < MAX_TRAIL_ATTACH_POINTS; i++) {
        if (!ptTrail->tParentData[i].ParentIsValid()) {
            return;
        }
        PRTransform transform;
        ptTrail->tParentData[i].GetTransform(transform);
        wp[i] = transform.wp;
    }

    ptTrail->tTrailKnotPoints[0].wpKnotPoint0 = wp[0];
    ptTrail->tTrailKnotPoints[0].wpKnotPoint1 = wp[1];

    float flNow = CGameGetClientTime();
    if (ptTrail->flCreateTime == flNow || ptTrail->flTimeParentWarped != 0.0f) {
        if (ptTrail->flTimeParentWarped != 0.0f && flNow > ptTrail->flTimeParentWarped) {
            ptTrail->flTimeParentWarped = 0.0f;
        }
        for (int32 i = 0; i < ptTrail->nLength; i++) {
            ptTrail->tTrailKnotPoints[i].wpKnotPoint0 = wp[0];
            ptTrail->tTrailKnotPoints[i].wpKnotPoint1 = wp[1];
        }
    }
}













uint32 TexAnimUpdate(const UberMaterial & tUberMaterial, const ObjectTexAnimParam * ptObjTexAnimParam, TexAnimState * ptTexAnimState, const int32 nTexAnimStateCount, float flDeltaTime, float flClientTime);  // TexAnimation.cpp

static void sTrailsClientUpdateOne(
    Trail * ptTrail,
    float flDeltaTime,
    float flClientTime)
{


    TrailUpdatePropertiesFromParents(ptTrail);

    const RActor * ptRActor = RActorTryToGet((RActorID)ptTrail->tParentData[0].m_idParent);
    if (ptRActor == NULL)
    {
        TrailDelete(ptTrail->id);
        return;
    }

    InterpTime tInterpTime;
    BOOL bStillGoing = ptTrail->tInterpState.UpdateAndGetTime(tInterpTime, flDeltaTime);
    if (!bStillGoing)
    {
        TrailDelete(ptTrail->id);
        return;
    }


    ScopedTrailDefinitionLockR pdefTrail(ptTrail->snoTrail);

    uint32 dwResultFlags = TexAnimUpdate(pdefTrail->tUberMaterial, NULL, ptTrail->tTexAnimStateList, NUM_SHADER_TEXTURES, flDeltaTime, flClientTime);

    if (dwResultFlags & pdefTrail->dwTrailDefFlags & (1 << TRAILDEF_DELETE_AT_ANIMATION_END_BIT))
    {

        TrailDelete(ptTrail->id);
        return;
    }

    while (flClientTime + flDeltaTime > ptTrail->flLastTimeBumped + OO_TICKS_PER_SECOND)
    {

        for (int32 i = ptTrail->nLength - 2; i >= 0; --i)
        {
            ptTrail->tTrailKnotPoints[i + 1].wpKnotPoint0 = ptTrail->tTrailKnotPoints[i].wpKnotPoint0;
            ptTrail->tTrailKnotPoints[i + 1].wpKnotPoint1 = ptTrail->tTrailKnotPoints[i].wpKnotPoint1;
        }

        ptTrail->flLastTimeBumped += OO_TICKS_PER_SECOND;
    }

    pdefTrail->pathColorOverTime.GetInterpolatedValueByTime(ptTrail->flColorOverTimeInterp, tInterpTime, ptTrail->rgbaColor);
    pdefTrail->pathAlphaOverTime.GetInterpolatedValueByTime(ptTrail->flAlphaOverTimeInterp, tInterpTime, ptTrail->flTrailAlpha);

    if (!(ptTrail->dwTrailFlags & (1 << TRAIL_IGNORE_OWNER_ALPHA)))
    {
        ptTrail->flTrailAlpha *= ptTrail->tParentData[0].GetAlpha();
    }

    if (ptTrail->flTrailAlpha < 1.0f - EPSILON)
    {
        ptTrail->rgbaColor.a = (uint8)Math::FloatRoundToInt(ptTrail->flTrailAlpha * 255.0f);
    }
}



// NON_MATCHING: 1164/1168 bytes. The inlined update uses a smaller stack frame and different spills.
void TrailsClientUpdate(
    float flDeltaTime)
{
    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    if (ptTrailGlobals->ptTrails->dwTotalCount == 0)
    {
        return;
    }


    float flClientTime = CGameGetClientTime();

    TDataArrayIterator<Trail> tIt;
    while (ptTrailGlobals->ptTrails->Iterate(tIt))
    {
        Trail * ptTrail = tIt.ptElement;


        if (!(ptTrail->dwTrailFlags & (1 << TRAIL_PENDING_DELETION)))
        {




            sTrailsClientUpdateOne(ptTrail, flDeltaTime, flClientTime);
        }
    }

}




static void sTrailsUpdateOne(Trail * ptTrail) {
    if (ptTrail->dwTrailFlags & (1 << TRAIL_PENDING_DELETION)) {
        return;
    }
    ScopedTrailDefinitionLockR pdefTrail(ptTrail->snoTrail);
    TrailUpdateParams tParams;
    tParams.FromTrailAndDefinition(ptTrail, pdefTrail);
    TrailUpdate(tParams);
}








inline TrailUpdateJob::TrailUpdateJob(TDataArrayIterator<Trail> tIt, int32 nCount) {
    m_tIt = tIt;
    m_nCount = nCount;
}





void TrailUpdateJob::Execute() {
    GameSetActiveGameCommonData(&CGameGet().tGCD, TRUE);
    int32 nCount = m_nCount;
    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    TDataArrayIterator<Trail> tIt = m_tIt;
    for (int32 i = 0; i < nCount; i++) {
        Trail * ptTrail = ptTrailGlobals->ptTrails->Iterate(tIt);
        sTrailsUpdateOne(ptTrail);
    }
}












void TrailUpdateJob::JobFcn(void * pParam) {
    ((TrailUpdateJob *)pParam)->Execute();
}









void TrailsUpdate() {
    char arJobBuffer[384];
    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    uint32 dwCount = ptTrailGlobals->ptTrails->dwTotalCount;
    if (dwCount == 0) {
        return;
    }

    int32 nJobIndex = 0;
    uint32 dwIncrement = dwCount / 16 + 1;
    TDataArrayIterator<Trail> tIt;
    for (uint32 i = 0; i < dwCount; i += dwIncrement) {
        void * pBuffer = &arJobBuffer[nJobIndex * sizeof(TrailUpdateJob)];
        TrailUpdateJob * ptJob = new (pBuffer) TrailUpdateJob(tIt, Math::Min(dwIncrement, dwCount - i));
        JobManager::Get()->PushJob(TrailUpdateJob::JobFcn, ptJob);
        for (uint32 j = 0; j < dwIncrement; j++) {
            ptTrailGlobals->ptTrails->Iterate(tIt);
        }
        nJobIndex++;
    }

    JobManager::Get()->WaitForIdle();
}

































void TrailsRenderUpdate(
    RWindow & tRWindow)
{


    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    if (!ptTrailGlobals->ptTrails->dwTotalCount)
        return;



    AsyncRWindowData & tAsyncRVData = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];

    const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());
    int32 nReflectionQuality = Prefs::GetVideoPrefs().GetReflectionQuality();

    TDataArrayIterator<Trail> tIt;
    while (ptTrailGlobals->ptTrails->Iterate(tIt))
    {
        Trail * ptTrail = tIt.ptElement;
        if (ptTrail->dwTrailFlags & (1 << TRAIL_PENDING_DELETION))
            continue;






        if (ptTrail->eContext != tAsyncRVData.dispList.GetRenderContext())
            continue;
        if (ptTrail->flTrailAlpha < EPSILON)
            continue;






        ScopedTrailDefinitionLockR pdefTrail(ptTrail->snoTrail);
        if (!(ptTrail->dwTrailFlags & (1 << TRAIL_ALL_TEXTURE_LOADED)))
        {

            int32 nCount = MaterialGetTextureCount(pdefTrail->tUberMaterial);
            const MaterialTextureEntry * pMatTexEntries = pdefTrail->tUberMaterial.ptMatTexList;

            BOOL fAnyTextureNotLoaded = FALSE;
            for (int32 i = 0; i < nCount; ++i, ++pMatTexEntries)
            {
                if (!Prefetch::AddToQueue(SNOName(44, pMatTexEntries->tMatTex.snoTex), 2))
                {
                    fAnyTextureNotLoaded = TRUE;
                    break;
                }
            }
            if (fAnyTextureNotLoaded)
                continue;
            ptTrail->dwTrailFlags |= (1 << TRAIL_ALL_TEXTURE_LOADED);
        }





        ScopedShaderDefinitionLockR pdefShader(ShaderMapGetDefaultShader(pdefTrail->tUberMaterial.snoShaderMap, TRUE, NULL));
        Renderable tRenderable(RENDERABLETYPE_TRAIL, ptTrail->id, PERFBIN_OTHER, pdefShader, pdefTrail->tUberMaterial.mat);





        RenderableHeader & tHeader = tRenderable.header;
        RenderableTrail & tDetails = tRenderable.details.tTrail;
        tDetails.snoTrail = ptTrail->snoTrail;
        tDetails.fEdgeAlpha = (ptTrail->dwTrailFlags & (1 << TRAIL_EDGE_ALPHA)) != 0;
        tDetails.ibid = ptTrail->ibid;
        tDetails.nCircleSegments = ptTrail->nCircleSegments;
        tDetails.nCurvePoints = ptTrail->nCurvePoints;
        tDetails.nTrailType = ptTrail->nTrailType;
        tDetails.ptCurvePoints = static_cast<TrailCurvePoint *>(CGameAsyncRenderGetMemory(ptTrail->nCurvePoints * sizeof(TrailCurvePoint)));
        SigmaMemoryCopy(tDetails.ptCurvePoints, ptTrail->tTrailCurvePoints, ptTrail->nCurvePoints * sizeof(TrailCurvePoint));
        ptTrail->dwLastFrameSubmitted = CGameAsyncGetUpdateFrameCount();
        if (ptTrail->nTrailType == TRAIL_CIRCLE)
        {
            tDetails.wvCurvePointAlong = static_cast<Vector3D *>(CGameAsyncRenderGetMemory(ptTrail->nCurvePoints * sizeof(Vector3D)));
            tDetails.wvCurvePointForward = static_cast<Vector3D *>(CGameAsyncRenderGetMemory(ptTrail->nCurvePoints * sizeof(Vector3D)));
            tDetails.wvCurvePointPerpendicular = NULL;
















            for (int32 i = 0; i < ptTrail->nCurvePoints; ++i)
            {
                if (!sTrailGetAlong(ptTrail, i, tDetails.wvCurvePointAlong[i]))
                {
                    tDetails.wvCurvePointForward[i] = cgwvZAxis;
                    continue;
                }

                TrailCurvePoint & tCurvePoint = ptTrail->tTrailCurvePoints[i];

                Vector3D wpCenter = (tCurvePoint.wpCurvePoint0 + tCurvePoint.wpCurvePoint1) * 0.5f;

                Vector3D wvAlong(tDetails.wvCurvePointAlong[i]);

                Vector3D wvUp = tCurvePoint.wpCurvePoint1 - wpCenter;



                Vector3D wvRight = Vector3D::CrossProduct(wvUp, wvAlong);
                Vector3D wvForward = Vector3D::CrossProduct(wvUp, wvRight);
                WorldDistance wdSegmentLength = wvForward.Normalize();


                if (wdSegmentLength < TRAIL_EPSILON)
                {
                    wvForward = ptTrail->wvLastForward;
                }

                WorldDistance wdDot = Vector3D::DotProduct(wvForward, ptTrail->wvLastForward);

                if (wdDot < 0.0f)
                {



                    wvForward = -wvForward;
                }

                tDetails.wvCurvePointForward[i] = wvForward;

                ptTrail->wvLastForward = wvForward;
            }
        }
        else
        {

            tDetails.wvCurvePointAlong = NULL;
            tDetails.wvCurvePointForward = NULL;
            tDetails.wvCurvePointPerpendicular = static_cast<Vector3D *>(CGameAsyncRenderGetMemory(ptTrail->nCurvePoints * sizeof(Vector3D)));




            for (int32 i = 0; i < ptTrail->nCurvePoints; ++i)
            {
                sTrailGetPerpendicular(ptTrail, i, tDetails.wvCurvePointPerpendicular[i]);
            }
        }


        int32 nTextureCount = MaterialGetTextureCount(pdefTrail->tUberMaterial);
        tDetails.arTexAnimStates = TexAnimRenderAsyncCopy(nTextureCount, ptTrail->tTexAnimStateList, &pdefTrail->tUberMaterial, SHADERTEX_NONE);




        const Vector3D & wpKnotPoint0 = ptTrail->tTrailKnotPoints[0].wpKnotPoint0;
        tHeader.SetDist(Vector3D::DistanceSquared(cam.cmState.transform.wp, wpKnotPoint0));
        tHeader.SetRectFromPoint(wpKnotPoint0);
        tHeader.rgbaColor = crgbaWhite;
        if (ptTrail->flTrailAlpha < 1.0f - EPSILON)
            tHeader.rgbaColor.a = Math::FloatRoundToInt(ptTrail->flTrailAlpha * 255.0f);
        tAsyncRVData.dispList.SubmitRenderable(tRenderable);
        if (nReflectionQuality > 0)
        {



            for (int i = 0; i < PostFXGetNumReflectionPlanes(); ++i)
            {
                tAsyncRVData.dispListReflections[i].SubmitRenderable(tRenderable);
            }
        }
    }
}




static uint8 sComputeEdgeAlpha(
    const Vector3D & wp,
    const Vector3D & wvNormal,
    const Vector3D & wpCam)
{
    Vector3D wvEye = wpCam - wp;
    wvEye.Normalize();

    float flAlpha = Vector3D::DotProduct(wvNormal, wvEye);
    flAlpha *= flAlpha;




    uint8 byteAlpha = Math::Pin(Math::FloatRoundToInt(flAlpha * 255.0f), 0, 255);
    return byteAlpha;
}




static BOOL sTrailGetAlong(
    Trail * ptTrail,
    int32 nCurvePoint,
    Vector3D & wvAlong)
{
    TrailCurvePoint & tCurvePoint = ptTrail->tTrailCurvePoints[nCurvePoint];
    Vector3D wpCenter = (tCurvePoint.wpCurvePoint0 + tCurvePoint.wpCurvePoint1) * 0.5f;
    if (nCurvePoint == 0)
    {



        for (int32 i = 1; i < ptTrail->nCurvePoints; ++i)
        {
            TrailCurvePoint & tCurvePointNext = ptTrail->tTrailCurvePoints[i];
            Vector3D wpCenterNext = (tCurvePointNext.wpCurvePoint0 + tCurvePointNext.wpCurvePoint1) * 0.5f;

            wvAlong = wpCenterNext - wpCenter;
            WorldDistance wdSegmentLength = wvAlong.Normalize();
            if (wdSegmentLength < TRAIL_EPSILON)
                continue;





            WorldDistance wdDot = Vector3D::DotProduct(wvAlong, ptTrail->wvLastAlong);
            if (wdDot < 0.0f)
            {




                wvAlong = -wvAlong;
            }

            ptTrail->wvLastAlong = wvAlong;
            return TRUE;
        }
        return FALSE;
    }
    else if (nCurvePoint < ptTrail->nCurvePoints - 1)
    {



        TrailCurvePoint & tCurvePointPrev = ptTrail->tTrailCurvePoints[nCurvePoint - 1];
        Vector3D wpCenterPrev = (tCurvePointPrev.wpCurvePoint0 + tCurvePointPrev.wpCurvePoint1) * 0.5f;

        TrailCurvePoint & tCurvePointNext = ptTrail->tTrailCurvePoints[nCurvePoint + 1];
        Vector3D wpCenterNext = (tCurvePointNext.wpCurvePoint0 + tCurvePointNext.wpCurvePoint1) * 0.5f;

        Vector3D wvForwardAlong = wpCenter - wpCenterPrev;

        Vector3D wvBackwardAlong = wpCenterNext - wpCenter;

        wvAlong = wvForwardAlong + wvBackwardAlong;
    }
    else
    {


        TrailCurvePoint & tCurvePointPrev = ptTrail->tTrailCurvePoints[nCurvePoint - 1];
        Vector3D wpCenterPrev = (tCurvePointPrev.wpCurvePoint0 + tCurvePointPrev.wpCurvePoint1) * 0.5f;

        wvAlong = wpCenter - wpCenterPrev;
    }
    return TRUE;
}





static void sTrailGetPerpendicular(
    Trail * ptTrail,
    int32 nCurvePoint,
    Vector3D & wvPerpendicular)
{
    Vector3D wvAlong;
    sTrailGetAlong(ptTrail, nCurvePoint, wvAlong);

    TrailCurvePoint & tCurvePoint = ptTrail->tTrailCurvePoints[nCurvePoint];

    Vector3D wvUp = tCurvePoint.wpCurvePoint1 - tCurvePoint.wpCurvePoint0;
    float flWidth = wvUp.Magnitude();
    wvPerpendicular = Vector3D::CrossProduct(wvAlong, wvUp);
    wvPerpendicular.Normalize();
    wvPerpendicular *= flWidth * 0.5f;
}






static void sTrailDrawLine(
    const RenderableTrail & tTrail,
    Vertex1TexL *& pv,
    const Vector3D & wpCam)
{ // Packed X-axis normal bytes recovered from original; constructor spelling inferred.
    UByte4 wvpXAxis(255, 127, 128, 0);

    Vector2D uv0, uv1;
    uv0.y = 0.0f;
    uv1.y = 1.0f;

    BOOL fEdgeAlpha = tTrail.fEdgeAlpha;

    for (int32 i = 0; i < tTrail.nCurvePoints; ++i)
    {
        TrailCurvePoint & tCurvePoint = tTrail.ptCurvePoints[i];
        uv0.x = uv1.x = tCurvePoint.flFractionAlongTrail;

        Vector3D wpPos0(tCurvePoint.wpCurvePoint0);
        Vector3D wpPos1(tCurvePoint.wpCurvePoint1);

        RGBAColor rgbaColor(tCurvePoint.rgbaCurvePointColor);
        if (fEdgeAlpha)
        {

            Vector3D wvPerpendicular(tTrail.wvCurvePointPerpendicular[i]);

            uint8 byteEdgeAlpha = sComputeEdgeAlpha(wpPos0, wvPerpendicular, wpCam);
            rgbaColor.a = MultiplyAlpha(rgbaColor.a, byteEdgeAlpha);
        }

        VertexColor vc(rgbaColor);

        Vertex1TexLSet(*pv, wpPos0, wvpXAxis, vc, crgbaBlack, Vector2DTexCoordToUByte4(uv0));
        ++pv;
        Vertex1TexLSet(*pv, wpPos1, wvpXAxis, vc, crgbaBlack, Vector2DTexCoordToUByte4(uv1));
        ++pv;
    }
}





static void sTrailDrawCross(
    const RenderableTrail & tTrail,
    Vertex1TexL *& pv,
    const Vector3D & wpCam)
{ // Packed X-axis normal bytes recovered from original; constructor spelling inferred.
    UByte4 wvpXAxis(255, 127, 128, 0);

    Vector2D uv0, uv1;
    uv0.y = 0.0f;
    uv1.y = 1.0f;

    BOOL fEdgeAlpha = tTrail.fEdgeAlpha;
    Vector3D wvLastPerpendicular(cgwvNull);

    for (int32 i = 0; i < tTrail.nCurvePoints; ++i)
    {
        TrailCurvePoint & tCurvePoint = tTrail.ptCurvePoints[i];
        Vector3D wpCenter = (tCurvePoint.wpCurvePoint0 + tCurvePoint.wpCurvePoint1) * 0.5f;

        Vector3D wvUp = tCurvePoint.wpCurvePoint1 - tCurvePoint.wpCurvePoint0;

        RGBAColor rgbaColor(tCurvePoint.rgbaCurvePointColor);
        if (fEdgeAlpha)
        {

            uint8 byteEdgeAlpha = sComputeEdgeAlpha(wpCenter, wvUp, wpCam);
            rgbaColor.a = MultiplyAlpha(rgbaColor.a, byteEdgeAlpha);
        }

        Vector3D wvPerpendicular(tTrail.wvCurvePointPerpendicular[i]);

        WorldDistance wdDot = Vector3D::DotProduct(wvPerpendicular, wvLastPerpendicular);
        if (wdDot < 0.0f)
        {




            wvPerpendicular = -wvPerpendicular;
        }
        wvLastPerpendicular = wvPerpendicular;
        uv0.x = uv1.x = tCurvePoint.flFractionAlongTrail;


        Vector3D wpPos0 = wpCenter + wvPerpendicular;
        Vector3D wpPos1 = wpCenter - wvPerpendicular;
        VertexColor vc(rgbaColor);

        Vertex1TexLSet(*pv, wpPos0, wvpXAxis, vc, crgbaBlack, Vector2DTexCoordToUByte4(uv0));
        ++pv;
        Vertex1TexLSet(*pv, wpPos1, wvpXAxis, vc, crgbaBlack, Vector2DTexCoordToUByte4(uv1));
        ++pv;
    }
}





static BOOL sTrailDrawCircle(
    const RenderableTrail & tTrail,
    Vertex1TexL *& pv,
    const Vector3D & wpCam)
{
    BOOL fEdgeAlpha = tTrail.fEdgeAlpha;

    for (int32 i = 0; i < tTrail.nCurvePoints; ++i)
    {
        TrailCurvePoint & tCurvePoint = tTrail.ptCurvePoints[i];

        Vector3D wpCenter = (tCurvePoint.wpCurvePoint0 + tCurvePoint.wpCurvePoint1) * 0.5f;

        Vector3D wvAlong;
        if (tTrail.nCurvePoints < 2)
            return FALSE;





        Vector3D wvUp = tCurvePoint.wpCurvePoint1 - wpCenter;

        Vector3D wvForward(tTrail.wvCurvePointForward[i]);

        Vector2D uv;
        uv.x = tCurvePoint.flFractionAlongTrail;

        for (int32 j = 0; j < tTrail.nCircleSegments; ++j)
        {
            Angle ang = (j * FULL_CIRCLE) / tTrail.nCircleSegments;

            Vector3D wvOffset = wvUp;
            Vector3D::Rotate(wvForward, ang, wvOffset);

            Vector3D wpPos = wpCenter + wvOffset;
            uv.y = (float)j / tTrail.nCircleSegments;

            RGBAColor rgbaColor(tCurvePoint.rgbaCurvePointColor);
            if (fEdgeAlpha)
            {

                uint8 byteEdgeAlpha = sComputeEdgeAlpha(wpPos, wvOffset, wpCam);
                rgbaColor.a = MultiplyAlpha(rgbaColor.a, byteEdgeAlpha);
            }

            VertexColor vc(rgbaColor);
            Vertex1TexLSet(*pv, wpPos, Vector3DNormalToUByte4(wvOffset), vc, crgbaBlack, Vector2DTexCoordToUByte4(uv));
            ++pv;
        }
    }
    return TRUE;
}




// NON_MATCHING: 2112/2112 bytes. One integer multiply exchanges its source registers in the circle primitive count.
void TrailDraw(
    const RenderableHeader & tHeader,
    const RenderableDetails & tDetails)
{


    TrailGlobals * ptTrailGlobals = CGameGet().ptTrailGlobals;
    const TrailDefinition * pdefTrail = static_cast<const TrailDefinition *>(gptGroupDefTrail->LockR(tDetails.tTrail.snoTrail, FALSE));











    int32 nVertCount;
    int32 nPrimCount;

    switch (tDetails.tTrail.nTrailType)
    {
    case TRAIL_LINE:
        nVertCount = tDetails.tTrail.nCurvePoints * 2;
        nPrimCount = nVertCount - 2;
        break;
    case TRAIL_CROSS:
        nVertCount = tDetails.tTrail.nCurvePoints * 4;
        nPrimCount = nVertCount;
        break;
    case TRAIL_CIRCLE:
        nVertCount = tDetails.tTrail.nCurvePoints * tDetails.tTrail.nCircleSegments;
        nPrimCount = (tDetails.tTrail.nCircleSegments + 1) * (tDetails.tTrail.nCurvePoints * 2 - 2) - 2;
        break;
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    BOOL bCanDraw = TRUE;
    uint32 dwStartIndex;
    Vertex1TexL * pv = static_cast<Vertex1TexL *>(GfxVBLock(ptTrailGlobals->vbid, nVertCount, dwStartIndex));
    if (!pv)
    {
        bCanDraw = FALSE;
    }




    else
    {
        const Camera & cam = CameraManagerGetCamera(CAMERA_CURRENT);

        switch (tDetails.tTrail.nTrailType)
        {
        case TRAIL_LINE:
            sTrailDrawLine(tDetails.tTrail, pv, cam.cmState.transform.wp);
            break;

        case TRAIL_CROSS:
            sTrailDrawLine(tDetails.tTrail, pv, cam.cmState.transform.wp);
            sTrailDrawCross(tDetails.tTrail, pv, cam.cmState.transform.wp);

            break;
        case TRAIL_CIRCLE:
            bCanDraw = sTrailDrawCircle(tDetails.tTrail, pv, cam.cmState.transform.wp);
            break;

        default:
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }







        GfxVBUnlock(ptTrailGlobals->vbid);
    }

    if (!bCanDraw)
    {
        gptGroupDefTrail->UnlockR((void *&)pdefTrail);
        return;
    }

    GfxWorldMatrixSet(tDetails.tCommon.mWorld);

    const ShaderDefinition * pdefShader = tHeader.pdefShader;
    for (int nPassIndex = 0; nPassIndex != pdefShader->nPassCount; ++nPassIndex)
    {
        GfxShaderInit(pdefShader, nPassIndex);
        GfxShaderSetTextureFactor(tHeader.rgbaColor);
        GfxShaderSetIB(tDetails.tTrail.ibid, 0);
        GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptTrailGlobals->vbid);
        GfxShaderSetTextures(pdefTrail->tUberMaterial, tDetails.tTrail.arTexAnimStates, "Trail", -1);
        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, dwStartIndex, 0, nVertCount, 0, nPrimCount, PERFBIN_OTHER);
    }
    gptGroupDefTrail->UnlockR((void *&)pdefTrail);
}
