// X/Client/Gfx/Gfx.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/r6-gfx/gfx/frag.cpp by build/try/r6-gfx/gen.py.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Gfx.h"
#include "GfxAPI.h"
#include "GfxCommon.h"
#include "GfxIB.h"
#include "GfxInternal.h"
#include "GfxVB.h"
#include "../CGame.h"
#include "../Definitions/TextureDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Sigma/Math/Matrix.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/GamePerf.h"
#include "../../Sigma/SigmaTiming.h"
#include "../../Sigma/ErrorManager/FileOutputStream.h"
void GfxShaderInitializeLastState();  // GfxShader.cpp:300
void GfxGetCurrentDimensions(uint32 & dwWidth, uint32 & dwHeight);
HardwareClass GfxGetHWClass();
void GfxScreenshot(LPCSTR szFolder, LPCSTR szFilename, LPCSTR szComment, int32 eFormat, uint32 nQuality);  // Gfx.cpp:1109 (defined below)

#include "../Definitions/PreferencesDefinition.h"
void GfxIdleLoop(Blizzard::Time::Millisecond & nLastTime, int32 nTargetFPS);  // Gfx.cpp:3930 (defined below)
BOOL ShellWindowChangeDisplayMode(const DisplayMode & tMode);  // X/Common/Shell/ShellNX64.cpp:151
BOOL GfxWindowOpen(const DisplayMode & tMode);  // Gfx.cpp:1365 (defined below)
inline BOOL GfxWindowClose();  // Gfx.cpp:1484 (defined below)
inline void GfxUpdateDisplayModes();  // Gfx.cpp:1200 (defined below)
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Common/Definitions/StringListDefinition.h"
#include "../../Common/GlobalSNO.h"
#include "../../../Contrib/Contrib/blz/include/blz/format.h"
void GfxInitializeLastState();  // Gfx.cpp:1656 (defined below)

#include "../../Sigma/Files/XFiles.h"
#include "../../Sigma/RefString.h"
#include "../ImageUtils/ImageFile.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
// Not declared in the adopted headers (XFiles.h / JPGExport.h / File_NX64.cpp in the original):
void FileReferenceAddDirectory( FileReference & tFileRef, LPCSTR szDirectory );  // XFilesCore.cpp:80
LPCSTR FileReferenceGetPath( const FileReference & tFileRef );  // XFilesCore.cpp:91
BOOL FileDelete( FileReference & tFileRef );  // XFilesCore.cpp:523
BOOL FileExists( const FileReference & tFileRef );  // XFilesCore.cpp:721
void JPGSetComment( LPCSTR szPath, LPCSTR szComment );  // JPGExport.cpp:134
bool IsROMPathNX64( const char * path );  // File_NX64.cpp:35
#include "../Definitions/ShaderCommon.h"
#include <stdlib.h>
void GfxVBInitialize();  // GfxVB.cpp:71
void GfxIBInitialize();  // GfxIB.cpp:70

#include "../Definitions/ShaderDefinition.h"
#include "../Definitions/AppearanceDefinition.h"
void GfxVBDispose();  // GfxVB.cpp
void GfxIBDispose();  // GfxIB.cpp












extern const DisplayCaps cgtDefaultDisplayCaps = { 0, 16, 0, 1 };







VBFormatInfo cgtVBFormatInfo[NUM_VBFORMATS];




const Rect2D cgrectViewportDefault(0.0f, 0.0f, 1.0f, 1.0f);
const Vector2D cgvecViewportScaleDefault(1.0f, 1.0f);
extern const Vector4D cgvecViewportScaleBiasDefault(1.0f, 0.0f, 1.0f, 0.0f);







GfxInternalData * gptGfxData;


















static void sVBFormatInfoInit()
{



    SigmaMemoryClear( cgtVBFormatInfo, sizeof( cgtVBFormatInfo ) );
    // Inference: literal sizes where the vertex struct is not in the adopted headers (billboard, sprite, bump basis, skin, D3DX atlas).
    cgtVBFormatInfo[VBFORMAT_NOTEX_L].dwSize = sizeof( VertexNoTexL );
    cgtVBFormatInfo[VBFORMAT_BILLBOARD].dwSize = 0x28;
    cgtVBFormatInfo[VBFORMAT_1TEX_L].dwSize = sizeof( Vertex1TexL );
    cgtVBFormatInfo[VBFORMAT_SPRITE].dwSize = 0x2c;
    cgtVBFormatInfo[VBFORMAT_2TEX_L].dwSize = sizeof( Vertex2TexL );
    cgtVBFormatInfo[VBFORMAT_BUMPBASIS].dwSize = 0x8;
    cgtVBFormatInfo[VBFORMAT_SKIN].dwSize = 0x28;
    cgtVBFormatInfo[VBFORMAT_CLOTH].dwSize = sizeof( VertexCloth );
    cgtVBFormatInfo[VBFORMAT_D3DXATLAS].dwSize = 0x24;

    cgtVBFormatInfo[VBFORMAT_2TEX_L_CPOS].dwSize = sizeof( Vertex2TexLCPos );
    cgtVBFormatInfo[VBFORMAT_SKIN_CPOS].dwSize = sizeof( VertexSkinCPos );

    cgtVBFormatInfo[VBFORMAT_BILLBOARD_PARTICLE].dwSize = sizeof( VertexBillboardParticle );
}






static void sInitializeFromPrefs()
{

    gptGfxData->tGfxOptions.flGammaValue = Prefs::GetVideoPrefs().GetGamma();
    if ( Prefs::GetVideoPrefs().GetUseLetterBox() ) gptGfxData->dwFlags |= ( 1 << GFXINTERNAL_LETTERBOX_BIT ); else gptGfxData->dwFlags &= ~( 1 << GFXINTERNAL_LETTERBOX_BIT );
}










































static int32 sModeSort(const void * lpArg0, const void * lpArg1)
{
    const DisplayMode * ptMode0 = (const DisplayMode *)lpArg0;
    const DisplayMode * ptMode1 = (const DisplayMode *)lpArg1;

    if (ptMode0->dwWidth > ptMode1->dwWidth)
        return 1;
    if (ptMode0->dwWidth < ptMode1->dwWidth)
        return -1;
    if (ptMode0->dwHeight > ptMode1->dwHeight)
        return 1;
    if (ptMode0->dwHeight < ptMode1->dwHeight)
        return -1;
    if (ptMode0->dwBitDepth > ptMode1->dwBitDepth)
        return 1;
    if (ptMode0->dwBitDepth < ptMode1->dwBitDepth)
        return -1;
    if (ptMode0->nRefreshRate > ptMode1->nRefreshRate)
        return 1;
    if (ptMode0->nRefreshRate < ptMode1->nRefreshRate)
        return -1;
    return 0;
}

// NON_MATCHING: 344 vs 348 bytes; loop induction/register allocation differs
























// NON_MATCHING: 348 both; loop form/register allocation: the original counts -i in a 64-bit register for the move size and indexes with umaddl; tMode/tMode1 on their DWARF lines (249/250) in the for block, == chain on lines 253-257; `--dwDisplayModeCount` in the 265 test gives the same code. Columns on 253/265 not explained.
uint32 sRejectInvalidModes(
    DisplayMode * ptDisplayModes,
    uint32 dwDisplayModeCount)
{
    qsort(ptDisplayModes, dwDisplayModeCount, sizeof(DisplayMode), sModeSort);

    for (uint32 i = 0; i < dwDisplayModeCount; i++)
    {
        DisplayMode & tMode = ptDisplayModes[i];
        DisplayMode & tMode1 = ptDisplayModes[i + 1];


        while (dwDisplayModeCount > i + 1 &&
            tMode.dwWidth == tMode1.dwWidth &&
            tMode.dwHeight == tMode1.dwHeight &&
            tMode.dwBitDepth == tMode1.dwBitDepth &&
            tMode.nRefreshRate == tMode1.nRefreshRate)
        {






            if (i >= dwDisplayModeCount - 1)
                return dwDisplayModeCount - 1;
            SigmaMemoryMove(&tMode, &tMode1, (dwDisplayModeCount - i - 1) * sizeof(DisplayMode));
            dwDisplayModeCount--;
        }
    }
    return dwDisplayModeCount;
}

















































































































































































































































































































































































































static void sSetAppDrawFlags()
{
    switch (GfxGetHWClass())
    {
    case HARDWARECLASS_SLOW:
    case HARDWARECLASS_LOW:
        AppDrawFlagSet(APP_DRAW_HWCLASS_UI_OUTLINES_BIT, FALSE);
        AppDrawFlagSet(APP_DRAW_HWCLASS_DISTORTIONS_BIT, FALSE);
        break;
    case HARDWARECLASS_MEDIUM:
    case HARDWARECLASS_HIGH:
    case HARDWARECLASS_ULTRA:
    case HARDWARECLASS_EXTREME:
        AppDrawFlagSet(APP_DRAW_HWCLASS_UI_OUTLINES_BIT, TRUE);
        AppDrawFlagSet(APP_DRAW_HWCLASS_DISTORTIONS_BIT, TRUE);
        break;
    default:
        break;
    }
}






// NON_MATCHING: 1460 B both, 8 of 365 insns differ: at 710/736 the constant 7 and gptGfxData swap registers (x8/x9) and the cmp is scheduled one slot later. Tried a chained assignment on 710, the negated test, and storing eNewGraphicsSystem first.
BOOL GfxInit()

{
    BOOL fSuccess;



    gptGfxData = (GfxInternalData *)SigmaMemoryNew( sizeof( GfxInternalData ), 4, NULL, TRUE );

    gptGfxData->eGraphicsSystem = CmdLineGetParams()->tBitArray.Get( CMDLINE_NULL_RENDER_BIT ) ? GFX_NULL : GFX_NX64NVN;

























    gptGfxData->eNewGraphicsSystem = gptGfxData->eGraphicsSystem;
    GFXAPIInitialize( gptGfxData->eGraphicsSystem );
    atexit( GFXAPIDispose );

    if ( !gptGFXAPI->Detect() )
    {
        GFXAPIDispose();
        SigmaMemoryFree( gptGfxData, NULL ); gptGfxData = NULL;
        return FALSE;
    }


    sInitializeFromPrefs();

    gptGfxData->tGfxOptions.fUseReferenceRasterizer = FALSE;
    gptGfxData->snoTexCursor = SNO_NONE;

    sVBFormatInfoInit();
    fSuccess = gptGFXAPI->Init( &gptGfxData->tGfxOptions, cgtVBFormatInfo );
    if ( !fSuccess )
    {
        GFXAPIDispose();
        SigmaMemoryFree( gptGfxData, NULL ); gptGfxData = NULL;
        return fSuccess;
    }

    gptGfxData->tCaps = cgtDefaultDisplayCaps;

    GfxUpdateDisplayModes();
    gptGFXAPI->GetValidTextureFormats( gptGfxData->fTextureFormatValid[0] );



    if ( CmdLineGetParams()->tBitArray.Test( CMDLINE_HW_HIGH_BIT ) || CmdLineGetParams()->tBitArray.Test( CMDLINE_BLIZZCON_BIT ) )
    {
        sgtAppGlobals.eHWClassOverride = HARDWARECLASS_HIGH;
    }
    else if ( CmdLineGetParams()->tBitArray.Test( CMDLINE_HW_MEDIUM_BIT ) )
    {
        sgtAppGlobals.eHWClassOverride = HARDWARECLASS_MEDIUM;
    }
    else if ( CmdLineGetParams()->tBitArray.Test( CMDLINE_HW_LOW_BIT ) )
    {
        sgtAppGlobals.eHWClassOverride = HARDWARECLASS_LOW;
    }
    else
    {
        sgtAppGlobals.eHWClassOverride = Prefs::GetVideoPrefs().GetHWClass();
    }
    sSetAppDrawFlags();

    GfxVBInitialize();
    GfxIBInitialize();

    for ( int32 worker = 0; worker < 1; worker++ )
    {
        RenderWorkerData & workerData = gptGfxData->workerData[worker];

        workerData.ptTexBinding = (TextureBinding *)SigmaMemoryNew( sizeof( TextureBinding ) * 62, 4, NULL, FALSE );
    }










    for ( int32 worker = 0; worker < 1; worker++ )
    {
        RenderWorkerData & workerData = gptGfxData->workerData[worker];

        for ( int32 i = 0; i < MAX_RENDER_TARGETS; i++ )
        {
            workerData.tCurrentRTs[i].snoRTTex = SNO_NONE;
            workerData.tCurrentRTs[i].eCubeMapFace = CUBEMAPFACE_POSX;
        }
        workerData.snoCurrentDepthTarget = SNO_NONE;


        workerData.flRTInvHeight = workerData.flRTInvWidth = 0.0f;
        workerData.flRTHeight = workerData.flRTWidth = 0.0f;
        workerData.dwRTHeight = workerData.dwRTWidth = 0;


        workerData.tRenderState.vecViewportScaleBias = cgvecViewportScaleBiasDefault;
        workerData.tRenderState.vecFixedPointPosScale = cgwvOne;
        workerData.tRenderState.vecFixedPointPosOffset = cgwvNull;
    }
    gptGfxData->flModeHeight = 0.0f;
    gptGfxData->flModeWidth = 0.0f;


    gptGfxData->snoSwapChainRenderTarget = SNO_NONE;

    gptGfxData->tVSCache.Initialize( NULL );
    gptGfxData->tPSCache.Initialize( NULL );

    for ( int32 i = 0; i < 2; i++ )
    {
        GfxRenderData & tData = gptGfxData->tRenderData[i];

        new ( &tData.tLineList2DVertices ) VertexArray;
        new ( &tData.tLineList3DVertices ) VertexArray;
        new ( &tData.tPointList3DVertices ) VertexArray;
        new ( &tData.tTriOverlayVertices ) TriOverlayVertexArray;
        tData.tLine2DShaderBatches.Initialize( NULL );
        tData.tLine3DShaderBatches.Initialize( NULL );
        tData.tTriOverlayShaderBatches.Initialize( NULL );
    }





    SigmaMemoryDumpOSStats();


    return fSuccess;
}



void GfxDispose()

{
    if ( !gptGfxData )
        return;







    gptGroupDefShader->DeleteAll();
    gptGroupDefTexture->DeleteAll();
    gptGroupDefAppearance->DeleteAll();

    gptGfxData->tPSCache.Dispose();
    gptGfxData->tVSCache.Dispose();

    for ( int32 i = 0; i < 2; i++ )
    {
        GfxRenderData & tData = gptGfxData->tRenderData[i];

        tData.tLineList2DVertices.~VertexArray();
        tData.tLineList3DVertices.~VertexArray();
        tData.tPointList3DVertices.~VertexArray();
        tData.tTriOverlayVertices.~TriOverlayVertexArray();
        tData.tLine2DShaderBatches.Dispose();
        tData.tLine3DShaderBatches.Dispose();
        tData.tTriOverlayShaderBatches.Dispose();
    }

    for ( int32 worker = 0; worker < 1; worker++ )
    {
        RenderWorkerData & workerData = gptGfxData->workerData[worker];

        SigmaMemoryFree( workerData.ptTexBinding, NULL ); workerData.ptTexBinding = NULL;
    }
    SigmaMemoryFree( gptGfxData->ptModes, NULL ); gptGfxData->ptModes = NULL;

    GfxIBDispose();
    GfxVBDispose();

    if ( !gptGFXAPI->Dispose() )
        return;



    delete gptGFXAPI; gptGFXAPI = NULL;
    SigmaMemoryFree( gptGfxData, NULL ); gptGfxData = NULL;


}

















void GfxForceDeviceReset(const BOOL fHardReset)
{
    if (gptGfxData == NULL)
        return;

    gptGfxData->dwFlags |= (1 << GFXINTERNAL_FORCED_RESET_BIT);
    if (fHardReset)
        gptGfxData->dwFlags |= (1 << GFXINTERNAL_HARD_RESET_BIT);
    else
        gptGfxData->dwFlags &= ~(1 << GFXINTERNAL_HARD_RESET_BIT);

    sSetAppDrawFlags();
}

















void GfxNewFrame()
{

    if ( gptGfxData->dwFlags & ( 1 << GFXINTERNAL_TEXMEM_DUMP_BIT ) )
    {

        gptGfxData->ptLogStream->Log( FALSE, "------- Tex Mem Dump End ----------\n", "TexMem " );
        gptGfxData->dwFlags &= ~( 1 << GFXINTERNAL_TEXMEM_DUMP_BIT );
    }

    delete gptGfxData->ptLogStream; gptGfxData->ptLogStream = NULL;

    AppGlobals & tAppGlobals = sgtAppGlobals;
    if ( tAppGlobals.fFrameOut )

        GfxScreenshot( tAppGlobals.szFrameOutPath, NULL, NULL, 2, 75 );

}















GfxAPI GfxGetSystemID()
{
    return gptGfxData->eGraphicsSystem;
}




void GfxDeviceGetState(BOOL & fCanRender, BOOL & fMustBeReset)
{
    if (gptGfxData->dwFlags & (1 << GFXINTERNAL_RENDERING_DISABLED_BIT))
    {
        fCanRender = FALSE;
        fMustBeReset = FALSE;
        return;
    }

    BOOL fCanBeReset;
    gptGFXAPI->DeviceGetState(fCanRender, fMustBeReset, fCanBeReset);
    if ((gptGfxData->dwFlags & (1 << GFXINTERNAL_FORCED_RESET_BIT)) && (gptGfxData->dwFlags & (1 << GFXINTERNAL_HARD_RESET_BIT)))
        fCanBeReset = TRUE;
    if (fCanBeReset && (gptGfxData->dwFlags & (1 << GFXINTERNAL_FORCED_RESET_BIT)))
        fMustBeReset = TRUE;
}












void GfxBlockUntilIdle()
{
    gptGFXAPI->BlockUntilIdle();
}




BOOL GfxDeviceIsSoftReset()
{
    BOOL fCanRender;
    BOOL fMustBeReset;
    GfxDeviceGetState(fCanRender, fMustBeReset);
    if (fMustBeReset && !(gptGfxData->dwFlags & (1 << GFXINTERNAL_HARD_RESET_BIT)))
        return TRUE;
    return FALSE;
}
















static void sShutDown(
    DisplayMode & tSavedMode )
{

    if ( gptGfxData )
    {



        SigmaMemoryCopy( &tSavedMode, &gptGfxData->tCurrentMode, sizeof( DisplayMode ) );


        SigmaMemoryFree( gptGfxData->ptModes, NULL ); gptGfxData->ptModes = NULL;



        GfxWindowClose();
    }
}







// NON_MATCHING: 420 B vs 424 B. Everything matches except the tail: the original branches on GfxWindowOpen's result (cbnz to the shared return-TRUE block, otherwise returns w0 as is); ours folds it to cmp/cset. The DWARF locals nRetries (1145) and fRetry (1146) show a retry construct whose uses are not recoverable; tried: if/return, negated if/return, a for loop and a do-while retry loop built from those two locals (no change).
BOOL GfxDeviceReset()
{

    BOOL fHardReset = gptGfxData->dwFlags & ( 1 << GFXINTERNAL_HARD_RESET_BIT );

    if ( !fHardReset && gptGfxData->eGraphicsSystem == gptGfxData->eNewGraphicsSystem )
    {
        BOOL fSuccess = gptGFXAPI->DeviceReset();

        if ( !fSuccess )
            return FALSE;





        gptGfxData->dwFlags &= ~( 1 << GFXINTERNAL_FORCED_RESET_BIT );
        gptGfxData->dwFlags &= ~( 1 << GFXINTERNAL_HARD_RESET_BIT );
        return TRUE;

    }
    else {
        gptGfxData->dwFlags &= ~( 1 << GFXINTERNAL_FORCED_RESET_BIT );
        gptGfxData->dwFlags &= ~( 1 << GFXINTERNAL_HARD_RESET_BIT );


        DisplayMode tSavedMode;
        sShutDown( tSavedMode );


        if ( !GFXAPIInitialize( gptGfxData->eNewGraphicsSystem ) )
            return FALSE;



        gptGfxData->eGraphicsSystem = gptGfxData->eNewGraphicsSystem;


        if ( !gptGFXAPI->Detect() )
            return FALSE;





        if ( !gptGFXAPI->Init( &gptGfxData->tGfxOptions, cgtVBFormatInfo ) )
            return FALSE;




        GfxUpdateDisplayModes();



        uint32 nRetries = 0;  // Inference: DWARF local (nested block) with no visible use
        BOOL fRetry;  // Inference: DWARF local with no visible use

        if ( GfxWindowOpen( tSavedMode ) )
            return TRUE;




















        return FALSE;
    }
}


















uint32 GfxGetShadowTech()
{
    return gptGFXAPI->GetShadowTech();
}





inline void GfxUpdateDisplayModes()
{


    SigmaMemoryFree( gptGfxData->ptModes, NULL ); gptGfxData->ptModes = NULL;
    gptGFXAPI->GetValidDisplayModes( gptGfxData->ptModes, gptGfxData->dwModeCount );
    gptGfxData->dwModeCount = sRejectInvalidModes( gptGfxData->ptModes, gptGfxData->dwModeCount );
}




inline BOOL GfxDisplayModeIsValid(const DisplayMode & tMode)
{
    if (tMode.dwWindowMode != 0)
        return TRUE;
    if (gptGfxData->eGraphicsSystem == GFX_NULL)
        return FALSE;
    for (uint32 i = 0; i < gptGfxData->dwModeCount; i++)
    {
        if (gptGfxData->ptModes[i].dwWidth == tMode.dwWidth && gptGfxData->ptModes[i].dwHeight == tMode.dwHeight && gptGfxData->ptModes[i].dwBitDepth == tMode.dwBitDepth && gptGfxData->ptModes[i].nRefreshRate == tMode.nRefreshRate && gptGfxData->ptModes[i].dwWindowMode == 0)
            return TRUE;
    }
    return FALSE;
}

















void GfxValidateDisplayMode(const DisplayMode & tMode, DisplayMode & tModeValidated)
{

    DisplayMode tModeToUse = tMode;
    if (!GfxDisplayModeIsValid(tModeToUse) || gptGfxData->eGraphicsSystem == GFX_NULL)
    {
        tModeToUse = cgtDisplayModeDefault;
    }


    tModeValidated = tModeToUse;
}

























































void GfxDynamicStuffInit()
{
    BOOL fSuccess;
    fSuccess = GfxVBAllocTracked(RESOURCEUSAGE_DYNAMIC, VBFORMAT_NOTEX_L, 0x8000, TRUE, NULL, cgtVBFormatInfo[VBFORMAT_NOTEX_L].vbidPooled, "", 0, "");
    fSuccess = GfxVBAllocTracked(RESOURCEUSAGE_DYNAMIC, VBFORMAT_BILLBOARD, 0x1000, TRUE, NULL, cgtVBFormatInfo[VBFORMAT_BILLBOARD].vbidPooled, "", 0, "");
    fSuccess = GfxVBAllocTracked(RESOURCEUSAGE_DYNAMIC, VBFORMAT_1TEX_L, 0x8000, TRUE, NULL, cgtVBFormatInfo[VBFORMAT_1TEX_L].vbidPooled, "", 0, "");
    fSuccess = GfxVBAllocTracked(RESOURCEUSAGE_DYNAMIC, VBFORMAT_2TEX_L, 0x8000, TRUE, NULL, cgtVBFormatInfo[VBFORMAT_2TEX_L].vbidPooled, "", 0, "");
    fSuccess = GfxVBAllocTracked(RESOURCEUSAGE_DYNAMIC, VBFORMAT_CLOTH, 0x8000, TRUE, NULL, cgtVBFormatInfo[VBFORMAT_CLOTH].vbidPooled, "", 0, "");
}
























void GfxDynamicStuffDispose()
{
    for (int32 i = 0; i < NUM_VBFORMATS - 1; i++)
        GfxVBRelease(cgtVBFormatInfo[i].vbidPooled);
}















// NON_MATCHING: 25 of 231 insns differ (size equal). Only the per-worker render-target dimension block (1463-1468) differs: the original keeps the six stores ahead of the two compare-and-divide sequences and loads the width into w10; ours hoists the divides above the stores. Tried ternary on the stored fields, on dwWidth/dwHeight with (float) casts, and on flRTWidth != 0.0f. GfxWindowChangeDisplayMode (1699) has the same block and the same mismatch.
BOOL GfxWindowOpen(
    const DisplayMode & tMode )
{


    if ( gptGfxData->dwFlags & ( 1 << GFXINTERNAL_WINDOW_OPEN_BIT ) )
        return FALSE;



    GfxValidateDisplayMode( tMode, gptGfxData->tCurrentMode );

    if ( !gptGFXAPI->CreateSurface( gptGfxData->tCurrentMode ) )
        return FALSE;




    GfxDeviceDesc deviceDesc;
    char vram[256];
    gptGFXAPI->GetDeviceDescription( deviceDesc );
    snprintf( vram, sizeof( vram ), "%u MB", (uint32)( deviceDesc.videoMemSize >> 20 ) );

    ErrorManagerAddCrashMetadata( "Video_Card", deviceDesc.deviceDescription );
    ErrorManagerAddCrashMetadata( "Video_Driver", deviceDesc.driverVersion );
    ErrorManagerAddCrashMetadata( "Video_Memory", vram );

    SNO snoStringList = GlobalSNOGet( GLOBALSNO_STRINGLIST_GENERAL );
    CRefString sError;



    gptGfxData->dwFlags |= ( 1 << GFXINTERNAL_WINDOW_OPEN_BIT );


    gptGFXAPI->GetCaps( gptGfxData->tCaps );
    gptGFXAPI->GetValidTextureFormats( &gptGfxData->fTextureFormatValid[0][0] );
    if ( !gptGfxData->fTextureFormatValid[RESOURCEUSAGE_IMMUTABLE][TEXTUREFORMAT_ARGB8888] )
    {
        sError = StringListFetch( snoStringList, "GfxWindowOpenErr_Prefix" );
        sError += StringListFetch( snoStringList, "GfxWindowOpenErr_ARGB8888" );
    }


    else if ( !gptGfxData->fTextureFormatValid[RESOURCEUSAGE_IMMUTABLE][TEXTUREFORMAT_DXT1] )
    {
        sError = StringListFetch( snoStringList, "GfxWindowOpenErr_Prefix" );
        sError += StringListFetch( snoStringList, "GfxWindowOpenErr_Features" );
    }
    else

    {   DisplayCaps * tCaps = &gptGfxData->tCaps;





        if ( !Prefs::GetVideoPrefs().GetHWClass() )
        {
            sError = StringListFetch( snoStringList, "GfxWindowOpenErr_Prefix" );
            sError += StringListFetch( snoStringList, "GfxWindowOpenErr_Features" );
        }


        else if ( tCaps->dwMaxVertexStreams <= 2 )
        {
            sError = StringListFetch( snoStringList, "GfxWindowOpenErr_Prefix" );
            sError += StringListFetch( snoStringList, "GfxWindowOpenErr_Streams" );
        }
        else
        {

            tCaps->dwMaxVertexStreams = 3;

            for ( int32 worker = 0; worker < 1; worker++ )
            {
                RenderWorkerData & workerData = gptGfxData->workerData[worker];

                for ( uint32 i = 0; i < 3; i++ )
                {
                    workerData.tRenderState.vbid[i] = 0; workerData.tRenderState.dwStreamOffset[i] = 0;

                    workerData.tRenderStateLast.vbid[i] = 0; workerData.tRenderStateLast.dwStreamOffset[i] = 0;
                }
            }


            GfxInitializeLastState();

            uint32 dwWidth, dwHeight;
            GfxGetCurrentDimensions( dwWidth, dwHeight );
            gptGfxData->flModeWidth = (float)dwWidth;
            gptGfxData->flModeHeight = (float)dwHeight;

            for ( int32 worker = 0; worker < 1; worker++ )
            {
                RenderWorkerData & workerData = gptGfxData->workerData[worker];

                workerData.dwRTWidth = dwWidth;
                workerData.dwRTHeight = dwHeight;
                workerData.flRTWidth = (float)dwWidth;
                workerData.flRTHeight = (float)dwHeight;
                workerData.flRTInvWidth = ( workerData.dwRTWidth != 0 ) ? 1.0f / workerData.flRTWidth : 0.0f;
                workerData.flRTInvHeight = ( workerData.dwRTHeight != 0 ) ? 1.0f / workerData.flRTHeight : 0.0f;
            }
            return TRUE;
        }
    }

    gptGFXAPI->CloseSurface();
    ErrorMessageBox( sError, StringListFetch( snoStringList, "GfxWindowOpenErr_Header" ).str() );
    ExitProgram( ERROR_NONE );


}




inline BOOL GfxWindowClose()
{



    if ( !( gptGfxData->dwFlags & ( 1 << GFXINTERNAL_WINDOW_OPEN_BIT ) ) )
        return TRUE;
    if ( !gptGFXAPI->CloseSurface() )
        return FALSE;



    gptGfxData->dwFlags &= ~( 1 << GFXINTERNAL_WINDOW_OPEN_BIT );
    return TRUE;
}






void GfxAcquireThreadOwnership()
{
    gptGFXAPI->AcquireThreadOwnership();
}





void GfxReleaseThreadOwnership()
{
    gptGFXAPI->ReleaseThreadOwnership();
}














void GfxBeginWorker(int32 nWorkerIndex)
{
    gptGFXAPI->BeginWorker(nWorkerIndex);
}





void GfxEndWorker()
{
    gptGFXAPI->EndWorker();
}





int32 GfxGetWorkerIndex()
{
    return 0;
}






































































































void GfxInitializeLastState()
{
    GfxShaderInitializeLastState();
    gptGfxData->snoSwapChainRenderTarget = SNO_NONE;
}
















const DisplayMode & GfxGetCurrentDisplayMode()
{
    gptGFXAPI->GetCurrentDisplayMode(gptGfxData->tCurrentMode);
    return gptGfxData->tCurrentMode;
}

// NON_MATCHING: 192 B both, 25/48 insns differ: the original stores the four RT dimension fields before each compare-and-divide (1709, 1710); ours computes both divides first. Tried if/else stores instead of the ternaries and the DWARF line layout (same as GfxWindowOpen 1463-1468).






void GfxWindowChangeDisplayMode(
    const DisplayMode & tMode )
{
    if (gptGFXAPI->ChangeResolution(tMode))
    {
        gptGfxData->tCurrentMode = tMode;
        uint32 dwWidth, dwHeight;
        GfxGetCurrentDimensions(dwWidth, dwHeight);
        gptGfxData->flModeWidth = (float)dwWidth;
        gptGfxData->flModeHeight = (float)dwHeight;

        for (int32 worker = 0; worker < 1; worker++)
        {
            RenderWorkerData & workerData = gptGfxData->workerData[worker];

            workerData.dwRTWidth = dwWidth;
            workerData.dwRTHeight = dwHeight;
            workerData.flRTWidth = (float)dwWidth;
            workerData.flRTHeight = (float)dwHeight;
            workerData.flRTInvWidth = (workerData.dwRTWidth != 0) ? 1.0f / workerData.flRTWidth : 0.0f;
            workerData.flRTInvHeight = (workerData.dwRTHeight != 0) ? 1.0f / workerData.flRTHeight : 0.0f;
        }
    }




}


















DisplayMode GfxGetDesiredDisplayMode()
{
    DisplayMode tDisplayMode = GfxGetCurrentDisplayMode();
    tDisplayMode.dwFlags |= 4;
    return tDisplayMode;
}




































void GfxSetPrefsChanged()
{
    gptGfxData->dwFlags |= (1 << GFXINTERNAL_PREFS_CHANGED_BIT);
}




BOOL GfxHasPrefsChanged()
{
    return (gptGfxData->dwFlags >> GFXINTERNAL_PREFS_CHANGED_BIT) & 1;
}




void GfxApplyChangedPrefs()
{

    if ( gptGfxData->dwFlags & ( 1 << GFXINTERNAL_PREFS_CHANGED_BIT ) )
    {

        gptGfxData->dwFlags &= ~( 1 << GFXINTERNAL_PREFS_CHANGED_BIT );

        sInitializeFromPrefs();

        if ( gptGfxData->tCaps.dwFlags & ( 1 << DISPLAY_SUPPORTS_GAMMA_CONTROL_BIT ) )

            gptGFXAPI->GammaSet( gptGfxData->tGfxOptions.flGammaValue );


        DisplayMode tMode = GfxGetDesiredDisplayMode();

        ShellWindowChangeDisplayMode( tMode );

    }
}







































void GfxGetCurrentDimensions(uint32 & dwWidth, uint32 & dwHeight)
{
    dwWidth = gptGfxData->tCurrentMode.dwWidth;
    dwHeight = gptGfxData->tCurrentMode.dwHeight;
}








void GfxGetCurrentFloatDimensions(float & flWidth, float & flHeight)
{
    flWidth = gptGfxData->flModeWidth;
    flHeight = gptGfxData->flModeHeight;
}








float GfxGetAspectRatio()
{
    return gptGfxData->tCurrentMode.flAspectRatio;
}
























uint32 GfxGetMSAALevel()
{
    if (!gptGFXAPI->TextureMSAASupported())
        return 1;
    return gptGfxData->tCurrentMode.dwMSAALevel;
}












BOOL GfxGetMSAAEnabled()
{
    return GfxGetMSAALevel() > 1;
}







BOOL GfxIsLetterboxed()
{
    if ((gptGfxData->dwFlags & (1 << GFXINTERNAL_LETTERBOX_BIT)) && Math::Abs(16.0f / 9.0f - gptGfxData->tCurrentMode.flAspectRatio) > 0.01f)
        return TRUE;
    return FALSE;
}











































































void GfxFixedPointPosSet(const Vector3D & vecFixedPointPosScale, const Vector3D & vecFixedPointPosOffset)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    workerData.tRenderState.vecFixedPointPosScale = vecFixedPointPosScale;
    workerData.tRenderState.vecFixedPointPosOffset = vecFixedPointPosOffset;
}







const Vector3D & GfxFixedPointPosScaleGet()
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    return workerData.tRenderState.vecFixedPointPosScale;
}






const Vector3D & GfxFixedPointPosOffsetGet()
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    return workerData.tRenderState.vecFixedPointPosOffset;
}






void GfxWorldMatrixSet(const Matrix4x4 & m)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    workerData.tRenderState.mWorld = m;
    workerData.tRenderState.mWorldTranspose = m.Transpose();
    workerData.tRenderState.mWorldView = m * GfxViewMatrixGet();
    workerData.tRenderState.mWorldViewTranspose = workerData.tRenderState.mWorldView.Transpose();
    workerData.tRenderState.mWorldViewProjectionTranspose = workerData.tRenderState.mProjectionTranspose * workerData.tRenderState.mWorldViewTranspose;
}








inline const Matrix4x4 & GfxWorldMatrixGet()
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    return workerData.tRenderState.mWorld;
}




























const Matrix4x4 & GfxWorldViewMatrixGet()
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    return workerData.tRenderState.mWorldView;
}

















void GfxBuildViewMatrix(
    Matrix4x4 & m,
    const Vector3D & wpFrom,
    const Vector3D & wvView,
    const Vector3D & wvUp,
    const BOOL fRightHanded )
{
    Vector3D wvViewToUse = wvView;
    if ( !fRightHanded )

        wvViewToUse = -wvViewToUse;

    wvViewToUse.Normalize();

    Vector3D wvRight = Vector3D::CrossProduct( wvViewToUse, wvUp );
    wvRight.Normalize();


    Vector3D wvUpToUse = Vector3D::CrossProduct( wvRight, wvViewToUse );



    m._11 = wvRight.x; m._12 = wvUpToUse.x; m._13 = -wvViewToUse.x; m._14 = 0.0f;
    m._21 = wvRight.y; m._22 = wvUpToUse.y; m._23 = -wvViewToUse.y; m._24 = 0.0f;
    m._31 = wvRight.z; m._32 = wvUpToUse.z; m._33 = -wvViewToUse.z; m._34 = 0.0f;


    m._41 = -Vector3D::DotProduct( wvRight, wpFrom );
    m._42 = -Vector3D::DotProduct( wvUpToUse, wpFrom );
    m._43 = Vector3D::DotProduct( wvViewToUse, wpFrom );
    m._44 = 1.0f;





}




void GfxViewMatrixSet(const Matrix4x4 & m)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    workerData.tRenderState.mView = m;
    workerData.tRenderState.mViewTranspose = m.Transpose();
    workerData.tRenderState.mWorldView = GfxWorldMatrixGet() * m;
    workerData.tRenderState.mWorldViewTranspose = workerData.tRenderState.mWorldView.Transpose();
    workerData.tRenderState.mWorldViewProjectionTranspose = workerData.tRenderState.mProjectionTranspose * workerData.tRenderState.mWorldViewTranspose;
}








const Matrix4x4 & GfxViewMatrixGet()
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    return workerData.tRenderState.mView;
}

















void GfxBuildPerspectiveProjectionMatrix(Matrix4x4 & m, const Angle aFOV, const float flAspectRatio, const float flNearPlane, const float flFarPlane, const BOOL fRightHanded)
{
    float flTanHalfFOV = Math::Tan(aFOV * 0.5f);
    float flCotanHalfFOV = 1.0f / flTanHalfFOV;
    float Q = flFarPlane / (flFarPlane - flNearPlane);

    m._11 = flCotanHalfFOV / flAspectRatio;
    m._12 = 0.0f;
    m._13 = 0.0f;
    m._14 = 0.0f;
    m._21 = 0.0f;
    m._22 = flCotanHalfFOV;
    m._23 = 0.0f;
    m._24 = 0.0f;
    m._31 = 0.0f;
    m._32 = 0.0f;
    m._33 = -Q;
    m._34 = -1.0f;
    m._41 = 0.0f;
    m._42 = 0.0f;
    m._43 = -Q * flNearPlane;
    m._44 = 0.0f;

    if (!fRightHanded)
    {
        m._33 = -m._33;
        m._34 = -m._34;
    }
}
















void GfxBuildOrthographicProjectionMatrix(Matrix4x4 & m, const float flHeight, const float flAspectRatio, const float flNearPlane, const float flFarPlane, const BOOL fRightHanded)
{
    float flWidth = flHeight * flAspectRatio;
    float Q = 1.0f / (flFarPlane - flNearPlane);

    m._11 = 2.0f / flWidth;
    m._12 = 0.0f;
    m._13 = 0.0f;
    m._14 = 0.0f;
    m._21 = 0.0f;
    m._22 = 2.0f / flHeight;
    m._23 = 0.0f;
    m._24 = 0.0f;
    m._31 = 0.0f;
    m._32 = 0.0f;
    m._33 = -Q;
    m._34 = 0.0f;
    m._41 = 0.0f;
    m._42 = 0.0f;
    m._43 = -Q * flNearPlane;
    m._44 = 1.0f;

    if (!fRightHanded)
    {
        m._33 = -m._33;
        m._34 = -m._34;
    }
}










void GfxBuild2DUIProjectionMatrix(Matrix4x4 & m)
{
    float flWidth, flHeight;
    GfxGetRenderTargetFloatDimensions(flWidth, flHeight);

    m = cgm4x4Identity;
    m._11 = 2.0f / flWidth;
    m._22 = -2.0f / flHeight;
    m._41 = -1.0f;
    m._42 = 1.0f;
}







void GfxViewportSet(const IRect2D & rectViewport)
{
    gptGFXAPI->ViewportSet(rectViewport);
}





void GfxViewportScaleBiasSet(const Vector4D & vScaleBias)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    workerData.tRenderState.vecViewportScaleBias = vScaleBias;
}






void GfxViewportScaleBiasGet(Vector4D & vScaleBias)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    vScaleBias = workerData.tRenderState.vecViewportScaleBias;
}
















const Matrix4x4 & GfxProjectionMatrixGet()
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    return workerData.tRenderState.mProjection;
}

















void GfxProjectionMatrixSet(const Matrix4x4 & m)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    workerData.tRenderState.mProjection = m;
    workerData.tRenderState.mProjectionTranspose = m.Transpose();
    workerData.tRenderState.mWorldViewProjectionTranspose = workerData.tRenderState.mProjectionTranspose * workerData.tRenderState.mWorldViewTranspose;
}












void GfxIrradMatrixSet(const Matrix4x4 & m)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    workerData.tRenderState.mIrrad = m;
    workerData.tRenderState.mIrradTranspose = m.Transpose();
}






























void GfxCameraBillboardSet(const Quaternion & q)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    workerData.tRenderState.qBillboard = q;
}






const Quaternion & GfxCameraBillboardGet()
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    return workerData.tRenderState.qBillboard;
}






void GfxFogSetParams(const RGBAColor & rgbaFogColor, const float flFogStart, const float flFogEnd, const float flHeightFogStart, const float flHeightFogEnd)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    workerData.tRenderState.rgbaFogColor = rgbaFogColor;
    workerData.tRenderState.rgbaFogColorValue.Set(rgbaFogColor);
    workerData.tRenderState.flFogStart = flFogStart;
    workerData.tRenderState.flFogEnd = flFogEnd;
    workerData.tRenderState.flHeightFogStart = flHeightFogStart;
    workerData.tRenderState.flHeightFogEnd = flHeightFogEnd;
}










void GfxFogForceDisabled(const BOOL fDisabled)
{
    AppDrawFlagSet(APP_DRAW_NO_FOG_BIT, fDisabled);
}






void GfxFogSetEnabled(const BOOL fEnabled)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    workerData.tRenderState.fFogEnabled = fEnabled;
}







BOOL GfxFogEnabled()
{
    if (sgtAppGlobals.eDebugRenderMode == 5 || sgtAppGlobals.eDebugRenderMode == 6)
        return FALSE;
    if (AppDrawFlagIsSet(APP_DRAW_NO_FOG_BIT))
        return FALSE;
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    return workerData.tRenderState.fFogEnabled;
}










void GfxFogGetParams(RGBAColor & rgbaFogColor, float & flFogStart, float & flFogEnd, float & flHeightFogStart, float & flHeightFogEnd)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    rgbaFogColor = workerData.tRenderState.rgbaFogColor;
    flFogStart = workerData.tRenderState.flFogStart;
    flFogEnd = workerData.tRenderState.flFogEnd;
    flHeightFogStart = workerData.tRenderState.flHeightFogStart;
    flHeightFogEnd = workerData.tRenderState.flHeightFogEnd;
}










void GfxDraw(const DrawMode eDrawMode, const uint32 dwBaseIndexOffset, const uint32 dwMinVertexIndex, const uint32 dwNumVertices, const uint32 dwStartIndex, const uint32 dwPrimCount, const PerfBin ePerfBin)
{
    if (gptGfxData->dwFlags & (1 << GFXINTERNAL_RENDERING_DISABLED_BIT))
        return;

    RenderWorkerData & workerData = gptGfxData->workerData[0];
    gptGFXAPI->Draw(eDrawMode, dwBaseIndexOffset, dwMinVertexIndex, dwNumVertices, dwStartIndex, dwPrimCount);
    workerData.tStats.tCurrent.tBinStats[ePerfBin].dwPolys += dwPrimCount;
    workerData.tStats.tCurrent.tBinStats[ePerfBin].nDrawCalls++;
}


















void GfxFinalizePerformanceStats()
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    gptGfxData->tGlobalStats.tLast = workerData.tStats.tCurrent;
    SigmaMemoryClear(&workerData.tStats.tCurrent, sizeof(workerData.tStats.tCurrent));
}
















































BOOL GfxRenderPresent()
{



    BOOL bResult;

    if ( gptGfxData->dwFlags & ( 1 << GFXINTERNAL_RENDERING_DISABLED_BIT ) )
        return TRUE;




    {
        FrameTimer::IdleScope renderIdle( sgtAppGlobals.renderTimer );
        bResult = gptGFXAPI->RenderPresent();
    }

    if ( !bResult ) gptGfxData->dwFlags |= ( 1 << GFXINTERNAL_PRESENT_FAILED_BIT ); else gptGfxData->dwFlags &= ~( 1 << GFXINTERNAL_PRESENT_FAILED_BIT );


    if ( !CmdLineGetParams()->tBitArray.Test( CMDLINE_NO_FPS_LIMIT_BIT ) &&
         Prefs::GetVideoPrefs().GetLimitForegroundFPS() )
    {
        static Blizzard::Time::Millisecond nLastTime;
        GfxIdleLoop( nLastTime, Prefs::GetVideoPrefs().GetForegroundFPSLimit() );
    }

    return bResult;
}




void GfxResetStates()
{
    gptGFXAPI->ResetStates();

    uint32 dwMaxStages = (gptGfxData->tCaps.dwFlags & (1 << DISPLAY_SUPPORTS_SM4_BIT)) ? 24 : 16;
    for (uint32 dwStage = 0; dwStage < dwMaxStages; dwStage++)
        gptGFXAPI->TextureSetCurrent(dwStage, NULL);

    for (int32 i = 0; i < 3; i++)
        GfxVBSetStream(i, 0, VBID_INVALID);
    GfxIBSetCurrent(IBID_INVALID);














    for (int32 worker = 0; worker < 1; worker++)
    {
        RenderWorkerData & workerData = gptGfxData->workerData[worker];

        for (uint32 dwStage = 0; dwStage < 24; dwStage++)
        {
            workerData.tRenderState.snoTex[dwStage] = SNO_NONE;
            workerData.tRenderStateLast.snoTex[dwStage] = SNO_NONE;
        }

        workerData.tRenderState.snoLightMapTex = SNO_NONE;



        for (int32 i = 0; i < 3; i++)
        {
            workerData.tRenderState.vbid[i] = VBID_INVALID;
            workerData.tRenderStateLast.vbid[i] = VBID_INVALID;
        }

        workerData.tRenderState.ibid = IBID_INVALID;
        workerData.tRenderStateLast.ibid = IBID_INVALID;
        workerData.tRenderState.nNumBones = 0;
        workerData.tRenderStateLast.nNumBones = 0;


        workerData.tRenderState.ptStateBlock = NULL;
        workerData.tRenderStateLast.ptStateBlock = NULL;


        for (int32 i = 0; i < 2; i++)
        {
            workerData.tCurrentRTs[i].snoRTTex = SNO_NONE;
        }

        workerData.snoCurrentDepthTarget = SNO_NONE;
    }
}





BOOL GfxRenderPresentLastStatus(BOOL bResetStatus)
{
    BOOL bResult = !(gptGfxData->dwFlags & (1 << GFXINTERNAL_PRESENT_FAILED_BIT));
    if (bResetStatus)
        gptGfxData->dwFlags &= ~(1 << GFXINTERNAL_PRESENT_FAILED_BIT);
    return bResult;
}







void GfxRenderSuspend()
{
    gptGFXAPI->RenderSuspend();
}





void GfxRenderResume()
{
    gptGFXAPI->RenderResume();
}





GfxPerfStats * GfxGetPerfStats()
{
    return &gptGfxData->tGlobalStats;
}





void GfxScreenshot(Pixel32 *& lpBuffer, uint32 & dwWidth, uint32 & dwHeight, uint32 & dwPitch)
{
















































    CGameAsyncRenderFlush();
    DisplayMode tCurrentMode;

    gptGFXAPI->GetCurrentDisplayMode(tCurrentMode);
    dwWidth = tCurrentMode.dwWidth;
    dwHeight = tCurrentMode.dwHeight;



    dwPitch = dwWidth * sizeof(Pixel32);


    CGameAsyncRenderFlush();


    lpBuffer = (Pixel32 *)gptGroupDefTexture->MemAlloc(dwHeight * dwPitch, 16);

    BOOL fBlitCursor = GfxCursorMakeResident();
    gptGFXAPI->ScreenGrab(lpBuffer, dwWidth, dwHeight, dwPitch, fBlitCursor);
}




void GfxScreenshot(
    LPCSTR pszFolderPath,
    LPCSTR pszFilename,
    LPCSTR pszComment,
    const ImageFileFormat eFileFormat,
    const uint32 dwQuality )
{
    if ( !( GfxGetCurrentDisplayMode().dwFlags & ( 1 << DISPLAYMODE_SCREENSHOT_ENABLE_BIT ) ) )
    {
        return;
    }



    if ( IsROMPathNX64( pszFolderPath ) )
    {
        return;
    }


    const BOOL fFolderIsValid = pszFolderPath && pszFolderPath[0] != '\0';
    FileReference tRefDir;
    tRefDir.Clear();
    if ( fFolderIsValid )
    {

        FileReferenceInit( tRefDir, pszFolderPath );
        FileDirectoryCreate( tRefDir, TRUE );
    }

    FileReference tRef;
    if ( pszFilename )
    {

        if ( fFolderIsValid )
        {
            tRef = tRefDir;
            FileReferenceAddDirectory( tRef, pszFilename );
        }
        else
        {
            FileReferenceInit( tRef, pszFilename );
        }
        if ( FileExists( tRef ) )
        {
            FileDelete( tRef );
        }
    }
    else
    {

        LPCSTR szExt = ImageFileExtFromFileFormat( eFileFormat );
        static int32 nNum = 0;
        BOOL fFileExists;
        do {  // Inference: loop form; the unswitched fFolderIsValid test carries line 2879
            CRefString sFilename = CRefString().Format( "Screenshot%03d.%s", nNum, szExt );
            if ( fFolderIsValid )
            {
                tRef = tRefDir;
                FileReferenceAddDirectory( tRef, sFilename );
            }
            else
            {
                FileReferenceInit( tRef, sFilename );
            }
            fFileExists = FileExists( tRef );
            nNum++;
        } while ( fFileExists );
    }

    Pixel32 * lpBuffer;
    uint32 dwWidth;
    uint32 dwHeight;
    uint32 dwPitch;
    GfxScreenshot( lpBuffer, dwWidth, dwHeight, dwPitch );

    if ( dwWidth && dwHeight && lpBuffer )
    {
        CRefString sPath( FileReferenceGetPath( tRef ) );
        char nativePath[4096] = { 0 };
        Blizzard::String::MakeNativePath( sPath.str(), nativePath, sizeof( nativePath ) );

        ImageFileExportTexture( TEXTUREFORMAT_ARGB8888, lpBuffer, dwPitch, dwWidth, dwHeight, nativePath, eFileFormat, dwQuality );
        if ( pszComment &&
             eFileFormat == IMAGEFILEFORMAT_JPG &&
             pszComment[0] != '\0' )
        {
            JPGSetComment( sPath, pszComment );
        }
    }

    void * lpVoidBuffer = lpBuffer;
    gptGroupDefTexture->MemFree( lpVoidBuffer );


}




void GfxSetShaderGPRAllocation(const uint32 dwVertexShaderCount, const uint32 dwPixelShaderCount)
{
    gptGFXAPI->SetShaderGPRAllocation(dwVertexShaderCount, dwPixelShaderCount);
}






void GfxRestoreShaderGPRAllocation()
{
    gptGFXAPI->RestoreShaderGPRAllocation();
}





BOOL GfxTextureFormatIsValid(const TextureFormat eTexFormat, const uint32 dwTexCreateFlags)
{
    uint32 dwUsage = TEXTUREUSAGE_NORMAL;
    if (dwTexCreateFlags & 1)
        dwUsage = TEXTUREUSAGE_RENDERTARGET;
    else if (dwTexCreateFlags & 2)
        dwUsage = TEXTUREUSAGE_DEPTHTARGET;
    return gptGfxData->fTextureFormatValid[dwUsage][eTexFormat];
}

















BOOL GfxTextureCreate(TexData & tTexData, const TextureFormat eTexFormat, const uint32 dwWidth, const uint32 dwHeight, const uint32 dwMipMapLevels, const ResourceUsage eUsage, const uint32 dwTexCreateFlags, const uint32 dwMSAALevel, const uint32 dwTargetERAMOffset, const int32 nExponentBias, const TextureInitialData * pInitialData, const char * szTexDebugName)
{
    return gptGFXAPI->TextureCreate(&tTexData.htex, eTexFormat, dwWidth, dwHeight, dwMipMapLevels, eUsage, dwTexCreateFlags, dwMSAALevel, dwTargetERAMOffset, nExponentBias, pInitialData, szTexDebugName);
}

























































































static void sTextureReallyDispose(void * htex, void *)
{
    gptGFXAPI->TextureDispose(htex);
}





void GfxTextureDispose(TexData & tTexData)
{
    if (gptGFXAPI->TextureIsBusy(tTexData.htex))
        CGameAsyncRenderRegisterGPUResourceDelayedDeletion(sTextureReallyDispose, tTexData.htex, NULL);
    else
        gptGFXAPI->TextureDispose(tTexData.htex);
}





















































BOOL GfxCubeMapCreate(TexData & tTexData, const TextureFormat eTexFormat, const uint32 dwEdgeWidth, const uint32 dwMipMapLevels, const ResourceUsage eUsage, const uint32 dwTexCreateFlags, const uint32 dwTargetERAMOffset, const int32 nExponentBias, const TextureInitialData * pTextureInitialData, const char * szTexDebugName)
{
    return gptGFXAPI->CubeMapCreate(&tTexData.htex, eTexFormat, dwEdgeWidth, dwMipMapLevels, eUsage, dwTexCreateFlags, dwTargetERAMOffset, nExponentBias, pTextureInitialData, szTexDebugName);
}




















































BOOL GfxVolumeTexCreate(TexData & tTexData, const TextureFormat eTexFormat, const uint32 dwWidth, const uint32 dwHeight, const uint32 dwDepth, const uint32 dwMipMapLevels, const ResourceUsage eUsage, const uint32 dwTexCreateFlags, const TextureInitialData * pTextureInitialData, const char * szTexDebugName)
{
    return gptGFXAPI->VolumeTexCreate(&tTexData.htex, eTexFormat, dwWidth, dwHeight, dwDepth, dwMipMapLevels, eUsage, dwTexCreateFlags, pTextureInitialData, szTexDebugName);
}


















































void GfxSetRenderTargets(
    const RenderTarget * tRenderTargets )
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];

    BOOL fResetViewport = FALSE;
    IRect2D rectViewport;
    for ( int32 i = 0; i < 2; i++ )
    {
        const RenderTarget & tRT = tRenderTargets[i];


        if ( memcmp( &tRT, &workerData.tCurrentRTs[i], sizeof( RenderTarget ) ) != 0 )
        {



            workerData.tCurrentRTs[i] = tRT;

            if ( tRT.snoRTTex == SNO_NONE )
                gptGFXAPI->TextureSetRenderTarget( i, NULL );

            else if ( tRT.snoRTTex == 0 )
            {


                gptGFXAPI->SetRenderTargetToSwapChainRT( i );
                if ( i == 0 )
                {

                    uint32 dwScreenWidth, dwScreenHeight;
                    GfxGetCurrentDimensions( dwScreenWidth, dwScreenHeight );
                    rectViewport.Set( 0, 0, dwScreenWidth, dwScreenHeight );
                    fResetViewport = TRUE;
                }
            }
            else
            {
                ScopedTextureDefinitionLockR pdefTexture( tRT.snoRTTex );

                if ( pdefTexture->dwFaceCount == 1 )









                    gptGFXAPI->TextureSetRenderTarget( i, pdefTexture->tTexData.htex );
                else



                    gptGFXAPI->CubeMapSetRenderTarget( i, pdefTexture->tTexData.htex, tRT.eCubeMapFace );


                if ( i == 0 )
                {

                    uint32 dwWidth, dwHeight;
                    gptGFXAPI->GetRenderTargetCurrentResolution( pdefTexture->tTexData.htex, dwWidth, dwHeight );
                    rectViewport.Set( 0, 0, dwWidth, dwHeight );
                    fResetViewport = TRUE;
                }
            }
        }
    }



    if ( fResetViewport )
    {

        GfxViewportSet( rectViewport );


        workerData.dwRTWidth = rectViewport.right;
        workerData.dwRTHeight = rectViewport.bottom;
        workerData.flRTWidth = (float)rectViewport.right;
        workerData.flRTHeight = (float)rectViewport.bottom;
        workerData.flRTInvWidth = ( Math::Abs( workerData.flRTWidth ) > 0.000001f ) ? 1.0f / workerData.flRTWidth : 0.0f;
        workerData.flRTInvHeight = ( Math::Abs( workerData.flRTHeight ) > 0.000001f ) ? 1.0f / workerData.flRTHeight : 0.0f;
    }


}




void GfxSetRenderTarget(const SNO sno, const CubeMapFace eCubeMapFace)
{
    RenderTarget tRenderTargets[2] = { { sno, eCubeMapFace }, { SNO_NONE, CUBEMAPFACE_POSX } };
    GfxSetRenderTargets(tRenderTargets);
}










void GfxSetDepthTarget(const SNO sno)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    if (workerData.snoCurrentDepthTarget != sno)
    {
        workerData.snoCurrentDepthTarget = sno;
        if (sno == SNO_NONE)
        {
            gptGFXAPI->TextureSetDepthTarget(NULL, FALSE);
        }
        else
        {
            ScopedTextureDefinitionLockR pdefTexture(sno);
            gptGFXAPI->TextureSetDepthTarget(pdefTexture->tTexData.htex, pdefTexture->eTexFormat == TEXTUREFORMAT_DEPTHOPAQUE);
        }
    }
}

























void GfxCopyDepthTarget(const SNO snoTex)
{
    ScopedTextureDefinitionLockR pdefTexture(snoTex);
    gptGFXAPI->TextureCopyDepthTarget(pdefTexture->tTexData.htex);
}






void GfxSetSwapChainRenderTarget(const SNO sno)
{
    gptGfxData->snoSwapChainRenderTarget = sno;
    ScopedTextureDefinitionLockR pdefTexture(sno);
    gptGFXAPI->TextureSetSwapChainRenderTarget(pdefTexture->tTexData.htex);
}

// NON_MATCHING: 204 vs 212 bytes; original tail-calls TextureSetDepthTarget on the -1 path




void GfxSetRenderAndDepthTarget(const SNO snoRenderTarget, const SNO snoDepthTarget, const CubeMapFace eCubeMapFace)
{
    GfxSetRenderTarget(snoRenderTarget, eCubeMapFace);
    GfxSetDepthTarget(snoDepthTarget);
}







void GfxSetRenderTargetToSwapChainRT()
{
    RenderTarget tRenderTargets[2] = { { 0, CUBEMAPFACE_POSX }, { SNO_NONE, CUBEMAPFACE_POSX } };
    GfxSetRenderTargets(tRenderTargets);
}









void GfxGetRenderTargets(RenderTarget * tRenderTargets)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    SigmaMemoryCopy(tRenderTargets, workerData.tCurrentRTs, sizeof(workerData.tCurrentRTs));
}






SNO GfxGetDepthTarget()
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    return workerData.snoCurrentDepthTarget;
}






void GfxGetRenderTargetDimensions(uint32 & dwWidth, uint32 & dwHeight)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    dwWidth = workerData.dwRTWidth;
    dwHeight = workerData.dwRTHeight;
}








void GfxGetRenderTargetFloatDimensions(float & flWidth, float & flHeight)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    flWidth = workerData.flRTWidth;
    flHeight = workerData.flRTHeight;
}







void GfxGetRenderTargetInvFloatDimensions(float & flInvWidth, float & flInvHeight)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    flInvWidth = workerData.flRTInvWidth;
    flInvHeight = workerData.flRTInvHeight;
}






















void GfxRestoreRenderTarget(const SNO snoTex)
{
}






void GfxEvictResources()
{
    if (CmdLineGetParams()->tBitArray.Test(CMDLINE_NO_EVICT_BIT))
        return;
    gptGFXAPI->EvictResources();
}






























TextureFormat GfxGetDepthTextureBackbufferFormat()
{
    return TEXTUREFORMAT_D24X8;
}

































void GfxRenderClear(BOOL fClearTarget, BOOL fClearZBuffer, BOOL fClearStencilBuffer, const RGBAColor & rgbaTargetColor)
{
    if (gptGfxData->dwFlags & (1 << GFXINTERNAL_RENDERING_DISABLED_BIT))
        return;

    if (GfxGetSystemID() == GFX_D3D11)
    {
        RenderWorkerData & workerData = gptGfxData->workerData[0];
        for (uint32 dwStage = 0; dwStage < 24; dwStage++)
        {
            if (workerData.tRenderState.snoTex[dwStage] == workerData.snoCurrentDepthTarget || workerData.tRenderState.snoTex[dwStage] == workerData.tCurrentRTs[0].snoRTTex)
            {
                gptGFXAPI->TextureSetCurrent(dwStage, NULL);
                workerData.tRenderState.snoTex[dwStage] = SNO_NONE;
                workerData.tRenderStateLast.snoTex[dwStage] = SNO_NONE;
            }
        }
    }
    gptGFXAPI->RenderClear(fClearTarget, fClearZBuffer, fClearStencilBuffer, rgbaTargetColor);
}














BOOL GfxRenderBegin()
{
    return gptGFXAPI->RenderBegin();
}





void GfxRenderEnd()
{
    gptGFXAPI->RenderEnd();
}





void GfxTextureLock(const TexData & tTexData, const uint32 dwMipMapLevel, void *& lpTexture, uint32 & dwPitch, const uint32 dwFlags)
{
    gptGFXAPI->TextureLock(tTexData.htex, dwMipMapLevel, lpTexture, dwPitch, dwFlags);
}









void GfxTextureUnlock(const TexData & tTexData, const uint32 dwMipMapLevel)
{
    gptGFXAPI->TextureUnlock(tTexData.htex, dwMipMapLevel);
}






void GfxCubeMapLock(const TexData & tTexData, const CubeMapFace eCubeMapFace, const uint32 dwMipMapLevel, void *& lpTexture, uint32 & dwPitch, const uint32 dwFlags)
{
    gptGFXAPI->CubeMapLock(tTexData.htex, eCubeMapFace, dwMipMapLevel, lpTexture, dwPitch, dwFlags);
}










void GfxCubeMapUnlock(const TexData & tTexData, const CubeMapFace eCubeMapFace, const uint32 dwMipMapLevel)
{
    gptGFXAPI->CubeMapUnlock(tTexData.htex, eCubeMapFace, dwMipMapLevel);
}







void GfxVolumeTexLock(const TexData & tTexData, const uint32 dwMipMapLevel, void *& lpTexture, uint32 & dwRowPitch, uint32 & dwSlicePitch, const uint32 dwGfxFlags)
{
    gptGFXAPI->VolumeTexLock(tTexData.htex, dwMipMapLevel, lpTexture, dwRowPitch, dwSlicePitch, dwGfxFlags);
}










void GfxVolumeTexUnlock(const TexData & tTexData, const uint32 dwMipMapLevel)
{
    gptGFXAPI->VolumeTexUnlock(tTexData.htex, dwMipMapLevel);
}






static void GfxCursorSet(const SNO sno_, const uint32 dwFrame)
{





























    SNO sno = sno_;
    ScopedTextureDefinitionLockR tLock(sno);

    if (tLock && tLock->tTexData.htex)
    {




        gptGfxData->snoTexCursor = sno;
        gptGfxData->dwTexCursorFrame = dwFrame;
        gptGFXAPI->CursorSet(tLock->tTexData.htex, tLock->pHotspot, dwFrame);
    }
}













































BOOL GfxCursorMakeResident()
{
    TexData tTexData;
    if (gptGfxData->snoTexCursor == SNO_NONE)
        return FALSE;




    TextureDefinitionSetTexture(gptGfxData->snoTexCursor, tTexData);

    if (!tTexData.htex)
        return FALSE;



    GfxCursorSet(gptGfxData->snoTexCursor, gptGfxData->dwTexCursorFrame);
    return TRUE;
}






















void GfxSetClipPlane(int32 nIndex, const float * pflPlane)
{
    RenderWorkerData & workerData = gptGfxData->workerData[0];
    if (pflPlane)
    {
        workerData.flClipPlane[nIndex * 4 + 0] = pflPlane[0];
        workerData.flClipPlane[nIndex * 4 + 1] = pflPlane[1];
        workerData.flClipPlane[nIndex * 4 + 2] = pflPlane[2];
        workerData.flClipPlane[nIndex * 4 + 3] = pflPlane[3];
    }
    else
    {
        workerData.flClipPlane[nIndex * 4 + 0] = 0.0f;
        workerData.flClipPlane[nIndex * 4 + 1] = 0.0f;
        workerData.flClipPlane[nIndex * 4 + 2] = 0.0f;
        workerData.flClipPlane[nIndex * 4 + 3] = 0.0f;
    }
    gptGFXAPI->SetClipPlane(nIndex, pflPlane);
}










void GfxSetReflectionPlane(int32 nIndex, const float * pflPlane)
{
    gptGfxData->flReflectionPlane[nIndex * 4 + 0] = pflPlane[0];
    gptGfxData->flReflectionPlane[nIndex * 4 + 1] = pflPlane[1];
    gptGfxData->flReflectionPlane[nIndex * 4 + 2] = pflPlane[2];
    gptGfxData->flReflectionPlane[nIndex * 4 + 3] = pflPlane[3];
}








BOOL GfxStereo3DEnabled()
{
    return gptGFXAPI->Stereo3DEnabled();
}




















HardwareClass GfxGetHWClass()
{
    return sgtAppGlobals.eHWClassOverride;
}




void GfxIdleLoop(Blizzard::Time::Millisecond & nLastTime, int32 nTargetFPS)
{
    Blizzard::Time::Millisecond nDiff;
    Blizzard::Time::Millisecond nCurrTime;
    Blizzard::Time::Millisecond nTicksPerFrame;

    if (SigmaTimingGetMethod() != TimingMethodQPF)
    {
        const GamePerfStats & tPerfStats = GamePerfStatsGet();
        if (tPerfStats.flAvgFPS > (float)nTargetFPS)
            Sleep(1);
        return;
    }

    nTargetFPS = Math::Max(nTargetFPS, 1);
    nTicksPerFrame = (2000 + nTargetFPS) / (nTargetFPS * 2);
    for (;;)
    {
        nCurrTime = Blizzard::Time::Milliseconds();
        nDiff = nCurrTime - nLastTime;
        if (nDiff >= nTicksPerFrame)
            break;
        Sleep(1);
    }
    nLastTime = nCurrTime;
}





























const char * GfxGPUCrashDescription()
{
    return "";
}











TextureFormat TextureFormatGetFallbackFormat(const TextureFormat eTextureFormat)
{
    TextureFormat eFallback = TEXTUREFORMAT_ARGB8888;
    switch (eTextureFormat)
    {
    case TEXTUREFORMAT_A16B16G16R16F:
        eFallback = TEXTUREFORMAT_A32B32G32R32F;
        break;
    case TEXTUREFORMAT_D24X8:
        eFallback = TEXTUREFORMAT_D16;
        break;
    case TEXTUREFORMAT_D16:
        eFallback = TEXTUREFORMAT_DF24;
        break;
    case TEXTUREFORMAT_DF24:
        eFallback = TEXTUREFORMAT_DF16;
        break;
    case TEXTUREFORMAT_R16F:
        eFallback = TEXTUREFORMAT_R32F;
        break;
    case TEXTUREFORMAT_G32FR32F:
        eFallback = TEXTUREFORMAT_A32B32G32R32F;
        break;
    case TEXTUREFORMAT_G16FR16F:
        eFallback = TEXTUREFORMAT_A32B32G32R32F;
        break;
    }
    return eFallback;
}
