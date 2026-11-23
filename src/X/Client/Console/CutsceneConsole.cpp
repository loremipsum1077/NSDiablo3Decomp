#include "../CutsceneInternal.h"
#include "../Gfx/GfxVB.h"
#include "../../Common/ThreadGlobals.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../Definitions/TextureDefinition.h"
#include "../../Sigma/RGBAColor.h"
#include "../Prefetch.h"
#include <string.h>
// Declarations recovered from DWARF; bodies owned by their original translation units.
void ConfigureCutsceneDecodeThread(Blizzard::Thread::Thread);
OggPageState CutsceneOggGetPacketBlocking(Cutscene*, ogg_stream_state*, ogg_packet*);
float CutsceneAudioBuffered(Cutscene*);
void CutsceneDecodeAudioConsole(Cutscene*);
void GfxTextureLock(const TexData&, uint32, void*&, uint32&, uint32);
void GfxTextureUnlock(const TexData&, uint32);
namespace SNOManager { void SetHasBlockedThisFrame(); }
// Original .data at0x1043da0 contains priority3 (PRIORITY_NORMAL).
#line 33 "X/Client/Console/CutsceneConsole.cpp"
uint32 s_nDecodePriority = 3;
namespace CutsceneConsole {
#line 43 "X/Client/Console/CutsceneConsole.cpp"
static Cutscene* s_ptCutscene;

static Blizzard::Lock::REvent s_tStartWork;
static Blizzard::Lock::REvent s_tWorkFinished;
static volatile BOOL s_bTerminateThread;
static Blizzard::Thread::Thread s_tDecodeThread;
static uint sDecodeThread(void* parms);
}
using namespace CutsceneConsole;
#line 56 "X/Client/Console/CutsceneConsole.cpp"
void CutsceneConsoleInitializeForApp()
{
    BOOL fSuccess = Blizzard::Thread::Create(sDecodeThread, 0, &s_tDecodeThread, "CutsceneDecodeThread", 1, 0x8000);
    Blizzard::Thread::SetPriority(s_tDecodeThread, static_cast<Blizzard::Thread::EThreadPriority>(s_nDecodePriority));
    ConfigureCutsceneDecodeThread(s_tDecodeThread);
    Blizzard::Thread::Resume(s_tDecodeThread);
}
#line 71 "X/Client/Console/CutsceneConsole.cpp"
void CutsceneConsoleDisposeForApp()
{
    s_bTerminateThread = TRUE;
    s_tStartWork.Set();
    Blizzard::Thread::WaitForever(s_tDecodeThread);
    Blizzard::Thread::Close(s_tDecodeThread);
}
#line 82 "X/Client/Console/CutsceneConsole.cpp"
void CutsceneDecodeAudioConsole(
    Cutscene * ptCutscene)
{
    if (ptCutscene->bAudioFinished)
    {
        return;
    }



    const float flMinBufferedAudio = 0.5f;
    while (CutsceneAudioBuffered(ptCutscene) < flMinBufferedAudio)
    {
        ogg_packet op;

        OggPageState eState = CutsceneOggGetPacketBlocking(ptCutscene, &ptCutscene->vorbisStream, &op);

        if (eState != PAGE_OK)
        {
            ptCutscene->bAudioFinished = TRUE;
            return;
        }

        if (vorbis_synthesis(&ptCutscene->vorbisBlock, &op) == 0)
        {
            vorbis_synthesis_blockin(&ptCutscene->vorbisDSP, &ptCutscene->vorbisBlock);

            float ** pcm;
            const int nSamples = vorbis_synthesis_pcmout(&ptCutscene->vorbisDSP, &pcm);


            const uint32 * channelRemap = ptCutscene->channelRemap;
            const int32 nChannels = ptCutscene->vorbisInfo.channels;
            const uint32 dwBufferSize = ptCutscene->tOVT.dwAudioBufferSize;
            int16 * pDst = ptCutscene->tOVT.lpAudioBuffer;
            uint32 dwHead = ptCutscene->tOVT.dwAudioHead;
            uint32 dwTail = ptCutscene->tOVT.dwAudioTail;
            uint32 dwOldTail = dwTail;


            dwHead = ((dwHead + nChannels - 1) / nChannels) * nChannels;






            uint32 dwSamplesStart = 0;
            uint32 dwSamplesEnd = nSamples;
            while (dwSamplesStart < dwSamplesEnd)
            {
                uint32 dwMaxAvailable;

                if (dwHead > dwTail)
                    dwMaxAvailable = dwHead - dwTail;
                else
                    dwMaxAvailable = dwBufferSize - dwTail;

                uint32 dwSamplesCurrentEnd = dwSamplesEnd;

                if (dwSamplesCurrentEnd - dwSamplesStart > dwMaxAvailable / nChannels)
                {
                    dwSamplesCurrentEnd = dwSamplesStart + dwMaxAvailable / nChannels;
                }



                if (!ptCutscene->bAudioMuted)
                {
                    for (int j = 0; j < nChannels; ++j)
                    {
                        uint32 r = channelRemap ? channelRemap[j] : j;

                        uint32 dwTailCurrent = dwTail + j;



                        uint32 i = dwSamplesStart;
                        if (dwSamplesCurrentEnd >= 16)
                        {
                            for (; i <= dwSamplesCurrentEnd - 16; i += 16)
                            {
                                int vals[16];

                                float * pSrc = pcm[r] + i;
                                int * pValDst = vals;
                                int * pValDstEnd = vals + 16;

                                while (pValDst != pValDstEnd)
                                {
                                    *pValDst++ = static_cast<int>(*pSrc++ * 32767.0f);
                                }



                                for (int k = 0; k < 16; ++k)
                                {
                                    int val = Math::Pin(vals[k], -32768, 32767);
                                    pDst[dwTailCurrent] = val;
                                    dwTailCurrent += nChannels;
                                }
                            }
                        }

                        for (; i < dwSamplesCurrentEnd; ++i)
                        {
                            int val = static_cast<int>(pcm[r][i] * 32767.0f);

                            val = Math::Pin(val, -32768, 32767);
                            pDst[dwTailCurrent] = val;
                            dwTailCurrent += nChannels;
                        }
                    }
                }

                dwTail += (dwSamplesCurrentEnd - dwSamplesStart) * nChannels;



                if (dwTail == dwHead)
                {
                    dwTail = dwOldTail;
                }

                if (dwTail >= dwBufferSize)
                {
                    dwTail = 0;
                }
                dwSamplesStart = dwSamplesCurrentEnd;
            }

            ptCutscene->tOVT.dwAudioTail = dwTail;

            vorbis_synthesis_read(&ptCutscene->vorbisDSP, nSamples);
        }
    }
}
#line 222 "X/Client/Console/CutsceneConsole.cpp"
static void sCopyCrCbY()
{
    th_ycbcr_buffer ycbcr;
    th_decode_ycbcr_out(s_ptCutscene->ptheoraDecoder, ycbcr);
    if (s_ptCutscene->theoraInfo.pixel_fmt == TH_PF_420) {
        int32 nFrame = s_ptCutscene->nFrameToWriteTo;
        s_ptCutscene->nFrameToWriteTo = (nFrame + 1) % Cutscene::FRAME_COUNT;
        TheoraFrame& tFrame = s_ptCutscene->theoraFrames[nFrame];
        while (!tFrame.m_tFrameUsed.Wait(100)) CutsceneDecodeAudioConsole(s_ptCutscene);
        tFrame.m_nFrameIndex = s_ptCutscene->nDecodingVideoFrame;
        int32 nHeight = s_ptCutscene->theoraInfo.frame_height;
        int32 nWidth = s_ptCutscene->theoraInfo.frame_width;
        for (int32 row = 0; row < nHeight; ++row) {
            uint8* pSrc = ycbcr[0].data + row * ycbcr[0].stride;
            uint8* pDest = tFrame.m_pYCbCr[0] + row * nWidth;
            memcpy(pDest, pSrc, nWidth);
        }
        for (int32 row = 0; row < nHeight / 2; ++row) {
            uint8* pSrc = ycbcr[1].data + row * ycbcr[1].stride;
            uint8* pDest = tFrame.m_pYCbCr[1] + row * nWidth;
            memcpy(pDest, pSrc, nWidth);
        }
        for (int32 row = 0; row < nHeight / 2; ++row) {
            uint8* pSrc = ycbcr[2].data + row * ycbcr[2].stride;
            uint8* pDest = tFrame.m_pYCbCr[2] + row * nWidth;
            memcpy(pDest, pSrc, nWidth);
        }
        tFrame.m_eState = TheoraFrame::STATE_BUFFER_FILLED;
        tFrame.m_tFrameDecoded.Set();
    }
}
#line 289 "X/Client/Console/CutsceneConsole.cpp"
static BOOL sDecodeVideoConsole()
{

    ogg_packet op;
    OggPageState eState = CutsceneOggGetPacketBlocking(s_ptCutscene, &s_ptCutscene->theoraStream, &op);

    if (eState != PAGE_OK)
    {
        return FALSE;
    }

    int ret = th_decode_packetin(s_ptCutscene->ptheoraDecoder, &op, &s_ptCutscene->videobuf_granulepos);
    s_ptCutscene->nDecodingVideoFrame = th_granule_frame(s_ptCutscene->ptheoraDecoder, s_ptCutscene->videobuf_granulepos);










    if (ret == 0)
    {
        sCopyCrCbY();
    }

    ++s_ptCutscene->nDecodingVideoFrame;

    return TRUE;
}

#line 326 "X/Client/Console/CutsceneConsole.cpp"
void sDupeLastFrame()
{
    sCopyCrCbY();
}
namespace CutsceneConsole {
#line 333 "X/Client/Console/CutsceneConsole.cpp"
static uint sDecodeThread(
    void * parms)
{

    ThreadGlobals::InitializeForThread();
    ThreadGlobals::Get().m_nThreadIndex = THREADINDEX_CUTSCENE_ASYNC_DECODE;

    for (;;)
    {
        s_tStartWork.WaitForever();
        if (s_bTerminateThread)
        {
            s_tWorkFinished.Set();
            break;
        }

        BOOL bVideoFinished = FALSE;



        while (!CutsceneConsole::s_ptCutscene->bStopDecoding)
        {
            if (!bVideoFinished)
            {
                if (!sDecodeVideoConsole())
                {
                    sDupeLastFrame();
                    bVideoFinished = TRUE;
                }
            }

            CutsceneDecodeAudioConsole(CutsceneConsole::s_ptCutscene);

            if (CutsceneConsole::s_ptCutscene->bAudioFinished && bVideoFinished)
            {
                break;
            }
        }


        CutsceneConsole::s_ptCutscene->bDecodingFinished = TRUE;
        for (int32 i = 0; i < Cutscene::FRAME_COUNT; ++i)
        {
            CutsceneConsole::s_ptCutscene->theoraFrames[i].m_tFrameDecoded.Set();
        }

        s_tWorkFinished.Set();
    }

    ThreadGlobals::DisposeForThread();

    return 1;
}
}
#line 390 "X/Client/Console/CutsceneConsole.cpp"
static void sUpdateTextureConsole(
    Cutscene * ptCutscene,
    int32 nUpdateTex)
{
    ptCutscene->nUpdateTextureIndex = (ptCutscene->nUpdateTextureIndex + 1) % Cutscene::TEXTURE_COUNT;
    SNO snoTex = ptCutscene->snoTex[ptCutscene->nUpdateTextureIndex];

    const TextureDefinition * pdefTexture = static_cast<const TextureDefinition *>(gptGroupDefTexture->LockR(snoTex, FALSE));
    void * ptDest = NULL;
    uint32 dwDestPitch;
    GfxTextureLock(pdefTexture->tTexData, 0, ptDest, dwDestPitch, 0);

    if (ptDest)
    {
        if (ptCutscene->theoraInfo.pixel_fmt == TH_PF_420)
        {


            int32 frameWidth = ptCutscene->theoraInfo.frame_width;

            int32 picWidth = ptCutscene->theoraInfo.pic_width;
            int32 picHeight = ptCutscene->theoraInfo.pic_height;

            TheoraFrame & tf = ptCutscene->theoraFrames[nUpdateTex];

            for (int32 row = 0; row < picHeight; ++row)
            {
                const uint8 * pSrcY = tf.m_pYCbCr[0] + row * frameWidth;
                const uint8 * pSrcCr = tf.m_pYCbCr[1] + (row / 2) * frameWidth;
                const uint8 * pSrcCb = tf.m_pYCbCr[2] + (row / 2) * frameWidth;
                RGBAColor * pDst = reinterpret_cast<RGBAColor *>(static_cast<uint8 *>(ptDest) + (picHeight - row - 1) * dwDestPitch);
                for (int32 col = 0; col < picWidth; ++col)
                {
                    pDst[col].r = pSrcY[col];
                    pDst[col].g = pSrcCb[col / 2];
                    pDst[col].b = pSrcCr[col / 2];
                }
            }






            tf.m_eState = TheoraFrame::STATE_EMPTY;
            tf.m_tFrameUsed.Set();
        }







        GfxTextureUnlock(pdefTexture->tTexData, 0);
    }

    gptGroupDefTexture->UnlockR((void *&)pdefTexture);
}

#line 453 "X/Client/Console/CutsceneConsole.cpp"
BOOL CutsceneDecodeFrameConsole(
    Cutscene * ptCutscene)
{
    int32 nUpdateTex = -1;

    int nInterationCount = 0;

    while (ptCutscene->nCurrentVideoFrame <= ptCutscene->nCurrentFrame)
    {
        int32 nFrame = ptCutscene->nFrameToReadFrom;
        TheoraFrame & tFrame = ptCutscene->theoraFrames[nFrame];

        if (ptCutscene->bDecodingFinished &&
            tFrame.m_eState == TheoraFrame::STATE_EMPTY)
        {
            return FALSE;
        }


        if (tFrame.m_eState != TheoraFrame::STATE_READY_FOR_RENDER)
        {




            while (!tFrame.m_tFrameDecoded.Wait(1))
            {
                SNOManager::SetHasBlockedThisFrame();
                Prefetch::Update(FALSE);
            }










            if (ptCutscene->bDecodingFinished &&
                tFrame.m_eState == TheoraFrame::STATE_EMPTY)
            {
                return FALSE;
            }

            tFrame.m_eState = TheoraFrame::STATE_READY_FOR_RENDER;
        }


        if (tFrame.m_nFrameIndex <= ptCutscene->nCurrentVideoFrame)
        {
            if (nUpdateTex != -1)
            {
                ptCutscene->theoraFrames[nUpdateTex].m_eState = TheoraFrame::STATE_EMPTY;
                ptCutscene->theoraFrames[nUpdateTex].m_tFrameUsed.Set();
            }



            nUpdateTex = nFrame;

            ptCutscene->nFrameToReadFrom = (ptCutscene->nFrameToReadFrom + 1) % Cutscene::FRAME_COUNT;


            ++nInterationCount;

            if (nInterationCount == 24)
            {
                ++ptCutscene->nCurrentVideoFrame;
                break;
            }
        }

        ++ptCutscene->nCurrentVideoFrame;
    }

    if (nUpdateTex != -1)
    {
        sUpdateTextureConsole(ptCutscene, nUpdateTex);
    }

    return TRUE;
}
#line 541 "X/Client/Console/CutsceneConsole.cpp"
void CutsceneStopConsole(Cutscene* ptCutscene)
{
    if (s_ptCutscene) {
        ptCutscene->bStopDecoding = TRUE;
        for (int32 i = 0; i < Cutscene::FRAME_COUNT; ++i) {
            ptCutscene->theoraFrames[i].m_eState = TheoraFrame::STATE_EMPTY;
            ptCutscene->theoraFrames[i].m_tFrameUsed.Set();
        }
        s_tWorkFinished.WaitForever();
        s_ptCutscene = 0;
        for (int32 i = 0; i < Cutscene::FRAME_COUNT; ++i) {
            TheoraFrame& frame = ptCutscene->theoraFrames[i];
            SigmaMemoryFree(frame.m_pYCbCr[0], 0);
            frame.m_pYCbCr[0] = 0;
            SigmaMemoryFree(frame.m_pYCbCr[1], 0);
            frame.m_pYCbCr[1] = 0;
            SigmaMemoryFree(frame.m_pYCbCr[2], 0);
            frame.m_pYCbCr[2] = 0;
        }
    }
}
#line 574 "X/Client/Console/CutsceneConsole.cpp"
void CutsceneInitConsole(Cutscene* ptCutscene)
{
    ptCutscene->nFrameToWriteTo = 0;
    ptCutscene->nFrameToReadFrom = 0;
    ptCutscene->bDecodingFinished = FALSE;
    ptCutscene->nDecodingVideoFrame = 0;
    for (int32 i = 0; i < Cutscene::FRAME_COUNT; ++i) {
        uint32 nPixelCount = ptCutscene->theoraInfo.frame_width * ptCutscene->theoraInfo.frame_height;
        TheoraFrame& frame = ptCutscene->theoraFrames[i];
        frame.m_pYCbCr[0] = static_cast<uint8*>(SigmaMemoryNew(nPixelCount, 16, 0, FALSE));
        frame.m_pYCbCr[1] = static_cast<uint8*>(SigmaMemoryNew(nPixelCount / 2, 16, 0, FALSE));
        frame.m_pYCbCr[2] = static_cast<uint8*>(SigmaMemoryNew(nPixelCount / 2, 16, 0, FALSE));
        frame.m_eState = TheoraFrame::STATE_EMPTY;
        frame.m_tFrameUsed.Set();
    }
    s_ptCutscene = ptCutscene;
    ptCutscene->bStopDecoding = FALSE;
    s_tStartWork.Set();
}
