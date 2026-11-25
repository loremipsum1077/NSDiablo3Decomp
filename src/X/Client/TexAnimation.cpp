#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "TexAnimation.h"
#include "AnimationCommon.h"
#include "CGame.h"
#include "Prefetch.h"
#include "RActors/RActors.h"
#include "RActors/RActorAnimation.h"
#include "Animation.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxMaterial.h"
#include "Gfx/Gfx.h"
#include "Definitions/TextureDefinition.h"
#include "../Sigma/Math/Transform.h"
#include "Definitions/ShaderCommon.h"
#include "../Sigma/Math/Matrix.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Random.h"

int32 MaterialGetTextureCount(const UberMaterial & tUberMaterial);  // Gfx/GfxMaterial.cpp
void AnimStateInitialize(AnimState & tState, SNO snoTexture, const FrameAnim * ptFrameAnim);  // Animation.cpp
void GfxShaderTexMatrixSet(const TextureBinding * ptTexBinding, const Matrix4x4 & mTex);  // Gfx/GfxShader.cpp
void GfxShaderTexMatrixSetIdentity(const TextureBinding * ptTexBinding);  // Gfx/GfxShader.cpp
TexAnimMatrixType TexAnimGetTexMatrix(Matrix4x4 & mTex, const TexAnimParams * ptTexAnimParams, const TexAnimState * ptTexAnimState, const TextureDefinition * ptTextureDef);
TexAnimMatrixType TexAnimGetTexMatrixForBillboard(Matrix4x4 & mTex, const TexAnimParams * ptTexAnimParams, const TexAnimState * ptTexAnimState, const TextureFramesInfo & tTexFramesInfo);
int32 RActorAnimationLookupBone(const RActor & tRActor, LPCSTR szBoneName);  // RActors/RActorAnimation.cpp

enum {
    MAX_POOLED_TEXANIMSTATES = 4,
    TEXANIMSTATE_POOL_BLOCKS = 1024,
};

struct TexAnimGlobals {  // size 0x30
    ChainedFixedBlockAllocator poolTexAnimStateArrays;  // 0x00
};
static_assert(sizeof(TexAnimGlobals) == 0x30, "TexAnimGlobals");

void sTexAnimInitialize(const SNO snoTexture, const TexAnimParams & tTexAnimParams, TexAnimState & tTexAnimState, RActor * ptRActor);
void sTexAnimInitialize(const SNO snoTexture, const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, TexAnimState & tTexAnimState, RActor * ptRActor);
uint32 TexAnimUpdate(const TexAnimParams & tTexAnimParams, const ObjectTexAnimParam * ptObjTexAnimParam, TexAnimState & tTexAnimState, float flDeltaTime, float flClientTime);
uint32 TexAnimUpdate(const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const ObjectTexAnimParam * ptObjTexAnimParam, TexAnimState & tTexAnimState, float flDeltaTime, float flClientTime);








void TexAnimInitializeForGame() {
    CGame & tCGame = CGameGet();
    tCGame.ptTexAnimGlobals = (TexAnimGlobals *)SigmaMemoryNew(sizeof(TexAnimGlobals), 4, NULL, TRUE);
    tCGame.ptTexAnimGlobals->poolTexAnimStateArrays.Initialize(sizeof(TexAnimState) * MAX_POOLED_TEXANIMSTATES, TEXANIMSTATE_POOL_BLOCKS, NULL);
    tCGame.ptTexAnimGlobals->poolTexAnimStateArrays.SetGrow(XALLOC_GROW_LINEAR, 128);
    tCGame.ptTexAnimGlobals->poolTexAnimStateArrays.SetShrinkable(TRUE);
}











void TexAnimDisposeForGame() {
    CGame & tCGame = CGameGet();
    tCGame.ptTexAnimGlobals->poolTexAnimStateArrays.Dispose();
    SigmaMemoryFree(tCGame.ptTexAnimGlobals, NULL);
    tCGame.ptTexAnimGlobals = NULL;
}







void sTexAnimInitialize(
    const SNO snoTexture,
    const TexAnimParams & tTexAnimParams,
    TexAnimState & tTexAnimState,
    RActor * ptRActor)
{
    SigmaMemoryClear(&tTexAnimState, sizeof(TexAnimState));
    tTexAnimState.snoTex = snoTexture;
    tTexAnimState.ptRActor = ptRActor;
    tTexAnimState.nBone = -1;

    switch (tTexAnimParams.eTexAnimType) {
    case TEXANIMTYPE_TRANSLATION:
        {

        RNG & rngLocal = CGameGlobalsGet().rngLocal;
        if (tTexAnimParams.dwAnimFlags & 0x1)
        {
            tTexAnimState.flUTrans = RandFloat_Internal(rngLocal);
        }
        else
        {
            tTexAnimState.flUTrans = tTexAnimParams.flUTransInitial;
        }

        if (tTexAnimParams.dwAnimFlags & 0x2)
        {
            tTexAnimState.flVTrans = RandFloat_Internal(rngLocal);
        }
        else
        {
            tTexAnimState.flVTrans = tTexAnimParams.flVTransInitial;
        }

        tTexAnimState.flUTransVel = RandRangeFloat_Internal(rngLocal, tTexAnimParams.flUTransVel, tTexAnimParams.flUTransVelDelta) * 60.0f;
        tTexAnimState.flVTransVel = RandRangeFloat_Internal(rngLocal, tTexAnimParams.flVTransVel, tTexAnimParams.flVTransVelDelta) * 60.0f;
        tTexAnimState.aRotVel = RandRangeFloat_Internal(rngLocal, tTexAnimParams.aRotVel, tTexAnimParams.aRotVelDelta) * 60.0f;
        tTexAnimState.aRot = Math::NormalizeAngle(tTexAnimParams.aRotInitial);
        }
        break;

    case TEXANIMTYPE_FRAMEBYFRAME:

        AnimStateInitialize(tTexAnimState.tAnimState, snoTexture, &tTexAnimParams.tFrameAnim);
        break;

    default:
        break;
    }


}






void sTexAnimInitialize(
    const SNO snoTexture,
    const CHAR * pTexAnimCompParams,
    uint32 dwTexAnimCompParamsSize,
    TexAnimState & tTexAnimState,
    RActor * ptRActor)
{
    SigmaMemoryClear(&tTexAnimState, sizeof(TexAnimState));
    tTexAnimState.snoTex = snoTexture;
    tTexAnimState.ptRActor = ptRActor;
    tTexAnimState.nBone = -1;


    if (pTexAnimCompParams == NULL)
    {
        return;
    }
    // line 160 col 16: the tail call is spelled `return <call>` in this void function
    if (dwTexAnimCompParamsSize == sizeof(TexAnimParams))
    {
        return sTexAnimInitialize(snoTexture, *(const TexAnimParams *)pTexAnimCompParams, tTexAnimState, ptRActor);
    }
    // no params local in the DWARF; the cast is spelled inline
    switch (((const TexAnimCompParams *)pTexAnimCompParams)->eCompType)
    {
    case TEXANIMCOMP_SIMPLETRANS:
    case TEXANIMCOMP_SIMPLETRANS2:
        {
            RNG & rngLocal = CGameGlobalsGet().rngLocal;
            tTexAnimState.flUTrans = RandFloat_Internal(rngLocal);
            tTexAnimState.flVTrans = RandFloat_Internal(rngLocal);
            tTexAnimState.flUTransVel = 0.0f;
            tTexAnimState.flVTransVel = 0.0f;
            tTexAnimState.aRotVel = 0.0f;
            tTexAnimState.aRot = 0.0f;
        }
        break;
    case TEXANIMCOMP_SIMPLETRANSVEL:
        {
            RNG & rngLocal = CGameGlobalsGet().rngLocal;
            tTexAnimState.flUTrans = RandFloat_Internal(rngLocal);
            tTexAnimState.flVTrans = RandFloat_Internal(rngLocal);
            tTexAnimState.flUTransVel = ((const TexAnimCompSimpleTransVelParams *)pTexAnimCompParams)->flUTransVel * 60.0f;
            tTexAnimState.flVTransVel = ((const TexAnimCompSimpleTransVelParams *)pTexAnimCompParams)->flVTransVel * 60.0f;
            tTexAnimState.aRotVel = 0.0f;
            tTexAnimState.aRot = 0.0f;
        }
        break;
    }
}





void TexAnimInitialize(const UberMaterial & tUberMaterial, TexAnimState * ptTexAnimState, RActor * ptRActor) {
    int32 nNumTextures = MaterialGetTextureCount(tUberMaterial);
    const MaterialTextureEntry * ptMatTexList = tUberMaterial.ptMatTexList;
    for (int32 i = 0; i < nNumTextures; i++) {
        const MaterialTexture & tMatTex = ptMatTexList[i].tMatTex;
        if (tMatTex.snoTex == SNO_NONE) {
            ptTexAnimState[i].snoTex = SNO_NONE;
        } else {
            sTexAnimInitialize(tMatTex.snoTex, tMatTex.pTexAnimCompParams, tMatTex.serTexAnimCompParams.dwSize, ptTexAnimState[i], ptRActor);
        }
    }
}


















void TexAnimInitialize(const MaterialTexture * ptMatTexList, TexAnimState * ptTexAnimState, const int32 nNumTextures) {
    for (int32 i = 0; i < nNumTextures; i++) {
        const MaterialTexture & tMatTex = ptMatTexList[i];
        if (tMatTex.snoTex == SNO_NONE) {
            ptTexAnimState[i].snoTex = SNO_NONE;
        } else {
            sTexAnimInitialize(tMatTex.snoTex, tMatTex.pTexAnimCompParams, tMatTex.serTexAnimCompParams.dwSize, ptTexAnimState[i], NULL);
        }
    }
}



















void TexAnimInitialize(const SNO snoTexture, const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, TexAnimState & tTexAnimState) {
    sTexAnimInitialize(snoTexture, pTexAnimCompParams, dwTexAnimCompParamsSize, tTexAnimState, NULL);
}


















uint32 TexAnimUpdate(
    const UberMaterial & tUberMaterial,
    const ObjectTexAnimParam * ptObjTexAnimParam,
    TexAnimState * ptTexAnimState,
    const int32 nTexAnimStateCount,
    float flDeltaTime,
    float flClientTime)
{

    uint32 dwCount = Math::Min(MaterialGetTextureCount(tUberMaterial), nTexAnimStateCount);
    const MaterialTextureEntry * ptMatTexList = tUberMaterial.ptMatTexList;
    uint32 dwResultFlags = 0;
    for (uint32 i = 0; i < dwCount; i++)
    {
        const MaterialTexture & tMatTex = ptMatTexList[i].tMatTex;
        if (tMatTex.snoTex != SNO_NONE)
        {


            uint32 dwTempResultFlags = 0;
            if (tMatTex.pTexAnimCompParams)
            {
                dwTempResultFlags = TexAnimUpdate(tMatTex.pTexAnimCompParams, tMatTex.serTexAnimCompParams.dwSize, ptObjTexAnimParam, ptTexAnimState[i], flDeltaTime, flClientTime);
            }



            if (i == 1)
            {
                dwResultFlags = dwTempResultFlags;
            }
        }
    }

    return dwResultFlags;
}





uint32 TexAnimUpdate(
    const CHAR * pTexAnimCompParams,
    uint32 dwTexAnimCompParamsSize,
    const ObjectTexAnimParam * ptObjTexAnimParam,
    TexAnimState & tTexAnimState,
    float flDeltaTime,
    float flClientTime)
{

    if (pTexAnimCompParams == NULL)
    {
        return 0;
    }

    if (dwTexAnimCompParamsSize == sizeof(TexAnimParams))
    {
        return TexAnimUpdate(*(const TexAnimParams *)pTexAnimCompParams, ptObjTexAnimParam, tTexAnimState, flDeltaTime, flClientTime);
    }
    // no params local in the DWARF; the casts are spelled inline
    switch (((const TexAnimCompParams *)pTexAnimCompParams)->eCompType)
    {
    case TEXANIMCOMP_SIMPLETRANSVEL:

        tTexAnimState.flUTrans += tTexAnimState.flUTransVel * flDeltaTime;
        tTexAnimState.flVTrans += tTexAnimState.flVTransVel * flDeltaTime;

        tTexAnimState.flUTrans = Math::PinWrap(tTexAnimState.flUTrans, 0.0f, 1.0f);
        tTexAnimState.flVTrans = Math::PinWrap(tTexAnimState.flVTrans, 0.0f, 1.0f);
        break;

    default:
        break;
    }


    tTexAnimState.flLastUpdated = flClientTime;
    return 0;
}


uint32 TexAnimUpdate(
    const TexAnimParams & tTexAnimParams,
    const ObjectTexAnimParam * ptObjTexAnimParam,
    TexAnimState & tTexAnimState,
    float flDeltaTime,
    float flClientTime)
{
    BOOL fUseGameTime = (tTexAnimParams.dwAnimFlags & 0x4) == 0;
    float flActualDeltaTime;
    if (fUseGameTime)
    {
        flActualDeltaTime = flDeltaTime;
    } else {
        flActualDeltaTime = flClientTime - tTexAnimState.flLastUpdated;
    }
    tTexAnimState.flLastUpdated = flClientTime;

    uint32 dwResultFlags = 0;

    switch (tTexAnimParams.eTexAnimType)
    {
    case TEXANIMTYPE_TRANSLATION:

        if (tTexAnimParams.fUseObjectValueForU && ptObjTexAnimParam)
        {

            tTexAnimState.flUTrans = ptObjTexAnimParam->flValue;
        }
        else
        {
            tTexAnimState.flUTrans += flActualDeltaTime * tTexAnimState.flUTransVel;
        }

        if (tTexAnimParams.fUseObjectValueForV && ptObjTexAnimParam)
        {

            tTexAnimState.flVTrans = ptObjTexAnimParam->flValue;
        }
        else
        {
            tTexAnimState.flVTrans += flActualDeltaTime * tTexAnimState.flVTransVel;
        }

        if (tTexAnimParams.fPinTranslation)
        {
            tTexAnimState.flUTrans = Math::Pin(tTexAnimState.flUTrans, 0.0f, 1.0f);
            tTexAnimState.flVTrans = Math::Pin(tTexAnimState.flVTrans, 0.0f, 1.0f);
        }
        else
        {
            tTexAnimState.flUTrans = Math::PinWrap(tTexAnimState.flUTrans, 0.0f, 1.0f);
            tTexAnimState.flVTrans = Math::PinWrap(tTexAnimState.flVTrans, 0.0f, 1.0f);
        }
        tTexAnimState.aRot = Math::NormalizeAngle(tTexAnimState.aRot + flActualDeltaTime * tTexAnimState.aRotVel);
        break;



    case TEXANIMTYPE_FRAMEBYFRAME:

        if (tTexAnimState.tAnimState.fPending)
        {
            if (Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tTexAnimState.snoTex), 2))
            {
                AnimStateInitialize(tTexAnimState.tAnimState, tTexAnimState.snoTex, &tTexAnimParams.tFrameAnim);
            }
        }

        dwResultFlags = AnimationAdvance(tTexAnimState.tAnimState, tTexAnimParams.tFrameAnim.ePlaybackMode, flActualDeltaTime);
        break;


    case TEXANIMTYPE_MAYA_KEYFRAME0:
    case TEXANIMTYPE_MAYA_KEYFRAME1:

        if (tTexAnimState.nBone == -1 && tTexAnimState.ptRActor && tTexAnimState.ptRActor->ptActorAnim)
        {
            LPCSTR szUVBoneName = tTexAnimParams.eTexAnimType == TEXANIMTYPE_MAYA_KEYFRAME0 ? "uvAnim" : "uvAnim2";
            int32 nBone = RActorAnimationLookupBone(*tTexAnimState.ptRActor, szUVBoneName);
            tTexAnimState.nBone = nBone == -1 ? -2 : nBone;
        }
        break;

    default:
        break;
    }









    return dwResultFlags;
}
















TexAnimMatrixType TexAnimGetTexMatrix(Matrix4x4 & mTex, const TexAnimParams * ptTexAnimParams, const TexAnimState * ptTexAnimState, const TextureDefinition * ptTextureDef) {
    if (ptTexAnimParams == NULL) {
        mTex = cgm4x4Identity;
        return TAMT_IDENTITY;
    }
    const TexAnimParams & tTexAnimParams = *ptTexAnimParams;
    const TexAnimState & tTexAnimState = *ptTexAnimState;
    TexAnimMatrixType eMatrixType;

    switch (tTexAnimParams.eTexAnimType) {
    case TEXANIMTYPE_MAYA_KEYFRAME0:
    case TEXANIMTYPE_MAYA_KEYFRAME1:
        if (tTexAnimState.nBone >= 0) {
            PRSTransform xform = *tTexAnimState.ptTransform;
            xform.wp = Vector3D(-xform.wp.y, -xform.wp.x, xform.wp.z);
            xform.ToMatrix(mTex);
            return TAMT_ROTATE_TRANSLATE;
        }
        mTex = cgm4x4Identity;
        eMatrixType = TAMT_IDENTITY;
        break;

    case TEXANIMTYPE_NONE:
        mTex = cgm4x4Identity;
        eMatrixType = TAMT_IDENTITY;
        break;

    case TEXANIMTYPE_FIXED:
        mTex = tTexAnimParams.mTex;
        eMatrixType = TAMT_ROTATE_TRANSLATE;
        break;

    case TEXANIMTYPE_WORLDREL_CUBEMAP: {
        mTex = GfxViewMatrixGet();
        mTex._41 = 0.0f;
        mTex._42 = 0.0f;
        mTex._43 = 0.0f;
        static const Matrix4x4 cgm4x4Reflect(-1.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, -1.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, -1.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 1.0f);
        mTex = mTex.Inverse() * cgm4x4Reflect;
        eMatrixType = TAMT_ROTATE_ONLY;
        break;
    }

    case TEXANIMTYPE_TRANSLATION:
        if (!(tTexAnimParams.dwAnimFlags & 0x8)) {
            float flCosRot;
            float flSinRot;
            if (tTexAnimState.aRot != 0.0f) {
                flCosRot = Math::Cos(tTexAnimState.aRot);
                flSinRot = Math::Sin(tTexAnimState.aRot);
            } else {
                flCosRot = 1.0f;
                flSinRot = 0.0f;
            }
            if (tTexAnimParams.mTex._11 > 0.0f && tTexAnimParams.mTex._22 > 0.0f) {
                float f11 = tTexAnimParams.mTex._11;
                float f22 = tTexAnimParams.mTex._22;
                mTex._11 = flCosRot * f11;
                mTex._12 = flSinRot * f11;
                mTex._21 = -flSinRot * f22;
                mTex._22 = flCosRot * f22;
            } else {
                mTex._11 = flCosRot;
                mTex._12 = flSinRot;
                mTex._21 = -flSinRot;
                mTex._22 = flCosRot;
            }
            mTex._13 = 0.0f; mTex._14 = 0.0f;
            mTex._23 = 0.0f; mTex._24 = 0.0f;
            mTex._31 = 0.0f; mTex._32 = 0.0f; mTex._33 = 1.0f; mTex._34 = 0.0f;
            mTex._41 = tTexAnimState.flUTrans;
            mTex._42 = tTexAnimState.flVTrans;
        } else {
            float flCosRot;
            float flSinRot;
            if (tTexAnimState.aRot != 0.0f) {
                flCosRot = Math::Cos(tTexAnimState.aRot);
                flSinRot = Math::Sin(tTexAnimState.aRot);
            } else {
                flCosRot = 1.0f;
                flSinRot = 0.0f;
            }
            float flTranslateX = tTexAnimState.flUTrans - 0.5f;
            float flTranslateY = tTexAnimState.flVTrans - 0.5f;
            if (tTexAnimParams.mTex._11 > 0.0f && tTexAnimParams.mTex._22 > 0.0f) {
                mTex._11 = flCosRot * tTexAnimParams.mTex._11;
                mTex._12 = flSinRot * tTexAnimParams.mTex._11;
                mTex._21 = -flSinRot * tTexAnimParams.mTex._22;
                mTex._22 = flCosRot * tTexAnimParams.mTex._22;
            } else {
                mTex._11 = flCosRot;
                mTex._12 = flSinRot;
                mTex._21 = -flSinRot;
                mTex._22 = flCosRot;
            }
            mTex._13 = 0.0f; mTex._14 = 0.0f;
            mTex._23 = 0.0f; mTex._24 = 0.0f;
            mTex._31 = 0.0f; mTex._32 = 0.0f; mTex._33 = 1.0f; mTex._34 = 0.0f;
            mTex._41 = flTranslateX * mTex._11 + flTranslateY * mTex._21 + 0.5f;
            mTex._42 = flTranslateX * mTex._12 + flTranslateY * mTex._22 + 0.5f;
        }
        mTex._43 = 0.0f;
        mTex._44 = 1.0f;
        eMatrixType = TAMT_ROTATE_TRANSLATE;
        break;

    case TEXANIMTYPE_FRAMEBYFRAME: {
        if (tTexAnimState.snoTex == SNO_NONE) {
            mTex = cgm4x4Identity;
            eMatrixType = TAMT_IDENTITY;
            break;
        }
        const TextureDefinition * pdefTexture = ptTextureDef;
        BOOL fLocalLock = FALSE;
        if (pdefTexture == NULL) {
            pdefTexture = DefinitionLockR<TextureDefinition>(tTexAnimState.snoTex);
            fLocalLock = TRUE;
        }
        int32 nFrame = Math::Pin(AnimationComputeCurrentFrame(tTexAnimState.tAnimState), 0, (int32)pdefTexture->dwFrameCount - 1);
        TexFrame & tFrame = pdefTexture->ptFrame[nFrame];
        if (tTexAnimParams.fScaleUVsToFrame) {
            mTex._11 = ((TexFrame *)pdefTexture->ptFrame)->flU1;
            mTex._22 = ((TexFrame *)pdefTexture->ptFrame)->flV1;
            eMatrixType = TAMT_TRANSLATE_SCALE;
        } else {
            mTex._11 = 1.0f;
            mTex._22 = 1.0f;
            eMatrixType = TAMT_TRANSLATE_ONLY;
        }
        mTex._12 = 0.0f; mTex._13 = 0.0f; mTex._14 = 0.0f;
        mTex._21 = 0.0f; mTex._23 = 0.0f; mTex._24 = 0.0f;
        mTex._31 = 0.0f; mTex._32 = 0.0f; mTex._33 = 1.0f; mTex._34 = 0.0f;
        mTex._41 = tFrame.flU0;
        mTex._42 = tFrame.flV0;
        mTex._43 = 0.0f;
        mTex._44 = 1.0f;
        if (fLocalLock) {
            gptGroupDefTexture->UnlockR((void *&)pdefTexture);
        }
        break;
    }

    default:
        eMatrixType = TAMT_IDENTITY;
        break;
    }
    return eMatrixType;
}























































































TexAnimMatrixType TexAnimGetTexMatrix(Matrix4x4 & mTex, const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const TexAnimState * ptTexAnimState, const TextureDefinition * ptTextureDef) {
    const TexAnimState & tTexAnimState = *ptTexAnimState;
    if (pTexAnimCompParams == NULL) {
        mTex = cgm4x4Identity;
        return TAMT_IDENTITY;
    }

    if (dwTexAnimCompParamsSize == sizeof(TexAnimParams)) {
        return TexAnimGetTexMatrix(mTex, (const TexAnimParams *)pTexAnimCompParams, ptTexAnimState, ptTextureDef);
    }

    const TexAnimCompParams * ptParams = (const TexAnimCompParams *)pTexAnimCompParams;
    switch (ptParams->eCompType) {
    case TEXANIMCOMP_SIMPLETRANS:
    case TEXANIMCOMP_SIMPLETRANS2: {
        const TexAnimCompSimpleTransParams * ptTrans = (const TexAnimCompSimpleTransParams *)ptParams;
        mTex._11 = ptTrans->mTex_11; mTex._12 = 0.0f; mTex._13 = 0.0f; mTex._14 = 0.0f;
        mTex._21 = 0.0f; mTex._22 = ptTrans->mTex_22; mTex._23 = 0.0f; mTex._24 = 0.0f;
        mTex._31 = 0.0f; mTex._32 = 0.0f; mTex._33 = 1.0f; mTex._34 = 0.0f;
        mTex._41 = 0.0f; mTex._42 = 0.0f; mTex._43 = 0.0f; mTex._44 = 1.0f;
        break;
    }
    case TEXANIMCOMP_SIMPLETRANSVEL: {
        const TexAnimCompSimpleTransVelParams * ptTransVel = (const TexAnimCompSimpleTransVelParams *)ptParams;
        float f11 = ptTransVel->mTex_11;
        float f22 = ptTransVel->mTex_22;
        if (!(f11 > 0.0f && f22 > 0.0f)) {
            f11 = 1.0f;
            f22 = 1.0f;
        }
        mTex._11 = f11; mTex._12 = 0.0f; mTex._13 = 0.0f; mTex._14 = 0.0f;
        mTex._21 = 0.0f; mTex._22 = f22; mTex._23 = 0.0f; mTex._24 = 0.0f;
        mTex._31 = 0.0f; mTex._32 = 0.0f; mTex._33 = 1.0f; mTex._34 = 0.0f;
        mTex._41 = tTexAnimState.flUTrans; mTex._42 = tTexAnimState.flVTrans; mTex._43 = 0.0f; mTex._44 = 1.0f;
        break;
    }
    default:
        break;
    }
    return TAMT_ROTATE_TRANSLATE;
}











































TexAnimMatrixType TexAnimGetTexMatrixForBillboard(Matrix4x4 & mTex, const TexAnimParams * ptTexAnimParams, const TexAnimState * ptTexAnimState, const TextureFramesInfo & tTexFramesInfo) {
    if (ptTexAnimParams == NULL) {
        return TAMT_IDENTITY;
    }
    const TexAnimParams & tTexAnimParams = *ptTexAnimParams;
    const TexAnimState & tTexAnimState = *ptTexAnimState;
    TexAnimMatrixType eMatrixType;

    switch (tTexAnimParams.eTexAnimType) {
    case TEXANIMTYPE_FIXED:
        mTex._11 = tTexAnimParams.mTex._11;
        mTex._12 = tTexAnimParams.mTex._12;
        mTex._21 = tTexAnimParams.mTex._21;
        mTex._22 = tTexAnimParams.mTex._22;
        mTex._41 = tTexAnimParams.mTex._41;
        mTex._42 = tTexAnimParams.mTex._42;
        return TAMT_ROTATE_TRANSLATE;

    case TEXANIMTYPE_WORLDREL_CUBEMAP: {
        mTex = GfxViewMatrixGet();
        mTex._41 = 0.0f;
        mTex._42 = 0.0f;
        mTex._43 = 0.0f;
        static const Matrix4x4 cgm4x4Reflect(-1.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, -1.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, -1.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 1.0f);
        mTex = mTex.Inverse() * cgm4x4Reflect;
        return TAMT_ROTATE_ONLY;
    }

    case TEXANIMTYPE_TRANSLATION:
        if (!(tTexAnimParams.dwAnimFlags & 0x8)) {
            float flCosRot;
            float flSinRot;
            if (tTexAnimState.aRot != 0.0f) {
                flCosRot = Math::Cos(tTexAnimState.aRot);
                flSinRot = Math::Sin(tTexAnimState.aRot);
            } else {
                flCosRot = 1.0f;
                flSinRot = 0.0f;
            }
            if (tTexAnimParams.mTex._11 > 0.0f && tTexAnimParams.mTex._22 > 0.0f) {
                float f11 = tTexAnimParams.mTex._11;
                float f22 = tTexAnimParams.mTex._22;
                mTex._11 = flCosRot * f11;
                mTex._12 = flSinRot * f11;
                mTex._21 = -(flSinRot * f22);
                mTex._22 = flCosRot * f22;
            } else {
                mTex._11 = flCosRot;
                mTex._12 = flSinRot;
                mTex._21 = -flSinRot;
                mTex._22 = flCosRot;
            }
            mTex._41 = tTexAnimState.flUTrans;
            mTex._42 = tTexAnimState.flVTrans;
        } else {
            float flCosRot;
            float flSinRot;
            if (tTexAnimState.aRot != 0.0f) {
                flCosRot = Math::Cos(tTexAnimState.aRot);
                flSinRot = Math::Sin(tTexAnimState.aRot);
            } else {
                flCosRot = 1.0f;
                flSinRot = 0.0f;
            }
            float flTranslateX = tTexAnimState.flUTrans - 0.5f;
            float flTranslateY = tTexAnimState.flVTrans - 0.5f;
            if (tTexAnimParams.mTex._11 > 0.0f && tTexAnimParams.mTex._22 > 0.0f) {
                mTex._11 = flCosRot * tTexAnimParams.mTex._11;
                mTex._12 = flSinRot * tTexAnimParams.mTex._11;
                mTex._21 = -(flSinRot * tTexAnimParams.mTex._22);
                mTex._22 = flCosRot * tTexAnimParams.mTex._22;
            } else {
                mTex._11 = flCosRot;
                mTex._12 = flSinRot;
                mTex._21 = -flSinRot;
                mTex._22 = flCosRot;
            }
            mTex._41 = flTranslateX * mTex._11 + flTranslateY * mTex._21 + 0.5f;
            mTex._42 = flTranslateX * mTex._12 + flTranslateY * mTex._22 + 0.5f;
        }
        return TAMT_ROTATE_TRANSLATE;

    case TEXANIMTYPE_FRAMEBYFRAME: {
        if (tTexAnimState.snoTex == SNO_NONE) {
            return TAMT_IDENTITY;
        }
        int32 nFrame = Math::Pin(AnimationComputeCurrentFrame(tTexAnimState.tAnimState), 0, (int32)tTexFramesInfo.dwFrameCount - 1);
        TexFrame & tFrame = tTexFramesInfo.ptTexFrames[nFrame];
        if (tTexAnimParams.fScaleUVsToFrame) {
            mTex._11 = tTexFramesInfo.ptTexFrames->flU1;
            mTex._12 = 0.0f;
            mTex._22 = tTexFramesInfo.ptTexFrames->flV1;
            mTex._21 = 0.0f;
            eMatrixType = TAMT_TRANSLATE_SCALE;
        } else {
            eMatrixType = TAMT_TRANSLATE_ONLY;
        }
        mTex._41 = tFrame.flU0;
        mTex._42 = tFrame.flV0;
        return eMatrixType;
    }

    default:
        return TAMT_IDENTITY;
    }
}





























































// NON_MATCHING: The simple-translation scale select and first matrix store are scheduled in a different order at +0x70.
TexAnimMatrixType TexAnimGetTexMatrixForBillboard(Matrix4x4 & mTex, const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const TexAnimState * ptTexAnimState, const TextureFramesInfo & tTexFramesInfo) {
    const TexAnimState & tTexAnimState = *ptTexAnimState;
    if (pTexAnimCompParams == NULL) {
        return TAMT_IDENTITY;
    }

    if (dwTexAnimCompParamsSize == sizeof(TexAnimParams)) {
        return TexAnimGetTexMatrixForBillboard(mTex, (const TexAnimParams *)pTexAnimCompParams, ptTexAnimState, tTexFramesInfo);
    }

    const TexAnimCompParams * ptParams = (const TexAnimCompParams *)pTexAnimCompParams;
    switch (ptParams->eCompType) {
    case TEXANIMCOMP_SIMPLETRANS:
    case TEXANIMCOMP_SIMPLETRANS2: {
        const TexAnimCompSimpleTransParams * ptTrans = (const TexAnimCompSimpleTransParams *)ptParams;
        mTex._11 = ptTrans->mTex_11;
        mTex._12 = 0.0f;
        mTex._21 = 0.0f;
        mTex._22 = ptTrans->mTex_22;
        break;
    }
    case TEXANIMCOMP_SIMPLETRANSVEL: {
        const TexAnimCompSimpleTransVelParams * ptTransVel = (const TexAnimCompSimpleTransVelParams *)ptParams;
        float f11 = ptTransVel->mTex_11;
        float f22 = ptTransVel->mTex_22;
        if (!(f11 > 0.0f && f22 > 0.0f)) {
            f11 = 1.0f;
            f22 = 1.0f;
        }
        mTex._11 = f11;
        mTex._12 = 0.0f;
        mTex._21 = 0.0f;
        mTex._22 = f22;
        break;
    }
    default:
        return TAMT_ROTATE_TRANSLATE;
    }
    mTex._41 = tTexAnimState.flUTrans;
    mTex._42 = tTexAnimState.flVTrans;
    return TAMT_ROTATE_TRANSLATE;
}












































void TexAnimApply(const TextureBinding * ptTexBinding, const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const TexAnimState & tTexAnimState, const TextureDefinition * ptTextureDef) {
    if (pTexAnimCompParams == NULL) {
        GfxShaderTexMatrixSetIdentity(ptTexBinding);
        return;
    }
    Matrix4x4 mTex;
    TexAnimGetTexMatrix(mTex, pTexAnimCompParams, dwTexAnimCompParamsSize, &tTexAnimState, ptTextureDef);
    GfxShaderTexMatrixSet(ptTexBinding, mTex);
}














TexAnimState * MaterialAllocTexAnimState(const UberMaterial & tUberMaterial, int32 & nTexAnimStateCount) {
    TexAnimState * arTexAnimStates = NULL;
    int32 nCount = MaterialGetTextureCount(tUberMaterial);
    if (nCount > 0) {
        if (nCount <= MAX_POOLED_TEXANIMSTATES) {
            arTexAnimStates = (TexAnimState *)CGameGet().ptTexAnimGlobals->poolTexAnimStateArrays.Alloc_Tracked();
        } else {
            arTexAnimStates = (TexAnimState *)SigmaMemoryNew(sizeof(TexAnimState) * nCount, 4, NULL, FALSE);
        }
        SigmaMemoryClear(arTexAnimStates, sizeof(TexAnimState) * nCount);
    }
    nTexAnimStateCount = nCount;
    return arTexAnimStates;
}

















void MaterialFreeTexAnimState(TexAnimState *& arTexAnimStates) {
    if (arTexAnimStates == NULL) {
        return;
    }
    ChainedFixedBlockAllocator & tPool = CGameGet().ptTexAnimGlobals->poolTexAnimStateArrays;
    if (tPool.ContainsMem(arTexAnimStates)) {
        tPool.Free(arTexAnimStates);
    } else {
        SigmaMemoryFree(arTexAnimStates, NULL);
    }
    arTexAnimStates = NULL;
}










TexAnimState * TexAnimRenderAsyncCopy(int32 nCount, const TexAnimState * ptTexAnimState, const UberMaterial * ptUberMaterial, ShaderTex eOverlayShaderTex) {
    if (nCount == 0) {
        return NULL;
    }

    const MaterialTextureEntry * ptMatTextures = ptUberMaterial ? (const MaterialTextureEntry *)ptUberMaterial->ptMatTexList : NULL;
    int32 nSize = nCount * sizeof(TexAnimState);
    TexAnimState * ptTexAnimStateCopy = (TexAnimState *)CGameAsyncRenderGetMemory(nSize);
    BOOL fAnyUberMaterial = ptUberMaterial != NULL;

    for (int32 nTexAnimIndex = 0; nTexAnimIndex < nCount; nTexAnimIndex++) {
        if (fAnyUberMaterial) {
            const MaterialTextureEntry & tMatTexture = ptMatTextures[nTexAnimIndex];
            const MaterialTexture & tMatTex = tMatTexture.tMatTex;
            if (tMatTex.snoTex == SNO_NONE || (const CHAR *)tMatTex.pTexAnimCompParams == NULL) {
                continue;
            }
            if (tMatTexture.eShaderTex != eOverlayShaderTex && tMatTexture.eShaderTex >= SHADERTEX_OVERLAY_BEGIN && tMatTexture.eShaderTex <= SHADERTEX_OVERLAY_END) {
                continue;
            }
        }

        const TexAnimState & tTexAnimState = ptTexAnimState[nTexAnimIndex];
        TexAnimState & tTexAnimStateRender = ptTexAnimStateCopy[nTexAnimIndex];
        float flUTrans = tTexAnimState.flUTrans;
        float flVTrans = tTexAnimState.flVTrans;
        Angle aRot = tTexAnimState.aRot;
        SNO snoTex = tTexAnimState.snoTex;
        int32 nFrame = tTexAnimState.tAnimState.nFrame;
        int32 nBone = tTexAnimState.nBone;
        tTexAnimStateRender.flUTrans = flUTrans;
        tTexAnimStateRender.flVTrans = flVTrans;
        tTexAnimStateRender.aRot = aRot;
        tTexAnimStateRender.snoTex = snoTex;
        tTexAnimStateRender.tAnimState.nFrame = nFrame;
        if (nBone >= 0 && tTexAnimState.ptRActor != NULL) {
            tTexAnimStateRender.nBone = nBone;
            PRSTransform * ptTransform = (PRSTransform *)CGameAsyncRenderGetMemory(sizeof(PRSTransform));
            const ActorAnim & tActorAnim = *tTexAnimState.ptRActor->ptActorAnim;
            *ptTransform = tActorAnim.ptProcessedBoneList[tTexAnimState.nBone].modelTransform;
            tTexAnimStateRender.ptTransform = ptTransform;
        } else {
            tTexAnimStateRender.nBone = -1;
        }
    }
    return ptTexAnimStateCopy;
}
