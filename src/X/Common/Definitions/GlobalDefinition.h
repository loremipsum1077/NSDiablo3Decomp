// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/GlobalDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../GlobalSNO.h"
#include "../SNO/SNOFiles.h"
#include "../ImageHandle.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/Math/MathGeneral.h"

typedef uint32 StringLabelHandle;  // X/Common/Labels.h

enum {
    MAX_ACTOR_GROUP_NAME_LEN = 64,
    MAX_STARTING_LOCATION_NAME_LEN = 64,
    MAX_GLOBAL_SCRIPT_VARIABLE_NAME_LEN = 32,
    MAX_BANNER_COLOR_TAG_LEN = 64,
    MAX_BANNER_SIGIL_PLACEMENT_NAME_LEN = 64,
    MAX_EPIC_BANNER_TAG_LEN = 128,
    MAX_RUNE_RANKS = 7,
    NUM_UNLOCKABLE_STASH_TABS = 5,
    DYNAMIC_ENTRANCE_GUID_START = 100000,
    DYNAMIC_ENTRANCE_GUID_MAX = 200000,
};

enum {
    SAVED_DYE_TYPE_NONE = 0,
    SAVED_DYE_TYPE_01 = 1,
    SAVED_DYE_TYPE_02 = 2,
    SAVED_DYE_TYPE_03 = 3,
    SAVED_DYE_TYPE_04 = 4,
    SAVED_DYE_TYPE_05 = 5,
    SAVED_DYE_TYPE_06 = 6,
    SAVED_DYE_TYPE_07 = 7,
    SAVED_DYE_TYPE_08 = 8,
    SAVED_DYE_TYPE_09 = 9,
    SAVED_DYE_TYPE_10 = 10,
    SAVED_DYE_TYPE_11 = 11,
    SAVED_DYE_TYPE_12 = 12,
    SAVED_DYE_TYPE_13 = 13,
    SAVED_DYE_TYPE_14 = 14,
    SAVED_DYE_TYPE_15 = 15,
    SAVED_DYE_TYPE_16 = 16,
    SAVED_DYE_TYPE_17 = 17,
    SAVED_DYE_TYPE_18 = 18,
    SAVED_DYE_TYPE_19 = 19,
    SAVED_DYE_TYPE_20 = 20,
    SAVED_DYE_TYPE_VANISHING = 21,
    SAVED_DYE_TYPE_21 = 22,
    MAX_SAVED_DYE_VALUE = 23,
};

typedef int32 SavedDyeType;

enum {
    DYE_TYPE_NONE = 0,
    DYE_TYPE_VANISHING = 1,
    DYE_TYPE_FIRST_COLOR = 2,
    DYE_TYPE_01 = 2,
    DYE_TYPE_02 = 3,
    DYE_TYPE_03 = 4,
    DYE_TYPE_04 = 5,
    DYE_TYPE_05 = 6,
    DYE_TYPE_06 = 7,
    DYE_TYPE_07 = 8,
    DYE_TYPE_08 = 9,
    DYE_TYPE_09 = 10,
    DYE_TYPE_10 = 11,
    DYE_TYPE_11 = 12,
    DYE_TYPE_12 = 13,
    DYE_TYPE_13 = 14,
    DYE_TYPE_14 = 15,
    DYE_TYPE_15 = 16,
    DYE_TYPE_16 = 17,
    DYE_TYPE_17 = 18,
    DYE_TYPE_18 = 19,
    DYE_TYPE_19 = 20,
    DYE_TYPE_20 = 21,
    DYE_TYPE_21 = 22,
    MAX_DYE_VALUE = 23,
    NUM_DYE_COLORS = 21,
    DYE_TYPE_PROMO_01 = 14,
    DYE_TYPE_PROMO_02 = 21,
};

typedef int32 DyeType;

enum {
    BANNER_BACKGROUND = 0,
    BANNER_PATTERN = 1,
    BANNER_SIGIL_MAIN = 2,
    BANNER_SIGIL_ACCENT = 3,
    NUM_BANNER_SNOS = 4,
};

enum {
    BANNERCOLOR_BACKGROUND = 0,
    BANNERCOLOR_PATTERN = 1,
    BANNERCOLOR_SIGIL = 2,
    NUM_BANNER_COLORS = 3,
};

enum {
    BANNERCOLORTYPE_MAIN = 0,
    BANNERCOLORTYPE_DUOTONE = 1,
    NUM_BANNERCOLOR_TYPES = 2,
};

enum {
    BANNERATTACHMENT_BASE = 0,
    BANNERATTACHMENT_CAP = 1,
    BANNERATTACHMENT_POLE = 2,
    BANNERATTACHMENT_RIBBON = 3,
    NUM_BANNER_ATTACHMENTS = 4,
};

enum BannerColorSetFlags {
    DEFAULT_SIGIL_COLOR_BIT = 0,
    DEFAULT_SHAPE_COLOR_BIT = 1,
    DEFAULT_PATTERN_COLOR_BIT = 2,
};

enum SkillSwapCoolupTypes {
    SWAP_COOLUP_NORMAL = 0,
    SWAP_COOLUP_MAX_LEVEL = 1,
    SWAP_COOLUP_COUNT = 2,
};

enum {
    ASSET_LIST_CONVERSATION_SNOS = 1,
    ASSET_LIST_TUTORIAL_SNOS = 2,
    ASSET_LIST_ITEM_ACTOR_SNOS = 3,
    ASSET_LIST_LORE_SNOS = 4,
    ASSET_LIST_RECIPE_GBIDS = 5,
    ASSET_LIST_TRANSMOG_GBIDS = 6,
    ASSET_LIST_COSMETIC_ITEM_GBIDS = 7,
    ASSET_LIST_EXTRACTABLE_LEGENDARY_GBIDS = 8,
    NUM_ASSET_LISTS = 9,
};

typedef int32 AssetListType;

struct ActorGroup {  // size 0x44
    uint32 uHash;  // 0x00
    CHAR szName[64];  // 0x04
};
static_assert(sizeof(ActorGroup) == 0x44, "ActorGroup");

struct StartLocationName {  // size 0x44
    uint32 uGUID;  // 0x00
    CHAR szName[64];  // 0x04
};
static_assert(sizeof(StartLocationName) == 0x44, "StartLocationName");

struct GlobalScriptVariable {  // size 0x28
    uint32 uHash;  // 0x00
    CHAR szName[32];  // 0x04
    float flDefaultValue;  // 0x24
};
static_assert(sizeof(GlobalScriptVariable) == 0x28, "GlobalScriptVariable");

struct DifficultyTuningParams {  // size 0x34
    float flMonsterHPPercentPerPlayer;  // 0x00
    float flMonsterXPPercentPerPlayer;  // 0x04
    float flMonsterDamagePercentPerPlayer;  // 0x08
    float flHealthGlobeReductionPercentPerPlayer;  // 0x0c
    float flInitialCooldownScale;  // 0x10
    float flAttackCooldownScale;  // 0x14
    float flClosingCooldownScale;  // 0x18
    float flLifeDrainScalar;  // 0x1c
    float flEliteMonsterHPScalar;  // 0x20
    float flUniqueAndBossMonsterHPScalar;  // 0x24
    float flTrashItemConvertChanceWhite;  // 0x28
    float flTrashItemConvertChanceMagic;  // 0x2c
    float flTrashItemConvertChanceRare;  // 0x30
};
static_assert(sizeof(DifficultyTuningParams) == 0x34, "DifficultyTuningParams");

struct BannerTexturePair {  // size 0x10
    SNO snoTexture;  // 0x00
    ImageHandle hIcon;  // 0x04
    StringLabelHandle hName;  // 0x08
    uint32 idReward;  // 0x0c
};
static_assert(sizeof(BannerTexturePair) == 0x10, "BannerTexturePair");

struct BannerColorSet {  // size 0x54
    void CopyValues(const BannerColorSet &);
    RGBAColor rgbaTint[2];  // 0x00
    CHAR szTagName[64];  // 0x08
    ImageHandle hIcon;  // 0x48
    uint32 dwFlags;  // 0x4c
    uint32 idReward;  // 0x50
    static uint32 XHash(const BannerColorSet & tBannerColorSet) {  // line 265
        return XHashKey<LPCSTR>(tBannerColorSet.szTagName);
    }
};
static_assert(sizeof(BannerColorSet) == 0x54, "BannerColorSet");

struct BannerSigilPlacement {  // size 0x44
    CHAR szName[64];  // 0x00
    ImageHandle hIcon;  // 0x40
};
static_assert(sizeof(BannerSigilPlacement) == 0x44, "BannerSigilPlacement");

struct EpicBannerDescription {  // size 0x94
    SNO snoBannerShape;  // 0x00
    SNO snoBannerBase;  // 0x04
    SNO snoBannerPole;  // 0x08
    ImageHandle hIcon;  // 0x0c
    CHAR szTagName[128];  // 0x10
    uint32 idReward;  // 0x90
    static uint32 XHash(const EpicBannerDescription & tEpicBannerDescription) {  // line 289
        return XHashKey<LPCSTR>(tEpicBannerDescription.szTagName);
    }
};
static_assert(sizeof(EpicBannerDescription) == 0x94, "EpicBannerDescription");

struct BannerParamsMaps {  // size 0x400
    ~BannerParamsMaps() {}  // line 295
    // line 297
    BannerParamsMaps(int32 nNumBackgrounds, int32 nNumPatterns, int32 nNumMainSigils, int32 nNumVariantSigils,
                     int32 nNumSigilAccents, int32 nNumColorSets, int32 nNumSigilPlacements,
                     int32 nNumEpicBannerDescriptions)
        : mapTexBackgrounds(16, NULL), mapTexPatterns(16, NULL), mapTexMainSigils(16, NULL),
          mapTexVariantSigils(16, NULL), mapTexSigilAccents(16, NULL), mapColorSets(16, NULL),
          mapSigilPlacements(16, NULL), mapEpicBannerDescriptions(16, NULL) {
        mapTexBackgrounds.SetHashTableSize_Tracked(Math::Max(nNumBackgrounds, 0));  // line 307
        mapTexPatterns.SetHashTableSize_Tracked(Math::Max(nNumPatterns, 0));
        mapTexMainSigils.SetHashTableSize_Tracked(Math::Max(nNumMainSigils, 0));
        mapTexVariantSigils.SetHashTableSize_Tracked(Math::Max(nNumVariantSigils, 0));
        mapTexSigilAccents.SetHashTableSize_Tracked(Math::Max(nNumSigilAccents, 0));
        mapColorSets.SetHashTableSize_Tracked(Math::Max(nNumColorSets, 0));
        mapSigilPlacements.SetHashTableSize_Tracked(Math::Max(nNumSigilPlacements, 0));
        mapEpicBannerDescriptions.SetHashTableSize_Tracked(Math::Max(nNumEpicBannerDescriptions, 0));  // line 314
    }
    typedef XGrowableMap<unsigned int, unsigned int, unsigned int, unsigned int> BannerParamsIndexMap;
    BannerParamsMaps::BannerParamsIndexMap mapTexBackgrounds;  // 0x00
    BannerParamsMaps::BannerParamsIndexMap mapTexPatterns;  // 0x80
    BannerParamsMaps::BannerParamsIndexMap mapTexMainSigils;  // 0x100
    BannerParamsMaps::BannerParamsIndexMap mapTexVariantSigils;  // 0x180
    BannerParamsMaps::BannerParamsIndexMap mapTexSigilAccents;  // 0x200
    BannerParamsMaps::BannerParamsIndexMap mapColorSets;  // 0x280
    BannerParamsMaps::BannerParamsIndexMap mapSigilPlacements;  // 0x300
    BannerParamsMaps::BannerParamsIndexMap mapEpicBannerDescriptions;  // 0x380
};
static_assert(sizeof(BannerParamsMaps) == 0x400, "BannerParamsMaps");

struct BannerParams {  // size 0xf0
    XPtr64<BannerTexturePair> arTexBackgrounds;  // 0x00
    SerializeData serTexBackgrounds;  // 0x08
    int32 nTexBackgroundDefaults;  // 0x10
    XPtr64<BannerTexturePair> arTexPatterns;  // 0x18
    SerializeData serTexPatterns;  // 0x20
    int32 nTexPatternDefaults;  // 0x28
    XPtr64<BannerTexturePair> arTexMainSigils;  // 0x30
    SerializeData serTexMainSigils;  // 0x38
    XPtr64<BannerTexturePair> arTexVariantSigils;  // 0x40
    SerializeData serTexVariantSigils;  // 0x48
    int32 nTexSigilDefaults;  // 0x50
    XPtr64<BannerTexturePair> arTexSigilAccents;  // 0x58
    SerializeData serTexSigilAccents;  // 0x60
    int32 nTexSigilAccentDefaults;  // 0x68
    XPtr64<BannerColorSet> arColorSets;  // 0x70
    SerializeData serColorSets;  // 0x78
    XPtr64<BannerSigilPlacement> arSigilPlacements;  // 0x80
    SerializeData serSigilPlacements;  // 0x88
    XPtr64<int> arActorBases;  // 0x90
    SerializeData serActorBases;  // 0x98
    XPtr64<int> arActorCaps;  // 0xa0
    SerializeData serActorCaps;  // 0xa8
    XPtr64<int> arActorPoles;  // 0xb0
    SerializeData serActorPoles;  // 0xb8
    XPtr64<int> arActorRibbons;  // 0xc0
    SerializeData serActorRibbons;  // 0xc8
    XPtr64<EpicBannerDescription> arEpicBannerDescriptions;  // 0xd0
    SerializeData serEpicBannerDescriptions;  // 0xd8
    XPtr64<BannerParamsMaps> ptParamsMaps;  // 0xe0
    uint32 nLastAllocatedRewardId;  // 0xe8
};
static_assert(sizeof(BannerParams) == 0xf0, "BannerParams");

struct AssetListEntry {  // size 0x8
    int32 nID;  // 0x00
    uint32 nIndex;  // 0x04
};
static_assert(sizeof(AssetListEntry) == 0x8, "AssetListEntry");

struct AssetList {  // size 0x10
    XPtr64<AssetListEntry> arEntries;  // 0x00
    SerializeData serEntries;  // 0x08
};
static_assert(sizeof(AssetList) == 0x10, "AssetList");

struct GlobalServerData {  // size 0x380
    XPtr64<ActorGroup> arActorGroups;  // 0x00
    SerializeData serActorGroups;  // 0x08
    XPtr64<GlobalScriptVariable> arScriptGlobalVars;  // 0x10
    SerializeData serScriptGlobalVars;  // 0x18
    DifficultyTuningParams tDifficultyParams[6];  // 0x20
    float flMonsterGetHitHealthPercent;  // 0x158
    float flChillDefaultMagnitude;  // 0x15c
    float flChillDefaultDuration;  // 0x160
    float flChillDefaultMagnitudePlayer;  // 0x164
    float flChillDefaultDurationPlayer;  // 0x168
    float flBleedDuration;  // 0x16c
    float flMonkSpiritGainedFromBasicAttack;  // 0x170
    float flHitChance;  // 0x174
    int32 nArenaPremadeHeroLevel;  // 0x178
    float flDurLossFrequency;  // 0x17c
    float flPlayerResurrectionTimer;  // 0x180
    float flHirelingResurrectionTimer;  // 0x184
    float flPlayerGhostDuration;  // 0x188
    float flPlayerBaseResurrectionDelay;  // 0x18c
    float flPVPPregameDuration;  // 0x190
    float flPVPGameDuration;  // 0x194
    float flPVPPlayerGhostDuration;  // 0x198
    float flPVPPlayerResurrectionDelay;  // 0x19c
    float flPVPPlayerCCReduction;  // 0x1a0
    uint32 nAfkTime;  // 0x1a4
    uint32 nCalloutDelayAfterConv;  // 0x1a8
    int32 nActTransitionTime;  // 0x1ac
    float flComboServerTimeout;  // 0x1b0
    float flMultiplayerMonsterXPMultiplier;  // 0x1b4
    float flMultiplayerQuestXPMultiplier;  // 0x1b8
    float flFollowerHealthGlobeBonusPercent;  // 0x1bc
    float flHealthGlobeDropDistance;  // 0x1c0
    float flPetAoEDamageScalar;  // 0x1c4
    float flHealthShrineConvertChance;  // 0x1c8
    int32 nMinMonsterLevelForShrineGlobe;  // 0x1cc
    float flHirelingCoreAttributeBonusMultiplier;  // 0x1d0
    int32 nTargetedLegendaryDropChanceBase;  // 0x1d4
    int32 nTargetedLegendaryDropChanceIncrement;  // 0x1d8
    int32 nTargetedLegendaryDropChanceIncrementDelta;  // 0x1dc
    int32 nTargetedRareDropChanceBase;  // 0x1e0
    int32 nTargetedRareDropChanceIncrement;  // 0x1e4
    int32 nTargetedRareDropChanceIncrementDelta;  // 0x1e8
    int32 nTargetedMagicDropChanceBase;  // 0x1ec
    int32 nTargetedMagicDropChanceIncrement;  // 0x1f0
    int32 nTargetedMagicDropChanceIncrementDelta;  // 0x1f4
    float flUnluckyBonusMinHours;  // 0x1f8
    float flUnluckyBonusChancePerHour;  // 0x1fc
    float arLegendaryChanceMultiplier[17];  // 0x200
    float flHirelingTargetedItemChance;  // 0x244
    float flHirelingTargetedInactiveChance;  // 0x248
    int32 nDungeonFinderLevelCount;  // 0x24c
    int32 nDungeonFinderLevelsPerTileset;  // 0x250
    int32 nDungeonFinderEliteValue;  // 0x254
    float flBindOnEquipDropChance;  // 0x258
    int32 nMinutesToRerollVendorInventory;  // 0x25c
    float flBaseGlobeDropChance;  // 0x260
    float flSpenderGlobeDropChance;  // 0x264
    float flSpecialMonsterGlobeDropChance;  // 0x268
    float flOverkillGlobeDropChance;  // 0x26c
    float flNephalemGloryActiveGlobeDropChance;  // 0x270
    float flOverkillNephalemGloryDropChance;  // 0x274
    int32 nCorpsesMax;  // 0x278
    float flCorpseMaxSpawnChance;  // 0x27c
    float flCorpseChanceReductionOnCorpseSpawn;  // 0x280
    float flCorpseChanceIncreasePerSecond;  // 0x284
    float flCorpseChanceIncreaseForElites;  // 0x288
    int32 nCorpseCapPerArea;  // 0x28c
    float flCorpseRadiusForCap;  // 0x290
    float flVoteKickAfterBossDelay;  // 0x294
    float flVoteKickAfterChampRareUniqueDelay;  // 0x298
    float flVoteKickTimeInGameDelay;  // 0x29c
    float flTimeInGameForPositiveKarma;  // 0x2a0
    float arSkillSwapCoolupTime[2];  // 0x2a4
    float flBaseGoldPickupRadius;  // 0x2ac
    float flVacuumPickupRadius;  // 0x2b0
    float flAttackSpeedItemPercentCap;  // 0x2b4
    float flCooldownMinimumTime;  // 0x2b8
    float flThornsPrimaryStatMultiplier;  // 0x2bc
    float flShatterMultiplierNonElites;  // 0x2c0
    float flCrushingBlowDamageExponent;  // 0x2c4
    float flMovementScalarCap;  // 0x2c8
    uint32 nPlatinumPerDailyGoblin;  // 0x2cc
    uint32 nPlatinumPerDailyBoss;  // 0x2d0
    uint32 nPlatinumPerDailyBounty;  // 0x2d4
    int32 nLootRunChampionProgressGlobeCount;  // 0x2d8
    int32 nLootRunRareProgressGlobeCount;  // 0x2dc
    int32 nLootRunGoblinProgressGlobeCount;  // 0x2e0
    float flLootRunProgressGlobeXPScalar;  // 0x2e4
    float flLootRunProgressGlobeDropRadiusMin;  // 0x2e8
    float flLootRunProgressGlobeDropRadiusDelta;  // 0x2ec
    float flGlobalMonsterXPDensityMin;  // 0x2f0
    float flGlobalMonsterXPDensityMax;  // 0x2f4
    float flGlobalEliteDensity;  // 0x2f8
    float arAncientLegendaryChance[13];  // 0x2fc
    float flPrimalAncientLegendaryChance;  // 0x330
    float flCrafterAncientLegendaryChance;  // 0x334
    int32 nMinimumAchievedRiftLevelToRollPrimalAncients;  // 0x338
    int32 nAncientSalvageMultiplier;  // 0x33c
    int32 nPrimalAncientSalvageMultiplier;  // 0x340
    int32 nOpenWorldBountiesTotalMin;  // 0x344
    int32 nOpenWorldBountiesTotalDelta;  // 0x348
    int32 nOpenWorldBountiesKillBossMin;  // 0x34c
    int32 nOpenWorldBountiesKillBossDelta;  // 0x350
    int32 nOpenWorldBountiesClearDungeonMin;  // 0x354
    int32 nOpenWorldBountiesClearDungeonDelta;  // 0x358
    int32 nOpenWorldBountiesCompleteEventMin;  // 0x35c
    int32 nOpenWorldBountiesCompleteEventDelta;  // 0x360
    int32 nOpenWorldBountiesKillUniqueMin;  // 0x364
    int32 nOpenWorldBountiesKillUniqueDelta;  // 0x368
    int32 nOpenWorldBountyCampMin;  // 0x36c
    int32 nOpenWorldBountyCampDelta;  // 0x370
    uint32 nPlatinumAwardMaxTimeToScale;  // 0x374
    uint32 nPlatinumAwardMinAmountForRoll;  // 0x378
    float flPlatinumAwardPercentToStartLimiting;  // 0x37c
};
static_assert(sizeof(GlobalServerData) == 0x380, "GlobalServerData");

struct GlobalDefinition {  // size 0x3f8
    RequiredDefinitionHeader tHeader;  // 0x00
    XPtr64<GlobalServerData> ptServerData;  // 0x10
    SerializeData serServerData;  // 0x18
    uint32 uStartingLocationGUID;  // 0x20
    XPtr64<StartLocationName> arStartLocationNames;  // 0x28
    SerializeData serStartLocationNames;  // 0x30
    float flSellPricePercent;  // 0x38
    float flComboClientTimeout;  // 0x3c
    float flPlayerPowerCacheTimeoutNormal;  // 0x40
    float flPlayerPowerCacheTimeoutInterrupt;  // 0x44
    uint32 nUIBusyTime;  // 0x48
    int32 nTrialVersionLevelCap;  // 0x4c
    float flRepairCostScalar;  // 0x50
    float flRepairCostScalarHighLevel;  // 0x54
    int32 nDurabilityScaleFactor;  // 0x58
    float flItemsDamagedThresh;  // 0x5c
    float flEnchantCraftedItemCostScalar;  // 0x60
    float flHirelingDiminishedBonusRatio;  // 0x64
    float flMinimapRevealRadius;  // 0x68
    float flAudioMin3DPanLevel;  // 0x6c
    float flAudioMin3DPanLevelRadius;  // 0x70
    float flAudioMax3DPanLevelRadius;  // 0x74
    float flMonsterCritDamageScalar;  // 0x78
    float flPlayerCritDamageScalar;  // 0x7c
    int32 nStashSlotsCost;  // 0x80
    int32 nStashTabCost[5];  // 0x84
    BannerParams tBannerParams;  // 0x98
    ImageHandle hSavepointWaypointButtonNormal;  // 0x188
    ImageHandle hSavepointWaypointButtonMouseOver;  // 0x18c
    ImageHandle hSavepointWaypointButtonPushed;  // 0x190
    StringLabelHandle hSavepointFolderLabel;  // 0x194
    float flPlayerErrorCooldownTime;  // 0x198
    uint32 nPVPTokenRedemptionCost;  // 0x19c
    float flFireDamageProcChance;  // 0x1a0
    float flLightningDamageProcChance;  // 0x1a4
    float flColdDamageProcChance;  // 0x1a8
    float flPoisonDamageProcChance;  // 0x1ac
    float flArcaneDamageProcChance;  // 0x1b0
    float flHolyDamageProcChance;  // 0x1b4
    float flSplashDamageEffectProcChance;  // 0x1b8
    float flCrushingBlowDamagePercentNormal;  // 0x1bc
    float flCrushingBlowDamagePercentElite;  // 0x1c0
    float flCrushingBlowDamagePercentBosses;  // 0x1c4
    float flFireDamageProcHitpointPercentToDamage;  // 0x1c8
    float flFireDamageProcDuration;  // 0x1cc
    float flLightningDamageProcChainRadius;  // 0x1d0
    float flLightningDamageProcChainTargets;  // 0x1d4
    float flLightningDamageProcChainDamagePercent;  // 0x1d8
    float flPoisonDamageProcDamagePercentToDamage;  // 0x1dc
    float flPoisonDamageProcDuration;  // 0x1e0
    float flArcaneDamageProcDuration;  // 0x1e4
    float flHolyDamageProcHitpointPercentToHeal;  // 0x1e8
    float flSplashDamageEffectRadius;  // 0x1ec
    float flHealthPotionHealPercent;  // 0x1f0
    BOOL bOverkillEnabled;  // 0x1f4
    float flPlayerDelayBeforeCorpseRevival;  // 0x1f8
    int32 nCorpseResurrectionMaxCharges;  // 0x1fc
    int32 nCorpseResurrectionChargeRegenTime;  // 0x200
    float flRestXPBonusPercent;  // 0x204
    float flRestXPGrantLevelPercent;  // 0x208
    float flRestXPGrantLevelCapModifier;  // 0x20c
    float flLootRunXPPercentToComplete;  // 0x210
    float flTieredLootRunXPPercentToComplete;  // 0x214
    float flTieredLootRunDelayPerDeath;  // 0x218
    float flTieredLootRunDelayDeathMax;  // 0x21c
    AssetList tAssetLists[9];  // 0x220
    float flVoteKickAttemptDelay;  // 0x2b0
    float flVoteKickSuccessfulDelay;  // 0x2b4
    float flControllerAutoTargetingSearchNearWidth;  // 0x2b8
    float flControllerAutoTargetingSearchAngle;  // 0x2bc
    float flControllerAutoTargetingSearchRange;  // 0x2c0
    float flControllerAutoTargetWreckablesRange;  // 0x2c4
    float flControllerAutoTargetWreckablesRangeHeight;  // 0x2c8
    float flControllerAutoTargetingStickyNearWidth;  // 0x2cc
    float flControllerAutoTargetingStickyAngle;  // 0x2d0
    float flControllerDeadZone;  // 0x2d4
    float flControllerMoveFacingZone;  // 0x2d8
    float flControllerMinMoveSpeed;  // 0x2dc
    float flControllerMaxMoveThreshold;  // 0x2e0
    float flControllerEvadeZone;  // 0x2e4
    float flControllerReboundDetectionZone;  // 0x2e8
    int32 nControllerReboundDuration;  // 0x2ec
    float flControllerReboundDeadZone;  // 0x2f0
    float flControllerAutoTargetingCloseRange;  // 0x2f4
    float flControllerAutoTargetingCloseAngle;  // 0x2f8
    float flControllerAutoTargetingSearchAngleGrowthRate;  // 0x2fc
    float flControllerAutoTargetingSearchGrowthRate;  // 0x300
    int32 nMaxTimesToGrow;  // 0x304
    float flTargetRangeGrowthPerSecond;  // 0x308
    float flTargetRangeMaxGrowth;  // 0x30c
    int32 holdTime;  // 0x310
    float flControllerAutoTargetingSearchRangeSecondary;  // 0x314
    float flControllerAutoTargetingSearchAngleSecondary;  // 0x318
    float flControllerAutoTargetingSearchNearWidthSecondary;  // 0x31c
    float flShuffleMovementSpeedBonus;  // 0x320
    float flShuffleMinMoveDistance;  // 0x324
    float flControllerAutoTargetingSearchAimModeGrowth;  // 0x328
    float flControllerLeftTriggerPressedForAimMode;  // 0x32c
    float flControllerNormalTurnSpeed;  // 0x330
    float flControllerFastTurnSpeed;  // 0x334
    float flControllerFastTurnThreshold;  // 0x338
    float flControllerPersistenceStickAngle;  // 0x33c
    int32 nMaximumAttackersPerPlayer;  // 0x340
    int32 nMaximumLargeAttackersPerPlayer;  // 0x344
    float flLocalCoopDropsPerAdditionalPlayer;  // 0x348
    float flLocalCoopMagicFindBonusPerPlayer;  // 0x34c
    float flPlayerBaseBossFightResurrectionDelay;  // 0x350
    float flEvadeBackAngle;  // 0x354
    float flGreyToGoldConversionChance;  // 0x358
    float flGoldReductionScalar;  // 0x35c
    float flTargetedClassItemChance;  // 0x360
    int32 nParagonCategoryResetCost;  // 0x364
    int32 nAvengerUpdateCheckInterval;  // 0x368
    int32 nAvengerCreationCooldown;  // 0x36c
    int32 nAvengerSpawnCooldown;  // 0x370
    int32 nAvengerSpawnWaitTimeMinutes;  // 0x374
    int32 nAvengerMinimumPlayerLevel;  // 0x378
    float flAvengerSpawnChance;  // 0x37c
    float flAvengerCreationChance;  // 0x380
    float flFriendGiftDropChance;  // 0x384
    float flMassacreStartingTimer;  // 0x388
    float flMassacreStreakTimerCoop;  // 0x38c
    float flMassacreStreakTimerRanged;  // 0x390
    float flMassacreStreakTimerMelee;  // 0x394
    int32 nMassacreMinCountSolo;  // 0x398
    int32 nMassacreMinCountCoop;  // 0x39c
    float flMassacreXPBonusLevel0;  // 0x3a0
    float flMassacreXPBonusLevel1;  // 0x3a4
    float flMassacreXPBonusLevel2;  // 0x3a8
    float flMassacreXPBonusLevel3;  // 0x3ac
    float flMassacreXPBonusLevel4;  // 0x3b0
    float flMassacreXPBonusLevel5;  // 0x3b4
    float flMassacreXPBonusLevel6;  // 0x3b8
    float flMassacreXPBonusLevel7;  // 0x3bc
    float flMassacreXPBonusLevel8;  // 0x3c0
    float flMassacreXPBonusLevel9;  // 0x3c4
    int32 nApprenticeshipLevelGap;  // 0x3c8
    float flMinimumStatScale;  // 0x3cc
    float flApprenticeshipXPBonus;  // 0x3d0
    int32 nApprenticeshipLevelRequirement;  // 0x3d4
    int32 nAnniversaryPrecursorStartMonth;  // 0x3d8
    int32 nAnniversaryPrecursorStartDay;  // 0x3dc
    int32 nAnniversaryEventStartMonth;  // 0x3e0
    int32 nAnniversaryEventStartDay;  // 0x3e4
    int32 nAnniversaryEventLastMonth;  // 0x3e8
    int32 nAnniversaryEventLastDay;  // 0x3ec
    int nPad;  // 0x3f0
    int nPad2;  // 0x3f4
};
static_assert(sizeof(GlobalDefinition) == 0x3f8, "GlobalDefinition");

class GlobalGroupDef : public GroupDefinition {  // size 0xa0
public:
    GlobalGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 732; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 735
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 737
        return SNOPACKTYPE_SERVERDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual void Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer, uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 14
};
static_assert(sizeof(GlobalGroupDef) == 0xa0, "GlobalGroupDef");

extern GlobalGroupDef * gptGroupDefGlobal;  // GlobalDefinition.cpp:490

typedef ScopedSingletonDefinitionLockR<GlobalDefinition> ScopedGlobalDefinitionLockR;

// Line 790. Written from the copy inlined into Minimap::DiscoverMinimapMarkers.
template <> inline const GlobalDefinition * DefinitionLockR<GlobalDefinition>() {
    return (const GlobalDefinition *)gptGroupDefGlobal->LockRFast(GlobalSNOGet(GLOBALSNO_GLOBALS));
}

// GlobalDefinition.cpp: the app subsystem entry points (X/Common/Game.cpp sgtAppSubsystems).
void GlobalDefinitionInitializeForApp();
void GlobalDefinitionDisposeForApp();

// GlobalDefinition.cpp:1240,1258,1288,1448,1483.
uint32 ActorGroupNameToHash(LPCSTR szGroupName);
LPCSTR ActorGroupHashToName(uint32 uHash);
int32 ActorGroupHashToIndex(uint32 uHash);
LPCSTR StartingLocationGUIDToName(uint32 uGUID);
uint32 StartingLocationNameToGUID(LPCSTR szName);

// GlobalDefinition.cpp:1746
DyeType DyeTypeTranslateFromSavedDyeType(const SavedDyeType eSavedDye);

// GlobalDefinition.cpp:1606, 1680 (called from X/Common/Crafting/ItemCrafting.cpp).
int32 AssetListArrayToBitfield(AssetListType eAssetList, const int32 * ptAssetIDArray, int32 nAssets, uint8 * ptByteArray, int32 nByteArrayLength, int32 * pnLeadingNullBytes);
int32 AssetListBitfieldToArray(AssetListType eAssetList, const uint8 * ptByteArray, int32 nByteArrayLength, int32 * ptAssetIDArray, int32 nMaxAssets, int32 nLeadingNullBytes);
