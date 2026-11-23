// Original file: X/Common/Definitions/GlobalDefinition.cpp

#include "GlobalDefinition.h"

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../Sigma/DataStructures/XBaseMap.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include <string.h>
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

enum {
    GLOBAL_XML_VERSION = 3,
    BANNER_PARAMS_VERSION = 10,
    GLOBAL_COMMON_SNO_VERSION = 232,
    GLOBAL_PC_SNO_VERSION = 234,
    GLOBAL_CONSOLE_SNO_VERSION = 253,
    GLOBAL_AXE_SNO_VERSION = 252,
    BANNERCOLORSET_XML_VERSION = 1,
};

typedef XGrowableMap<int, int, unsigned int, unsigned int> AssetListIDToBitMap;

struct GlobalDefinitionAppGlobals {  // size 0x8
    AssetListIDToBitMap * pmapAssetListMaps;  // 0x00
};

struct SavedDyeDef {  // size 0x8
    SavedDyeType eSavedDyeType;  // 0x00
    DyeType eDyeType;  // 0x04
};

// X/Common/Game.h:543 and Game.cpp:412. AppGlobals is 0x23a8 bytes of mostly unadopted types;
// only the GlobalDefinition app globals are read here.
#include "../Game.h"




extern AppGlobals sgtAppGlobals;

typedef int32 DifficultyLevel;  // X/Common/GameCommonData.h:129

// Defined in X/Common/Game.cpp:3511; no adopted header declares it yet.
DifficultyLevel GameGetDifficultyParamsLevel(BOOL bIsFromTieredRift);

// Lines 72-480: the type descriptors (_GLOBAL__sub_I_GlobalDefinition.cpp). Generated with
// build/try/solo/desc.py from the original static initializer; the field names are "" in the binary.
static const TypeDescriptorField ActorGroup_Fields[3] = {
    TypeDescriptorField("", DT_INT, offsetof(ActorGroup, uHash), NULL, 0),
    CharArrayTypeDescriptorField("", offsetof(ActorGroup, szName), "", 1, 0x40, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActorGroup), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActorGroup("ActorGroup", 0, ActorGroup_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACTOR_GROUP_STRUCT = &TypeDescriptor_ActorGroup;

static const TypeDescriptorField StartLocationName_Fields[3] = {
    TypeDescriptorField("", DT_INT, offsetof(StartLocationName, uGUID), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(StartLocationName, szName), "", 1, 0x40, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(StartLocationName), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_StartLocationName("StartLocationName", 0, StartLocationName_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_STARTING_LOCATION_NAME = &TypeDescriptor_StartLocationName;

static const TypeDescriptorField GlobalScriptVariable_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(GlobalScriptVariable, uHash), NULL, 0),
    CharArrayTypeDescriptorField("", offsetof(GlobalScriptVariable, szName), "", 1, 0x20, -1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalScriptVariable, flDefaultValue), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GlobalScriptVariable), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GlobalScriptVariable("GlobalScriptVariable", 0, GlobalScriptVariable_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GLOBAL_SCRIPT_VARIABLE_STRUCT = &TypeDescriptor_GlobalScriptVariable;

static const TypeDescriptorField DifficultyTuningParams_Fields[14] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flMonsterHPPercentPerPlayer), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flMonsterXPPercentPerPlayer), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flMonsterDamagePercentPerPlayer), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flHealthGlobeReductionPercentPerPlayer), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flInitialCooldownScale), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flAttackCooldownScale), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flClosingCooldownScale), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flLifeDrainScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flEliteMonsterHPScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flUniqueAndBossMonsterHPScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flTrashItemConvertChanceWhite), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flTrashItemConvertChanceMagic), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DifficultyTuningParams, flTrashItemConvertChanceRare), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DifficultyTuningParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DifficultyTuningParams("DifficultyTuningParams", 0, DifficultyTuningParams_Fields, 14, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_DIFFICULTY_TUNING_PARAMS = &TypeDescriptor_DifficultyTuningParams;

static const TypeDescriptorField BannerTexturePair_Fields[4] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(BannerTexturePair, snoTexture), "", TEXTURE_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(BannerTexturePair, hIcon), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BannerTexturePair, hName), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BannerTexturePair), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BannerTexturePair("BannerTexturePair", 0, BannerTexturePair_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_BANNER_TEXTURE_PAIR = &TypeDescriptor_BannerTexturePair;

static const TypeDescriptorField BannerColorSet_Fields[5] = {
    FixedArrayTypeDescriptorField("", DT_RGBACOLOR, offsetof(BannerColorSet, rgbaTint), NULL, 1, 2, 0),
    CharArrayTypeDescriptorField("", offsetof(BannerColorSet, szTagName), "", 1, 0x40, -1),
    TypeDescriptorField("", DT_INT, offsetof(BannerColorSet, hIcon), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BannerColorSet, dwFlags), NULL, 0x80001),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BannerColorSet), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BannerColorSet("BannerColorSet", BANNERCOLORSET_XML_VERSION, BannerColorSet_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_BANNER_COLOR_SET = &TypeDescriptor_BannerColorSet;

static const TypeDescriptorField BannerSigilPlacement_Fields[3] = {
    CharArrayTypeDescriptorField("", offsetof(BannerSigilPlacement, szName), "", 1, 0x40, -1),
    TypeDescriptorField("", DT_INT, offsetof(BannerSigilPlacement, hIcon), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BannerSigilPlacement), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BannerSigilPlacement("BannerSigilPlacement", 0, BannerSigilPlacement_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_BANNER_SIGIL_PLACEMENT = &TypeDescriptor_BannerSigilPlacement;

static const TypeDescriptorField EpicBannerDescription_Fields[6] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(EpicBannerDescription, snoBannerShape), "", TEXTURE_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(EpicBannerDescription, snoBannerBase), "", ACTOR_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(EpicBannerDescription, snoBannerPole), "", ACTOR_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(EpicBannerDescription, hIcon), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(EpicBannerDescription, szTagName), "", 1, 0x80, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(EpicBannerDescription), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_EpicBannerDescription("EpicBannerDescription", 0, EpicBannerDescription_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_EPIC_BANNER_DESCRIPTION = &TypeDescriptor_EpicBannerDescription;

static const TypeDescriptorField BannerParams_Fields[29] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serTexBackgrounds), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BANNER_TEXTURE_PAIR, offsetof(BannerParams, arTexBackgrounds), 0x21, 8),
    TypeDescriptorField("", DT_INT, offsetof(BannerParams, nTexBackgroundDefaults), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serTexPatterns), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BANNER_TEXTURE_PAIR, offsetof(BannerParams, arTexPatterns), 0x21, 8),
    TypeDescriptorField("", DT_INT, offsetof(BannerParams, nTexPatternDefaults), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serTexMainSigils), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BANNER_TEXTURE_PAIR, offsetof(BannerParams, arTexMainSigils), 0x21, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serTexVariantSigils), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BANNER_TEXTURE_PAIR, offsetof(BannerParams, arTexVariantSigils), 0x21, 8),
    TypeDescriptorField("", DT_INT, offsetof(BannerParams, nTexSigilDefaults), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serTexSigilAccents), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BANNER_TEXTURE_PAIR, offsetof(BannerParams, arTexSigilAccents), 0x21, 8),
    TypeDescriptorField("", DT_INT, offsetof(BannerParams, nTexSigilAccentDefaults), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serColorSets), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BANNER_COLOR_SET, offsetof(BannerParams, arColorSets), 0x21, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serSigilPlacements), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BANNER_SIGIL_PLACEMENT, offsetof(BannerParams, arSigilPlacements), 0x21, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serActorBases), NULL, 0),
    FilteredVariableArrayTypeDescriptorField("", DT_SNO, offsetof(BannerParams, arActorBases), 0x21, 8, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serActorCaps), NULL, 0),
    FilteredVariableArrayTypeDescriptorField("", DT_SNO, offsetof(BannerParams, arActorCaps), 0x21, 8, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serActorPoles), NULL, 0),
    FilteredVariableArrayTypeDescriptorField("", DT_SNO, offsetof(BannerParams, arActorPoles), 0x21, 8, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serActorRibbons), NULL, 0),
    FilteredVariableArrayTypeDescriptorField("", DT_SNO, offsetof(BannerParams, arActorRibbons), 0x21, 8, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BannerParams, serEpicBannerDescriptions), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_EPIC_BANNER_DESCRIPTION, offsetof(BannerParams, arEpicBannerDescriptions), 0x21, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BannerParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BannerParams("BannerParams", BANNER_PARAMS_VERSION, BannerParams_Fields, 29, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_BANNER_PARAMS = &TypeDescriptor_BannerParams;

static const TypeDescriptorField AssetListEntry_Fields[3] = {
    TypeDescriptorField("", DT_INT, offsetof(AssetListEntry, nID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AssetListEntry, nIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AssetListEntry), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AssetListEntry("AssetListEntry", 0, AssetListEntry_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ASSET_LIST_ENTRY = &TypeDescriptor_AssetListEntry;

static const TypeDescriptorField AssetList_Fields[3] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AssetList, serEntries), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ASSET_LIST_ENTRY, offsetof(AssetList, arEntries), 0x21, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AssetList), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AssetList("AssetList", 0, AssetList_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ASSET_LIST = &TypeDescriptor_AssetList;

static const TypeDescriptorField GlobalServerData_Fields[115] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(GlobalServerData, serActorGroups), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ACTOR_GROUP_STRUCT, offsetof(GlobalServerData, arActorGroups), 0x21, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(GlobalServerData, serScriptGlobalVars), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_GLOBAL_SCRIPT_VARIABLE_STRUCT, offsetof(GlobalServerData, arScriptGlobalVars), 0x21, 8),
    FixedArrayTypeDescriptorField("", DT_DIFFICULTY_TUNING_PARAMS, offsetof(GlobalServerData, tDifficultyParams), NULL, 1, 6, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flMonsterGetHitHealthPercent), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flChillDefaultMagnitude), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flChillDefaultDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flChillDefaultMagnitudePlayer), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flChillDefaultDurationPlayer), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flBleedDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flMonkSpiritGainedFromBasicAttack), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flHitChance), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nArenaPremadeHeroLevel), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flDurLossFrequency), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flPlayerResurrectionTimer), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flHirelingResurrectionTimer), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flPlayerGhostDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flPVPPregameDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flPVPGameDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flPVPPlayerGhostDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flPVPPlayerResurrectionDelay), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flPVPPlayerCCReduction), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flPlayerBaseResurrectionDelay), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nAfkTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nCalloutDelayAfterConv), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nActTransitionTime), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flComboServerTimeout), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flMultiplayerMonsterXPMultiplier), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flMultiplayerQuestXPMultiplier), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flFollowerHealthGlobeBonusPercent), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flHealthGlobeDropDistance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flPetAoEDamageScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flHirelingCoreAttributeBonusMultiplier), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flHealthShrineConvertChance), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nMinMonsterLevelForShrineGlobe), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flVoteKickAfterBossDelay), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flVoteKickAfterChampRareUniqueDelay), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flVoteKickTimeInGameDelay), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flTimeInGameForPositiveKarma), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, arSkillSwapCoolupTime), NULL, 1, 2, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flBaseGoldPickupRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flVacuumPickupRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flAttackSpeedItemPercentCap), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flCooldownMinimumTime), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flThornsPrimaryStatMultiplier), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nTargetedLegendaryDropChanceBase), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nTargetedLegendaryDropChanceIncrement), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nTargetedLegendaryDropChanceIncrementDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nTargetedRareDropChanceBase), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nTargetedRareDropChanceIncrement), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nTargetedRareDropChanceIncrementDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nTargetedMagicDropChanceBase), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nTargetedMagicDropChanceIncrement), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nTargetedMagicDropChanceIncrementDelta), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flUnluckyBonusMinHours), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flUnluckyBonusChancePerHour), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, arLegendaryChanceMultiplier), NULL, 1, 0x11, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flHirelingTargetedItemChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flHirelingTargetedInactiveChance), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nDungeonFinderLevelCount), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nDungeonFinderLevelsPerTileset), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nDungeonFinderEliteValue), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flBindOnEquipDropChance), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nMinutesToRerollVendorInventory), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flShatterMultiplierNonElites), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flCrushingBlowDamageExponent), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flBaseGlobeDropChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flSpenderGlobeDropChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flSpecialMonsterGlobeDropChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flOverkillGlobeDropChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flNephalemGloryActiveGlobeDropChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flOverkillNephalemGloryDropChance), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nCorpsesMax), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flCorpseMaxSpawnChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flCorpseChanceReductionOnCorpseSpawn), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flCorpseChanceIncreasePerSecond), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flCorpseChanceIncreaseForElites), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nCorpseCapPerArea), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flCorpseRadiusForCap), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flMovementScalarCap), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nLootRunChampionProgressGlobeCount), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nLootRunRareProgressGlobeCount), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nLootRunGoblinProgressGlobeCount), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flLootRunProgressGlobeXPScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flLootRunProgressGlobeDropRadiusMin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flLootRunProgressGlobeDropRadiusDelta), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flGlobalMonsterXPDensityMin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flGlobalMonsterXPDensityMax), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flGlobalEliteDensity), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, arAncientLegendaryChance), NULL, 1, 0xd, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flPrimalAncientLegendaryChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flCrafterAncientLegendaryChance), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nMinimumAchievedRiftLevelToRollPrimalAncients), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nAncientSalvageMultiplier), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nPrimalAncientSalvageMultiplier), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nPlatinumPerDailyGoblin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nPlatinumPerDailyBoss), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nPlatinumPerDailyBounty), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountiesTotalMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountiesTotalDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountiesKillBossMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountiesKillBossDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountiesClearDungeonMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountiesClearDungeonDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountiesCompleteEventMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountiesCompleteEventDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountiesKillUniqueMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountiesKillUniqueDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountyCampMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nOpenWorldBountyCampDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nPlatinumAwardMaxTimeToScale), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalServerData, nPlatinumAwardMinAmountForRoll), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalServerData, flPlatinumAwardPercentToStartLimiting), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GlobalServerData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GlobalServerData("GlobalServerData", 0, GlobalServerData_Fields, 115, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GLOBAL_SERVER_DATA = &TypeDescriptor_GlobalServerData;

static const TypeDescriptorField GlobalDefinition_Fields[148] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(GlobalDefinition, serServerData), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_GLOBAL_SERVER_DATA, offsetof(GlobalDefinition, ptServerData), 0x421, 8),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, uStartingLocationGUID), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(GlobalDefinition, serStartLocationNames), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_STARTING_LOCATION_NAME, offsetof(GlobalDefinition, arStartLocationNames), 0x21, 8),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flSellPricePercent), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flComboClientTimeout), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flPlayerPowerCacheTimeoutNormal), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flPlayerPowerCacheTimeoutInterrupt), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nUIBusyTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nTrialVersionLevelCap), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flRepairCostScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flRepairCostScalarHighLevel), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nDurabilityScaleFactor), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flItemsDamagedThresh), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flEnchantCraftedItemCostScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flHirelingDiminishedBonusRatio), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMinimapRevealRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flAudioMin3DPanLevel), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flAudioMin3DPanLevelRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flAudioMax3DPanLevelRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMonsterCritDamageScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flPlayerCritDamageScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flFireDamageProcChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flLightningDamageProcChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flColdDamageProcChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flPoisonDamageProcChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flArcaneDamageProcChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flHolyDamageProcChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flSplashDamageEffectProcChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flCrushingBlowDamagePercentNormal), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flCrushingBlowDamagePercentElite), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flCrushingBlowDamagePercentBosses), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flHolyDamageProcHitpointPercentToHeal), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flArcaneDamageProcDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flFireDamageProcHitpointPercentToDamage), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flFireDamageProcDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flPoisonDamageProcDamagePercentToDamage), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flPoisonDamageProcDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flLightningDamageProcChainRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flLightningDamageProcChainTargets), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flLightningDamageProcChainDamagePercent), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flSplashDamageEffectRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flHealthPotionHealPercent), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, bOverkillEnabled), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nStashSlotsCost), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nStashTabCost), NULL, 1, 5, 0),
    TypeDescriptorField("", DT_BANNER_PARAMS, offsetof(GlobalDefinition, tBannerParams), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, hSavepointWaypointButtonNormal), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, hSavepointWaypointButtonMouseOver), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, hSavepointWaypointButtonPushed), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, hSavepointFolderLabel), (const void *)-1, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flPlayerErrorCooldownTime), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flVoteKickAttemptDelay), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flVoteKickSuccessfulDelay), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nPVPTokenRedemptionCost), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flPlayerDelayBeforeCorpseRevival), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nCorpseResurrectionMaxCharges), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nCorpseResurrectionChargeRegenTime), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flRestXPBonusPercent), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flRestXPGrantLevelPercent), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flRestXPGrantLevelCapModifier), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flLootRunXPPercentToComplete), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flTieredLootRunXPPercentToComplete), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flTieredLootRunDelayPerDeath), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flTieredLootRunDelayDeathMax), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_ASSET_LIST, offsetof(GlobalDefinition, tAssetLists), NULL, 1, 9, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerDeadZone), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerMoveFacingZone), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerMinMoveSpeed), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerMaxMoveThreshold), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerEvadeZone), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerReboundDetectionZone), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nControllerReboundDuration), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerReboundDeadZone), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingSearchRange), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetWreckablesRange), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetWreckablesRangeHeight), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingCloseRange), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingCloseAngle), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingSearchGrowthRate), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nMaxTimesToGrow), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flTargetRangeGrowthPerSecond), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flTargetRangeMaxGrowth), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, holdTime), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingSearchAngleGrowthRate), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingSearchRangeSecondary), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingSearchAngleSecondary), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingSearchNearWidthSecondary), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingSearchNearWidth), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingSearchAngle), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingStickyNearWidth), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingStickyAngle), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flShuffleMovementSpeedBonus), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flShuffleMinMoveDistance), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerAutoTargetingSearchAimModeGrowth), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerLeftTriggerPressedForAimMode), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerNormalTurnSpeed), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerFastTurnSpeed), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerFastTurnThreshold), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flControllerPersistenceStickAngle), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nMaximumAttackersPerPlayer), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nMaximumLargeAttackersPerPlayer), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flLocalCoopDropsPerAdditionalPlayer), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flLocalCoopMagicFindBonusPerPlayer), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flPlayerBaseBossFightResurrectionDelay), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flEvadeBackAngle), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flGreyToGoldConversionChance), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flGoldReductionScalar), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flTargetedClassItemChance), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nParagonCategoryResetCost), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nAvengerUpdateCheckInterval), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nAvengerCreationCooldown), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nAvengerSpawnCooldown), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nAvengerSpawnWaitTimeMinutes), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nAvengerMinimumPlayerLevel), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flAvengerSpawnChance), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flAvengerCreationChance), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flFriendGiftDropChance), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreStartingTimer), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreStreakTimerCoop), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreStreakTimerRanged), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreStreakTimerMelee), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nMassacreMinCountSolo), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nMassacreMinCountCoop), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreXPBonusLevel0), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreXPBonusLevel1), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreXPBonusLevel2), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreXPBonusLevel3), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreXPBonusLevel4), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreXPBonusLevel5), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreXPBonusLevel6), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreXPBonusLevel7), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreXPBonusLevel8), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMassacreXPBonusLevel9), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nApprenticeshipLevelGap), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flMinimumStatScale), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(GlobalDefinition, flApprenticeshipXPBonus), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nApprenticeshipLevelRequirement), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nAnniversaryPrecursorStartMonth), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nAnniversaryPrecursorStartDay), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nAnniversaryEventStartMonth), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nAnniversaryEventStartDay), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nAnniversaryEventLastMonth), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nAnniversaryEventLastDay), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nPad), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(GlobalDefinition, nPad2), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GlobalDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GlobalDefinition("Globals", GLOBAL_XML_VERSION, GlobalDefinition_Fields, 148, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GLOBAL = &TypeDescriptor_GlobalDefinition;

GlobalGroupDef * gptGroupDefGlobal;  // line 490

// line 496
GlobalGroupDef::GlobalGroupDef()
    : GroupDefinition("Globals", GLOBAL_SNO,
                      SNOVersions(GLOBAL_PC_SNO_VERSION, 254, GLOBAL_CONSOLE_SNO_VERSION, GLOBAL_CONSOLE_SNO_VERSION, 254),
                      SNODATATYPE_TITLE, sizeof(GlobalDefinition), DT_GLOBAL, 0x5005, 14) {
}

// line 518
uint32 GlobalGroupDef::GetMaxCount() {
    return 1;
}

// line 731
static void sPopulateBannerTextureIndexMap(BannerParamsMaps::BannerParamsIndexMap & tMap,
                                           XPtr64<BannerTexturePair> arBannerTexturePair,
                                           SerializeData serBannerTexturePair, uint32 & rewardId) {
    // The dwSize zero check the original makes here is SerializeData::ElementCount's
    // (XSerializeData.h:20).

    for (int32 i = 0; i < (int32)serBannerTexturePair.ElementCount(sizeof(BannerTexturePair)); i++) {
        arBannerTexturePair[i].idReward = ++rewardId;
        tMap.SetAt_Tracked(arBannerTexturePair[i].snoTexture, i);
    }
}

// line 751
// Statement order from the DWARF: nNumSigilAccents is declared on 762 (before nNumColorSets),
// nextRewardId on 770 (after the new), the eight ElementCount calls on 758-765.
//
//
void GlobalGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    GlobalDefinition * ptDefinition = (GlobalDefinition *)lpDefinition;

    int32 nNumBackgrounds = ptDefinition->tBannerParams.serTexBackgrounds.ElementCount(sizeof(BannerTexturePair));
    int32 nNumPatterns = ptDefinition->tBannerParams.serTexPatterns.ElementCount(sizeof(BannerTexturePair));
    int32 nNumMainSigils = ptDefinition->tBannerParams.serTexMainSigils.ElementCount(sizeof(BannerTexturePair));
    int32 nNumVariantSigils = ptDefinition->tBannerParams.serTexVariantSigils.ElementCount(sizeof(BannerTexturePair));
    int32 nNumSigilAccents = ptDefinition->tBannerParams.serTexSigilAccents.ElementCount(sizeof(BannerTexturePair));
    int32 nNumColorSets = ptDefinition->tBannerParams.serColorSets.ElementCount(sizeof(BannerColorSet));
    int32 nNumSigilPlacements = ptDefinition->tBannerParams.serSigilPlacements.ElementCount(sizeof(BannerSigilPlacement));
    int32 nNumEpicBannerDescriptions = ptDefinition->tBannerParams.serEpicBannerDescriptions.ElementCount(sizeof(EpicBannerDescription));

    ptDefinition->tBannerParams.ptParamsMaps = new BannerParamsMaps(nNumBackgrounds, nNumPatterns, nNumMainSigils, nNumVariantSigils,
                                                                    nNumSigilAccents, nNumColorSets, nNumSigilPlacements,
                                                                    nNumEpicBannerDescriptions);

    ptDefinition->tBannerParams.nLastAllocatedRewardId = 0;
    uint32 nextRewardId = 0;
    sPopulateBannerTextureIndexMap(ptDefinition->tBannerParams.ptParamsMaps->mapTexBackgrounds,
                                   ptDefinition->tBannerParams.arTexBackgrounds,
                                   ptDefinition->tBannerParams.serTexBackgrounds, nextRewardId);
    sPopulateBannerTextureIndexMap(ptDefinition->tBannerParams.ptParamsMaps->mapTexPatterns,
                                   ptDefinition->tBannerParams.arTexPatterns,
                                   ptDefinition->tBannerParams.serTexPatterns, nextRewardId);
    sPopulateBannerTextureIndexMap(ptDefinition->tBannerParams.ptParamsMaps->mapTexMainSigils,
                                   ptDefinition->tBannerParams.arTexMainSigils,
                                   ptDefinition->tBannerParams.serTexMainSigils, nextRewardId);

    // The variant sigils share the main sigils' reward ids.
    nextRewardId -= nNumMainSigils;
    sPopulateBannerTextureIndexMap(ptDefinition->tBannerParams.ptParamsMaps->mapTexVariantSigils,
                                   ptDefinition->tBannerParams.arTexVariantSigils,
                                   ptDefinition->tBannerParams.serTexVariantSigils, nextRewardId);
    sPopulateBannerTextureIndexMap(ptDefinition->tBannerParams.ptParamsMaps->mapTexSigilAccents,
                                   ptDefinition->tBannerParams.arTexSigilAccents,
                                   ptDefinition->tBannerParams.serTexSigilAccents, nextRewardId);

    for (int32 i = 0; i < (int32)(ptDefinition->tBannerParams.serColorSets.dwSize / sizeof(BannerColorSet)); i++) {
        BannerColorSet & tColorSet = ptDefinition->tBannerParams.arColorSets[i];
        tColorSet.idReward = ++nextRewardId;
        uint32 uHash = BannerColorSet::XHash(tColorSet);
        ptDefinition->tBannerParams.ptParamsMaps->mapColorSets.SetAt_Tracked(uHash, i);
    }

    for (int32 i = 0; i < (int32)(ptDefinition->tBannerParams.serSigilPlacements.dwSize / sizeof(BannerSigilPlacement)); i++) {
        const BannerSigilPlacement & tSigilPlacement = ptDefinition->tBannerParams.arSigilPlacements[i];
        uint32 uHash = XHashKey<LPCSTR>(tSigilPlacement.szName);
        ptDefinition->tBannerParams.ptParamsMaps->mapSigilPlacements.SetAt_Tracked(uHash, i);
    }

    for (int32 i = 0; i < (int32)(ptDefinition->tBannerParams.serEpicBannerDescriptions.dwSize / sizeof(EpicBannerDescription)); i++) {
        EpicBannerDescription & tEpicBannerDescription = ptDefinition->tBannerParams.arEpicBannerDescriptions[i];
        tEpicBannerDescription.idReward = ++nextRewardId;
        uint32 uHash = EpicBannerDescription::XHash(tEpicBannerDescription);
        ptDefinition->tBannerParams.ptParamsMaps->mapEpicBannerDescriptions.SetAt_Tracked(uHash, i);
    }

    ptDefinition->tBannerParams.nLastAllocatedRewardId = nextRewardId;
}

// line 827
void GlobalGroupDef::Delete(void * lpDefinition) const {
    GlobalDefinition * ptDefinition = (GlobalDefinition *)lpDefinition;
    delete (BannerParamsMaps *)ptDefinition->tBannerParams.ptParamsMaps;
    GroupDefinition::Delete(lpDefinition);
}

// line 849
void GlobalGroupDef::Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement,
                          BOOL fContiguousBuffer, uint32 dwContiguousSize,
                          XMemoryAllocatorInterface * ptMemoryInterface) const {
    GroupDefinition::Read(tHeader, lpDefinition, ptSNOElement, fContiguousBuffer, dwContiguousSize, ptMemoryInterface);  // line 857
}

// line 1240
uint32 ActorGroupNameToHash(LPCSTR szGroupName) {
    if (szGroupName == NULL || *szGroupName == '\0')
        return (uint32)-1;

    CHAR szLoweredName[MAX_ACTOR_GROUP_NAME_LEN];
    Blizzard::String::Copy(szLoweredName, szGroupName, MAX_ACTOR_GROUP_NAME_LEN);
    Blizzard::String::ToLower(szLoweredName);
    return XHashKey<LPCSTR>(szLoweredName);
}

// line 1258
LPCSTR ActorGroupHashToName(uint32 uHash) {
    if (uHash == (uint32)-1)
        return "- None -";

    LPCSTR szName = "Unknown Group";
    ScopedGlobalDefinitionLockR ptGlobals;
    int nGroupCount = ptGlobals->ptServerData->serActorGroups.dwSize / sizeof(ActorGroup);
    for (int i = 0; i < nGroupCount; i++) {
        ActorGroup & tActorGroup = ptGlobals->ptServerData->arActorGroups[i];
        if (tActorGroup.uHash == uHash) {
            szName = tActorGroup.szName;
            break;
        }
    }
    return szName;
}

// line 1288
int32 ActorGroupHashToIndex(uint32 uHash) {
    if (uHash == (uint32)-1)
        return -1;

    int32 nIndex = -1;
    ScopedGlobalDefinitionLockR ptGlobals;
    int nGroupCount = ptGlobals->ptServerData->serActorGroups.dwSize / sizeof(ActorGroup);
    for (int i = 0; i < nGroupCount; i++) {
        ActorGroup & tActorGroup = ptGlobals->ptServerData->arActorGroups[i];
        if (tActorGroup.uHash == uHash) {
            nIndex = i;
            break;
        }
    }
    return nIndex;
}

// line 1318
uint32 ActorGroupIndexToHash(int32 nIndex) {
    uint32 uHash = (uint32)-1;
    if (nIndex != -1) {
        int nGroupCount;
        ScopedGlobalDefinitionLockR ptGlobals;
        nGroupCount = ptGlobals->ptServerData->serActorGroups.dwSize / sizeof(ActorGroup);
        if (nIndex < 0 || nIndex >= nGroupCount)
            return (uint32)-1;

        ActorGroup & tActorGroup = ptGlobals->ptServerData->arActorGroups[nIndex];
        uHash = tActorGroup.uHash;
    }
    return uHash;
}

// line 1448
LPCSTR StartingLocationGUIDToName(const uint32 uGUID) {
    if (uGUID == (uint32)-1)
        return "- None -";

    if (uGUID >= DYNAMIC_ENTRANCE_GUID_START && uGUID < DYNAMIC_ENTRANCE_GUID_MAX)
        return "Dynamic Entrance GUID";

    LPCSTR szName = "Unknown";
    ScopedGlobalDefinitionLockR pdefGlobals;
    int32 nGroupCount = pdefGlobals->serStartLocationNames.dwSize / sizeof(StartLocationName);
    for (int32 i = 0; i < nGroupCount; i++) {
        StartLocationName & tName = pdefGlobals->arStartLocationNames[i];
        if (tName.uGUID == uGUID) {
            szName = tName.szName;
            break;
        }
    }
    return szName;
}

// line 1483
uint32 StartingLocationNameToGUID(LPCSTR szName) {
    if (szName == NULL || *szName == '\0')
        return (uint32)-1;

    ScopedGlobalDefinitionLockR pdefGlobals;
    int32 nGroupCount = pdefGlobals->serStartLocationNames.dwSize / sizeof(StartLocationName);
    for (int32 i = 0; i < nGroupCount; i++) {
        StartLocationName & tName = pdefGlobals->arStartLocationNames[i];
        if (strcmp(szName, tName.szName) == 0)
            return tName.uGUID;
    }
    return (uint32)-1;
}

// line 1511
BOOL StartingLocationGUIDIsValid(const uint32 uGUID) {
    if (uGUID == (uint32)-1)
        return FALSE;

    if (uGUID >= DYNAMIC_ENTRANCE_GUID_START && uGUID < DYNAMIC_ENTRANCE_GUID_MAX)
        return TRUE;

    ScopedGlobalDefinitionLockR pdefGlobals;
    int32 nGroupCount = pdefGlobals->serStartLocationNames.dwSize / sizeof(StartLocationName);
    for (int32 i = 0; i < nGroupCount; i++) {
        StartLocationName & tName = pdefGlobals->arStartLocationNames[i];
        if (tName.uGUID == uGUID)
            return TRUE;
    }
    return FALSE;
}

// line 1543
float AdjustHPForPlayerCount(const float flHP, const int32 nOldPlayerMult, const int32 nNewPlayerMult) {
    float flBonusForNewPlayerCount;
    float flBonusForOldPlayerCount;
    ScopedGlobalDefinitionLockR pdefGlobal;
    const DifficultyTuningParams & tTuningParams = pdefGlobal->ptServerData->tDifficultyParams[GameGetDifficultyParamsLevel(FALSE)];

    flBonusForOldPlayerCount = 1.0f + nOldPlayerMult * tTuningParams.flMonsterHPPercentPerPlayer;
    flBonusForNewPlayerCount = 1.0f + nNewPlayerMult * tTuningParams.flMonsterHPPercentPerPlayer;

    return flHP * flBonusForNewPlayerCount / flBonusForOldPlayerCount;
}

// line 1572
int32 AssetListIndexGet(AssetListType eAssetList, int32 nAssetID) {
    AppGlobals & tAppGlobals = sgtAppGlobals;
    GlobalDefinitionAppGlobals * ptGlobalDefAppGlobals = tAppGlobals.ptGlobalDefAppGlobals;
    uint32 uIndex;
    if (ptGlobalDefAppGlobals->pmapAssetListMaps[eAssetList].Lookup(nAssetID, uIndex))
        return uIndex;
    return -1;
}

// line 1590
int32 AssetListGetAssetAtIndex(AssetListType eAssetList, int32 nIndex) {
    ScopedGlobalDefinitionLockR pdefGlobal;
    int32 nCount = pdefGlobal->tAssetLists[eAssetList].serEntries.dwSize / sizeof(AssetListEntry);
    if (nIndex < 0 || nIndex >= nCount)
        return -1;
    return pdefGlobal->tAssetLists[eAssetList].arEntries[nIndex].nID;
}

static const SavedDyeDef cgarSavedDyeDefs[MAX_SAVED_DYE_VALUE] = {  // line 1719
    { SAVED_DYE_TYPE_NONE, DYE_TYPE_NONE },
    { SAVED_DYE_TYPE_01, DYE_TYPE_01 },
    { SAVED_DYE_TYPE_02, DYE_TYPE_02 },
    { SAVED_DYE_TYPE_03, DYE_TYPE_03 },
    { SAVED_DYE_TYPE_04, DYE_TYPE_04 },
    { SAVED_DYE_TYPE_05, DYE_TYPE_05 },
    { SAVED_DYE_TYPE_06, DYE_TYPE_06 },
    { SAVED_DYE_TYPE_07, DYE_TYPE_07 },
    { SAVED_DYE_TYPE_08, DYE_TYPE_08 },
    { SAVED_DYE_TYPE_09, DYE_TYPE_09 },
    { SAVED_DYE_TYPE_10, DYE_TYPE_10 },
    { SAVED_DYE_TYPE_11, DYE_TYPE_11 },
    { SAVED_DYE_TYPE_12, DYE_TYPE_12 },
    { SAVED_DYE_TYPE_13, DYE_TYPE_13 },
    { SAVED_DYE_TYPE_14, DYE_TYPE_14 },
    { SAVED_DYE_TYPE_15, DYE_TYPE_15 },
    { SAVED_DYE_TYPE_16, DYE_TYPE_16 },
    { SAVED_DYE_TYPE_17, DYE_TYPE_17 },
    { SAVED_DYE_TYPE_18, DYE_TYPE_18 },
    { SAVED_DYE_TYPE_19, DYE_TYPE_19 },
    { SAVED_DYE_TYPE_20, DYE_TYPE_20 },
    { SAVED_DYE_TYPE_21, DYE_TYPE_21 },
    { SAVED_DYE_TYPE_VANISHING, DYE_TYPE_VANISHING },
};

// line 1746
DyeType DyeTypeTranslateFromSavedDyeType(const SavedDyeType eSavedDye) {
    for (int32 i = 0; i < MAX_SAVED_DYE_VALUE; i++) {
        const SavedDyeDef & tEntry = cgarSavedDyeDefs[i];
        if (tEntry.eSavedDyeType == eSavedDye)
            return tEntry.eDyeType;
    }
    return DYE_TYPE_NONE;
}

// line 1764
SavedDyeType DyeTypeTranslateToSavedDyeType(const DyeType eDye) {
    for (int32 i = 0; i < MAX_SAVED_DYE_VALUE; i++) {
        const SavedDyeDef & tEntry = cgarSavedDyeDefs[i];
        if (tEntry.eDyeType == eDye)
            return tEntry.eSavedDyeType;
    }
    return SAVED_DYE_TYPE_NONE;
}
#include "../../Sigma/Math/MathGeneral.h"
// Inference: the 1645 test is unsigned in the original (`cmp #32; b.lo` then a signed /8); the spelling of the
// unsigned operand (32u here) is not recoverable. No Math::Max/Min inline: plain ifs on 1625/1630.
// 1673:15 is the `-=` of nNumBytes.
#line 1606
int32 AssetListArrayToBitfield(AssetListType eAssetList, const int32 * ptAssetIDArray, int32 nAssets, uint8 * ptByteArray, int32 nByteArrayLength, int32 * pnLeadingNullBytes)
{
#line 1614
    SigmaMemoryClear(ptByteArray, nByteArrayLength);
#line 1617
    int32 nHighestListIndex = -1;
    int32 nLowestListIndex = nByteArrayLength * 8;
#line 1620
    for (int32 i = 0; i < nAssets; ++i)
    {
        int32 nListIndex = AssetListIndexGet(eAssetList, ptAssetIDArray[i]);
        if (nListIndex != -1)
        {
            if (nListIndex > nHighestListIndex)
            {
                nHighestListIndex = nListIndex;
            }
#line 1630
            if (nListIndex < nLowestListIndex)
            {
                nLowestListIndex = nListIndex;
            }
        }
    }
#line 1637
    if (nHighestListIndex < 0)
        return 0;
#line 1644
    int32 nLeadingNullBytes = 0;
    if (pnLeadingNullBytes && nLowestListIndex >= 32u)
    {
        nLeadingNullBytes = nLowestListIndex / 8;
        *pnLeadingNullBytes = nLeadingNullBytes;
    }
#line 1651
    for (int32 i = 0; i < nAssets; ++i)
    {
        int32 nListIndex = AssetListIndexGet(eAssetList, ptAssetIDArray[i]);
        if (nListIndex != -1)
        {
            int32 nByteIndex = nListIndex / 8 - nLeadingNullBytes;
            int32 nBitIndex = nListIndex & 7;
#line 1668
            ptByteArray[nByteIndex] |= 1 << nBitIndex;
        }
    }
#line 1672
    int32 nNumBytes = nHighestListIndex / 8 + 1;
    nNumBytes -= nLeadingNullBytes;
#line 1675
    return nNumBytes;
}
// Line table: 1700:66 is the last `+` of nIndex, and nAssetCount++ has its own row (1702:17).
#line 1680
int32 AssetListBitfieldToArray(AssetListType eAssetList, const uint8 * ptByteArray, int32 nByteArrayLength, int32 * ptAssetIDArray, int32 nMaxAssets, int32 nLeadingNullBytes)
{
#line 1688
    int32 nAssetCount = 0;
#line 1689
    int32 nLeadingNullBits = nLeadingNullBytes * 8;
#line 1691
    for (int32 nByteIndex = 0; nByteIndex < nByteArrayLength; ++nByteIndex)
    {
#line 1693
        uint8 uByte = ptByteArray[nByteIndex];
#line 1694
        for (int32 nBitIndex = 0; nBitIndex < 8; ++nBitIndex)
        {
            if (uByte & (1 << nBitIndex))
            {
#line 1700
                int32 nIndex = nLeadingNullBits + nByteIndex * 8 + nBitIndex;
                ptAssetIDArray[nAssetCount] = AssetListGetAssetAtIndex(eAssetList, nIndex);
                nAssetCount++;
            }
        }
    }
    return nAssetCount;
}
#line 1781
void GlobalDefinitionInitializeForApp()
{
#line 1784
    AppGlobals & tAppGlobals = sgtAppGlobals;
#line 1785
    GlobalDefinitionAppGlobals *& ptGlobalDefAppGlobals = tAppGlobals.ptGlobalDefAppGlobals;
    ptGlobalDefAppGlobals = (GlobalDefinitionAppGlobals *)SigmaMemoryNew(sizeof(GlobalDefinitionAppGlobals), 4, NULL, TRUE);
    ptGlobalDefAppGlobals->pmapAssetListMaps = (AssetListIDToBitMap *)SigmaMemoryNew(sizeof(AssetListIDToBitMap) * NUM_ASSET_LISTS, 4, NULL, TRUE);
#line 1790
    ScopedGlobalDefinitionLockR pdefGlobal;
#line 1791
    for (int32 i = 0; i < NUM_ASSET_LISTS; ++i)
    {
        // Original immediate table sizes are 16 initially and 64 after resizing.
        ptGlobalDefAppGlobals->pmapAssetListMaps[i].Initialize(16, FALSE, NULL);
        ptGlobalDefAppGlobals->pmapAssetListMaps[i].SetHashTableSize_Tracked(64);
#line 1796
        int32 nCount = pdefGlobal->tAssetLists[i].serEntries.ElementCount(sizeof(AssetListEntry));
#line 1797
        for (int32 j = 0; j < nCount; ++j)
            ptGlobalDefAppGlobals->pmapAssetListMaps[i].SetAt_Tracked(pdefGlobal->tAssetLists[i].arEntries[j].nID, pdefGlobal->tAssetLists[i].arEntries[j].nIndex);
    }
}
#line 1807
void GlobalDefinitionDisposeForApp()
{
#line 1810
    AppGlobals & tAppGlobals = sgtAppGlobals;
#line 1811
    GlobalDefinitionAppGlobals *& ptGlobalDefAppGlobals = tAppGlobals.ptGlobalDefAppGlobals;
#line 1816
    for (int32 i = 0; i < NUM_ASSET_LISTS; ++i)
        ptGlobalDefAppGlobals->pmapAssetListMaps[i].Dispose();
    SigmaMemoryFree(ptGlobalDefAppGlobals->pmapAssetListMaps, NULL);
    ptGlobalDefAppGlobals->pmapAssetListMaps = NULL;
    SigmaMemoryFree(ptGlobalDefAppGlobals, NULL);
    ptGlobalDefAppGlobals = NULL;
}

// Defaults are original scalar stores expressed through DWARF fields. The
// six difficulty records share the original six unit scalars and three zero chances.
// NON_MATCHING: 1448 vs 1460. pdef stores follow the original line order (623-726); the store merger still pairs 0x3d0/0x3d4, 0x2b0/0x2b4 (original: 0x3cc/0x3d0, none).
#line 531
void GlobalGroupDef::GetDefaultData(
    void * ptDefaultDef) const
{
    SigmaMemoryClear(ptDefaultDef, sizeof(GlobalDefinition));
    GlobalDefinition * pdef = (GlobalDefinition *)ptDefaultDef;
    pdef->serServerData.dwSize = sizeof(GlobalServerData);
    pdef->ptServerData = (GlobalServerData *)SigmaMemoryNew(sizeof(GlobalServerData), 4, NULL, TRUE);
    pdef->ptServerData->flChillDefaultMagnitude = 0.6f;
    pdef->ptServerData->flChillDefaultDuration = 3.0f;
    pdef->ptServerData->flChillDefaultMagnitudePlayer = 0.6f;
    pdef->ptServerData->flChillDefaultDurationPlayer = 3.0f;
    pdef->ptServerData->flBleedDuration = 2.0f;
    pdef->ptServerData->nArenaPremadeHeroLevel = 20;
    pdef->ptServerData->flComboServerTimeout = 1.5f;
    pdef->ptServerData->nActTransitionTime = 120;
    pdef->ptServerData->nAfkTime = 30;
    pdef->ptServerData->nCalloutDelayAfterConv = 30;
    pdef->ptServerData->flDurLossFrequency = 60.0f;
    pdef->ptServerData->flPlayerResurrectionTimer = 10.0f;
    pdef->ptServerData->flHirelingResurrectionTimer = 20.0f;
    pdef->ptServerData->flPlayerGhostDuration = 3.0f;
    pdef->ptServerData->flPlayerBaseResurrectionDelay = 5.0f;
    pdef->ptServerData->flPVPGameDuration = 600.0f;
    pdef->ptServerData->flPVPPregameDuration = 15.0f;
    pdef->ptServerData->flPVPPlayerGhostDuration = 10.0f;
    pdef->ptServerData->flPVPPlayerResurrectionDelay = 5.0f;
    pdef->ptServerData->flMultiplayerMonsterXPMultiplier = 1.0f;
    pdef->ptServerData->flMultiplayerQuestXPMultiplier = 1.0f;
    pdef->ptServerData->flPetAoEDamageScalar = 0.1f;
    pdef->ptServerData->flHirelingCoreAttributeBonusMultiplier = 2.5f;
    pdef->ptServerData->flMonkSpiritGainedFromBasicAttack = 3.0f;
    pdef->ptServerData->flHitChance = 0.95f;
    pdef->ptServerData->flHealthShrineConvertChance = 0.0f;
    pdef->ptServerData->nMinMonsterLevelForShrineGlobe = 1;
    pdef->ptServerData->flVoteKickAfterBossDelay = 120.0f;
    pdef->ptServerData->flVoteKickAfterChampRareUniqueDelay = 120.0f;
    pdef->ptServerData->flVoteKickTimeInGameDelay = 5.0f;
    pdef->ptServerData->flTimeInGameForPositiveKarma = 120.0f;
    pdef->ptServerData->flBaseGoldPickupRadius = 1.5f;
    pdef->ptServerData->flVacuumPickupRadius = 40.0f;
    pdef->ptServerData->flAttackSpeedItemPercentCap = 0.4f;
    pdef->ptServerData->flCooldownMinimumTime = 0.5f;
    pdef->ptServerData->flThornsPrimaryStatMultiplier = 0.25f;
    pdef->ptServerData->flShatterMultiplierNonElites = 0.3f;
    pdef->ptServerData->flCrushingBlowDamageExponent = 1.0f;
    pdef->ptServerData->nMinutesToRerollVendorInventory = 15;
    pdef->ptServerData->flBaseGlobeDropChance = 0.01f;
    pdef->ptServerData->flSpenderGlobeDropChance = 0.03f;
    pdef->ptServerData->flSpecialMonsterGlobeDropChance = 0.1f;
    pdef->ptServerData->flOverkillGlobeDropChance = 0.5f;
    pdef->ptServerData->flNephalemGloryActiveGlobeDropChance = 0.05f;
    pdef->ptServerData->flOverkillNephalemGloryDropChance = 0.02f;
    pdef->ptServerData->flCorpseMaxSpawnChance = 0.6f;
    pdef->ptServerData->flCorpseChanceReductionOnCorpseSpawn = 0.25f;
    pdef->ptServerData->flCorpseChanceIncreasePerSecond = 0.02f;
    pdef->ptServerData->flCorpseChanceIncreaseForElites = 0.25f;
    pdef->ptServerData->nCorpsesMax = 15;
    pdef->ptServerData->nCorpseCapPerArea = 3;
    pdef->ptServerData->flCorpseRadiusForCap = 50.0f;
    pdef->ptServerData->nPlatinumPerDailyGoblin = 0;
    pdef->ptServerData->nPlatinumPerDailyBoss = 0;
    pdef->ptServerData->nPlatinumPerDailyBounty = 0;
    pdef->ptServerData->flMovementScalarCap = 1.25f;
    pdef->ptServerData->nAncientSalvageMultiplier = 3;
    pdef->ptServerData->nPrimalAncientSalvageMultiplier = 8;
#line 608
    for (int32 i = 0; i < 6; ++i)
    {
#line 610
        DifficultyTuningParams & tTuningParams = pdef->ptServerData->tDifficultyParams[i];
        tTuningParams.flInitialCooldownScale = 1.0f;
        tTuningParams.flAttackCooldownScale = 1.0f;
        tTuningParams.flClosingCooldownScale = 1.0f;
        tTuningParams.flLifeDrainScalar = 1.0f;
        tTuningParams.flEliteMonsterHPScalar = 1.0f;
        tTuningParams.flUniqueAndBossMonsterHPScalar = 1.0f;
        tTuningParams.flTrashItemConvertChanceWhite = 0.0f;
        tTuningParams.flTrashItemConvertChanceMagic = 0.0f;
        tTuningParams.flTrashItemConvertChanceRare = 0.0f;
    }
    pdef->flSellPricePercent = 0.5f;
    pdef->flComboClientTimeout = 1.0f;
    pdef->flPlayerPowerCacheTimeoutNormal = 0.25f;
    pdef->flPlayerPowerCacheTimeoutInterrupt = 0.5f;
    pdef->nUIBusyTime = 6;
    pdef->flRepairCostScalar = 1.0f;
    pdef->flRepairCostScalarHighLevel = 1.0f;
    pdef->nDurabilityScaleFactor = 10;
    pdef->flItemsDamagedThresh = 0.2f;
    pdef->flEnchantCraftedItemCostScalar = 1.0f;
    pdef->flHirelingDiminishedBonusRatio = 0.2f;
    pdef->flMinimapRevealRadius = 65.0f;
    pdef->flAudioMin3DPanLevel = 1.0f;
    pdef->flAudioMin3DPanLevelRadius = 0.0f;
    pdef->flAudioMax3DPanLevelRadius = 0.0f;
    pdef->flPlayerDelayBeforeCorpseRevival = 5.0f;
    pdef->nCorpseResurrectionMaxCharges = 3;
    pdef->nCorpseResurrectionChargeRegenTime = 120;
    pdef->flRestXPBonusPercent = 0.25f;
    pdef->flRestXPGrantLevelPercent = 0.02f;
    pdef->flRestXPGrantLevelCapModifier = 0.4f;
    pdef->bOverkillEnabled = 0;
    pdef->flLootRunXPPercentToComplete = 250.0f;
    pdef->flTieredLootRunXPPercentToComplete = 250.0f;
    pdef->flTieredLootRunDelayPerDeath = 5.0f;
    pdef->flTieredLootRunDelayDeathMax = 30.0f;
    pdef->flControllerAutoTargetingSearchRange = 70.0f;
    pdef->flControllerAutoTargetWreckablesRange = 70.0f;
    pdef->flControllerAutoTargetingCloseRange = 10.0f;
    pdef->flControllerAutoTargetingCloseAngle = 180.0f;
    pdef->flControllerAutoTargetingSearchAngleGrowthRate = 5.0f;
    pdef->flControllerDeadZone = 0.15f;
    pdef->flControllerMoveFacingZone = 0.7f;
    pdef->flControllerMinMoveSpeed = 0.2f;
    pdef->flControllerMaxMoveThreshold = 0.9f;
    pdef->flControllerEvadeZone = 0.7f;
    pdef->flControllerReboundDetectionZone = 0.8f;
    pdef->nControllerReboundDuration = 50;
    pdef->flControllerReboundDeadZone = 0.4f;
    pdef->flControllerAutoTargetingSearchNearWidth = 8.0f;
    pdef->flControllerAutoTargetingSearchAngle = 30.0f;
    pdef->flControllerAutoTargetingStickyNearWidth = 14.0f;
    pdef->flControllerAutoTargetingStickyAngle = 50.0f;
    pdef->flControllerAutoTargetingSearchGrowthRate = 1.0f;
    pdef->nMaxTimesToGrow = 5;
    pdef->flTargetRangeGrowthPerSecond = 1.0f;
    pdef->flTargetRangeMaxGrowth = 5.0f;
    pdef->holdTime = 200;
    pdef->flControllerAutoTargetingSearchRangeSecondary = 70.0f;
    pdef->flControllerAutoTargetingSearchAngleSecondary = 120.0f;
    pdef->flControllerAutoTargetingSearchNearWidthSecondary = 9.0f;
    pdef->flShuffleMovementSpeedBonus = 0.2f;
    pdef->flShuffleMinMoveDistance = 5.0f;
    pdef->flControllerAutoTargetingSearchAimModeGrowth = 10.0f;
    pdef->flControllerLeftTriggerPressedForAimMode = 0.0f;
    pdef->flControllerNormalTurnSpeed = 225.0f;
    pdef->flControllerFastTurnSpeed = 900.0f;
    pdef->flControllerFastTurnThreshold = 90.0f;
    pdef->flControllerPersistenceStickAngle = 15.0f;
    pdef->nMaximumAttackersPerPlayer = 4;
    pdef->nMaximumLargeAttackersPerPlayer = 2;
    pdef->flHealthPotionHealPercent = 0.5f;
    pdef->flLocalCoopDropsPerAdditionalPlayer = 0.25f;
    pdef->flLocalCoopMagicFindBonusPerPlayer = 0.5f;
    pdef->flPlayerBaseBossFightResurrectionDelay = 10.0f;
    pdef->flEvadeBackAngle = 45.0f;
    pdef->flGreyToGoldConversionChance = 0.0f;
    pdef->flGoldReductionScalar = 1.0f;
    pdef->flTargetedClassItemChance = 1.0f;
    pdef->nParagonCategoryResetCost = 0;
    pdef->nAvengerUpdateCheckInterval = 60;
    pdef->nAvengerCreationCooldown = 120;
    pdef->nAvengerSpawnCooldown = 300;
    pdef->nAvengerSpawnWaitTimeMinutes = 10;
    pdef->nAvengerMinimumPlayerLevel = 10;
    pdef->flAvengerSpawnChance = 0.1f;
    pdef->flAvengerCreationChance = 0.25f;
    pdef->flFriendGiftDropChance = 0.0f;
    pdef->flMassacreStartingTimer = 5.0f;
    pdef->nApprenticeshipLevelGap = 3;
    pdef->flMinimumStatScale = 0.85f;
    pdef->flApprenticeshipXPBonus = 0.35f;
    pdef->nApprenticeshipLevelRequirement = 5;
    pdef->flMassacreStreakTimerCoop = 1.5f;
    pdef->flMassacreStreakTimerRanged = 2.0f;
    pdef->flMassacreStreakTimerMelee = 2.5f;
    pdef->nMassacreMinCountSolo = 10;
    pdef->nMassacreMinCountCoop = 15;
    pdef->flPlayerErrorCooldownTime = 3.0f;
    pdef->flVoteKickAttemptDelay = 5.0f;
    pdef->flVoteKickSuccessfulDelay = 30.0f;
    pdef->nAnniversaryPrecursorStartMonth = 1;
    pdef->nAnniversaryPrecursorStartDay = 1;
    pdef->nAnniversaryEventStartMonth = 1;
    pdef->nAnniversaryEventStartDay = 4;
    pdef->nAnniversaryEventLastMonth = 1;
    pdef->nAnniversaryEventLastDay = 31;
}
