// Adopted DWARF declarations; duplicate type DIEs consolidated.
// Original file: X/Common/OnlineService/Achievements.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Crafting/ItemCrafting.h"
#include "../ImageHandle.h"
#include "Callback/Callback_Impl.h"
#include "OnlineService.h"
#include "OnlineServiceErrorCodes.h"
#include "SubscriptionEvent.h"
#include "../../Sigma/DataStructures/XFourCC.h"
#include "../../Sigma/SigmaTypes.h"

#include <stddef.h>
#include <string.h>
#include <stdint.h>

class CRefString;
namespace D3 { namespace Achievements { class Snapshot; } }
namespace D3 { namespace AchievementsStaticData { class AchievementFile; } }
namespace D3 { namespace GameMessage { class AchievementsSnapshot; } }
namespace D3 { namespace OnlineService { class ContentHandle; } }
namespace OnlineService { namespace AchievementsEngine { struct Achievement; } }
namespace OnlineService { namespace AchievementsEngine { class AchievementsStaticData; } }
namespace OnlineService { namespace AchievementsEngine { struct Criteria; } }
namespace bgs { namespace protocol { class ContentHandle; } }

namespace OnlineService {
class AchievementsInterface;
typedef uint32 CategoryId;


typedef uint64 AchievementId;
const OnlineService::AchievementId INVALID_ACHIEVEMENT_ID = 0;  // DWARF Achievements.h:81


typedef uint32 RewardId;


typedef uint64 CriteriaId;


typedef uint64 CriteriaQuantity;


typedef uint32 CriteriaEventId;


typedef uint64 CriteriaComparand;


typedef uint32 CriteriaModifierConditionId;


typedef blz::vector<unsigned int, blz::allocator<unsigned int> > CategoryIdList;

typedef blz::vector<unsigned long, blz::allocator<unsigned long> > AchievementIdList;


typedef blz::vector<unsigned long, blz::allocator<unsigned long> > CriteriaIdList;


typedef blz::vector<unsigned int, blz::allocator<unsigned int> > RewardIdList;


enum AchievementFlag {
    ACHIEVEMENT_FLAG_STATISTIC = 0,
    ACHIEVEMENT_FLAG_CUMULATIVE = 1,
    ACHIEVEMENT_FLAG_TRACK_LOCAL_UNTIL_AWARDED = 2,
    ACHIEVEMENT_FLAG_HIDDEN = 8,
    ACHIEVEMENT_FLAG_HIDDEN_TILL_AWARDED = 9,
    ACHIEVEMENT_FLAG_NOTOAST = 10,
    ACHIEVEMENT_FLAG_DISPLAY_HIGHEST = 11,
    ACHIEVEMENT_FLAG_CRITERIA_COUNT = 12,
    ACHIEVEMENT_FLAG_SHOWCASEDEFAULT_0 = 13,
    ACHIEVEMENT_FLAG_SHOWCASEDEFAULT_1 = 14,
    ACHIEVEMENT_FLAG_SHOWCASEDEFAULT_2 = 15,
    ACHIEVEMENT_FLAG_HAS_PROGRESS_BAR = 16,
    ACHIEVEMENT_FLAG_HIDE_SERIES = 17,
    ACHIEVEMENT_FLAG_CONQUEST = 18,
    ACHIEVEMENT_FLAG_SHOW_IN_BANNER_UI = 19,
    ACHIEVEMENT_FLAG_GUARANTEE_REWARD = 20,
};


enum CategoryFlag {
    CATEGORY_FLAG_HIDDEN = 0,
    CATEGORY_FLAG_HIDDEN_TILL_ONE_AWARDED = 1,
    CATEGORY_FLAG_SEASON_JOURNEY = 2,
};

enum {
    REWARD_TYPE_BANNER_SIGIL = 0,
    REWARD_TYPE_BANNER_SIGIL_ACCENT = 1,
    REWARD_TYPE_BANNER_PATTERN = 2,
    REWARD_TYPE_BANNER_SHAPE = 3,
    REWARD_TYPE_BANNER_COLOR = 4,
    REWARD_TYPE_BANNER_EPIC_BANNER = 5,
    REWARD_TYPE_VANITY_TITLE = 6,
    REWARD_TYPE_ITEM_TRANSMOG = 7,
    REWARD_TYPE_ITEM_CURRENCY = 8,
    REWARD_TYPE_COSMETIC = 9,
    REWARD_TYPE_ITEM = 10,
    REWARD_TYPE_STASH_TAB = 11,
};


typedef int32 RewardType;


enum CriteriaFlag {
    CRITERIA_FLAG_FAIL_ACHIEVEMENT = 0,
    CRITERIA_FLAG_RESET_ON_START = 1,
    CRITERIA_FLAG_PROGRESS_BAR = 8,
    CRITERIA_FLAG_HIDDEN = 9,
    CRITERIA_FLAG_IS_ACHIEVEMENT_ID = 10,
    CRITERIA_FLAG_RETRIEVE_NAME_FROM_STRINGLIST = 11,
};


struct CategoryDefinition {  // size 0x18
    CategoryDefinition();
    OnlineService::CategoryId m_id;  // 0x00
    OnlineService::CategoryId m_parentCategoryId;  // 0x04
    uint32 m_flags;  // 0x08
    uint32 m_uiOrder;  // 0x0c
    const char * m_pszNameKey;  // 0x10
};
static_assert(sizeof(CategoryDefinition) == 0x18, "CategoryDefinition");

inline CategoryDefinition::CategoryDefinition()
{
}

struct AchievementDefinition {  // size 0x78
    AchievementDefinition();
    OnlineService::AchievementId m_id;  // 0x00
    OnlineService::AchievementId m_supersedes;  // 0x08
    OnlineService::CategoryId m_parentCategoryId;  // 0x10
    uint32 m_pointsValue;  // 0x14
    uint32 m_flags;  // 0x18
    uint32 m_uiOrder;  // 0x1c
    uint32 m_partitionMask;  // 0x20
    uint32 m_nMinRequiredCriteria;  // 0x24
    uint32 m_seasonMask;  // 0x28
    OnlineService::AchievementId m_sharesCriteria;  // 0x30
    const char * m_pszNameKey;  // 0x38
    const char * m_pszIconTexture;  // 0x40
    const char * m_pszDescriptionKey;  // 0x48
    uint32 m_nDeprecatedAsOf;  // 0x50
    uint32 m_nAvailableBeginningAt;  // 0x54
    uint32 m_profileAwardId;  // 0x58
    int32 m_profileAwardAmount;  // 0x5c
    OnlineService::RewardIdList m_rewards;  // 0x60
};
static_assert(sizeof(AchievementDefinition) == 0x78, "AchievementDefinition");

inline AchievementDefinition::AchievementDefinition()  // DWARF Achievements.h:181
{
    memset(this, 0, sizeof(*this));
}


struct CriteriaModifierOperand {  // size 0x1
    enum Enum {
        AND = 0,
        OR = 1,
        INVALID = 2,
    };
};
static_assert(sizeof(CriteriaModifierOperand) == 0x1, "CriteriaModifierOperand");


struct CriteriaModifier {  // size 0x18
    CriteriaModifier();
    bool operator==(const OnlineService::CriteriaModifier & other);
    OnlineService::CriteriaModifierConditionId m_necessaryCondition;  // 0x00
    XFourCC m_target;  // 0x04
    OnlineService::CriteriaModifierOperand::Enum m_operand;  // 0x08
    OnlineService::CriteriaComparand m_comparand;  // 0x10
};
static_assert(sizeof(CriteriaModifier) == 0x18, "CriteriaModifier");

inline bool CriteriaModifier::operator==(const OnlineService::CriteriaModifier & other)  // DWARF Achievements.h:247
{
    return m_necessaryCondition == other.m_necessaryCondition &&
           m_operand == other.m_operand &&
           m_comparand == other.m_comparand;
}

inline CriteriaModifier::CriteriaModifier()  // DWARF Achievements.h:240
    : m_necessaryCondition(0)
    , m_target()
    , m_operand(CriteriaModifierOperand::INVALID)
    , m_comparand(0)
{
}


struct CriteriaEvent {  // size 0x20
    CriteriaEvent();
    ~CriteriaEvent();
    void SetModifierCount(uint32 dwCount);
    bool operator==(const OnlineService::CriteriaEvent & other);
    OnlineService::CriteriaEvent & operator=(const OnlineService::CriteriaEvent &);
    OnlineService::CriteriaEventId m_id;  // 0x00
    uint32 m_usesComparand : 1;  // 0x04
    uint32 m_dwModifierCount : 31;  // 0x04
    uint32 m_timerDuration;  // 0x08
    OnlineService::CriteriaModifier * m_modifier;  // 0x10
    OnlineService::CriteriaComparand m_comparand;  // 0x18
};
static_assert(sizeof(CriteriaEvent) == 0x20, "CriteriaEvent");

inline CriteriaEvent::CriteriaEvent()
{
}

inline CriteriaEvent::~CriteriaEvent()  // DWARF Achievements.h:273
{
    SetModifierCount(0);
}

inline bool CriteriaEvent::operator==(const OnlineService::CriteriaEvent & other)  // DWARF Achievements.h:294
{
    if (m_dwModifierCount != other.m_dwModifierCount)
        return false;

    for (size_t i = 0; i < m_dwModifierCount; ++i)
    {
        if (!(m_modifier[i] == other.m_modifier[i]) || m_modifier[i].m_target.GetValue() != other.m_modifier[i].m_target.GetValue())
            return false;
    }

    if (m_usesComparand != other.m_usesComparand)
        return false;

    return m_id == other.m_id &&
           m_comparand == other.m_comparand;
}

inline void CriteriaEvent::SetModifierCount(uint32 dwCount)  // DWARF Achievements.h:278
{
    if (m_modifier)
    {
        delete[] m_modifier;
        m_modifier = NULL;
    }

    if (dwCount > 0)
    {
        m_modifier = new OnlineService::CriteriaModifier[dwCount];
    }
    m_dwModifierCount = dwCount;
}


struct CriteriaStartEvent : public OnlineService::CriteriaEvent {  // size 0x20
    CriteriaStartEvent();
    bool operator==(const OnlineService::CriteriaStartEvent & other);
};
static_assert(sizeof(CriteriaStartEvent) == 0x20, "CriteriaStartEvent");

inline CriteriaStartEvent::CriteriaStartEvent()
{
}

inline bool CriteriaStartEvent::operator==(const OnlineService::CriteriaStartEvent & other)  // DWARF Achievements.h:343
{
    if (m_dwModifierCount != other.m_dwModifierCount)
        return false;

    for (size_t i = 0; i < m_dwModifierCount; ++i)
    {
        if (!(m_modifier[i] == other.m_modifier[i]) || m_modifier[i].m_target.GetValue() != other.m_modifier[i].m_target.GetValue())
            return false;
    }

    return m_id == other.m_id &&
           m_comparand == other.m_comparand &&
           m_timerDuration == other.m_timerDuration;
}


struct CriteriaDefinition {  // size 0x98
    CriteriaDefinition();
    OnlineService::CriteriaId m_id;  // 0x00
    OnlineService::AchievementId m_parentAchievementId;  // 0x08
    OnlineService::CriteriaQuantity m_necessaryQuantity;  // 0x10
    OnlineService::CriteriaStartEvent m_startEvent;  // 0x18
    OnlineService::CriteriaEvent m_advanceEvent;  // 0x38
    OnlineService::CriteriaEvent m_failEvent;  // 0x58
    XFourCC m_evaluationClass;  // 0x78
    uint32 m_flags;  // 0x7c
    uint32 m_uiOrder;  // 0x80
    const char * m_pszNameKey;  // 0x88
    const char * m_pszStringlabelKey;  // 0x90
};
static_assert(sizeof(CriteriaDefinition) == 0x98, "CriteriaDefinition");

inline CriteriaDefinition::CriteriaDefinition()  // DWARF Achievements.h:363
{
    memset(this, 0, sizeof(*this));
}


struct RewardDefinition {  // size 0x28
    RewardDefinition()  // DWARF Achievements.h:401, in-class (struct 399, members 412-418); stores 402-408 in member order, 408:15 is the `=` of an assignment (no init list)
    {
        m_id = 0;
        m_type = 0;
        m_flags = 0;
        m_pszNameKey = NULL;
        m_hIcon = 0;
        m_currency = -1;  // CURRENCY_INVALID; Items.h is not included here
        m_quantity = 0;
    }
    OnlineService::RewardId m_id;  // 0x00
    OnlineService::RewardType m_type;  // 0x04
    uint32 m_flags;  // 0x08
    const char * m_pszNameKey;  // 0x10
    ImageHandle m_hIcon;  // 0x18
    CurrencyType m_currency;  // 0x1c
    uint32 m_quantity;  // 0x20
};
static_assert(sizeof(RewardDefinition) == 0x28, "RewardDefinition");




struct CriteriaRecord {  // size 0x18
    OnlineService::CriteriaId m_criteriaId;  // 0x00
    Blizzard::Time::Timestamp m_timestampStarted;  // 0x08
    OnlineService::CriteriaQuantity m_quantity;  // 0x10
};
static_assert(sizeof(CriteriaRecord) == 0x18, "CriteriaRecord");

struct AchievementRecord {  // size 0x10
    AchievementRecord();
    AchievementRecord(OnlineService::AchievementId, Blizzard::Time::Timestamp);
    OnlineService::AchievementId m_achievementId;  // 0x00
    Blizzard::Time::Timestamp m_timestampEarned;  // 0x08
};
static_assert(sizeof(AchievementRecord) == 0x10, "AchievementRecord");

inline AchievementRecord::AchievementRecord()  // DWARF Achievements.h:430
{
    m_achievementId = 0;
    m_timestampEarned = 0;
}


struct ClientSnapshot {  // size 0x30
    blz::vector<OnlineService::CriteriaRecord, blz::allocator<OnlineService::CriteriaRecord> > m_criteriaRecords;  // 0x00
    blz::vector<OnlineService::AchievementRecord, blz::allocator<OnlineService::AchievementRecord> > m_achievementRecords;  // 0x18
};
static_assert(sizeof(ClientSnapshot) == 0x30, "ClientSnapshot");

struct CachedSnapshot {  // size 0x78
    typedef blz::unordered_map<unsigned long, unsigned long, blz::hash<unsigned long>, blz::equal_to<unsigned long>, blz::allocator<blz::pair<const unsigned long, unsigned long> > > AchievementMap;
    typedef blz::unordered_map<unsigned long, unsigned long, blz::hash<unsigned long>, blz::equal_to<unsigned long>, blz::allocator<blz::pair<const unsigned long, unsigned long> > > CriteriaMap;
    Blizzard::Time::Second m_timestamp;  // 0x00
    OnlineService::ClientSnapshot m_clientSnapshot;  // 0x08
    OnlineService::CachedSnapshot::AchievementMap m_achievementMap;  // 0x38
    OnlineService::CachedSnapshot::CriteriaMap m_criteriaMap;  // 0x58
};
static_assert(sizeof(CachedSnapshot) == 0x78, "CachedSnapshot");

class Achievements {  // size 0x118
    // vtable pointer
public:
    Achievements();
    virtual ~Achievements();  // vtable slot 0
    void Enable(bool);
    bool IsEnabled() const;
    virtual bool IsInitialized();  // vtable slot 2
    virtual bool IsStaticDataLoaded();  // vtable slot 3
    virtual void Process() = 0;  // vtable slot 4
    virtual void LoadAchievementsDataByContentHandle(const bgs::protocol::ContentHandle &);  // vtable slot 5
    void LoadAchievementsDataByProtobufContentHandle(const D3::OnlineService::ContentHandle &);
    static const uint32 ACHIEVEMENTS_NUMBER_SHARDS_DEFAULT = 10;
    static void RegisterSyncedVars();
    static uint32 GetNumShards();
    static void SetNumShards(uint32);
    static uint32 GetShardForAccount(const OnlineService::GameAccountHandle &);
    bool ShouldCountAchievementPoints(const OnlineService::AchievementDefinition * ptAchievementDef) const;
    blz::string GetTimestamp() const;
    bool GetChildCategories(OnlineService::CategoryId id, OnlineService::CategoryIdList & idList) const;
    bool GetCategoryAchievements(OnlineService::CategoryId id, OnlineService::AchievementIdList & idList) const;
    bool GetAchievementCriteria(OnlineService::AchievementId id, OnlineService::CriteriaIdList & idList) const;
    bool GetAchievementRewards(OnlineService::AchievementId id, OnlineService::RewardIdList & idList) const;
    const OnlineService::CategoryDefinition * GetCategoryDefinition(OnlineService::CategoryId id) const;
    const OnlineService::AchievementDefinition * GetAchievementDefinition(OnlineService::AchievementId id) const;
    const OnlineService::CriteriaDefinition * GetCriteriaDefinition(OnlineService::CriteriaId id) const;
    bool GetRewardDefinition(OnlineService::RewardId id, OnlineService::RewardDefinition & tReward) const;
    bool GetRewardParentAchievements(OnlineService::RewardId id, OnlineService::AchievementIdList & idList) const;
    void GetAllRewardsByType(OnlineService::RewardType eType, OnlineService::RewardIdList & idList, uint32 nSeasonId) const;
    void GetAllRewardsOfTypes(uint32 dwTypes, OnlineService::RewardIdList & idList, uint32 nSeasonId) const;
    BOOL IsSeasonJourneyAchievement(OnlineService::AchievementId achievementId) const;
    BOOL IsSeasonJourneyAchievementObjective(OnlineService::AchievementId achievementId) const;
    BOOL GetAchievementObjectiveParent(OnlineService::AchievementId achievementId, OnlineService::AchievementId & achievementIdParent);
    void GetConquestsForSeason(OnlineService::AchievementIdList &, uint32);
    int32 GetSeasonCycleLength() const;
    int32 GetSeasonDataPartition(uint32 seasonNum) const;
    blz::vector<unsigned int, blz::allocator<unsigned int> > GetPersistLoadSnapshotPartitions();
    BOOL IsAchievementValidForSeason(const OnlineService::AchievementDefinition * ptAchievementDef, uint32 nSeasonId) const;
    BOOL IsAchievementHardcore(OnlineService::AchievementId) const;
    BOOL IsAchievementHardcoreByCriteria(OnlineService::AchievementId idAchievement) const;
    OnlineService::AchievementId GetSeasonJourneyRankSeriesTail(uint32 nSeasonId) const;
    OnlineService::AchievementId GetHighestEarnedSeasonJourneyAchievement(uint32 nSeasonId, const OnlineService::GameAccountHandle & gameAccountId, OnlineService::AchievementsInterface * ptAchievementsInterface);
    OnlineService::CategoryId GetTopMostCategory(OnlineService::CategoryId) const;
    bool IsAchievementInSeries(OnlineService::AchievementId achievementId) const;
    OnlineService::AchievementId GetSeriesTail(OnlineService::AchievementId achievementId) const;
    OnlineService::AchievementId GetSeriesHead(OnlineService::AchievementId) const;
    void NotifyAchievementsStatus(BOOL);
    typedef OnlineService::SubscriptionEvent<OnlineService::AchievementsInterface *, OnlineService::GameAccountHandle> CacheInvalidatedEventType;
    OnlineService::Achievements::CacheInvalidatedEventType & CacheInvalidatedEvent();
    typedef OnlineService::SubscriptionEvent<OnlineService::AchievementsInterface *, OnlineService::GameAccountHandle, unsigned long> AchievementUpdateEventType;
    OnlineService::Achievements::AchievementUpdateEventType & AchievementUpdateEvent();
    typedef OnlineService::SubscriptionEvent<OnlineService::AchievementsInterface *, OnlineService::GameAccountHandle, unsigned long> CriteriaUpdateEventType;
    OnlineService::Achievements::CriteriaUpdateEventType & CriteriaUpdateEvent();
    typedef OnlineService::SubscriptionEvent<int> AchievementsStatusEventType;
    OnlineService::Achievements::AchievementsStatusEventType & AchievementsStatusEvent();
    typedef OnlineService::SubscriptionEvent<blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> > > AchievementStaticDataUpdatedEventType;
    OnlineService::Achievements::AchievementStaticDataUpdatedEventType & AchievementStaticDataUpdatedEvent();
    virtual OnlineService::AchievementsInterface * CreateAchievementsInterface() = 0;  // vtable slot 6
    virtual void DestroyAchievementsInterface(OnlineService::AchievementsInterface *) = 0;  // vtable slot 7
    virtual OnlineService::AchievementsInterface * CreateUtilityAchievementsInterface();  // vtable slot 8
    void OnStaticDataUpdated(const blz::string &);
    OnlineService::AchievementsEngine::AchievementsStaticData * GetStaticDataFile() const { return m_pStaticDataFile; } // line 682
    virtual void GetStaticDataContentHandle(D3::OnlineService::ContentHandle & handle);  // vtable slot 9
    static CRefString GetLooseFilenameConsole();
    static CRefString GetLooseFilenamePC();
    static CRefString GetMPQFilename();
protected:
    static bool InsertReward(OnlineService::AchievementsEngine::AchievementsStaticData * staticData, const OnlineService::RewardDefinition & tReward, OnlineService::AchievementId achievementId);
    static void LoadAchievementsRewards(OnlineService::AchievementsEngine::AchievementsStaticData * staticData, const D3::AchievementsStaticData::AchievementFile & achievementFile);
    static void CreateBannerTexturePairReward(OnlineService::AchievementsEngine::AchievementsStaticData * staticData, OnlineService::RewardType eRewardType, const char * pszSnoName, OnlineService::AchievementId achId, uint32 uRewardFlags);
    static void CreateBannerColorSetReward(OnlineService::AchievementsEngine::AchievementsStaticData * staticData, const char * pszSnoName, OnlineService::AchievementId achId, uint32 uRewardFlags);
    static void CreateItemTransmogReward(OnlineService::AchievementsEngine::AchievementsStaticData * staticData, const char * pszItemName, OnlineService::AchievementId achId, uint32 uRewardFlags);
    static bool CreateItemRewardCurrency(OnlineService::AchievementsEngine::AchievementsStaticData * staticData, CurrencyType eCurrencyType, uint32 nQuantity, OnlineService::AchievementId achId, uint32 uRewardFlags);
    static void CreateCosmeticReward(OnlineService::AchievementsEngine::AchievementsStaticData * staticData, const char * pszItemName, OnlineService::AchievementId achId, uint32 uRewardFlags);
    static void CreateItemReward(OnlineService::AchievementsEngine::AchievementsStaticData * staticData, const char * pszItemName, OnlineService::AchievementId achId, uint32 uRewardFlags);
    static void CreateStashTabReward(OnlineService::AchievementsEngine::AchievementsStaticData *, const char *, OnlineService::AchievementId, uint32);
    bool LoadAchievementsDataRaw(const uint8 * pData, size_t dwDataSize, const blz::string & contentHandleHash);
    bool LoadAchievementsDataByLooseFile();
    static void PostprocessStaticData(OnlineService::AchievementsEngine::AchievementsStaticData * staticData);
    static void ProcessSeasonJourneyObjectives(OnlineService::AchievementsEngine::AchievementsStaticData * ptStaticData);
    OnlineService::Achievements::CacheInvalidatedEventType m_CacheInvalidatedEvent;  // 0x08
    OnlineService::Achievements::AchievementUpdateEventType m_AchievementUpdateEvent;  // 0x38
    OnlineService::Achievements::CriteriaUpdateEventType m_CriteriaUpdateEvent;  // 0x68
    OnlineService::Achievements::AchievementsStatusEventType m_AchievementsStatusEvent;  // 0x98
    OnlineService::Achievements::AchievementStaticDataUpdatedEventType m_AchievementStaticDataUpdatedEvent;  // 0xc8
    bool m_bStaticDataLoaded;  // 0xf8
    OnlineService::AchievementsEngine::AchievementsStaticData * m_pStaticDataFile;  // 0x100
    OnlineService::AchievementsEngine::AchievementsStaticData * m_pOldStaticDataFile;  // 0x108
public:
    bool m_bEnabled;  // 0x110
    static int s_nextRewardId;
};
static_assert(sizeof(Achievements) == 0x118, "Achievements");

class AchievementsInterface {  // size 0x10
    // vtable pointer
public:
    typedef OnlineService::Callback<void (int)> RegistrationCallback;
    AchievementsInterface(OnlineService::Achievements *);
    virtual ~AchievementsInterface();  // vtable slot 0
    virtual void Process() = 0;  // vtable slot 2
    virtual void Flush(const OnlineService::GameAccountHandle &);  // vtable slot 3
    virtual void RegisterSubscription(OnlineService::AccountId, const OnlineService::GameAccountHandle &, const OnlineService::AchievementsInterface::RegistrationCallback &) = 0;  // vtable slot 4
    virtual bool CancelSubscription(const OnlineService::GameAccountHandle &, const OnlineService::AchievementsInterface::RegistrationCallback &) = 0;  // vtable slot 5
    typedef OnlineService::Callback<bool (unsigned long, unsigned long)> ModifierCallback;
    virtual OnlineService::ErrorCode EvaluateEvent(const OnlineService::GameAccountHandle & gameAccount, uint32 eventId, uint64 quantity, OnlineService::Timestamp timestamp, const OnlineService::AchievementsInterface::ModifierCallback & cb);  // vtable slot 6
    virtual OnlineService::ErrorCode EvaluateEvent(const OnlineService::GameAccountHandle & gameAccount, uint32 eventId, uint64 comparand, uint64 quantity, OnlineService::Timestamp timestamp, const OnlineService::AchievementsInterface::ModifierCallback & cb);  // vtable slot 7
    typedef OnlineService::Callback<void (int, const OnlineService::GameAccountHandle &)> SnapshotCallback;
    virtual void SetSnapshotCacheSize(uint64) = 0;  // vtable slot 8
    virtual void SetSnapshotExpireSeconds(double) = 0;  // vtable slot 9
    virtual void RequestSnapshot(const OnlineService::GameAccountHandle &, const OnlineService::AchievementsInterface::SnapshotCallback &) = 0;  // vtable slot 10
    virtual bool GetSnapshot(const OnlineService::GameAccountHandle &, D3::Achievements::Snapshot &) = 0;  // vtable slot 11
    virtual void OnAchievementsSnapshotReceived(D3::GameMessage::AchievementsSnapshot &) = 0;  // vtable slot 12
    virtual bool FindAwardedAchievement(const OnlineService::GameAccountHandle &, OnlineService::AchievementId, OnlineService::AchievementRecord &) = 0;  // vtable slot 13
    virtual bool FindAwardedAchievements(const OnlineService::GameAccountHandle &, blz::vector<OnlineService::AchievementRecord, blz::allocator<OnlineService::AchievementRecord> > &) = 0;  // vtable slot 14
    virtual bool FindCriteriaRecord(const OnlineService::GameAccountHandle &, OnlineService::CriteriaId, OnlineService::CriteriaRecord &) = 0;  // vtable slot 15
    virtual bool IsReadyForDeletion();  // vtable slot 16
    virtual uint32 GetPartitionMask();  // vtable slot 17
    static void ConvertAchievementRecord(const OnlineService::AchievementsEngine::Achievement & bnetRecord, OnlineService::AchievementRecord & osRecord);
    static void ConvertCriteriaRecord(const OnlineService::AchievementsEngine::Criteria & bnetRecord, OnlineService::CriteriaRecord & osRecord);
protected:
    OnlineService::Achievements * m_ptAchievements;  // 0x08
};
static_assert(sizeof(AchievementsInterface) == 0x10, "AchievementsInterface");

// Inline bodies (Achievements.h:496, 671, 684, 800, 887, 892); their out-of-line copies are
// emitted with Console::Achievements' vtables (ConsoleAchievements.cpp).
inline void Achievements::LoadAchievementsDataByContentHandle(const bgs::protocol::ContentHandle &) {}
inline OnlineService::AchievementsInterface * Achievements::CreateUtilityAchievementsInterface() { return NULL; }
inline void Achievements::GetStaticDataContentHandle(D3::OnlineService::ContentHandle & handle) {}
inline void AchievementsInterface::Flush(const OnlineService::GameAccountHandle &) {}
inline bool AchievementsInterface::IsReadyForDeletion() { return true; }
inline uint32 AchievementsInterface::GetPartitionMask() { return 1; }
inline bool Achievements::IsEnabled() const { return m_bEnabled; }  // line 485


} // namespace OnlineService
