// X/Client/Sound/SoundPlatformFModExAsync.cpp
// Included by SoundPlatformFModEx.cpp (the original compiles it as part of that file: its
// functions are attributed to SoundPlatformFModEx.cpp). Functions sit on their original lines.
// The build compiles every .cpp under src/X on its own; this one only has content when
// SoundPlatformFModEx.cpp includes it (the guard macro name is invented).
#ifdef SOUNDPLATFORMFMODEX_INCLUDES_ASYNC

// SoundPlatformFModExAsync.h:82 and :93; the header only forward-declares SoundInstance.
inline BOOL AsyncAudio::IsPending(const SoundInstance * ptSound)
{
    if (ptSound->fStitching)
    {
        return ptSound->tChannelState[ptSound->nCurrentChannel].nChannelIndex == ASYNCAUDIO_PENDING_CHANNEL ||
               ptSound->tChannelState[1 - ptSound->nCurrentChannel].nChannelIndex == ASYNCAUDIO_PENDING_CHANNEL;
    }
    return ptSound->tChannelState[ptSound->nCurrentChannel].nChannelIndex == ASYNCAUDIO_PENDING_CHANNEL;
}
inline BOOL AsyncAudio::IsCancelled(const SoundInstance * ptSound)
{
    if (ptSound->fStitching)
    {
        return ptSound->tChannelState[ptSound->nCurrentChannel].nChannelIndex == ASYNCAUDIO_CANCELLED_CHANNEL ||
               ptSound->tChannelState[1 - ptSound->nCurrentChannel].nChannelIndex == ASYNCAUDIO_CANCELLED_CHANNEL;
    }
    return ptSound->tChannelState[ptSound->nCurrentChannel].nChannelIndex == ASYNCAUDIO_CANCELLED_CHANNEL;
}


AsyncAudio::AsyncAudio()
{

    m_delayedSettings.SetHashTableSize_Tracked(256);
}

// NON_MATCHING: the two FMOD_OPENSTATE stack slots (the loop's and the inlined sReleaseFModSound's, sp+0x18/0x1c) are swapped
AsyncAudio::~AsyncAudio()
{



    uint32 nPendingReleaseCalls;
    uint nLoopCount = 0;
    do
    {
        nPendingReleaseCalls = 0;
        POS pos = m_queuedCalls.GetHeadPosition();
        while (pos)
        {
            POS posLast = pos;
            QueuedAPICall & call = m_queuedCalls.GetNext(pos);
            if (call.type == QueuedAPICallType_ReleaseFModSound)
            {
                FMOD_OPENSTATE eOpenState;
                FMOD_RESULT result = FMOD_Sound_GetOpenState(call.releaseFModSound_Params.ptFModSound, &eOpenState, NULL, NULL, NULL);
                if (result != FMOD_OK)
                {
                    if (nLoopCount < 5)
                    {
                        nPendingReleaseCalls++;
                        continue;
                    }
                }
                else if (eOpenState != FMOD_OPENSTATE_READY && eOpenState != FMOD_OPENSTATE_ERROR)
                {
                    nPendingReleaseCalls++;
                    continue;
                }



                sReleaseFModSound(call.releaseFModSound_Params.ptFModSound, TRUE);
                m_queuedCalls.RemoveAt(posLast);
            }
        }
        if (nPendingReleaseCalls == 0)
            break;
        Sleep(16);
        nLoopCount++;
    } while (TRUE);
}


inline void AsyncAudio::QueueSound(FModCachedSound * ptCachedSound, SoundInstance * ptSound, const SNO snoSoundBank, int32 nWhichChannel, int32 nSampleIndex)
{
    ptSound->tChannelState[nWhichChannel].nChannelIndex = ASYNCAUDIO_PENDING_CHANNEL;

    QueuedAPICall call;
    call.type = QueuedAPICallType_QueueSound;
    call.ptSound = ptSound;
    call.idSound = ptSound->id;
    call.queueSound_Params.ptCachedSound = ptCachedSound;
    call.queueSound_Params.nChannelIndex = nWhichChannel;
    call.queueSound_Params.snoSoundBank = snoSoundBank;
    call.queueSound_Params.nSampleIndex = nSampleIndex;

    QueueAPICall(call);
}






inline void AsyncAudio::QueueMusic(FModCachedSound * ptCachedSound, SoundInstance * ptSound, PlaylistEntry & tEntry, const uint32 dwStartSample, const float flVolume)
{
    ptSound->tChannelState[ptSound->nCurrentChannel].nChannelIndex = ASYNCAUDIO_PENDING_CHANNEL;

    QueuedMusicAPICall call;
    call.type = QueuedMusicAPICallType_QueueMusic;
    call.ptSound = ptSound;
    call.idSound = ptSound->id;
    call.queueMusic_Params.ptCachedSound = ptCachedSound;
    call.queueMusic_Params.nChannelIndex = ptSound->nCurrentChannel;
    call.queueMusic_Params.tEntry = tEntry;
    call.queueMusic_Params.dwStartSample = dwStartSample;
    call.queueMusic_Params.flVolume = flVolume;

    QueueMusicAPICall(call);
}






inline void AsyncAudio::PreloadSound(FModCachedSound * ptCachedSound)
{

    if (ptCachedSound->fPreloading)
    {
        sReleaseSound(ptCachedSound, FALSE);
        return;
    }
    QueuedAPICall call;
    call.type = QueuedAPICallType_PreloadSound;
    call.ptSound = NULL;
    call.preloadSound_Params.ptCachedSound = ptCachedSound;
    ptCachedSound->fPreloading = TRUE;
    QueueAPICall(call);
}

void AsyncAudio::Cancel(SoundInstance * ptSound)
{

    POS pos = m_queuedCalls.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        QueuedAPICall & call = m_queuedCalls.GetNext(pos);
        if (call.ptSound == ptSound)
        {

            if (call.type == QueuedAPICallType_QueueSound)
            {
                sReleaseSound(call.queue_Params.ptCachedSound, FALSE);
            }
            m_queuedCalls.RemoveAt(posLast);
        }
    }



    pos = m_queuedMusicCalls.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        QueuedMusicAPICall & call = m_queuedMusicCalls.GetNext(pos);
        if (call.ptSound == ptSound)
        {

            if (call.type == QueuedMusicAPICallType_QueueMusic)
            {
                sReleaseSound(call.queue_Params.ptCachedSound, FALSE);
            }
            m_queuedMusicCalls.RemoveAt(posLast);
        }
    }
}

inline void AsyncAudio::SetPosition(SoundInstance * ptSound, const Vector3D & wp)
{
    DelayedSettings & settings = GetDelayedSetting(ptSound);
    settings.dwFlags |= (1 << DELAYED_SET_POSITION_BIT);

    settings.posParam.wp = wp;
}


inline void AsyncAudio::SetVelocity(SoundInstance * ptSound, const Vector3D & wvVel)
{
    DelayedSettings & settings = GetDelayedSetting(ptSound);
    settings.dwFlags |= (1 << DELAYED_SET_VELOCITY_BIT);

    settings.velParam.wvVel = wvVel;
}


inline void AsyncAudio::SetPitch(SoundInstance * ptSound, const float flPitch)
{
    DelayedSettings & settings = GetDelayedSetting(ptSound);
    settings.dwFlags |= (1 << DELAYED_SET_PITCH_BIT);
    settings.pitchParam.flPitch = flPitch;
}



inline void AsyncAudio::SetPaused(SoundInstance * ptSound, BOOL fPaused)
{
    DelayedSettings & settings = GetDelayedSetting(ptSound);
    settings.dwFlags |= (1 << DELAYED_SET_PAUSED_BIT);
    settings.pausedParam.fPaused = fPaused;
}



inline void AsyncAudio::SetVolume(SoundInstance * ptSound, const float flVolume)
{
    DelayedSettings & settings = GetDelayedSetting(ptSound);
    settings.dwFlags |= (1 << DELAYED_SET_VOLUME_BIT);
    settings.volParam.flVolume = flVolume;
}



inline void AsyncAudio::SetOcclusion(SoundInstance * ptSound, const float flOcclusion)
{
    DelayedSettings & settings = GetDelayedSetting(ptSound);
    settings.dwFlags |= (1 << DELAYED_SET_OCCLUSION_BIT);
    settings.occParam.flOcclusion = flOcclusion;
}



inline void AsyncAudio::SetStreamPosition(SoundInstance * ptSound, const uint32 dwPos)
{
    DelayedSettings & settings = GetDelayedSetting(ptSound);
    settings.dwFlags |= (1 << DELAYED_SET_STREAM_POSITION_BIT);
    settings.streamPosParam.dwPos = dwPos;
}



inline void AsyncAudio::ReleaseFModSound(FMOD_SOUND * ptFModSound)
{
    QueuedAPICall call;
    call.type = QueuedAPICallType_ReleaseFModSound;
    call.ptSound = NULL;
    call.releaseFModSound_Params.ptFModSound = ptFModSound;
    call.releaseFModSound_Params.nFrame = sgtAppGlobals.dwFrameCounter;

    QueueAPICall(call);
}

AsyncAudio::DelayedSettings & AsyncAudio::GetDelayedSetting(SoundInstance * ptSound)
{

    DelayedSettings * ptSettings = m_delayedSettings.LookupPtr(ptSound);
    if (!ptSettings)
    {
        DelayedSettings tSettings;
        SigmaMemoryClear(&tSettings, sizeof(tSettings));

        m_delayedSettings.SetAt_Tracked(ptSound, tSettings);
        ptSettings = m_delayedSettings.LookupPtr(ptSound);
    }
    return *ptSettings;
}

inline void AsyncAudio::QueueAPICall(const QueuedAPICall & call)
{
    m_queuedCalls.AddTail_Tracked(call);
}


inline void AsyncAudio::QueueMusicAPICall(const QueuedMusicAPICall & call)
{


    m_queuedMusicCalls.AddTail_Tracked(call);
}
























































































void AsyncAudio::ProcessPendingSounds()

{
    POS pos = m_queuedCalls.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        QueuedAPICall & call = m_queuedCalls.GetNext(pos);

        if ((call.ptSound != NULL) && IsPending(call.ptSound))
        {



            if (call.type == QueuedAPICallType_QueueSound)
            {
                FModCachedSound * ptCachedSound = call.queueSound_Params.ptCachedSound;

                if (ptCachedSound->fPending)
                {
                    FMOD_OPENSTATE eOpenState;
                    FMOD_RESULT result = FMOD_Sound_GetOpenState(ptCachedSound->ptFModSound, &eOpenState, NULL, NULL, NULL);
                    if (eOpenState == FMOD_OPENSTATE_ERROR)
                    {

                        if (result == FMOD_ERR_MEMORY)
                        {

                            if (ptCachedSound->fAlreadyReloaded)
                            {




                                call.ptSound->tChannelState[call.queueSound_Params.nChannelIndex].nChannelIndex = ASYNCAUDIO_CANCELLED_CHANNEL;


                                sReleaseSound(ptCachedSound, TRUE);

                                m_queuedCalls.RemoveAt(posLast);
                                continue;
                            }

                            call.queueSound_Params.ptCachedSound = sReloadSound(ptCachedSound, call.ptSound, call.queueSound_Params.snoSoundBank, call.queueSound_Params.nSampleIndex);

                            if (call.queueSound_Params.ptCachedSound)
                            {
                                call.queueSound_Params.ptCachedSound->fAlreadyReloaded = TRUE;
                            }
                            else
                            {


                                call.ptSound->tChannelState[call.queueSound_Params.nChannelIndex].nChannelIndex = ASYNCAUDIO_CANCELLED_CHANNEL;
                                m_queuedCalls.RemoveAt(posLast);
                            }
                        }
                        else
                        {


                            call.ptSound->tChannelState[call.queueSound_Params.nChannelIndex].nChannelIndex = ASYNCAUDIO_CANCELLED_CHANNEL;
                            sReleaseSound(ptCachedSound, TRUE);
                            m_queuedCalls.RemoveAt(posLast);
                        }
                        continue;
                    }





                    if (eOpenState == FMOD_OPENSTATE_READY || eOpenState == FMOD_OPENSTATE_PLAYING)
                    {
                        if (ptCachedSound->fIsFSB)
                        {
                            FMOD_SOUND * ptFModSubSound;
                            FMOD_RESULT result = FMOD_Sound_GetSubSound(ptCachedSound->ptFModSound, 0, &ptFModSubSound);
                            sCheckResult(result, "FMOD_Sound_GetSubSound", "");

                            ptCachedSound->ptFModParentSound = ptCachedSound->ptFModSound;
                            ptCachedSound->ptFModSound = ptFModSubSound;
                        }

                        FMOD_RESULT result = FMOD_Sound_SetUserData(ptCachedSound->ptFModSound, ptCachedSound);
                        sCheckResult(result, "FMOD_Sound_SetUserData", "");

                        ptCachedSound->fPending = FALSE;
                    }
                }


                if (!ptCachedSound->fPending)
                {
                    call.ptSound->tChannelState[call.queueSound_Params.nChannelIndex].nChannelIndex = ASYNCAUDIO_READY_CHANNEL;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }




        if (call.ptSound)
        {


            if (IsCancelled(call.ptSound))
            {
                m_queuedCalls.RemoveAt(posLast);
                continue;
            }

            switch (call.type)
            {
                case QueuedAPICallType_QueueSound:
                {
                    ScopedSoundBankDefinitionLockR pdefSoundBank(call.queueSound_Params.snoSoundBank);
                    SoundSample & tSample = pdefSoundBank->ptSoundSamples[call.queueSound_Params.nSampleIndex];

                    if (!sPlaySound(call.queueSound_Params.ptCachedSound, call.ptSound, call.queueSound_Params.snoSoundBank, call.queueSound_Params.nChannelIndex, tSample))
                    {
                        call.ptSound->tChannelState[call.queueSound_Params.nChannelIndex].nChannelIndex = ASYNCAUDIO_CANCELLED_CHANNEL;
                    }
                    else
                    {

                        DelayedSettings * ptSettings = m_delayedSettings.LookupPtr(call.ptSound);
                        if (ptSettings)
                        {
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_POSITION_BIT))
                            {
                                SoundPlatform::SetPosition(call.ptSound, ptSettings->posParam.wp);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_VELOCITY_BIT))
                            {
                                SoundPlatform::SetVelocity(call.ptSound, ptSettings->velParam.wvVel);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_PITCH_BIT))
                            {
                                SoundPlatform::SetPitch(call.ptSound, ptSettings->pitchParam.flPitch);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_PAUSED_BIT))
                            {
                                SoundPlatform::SetPaused(call.ptSound, ptSettings->pausedParam.fPaused);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_VOLUME_BIT))
                            {
                                SoundPlatform::SetVolume(call.ptSound, ptSettings->volParam.flVolume);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_OCCLUSION_BIT))
                            {
                                SoundPlatform::SetOcclusion(call.ptSound, ptSettings->occParam.flOcclusion);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_STREAM_POSITION_BIT))
                            {
                                SoundPlatform::SetStreamPosition(call.ptSound, ptSettings->streamPosParam.dwPos);
                            }
                            m_delayedSettings.RemoveAt(call.ptSound);
                        }
                    }
                }
                break; // Inference: unhandled queue types take the common removal path.
                default:
                    break;
            }
        }
        else
        {



            switch (call.type)
            {
                case QueuedAPICallType_PreloadSound:
                {
                    FModCachedSound * ptCachedSound = call.preloadSound_Params.ptCachedSound;
                    if (ptCachedSound->fPending)
                    {
                        FMOD_OPENSTATE eOpenState;
                        FMOD_Sound_GetOpenState(ptCachedSound->ptFModSound, &eOpenState, NULL, NULL, NULL);
                        if (eOpenState == FMOD_OPENSTATE_ERROR)
                        {
                            ptCachedSound->fPreloading = FALSE;
                            sReleaseSound(ptCachedSound, TRUE);
                            m_queuedCalls.RemoveAt(posLast);
                            continue;
                        }
                        if (eOpenState == FMOD_OPENSTATE_READY || eOpenState == FMOD_OPENSTATE_PLAYING)
                        {
                            if (ptCachedSound->fIsFSB)
                            {
                                FMOD_SOUND * ptFModSubSound;
                                FMOD_RESULT result = FMOD_Sound_GetSubSound(ptCachedSound->ptFModSound, 0, &ptFModSubSound);
                                sCheckResult(result, "FMOD_Sound_GetSubSound", "");

                                ptCachedSound->ptFModParentSound = ptCachedSound->ptFModSound;
                                ptCachedSound->ptFModSound = ptFModSubSound;
                            }

                            FMOD_RESULT result = FMOD_Sound_SetUserData(ptCachedSound->ptFModSound, ptCachedSound);
                            sCheckResult(result, "FMOD_Sound_SetUserData", "");

                            ptCachedSound->fPending = FALSE;

                        }
                        else
                        {
                            continue;
                        }
                    }
                    ptCachedSound->fPreloading = FALSE;
                    ptCachedSound->nRefCount--;
                }
                break;




                case QueuedAPICallType_ReleaseFModSound:
                {
                    FMOD_OPENSTATE eOpenState;
                    if (FMOD_Sound_GetOpenState(call.releaseFModSound_Params.ptFModSound, &eOpenState, NULL, NULL, NULL) == FMOD_OK)
                    {
                        if (eOpenState != FMOD_OPENSTATE_READY && eOpenState != FMOD_OPENSTATE_ERROR)
                            continue;
                    }
                    else
                    {


                        if (call.releaseFModSound_Params.nFrame + 5 < sgtAppGlobals.dwFrameCounter)
                            continue;
                    }



                    sReleaseFModSound(call.releaseFModSound_Params.ptFModSound, TRUE);
                }
                break; // Inference: unhandled queue types take the common removal path.
                default:
                    break;
            }
        }
        m_queuedCalls.RemoveAt(posLast);
    }




    pos = m_queuedMusicCalls.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        QueuedMusicAPICall & call = m_queuedMusicCalls.GetNext(pos);

        if ((call.ptSound != NULL) && IsPending(call.ptSound))
        {


            if (call.type == QueuedMusicAPICallType_QueueMusic)
            {

                FModCachedSound * ptCachedSound = call.queueMusic_Params.ptCachedSound;
                if (ptCachedSound->fPending)
                {
                    FMOD_OPENSTATE eOpenState;
                    FMOD_RESULT result = FMOD_Sound_GetOpenState(ptCachedSound->ptFModSound, &eOpenState, NULL, NULL, NULL);
                    if (eOpenState == FMOD_OPENSTATE_ERROR)
                    {

                        if (result == FMOD_ERR_MEMORY)
                        {

                            if (ptCachedSound->fAlreadyReloaded)
                            {




                                call.ptSound->tChannelState[call.queueMusic_Params.nChannelIndex].nChannelIndex = ASYNCAUDIO_CANCELLED_CHANNEL;


                                sReleaseSound(ptCachedSound, TRUE);

                                m_queuedMusicCalls.RemoveAt(posLast);
                                continue;
                            }

                            call.queueMusic_Params.ptCachedSound = sReloadMusic(ptCachedSound, call.queueMusic_Params.tEntry, call.queueMusic_Params.flVolume);

                            if (call.queueMusic_Params.ptCachedSound)
                            {
                                call.queueMusic_Params.ptCachedSound->fAlreadyReloaded = TRUE;
                            }
                            else
                            {


                                call.ptSound->tChannelState[call.queueMusic_Params.nChannelIndex].nChannelIndex = ASYNCAUDIO_CANCELLED_CHANNEL;
                                m_queuedMusicCalls.RemoveAt(posLast);
                            }
                        }
                        else
                        {


                            call.ptSound->tChannelState[call.queueMusic_Params.nChannelIndex].nChannelIndex = ASYNCAUDIO_CANCELLED_CHANNEL;
                            sReleaseSound(ptCachedSound, TRUE);
                            m_queuedMusicCalls.RemoveAt(posLast);
                        }
                        continue;
                    }





                    if (eOpenState == FMOD_OPENSTATE_READY || eOpenState == FMOD_OPENSTATE_PLAYING)
                    {
                        if (ptCachedSound->fIsFSB)
                        {
                            FMOD_SOUND * ptFModSubSound;
                            FMOD_RESULT result = FMOD_Sound_GetSubSound(ptCachedSound->ptFModSound, 0, &ptFModSubSound);
                            sCheckResult(result, "FMOD_Sound_GetSubSound", "");

                            ptCachedSound->ptFModParentSound = ptCachedSound->ptFModSound;
                            ptCachedSound->ptFModSound = ptFModSubSound;
                        }

                        FMOD_RESULT result = FMOD_Sound_SetUserData(ptCachedSound->ptFModSound, ptCachedSound);
                        sCheckResult(result, "FMOD_Sound_SetUserData", "");

                        ptCachedSound->fPending = FALSE;
                    }
                }


                if (!ptCachedSound->fPending)
                {
                    call.ptSound->tChannelState[call.queueMusic_Params.nChannelIndex].nChannelIndex = ASYNCAUDIO_READY_CHANNEL;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }




        if (call.ptSound)
        {


            if (IsCancelled(call.ptSound))
            {
                m_queuedMusicCalls.RemoveAt(posLast);
                continue;
            }
            switch (call.type)
            {
                case QueuedMusicAPICallType_QueueMusic:
                {
                    if (!sPlayMusic(call.queueMusic_Params.ptCachedSound, call.ptSound, call.queueMusic_Params.tEntry, call.queueMusic_Params.dwStartSample, call.queueMusic_Params.flVolume))
                    {
                        call.ptSound->tChannelState[call.queueMusic_Params.nChannelIndex].nChannelIndex = ASYNCAUDIO_CANCELLED_CHANNEL;
                    }
                    else
                    {

                        DelayedSettings * ptSettings = m_delayedSettings.LookupPtr(call.ptSound);
                        if (ptSettings)
                        {
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_POSITION_BIT))
                            {
                                SoundPlatform::SetPosition(call.ptSound, ptSettings->posParam.wp);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_VELOCITY_BIT))
                            {
                                SoundPlatform::SetVelocity(call.ptSound, ptSettings->velParam.wvVel);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_PITCH_BIT))
                            {
                                SoundPlatform::SetPitch(call.ptSound, ptSettings->pitchParam.flPitch);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_PAUSED_BIT))
                            {
                                SoundPlatform::SetPaused(call.ptSound, ptSettings->pausedParam.fPaused);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_VOLUME_BIT))
                            {
                                SoundPlatform::SetVolume(call.ptSound, ptSettings->volParam.flVolume);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_OCCLUSION_BIT))
                            {
                                SoundPlatform::SetOcclusion(call.ptSound, ptSettings->occParam.flOcclusion);
                            }
                            if (ptSettings->dwFlags & (1 << DELAYED_SET_STREAM_POSITION_BIT))
                            {
                                SoundPlatform::SetStreamPosition(call.ptSound, ptSettings->streamPosParam.dwPos);
                            }
                            m_delayedSettings.RemoveAt(call.ptSound);
                        }
                    }
                }
                break; // Inference: unhandled queue types take the common removal path.
                default:
                    break;
            }
            m_queuedMusicCalls.RemoveAt(posLast);
        }
    }

}

#endif  // SOUNDPLATFORMFMODEX_INCLUDES_ASYNC
