// Original file: X/Client/Battlenet/Subsystems/ToastManager.cpp
#include "X/Client/Battlenet/Subsystems/ToastManager.h"

#include "X/Client/CGame.h"
#include "X/Client/UserInterface/UI.h"
#include "X/Client/UserInterface/Controls/UIWindowControl.h"
#include "X/Common/Shell/CmdLine.h"













































extern const ToastId INVALID_TOAST_ID = -1;



ToastManagerBase::ToastManagerBase(uint32 nMaxVisible, float flToastDuration, BOOL fKeepToastOnHover)



    : m_nMaxVisible(nMaxVisible)
    , m_flDefaultToastDuration(flToastDuration)
    , m_nToastIdGenerator(0)
    , m_fKeepToastOnHover(fKeepToastOnHover)
    , m_fInDestructor(FALSE)
    , m_fIsEnabled(TRUE)
{
    m_currentToast.resize(m_nMaxVisible);
    m_toastTimeElapsed.resize(m_nMaxVisible);
    for (uint32 i = 0; i < m_nMaxVisible; ++i)
    {
        m_currentToast[i] = NULL;
        m_toastTimeElapsed[i] = 0.0f;
    }
}



ToastManagerBase::~ToastManagerBase()
{
    m_fInDestructor = TRUE;
    ClearToasts();
}










































void ToastManagerBase::SendToast(CRefString sMessage,
                                 const ImageTextureFrame & tIconTexture,
                                 ToastPriority ePriority,
                                 ToastManagerBase::ToastOnClickCallback cb,
                                 const float flToastDurationTime)

{







    Toast * pNewToast = new Toast();
    pNewToast->sMessage = sMessage;
    pNewToast->tIcon = tIconTexture;
    pNewToast->cbOnClick = cb;
    pNewToast->flToastDuration = flToastDurationTime == 0.0f ? GetDefaultToastDuration() : flToastDurationTime;

    AddToastToQueue(pNewToast, ePriority);
}



ToastId ToastManagerBase::SendIndefiniteToast(CRefString sMessage,
                                              const ImageTextureFrame & tIconTexture,
                                              ToastPriority ePriority,
                                              ToastManagerBase::ToastOnClickCallback cb)

{







    Toast * pNewToast = new Toast();
    pNewToast->sMessage = sMessage;
    pNewToast->tIcon = tIconTexture;
    pNewToast->cbOnClick = cb;
    pNewToast->flToastDuration = -1.0f;
    pNewToast->nId = GetNextToastId();

    AddToastToQueue(pNewToast, ePriority);
    return pNewToast->nId;
}



void ToastManagerBase::UpdateToastDisplay()

{
    float flDeltaTime = CGameGetRealDeltaTime();
    for (uint32 i = 0; i < m_nMaxVisible; ++i)
    {
        m_toastTimeElapsed[i] += flDeltaTime;

        if (m_currentToast[i] != NULL)
        {


            if (!UI_TestID(m_currentToast[i]->idToastWin))
            {
                m_currentToast[i]->idToastWin = CreateToastVisual(i, m_currentToast[i]);
            }










            if (m_toastTimeElapsed[i] >= m_currentToast[i]->flToastDuration)
            {
                RemoveToast(i);
            }
            else
            {
                UpdateToastVisual(i, m_currentToast[i], m_toastTimeElapsed[i]);
            }
        }
    }
}



void ToastManagerBase::ProcessToasts()

{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return;
    }


    UpdateToastDisplay();
    if (!m_fIsEnabled)
    {
        return;
    }

    int32 nFreeToastSlot = GetFreeToastSlot();
    for (int32 i = TOAST_NUM_PRIORITIES - 1; i >= 0 && nFreeToastSlot != -1; --i)
    {
        while (m_toastQueue[i].Count() != 0 && nFreeToastSlot != -1)
        {
            Toast * ptToast = m_toastQueue[i].GetHead();
            ProcessToast(nFreeToastSlot, ptToast);
            m_toastQueue[i].RemoveHead();
            nFreeToastSlot = GetFreeToastSlot();
        }
    }
}



void ToastManagerBase::ClearToasts()

{
    for (uint32 i = 0; i < m_currentToast.size(); ++i)
    {
        if (m_currentToast[i] != NULL)
        {
            RemoveToast(i);
        }
    }

    for (int32 i = 0; i < TOAST_NUM_PRIORITIES; ++i)
    {
        POS pos = m_toastQueue[i].GetHeadPosition();
        while (pos != NULL)
        {
            Toast * pToast = m_toastQueue[i].GetNext(pos);
            ::delete pToast;
        }
        m_toastQueue[i].RemoveAll();
    }
}



void ToastManagerBase::OutroToast(ToastId idToast)

{
    if (idToast == INVALID_TOAST_ID)
        return;



    for (uint32 i = 0; i < m_currentToast.size(); ++i)
    {
        Toast * ptToast = m_currentToast[i];
        if (ptToast != NULL && ptToast->nId == idToast)
        {
            OutroToastVisual(ptToast, m_toastTimeElapsed[i]);
            return;
        }
    }
    ClearToast(idToast);
}


void ToastManagerBase::ClearToast(ToastId idToast)

{
    if (idToast == INVALID_TOAST_ID)
    {
        return;
    }

    for (uint32 i = 0; i < m_currentToast.size(); ++i)
    {
        if (m_currentToast[i] != NULL && m_currentToast[i]->nId == idToast)
        {
            RemoveToast(i);
            return;
        }
    }

    for (int32 i = 0; i < TOAST_NUM_PRIORITIES; ++i)
    {
        POS pos = m_toastQueue[i].GetHeadPosition();
        while (pos != NULL)
        {
            POS posSaved = pos;
            Toast * pToast = m_toastQueue[i].GetNext(pos);
            if (pToast->nId == idToast)
            {
                ::delete pToast;
                m_toastQueue[i].RemoveAt(posSaved);
                return;
            }
        }
    }
}











void ToastManagerBase::AddToastToQueue(ToastManagerBase::Toast * ptToast,
                                       ToastPriority ePriority)

{
    m_toastQueue[ePriority].AddTail_Tracked(ptToast);
}



void ToastManagerBase::ProcessToast(int32 nToastSlot,
                                    ToastManagerBase::Toast * ptToast)

{
    ptToast->idToastWin = CreateToastVisual(nToastSlot, ptToast);

    m_toastTimeElapsed[nToastSlot] = 0.0f;
    m_currentToast[nToastSlot] = ptToast;
    m_toastVisualsToSlots.SetAt_Tracked(ptToast->idToastWin, nToastSlot);

    if (UI_TestID(ptToast->idToastWin))
    {
        UI_GetWindowFromID(ptToast->idToastWin).SetActive(TRUE);
    }
}



void ToastManagerBase::RemoveToast(int32 nToastSlot)



{
    Toast * ptToast = m_currentToast[nToastSlot];
    UIID idToastWin = ptToast->idToastWin;
    m_toastVisualsToSlots.RemoveAt(idToastWin);
    if (!m_fInDestructor)
    {
        DisposeToastVisual(ptToast);
    }
    ::delete m_currentToast[nToastSlot]; m_currentToast[nToastSlot] = NULL;
}


int32 ToastManagerBase::GetFreeToastSlot() const

{
    for (uint32 i = 0; i < m_currentToast.size(); ++i)
    {
        if (m_currentToast[i] == NULL)
        {
            return i;
        }
    }
    return -1;
}
