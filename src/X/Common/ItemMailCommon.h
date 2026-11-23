// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/ItemMailCommon.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "CommonTypes.h"
#include "Protobuf/Items.pb.h"

enum MailStatus {
    MS_UNREAD = 0,
    MS_READ = 1,
    MS_DELETED = 2,
};

struct ActorInventoryMailRecord {  // size 0xa0
    uint32 tAccountTo;  // 0x00
    uint32 tAccountFrom;  // 0x04
    uint64 tMailId;  // 0x08
    blz::string szTitle;  // 0x10
    blz::string szBody;  // 0x38
    MailStatus eStatus;  // 0x60
    uint32 sendTime;  // 0x64
    Blizzard::Time::Timestamp tExpireTime;  // 0x68
    uint32 uPartition;  // 0x70
    uint32 uSeasonCreated;  // 0x74
    uint32 uFlags;  // 0x78
    bool bDirty;  // 0x7c
    D3::Items::MailAttachments attachments;  // 0x80
    void Export(D3::Items::Mail &);
    void Import(const D3::Items::Mail & tMail);
};
static_assert(sizeof(ActorInventoryMailRecord) == 0xa0, "ActorInventoryMailRecord");

typedef blz::unordered_map<unsigned long, ActorInventoryMailRecord, blz::hash<unsigned long>, blz::equal_to<unsigned long>, blz::allocator<blz::pair<const unsigned long, ActorInventoryMailRecord> > > MailRecordMap;

struct MailSendMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    int nToAccount;  // 0x08
};
static_assert(sizeof(MailSendMessage) == 0xc, "MailSendMessage");

struct MailSentResponseMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    uint64 idMail;  // 0x08
    int idErr;  // 0x10
};
static_assert(sizeof(MailSentResponseMessage) == 0x18, "MailSentResponseMessage");


// Functions defined in this file (bodies are not in the DWARF):
//   line    39  ActorInventoryMailRecord::ActorInventoryMailRecord()  [0x7100419a80, inlined into 3 functions]
//   line    39  ActorInventoryMailRecord & ActorInventoryMailRecord::operator=(const ActorInventoryMailRecord &)  [0x71007394f0]
//   line    39  ActorInventoryMailRecord::~ActorInventoryMailRecord()  [inlined into 6 functions]
