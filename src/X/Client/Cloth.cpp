#include "Attachments.h"
#include "CGame.h"
#include "Cloth.h"
#include "ClothUpdate.h"
#include "Definitions/ClothDefinition.h"
#include "Explosions.h"
#include "Wind.h"
#include "RMessage.h"
#include "Definitions/AppearanceDefinition.h"
#include "SubObjectGfx.h"
#include "Gfx/GfxVB.h"
#include "Camera/CameraManager.h"
#include "../Sigma/Random.h"
#include "RActors/RActors.h"
#include "../Common/JobManager/JobManager.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/Math/Intersect.h"
#include "../Sigma/DataStructures/XSphereArray.h"
#include "../Sigma/DataStructures/XSphereNode.h"
#include "../Sigma/MemoryPools.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/Memory/SigmaMemory.h"

// X/Common/Game.cpp:1938
XMemoryAllocatorInterface * AppGetCurrentMemoryInterface();
// X/Client/Attachments.cpp
Attachment * AttachmentTryToGet(AttachmentID id);
void AttachmentDelete(Attachment & tAttachment);
// X/Client/Gfx/GfxVB.cpp
void * GfxVBLock(const VBID vbid, const uint32 dwCount, uint32 & dwStartIndex);
void GfxVBUnlock(const VBID vbid);

enum {
    CLOTHFLAG_SETTLING = 0x1,
    CLOTHFLAG_PLANES_DISABLED = 0x2,
    CLOTHFLAG_PENDING_DELETION = 0x4,
};

static void sDestroyAsyncCloth(void * pParam, void *);
XSphereArray * RActorGetCollisionSphereTree();  // RActors/RActorCollision.cpp
BOOL CameraComputeSphereVisibility(const Camera & camera, const Sphere & tSphere);  // Camera/Camera.cpp
#include "../Sigma/TypeDescriptor/SymbolicValue.h"
#include "RActors/RActorAnimation.h"
extern const SymbolicEntry gClothPlanes[10];  // X/Common/SNO/SymbolicValue.cpp:4149
#include "Physics/Ragdoll.h"
#include "Sound/AmbientSound.h"
Vector3D UByte4NormalToVector3D(const UByte4 packed);  // X/Client/Gfx/GfxVB.cpp:1199
void ClothUpdateBoundingSphere(const ClothUpdateInputParams & tInputParams, ClothUpdateInputOutputParams & tInputOutputParams);  // X/Client/ClothUpdate.cpp:176
void ClothUpdateFaceNormals(const ClothUpdateInputParams & tInputParams, ClothUpdateInputOutputParams & tInputOutputParams);  // X/Client/ClothUpdate.cpp:210
RagdollType RActorGetRagdollType(const RActor * ptRActor);  // X/Client/RActors/RActors.cpp:11362
AttachmentID RActorAddAttachment(RActor & tRActor, const AttachmentType eAttachmentType, const SNO sno, const uint32 dwAttachmentID, HardpointHash szHardpointName, BOOL bIgnoreOwnerScale);  // X/Client/RActors/RActorAttachments.cpp:2325
#include <float.h>














void ClothManager::InitializeForGame() {
    CGame & tCGame = CGameGet();
    ClothManager * ptManager = (ClothManager *)SigmaMemoryNew(sizeof(ClothManager), 4, NULL, TRUE);
    ptManager->m_ptCloths = TDataArrayCreateTracked<Cloth, int>("Cloth", AppGetCurrentMemoryInterface(), CLOTH_MAX_COUNT);
    tCGame.ptClothManager = ptManager;
}










void ClothManager::DisposeForGame() {
    TDataArrayDispose(CGameGet().ptClothManager->m_ptCloths);
    SigmaMemoryFree(CGameGet().ptClothManager, NULL);
    CGameGet().ptClothManager = NULL;
}







ClothID ClothManager::Create(RActor * ptRActor, int32 nSubObjectIndex, SNO snoCloth) {
    ClothManager * ptManager = CGameGet().ptClothManager;
    ClothID id = ptManager->m_ptCloths->ElementNew();
    if (id == CLOTHID_INVALID) {
        return CLOTHID_INVALID;
    }

    Cloth * ptCloth = ptManager->m_ptCloths->ElementGet(id);
    ptCloth->dwLastFrameSubmitted = CGameAsyncGetDrawFrameCount() - 1;
    if (!ptCloth->Create(ptRActor, nSubObjectIndex, snoCloth)) {
        ptCloth->Destroy();
        ptManager->m_ptCloths->ElementDelete(ptCloth);
        return CLOTHID_INVALID;
    }

    ptManager->m_nClothCount++;
    return id;
}



















Cloth * ClothManager::Get(ClothID id) {
    return CGameGet().ptClothManager->m_ptCloths->ElementGet(id);
}








Cloth * ClothManager::TryToGet(ClothID id) {
    ClothManager * ptManager = CGameGet().ptClothManager;
    return ptManager->m_ptCloths->ElementTryToGet(id);
}







static void sDestroyAsyncCloth(void * pParam, void *) {
    ClothManager::Destroy((ClothID)(intptr_t)pParam);
}






void ClothManager::Destroy(ClothID id) {
    ClothManager * ptManager = CGameGet().ptClothManager;
    Cloth * ptCloth = TryToGet(id);
    if (ptCloth == NULL) {
        return;
    }

    if (ptCloth->dwLastFrameSubmitted == CGameAsyncGetDrawFrameCount() &&
        CGameAsyncRenderRegisterDelayedCallback(sDestroyAsyncCloth, (void *)(intptr_t)id, NULL)) {
        ptCloth->SetPendingDeletion();
        return;
    }

    ptCloth->Destroy();
    ptManager->m_ptCloths->ElementDelete(ptCloth);
    ptManager->m_nClothCount--;
}

// NON_MATCHING: 1 store; m_nEdgeCount store scheduled before (original: after) the loop-carried register moves










#line 194
void ClothManager::Update(
    float flDeltaTime,
    BOOL fFixedUpdateOnly)
{
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    ClothManager * ptManager = CGameGet().ptClothManager;


    const Camera & camera = CameraManagerGetCamera(-1);

    ptManager->m_nFaceCount = 0;
    ptManager->m_nEdgeCount = 0;
    ptManager->m_nVisibleCount = 0;
    ptManager->m_nVertexCount = 0;


    const uint32 cdwChance = 16;

    TDataArrayIterator<Cloth> tIt;
    while (ptManager->m_ptCloths->Iterate(tIt))
    {
        Cloth * ptCloth = tIt.ptElement;


        if (ptCloth->IsPendingDeletion())
        {
            continue;
        }


        if (!ptCloth->IsVisible(camera))
        {

            ptCloth->m_dwFlags |= CLOTHFLAG_SETTLING;

            uint32 dwRandom = RandGetDWord(tCGameGlobals.rngLocal);
            if ((dwRandom % cdwChance) != 0)
            {
                continue;
            }
        }
        else
        {
            ptManager->m_nVisibleCount++;
            ptManager->m_nVertexCount += ptCloth->m_tUpdateParams.tInput.nVertexCount;
            ptManager->m_nFaceCount += ptCloth->m_tUpdateParams.tInput.nFaceCount;
            ptManager->m_nEdgeCount += ptCloth->m_tUpdateParams.tInput.nDistanceConstraintCount;
        }

        if (fFixedUpdateOnly)
        {
            RActor * ptRActor = RActorGet(ptCloth->m_idRActor);
            if (!(ptRActor->dwFlagsEx & 0x1))
            {
                continue;
            }
        }

        ptCloth->m_flOldDeltaTime = ptCloth->m_flDeltaTime;
        ptCloth->m_flDeltaTime = flDeltaTime;

        ptCloth->PreStep();

        JobManager::Get()->PushJob(Cloth::Execute, ptCloth);
    }
    JobManager::Get()->PushingCompleted();
}
#line 262
void ClothManager::WaitForCompletion() {
    JobManager::Get()->WaitForIdle();
}







// Cloth::GetBoundingSphere (line 272) is defined in Cloth.h.
void ClothManager::CopyToAsyncVertices() {
    ClothManager * ptManager = CGameGet().ptClothManager;
    uint32 dwCurrentFrameCount = CGameAsyncGetUpdateFrameCount();
    TDataArrayIterator<Cloth> tIt;
    while (Cloth * ptCloth = ptManager->m_ptCloths->Iterate(tIt)) {
        if (ptCloth->IsPendingDeletion()) {
            continue;
        }
        RActor * ptRActor = RActorGet(ptCloth->m_idRActor);
        if (ptRActor->dwLastFrameSubmitted != dwCurrentFrameCount) {
            continue;
        }
        ptCloth->dwLastFrameSubmitted = CGameAsyncGetUpdateFrameCount();
        ptCloth->CopyToAsyncVertices();
    }
}




















































void ClothManager::IntersectSphere(const Sphere & tSphere, XPooledDataIDList & listCloth) {
    ClothManager * ptManager = CGameGet().ptClothManager;
    TDataArrayIterator<Cloth> tIt;
    while (ptManager->m_ptCloths->Iterate(tIt)) {
        Cloth * ptCloth = tIt.ptElement;
        if (ptCloth->IsPendingDeletion()) {
            continue;
        }
        if (Intersect::SphereAndSphere(tSphere, ptCloth->m_tUpdateParams.tInputOutput.wsBoundingSphere)) {
            listCloth.AddTail_Tracked(ptCloth->id);
        }
    }
}




























BOOL Cloth::Create(
    RActor * ptRActor,
    int32 nSubObjectIndex,
    SNO snoCloth)
{



    m_idRActor = ptRActor->id;
    m_nSubObjectIndex = nSubObjectIndex;

    m_snoCloth = snoCloth;
    m_flDeltaTime = m_flOldDeltaTime = 1.0f / 60.0f;


    m_tUpdateParams.tInputOutput.ptVertices = NULL;
    m_tUpdateParams.tInput.nVertexCount = 0;
    m_tUpdateParams.tInput.ptDistanceConstraints = NULL;
    m_tUpdateParams.tInput.nDistanceConstraintCount = 0;
    m_tUpdateParams.tInput.ptBendingConstraints = NULL;
    m_tUpdateParams.tInput.nBendingConstraintCount = 0;
    m_tUpdateParams.tInput.ptFaces = NULL;
    m_tUpdateParams.tInput.nFaceCount = 0;

	m_dwFlags = CLOTHFLAG_SETTLING;
	m_tUpdateParams.tInput.nIslandCount = 0;
    m_tUpdateParams.tInput.nCapsuleCount = 0;
    m_ptStaples = NULL;
    m_ptCachedFatNormals = NULL;
    m_tUpdateParams.tInput.nStapleCount = 0;
    m_flArea = 0.0f;
	m_tUpdateParams.tInputOutput.flAverageVelocity = 0.0f;

    // 2: the m_tRenderData array size (Cloth.h)
    for (int32 i = 0; i < 2; ++i)
    {
        m_tRenderData[i].m_pAsyncVertices2TexL = NULL; m_tRenderData[i].m_pAsyncVerticesCloth = NULL; m_tRenderData[i].m_dwAsyncVertexCount = 0;
    }




	RNG &rng = CGameGlobalsGet().rngLocal;
	m_flPhaseShift = RandFloat_Internal(rng) * FULL_CIRCLE;

    ScopedClothDefinitionLockR pdefCloth(m_snoCloth);

    if (pdefCloth->flDensity < EPSILON)
    {
        return FALSE;
    }


    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[GEOSET_HIGHPOLY];
    const PRSTransform & transformRActor = ptRActor->transformClient;

    const float cflScale = transformRActor.flScale;
    const float cflScaleSquared = cflScale * cflScale;
    const float cflInvScaleSquared = cflScale > FLT_EPSILON ? 1.0f / cflScaleSquared : 1.0f;



    const SubObject *ptSubObject = tGeoSet.ptSubObjects + m_nSubObjectIndex;

    const ClothStructure *ptClothStructure = ptSubObject->ptClothStructure;


    if (ptClothStructure == NULL)
    {
        return FALSE;
    }








    if (ptClothStructure->nStapleCount == 0)
    {
        return FALSE;
    }








    m_tUpdateParams.tInput.nVertexCount = ptClothStructure->nVertexCount;
    m_tUpdateParams.tInput.nFaceCount = ptClothStructure->nFaceCount;
    m_tUpdateParams.tInput.nStapleCount = ptClothStructure->nStapleCount;
    m_tUpdateParams.tInput.nDistanceConstraintCount = ptClothStructure->nDistanceConstraintCount;
    m_tUpdateParams.tInput.nBendingConstraintCount = ptClothStructure->nBendingConstraintCount;
	m_tUpdateParams.tInput.nIslandCount = ptClothStructure->nIslandCount;


    int32 size = m_tUpdateParams.tInput.nVertexCount * sizeof(ClothVertex);
    size += m_tUpdateParams.tInput.nFaceCount * sizeof(ClothFace);

    size += m_tUpdateParams.tInput.nStapleCount * sizeof(ClothStaple);
	size += m_tUpdateParams.tInput.nStapleCount * sizeof(Vector3D);
    size += (m_tUpdateParams.tInput.nDistanceConstraintCount + m_tUpdateParams.tInput.nBendingConstraintCount) * sizeof(ClothConstraint);
    size += m_tUpdateParams.tInput.nIslandCount * sizeof(ClothIsland);
    m_pWorkSpace = (int8*)SigmaMemoryNew(size, 4, NULL, TRUE);
    int8 * pWorkSpace = m_pWorkSpace;


    m_tUpdateParams.tInputOutput.ptVertices = (ClothVertex *)pWorkSpace; pWorkSpace += m_tUpdateParams.tInput.nVertexCount * sizeof(ClothVertex);
    for (int32 i = 0; i < m_tUpdateParams.tInput.nVertexCount; ++i)
    {
        ClothVertex &tVertex = m_tUpdateParams.tInputOutput.ptVertices[i];
        ClothVertexSerializable &tVertexSrc = ptClothStructure->ptVertices[i];

        tVertex.wp = tVertexSrc.wp;
        tVertex.wp0 = tVertexSrc.wp0;
        tVertex.flInvMass = tVertexSrc.flInvMass;
        tVertex.nParentIndex = tVertexSrc.nParentIndex;
        tVertex.nAttachmentDistance = tVertexSrc.nAttachmentDistance;
        tVertex.nFatVertexIndex = tVertexSrc.nFatVertexIndex;
        tVertex.nIslandIndex = tVertexSrc.nIslandIndex;
    }

    const float cflDensity = (1.0f/32.2f) * pdefCloth->flDensity;
    const float cflInvDensity = 1.0f / cflDensity;


    Vertex2TexLCPos * ptPackVertList = (Vertex2TexLCPos*)ptSubObject->pPackVertList.Get();


    for (int32 i = 0; i < m_tUpdateParams.tInput.nVertexCount; ++i)
    {
        ClothVertex *pv = m_tUpdateParams.tInputOutput.ptVertices + i;

        if (ptPackVertList)
        {
            const Vertex2TexLCPos *pfv = ptPackVertList + pv->nFatVertexIndex;


            pv->wp0.x = (float)(pfv->nX) * ptSubObject->wpFixedPointPosScale.x + ptSubObject->wpFixedPointPosOffset.x;
            pv->wp0.y = (float)(pfv->nY) * ptSubObject->wpFixedPointPosScale.y + ptSubObject->wpFixedPointPosOffset.y;
            pv->wp0.z = (float)(pfv->nZ) * ptSubObject->wpFixedPointPosScale.z + ptSubObject->wpFixedPointPosOffset.z;
        }
        else
        {

            const FatVertex *pfv = ptSubObject->ptVertList + pv->nFatVertexIndex;


            pv->wp0 = pfv->wpPos;
        }

        pv->wp = transformRActor * pv->wp0;
        pv->wv = cgwvNull;


        pv->flInvMass *= cflInvScaleSquared;


        pv->flInvMass *= cflInvDensity;
    }


    m_tUpdateParams.tInput.ptFaces = (ClothFace *)pWorkSpace; pWorkSpace += m_tUpdateParams.tInput.nFaceCount * sizeof(ClothFace);
    for (int32 i = 0; i < m_tUpdateParams.tInput.nFaceCount; ++i)
    {
        ClothFace &tFace = m_tUpdateParams.tInput.ptFaces[i];
        ClothFaceSerializable &tFaceSrc = ptClothStructure->ptFaces[i];

        tFace.flRestArea = tFaceSrc.flRestArea;
        tFace.nVertex1 = tFaceSrc.nVertex1;
        tFace.nVertex2 = tFaceSrc.nVertex2;
        tFace.nVertex3 = tFaceSrc.nVertex3;
    }

    for (int32 i = 0; i < m_tUpdateParams.tInput.nFaceCount; ++i)
    {

        m_tUpdateParams.tInput.ptFaces[i].flRestArea *= cflScaleSquared;
    }


    m_flArea = cflScaleSquared * ptClothStructure->flArea;


    m_ptStaples = (ClothStaple *)pWorkSpace; pWorkSpace += m_tUpdateParams.tInput.nStapleCount * sizeof(ClothStaple);
    SigmaMemoryCopy(m_ptStaples, ptClothStructure->ptStaples, m_tUpdateParams.tInput.nStapleCount * sizeof(ClothStaple));


    m_ptCachedFatNormals = (Vector3D *)pWorkSpace; pWorkSpace += m_tUpdateParams.tInput.nStapleCount * sizeof(Vector3D);
    const FatVertex *ptVertList = ptSubObject->ptVertList;
    ClothVertex *ptVertices = m_tUpdateParams.tInputOutput.ptVertices;
    for (int32 i = 0; i < m_tUpdateParams.tInput.nStapleCount; ++i)
    {
        ClothStaple *ptStaple = m_ptStaples + i;
        ClothVertex *pv = ptVertices + ptStaple->nVertex;

        if (ptPackVertList)
        {
            const Vertex2TexLCPos *ptVertex = ptPackVertList + pv->nFatVertexIndex;
            m_ptCachedFatNormals[i] = UByte4NormalToVector3D(ptVertex->wvpNormal);
        }
        else
        {

            const FatVertex *ptFatVertex = ptVertList + pv->nFatVertexIndex;
            m_ptCachedFatNormals[i] = UByte4NormalToVector3D(ptFatVertex->wvpNormal);
        }
    }

	m_tUpdateParams.tInput.ptIslands = (ClothIsland *)pWorkSpace; pWorkSpace += m_tUpdateParams.tInput.nIslandCount * sizeof(ClothIsland);
	SigmaMemoryClear(m_tUpdateParams.tInput.ptIslands, m_tUpdateParams.tInput.nIslandCount * sizeof(ClothIsland));

    m_bAnimated = ptRActor->ptActorAnim != NULL;
    for (int32 i = 0; i < m_tUpdateParams.tInput.nStapleCount; ++i)
    {

		ClothVertex *pv = m_tUpdateParams.tInputOutput.ptVertices + m_ptStaples[i].nVertex;

		m_tUpdateParams.tInput.ptIslands[pv->nIslandIndex].m_nStapleCount++;
        for (int32 j = CLOTH_MAX_BONE_INFLUENCES - 1; j >= 0; --j)
        {
            if (m_ptStaples[i].aflBoneWeights[j] != 0.0f)
            {
                break;
            }


            m_ptStaples[i].anBoneIndices[j] = -1;
        }
    }

    for (int32 i = m_tUpdateParams.tInput.nStapleCount; i < m_tUpdateParams.tInput.nVertexCount; ++i)
    {
        ClothVertex *pv = m_tUpdateParams.tInputOutput.ptVertices + i;
        ClothIsland &tIsland = m_tUpdateParams.tInput.ptIslands[pv->nIslandIndex];
        if (tIsland.m_nMinVertexIndex == 0)
        {
            tIsland.m_nMinVertexIndex = i;
        }
        else
        {
            tIsland.m_nMinVertexIndex = Math::Min(tIsland.m_nMinVertexIndex, i);
        }
        tIsland.m_nMaxVertexIndex = Math::Max(tIsland.m_nMaxVertexIndex, i);
        tIsland.m_nVertexCount++;
    }

	for (int32 i = 0; i < m_tUpdateParams.tInput.nIslandCount; ++i)
	{
        if (m_tUpdateParams.tInput.ptIslands[i].m_nVertexCount == 0)
        {
            m_tUpdateParams.tInput.ptIslands[i].m_nMaxVertexIndex = m_tUpdateParams.tInput.ptIslands[i].m_nMinVertexIndex - 1;
        }
		m_tUpdateParams.tInput.ptIslands[i].m_transform1 = transformRActor;
		m_tUpdateParams.tInput.ptIslands[i].m_transform2 = m_tUpdateParams.tInput.ptIslands[i].m_transform1;
		m_tUpdateParams.tInput.ptIslands[i].m_fContiguous = m_tUpdateParams.tInput.ptIslands[i].m_nMaxVertexIndex + 1 - m_tUpdateParams.tInput.ptIslands[i].m_nMinVertexIndex == m_tUpdateParams.tInput.ptIslands[i].m_nVertexCount;
	}


    const float cflStabilityFactor = 1.0f / Math::Max(1.0f, pdefCloth->flStabilityFactor);


	if (m_tUpdateParams.tInput.nDistanceConstraintCount > 0)
	{
		m_tUpdateParams.tInput.ptDistanceConstraints = (ClothConstraint *)pWorkSpace; pWorkSpace += m_tUpdateParams.tInput.nDistanceConstraintCount * sizeof(ClothConstraint);
        for (int32 i = 0; i < m_tUpdateParams.tInput.nDistanceConstraintCount; ++i)
        {
            ClothConstraint &tContraint = m_tUpdateParams.tInput.ptDistanceConstraints[i];
            ClothConstraintSerializable &tContraintSrc = ptClothStructure->ptDistanceConstraints[i];

            tContraint.nVertex1 = tContraintSrc.nVertex1;
            tContraint.nVertex2 = tContraintSrc.nVertex2;
            tContraint.flRestLengthSquared = tContraintSrc.flRestLengthSquared;
            tContraint.flHorizontal = tContraintSrc.flHorizontal;
        }
	}
	else
	{
		m_tUpdateParams.tInput.ptDistanceConstraints = NULL;
	}

    for (int32 i = 0; i < m_tUpdateParams.tInput.nDistanceConstraintCount; ++i)
    {
        ClothConstraint *ptConstraint = m_tUpdateParams.tInput.ptDistanceConstraints + i;

        const ClothVertex *pv1 = m_tUpdateParams.tInputOutput.ptVertices + ptConstraint->nVertex1;
        const ClothVertex *pv2 = m_tUpdateParams.tInputOutput.ptVertices + ptConstraint->nVertex2;

        int32 nAD1 = pv1->nAttachmentDistance;
        int32 nAD2 = pv2->nAttachmentDistance;

        if (nAD1 > nAD2)
        {
            Math::Swap(ptConstraint->nVertex1, ptConstraint->nVertex2);
            Math::Swap(pv1, pv2);
            Math::Swap(nAD1, nAD2);
        }

        float flBias = 1.0f;
        if (nAD1 < nAD2 && nAD1 > 0)
        {
            flBias = cflStabilityFactor;
        }


        float flDenom = flBias * pv1->flInvMass + pv2->flInvMass;

        flDenom = 1.0f / flDenom;
        ptConstraint->flScale1 = flBias * pv1->flInvMass * flDenom;
        ptConstraint->flScale2 = pv2->flInvMass * flDenom;


        ptConstraint->flRestLengthSquared *= cflScaleSquared;
    }


	if (m_tUpdateParams.tInput.nBendingConstraintCount)
	{
		m_tUpdateParams.tInput.ptBendingConstraints = (ClothConstraint *)pWorkSpace; pWorkSpace += m_tUpdateParams.tInput.nBendingConstraintCount * sizeof(ClothConstraint);

        for (int32 i = 0; i < m_tUpdateParams.tInput.nBendingConstraintCount; ++i)
        {
            ClothConstraint &tContraint = m_tUpdateParams.tInput.ptBendingConstraints[i];
            ClothConstraintSerializable &tContraintSrc = ptClothStructure->ptBendingConstraints[i];

            tContraint.nVertex1 = tContraintSrc.nVertex1;
            tContraint.nVertex2 = tContraintSrc.nVertex2;
            tContraint.flRestLengthSquared = tContraintSrc.flRestLengthSquared;
            tContraint.flHorizontal = tContraintSrc.flHorizontal;
        }

		for (int32 i = 0; i < m_tUpdateParams.tInput.nBendingConstraintCount; ++i)
		{
			ClothConstraint *ptConstraint = m_tUpdateParams.tInput.ptBendingConstraints + i;

			const ClothVertex *pv1 = m_tUpdateParams.tInputOutput.ptVertices + ptConstraint->nVertex1;
			const ClothVertex *pv2 = m_tUpdateParams.tInputOutput.ptVertices + ptConstraint->nVertex2;

			int32 nAD1 = pv1->nAttachmentDistance;
			int32 nAD2 = pv2->nAttachmentDistance;

			if (nAD1 > nAD2)
			{
				Math::Swap(ptConstraint->nVertex1, ptConstraint->nVertex2);
				Math::Swap(pv1, pv2);
				Math::Swap(nAD1, nAD2);
			}

			float flBias = 1.0f;
			if (nAD1 < nAD2 && nAD1 > 0)
			{
				flBias = cflStabilityFactor;
			}

			float flDenom = flBias * pv1->flInvMass + pv2->flInvMass;

			flDenom = 1.0f / flDenom;
			ptConstraint->flScale1 = flBias * pv1->flInvMass * flDenom;
			ptConstraint->flScale2 = pv2->flInvMass * flDenom;


			ptConstraint->flRestLengthSquared *= cflScaleSquared;
		}
	}

    ClothUpdateBoundingSphere(m_tUpdateParams.tInput, m_tUpdateParams.tInputOutput);
    ClothUpdateFaceNormals(m_tUpdateParams.tInput, m_tUpdateParams.tInputOutput);


	RagdollType type = RActorGetRagdollType(ptRActor);
	if (type == FULL_RAGDOLL_TYPE)
	{
		m_dwFlags |= CLOTHFLAG_PLANES_DISABLED;
	}


    m_idAttachment = ATTACHMENTID_INVALID;
    if (pdefCloth->snoAmbientSound != SNO_NONE)
    {
        m_idAttachment = RActorAddAttachment(*ptRActor, ATTACHMENTTYPE_AMBIENTSOUND, pdefCloth->snoAmbientSound, (uint32)-1, DEFAULT_HARDPOINT_NAME, FALSE);
        Attachment * ptAttachment = AttachmentTryToGet(m_idAttachment);
        if (ptAttachment)
        {
            AmbientSound * ptAmbientSound = AmbientSoundGet(ptAttachment->idAttachedObject);
            ptAmbientSound->SetParentCloth(id);
        }
    }

    return TRUE;
}



inline void Cloth::Destroy() {
    Attachment * ptAttachment = AttachmentTryToGet(m_idAttachment);
    if (ptAttachment) {
        AttachmentDelete(*ptAttachment);
    }
    SigmaMemoryFree(m_pWorkSpace, NULL);
    m_pWorkSpace = NULL;
}






void Cloth::ApplyExplosion(const ExplosionImpulse & tExplosion) {
    const float cflFudgeFactor = 0.0005f;
    if (!IsRendered()) {
        return;
    }

    ScopedClothDefinitionLockR pdefCloth(m_snoCloth);
    if (pdefCloth->flExplosionFactor > 0.0f) {
        const float cflMaxDeltaV = 1.0f;
        const float cflMaxDeltaVSqr = cflMaxDeltaV * cflMaxDeltaV;
        const float cflScale = pdefCloth->flExplosionFactor * cflFudgeFactor * m_flArea;
        for (int32 i = m_tUpdateParams.tInput.nStapleCount; i < m_tUpdateParams.tInput.nVertexCount; i++) {
            ClothVertex * pv = &m_tUpdateParams.tInputOutput.ptVertices[i];
            Vector3D wvImpulse = tExplosion.ComputeImpulse(pv->wp);
            Vector3D wv = (cflScale * pv->flInvMass) * wvImpulse;
            if (wv.MagnitudeSquared() > cflMaxDeltaVSqr) {
                wv.Normalize();
            }
            pv->wv += wv;
        }
    }
}














void Cloth::CacheCollisionCapsules(const ClothDefinition * pdefCloth) {
    m_tUpdateParams.tInput.nCapsuleCount = 0;
    if (pdefCloth->dwFlags & 0x2) {
        XSphereArray * ptSphereTree = RActorGetCollisionSphereTree();
        XSphereNodeList listSpheres(MemoryPoolsGetSphereNodeListPool());
        ptSphereTree->SphereCull(m_tUpdateParams.tInputOutput.wsBoundingSphere, listSpheres);
        POS pos = listSpheres.GetHeadPosition();
        while (pos) {
            if (m_tUpdateParams.tInput.nCapsuleCount >= 16) {
                break;
            }
            const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);
            CollisionCapsuleInstance * ptCCInstance = (CollisionCapsuleInstance *)ptSphereNode->GetUserData();
            m_tUpdateParams.tInput.aCapsules[m_tUpdateParams.tInput.nCapsuleCount] = ptCCInstance->tCapsule;
            m_tUpdateParams.tInput.nCapsuleCount++;
        }
    } else if (pdefCloth->dwFlags & 0x4) {
        CollisionCapsuleList & tCollisionCapsuleList = m_ptRActor->tCollisionCapsuleList;
        Sphere tClothSphere(m_tUpdateParams.tInputOutput.wsBoundingSphere);
        POS pos = tCollisionCapsuleList.GetHeadPosition();
        while (pos) {
            if (m_tUpdateParams.tInput.nCapsuleCount >= 16) {
                return;
            }
            CollisionCapsuleInstance & tCCInstance = tCollisionCapsuleList.GetNext(pos);
            Sphere tCapsuleSphere;
            tCCInstance.ptSphereNode->GetSphere(tCapsuleSphere);
            if (Intersect::SphereAndSphere(tClothSphere, tCapsuleSphere)) {
                m_tUpdateParams.tInput.aCapsules[m_tUpdateParams.tInput.nCapsuleCount] = tCCInstance.tCapsule;
                m_tUpdateParams.tInput.nCapsuleCount++;
            }
        }
    }
}















void Cloth::CacheCollisionPlanes(
    const ClothDefinition * pdefCloth)
{
    m_tUpdateParams.tInput.nPlaneCount = 0;

    if (m_dwFlags & CLOTHFLAG_PLANES_DISABLED)
        return;



    RActor * ptRActor = RActorGet(m_idRActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure * ptStructure = &pdefAppearance->tStructure;

    for (int32 i = 0; i < 4; ++i)
    {
        int32 nIndex = pdefCloth->anPlanes[i];
        if (nIndex == 0 || nIndex > 9)
            continue;



        const Hardpoint * ptHardpoint = NULL;
        HardpointHash szHardpointName(gClothPlanes[nIndex].sName);
        for (int32 j = 0; j < ptStructure->nHardpointCount; ++j)
        {
            if (ptStructure->ptHardpoints[j].tHash == szHardpointName)
            {
                ptHardpoint = &ptStructure->ptHardpoints[j];
                break;
            }
        }

        if (ptHardpoint == NULL)
            continue;



        PRTransform transform;
        int32 nBoneIndex = ptHardpoint->nBoneIndex;
        if (nBoneIndex != -1 && ptRActor->ptActorAnim != NULL)
        {

            transform = ptRActor->ptActorAnim->ptProcessedBoneList[nBoneIndex].modelTransform * ptHardpoint->transform;
        }
        else
        {
            transform = ptHardpoint->transform;
        }

        transform = ptRActor->transformClient * transform;

        Vector3D normal = transform.Rotate(cgwvXAxis);
        float offset = Vector3D::DotProduct(transform.wp, normal);

        m_tUpdateParams.tInput.aPlanes[m_tUpdateParams.tInput.nPlaneCount].wvNormal = normal;
        m_tUpdateParams.tInput.aPlanes[m_tUpdateParams.tInput.nPlaneCount].wdD = -offset;
        m_tUpdateParams.tInput.nPlaneCount++;
    }
}




inline void Cloth::CacheFatNormals() {
    int32 nStapleCount = m_tUpdateParams.tInput.nStapleCount;
    ClothVertex * ptVertices = m_tUpdateParams.tInputOutput.ptVertices;
    for (int32 i = 0; i < nStapleCount; i++) {
        ptVertices[m_ptStaples[i].nVertex].wvNormal = m_ptCachedFatNormals[i];
    }
}










void Cloth::PreStep() {
    m_ptRActor = RActorGet(m_idRActor);
    ScopedClothDefinitionLockR pdefCloth(m_snoCloth);
    if (pdefCloth->eWindType == CLOTH_WIND_TYPE_GLOBAL) {
        m_tUpdateParams.tInput.wvWind = Wind::GetGlobalWindState().GetPrevailing(m_flPhaseShift);
    } else {
        RActor * pRootParent = m_ptRActor;
        Vector3D wvLocalWind(pdefCloth->wvLocalWind);
        while (pRootParent->tParentData.m_eParentType == PARENT_RACTOR) {
            pRootParent = RActorGet((RActorID)pRootParent->tParentData.m_idParent);
        }
        m_tUpdateParams.tInput.wvWind = pRootParent->transformClient.Rotate(wvLocalWind);
    }
    m_tUpdateParams.tInput.flWindFactor = pdefCloth->flWindFactor;
    m_dwDefFlags = pdefCloth->dwFlags;
    m_tUpdateParams.tInput.flAirBuoyancy = pdefCloth->flAirBuoyancy * 60.0f * 60.0f;
    m_tUpdateParams.tInput.flVelocityFromParentFraction = pdefCloth->flVelocityFromParentFraction;
    m_tUpdateParams.tInput.flStretchingStiffness = pdefCloth->flStretchingStiffness;
    m_tUpdateParams.tInput.flHorizontalStiffness = pdefCloth->flHorizontalStiffness;
    m_tUpdateParams.tInput.flBendingStiffness = pdefCloth->flBendingStiffness;
    m_tUpdateParams.tInput.flDamping = pdefCloth->flDamping;
    m_tUpdateParams.tInput.flFriction = pdefCloth->flFriction;
    m_tUpdateParams.tInput.wvActorVelocity = m_ptRActor->wvVelocity;
    CacheCollisionCapsules(pdefCloth);
    CacheCollisionPlanes(pdefCloth);
    CacheFatNormals();
}
















// NON_MATCHING: 2868 bytes, same as original (717 instructions); same calls and order. 25 instructions differ, all in the island normalisation at 1097-1111: the original multiplies the island position x first with its store, defers the y/z/rotation stores and scales flScale after the rotation stores; ours schedules the scalings differently. Tried the four orderings of the four *= a statements allowed by the DWARF call lines 1098/1100 (best of them kept).
void Cloth::Staple()
{



    RActor * ptRActor = RActorGet(m_idRActor);
    const PRSTransform & transformRActor = RActorGetClientTransform(ptRActor);





    const ActorAnim * ptActorAnim = ptRActor->ptActorAnim;


    if (m_bAnimated)
    {



        ClothIsland * ptIslands = m_tUpdateParams.tInput.ptIslands;
        int32 nIslandCount = m_tUpdateParams.tInput.nIslandCount;
        ClothIsland * ptIsland = ptIslands;
        for (int32 i = 0; i < nIslandCount; ++i, ++ptIsland)
        {
            ptIsland->m_transform1 = ptIsland->m_transform2;
            PRSTransform & transform2 = ptIsland->m_transform2;
            transform2.q.wvImag = cgwvNull;
            transform2.q.wdReal = 0.0f;
            transform2.wp = cgwvNull;
            transform2.flScale = 0.0f;
            ptIsland->m_maxDispacementSqr = 0.0f;
        }


        int32 nStapleCount = m_tUpdateParams.tInput.nStapleCount;
        ClothVertex * ptVertices = m_tUpdateParams.tInputOutput.ptVertices;
        ProcessedBone * ptProcessedBoneList = ptActorAnim->ptProcessedBoneList;
        for (int32 i = 0; i < nStapleCount; ++i)
        {
            ClothStaple * ptStaple = &m_ptStaples[i];
            ClothVertex * pv = &ptVertices[ptStaple->nVertex];
            ClothIsland * ptIslandLoc = &ptIslands[pv->nIslandIndex];
            Vector3D wp1 = pv->wp;
            Vector3D wvNormalFat = pv->wvNormal;
            // Inference: the original loads cgwpNull here and cgwvNull in the island loop at 1030-1032 (GOT identity from land.py).
            pv->wp = cgwpNull;
            pv->wvNormal = cgwpNull;

            for (int32 j = 0; j < CLOTH_MAX_BONE_INFLUENCES; ++j)
            {
                int32 nBoneIndex = ptStaple->anBoneIndices[j];
                if (nBoneIndex == -1)
                    break;



                const ProcessedBone * ptBone = &ptProcessedBoneList[nBoneIndex];
                float flWeight = ptStaple->aflBoneWeights[j];
                PRSTransform stapleTransform = transformRActor * ptBone->skinDelta;
                pv->wp += flWeight * (stapleTransform * pv->wp0);
                pv->wvNormal += flWeight * stapleTransform.q.Rotate(wvNormalFat);

                if (Quaternion::DotProduct(stapleTransform.q, ptIslandLoc->m_transform2.q) < 0.0f)
                {
                    stapleTransform.q.wvImag = -stapleTransform.q.wvImag;
                    stapleTransform.q.wdReal = -stapleTransform.q.wdReal;
                }

                PRSTransform & transform2 = ptIslandLoc->m_transform2;
                transform2.q += flWeight * stapleTransform.q;
                transform2.wp += flWeight * stapleTransform.wp;
                transform2.flScale += flWeight * stapleTransform.flScale;
            }





            float displacementSqr = Vector3D::DistanceSquared(wp1, pv->wp);
            ptIslandLoc->m_maxDispacementSqr = Math::Max(ptIslandLoc->m_maxDispacementSqr, displacementSqr);
        }


        for (int32 i = 0; i < nIslandCount; ++i)
        {
            ClothIsland * ptIslandLoc = &ptIslands[i];

            if (ptIslandLoc->m_nStapleCount == 0)
            {
                ptIslandLoc->m_transform2 = transformRActor;
            }
            else
            {
                float a = 1.0f / ptIslandLoc->m_nStapleCount;
                ptIslandLoc->m_transform2.q.wvImag *= a;
                ptIslandLoc->m_transform2.q.wdReal *= a;
                ptIslandLoc->m_transform2.wp *= a;
                ptIslandLoc->m_transform2.flScale *= a;

                Quaternion & q = ptIslandLoc->m_transform2.q;


                if (q.wvImag.MagnitudeSquared() + q.wdReal * q.wdReal < EPSILON)
                {
                    q = ptIslandLoc->m_transform1.q;
                }

                q.Normalize();
            }
        }
    }
    else
    {

        ClothIsland * ptIslands = m_tUpdateParams.tInput.ptIslands;
        int32 nIslandCount = m_tUpdateParams.tInput.nIslandCount;
        for (int32 i = 0; i < nIslandCount; ++i)
        {
            ptIslands[i].m_transform1 = ptIslands[i].m_transform2;
            ptIslands[i].m_transform2 = transformRActor;
            ptIslands[i].m_maxDispacementSqr = 0.0f;
        }


        int32 nStapleCount = m_tUpdateParams.tInput.nStapleCount;
        ClothVertex * ptVertices = m_tUpdateParams.tInputOutput.ptVertices;
        for (int32 i = 0; i < nStapleCount; ++i)
        {
            ClothStaple * ptStaple = &m_ptStaples[i];
            ClothVertex * pv = &ptVertices[ptStaple->nVertex];
            ClothIsland * ptIsland = &ptIslands[pv->nIslandIndex];

            Vector3D wp1 = pv->wp;

            pv->wp = transformRActor * pv->wp0;
            pv->wvNormal = transformRActor.q.Rotate(pv->wvNormal);





            float displacementSqr = Vector3D::DistanceSquared(wp1, pv->wp);
            ptIsland->m_maxDispacementSqr = Math::Max(ptIsland->m_maxDispacementSqr, displacementSqr);
        }
    }

    m_tUpdateParams.tInput.fRelax = m_dwFlags & CLOTHFLAG_SETTLING;
    m_dwFlags &= ~CLOTHFLAG_SETTLING;
}




void Cloth::Execute(void * pParam) {
    Cloth * ptCloth = (Cloth *)pParam;
    ptCloth->Staple();
    ptCloth->Step();
}


inline void Cloth::Step() {
    float flDeltaTime = Math::Pin(m_flDeltaTime, 0.005f, 1.0f / 60.0f);
    m_flDeltaTime = m_flOldDeltaTime * 0.9f + flDeltaTime * 0.1f;
    ClothUpdate(m_tUpdateParams.tInput, m_tUpdateParams.tInputOutput, m_flDeltaTime);
}






























































































static inline UByte4 sVector3DNormalToUByte4(const Vector3D & wvNormal) {
    UByte4 normal;
    float x = Math::Pin((wvNormal.x * 0.5f + 0.5f) * 255.99f, 0.0f, 255.0f);
    float y = Math::Pin((wvNormal.y * 0.5f + 0.5f) * 255.99f, 0.0f, 255.0f);
    float z = Math::Pin((wvNormal.z * 0.5f + 0.5f) * 255.99f, 0.0f, 255.0f);
    normal.r = (uint8)x;
    normal.g = (uint8)y;
    normal.b = (uint8)z;
    normal.a = 0;
    return normal;
}





























void Cloth::CopyAsyncVertices(VBID vbid, VBID vbidModelSpacePos, uint32 & dwIBOffset) {
    ClothRenderData & tRenderData = m_tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)];
    uint32 dwCount = tRenderData.m_dwAsyncVertexCount;
    uint32 dwStartIndex;
    void * pDest = GfxVBLock(vbid, dwCount, dwStartIndex);
    void * pDestModelSpacePos = GfxVBLock(vbidModelSpacePos, dwCount, dwIBOffset);
    memcpy(pDest, tRenderData.m_pAsyncVertices2TexL, dwCount * sizeof(Vertex2TexL));
    memcpy(pDestModelSpacePos, tRenderData.m_pAsyncVerticesCloth, dwCount * sizeof(VertexCloth));
    GfxVBUnlock(vbidModelSpacePos);
    GfxVBUnlock(vbid);
}

// NON_MATCHING: 5 insns; loop pointer increments ordered differently (pvCloth stepped before the mapping pointer)











#line 1327
void Cloth::CopyToAsyncVertices()

{
    RActor * ptRActor = RActorGet(m_idRActor);
    const AppearanceDefinition * pdefAppearance = (const AppearanceDefinition *)gptGroupDefAppearance->LockR(ptRActor->snoAppearance, FALSE);
    const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[GEOSET_HIGHPOLY];


    const SubObject * ptSubObject = &tGeoSet.ptSubObjects[m_nSubObjectIndex];


    int32 nRenderVertCount =

        ((ptSubObject->dwFlags & 0x18000) != 0x8000)


            ? ptSubObject->nVertCount



            : ptSubObject->nPackVertCount;






    ClothRenderData & tRenderData = m_tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];

    tRenderData.m_dwAsyncVertexCount = nRenderVertCount;

    tRenderData.m_pAsyncVertices2TexL = CGameAsyncRenderGetMemory(nRenderVertCount * sizeof(Vertex2TexL));
    Vertex2TexL * pv = (Vertex2TexL *)tRenderData.m_pAsyncVertices2TexL;
    tRenderData.m_pAsyncVerticesCloth = CGameAsyncRenderGetMemory(nRenderVertCount * sizeof(VertexCloth));
    VertexCloth * pvCloth = (VertexCloth *)tRenderData.m_pAsyncVerticesCloth;


    uint16 * ptClothVertexMapping = ptSubObject->ptClothStructure->ptClothVertexMapping;



    const Vertex2TexLCPos * ptPackVertList = (const Vertex2TexLCPos *)(unsigned char *)ptSubObject->pPackVertList;


    for (int32 i = 0; i < nRenderVertCount; i++)
    {

        if (ptPackVertList)
        {
            const Vertex2TexLCPos * ptPackVertex = &ptPackVertList[i];
            ClothVertex * ptVertex = &m_tUpdateParams.tInputOutput.ptVertices[ptClothVertexMapping[i]];

            UByte4 normal = sVector3DNormalToUByte4(ptVertex->wvNormal);





            Vertex2TexLSet(
                pv[i],
                ptVertex->wp,
                normal,
                ptPackVertex->vcDiffuse,
                ptPackVertex->vcAuxColor,
                ptPackVertex->vpUV0,
                ptPackVertex->vpUV1);







            VertexClothSet(
                pvCloth[i],
                Vector3D(
                    ptPackVertex->nX * ptSubObject->wpFixedPointPosScale.x + ptSubObject->wpFixedPointPosOffset.x,
                    ptPackVertex->nY * ptSubObject->wpFixedPointPosScale.y + ptSubObject->wpFixedPointPosOffset.y,
                    ptPackVertex->nZ * ptSubObject->wpFixedPointPosScale.z + ptSubObject->wpFixedPointPosOffset.z));
        }
        else
        {

            const FatVertex * ptFatVertex = &ptSubObject->ptVertList[i];

            ClothVertex * ptVertex = &m_tUpdateParams.tInputOutput.ptVertices[ptClothVertexMapping[i]];



            UByte4 normal = sVector3DNormalToUByte4(ptVertex->wvNormal);





            Vertex2TexLSet(
                pv[i],
                ptVertex->wp,
                normal,
                ptFatVertex->rgbaColor[0],
                ptFatVertex->rgbaColor[1],
                ptFatVertex->vpUVSet[0],
                ptFatVertex->vpUVSet[1]);







            VertexClothSet(pvCloth[i], ptFatVertex->wpPos);
        }
    }






    gptGroupDefAppearance->UnlockR((void *&)pdefAppearance);
}
#line 1584
const Vector3D & Cloth::GetVertexPosition(uint16 wIndex) const {
    return m_tUpdateParams.tInputOutput.ptVertices[wIndex].wp;
}




inline BOOL Cloth::IsRendered() const {
    const GeoSetType eGeoSetType = 0;
    return (RActorGet(m_idRActor)->tGeoSetData[eGeoSetType].ptSubObjectState[m_nSubObjectIndex].dwFlags & 0x2) != 0;
}











inline BOOL Cloth::IsPendingDeletion() const {
    return (m_dwFlags & CLOTHFLAG_PENDING_DELETION) != 0;
}






inline void Cloth::SetPendingDeletion() {
    m_dwFlags |= CLOTHFLAG_PENDING_DELETION;
}















inline BOOL Cloth::IsVisible(const Camera & camera) const {
    if (!IsRendered()) {
        return FALSE;
    }
    RActor * ptRActor = RActorGet(m_idRActor);
    if (ptRActor->eContext != RENDERCONTEXT_DEFAULT) {
        return TRUE;
    }
    if (CameraComputeSphereVisibility(camera, GetBoundingSphere())) {
        return TRUE;
    }
    return (ptRActor->dwVisibilityFlags & 0x1) != 0;
}















void Cloth::DisablePlanes() {
    m_dwFlags |= CLOTHFLAG_PLANES_DISABLED;
}










void Cloth::Settle() {
    ScopedClothDefinitionLockR pdefCloth(m_snoCloth);
    m_dwFlags |= CLOTHFLAG_SETTLING;
    PreStep();
    Staple();
    for (int32 i = 0; i < pdefCloth->nPreStepCount; i++) {
        ClothUpdate(m_tUpdateParams.tInput, m_tUpdateParams.tInputOutput, 1.0f / 60.0f);
        CacheCollisionCapsules(pdefCloth);
    }
}
