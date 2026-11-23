// Original file: X/Client/RMessage.cpp

#include "RMessage.h"
#include "CGame.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/MemoryPools.h"
#include "../Sigma/SigmaTypes.h"

XMemoryAllocatorInterface * AppGetCurrentMemoryInterface();

enum {
    MAX_RMESSAGES = 1024,
};

enum RMessageID {
    RMESSAGEID_INVALID = -1,
};

struct RMessageDelayed {  // size 0x130
    RMessageID id;  // 0x00
    BOOL fMarkedAsDeleted;  // 0x04
    RMessage msg;  // 0x08
};
static_assert(sizeof(RMessageDelayed) == 0x130, "RMessageDelayed");

struct RMessageGlobals {  // size 0x8
    TDataArray<RMessageDelayed, RMessageID> * ptMsgs;  // 0x00
};
static_assert(sizeof(RMessageGlobals) == 0x8, "RMessageGlobals");

static RMessageID sRMsgDelayedNew();
static void sRMsgDelayedDelete(RMessageDelayed * ptMsg);
static RMessageDelayed * sRMsgDelayedGet(const RMessageID idMsg);

void RMessagesInitialize()
{
    RMessageGlobals *& ptGlobals = CGameGet().ptRMessageGlobals;
    ptGlobals = (RMessageGlobals *)SigmaMemoryNew(sizeof(RMessageGlobals), 4, NULL, TRUE);
    ptGlobals->ptMsgs = TDataArrayCreateTracked<RMessageDelayed, RMessageID>("Delayed RMessages", AppGetCurrentMemoryInterface(), MAX_RMESSAGES);
}

void RMessagesDispose()
{
    RMessageGlobals *& ptGlobals = CGameGet().ptRMessageGlobals;
    TDataArrayDispose(ptGlobals->ptMsgs);
    SigmaMemoryFree(ptGlobals, NULL);
    ptGlobals = NULL;
}

BOOL RMessage::Initialize(const TriggerEvent & _tTriggerEvent, const float _flImpulse, const ParentType _eParentType, const DataID _idParent, const float _flExpireTime, const BOOL _bForRepeat, const BOOL _bRemote, const float _flStartOffset)
{
    uint32 dwDelay = _tTriggerEvent.tConditions.GetDelay(_bForRepeat);
    if (dwDelay == (uint32)-1)
        return FALSE;

    flDelay = (float)dwDelay * (1.0f / 60.0f);
    flStartOffset = _flStartOffset;

    SigmaMemoryCopy(&tTriggerEvent, &_tTriggerEvent, sizeof(TriggerEvent));
    flImpulse = _flImpulse;
    nParentType = _eParentType;
    idParent = _idParent;
    flExpireTime = _flExpireTime;
    bRemote = _bRemote;

    for (int32 i = 0; i < 2; i++)
        tHardpoints[i].tHash = 0u;

    return TRUE;
}

void RMessageList::Initialize(const DataID _idOwner, RMessageHandlerFunc _pfnHandleMsg)
{
    idOwner = _idOwner;
    pfnHandleMsg = _pfnHandleMsg;
    tList.SetPool(MemoryPoolsGetDATAIDListPool());
}

void RMessageList::Dispose()
{
    DeleteAll();
}

void RMessageList::DeleteAll()
{
    while (!tList.IsEmpty())
    {
        RMessageID idRMsg = (RMessageID)tList.GetHead();
        RMessageDelayed * ptRMsg = sRMsgDelayedGet(idRMsg);

        sRMsgDelayedDelete(ptRMsg);
        tList.RemoveHead();
    }
}

void RMessageList::DeleteforEventID(uint32 dwID)
{
    POS pos = tList.GetHeadPosition();
    while (pos)
    {
        RMessageID idRMsg = (RMessageID)tList.GetNext(pos);
        RMessageDelayed * ptRMsg = sRMsgDelayedGet(idRMsg);

        if (ptRMsg->msg.tTriggerEvent.eType == TRIGGEREDEVENTTYPE_ADD_OBJECT &&
            ptRMsg->msg.tTriggerEvent.dwID == dwID)
        {
            ptRMsg->fMarkedAsDeleted = TRUE;
        }
    }
}

void RMessageList::DeleteforFlags(uint32 dwFlags)
{
    POS pos = tList.GetHeadPosition();
    while (pos)
    {
        POS posOld = pos;
        RMessageID idRMsg = (RMessageID)tList.GetNext(pos);
        RMessageDelayed * ptRMsg = sRMsgDelayedGet(idRMsg);

        if (ptRMsg->msg.tTriggerEvent.eType == TRIGGEREDEVENTTYPE_ADD_OBJECT &&
            (ptRMsg->msg.tTriggerEvent.dwTriggerFlags & dwFlags))
        {
            sRMsgDelayedDelete(ptRMsg);
            tList.RemoveAt(posOld);
        }
    }
}

void RMessageList::Update(float flDeltaTime)
{
    POS pos = tList.GetHeadPosition();
    while (pos)
    {
        POS posOld = pos;
        RMessageID idRMsg = (RMessageID)tList.GetNext(pos);
        RMessageDelayed * ptRMsg = sRMsgDelayedGet(idRMsg);

        if (ptRMsg->fMarkedAsDeleted)
        {
            sRMsgDelayedDelete(ptRMsg);
            tList.RemoveAt(posOld);
            continue;
        }

        ptRMsg->msg.flDelay = Math::Max(ptRMsg->msg.flDelay - flDeltaTime, 0.0f);
        if (ptRMsg->msg.flDelay == 0.0f)
        {
            pfnHandleMsg(idOwner, ptRMsg->msg);

            sRMsgDelayedDelete(ptRMsg);
            tList.RemoveAt(posOld);
        }
    }
}

void RMessageList::SendMessage(const ParentType nParentType, const DataID idParent, const BOOL bForRepeat, const TriggerEvent & tTriggerEvent, const float flImpulse, const BOOL bRemote)
{
    RMessage msg;
    if (!msg.Initialize(tTriggerEvent, flImpulse, nParentType, idParent, -1.0f, bForRepeat, bRemote, 0.0f))
        return;

    if (msg.flDelay == 0.0f)
        pfnHandleMsg(idOwner, msg);
    else
        QueueDelayedMessage(msg);
}

void RMessageList::QueueDelayedMessage(RMessage & msg)
{
    RMessageID idRMsg = sRMsgDelayedNew();
    if (idRMsg == RMESSAGEID_INVALID)
        return;

    tList.AddTail_Tracked(idRMsg);

    RMessageDelayed * ptRMsgDelayed = sRMsgDelayedGet(idRMsg);
    ptRMsgDelayed->fMarkedAsDeleted = FALSE;
    ptRMsgDelayed->msg = msg;
}

static RMessageID sRMsgDelayedNew()
{
    RMessageGlobals * ptGlobals = CGameGet().ptRMessageGlobals;
    return ptGlobals->ptMsgs->ElementNew();
}

static void sRMsgDelayedDelete(RMessageDelayed * ptMsg)
{
    RMessageGlobals * ptGlobals = CGameGet().ptRMessageGlobals;
    ptGlobals->ptMsgs->ElementDelete(ptMsg);
}

static RMessageDelayed * sRMsgDelayedGet(const RMessageID idMsg)
{
    RMessageGlobals * ptGlobals = CGameGet().ptRMessageGlobals;
    return ptGlobals->ptMsgs->ElementGet(idMsg);
}
