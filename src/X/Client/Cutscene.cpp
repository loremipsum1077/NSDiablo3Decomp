#include "CutsceneInternal.h"
#include "CGame.h"
#include "CPlayer.h"
#include "Gfx/GfxVB.h"
#include "Definitions/PreferencesDefinition.h"
#include "Sound/Sound.h"
#include "../Common/GameSwig.h"
#include "../Common/Player.h"
#include "../Common/WorldPlace.h"
#include "../Common/SNO/SNOName.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/SigmaTiming.h"
#include "../Sigma/StringManip.h"
#include "Console/GamerProfile.h"
int32 ActIndexFromActEnum(const Act eAct);  // X/Common/GameCommonData.cpp:395
void CutsceneAddToQueue(const CutsceneParams & tParams);  // Cutscene.cpp
namespace Console { namespace GamerProfile { void SavePrefs(); } }  // GamerProfileNX64.cpp:1694

typedef blz::vector<CutsceneParams, blz::allocator<CutsceneParams> > CutsceneQueue;

struct CutsceneGlobals {  // size 0x20
    CutsceneQueue tQueue;  // 0x00
    CutsceneID idFullscreenCutscene;  // 0x18
    BOOL fDisposeForGameInProgress;  // 0x1c
};

// X/Common/Shell/MainNX64.cpp:216
void ConfigureCutsceneAsyncThread(Blizzard::Thread::Thread tThread);
// X/Client/Console/CutsceneConsole.cpp
void CutsceneConsoleInitializeForApp();
void CutsceneConsoleDisposeForApp();
// X/Client/UserInterface/UI.cpp:556
BOOL UI_ValidateController(int32 nController);
namespace Console { namespace GamerProfile { void SavePrefs(); } }

void CutscenePlayFinalCinematic(const BOOL fExpansion, const PlayerClass ePlayerClass, const BOOL fIsMale, CutsceneCallback pfnOnFinishedCallback);
CutsceneID CutsceneGetFullScreenCutscene();
static OggPageState sOggGetPacket(Cutscene * ptCutscene, ogg_stream_state * pOggStream, ogg_packet * pOggPacket);
BOOL CutsceneCanBeCancelled(const CutsceneID idCutscene);

#include "Definitions/TextureDefinition.h"
Cutscene * CutsceneGet(CutsceneID id);
// X/Client/Gfx/Gfx.cpp:3651,3664; declarations only.
void GfxTextureLock(const TexData &, uint32, void *&, uint32 &, uint32);
void GfxTextureUnlock(const TexData &, uint32);
#include "../Common/ThreadGlobals.h"
// X/Sigma/Files/XFilesCore.cpp:1146.
BOOL FileRead(FileReference &, void *, uint32, uint32, ErrorCode);
#include "../Sigma/UnicodeText.h"
#include <stdio.h>
#include <string.h>
// X/Common/Sno/SNOFiles.cpp:7572.
CRefString SNOFilesGetLocalePath(XLocale eXLocale);
#include "UserInterface/UI.h"
#include "UserInterface/Controls/UITextControl.h"
#include "../Common/GameTesting.h"
#include "../Common/Shell/Shell.h"
// X/Common/GameTesting.cpp:625.
namespace GameTesting { TestType GetTestType(); }
// X/Client/UserInterface/UIGlobals.cpp:1406-1407.
extern UIID UIDLG_CUTSCENE;
extern UIID UITXT_CUTSCENE_SUBTITLE;
// Sound.cpp: SoundManager::SuppressSounds; Cutscene.cpp:1616,1566.
namespace SoundManager { void SuppressSounds(uint32); }
static CutsceneID sCutscenePlay(const CutsceneParams & tParams);
void CutsceneTryToCancel(const UIID &);
#include "UserInterface/Console/ConsoleScreenManager.h"
#include "UserInterface/Console/ConsoleModalNotificationManager.h"
#include "../Sigma/BlzExt/BlzVectorHelper.h"



BOOL s_bCutsceneDebug = FALSE;
BOOL s_bDebugLog = FALSE;

uint32 s_nAsyncReaderHardwareThread = 0;

BOOL s_bCutsceneAudioSync = TRUE;
int32 s_bCutsceneStopAtFrame = 0;

int32 s_nCutsceneAudioFrame = 0;
int32 s_nCutsceneTimeBasedFrame = 0;
















static const uint32 c_channelRemap6[6] = { 0, 2, 1, 5, 3, 4 };








static const uint32 c_channelRemap8[8] = { 0, 2, 1, 7, 5, 6, 3, 4 };








Blizzard::Thread::Thread AsyncReader::s_tThread;



Blizzard::Lock::REvent AsyncReader::s_tStartWork;
Blizzard::Lock::REvent AsyncReader::s_tWorkFinished;
volatile uint32 AsyncReader::s_bTerminateThread = 0;
AsyncReader * AsyncReader::s_ptAsyncReader = NULL;
#include "Gfx/GfxPrimitives.h"
// Gfx.cpp:1854, Input.cpp:1073, CutsceneConsole.cpp:543, Sound.cpp: StopOVTStream.
void GfxGetCurrentDimensions(uint32 &, uint32 &);
void InputShowPlatformCursor(BOOL, BOOL);
void CutsceneStopConsole(Cutscene *);
namespace SoundManager { void StopOVTStream(); }
// Cutscene.cpp:2657.
static void sShowCursor(const BOOL fShow);
#include "Font/Font.h"
#include "Font/FormattedText.h"
// CutsceneConsole.cpp:84,394,576; Sound.cpp OVT entry points.
void CutsceneDecodeAudioConsole(Cutscene *);
int32 CutsceneDecodeFrameConsole(Cutscene *);
void CutsceneInitConsole(Cutscene *);
namespace SoundManager { BOOL GetOVTStreamPos(float &); void OpenOVTStream(SoundOVT *); void PlayOVTStream(); }
// Cutscene.cpp:2911,2166,1583,2518,2704.
static BOOL sOpenCutscene(Cutscene *);
void sTearDownOgg(Cutscene *);
static void sCommonCleanup(Cutscene *);
static OggPageState sOggGetPage(Cutscene *);
static BOOL sInitCutscene(Cutscene *);
OggPageState CutsceneOggGetPacketBlocking(Cutscene *, ogg_stream_state *, ogg_packet *);
#include "Sprite.h"
// Input.cpp:1064; Font.cpp:889.
BOOL InputIsCursorInWindow();
void FontStringDrawCenteredUIC(const TextRenderProperties &, const FormattedText &, const Vector2D &, const RGBAColor &, const RGBAColor *, float, RenderLayer, BOOL);





































void CutscenesInitializeForApp()
{
    AsyncReader::s_bTerminateThread = FALSE;
    BOOL fSuccess = Blizzard::Thread::Create(AsyncReader::sThreadFunction, NULL, &AsyncReader::s_tThread, "sCutsceneAsyncThread", 1, 0x8000);
    ConfigureCutsceneAsyncThread(AsyncReader::s_tThread);
    Blizzard::Thread::Resume(AsyncReader::s_tThread);

    CutsceneConsoleInitializeForApp();
}












void CutscenesDisposeForApp()
{
    AsyncReader::s_bTerminateThread = TRUE;
    AsyncReader::s_tStartWork.Set();
    Blizzard::Thread::WaitForever(AsyncReader::s_tThread);
    Blizzard::Thread::Close(AsyncReader::s_tThread);

    CutsceneConsoleDisposeForApp();
}








void CutscenesInitializeForGame()
{
    CGame & tCGame = CGameGet();

    CutsceneGlobals *& ptCutsceneGlobals = tCGame.ptCutsceneGlobals;
    ptCutsceneGlobals = (CutsceneGlobals *)SigmaMemoryNew(sizeof(CutsceneGlobals), 4, AppGetCurrentMemoryInterface(), TRUE);
    new (&ptCutsceneGlobals->tQueue) CutsceneQueue();
    tCGame.ptCutsceneGlobals->idFullscreenCutscene = CUTSCENEID_INVALID;
    tCGame.ptCutsceneGlobals->fDisposeForGameInProgress = FALSE;

    tCGame.ptCutscenes = TDataArrayCreateTracked<Cutscene, CutsceneID>("Cutscenes", AppGetCurrentMemoryInterface(), 1);
}







static void sCutscenesStopAll()
{
    CGame & tCGame = CGameGet();
    TDataArrayIterator<Cutscene> tIt;
    while (tCGame.ptCutscenes->Iterate(tIt))
    {
        Cutscene * ptCutscene = tIt.ptElement;
        CutsceneStop(ptCutscene->id, TRUE);
    }
}








void CutscenesDisposeForGame()
{
    CGame & tCGame = CGameGet();
    CutsceneGlobals *& ptCutsceneGlobals = tCGame.ptCutsceneGlobals;

    ptCutsceneGlobals->fDisposeForGameInProgress = TRUE;

    sCutscenesStopAll();

    TDataArrayDispose(tCGame.ptCutscenes);

    ptCutsceneGlobals->tQueue.~CutsceneQueue();
    ptCutsceneGlobals->fDisposeForGameInProgress = FALSE;
    SigmaMemoryFree(ptCutsceneGlobals, AppGetCurrentMemoryInterface());
    ptCutsceneGlobals = NULL;
}




static void sCutsceneGfxInit(
    const CutsceneID idCutscene)
{
    Cutscene * ptCutscene = CutsceneGet(idCutscene);


    TextureFormat eFormat = TEXTUREFORMAT_LIN_ARGB8888;




    for (int32 nTex = 0; nTex < Cutscene::TEXTURE_COUNT; ++nTex)
    {




        ptCutscene->snoTex[nTex] = TextureDefinitionCreateTexture("Cutscene", eFormat, eFormat,
            ptCutscene->theoraInfo.pic_width, ptCutscene->theoraInfo.pic_height,
            RESOURCEUSAGE_STATIC, 0, 1, 0, 0);







        if (ptCutscene->snoTex[nTex] != SNO_NONE)
        { // Inference: original calls the group API directly; its typed result is a TextureDefinition.
            const TextureDefinition * pdefTexture = (const TextureDefinition *)gptGroupDefTexture->LockR(ptCutscene->snoTex[nTex], FALSE);
            void * ptDest = NULL;
            uint32 dwDestPitch;
            GfxTextureLock(pdefTexture->tTexData, 0, ptDest, dwDestPitch, 0);
            if (ptDest) {
                // Inference: neutral YCbCr color from the original 0xff808000 word stores.
                RGBAColor rgba(0, 128, 128, 255);

                for (uint32 row = 0; row < ptCutscene->theoraInfo.pic_height; ++row)
                { // Inference: the original addresses rows by the texture byte pitch.
                    RGBAColor * pDst = (RGBAColor *)((uint8 *)ptDest + row * dwDestPitch);
                    for (uint32 col = 0; col != ptCutscene->theoraInfo.pic_width; ++col)
                    {
                        *pDst++ = rgba;
                    }
                }
                GfxTextureUnlock(pdefTexture->tTexData, 0);
            } // Inference: UnlockR takes the address of the typed lock local (no intermediate local in DWARF).
            gptGroupDefTexture->UnlockR((void *&)pdefTexture);
        }
    }
    ptCutscene->nUpdateTextureIndex = 0;
    ptCutscene->nDrawTextureIndex[CGameAsyncGetRenderDataIndex(FALSE)] = Cutscene::TEXTURE_COUNT - 1;

    ptCutscene->bSceneOpacity = ptCutscene->bBorderOpacity = 255;

}

#include "Definitions/TextureDefinition.h"
inline Cutscene * CutsceneGet(CutsceneID id);

static void sCutsceneGfxDispose(CutsceneID idCutscene)
{
    Cutscene * ptCutscene = CutsceneGet(idCutscene);
    CGameAsyncRenderFlush();
    for (int32 i = 0; i < 3; i++)
    {
        gptGroupDefTexture->Purge(ptCutscene->snoTex[i], FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
        ptCutscene->snoTex[i] = SNO_NONE;
    }
}









void CutscenesGfxInit()
{

    CGame & tCGame = CGameGet();

    TDataArrayIterator<Cutscene> tIt;
    while (tCGame.ptCutscenes->Iterate(tIt))
    {
        Cutscene * ptCutscene = tIt.ptElement;
        sCutsceneGfxInit(ptCutscene->id);
    }
}




void CutscenesGfxDispose()
{
    CGame & tCGame = CGameGet();
    TDataArrayIterator<Cutscene> tIt;
    while (tCGame.ptCutscenes->Iterate(tIt))
    {
        Cutscene * ptCutscene = tIt.ptElement;
        sCutsceneGfxDispose(ptCutscene->id);
    }
}






inline Cutscene * CutsceneGet(CutsceneID id)
{
    return CGameGet().ptCutscenes->ElementGet(id);
}





inline Cutscene * CutsceneTryToGet(CutsceneID id)
{
    return CGameGet().ptCutscenes->ElementTryToGet(id);
}





inline void AsyncReader::SetIOThreadOwner(AsyncReader * ptAsyncReader)
{
    s_ptAsyncReader = ptAsyncReader;
}

#include "../Common/FileIO/FileIO.h"
namespace FileIO {

bool FileExists(const char * szFileName, XLocale eLocale);
}  // namespace FileIO
// X/Sigma/Files/XFilesCore.cpp
void FileReferenceInit(FileReference & tFileRef, LPCSTR szPath); BOOL FileExists(const FileReference & tFileRef);
BOOL FileOpen(FileReference & tFileRef, uint32 dwAccessFlags); BOOL FileGetSize(const FileReference & tFileRef, uint32 & dwSize);
void FileClose(FileReference & tFileRef);




inline void AsyncReader::sSetupIOThread(AsyncReader & tAsyncReader)
{
    SetIOThreadOwner(&tAsyncReader);
    tAsyncReader.m_bStopReading = FALSE;
    s_tStartWork.Set();
}




BOOL AsyncReader::Initialize(LPCSTR szFilePath)
{
    m_nLastSequenceIndexReturned = (uint32)-1;
    m_ptFile = NULL;
    m_nBufferUsedStart = 0;
    m_nBufferFreeStart = 0;
    m_nBufferFreeCount = BUFFER_COUNT;
    m_nLastSequenceIndexRead = 0;
    m_dwFileOffset = 0;
    if (FileIO::FileExists(szFilePath, 1))
    {
        m_ptFile = new FileIO::File(1);
        if (!m_ptFile->Open(szFilePath))
            return FALSE;
        m_dwFileSize = (uint32)m_ptFile->GetFileSize();
    }
    else
    {
        CRefString sPath(szFilePath);
        FileReferenceInit(m_tFileRef, sPath.str());
        if (!FileExists(m_tFileRef) || !FileGetSize(m_tFileRef, m_dwFileSize))
            return FALSE;
        if (!FileOpen(m_tFileRef, 1))
            return FALSE;
    }

    for (int32 i = 0; i < BUFFER_COUNT; i++)
        m_tChunkBuffers[i].Initialize();

    m_chunkProcessed.Create();
    sSetupIOThread(*this);
    return TRUE;
}

































inline void AsyncReader::sReleaseIOThread(AsyncReader & tAsyncReader)
{
    tAsyncReader.m_bStopReading = TRUE;
    tAsyncReader.m_chunkProcessed.Set();
    s_tWorkFinished.WaitForever();
    SetIOThreadOwner(NULL);
}






void AsyncReader::Dispose()
{
    sReleaseIOThread(*this);

    m_chunkProcessed.Destroy();
    if (m_ptFile)
    {
        delete m_ptFile;
        m_ptFile = NULL;
    }
    else
    {
        FileClose(m_tFileRef);
    }

    for (int32 i = 0; i < BUFFER_COUNT; i++)
        m_tChunkBuffers[i].Dispose();
}








uint32 AsyncReader::GetChunk(char * lpDest)
{
    uint32 dwNumRead;
    if (m_nBufferFreeCount == BUFFER_COUNT)
        return IsTerminated() ? 0 : (uint32)-1;

    ChunkBuffer & tCurrBuffer = m_tChunkBuffers[m_nBufferUsedStart];
    m_nLastSequenceIndexReturned = tCurrBuffer.nSequenceIndex;
    dwNumRead = tCurrBuffer.dwNumBytes;
    if (dwNumRead)
        SigmaMemoryCopy(lpDest, tCurrBuffer.lpBuffer, dwNumRead);

    m_nBufferUsedStart = (m_nBufferUsedStart + 1 == BUFFER_COUNT) ? 0 : m_nBufferUsedStart + 1;
    Blizzard::Lock::Atomic::Increment(&m_nBufferFreeCount);
    m_chunkProcessed.Set();
    return dwNumRead;
}


























inline BOOL AsyncReader::IsTerminated() const
{
    return m_bStopReading;
}

// DWARF inline instances support this real callee; no standalone original body is checkable.
uint32 AsyncReader::GetBufferFreeCount() const
{
    return m_nBufferFreeCount;
}







BOOL AsyncReader::ReadChunk(
    ChunkBuffer & tChunkBuffer)
{
    uint32 dwNumToRead = Math::Min(m_dwFileSize - m_dwFileOffset, (uint32)CHUNK_BUFFER_SIZE);



    tChunkBuffer.dwNumBytes = dwNumToRead;
    if (m_ptFile)
    {




















        if (!m_ptFile->ReadFile(tChunkBuffer.lpBuffer, dwNumToRead, NULL))
            return FALSE;







    }
    else
    {
        if (!FileRead(m_tFileRef, tChunkBuffer.lpBuffer, m_dwFileOffset, dwNumToRead, ERROR_FILE_READ))
            return FALSE;
    }








    m_dwFileOffset += dwNumToRead;

    tChunkBuffer.nSequenceIndex = m_nLastSequenceIndexRead++;

    return TRUE;
}




uint AsyncReader::sThreadFunction(
    void * parms)
{

    ThreadGlobals::InitializeForThread();
    ThreadGlobals::Get().m_nThreadIndex = THREADINDEX_CUTSCENE_ASYNC_LOAD;

    for (;;)
    {
        s_tStartWork.WaitForever();

        if (s_bTerminateThread)
            break;





        while (s_ptAsyncReader->m_dwFileOffset < s_ptAsyncReader->m_dwFileSize)
        {
            while (s_ptAsyncReader->m_nBufferFreeCount == 0 && !s_ptAsyncReader->m_bStopReading)
            {
                s_ptAsyncReader->m_chunkProcessed.WaitForever();
            }

            if (s_ptAsyncReader->m_bStopReading)
                break;



            if (!s_ptAsyncReader->ReadChunk(s_ptAsyncReader->m_tChunkBuffers[s_ptAsyncReader->m_nBufferFreeStart]))
                break;





            ++s_ptAsyncReader->m_nBufferFreeStart;
            if (s_ptAsyncReader->m_nBufferFreeStart == BUFFER_COUNT)
                s_ptAsyncReader->m_nBufferFreeStart = 0;



            Blizzard::Lock::Atomic::Decrement(&s_ptAsyncReader->m_nBufferFreeCount);
        }
        s_ptAsyncReader->m_bStopReading = TRUE;
        s_tWorkFinished.Set();
    }

    ThreadGlobals::DisposeForThread();
    return 0;
}



// DWARF inline instances support this real callee; no standalone original body is checkable.
BOOL AsyncReader::HasData() const
{
    return m_nBufferFreeCount != BUFFER_COUNT;
}











































































































































uint32 sSMPTEToFrameNum(
    const float flFrameRate,
    const float * flSMPTE)
{
    uint32 dwFrameNum = (uint32)((flSMPTE[0] * 3600.0f + flSMPTE[1] * 60.0f +
        flSMPTE[2] + flSMPTE[3] / 100.0f) * flFrameRate);
    return dwFrameNum;
}



static char * sReadFileToString(
    const char * szFilename,
    uint32 & uSize)
{
    FileIO::File tFile(XLOCALE_GLOBAL);
    CRefString sLocalePath = SNOFilesGetLocalePath(XLocaleGetBaseLocale(UnicodeText::GetStringDisplayLocale()));
    CRefString sPath = CRefString().Format("%s\\Cutscenes\\%s", sLocalePath.str(), szFilename);

    char szFixedUpFilename[4096];
    // Inference: failed replacement and null filename share the original zero-size/null return tail.
    if (!strreplace(sPath.str(), szFixedUpFilename, sizeof(szFixedUpFilename), "-stream", "") || !szFilename)
    {
        uSize = 0;
        return NULL;
    }
    uSize = 0;
    if (szFilename && *szFilename && tFile.Open(szFixedUpFilename))
    {
        uSize = (uint32)tFile.GetFileSize();

        char * lpBuffer = (char *)SigmaMemoryNew(uSize + 1, 4, NULL, FALSE);
        uint64 dwBytesRead = 0;

        if (!tFile.ReadFile(lpBuffer, uSize, &dwBytesRead) || dwBytesRead != uSize)
        {
            SigmaMemoryFree(lpBuffer, NULL);
            return NULL;
        }

        lpBuffer[uSize] = '\0';
        return lpBuffer;
    }

    return NULL;
}





void sLoadSubTitle(
    Cutscene * ptCutscene,
    LPCSTR szSubtitleName)
{
    uint32 dwFileSize;
    ptCutscene->szSubTitlesFromFile = sReadFileToString(szSubtitleName, dwFileSize);
    if (dwFileSize > 1 &&
        // Inference: the line-zero cbz at original +0x15c checks the loaded subtitle buffer.
        ptCutscene->szSubTitlesFromFile) { ptCutscene->nSubTitleSearchStartPoint = 0;
        ptCutscene->parSubTitles = new blz::vector<SubTitle>;
        ptCutscene->parSubTitles->reserve(dwFileSize / 120 + 1);
        for (LPSTR szIter = ptCutscene->szSubTitlesFromFile; (uint32)(szIter - ptCutscene->szSubTitlesFromFile) < dwFileSize;)
        {
            if (*szIter < '0' || *szIter > '9')
            {
                ++szIter;
                continue;
            }

            float flSMPTE[8];
            int32 nFields = sscanf(szIter, "%f:%f:%f:%f - %f:%f:%f:%f", &flSMPTE[0], &flSMPTE[1], &flSMPTE[2], &flSMPTE[3],
                &flSMPTE[4], &flSMPTE[5], &flSMPTE[6], &flSMPTE[7]);


            if (nFields == 8)
            {
                SubTitle tSubTitle;

                tSubTitle.dwStart = sSMPTEToFrameNum(ptCutscene->flFrameRate, flSMPTE);
                tSubTitle.dwEnd = sSMPTEToFrameNum(ptCutscene->flFrameRate, flSMPTE + 4);
                szIter = strchr(szIter, '\n');
                if (!szIter)
                {
                    return;
                }

                ++szIter;

                tSubTitle.szText = szIter;
                while (*szIter != '\0' && *szIter != '\n' && *szIter != '\r')
                    ++szIter;

                // Inference: the terminator is consumed; original stores zero at the preceding byte.
                *szIter++ = '\0';

                ptCutscene->parSubTitles->push_back(tSubTitle);
            }
        }
    }
}



// DWARF inline instances support this real callee; no standalone original body is checkable.
static BOOL sPrepareCutscene(
    Cutscene * ptCutscene,
    LPCSTR szName,
    const uint32 nAudioStreamIndex,
    const uint32 nVideoStreamIndex)
{




    CRefString sLocalePath = SNOFilesGetLocalePath(XLocaleGetBaseLocale(UnicodeText::GetCurrentLocale()));



    CRefString strFinalName;

    strFinalName = CRefString().Format("%s\\Cutscenes\\%s", sLocalePath.str(), szName);
    // DWARF DIE 0x4c3fec records bStreamingMode as constant FALSE.
    BOOL bStreamingMode = FALSE;

















































    ptCutscene->m_eCutsceneState = Cutscene::INITALIZING;


    CutsceneInitParams & tParams = ptCutscene->m_tInitParams;

    tParams.m_ptCutscene = ptCutscene;
    tParams.m_strFinalName = strFinalName;
    tParams.m_nVideoStreamIndex = nVideoStreamIndex;
    tParams.m_nAudioStreamIndex = nAudioStreamIndex;
    tParams.m_bStreamingMode = bStreamingMode;


    if (!ptCutscene->tAsyncReader.Initialize(tParams.m_strFinalName))
        return FALSE;




    ogg_sync_init(&ptCutscene->oggSync);
    ptCutscene->fFoundTheoraStream = FALSE;
    ptCutscene->fFoundVorbisStream = FALSE;

    return TRUE;
}



// DWARF inline instances support this real callee; no standalone original body is checkable.
static void sRenderDebug(
    Cutscene * ptCutscene)
{

    CRefString strFPS;

    Rect2D rectViewportUIC;
    CGameGetSafeViewportUIC(rectViewportUIC);
    Vector2D vecUIC(rectViewportUIC.left, rectViewportUIC.top);
    TextRenderProperties tProps(GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF), 13, 1.0f, 1.0f);
    vecUIC.y += 16.0f;


    strFPS.Format("Cutscene buffer state: [");

    int32 nBuffersFull = 0;

    for (int32 i = 0; i < Cutscene::FRAME_COUNT; ++i)
    {
        if (ptCutscene->theoraFrames[i].m_eState != TheoraFrame::STATE_EMPTY)
            ++nBuffersFull;
    }



    for (int32 i = 0; i < Cutscene::FRAME_COUNT; ++i)
    {
        if (i < nBuffersFull)
        {
            strFPS += "X";
        }
        else
        {
            strFPS += " ";
        }
    }
    strFPS += "]";

    FontStringDrawUIC(tProps, FormattedText(strFPS.str(), PARSE_DEFAULT, FALSE), vecUIC, crgbaWhite, &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
    vecUIC.y += 16.0f;

    strFPS.Format("DecodingFrame: %d", ptCutscene->nDecodingVideoFrame);
    FontStringDrawUIC(tProps, FormattedText(strFPS.str(), PARSE_DEFAULT, FALSE), vecUIC, crgbaWhite, &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
    vecUIC.y += 16.0f;

    strFPS.Format("ReadFrom: %d, WriteTo: %d", ptCutscene->nFrameToReadFrom, ptCutscene->nFrameToWriteTo);
    FontStringDrawUIC(tProps, FormattedText(strFPS.str(), PARSE_DEFAULT, FALSE), vecUIC, crgbaWhite, &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
    vecUIC.y += 16.0f;

    for (int32 i = 0; i < Cutscene::FRAME_COUNT; ++i)
    {
        strFPS.Format("%d: State: %d, Frame: %d", i, ptCutscene->theoraFrames[i].m_eState, ptCutscene->theoraFrames[i].m_nFrameIndex);
        FontStringDrawUIC(tProps, FormattedText(strFPS.str(), PARSE_DEFAULT, FALSE), vecUIC, crgbaWhite, &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
        vecUIC.y += 16.0f;
    }



    strFPS.Format("VideoFrame: %d, Frame: %d", ptCutscene->nCurrentVideoFrame, ptCutscene->nCurrentFrame);
    FontStringDrawUIC(tProps, FormattedText(strFPS.str(), PARSE_DEFAULT, FALSE), vecUIC, crgbaWhite, &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
}






// DWARF inline instances support this real callee; no standalone original body is checkable.
static void sCutsceneRenderUpdate(
    const CutsceneID idCutscene)
{
    Cutscene * ptCutscene = CutsceneGet(idCutscene);

    if (s_bCutsceneDebug)
    {
        sRenderDebug(ptCutscene);
    }

    switch (ptCutscene->m_eCutsceneState)
    {
    case Cutscene::INITALIZING:
        {
            BOOL bError = FALSE;


            if (!ptCutscene->tAsyncReader.HasData())
            {
                // Inference: original line-zero branch skips processing until data or termination.
                if (!ptCutscene->tAsyncReader.IsTerminated())
                    return;
                bError = TRUE;
            }
            else
            {

                bError = !sOpenCutscene(ptCutscene);
            }

            if (bError)
            {


                CutsceneStop(ptCutscene->id, FALSE);
                break;
            }
        }
        // Inference: successful initialization falls through the loading-screen and preloading states.
    case Cutscene::WAITING_FOR_THE_LOADING_SCREEN_TO_END:

        ptCutscene->m_eCutsceneState = Cutscene::WAITING_FOR_THE_LOADING_SCREEN_TO_END;
        if (CGameLoadingScreenIsEnabled())
            return;






    case Cutscene::PRELOADING:
        {
            ptCutscene->m_eCutsceneState = Cutscene::PRELOADING;
            if (!ptCutscene->tAsyncReader.IsTerminated() && ptCutscene->tAsyncReader.GetBufferFreeCount() != 0)
                break;




            ptCutscene->m_eCutsceneState = Cutscene::PLAYBACK;



            SoundManager::SetGlobalMute(FALSE);
            CutsceneDecodeAudioConsole(ptCutscene);
            CutsceneInitConsole(ptCutscene);





            ptCutscene->tOVT.dwStartTick = GetMachineTicks();

            SoundManager::OpenOVTStream(&ptCutscene->tOVT);
            SoundManager::PlayOVTStream();
        }
        // Inference: preloading completion immediately enters playback in the original control flow.






    case Cutscene::PLAYBACK:
        {

            SoundOVT & tOVT = ptCutscene->tOVT;


            uint32 dwTickCount = GetMachineTicks();
            uint32 dwFakeAudioPos = (uint32)((uint64)(tOVT.dwFrequency * tOVT.dwChannels) * (dwTickCount - tOVT.dwStartTick) / 1000);
            tOVT.dwFakeAudioPos = Math::Max(tOVT.dwFakeAudioPos, dwFakeAudioPos);


            // DWARF DIE 0x4c2148 records this local as constant FALSE throughout playback.
            BOOL bCriticalDesyncTerminate = FALSE;

            if (s_bCutsceneAudioSync)
            {
                const uint32 dwSamplesPerFrame = tOVT.dwFrequency / tOVT.dwFPS * tOVT.dwChannels;

                float flStreamPos = 0.0f;
                BOOL bAudioStreamPosValid = SoundManager::GetOVTStreamPos(flStreamPos);

                int32 nAudioBufferFrame = (int32)(flStreamPos * ptCutscene->flFrameRate);
                int32 nTimeBasedFrame = tOVT.dwFakeAudioPos / dwSamplesPerFrame;

                s_nCutsceneAudioFrame = nAudioBufferFrame;
                s_nCutsceneTimeBasedFrame = nTimeBasedFrame;

                if (SoundManager::GetGlobalMute() || ptCutscene->bAudioMuted || !bAudioStreamPosValid)
                    ptCutscene->nCurrentFrame = nTimeBasedFrame;
                else
                    ptCutscene->nCurrentFrame = nAudioBufferFrame;
            }
            else
            {




                ptCutscene->nCurrentFrame++; // original rows 1259:29 (member) / 1259:42 (++): postfix
            }

            // Inference: decoder failure joins the frame-limit stop; DWARF keeps the desync flag FALSE. Rows: final branch 1263:17, call 1267:18, || 1269:17, StopAtFrame 1269:21, && 1269:49.
            if (bCriticalDesyncTerminate ||



                !CutsceneDecodeFrameConsole(ptCutscene)

                || (s_bCutsceneStopAtFrame != 0 && ptCutscene->nCurrentFrame > s_bCutsceneStopAtFrame))
            {
                CutsceneStop(ptCutscene->id, FALSE);
            }
            break;
        }
    }

    ptCutscene->nDrawTextureIndex[CGameAsyncGetRenderDataIndex(FALSE)] = ptCutscene->nUpdateTextureIndex;
}



// DWARF inline instances support this real callee; no standalone original body is checkable.
static void sUpdateQueue()
{

    CGame & tCGame = CGameGet();
    CutsceneGlobals * ptCutsceneGlobals = tCGame.ptCutsceneGlobals;


    if (tCGame.ptCutscenes->GetTotalCount() == 0)
    {
        if (ptCutsceneGlobals->tQueue.size())
        {
            CutsceneParams tParams = ptCutsceneGlobals->tQueue[0];
            blzext::vector_erase_at(ptCutsceneGlobals->tQueue, 0);
            sCutscenePlay(tParams);
        }
    }
}




void CutscenesUpdate()

{







    ConsoleModalNotificationManager & tManager = Console::Screens::GetModalNotificationManager();
    if (tManager.IsNotificationShowing())
    {
        CutsceneTryToCancel(GetUIIDNone());
    }


    sUpdateQueue();
}



// NON_MATCHING: 3264/3276 bytes; the call to CutsceneDecodeFrameConsole is present (not inlined): the original tail-duplicates it into both 1235 branches because its merge block is mov/bl/cbz only, ours reloads a spilled index after the call; ours also keeps GlobalSNO 637 (1079) in w21 where the original rematerialises it. Register pressure, no source lead.
void CutscenesRenderUpdate()

{
    CGame & tCGame = CGameGet();

    TDataArrayIterator<Cutscene> tIt;
    while (tCGame.ptCutscenes->Iterate(tIt))
    {
        Cutscene * ptCutscene = tIt.ptElement;
        if (ptCutscene->fShouldBeCancelled)
        {
            CutsceneStop(ptCutscene->id, FALSE);
            continue;
        }


        sCutsceneRenderUpdate(ptCutscene->id);
    }
}




void CutscenesRenderFullscreen()

{
    CGame & tCGame = CGameGet();
    CutsceneGlobals * ptCutsceneGlobals = tCGame.ptCutsceneGlobals;

    if (ptCutsceneGlobals->idFullscreenCutscene == CUTSCENEID_INVALID)
        return;



    Cutscene * ptCutscene = CutsceneGet(ptCutsceneGlobals->idFullscreenCutscene);
    const float cflZCutscene = 0.0001f;
    uint32 dwScreenWidth;
    uint32 dwScreenHeight;
    GfxGetCurrentDimensions(dwScreenWidth, dwScreenHeight);
    sShowCursor(FALSE);

    if (ptCutscene->m_eCutsceneState == Cutscene::INITALIZING || ptCutscene->m_eCutsceneState == Cutscene::PRELOADING)
    {

        Rect2D rect;
        RGBAColor rgbaColor(0, 0, 0, 255);
        rect.Set(0.0f, 0.0f, (float)dwScreenWidth, (float)dwScreenHeight);

        GfxRectDrawDeferred(rect, rgbaColor, cflZCutscene, RENDERLAYER_INVALID, TRUE);
        return;
    }








    uint32 nAdjustedWidth = ptCutscene->theoraInfo.pic_width * dwScreenHeight / ptCutscene->theoraInfo.pic_height;
    uint32 nAdjustedHeight = ptCutscene->theoraInfo.pic_height * dwScreenWidth / ptCutscene->theoraInfo.pic_width;
    Rect2D rectTopBar;
    Rect2D rectMain;
    Rect2D rectBottomBar;
    if (nAdjustedHeight <= dwScreenHeight)
    {
        int32 nOffset = (dwScreenHeight - nAdjustedHeight) / 2;
        rectTopBar.Set(0.0f, 0.0f, (float)dwScreenWidth, (float)nOffset);
        rectMain.Set(0.0f, (float)nOffset, (float)dwScreenWidth, (float)nAdjustedHeight + nOffset);
        rectBottomBar.Set(0.0f, (float)nAdjustedHeight + nOffset, (float)dwScreenWidth, (float)dwScreenHeight);
    }
    else
    {

        int32 nOffset = (dwScreenWidth - nAdjustedWidth) / 2;
        rectTopBar.Set(0.0f, 0.0f, (float)nOffset, (float)dwScreenHeight);
        rectMain.Set((float)nOffset, 0.0f, (float)nAdjustedWidth + nOffset, (float)dwScreenHeight);
        rectBottomBar.Set((float)nAdjustedWidth + nOffset, 0.0f, (float)dwScreenWidth, (float)dwScreenHeight);
    }




    RenderLayer eCutsceneRenderLayer = RENDERLAYER_UI;



    {
        RGBAColor rgbaColor(0, 0, 0, ptCutscene->bBorderOpacity);
        GfxRectDrawDeferred(rectTopBar, rgbaColor, cflZCutscene, eCutsceneRenderLayer, TRUE);
        GfxRectDrawDeferred(rectBottomBar, rgbaColor, cflZCutscene, eCutsceneRenderLayer, TRUE);


        rgbaColor = crgbaWhite;
        rgbaColor.a = ptCutscene->bSceneOpacity;
        SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_CUTSCENE);

        Rect2D rectTexCoordsAlphaMask(0.0f, 0.0f, 1.0f, 1.0f);
        SNO snoTex = ptCutscene->snoTex[ptCutscene->nDrawTextureIndex[CGameAsyncGetRenderDataIndex(TRUE)]];

        ImageTextureFrame tImageTexFrame(snoTex, -1);
        BOOL fFlipV = TRUE;
        SpriteDrawDeferred(tImageTexFrame, snoShader, rectMain, rgbaColor, crgbaBlack, cflZCutscene, NULL, ptCutscene->snoAlphaMask, &rectTexCoordsAlphaMask, fFlipV, RENDERLAYER_INVALID, TRUE);

        if (ptCutscene->fFadeIn)
        {
            if (ptCutscene->flFadeTimeStart == 0.0f)
            {
                ptCutscene->flFadeTimeStart = CGameGetRealTime();
            }


            const float FADE_IN_TIME = 0.5f;
            const float FADE_IN_OFFSET = 2.0f;

            float flElapsed = CGameGetRealTime() - ptCutscene->flFadeTimeStart;
            if (flElapsed > FADE_IN_TIME + FADE_IN_OFFSET)
            {
                ptCutscene->fFadeIn = FALSE;
                ptCutscene->flFadeTimeStart = 0.0f;
            }

            float flAlpha = 1.0f;
            if (flElapsed > FADE_IN_OFFSET)
            {
                flAlpha = 1.0f - (flElapsed - FADE_IN_OFFSET) / FADE_IN_TIME;
            }
            flAlpha = Math::Pin(flAlpha, 0.0f, 1.0f);
            RGBAColor rgbaColorLoc(0, 0, 0, Math::FloatToInt(flAlpha * 255.0f));
            GfxRectDrawDeferred(rectMain, rgbaColorLoc, cflZCutscene, eCutsceneRenderLayer, TRUE);
        }


        if (ptCutscene->parSubTitles)
        {
            const float cflZSubtitles = 0.00001f;
            uint32 dwCurrentFrame = ptCutscene->nCurrentFrame;

            for (int32 i = ptCutscene->nSubTitleSearchStartPoint; i != (int32)ptCutscene->parSubTitles->size(); ++i)
            {
                if (dwCurrentFrame >= (*ptCutscene->parSubTitles)[i].dwStart &&
                    dwCurrentFrame <= (*ptCutscene->parSubTitles)[i].dwEnd)
                {



                    const uint32 dwFontSize = 19;
                    const int32 nMaxLines = 3;
                    TextRenderProperties tProps(GlobalSNOGet(GLOBALSNO_FONT_SCRIPT), dwFontSize, 1.0f, 1.0f);
                    float flHeightUIC = FontGetHeightUIC(tProps, nMaxLines);
                    UIText & tTxtSubtitle = UI_GetTextFromID(UITXT_CUTSCENE_SUBTITLE);

                    Rect2D rectViewportUIC;
                    CGameGetSafeViewportUIC(rectViewportUIC);


                    Rect2D rect(0.0f, 0.0f, rectViewportUIC.Width(), rectViewportUIC.Height());
                    tTxtSubtitle.SetWindowRect(rect);
                    tTxtSubtitle.SetText((*ptCutscene->parSubTitles)[i].szText, FALSE);

                    Vector2D vecUIC((rectViewportUIC.left + rectViewportUIC.right) * 0.5f, rectViewportUIC.bottom - flHeightUIC * 0.5f);
                    FontStringDrawCenteredUIC(tProps, tTxtSubtitle.GetText(), vecUIC, crgbaWhite, &crgbaBlack, cflZSubtitles, RENDERLAYER_INVALID, TRUE);

                    ptCutscene->nSubTitleSearchStartPoint = i;
                    break;
                }
            }
        }
    }












}




SNO CutsceneGetTexture(const CutsceneID idCutscene)
{
    Cutscene * ptCutscene = CutsceneTryToGet(idCutscene);
    if (ptCutscene)
        return ptCutscene->snoTex[ptCutscene->nUpdateTextureIndex];
    return SNO_NONE;
}







BOOL CutsceneIsPlaying(CutsceneID idCutscene)
{
    BOOL bPlaying = FALSE;
    CGame & tCGame = CGameGet();
    Cutscene * ptCutscene = tCGame.ptCutscenes->ElementTryToGet(idCutscene);
    if (ptCutscene)
        bPlaying = TRUE;
    return bPlaying;
}










BOOL CutsceneGetFrameSize(const CutsceneID idCutscene, IVector2D & tSize)
{
    CGame & tCGame = CGameGet();

    Cutscene * ptCutscene = tCGame.ptCutscenes->ElementTryToGet(idCutscene);
    if (!ptCutscene)
        return FALSE;

    tSize = IVector2D(ptCutscene->theoraInfo.pic_width, ptCutscene->theoraInfo.pic_height);
    return TRUE;
}









void CutsceneTryToCancel(const UIID &)
{
    CutsceneID idCutscene = CutsceneGetFullScreenCutscene();
    if (idCutscene != CUTSCENEID_INVALID)
    {
        if (CutsceneCanBeCancelled(idCutscene))
            CutsceneStop(idCutscene, FALSE);
    }
}







// DWARF inline instances support this real callee; no standalone original body is checkable.
static void sCommonCleanup(
    Cutscene * ptCutscene)
{


    CGame & tCGame = CGameGet();
    CutsceneGlobals * ptCutsceneGlobals = tCGame.ptCutsceneGlobals;

    if (ptCutsceneGlobals->idFullscreenCutscene == ptCutscene->id)
    {
        ptCutsceneGlobals->idFullscreenCutscene = CUTSCENEID_INVALID;
        SoundManager::SuppressSounds(0);
    }
    if (UI_TestID(UIDLG_CUTSCENE))
    {
        UI_GetWindowFromID(UIDLG_CUTSCENE).SetActive(FALSE);
    }

    CutsceneCallback pfnSavedOnFinishedCallback = ptCutscene->pfnOnFinishedCallback;
    tCGame.ptCutscenes->ElementDeleteByID(ptCutscene->id);




    if (pfnSavedOnFinishedCallback && SHELL_APP_STATE_GET() != APPSTATE_FINISHED)
    {
        pfnSavedOnFinishedCallback();
    }
}




static CutsceneID sCutscenePlay(
    const CutsceneParams & tParams)
{


    uint32 nAudioStreamIndex = tParams.m_nAudioTrack;
    uint32 nVideoStreamIndex = 0;

    CGame & tCGame = CGameGet();
    CutsceneGlobals * ptCutsceneGlobals = tCGame.ptCutsceneGlobals;

    CutsceneID idCutscene = tCGame.ptCutscenes->ElementNew();
    if (idCutscene == CUTSCENEID_INVALID)
        return CUTSCENEID_INVALID;



    // Original 0x862e4 sets w22=-1, stored to pauseId at source line 1637.
    Cutscene * ptCutscene = CutsceneGet(idCutscene);
    Blizzard::String::Copy(ptCutscene->szPath, tParams.m_szPath, sizeof(ptCutscene->szPath));
    Blizzard::String::Copy(ptCutscene->szSubtitlePath, tParams.m_szSubtitlePath, sizeof(ptCutscene->szSubtitlePath));
    ptCutscene->pauseId = -1;

    ptCutscene->fCanBeCancelled = tParams.m_fCanBeCancelled;
    ptCutscene->fShouldBeCancelled = FALSE;
    ptCutscene->bAudioMuted = tParams.m_bAudioMuted;
    ptCutscene->pfnOnFinishedCallback = tParams.m_pfnOnFinishedCallback;


    if (GameTesting::GetTestType() != TESTTYPE_NONE)
    {

        ptCutscene->fShouldBeCancelled = TRUE;
    }

    if (tParams.m_fFullscreen)
    {
        ptCutsceneGlobals->idFullscreenCutscene = idCutscene;

        UI_GetWindowFromID(UIDLG_CUTSCENE).SetActive(TRUE);
        UI_GetTextFromID(UITXT_CUTSCENE_SUBTITLE).SetActive(FALSE);


        SoundManager::SuppressSounds((uint32)-1);
    }

    for (int32 nTex = 0; nTex < Cutscene::TEXTURE_COUNT; ++nTex)
    {
        ptCutscene->snoTex[nTex] = SNO_NONE;
    }
    ptCutscene->snoAlphaMask = tParams.m_snoAlphaMask;
    if (ptCutscene->snoAlphaMask == SNO_NONE)
    {
        ptCutscene->snoAlphaMask = GlobalSNOGet(GLOBALSNO_OPAQUE_WHITE_TEXTURE);
    }
    ptCutscene->fFadeIn = tParams.m_fFadeIn;
    ptCutscene->flFadeTimeStart = 0.0f;


    if (!sPrepareCutscene(ptCutscene, tParams.m_szPath, nAudioStreamIndex, nVideoStreamIndex))
    {
        sCommonCleanup(ptCutscene);
        return CUTSCENEID_INVALID;
    }


    return idCutscene;
}



CRefString CutsceneGetActCutscenePath(Act eAct, BOOL fUseNoLogoVersion)
{
    CRefString sFilename;
    switch (eAct)
    {
    case ACT_1:
        sFilename = "Cinematic_1_Intro.ogv";
        break;
    case ACT_2:
        sFilename = "Cinematic_2_DOC.ogv";
        break;
    case ACT_3:
        sFilename = "Cinematic_3_BlackSoulstone.ogv";
        break;
    case ACT_4:
        sFilename = "Cinematic_4_HeavensGate.ogv";
        break;
    case ACT_5:
        sFilename = fUseNoLogoVersion ? "Cinematic_6_Intro_nl.ogv" : "Cinematic_6_Intro.ogv";
        break;
    }
    return sFilename;
}










CRefString CutsceneGetFinalCinematicPath(const BOOL fExpansionOutro, const PlayerClass ePlayerClass, const BOOL fIsMale)
{
    CRefString sFilename;
    if (fExpansionOutro)
    {
        sFilename = "Cinematic_7_Outro";
        CRefString sClassGenderExtension = "_";
        sClassGenderExtension += PlayerGetShortClassName(ePlayerClass);
        sClassGenderExtension += fIsMale ? "_M" : "_F";
        sFilename += (LPCSTR)sClassGenderExtension;
        sFilename += ".ogv";
    }
    else
    {
        sFilename = "Cinematic_5_Outro.ogv";
    }
    return sFilename;
}













static int32 sFindInterstitialAudioTrack(PlayerClass ePlayerClass, BOOL fIsMale)
{
    static const int32 nMaleTrack[] = { 2, 0, 8, 6, 4, 0, 0 };
    if (ePlayerClass == PLAYER_CLASS_CRUSADER || ePlayerClass == PLAYER_CLASS_NECROMANCER)
        return fIsMale ? 0 : 1;
    return nMaleTrack[ePlayerClass] + (fIsMale ? 0 : 1);
}


















static CRefString sFindInterstitialClassSuffix(PlayerClass ePlayerClass)
{
    if (ePlayerClass == PLAYER_CLASS_CRUSADER || ePlayerClass == PLAYER_CLASS_NECROMANCER)
    {
        CRefString sClassGenderExtension = "_";
        sClassGenderExtension += PlayerGetShortClassName(ePlayerClass);
        return sClassGenderExtension;
    }
    return "";
}






CRefString CutsceneGetInterstitialCutscenePath(int32 & nAudioTrack, const Act eAct, const int32 nIndex, const PlayerClass ePlayerClass, const BOOL fIsMale)
{
    nAudioTrack = 0;
    CRefString sFilename;
    sFilename = "Interstitial_";
    switch (eAct)
    {
    case ACT_1: sFilename += "Act1_"; break;
    case ACT_2: sFilename += "Act2_"; break;
    case ACT_3: sFilename += "Act3_"; break;
    case ACT_4: sFilename += "Act4_"; break;
    case ACT_5: sFilename += "Act5_"; break;
    }
    sFilename += CRefString().Format("%02d", nIndex);
    if (nIndex == 0 && eAct == ACT_1)
    {
        CRefString sClassGenderExtension = "_";
        sClassGenderExtension += PlayerGetShortClassName(ePlayerClass);
        sClassGenderExtension += fIsMale ? "_M" : "_F";
        sFilename += (LPCSTR)sClassGenderExtension;
        nAudioTrack = 0;
    }
    else if (eAct == ACT_5 && nIndex == 3)
    {
        nAudioTrack = fIsMale ? 0 : 1;
    }
    else
    {
        nAudioTrack = sFindInterstitialAudioTrack(ePlayerClass, fIsMale);
        sFilename += (LPCSTR)sFindInterstitialClassSuffix(ePlayerClass);
    }
    sFilename += ".ogv";
    return sFilename;
}






























inline CRefString CutsceneGetAnniversaryCinematicPath(const BOOL fIsMale)
{
    if (fIsMale)
        return "D1_Outro_Warrior.ogv";
    return "D1_Outro_Rogue.ogv";
}










void CutscenePlayActCutscene(const Act eAct, const BOOL fUseNoLogoVersion, CutsceneCallback pfnOnFinishedCallback, int32 nControllerIndex)
{
    CRefString sFilename;
    switch (eAct)
    {
    case ACT_1:
    case ACT_2:
    case ACT_3:
    case ACT_4:
    case ACT_5:
        sFilename = CutsceneGetActCutscenePath(eAct, fUseNoLogoVersion);
        break;

    default:
        return;
    }

    BOOL bPrimaryLocalPlayer = TRUE;
    Player * ptPlayer = LocalPlayerGetPrimary();
    if (ptPlayer && ptPlayer->nControllerIndex != nControllerIndex)
        bPrimaryLocalPlayer = FALSE;

    if (bPrimaryLocalPlayer)
    {
        CRefString sSubtitleFilename = CutsceneGetSubtitleName(sFilename, 0);
        CutsceneParams tParams(sFilename, sSubtitleFilename);
        tParams.m_pfnOnFinishedCallback = pfnOnFinishedCallback;
        CutsceneAddToQueue(tParams);
    }

    uint32 nDataLen;
    uint8 * pbPlayedCutscene = Prefs::GetPlayedCutScenes(nDataLen);
    int32 nActIndex = ActIndexFromActEnum(eAct);
    if (!(pbPlayedCutscene[nActIndex] & 1))
    {
        pbPlayedCutscene[nActIndex] |= 1;
        Console::GamerProfile::SavePrefs();
    }
}




























void CutscenePlayFinalCinematic(const BOOL fExpansion, const PlayerClass ePlayerClass, const BOOL fIsMale, CutsceneCallback pfnOnFinishedCallback)
{
    uint32 nDataLen;
    CRefString sFilename = CutsceneGetFinalCinematicPath(fExpansion, ePlayerClass, fIsMale);
    CRefString sSubtitleFilename = CutsceneGetSubtitleName(sFilename, 0);
    CutsceneParams tParams(sFilename, sSubtitleFilename);
    tParams.m_pfnOnFinishedCallback = pfnOnFinishedCallback;
    CutsceneAddToQueue(tParams);
    BOOL fSavePrefs = FALSE;
    uint8 * pbPlayedCutscene = Prefs::GetPlayedCutScenes(nDataLen);
    if (fExpansion) {
        if (!(pbPlayedCutscene[4] & 0x80)) { pbPlayedCutscene[4] |= 0x80; fSavePrefs = TRUE; }
    } else {
        if (!(pbPlayedCutscene[3] & 0x80)) { pbPlayedCutscene[3] |= 0x80; fSavePrefs = TRUE; }
    }
    if (fSavePrefs)
        Console::GamerProfile::SavePrefs();
}

void CutsceneUnlockFinalCinematic(const BOOL fExpansion)
{
    uint32 nDataLen;
    BOOL fSavePrefs = FALSE;
    uint8 * pbPlayedCutscene = Prefs::GetPlayedCutScenes(nDataLen);
    if (fExpansion)
    {
        if (!(pbPlayedCutscene[4] & 0x80))
        {
            pbPlayedCutscene[4] |= 0x80;
            fSavePrefs = TRUE;
        }
    }
    else
    {
        if (!(pbPlayedCutscene[3] & 0x80))
        {
            pbPlayedCutscene[3] |= 0x80;
            fSavePrefs = TRUE;
        }
    }

    if (fSavePrefs)
        Console::GamerProfile::SavePrefs();
}












void CutscenePlayFinalCinematicForLocalPlayer(const BOOL fExpansion, CutsceneCallback pfnOnFinishedCallback)
{
    BOOL bIsMale = FALSE;
    PlayerClass ePlayerClass = PLAYER_CLASS_NONE;
    Player * ptPlayer = LocalPlayerGetPrimary();
    if (ptPlayer)
    {
        ePlayerClass = PlayerGetPlayerClass(ptPlayer->idPlayerACD);
        bIsMale = PlayerIsMale(*ptPlayer);
    }
    CutscenePlayFinalCinematic(fExpansion, ePlayerClass, bIsMale, pfnOnFinishedCallback);
}







void CutscenePlayAnniversaryCinematic(const BOOL fIsMale, CutsceneCallback m_pfnOnFinishedCallback)
{
    CRefString sFilename = CutsceneGetAnniversaryCinematicPath(fIsMale);
    CRefString sSubtitleFilename;
    CutsceneParams tParams(sFilename, sSubtitleFilename);
    tParams.m_pfnOnFinishedCallback = m_pfnOnFinishedCallback;
    CutsceneAddToQueue(tParams);
}







void CutscenePlayInterstitialCutscene(const Act eAct, const int32 nIndex, const PlayerClass ePlayerClass, const BOOL fIsMale, int32 nControllerIndex, CutsceneCallback pfnOnFinishedCallback)
{
    if (ePlayerClass == PLAYER_CLASS_NONE)
        return;

    BOOL bPrimaryLocalPlayer = TRUE;
    Player * ptPlayer = LocalPlayerGetPrimary();
    if (ptPlayer && ptPlayer->nControllerIndex != nControllerIndex)
        bPrimaryLocalPlayer = FALSE;

    if (bPrimaryLocalPlayer)
    {
        int32 nAudioTrack = 0;
        CRefString sFilename = CutsceneGetInterstitialCutscenePath(nAudioTrack, eAct, nIndex, ePlayerClass, fIsMale);
        CRefString sSubtitleFilename = CutsceneGetSubtitleName(sFilename, nAudioTrack);
        CutsceneParams tParams(sFilename, sSubtitleFilename);
        tParams.m_nAudioTrack = nAudioTrack;
        tParams.m_fFadeIn = TRUE;
        tParams.m_pfnOnFinishedCallback = pfnOnFinishedCallback;
        CutsceneAddToQueue(tParams);
    }

    uint32 nDataLen;
    uint8 * pbPlayedCutscene = Prefs::GetPlayedCutScenes(nDataLen);
    int32 nActIndex = ActIndexFromActEnum(eAct);
    if (!(pbPlayedCutscene[nActIndex] & (1 << (nIndex + 1))))
    {
        pbPlayedCutscene[nActIndex] |= (1 << (nIndex + 1));
        Console::GamerProfile::SavePrefs();
    }
}



























void CutsceneAddToQueue(const CutsceneParams & tParams)
{
    CGame & tCGame = CGameGet();
    CutsceneGlobals * ptCutsceneGlobals = tCGame.ptCutsceneGlobals;
    ptCutsceneGlobals->tQueue.push_back(tParams);
}






CutsceneID CutscenePlay(
    const CutsceneParams & tParams)
{
    return sCutscenePlay(tParams);
}




CutsceneID CutsceneGetFullScreenCutscene()
{
    if (CGameIsValid())
    {
        CutsceneGlobals * ptCutsceneGlobals = CGameGet().ptCutsceneGlobals;
        if (ptCutsceneGlobals)
            return ptCutsceneGlobals->idFullscreenCutscene;
    }
    return CUTSCENEID_INVALID;
}











BOOL CutsceneIsPlaying()
{
    return CGameGet().ptCutscenes->GetTotalCount() != 0;
}





BOOL CutsceneCanBeCancelled(const CutsceneID idCutscene)
{
    Cutscene * ptCutscene = CutsceneTryToGet(idCutscene);
    if (ptCutscene)
        return ptCutscene->fCanBeCancelled;
    return FALSE;
}








void CutsceneMarkToBeStopped(const CutsceneID idCutscene)
{
    Cutscene * ptCutscene = CutsceneTryToGet(idCutscene);
    if (ptCutscene)
        ptCutscene->fShouldBeCancelled = TRUE;
}







void sTearDownOgg(Cutscene * ptCutscene)
{
    if (ptCutscene->fFoundTheoraStream && ptCutscene->fFoundVorbisStream)
    {
        th_decode_free(ptCutscene->ptheoraDecoder);
        vorbis_block_clear(&ptCutscene->vorbisBlock);
        vorbis_dsp_clear(&ptCutscene->vorbisDSP);
    }

    if (ptCutscene->fFoundTheoraStream)
    {
        th_comment_clear(&ptCutscene->theoraComment);
        th_info_clear(&ptCutscene->theoraInfo);
        ogg_stream_clear(&ptCutscene->theoraStream);
    }

    if (ptCutscene->fFoundVorbisStream)
    {
        vorbis_comment_clear(&ptCutscene->vorbisComment);
        vorbis_info_clear(&ptCutscene->vorbisInfo);
        ogg_stream_clear(&ptCutscene->vorbisStream);
    }

    ogg_sync_clear(&ptCutscene->oggSync);
}






void CutsceneStop(
    const CutsceneID idCutscene,
    const BOOL fStoppingAll)
{





    Cutscene * ptCutscene = CutsceneTryToGet(idCutscene);
    if (!ptCutscene)
        return;



    CGame & tCGame = CGameGet();
    CutsceneGlobals *& ptCutsceneGlobals = tCGame.ptCutsceneGlobals;

    if (ptCutsceneGlobals->idFullscreenCutscene == idCutscene &&
        !ptCutsceneGlobals->fDisposeForGameInProgress)
    {

        const float cflZCutscene = 0.0001f;
        uint32 dwScreenWidth;
        uint32 dwScreenHeight;
        GfxGetCurrentDimensions(dwScreenWidth, dwScreenHeight);

        Rect2D rect;
        RGBAColor rgbaColor(0, 0, 0, 255);
        rect.Set(0.0f, 0.0f, (float)dwScreenWidth, (float)dwScreenHeight);
        GfxRectDrawDeferred(rect, rgbaColor, cflZCutscene, RENDERLAYER_INVALID, FALSE);
    }







    CutsceneStopConsole(ptCutscene);

    if (ptCutsceneGlobals->idFullscreenCutscene == ptCutscene->id)
    {
        sShowCursor(TRUE);
    }
    sCutsceneGfxDispose(idCutscene);

    SoundManager::StopOVTStream();
    ptCutscene->tAsyncReader.Dispose();

    delete ptCutscene->parSubTitles; ptCutscene->parSubTitles = NULL;
    SigmaMemoryFree(ptCutscene->szSubTitlesFromFile, NULL); ptCutscene->szSubTitlesFromFile = NULL;





    SigmaMemoryFree(ptCutscene->tOVT.lpAudioBuffer, NULL);
    ptCutscene->tOVT.lpAudioBuffer = NULL;

    sTearDownOgg(ptCutscene);
    sCommonCleanup(ptCutscene);


    if (!fStoppingAll)
    {
        sUpdateQueue();
    }
}






























































































































static void sQueueOggPage(Cutscene * ptCutscene)
{
    if (ogg_stream_pagein(&ptCutscene->theoraStream, &ptCutscene->oggPage) != 0)
        ogg_stream_pagein(&ptCutscene->vorbisStream, &ptCutscene->oggPage);
}































































































// DWARF inline instances support this real callee; no standalone original body is checkable.
static OggPageState sOggGetPageBlocking(
    Cutscene * ptCutscene)
{
    for (;;)
    {
        OggPageState eState = sOggGetPage(ptCutscene);
        if (eState != PAGE_BUFFERING)
            return eState;


        Blizzard::Process::Sleep(1);
    }
}












static OggPageState sOggGetPage(
    Cutscene * ptCutscene)
{
    int32 ret;
    while ((ret = ogg_sync_pageout(&ptCutscene->oggSync, &ptCutscene->oggPage)) == 0)
    {

        char * buffer = ogg_sync_buffer(&ptCutscene->oggSync, 0x40000);
        uint32 dwNumRead = ptCutscene->tAsyncReader.GetChunk(buffer);
        if (dwNumRead == (uint32)-1)
        {
            ogg_sync_wrote(&ptCutscene->oggSync, 0);
            return PAGE_BUFFERING;
        }
        ogg_sync_wrote(&ptCutscene->oggSync, dwNumRead);
        if (dwNumRead == 0)
            return PAGE_FINISHED;
    }
    return ret > 0 ? PAGE_OK : PAGE_ERROR;
}














OggPageState CutsceneOggGetPacketBlocking(
    Cutscene * ptCutscene,
    ogg_stream_state * pOggStream,
    ogg_packet * pOggPacket)
{
    for (;;)
    {
        OggPageState eState = sOggGetPacket(ptCutscene, pOggStream, pOggPacket);
        if (eState != PAGE_BUFFERING)
            return eState;
        Blizzard::Process::Sleep(1);
    }
}














static OggPageState sOggGetPacket(Cutscene * ptCutscene, ogg_stream_state * pOggStream, ogg_packet * pOggPacket)
{
    int32 ret = ogg_stream_packetout(pOggStream, pOggPacket);
    while (ret == 0)
    {
        OggPageState ePageState = sOggGetPage(ptCutscene);
        if (ePageState != PAGE_OK)
            return ePageState;
        sQueueOggPage(ptCutscene);
        ret = ogg_stream_packetout(pOggStream, pOggPacket);
    }
    return PAGE_OK;
}



























float CutsceneAudioBuffered(Cutscene * ptCutscene)
{
    uint32 dwSamples;
    SoundOVT & tOVT = ptCutscene->tOVT;
    if (SoundManager::GetGlobalMute())
    {
        uint32 dwFakeAudioHead = tOVT.dwFakeAudioPos % tOVT.dwAudioBufferSize;
        tOVT.dwAudioHead = Math::Min(dwFakeAudioHead, tOVT.dwAudioTail);
    }

    if (tOVT.dwAudioHead > tOVT.dwAudioTail)
        dwSamples = tOVT.dwAudioBufferSize - tOVT.dwAudioHead + tOVT.dwAudioTail;
    else
        dwSamples = tOVT.dwAudioTail - tOVT.dwAudioHead;

    float flAudioBuffered = (float)dwSamples / ((float)tOVT.dwFrequency * (float)tOVT.dwChannels);
    return flAudioBuffered;
}




















static void sShowCursor(
    const BOOL fShow)
{
    if (!fShow)
    {
        BOOL fShowCursor;
        fShowCursor = !InputIsCursorInWindow();



        InputShowPlatformCursor(fShowCursor, FALSE);
    }
    else
    {
        InputShowPlatformCursor(TRUE, TRUE);
    }
}





























// DWARF inline instances support this real callee; no standalone original body is checkable.
static BOOL sInitCutscene(
    Cutscene * ptCutscene)
{
    const CutsceneInitParams & tParams = ptCutscene->m_tInitParams;

    uint32 nCurrVideoStream = 0;
    uint32 nCurrAudioStream = 0;

    ptCutscene->m_eCutsceneState = Cutscene::PRELOADING;

    for (;;)
    {


        if (sOggGetPageBlocking(ptCutscene) != PAGE_OK)
            return FALSE;




        if (!ogg_page_bos(&ptCutscene->oggPage))
        {
            sQueueOggPage(ptCutscene);
            break;
        }


        ogg_packet op;
        ogg_stream_state test;
        ogg_stream_init(&test, ogg_page_serialno(&ptCutscene->oggPage));
        ogg_stream_pagein(&test, &ptCutscene->oggPage);
        ogg_stream_packetout(&test, &op);

        BOOL bStreamIdentified = FALSE;
        BOOL bStreamUtilized = FALSE;


        if (!ptCutscene->fFoundTheoraStream)
        {
            th_info theoraInfo;
            th_comment theoraComment;
            th_setup_info * ptheoraSetupInfo = NULL;
            th_info_init(&theoraInfo);
            th_comment_init(&theoraComment);

            if (th_decode_headerin(&theoraInfo, &theoraComment, &ptheoraSetupInfo, &op) >= 0)
            {
                // Inference: original line-zero flag selects whether the Vorbis probe is skipped.
                bStreamIdentified = TRUE;
                if (nCurrVideoStream == tParams.m_nVideoStreamIndex)
                {
                    ptCutscene->fFoundTheoraStream = TRUE;
                    SigmaMemoryCopy(&ptCutscene->theoraStream, &test, sizeof(test));
                    SigmaMemoryCopy(&ptCutscene->theoraInfo, &theoraInfo, sizeof(theoraInfo));
                    SigmaMemoryCopy(&ptCutscene->theoraComment, &theoraComment, sizeof(theoraComment));
                    ptCutscene->ptheoraSetupInfo = ptheoraSetupInfo;
                    // Inference: line-zero flag suppresses stream cleanup when ownership is transferred.
                    bStreamUtilized = TRUE;
                }
                ++nCurrVideoStream;
            }

            if (!ptCutscene->fFoundTheoraStream)
            {
                th_comment_clear(&theoraComment);
                th_info_clear(&theoraInfo);
                if (ptheoraSetupInfo)
                {
                    th_setup_free(ptheoraSetupInfo);
                }
            }
        }

        if (!ptCutscene->fFoundVorbisStream && !bStreamIdentified)
        {
            vorbis_info vorbisInfo;
            vorbis_comment vorbisComment;
            vorbis_info_init(&vorbisInfo);
            vorbis_comment_init(&vorbisComment);

            if (vorbis_synthesis_headerin(&vorbisInfo, &vorbisComment, &op) >= 0)
            {


                if (nCurrAudioStream == tParams.m_nAudioStreamIndex)
                {
                    ptCutscene->fFoundVorbisStream = TRUE;
                    // Inference: original branch joins the utilized-stream path after transferring ownership.
                    bStreamUtilized = TRUE;
                    SigmaMemoryCopy(&ptCutscene->vorbisStream, &test, sizeof(test));
                    SigmaMemoryCopy(&ptCutscene->vorbisInfo, &vorbisInfo, sizeof(vorbisInfo));
                    SigmaMemoryCopy(&ptCutscene->vorbisComment, &vorbisComment, sizeof(vorbisComment));
                }

                ++nCurrAudioStream;
            }

            if (!ptCutscene->fFoundVorbisStream)
            {
                vorbis_comment_clear(&vorbisComment);
                vorbis_info_clear(&vorbisInfo);
            }
        }

        if (!bStreamUtilized)
        {

            ogg_stream_clear(&test);
        }
    }

    if (!ptCutscene->fFoundTheoraStream)
        return FALSE;



    if (!ptCutscene->fFoundVorbisStream)
        return FALSE;





    {
        ogg_packet op;
        OggPageState opstate;
        int ret;

        opstate = CutsceneOggGetPacketBlocking(ptCutscene, &ptCutscene->theoraStream, &op);

        ret = th_decode_headerin(&ptCutscene->theoraInfo, &ptCutscene->theoraComment, &ptCutscene->ptheoraSetupInfo, &op);


        opstate = CutsceneOggGetPacketBlocking(ptCutscene, &ptCutscene->theoraStream, &op);

        ret = th_decode_headerin(&ptCutscene->theoraInfo, &ptCutscene->theoraComment, &ptCutscene->ptheoraSetupInfo, &op);


        ptCutscene->ptheoraDecoder = th_decode_alloc(&ptCutscene->theoraInfo, ptCutscene->ptheoraSetupInfo);

        ptCutscene->flFrameRate = (float)ptCutscene->theoraInfo.fps_numerator / ptCutscene->theoraInfo.fps_denominator;
        th_setup_free(ptCutscene->ptheoraSetupInfo);
    }



    {
        ogg_packet op;
        OggPageState opstate;
        int ret;

        opstate = CutsceneOggGetPacketBlocking(ptCutscene, &ptCutscene->vorbisStream, &op);

        ret = vorbis_synthesis_headerin(&ptCutscene->vorbisInfo, &ptCutscene->vorbisComment, &op);


        opstate = CutsceneOggGetPacketBlocking(ptCutscene, &ptCutscene->vorbisStream, &op);

        ret = vorbis_synthesis_headerin(&ptCutscene->vorbisInfo, &ptCutscene->vorbisComment, &op);


        vorbis_synthesis_init(&ptCutscene->vorbisDSP, &ptCutscene->vorbisInfo);
        vorbis_block_init(&ptCutscene->vorbisDSP, &ptCutscene->vorbisBlock);



        ptCutscene->tOVT.dwAudioBufferSize = ptCutscene->vorbisInfo.rate * ptCutscene->vorbisInfo.channels * AUDIO_BUF_SIZE_SEC;
        size_t dwSize = ptCutscene->tOVT.dwAudioBufferSize * sizeof(int16);



        ptCutscene->tOVT.lpAudioBuffer = (int16 *)SigmaMemoryNew(dwSize, 4, NULL, FALSE);

        SigmaMemoryClear(ptCutscene->tOVT.lpAudioBuffer, dwSize);

        ptCutscene->tOVT.dwFrequency = ptCutscene->vorbisInfo.rate;
        ptCutscene->tOVT.dwChannels = ptCutscene->vorbisInfo.channels;
        ptCutscene->tOVT.dwFPS = (int32)ptCutscene->flFrameRate;
        ptCutscene->tOVT.dwStartTick = GetMachineTicks();

        switch (ptCutscene->vorbisInfo.channels)
        {
        case 6: ptCutscene->channelRemap = c_channelRemap6; break;
        case 8: ptCutscene->channelRemap = c_channelRemap8; break;
        default: ptCutscene->channelRemap = NULL; break;
        }

        if (Prefs::GetSoundPrefs().CinematicsSubtitlesEnabled())
        {
            sLoadSubTitle(ptCutscene, ptCutscene->szSubtitlePath);
        }
    }
    return TRUE;
}












// DWARF inline instances support this real callee; no standalone original body is checkable.
static BOOL sOpenCutscene(
    Cutscene * ptCutscene)
{




    if (!sInitCutscene(ptCutscene))
    {
        ptCutscene->tAsyncReader.Dispose();
        sTearDownOgg(ptCutscene);
        sCommonCleanup(ptCutscene);
        return FALSE;
    }



    sCutsceneGfxInit(ptCutscene->id);
    return TRUE;
}




CRefString CutsceneGetSubtitleName(const CRefString & sFilename, int32 nAudioTrack)
{
    char szSubtitleName[4096];
    if (Blizzard::String::Find(sFilename, "stitial") && !Blizzard::String::Find(sFilename, "ct1_00"))
    {
        if (Blizzard::String::Find(sFilename, "_Crusader.ogv") || Blizzard::String::Find(sFilename, "_Necromancer.ogv") || strcmp(sFilename, "Interstitial_Act5_03.ogv") == 0)
        {
            const char * szPostFix[2] = { "_M.sbt", "_F.sbt" };
            nAudioTrack = Math::Pin(nAudioTrack, 0, 1);
            strreplace(sFilename.str(), szSubtitleName, sizeof(szSubtitleName), ".ogv", szPostFix[nAudioTrack]);
        }
        else
        {
            const char * szPostFix[10] = {
                "_Barb_M.sbt", "_Barb_F.sbt",
                "_DHunter_M.sbt", "_DHunter_F.sbt",
                "_Monk_M.sbt", "_Monk_F.sbt",
                "_WitchDoctor_M.sbt", "_WitchDoctor_F.sbt",
                "_Wiz_M.sbt", "_Wiz_F.sbt",
            };
            nAudioTrack = Math::Pin(nAudioTrack, 0, 9);
            strreplace(sFilename.str(), szSubtitleName, sizeof(szSubtitleName), ".ogv", szPostFix[nAudioTrack]);
        }
    }
    else
    {
        strreplace(sFilename.str(), szSubtitleName, sizeof(szSubtitleName), ".ogv", ".sbt");
    }
    return CRefString(szSubtitleName);
}





















void CutsceneTryToCancelConsole(const UIID & uId, const int32 nControllerIndex)
{
    if (!UI_ValidateController(nControllerIndex))
        return;

    CutsceneTryToCancel(uId);
}





















extern "C" void *_ogg_malloc(int size) { return SigmaMemoryNew(size, 4, NULL, FALSE); }








extern "C" void *_ogg_calloc(int num, int size) { return SigmaMemoryNew(num * size, 4, NULL, TRUE); }














extern "C" void *_ogg_realloc(void *ptr, int size) { return SigmaMemoryResize(ptr, NULL, size, 4); }








extern "C" void _ogg_free(void *ptr) { SigmaMemoryFree(ptr, NULL); }
