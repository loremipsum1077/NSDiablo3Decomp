// Types adopted from Hyperlink CU 0x0427bd30; original X/Client/OnlineService/ChatTypes.h.
// Inline bodies sit on their DWARF lines (from code inlined into sParseChatId/HyperlinkValidate). Inference: Copy tests CHANNEL before WHISPER; that order reproduces the original compare sequence.
#pragma once
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Sigma/RefString.h"
#include "../../../Contrib/Contrib/blz/include/blz/iosfwd.h"
















namespace OnlineService {

struct ChatId {  // size 0x48
    enum ChatType {
        INVALID = 0,
        CHANNEL = 1,
        WHISPER = 2,
    };





    ChatId()
        : m_type(INVALID)
        , m_instanceNumber(-1)
    {}

    explicit ChatId(LPCSTR szInternalChannelName)
        : m_type(CHANNEL)
        , m_name(szInternalChannelName)
        , m_instanceNumber(-1)
    {}

    explicit ChatId(const OnlineService::GameAccountHandle &);
    explicit ChatId(const OnlineService::ChannelId &);
    ChatId(const OnlineService::ChatId &);

















    OnlineService::ChatId & operator=(const OnlineService::ChatId & copy) { if (this != &copy) Copy(copy); return *this; }

    bool operator==(const OnlineService::ChatId &) const;
    bool operator!=(const OnlineService::ChatId &) const;



    BOOL IsInvalid() const { return m_type == INVALID; }

    BOOL IsChannel() const;
    BOOL IsWhisper() const;
    uint32 HashKey() const;
    blz::ostream & ToString(blz::ostream &) const;
    CRefString GetName() const;
    void SetName(LPCSTR);
    const OnlineService::GameAccountHandle & GetGameAccount() const;
    const OnlineService::ChannelId & GetChannelId() const;
    int32 GetInstanceNumber() const;





















protected:

    virtual bool IsUpgradedWhisper() const { return false; }

    void Copy(const OnlineService::ChatId & copy)
    {
        m_type = copy.m_type;
        m_name = copy.m_name;
        m_instanceNumber = copy.m_instanceNumber;
        if (m_type == CHANNEL)
            m_channelId = copy.m_channelId;
        else if (m_type == WHISPER)
            m_gameAccount = copy.m_gameAccount;
    }

    OnlineService::ChatId::ChatType m_type;  // 0x08
    OnlineService::GameAccountHandle m_gameAccount;  // 0x0c
    OnlineService::ChannelId m_channelId;  // 0x18
    CRefString m_name;  // 0x28
    int32 m_instanceNumber;  // 0x40
};
static_assert(sizeof(ChatId) == 0x48, "ChatId");

}  // namespace OnlineService
