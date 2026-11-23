// Original file: X/Client/Gfx/GfxShader.cpp
// All functions written. The tag tables
// scgtVSTagsOfInterest / scgtPSTagsOfInterest are not written.
// GfxSetShaderState ignores its ePerfBin parameter, so GfxShaderDraw does not pass it (the compiler drops
// the argument because the definition is in this file).
// The loops of GfxSetShaderState have source lines the DWARF shows nothing for (672-703 for example);
// only what compiles to code is written.
//
// The values of the scgtNull* lights are the bytes of the original image (tools/data.py).
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "GfxInternal.h"
#include "GfxCommon.h"
#include "../Definitions/ShaderCommon.h"
#include "../Definitions/ShaderDefinition.h"
#include "NX64/NX64NVN.h"
BOOL AppDrawFlagIsSet( int32 nFlag );  // X/Common/Game.h
void GfxShaderInit( const ShaderDefinition * pdefShader, const int32 nPassIndex );
#include "../../Sigma/Math/Matrix.h"
BOOL GfxFogEnabled();  // X/Client/Gfx/Gfx.cpp
static void sSetStateBlockStates( const RenderPass & tPass );
static ShaderTex sGetShaderTex( const RenderPass & tPass, const uint32 dwStage );
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../Definitions/PreferencesDefinition.h"
uint32 GfxGetShadowTech();  // X/Client/Gfx/Gfx.cpp:1191
#include "../Prefetch.h"
#include "GfxIB.h"
#include "GfxVB.h"
BOOL GfxSetShaderState( const PerfBin ePerfBin );
void GfxDraw( const DrawMode eDrawMode, const uint32 dwBaseIndexOffset, const uint32 dwMinVertexIndex, const uint32 dwNumVertices, const uint32 dwStartIndex, const uint32 dwPrimCount, const PerfBin ePerfBin );  // X/Client/Gfx/Gfx.cpp:2526
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/TagMaps/VertexShaderTags.h"
#include "../CGame.h"
static BOOL sSearchShaderCache( XGrowableList<ShaderData *, ShaderData *> & tCache, uint32 dwKey, XPtr64<ShaderData> & dst );
void sReallyDeleteVertexShader( void * ptVSD, void * ); void sReallyDeletePixelShader( void * ptPSD, void * );
#include "GfxShader.h"
#include "../RWindow.h"
#include "../CWorld.h"
#include "../Camera/CameraManager.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraLocation.h"
#include "../Definitions/TextureDefinition.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GlobalSNO.h"
void GfxFogGetParams( RGBAColor & rgbaFogColor, float & flFogStart, float & flFogEnd, float & flHeightFogStart, float & flHeightFogEnd );  // X/Client/Gfx/Gfx.cpp:2507
void GfxViewportScaleBiasGet( Vector4D & vScaleBias );  // X/Client/Gfx/Gfx.cpp:2326
void GfxGetRenderTargetInvFloatDimensions( float & flInvWidth, float & flInvHeight );  // X/Client/Gfx/Gfx.cpp:3489
const Matrix4x4 & GfxWorldViewMatrixGet();  // X/Client/Gfx/Gfx.cpp:2103
const Vector3D & GfxFixedPointPosScaleGet();  // X/Client/Gfx/Gfx.cpp:2031
const Vector3D & GfxFixedPointPosOffsetGet();  // X/Client/Gfx/Gfx.cpp:2042
const Quaternion & GfxCameraBillboardGet();  // X/Client/Gfx/Gfx.cpp:2435
Matrix4x4 PostFXGetSSAOViewProjTranspose();  // X/Client/PostFX.cpp:409
SNO SubObjectGfxGetWaterTexture();  // X/Client/SubObjectGfx.cpp:3601
// X/Client/Weather.h:12 and :22 (also defined locally in CGame.cpp and Weather.cpp)
struct WeatherRenderFogState { BOOL bFogEnabled; RGBAColor rgbaFogColor; float flFogNear; float flFogEnd; float flHeightFogNear; float flHeightFogEnd; };
struct WeatherRenderData { float flIntensity; WeatherRenderFogState tFogState; };
namespace Weather { const WeatherRenderData * GetRenderData( const WeatherRWindowData & tData ); }
static void sSetCommonVSConstants();
#include "GfxMaterial.h"
#include "../PostFX.h"
#include "../SubObjectGfx.h"
#include "../TexAnimation.h"
SNO SubObjectGfxGetInteractiveFogTexture();  // X/Client/SubObjectGfx.cpp:3616
void SubObjectGfxGetShadowColorData( SNO & snoTex );  // X/Client/SubObjectGfx.cpp:3652
void TexAnimApply( const TextureBinding * ptTexBinding, const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const TexAnimState & tTexAnimState, const TextureDefinition * ptTextureDef );  // X/Client/TexAnimation.cpp:1048
int32 MaterialGetTextureCount( const UberMaterial & tUberMaterial );  // X/Client/Gfx/GfxMaterial.cpp:412
GfxAPI GfxGetSystemID();  // X/Client/Gfx/Gfx.cpp:994
const Matrix4x4 & GfxProjectionMatrixGet();  // X/Client/Gfx/Gfx.cpp:2347
typedef int32 OverlayType;  // same typedef as src/X/Client/RActors/RActors.h:47 (not included here: heavy header)
ShaderTex OverlayTypeToShaderTex( const int32 eOverlayType );  // X/Client/Gfx/GfxMaterial.cpp:552 (parameter type: OverlayType)
const MaterialTextureEntry * MaterialLookup( const MaterialTextureEntry * arMaterialTex, int32 nCount, ShaderTex eShaderTex );  // X/Client/Gfx/GfxMaterial.cpp:310
int32 TexAnimGetTexMatrix( Matrix4x4 & mTex, const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const TexAnimState * ptTexAnimState, const TextureDefinition * ptTextureDef );  // X/Client/TexAnimation.cpp:707 (returns TexAnimMatrixType)


static const DirectionalLight scgtNullDirLight =
{
    Vector3D( 1.0f, 0.0f, 0.0f ),
    0.0f,
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
};

static const PointLight scgtNullPointLight =
{
    Vector4D( 0.0f, 0.0f, 0.0f, 1.0f ),
    { 1.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
};

static const SpotLight scgtNullSpotLight =
{
    Vector3D( 0.0f, 0.0f, 0.0f ),
    0.0f,
    Vector3D( 0.0f, 0.0f, -1.0f ),
    0.0f,
    { 0.0f, 0.0f, 0.999f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
};

static const CylindricalLight scgtNullCylindricalLight =
{
    Vector3D( 0.0f, 0.0f, 0.0f ),
    0.0f,
    Vector3D( 0.0f, 0.0f, 0.0f ),
    0.0f,
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
};

static const PointLinearLight scgtNullPointLinearLight =
{
    Vector3D( 0.0f, 0.0f, 0.0f ),
    0.0f,
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
};









































































































































































static ShaderConstFastBinding stEmptyShaderConstantFastBindings[86];







void GfxShaderInitializeLastState()
{



    for ( int32 worker = 0; worker < 1; worker++ )
    {

        RenderState & tLastRenderState = gptGfxData->workerData[worker].tRenderStateLast;
        RenderWorkerData & workerData = gptGfxData->workerData[worker];
        RenderState & tRenderState = workerData.tRenderState;
        SigmaMemoryClear( &tLastRenderState, sizeof( RenderState ) );

        tLastRenderState.ibid = -1;

        for ( uint32 i = 0; i < 3; i++ )
        {
            tLastRenderState.vbid[i] = -1;
            tLastRenderState.dwStreamOffset[i] = 0;
        }


        for ( uint32 i = 0; i < 12; i++ )
            tRenderState.fTexMatrixModified[i] = TRUE;

        tLastRenderState.dwStageCount = 0;
        tLastRenderState.rgbaTexFactor.r = 0x12; tLastRenderState.rgbaTexFactor.g = 0x23; tLastRenderState.rgbaTexFactor.b = 0x45; tLastRenderState.rgbaTexFactor.a = 0x67;
        tLastRenderState.eCullMode = CULLMODE_UNKNOWN;
        tLastRenderState.fHWFogEnable = 2;


        for ( uint32 dwStage = 0; dwStage < 24; dwStage++ )
            tLastRenderState.snoTex[dwStage] = -2;


        tRenderState.nNonNullDirLightCount = 0;

        for ( uint32 i = 0; i < 16; i++ )
            tRenderState.tDirLightList[i] = scgtNullDirLight;


        tRenderState.nNonNullPointLightCount = 0;

        for ( uint32 i = 0; i < 16; i++ )
            tRenderState.tPointLightList[i] = scgtNullPointLight;


        tRenderState.nNonNullSpotLightCount = 0;

        for ( uint32 i = 0; i < 16; i++ )
            tRenderState.tSpotLightList[i] = scgtNullSpotLight;


        tRenderState.nNonNullCylindricalLightCount = 0;

        for ( uint32 i = 0; i < 16; i++ )
            tRenderState.tCylLightList[i] = scgtNullCylindricalLight;


        tRenderState.nNonNullPointLinearLightCount = 0;

        for ( uint32 i = 0; i < 16; i++ )
            tRenderState.tPointLinearLightList[i] = scgtNullPointLinearLight;
    }


    for ( uint32 i = 0; i < 86; i++ )
        stEmptyShaderConstantFastBindings[i].dwFlags = 0;








}





void GfxShaderInit( const SNO sno, const int32 nPassIndex )
{
    GfxShaderInit( (const ShaderDefinition *)gptGroupDefShader->LockRFast( sno ), nPassIndex );
}























void GfxShaderInit( const ShaderDefinition * pdefShader, const int32 nPassIndex )
{

    ShaderDefinitionForceLoad( pdefShader, nPassIndex );


    const RenderPass & tShaderPass = pdefShader->ptPasses[nPassIndex];
    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];


    RenderState & tRenderStateLast = workerData.tRenderStateLast;
    RenderState & tRenderState = workerData.tRenderState;
    if ( tShaderPass.tPostprocessed.ptStateBlock != tRenderStateLast.ptStateBlock )
    {









        if ( AppDrawFlagIsSet( 0x70 ) )
        {
            sSetStateBlockStates( tShaderPass );
        }
        else
        {

            gptGFXAPI->StateBlockApply( tShaderPass.tPostprocessed.ptStateBlock );
        }

        tRenderStateLast.ptStateBlock = tShaderPass.tPostprocessed.ptStateBlock;
    }

    tRenderStateLast.snoShader = tRenderState.snoShader;
    tRenderStateLast.nPassIndex = tRenderState.nPassIndex;

    tRenderState.snoShader = pdefShader->tHeader.snoHandle;
    tRenderState.nPassIndex = nPassIndex;


    tRenderState.ibid = -1;
    tRenderState.dwIndexOffset = 0;
    for ( uint32 i = 0; i < 3; i++ ) {
        tRenderState.vbid[i] = -1;
        tRenderState.dwStreamOffset[i] = 0;
    }

    gptGFXAPI->SetVertexShader( pdefShader, nPassIndex );
    gptGFXAPI->SetPixelShader( pdefShader, nPassIndex );

    workerData.ptShaderConstantFastBindings = gptGFXAPI->GetShaderConstantFastBindings();
    if ( !workerData.ptShaderConstantFastBindings )
        workerData.ptShaderConstantFastBindings = stEmptyShaderConstantFastBindings;

















    tRenderState.dwStageCount = tShaderPass.dwStageCount;

    TextureBinding * ptTexBinding = workerData.ptTexBinding;

    SigmaMemoryFill( ptTexBinding, 0xff, sizeof( TextureBinding ) * 62 );

    uint32 dwStageCount = tShaderPass.dwStageCount;
    ShaderTex * ptShaderTex = workerData.eShaderTex;
    SNO * snoTex = tRenderState.snoTex;
    Matrix4x4 * mTex = tRenderState.mTex;
    BOOL * fTexMatrixModified = tRenderState.fTexMatrixModified;

    for ( uint32 dwStage = 0; dwStage < dwStageCount; dwStage++ )
    {
        ShaderTex eShaderTex = sGetShaderTex( tShaderPass, dwStage );
        ptShaderTex[dwStage] = eShaderTex;
        ptTexBinding[eShaderTex].nStage = dwStage;
        snoTex[dwStage] = SNO_NONE;

        if ( dwStage < 12 && fTexMatrixModified[dwStage] )
        {
            mTex[dwStage] = cgm4x4Identity;
            fTexMatrixModified[dwStage] = FALSE;
        }
    }


    tRenderState.snoLightMapTex = SNO_NONE;


    tRenderState.eCullMode = tShaderPass.tRenderParams.eCullMode;
    tRenderStateLast.eCullMode = tRenderState.eCullMode;
    tRenderState.flZBias = tShaderPass.tRenderParams.flZBias;
    tRenderState.fHWFogEnable = tShaderPass.tRenderParams.fFogEnable;
    tRenderState.rgbaTexFactor = tShaderPass.tRenderParams.rgbaTexFactor;
    tRenderState.rgbaAlphaTestRef.a = tShaderPass.tRenderParams.bAlphaTestRefValue;


    if ( *(const uint32 *)&tShaderPass.tRenderParams.rgbaTexFactor == 0xffffffff )
    {
        tRenderState.rgbaTexFactorValue.Set( 1.0f, 1.0f, 1.0f, 1.0f );
    }
    else
    {
        tRenderState.rgbaTexFactorValue.Set( tRenderState.rgbaTexFactor );
    }

    if ( !GfxFogEnabled() )
    {
        tRenderState.fHWFogEnable = FALSE;
    }
}



static ShaderTex sGetShaderTex( const RenderPass & tPass, const uint32 dwStage )
{


    if ( dwStage >= tPass.dwStageCount )
    {
        return SHADERTEX_NONE;
    }



    return tPass.ptsParams[dwStage].eShaderTex;
}






BOOL GfxSetShaderState( const PerfBin ePerfBin )
{

    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];


    RenderState & tRenderState = workerData.tRenderState;
    RenderState & tRenderStateLast = workerData.tRenderStateLast;
    const uint32 dwStageCount = tRenderState.dwStageCount;


























    int32 nPrevTexSwitchCount = workerData.tStats.tCurrent.nTextureSwitches;

    if ( tRenderState.fHWFogEnable != tRenderStateLast.fHWFogEnable )
    {
        gptGFXAPI->FogEnable( tRenderState.fHWFogEnable );
        tRenderStateLast.fHWFogEnable = tRenderState.fHWFogEnable;
    }
    if ( tRenderStateLast.rgbaFogColor != tRenderState.rgbaFogColor )
    {
        gptGFXAPI->FogSetColor( tRenderState.rgbaFogColor );
        tRenderStateLast.rgbaFogColor = tRenderState.rgbaFogColor;
        tRenderStateLast.rgbaFogColorValue = tRenderState.rgbaFogColorValue;
    }

    if ( tRenderState.nNumBones != tRenderStateLast.nNumBones )
    {
        gptGFXAPI->SetNumBones( tRenderState.nNumBones );
        tRenderStateLast.nNumBones = tRenderState.nNumBones;
    }

    tRenderStateLast.flFogStart = tRenderState.flFogStart;
    tRenderStateLast.flFogEnd = tRenderState.flFogEnd;
    tRenderStateLast.flHeightFogStart = tRenderState.flHeightFogStart;
    tRenderStateLast.flHeightFogEnd = tRenderState.flHeightFogEnd;
    tRenderStateLast.rgbaAlphaTestRef = tRenderState.rgbaAlphaTestRef;
    sSetCommonVSConstants();


    uint32 dwMinStageMatrixCount = Math::Min( dwStageCount, (uint32)MAX_TEXTURE_MATRICES );

    BOOL fSameShader = ( tRenderState.snoShader == tRenderStateLast.snoShader ) && ( tRenderState.nPassIndex == tRenderStateLast.nPassIndex );


    for ( uint32 dwStage = 0; dwStage < dwMinStageMatrixCount; dwStage++ )
    {

        if ( fSameShader && !tRenderStateLast.fTexMatrixModified[dwStage] && !tRenderState.fTexMatrixModified[dwStage] )
        {
            tRenderStateLast.fTexMatrixModified[dwStage] = FALSE;
            continue;
        }

        const ShaderConstBinding * ptBinding = GfxShaderGetConstantBinding( VSC_MATRIX_TEX0 + dwStage );
        if ( ptBinding )
        {
            GfxShaderSetConstant( ptBinding, &tRenderState.mTex[dwStage], 4 );
        }

        tRenderStateLast.fTexMatrixModified[dwStage] = tRenderState.fTexMatrixModified[dwStage];
    }


    BOOL fInGame = ( CGameGetGameCommonData().GameModeGet() == GAMEMODE_PLAYING );
    BOOL fFewFramesAfterAnExpiredLoadingScreen = !CGameLoadingScreenIsEnabled() && CGameLoadingScreenHasExpired() && ( CGameGlobalsGet().dwLoadingScreenDoneFrame > sgtAppGlobals.dwFrameCounter - 2 );


    BOOL fForceTextureLoad = !fInGame || fFewFramesAfterAnExpiredLoadingScreen;



    for ( uint32 dwStage = 0; dwStage < dwStageCount; dwStage++ )
    {
        SNO snoTex = tRenderState.snoTex[dwStage];
        if ( snoTex == 0 )
        {
            snoTex = SNO_NONE;
        }




        if ( snoTex == SNO_NONE )
        {
            snoTex = GlobalSNOGet( GLOBALSNO_OPAQUE_BLACK_TEXTURE );
        }


        if ( snoTex != SNO_NONE )
        {

            if ( !fForceTextureLoad && !Prefetch::AddToQueue( SNOName( TEXTURE_SNO, snoTex ), 2 ) )
            {
                return FALSE;
            }
        }




























        if ( snoTex != tRenderStateLast.snoTex[dwStage] )
        {
            tRenderStateLast.snoTex[dwStage] = snoTex;

            HTexture htex = NULL;

            if ( snoTex != SNO_NONE )
            {
                TexData tTexData;
                TextureDefinitionSetTexture( snoTex, tTexData );

                htex = tTexData.htex;
            }

            gptGFXAPI->TextureSetCurrent( dwStage, htex );

            workerData.tStats.tCurrent.nTextureSwitches++;
        }
    }


    for ( uint32 dwStage = dwStageCount; dwStage < tRenderStateLast.dwStageCount; dwStage++ )
    {
        SNO snoTex = SNO_NONE;
        HTexture htex = NULL;
        if ( tRenderStateLast.snoTex[dwStage] != snoTex )
        {
            tRenderStateLast.snoTex[dwStage] = snoTex;

            gptGFXAPI->TextureSetCurrent( dwStage, htex );

            workerData.tStats.tCurrent.nTextureSwitches++;
        }
    }

    tRenderStateLast.dwStageCount = tRenderState.dwStageCount;

    if ( tRenderState.eCullMode != tRenderStateLast.eCullMode )
    {
        gptGFXAPI->SetCullMode( tRenderState.eCullMode );
        tRenderStateLast.eCullMode = tRenderState.eCullMode;
    }











































    if ( workerData.tStats.tCurrent.nTextureSwitches != nPrevTexSwitchCount )
    {
        workerData.tStats.tCurrent.nBatchedTextureSwitches++;
    }


    return TRUE;
}



void GfxShaderDraw( const DrawMode eDrawMode, const uint32 dwBaseIndexOffset, const uint32 dwMinVertexIndex, const uint32 dwNumVertices, const uint32 dwStartIndex, const uint32 dwPrimCount, const PerfBin ePerfBin )







{
    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];
    RenderState & tRenderState = workerData.tRenderState;
    RenderState & tRenderStateLast = workerData.tRenderStateLast;
    if ( !Prefetch::AddToQueue( SNOName( TEXTURE_SNO, tRenderState.snoLightMapTex ), 2 ) )
    {
        return;
    }






    if ( !GfxSetShaderState( ePerfBin ) )
    {
        return;
    }


    if ( tRenderState.ibid != tRenderStateLast.ibid )
    {
        GfxIBSetCurrent( tRenderState.ibid );

        tRenderStateLast.ibid = tRenderState.ibid;
    }

    for ( int32 i = 0; i < 3; i++ )
    {
        if ( tRenderStateLast.vbid[i] != tRenderState.vbid[i] ||
             tRenderStateLast.dwStreamOffset[i] != tRenderState.dwStreamOffset[i] )
        {
            GfxVBSetStream( i, tRenderState.dwStreamOffset[i], tRenderState.vbid[i] );

            workerData.tStats.tCurrent.nVBSwitches++;
            tRenderStateLast.vbid[i] = tRenderState.vbid[i];
            tRenderStateLast.dwStreamOffset[i] = tRenderState.dwStreamOffset[i];
        }
    }
    gptGFXAPI->SetVertexDeclaration();














    GfxDraw( eDrawMode, dwBaseIndexOffset, dwMinVertexIndex, dwNumVertices, dwStartIndex + tRenderState.dwIndexOffset, dwPrimCount, ePerfBin );
}




TextureBinding * GfxGetTextureBinding( const ShaderTex eShaderTex )
{
    TextureBinding * ptTexBinding = &gptGfxData->workerData[GfxGetWorkerIndex()].ptTexBinding[eShaderTex];
    if ( ptTexBinding->nStage == -1 )
        return NULL;
    return ptTexBinding;
}










































void GfxShaderSetTexture( const TextureBinding * ptTexBinding, const SNO sno )
{

    if ( !ptTexBinding )
    {
        return;
    }

    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];

    workerData.tRenderState.snoTex[ptTexBinding->nStage] = sno;
}





inline void GfxShaderSetLightmapTexture( const TextureBinding * ptTexBinding, const SNO sno )
{

    if ( !ptTexBinding )
    {
        return;
    }

    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];

    workerData.tRenderState.snoTex[ptTexBinding->nStage] = sno;


    workerData.tRenderState.snoLightMapTex = sno;
}






void GfxShaderSetTexture( const SNO snoTex, const ShaderTex eShaderTex )
{

    GfxShaderSetTexture( GfxGetTextureBinding( eShaderTex ), snoTex );
}






void GfxShaderSetLightmapTexture( const SNO snoTex, const ShaderTex eShaderTex )
{

    GfxShaderSetLightmapTexture( GfxGetTextureBinding( eShaderTex ), snoTex );
}















void GfxShaderSetTextures( const UberMaterial & tUberMaterial, const TexAnimState * ptTexAnimState, const LPCSTR szSubObjectName, const SNO snoAppearance )




{



    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];

    int32 nCount = MaterialGetTextureCount( tUberMaterial );
    const MaterialTextureEntry * ptMatTexList = tUberMaterial.ptMatTexList;
    const MaterialTextureEntry * ptMatTexPerShaderTex[NUM_SHADER_TEXTURES];
    for ( int32 i = 0; i < NUM_SHADER_TEXTURES; i++ )
    {
        ptMatTexPerShaderTex[i] = NULL;
    }

    for ( int32 i = 0; i < nCount; i++ )
    {
        ptMatTexPerShaderTex[ptMatTexList[i].eShaderTex] = &ptMatTexList[i];
    }

    uint32 dwStageCount = workerData.tRenderState.dwStageCount;
    for ( uint32 dwStage = 0; dwStage < dwStageCount; dwStage++ )
    {
        ShaderTex eShaderTex = workerData.eShaderTex[dwStage];
        if ( eShaderTex == SHADERTEX_NONE )
        {
            continue;
        }

        TextureBinding * ptTexBinding = GfxGetTextureBinding( eShaderTex );
        if ( !ptTexBinding )
        {
            continue;
        }

        switch ( eShaderTex )
        {





            case SHADERTEX_LIGHTMAP:
            {
                SNO snoTex = SNO_NONE;
                const MaterialTextureEntry * ptMatTexEntry = ptMatTexPerShaderTex[eShaderTex];
                if ( ptMatTexEntry )
                {
                    const MaterialTexture & tMatTex = ptMatTexEntry->tMatTex;
                    snoTex = tMatTex.snoTex;
                }

                if ( snoTex == SNO_NONE )
                {
                    snoTex = GlobalSNOGet( GLOBALSNO_SUBOBJECT_LIGHTMAP_TEXTURE );
                }

                GfxShaderSetLightmapTexture( ptTexBinding, snoTex );

                if ( ptMatTexEntry )
                {
                    const MaterialTexture & tMatTex = ptMatTexEntry->tMatTex;


                    TexAnimApply( ptTexBinding, tMatTex.pTexAnimCompParams, tMatTex.serTexAnimCompParams.dwSize, ptTexAnimState[ptMatTexEntry - ptMatTexList], NULL );
                }
                break;
            }



            case SHADERTEX_REFLECTION:

                if ( Prefs::GetVideoPrefs().GetReflectionQuality() > 0 )
                {
                    GfxShaderSetTexture( ptTexBinding, PostFXGetBuffer( EFFECTBUFFER_REFLECTION_0 ) );
                }
                else
                {
                    GfxShaderSetTexture( ptTexBinding, SNO_NONE );
                }
                break;
            case SHADERTEX_REFLECTION_1:
                if ( Prefs::GetVideoPrefs().GetReflectionQuality() > 0 )
                {
                    GfxShaderSetTexture( ptTexBinding, PostFXGetBuffer( EFFECTBUFFER_REFLECTION_1 ) );
                }
                else
                {
                    GfxShaderSetTexture( ptTexBinding, SNO_NONE );
                }
                break;

            case SHADERTEX_SSAO:
                GfxShaderSetTexture( ptTexBinding, PostFXGetBuffer( EFFECTBUFFER_SSAO ) );
                break;

            case SHADERTEX_REFRACTION:
                GfxShaderSetTexture( ptTexBinding, PostFXGetBuffer( EFFECTBUFFER_REFRACTION ) );
                break;

            case SHADERTEX_WATER:
            case SHADERTEX_INTERACTIVE_FOG:
            {
                Matrix4x4 mTex;
                const Camera & viewCam = CameraManagerGetCamera( CAMERA_CURRENT );
                const Camera & addCam = CameraManagerGetCamera( CAMERA_RIPPLEMAP );

                mTex = cgm4x4Identity;
                mTex._11 = 0.5f;
                mTex._41 = 0.5f;
                mTex._22 = -0.5f;
                mTex._42 = 0.5f;
                mTex = viewCam.mView.Inverse() * addCam.mView * addCam.mProjection * mTex;

                SNO snoTex = ( eShaderTex == SHADERTEX_WATER ) ?
                    SubObjectGfxGetWaterTexture() :
                    SubObjectGfxGetInteractiveFogTexture();
                GfxShaderSetTexture( ptTexBinding, snoTex );
                GfxShaderTexMatrixSet( ptTexBinding, mTex );
                break;
            }

            case SHADERTEX_VIGNETTE:

                GfxShaderSetTexture( ptTexBinding, GlobalSNOGet( GLOBALSNO_SUBOBJECT_VIGNETTE_TEXTURE ) );
                GfxShaderTexMatrixSet( ptTexBinding, GfxProjectionMatrixGet() * cgm4x4ProjectionToNDC );
                break;

            case SHADERTEX_IRRADIANCE:
            {

                const MaterialTextureEntry * ptMatTexEntry = ptMatTexPerShaderTex[eShaderTex];
                SNO snoTex = SNO_NONE;

                if ( ptMatTexEntry )
                {
                    const MaterialTexture & tMatTex = ptMatTexEntry->tMatTex;
                    snoTex = tMatTex.snoTex;
                }













                if ( snoTex == SNO_NONE )
                {
                    CWorld * ptCWorld = CWorldGetFromSWorldID( CGameGetCurrentSWorldID() );
                    snoTex = ptCWorld->snoIrradianceTex;

                    if ( snoTex == SNO_NONE )
                    {
                        snoTex = GlobalSNOGet( GLOBALSNO_SUBOBJECT_IRRADIANCE_TEXTURE );
                    }
                }













                GfxShaderSetTexture( ptTexBinding, snoTex );
                break;
            }

            case SHADERTEX_SHADOW:
            {
                SNO snoTex;
                Matrix4x4 mTex;
                SubObjectGfxGetShadowDepthData( snoTex, mTex );
                GfxShaderSetTexture( ptTexBinding, snoTex );
                GfxShaderTexMatrixSet( ptTexBinding, mTex );
                break;
            }

            case SHADERTEX_SHADOW_COLOR:
            {
                SNO snoTex;
                Matrix4x4 mTex;
                SubObjectGfxGetShadowColorData( snoTex );
                GfxShaderSetTexture( ptTexBinding, snoTex );

                break;
            }

            case SHADERTEX_SHADOW_MASK:

                GfxShaderSetTexture( ptTexBinding, GlobalSNOGet( GLOBALSNO_SUBOBJECT_SHADOW_MAP_MASK ) );
                break;

            case SHADERTEX_DYE_TINT_RAMP:



                GfxShaderSetTexture( ptTexBinding, GlobalSNOGet( GLOBALSNO_DYE_TINT_RAMP ) );
                break;

            case SHADERTEX_BANNER_TINT_RAMP:

                GfxShaderSetTexture( ptTexBinding, GlobalSNOGet( GLOBALSNO_BANNER_TINT_RAMP ) );
                break;

            case SHADERTEX_DEPTH:

                if ( GfxGetSystemID() == GFX_D3D11 || GfxGetSystemID() == GFX_PS4GNM || GfxGetSystemID() == GFX_XBOXONED3D )
                {
                    GfxShaderSetTexture( ptTexBinding, PostFXGetBuffer( EFFECTBUFFER_DEPTH_PREV ) );
                }
                else
                {
                    GfxShaderSetTexture( ptTexBinding, PostFXGetBuffer( EFFECTBUFFER_DEPTH ) );
                }
                break;

            case SHADERTEX_BUMPMAP:
            case SHADERTEX_BUMPMAP_1:
            case SHADERTEX_BUMPMAP_2:
            case SHADERTEX_BUMPMAP_3:
            case SHADERTEX_BUMPMAP_4:
            case SHADERTEX_BUMPMAP_5:
            case SHADERTEX_BUMPMAP_6:
            {
                const MaterialTextureEntry * ptMatTexEntry = ptMatTexPerShaderTex[eShaderTex];
                SNO snoTex = SNO_NONE;

                if ( ptMatTexEntry )
                {
                    snoTex = ptMatTexEntry->tMatTex.snoTex;
                }

                if ( snoTex == SNO_NONE )
                {
                    snoTex = GlobalSNOGet( GLOBALSNO_FLAT_NORMALMAP_TEXTURE );
                }








                GfxShaderSetTexture( ptTexBinding, snoTex );

                if ( ptMatTexEntry )
                {
                    const MaterialTexture & tMatTex = ptMatTexEntry->tMatTex;
                    TexAnimApply( ptTexBinding, tMatTex.pTexAnimCompParams, tMatTex.serTexAnimCompParams.dwSize, ptTexAnimState[ptMatTexEntry - ptMatTexList], NULL );
                }
                break;
            }

            case SHADERTEX_DIFFUSE_OVERLAY:
            case SHADERTEX_DIFFUSE_OVERLAY_1:
            case SHADERTEX_DIFFUSE_OVERLAY_2:
                break;

            default:
            {
                const MaterialTextureEntry * ptMatTexEntry = ptMatTexPerShaderTex[eShaderTex];
                SNO snoTex = SNO_NONE;

































































                if ( ptMatTexEntry )
                {
                    snoTex = ptMatTexEntry->tMatTex.snoTex;
                }

                GfxShaderSetTexture( ptTexBinding, snoTex );

                if ( ptMatTexEntry )
                {
                    const MaterialTexture & tMatTex = ptMatTexEntry->tMatTex;
                    TexAnimApply( ptTexBinding, tMatTex.pTexAnimCompParams, tMatTex.serTexAnimCompParams.dwSize, ptTexAnimState[ptMatTexEntry - ptMatTexList], NULL );
                }
                break;
            }
        }
    }







}




BOOL GfxShaderSetDiffuseOverlayTexture( const OverlayType eOverlayType, const OverlayProjectionAxis eOverlayProjectionAxis, const UberMaterial & tUberMaterial, const TexAnimState * ptTexAnimState, const int32 nTextAnimStateCount )





{
    if ( !ptTexAnimState )
    {
        return TRUE;
    }

    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];

    ShaderTex eShaderTexToUse;

    BOOL fBrokenOverlay = FALSE;
    uint32 dwStageCount = workerData.tRenderState.dwStageCount;
    for ( uint32 dwStage = 0; dwStage < dwStageCount; dwStage++ )
    {



        ShaderTex eShaderTex = workerData.eShaderTex[dwStage];

        if ( eShaderTex == SHADERTEX_DIFFUSE_OVERLAY || eShaderTex == SHADERTEX_DIFFUSE_OVERLAY_1 || eShaderTex == SHADERTEX_DIFFUSE_OVERLAY_2 )
        {


            if ( eShaderTex == SHADERTEX_DIFFUSE_OVERLAY )
            {
                eShaderTexToUse = OverlayTypeToShaderTex( eOverlayType );
                if ( eShaderTexToUse == SHADERTEX_NONE )
                {
                    eShaderTexToUse = SHADERTEX_DIFFUSE_OVERLAY;
                }
            }
            else
            {
                eShaderTexToUse = eShaderTex;
            }

            int32 nCount = MaterialGetTextureCount( tUberMaterial );
            const MaterialTextureEntry * ptMatTexList = tUberMaterial.ptMatTexList;

            SNO snoTex = SNO_NONE;
            const MaterialTextureEntry * ptMatTexEntry = MaterialLookup( ptMatTexList, nCount, eShaderTexToUse );
            if ( ptMatTexEntry )
            {
                snoTex = ptMatTexEntry->tMatTex.snoTex;
            }

            if ( snoTex == SNO_NONE )
            {
                if ( eShaderTex == SHADERTEX_DIFFUSE_OVERLAY )
                {
                    fBrokenOverlay = TRUE;
                }
                snoTex = gptGroupDefTexture->GetFallbackDataSNO();
            }

            TextureBinding * ptTexBinding = GfxGetTextureBinding( eShaderTex );
            if ( ptTexBinding )
            {



                GfxShaderSetTexture( ptTexBinding, snoTex );

                const Matrix4x4 cgmOPABase(
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f );

                Matrix4x4 mTex = cgmOPABase;

                switch ( eOverlayProjectionAxis )
                {
                    case OPA_FRONT:
                        mTex._21 = 1.0f;
                        mTex._32 = 1.0f;
                        break;
                    case OPA_SIDE:
                        mTex._31 = 1.0f;
                        mTex._12 = 1.0f;
                        break;
                    case OPA_TOP:
                        mTex._11 = 1.0f;
                        mTex._22 = 1.0f;
                        break;
                }






                if ( ptMatTexEntry )
                {

                    if ( ptMatTexEntry->tMatTex.pTexAnimCompParams )
                    {




                        int nIndex = ptMatTexEntry - ptMatTexList;



                        const TexAnimState & tTexAnimState = ptTexAnimState[nIndex];

                        Matrix4x4 mTexAnimated;

                        TexAnimGetTexMatrix( mTexAnimated, ptMatTexEntry->tMatTex.pTexAnimCompParams, ptMatTexEntry->tMatTex.serTexAnimCompParams.dwSize, &tTexAnimState, NULL );



                        mTex = mTex * mTexAnimated;
                    }
                }

                GfxShaderTexMatrixSet( ptTexBinding, mTex );
            }
        }
    }
    return !fBrokenOverlay;
}



void GfxShaderSetStream( const StreamIndex eStreamIndex, const uint32 dwStreamOffset, const VBID vbid )
{
    RenderState & tRenderState = gptGfxData->workerData[GfxGetWorkerIndex()].tRenderState;
    tRenderState.vbid[eStreamIndex] = vbid;
    tRenderState.dwStreamOffset[eStreamIndex] = dwStreamOffset;
}
























































void GfxShaderSetIB( const IBID ibid, const uint32 dwIndexOffset )
{
    RenderState & tRenderState = gptGfxData->workerData[GfxGetWorkerIndex()].tRenderState;
    tRenderState.ibid = ibid;
    tRenderState.dwIndexOffset = dwIndexOffset;
}











void GfxShaderSetNumBones( const int32 nNumBones )
{
    gptGfxData->workerData[GfxGetWorkerIndex()].tRenderState.nNumBones = nNumBones;
}








void GfxShaderSetTextureFactor( const RGBAColor & rgbaColor )
{
    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];

    workerData.tRenderState.rgbaTexFactor = rgbaColor;
    workerData.tRenderState.rgbaTexFactorValue.Set( rgbaColor );
}




void GfxShaderSetPointLinearLightCount( const int32 nPassIndex, const ShaderDefinition * pdefShader, const int32 nLightCount, const LightState *const * pArrayLights )
{
}
























// The Vector3D( 0, 0, 1 ) of the cylindrical case stands for a constant the binary folds; its source name is not known.

void GfxShaderSetLights( const Material & mat, const ShaderDefinition * pdefShader, const int32 nLightCount, const LightState *const * pArrayLights )




{
    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];

    const RGBAColorValue & rgbavalMatAmbient = mat.rgbavalAmbient;
    const RGBAColorValue & rgbavalMatDiffuse = mat.rgbavalDiffuse;
    const RGBAColorValue & rgbavalMatSpecular = mat.rgbavalSpecular;
    const RGBAColorValue & rgbavalMatEmissive = mat.rgbavalEmissive;
    RGBAColorValue rgbavalAmbient = rgbavalMatEmissive;

    DirectionalLight * tDirLightList = workerData.tRenderState.tDirLightList;
    PointLight * tPointLightList = workerData.tRenderState.tPointLightList;
    SpotLight * tSpotLightList = workerData.tRenderState.tSpotLightList;
    CylindricalLight * tCylLightList = workerData.tRenderState.tCylLightList;
    PointLinearLight * tPointLinearLightList = workerData.tRenderState.tPointLinearLightList;

    struct LightCounts
    {
        int32 nDirectional;
        int32 nPoint;
        int32 nPointLinear;
        int32 nSpot;
        int32 nCylindrical;
    };


    LightCounts tMaxLights;
    ShaderData * ptVSD = pdefShader->ptPasses[workerData.tRenderState.nPassIndex].tPostprocessed.ptVSD;
    tMaxLights.nDirectional = ptVSD->dwNumDirLights;
    tMaxLights.nPoint = ptVSD->dwNumPointLights;
    tMaxLights.nSpot = ptVSD->dwNumSpotLights;
    tMaxLights.nCylindrical = ptVSD->dwNumCylindricalLights;
    tMaxLights.nPointLinear = ptVSD->dwNumPointLinearLights;















    const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();

    const ShaderConstFastBinding * ptDirLightBinding = &ptFastBindings[VSC_DIRECTIONALLIGHTS];
    const ShaderConstFastBinding * ptPointLightBinding = &ptFastBindings[VSC_POINTLIGHTS];
    const ShaderConstFastBinding * ptSpotLightBinding = &ptFastBindings[VSC_SPOTLIGHTS];
    const ShaderConstFastBinding * ptCylindricalLightBinding = &ptFastBindings[VSC_CYLINDRICALLIGHTS];
    const ShaderConstFastBinding * ptPointLightLinearBinding = &ptFastBindings[VSC_POINTLINEARLIGHTS];
    const ShaderConstFastBinding * ptAmbientBinding = &ptFastBindings[VSC_AMBIENT];
    const ShaderConstFastBinding * ptSpecularPowerBinding = &ptFastBindings[VSC_SPECULARPOWER];













    LightCounts tLocalLights = { 0 };
    const Matrix4x4 & mView = workerData.tRenderState.mView;
    float fDirectional = 0.0f;
    float fPointLinear = 0.0f;
    for ( int32 i = 0; i < nLightCount; i++, pArrayLights++ )
    {
        const LightState * pLightState = *pArrayLights;
        const LightState & tLightState = *pLightState;
        const LightInterpState & tInterpState = tLightState.tInterpState;

        switch ( tLightState.eType )
        {
            case LIGHTTYPE_DIRECTIONAL:


                if ( tLocalLights.nDirectional < tMaxLights.nDirectional )
                {
                    DirectionalLight & tDirLight = tDirLightList[tLocalLights.nDirectional++];
                    tDirLight.wv = Matrix4x4::Transform3x3( tLightState.wv, mView );
                    tDirLight.rgbavalDiffuse = tInterpState.rgbavalDiffuse * rgbavalMatDiffuse;
                    tDirLight.rgbavalSpecular = tInterpState.rgbavalSpecular * rgbavalMatSpecular;


                    rgbavalAmbient += tInterpState.rgbavalAmbient;
                    fDirectional += 1.0f;
                }
                else if ( tLocalLights.nPoint < tMaxLights.nPoint )
                {
                    PointLight & tPointLight = tPointLightList[tLocalLights.nPoint++];
                    Vector3D v3d = -Matrix4x4::Transform3x3( tLightState.wv, mView );
                    tPointLight.wp.x = v3d.x; tPointLight.wp.y = v3d.y; tPointLight.wp.z = v3d.z; tPointLight.wp.w = 0.0f;
                    tPointLight.flAtten[0] = 1.0f;
                    tPointLight.flAtten[1] = 0.0f;
                    tPointLight.flAtten[2] = 0.0f;
                    tPointLight.flAtten[3] = 1.0f;


                    tPointLight.rgbavalAmbient = tInterpState.rgbavalAmbient;
                    tPointLight.rgbavalDiffuse = tInterpState.rgbavalDiffuse * rgbavalMatDiffuse;
                    tPointLight.rgbavalSpecular = tInterpState.rgbavalSpecular * rgbavalMatSpecular;

                }
                break;
            case LIGHTTYPE_POINT:
                if ( tLocalLights.nPoint < tMaxLights.nPoint )
                {
                    PointLight & tPointLight = tPointLightList[tLocalLights.nPoint++];
                    Vector3D v3d = tLightState.wp * mView;
                    tPointLight.wp.x = v3d.x; tPointLight.wp.y = v3d.y; tPointLight.wp.z = v3d.z; tPointLight.wp.w = 1.0f;
                    tPointLight.flAtten[0] = tInterpState.flAtten[0];
                    tPointLight.flAtten[1] = tInterpState.flAtten[1];
                    tPointLight.flAtten[2] = tInterpState.flAtten[2];
                    tPointLight.flAtten[3] = 1.0f;
                    tPointLight.rgbavalAmbient = tInterpState.rgbavalAmbient * rgbavalMatAmbient;
                    tPointLight.rgbavalDiffuse = tInterpState.rgbavalDiffuse * rgbavalMatDiffuse;
                    tPointLight.rgbavalSpecular = tInterpState.rgbavalSpecular * rgbavalMatSpecular;

                }
                break;
            case LIGHTTYPE_SPOTLIGHT:
                if ( tLocalLights.nSpot < tMaxLights.nSpot )
                {
                    SpotLight & tSpotLight = tSpotLightList[tLocalLights.nSpot++];
                    tSpotLight.wp = tLightState.wp * mView;
                    tSpotLight.wv = Matrix4x4::Transform3x3( tLightState.wv, mView );
                    tSpotLight.flRadii[0] = 1.0f / Math::Max( tInterpState.wdRadius - tInterpState.wdInnerRadius, 0.001f );
                    tSpotLight.flRadii[1] = tInterpState.wdRadius;
                    tSpotLight.flRadii[2] = Math::Cos( tInterpState.aFalloff );
                    tSpotLight.flRadii[3] = Math::Cos( tInterpState.aHotspot );
                    tSpotLight.rgbavalAmbient = tInterpState.rgbavalAmbient * rgbavalMatAmbient;
                    tSpotLight.rgbavalDiffuse = tInterpState.rgbavalDiffuse * rgbavalMatDiffuse;
                    tSpotLight.rgbavalSpecular = tInterpState.rgbavalSpecular * rgbavalMatSpecular;

                }
                break;
            case LIGHTTYPE_CYLINDRICAL:
                if ( tLocalLights.nCylindrical < tMaxLights.nCylindrical )
                {
                    CylindricalLight & tCylLight = tCylLightList[tLocalLights.nCylindrical++];
                    tCylLight.wp = tLightState.wp * mView;
                    tCylLight.wv = Matrix4x4::Transform3x3( Vector3D( 0.0f, 0.0f, 1.0f ), mView );
                    tCylLight.flRadii[0] = 1.0f / Math::Max( tInterpState.wdRadius - tInterpState.wdInnerRadius, 0.001f );
                    tCylLight.flRadii[1] = tInterpState.wdRadius;
                    tCylLight.rgbavalAmbient = tInterpState.rgbavalAmbient * rgbavalMatAmbient;
                    tCylLight.rgbavalDiffuse = tInterpState.rgbavalDiffuse * rgbavalMatDiffuse;
                    tCylLight.rgbavalSpecular = tInterpState.rgbavalSpecular * rgbavalMatSpecular;

                }
                break;
            case LIGHTTYPE_POINT_LINEAR:
                if ( tLocalLights.nPointLinear < tMaxLights.nPointLinear )
                {
                    PointLinearLight & tPointLight = tPointLinearLightList[tLocalLights.nPointLinear++];
                    RGBAColorValue rgbavalAmbientScoped = tInterpState.rgbavalAmbient * rgbavalMatAmbient;
                    RGBAColorValue rgbavalDiffuse = tInterpState.rgbavalDiffuse * rgbavalMatDiffuse;

                    tPointLight.wp = tLightState.wp * mView;
                    ShaderFloatSet( tPointLight.rgbavalAmbient_radiiA, rgbavalAmbientScoped.r, rgbavalAmbientScoped.g, rgbavalAmbientScoped.b, 1.0f / Math::Max( tInterpState.wdRadius - tInterpState.wdInnerRadius, 0.001f ) );
                    ShaderFloatSet( tPointLight.rgbavalDiffuse_radiiB, rgbavalDiffuse.r, rgbavalDiffuse.g, rgbavalDiffuse.b, tInterpState.wdRadius );
                    tPointLight.rgbavalSpecular = tInterpState.rgbavalSpecular * rgbavalMatSpecular;
                    fPointLinear += 1.0f;
                }
                break;
        }
    }

    int32 nNonNullDirLightCount = workerData.tRenderState.nNonNullDirLightCount;
    int32 nNonNullPointLightCount = workerData.tRenderState.nNonNullPointLightCount;
    int32 nNonNullSpotLightCount = workerData.tRenderState.nNonNullSpotLightCount;
    int32 nNonNullCylindricalLightCount = workerData.tRenderState.nNonNullCylindricalLightCount;
    int32 nNonNullPointLinearLightCount = workerData.tRenderState.nNonNullPointLinearLightCount;


    for ( int32 i = tLocalLights.nDirectional; i < nNonNullDirLightCount; i++ )
    {
        tDirLightList[i] = scgtNullDirLight;
    }
    for ( int32 i = tLocalLights.nPoint; i < nNonNullPointLightCount; i++ )
    {
        tPointLightList[i] = scgtNullPointLight;
    }
    for ( int32 i = tLocalLights.nSpot; i < nNonNullSpotLightCount; i++ )
    {
        tSpotLightList[i] = scgtNullSpotLight;
    }
    for ( int32 i = tLocalLights.nCylindrical; i < nNonNullCylindricalLightCount; i++ )
    {
        tCylLightList[i] = scgtNullCylindricalLight;
    }
    for ( int32 i = tLocalLights.nPointLinear; i < nNonNullPointLinearLightCount; i++ )
    {
        tPointLinearLightList[i] = scgtNullPointLinearLight;
    }


    if ( workerData.tRenderState.snoShader == workerData.tRenderStateLast.snoShader &&
         workerData.tRenderState.nPassIndex == workerData.tRenderStateLast.nPassIndex )
    {
        tMaxLights.nDirectional = Math::Max( tLocalLights.nDirectional, nNonNullDirLightCount );
        tMaxLights.nPoint = Math::Max( tLocalLights.nPoint, nNonNullPointLightCount );
        tMaxLights.nSpot = Math::Max( tLocalLights.nSpot, nNonNullSpotLightCount );
        tMaxLights.nCylindrical = Math::Max( tLocalLights.nCylindrical, nNonNullCylindricalLightCount );
        tMaxLights.nPointLinear = Math::Max( tLocalLights.nPointLinear, nNonNullPointLinearLightCount );
    }


    workerData.tRenderState.nNonNullDirLightCount = tLocalLights.nDirectional;
    workerData.tRenderState.nNonNullPointLightCount = tLocalLights.nPoint;
    workerData.tRenderState.nNonNullSpotLightCount = tLocalLights.nSpot;
    workerData.tRenderState.nNonNullCylindricalLightCount = tLocalLights.nCylindrical;
    workerData.tRenderState.nNonNullPointLinearLightCount = tLocalLights.nPointLinear;


    if ( tMaxLights.nDirectional )
    {
        const int32 DIRECTION_LIGHT_SIZE = sizeof( DirectionalLight ) / sizeof( ShaderFloat );

        GfxShaderSetConstant( ptDirLightBinding, tDirLightList, tMaxLights.nDirectional * DIRECTION_LIGHT_SIZE );
    }
    if ( tMaxLights.nPoint )
    {
        const int32 POINT_LIGHT_SIZE = sizeof( PointLight ) / sizeof( ShaderFloat );

        GfxShaderSetConstant( ptPointLightBinding, tPointLightList, tMaxLights.nPoint * POINT_LIGHT_SIZE );
        GfxShaderSetConstant( &ptFastBindings[VSC_LIGHTCOLOR], &tPointLightList[0].rgbavalDiffuse, 1 );
    }
    if ( tMaxLights.nSpot )
    {
        const int32 SPOT_LIGHT_SIZE = sizeof( SpotLight ) / sizeof( ShaderFloat );

        GfxShaderSetConstant( ptSpotLightBinding, tSpotLightList, tMaxLights.nSpot * SPOT_LIGHT_SIZE );
    }
    if ( tMaxLights.nCylindrical )
    {
        const int32 CYLINDRICAL_LIGHT_SIZE = sizeof( CylindricalLight ) / sizeof( ShaderFloat );

        GfxShaderSetConstant( ptCylindricalLightBinding, tCylLightList, tMaxLights.nCylindrical * CYLINDRICAL_LIGHT_SIZE );
    }
    if ( tMaxLights.nPointLinear )
    {
        const int32 POINT_LINEAR_LIGHT_SIZE = sizeof( PointLinearLight ) / sizeof( ShaderFloat );
        GfxShaderSetConstant( ptPointLightLinearBinding, tPointLinearLightList, tMaxLights.nPointLinear * POINT_LINEAR_LIGHT_SIZE );
    }

    {
        ShaderInt nNumLights =
        { tLocalLights.nPointLinear, tLocalLights.nDirectional, 0, 0 };



        ShaderFloat fNumLights =
        { fPointLinear, fDirectional, 0.0f, 0.0f };










        GfxShaderSetConstant( &ptFastBindings[VSC_NUM_LIGHTS_INT], nNumLights, 1 );
        GfxShaderSetConstant( &ptFastBindings[VSC_NUM_LIGHTS_FLOAT], fNumLights, 1 );
    }

    GfxShaderSetConstant( ptAmbientBinding, &rgbavalAmbient, 1 );


    {
        float flPower = mat.flPower;
        if ( flPower == 0.0f )
        {
            flPower = 1.0f;
        }

        ShaderFloat tParams = { flPower, 1.0f, 1.0f, 1.0f };



        GfxShaderSetConstant( ptSpecularPowerBinding, tParams, 1 );
    }
}

















static void sSetCommonVSConstants()
{

    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];
    const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();
    const CGame & tCGame = CGameGet();

    if ( GfxShaderHasConstant( ptFastBindings, VSC_MATRIX_WORLD ) )
    {
        GfxShaderSetConstant( &ptFastBindings[VSC_MATRIX_WORLD], &workerData.tRenderState.mWorldTranspose, 4 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_MATRIX_VIEW ) )
    {
        GfxShaderSetConstant( &ptFastBindings[VSC_MATRIX_VIEW], &workerData.tRenderState.mViewTranspose, 4 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_MATRIX_WV ) )
    {
        GfxShaderSetConstant( &ptFastBindings[VSC_MATRIX_WV], &workerData.tRenderState.mWorldViewTranspose, 4 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_MATRIX_WVP ) )
    {
        GfxShaderSetConstant( &ptFastBindings[VSC_MATRIX_WVP], &workerData.tRenderState.mWorldViewProjectionTranspose, 4 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_FOGPARAMS ) )
    {
        if ( GfxFogEnabled() )
        {
            RGBAColor rgbaFogColor;
            float flFogStart, flFogEnd, flHeightFogStart, flHeightFogEnd;

            GfxFogGetParams( rgbaFogColor, flFogStart, flFogEnd, flHeightFogStart, flHeightFogEnd );
            flFogEnd = Math::Max( flFogEnd, flFogStart + 0.000001f );
            float flInvFogRange = 1.0f / ( flFogEnd - flFogStart );

            flHeightFogStart = Math::Max( flHeightFogStart, flHeightFogEnd + 0.000001f );
            float flInvHeightFogRange = 1.0f / ( flHeightFogStart - flHeightFogEnd );

            ShaderFloat tRegFogParams =
            {
                -flInvFogRange,
                flFogEnd * flInvFogRange,
                -flInvHeightFogRange,
                flHeightFogStart * flInvHeightFogRange,
            };

            GfxShaderSetConstant( &ptFastBindings[VSC_FOGPARAMS], tRegFogParams, 1 );
        }
        else
        {
            ShaderFloat tRegFogParams = { 0.0f, 1.0f, 0.0f, 0.0f };



            GfxShaderSetConstant( &ptFastBindings[VSC_FOGPARAMS], tRegFogParams, 1 );
        }
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_VIEWPORT_UV_SCALE_BIAS ) )
    {
        Vector4D tViewport;
        GfxViewportScaleBiasGet( tViewport );
        GfxShaderSetConstant( &ptFastBindings[VSC_VIEWPORT_UV_SCALE_BIAS], &tViewport, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_WINDOW_RESOLUTION ) )
    {
        float flInvWidth, flInvHeight;
        ShaderFloat tParms;
        GfxGetRenderTargetInvFloatDimensions( flInvWidth, flInvHeight );
        tParms[0] = flInvWidth;
        tParms[1] = flInvHeight;
        tParms[2] = tParms[3] = 0.0f;

        GfxShaderSetConstant( &ptFastBindings[VSC_WINDOW_RESOLUTION], tParms, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_SCREEN_Z ) )
    {
        Vector3D wvScreenZ = Matrix4x4::Transform3x3( cgwvZAxis, GfxWorldViewMatrixGet() );
        Vector4D tParam( wvScreenZ.x, wvScreenZ.y, wvScreenZ.z, 1.0f );
        GfxShaderSetConstant( &ptFastBindings[VSC_SCREEN_Z], &tParam, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_VIEW_POS ) )
    {
        Vector3D wpViewPos = cgwvNull;
        if ( tCGame.ptCameraManager )
        {
            wpViewPos = CameraManagerGetCameraState( CAMERA_CURRENT ).transform.wp;
        }
        Vector4D tParam( wpViewPos.x, wpViewPos.y, wpViewPos.z, 1.0f );
        GfxShaderSetConstant( &ptFastBindings[VSC_VIEW_POS], &tParam, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_TIME ) )
    {
        ScopedDefinitionLockR<ShaderDefinition> pdefShader( workerData.tRenderState.snoShader );
        const RenderPass & tPass = pdefShader->ptPasses[workerData.tRenderState.nPassIndex];

        float flPeriod = TagMapGetFloat( tPass.hTagMap, TAG_VS_TIMER_PERIOD );
        float flClientTime = GetRWindowMgr()->GetCurrentRWindow().tAsyncData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_DRAW )].flClientTime;
        float flTrunc = Math::FloatTrunc( flClientTime / flPeriod );
        float normalizedClientTime = ( flClientTime - flTrunc * flPeriod ) / flPeriod;
        float periodicClientTime = normalizedClientTime * 2.0f * PI;

        ShaderFloat tParams =
        { flClientTime, normalizedClientTime, periodicClientTime, 0.0f };






        GfxShaderSetConstant( &ptFastBindings[VSC_TIME], tParams, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_FACTOR ) )
    {
        GfxShaderSetConstant( &ptFastBindings[VSC_FACTOR], &workerData.tRenderState.rgbaTexFactorValue, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_FOGCOLOR ) )
    {
        GfxShaderSetConstant( &ptFastBindings[VSC_FOGCOLOR], &workerData.tRenderState.rgbaFogColorValue, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_CAMERA_PARAMS ) )
    {
        ShaderFloat tParams;
        SigmaMemoryClear( tParams, sizeof( tParams ) );

        if ( tCGame.ptCameraManager )
        {
            const Camera & cam = CameraManagerGetCamera( GetRWindowMgr()->GetCurrentRWindow().GetCameraIndex() );

            tParams[0] = -cam.mProjection._43;
            tParams[1] = -cam.mProjection._33;
            tParams[2] = -cam.mProjection._34 / cam.mProjection._11;
            tParams[3] = -cam.mProjection._34 / cam.mProjection._22;
        }

        GfxShaderSetConstant( &ptFastBindings[VSC_CAMERA_PARAMS], tParams, 1 );
    }





































    if ( GfxShaderHasConstant( ptFastBindings, VSC_EDGE_FILTER ) )
    {
        float flInvWidth, flInvHeight;
        GfxGetRenderTargetInvFloatDimensions( flInvWidth, flInvHeight );

        ShaderFloat tParams;
        tParams[0] = flInvWidth;
        tParams[1] = flInvHeight;

        GfxShaderSetConstant( &ptFastBindings[VSC_EDGE_FILTER], tParams, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_WATER_TEXTURE_CONSTANTS ) )
    {
        float flInvWidth = 1.0f;
        if ( tCGame.ptSubObjectGlobals )
        {
            ScopedDefinitionLockR<TextureDefinition> pdefTexture( SubObjectGfxGetWaterTexture() );
            flInvWidth = 1.0f / (float)pdefTexture->dwWidth;
        }


        ShaderFloat tParams;
        tParams[0] = flInvWidth;
        tParams[1] = 0.0f;
        tParams[2] = flInvWidth;
        tParams[3] = 0.0f;

        GfxShaderSetConstant( &ptFastBindings[VSC_WATER_TEXTURE_CONSTANTS], tParams, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_WEATHER_INTENSITY ) )
    {
        const WeatherRenderData * ptWeatherData = Weather::GetRenderData( GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData() );
        ShaderFloat tParams;
        tParams[0] = ptWeatherData ? ptWeatherData->flIntensity : 0.0f;
        GfxShaderSetConstant( &ptFastBindings[VSC_WEATHER_INTENSITY], tParams, 1 );
    }










    if ( GfxShaderHasConstant( ptFastBindings, VSC_DEFORMATION_SCALE ) )
    {
        float flScale = 1.0f;
        if ( tCGame.ptCWorlds )
        {
            CWorld * ptCWorld = CWorldGetFromSWorldID( CGameGetCurrentSWorldID() );
            if ( ptCWorld )
            {
                flScale = ptCWorld->flDeformationScale;
            }
        }

        ShaderFloat tParams;
        tParams[0] = flScale;
        GfxShaderSetConstant( &ptFastBindings[VSC_DEFORMATION_SCALE], tParams, 1 );
    }


    if ( GfxShaderHasConstant( ptFastBindings, VSC_LIGHTMAP_V_SCALE ) && workerData.tRenderState.snoLightMapTex != SNO_NONE )
    {
        ScopedDefinitionLockR<TextureDefinition> pdefTexture( workerData.tRenderState.snoLightMapTex );
        ShaderFloat tParams;
        tParams[0] = pdefTexture->flConsoleLightmapVScale != 0.0f ? pdefTexture->flConsoleLightmapVScale : 1.0f;
        GfxShaderSetConstant( &ptFastBindings[VSC_LIGHTMAP_V_SCALE], tParams, 1 );
    }


    if ( GfxShaderHasConstant( ptFastBindings, VSC_VEC_FIXED_POINT_POS_SCALE ) )
    {
        Vector3D vecFixedPointPosScale( GfxFixedPointPosScaleGet() );
        Vector4D tParam( vecFixedPointPosScale.x, vecFixedPointPosScale.y, vecFixedPointPosScale.z, 1.0f );
        GfxShaderSetConstant( &ptFastBindings[VSC_VEC_FIXED_POINT_POS_SCALE], &tParam, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_VEC_FIXED_POINT_POS_OFFSET ) )
    {
        Vector3D vecFixedPointPosOffset( GfxFixedPointPosOffsetGet() );
        Vector4D tParam( vecFixedPointPosOffset.x, vecFixedPointPosOffset.y, vecFixedPointPosOffset.z, 0.0f );
        GfxShaderSetConstant( &ptFastBindings[VSC_VEC_FIXED_POINT_POS_OFFSET], &tParam, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_MATRIX_WORLD_IRRAD ) )
    {
        Matrix4x4 mWorldIrradTranspose = workerData.tRenderState.mIrradTranspose * workerData.tRenderState.mWorldTranspose;
        GfxShaderSetConstant( &ptFastBindings[VSC_MATRIX_WORLD_IRRAD], &mWorldIrradTranspose, 4 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_ALPHATEST_REF ) )
    {
        uint8 alpha = workerData.tRenderState.rgbaAlphaTestRef.a;
        RGBAColor rgba( alpha, alpha, alpha, alpha );
        RGBAColorValue rgbaval; rgbaval.Set( rgba );  // stand-in for the RGBAColorValue(const RGBAColor &) ctor (DWARF RGBAColor.h:125) our header lacks
        GfxShaderSetConstant( &ptFastBindings[VSC_ALPHATEST_REF], &rgbaval, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_QCAMERA_BILLBOARD ) )
    {
        GfxShaderSetConstant( &ptFastBindings[VSC_QCAMERA_BILLBOARD], &GfxCameraBillboardGet(), 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_MATRIX_PROJECTION ) )
    {
        GfxShaderSetConstant( &ptFastBindings[VSC_MATRIX_PROJECTION], &workerData.tRenderState.mProjectionTranspose, 4 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_VEC_CLIP_PLANE ) )
    {
        ShaderFloat tPlane[MAX_CLIP_PLANES];
        for ( int i = 0; i < MAX_CLIP_PLANES; i++ )
        {
            tPlane[i][0] = workerData.flClipPlane[i * 4 + 0];
            tPlane[i][1] = workerData.flClipPlane[i * 4 + 1];
            tPlane[i][2] = workerData.flClipPlane[i * 4 + 2];
            tPlane[i][3] = workerData.flClipPlane[i * 4 + 3];
        }
        GfxShaderSetConstant( &ptFastBindings[VSC_VEC_CLIP_PLANE], tPlane, MAX_CLIP_PLANES );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_VEC_REFLECTION_PLANE ) )
    {
        ShaderFloat tPlane[MAX_REFLECTION_PLANES];
        for ( int i = 0; i < MAX_REFLECTION_PLANES; i++ )
        {
            tPlane[i][0] = gptGfxData->flReflectionPlane[i * 4 + 0];
            tPlane[i][1] = gptGfxData->flReflectionPlane[i * 4 + 1];
            tPlane[i][2] = gptGfxData->flReflectionPlane[i * 4 + 2];
            tPlane[i][3] = gptGfxData->flReflectionPlane[i * 4 + 3];
        }
        GfxShaderSetConstant( &ptFastBindings[VSC_VEC_REFLECTION_PLANE], tPlane, MAX_REFLECTION_PLANES );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_MATRIX_SSAO_VIEW_PROJ ) )
    {
        Matrix4x4 mViewProjTranspose = tCGame.ptPostFXGlobals ? PostFXGetSSAOViewProjTranspose() : cgm4x4Identity;
        GfxShaderSetConstant( &ptFastBindings[VSC_MATRIX_SSAO_VIEW_PROJ], &mViewProjTranspose, 4 );
    }
}



























void GfxShaderSetCullMode( const CullMode eCullMode )
{
    gptGfxData->workerData[GfxGetWorkerIndex()].tRenderState.eCullMode = eCullMode;
}









void GfxShaderTexMatrixSet( const TextureBinding * ptTexBinding, const Matrix4x4 & m )
{









    if ( ptTexBinding->nStage < MAX_TEXTURE_MATRICES )
    {
        RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];

        workerData.tRenderState.mTex[ptTexBinding->nStage] = m.Transpose();
        workerData.tRenderState.fTexMatrixModified[ptTexBinding->nStage] = TRUE;
    }
}





void GfxShaderTexMatrixSetIdentity( const TextureBinding * ptTexBinding )
{








    if ( ptTexBinding->nStage < MAX_TEXTURE_MATRICES )
    {
        RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];

        if ( workerData.tRenderState.fTexMatrixModified[ptTexBinding->nStage] )
        {
            workerData.tRenderState.mTex[ptTexBinding->nStage] = cgm4x4Identity;
            workerData.tRenderState.fTexMatrixModified[ptTexBinding->nStage] = FALSE;
        }
    }
}





const ShaderConstBinding * GfxShaderGetConstantBinding( const int32 nIndex )
{
    const ShaderConstFastBinding * ptBinding = &gptGfxData->workerData[GfxGetWorkerIndex()].ptShaderConstantFastBindings[nIndex];
    if ( !ptBinding->dwFlags )
        return NULL;
    return (const ShaderConstBinding *)ptBinding;
}























const ShaderConstFastBinding * GfxShaderGetConstantFastBindings()
{
    return gptGfxData->workerData[GfxGetWorkerIndex()].ptShaderConstantFastBindings;
}










void GfxShaderSetConstant( const ShaderConstFastBinding * ptBinding, const void * lpData, const uint32 dwNumFloat4 )
{


    if ( !ptBinding )
    {
        return;
    }






    if ( ptBinding->dwFlags & ( 1 << SHADERCONSTFASTBINDING_VERTEX_SHADER_BIT ) )
    {
        gptGFXAPI->SetVertexShaderConstant( &ptBinding->tVSBinding, lpData, dwNumFloat4 );
    }
    else if ( ptBinding->dwFlags & ( 1 << SHADERCONSTFASTBINDING_PIXEL_SHADER_BIT ) )
    {
        gptGFXAPI->SetPixelShaderConstant( &ptBinding->tPSBinding, lpData, dwNumFloat4 );
    }
}







void GfxShaderSetConstant( const ShaderConstBinding * ptBinding_, const void * lpData, const uint32 dwNumFloat4 )
{


    const ShaderConstFastBinding * ptBinding = (const ShaderConstFastBinding *)ptBinding_;
    if ( !ptBinding )
    {
        return;
    }

    GfxShaderSetConstant( ptBinding, lpData, dwNumFloat4 );
}


























static void sSetTextureStageParams( const uint32 dwStage, const TextureStageParams * const ptsParams, BOOL bShadowStage, uint32 dwShadowTech, BOOL fDisableTrilinearFiltering, float flTexMipBias )
{








    gptGFXAPI->SetTextureAddressMode( dwStage, ptsParams->eTexAddressModeU, ptsParams->eTexAddressModeV, ptsParams->eTexAddressModeW );

    FilterMode eFilterMode = ptsParams->eFilterMode;

    if ( bShadowStage )
    {
        eFilterMode = FILTERMODE_BILINEAR;
    }











    if ( eFilterMode == FILTERMODE_PREF || eFilterMode == FILTERMODE_TRILINEAR )
    {
        eFilterMode = fDisableTrilinearFiltering ? FILTERMODE_BILINEAR : FILTERMODE_ANISOTROPIC;
    }











    gptGFXAPI->SetFilterMode( dwStage, eFilterMode );

    float flMipBias = ptsParams->flMipMapBias + flTexMipBias;

    if ( bShadowStage && dwShadowTech == 2 )
    {
        flMipBias = 512.0f;
    }


    gptGFXAPI->SetMipMapBias( dwStage, flMipBias );
}








static void sSetStateBlockStates( const RenderPass & tPass )

{


    gptGFXAPI->ZBufferWriteEnable( tPass.tRenderParams.fZBufferWriteEnable );
    gptGFXAPI->ZBufferSetCompareFunc( tPass.tRenderParams.eZBufferCompareFunc );
    gptGFXAPI->SetZBias( tPass.tRenderParams.flZBias );

    gptGFXAPI->SetCullMode( tPass.tRenderParams.eCullMode );




    gptGFXAPI->AlphaTestEnable( tPass.tRenderParams.fAlphaTestEnable );
    if ( tPass.tRenderParams.fAlphaTestEnable )
    {
        gptGFXAPI->AlphaTestSetParams( tPass.tRenderParams.eAlphaTestCompareFunc, tPass.tRenderParams.bAlphaTestRefValue );
    }
    else
    {
        gptGFXAPI->AlphaTestSetParams( COMPARE_UNKNOWN, 0 );
    }

    gptGFXAPI->SetWriteMasks( tPass.tRenderParams.fColorWriteEnable, tPass.tRenderParams.fAlphaWriteEnable );
    gptGFXAPI->SetBlendFactors( tPass.tRenderParams.fAlphaBlendEnable, tPass.tRenderParams.eSrcBlendFactor, tPass.tRenderParams.eDestBlendFactor, tPass.tRenderParams.eBlendOp );


    gptGFXAPI->StencilBufferEnable( tPass.tRenderParams.fStencilBufferEnable );
    if ( tPass.tRenderParams.fStencilBufferEnable )
    {
        gptGFXAPI->StencilBufferSetParams( tPass.tRenderParams.eStencilBufferCompareFunc,
                                           tPass.tRenderParams.nStencilBufferRefValue,
                                           tPass.tRenderParams.eStencilOpPass,
                                           tPass.tRenderParams.eStencilOpFail,
                                           tPass.tRenderParams.eStencilOpZFail );
    }



    gptGFXAPI->SetFillMode( tPass.tRenderParams.eFillMode );
    gptGFXAPI->AlphaMaskEnable( tPass.tRenderParams.fAlphaMaskEnable );


    uint32 dwShadowTech = GfxGetShadowTech();
    BOOL fDisableTrilinearFiltering = Prefs::GetVideoPrefs().GetDisableTrilinearFiltering();
    float flTexMipBias = Prefs::GetVideoPrefs().GetTextureMipBias();

    for ( uint32 dwStage = 0; dwStage < tPass.dwStageCount && dwStage < 16; dwStage++ )
    {
        ShaderTex eShaderTex = sGetShaderTex( tPass, dwStage );

        if ( eShaderTex != SHADERTEX_NONE )
        {
            sSetTextureStageParams( dwStage, &tPass.ptsParams[dwStage], eShaderTex == SHADERTEX_SHADOW, dwShadowTech, fDisableTrilinearFiltering, flTexMipBias );
        }
    }
}




void GfxShaderStateBlockCreate( ShaderDefinition * pdefShader, const int32 nPassIndex, XPtr64<unsigned char> & ptStateBlock )
{





    const RenderPass & tPass = pdefShader->ptPasses[nPassIndex];

    gptGFXAPI->StateBlockBegin();
    sSetStateBlockStates( tPass );
    gptGFXAPI->StateBlockEnd( ptStateBlock );
}















void GfxShaderStateBlockDispose( XPtr64<unsigned char> & ptStateBlock )
{
    if ( gptGFXAPI )
    {
        gptGFXAPI->StateBlockDispose( ptStateBlock );
    }
}
















void GfxCreateVertexShader( ShaderDefinition * pdefShader, const int32 nPassIndex )


{

    RenderPass & tPass = pdefShader->ptPasses[nPassIndex];




    if ( sSearchShaderCache( gptGfxData->tVSCache, tPass.dwVSKey, tPass.tPostprocessed.ptVSD ) )
    {
        return;
    }





































    ShaderData * newVSData = (ShaderData *)SigmaMemoryNew( sizeof( ShaderData ), 4, NULL, TRUE );

    newVSData->dwRefCount++;

    newVSData->dwKey = tPass.dwVSKey;











    newVSData->dwNumDirLights = Math::Min( TagMapGetInt( tPass.hTagMap, TAG_VS_NUM_DIRECTIONAL_LIGHTS ), 16 );
    newVSData->dwNumPointLights = Math::Min( TagMapGetInt( tPass.hTagMap, TAG_VS_NUM_POINT_LIGHTS ), 16 );
    newVSData->dwNumSpotLights = Math::Min( TagMapGetInt( tPass.hTagMap, TAG_VS_NUM_SPOT_LIGHTS ), 16 );
    newVSData->dwNumCylindricalLights = Math::Min( TagMapGetInt( tPass.hTagMap, TAG_VS_NUM_CYLINDRICAL_LIGHTS ), 16 );
    newVSData->dwNumPointLinearLights = Math::Min( TagMapGetInt( tPass.hTagMap, TAG_VS_NUM_POINT_LINEAR_LIGHTS ), 16 );

    gptGfxData->tVSCache.AddTail_Tracked( newVSData );
    tPass.tPostprocessed.ptVSD = newVSData;

    gptGFXAPI->CreateVertexShader( pdefShader, nPassIndex );

















}





void sReallyDeleteVertexShader( void * ptVSD, void * )
{
    gptGFXAPI->DeleteVertexShader( (ShaderData *)ptVSD );
    SigmaMemoryFree( ptVSD, NULL );
}




void GfxDeleteVertexShader( ShaderDefinition * pdefShader, const int32 nPassIndex )


{
    RenderPass & tPass = pdefShader->ptPasses[nPassIndex];
    ShaderData * cachedVS = tPass.tPostprocessed.ptVSD;

    if ( !cachedVS )
    {
        return;
    }

    cachedVS->dwRefCount--;
    if ( cachedVS->dwRefCount == 0 )
    {
        BOOL fDeleteCachedVS = TRUE;

        if ( gptGFXAPI )
        {





            if ( gptGFXAPI->VertexShaderIsBusy( cachedVS ) )
            {
                CGameAsyncRenderRegisterGPUResourceDelayedDeletion( sReallyDeleteVertexShader, cachedVS, NULL );
                fDeleteCachedVS = FALSE;
            }
            else
            {

                gptGFXAPI->DeleteVertexShader( cachedVS );
            }
        }



        if ( gptGfxData )
        {
            POS pos = gptGfxData->tVSCache.Find( cachedVS );

            gptGfxData->tVSCache.RemoveAt( pos );
        }

        if ( fDeleteCachedVS )
        {
            SigmaMemoryFree( cachedVS, NULL );
        }
    }
}




void GfxCreatePixelShader( ShaderDefinition * pdefShader, const int32 nPassIndex )


{

    RenderPass & tPass = pdefShader->ptPasses[nPassIndex];




    if ( sSearchShaderCache( gptGfxData->tPSCache, tPass.dwPSKey, tPass.tPostprocessed.ptPSD ) )
    {
        return;
    }





































    ShaderData * newPSData = (ShaderData *)SigmaMemoryNew( sizeof( ShaderData ), 4, NULL, TRUE );

    newPSData->dwRefCount++;

    newPSData->dwKey = tPass.dwPSKey;










    gptGfxData->tPSCache.AddTail_Tracked( newPSData );
    tPass.tPostprocessed.ptPSD = newPSData;

    gptGFXAPI->CreatePixelShader( pdefShader, nPassIndex );

















}





void sReallyDeletePixelShader( void * ptPSD, void * )
{
    gptGFXAPI->DeletePixelShader( (ShaderData *)ptPSD );
    SigmaMemoryFree( ptPSD, NULL );
}





void GfxDeletePixelShader( ShaderDefinition * pdefShader, const int32 nPassIndex )


{
    RenderPass & tPass = pdefShader->ptPasses[nPassIndex];
    ShaderData * cachedPS = tPass.tPostprocessed.ptPSD;

    if ( !cachedPS )
    {
        return;
    }

    cachedPS->dwRefCount--;
    if ( cachedPS->dwRefCount == 0 )
    {
        BOOL fDeleteCachedPS = TRUE;

        if ( gptGFXAPI )
        {





            if ( gptGFXAPI->PixelShaderIsBusy( cachedPS ) )
            {
                CGameAsyncRenderRegisterGPUResourceDelayedDeletion( sReallyDeletePixelShader, cachedPS, NULL );
                fDeleteCachedPS = FALSE;
            }
            else
            {

                gptGFXAPI->DeletePixelShader( cachedPS );
            }
        }



        if ( gptGfxData )
        {
            POS pos = gptGfxData->tPSCache.Find( cachedPS );

            gptGfxData->tPSCache.RemoveAt( pos );
        }

        if ( fDeleteCachedPS )
        {
            SigmaMemoryFree( cachedPS, NULL );
        }
    }
}


































































static BOOL sSearchShaderCache( XGrowableList<ShaderData *, ShaderData *> & tCache, uint32 dwKey, XPtr64<ShaderData> & dst )
{


    POS pos = tCache.GetHeadPosition();
    while ( pos )
    {
        ShaderData * cachedData = tCache.GetNext( pos );

        if ( cachedData->dwKey == dwKey )
        {
            cachedData->dwRefCount++;
            dst = cachedData;
            return TRUE;
        }
    }
    return FALSE;
}
