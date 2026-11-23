// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/CutsceneInternal.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/OggVorbis/libogg/include/ogg/ogg.h"
#include "../../Contrib/Contrib/OggVorbis/libogg/include/ogg/os_types.h"
#include "../../Contrib/Contrib/OggVorbis/libtheora/include/theora/codec.h"
#include "../../Contrib/Contrib/OggVorbis/libtheora/include/theora/theoradec.h"
#include "../../Contrib/Contrib/OggVorbis/libvorbis/include/vorbis/codec.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Cutscene.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "Sound/Sound.h"
#include "../Sigma/Files/XFiles.h"
#include "../Sigma/RefString.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/Unicode.h"

namespace FileIO { class File; }

// from X/Common/OnlineService/BackgroundDownload.h (not in src yet)
namespace BackgroundDownload {
typedef int32 PauseID;
}

struct Cutscene;

enum {
    CHUNK_BUFFER_SIZE = 262144,
    AUDIO_BUF_SIZE_SEC = 1,
    VIDEO_DELAY = 0,
    MIN_BANDWIDTH_FOR_HIGH_BITRATE_MOVIE = 655360,
};

struct SubTitle {  // size 0x10
    uint32 dwStart;  // 0x00
    uint32 dwEnd;  // 0x04
    char * szText;  // 0x08
};
static_assert(sizeof(SubTitle) == 0x10, "SubTitle");

struct TheoraFrame {  // size 0x38
    enum State {
        STATE_EMPTY = 0,
        STATE_BUFFER_FILLED = 1,
        STATE_READY_FOR_RENDER = 2,
    };
    uint8 * m_pYCbCr[3];  // 0x00
    int32 m_nFrameIndex;  // 0x18
    volatile TheoraFrame::State m_eState;  // 0x1c
    Blizzard::Lock::REvent m_tFrameDecoded;  // 0x20
    Blizzard::Lock::REvent m_tFrameUsed;  // 0x2c
};
static_assert(sizeof(TheoraFrame) == 0x38, "TheoraFrame");

struct CutsceneInitParams {  // size 0x30
    Cutscene * m_ptCutscene;  // 0x00
    CRefString m_strFinalName;  // 0x08
    uint32 m_nVideoStreamIndex;  // 0x20
    uint32 m_nAudioStreamIndex;  // 0x24
    BOOL m_bStreamingMode;  // 0x28
};
static_assert(sizeof(CutsceneInitParams) == 0x30, "CutsceneInitParams");

struct ChunkBuffer {  // size 0x10
    uint8 * lpBuffer;  // 0x00
    uint32 dwNumBytes;  // 0x08
    uint32 nSequenceIndex;  // 0x0c
    void Initialize();
    void Dispose();
};
static_assert(sizeof(ChunkBuffer) == 0x10, "ChunkBuffer");

inline void ChunkBuffer::Initialize()  // line 95
{
    lpBuffer = (uint8 *)SigmaMemoryNew(0x40000, 4, NULL, FALSE);
    dwNumBytes = 0;
    nSequenceIndex = (uint32)-1;
}

inline void ChunkBuffer::Dispose()  // line 103
{
    SigmaMemoryFree(lpBuffer, NULL);
    lpBuffer = NULL;
}

class AsyncReader {  // size 0x10a8
public:
    BOOL Initialize(LPCSTR szFilePath);
    void Dispose();
    uint32 GetChunk(char * lpDest);
    BOOL IsTerminated() const;
    uint32 GetBufferFreeCount() const;
    BOOL HasData() const;
    enum {
        BUFFER_COUNT = 6,
    };
private:
    void InitializeStateForBuffer();
    BOOL ReadChunk(ChunkBuffer & tChunkBuffer);
    static void sSetupIOThread(AsyncReader & tAsyncReader);
    static void sReleaseIOThread(AsyncReader & tAsyncReader);
    Blizzard::Lock::Event m_chunkProcessed;  // 0x00
    volatile uint32 m_bStopReading;  // 0x0c
    static volatile uint32 s_bTerminateThread;
    static Blizzard::Thread::Thread s_tThread;
    static Blizzard::Lock::REvent s_tStartWork;
    static Blizzard::Lock::REvent s_tWorkFinished;
    static AsyncReader * s_ptAsyncReader;
    FileIO::File * m_ptFile;  // 0x10
    FileReference m_tFileRef;  // 0x18
    uint32 m_dwFileSize;  // 0x1028
    uint32 m_dwFileOffset;  // 0x102c
    ChunkBuffer m_tChunkBuffers[6];  // 0x1030
    uint32 m_nBufferUsedStart;  // 0x1090
    uint32 m_nBufferFreeStart;  // 0x1094
    int32 m_nBufferFreeCount;  // 0x1098
    uint32 m_nLastSequenceIndexRead;  // 0x109c
    uint32 m_nLastSequenceIndexReturned;  // 0x10a0
    static uint sThreadFunction(void * parms);
    // friends are not in the DWARF; the app-level init/dispose touch the thread statics
    friend void CutscenesInitializeForApp();
    friend void CutscenesDisposeForApp();
    static void SetIOThreadOwner(AsyncReader * ptAsyncReader);
};
static_assert(sizeof(AsyncReader) == 0x10a8, "AsyncReader");

struct Cutscene {  // size 0x18b8
    enum State {
        INITALIZING = 0,
        WAITING_FOR_THE_LOADING_SCREEN_TO_END = 1,
        PRELOADING = 2,
        PLAYBACK = 3,
    };
    enum {
        FRAME_COUNT = 2,
        TEXTURE_COUNT = 3,
    };
    int32 nFrameToWriteTo;  // 0x00
    int32 nFrameToReadFrom;  // 0x04
    BOOL bDecodingFinished;  // 0x08
    int32 nDecodingVideoFrame;  // 0x0c
    TheoraFrame theoraFrames[2];  // 0x10
    volatile BOOL bStopDecoding;  // 0x80
    BOOL bAudioFinished;  // 0x84
    Cutscene::State m_eCutsceneState;  // 0x88
    CutsceneInitParams m_tInitParams;  // 0x90
    CutsceneID id;  // 0xc0
    CHAR szPath[128];  // 0xc4
    CHAR szSubtitlePath[128];  // 0x144
    SNO snoTex[3];  // 0x1c4
    SNO snoAlphaMask;  // 0x1d0
    int32 nUpdateTextureIndex;  // 0x1d4
    int32 nDrawTextureIndex[2];  // 0x1d8
    BOOL fCanBeCancelled;  // 0x1e0
    BOOL fShouldBeCancelled;  // 0x1e4
    BOOL bAudioMuted;  // 0x1e8
    CutsceneCallback pfnOnFinishedCallback;  // 0x1f0
    uint8 bSceneOpacity;  // 0x1f8
    uint8 bBorderOpacity;  // 0x1f9
    BOOL fFadeIn;  // 0x1fc
    float flFadeTimeStart;  // 0x200
    int32 nCurrentFrame;  // 0x204
    SoundOVT tOVT;  // 0x208
    LPSTR szSubTitlesFromFile;  // 0x240
    blz::vector<SubTitle, blz::allocator<SubTitle> > * parSubTitles;  // 0x248
    uint32 nSubTitleSearchStartPoint;  // 0x250
    AsyncReader tAsyncReader;  // 0x258
    BOOL fFoundTheoraStream;  // 0x1300
    BOOL fFoundVorbisStream;  // 0x1304
    ogg_sync_state oggSync;  // 0x1308
    ogg_page oggPage;  // 0x1328
    ogg_stream_state vorbisStream;  // 0x1348
    ogg_stream_state theoraStream;  // 0x14e0
    th_info theoraInfo;  // 0x1678
    th_comment theoraComment;  // 0x16b8
    th_dec_ctx * ptheoraDecoder;  // 0x16d8
    th_setup_info * ptheoraSetupInfo;  // 0x16e0
    float flFrameRate;  // 0x16e8
    ogg_int64_t videobuf_granulepos;  // 0x16f0
    int32 nCurrentVideoFrame;  // 0x16f8
    vorbis_info vorbisInfo;  // 0x1700
    vorbis_comment vorbisComment;  // 0x1738
    vorbis_dsp_state vorbisDSP;  // 0x1758
    vorbis_block vorbisBlock;  // 0x17e8
    const uint32 * channelRemap;  // 0x18a8
    BackgroundDownload::PauseID pauseId;  // 0x18b0
};
static_assert(sizeof(Cutscene) == 0x18b8, "Cutscene");

enum OggPageState {
    PAGE_OK = 0,
    PAGE_BUFFERING = 1,
    PAGE_FINISHED = 2,
    PAGE_ERROR = 3,
};


// Functions defined in this file (bodies are not in the DWARF):
//   line    60  TheoraFrame::TheoraFrame()  [inlined into 1 functions]
//   line    60  TheoraFrame::~TheoraFrame()  [inlined into 1 functions]
//   line    80  CutsceneInitParams::CutsceneInitParams()  [inlined into 1 functions]
//   line    80  CutsceneInitParams::~CutsceneInitParams()  [inlined into 1 functions]
//   line    95  void ChunkBuffer::Initialize()  [inlined into 1 functions]
//   line   103  void ChunkBuffer::Dispose()  [inlined into 1 functions]
//   line   110  AsyncReader::AsyncReader()  [inlined into 1 functions]
//   line   176  Cutscene::Cutscene()  [inlined into 1 functions]
//   line   176  Cutscene::~Cutscene()  [inlined into 1 functions]
