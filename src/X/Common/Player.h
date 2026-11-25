// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Player.h
// Every Player and PlayerClientInfo data member is the DWARF member (no opaque storage left); member functions and
// the rest of the reference header are added as callers need them: the full reference header pulls in most of the game.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/list.h"
#include "CommonTypes.h"
#include "CosmeticItems.h"
#include "Crafting/ItemCrafting.h"
#include "Definitions/HeroDefinition.h"
#include "Hireling.h"
#include "OnlineService/OnlineService.h"
#include "PlayerAccount.h"
#include "Protobuf/Account.pb.h"
#include "Protobuf/Items.pb.h"
#include "Trade.h"
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/DataStructures/XPooledMap.h"
#include "Console/Avenger.h"
#include "Powers/PowersStruct.h"
#include "Target.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Random.h"
#include "../../BlizzardCore/Include/Blizzard/Util/BitField.h"
#include "../Sigma/Math/Quaternion.h"
#include "../../Contrib/Contrib/blz/include/blz/unordered_map.h"
typedef int32 GameConnectionID;  // X/Server/PlayerList.h
typedef int32 SNO;  // X/Common/SNO/SNOFiles.h
typedef int32 GBID;  // X/Common/GameBalance.h
typedef int32 RuneType;  // X/Common/Definitions/AttachmentDefinition.h
typedef int32 CrafterType;  // X/Common/Crafting/ItemCraftingSwig.h
typedef int32 PlayerClass;  // X/Common/GameSwig.h
typedef int32 Team;  // X/Common/TeamCommon.h
typedef int32 InventorySlotType;  // build/dwarf/headers/X/Common/Player.h:65

struct LoadoutSkillData {  // size 0x8
    SNO snoPower;  // 0x00
    RuneType eRune;  // 0x04
};
static_assert(sizeof(LoadoutSkillData) == 0x8, "LoadoutSkillData");

struct LoadoutItemData {  // size 0x30
    ACDNetworkName annItem;  // 0x00
    uint64 uOriginalItemIdLow;  // 0x08
    GBID gbidSockets[3];  // 0x10
    int32 nSocketsCount;  // 0x1c
    int32 unk_20;  // 0x20
    uint64 uSoulShardIdLow;  // 0x28
};
static_assert(sizeof(LoadoutItemData) == 0x30, "LoadoutItemData");

struct LoadoutPotionData {  // size 0x18
    ACDNetworkName annPotion;  // 0x00
    uint64 uOriginalItemIdLow;  // 0x08
    GBID gbidPotion;  // 0x10
};
static_assert(sizeof(LoadoutPotionData) == 0x18, "LoadoutPotionData");

struct LoadoutSavedData {  // size 0x318
    void Import(const D3::Hero::Loadout & tSavedLoadout); // DWARF Player.h:827; Header inline Initialize below:
    void Export(D3::Hero::Loadout & tSavedLoadout, ACDID idACDPlayer) const; // DWARF Player.h:824; Initialize body from ClientMessageReceive.cpp:8588.
// Initialize: DWARF Player.h:788; loop counters i declared at :791, :798, :804, :809 (cu_04439fcf abstract
// lexical blocks); line table stores at :793-795, :806 (merged 0xff memset), :814, :816-818 and bEmpty at :820.
#line 788
    void Initialize()
    {

        for (int32 i = 0; i < NUM_PLAYER_BODYSLOTS; i++)
        {
            tEquipment[i].annItem = ANN_INVALID;
            tEquipment[i].uOriginalItemIdLow = 0;
            tEquipment[i].nSocketsCount = 0;
            tEquipment[i].unk_20 = -1;
            tEquipment[i].uSoulShardIdLow = 0;
        }

        for (int32 i = 0; i < NUM_SKILL_SLOTS; i++)
        {
            tActiveSkills[i].snoPower = SNO_NONE;
            tActiveSkills[i].eRune = -1;
        }

        for (int32 i = 0; i < NUM_TRAIT_SLOTS; i++)
        {
            snoTraits[i] = SNO_NONE;
        }

        for (int32 i = 0; i < 4; i++)
        {
            gbidLegendaryPowers[i] = GBID_NONE;
        }

        SigmaMemoryClear(usName, sizeof(usName));

        tEquippedPotion.annPotion = ANN_INVALID;
        tEquippedPotion.uOriginalItemIdLow = 0;
        tEquippedPotion.gbidPotion = GBID_NONE;

        bEmpty = TRUE;
    }
#line 88
    LoadoutItemData tEquipment[NUM_PLAYER_BODYSLOTS];  // 0x00
    LoadoutPotionData tEquippedPotion;  // 0x270
    LoadoutSkillData tActiveSkills[6];  // 0x288
    SNO snoTraits[4];  // 0x2b8
    GBID gbidLegendaryPowers[4];  // 0x2c8
    CHAR usName[53];  // 0x2d8
    int32 nTabIcon;  // 0x310
    BOOL bEmpty;  // 0x314
};
static_assert(sizeof(LoadoutSavedData) == 0x318, "LoadoutSavedData");

struct PlayerTalkData {  // size 0x4
    ACDID idACDTalkingTo;  // 0x00
    void Reset();  // Player.cpp:695
};
static_assert(sizeof(PlayerTalkData) == 0x4, "PlayerTalkData");

struct PlayerQuestData {  // size 0x4
    SSceneID idSSceneLast;  // 0x00
};
static_assert(sizeof(PlayerQuestData) == 0x4, "PlayerQuestData");

enum {
    PSF_IN_AV_CAMERA_MODE = 0,
    PSF_ELIGIBLE_FOR_QUEST_STATE = 1,
    PSF_INELIGIBLE_FOR_SURVIVOR_AWARD = 2,
    PSF_COUNT = 3,
};

typedef int32 Act;  // X/Common/Act.h

struct BannerRenderData {  // size 0x30
// Initialize: DWARF Player.h:613; loop counters i at :616, :620, :625 (cu_04439fcf abstract lexical blocks);
// line table stores at :618 (banner textures -1), :624 (tints and look 0); attachments -1 merge into the caller's stores.
#line 613
    void Initialize()
    {

        for (int32 i = 0; i < 4; i++)
        {
            snoTexBanners[i] = SNO_NONE;
        }
        for (int32 i = 0; i < 3; i++)
        {
            flTintU[i] = 0.0f;
        }
        nLook = 0;
        for (int32 i = 0; i < 4; i++)
        {
            snoActorAttachments[i] = SNO_NONE;
        }
    }
#line 121
    SNO snoTexBanners[4];  // 0x00
    float flTintU[3];  // 0x10
    int32 nLook;  // 0x1c
    SNO snoActorAttachments[4];  // 0x20
};
static_assert(sizeof(BannerRenderData) == 0x30, "BannerRenderData");

struct HeroStateData {  // size 0x47cc; X/Common/Player.h:508
    void SetDefaults();
    Act eLastPlayedAct;  // 0x00
    Act eHighestUnlockedAct;  // 0x04
    uint32 uPlayerFlags;  // 0x08
    uint32 uPlayerFlags2;  // 0x0c
    PlayerSavedData tSavedData;  // 0x10
    int32 nNumQuestRewardEntries;  // 0x12c0
    PlayerQuestRewardHistoryEntry tQuestRewardHistory[100];  // 0x12c4
    PlayerQuestHistoryEntry tQuestHistory[200];  // 0x1774
    PlayerSavedQuest tSavedQuest[64];  // 0x1db4
    uint32 uBossKillFlags[6];  // 0x47b4
};
static_assert(sizeof(HeroStateData) == 0x47cc, "HeroStateData");
static_assert(offsetof(HeroStateData, uPlayerFlags) == 0x08, "HeroStateData");
static_assert(offsetof(HeroStateData, tSavedData) == 0x10, "HeroStateData");
static_assert(offsetof(HeroStateData, nNumQuestRewardEntries) == 0x12c0, "HeroStateData");
static_assert(offsetof(HeroStateData, tQuestRewardHistory) == 0x12c4, "HeroStateData");
static_assert(offsetof(HeroStateData, tQuestHistory) == 0x1774, "HeroStateData");
static_assert(offsetof(HeroStateData, tSavedQuest) == 0x1db4, "HeroStateData");
static_assert(offsetof(HeroStateData, uBossKillFlags) == 0x47b4, "HeroStateData");

struct CurrencyData {  // size 0x18; X/Common/Player.h
    CurrencyType eType;  // 0x00
    int64 nCount;  // 0x08
    uint32 uFlags;  // 0x10
};
static_assert(sizeof(CurrencyData) == 0x18, "CurrencyData");
static_assert(offsetof(CurrencyData, nCount) == 0x08, "CurrencyData");
static_assert(offsetof(CurrencyData, uFlags) == 0x10, "CurrencyData");

struct CurrencySavedData {  // size 0x210; X/Common/Player.h:678
    enum ExportPartitionScope {
        EXPORT_PARTITION_ACCOUNT_WIDE = 0,
        EXPORT_PARTITION_INDIVIDUAL = 1,
        EXPORT_PARTITION_IGNORE = 2,
    };
    enum ExportDirtyScope {
        EXPORT_DIRTY_PERSIST = 0,
        EXPORT_DIRTY_CLIENTSYNC = 1,
        EXPORT_DIRTY_IGNORE = 2,
    };
    CurrencySavedData();  // Player.h:694
    void SetToDefaults();
    void Export(D3::Items::CurrencySavedData & tCurrencySavedData, CurrencySavedData::ExportPartitionScope ePartitionScope, CurrencySavedData::ExportDirtyScope eDirtyScope, BOOL bExportFlags, uint32 uFlagsToClear);
    void Import(const D3::Items::CurrencySavedData & tCurrencySavedData, BOOL bResetCurrency);
    void Import(const D3::Items::CurrencyData & tCurrencyData);
    CurrencyData arCurrency[NUM_CURRENCY_TYPES];  // 0x00
};
static_assert(sizeof(CurrencySavedData) == 0x210, "CurrencySavedData");

namespace UIInventory { namespace Console { template <typename T> class NewItemIndicatorManager; } }

class CheckpointData {  // size 0x20; X/Common/Player.h:558
public:
    WorldPlace place;  // 0x00
    Quaternion qOrientation;  // 0x10
    void Initialize()  // line 564
    {
        place.idSWorld = SWORLDID_INVALID;
    }
    BOOL IsValid() const  // line 570
    {
        return place.idSWorld != SWORLDID_INVALID;
    }
};
static_assert(sizeof(CheckpointData) == 0x20, "CheckpointData");

class HearthPortalData {  // size 0x18
public:
    WorldPlace place;  // 0x00
    SNO snoLastWaypointLevelArea;  // 0x10
    SNO snoPortalLevelArea;  // 0x14
    void Initialize()  // line 585
    {
        place.idSWorld = SWORLDID_INVALID;
        snoLastWaypointLevelArea = snoPortalLevelArea = SNO_NONE;
    }
    BOOL IsValid() const;  // Player.cpp
};
static_assert(sizeof(HearthPortalData) == 0x18, "HearthPortalData");

struct HandicapLevelLimitation {  // size 0x8
    int32 m_nRequiredLevel;  // 0x00
    int32 m_nRequiredHighestSoloRiftLevel;  // 0x04
};
static_assert(sizeof(HandicapLevelLimitation) == 0x8, "HandicapLevelLimitation");

typedef int32 X1HandicapLevel;  // X/Common/Player.h

enum {
    PCSF_SAVED_QUESTS = 0,
    PCSF_QUEST_HISTORY = 1,
    PCSF_QUEST_REWARD_HISTORY = 2,
    PCSF_SAVED_CONVERSATIONS = 3,
    PCSF_ACTIVE_HIRELING = 4,
    PCSF_SAVED_QUESTS_ELIGIBLE_IN_GAME_LONG_ENOUGH = 5,
    PCSF_PARTITION_FLAGS = 6,
    PCSF_RANDOM_SEEDS = 7,
    PCSF_PARTITION_WIDE_CUBE_SEEDS = 8,
    PCSF_AMIIBO_SCAN_TIMES = 9,
};

struct KillCounter {  // size 0x38
    void Reset();
    uint32 uTotalCount;  // 0x00
    uint32 uCountGrantingXP;  // 0x04
    uint32 uLastTick;  // 0x08
    BOOL bNewRecord;  // 0x0c
    uint32 uStartTime;  // 0x10
    BOOL bExpired;  // 0x14
    uint64 nTotalXP;  // 0x18
    uint32 uLastMessageSentTime;  // 0x20
    uint32 uLastMessageCount;  // 0x24
    uint32 uMassacreTier;  // 0x28
    BOOL bStartingTimerActive;  // 0x2c
    uint32 uPreviousLastTick;  // 0x30
    uint32 uLastUpdate;  // 0x34
};
static_assert(sizeof(KillCounter) == 0x38, "KillCounter");

struct CurrencyCounter {  // size 0x10
    void Reset();
    uint32 uCount;  // 0x00
    uint32 uLastCount;  // 0x04
    uint32 uLastTick;  // 0x08
    uint32 uLastUpdate;  // 0x0c
};
static_assert(sizeof(CurrencyCounter) == 0x10, "CurrencyCounter");

typedef int32 PetType;  // X/Common/Player.h

enum {
    MAX_HERO_FILENAME_LENGTH = 32,
    MAX_PETS = 64,
    MAX_TUTORIAL_BITS = 384,
};  enum { VE_LOW = 0, VE_MODERATE = 1, VE_SERIOUS = 2, NUM_VALIDATION_ERROR_TYPES = 3 };  // X/Common/Player.h:77

struct PlayerPet {  // size 0x8
    ACDID idACD;  // 0x00
    PetType eType;  // 0x04
};
static_assert(sizeof(PlayerPet) == 0x8, "PlayerPet");

enum {
    KILLCOUNTER_MONSTER = 0,
    KILLCOUNTER_OBJECT_DESTRUCTION = 1,
    KILLCOUNTER_SINGLE_ATTACK = 2,
    KILLCOUNTER_ENVIRONMENT_ATTACK = 3,
    KILLCOUNTER_COUNT = 4,
};

typedef int32 KillCounterType;

enum {
    FLOATING_NUMBER_TYPE_DAMAGE_DONE = 0,
    FLOATING_NUMBER_TYPE_DOT_DAMAGE_DONE = 1,
    FLOATING_NUMBER_TYPE_CRITICAL_HIT_DONE = 2,
    FLOATING_NUMBER_TYPE_DAMAGE_TAKEN = 3,
    FLOATING_NUMBER_TYPE_DOT_DAMAGE_TAKEN = 4,
    FLOATING_NUMBER_TYPE_CRITICAL_HIT_TAKEN = 5, FLOATING_NUMBER_TYPE_HEAL = 10,
};

enum {
    FLOATING_NUMBER_TYPE_GOLD = 29,
    FLOATING_NUMBER_TYPE_BLOODSHARDS = 30,
    FLOATING_NUMBER_TYPE_PLATINUM = 31,
};

typedef int32 FloatingNumberType;

struct Player;
class ActorCommonData;
struct PowerDefinition;

struct PlayerQueuedPower {  // size 0x48
    void SetDefaults();
    void Clear();
    uint32 dwPowerLastStartTime;  // 0x00
    uint32 dwPowerLastInterruptTime;  // 0x04
    uint32 dwPowerLastEndTime;  // 0x08
    SNO snoQueuedPower;  // 0x0c
    uint32 dwQueuedTime;  // 0x10
    Target tQueuedTarget;  // 0x14
    int32 nQueuedComboLevel;  // 0x30
    ACDID idACDQueuedItemBeingUsed;  // 0x34
    PreplayedAnimData tPreplayedAnimData;  // 0x38
    uint32 dwQueuedTargetMessageFlags;  // 0x44
};
static_assert(sizeof(PlayerQueuedPower) == 0x48, "PlayerQueuedPower");

struct EnchantAffixChoice {  // size 0x8; X/Common/Items.h:343
    GBID gbidAffix;  // 0x00
    uint32 dwSeed;  // 0x04
};
static_assert(sizeof(EnchantAffixChoice) == 0x8, "EnchantAffixChoice");

struct PlayerVendorData {  // size 0x3c
    void Initialize();
    void VendorReset();
    ACDID idACDEnchantAffixPendingItem;  // 0x00
    GBID gbidEnchantAffixPendingRemoved;  // 0x04
    EnchantAffixChoice tEnchantAffixChoices[6];  // 0x08
    int32 nEnchantAffixChoicesCount;  // 0x38
};
static_assert(sizeof(PlayerVendorData) == 0x3c, "PlayerVendorData");

class ConsoleTargetComponent {  // size 0x40
public:
    enum TargetLockType {
        TARGETLOCK_SOFT = 0,
        TARGETLOCK_HARD = 1,
        TARGETLOCK_PERSISTENT = 2,
        TARGETLOCK_STICKY = 3,
        TARGETLOCK_NONE = 4,
    };
public:
    ACDID GetLastTarget() const;
    ActorCommonData * ResolveStickyTarget();
// SetLastTarget: DWARF Player.h:958, both stores attributed to :958 in ResolveStickyTarget's line table. GetLastTargetInSecondaryRange :959, load of +0x18 inlined at ConsoleTargeting.cpp:969.
#line 958
    void SetLastTarget(ACDID idACD, BOOL bSecondary) { m_lastTarget = idACD; bLastTargetInSecondaryRange = bSecondary; }
    BOOL GetLastTargetInSecondaryRange() const { return bLastTargetInSecondaryRange; }
#line 348
    ActorCommonData * ResolveLockedTarget();
    void SetLockedTarget(ACDID idACD);
    void ClearLockedTarget();
    ConsoleTargetComponent::TargetLockType GetLockedTargetType() const;
    void SetStickyTarget(ACDID idACD);
    void SetPersistentTarget(ACDID idACD, Vector2D vStickDirection);
    void SetSoftLockDirection(Vector2D vStickDirection);
#line 969
    Vector2D GetLockedStickDirection() const { return vLockedStickDirection; }
// GetLockedStickDirection: DWARF Player.h:969, +0x38/+0x3c loads inlined into CPlayerHandleConsoleInput. SetOwner: DWARF :971, PlayerSetDefaults stores ptOwner at :971.
    void SetOwner(const Player * ptPlayer) { ptOwner = ptPlayer; }
#line 357
    float GetTargetRangeAdjustment(SNO snoPower) const;
    float GetTargetRangeAdjustment(const PowerDefinition * pdefPower) const;
    void ResetTargetRange();
    void AdjustTargetRange(float);
    void SetInAimMode(BOOL);
    BOOL GetInAimMode() const;
    void SetIsHoldingReticlePowerDown(BOOL flag);
    BOOL GetIsHoldingReticlePowerDown();
    void UpdateReticleDistanceRatio(SNO snoPower);
    float GetReticleDistanceRatio();
    void SetInReticleMode(BOOL flag, SNO snoPower);
    BOOL GetInReticleMode() const;
    void ResetTargetingReticle(const Player * ptPlayer, SNO snoPower, BOOL bMaxDistance);
    void GetTargetingReticle(Vector2D & vOutput) const;
    void AdjustTargetingReticle(float flScalar);
    SNO GetReticlePower() const;
    Vector2D GetTargetingDirection() const;
// ConsoleTargetComponent(): DWARF Player.h:994, inlined into Player::Player with one store group per line :995-:1005;
// the initializer-list form (one member per line, in member order) is inferred from those lines.
#line 994
    ConsoleTargetComponent()
        : m_lastTarget(ACDID_INVALID)
        , m_lockedTarget(ACDID_INVALID)
        , ptOwner(NULL)
        , flTargetRangeAdjustment(0.0f)
        , flReticleDistanceRatio(0.0f)
        , bLastTargetInSecondaryRange(FALSE)
        , eTargetLockType(TARGETLOCK_NONE)
        , bInAimMode(FALSE)
        , bInReticleMode(FALSE)
        , m_snoReticlePower(SNO_NONE)
        , bIsHoldingReticlePowerDown(FALSE)
    {
    }
#line 374
private:
    ACDID m_lastTarget;  // 0x00
    ACDID m_lockedTarget;  // 0x04
    const Player * ptOwner;  // 0x08
    float flTargetRangeAdjustment;  // 0x10
    float flReticleDistanceRatio;  // 0x14
    BOOL bLastTargetInSecondaryRange;  // 0x18
    ConsoleTargetComponent::TargetLockType eTargetLockType;  // 0x1c
    BOOL bInAimMode;  // 0x20
    BOOL bInReticleMode;  // 0x24
    SNO m_snoReticlePower;  // 0x28
    BOOL bIsHoldingReticlePowerDown;  // 0x2c
    Vector2D vTargetingReticle;  // 0x30
    Vector2D vLockedStickDirection;  // 0x38
};
static_assert(sizeof(ConsoleTargetComponent) == 0x40, "ConsoleTargetComponent");

// Player.h:974, :985, :990. Bodies written from the copies inlined into CPlayerInputClearHeldButton (X/Client/CPlayerInput.cpp).
inline void ConsoleTargetComponent::ResetTargetRange() { flTargetRangeAdjustment = 0.0f; }
inline void ConsoleTargetComponent::SetInReticleMode(BOOL flag, SNO snoPower) { bInReticleMode = flag; m_snoReticlePower = snoPower; }
inline SNO ConsoleTargetComponent::GetReticlePower() const { return bInReticleMode ? m_snoReticlePower : SNO_NONE; }
// Player.h:956, :962, :964. Bodies written from the copies inlined into CPlayerConsoleTargetLock (X/Client/CPlayerInput.cpp).
inline ACDID ConsoleTargetComponent::GetLastTarget() const { return m_lastTarget; }
inline void ConsoleTargetComponent::SetLockedTarget(ACDID idACD) { m_lockedTarget = idACD; eTargetLockType = TARGETLOCK_HARD; }
inline ConsoleTargetComponent::TargetLockType ConsoleTargetComponent::GetLockedTargetType() const { return eTargetLockType; }
// Player.h:978. Body written from the copy inlined into UIGame::UpdateRActorHighlights.
inline BOOL ConsoleTargetComponent::GetInAimMode() const { return bInAimMode; }
// Player.h:966. Body written from the copy inlined into CPlayerConsoleUseSkill.
inline void ConsoleTargetComponent::SetStickyTarget(ACDID idACD) { m_lockedTarget = idACD; eTargetLockType = TARGETLOCK_STICKY; }
// Player.h:980, :981, :986. Bodies written from the copies inlined into CPlayerConsoleProcessHeldButton.
inline void ConsoleTargetComponent::SetIsHoldingReticlePowerDown(BOOL flag) { bIsHoldingReticlePowerDown = flag; }
inline BOOL ConsoleTargetComponent::GetIsHoldingReticlePowerDown() { return bIsHoldingReticlePowerDown; }
inline BOOL ConsoleTargetComponent::GetInReticleMode() const { return bInReticleMode; }
// Player.h:967, :968. Bodies written from the copies inlined into CPlayerConsoleUseEvade.
inline void ConsoleTargetComponent::SetPersistentTarget(ACDID idACD, Vector2D vStickDirection) { m_lockedTarget = idACD; eTargetLockType = TARGETLOCK_PERSISTENT; vLockedStickDirection = vStickDirection; }
inline void ConsoleTargetComponent::SetSoftLockDirection(Vector2D vStickDirection) { m_lockedTarget = ACDID_INVALID; eTargetLockType = TARGETLOCK_SOFT; vLockedStickDirection = vStickDirection; }

enum MoveValidationLogType { MOVE_LOG_INVALID = -1, MOVE_LOG_IMMOBILIZED = 0, MOVE_LOG_ARCING_MOVEMENT = 1, MOVE_LOG_POWER_IGNORE = 2, MOVE_LOG_EPSILON = 3, MOVE_LOG_SPEED = 4, MOVE_LOG_ACK = 5,
    MOVE_LOG_BANK = 6, MOVE_LOG_OUT_OF_RANGE_SNAP = 7, MOVE_LOG_OUT_OF_RANGE = 8, MOVE_LOG_SUCCESS = 9, MOVE_LOG_SERVER_DELAY = 10, MOVE_LOG_COUNT = 11 };  // X/Common/Player.h:1029
struct MoveValidationLog { uint32 uTime; MoveValidationLogType eType; float flSpeed; uint32 uTickBase; uint32 uTicksAhead; float flData; uint32 uData; SNO snoPower; };  static_assert(sizeof(MoveValidationLog) == 0x20, "MoveValidationLog");  // X/Common/Player.h:1047
struct LogoutTimerData {  // size 0xc; X/Common/Player.h
    uint32 uLogoutTimeTicks;  // 0x00
    BOOL bAbortable;  // 0x04
    uint32 uGracePeriodTimeTicks;  // 0x08
// Clear: DWARF Player.h:1065 (members at :1061-1063); PlayerSetDefaults' merged zero stores carry line :1068.
#line 1065
    void Clear()
    {
        uLogoutTimeTicks = 0;
        bAbortable = FALSE;
        uGracePeriodTimeTicks = 0;
    }
#line 419
};

class PlayerStats {  // size 0x288; X/Common/Player.h
public:
    void SetDefaults();
    uint32 m_uStartTickForPlaySession;  // 0x00
    uint64 m_uTotalCurrencyPickedUp[NUM_CURRENCY_TYPES];  // 0x8
    uint64 m_uMaxLumpOfCurrencyPickedUp[NUM_CURRENCY_TYPES];  // 0xb8
    uint64 m_uGoldSpent;  // 0x168
    uint64 m_uGoldObtainedThroughSelling;  // 0x170
    float m_flTotalHealthPickedUp;  // 0x178
    float m_flMaxLumpOfHealthPickedUp;  // 0x17c
    uint32 m_uTownPortalsUsed;  // 0x180
    KillCounter m_arKillCounter[4];  // 0x188
    CurrencyCounter m_tGoldCounter;  // 0x268
    CurrencyCounter m_tPlatinumCounter;  // 0x278
};
static_assert(sizeof(PlayerStats) == 0x288, "PlayerStats");
#include "../Sigma/DataStructures/XMap.h"
typedef XMap<int, int, unsigned int, unsigned int, 8> SnoStoredStates;  /* X/Common/Player.h:643 */  typedef XMap<int, int, unsigned int, unsigned int, 32> SkillsUsedStorage;  // X/Common/Player.h:645
typedef int32 UIAction;  /* X/Common/Player.h:647 */  typedef int32 EInputModes;  // X/Common/Player.h:649

#include "ActorInventory.h"  // X/Common/Player.h:653
#include "../Sigma/DataStructures/XGrowableList.h"
struct PlayerClientInfo {  // size 0xbe8
    void SetDefaults();
    void Reset();
    uint32 dwNameDisplayEnd;  // 0x00
    uint32 dwFadeOutTime;  // 0x04
    uint32 dwLastItemMoveProcessed;  // 0x08
    uint32 dwMinChanneledDurationTime;  // 0x0c
    uint32 dwTimeLastComboAttack;  // 0x10
    uint32 dwInterruptedAnimCompleteTime;  // 0x14
    uint32 dwTimeLastNonPreplayedAnimatingPower;  // 0x18
    uint32 dwTimeInputWasCached;  // 0x1c
    uint32 dwNextTimeCanSayImFull;  // 0x20
    uint32 dwClosingAbortTime;  // 0x24
    uint32 dwDownEventBits;  // 0x28
    uint32 dwGameTickOfLastHeldEvent;  // 0x2c
    int32 nComboLevel;  // 0x30
    int32 nKillsSinceMoving;  // 0x34
    uint32 dwGameTickOfLastAction[98];  // 0x38
    uint32 dwGameTickOfLastActionSecondaryPowerCast[98];  // 0x1c0
    uint32 dwGameTickOfLastHeldAction[98];  // 0x348
    uint32 dwGameTickOfLastActionOtherListener[98];  // 0x4d0
    SkillsUsedStorage GameTickOfLastHeldPower;  // 0x658
    UIAction nUIAction;  // 0x9b0
    EInputModes nInputMode;  // 0x9b4
    int32 eCurrentMessage;  // 0x9b8 InputMsg
    ACDID idACDCurrentTarget;  // 0x9bc
    ACDNetworkName ann;  // 0x9c0
    SnoStoredStates tCachedSnoStates;  // 0x9c8
    SNO snoCurrentPower;  // 0xae0
    SNO snoCurrentAnimPower;  // 0xae4
    SNO snoCurrentOriginalAnimPower;  // 0xae8
    SNO snoCurrentChannelledPower;  // 0xaec
    SNO snoCurrentClientWalkPower;  // 0xaf0
    SNO snoCurrentLoopingPower;  // 0xaf4
    SNO snoLastUsedPower;  // 0xaf8
    SNO snoLastCursorQueuedPower;  // 0xafc
    BOOL bWantToCancelChanneledPower;  // 0xb00
    BOOL fSuppressVendorCloseNotification;  // 0xb04
    BOOL bClosing;  // 0xb08
    BOOL bQueuedHighlightForceUpdate;  // 0xb0c
    BOOL bPlayerIsLocal;  // 0xb10
    BOOL bJoinRequested;  // 0xb14
    BOOL bUpscaled;  // 0xb18
    RNG rngSyncedAnim;  // 0xb1c
    TargetLock tCachedTargetLock;  // 0xb24
    TargetLock tTargetLock;  // 0xb50
    IVector2D ptLastMouse;  // 0xb7c
    IVector2D ptLastHeldEventMousePos;  // 0xb84
    XGrowableList<InventoryRequestMoveMessage, InventoryRequestMoveMessage &> listQueuedItemMoves;  // 0xb90 (ItemMoveQueue)
};
static_assert(sizeof(PlayerClientInfo) == 0xbe8, "PlayerClientInfo");  static_assert(offsetof(PlayerClientInfo, GameTickOfLastHeldPower) == 0x658, "PlayerClientInfo");  static_assert(offsetof(PlayerClientInfo, nUIAction) == 0x9b0, "PlayerClientInfo");  static_assert(offsetof(PlayerClientInfo, tCachedSnoStates) == 0x9c8, "PlayerClientInfo");  static_assert(offsetof(PlayerClientInfo, snoCurrentPower) == 0xae0, "PlayerClientInfo");

struct Player {  // size 0xebf8
    int32 nPlayerIndex;  // 0x00
    ACDID idPlayerACD;  // 0x04
    RActorID idPlayerRActor;  // 0x08
    BannerRenderData tBannerRenderData;  // 0x0c
    Team eTeam;  // 0x3c
    Team eJoinTeam;  // 0x40
    GameConnectionID idGameConnection;  // 0x44
    LogoutTimerData auLogoutTimerData[2];  // 0x48
    int32 nPlayerKills;  // 0x60
    int32 nDeaths;  // 0x64
    int32 nAssists;  // 0x68
    int32 nTeamWins;  // 0x6c
    uint32 nValidationErrors[3];  // 0x70
    ActorID idActorTownPortalInUse;  // 0x7c
    uint64 nTeamId;  // 0x80
    float teamRating;  // 0x88
    float teamVariance;  // 0x8c
    float individualRating;  // 0x90
    float individualVariance;  // 0x94
    uint32 nLastRewardChallengeNumber;  // 0x98
    D3::Items::Mail tChallengeMail;  // 0xa0
    int32 nChallengeBestTimesMilliseconds[4];  // 0x108
    BOOL bCompletedSoloRift;  // 0x118
    RNG rngSyncedAnim;  // 0x11c
    HeroStateData tHeroStateData;  // 0x124
    CrafterSavedData tCrafterSavedData;  // 0x48f0
    CurrencySavedData tCurrencySavedData;  // 0x9d28
    CosmeticItemSavedData tCosmeticItemSavedData;  // 0x9f38
    PlayerAccount tAccount;  // 0xb148
    OnlineService::HeroId uHeroId;  // 0xb208
    CHAR usHeroName[49];  // 0xb210
    CHAR szHeroFilename[32];  // 0xb241
    uint32 dwJoinTime;  // 0xb264
    uint32 dwNextStatTime;  // 0xb268
    uint32 dwNextAttributeUpdateTime;  // 0xb26c
    uint32 dwLastInputTime;  // 0xb270
    ACDID idACDActiveVendor;  // 0xb274
    CrafterType eActiveCrafterType;  // 0xb278
    SNO snoLastResidualFootstep;  // 0xb27c
    uint32 dwLastResidualFootstepTime;  // 0xb280
    WorldPlace placeCheckedForActivatingScenes;  // 0xb284
    XPooledDataIDList listVisibleACDs;  // 0xb298
    XPooledDataIDList listPrefetchableACDs;  // 0xb2c0
    XPooledDataIDList listDestinationsToPrefetch;  // 0xb2e8
    BOOL fRefreshWaypointDestination;  // 0xb310
    PlayerTradeData tTradeData;  // 0xb318
    PlayerQuestData tQuestData;  // 0xb348
    PlayerTalkData tTalkData;  // 0xb34c
    ACDID idACDPlayerToInspect;  // 0xb350
    CheckpointData tLastCheckpoint;  // 0xb354
    HearthPortalData tHearthPortalInfo;  // 0xb374
    uint8 arGameFlags[128];  // 0xb38c
    PlayerPet tPets[MAX_PETS];  // 0xb40c
    int32 nMaxUsedPetIndex;  // 0xb60c
    BOOL bCosmeticPetMissing;  // 0xb610
    HirelingStable tHirelingStable;  // 0xb614
    ACDID idRequestedHirelingLocation;  // 0xb77c
    HirelingClass eRequestedHirelingClass;  // 0xb780
    uint32 dwRequestedHirelingTimeStamp;  // 0xb784
    LoadoutSavedData tLoadouts[10];  // 0xb788
    SNO snoActorPortrait;  // 0xd678
    float flHealthPercent;  // 0xd67c
    SNO snoAreaLocation;  // 0xd680
    SNO snoLastWaypointLevelArea;  // 0xd684
    PlayerClass ePlayerClassPortrait;  // 0xd688
    int32 nCurrentLevel;  // 0xd68c
    int32 nCurrentAltLevel;  // 0xd690
    int32 nCurrentHighestHeroSoloRiftLevel;  // 0xd694
    int32 nPortraitAction;  // 0xd698
    int32 nSkillQuickSelectId;  // 0xd69c
    PooledSNONameList listConversations;  // 0xd6a0
    PlayerStats tStats;  // 0xd6c8
    int32 eLocale; /* 0xd950 XLocale */ int32 eWarpReason; /* 0xd954 PlayerWarpReason */ float flWarpFadeInSecs; /* 0xd958 */ int32 eOpenWorldStatus;  // 0xd95c OpenWorldStatus
    BOOL bSetDungeonActive;  // 0xd960
    blz::vector<ACDID, blz::allocator<ACDID> > listNecroCorpses;  // 0xd968 (ACDIDArray)
    uint32 uServerFlags;  // 0xd980
    PRTransform tAVCameraTransform;  // 0xd984
    struct { Act eLastPlayedAct; SNO snoLastPlayedQuest; int32 nLastPlayedQuestStep; uint32 uActivatedWaypoints; SavePointData tLastSavePoint; int32 nHandicapLevelSnapshot; } tPlayerResumeStateUponJoining;  // 0xd9a0 (size 0x24)
    XPooledDataIDList tLoresUsed;  // 0xd9c8
    uint32 dwBossEncounterFlags;  // 0xd9f0
    uint32 dwPrevPortalHash;  // 0xd9f4
    uint32 dwBossesKilledFlags;  // 0xd9f8
    uint32 dwNephalemRiftFlags;  // 0xd9fc
    uint32 dwSetDungeonFlags;  // 0xda00
    ACDID idACDActiveChallengePortal;  // 0xda04
    BOOL bChallengeAccepted;  // 0xda08
    BOOL bOptedInToOpenWorldModeChange;  // 0xda0c
    BOOL bPlayOpenWorldTutorial;  // 0xda10
    float flPVPPlayerDamageTaken[4];  // 0xda14
    ACDID idACDStashLastOperated;  // 0xda24
    uint32 dwTickLastVerification;  // 0xda28
    WorldPlace tPlaceLastVerification;  // 0xda2c
    float flMaxSpeedThisVerificationInterval;  // 0xda3c
    uint32 dwTickLastSpamCheck;  /* 0xda40 */  uint32 nMessageCostLast300;  /* 0xda44 */  uint32 nMessageCostLast600;  /* 0xda48 */  uint32 nMessageCostLast3600;  /* 0xda4c */  uint32 nMessageCostLastUpdateTick;  /* 0xda50 */  uint32 nMessageCostMax300;  /* 0xda54 */  uint32 nMessageCostMax600;  /* 0xda58 */  uint32 nMessageCostMax3600;  // 0xda5c
    PlayerQueuedPower tQueuedPower;  // 0xda60
    uint64 nKicksInitiated;  // 0xdaa8
    uint64 nKicksParticipatedIn;  // 0xdab0
    uint64 nGamesWithoutKicks;  // 0xdab8
    uint64 nTimesVoteKicked;  // 0xdac0
    uint32 uEntitledRewardBits;  // 0xdac8
    uint32 uOutstandingRewardBits;  // 0xdacc
    uint32 dwTimeStampJoined;  // 0xdad0
    GBHandle hGBKiller;  // 0xdad4
    SNO snoKiller;  // 0xdadc
    MonsterRarity eKillerRarity;  // 0xdae0
    GBID killerRareNameGBIDs[2];  // 0xdae4
    SNO snoKilledIn;  // 0xdaec
    TimerID idTimerForcedResurrect;  /* 0xdaf0 */  TimerID idTimerCorpseResurrect;  /* 0xdaf4 */  TimerID idTimerCorpseResurrectGrantCharge;  /* 0xdaf8 */  uint32 dwTickLastResurrection;  /* 0xdafc */  uint32 uSeasonCreated;  // 0xdb00
    PlayerVendorData tVendorData;  // 0xdb04
    XPooledList<int, int> listOnGroundCraftingPlans;  // 0xdb40
    int32 nBusyMessagesSinceNPCInteract;  // 0xdb68
    TimerID idTimerLoadoutEquipApplyPassives;  // 0xdb6c
    uint32 dwServerSidePrefFlags;  // 0xdb70
    int32 nControllerIndex;  // 0xdb74
    BOOL bIsPrimaryPlayerServerOnly;  // 0xdb78
    BOOL bIsGuestPlayerServerOnly;  // 0xdb7c
    BOOL bIsLocalToServerServerOnly;  // 0xdb80
    SNO snoControllerBinding;  // 0xdb84
    ConsoleTargetComponent tTargetComponent;  // 0xdb88
    PlayerAvengerData tSoloAvenger;  // 0xdbc8
    PlayerAvengerData tHardcoreAvenger;  // 0xdc30
    PlayerAvengerData tFriendAvenger;  // 0xdc98
    AvengerSource eAvengerSource;  // 0xdd00
    uint32 uConsoleSyncFlags;  // 0xdd04
    uint32 uLastPlayedTimeUpdate;  // 0xdd08
    struct {  // size 0x10
        uint32 uMonstersKilled;  // 0x00
        uint32 uElitesKilled;  // 0x04
        uint32 uGoldPickedUp;  // 0x08
        uint32 uLootRunsCompleted;  // 0x0c
    } tConsoleCounters;  // 0xdd0c
    BOOL bHasPerformedFinalSaveForQuit;  // 0xdd1c
    UIInventory::Console::NewItemIndicatorManager<ACDID> * ptNewItemManager;  // 0xdd20
    UIInventory::Console::NewItemIndicatorManager<ACDID> * ptNewEliteItemManager;  // 0xdd28
    UIInventory::Console::NewItemIndicatorManager<GBID> * ptNewRecipeManager;  // 0xdd30
    UIInventory::Console::NewItemIndicatorManager<GBID> * ptNewCosmeticManager;  // 0xdd38
    blz::unordered_map<int32, blz::pair<uint32, uint32> > tMapVendorSeeds;  /* 0xdd40 */  blz::unordered_map<int32, blz::pair<uint32, uint32> > tMapPartitionWideCubeSeeds;  /* 0xdd60 */  uint32 uCubeBaseSeed;  // 0xdd80
    blz::list<D3::Account::ConsoleChallengeRiftReward, blz::allocator<D3::Account::ConsoleChallengeRiftReward> > tChallengeRiftRewards;  // 0xdd88
    Blizzard::Time::Timestamp tNFPDefaultScanTime;  /* 0xdda0 */  Blizzard::Time::Timestamp tNFPGoblinScanTime;  // 0xdda8
    PlayerClientInfo tClientInfo;  // 0xddb0
    MoveValidationLog arMoveValidationLog[16];  /* 0xe998 */  uint32 uMoveLogIdx;  // 0xeb98
    Blizzard::Util::BitField tSeenTutorials;  // 0xeba0
    uint8 bStashIcons[12];  // 0xebc0
    uint32 dwLastKickAttempt;  // 0xebcc
    uint32 dwLastSuccessfulKick;  // 0xebd0
    uint64 uAcceptedLicenseBitsAccount;  /* 0xebd8 */  uint64 uAcceptedLicenseBitsHero;  /* 0xebe0 */  uint64 uGuildId;  // 0xebe8
    uint8 bDidFirstTimeSetup : 1;  // 0xebf0
    uint8 bIsBeingDeleted : 1;  // 0xebf0
    uint8 bParticipatingInVoteKick : 1;  // 0xebf0
    uint8 fOptedIntoActTransition : 1;  // 0xebf0
    uint8 bForceKnownListUpdate : 1;  // 0xebf0
    uint8 bParticipatedInVoteKick : 1;  // 0xebf0
    uint8 bForceHirelingItemSave : 1;  // 0xebf0
    uint8 bNewX1Player : 1;  // 0xebf0
    // Player::Player is declared at Player.h:1373 (DWARF); the original calls BitField(0x180) for tSeenTutorials there.
#line 1373
    Player() : tSeenTutorials(MAX_TUTORIAL_BITS) {}
#line 650
};
static_assert(sizeof(Player) == 0xebf8, "Player");  static_assert(offsetof(Player, nValidationErrors) == 0x70, "Player");  static_assert(offsetof(Player, idActorTownPortalInUse) == 0x7c, "Player");  static_assert(offsetof(Player, rngSyncedAnim) == 0x11c, "Player");  static_assert(offsetof(Player, listNecroCorpses) == 0xd968, "Player");  static_assert(offsetof(Player, tClientInfo) == 0xddb0, "Player");  static_assert(offsetof(Player, arMoveValidationLog) == 0xe998, "Player");  static_assert(offsetof(Player, uMoveLogIdx) == 0xeb98, "Player");  static_assert(offsetof(Player, tSeenTutorials) == 0xeba0, "Player");  static_assert(offsetof(Player, dwLastSuccessfulKick) == 0xebd0, "Player");  static_assert(offsetof(Player, uAcceptedLicenseBitsAccount) == 0xebd8, "Player");  static_assert(offsetof(Player, uAcceptedLicenseBitsHero) == 0xebe0, "Player");  static_assert(offsetof(Player, uGuildId) == 0xebe8, "Player");
static_assert(offsetof(Player, nLastRewardChallengeNumber) == 0x98, "Player");
static_assert(offsetof(Player, tCosmeticItemSavedData) == 0x9f38, "Player");
static_assert(offsetof(Player, tAccount) == 0xb148, "Player");
static_assert(offsetof(Player, tChallengeMail) == 0xa0, "Player");
static_assert(offsetof(Player, nChallengeBestTimesMilliseconds) == 0x108, "Player");
static_assert(offsetof(Player, dwNephalemRiftFlags) == 0xd9fc, "Player");
static_assert(offsetof(Player, tChallengeRiftRewards) == 0xdd88, "Player");
static_assert(offsetof(Player, tCrafterSavedData) == 0x48f0, "Player");
static_assert(offsetof(Player, tHeroStateData) == 0x124, "Player");
static_assert(offsetof(Player, tCurrencySavedData) == 0x9d28, "Player");
static_assert(offsetof(Player, tPets) == 0xb40c, "Player");
static_assert(offsetof(Player, listOnGroundCraftingPlans) == 0xdb40, "Player");
static_assert(offsetof(Player, usHeroName) == 0xb210, "Player");
static_assert(offsetof(Player, dwLastInputTime) == 0xb270, "Player");
static_assert(offsetof(Player, tQueuedPower) == 0xda60, "Player");
static_assert(offsetof(Player, tVendorData) == 0xdb04, "Player");
static_assert(offsetof(Player, tTargetComponent) == 0xdb88, "Player");
static_assert(offsetof(Player, tSoloAvenger) == 0xdbc8, "Player");
static_assert(offsetof(Player, tFriendAvenger) == 0xdc98, "Player");
static_assert(offsetof(Player, uConsoleSyncFlags) == 0xdd04, "Player");

// line 360
enum {
    PlayerSaveReason_Item = 1,
    PlayerSaveReason_Timer = 2,
    PlayerSaveReason_Utility = 4,
    PlayerSaveReason_Delete = 8,
    PlayerSaveReason_Web = 16,
    PlayerSaveReason_Clear = 32,
    PlayerSaveReason_Console = 64,
    PlayerSaveReason_Reset = 128,
    PlayerSaveReason_SendBug = 256,
    PlayerSaveReason_DebugDump = 512,
    PlayerSaveReason_LocalPlayer = 1024,
    PlayerSaveReason_Dead = 2048,
    PlayerSaveReason_DB = 4096,
    PlayerSaveReason_AcctFlags = 8192,
    PlayerSaveReason_Diablo = 16384,
    PlayerSaveReason_Consolidate = 32768,
    PlayerSaveReason_Create = 65536,
    PlayerSaveReason_ServerQuit = 131072,
    PlayerSaveReason_PlayerQuit = 262144,
    PlayerSaveReason_Checkpoint = 524288,
    PlayerSaveReason_GameEnd = 1048576,
    PlayerSaveReason_LevelUp = 2097152,
    PlayerSaveReason_Quest = 4194304,
    PlayerSaveReason_RareItem = 8388608,
    PlayerSaveReason_Adventure = 16777216,
    PlayerSaveReason_Store = 33554432,
    PlayerSaveReason_ChallengeR = 67108864,
    PlayerSaveReason_LAST = 134217728,
};

typedef int32 PlayerSaveReason;  // line 391

// X/Common/Player.h:459 / :989. TimeMapPool is XMapMemoryPool<int, unsigned int>; PlayerGlobals
// holds the two per-game pools followed by the fixed pool of players the game keeps.
typedef XMapMemoryPool<int, unsigned int> TimeMapPool;

struct PlayerGlobals {  // size 0x3b040
    TimeMapPool poolTimeMap;  // 0x00
    TSnoNameListPool poolSNOName;  // 0x30
    Player tPlayerArray[4];  // 0x60
};
static_assert(sizeof(PlayerGlobals) == 0x3b040, "PlayerGlobals");

// Declared here from their definitions in Player.cpp.
Player * PlayerGetByACD(const ACDID idACD);  // Player.cpp:1467
void PlayerRemovePet(Player & tPlayer, ACDID idACDPet, BOOL bDisplayChatMessage);  // Player.cpp:2393
int32 PlayerGetParagonLevelFromPortraitGBID(GBID gbidPortrait);  // Player.cpp:5449
BOOL ACDIsPlayer(const ACDID idACD);  // Player.cpp:1659
void PlayerDetermineNextDualWieldHand(const ACDID idACDPlayer);  // Player.cpp:1916
Player * ACDGetControllingPlayer(ACDID idACD);  // Player.cpp
Player * PlayerGetByHeroId(OnlineService::HeroId uHeroId);  // Player.cpp:1546
BOOL PlayerHasActivatedWaypoint(const Player & tPlayer, const int32 nWaypointNumber);  // Player.cpp:3214
Player * PlayerGetByRActor(RActorID idRActor);  // Player.cpp
BOOL PlayerIsInWorld(const Player & tPlayer);  // Player.cpp
Player * PlayerGetFirstInGame();  // Player.cpp:1974
Player * PlayerGetNextInGame(const Player * ptPlayer);  // Player.cpp:1993
Player * PlayerGetFirstAll();  // Player.cpp:2117
Player * PlayerGetNextAll(const Player * ptPlayer);  // Player.cpp:2126
BOOL PlayerIsHardcoreAndDead(const Player & tPlayer);  // Player.cpp:5161
BOOL PlayersAreLocalToEachOther(const Player & tPlayer0, const Player & tPlayer1);  // Player.cpp:5179
int32 PlayerGetNumPlayers();  // Player.cpp:2181
uint32 PlayersGetCount();  // Player.cpp:1371
Player * PlayerGetByPlayerIndex(int32 nPlayerIndex);  // Player.cpp:1956
Player * PlayerGetByControllerIndex(int32 nControllerIndex);  // Player.cpp:1598
Player * PlayerGetPrimaryPlayerForGameConnection(GameConnectionID idGameConnection);  // Player.cpp:5302
PlayerAccount PlayerGetAccount(const ACDID idACDPlayer);  // Player.cpp:3338
BOOL PlayerCanAttemptVoteKick(const Player & tPlayer);  // Player.cpp:4946
class ActorCommonData;
BOOL PlayerIsMale(const Player & tPlayer);  // Player.cpp:646
PlayerClass PlayerGetPlayerClass(const ActorCommonData * ptACD);  // Player.cpp:2300
PlayerClass PlayerGetPlayerClass(const ACDID idACD);  // Player.cpp:2292
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);  // Player.cpp
class CRefString;
CRefString PlayerGetClassUIName(const PlayerClass ePlayerClass, const BOOL fIsMale);  // Player.cpp:3349

typedef int32 GameError;
// X/Common/Player.h:89 (every GameError value)
enum {
    GAMEERROR_NONE = -1,
    GAMEERROR_POWER_UNUSABLE_IN_AREA = 0,
    GAMEERROR_ITEM_NOT_READY = 1,
    GAMEERROR_POWER_INVALID_TARGET = 2,
    GAMEERROR_POWER_NOT_ENOUGH_HEALTH = 3,
    GAMEERROR_POWER_NOT_ENOUGH_CHARGES = 4,
    GAMEERROR_POWER_NOT_ENOUGH_MANA = 5,
    GAMEERROR_POWER_NOT_ENOUGH_ARCANUM = 6,
    GAMEERROR_POWER_NOT_ENOUGH_FURY = 7,
    GAMEERROR_POWER_NOT_ENOUGH_SPIRIT = 8,
    GAMEERROR_POWER_NOT_ENOUGH_HATRED = 9,
    GAMEERROR_POWER_NOT_ENOUGH_DISCIPLINE = 10,
    GAMEERROR_POWER_NOT_ENOUGH_FAITH = 11,
    GAMEERROR_POWER_NOT_ENOUGH_ESSENCE = 12,
    GAMEERROR_POWER_WRONG_WEAPON = 13,
    GAMEERROR_POWER_IN_TOWN = 14,
    GAMEERROR_POWER_TARGET_TOO_FAR = 15,
    GAMEERROR_POWER_IN_COOLDOWN = 16,
    GAMEERROR_POWER_NEEDS_LINE_OF_SIGHT = 17,
    GAMEERROR_POWER_UNUSABLE_IN_COMBAT = 18,
    GAMEERROR_POWER_UNUSABLE_GENERIC = 19,
    GAMEERROR_POWER_UNUSABLE_WHILE_MOVING = 20,
    GAMEERROR_POWER_MISSING_ITEM = 21,
    GAMEERROR_POWER_UNUSABLE_DURING_BOSS_ENCOUNTER = 22,
    GAMEERROR_POWER_NEED_CORPSES = 23,
    GAMEERROR_POWER_NEED_TARGET = 24,
    GAMEERROR_IARESULT_UNKNOWN = 25,
    GAMEERROR_IARESULT_CANT_TRADE_QUEST_ITEMS = 26,
    GAMEERROR_IARESULT_CANT_EQUIP_UNIDENTIFIED_ITEM = 27,
    GAMEERROR_IARESULT_PLAYER_DOESNT_MEET_ITEM_REQUIREMENTS = 28,
    GAMEERROR_IARESULT_PLAYER_DOESNT_MEET_ITEM_LEVEL = 29,
    GAMEERROR_IARESULT_PLAYER_IS_WRONG_CLASS_FOR_ITEM = 30,
    GAMEERROR_IARESULT_TEMPLAR_CANT_EQUIP_ITEM = 31,
    GAMEERROR_IARESULT_SCOUNDREL_CANT_EQUIP_ITEM = 32,
    GAMEERROR_IARESULT_ENCHANTRESS_CANT_EQUIP_ITEM = 33,
    GAMEERROR_IARESULT_TEMPLAR_CANT_EQUIP_SPECIAL_ITEM = 34,
    GAMEERROR_IARESULT_SCOUNDREL_CANT_EQUIP_SPECIAL_ITEM = 35,
    GAMEERROR_IARESULT_ENCHANTRESS_CANT_EQUIP_SPECIAL_ITEM = 36,
    GAMEERROR_IARESULT_HIRELING_CANT_USE_ITEM = 37,
    GAMEERROR_IARESULT_HIRELING_DOESNT_MEET_ITEM_LEVEL = 38,
    GAMEERROR_IARESULT_ITEM_TYPE_DOESNT_MATCH_SLOT = 39,
    GAMEERROR_IARESULT_NOT_ENOUGH_ROOM = 40,
    GAMEERROR_IARESULT_REMOVE_ONLY = 41,
    GAMEERROR_IARESULT_NO_FREE_SOCKETS = 42,
    GAMEERROR_IARESULT_ITEM_IS_SEASONAL = 43,
    GAMEERROR_IARESULT_ITEM_IS_NON_SEASONAL = 44,
    GAMEERROR_IARESULT_ITEM_REQUIRES_SKILL = 45,
    GAMEERROR_IARESULT_ITEM_REQUIRES_XBOW = 46,
    GAMEERROR_ITEMCRAFTINGRESULT_NO_COMBINE_DESIGN = 47,
    GAMEERROR_ITEMCRAFTINGRESULT_MISSINGREAGENTS = 48,
    GAMEERROR_IARESULT_CANNOT_SALVAGE = 49,
    GAMEERROR_IARESULT_CANNOT_SALVAGE_GEMMED_ITEMS = 50,
    GAMEERROR_IARESULT_CANNOT_SALVAGE_VENDOR_ITEMS = 51,
    GAMEERROR_SKILLS_SKILLBAR_GENERIC = 52,
    GAMEERROR_SKILLS_NOT_ASSIGNABLE_TO_THAT_SLOT = 53,
    GAMEERROR_SKILLS_CANT_CHANGE_WHILE_IN_COOLDOWN = 54,
    GAMEERROR_SKILLS_CANT_CHANGE_WHILE_IN_COMBAT = 55,
    GAMEERROR_SKILLS_CANT_CHANGE_WHILE_IN_BOSS_ENCOUNTER = 56,
    GAMEERROR_SKILLS_CANT_CHANGE_AT_THIS_TIME = 57,
    GAMEERROR_BUFF_LOWER_THEN_CURRENT = 58,
    GAMEERROR_IARESULT_ALREADY_IDENTIFIED = 59,
    GAMEERROR_IARESULT_CANNOT_SOCKET = 60,
    GAMEERROR_IARESULT_ALREADY_SOCKETED = 61,
    GAMEERROR_IARESULT_CANNOT_UNSOCKET = 62,
    GAMEERROR_IARESULT_CANNOT_ADD_SOCKETS_WITH_THIS_ITEM = 63,
    GAMEERROR_IARESULT_CANNOT_ACCEPT_TRADE = 64,
    GAMEERROR_IARESULT_CANNOT_TRADE = 65,
    GAMEERROR_IARESULT_CANNOT_TRADE_BECAUSE_OF_GEMS = 66,
    GAMEERROR_ITEMUSE_UNKNOWN = 67,
    GAMEERROR_ITEMUSE_NOT_ON_UNIDENTIFIED_ITEMS = 68,
    GAMEERROR_ITEMUSE_ALREADY_AT_MAX_HEALTH = 69,
    GAMEERROR_ITEMUSE_RECIPE_ALREADY_KNOWN = 70,
    GAMEERROR_ITEMUSE_ONLY_AT_BLACKSMITH = 71,
    GAMEERROR_ITEMUSE_ONLY_AT_JEWELER = 72,
    GAMEERROR_ITEMUSE_ONLY_AT_MYSTIC = 73,
    GAMEERROR_ITEMUSE_DOESNT_MEET_ARTISAN_LEVEL = 74,
    GAMEERROR_ITEMUSE_NOT_ENOUGH_PAGES_TO_CONVERT = 75,
    GAMEERROR_CANNOT_DO_THIS_WHILE_DEAD = 76,
    GAMEERROR_BOSS_ENCOUNTER_SOMEONE_DECLINED = 77,
    GAMEERROR_PICKUP_FAILED = 78,
    GAMEERROR_PICKUP_NO_SUITABLE_SLOT = 79,
    GAMEERROR_PICKUP_ONLY_ONE_ALLOWED = 80,
    GAMEERROR_PICKUP_BELONGS_TO_OTHER = 81,
    GAMEERROR_INSUFFICIENT_FUNDS = 82,
    GAMEERROR_INSUFFICIENT_FUNDS_BLOODSHARDS = 83,
    GAMEERROR_INSUFFICIENT_FUNDS_PLATINUM = 84,
    GAMEERROR_INSUFFICIENT_PARAGON_POINTS = 85,
    GAMERROR_PARAGON_CATEGORY_AT_CAP = 86,
    GAMEERROR_ITEM_CANNOT_BE_REPAIRED = 87,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED = 88,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_BY_TYPE = 89,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_BELOW_ILVL = 90,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_ABOVE_ILVL = 91,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_UNIDENTIFIED = 92,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_SOCKETABLE = 93,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_INVALID_QUALITY = 94,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_FILLED_SOCKETS = 95,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_CRAFTED = 96,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_NO_RANDOM_AFFIXES = 97,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_LEGACY_ITEM = 98,
    GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_ITEM_TYPE_ABOVE_CRAFTER_LEVEL = 99,
    GAMEERROR_ITEM_CANNOT_BE_IMPROVED = 100,
    GAMEERROR_ITEM_CANNOT_BE_DYED = 101,
    GAMEERROR_ITEM_CANNOT_BE_REFORGED = 102,
    GAMEERROR_ITEM_CANNOT_BE_REFORGED_UNIDENTIFIED = 103,
    GAMEERROR_ITEM_CANNOT_BE_REFORGED_NOT_LEGENDARY = 104,
    GAMEERROR_ITEM_CANNOT_BE_REFORGED_CANNOT_EQUIP = 105,
    GAMEERROR_ITEM_CANNOT_BE_REFORGED_HAS_GEMS = 106,
    GAMEERROR_ITEM_CANNOT_BE_USED_FOR_TRANSMOG = 107,
    GAMEERROR_ITEM_CANNOT_BE_USED_FOR_TRANSMOG_UNIDENTIFIED = 108,
    GAMEERROR_ITEM_TRANSMOG_ALREADY_UNLOCKED = 109,
    GAMEERROR_TRADE_OTHER_BUSY = 110,
    GAMEERROR_TRADE_SPLIT_STACKS = 111,
    GAMEERROR_TRADE_DISABLED = 112,
    GAMEERROR_TRADE_OF_GOLD_DISABLED = 113,
    GAMEERROR_NO_OTHER_WAYPOINTS = 114,
    GAMEERROR_BNET_PARTY_LEADER_CANCEL_LEAVE_GAME = 115,
    GAMEERROR_BNET_PARTY_INVITE_REMOVED_LEAVE_GAME_CANCELED = 116,
    GAMEERROR_BNET_MULTIPLAYER_GAMENOTPAUSED = 117,
    GAMEERROR_BNET_WEEKLY_CHALLENGE_GAMENOTPAUSED = 118,
    GAMEERROR_BNET_NO_SEASON_JOURNEY_AVAILABLE_IN_NO_SEASONAL_GAME = 119,
    GAMEERROR_CANNOT_ENTER_ACTIVE_BOSS_ENCOUNTER = 120,
    GAMEERROR_CANNOT_ENTER_QUEUED_BOSS_ENCOUNTER = 121,
    GAMEERROR_BANNER_PLAYER_TOO_CLOSE = 122,
    GAMEERROR_BANNER_PLAYER_IS_IN_BOSS_ENCOUNTER = 123,
    GAMEERROR_BANNER_PLAYER_IS_DEAD = 124,
    GAMEERROR_BANNER_PLAYER_IS_DUELING = 125,
    GAMEERROR_CANNOT_KICK_NOW = 126,
    GAMEERROR_ITEM_SOLD_NONERROR = 127,
    GAMEERROR_ITEM_CANNOT_BE_SOLD = 128,
    GAMEERROR_ITEM_UNIQUE_EQUIPPED = 129,
    GAMEERROR_CANNOT_DO_THAT_RIGHT_NOW = 130,
    GAMEERROR_ITEM_SOCKET_TYPE_MISMATCH = 131,
    GAMEERROR_ITEM_SOCKET_UNIQUE_EQUIPPED = 132,
    GAMEERROR_EQUIP_ITEM_SOCKET_UNIQUE_EQUIPPED = 133,
    GAMEERROR_IDENTIFY_ALL_NO_ITEMS = 134,
    GAMEERROR_ITEM_CANNOT_BE_DROPPED = 135,
    GAMEERROR_WAYPOINT_TOO_CLOSE = 136,
    GAMEERROR_LOOTRUN_PARTY_INSIDE = 137,
    GAMEERROR_BANNER_PLAYER_IS_IN_TIERED_LOOT_RUN = 138,
    GAMEERROR_LOOTRUN_NEED_KEY = 139,
    GAMEERROR_LOOTRUN_TIERED_NEED_KEY = 140,
    GAMEERROR_LOOTRUN_SOMEONE_DECLINED = 141,
    GAMEERROR_LOOTRUN_TIERED_ALREADY_STARTED = 142,
    GAMEERROR_LOOTRUN_TIERED_NOT_MAX_LEVEL = 143,
    GAMEERROR_LOOTRUN_TIERED_GAME_NOT_MAX_LEVEL = 144,
    GAMEERROR_LOOTRUN_TIERED_CANNOT_DO_THAT = 145,
    GAMEERROR_LOOTRUN_TIERED_CANNOT_CHANGE_SKILLS_OR_EQUIPMENT_OUTSIDE_GAME = 146,
    GAMEERROR_LOOTRUN_TIERED_CANNOT_BE_IN_SET_DUNGEON = 147,
    GAMEERROR_DIFFICULTY_TOO_LOW = 148,
    GAMEERROR_ITEMS_CANNOT_BE_MOVED_AT_THIS_TIME = 149,
    GAMEERROR_PLAYER_NOT_ALLOWED_IN_WORLD = 150,
    GAMEERROR_TRANSMUTE_CANNOT_USE_ITEM_WITH_FILLED_SOCKETS = 151,
    GAMEERROR_TRANSMUTE_CANNOT_USE_UNIDENTIFIED_ITEM = 152,
    GAMEERROR_TRANSMUTE_MISSING_CORE_INGREDIENTS = 153,
    GAMEERROR_TRANSMUTE_CANNOT_USE_ITEM_IN_CUBE = 154,
    GAMEERROR_TRANSMUTE_FAILED = 155,
    GAMEERROR_TRANSMUTE_FAILED_LEGENDARY_POWER_ALREADY_EXTRACTED = 156,
    GAMEERROR_TRANSMUTE_FAILED_LEGENDARY_POWER_BLACKLISTED = 157,
    GAMEERROR_TRANSMUTE_FAILED_CANNOT_EXTRACT_FROM_THIS_ITEM = 158,
    GAMEERROR_TRANSMUTE_FAILED_CANNOT_USE_CRAFTED_ITEM = 159,
    GAMEERROR_TRANSMUTE_FAILED_CANNOT_EQUIP_ITEM = 160,
    GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE = 161,
    GAMEERROR_TRANSMUTE_FAILED_ITEM_LEVEL_TOO_LOW = 162,
    GAMEERROR_TRANSMUTE_FAILED_ALREADY_REMOVED_LEVEL_REQ = 163,
    GAMEERROR_TRANSMUTE_FAILED_NOT_ENOUGH_ITEMS_IN_SET = 164,
    GAMEERROR_EMPOWERED_RIFT_CANNOT_AFFORD = 165,
    GAMEERROR_BANNER_PLAYER_IS_IN_SET_DUNGEON = 166,
    GAMEERROR_TRANSMUTE_FAILED_LOW_JEWEL_RANK = 167,
    GAMEERROR_SET_DUNGEON_SOMEONE_DECLINED = 168,
    GAMEERROR_SET_DUNGEON_CANNOT_ENTER_NOT_WORTHY = 169,
    GAMEERROR_SET_DUNGEON_CANNOT_ENTER_CLOSING = 170,
    GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT = 171,
    GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT_IS_IN_GRIFT = 172,
    GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT_SKILL_ON_COOLDOWN = 173,
    GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT_CURRENTLY_EQUIPPED = 174,
    GAMEERROR_EQUIPMENT_MANAGER_CANNOT_EQUIP_POTION = 175,
    GAMEERROR_CHALLENGE_RIFT_NUMBER_MISMATCH = 176,
    GAMEERROR_CHALLENGE_RIFT_ALREADY_STARTED = 177,
    GAMEERROR_AMIIBO_POWER_ALREADY_ACTIVE = 178,
    GAMEERROR_AMIIBO_POWER_IN_COOLDOWN = 179,
    GAMEERROR_AMIIBO_INVALID = 180,
    GAMEERROR_AMIIBO_SCAN_FAILED = 181,
    GAMEERROR_AMIIBO_NO_DEVICE = 182,
    GAMEERROR_AMIIBO_ERROR = 183,
    GAMEERROR_COUNT = 184,
};
void PlayerDisplayErrorMessage(ACDID idACDPlayer, GameError eError);  // Player.cpp:3475

struct HotbarButtonData;  // X/Common/HotbarButton.h
const HotbarButtonData & PlayerGetPotionBtnAssignment(const Player * ptPlayer);  // Player.cpp:1908
void PlayerSetHotbarBtnAssignment(Player * ptPlayer, const SkillButton eButton, const HotbarButtonData & tButtonData);  // Player.cpp:1778
const HotbarButtonData & PlayerGetHotbarBtnAssignment(const Player * ptPlayer, const SkillButton eButton);  // Player.cpp:1846

enum {
    LOGOUT_CONTEXT_SOLO = 0,
    LOGOUT_CONTEXT_PARTY = 1,
    LOGOUT_CONTEXT_BOTH = 2,
    LOGOUT_CONTEXT_COUNT = 2,
};

typedef int32 LogoutContext;

// X/Common/Player.h:310
struct GameErrorData {  // size 0x8
    GameErrorData() : eError(GAMEERROR_NONE), dwVal(0) {}  // line 312
    GameErrorData(const GameError eError) : eError(eError), dwVal(0) {}  // line 318
    GameError eError;  // 0x00
    uint32 dwVal;  // 0x04
};
static_assert(sizeof(GameErrorData) == 0x8, "GameErrorData");
static_assert(offsetof(GameErrorData, dwVal) == 0x4, "GameErrorData");

// X/Common/Player.h:297
enum {
    TRICKLE_INFO_INVALID = 0,
    TRICKLE_INFO_LOCAL_PLAYER = 1,
    TRICKLE_INFO_MY_PET = 2,
    TRICKLE_INFO_FRIENDLY_PLAYER = 3,
    TRICKLE_INFO_FRIENDLY_PLAYER_PET = 4,
    TRICKLE_INFO_FOLLOWER_PET = 5,
    TRICKLE_INFO_HOSTILE_PLAYER = 6,
    TRICKLE_INFO_HOSTILE_PLAYER_PET = 7,
    TRICKLE_INFO_NPC = 8,
    TRICKLE_INFO_FRIENDLY_MONSTER = 9,
    TRICKLE_INFO_HOSTILE_MONSTER = 10,
    TRICKLE_INFO_GIZMO = 11,
    TRICKLE_INFO_PVP_FRIENDLY = 12,
    TRICKLE_INFO_PVP_HOSTILE = 13,
    TRICKLE_INFO_PVP_NEUTRAL = 14,
    TRICKLE_INFO_NUM_TYPES = 15,
};

LPCSTR PlayerGetShortClassName(PlayerClass ePlayerClass);  // Player.cpp:3417
PlayerClass PlayerClassFromGBID(const GBID gbid); // Player.cpp:2255
GBID PlayerClassToGBID(const PlayerClass ePlayerClass); // Player.cpp:2275
void CrafterSavedDataSetDefaults(CrafterSavedData & tData);  // Player.cpp:997
void PlayerInitializeWaypoints(Player * ptPlayer);  // Player.cpp:2226

ACDID PlayerGetHireling(ACDID idACDPlayer); // Player.cpp:2641


BOOL PlayerHasOnGroundCraftingPlan(const Player & tPlayer, GBID gbidPlan);  // Player.cpp:5568

// More of Player.h, for X/Server/SPlayer.cpp.
struct PlayerKickTimerMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    uint32 dwLastKickAttempt;  // 0x08
    uint32 dwLastSuccessfulKick;  // 0x0c
};
static_assert(sizeof(PlayerKickTimerMessage) == 0x10, "PlayerKickTimerMessage");

SNO PlayerGetLastWaypointLevelArea(const Player * ptPlayer);  // Player.cpp:1876

struct HeroStateMessage {  // size 0x47d8
    RequiredMessageHeader tHeader;  // 0x00
    HeroStateData tStateData;  // 0x08
    int32 nPlayerIndex;  // 0x47d4
};
static_assert(sizeof(HeroStateMessage) == 0x47d8, "HeroStateMessage");

enum {
    SSPREF_AUTOEQUIP_ITEMS = 0,
    SSPREF_SHOW_DAMAGE_NUMBERS = 1,
    SSPREF_SHOW_HEAL_NUMBERS = 2,
    SSPREF_SHOW_CRITICALS = 3,
    SSPREF_EQUIPMENT_MANAGER_DROP_TO_STASH = 4,
    SSPREF_COUNT = 5,
};

// X/Common/Player.h:1385
struct NewPlayerMessage {  // size 0x48f0
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
    OnlineService::HeroId uHeroId;  // 0x10
    OnlineService::GameAccountHandle tAccountId;  // 0x18
    CHAR usHeroName[49];  // 0x24
    Team eTeam;  // 0x58
    PlayerClass eClass;  // 0x5c
    SNO snoActorPortrait;  // 0x60
    int32 nCurrentLevel;  // 0x64
    int32 nCurrentAltLevel;  // 0x68
    int32 nCurrentHighestHeroSoloRiftLevel;  // 0x6c
    HeroStateData tHeroStateData;  // 0x70
    BOOL bJustJoined;  // 0x483c
    GameConnectionID idGameConnection;  // 0x4840
    ACDNetworkName ann;  // 0x4844
    CHAR usGamerTag[128];  // 0x4848
    uint64 uAcceptedLicenseBitsHero;  // 0x48c8
    BOOL bUpscaled;  // 0x48d0
    uint32 uPartitionFlags;  // 0x48d4
    uint32 uSeasonCreated;  // 0x48d8
    int64 nNFPDefaultScanTime;  // 0x48e0
    int64 nNFPGoblinScanTime;  // 0x48e8
};
static_assert(sizeof(NewPlayerMessage) == 0x48f0, "NewPlayerMessage");
static_assert(offsetof(NewPlayerMessage, tHeroStateData) == 0x70, "NewPlayerMessage");
static_assert(offsetof(NewPlayerMessage, ann) == 0x4844, "NewPlayerMessage");
static_assert(offsetof(Player, uAcceptedLicenseBitsHero) == 0xebe0, "Player");
static_assert(offsetof(Player, tNFPGoblinScanTime) == 0xdda8, "Player");
// X/Common/Player.h:1045
struct PlayerQuestRewardHistoryUpdateMessage {  // size 0x4bc
    RequiredMessageHeader tHeader;  // 0x00
    int32 nNumQuestRewardEntries;  // 0x08
    PlayerQuestRewardHistoryEntry tQuestRewardHistory[100];  // 0x0c
};
static_assert(sizeof(PlayerQuestRewardHistoryUpdateMessage) == 0x4bc, "PlayerQuestRewardHistoryUpdateMessage");
static_assert(offsetof(PlayerQuestRewardHistoryUpdateMessage, tQuestRewardHistory) == 0xc, "PlayerQuestRewardHistoryUpdateMessage");
// X/Common/Player.h:1064
struct PlayerLoadoutDataMessage {  // size 0x328
    RequiredMessageHeader tHeader;  // 0x00
    int32 nLoadoutIndex;  // 0x08
    LoadoutItemData tEquipment[NUM_PLAYER_BODYSLOTS];  // 0x10
    LoadoutPotionData tEquippedPotion;  // 0x280
    LoadoutSkillData tActiveSkills[6];  // 0x298
    SNO snoTraits[4];  // 0x2c8
    GBID gbidLegendaryPowers[4];  // 0x2d8
    CHAR usName[53];  // 0x2e8
    int32 nTabIcon;  // 0x320
};
static_assert(sizeof(PlayerLoadoutDataMessage) == 0x328, "PlayerLoadoutDataMessage");
static_assert(offsetof(PlayerLoadoutDataMessage, tEquippedPotion) == 0x280, "PlayerLoadoutDataMessage");
static_assert(offsetof(PlayerLoadoutDataMessage, usName) == 0x2e8, "PlayerLoadoutDataMessage");
static_assert(offsetof(PlayerLoadoutDataMessage, nTabIcon) == 0x320, "PlayerLoadoutDataMessage");
// X/Common/Player.h:1100
struct PlayerLoadoutTabIconMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nLoadoutIndex;  // 0x08
    int32 nTabIcon;  // 0x0c
};
static_assert(sizeof(PlayerLoadoutTabIconMessage) == 0x10, "PlayerLoadoutTabIconMessage");
// X/Common/Player.h:1107
struct PlayerLoadoutEquipResultsMessage {  // size 0x9c
    RequiredMessageHeader tHeader;  // 0x00
    BOOL bSocketsChanged;  // 0x08
    ACDNetworkName arRemovedItemANNs[35];  // 0x0c
    int32 nRemoveItemANNsCount;  // 0x98
};
static_assert(sizeof(PlayerLoadoutEquipResultsMessage) == 0x9c, "PlayerLoadoutEquipResultsMessage");
static_assert(offsetof(PlayerLoadoutEquipResultsMessage, nRemoveItemANNsCount) == 0x98, "PlayerLoadoutEquipResultsMessage");

int32 PlayerGetNumPlayersForDifficulty();  // Player.cpp:2159

ACDID PlayerGetFirstPet(ACDID idACDPlayer, PetType eType, int32 & nPos);  // Player.cpp:2589
ACDID PlayerGetNextPet(ACDID idACDPlayer, PetType eType, int32 & nPos);  // Player.cpp:2600

// FloatingNumberType enumerators recovered from Player.h.
enum { FLOATING_NUMBER_TYPE_BROKE_FREEZE = 21, FLOATING_NUMBER_TYPE_BROKE_BLIND = 22, FLOATING_NUMBER_TYPE_BROKE_STUN = 23 };

#line 352 "X/Common/Player.h"
enum { FLOATING_NUMBER_TYPE_ABSORB = 11 };

static_assert(offsetof(LoadoutItemData, unk_20) == 0x20, "LoadoutItemData");
static_assert(offsetof(LoadoutItemData, uSoulShardIdLow) == 0x28, "LoadoutItemData");
static_assert(offsetof(HeroStateData, uPlayerFlags2) == 0xc, "HeroStateData");
static_assert(offsetof(Player, nControllerIndex) == 0xdb74, "Player controller");
static_assert(offsetof(Player, uHeroId) == 0xb208, "Player hero ID");
