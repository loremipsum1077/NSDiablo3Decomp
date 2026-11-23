// Original file: X/Common/OnlineService/Achievements.cpp
#include "Achievements.h"
// For SNONAME_NONE, cgplaceNull, the GfxVB.h normals and blz::ignore, whose initializers the
// original runs in this file's static initializer. The DWARF does not record the include chain.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "X/Server/Achievements/AchievementsEngine/achievements_update_structs.h"
#include "X/Server/Achievements/AchievementsEngine/achievements_progress_data.h"
#include "../Achievements/AchievementsEngine/achievements_static_data.h"
#include "../Achievements/AchievementsEngine/achievements_internal_static_data_structs.h"
#include "../Achievements/AchievementDefines.h"
#include "../Definitions/GameBalanceDefinition.h"
#include "../Items.h"
#include <stdlib.h>
#include "../Definitions/GlobalDefinition.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../Protobuf/AchievementsStaticData.pb.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/DataStructures/XVarCollection.h"
#include "../../Sigma/DataStructures/XVariant.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/UnicodeText.h"

// X/Common/FileIO/FileIO.h:36. FileIO.h is not in src yet; only the members this file calls are
#include "../FileIO/FileIO.h"
namespace FileIO {











}  // namespace FileIO

ImageHandle StringToImageHandle(LPCSTR pszName);  // X/Client/ImageCollection.cpp

// Defined in X/Common/SNO/SNOFiles.cpp:9920; not declared in src/X/Common/SNO/SNOFiles.h yet.
CRefString SNOFilesGetMiscPath();
static XVarScope s_scopeAchievements("Achievements", NULL);  // line 34
XVarUint32 s_varAchievementsNumShards(s_scopeAchievements, "NumShards", OnlineService::Achievements::ACHIEVEMENTS_NUMBER_SHARDS_DEFAULT);  // line 35
int OnlineService::Achievements::s_nextRewardId;
namespace OnlineService {
inline BOOL operator==(const RewardDefinition & tDefA, const RewardDefinition & tDefB)  // line 41
{
    return tDefA.m_flags == tDefB.m_flags &&
           tDefA.m_hIcon == tDefB.m_hIcon &&
           tDefA.m_id == tDefB.m_id &&
           tDefA.m_type == tDefB.m_type &&
           Blizzard::String::Equal(tDefA.m_pszNameKey, tDefB.m_pszNameKey);
}
inline BOOL operator!=(const RewardDefinition & tDefA, const RewardDefinition & tDefB)  // line 50
{
    return !(tDefA == tDefB);
}
inline const char * FindAchievementAttribute(const google::protobuf::RepeatedPtrField<D3::AchievementsStaticData::Attribute> & list, const char * pszAttributeName)  // line 57
{
    for (int i = 0; i < list.size(); ++i)
    {
        if (list.Get(i).key() == pszAttributeName)
            return list.Get(i).value().c_str();
    }
    return NULL;
}
// Original line 76.
Achievements::Achievements() : m_CacheInvalidatedEvent(NULL), m_AchievementUpdateEvent(NULL), m_CriteriaUpdateEvent(NULL), m_AchievementsStatusEvent(NULL), m_AchievementStaticDataUpdatedEvent(NULL), m_bStaticDataLoaded(false), m_pStaticDataFile(new AchievementsEngine::AchievementsStaticData("")), m_pOldStaticDataFile(NULL), m_bEnabled(true) {}
Achievements::~Achievements() { delete m_pStaticDataFile; delete m_pOldStaticDataFile; }
bool Achievements::IsInitialized() { return true; }
bool Achievements::IsStaticDataLoaded() { return m_bStaticDataLoaded; }
void Achievements::RegisterSyncedVars() {
    GetSyncronizedXVarCollection().Add(s_varAchievementsNumShards, XVariant::MakeUint32(ACHIEVEMENTS_NUMBER_SHARDS_DEFAULT));
}
bool Achievements::ShouldCountAchievementPoints(const AchievementDefinition * ptAchievementDef) const {
    return ptAchievementDef && !(ptAchievementDef->m_flags & ((1u << ACHIEVEMENT_FLAG_STATISTIC) | (1u << ACHIEVEMENT_FLAG_HIDDEN)));
}
bool Achievements::GetChildCategories(CategoryId id, CategoryIdList & idList) const {
    AchievementsEngine::AchievementsStaticData * ptStaticData = GetStaticDataFile();
    const AchievementsEngine::InternalCategoryDefinition * ptCategory = ptStaticData->GetCategoryDefinition(id);
    if (!ptCategory) return false;
    idList.reserve(ptCategory->m_childCategories.size());
    for (uint32 i = 0; i < ptCategory->m_childCategories.size(); ++i) idList.push_back(ptCategory->m_childCategories[i]->m_id);
    return true;
}
bool Achievements::GetCategoryAchievements(CategoryId id, AchievementIdList & idList) const {
    AchievementsEngine::AchievementsStaticData * ptStaticData = GetStaticDataFile();
    const AchievementsEngine::InternalCategoryDefinition * ptCategory = ptStaticData->GetCategoryDefinition(id);
    if (!ptCategory) return false;
    idList.reserve(ptCategory->m_childAchievements.size());
    for (uint32 i = 0; i < ptCategory->m_childAchievements.size(); ++i) idList.push_back(ptCategory->m_childAchievements[i]->m_id);
    return true;
}
bool Achievements::GetAchievementCriteria(AchievementId id, CriteriaIdList & idList) const {
    AchievementsEngine::AchievementsStaticData * ptStaticData = GetStaticDataFile();
    const AchievementsEngine::InternalAchievementDefinition * ptAchievement = ptStaticData->GetAchievementDefinition(id);
    if (!ptAchievement) return false;
    idList.reserve(ptAchievement->m_criteriaList.size());
    for (uint32 i = 0; i < ptAchievement->m_criteriaList.size(); ++i) idList.push_back(ptAchievement->m_criteriaList[i]->m_id);
    return true;
}
bool Achievements::GetAchievementRewards(AchievementId id, RewardIdList & idList) const {
    const AchievementDefinition * achievement = GetAchievementDefinition(id);
    if (!achievement) return false;
    idList = achievement->m_rewards;
    return true;
}
const CategoryDefinition * Achievements::GetCategoryDefinition(CategoryId id) const { return GetStaticDataFile()->GetCategoryDefinition(id); }
const AchievementDefinition * Achievements::GetAchievementDefinition(AchievementId id) const { return GetStaticDataFile()->GetAchievementDefinition(id); }
const CriteriaDefinition * Achievements::GetCriteriaDefinition(CriteriaId id) const { return GetStaticDataFile()->GetCriteriaDefinition(id); }
bool Achievements::GetRewardDefinition(RewardId id, RewardDefinition & tReward) const {
    AchievementsEngine::AchievementsStaticData * ptStaticData = GetStaticDataFile();
    const RewardDefinition * ptReward = ptStaticData->GetRewardDefinition(id);
    if (!ptReward) return false;
    tReward = *ptReward;
    return true;
}
bool Achievements::GetRewardParentAchievements(RewardId id, AchievementIdList & idList) const {
    AchievementsEngine::AchievementsStaticData * ptStaticData = GetStaticDataFile();
    const AchievementsEngine::InternalRewardDefinition * ptReward = ptStaticData->GetRewardDefinition(id);
    if (!ptReward) return false;
    idList.reserve(ptReward->m_parentAchievements.size());
    for (uint32 i = 0; i < ptReward->m_parentAchievements.size(); ++i) idList.push_back(ptReward->m_parentAchievements[i]->m_id);
    return true;
}
void Achievements::GetAllRewardsByType(RewardType eType, RewardIdList & idList, uint32 nSeasonId) const { GetAllRewardsOfTypes(1u << eType, idList, nSeasonId); }
inline int32 Achievements::GetSeasonCycleLength() const { return GetStaticDataFile()->GetSeasonCycleLength(); }
int32 Achievements::GetSeasonDataPartition(uint32 seasonNum) const {
    int32 seasonCycleLength = GetSeasonCycleLength();
    if (!seasonCycleLength) return -1;
    if (!seasonNum) return 0;
    return (seasonNum - 1) % seasonCycleLength + 1;
}
void Achievements::GetAllRewardsOfTypes(uint32 dwTypes, RewardIdList & idList, uint32 nSeasonId) const
{
    AchievementsEngine::AchievementsStaticData * ptStaticData = GetStaticDataFile();
    for (AchievementsEngine::RewardMap::const_iterator it = ptStaticData->GetRewardMap().begin(); it != ptStaticData->GetRewardMap().end(); it++)
    {
        const AchievementsEngine::InternalRewardDefinition * ptReward = it->second;
        if (!((1u << ptReward->m_type) & dwTypes))
            continue;
        for (uint32 i = 0; i < ptReward->m_parentAchievements.size(); ++i)
        {
            const AchievementDefinition * ptAchievementDef = ptReward->m_parentAchievements[i];
            if ((nSeasonId != 0) != (ptAchievementDef->m_seasonMask != 0))
                continue;
            if (!IsAchievementValidForSeason(ptAchievementDef, nSeasonId))
                continue;
            if (nSeasonId <= 3 && IsSeasonJourneyAchievement(ptAchievementDef->m_id))
                continue;
            idList.push_back(it->first);
            break;
        }
    }
}
BOOL Achievements::IsSeasonJourneyAchievement(AchievementId achievementId) const {
    const AchievementDefinition * ptAchievementDefinition = GetAchievementDefinition(achievementId);
    if (!ptAchievementDefinition || !ptAchievementDefinition->m_seasonMask) return FALSE;
    const CategoryDefinition * ptCategoryDef;
    CategoryId idParentCategory = ptAchievementDefinition->m_parentCategoryId;
    do {
        ptCategoryDef = GetCategoryDefinition(idParentCategory);
        if (!ptCategoryDef) return FALSE;
        idParentCategory = ptCategoryDef->m_parentCategoryId;
    } while (idParentCategory);
    return (ptCategoryDef->m_flags & (1u << CATEGORY_FLAG_SEASON_JOURNEY)) != 0;
}
BOOL Achievements::IsSeasonJourneyAchievementObjective(AchievementId achievementId) const
{
    int32 seasonPartition = GetSeasonDataPartition(GetSeasonNum());
    int32 seasonIndex = seasonPartition - 1;
    const AchievementsEngine::SeasonJourneyObjective & arObjectives = GetStaticDataFile()->GetSeasonJourneyObjectiveArray();
    if (arObjectives.size() == 0)
        return FALSE;
    seasonIndex = Math::Pin(seasonIndex, 0, (int32)arObjectives.size() - 1);
    return arObjectives[seasonIndex].count(achievementId) != 0;
}
BOOL Achievements::GetAchievementObjectiveParent(AchievementId achievementId, AchievementId & achievementIdParent)
{
    int32 seasonPartition = GetSeasonDataPartition(GetSeasonNum());
    int32 seasonIndex = seasonPartition - 1;
    const AchievementsEngine::SeasonJourneyObjective & arObjectives = GetStaticDataFile()->GetSeasonJourneyObjectiveArray();
    if (arObjectives.size() == 0)
        return FALSE;
    seasonIndex = Math::Pin(seasonIndex, 0, (int32)arObjectives.size() - 1);
    blz::unordered_map<AchievementId, AchievementId>::const_iterator it = arObjectives[seasonIndex].find(achievementId);
    if (it == arObjectives[seasonIndex].end())
        return FALSE;
    achievementIdParent = it->second;
    return TRUE;
}
BOOL Achievements::IsAchievementValidForSeason(const AchievementDefinition * ptAchievementDef, uint32 nSeasonId) const {
    if (!nSeasonId) return ptAchievementDef->m_seasonMask == 0;
    AchievementsEngine::AchievementsStaticData * ptStaticData = GetStaticDataFile();
    int32 nCycle = ptStaticData->GetSeasonCycleLength();
    if (nCycle < 1) return TRUE;
    if (ptAchievementDef->m_nDeprecatedAsOf && ptAchievementDef->m_nDeprecatedAsOf <= nSeasonId) return FALSE;
    if (ptAchievementDef->m_nAvailableBeginningAt && ptAchievementDef->m_nAvailableBeginningAt > nSeasonId) return FALSE;
    // Intermediate name invented; DWARF records only nCycle here.
    uint32 nSeasonBit = (nSeasonId - 1) % nCycle;
    return (ptAchievementDef->m_seasonMask >> nSeasonBit) & 1;
}
static bool IsCriteriaEventHardcore(const CriteriaEvent & tEvent)
{
    for (uint32 i = 0; i < tEvent.m_dwModifierCount; ++i)
    {
        if (tEvent.m_modifier[i].m_necessaryCondition == Achievement::MODIFIER_PLAYER_IS_HARDCORE)
            return true;
    }
    return false;
}
BOOL Achievements::IsAchievementHardcoreByCriteria(AchievementId idAchievement) const
{
    const AchievementDefinition * ptDef = GetAchievementDefinition(idAchievement);
    if (ptDef)
    {
        CriteriaIdList idCriteriaList;
        GetAchievementCriteria(idAchievement, idCriteriaList);
        for (CriteriaId idCriteria : idCriteriaList)
        {
            if (const CriteriaDefinition * ptCriteria = GetCriteriaDefinition(idCriteria))
            {
                if (IsCriteriaEventHardcore(ptCriteria->m_startEvent) ||
                    IsCriteriaEventHardcore(ptCriteria->m_advanceEvent) ||
                    IsCriteriaEventHardcore(ptCriteria->m_failEvent))
                    return TRUE;
            }
        }
    }
    return FALSE;
}
AchievementId Achievements::GetSeasonJourneyRankSeriesTail(uint32 nSeasonId) const
{
    if (!nSeasonId)
        return INVALID_ACHIEVEMENT_ID;
    AchievementsEngine::AchievementsStaticData * ptStaticData = m_pStaticDataFile;
    uint32 seasonPartition = GetSeasonDataPartition(nSeasonId);
    AchievementsEngine::SeasonJourneyRankTailsMap::const_iterator it = ptStaticData->GetSeasonJourneyRankTails().find(seasonPartition);
    if (it == ptStaticData->GetSeasonJourneyRankTails().end())
        return INVALID_ACHIEVEMENT_ID;
    return it->second;
}
AchievementId Achievements::GetHighestEarnedSeasonJourneyAchievement(uint32 nSeasonId, const GameAccountHandle & gameAccountId, AchievementsInterface * ptAchievementsInterface)
{
    AchievementId idSeasonJourneyRank = INVALID_ACHIEVEMENT_ID;
    AchievementId idSeasonJourneyRankTail = GetSeasonJourneyRankSeriesTail(nSeasonId);
    if (ptAchievementsInterface && idSeasonJourneyRankTail)
    {
        AchievementId idCurrentSeriesAchievement = idSeasonJourneyRankTail;
        const AchievementDefinition * ptDef;
        while ((ptDef = GetAchievementDefinition(idCurrentSeriesAchievement)) != NULL)
        {
            AchievementRecord tRecord;
            bool bAchievementEarned = ptAchievementsInterface->FindAwardedAchievement(gameAccountId, idCurrentSeriesAchievement, tRecord);
            if (idSeasonJourneyRank == INVALID_ACHIEVEMENT_ID && bAchievementEarned)
                idSeasonJourneyRank = ptDef->m_id;
            else if (!bAchievementEarned)
                idSeasonJourneyRank = INVALID_ACHIEVEMENT_ID;
            idCurrentSeriesAchievement = ptDef->m_supersedes;
        }
    }
    return idSeasonJourneyRank;
}
bool Achievements::IsAchievementInSeries(AchievementId achievementId) const {
    AchievementsEngine::AchievementsStaticData * ptStaticData = GetStaticDataFile();
    const AchievementsEngine::InternalAchievementDefinition * ptAchievement = ptStaticData->GetAchievementDefinition(achievementId);
    return ptAchievement && ptAchievement->m_seriesTailId;
}
AchievementId Achievements::GetSeriesTail(AchievementId achievementId) const {
    AchievementsEngine::AchievementsStaticData * ptStaticData = GetStaticDataFile();
    const AchievementsEngine::InternalAchievementDefinition * ptAchievement = ptStaticData->GetAchievementDefinition(achievementId);
    return ptAchievement && ptAchievement->m_seriesTailId ? ptAchievement->m_seriesTailId->m_id : 0;
}
bool Achievements::InsertReward(AchievementsEngine::AchievementsStaticData * staticData, const RewardDefinition & tReward, AchievementId achievementId)
{
    AchievementsEngine::InternalRewardDefinition * ptNewReward;
    AchievementsEngine::RewardMap::iterator it = staticData->GetRewardMap().find(tReward.m_id);
    if (it != staticData->GetRewardMap().end())
    {
        ptNewReward = it->second;
        if (*ptNewReward != tReward)
            return false;
    }
    else
    {
        ptNewReward = new AchievementsEngine::InternalRewardDefinition;
        staticData->GetRewardMap()[tReward.m_id] = ptNewReward;
        ptNewReward->m_flags = tReward.m_flags;
        ptNewReward->m_hIcon = tReward.m_hIcon;
        ptNewReward->m_id = tReward.m_id;
        ptNewReward->m_type = tReward.m_type;
        ptNewReward->m_currency = tReward.m_currency;
        ptNewReward->m_quantity = tReward.m_quantity;
        ptNewReward->m_nameKey = tReward.m_pszNameKey ? tReward.m_pszNameKey : "";
        ptNewReward->m_pszNameKey = ptNewReward->m_nameKey.c_str();
    }
    AchievementsEngine::AchievementMap::const_iterator achievementIt = staticData->GetAchievementMap().find(achievementId);
    ptNewReward->m_parentAchievements.push_back(achievementIt->second);
    return true;
}
// NON_MATCHING: 1324 vs 1304 bytes. Re-laid on DWARF lines 813-947 (`if (CreateItemRewardCurrency(...))`, `(CurrencyType) atoi`
// read from columns 891:21 / 893:40). Remaining: the RewardDefinition ctor's m_hIcon/m_currency 64-bit store is sunk into
// `stp x23, x8, [sp, #0x38]` (with m_pszNameKey) in the original; ours stores it right after the first call in each
// transmog/cosmetic/item branch. 945:9 is one row in the original (likely a macro; ours is an if/else).
#line 813
void Achievements::LoadAchievementsRewards(
    AchievementsEngine::AchievementsStaticData * staticData,
    const D3::AchievementsStaticData::AchievementFile & achievementFile)
{

    ScopedGlobalDefinitionLockR ptDefinition;
    s_nextRewardId = ptDefinition->tBannerParams.nLastAllocatedRewardId + 1;


    const int achievementCount = achievementFile.achievement_size();
    for (int ach = 0; ach < achievementCount; ++ach)
    {
        const D3::AchievementsStaticData::StaticAchievementDefinition & achievementDefinition =
            achievementFile.achievement(ach);

        bool hasReward = false;

        for (int32 i = 0; i < achievementDefinition.attributes_size(); ++i)
        {
            const D3::AchievementsStaticData::Attribute & attribute = achievementDefinition.attributes(i);

            if (attribute.key() == Achievement::REWARD_BANNER_SIGIL)
            {
                CreateBannerTexturePairReward(
                    staticData,
                    REWARD_TYPE_BANNER_SIGIL,
                    attribute.value().c_str(),
                    achievementDefinition.id(),
                    0);
            }
            else if (attribute.key() == Achievement::REWARD_BANNER_SIGIL_ACCENT)
            {
                CreateBannerTexturePairReward(
                    staticData,
                    REWARD_TYPE_BANNER_SIGIL_ACCENT,
                    attribute.value().c_str(),
                    achievementDefinition.id(),
                    0);
            }
            else if (attribute.key() == Achievement::REWARD_BANNER_PATTERN)
            {
                CreateBannerTexturePairReward(
                    staticData,
                    REWARD_TYPE_BANNER_PATTERN,
                    attribute.value().c_str(),
                    achievementDefinition.id(),
                    0);
            }
            else if (attribute.key() == Achievement::REWARD_BANNER_SHAPE)
            {
                CreateBannerTexturePairReward(
                    staticData,
                    REWARD_TYPE_BANNER_SHAPE,
                    attribute.value().c_str(),
                    achievementDefinition.id(),
                    0);
            }
            else if (attribute.key() == Achievement::REWARD_BANNER_COLOR)
            {
                CreateBannerColorSetReward(
                    staticData,
                    attribute.value().c_str(),
                    achievementDefinition.id(),
                    0);
            }
            else if (attribute.key() == Achievement::REWARD_ITEM_TRANSMOG)
            {
                CreateItemTransmogReward(
                    staticData,
                    attribute.value().c_str(),
                    achievementDefinition.id(),
                    0);
                hasReward = true;
            }

            else if (attribute.key() == Achievement::REWARD_CURRENCY)
            {
                const char * pValue = FindAchievementAttribute(achievementDefinition.attributes(), Achievement::REWARD_CURRENCY_QUANTITY);
                if (CreateItemRewardCurrency(
                        staticData,
                        (CurrencyType) atoi(attribute.value().c_str()),
                        pValue != NULL ? atoi(pValue) : 0,
                        achievementDefinition.id(),
                        0))
                {
                    hasReward = true;
                }
            }
            else if (attribute.key() == Achievement::REWARD_COSMETIC)
            {
                CreateCosmeticReward(
                    staticData,
                    attribute.value().c_str(),
                    achievementDefinition.id(),
                    0);
                hasReward = true;
            }

            else if (attribute.key() == Achievement::REWARD_ITEM)
            {
                CreateItemReward(
                    staticData,
                    attribute.value().c_str(),
                    achievementDefinition.id(),
                    0);
                hasReward = true;
            }
        }














        AchievementsEngine::InternalAchievementDefinition * achievement = const_cast<AchievementsEngine::InternalAchievementDefinition*>(staticData->GetAchievementDefinition(achievementDefinition.id()));








        // The original has one row for this at 945:9 (statement start; likely a macro, name unknown).
        if (hasReward) achievement->m_flags |= (1u << ACHIEVEMENT_FLAG_GUARANTEE_REWARD); else achievement->m_flags &= ~(1u << ACHIEVEMENT_FLAG_GUARANTEE_REWARD);
    }
}
#line 372
void Achievements::CreateBannerTexturePairReward(AchievementsEngine::AchievementsStaticData * staticData, RewardType eRewardType, const char * pszSnoName, AchievementId achId, uint32 uRewardFlags)
{
    uint32 uHash = SNOFileLookupGUID(TEXTURE_SNO, pszSnoName);
    uint32 nIndex;
    BannerTexturePair * ptBannerTexturePair = NULL;
    ScopedGlobalDefinitionLockR tGlobals;
    switch (eRewardType)
    {
        case REWARD_TYPE_BANNER_SIGIL:
            if (tGlobals->tBannerParams.ptParamsMaps->mapTexMainSigils.Lookup(uHash, nIndex))
            {
                ptBannerTexturePair = &tGlobals->tBannerParams.arTexMainSigils[nIndex];
            }
            break;
        case REWARD_TYPE_BANNER_SIGIL_ACCENT:
            if (tGlobals->tBannerParams.ptParamsMaps->mapTexSigilAccents.Lookup(uHash, nIndex))
            {
                ptBannerTexturePair = &tGlobals->tBannerParams.arTexSigilAccents[nIndex];
            }
            break;
        case REWARD_TYPE_BANNER_PATTERN:
            if (tGlobals->tBannerParams.ptParamsMaps->mapTexPatterns.Lookup(uHash, nIndex))
            {
                ptBannerTexturePair = &tGlobals->tBannerParams.arTexPatterns[nIndex];
            }
            break;
        case REWARD_TYPE_BANNER_SHAPE:
            if (tGlobals->tBannerParams.ptParamsMaps->mapTexBackgrounds.Lookup(uHash, nIndex))
            {
                ptBannerTexturePair = &tGlobals->tBannerParams.arTexBackgrounds[nIndex];
            }
            break;
    }
    if (ptBannerTexturePair)
    {
        RewardDefinition tReward;
        tReward.m_id = ptBannerTexturePair->idReward;
        tReward.m_type = eRewardType;
        tReward.m_flags = uRewardFlags;
        tReward.m_pszNameKey = pszSnoName;
        tReward.m_hIcon = ptBannerTexturePair->hIcon;
        InsertReward(staticData, tReward, achId);
    }
}
void Achievements::CreateBannerColorSetReward(AchievementsEngine::AchievementsStaticData * staticData, const char * pszSnoName, AchievementId achId, uint32 uRewardFlags)
{
    uint32 uHash = XHashKey<LPCSTR>(pszSnoName);
    uint32 nIndex;
    ScopedGlobalDefinitionLockR tGlobals;
    if (!tGlobals->tBannerParams.ptParamsMaps->mapColorSets.Lookup(uHash, nIndex))
        return;
    BannerColorSet * ptBannerColorSet = &tGlobals->tBannerParams.arColorSets[nIndex];
    if (ptBannerColorSet)
    {
        RewardDefinition tReward;
        tReward.m_id = ptBannerColorSet->idReward;
        tReward.m_flags = uRewardFlags;
        tReward.m_pszNameKey = pszSnoName;
        tReward.m_hIcon = ptBannerColorSet->hIcon;
        InsertReward(staticData, tReward, achId);
    }
}
void Achievements::CreateItemTransmogReward(AchievementsEngine::AchievementsStaticData * staticData, const char * pszItemName, AchievementId achId, uint32 uRewardFlags)  // line 1053
{
    GBID gbidItemReward = atoi(pszItemName);
    RewardDefinition tReward;
    tReward.m_id = gbidItemReward;
    tReward.m_type = REWARD_TYPE_ITEM_TRANSMOG;
    tReward.m_flags = uRewardFlags;
    tReward.m_pszNameKey = pszItemName;
    InsertReward(staticData, tReward, achId);
}
bool Achievements::CreateItemRewardCurrency(AchievementsEngine::AchievementsStaticData * staticData, CurrencyType eCurrencyType, uint32 nQuantity, AchievementId achId, uint32 uRewardFlags)  // line 1076
{
    bool freeToPlayMode = false;
    if (eCurrencyType <= CURRENCY_INVALID || eCurrencyType >= NUM_CURRENCY_TYPES || (eCurrencyType == CURRENCY_PLATINUM && !freeToPlayMode))
        return false;

    RewardDefinition tReward;
    tReward.m_id = s_nextRewardId++;
    tReward.m_currency = eCurrencyType;
    tReward.m_type = REWARD_TYPE_ITEM_CURRENCY;
    tReward.m_flags = uRewardFlags;
    tReward.m_quantity = nQuantity;
    switch (tReward.m_currency)  // on the member, not the parameter: out-of-line size 0xbc as in the DWARF (0xc8 on eCurrencyType)
    {
        case CURRENCY_GOLD:
            tReward.m_pszNameKey = "RewardTypeGold";
            break;
        case CURRENCY_BLOODSHARDS:
            tReward.m_pszNameKey = "RewardTypeBloodShards";
            break;
        case CURRENCY_PLATINUM:
            tReward.m_pszNameKey = "RewardTypePlatinum";
            break;
    }
    if (nQuantity == 0)
        return false;
    switch (eCurrencyType)
    {
        case CURRENCY_GOLD:
            tReward.m_hIcon = StringToImageHandle("Achievement_Temp_01_icon");
            break;
        case CURRENCY_BLOODSHARDS:
            tReward.m_hIcon = StringToImageHandle("Achievement_Temp_01_icon");
            break;
        case CURRENCY_PLATINUM:
            tReward.m_hIcon = StringToImageHandle("p2_Platinum_AchievementIcon");
            break;
    }
    return InsertReward(staticData, tReward, achId);
}
void Achievements::CreateCosmeticReward(AchievementsEngine::AchievementsStaticData * staticData, const char * pszItemName, AchievementId achId, uint32 uRewardFlags)  // line 1167
{
    GBID gbidCosmeticReward = atoi(pszItemName);
    RewardDefinition tReward;
    tReward.m_id = gbidCosmeticReward;
    tReward.m_type = REWARD_TYPE_COSMETIC;
    tReward.m_flags = uRewardFlags;
    tReward.m_pszNameKey = pszItemName;
    InsertReward(staticData, tReward, achId);
}
void Achievements::CreateItemReward(AchievementsEngine::AchievementsStaticData * staticData, const char * pszItemName, AchievementId achId, uint32 uRewardFlags)  // line 1190
{
    GBID gbidItemReward = atoi(pszItemName);
    RewardDefinition tReward;
    tReward.m_id = gbidItemReward;
    tReward.m_type = REWARD_TYPE_ITEM;
    tReward.m_flags = uRewardFlags;
    tReward.m_pszNameKey = pszItemName;
    InsertReward(staticData, tReward, achId);
}
bool Achievements::LoadAchievementsDataRaw(const uint8 * pData, size_t dwDataSize, const blz::string & contentHandleHash)
{
    D3::AchievementsStaticData::AchievementFile achievementFile;
    bool success = false;
    if (achievementFile.ParseFromArray(pData, dwDataSize))
    {
        AchievementsEngine::AchievementsStaticData * staticData = new AchievementsEngine::AchievementsStaticData(contentHandleHash);
        staticData->ParseFromPb(achievementFile);
        LoadAchievementsRewards(staticData, achievementFile);
        PostprocessStaticData(staticData);
        ProcessSeasonJourneyObjectives(staticData);
        delete m_pOldStaticDataFile;
        m_pOldStaticDataFile = m_pStaticDataFile;
        m_pStaticDataFile = staticData;
        m_bStaticDataLoaded = true;
        success = true;
    }
    return success;
}


bool Achievements::LoadAchievementsDataByLooseFile()
{
    FileIO::File tFile(XLOCALE_GLOBAL);
    if (!tFile.Open(GetLooseFilenameConsole().str(), GetMPQFilename().str()))
        return false;
    int64 nFileSize = tFile.GetFileSize();
    if ((nFileSize & 0xFFFFFFFF) == 0)  // original line 1304 tests the low 32 bits before dwDataSize is declared (1309): one ands x21, x0, #0xffffffff (0x7100674ba0) feeds both the test and SigmaMemoryNew
        return false;
    uint32 dwDataSize = (uint32)nFileSize;
    uint8 * pData = (uint8 *)SigmaMemoryNew(dwDataSize, 4, NULL, FALSE);
    bool success = false;
    if (tFile.ReadFile(pData, dwDataSize, NULL))
        success = LoadAchievementsDataRaw(pData, dwDataSize, blz::string());
    SigmaMemoryFree(pData, NULL);
    return success;
}
static void sVerifyInitialBannerPieces(AchievementsEngine::AchievementsStaticData * staticData, XPtr64<BannerTexturePair> arTextures, SerializeData serTextures, int32 nTextureDefaults)
{
    int32 nTextureCount = serTextures.ElementCount(sizeof(BannerTexturePair));
    int32 nNumInitialPieces = nTextureDefaults ? nTextureDefaults : nTextureCount;
    nNumInitialPieces = Math::Min(nNumInitialPieces, nTextureCount);
    for (int32 i = 0; i < nNumInitialPieces; ++i)
    {
        // Only the lookup survives; whatever checked its result is compiled out.
        staticData->GetRewardDefinition(arTextures[i].idReward);
    }
}
static void sVerifyInitialBannerColors(AchievementsEngine::AchievementsStaticData * staticData, XPtr64<BannerColorSet> arColors, SerializeData serColors)
{
    for (int32 i = 0; i < serColors.ElementCount(sizeof(BannerColorSet)); ++i)
    {
        staticData->GetRewardDefinition(arColors[i].idReward);
    }
}





void Achievements::PostprocessStaticData(AchievementsEngine::AchievementsStaticData * staticData)
{
    for (AchievementsEngine::AchievementMap::const_iterator it = staticData->GetAchievementMap().begin(); it != staticData->GetAchievementMap().end(); it++)
    {
        AchievementsEngine::InternalAchievementDefinition * ptDefinition = it->second;
        if (ptDefinition->m_supersedes)
        {
            const AchievementsEngine::InternalCategoryDefinition * ptCategory = staticData->GetCategoryDefinition(ptDefinition->m_parentCategoryId);
            while (ptCategory->m_parentCategoryId)
                ptCategory = staticData->GetCategoryDefinition(ptCategory->m_parentCategoryId);
            if (ptCategory->m_flags & (1u << CATEGORY_FLAG_SEASON_JOURNEY))
            {
                for (int32 i = 0; i <= staticData->GetSeasonCycleLength(); ++i)
                {
                    if (ptDefinition->m_seasonMask & (1u << i))
                        staticData->GetSeasonJourneyRankTails()[i + 1] = ptDefinition->m_seriesTailId->m_id;
                }
            }
        }
    }

    for (AchievementsEngine::RewardMap::const_iterator it = staticData->GetRewardMap().begin(); it != staticData->GetRewardMap().end(); it++)
    {
        for (uint32 i = 0; i < it->second->m_parentAchievements.size(); ++i)
        {
            AchievementDefinition * achievement = it->second->m_parentAchievements[i];
            achievement->m_rewards.push_back(it->second->m_id);
        }
    }

    ScopedGlobalDefinitionLockR tGlobals;
    sVerifyInitialBannerPieces(staticData, tGlobals->tBannerParams.arTexBackgrounds, tGlobals->tBannerParams.serTexBackgrounds, tGlobals->tBannerParams.nTexBackgroundDefaults);
    sVerifyInitialBannerPieces(staticData, tGlobals->tBannerParams.arTexPatterns, tGlobals->tBannerParams.serTexPatterns, tGlobals->tBannerParams.nTexPatternDefaults);
    sVerifyInitialBannerPieces(staticData, tGlobals->tBannerParams.arTexMainSigils, tGlobals->tBannerParams.serTexMainSigils, tGlobals->tBannerParams.nTexSigilDefaults);
    sVerifyInitialBannerPieces(staticData, tGlobals->tBannerParams.arTexSigilAccents, tGlobals->tBannerParams.serTexSigilAccents, tGlobals->tBannerParams.nTexSigilAccentDefaults);
    sVerifyInitialBannerColors(staticData, tGlobals->tBannerParams.arColorSets, tGlobals->tBannerParams.serColorSets);
}
void Achievements::ProcessSeasonJourneyObjectives(AchievementsEngine::AchievementsStaticData * ptStaticData)
{
    AchievementsEngine::SeasonJourneyObjective & seasonJourneyObjectiveArray = ptStaticData->GetSeasonJourneyObjectiveArray();
    for (const blz::pair<const uint32, AchievementId> & seasonToJourneyRankTail : ptStaticData->GetSeasonJourneyRankTails())
    {
        AchievementId idSeriesTail = seasonToJourneyRankTail.second;
        uint32 nSeasonId = seasonToJourneyRankTail.first;
        seasonJourneyObjectiveArray.push_back(blz::unordered_map<AchievementId, AchievementId>());
        AchievementId idSeriesAchievement = idSeriesTail;
        const AchievementsEngine::InternalAchievementDefinition * ptSeriesDefinition = ptStaticData->GetAchievementDefinition(idSeriesAchievement);
        while (ptSeriesDefinition)
        {
            for (AchievementsEngine::InternalCriteriaDefinition * const ptCritDef : ptSeriesDefinition->m_criteriaList)
            {
                if (!ptCritDef || (ptCritDef->m_flags & (1u << CRITERIA_FLAG_HIDDEN)))
                    continue;
                const AchievementsEngine::InternalAchievementDefinition * ptCriteriaAchievementDefinition = ptStaticData->GetAchievementDefinition(ptCritDef->m_advanceEvent.m_comparand);
                if (ptCriteriaAchievementDefinition && ptCritDef->m_advanceEvent.m_id == Achievement::ACH_EVENT_EARNED_ACHIEVEMENT)
                {
                    if (!seasonJourneyObjectiveArray[nSeasonId - 1].count(ptCriteriaAchievementDefinition->m_id))
                        seasonJourneyObjectiveArray[nSeasonId - 1][ptCriteriaAchievementDefinition->m_id] = ptSeriesDefinition->m_id;
                }
            }
            idSeriesAchievement = ptSeriesDefinition->m_supersedes;
            ptSeriesDefinition = ptStaticData->GetAchievementDefinition(idSeriesAchievement);
        }
    }
}
inline CRefString Achievements::GetMPQFilename()  // line 1528
{
    return CRefString("d3.achievements_console.pb.bin.achu");
}
inline CRefString Achievements::GetLooseFilenameConsole()  // line 1536
{
    CRefString strFilename;
    return strFilename.Format("%s\\d3.achievements_console.pb.bin.achu", SNOFilesGetMiscPath().str());
}
AchievementsInterface::AchievementsInterface(Achievements * ptAchievements) : m_ptAchievements(ptAchievements) {}
AchievementsInterface::~AchievementsInterface() {}
ErrorCode AchievementsInterface::EvaluateEvent(const GameAccountHandle & gameAccount, uint32 eventId, uint64 quantity, Timestamp timestamp, const ModifierCallback & cb) { return EvaluateEvent(gameAccount, eventId, 0, quantity, timestamp, cb); }
ErrorCode AchievementsInterface::EvaluateEvent(const GameAccountHandle & gameAccount, uint32 eventId, uint64 comparand, uint64 quantity, Timestamp timestamp, const ModifierCallback & cb) { return 0; }
void AchievementsInterface::ConvertAchievementRecord(const AchievementsEngine::Achievement & bnetRecord, AchievementRecord & osRecord) {
    osRecord.m_achievementId = bnetRecord.m_achievementId;
    osRecord.m_timestampEarned = Blizzard::Time::FromUnixTime64(bnetRecord.m_completion);
}
void AchievementsInterface::ConvertCriteriaRecord(const AchievementsEngine::Criteria & bnetRecord, CriteriaRecord & osRecord) {
    osRecord.m_criteriaId = bnetRecord.m_criteriaId;
    osRecord.m_quantity = bnetRecord.m_baseQuantity + bnetRecord.m_deltaQuantity;
    osRecord.m_timestampStarted = Blizzard::Time::FromUnixTime64(bnetRecord.m_startTime);
}
} // namespace OnlineService
