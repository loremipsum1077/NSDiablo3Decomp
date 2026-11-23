// X/Client/Console/MailService.cpp
// DemonWare mail for the console builds: per-user inbox lists (messages and events), header and
// body retrieval tasks, and the send / delete / verify / mark-read requests. Classes are defined
// here with their members in the class body, at the lines the DWARF records.
#include "../../Common/OnlineService/OnlineService.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdSort.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdUserAccountID.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdMail/bdMail.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdMail/bdMailBody.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdMail/bdMailInfo.h"
#include "../../../Contrib/Contrib/DemonWare/bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../../Contrib/Contrib/blz/include/blz/atomic.h"
#include "../../../Contrib/Contrib/blz/include/blz/list.h"
#include "../../../Contrib/Contrib/blz/include/blz/queue.h"
// The original Online.h pulls these in through its UI headers (Battlenet/UI/Util/UIUtil.h); only
// their constants show, in the static initializer.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "LobbyService.h"
#include "MailService.h"
#include "Online.h"
#include "GamerProfile.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../Common/OnlineService/SubscriptionEvent.h"
#include "../../Common/Protobuf/ConsoleMail.pb.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/BlzExt/fixed_vector.h"
namespace Console {
namespace Online {

class MailInternal;
typedef bdReference<Console::Online::MailInternal> MailInternalRef;

// Defined in X/Client/Console/Online.cpp.
void QueueNetworkEventForMainThreadAlways(OnlineService::Callback<void ()> pfnEvent);

// Invented macro names: bind a mail callback to its result (and mail) for the main thread.
#define MAIL_BIND_RESULT(pfnCallback, eResult) \
    OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, SendMailCallback>, const MailResult &>:: \
        bind<&OnlineService::internal::callback_base<1, SendMailCallback>::execute>(pfnCallback, eResult)
#define MAIL_BIND_RESULT_AND_MAIL(pfnCallback, eResult, ptMail) \
    OnlineService::internal::method_binder_2<void, OnlineService::internal::callback_base<2, DeleteMailCallback>, const MailResult &, const MailRef &>:: \
        bind<&OnlineService::internal::callback_base<2, DeleteMailCallback>::execute>(pfnCallback, eResult, ptMail)

typedef OnlineService::SubscriptionEvent<int> NewMailEvent;
NewMailEvent & GetNewMailEvent();
typedef OnlineService::SubscriptionEvent<> SeasonsChangedEvent;
SeasonsChangedEvent & GetSeasonsChangedEvent();
enum {
    MAX_NUM_MAIL_HEADERS_PER_REQUEST = 50,
    MAX_NUM_MAIL_BODIES_PER_REQUEST = 10,
    ALL_MAIL_TYPE_FLAGS = 3,
    NEW_MAIL_CHECK_DELAY = 3000,
};
static const uint32 sauMaxMailsByType[NUM_MAIL_TYPES] = { 30, 5 };
enum {
    SECONDS_PER_DAY = 86400,
};
uint32 sauMailDurations[NUM_MAIL_TYPES] = { 31 * SECONDS_PER_DAY, 14 * SECONDS_PER_DAY };


static bdUInt16 MailTypeToCategory(MailType eType, bool bHardcore, uint32 uSeason)
{
    // Hardcore messages have their own category; each season shifts the categories by 100.

    if (eType == MAIL_TYPE_MESSAGE && bHardcore)
    {
        eType = MAIL_TYPE_MESSAGE_HC;
    }

    uint32 uSeasonOffset = 0;
    if (uSeason != 0)
    {
        uint32 uSeasonIndex = (uSeason - 1) % 10;
        uSeasonOffset = (uSeasonIndex + 1) * 100;
    }
    return (bdUInt16)(eType + uSeasonOffset);
}

class MailServiceInternal : public MailService
{
public:
    MailServiceInternal(int32);
    virtual ~MailServiceInternal();
    void IdleInternal();
    virtual void GetMail(MailType eType, MailArray & atMail, uint32 uStart, uint32 uMax);
    virtual BOOL GetHasMail(MailType eType);
    virtual void SendMail(MailType eType, const D3::Console::MailItem & tMail, const bdUserAccountID & idUser, SendMailCallback pfnCallback);
    virtual void DeleteMail(uint64 idMail, DeleteMailCallback pfnCallback);
    virtual void VerifyMail(uint64 idMail, VerifyMailCallback pfnCallback);
    virtual void MarkRead(uint64 idMail);
    void OnNewMail();
    void OnSeasonsChanged();
    void OnMailHeadersReceived(MailType eType, bdMailInfo * atMailInfo, uint32 uNumMailHeaders);
    void RetrieveMailHeaders(MailType eMailType, uint32 uStartIndex, uint32 uMaxRetrieve);
    void RetrieveMailBodies();
    void OnMailDeleted(MailInternalRef ptMail);
    MailInternalRef FindMailByID(uint64 idMail);

    int32 m_nUserIndex;  // 0x08
    LobbyService * m_ptLobbyService;  // 0x10
    typedef blz::list<bdReference<MailInternal>, blz::allocator<bdReference<MailInternal> > > MailList;
    typedef blz::queue<bdReference<MailInternal>, blz::deque<bdReference<MailInternal>, blz::allocator<bdReference<MailInternal> > > > MailQueue;
    MailList m_atMail[NUM_MAIL_TYPES];  // 0x18
    MailQueue m_tMailRetrievalQueue;  // 0x48
    bool m_bMailActive;  // 0x68
    int32 m_nGetMailHeadersInProgress;  // 0x6c
    uint32 m_uNewMail;  // 0x70
    Blizzard::Time::Millisecond m_msNewMailTime;  // 0x74
    static blz::atomic_bool ms_bGameActive;
    static blz::atomic_bool ms_bGameIsHardcore;
    static blz::atomic_bool ms_bGameIsSeasonal;
    static blz::atomic<unsigned int> ms_uGameSeason;
};
static_assert(sizeof(MailServiceInternal) == 0x78, "MailServiceInternal");

} // namespace Online
} // namespace Console
// Owner-local recovery of Callback.h:650; shared headers are unchanged.
// The four get_bound_type instantiations in this CU resolve to A1, A2, A3 and A4.
namespace OnlineService {
namespace internal {
template <typename R, typename A1, typename A2, typename A3, typename A4, typename P1, typename P2,
          R (*F)(A1, A2, A3, A4, P1, P2)>
struct function_bind_4_2 {
    R operator()(typename d3boost::call_traits<P1>::param_type param1,
                 typename d3boost::call_traits<P2>::param_type param2) const // Callback.h:657
    {
        return F(m_arg1, m_arg2, m_arg3, m_arg4, param1, param2);
    }
    function_bind_4_2(typename d3boost::call_traits<A1>::param_type arg1,
                      typename d3boost::call_traits<A2>::param_type arg2,
                      typename d3boost::call_traits<A3>::param_type arg3,
                      typename d3boost::call_traits<A4>::param_type arg4) // Callback.h:661
        : m_arg1(arg1), m_arg2(arg2), m_arg3(arg3), m_arg4(arg4)
    {
    }
    A1 m_arg1; // Callback.h:677
    A2 m_arg2; // Callback.h:678
    A3 m_arg3; // Callback.h:679
    A4 m_arg4; // Callback.h:680
};  // method_binder_3 (Callback.h:7904) now lives in Callback.h.
}
}
namespace Console {
namespace Online {
















blz::atomic_bool MailServiceInternal::ms_bGameActive(false);
blz::atomic_bool MailServiceInternal::ms_bGameIsHardcore(false);
blz::atomic_bool MailServiceInternal::ms_bGameIsSeasonal(false);
blz::atomic<unsigned int> MailServiceInternal::ms_uGameSeason(0);

void MailSetGameState(bool bActive, bool bIsHardcore, bool bIsSeasonal, uint32 uSeason)
{
    // Only the transitions matter: entering a game records its mode, leaving clears it.
    // (176-178)

    if (bActive != MailServiceInternal::ms_bGameActive)
    {
        if (bActive)
        {
            MailServiceInternal::ms_bGameIsHardcore = bIsHardcore;
            MailServiceInternal::ms_bGameIsSeasonal = bIsSeasonal;
            MailServiceInternal::ms_uGameSeason = bIsSeasonal ? uSeason : 0;
        }
        MailServiceInternal::ms_bGameActive = bActive;
    }
}



class MailInternal : public Mail
{
public:

    // Inference: initialize from the header/type and zero body state. DWARF-only constructor; standalone body is not checkable.
    MailInternal(
        const bdMailInfo & tHeader,
        MailType eType)
        : m_tHeader(tHeader)
        , m_eType(eType)
        , m_pBody(NULL)
        , m_bBodyRetrieved(FALSE)
        , m_bContentsValid(FALSE)
        , m_tExpiration(0)
    {
    }

    virtual ~MailInternal()
    {
        SigmaMemoryFree(m_pBody, NULL);
    }

    MailType GetType() const
    {
        return m_eType;
    }


    virtual uint64 GetID() const
    {
        return m_tHeader.m_mailID;
    }


    virtual bdUserAccountID GetSenderID() const
    {
        return m_tHeader.m_senderID;
    }


    virtual const char * GetSender() const
    {
        return m_tHeader.m_senderName;
    }


    virtual Blizzard::Time::Timestamp GetSentTime() const
    {
        return Blizzard::Time::FromUnixTime(m_tHeader.m_timeStamp);
    }


    virtual Blizzard::Time::Timestamp GetExpiration() const
    {
        return m_tExpiration;
    }


    virtual const D3::Console::MailItem & GetContents() const
    {
        return m_tContents;
    }


    virtual BOOL AreContentsValid() const
    {
        return m_bContentsValid;
    }


    virtual BOOL IsRead() const
    {
        return m_tHeader.m_isRead;
    }


    virtual BOOL IsReturned() const
    {
        return m_tHeader.m_isUndelivered;
    }


    void OnBodyRetrieved(BOOL bSuccess)
    {
        m_bBodyRetrieved = bSuccess;


        if (bSuccess)
        {
            m_bContentsValid = m_tContents.ParseFromArray(m_pBody, m_tHeader.m_size);

            if (m_tHeader.m_messageDuration != 0)
            {
                m_tExpiration = Blizzard::Time::FromUnixTime64(Blizzard::Time::ToUnixTime64(GetNetworkTimestamp())
                    + m_tHeader.m_secondsToExpiration);
            }
        }

        SigmaMemoryFree(m_pBody, NULL);
        m_pBody = NULL;
    }

    bdMailInfo m_tHeader;  // 0x10
    MailType m_eType;  // 0xc0
    void * m_pBody;  // 0xc8
    BOOL m_bBodyRetrieved;  // 0xd0
    Blizzard::Time::Timestamp m_tExpiration;  // 0xd8
    BOOL m_bContentsValid;  // 0xe0
    D3::Console::MailItem m_tContents;  // 0xe8
};
static_assert(sizeof(MailInternal) == 0x138, "MailInternal");

class MailTask : public LobbyTask
{
public:


    MailTask(MailServiceInternal * ptMailService)

        : LobbyTask(ptMailService->m_ptLobbyService)
        , m_ptMailService(ptMailService)
    {
    }

    MailServiceInternal * m_ptMailService;  // 0x20
};



MailServiceInternal * sptMailService[4];






void MailService::Create()
{
    // One service per local user.
    for (int32 i = 0; i < 4; ++i)
    {
        sptMailService[i] = new MailServiceInternal(i);
    }
}





MailService * MailService::Get(int32 nUserIndex)
{
    // (342-344)

    return sptMailService[nUserIndex];
}




void MailService::Idle()
{
    for (int32 i = 0; i < 4; ++i)
    {
        sptMailService[i]->IdleInternal();
    }
}





void MailService::SetTimeout(MailType eType, int32 nTimeoutDays)
{
    // (365-366)
    if (eType >= MAIL_TYPE_MESSAGE && eType < NUM_MAIL_TYPES)
    {
        sauMailDurations[eType] = nTimeoutDays * SECONDS_PER_DAY;
    }
}





MailServiceInternal::MailServiceInternal(int32 nUserIndex)
    : m_nUserIndex(nUserIndex)
    , m_ptLobbyService(LobbyService::Get(nUserIndex))
    , m_bMailActive(false)
    , m_nGetMailHeadersInProgress(0)
    , m_uNewMail(0)
    , m_msNewMailTime(0)
{
    m_ptLobbyService->GetNewMailEvent().Subscribe(OnlineService::internal::method_bind_0_0<void, MailServiceInternal, MailServiceInternal *, &MailServiceInternal::OnNewMail>(this));
    GetSeasonsChangedEvent().Subscribe(OnlineService::internal::method_bind_0_0<void, MailServiceInternal, MailServiceInternal *, &MailServiceInternal::OnSeasonsChanged>(this));
}





MailServiceInternal::~MailServiceInternal()
{
}





void MailServiceInternal::IdleInternal()
// Inference: mail activity requires game/connection/profile/party; transitions clear cached mail and refresh after the delay.
{
    bool bMailAllowed = ms_bGameActive &&
        m_ptLobbyService->IsConnected() &&
        GamerProfile::IsActive(m_nUserIndex) &&
        PartyIsInParty(m_nUserIndex);

    if (bMailAllowed != m_bMailActive)
    {
        for (MailList & tMail : m_atMail)
        {
            tMail.clear();
        }
        MailQueue().swap(m_tMailRetrievalQueue);

        m_bMailActive = bMailAllowed;

        if (bMailAllowed)
        {

            m_uNewMail = ALL_MAIL_TYPE_FLAGS;
            m_msNewMailTime = 0;
        }
    }

    if (m_bMailActive &&
        m_uNewMail &&
        !m_nGetMailHeadersInProgress &&
        Blizzard::Time::GetTimeElapsed(m_msNewMailTime, Blizzard::Time::Milliseconds()) > NEW_MAIL_CHECK_DELAY)
    {
        for (int32 i = 0; i < NUM_MAIL_TYPES; ++i)
        {
            if (m_uNewMail & (1 << i))
            {
                RetrieveMailHeaders((MailType)i, 0, sauMaxMailsByType[i]);
            }
        }

        m_uNewMail = 0;
    }
}








class SendMailTask : public MailTask  // DWARF: class at 450; ~SendMailTask is implicit (artificial)
{
public:

    SendMailTask(MailServiceInternal * ptMailService, const D3::Console::MailItem & tMail, uint16 uCategory, uint32 uDurationSeconds, BOOL bReturnToSender, const bdUserAccountID & idUser, SendMailCallback pfnCallback)
        : MailTask(ptMailService)
        , m_tMail(tMail)
        , m_uCategory(uCategory)
        , m_uDurationSeconds(uDurationSeconds)
        , m_bReturnToSender(bReturnToSender)
        , m_idUser(idUser)
        , m_pfnCallback(pfnCallback)
    {
    }








    virtual void Start()
    {
        blz::string sMail = m_tMail.SerializeAsString();
        m_ptRemoteTask = m_ptLobbyService->GetMail()->sendMailWithDuration(sMail.data(), sMail.size(), m_uCategory, m_uDurationSeconds, m_bReturnToSender != FALSE, &m_idUser, 1, NULL);
    }



    virtual void OnComplete()
    {
        // (482-484)

        if (m_pfnCallback)
        {
            QueueNetworkEventForMainThread(MAIL_BIND_RESULT(m_pfnCallback, MAIL_SUCCESS));
        }
    }


    virtual void OnFailed()
    {
        MailResult eResult = m_ptLobbyService->IsConnected() ? MAIL_FAILURE : MAIL_FAILURE_NOT_CONNECTED;

        if (m_pfnCallback)
        {
            QueueNetworkEventForMainThread(MAIL_BIND_RESULT(m_pfnCallback, eResult));
        }
    }

    D3::Console::MailItem m_tMail;  // 0x28
    uint16 m_uCategory;  // 0x78
    uint32 m_uDurationSeconds;  // 0x7c
    BOOL m_bReturnToSender;  // 0x80
    bdUserAccountID m_idUser;  // 0x88
    SendMailCallback m_pfnCallback;  // 0xc0
};














void MailServiceInternal::SendMail(MailType eType, const D3::Console::MailItem & tMail, const bdUserAccountID & idUser, SendMailCallback pfnCallback)
{
    // DemonWare limits a mail body to 1 KB.
    if (tMail.ByteSize() > 1024U)
    {
        if (pfnCallback)
        {
            QueueNetworkEventForMainThread(MAIL_BIND_RESULT(pfnCallback, MAIL_FAILURE));
        }
        return;
    }

    uint32 uDurationSeconds = 0;
    BOOL bReturnToSender = FALSE;
    switch (eType)
    {
    case MAIL_TYPE_MESSAGE:
    case MAIL_TYPE_MESSAGE_HC:
        bReturnToSender = tMail.has_item_mail();
        uDurationSeconds = sauMailDurations[MAIL_TYPE_MESSAGE];
        break;
    case MAIL_TYPE_EVENT:
        bReturnToSender = FALSE;
        uDurationSeconds = sauMailDurations[MAIL_TYPE_EVENT];
        break;
    }

    if (uDurationSeconds == 0)
    {
        if (pfnCallback)
        {
            QueueNetworkEventForMainThread(MAIL_BIND_RESULT(pfnCallback, MAIL_FAILURE_NOT_CONNECTED));
        }
        return;
    }

    bdUInt16 uCategory = MailTypeToCategory(eType, MailServiceInternal::ms_bGameIsHardcore, MailServiceInternal::ms_uGameSeason);
    ::new SendMailTask(this, tMail, uCategory, uDurationSeconds, bReturnToSender, idUser, pfnCallback);
}




















class DeleteMailTask : public MailTask  // DWARF: class at 581; ~DeleteMailTask is implicit (artificial)
{
public:

    DeleteMailTask(MailServiceInternal * ptMailService, MailInternalRef ptMail, DeleteMailCallback pfnCallback)
        : MailTask(ptMailService)
        , m_ptMail(ptMail)
        , m_pfnCallback(pfnCallback)
    {
    }




    virtual void Start()
    {
        uint64 idMail = m_ptMail->GetID();

        m_ptRemoteTask = m_ptLobbyService->GetMail()->deleteMail(&idMail, 1, NULL);
    }



    virtual void OnComplete()
    {
        // (606-608)
        m_ptMailService->OnMailDeleted(m_ptMail);

        if (m_pfnCallback)
        {
            QueueNetworkEventForMainThread(MAIL_BIND_RESULT_AND_MAIL(m_pfnCallback, MAIL_SUCCESS, MailRef(m_ptMail)));
        }
    }



    virtual void OnFailed()
    {
        // (619-621)
        MailResult eResult = m_ptLobbyService->IsConnected() ? MAIL_FAILURE : MAIL_FAILURE_NOT_CONNECTED;

        if (m_pfnCallback)
        {
            QueueNetworkEventForMainThread(MAIL_BIND_RESULT_AND_MAIL(m_pfnCallback, eResult, MailRef(m_ptMail)));
        }
    }

    MailInternalRef m_ptMail;  // 0x28
    DeleteMailCallback m_pfnCallback;  // 0x30
};













void MailServiceInternal::DeleteMail(uint64 idMail, DeleteMailCallback pfnCallback)
{
    MailInternalRef ptMail = FindMailByID(idMail);
    if (ptMail.isNull())
    {
        QueueNetworkEventForMainThreadAlways(MAIL_BIND_RESULT_AND_MAIL(pfnCallback, MAIL_FAILURE, MailRef()));
        return;
    }

    ::new DeleteMailTask(this, ptMail, pfnCallback);
}










// Original unnamed timestamp parameter is unused; bind uses the original MailRef conversion.
void sVerifyMailHandler(int32 nUserIndex, LobbyService * ptLobbyService, MailInternalRef ptMail, VerifyMailCallback pfnCallback, BOOL bSuccess, const bdTimeStamp &)
{

    MailResult eResult;
    if (bSuccess)
        eResult = MAIL_SUCCESS;
    else
        eResult = ptLobbyService->IsConnected() ? MAIL_FAILURE : MAIL_FAILURE_NOT_CONNECTED;


    QueueNetworkEventForMainThread(MAIL_BIND_RESULT_AND_MAIL(pfnCallback, eResult, MailRef(ptMail)));
}



void MailServiceInternal::VerifyMail(
    uint64 idMail,
    VerifyMailCallback pfnCallback)
{
    MailInternalRef ptMail = FindMailByID(idMail);
    if (ptMail.isNull()) {
        // Original queues failure and still requests server time: no return at line689.

        QueueNetworkEventForMainThreadAlways(MAIL_BIND_RESULT_AND_MAIL(pfnCallback, MAIL_FAILURE, MailRef()));
    }
    // Instantiate original binder directly; Callback.h:1721 factory only forwards these four captures.
    m_ptLobbyService->GetServerTime(
        OnlineService::internal::function_bind_4_2<void, int32, LobbyService *, MailInternalRef, VerifyMailCallback, BOOL, const bdTimeStamp &, &sVerifyMailHandler>(m_nUserIndex, m_ptLobbyService, ptMail, pfnCallback), false);
}





class MarkMailReadTask : public MailTask
{
public:





    MarkMailReadTask(MailServiceInternal * ptMailService, bdUInt64 idMail)
        : MailTask(ptMailService)
        , m_idMail(idMail)
    {
    }



    virtual void Start()
    {
        m_ptRemoteTask = m_ptLobbyService->GetMail()->setMessageReadStatus(m_idMail, true, NULL);
    }



    virtual void OnComplete()
    {
    }



    virtual void OnFailed()
    {
        if (!m_ptLobbyService->IsConnected())
        {
        }
    }

    bdUInt64 m_idMail;  // 0x28
};









void MailServiceInternal::MarkRead(uint64 idMail)
{
    for (MailList & tMailList : m_atMail)
    {
        for (MailInternalRef & ptMail : tMailList)
        {
            if (ptMail->GetID() == idMail)
            {
                ptMail->m_tHeader.m_isRead = true;
                ::new MarkMailReadTask(this, ptMail->GetID());
                return;
            }
        }
    }
}




// Inference: reject invalid types and skip the first uStart list entries before collecting mail.
void MailServiceInternal::GetMail(
    MailType eType,
    MailArray & atMail,
    uint32 uStart,
    uint32 uMax)
{
    if (eType < MAIL_TYPE_MESSAGE || eType >= NUM_MAIL_TYPES)
    {
        return;
    }

    MailList & tMailList = m_atMail[eType];

    MailList::iterator it = tMailList.begin();
    while (it != tMailList.end() && uStart)
    {
        --uStart;
        ++it;
    }

    // Inference: the second loop appends retrieved bodies up to the total output limit.
    for (; it != tMailList.end() && atMail.size() < uMax; ++it)
    {
        MailInternalRef ptMail = *it;

        if (ptMail->GetType() == eType &&
            ptMail->m_bBodyRetrieved)
        {
            atMail.push_back(MailRef(ptMail));
        }
    }
}




BOOL MailServiceInternal::GetHasMail(MailType eType)
{
    // (805)
    if (eType < MAIL_TYPE_MESSAGE || eType >= NUM_MAIL_TYPES)
    {
        return FALSE;
    }

    return !m_atMail[eType].empty();
}




void MailServiceInternal::OnNewMail()
{
    // Ask for the headers of every type that still has room.
    for (int32 i = 0; i < NUM_MAIL_TYPES; ++i)
    {
        if (m_atMail[i].size() < sauMaxMailsByType[i])
        {
            m_uNewMail |= (1 << i);
        }
    }

    m_msNewMailTime = Blizzard::Time::Milliseconds();
}




void MailServiceInternal::OnSeasonsChanged()
{
    ScopedNetworkThreadLock tLock;

    OnNewMail();
}




// Inference: reconcile returned IDs with cached mail, retaining existing objects and fetching only new bodies.
void MailServiceInternal::OnMailHeadersReceived(
    MailType eType,
    bdMailInfo * atMailInfo,
    uint32 uNumMailHeaders)
{
    MailList & tMail = m_atMail[eType];

    --m_nGetMailHeadersInProgress;

    if (!m_ptLobbyService->IsConnected())
    {
        return;
    }

    // Inference: absent IDs are erased; short-circuit the scan after finding a matching ID.
    for (MailList::iterator it = tMail.begin(); it != tMail.end();)
    {
        MailInternalRef ptMail = *it;

        BOOL bFound = FALSE;
        for (uint32 i = 0; i < uNumMailHeaders && !bFound; ++i)
        {






            bFound = ptMail->GetID() == atMailInfo[i].m_mailID;

        }
        if (!bFound)
        {

            it = tMail.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Inference: only previously unseen IDs create mail objects and enter the body-retrieval queue.
    for (uint32 i = 0; i < uNumMailHeaders; ++i)
    {







        BOOL bFound = FALSE;
        for (MailInternalRef ptMail : tMail)
        {
            if (ptMail->GetID() == atMailInfo[i].m_mailID)
            {
                bFound = TRUE;
                break;
            }
        }

        if (!bFound)
        {

            // Inference: preserve the returned header and its requested mail type in the new object.
            MailInternalRef ptMail = ::new MailInternal(atMailInfo[i], eType);
            tMail.push_back(ptMail);
            m_tMailRetrievalQueue.push(ptMail);
        }
    }

    // Inference: start body retrieval only after the last outstanding header request finishes.
    if (m_nGetMailHeadersInProgress == 0)
    {
        RetrieveMailBodies();
    }
}




class GetMailHeadersTask : public MailTask
{
public:
    typedef OnlineService::Callback<void (bdMailInfo *, unsigned int)> GetMailHeadersTaskCallback;







    // Inference: retain requested category/range and callback, leaving the mail-info array default constructed.
    GetMailHeadersTask(
        MailServiceInternal * ptMailService,
        MailType eMailType,
        uint32 uStartIndex,
        uint32 uMaxRetrieve,
        GetMailHeadersTaskCallback pfnCallback)
        : MailTask(ptMailService)
        , m_eMailType(eMailType)
        , m_uIndex(uStartIndex)
        , m_uMaxRetrieve(uMaxRetrieve)
        , m_pfnCallback(pfnCallback)
    {
    }

    virtual void Start()

    {
        blzext::fixed_vector<unsigned short, 2> auCategories;
        // Inference: request the current category, and the most recently ended season when outside seasonal play.
        auCategories.push_back(
            MailTypeToCategory(
                m_eMailType,
                MailServiceInternal::ms_bGameIsHardcore,
                MailServiceInternal::ms_uGameSeason));

        if (!MailServiceInternal::ms_uGameSeason)
        {

            uint32 uSeason = OnlineService::GetSeasonNum();
            if (OnlineService::GetSeasonState() != OnlineService::SEASON_STATE_ENDED &&
                uSeason != 0)
            {
                --uSeason;
            }

            if (uSeason != 0)
            {
                auCategories.push_back(MailTypeToCategory(m_eMailType, MailServiceInternal::ms_bGameIsHardcore, uSeason));
            }
        }

        m_ptRemoteTask = m_ptLobbyService->GetMail()->getMailInfoForCategories(
            m_atMailInfos,
            m_uIndex,
            Math::Min<uint32>(m_uMaxRetrieve, MAX_NUM_MAIL_HEADERS_PER_REQUEST),
            auCategories.data(),
            auCategories.size(),
            false,
            NULL);

    }

    virtual void OnComplete()
    {
        m_pfnCallback(m_atMailInfos, m_ptRemoteTask->getNumResults());
    }



    virtual void OnFailed()
    {
        if (!m_ptLobbyService->IsConnected())
        {
        }







        m_pfnCallback(m_atMailInfos, 0);
    }

    MailType m_eMailType;  // 0x28
    uint32 m_uIndex;  // 0x2c
    uint32 m_uMaxRetrieve;  // 0x30
    bdMailInfo m_atMailInfos[MAX_NUM_MAIL_HEADERS_PER_REQUEST];  // 0x38
    GetMailHeadersTaskCallback m_pfnCallback;  // 0x2298
};


// Inference: bind the requested mail type to the response handler and count the outstanding request.
// DWARF-only helper needed by IdleInternal; standalone body is not checkable.
void MailServiceInternal::RetrieveMailHeaders(
    MailType eMailType,
    uint32 uStartIndex,
    uint32 uMaxRetrieve)
{
    ++m_nGetMailHeadersInProgress;
    ::new GetMailHeadersTask(this, eMailType, uStartIndex, uMaxRetrieve,
        OnlineService::internal::method_binder_3<void, MailServiceInternal, MailType, bdMailInfo *, unsigned int>::bind<&MailServiceInternal::OnMailHeadersReceived>(this, eMailType));
}




class GetMailBodiesTask : public MailTask
{
public:

    virtual ~GetMailBodiesTask() {}



    GetMailBodiesTask(MailServiceInternal * ptMailService)

        : MailTask(ptMailService)
        , m_uNumMails(0)
    {
    }

    virtual void Start()
    {
        bdCombSort(m_aptMail, m_uNumMails, sSortMailFunc);

        bdUInt64 aidMails[MAX_NUM_MAIL_BODIES_PER_REQUEST];
        for (uint32 i = 0; i < m_uNumMails; ++i)
        {
            MailInternalRef ptMail = m_aptMail[i];

            // The body is read straight into the mail's own buffer.

            ptMail->m_pBody = SigmaMemoryNew(ptMail->m_tHeader.m_size, 4, NULL, FALSE);

            m_atMailBodies[i].m_mailBody = ptMail->m_pBody;
            m_atMailBodies[i].m_mailID = ptMail->GetID();
            m_atMailBodies[i].m_size = ptMail->m_tHeader.m_size;

            aidMails[i] = m_atMailBodies[i].m_mailID;
        }

        m_ptRemoteTask = m_ptLobbyService->GetMail()->getMail(aidMails, m_uNumMails, m_atMailBodies, false, NULL);
    }



    virtual void OnComplete()
    {
        // The bodies were written straight into each mail's buffer.
        for (uint32 i = 0; i < m_uNumMails; ++i)
        {
            MailInternalRef ptMail = m_aptMail[i];
            ptMail->OnBodyRetrieved(TRUE);
        }



        m_ptMailService->RetrieveMailBodies();
    }

    virtual void OnFailed()
    {
        if (!m_ptLobbyService->IsConnected())
        {
        }







        for (uint32 i = 0; i < m_uNumMails; ++i)
        {
            MailInternalRef ptMail = m_aptMail[i];
            ptMail->OnBodyRetrieved(FALSE);
        }


        m_ptMailService->RetrieveMailBodies();
    }

    void AddMail(MailInternalRef ptMail)
    {
        // (1114-1116)


        m_aptMail[m_uNumMails++] = ptMail;
    }

    BOOL IsFull() const { return m_uNumMails >= MAX_NUM_MAIL_BODIES_PER_REQUEST; }

    static bdBool sSortMailFunc(const MailInternalRef & lhs, const MailInternalRef & rhs)
    {
        // Newest (highest id) first.
        // (1126-1130)





        return lhs->GetID() > rhs->GetID();
    }

    uint32 m_uNumMails;  // 0x28
    MailInternalRef m_aptMail[MAX_NUM_MAIL_BODIES_PER_REQUEST];  // 0x30
    bdMailBody m_atMailBodies[MAX_NUM_MAIL_BODIES_PER_REQUEST];  // 0x80
};




void MailServiceInternal::RetrieveMailBodies()
{
    if (m_tMailRetrievalQueue.empty())
    {
        // Everything is in: tell the UI if there is anything to show.
        for (MailList & tMailList : m_atMail)
        {
            if (!tMailList.empty())
            {
                QueueNotification(GetNewMailEvent(), m_nUserIndex);
                break;
            }
        }
        return;
    }

    GetMailBodiesTask * ptTask = ::new GetMailBodiesTask(this);

    while (!ptTask->IsFull() && !m_tMailRetrievalQueue.empty())
    {
        ptTask->AddMail(m_tMailRetrievalQueue.front());
        m_tMailRetrievalQueue.pop();
    }
}








void MailServiceInternal::OnMailDeleted(MailInternalRef ptMail)
{
    MailType eType = ptMail->GetType();
    if (eType < MAIL_TYPE_MESSAGE || eType >= sizeof(m_atMail) / sizeof(m_atMail[0]))
    {
        return;
    }

    MailList & tMailList = m_atMail[eType];
    MailList::iterator it = blz::find(tMailList.begin(), tMailList.end(), ptMail);

    if (it != tMailList.end())
    {
        // A full list may have more mail waiting on the server: refresh it.
        if (tMailList.size() == sauMaxMailsByType[eType])
        {
            m_uNewMail |= (1 << eType);
            m_msNewMailTime = Blizzard::Time::Milliseconds();
        }
        tMailList.erase(it);
    }
}




MailInternalRef MailServiceInternal::FindMailByID(uint64 idMail)
{
    for (MailList & tMailList : m_atMail)
    {
        for (MailInternalRef & ptMail : tMailList)
        {
            if (ptMail->GetID() == idMail)
            {
                return ptMail;
            }
        }
    }

    return MailInternalRef();
}





void MailGetAllOfType(int32 nUserIndex, MailType eType, MailArray & atMail, int32 nStart, int32 nMax)
{
    // (1222-1225)

    ScopedNetworkThreadLock tLock;

    MailService::Get(nUserIndex)->GetMail(eType, atMail, nStart, nMax);
}







BOOL MailHasMail(int32 nUserIndex, MailType eType)
{
    // (1237-1238)

    return MailService::Get(nUserIndex)->GetHasMail(eType);
}




void MailSend(int32 nUserIndex, MailType eType, const D3::Console::MailItem & tMail, bdOnlineUserID idDestinationUser, SendMailCallback pfnCallback)
{
    // (1247-1250)

    ScopedNetworkThreadLock tLock;

    MailService::Get(nUserIndex)->SendMail(eType, tMail, bdUserAccountID(idDestinationUser), pfnCallback);
}


























































void MailDelete(int32 nUserIndex, uint64 idMail, DeleteMailCallback pfnCallback)
{
    // (1313-1314)

    ScopedNetworkThreadLock tLock;

    MailService::Get(nUserIndex)->DeleteMail(idMail, pfnCallback);
}





void MailVerify(int32 nUserIndex, uint64 idMail, VerifyMailCallback pfnCallback)
{
    // (1326-1327)

    ScopedNetworkThreadLock tLock;

    MailService::Get(nUserIndex)->VerifyMail(idMail, pfnCallback);
}

}  // namespace Online
}  // namespace Console
