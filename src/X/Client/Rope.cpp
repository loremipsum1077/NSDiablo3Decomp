// X/Client/Rope.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/r6-render/rope/frag.cpp by build/try/r6-render/gen.py.
#include <float.h>
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Rope.h"
#include "Attachments.h"
#include "Explosions.h"
#include "CGame.h"
#include "Definitions/RopeDefinition.h"
#include "RActors/RActors.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxIB.h"
#include "Gfx/GfxVB.h"
#include "../Common/Game.h"
#include "../Common/Definitions/AttachmentDefinition.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/Intersect.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Math/Curve.h"
extern VBFormatInfo cgtVBFormatInfo[NUM_VBFORMATS];  // Gfx/GfxVB.cpp
VBID GfxVBAddRef(const VBID vbid);  // Gfx/GfxVB.cpp
void GfxStripIBAlloc(const ResourceUsage eUsage, const int32 nLength, const BOOL fPurgeIfNecessary, int32 & nIndexCount, IBID & ibid);  // Gfx/GfxIB.cpp

void ParentObjectRegister(int nDepth, ParentType eType, DataID idObject, void * pObject, ACDID idACD);  // ParentObjects.cpp:723

enum {  // Rope.h:42
    MAX_KNOT_POINTS = 16,
    MAX_CURVE_POINTS = 64,
    MAX_ROPE_ATTACH_POINTS = 2,
    MAX_ROPE_CONSTRAINTS = 128,
};

enum {
    MAX_ROPES = 64,
};

enum {
    ROPE_INST_IGNORE_OWNER_ALPHA = 0,
    ROPE_PENDING_DELETION = 1,
    ROPE_VISIBLE_BIT = 2,
    ROPE_ALL_TEXTURE_LOADED = 3,
};


static void sUpdateARope(Rope * ptRope, const RopeDefinition * pdefRope, BOOL bFirstTime, float flDeltaTime);  // Rope.cpp:793
void PhysicsSystemClear(PhysicsSystem & tPhysics);  // Physics/Physics.cpp:191
void PhysicsSystemSetPoint(PhysicsSystem & tPhysics, SNO snoPhysics, RActorID idLocalRActor);  // Physics/Physics.cpp:174
void PhysicsSystemAwaken(PhysicsSystem & tPhysics, const int32 nInitialRelaxationSteps); void PhysicsSystemUpdate(PhysicsSystem & tPhysics, float flDeltaTime);  // Physics/Physics.cpp:209, :233
PointMass * PointMassCreate(PhysicsSystem & tPhysics);  // Physics/Physics.cpp:95
static void sRopeGetEndPositions(Rope * ptRope, const RopeDefinition * pdefRope, PRTransform * transformPin, InterpTime & tInterp, BOOL bFirstTime);  // Rope.cpp:345
void RActorGetCenterPoint(const RActor * ptRActor, Vector3D & wpCenter);  // RActors/RActors.cpp:6513
#include "TexAnimation.h"
#include "../Sigma/MemoryPools.h"
void TexAnimInitialize(const UberMaterial & tUberMaterial, TexAnimState * ptTexAnimState, RActor * ptRActor);  // TexAnimation.cpp:195
int32 MaterialGetTextureCount(const UberMaterial & tUberMaterial); uint32 TexAnimUpdate(const UberMaterial & tUberMaterial, const ObjectTexAnimParam * ptObjTexAnimParam, TexAnimState * ptTexAnimState, const int32 nTexAnimStateCount, float flDeltaTime, float flClientTime);  // Gfx/GfxMaterial.cpp:412, TexAnimation.cpp
#include "../Common/GameConstants.h"
#include "SubObjectGfx.h"
#include "Camera/Camera.h"
#include "Definitions/ShaderDefinition.h"
#include "Definitions/ShaderMapDefinition.h"
TexAnimState * TexAnimRenderAsyncCopy(int32 nCount, const TexAnimState * ptTexAnimState, const UberMaterial * ptUberMaterial, ShaderTex eOverlayShaderTex);  // TexAnimation.cpp:1124
#include "RWindow.h"
#include "PostFX.h"
#include "Camera/CameraManager.h"
#include "Gfx/GfxPrimitives.h"
#include "Definitions/PreferencesDefinition.h"
#include "Prefetch.h"
#include "../Sigma/Math/Plane.h"
BOOL CameraComputeSphereVisibility(const Camera & cam, const Sphere & ws); WorldPlace ClientPointToPlace(const Vector3D & wp); BOOL CScenesGetFloorHeight(SWorldID idSWorld, const Vector3D & wp, float & fFloorHeight);  // Camera/Camera.cpp:135, CWorld.cpp:1084, Scene.cpp:4553
static void sRopeEvaluate(Rope * ptRope); static void sRopeDebugDraw(Rope * ptRope);  // Rope.cpp:1685, 1703
struct RopeGlobals {
    int32 nIndexCount;  // 0x00
    IBID ibid;  // 0x04
    VBID vbid;  // 0x08
    TDataArray<Rope, RopeID> * ptRopes;  // 0x10
};


struct VertexBillboard {  // size 0x28; X/Client/Gfx/GfxVB.h:112 (DWARF), not in src/X/Client/Gfx/GfxVB.h
    Vector3D wpPos;  // 0x00
    UByte4 wvpNormal;  // 0x0c
    VertexColor vcDiffuse;  // 0x10
    VertexColor vcAuxColor;  // 0x14
    UByte4 vpUV0;  // 0x18
    UByte4 vpUV1;  // 0x1c
    UByte4 vpUV2;  // 0x20
    UByte4 vpUV3;  // 0x24
};
BOOL CWorldIsRetro();  // CWorld.cpp:1390
void GfxBuildPerspectiveProjectionMatrix(Matrix4x4 & m, const Angle aFOV, const float flAspectRatio, const float flNearPlane, const float flFarPlane, const BOOL fRightHanded); void GfxWorldMatrixSet(const Matrix4x4 & m);  // Gfx/Gfx.cpp:2205, :2053
void * GfxVBLock(const VBID vbid, const uint32 dwCount, uint32 & dwStartIndex);  // Gfx/GfxVB.cpp:454
void GfxShaderInit(const ShaderDefinition * pdefShader, const int32 nPassIndex); void GfxShaderSetTextureFactor(const RGBAColor & rgbaColor); void GfxShaderSetIB(const IBID ibid, const uint32 dwIndexOffset);  // Gfx/GfxShader.cpp:409, :1582, :1553
void GfxShaderSetStream(const StreamIndex eStreamIndex, const uint32 dwStreamOffset, const VBID vbid); void GfxShaderSetTextures(const UberMaterial & tUberMaterial, const TexAnimState * ptTexAnimState, const LPCSTR szSubObjectName, const SNO snoAppearance); void GfxShaderDraw(const DrawMode eDrawMode, const uint32 dwBaseIndexOffset, const uint32 dwMinVertexIndex, const uint32 dwNumVertices, const uint32 dwStartIndex, const uint32 dwPrimCount, const PerfBin ePerfBin);  // Gfx/GfxShader.cpp:1491, :986, :800
Rope * RopeGet(RopeID id)
{
    // get the rope
    RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
    return ptRopeGlobals->ptRopes->ElementGet(id);
}




Rope * RopeTryToGet(RopeID id)
{
    // try to get the rope
    RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
    return ptRopeGlobals->ptRopes->ElementTryToGet(id);
}




void RopesInitializeForGame()
{
    // allocate the globals
    RopeGlobals *& ptRopeGlobals = CGameGet().ptRopeGlobals;
    ptRopeGlobals = (RopeGlobals *)SigmaMemoryNew(sizeof(RopeGlobals), 4, AppGetCurrentMemoryInterface(), TRUE);

    ptRopeGlobals->ptRopes = TDataArrayCreateTracked<Rope, RopeID>("Rope", AppGetCurrentMemoryInterface(), MAX_ROPES);
}






void RopesDisposeForGame()
{
    // delete all the ropes
    RopesDeleteAll();

    RopeGlobals *& ptRopeGlobals = CGameGet().ptRopeGlobals;
    if (ptRopeGlobals->ptRopes)
    {
        TDataArrayDispose(ptRopeGlobals->ptRopes);
    }

    SigmaMemoryFree(ptRopeGlobals, AppGetCurrentMemoryInterface()); ptRopeGlobals = NULL;
}






void RopesGfxInit()
{
    // get a vertex buffer and a strip index buffer
    RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
    ptRopeGlobals->vbid = GfxVBAddRef(cgtVBFormatInfo[VBFORMAT_BILLBOARD].vbidPooled);
    GfxStripIBAlloc(RESOURCEUSAGE_IMMUTABLE, MAX_CURVE_POINTS, FALSE, ptRopeGlobals->nIndexCount, ptRopeGlobals->ibid);
}




void RopesGfxDispose()
{
    // release the vertex and index buffers
    RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
    GfxVBRelease(ptRopeGlobals->vbid);
    GfxIBRelease(ptRopeGlobals->ibid);
}




void RopesDeleteAll()
{
    // delete all the ropes
    RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
    if (ptRopeGlobals && ptRopeGlobals->ptRopes)
    {
        TDataArrayIterator<Rope> tIt;
        while (ptRopeGlobals->ptRopes->Iterate(tIt))
        {
            Rope * ptRope = tIt.ptElement;
            if (!(ptRope->dwRopeInstFlags & (1 << ROPE_PENDING_DELETION)))
            {
                RopeDelete(ptRope->id);
            }
        }

        ptRopeGlobals->ptRopes->DeleteAll();
    }
}












































static void sInitRopeMasses(
    Rope * ptRope)
{
    const RopeDefinition * pdefRope = (const RopeDefinition *)gptGroupDefRope->LockR(ptRope->snoRope, FALSE);

    float flMassPerNode = pdefRope->flMass / ptRope->nRopeKnots;
    for (int32 i = 0; i < ptRope->nRopeKnots; i++)
    {
        PointMass * pm = ptRope->ptKnotPoints[i].ptPointMass;


        if (i == 0 &&
            ptRope->tRopeAttachPoints[0].nRopeAttachType != ROPE_ATTACH_NONE)
        {
            pm->flInvMass = 0.0f;
        }

        else if (i == ptRope->nRopeKnots - 1 &&
                 ptRope->tRopeAttachPoints[1].nRopeAttachType != ROPE_ATTACH_NONE)
        {
            pm->flInvMass = 0.0f;
        }
        else
        {
            float flMass;
            if (i == ptRope->nRopeKnots - 1 && (pdefRope->dwRopeFlags & (1 << ROPEDEF_WEIGHT_END_BIT)))
            {

                flMass = pdefRope->flMassEnd;
            }
            else
            {
                flMass = flMassPerNode * (1.0f - (pdefRope->flMassTaperFactor * i) / (ptRope->nRopeKnots - 1));
            }

            if (flMass < 1e-6f)
            {
                pm->flInvMass = FLT_MAX;
            }
            else
            {
                pm->flInvMass = 1.0f / flMass;
            }
        }
    }
    gptGroupDefRope->UnlockR((void *&)pdefRope);
}




static void sRopeUpdateConstraints(
    Rope * ptRope,
    InterpTime & tInterp)
{
    PhysicsSystem & tPhysics = ptRope->tPhysics;

    const RopeDefinition * pdefRope = (const RopeDefinition *)gptGroupDefRope->LockR(ptRope->snoRope, FALSE);
    tPhysics.ptConstraints = ptRope->tConstraints;

    WorldDistance wdSeparation;
    pdefRope->pathSeparation.GetInterpolatedValueByTime(ptRope->flSeparationInterp, tInterp, wdSeparation);

    int32 nIndex = 0;
    for (int32 i = 0; i < ptRope->nRopeKnots - 1; i++)
    {

        DistanceConstraint & tConstraint = tPhysics.ptConstraints[nIndex];
        tConstraint.ptPointMassA = ptRope->ptKnotPoints[i].ptPointMass;
        tConstraint.ptPointMassB = ptRope->ptKnotPoints[i + 1].ptPointMass;
        tConstraint.wdRestLength = wdSeparation;
        nIndex++;
    }


    int32 nPointToConstrain = Math::FloatToInt(ptRope->nRopeKnots * pdefRope->flStiffness);
    int32 nPointsToSkip = nPointToConstrain;
    for (; nPointsToSkip > 1; nPointsToSkip--)
    {
        for (int32 i = 0; i < ptRope->nRopeKnots - nPointsToSkip; i++)
        {

            DistanceConstraint & tConstraint = tPhysics.ptConstraints[nIndex];
            tConstraint.ptPointMassA = ptRope->ptKnotPoints[i].ptPointMass;
            tConstraint.ptPointMassB = ptRope->ptKnotPoints[i + nPointsToSkip].ptPointMass;
            tConstraint.wdRestLength = wdSeparation * nPointsToSkip;
            nIndex++;
        }
    }

    if (ptRope->ptPointMassHardpoint)
    {
        nPointToConstrain = Math::Max(nPointToConstrain - 1, 1);

        ptRope->flHardpointConstraintDistance = nPointToConstrain * wdSeparation;


        DistanceConstraint & tConstraint = tPhysics.ptConstraints[nIndex++];
        tConstraint.ptPointMassA = ptRope->ptPointMassHardpoint;
        tConstraint.ptPointMassB = ptRope->ptKnotPoints[nPointToConstrain].ptPointMass;
        tConstraint.wdRestLength = 0.0f;
    }

    gptGroupDefRope->UnlockR((void *&)pdefRope);

    tPhysics.nConstraintCount = nIndex;
}





static void sRopeGetEndPositions(
    Rope * ptRope,
    const RopeDefinition * pdefRope,
    PRTransform * transformPin,
    InterpTime & tInterp,
    BOOL bFirstTime)
{
    WorldDistance wdSeparation;
    pdefRope->pathSeparation.GetInterpolatedValueByTime(ptRope->flSeparationInterp, tInterp, wdSeparation);

    for (int32 i = 0; i < MAX_ROPE_ATTACH_POINTS; i++)
    {
        RopeAttachPoint & tRopeAttach = ptRope->tRopeAttachPoints[i];

        switch (tRopeAttach.nRopeAttachType)
        {
        case ROPE_ATTACH_NONE:
            transformPin[i] = cgtPRTransformIdentity;

            if (i == 1)
            {
                if (ptRope->tPhysics.snoPhysics != -1 && !bFirstTime)
                {


                    PointMass & tpmDest = *ptRope->ptKnotPoints[ptRope->nRopeKnots - 1].ptPointMass;
                    transformPin[i].wp = tpmDest.wp;
                }
                else
                {



                    Vector3D wvDirection = transformPin[0].q.Rotate(cgwvXAxis);
                    transformPin[i].wp = transformPin[0].wp + wvDirection * wdSeparation * ptRope->nRopeKnots;
                }
            }
            break;



        case ROPE_ATTACH_PARENT_CENTER:
            if (tRopeAttach.tParentData.ParentIsValid())
            {
                const RActor * ptRActorParent = RActorGet((RActorID)tRopeAttach.tParentData.m_idParent);
                RActorGetCenterPoint(ptRActorParent, transformPin[i].wp);
                transformPin[i].q = cgqIdentity;
            }
            else
            {
                tRopeAttach.nRopeAttachType = ROPE_ATTACH_NONE;
            }
            break;

        case ROPE_ATTACH_PARENT:

            if (tRopeAttach.tParentData.ParentIsValid())
            {
                tRopeAttach.tParentData.GetTransform(transformPin[i]);
            }
            else
            {
                tRopeAttach.nRopeAttachType = ROPE_ATTACH_NONE;
            }
            break;

        case ROPE_ATTACH_WORLDPOINT:
            transformPin[i].Set(cgqIdentity, tRopeAttach.wpAttach);
            break;

        default:
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }
}




void RopeGetHardpointOrientation(
    const Rope * ptRope,
    const int32 nKnotIndex,
    PRTransform & transform)
{
    int32 nKnotNumber = Math::Min(nKnotIndex, ptRope->nRopeKnots - 1);

    int32 nKnotNext;
    BOOL bLast = nKnotIndex >= ptRope->nRopeKnots - 1;

    nKnotNext = bLast ? nKnotNumber - 1 : nKnotNumber + 1;





    PointMass * ptPointMass0 = ptRope->ptKnotPoints[nKnotNumber].ptPointMass;
    PointMass * ptPointMass1 = ptRope->ptKnotPoints[nKnotNext].ptPointMass;
    PointMass & tpm0 = *ptPointMass0;
    PointMass & tpm1 = *ptPointMass1;
    transform.wp = tpm0.wp;
    Vector3D wpNext(tpm1.wp);
    Vector3D wvAlongRope;

    if (bLast)
    {

        wvAlongRope = transform.wp - wpNext;
    }
    else
    {
        wvAlongRope = wpNext - transform.wp;
    }

    WorldDistance wdMag = wvAlongRope.Normalize();

    if (wdMag < 1e-6f)
    {

        transform.q = cgqIdentity;
        return;
    }


    Vector3D wvRight = Vector3D::CrossProduct(cgwvZAxis, wvAlongRope);

    WorldDistance wdMagRight = wvRight.Normalize();
    if (wdMagRight < 1e-6f)
    {
        transform.q = cgqIdentity;
        return;
    }


    Vector3D wvFacingUp = Vector3D::CrossProduct(wvRight, wvAlongRope);
    wvFacingUp.Normalize();

    transform.q.FromChangeOfBasis(cgwvXAxis, cgwvYAxis, cgwvZAxis, wvAlongRope, wvRight, -wvFacingUp);






}




static void sRopeAttachmentCreateAtKnot(
    Rope * ptRope,
    int32 nKnotNumber,
    RMessage & msg)
{
    msg.tHardpoints[0].nBoneIndex = nKnotNumber;

    PRTransform transform;
    RopeGetHardpointOrientation(ptRope, nKnotNumber, transform);

    ParentData tParentData;
    tParentData.SetKnot(ptRope->id, nKnotNumber);

    AttachmentCreate(msg, tParentData, transform, 1.0f, FALSE);
}




static void sRopeAttachmentCreate(
    const RopeID idRope,
    RMessage & msg)
{
    Rope * ptRope = RopeTryToGet(idRope);
    if (ptRope == NULL)
    {
        return;
    }

    HardpointHash sHardPointName(msg.tTriggerEvent.tHardpointLinks[0].tHash);

    int32 nRopeHardpoint = ROPE_HARDPOINT_HEAD;

    if (sHardPointName == "Tail")
    {
        nRopeHardpoint = ROPE_HARDPOINT_TAIL;
    }
    else if (sHardPointName == "Knots")
    {
        nRopeHardpoint = ROPE_HARDPOINT_KNOTS;
    }

    switch (nRopeHardpoint)
    {
    case ROPE_HARDPOINT_HEAD:
        sRopeAttachmentCreateAtKnot(ptRope, 0, msg);
        break;

    case ROPE_HARDPOINT_TAIL:
        sRopeAttachmentCreateAtKnot(ptRope, ptRope->nRopeKnots - 1, msg);
        break;

    case ROPE_HARDPOINT_KNOTS:
        for (int32 i = 0; i < ptRope->nRopeKnots; i++)
        {
            sRopeAttachmentCreateAtKnot(ptRope, i, msg);
        }
        break;
    }
}







static void sHandleMsg(
    const DataID id,
    RMessage & msg)
{
    RopeID idRope = (RopeID)id;
    Rope * ptRope = RopeGet(idRope);

    const TriggerConditions & tConditions = msg.tTriggerEvent.tConditions;

    if (tConditions.dwRepeatTimeMin != 0 || tConditions.dwRepeatTimeDelta != 0)
    {

        BOOL bRemote = (msg.tTriggerEvent.dwTriggerFlags >> TRIGGERFLAG_DISABLED_IF_REMOTE_PLAYER) & 1;
        ptRope->tRMsgList.SendMessage(PARENT_ROPE, idRope, TRUE, msg.tTriggerEvent, 0.0f, bRemote);
    }

    switch (msg.tTriggerEvent.eType)
    {
    case TRIGGEREDEVENTTYPE_ADD_OBJECT:
        sRopeAttachmentCreate(idRope, msg);
        break;

    case TRIGGEREDEVENTTYPE_DETACH_OBJECT:
        AttachmentFindAndDetach(ptRope->tAttachmentList, msg.tTriggerEvent.dwID);
        break;

    case TRIGGEREDEVENTTYPE_DELETE_OBJECT:
        AttachmentFindAndDelete(ptRope->tAttachmentList, msg.tTriggerEvent.dwID, FALSE);
        break;

    case TRIGGEREDEVENTTYPE_OUTRO_OBJECT:
        AttachmentFindAndDelete(ptRope->tAttachmentList, msg.tTriggerEvent.dwID, TRUE);
        break;

    default:
        break;
    }


}




static void sRopeAttachmentsOnMsg(const RopeID idRope, const uint32 dwMsgKey)
{
    Rope * ptRope = RopeTryToGet(idRope);
    if (ptRope)
    {
        const RopeDefinition * pdefRope = (const RopeDefinition *)gptGroupDefRope->LockR(ptRope->snoRope, FALSE);
        for (int32 i = 0; i < pdefRope->nMsgTriggeredEventCount; i++)
        {
            const MsgTriggeredEvent & tEvent = pdefRope->ptMsgTriggeredEvents[i];
            if (tEvent.dwMsgKey == dwMsgKey)
            {
                ptRope->tRMsgList.SendMessage(PARENT_ROPE, idRope, FALSE, tEvent.tTriggerEvent, 0.0f, FALSE);
            }
        }
        gptGroupDefRope->UnlockR((void *&)pdefRope);
    }
}














static void sRopeUpdatePosition(
    Rope * ptRope,
    const RopeDefinition * pdefRope,
    PRTransform * transformPin,
    BOOL bFirstTime,
    float flDeltaTime)
{
    SigmaMemoryCopy(ptRope->transformPin, transformPin, sizeof(ptRope->transformPin));



    if (ptRope->tPhysics.snoPhysics != -1 && !bFirstTime)
    {
        PointMass & tpmSource = *ptRope->ptKnotPoints[0].ptPointMass;
        PointMass & tpmDest = *ptRope->ptKnotPoints[ptRope->nRopeKnots - 1].ptPointMass;
        tpmSource.wp = transformPin[0].wp;
        tpmDest.wp = transformPin[1].wp;
    }
    else
    {



        Vector3D wvToNext = transformPin[1].wp - transformPin[0].wp;
        wvToNext *= 1.0f / (ptRope->nRopeKnots - 1);

        PRTransform transformAlongLine;
        transformAlongLine.Set(transformPin[0].wp, transformPin[1].wp, cgwvZAxis);

        float flJitterScale = 1.0f;
        if (pdefRope->dwRopeFlags & (1 << ROPEDEF_SCALE_JITTER_BY_ROPE_LENGTH_BIT))
        {
            float flDistance = Vector3D::Distance(transformPin[0].wp, transformPin[1].wp);
            flJitterScale = Math::Min(flDistance / pdefRope->flExpectedLength, 1.0f);
        }

        float flPhase = ptRope->flTime * (ptRope->flPhaseSpeed * FULL_CIRCLE);

        for (int32 i = 0; i < ptRope->nRopeKnots; i++)
        {
            RopeKnotPoint & tRopeKnotPoint = ptRope->ptKnotPoints[i];
            PointMass & pm = *tRopeKnotPoint.ptPointMass;

            pm.wp = transformPin[0].wp + wvToNext * (float)i;

            pm.wp += transformAlongLine.Rotate(tRopeKnotPoint.wvJitter) * flJitterScale;



            Angle ang = ((float)i / (float)(ptRope->nRopeKnots - 1)) * FULL_CIRCLE * ptRope->flFreq + flPhase;

            InterpTickTime tInterp;
            tInterp.Init();

            tInterp.tTickTime = i;
            tInterp.tScale = ptRope->nRopeKnots - 1;
            tInterp.nTimeType = TICK_TIME_NON_LOOPING;

            float flXAmp, flYAmp;
            pdefRope->pathXAmpOverLength.GetInterpolatedValueByTickTime(ptRope->flXAmpOverLengthInterp, tInterp, flXAmp);
            pdefRope->pathYAmpOverLength.GetInterpolatedValueByTickTime(ptRope->flYAmpOverLengthInterp, tInterp, flYAmp);

            Vector3D wvSpiral(
                Math::Cos(ang) * flXAmp, Math::Sin(ang) * flYAmp, 0.0f);
            pm.wp += transformAlongLine.Rotate(wvSpiral);
        }
    }


    ptRope->tRMsgList.Update(flDeltaTime);



    if (ptRope->ptPointMassHardpoint)
    {
        PointMass & pm = *ptRope->ptPointMassHardpoint;
        PointMass & pm0 = *ptRope->ptKnotPoints[0].ptPointMass;

        Vector3D wvDirection = transformPin[0].q.Rotate(cgwvXAxis);
        pm.wp = pm0.wp + wvDirection * ptRope->flHardpointConstraintDistance;
    }



    ptRope->ws.wpCenter = (transformPin[0].wp + transformPin[1].wp) * 0.5f;
    ptRope->ws.wdRadius = Vector3D::Distance(transformPin[0].wp, transformPin[1].wp);
}




static void sRopeCalcKnotJitter(
    Rope * ptRope,
    const RopeDefinition * pdefRope,
    int32 nKnotIndex,
    Vector3D & wvJitterTarget)
{
    InterpTickTime tKnotInterp;
    tKnotInterp.Init();

    tKnotInterp.tTickTime = nKnotIndex;
    tKnotInterp.tScale = ptRope->nRopeKnots - 1;
    tKnotInterp.nTimeType = TICK_TIME_NON_LOOPING;

    RNG & rng = CGameGlobalsGet().rngLocal;

    Vector3D wvJitterAmplitude;
    pdefRope->pathJitterAmplitude.GetInterpolatedValueByTickTime(ptRope->wvJitterAmplitudeInterp, tKnotInterp, wvJitterAmplitude);

    wvJitterTarget.Set(
        RandRangeFloat_Internal(rng, -wvJitterAmplitude.x, wvJitterAmplitude.x * 2.0f),
        RandRangeFloat_Internal(rng, -wvJitterAmplitude.y, wvJitterAmplitude.y * 2.0f),
        RandRangeFloat_Internal(rng, -wvJitterAmplitude.z, wvJitterAmplitude.z * 2.0f));
}




static BOOL sValidateParents(
    const Rope * ptRope)
{
    for (int32 i = 0; i < MAX_ROPE_ATTACH_POINTS; i++)
    {
        const RopeAttachPoint & tRopeAttach = ptRope->tRopeAttachPoints[i];

        if (tRopeAttach.nRopeAttachType == ROPE_ATTACH_PARENT || tRopeAttach.nRopeAttachType == ROPE_ATTACH_PARENT_CENTER)
        {

            if (tRopeAttach.tParentData.m_eParentType != PARENT_NONE &&
                !tRopeAttach.tParentData.ParentIsValid())
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}





static RActorID sGetRActorIDAttachedTo(Rope * ptRope)
{
    RopeAttachPoint & tRopeAttach = ptRope->tRopeAttachPoints[0];
    if (ParentIsRActor(tRopeAttach.tParentData.m_eParentType))
    {
        return (RActorID)tRopeAttach.tParentData.m_idParent;
    }

    return RACTORID_INVALID;
}





static void sUpdateARope(
    Rope * ptRope,
    const RopeDefinition * pdefRope,
    BOOL bFirstTime,
    float flDeltaTime)
{
    if (!bFirstTime)
    {
        if (ptRope->fFixedUpdate)
        {
            ptRope->flTime += flDeltaTime;
        }
        else
        {
            float flCurrentTime = CGameGetClientTime();
            if (ptRope->flLastUpdateTime == flCurrentTime)
            {
                return;
            }
            ptRope->flTime += flCurrentTime - ptRope->flLastUpdateTime;
            ptRope->flLastUpdateTime = flCurrentTime;
        }
    }


    if (!sValidateParents(ptRope))
    {
        RopeDelete(ptRope->id);
        return;
    }

    uint32 dwResultFlags = TexAnimUpdate(pdefRope->tUberMaterial, NULL, ptRope->tTexAnimStateList, NUM_SHADER_TEXTURES, flDeltaTime, 0.0f);

    if ((pdefRope->dwRopeFlags & (1 << ROPEDEF_DELETE_AT_ANIMATION_END_BIT)) && (dwResultFlags & 1))
    {
        RopeDelete(ptRope->id);
        return;
    }


    InterpTime tInterpTime;
    tInterpTime.Init();

    tInterpTime.flTime = ptRope->flTime;
    tInterpTime.flScale = ptRope->flSystemDuration;

    if (ptRope->bOutro)
    {
        ptRope->flOutroTime += flDeltaTime;

        ptRope->flOutroCrossFade = ptRope->flOutroTime / ptRope->flOutroDuration;
        ptRope->flOutroCrossFade = Math::Pin(ptRope->flOutroCrossFade, 0.0f, 1.0f);
    }
    else
    {
        ptRope->flOutroCrossFade = 0.0f;
    }

    if (pdefRope->dwRopeFlags & (1 << ROPEDEF_INFINITE_DURATION_BIT))
    {
        if (ptRope->bOutro)
        {
            if (ptRope->flOutroTime >= ptRope->flOutroDuration)
            {
                RopeDelete(ptRope->id);
                return;
            }

            tInterpTime.flRatio = ptRope->flOutroCrossFade;
            tInterpTime.flOutro = ptRope->flOutroCrossFade;
            tInterpTime.nTimeType = TICK_TIME_OUTRO_CROSSFADE;
        }
        else
        {
            tInterpTime.nTimeType = TICK_TIME_LOOPING;
        }
    }
    else
    {
        if (ptRope->flTime >= ptRope->flSystemDuration)
        {
            RopeDelete(ptRope->id);
            return;
        }

        tInterpTime.nTimeType = TICK_TIME_NON_LOOPING;
    }


    InterpTime tInterpTimeNoCrossFace;
    tInterpTimeNoCrossFace = tInterpTime;
    tInterpTimeNoCrossFace.flRatio = 1.0f;

    PRTransform transformPin[2];
    sRopeGetEndPositions(ptRope, pdefRope, transformPin, tInterpTime, bFirstTime);

    Vector3D wvAlongRope = transformPin[1].wp - transformPin[0].wp;

    Vector3D wvPerpendicular = Vector3D::CrossProduct(wvAlongRope, cgwvZAxis);
    wvPerpendicular.Normalize();

    RGBAColor rgbaColorOverTime;
    pdefRope->pathColorOverTime.GetInterpolatedValueByTime(ptRope->flColorOverTimeInterp, tInterpTime, rgbaColorOverTime);

    pdefRope->pathAlphaOverTime.GetInterpolatedValueByTime(ptRope->flAlphaOverTimeInterp, tInterpTime, ptRope->flRopeAlpha);

    pdefRope->pathFreqOverTime.GetInterpolatedValueByTime(ptRope->flFreqOverTimeInterp, tInterpTime, ptRope->flFreq);
    pdefRope->pathPhaseSpeedOverTime.GetInterpolatedValueByTime(ptRope->flPhaseSpeedOverTimeInterp, tInterpTime, ptRope->flPhaseSpeed);




    if (!(ptRope->dwRopeInstFlags & (1 << ROPE_INST_IGNORE_OWNER_ALPHA)))
    {
        if (pdefRope->dwRopeFlags & (1 << ROPEDEF_PICK_MAX_ENDPOINT_ALPHA_BIT))
        {
            ptRope->flRopeAlpha *= Math::Max(ptRope->tRopeAttachPoints[1].tParentData.GetAlpha(), ptRope->tRopeAttachPoints[0].tParentData.GetAlpha());
        }
        else
        {
            ptRope->flRopeAlpha *= ptRope->tRopeAttachPoints[0].tParentData.GetAlpha();
        }
    }

    if (ptRope->flRopeAlpha < 0.999999f)
    {
        rgbaColorOverTime.a = (uint8)Math::FloatRoundToInt(ptRope->flRopeAlpha * 255.0f);
    }

    float flTimeWidth;
    pdefRope->pathWidthOverTime.GetInterpolatedValueByTime(ptRope->flWidthOverTimeInterp, tInterpTime, flTimeWidth);

    for (int32 i = 0; i < ptRope->nCurvePoints; i++)
    {
        RopeCurvePoint & tRopeCurvePoint = ptRope->ptCurvePoints[i];

        InterpTickTime tCurveInterp;
        tCurveInterp.Init();

        tCurveInterp.tTickTime = i;
        tCurveInterp.tScale = ptRope->nCurvePoints - 1;
        tCurveInterp.nTimeType = TICK_TIME_NON_LOOPING;

        float flLengthWidth;
        pdefRope->pathWidthOverLength.GetInterpolatedValueByTickTime(ptRope->flWidthOverLengthInterp, tCurveInterp, flLengthWidth);

        tRopeCurvePoint.wdRopeWidth = flTimeWidth * flLengthWidth;

        RGBAColor rgbaColorOverLength;
        pdefRope->pathColorOverLength.GetInterpolatedValueByTickTime(ptRope->flColorOverLengthInterp, tCurveInterp, rgbaColorOverLength);

        float flAlphaOverLength;
        pdefRope->pathAlphaOverLength.GetInterpolatedValueByTickTime(ptRope->flAlphaOverLengthInterp, tCurveInterp, flAlphaOverLength);

        if (flAlphaOverLength < 0.999999f)
        {
            rgbaColorOverLength.a = (uint8)Math::FloatRoundToInt(flAlphaOverLength * 255.0f);
        }

        tRopeCurvePoint.rgbaCurvePointColor = rgbaColorOverLength * rgbaColorOverTime;
    }

    float flJitterRate;
    pdefRope->pathJitterRate.GetInterpolatedValueByTime(ptRope->flJitterRateInterp, tInterpTime, flJitterRate);

    ptRope->flJitterRateAccumulator += flJitterRate;

    BOOL bJitterThisTick = (ptRope->flJitterRateAccumulator > 1.0f) | bFirstTime;
    if (ptRope->flJitterRateAccumulator > 1.0f)
    {

        ptRope->flJitterRateAccumulator -= 1.0f;
    }

    for (int32 i = 0; i < ptRope->nRopeKnots; i++)
    {
        RopeKnotPoint & tRopeKnotPoint = ptRope->ptKnotPoints[i];

        if (bJitterThisTick)
        {
            sRopeCalcKnotJitter(ptRope, pdefRope, i, tRopeKnotPoint.wvJitterTarget);
        }

        tRopeKnotPoint.wvJitter += (tRopeKnotPoint.wvJitterTarget - tRopeKnotPoint.wvJitter) * pdefRope->flJitterApproachSpeed;
    }

    float flSourcePointWorldUp;
    pdefRope->pathSourcePointWorldUpMotion.GetInterpolatedValueByTime(ptRope->flSourcePointWorldUpMotionInterp, tInterpTime, flSourcePointWorldUp);

    transformPin[0].wp += cgwvZAxis * flSourcePointWorldUp;

    float flSourcePointAlongRope;
    pdefRope->pathSourcePointAlongRopeMotion.GetInterpolatedValueByTime(ptRope->flSourcePointAlongRopeMotionInterp, tInterpTime, flSourcePointAlongRope);

    transformPin[0].wp += wvAlongRope * flSourcePointAlongRope;

    float flSourcePointPerpendicular;
    pdefRope->pathSourcePointPerpendicularMotion.GetInterpolatedValueByTime(ptRope->flSourcePointPerpendicularMotionInterp, tInterpTime, flSourcePointPerpendicular);

    transformPin[0].wp += wvPerpendicular * flSourcePointPerpendicular;

    float flDestPointWorldUp;
    pdefRope->pathDestPointWorldUpMotion.GetInterpolatedValueByTime(ptRope->flDestPointWorldUpMotionInterp, tInterpTime, flDestPointWorldUp);

    transformPin[1].wp += cgwvZAxis * flDestPointWorldUp;

    float flDestPointAlongRope;
    pdefRope->pathDestPointAlongRopeMotion.GetInterpolatedValueByTime(ptRope->flDestPointAlongRopeMotionInterp, tInterpTime, flDestPointAlongRope);

    transformPin[1].wp -= wvAlongRope * flDestPointAlongRope;

    float flDestPointPerpendicular;
    pdefRope->pathDestPointPerpendicularMotion.GetInterpolatedValueByTime(ptRope->flDestPointPerpendicularMotionInterp, tInterpTime, flDestPointPerpendicular);

    transformPin[1].wp += wvPerpendicular * flDestPointPerpendicular;

    if (pdefRope->dwRopeFlags & (1 << ROPEDEF_SNAP_DESTINATION_TO_RENDERED_FLOOR_BIT))
    {
        WorldPlace place = ClientPointToPlace(transformPin[1].wp);
        CScenesGetFloorHeight(place.idSWorld, place.wp, transformPin[1].wp.z);
    }





    sRopeUpdatePosition(ptRope, pdefRope, transformPin, bFirstTime, flDeltaTime);

    sRopeUpdateConstraints(ptRope, tInterpTime);

    PhysicsSystemUpdate(ptRope->tPhysics, flDeltaTime);


}




BOOL sIsTopmostParentInUI(Rope * ptRope)
{
    RActorID idRActor = sGetRActorIDAttachedTo(ptRope);
    if (idRActor != RACTORID_INVALID)
    {
        RActorID idParent = RActorGetTopmostOwner(idRActor, FALSE, TRUE);
        if (idParent != RACTORID_INVALID)
        {
            RActor * ptRActorParent = RActorTryToGet(idParent);
            if (ptRActorParent)
                return ptRActorParent->dwFlagsEx & (1 << RACTOREX_FIXED_UPDATE_BIT);
        }
    }
    return FALSE;
}


static int32 sRegisterParentObject(Rope * ptRope)
{
    float flCurrentTime = (ptRope->fFixedUpdate && (GameIsPaused() || GameIsSoftPaused())) ? CGameGetRealTime() + CGameGetRealDeltaTime() : CGameGetClientTime() + CGameGetClientDeltaTime();
    if (ptRope->flLastRegisterTime == flCurrentTime)
        return ptRope->nSimTreeDepth;

    ptRope->flLastRegisterTime = flCurrentTime;

    int32 nParentDepth = -1;
    for (int32 i = 0; i < MAX_ROPE_ATTACH_POINTS; i++)
    {
        const RopeAttachPoint & tRopeAttach = ptRope->tRopeAttachPoints[i];
        if ((tRopeAttach.nRopeAttachType == ROPE_ATTACH_PARENT_CENTER || tRopeAttach.nRopeAttachType == ROPE_ATTACH_PARENT) && tRopeAttach.tParentData.m_eParentType != PARENT_NONE)
        {
            int nCurrentParentDepth = tRopeAttach.tParentData.RegisterParentObject();
            nParentDepth = Math::Max(nParentDepth, nCurrentParentDepth);
        }
    }

    ptRope->nSimTreeDepth = nParentDepth + 1;
    ParentObjectRegister(ptRope->nSimTreeDepth, PARENT_ROPE, ptRope->id, ptRope, ACDID_INVALID);

    return ptRope->nSimTreeDepth;
}










int32 RopeRegisterParentObject(const RopeID idRope)
{
    Rope * ptRope = RopeGet(idRope);
    return sRegisterParentObject(ptRope);
}





void RopeRegisterParentObjects(BOOL fFixedUpdateOnly)
{
    RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
    TDataArrayIterator<Rope> tIt;
    while (ptRopeGlobals->ptRopes->Iterate(tIt))
    {
        Rope * ptRope = tIt.ptElement;
        if (!(ptRope->dwRopeInstFlags & (1 << ROPE_PENDING_DELETION)) && (!fFixedUpdateOnly || ptRope->fFixedUpdate))
        {
            sRegisterParentObject(ptRope);
        }
    }
}














void RopeUpdate(
    Rope * ptRope,
    float flDeltaTime)
{

    const RopeDefinition * pdefRope = (const RopeDefinition *)gptGroupDefRope->LockR(ptRope->snoRope, FALSE);
    sUpdateARope(ptRope, pdefRope, FALSE, flDeltaTime);
    gptGroupDefRope->UnlockR((void *&)pdefRope);
}




static void sRopeSetNumberOfKnots(
    Rope * ptRope,
    const RopeDefinition * pdefRope,
    InterpTime & tInterp,
    float flDeltaTime)
{
    PhysicsSystem & tPhysics = ptRope->tPhysics;

    int32 nNumKnots;

    if (pdefRope->dwRopeFlags & (1 << ROPEDEF_USE_KNOT_COUNT_PER_FOOT))
    {
        PRTransform transformPin[2];
        sRopeGetEndPositions(ptRope, pdefRope, transformPin, tInterp, TRUE);

        WorldDistance wdSeparation = Vector3D::Distance(transformPin[0].wp, transformPin[1].wp);

        nNumKnots = Math::FloatToInt(wdSeparation * pdefRope->flKnotsPerFoot);
    }
    else
    {
        nNumKnots = pdefRope->nWidth;
    }

    nNumKnots = Math::Pin<int32, int32>(nNumKnots, 2, MAX_KNOT_POINTS);

    if (nNumKnots != ptRope->nRopeKnots)
    {






        tPhysics.ptConstraints = NULL;

        PhysicsSystemClear(tPhysics);
        PhysicsSystemSetPoint(tPhysics, pdefRope->snoPhysics, sGetRActorIDAttachedTo(ptRope));

        ptRope->nRopeKnots = nNumKnots;
        ptRope->nCurvePoints = nNumKnots * 4;
        ptRope->nVertCount = nNumKnots * 8;

        PRTransform transformPin[2];
        sRopeGetEndPositions(ptRope, pdefRope, transformPin, tInterp, TRUE);

        for (int32 i = 0; i < ptRope->nRopeKnots; i++)
        {
            ptRope->ptKnotPoints[i].ptPointMass = PointMassCreate(tPhysics);

            PointMass * pm = ptRope->ptKnotPoints[i].ptPointMass;
            pm->wp = transformPin[0].wp;
        }

        ptRope->ptPointMassHardpoint = NULL;
        if (pdefRope->dwRopeFlags & (1 << ROPEDEF_CONSTRAIN_TO_HARDPOINT_ORIENTATION_BIT))
        {
            ptRope->ptPointMassHardpoint = PointMassCreate(tPhysics);


            ptRope->ptPointMassHardpoint->flInvMass = 0.0f;
        }

        sInitRopeMasses(ptRope);

        sRopeUpdateConstraints(ptRope, tInterp);

        PhysicsSystemAwaken(tPhysics, 50);

        sUpdateARope(ptRope, pdefRope, TRUE, 1.0f / 60.0f);
    }
}




















RopeID RopeAdd(
    const SNO snoRope,
    RopeAttachPoint * ptRopeAttachPoints,
    BOOL fIgnoreOwnerAlpha,
    const RenderContext eContext)
{
    RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
    ScopedRopeDefinitionLockR pdefRope(snoRope);

    if (pdefRope->nWidth < 2)
    {
        return ROPEID_INVALID;
    }


    RopeID id = ptRopeGlobals->ptRopes->ElementNew();
    if (id == ROPEID_INVALID)
    {
        return ROPEID_INVALID;
    }











    Rope * ptRope = RopeGet(id);


    ptRope->dwLastFrameSubmitted = CGameAsyncGetDrawFrameCount() - 1;
    ptRope->snoRope = snoRope;


    ptRope->dwRopeInstFlags = 0;
    ptRope->eContext = eContext;

    if (fIgnoreOwnerAlpha)
    {
        ptRope->dwRopeInstFlags |= (1 << ROPE_INST_IGNORE_OWNER_ALPHA);
    }

    SigmaMemoryCopy(ptRope->tRopeAttachPoints, ptRopeAttachPoints, sizeof(ptRope->tRopeAttachPoints));

    ptRope->fFixedUpdate = sIsTopmostParentInUI(ptRope);

    ptRope->tAttachmentList.SetPool(MemoryPoolsGetDATAIDListPool());
    ptRope->tRMsgList.Initialize(ptRope->id, sHandleMsg);



    RNG & rng = CGameGlobalsGet().rngLocal;

    ptRope->flTime = 0.0f;
    ptRope->bOutro = FALSE;
    ptRope->flOutroTime = 0.0f;
    ptRope->flJitterRateAccumulator = 0.0f;

    ptRope->flSystemDuration = (float)RandRange_Internal(rng, pdefRope->tSystemDurationMin, pdefRope->tSystemDurationDelta) * OO_TICKS_PER_SECOND;
    ptRope->flOutroDuration = (float)RandRange_Internal(rng, pdefRope->tOutroDurationMin, pdefRope->tOutroDurationDelta) * OO_TICKS_PER_SECOND;

    ptRope->flSystemDuration = Math::Max(ptRope->flSystemDuration, OO_TICKS_PER_SECOND);

    ptRope->flColorOverTimeInterp = pdefRope->pathColorOverTime.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flColorOverLengthInterp = pdefRope->pathColorOverLength.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flAlphaOverTimeInterp = pdefRope->pathAlphaOverTime.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flAlphaOverLengthInterp = pdefRope->pathAlphaOverLength.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flWidthOverTimeInterp = pdefRope->pathWidthOverTime.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flWidthOverLengthInterp = pdefRope->pathWidthOverLength.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flSourcePointWorldUpMotionInterp = pdefRope->pathSourcePointWorldUpMotion.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flSourcePointAlongRopeMotionInterp = pdefRope->pathSourcePointAlongRopeMotion.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flSourcePointPerpendicularMotionInterp = pdefRope->pathSourcePointPerpendicularMotion.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flDestPointWorldUpMotionInterp = pdefRope->pathDestPointWorldUpMotion.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flDestPointAlongRopeMotionInterp = pdefRope->pathDestPointAlongRopeMotion.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flDestPointPerpendicularMotionInterp = pdefRope->pathDestPointPerpendicularMotion.tHeader.GetInterpolantFromDistributionFunction(rng);

    pdefRope->pathJitterAmplitude.tHeader.GetInterpolantVectorFromDistributionFunction(rng, ptRope->wvJitterAmplitudeInterp);
    ptRope->flJitterRateInterp = pdefRope->pathJitterRate.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flSeparationInterp = pdefRope->pathSeparation.tHeader.GetInterpolantFromDistributionFunction(rng);

    ptRope->flXAmpOverLengthInterp = pdefRope->pathXAmpOverLength.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flYAmpOverLengthInterp = pdefRope->pathYAmpOverLength.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flFreqOverTimeInterp = pdefRope->pathFreqOverTime.tHeader.GetInterpolantFromDistributionFunction(rng);
    ptRope->flPhaseSpeedOverTimeInterp = pdefRope->pathPhaseSpeedOverTime.tHeader.GetInterpolantFromDistributionFunction(rng);

    InterpTime tInterp;
    tInterp.Init();
    tInterp.nTimeType = TICK_TIME_NON_LOOPING;
    tInterp.flTime = 0.0f;
    tInterp.flScale = ptRope->flSystemDuration;



    ptRope->tTexAnimStateList = new TexAnimState[MaterialGetTextureCount(pdefRope->tUberMaterial)];

    ptRope->ptKnotPoints = new RopeKnotPoint[MAX_KNOT_POINTS];
    SigmaMemoryClear(ptRope->ptKnotPoints, sizeof(RopeKnotPoint) * MAX_KNOT_POINTS);

    ptRope->ptCurvePoints = new RopeCurvePoint[MAX_CURVE_POINTS];
    SigmaMemoryClear(ptRope->ptCurvePoints, sizeof(RopeCurvePoint) * MAX_CURVE_POINTS);

    ptRope->tConstraints = new DistanceConstraint[MAX_ROPE_CONSTRAINTS];
    SigmaMemoryClear(ptRope->tConstraints, sizeof(DistanceConstraint) * MAX_ROPE_CONSTRAINTS);


    TexAnimInitialize(pdefRope->tUberMaterial, ptRope->tTexAnimStateList, NULL);
    float flDeltaTime;  // INFERENCE: the original leaves s0 unset at the call below (uninitialized argument), as in RopeSetAttachPoint
    PhysicsSystemInitialize(ptRope->tPhysics, pdefRope->snoPhysics, sGetRActorIDAttachedTo(ptRope));
    sRopeSetNumberOfKnots(ptRope, pdefRope, tInterp, flDeltaTime);








    ptRope->flLastUpdateTime = CGameGetClientTime();
    sRopeAttachmentsOnMsg(id, ROPE_MSGKEY_CREATE);

    return id;
}




void RopeSetAttachPoint(
    RopeID idRope,
    BOOL bDest,
    RopeAttachPoint & tRopeAttachPoint)
{
    Rope * ptRope = RopeTryToGet(idRope);
    if (ptRope == NULL)
    {
        return;
    }


    if (bDest)
    {
        SigmaMemoryCopy(&ptRope->tRopeAttachPoints[1], &tRopeAttachPoint, sizeof(RopeAttachPoint));
    }
    else
    {
        SigmaMemoryCopy(&ptRope->tRopeAttachPoints[0], &tRopeAttachPoint, sizeof(RopeAttachPoint));
    }

    ptRope->fFixedUpdate = sIsTopmostParentInUI(ptRope);

    const RopeDefinition * pdefRope = (const RopeDefinition *)gptGroupDefRope->LockR(ptRope->snoRope, FALSE);

    InterpTime tInterp;
    tInterp.Init();
    tInterp.nTimeType = TICK_TIME_NON_LOOPING;
    tInterp.flTime = ptRope->flTime;
    tInterp.flScale = ptRope->flSystemDuration;

    float flDeltaTime;  // INFERENCE: the original leaves s0 unset at the call below (uninitialized argument)

    sRopeSetNumberOfKnots(ptRope, pdefRope, tInterp, flDeltaTime);

    gptGroupDefRope->UnlockR((void *&)pdefRope);
}





static void sDeleteAsyncRope(void * pParam, void *)
{
    RopeID id = (RopeID)(intptr_t)pParam;
    RopeDelete(id);
}





void RopeDelete(RopeID id)
{
    Rope * ptRope = RopeTryToGet(id);
    if (ptRope)
    {
        if (ptRope->dwLastFrameSubmitted == CGameAsyncGetDrawFrameCount() && CGameAsyncRenderRegisterDelayedCallback(sDeleteAsyncRope, (void *)(intptr_t)id, NULL))
        {
            ptRope->dwRopeInstFlags |= (1 << ROPE_PENDING_DELETION);
            return;
        }

        sRopeAttachmentsOnMsg(id, ROPE_MSGKEY_DELETE);
        AttachmentListHandleParentDeletion(ptRope->tAttachmentList);
        ptRope->tRMsgList.Dispose();

        ptRope->tPhysics.ptConstraints = NULL;
        PhysicsSystemDispose(ptRope->tPhysics);

        delete [] ptRope->ptKnotPoints;
        delete [] ptRope->ptCurvePoints;
        delete [] ptRope->tConstraints;
        delete [] ptRope->tTexAnimStateList;

        RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
        ptRopeGlobals->ptRopes->ElementDelete(ptRope);
    }
}


















void RopePlayOutro(const RopeID idRope)
{
    Rope * ptRope = RopeTryToGet(idRope);
    if (ptRope)
    {
        ptRope->bOutro = TRUE;
    }
}

















































void RopeRenderUpdate(
    RWindow & tRWindow)
{
    RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
    if (ptRopeGlobals->ptRopes->dwTotalCount == 0)
    {
        return;
    }

    AsyncRWindowData & tAsyncRVData = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];

    const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());
    int32 nReflectionQuality = Prefs::GetVideoPrefs().GetReflectionQuality();

    TDataArrayIterator<Rope> tIt;
    while (ptRopeGlobals->ptRopes->Iterate(tIt))
    {
        Rope * ptRope = tIt.ptElement;


        if (ptRope->dwRopeInstFlags & (1 << ROPE_PENDING_DELETION))
        {
            continue;
        }


        if (ptRope->eContext != tAsyncRVData.dispList.GetRenderContext())
        {
            continue;
        }


        if (!CameraComputeSphereVisibility(cam, ptRope->ws))
        {
            ptRope->dwRopeInstFlags &= ~(1 << ROPE_VISIBLE_BIT);
            continue;
        }

        ptRope->dwRopeInstFlags |= (1 << ROPE_VISIBLE_BIT);

        if (AppDrawFlagIsSet(APP_DRAW_NO_PLAYER_ACTOR_BIT))
        {
            if (ptRope->tRopeAttachPoints[0].tParentData.HasLocalPlayerAsParent() ||
                ptRope->tRopeAttachPoints[1].tParentData.HasLocalPlayerAsParent())
            {
                continue;
            }
        }

        if (ptRope->flRopeAlpha < 1e-6f)
        {
            continue;
        }

        sRopeEvaluate(ptRope);

        const RopeDefinition * pdefRope = (const RopeDefinition *)gptGroupDefRope->LockR(ptRope->snoRope, FALSE);

        if (!(ptRope->dwRopeInstFlags & (1 << ROPE_ALL_TEXTURE_LOADED)))
        {
            int32 nCount = MaterialGetTextureCount(pdefRope->tUberMaterial);
            const MaterialTextureEntry * pMatTexEntries = pdefRope->tUberMaterial.ptMatTexList;

            BOOL fAnyTextureNotLoaded = FALSE;
            for (int32 i = 0; i < nCount; i++, pMatTexEntries++)
            {
                if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, pMatTexEntries->tMatTex.snoTex), 2))
                {
                    fAnyTextureNotLoaded = TRUE;
                    break;
                }
            }

            if (fAnyTextureNotLoaded)
            {
                gptGroupDefRope->UnlockR((void *&)pdefRope);
                continue;
            }

            ptRope->dwRopeInstFlags |= (1 << ROPE_ALL_TEXTURE_LOADED);
        }

        ScopedShaderDefinitionLockR pdefShader(ShaderMapGetDefaultShader(pdefRope->tUberMaterial.snoShaderMap, TRUE, NULL));
        Renderable tRenderable(RENDERABLETYPE_ROPE,

                               ptRope->id,
                               PERFBIN_OTHER,
                               pdefShader,
                               pdefRope->tUberMaterial.mat);
        RenderableHeader & tHeader = tRenderable.header;
        RenderableRope & tDetails = tRenderable.details.tRope;


        ptRope->dwLastFrameSubmitted = CGameAsyncGetUpdateFrameCount();


        tDetails.snoRope = ptRope->snoRope;
        tDetails.nVertCount = ptRope->nVertCount;
        tDetails.nCurvePoints = ptRope->nCurvePoints;


        tDetails.ptCurvePoints = (RopeCurvePoint *)CGameAsyncRenderGetMemory(ptRope->nCurvePoints * sizeof(RopeCurvePoint));
        SigmaMemoryCopy(tDetails.ptCurvePoints, ptRope->ptCurvePoints, ptRope->nCurvePoints * sizeof(RopeCurvePoint));
        int32 nTextureCount = MaterialGetTextureCount(pdefRope->tUberMaterial);
        tDetails.arTexAnimStates = TexAnimRenderAsyncCopy(nTextureCount, ptRope->tTexAnimStateList, &pdefRope->tUberMaterial, SHADERTEX_NONE);





        tHeader.SetDist(Vector3D::DistanceSquared(cam.cmState.transform.wp, ptRope->ws.wpCenter));
        tHeader.SetRectFromPoint(ptRope->ws.wpCenter);

        tHeader.rgbaColor = crgbaWhite;
        if (ptRope->flRopeAlpha < 0.999999f)
        {
            tHeader.rgbaColor.a = (uint8)Math::FloatRoundToInt(ptRope->flRopeAlpha * 255.0f);
        }

        tAsyncRVData.dispList.SubmitRenderable(tRenderable);
        if (nReflectionQuality > 0)
        {
            for (int32 i = 0; i < PostFXGetNumReflectionPlanes(); i++)
            {
                const Camera & tCamera = CameraManagerGetCamera(i + 2);
                const Plane & tPlane = PostFXGetReflectionPlane(i);

                if (CameraComputeSphereVisibility(tCamera, ptRope->ws))
                {
                    float flDist = tPlane.ContainsPoint(ptRope->ws.wpCenter);
                    if (!(flDist > ptRope->ws.wdRadius + 6.0 ||
                          flDist < -ptRope->ws.wdRadius))
                    {
                        tAsyncRVData.dispListReflections[i].SubmitRenderable(tRenderable);
                    }
                }
            }
        }

        if (AppDrawFlagIsSet(APP_DRAW_ROPE_DEBUG_BIT))
        {
            sRopeDebugDraw(ptRope);
        }
        gptGroupDefRope->UnlockR((void *&)pdefRope);
    }

}




static void sCurveEvaluate(
    Rope * ptRope,
    const int32 nCurveIndex,
    const int32 nKnotIndex,
    const float flT)
{
    int32 nMaxPointIndex = ptRope->nRopeKnots - 1;

    PointMass * ptPointMasses[4];
    ptPointMasses[0] = ptRope->ptKnotPoints[Math::Max(nKnotIndex - 1, 0)].ptPointMass;
    ptPointMasses[1] = ptRope->ptKnotPoints[nKnotIndex].ptPointMass;
    ptPointMasses[2] = ptRope->ptKnotPoints[Math::Min(nMaxPointIndex, nKnotIndex + 1)].ptPointMass;
    ptPointMasses[3] = ptRope->ptKnotPoints[Math::Min(nMaxPointIndex, nKnotIndex + 2)].ptPointMass;

    Vector3D wpControl[4];
    wpControl[0] = ptPointMasses[0]->wp;
    wpControl[1] = ptPointMasses[1]->wp;
    wpControl[2] = ptPointMasses[2]->wp;
    wpControl[3] = ptPointMasses[3]->wp;

    CurveCatmullRomEvaluate(wpControl, flT, ptRope->ptCurvePoints[nCurveIndex].wpCurvePoint);
}




static void sRopeEvaluate(
    Rope * ptRope)
{
    for (int32 i = 0; i < ptRope->nCurvePoints; i++)
    {

        float flFractionAlongRope = (float)i / (float)(ptRope->nCurvePoints - 1);

        float flKnotPos = flFractionAlongRope * (float)(ptRope->nRopeKnots - 1);
        int32 nKnotIndex = Math::FloatToInt(flKnotPos);
        float flKnotFraction = flKnotPos - (float)nKnotIndex;

        sCurveEvaluate(ptRope, i, nKnotIndex, flKnotFraction);
    }
}



static void sRopeDebugDraw(
    Rope * ptRope)
{
    sRopeEvaluate(ptRope);

    GfxLine3DDrawDeferred(ptRope->transformPin[0].wp, ptRope->transformPin[1].wp, crgbaBlue, GLOBALSNO_SHADER_3D_PRIMS, ptRope->eContext);

    for (int32 i = 0; i < ptRope->nCurvePoints; i++)
    {
        Sphere ws;
        ws.Set(ptRope->ptCurvePoints[i].wpCurvePoint, 0.05f);
        GfxSphereDrawDeferred(ws, crgbaRed, GLOBALSNO_SHADER_3D_PRIMS, ptRope->eContext);
    }

    for (int32 i = 0; i < ptRope->nRopeKnots; i++)
    {
        Sphere ws;
        ws.Set(ptRope->ptKnotPoints[i].ptPointMass->wp, 0.05f);
        GfxSphereDrawDeferred(ws, crgbaBlue, GLOBALSNO_SHADER_3D_PRIMS, ptRope->eContext);
    }


}


void RopeDraw(
    const RenderableHeader & tHeader,
    const RenderableDetails & tDetails)
{


	RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
	const Camera & cam = CameraManagerGetCamera(CAMERA_CURRENT);
	Matrix4x4 mWorldToScreen;
	Matrix4x4 mWorldToScreenInv;

	if (tHeader.pdefShader->dwRenderLayers & (1 << RENDERLAYER_PROJECTED_ADDITIVE))
	{
		mWorldToScreen = cam.mView;
		mWorldToScreenInv = mWorldToScreen.Inverse();
	}
	else
	{
        if (CWorldIsRetro())
        {
            Matrix4x4 mProjectionAdjusted;
            GfxBuildPerspectiveProjectionMatrix(mProjectionAdjusted, Math::DegToAngle(35.0f), cam.cmState.flAspectRatio, cam.cmState.flNearPlane, cam.cmState.flFarPlane, TRUE);
            mWorldToScreen = cam.mView * mProjectionAdjusted;
        }
        else
        {
            mWorldToScreen = cam.mView * cam.mProjectionUnadjusted;
        }
		mWorldToScreenInv = mWorldToScreen.Inverse();
	}

	const RopeDefinition * pdefRope = (const RopeDefinition *)gptGroupDefRope->LockR(tDetails.tRope.snoRope, FALSE);
	float fTextureRepeatCount = pdefRope->fTextureRepeatCount;
	uint32 dwRopeFlags = pdefRope->dwRopeFlags;

	WorldDistance wdRopeLength = 0.0f;
	if (dwRopeFlags & (1 << ROPEDEF_TEXTURE_REPEAT_COUNT_IS_PER_FOOT))
	{
		Vector3D wpLast = tDetails.tRope.ptCurvePoints[0].wpCurvePoint;

		for (int32 i = 1; i < tDetails.tRope.nCurvePoints; i++)
		{
			Vector3D wp = tDetails.tRope.ptCurvePoints[i].wpCurvePoint;
			wdRopeLength += Vector3D::Distance(wpLast, wp);
			wpLast = wp;
		}

		fTextureRepeatCount *= wdRopeLength;
	}


	for (int32 i = 0; i < tDetails.tRope.nCurvePoints; i++)
	{
		RopeCurvePoint & tCurCurvePoint = tDetails.tRope.ptCurvePoints[i];
		Vector3D wpScreenSpacePoint3D = tCurCurvePoint.wpCurvePoint * mWorldToScreen;
		tCurCurvePoint.wpScreenSpacePoint.Set(wpScreenSpacePoint3D.x, wpScreenSpacePoint3D.y);
	}


	for (int32 i = 0; i < tDetails.tRope.nCurvePoints; i++)
	{
		RopeCurvePoint & tCurCurvePoint = tDetails.tRope.ptCurvePoints[i];

		Vector2D wvPerp;

		if (i == 0)
		{
			RopeCurvePoint & tNextCurvePoint = tDetails.tRope.ptCurvePoints[i + 1];


			Vector2D wvNextVectorView = tNextCurvePoint.wpScreenSpacePoint - tCurCurvePoint.wpScreenSpacePoint;
			wvNextVectorView.Set(-wvNextVectorView.y, wvNextVectorView.x);
			wvNextVectorView.Normalize();

			wvPerp = wvNextVectorView;
		}
		else if (i == tDetails.tRope.nCurvePoints - 1)
		{
			RopeCurvePoint & tPrevCurvePoint = tDetails.tRope.ptCurvePoints[i - 1];


			Vector2D wvPrevVectorView = tCurCurvePoint.wpScreenSpacePoint - tPrevCurvePoint.wpScreenSpacePoint;
			wvPrevVectorView.Set(-wvPrevVectorView.y, wvPrevVectorView.x);
			wvPrevVectorView.Normalize();

			wvPerp = wvPrevVectorView;
		}
		else
		{
			RopeCurvePoint & tNextCurvePoint = tDetails.tRope.ptCurvePoints[i + 1];
			RopeCurvePoint & tPrevCurvePoint = tDetails.tRope.ptCurvePoints[i - 1];


			Vector2D wvNextVectorView = tNextCurvePoint.wpScreenSpacePoint - tCurCurvePoint.wpScreenSpacePoint;
			wvNextVectorView.Set(-wvNextVectorView.y, wvNextVectorView.x);
			wvNextVectorView.Normalize();


			Vector2D wvPrevVectorView = tCurCurvePoint.wpScreenSpacePoint - tPrevCurvePoint.wpScreenSpacePoint;
			wvPrevVectorView.Set(-wvPrevVectorView.y, wvPrevVectorView.x);
			wvPrevVectorView.Normalize();

			wvPerp = wvNextVectorView + wvPrevVectorView;
			wvPerp.Normalize();
		}

		wvPerp *= tDetails.tRope.ptCurvePoints[i].wdRopeWidth;

		tDetails.tRope.ptCurvePoints[i].wvScreenSpaceEdge[0] = wvPerp;
		tDetails.tRope.ptCurvePoints[i].wvScreenSpaceEdge[1] = -wvPerp;
	}







































	uint32 dwStartIndex;
	VertexBillboard * pv = (VertexBillboard *)GfxVBLock(ptRopeGlobals->vbid, tDetails.tRope.nVertCount, dwStartIndex);
	if (pv)
	{
		Vector3D wpPrev = tDetails.tRope.ptCurvePoints[0].wpCurvePoint;
		float wdRopeLengthNumerator = 0.0f;

		for (int32 i = 0; i < tDetails.tRope.nCurvePoints; i++)
		{
			RopeCurvePoint & tCurCurvePoint = tDetails.tRope.ptCurvePoints[i];
			Vector3D & wp = tCurCurvePoint.wpCurvePoint;

			Vector3D wvEdge0;
			Vector3D wvEdge1;

			wvEdge0.Set(tCurCurvePoint.wvScreenSpaceEdge[0].x, tCurCurvePoint.wvScreenSpaceEdge[0].y, 0.0f);
			wvEdge1.Set(tCurCurvePoint.wvScreenSpaceEdge[1].x, tCurCurvePoint.wvScreenSpaceEdge[1].y, 0.0f);

			wvEdge0 = Matrix4x4::Transform3x3(wvEdge0, mWorldToScreenInv);
			wvEdge1 = Matrix4x4::Transform3x3(wvEdge1, mWorldToScreenInv);

			VertexColor vc(tCurCurvePoint.rgbaCurvePointColor);

			float flV;

			if (!(dwRopeFlags & (1 << ROPEDEF_TEXTURE_REPEAT_COUNT_IS_PER_FOOT)))
			{
				flV = (float)i / (tDetails.tRope.nCurvePoints - 1) * fTextureRepeatCount;
			}
			else
			{
				wdRopeLengthNumerator += Vector3D::Distance(wp, wpPrev);
				wpPrev = wp;
				flV = (wdRopeLength > EPSILON) ? (fTextureRepeatCount * wdRopeLengthNumerator) / wdRopeLength : 0.0f;
			}

			pv->wpPos = wp + wvEdge0;
			pv->wvpNormal = cgcDefaultNormal;
			pv->vcDiffuse = vc;
			pv->vcAuxColor = crgbaWhite;
			pv->vpUV0 = Vector2DTexCoordToUByte4(Vector2D(0.0f, flV));
			pv->vpUV1 = Vector2DTexCoordToUByte4(Vector2D(0.0f, flV));
			pv++;

			pv->wpPos = wp + wvEdge1;
			pv->wvpNormal = cgcDefaultNormal;
			pv->vcDiffuse = vc;
			pv->vcAuxColor = crgbaWhite;
			pv->vpUV0 = Vector2DTexCoordToUByte4(Vector2D(1.0f, flV));
			pv->vpUV1 = Vector2DTexCoordToUByte4(Vector2D(1.0f, flV));
			pv++;
		}
		GfxVBUnlock(ptRopeGlobals->vbid);
	}

	GfxWorldMatrixSet(tDetails.tCommon.mWorld);

	const ShaderDefinition * pdefShader = tHeader.pdefShader;
	for (int nPassIndex = 0; nPassIndex != pdefShader->nPassCount; ++nPassIndex)
	{
		GfxShaderInit(pdefShader, nPassIndex);
		GfxShaderSetTextureFactor(tHeader.rgbaColor);
		GfxShaderSetIB(ptRopeGlobals->ibid, 0);
		GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptRopeGlobals->vbid);
		GfxShaderSetTextures(pdefRope->tUberMaterial, tDetails.tRope.arTexAnimStates, "Rope", -1);
		GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, dwStartIndex, 0, tDetails.tRope.nVertCount, 0, tDetails.tRope.nCurvePoints * 2 - 2, PERFBIN_OTHER);
	}

	gptGroupDefRope->UnlockR((void *&)pdefRope);
}




void RopeIntersectSphere(const Sphere & tSphere, XPooledDataIDList & listRope)
{
    RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
    TDataArrayIterator<Rope> tIt;
    while (ptRopeGlobals->ptRopes->Iterate(tIt))
    {
        Rope * ptRope = tIt.ptElement;
        if (ptRope->dwRopeInstFlags & (1 << ROPE_PENDING_DELETION))
        {
            continue;
        }

        if (Intersect::SphereAndSphere(tSphere, ptRope->ws))
        {
            listRope.AddTail_Tracked(ptRope->id);
        }
    }
}












void RopeApplyExplosion(const RopeID idRope, const ExplosionImpulse & tExplosion)
{
    Rope * ptRope = RopeGet(idRope);
    for (int32 i = 0; i < ptRope->nRopeKnots; i++)
    {
        PointMass & pm = *ptRope->ptKnotPoints[i].ptPointMass;
        pm.wv += pm.flInvMass * tExplosion.ComputeImpulse(pm.wp);
    }
}







void RopesUpdateFixedUpdateFlag()
{
    RopeGlobals * ptRopeGlobals = CGameGet().ptRopeGlobals;
    if (ptRopeGlobals->ptRopes->dwTotalCount == 0)
    {
        return;
    }

    TDataArrayIterator<Rope> tIt;
    while (ptRopeGlobals->ptRopes->Iterate(tIt))
    {
        Rope * ptRope = tIt.ptElement;
        if (ptRope)
        {
            ptRope->fFixedUpdate = sIsTopmostParentInUI(ptRope);
        }
    }
}






void RopeRenderInDisplayList(
    Rope * ptRope,
    const Camera & cam,
    DisplayList & tDisplayList,
    const float flDeltaTime)
{



    if (ptRope->dwRopeInstFlags & (1 << ROPE_PENDING_DELETION))
    {
        return;
    }


    if (AppDrawFlagIsSet(APP_DRAW_NO_PLAYER_ACTOR_BIT))
    {
        if (ptRope->tRopeAttachPoints[0].tParentData.HasLocalPlayerAsParent() ||
            ptRope->tRopeAttachPoints[1].tParentData.HasLocalPlayerAsParent())
        {
            return;
        }
    }

    if (ptRope->eContext != tDisplayList.GetRenderContext())
    {
        return;
    }

    if (ptRope->flRopeAlpha < 1e-6f)
    {
        return;
    }

    ScopedRopeDefinitionLockR pdefRope(ptRope->snoRope);
    ScopedShaderDefinitionLockR pdefShader(ShaderMapGetDefaultShader(pdefRope->tUberMaterial.snoShaderMap, TRUE, NULL));
    Renderable tRenderable(RENDERABLETYPE_ROPE, ptRope->id, PERFBIN_OTHER, pdefShader, pdefRope->tUberMaterial.mat);





    RenderableHeader & tHeader = tRenderable.header;
    RenderableDetails & tDetails = tRenderable.details;


    ptRope->dwLastFrameSubmitted = CGameAsyncGetUpdateFrameCount();


    tDetails.tRope.snoRope = ptRope->snoRope;
    tDetails.tRope.nVertCount = ptRope->nVertCount;
    tDetails.tRope.nCurvePoints = ptRope->nCurvePoints;


    tDetails.tRope.ptCurvePoints = (RopeCurvePoint *)CGameAsyncRenderGetMemory(ptRope->nCurvePoints * sizeof(RopeCurvePoint));
    SigmaMemoryCopy(tDetails.tRope.ptCurvePoints, ptRope->ptCurvePoints, ptRope->nCurvePoints * sizeof(RopeCurvePoint));
    int32 nTextureCount = MaterialGetTextureCount(pdefRope->tUberMaterial);
    tDetails.tRope.arTexAnimStates = TexAnimRenderAsyncCopy(nTextureCount, ptRope->tTexAnimStateList, &pdefRope->tUberMaterial, SHADERTEX_NONE);





    tHeader.SetDist(Vector3D::DistanceSquared(cam.cmState.transform.wp, ptRope->ws.wpCenter));
    tHeader.SetRectFromPoint(ptRope->ws.wpCenter);

    tHeader.rgbaColor = crgbaWhite;
    if (ptRope->flRopeAlpha < 0.999999f)
    {
        tHeader.rgbaColor.a = (uint8)Math::FloatRoundToInt(ptRope->flRopeAlpha * 255.0f);
    }

    tDisplayList.SubmitRenderable(tRenderable);
}
