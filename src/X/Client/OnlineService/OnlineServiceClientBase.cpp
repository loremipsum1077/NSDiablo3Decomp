#include "OnlineServiceClientBase.h"
#include "../CGame.h"
#include "../../Common/Player.h"
#include "../../Common/Definitions/ActDefinition.h"
#include "../../Sigma/DataStructures/XBaseMap.h"

namespace OnlineService {
// Original line 192. The optional values themselves are intentionally uninitialized.
inline HeroBasics::HeroBasics()
    : m_id(), m_nickname(""), m_selectedHeroId(0), m_selectedHeroName(""),
      m_availability(Availability::OFFLINE), m_currentActivity(CurrentActivity::NONE),
      m_playerClass(-1), m_isInvalid(FALSE), m_heroLevel(0), m_altAdvancementLevel(),
      m_highestHeroSoloRiftCompleted(0), m_pvpRank(), m_highestUnlockedAct(0),
      m_seasonCreated(GLOBAL_SEASON_ID), m_banner(), m_nFlags(0) {}

BOOL HeroBasics::IsFemale() const { return (m_nFlags & (1 << PLAYERFLAG_FEMALE)) != 0; }
BOOL HeroBasics::IsHardcore() const { return (m_nFlags & (1 << PLAYERFLAG_HARDCORE)) != 0; }
inline uint32 HeroBasics::GetPlayerFlags() const { return m_nFlags; }
inline void HeroBasics::SetPlayerFlags(uint32 nFlags) { m_nFlags = nFlags; }
CRefString HeroBasics::GetClassName() const { return PlayerGetClassUIName(m_playerClass, !IsFemale()); }

DetailsDigest::DetailsDigest()
    : m_completedQuestMap(new QuestCompletionMap), m_lastPlayedAct(-1),
      m_lastPlayedQuest(SNO_NONE), m_lastPlayedQuestStep(-1), m_secondsPlayed(0),
      m_createdTimestamp(), m_lastPlayedTimestamp() {}

DetailsDigest::~DetailsDigest() {
    delete m_completedQuestMap;
    m_completedQuestMap = NULL;
}

// Inferred: fIsAtFirstQuest set inside an if (initialised FALSE) gives the original's predicate
// order; the combined expression and two-statement forms scheduled the act test later.
BOOL DetailsDigest::CanSelectQuests() const {
    Act eLastPlayedAct = m_lastPlayedAct;
    SNO snoLastPlayedQuest = m_lastPlayedQuest;
    Act eAct = 0;
    SNO snoAct = ActSNOFromActEnum(eAct);
    ScopedActDefinitionLockR ptDefAct(snoAct);
    SNO snoFirstQuest = ptDefAct->arQuests[0].snoQuest;
    BOOL fIsAtFirstQuest = FALSE; // inferred: no row of its own (DWARF decl 546; test on 547-548)
    if ((snoLastPlayedQuest == snoFirstQuest || snoLastPlayedQuest == SNO_NONE) && (eLastPlayedAct == eAct || eLastPlayedAct == -1)) fIsAtFirstQuest = TRUE;
    AppGlobals & tAppGlobals = sgtAppGlobals;
    // Original line 554 tests byte AppGlobals+0x43, bit 5 (dwDebugFlags bit 29);
    // the flag name is not recorded in this compile unit.
    if (tAppGlobals.dwDebugFlags & (1u << 29)) return TRUE;
    return !m_completedQuestMap->empty() || !fIsAtFirstQuest;
}

void HeroDigest::Import(const GameAccountHandle & gameAccountHandle, const CRefString & nickname,
                        const D3::Hero::Digest & heroDigest, int32 nAltAvancementLevel) {
    PlayerClass playerClass = PlayerClassFromGBID(heroDigest.gbid_class());
    BOOL heroIsInvalid = playerClass == -1;
    uint32 uPlayerFlags = heroDigest.player_flags();
    if (heroDigest.version() != gptGroupDefHero->GetCurrentVersion(SIGMAPLATFORM_UNKNOWN)) {
        if (heroDigest.version() < HERO_DEFINITION_VERSION_1_0_5) {
            // These retired quest-completion flag bits are identified by HeroDefinition.h.
            uPlayerFlags &= ~(((1u << (DEPRECATED_PLAYERFLAG_KILLED_LAST + 1)) - 1)
                & ~((1u << DEPRECATED_PLAYERFLAG_KILLED_FIRST) - 1));
        } else if (heroDigest.version() != HERO_DEFINITION_VERSION_1_0_5) {
            heroIsInvalid |= heroDigest.version() > HERO_DEFINITION_VERSION_2_0_0;
        }
    }
    VisualEquipment tempVisualEquipment;
    tempVisualEquipment.Import(heroDigest.visual_equipment());
    m_visual.SetEquipment(tempVisualEquipment);
    m_details.m_id = gameAccountHandle;
    m_details.m_nickname = nickname;
    m_details.m_selectedHeroId = heroDigest.hero_id().id_low();
    m_details.m_selectedHeroName = heroDigest.hero_name().c_str();
    m_details.m_playerClass = playerClass;
    m_details.m_isInvalid = heroIsInvalid;
    m_details.SetPlayerFlags(uPlayerFlags);
    m_details.m_heroLevel = heroDigest.level();
    m_details.m_highestHeroSoloRiftCompleted = heroDigest.highest_solo_rift_completed();
    if (heroDigest.has_pvp_rank()) m_details.m_pvpRank.Set(heroDigest.pvp_rank());
    else m_details.m_pvpRank.SetExists(FALSE);
    m_details.m_lastPlayedAct = heroDigest.last_played_act();
    m_details.m_highestUnlockedAct = heroDigest.highest_unlocked_act();
    m_details.m_secondsPlayed = heroDigest.time_played();
    m_details.m_createdTimestamp = Timestamp::FromSeconds(heroDigest.create_time());
    m_details.m_lastPlayedTimestamp = Timestamp::FromSeconds(heroDigest.last_played_time());
    m_details.PopulateCompletedQuestList(heroDigest.quest_history());
    m_details.m_lastPlayedQuest = heroDigest.last_played_quest();
    m_details.m_lastPlayedQuestStep = heroDigest.last_played_quest_step();
    m_details.m_seasonCreated = heroDigest.season_created();
    m_details.m_altAdvancementLevel.SetExists(FALSE);
    if (nAltAvancementLevel >= 0) m_details.m_altAdvancementLevel.Set(nAltAvancementLevel);
}
}

size_t blz::hash<OnlineService::QuestId>::operator()(const OnlineService::QuestId & tQuestEntry) const {
    return XHashKey<int>(tQuestEntry.snoQuest);
}
