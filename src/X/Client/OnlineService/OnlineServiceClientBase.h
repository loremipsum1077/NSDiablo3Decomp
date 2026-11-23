// Adopted from the DWARF reference; hand-maintained digest and quest value types.
// Original file: X/Client/OnlineService/OnlineServiceClientBase.h
#pragma once

#include "../../Common/Act.h"
#include "../../Common/Definitions/HeroDefinition.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/PlayerAccount.h"
#include "../../Common/VisualInventory.h"
#include "../../Sigma/DataStructures/XOptional.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"

namespace OnlineService { struct QuestId; }
namespace blz {
// Header line 116; the body belongs to OnlineServiceClientBase.cpp:1545.
template <> struct hash<OnlineService::QuestId> {
    size_t operator()(const OnlineService::QuestId & tQuestEntry) const;
};
}

namespace OnlineService {
typedef blz::unordered_map<QuestId, int> QuestCompletionMap;

struct VisualDigest {  // 0x94
    // Line 143: the challenge-rift callback at 0x71003afd94 zeroes all 0x94 bytes.
    // The full callback is not yet matched; these three accessors use its inline evidence.
    VisualDigest() : m_isEquipmentValid(FALSE), m_visualEquipment() {}
    BOOL IsEquipmentValid() const { return m_isEquipmentValid; }
    const VisualEquipment & GetEquipment() const { return m_visualEquipment; }
    void SetEquipment(const VisualEquipment & equipment) {
        m_visualEquipment = equipment;
        m_isEquipmentValid = TRUE;
    }
private:
    BOOL m_isEquipmentValid;
    VisualEquipment m_visualEquipment;
};
static_assert(sizeof(VisualDigest) == 0x94, "VisualDigest");
struct SkillSummary {};

namespace Availability {
enum Enum { OFFLINE = 0, ONLINE_AVAILABLE = 1, ONLINE_AWAY = 2,
    ONLINE_BUSY = 3, ONLINE_INVISIBLE = 4, COUNT = 5, FIRST = 0 };
}
namespace CurrentActivity {
enum Enum { NONE = 0, HERO_SELECT = 1, HERO_CREATE = 2 };
}

struct HeroBasics {  // 0xb0
    GameAccountHandle m_id;  // 0x08
    CRefString m_nickname;  // 0x18
    HeroId m_selectedHeroId;  // 0x30
    CRefString m_selectedHeroName;  // 0x38
    Availability::Enum m_availability;  // 0x50
    CurrentActivity::Enum m_currentActivity;  // 0x54
    PlayerClass m_playerClass;  // 0x58
    BOOL m_isInvalid;  // 0x5c
    BOOL IsFemale() const;
    BOOL IsHardcore() const;
    uint32 GetPlayerFlags() const;
    void SetPlayerFlags(uint32 nFlags);
    CRefString GetClassName() const;
    int32 m_heroLevel;  // 0x60
    XOptional<int> m_altAdvancementLevel;  // 0x64
    int32 m_highestHeroSoloRiftCompleted;  // 0x6c
    XOptional<int> m_pvpRank;  // 0x70
    Act m_highestUnlockedAct;  // 0x78
    uint32 m_seasonCreated;  // 0x7c
    PlayerAccountBanner m_banner;  // 0x80
    HeroBasics();
    virtual ~HeroBasics() {}  // line 253
    HeroBasics(const HeroBasics &);
    HeroBasics & operator=(const HeroBasics & copy);
    void Copy(const HeroBasics & copy);
private:
    uint32 m_nFlags;  // 0xa8
};
static_assert(sizeof(HeroBasics) == 0xb0, "HeroBasics");

struct QuestId {  // line 459
    QuestId() : snoQuest(SNO_NONE) {}  // line 461
    SNO snoQuest;
};
static_assert(sizeof(QuestId) == 4, "QuestId");
inline bool operator==(const QuestId & a, const QuestId & right) {  // line 467
    return a.snoQuest == right.snoQuest;
}

struct DetailsDigest : public HeroBasics {  // 0xe0
    QuestCompletionMap * m_completedQuestMap;  // 0xb0
    SkillSummary m_skillSummary;  // 0xb8
    Act m_lastPlayedAct;  // 0xbc
    SNO m_lastPlayedQuest;  // 0xc0
    int32 m_lastPlayedQuestStep;  // 0xc4
    uint32 m_secondsPlayed;  // 0xc8
    Timestamp m_createdTimestamp;  // 0xd0
    Timestamp m_lastPlayedTimestamp;  // 0xd8
    DetailsDigest();
    virtual ~DetailsDigest();
    DetailsDigest(const DetailsDigest &);
    DetailsDigest & operator=(const DetailsDigest &);
    void Copy(const DetailsDigest & copy);
    void PopulateCompletedQuestList(const google::protobuf::RepeatedPtrField<D3::Hero::QuestHistoryEntry> & arQuestHistory) {  // line 542
        PlayerQuestHistoryEntry tEntry;
        for (int32 i = 0; i < arQuestHistory.size(); ++i) {
            tEntry.Import(arQuestHistory.Get(i));
            QuestId tQuestEntry;
            tQuestEntry.snoQuest = tEntry.snoQuest;
            (*m_completedQuestMap)[tQuestEntry] = tEntry.nStepUID;
        }
    }
    BOOL CanSelectQuests() const;
    BOOL IsQuestCompleted(const QuestId & tQuestEntry) const {  // line 569
        QuestCompletionMap::const_iterator it = m_completedQuestMap->find(tQuestEntry);
        // Original line 572 compares against -3 (completed quest step).
        return it != m_completedQuestMap->end() && it->second == -3;
    }
    int32 GetHighestProgressedStep(const QuestId & tQuestEntry) const {  // line 575
        QuestCompletionMap::const_iterator it = m_completedQuestMap->find(tQuestEntry);
        // Original line 578 returns -2 for a missing quest-history entry.
        return it != m_completedQuestMap->end() ? it->second : -2;
    }
};
static_assert(sizeof(DetailsDigest) == 0xe0, "DetailsDigest");

struct HeroDigest {  // 0x178
    VisualDigest m_visual;
    DetailsDigest m_details;  // 0x98
    void Import(const GameAccountHandle & gameAccountHandle, const CRefString & nickname,
                const D3::Hero::Digest & heroDigest, int32 nAltAvancementLevel);
};
static_assert(sizeof(HeroDigest) == 0x178, "HeroDigest");

struct PlayerDigest {  // 0x148
    VisualDigest m_visual;
    HeroBasics m_details;  // 0x98
    void Import(const D3::Profile::HeroMiniProfile &);
    void Copy(const HeroDigest &);
};
static_assert(sizeof(PlayerDigest) == 0x148, "PlayerDigest");
}
