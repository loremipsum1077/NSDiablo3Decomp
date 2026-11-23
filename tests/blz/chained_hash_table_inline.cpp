// Checks for the inline-only parts of blz/chained_hash_table.h (find, begin,
// end, iterator increments, clear, swap, the constructors ...) through small
// game functions that inline them. Each function here is a minimal
// re-creation of the original caller (names from the DWARF); only the blz
// code it inlines is under test.
#include "Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "Contrib/Contrib/blz/include/blz/string.h"

typedef int GBID;
typedef unsigned int uint32;

// X/Client/Console/LobbyService.cpp:830 -- find, const_iterator !=, end
namespace Console {
namespace Online {
extern blz::unordered_map<int, unsigned int> tBlacklistedGBIDs;

bool GetGBIDBlacklistData(GBID gbid, uint32* flags) {
    blz::unordered_map<int, unsigned int>::const_iterator it = tBlacklistedGBIDs.find(gbid);
    if (it != tBlacklistedGBIDs.end()) {
        if (flags)
            *flags = it->second;
        return true;
    }
    return false;
}
}  // namespace Online
}  // namespace Console

// X/Common/ActorInventory.cpp:11547 -- begin, iterator !=, end, iterator++(int), erase
namespace Blizzard {
namespace Thread {
struct TLSSlot;
void* GetLocalStorage(const TLSSlot* slot);
}  // namespace Thread
}  // namespace Blizzard

struct ThreadData {
    Blizzard::Thread::TLSSlot* m_ptTlsSlot;
};
struct ThreadGlobals {
    static ThreadData s_tThreadData;
};

enum ACDID { ACDID_INVALID = -1 };
struct ActorCommonData {
    ACDID id;
};
struct Player {
    char m_pad[0xb110];
    uint32 tAccountId;
};
Player* PlayerGetByACD(ACDID id);

namespace D3 {
namespace Items {
class MailAttachments {  // protobuf message; destructor out of line
public:
    MailAttachments();
    MailAttachments(const MailAttachments&);
    virtual ~MailAttachments();
    char m_pad[0x18];
};
}  // namespace Items
}  // namespace D3

struct ActorInventoryMailRecord {  // X/Common/ItemMailCommon.h, size 0xa0
    uint32 tAccountTo;
    uint32 tAccountFrom;
    unsigned long tMailId;
    blz::string szTitle;
    blz::string szBody;
    int eStatus;
    uint32 sendTime;
    long tExpireTime;
    uint32 uPartition;
    uint32 uSeasonCreated;
    uint32 uFlags;
    bool bDirty;
    D3::Items::MailAttachments attachments;
};
typedef blz::unordered_map<unsigned long, ActorInventoryMailRecord> MailRecordMap;

namespace D3 {
namespace Items {
class SavedItem {  // protobuf message, size 0x50
public:
    SavedItem();
    virtual ~SavedItem();
    char m_pad[0x48];
};
}  // namespace Items
}  // namespace D3

struct ActorInventoryDatabaseRecord {  // X/Common/ActorInventory.h, size 0x80
    ACDID idACD;
    int eAction;
    int uActionMask;
    int eDestination;
    D3::Items::SavedItem tSavedItem;
    bool bNewItem;
    unsigned long firstAccountId;
    unsigned long lastAccountId;
    uint32 uUpdateCounter;
};
typedef blz::unordered_map<unsigned long, ActorInventoryDatabaseRecord> DirtyInventoryRecordMap;

struct ActorInventoryGlobals {
    char m_pad[0x90];
    DirtyInventoryRecordMap* ptDirtyRecords;
    char m_pad2[0x8];
    MailRecordMap* ptMailMap;
};
struct GameCommonData {
    char m_pad[0x1d8];
    ActorInventoryGlobals* ptActorInventoryGlobals;
};

inline GameCommonData& GameGetActiveGameCommonData() {
    return ***static_cast<GameCommonData***>(Blizzard::Thread::GetLocalStorage(ThreadGlobals::s_tThreadData.m_ptTlsSlot));
}

void ACDInventoryDeleteMail(ActorCommonData& tACDPlayer) {
    Player* ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer)
        return;

    ActorInventoryGlobals*& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;
    MailRecordMap* ptMailMap = ptActorInventoryGlobals->ptMailMap;
    for (MailRecordMap::iterator iter = ptMailMap->begin(); iter != ptMailMap->end();) {
        const ActorInventoryMailRecord& tMailRecord = iter->second;
        if (tMailRecord.tAccountTo == ptPlayer->tAccountId)
            iter = ptMailMap->erase(iter);
        else
            iter++;
    }
}

// X/Common/ActorInventory.cpp:12823 -- swap, clear
void ACDPurgeGlobalItemLists(DirtyInventoryRecordMap* ptMapToCopyTo) {
    ActorInventoryGlobals*& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;
    if (!ptActorInventoryGlobals)
        return;

    if (ptMapToCopyTo)
        ptMapToCopyTo->swap(*ptActorInventoryGlobals->ptDirtyRecords);

    ptActorInventoryGlobals->ptDirtyRecords->clear();
}

// X/Server/Achievements/AchievementsEngine/achievements_evaluator.cpp:1218 --
// begin, const_iterator ++, end, clear. Differs by one instruction: the
// original reuses end()'s loads of m_bucket_count/m_buckets (one ldp at
// line 575) for the clear() that follows; ours loads them again in clear().
namespace OnlineService {
namespace AchievementsEngine {
struct Achievement;
typedef Achievement AchievementUpdateRecord;

class AchievementsEvaluator {
public:
    class Listener {
    public:
        virtual ~Listener();
        virtual void OnAchievementUpdated(const AchievementUpdateRecord* achievement) = 0;
    };

    void FlushAchievements();

    char m_pad0[0x38];
    blz::unordered_map<unsigned long, Achievement*> m_dirtyAchievements;
    char m_pad1[0xb0 - 0x58];
    Listener* m_stateListener;
};

void AchievementsEvaluator::FlushAchievements() {
    if (m_stateListener) {
        for (blz::unordered_map<unsigned long, Achievement*>::const_iterator it = m_dirtyAchievements.begin();
             it != m_dirtyAchievements.end(); ++it) {
            AchievementUpdateRecord* achievement = it->second;
            m_stateListener->OnAchievementUpdated(achievement);
        }
        m_dirtyAchievements.clear();
    }
}
}  // namespace AchievementsEngine
}  // namespace OnlineService
