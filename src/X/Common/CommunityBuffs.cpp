// New in 2.7.7: the community buff table (0x71004a5ac0..0x71004a8140) and the "Perk" cvars,
// which 2.6.2 kept in SPlayer.cpp. See CommunityBuffs.h.
#include "CommunityBuffs.h"
#include "../Client/Gfx/GfxVB.h"  // Its per-unit vertex normals open this file's initializer (0x71004a7bb0).
#include "Attributes/FastAttribConstantsSwig.h"
#include "Attributes/FastAttribute.h"
#include "ActorCommonData.h"
#include "Definitions/StringListDefinition.h"
#include "../Server/Powers/Powers.h"
#include "../Server/Powers/PowerAPI.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/UnicodeText.h"

// Initializer 0x71004a7bb0, in this order.
// NON_MATCHING (the initializer only): the GfxVB vertex-normal constants at its start are
// materialized and stored in another order (stp x8, x9 vs x9, x8); the cvars match.
static XVarScope s_scopePerk("Perk", NULL);
static XVarFloat s_varCommunityEventPerkXP(s_scopePerk, "CommunityEventXP", 0.0f);
static XVarFloat s_varCommunityEventPerkLegendaryFind(s_scopePerk, "CommunityEventLegendaryFind", 0.0f);
static XVarFloat s_varCommunityEventPerkGoldFind(s_scopePerk, "CommunityEventGoldFind", 0.0f);
static XVarBool s_varCommunityEventPerkDoubleRiftKeystones(s_scopePerk, "CommunityEventDoubleRiftKeystones", false);
static XVarBool s_varCommunityEventPerkDoubleBloodShards(s_scopePerk, "CommunityEventDoubleBloodShards", false);
static XVarBool s_varCommunityEventPerkDoubleTreasureGoblins(s_scopePerk, "CommunityEventDoubleTreasureGoblins", false);
static XVarBool s_varCommunityEventPerkDoubleBountyBags(s_scopePerk, "CommunityEventDoubleBountyBags", false);
static XVarBool s_varCommunityEventPerkRoyalGrandeur(s_scopePerk, "CommunityEventRoyalGrandeur", false);
static XVarBool s_varCommunityEventPerkLegacyOfNightmares(s_scopePerk, "CommunityEventLegacyOfNightmares", false);
static XVarBool s_varCommunityEventPerkTriunesWill(s_scopePerk, "CommunityEventTriunesWill", false);
static XVarBool s_varCommunityEventPerkPandemonium(s_scopePerk, "CommunityEventPandemonium", false);
static XVarBool s_varCommunityEventPerkKanaiPowers(s_scopePerk, "CommunityEventKanaiPowers", false);
static XVarBool s_varCommunityEventPerkTrialsOfTempests(s_scopePerk, "CommunityEventTrialsOfTempests", false);
static XVarBool s_varCommunityEventPerkShadowClones(s_scopePerk, "CommunityEventShadowClones", false);
static XVarBool s_varCommunityEventPerkFourthKanaisCubeSlot(s_scopePerk, "CommunityEventFourthKanaisCubeSlot", false);
static XVarBool s_varCommunityEventPerkEtherealItems(s_scopePerk, "CommunityEventEthrealItems", false);  // sic
static XVarBool s_varCommunityEventPerkSoulShards(s_scopePerk, "CommunityEventSoulShards", false);
static XVarBool s_varCommunityEventPerkSwarmRifts(s_scopePerk, "CommunityEventSwarmRifts", false);
static XVarBool s_varCommunityEventPerkSanctifiedItems(s_scopePerk, "CommunityEventSanctifiedItems", false);
static XVarBool s_varCommunityEventPerkDarkAlchemy(s_scopePerk, "CommunityEventDarkAlchemy", true);
static XVarBool s_varCommunityEventPerkParagonCap(s_scopePerk, "CommunityEventParagonCap", false);
static XVarBool s_varCommunityEventPerkNestingPortals(s_scopePerk, "CommunityEventNestingPortals", true);
XVarBool s_varIgrPerkEnabled(s_scopePerk, "IgrEnabled", false);
XVarFloat s_varIgrPerkXP(s_scopePerk, "IgrXP", 0.0f);
XVarBool s_varAnniversaryPerkEnabled(s_scopePerk, "AnniversaryEnabled", false);
XVarBool s_varEasterEggWorldPerkEnabled(s_scopePerk, "EasterEggWorldEnabled", false);
XVarBool s_varCommunityEventPerkEnabled(s_scopePerk, "CommunityEventEnabled", false);
XVarBool s_varCommunityEventPerkSeasonOnly(s_scopePerk, "CommunityEventSeasonOnly", false);

// 0x71004a5ac0
CommunityBuff::CommunityBuff(int32 nId, FastAttribKey tAttribKey, const blz::string & sConfigKey,
                             const blz::string & sLabel)
    : m_nId(nId), m_tAttribKey(tAttribKey), m_sConfigKey(sConfigKey), m_sLabel(sLabel)
{
}

// 0x71004a5c80
void CommunityBuff::ReadConfig(const KeyValueMap & mapKeyValues)
{
    CRefString sValue;
    if (mapKeyValues.Lookup(m_sConfigKey.c_str(), sValue))
    {
        ParseValue(blz::string(sValue.str()));
    }
}

// Adds buff index nId of the community power to its owner (the start data's attributes are
// filled by the caller through tAttribKey/value). Shared shape of 0x71004a5d60/0x71004a5f70.

// 0x71004a5d60
BOOL CommunityBuffToggle::Apply(PowerID idPower)
{
    int32 nId = m_nId;
    FastAttribKey tKey = m_tAttribKey;
    int32 nValue = GetValue();

    if (PowerTryToGet(idPower) == NULL)
        return FALSE;
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL)
        return FALSE;

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = true;
    tBuffStartData.bPositiveEffect = true;
    tBuffStartData.nBuffIndex = nId;
    if (tKey.nValue != ATTRIBUTE_NONE)
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, tKey, nValue);
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
    return TRUE;
}

// 0x71004a5e20
BOOL CommunityBuffToggle::IsFeatureEnabled() const
{
    return m_pCvar->Get();
}

// 0x71004a5e40
BOOL CommunityBuffToggle::IsActive() const
{
    return GetValue();
}

// 0x71004a5e60
BOOL CommunityBuffToggle::IsActiveInCvar() const
{
    return m_pCvar->Get();
}

// 0x71004a5e80
void CommunityBuffToggle::Reset()
{
    m_nValue = m_nDefault;
}

// 0x71004a5e90
void CommunityBuffToggle::LoadFromCvar()
{
    m_nValue = m_pCvar->Get();
}

// 0x71004a5ec0
blz::string CommunityBuffToggle::GetDescription() const
{
    return blz::string(StringListFetch(m_sLabel.c_str()).str());
}

// 0x71004a5f10: the config value is an integer, nonzero = on.
void CommunityBuffToggle::ParseValue(const blz::string & sValue)
{
    m_nValue = blz::stoi(sValue, NULL, 10);
}

// 0x71004a5f40
void CommunityBuffToggle::StoreToCvar() const
{
    m_pCvar->Set(GetValue() != 0, XVarBase::DIRTY_DEFAULT);
}

// 0x71004a5f70
BOOL CommunityBuffScalar::Apply(PowerID idPower)
{
    float flValue = m_flValue;
    int32 nId = m_nId;
    FastAttribKey tKey = m_tAttribKey;

    if (PowerTryToGet(idPower) == NULL)
        return FALSE;
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL)
        return FALSE;

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = true;
    tBuffStartData.bPositiveEffect = true;
    tBuffStartData.nBuffIndex = nId;
    if (tKey.nValue != ATTRIBUTE_NONE)
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, tKey, flValue);
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
    return TRUE;
}

// 0x71004a6020
BOOL CommunityBuffScalar::IsFeatureEnabled() const
{
    return FALSE;
}

// 0x71004a6030
BOOL CommunityBuffScalar::IsActive() const
{
    return m_flValue > EPSILON;
}

// 0x71004a6050
BOOL CommunityBuffScalar::IsActiveInCvar() const
{
    return m_pCvar->Get() > EPSILON;
}

// 0x71004a6080
void CommunityBuffScalar::Reset()
{
    m_flValue = m_flDefault;
}

// 0x71004a6090
void CommunityBuffScalar::LoadFromCvar()
{
    m_flValue = m_pCvar->Get();
}

// 0x71004a60c0: the label is a format with one argument, the value as a percentage.
blz::string CommunityBuffScalar::GetDescription() const
{
    return blz::string(LocalizedSprintf(StringListFetch(m_sLabel.c_str()).str(),
                                        UnicodeText::FormatNumber(m_flValue * 100.0f, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID).str(),
                                        NULL).str());
}

// 0x71004a6180: the config value is a fraction (0.5 = +50%).
void CommunityBuffScalar::ParseValue(const blz::string & sValue)
{
    m_flValue = blz::stof(sValue, NULL);
}

// 0x71004a61b0
void CommunityBuffScalar::StoreToCvar() const
{
    m_pCvar->Set(m_flValue, XVarBase::DIRTY_DEFAULT);
}

namespace {
// The creation temporaries, constructor templates on the two string-literal sizes. With internal
// linkage the instantiations used once are inlined into CommunityBuffs::CommunityBuffs, as in the
// original; the three used three times stay out of line: 0x71004a6d00 (27/38-character
// literals), 0x71004a6de0 (25/36) and 0x71004a6ec0 (24/35).
// NON_MATCHING (those three only): one store is scheduled one instruction later.
class NewCommunityBuff : public CommunityBuffPtr {
public:
    template <size_t N, size_t M>
    NewCommunityBuff(int32 nId, XVarBool * pCvar, Attrib eAttrib, const char (&szKey)[N], const char (&szLabel)[M])
        : CommunityBuffPtr(new CommunityBuffToggle(nId, pCvar, eAttrib, szKey, szLabel)) {}
    template <size_t N, size_t M>
    NewCommunityBuff(int32 nId, XVarBool * pCvar, Attrib eAttrib, const char (&szKey)[N], const char (&szLabel)[M], BOOL bAlwaysOn)
        : CommunityBuffPtr(new CommunityBuffToggle(nId, pCvar, eAttrib, szKey, szLabel, bAlwaysOn)) {}
    template <size_t N, size_t M>
    NewCommunityBuff(int32 nId, XVarFloat * pCvar, Attrib eAttrib, const char (&szKey)[N], const char (&szLabel)[M])
        : CommunityBuffPtr(new CommunityBuffScalar(nId, pCvar, eAttrib, szKey, szLabel)) {}
};
}  // namespace

// 0x71004a61d0
CommunityBuffs::CommunityBuffs()
{
    m_apBuffs[COMMUNITY_BUFF_XP] = NewCommunityBuff(COMMUNITY_BUFF_XP, &s_varCommunityEventPerkXP, ATTRIBUTE_EXPERIENCE_BONUS_PCT_COMMUNITY_BUFF,
        "CommunityBuffXP", "ConsoleUI:CommunityBuff_XP");
    m_apBuffs[COMMUNITY_BUFF_LEGENDARY_FIND] = NewCommunityBuff(COMMUNITY_BUFF_LEGENDARY_FIND, &s_varCommunityEventPerkLegendaryFind, ATTRIBUTE_LEGENDARY_FIND_COMMUNITY_BUFF,
        "CommunityBuffLegendaryFind", "ConsoleUI:CommunityBuff_Legendary");
    m_apBuffs[COMMUNITY_BUFF_GOLD_FIND] = NewCommunityBuff(COMMUNITY_BUFF_GOLD_FIND, &s_varCommunityEventPerkGoldFind, ATTRIBUTE_GOLD_FIND_COMMUNITY_BUFF,
        "CommunityBuffGoldFind", "ConsoleUI:CommunityBuff_Gold");
    m_apBuffs[COMMUNITY_BUFF_DOUBLE_GOBLINS] = NewCommunityBuff(COMMUNITY_BUFF_DOUBLE_GOBLINS, &s_varCommunityEventPerkDoubleTreasureGoblins, ATTRIBUTE_NONE,
        "CommunityBuffDoubleGoblins", "ConsoleUI:CommunityBuff_DoubleGoblins");
    m_apBuffs[COMMUNITY_BUFF_DOUBLE_BLOOD_SHARDS] = NewCommunityBuff(COMMUNITY_BUFF_DOUBLE_BLOOD_SHARDS, &s_varCommunityEventPerkDoubleBloodShards, ATTRIBUTE_DOUBLE_BLOODSHARDS,
        "CommunityBuffDoubleBloodShards", "ConsoleUI:CommunityBuff_DoubleBloodShards");
    m_apBuffs[COMMUNITY_BUFF_DOUBLE_RIFT_KEYSTONES] = NewCommunityBuff(COMMUNITY_BUFF_DOUBLE_RIFT_KEYSTONES, &s_varCommunityEventPerkDoubleRiftKeystones, ATTRIBUTE_NONE,
        "CommunityBuffDoubleRiftKeystones", "ConsoleUI:CommunityBuff_DoubleRiftKeystones");
    m_apBuffs[COMMUNITY_BUFF_DOUBLE_BOUNTY_BAGS] = NewCommunityBuff(COMMUNITY_BUFF_DOUBLE_BOUNTY_BAGS, &s_varCommunityEventPerkDoubleBountyBags, ATTRIBUTE_DOUBLE_BOUNTY_BAGS,
        "CommunityBuffDoubleBountyBags", "ConsoleUI:CommunityBuff_DoubleBountyCacheBags");
    m_apBuffs[COMMUNITY_BUFF_ROYAL_GRANDEUR] = NewCommunityBuff(COMMUNITY_BUFF_ROYAL_GRANDEUR, &s_varCommunityEventPerkRoyalGrandeur, ATTRIBUTE_SET_ITEM_DISCOUNT,
        "CommunityBuffRoyalGrandeur", "ConsoleUI:CommunityBuff_RoyalGrandeur");
    m_apBuffs[COMMUNITY_BUFF_LEGACY_OF_NIGHTMARES] = NewCommunityBuff(COMMUNITY_BUFF_LEGACY_OF_NIGHTMARES, &s_varCommunityEventPerkLegacyOfNightmares, ATTRIBUTE_NONE,
        "CommunityBuffLegacyOfNightmares", "ConsoleUI:CommunityBuff_LegacyOfNightmares");
    m_apBuffs[COMMUNITY_BUFF_TRIUNES_WILL] = NewCommunityBuff(COMMUNITY_BUFF_TRIUNES_WILL, &s_varCommunityEventPerkTriunesWill, ATTRIBUTE_NONE,
        "CommunityBuffTriunesWill", "ConsoleUI:CommunityBuff_TriunesWill");
    m_apBuffs[COMMUNITY_BUFF_PANDEMONIUM] = NewCommunityBuff(COMMUNITY_BUFF_PANDEMONIUM, &s_varCommunityEventPerkPandemonium, ATTRIBUTE_NONE,
        "CommunityBuffPandemonium", "ConsoleUI:CommunityBuff_Pandemonium");
    m_apBuffs[COMMUNITY_BUFF_KANAI_POWERS] = NewCommunityBuff(COMMUNITY_BUFF_KANAI_POWERS, &s_varCommunityEventPerkKanaiPowers, ATTRIBUTE_KANAI_POWERS,
        "CommunityBuffKanaiPowers", "ConsoleUI:CommunityBuff_KanaiPowers");
    m_apBuffs[COMMUNITY_BUFF_TRIALS_OF_TEMPESTS] = NewCommunityBuff(COMMUNITY_BUFF_TRIALS_OF_TEMPESTS, &s_varCommunityEventPerkTrialsOfTempests, ATTRIBUTE_NONE,
        "CommunityBuffTrialsOfTempests", "ConsoleUI:CommunityBuff_TrialsOfTempests");
    m_apBuffs[COMMUNITY_BUFF_SHADOW_CLONES] = NewCommunityBuff(COMMUNITY_BUFF_SHADOW_CLONES, &s_varCommunityEventPerkShadowClones, ATTRIBUTE_NONE,
        "CommunityBuffShadowClones", "ConsoleUI:CommunityBuff_ShadowClones");
    m_apBuffs[COMMUNITY_BUFF_FOURTH_KANAIS_CUBE_SLOT] = NewCommunityBuff(COMMUNITY_BUFF_FOURTH_KANAIS_CUBE_SLOT, &s_varCommunityEventPerkFourthKanaisCubeSlot, ATTRIBUTE_FOURTH_KANAIS_CUBE_SLOT,
        "CommunityBuffFourthKanaisCubeSlot", "ConsoleUI:CommunityBuff_FourthKanaisCubeSlot");
    m_apBuffs[COMMUNITY_BUFF_ETHEREAL_ITEMS] = NewCommunityBuff(COMMUNITY_BUFF_ETHEREAL_ITEMS, &s_varCommunityEventPerkEtherealItems, ATTRIBUTE_ETHEREAL_ITEMS_UNLOCKED,
        "CommunityBuffEtherealItems", "ConsoleUI:CommunityBuff_EtherealItems");
    m_apBuffs[COMMUNITY_BUFF_SOUL_SHARDS] = NewCommunityBuff(COMMUNITY_BUFF_SOUL_SHARDS, &s_varCommunityEventPerkSoulShards, ATTRIBUTE_SOUL_SHARDS_UNLOCKED,
        "CommunityBuffSoulShards", "ConsoleUI:CommunityBuff_SoulShards");
    m_apBuffs[COMMUNITY_BUFF_SWARM_RIFTS] = NewCommunityBuff(COMMUNITY_BUFF_SWARM_RIFTS, &s_varCommunityEventPerkSwarmRifts, ATTRIBUTE_NONE,
        "CommunityBuffSwarmRifts", "ConsoleUI:CommunityBuff_SwarmRifts");
    m_apBuffs[COMMUNITY_BUFF_SANCTIFIED_ITEMS] = NewCommunityBuff(COMMUNITY_BUFF_SANCTIFIED_ITEMS, &s_varCommunityEventPerkSanctifiedItems, ATTRIBUTE_SANCTIFIED_ITEMS_UNLOCKED,
        "CommunityBuffSanctifiedItems", "ConsoleUI:CommunityBuff_SanctifiedItems");
    m_apBuffs[COMMUNITY_BUFF_DARK_ALCHEMY] = NewCommunityBuff(COMMUNITY_BUFF_DARK_ALCHEMY, &s_varCommunityEventPerkDarkAlchemy, ATTRIBUTE_DARK_ALCHEMY_POWERS_UNLOCKED,
        "CommunityBuffDarkAlchemy", "ConsoleUI:CommunityBuff_DarkAlchemy", TRUE);
    m_apBuffs[COMMUNITY_BUFF_PARAGON_CAP] = NewCommunityBuff(COMMUNITY_BUFF_PARAGON_CAP, &s_varCommunityEventPerkParagonCap, ATTRIBUTE_PARAGON_CAP_ENABLED,
        "CommunityBuffParagonCap", "ConsoleUI:CommunityBuff_ParagonCap");
    m_apBuffs[COMMUNITY_BUFF_NESTING_PORTALS] = NewCommunityBuff(COMMUNITY_BUFF_NESTING_PORTALS, &s_varCommunityEventPerkNestingPortals, ATTRIBUTE_NONE,
        "CommunityBuffNestingPortals", "ConsoleUI:CommunityBuff_NestingPortals", TRUE);
    ResetAll();
}

// 0x71004a6fa0
void CommunityBuffs::ResetAll()
{
    for (int32 i = 1; i < NUM_COMMUNITY_BUFFS; ++i)
        m_apBuffs[i]->Reset();
}

// 0x71004a7120
BOOL CommunityBuffs::IsFeatureEnabled(int32 nType) const
{
    return m_apBuffs[nType]->IsFeatureEnabled();
}

// The marker buff (index 0) the community power always carries while any buff is on.
static BOOL sAddMarkerBuff(PowerID idPower)
{
    if (PowerTryToGet(idPower) == NULL)
        return FALSE;
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL)
        return FALSE;

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = true;
    tBuffStartData.bPositiveEffect = true;
    tBuffStartData.nBuffIndex = COMMUNITY_BUFF_NONE;
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
    return TRUE;
}

// 0x71004a7130
void CommunityBuffs::Apply(PowerID idPower) const
{
    if (!AnyActive())
        return;

    sAddMarkerBuff(idPower);
    for (int32 i = 1; i < NUM_COMMUNITY_BUFFS; ++i)
    {
        if (m_apBuffs[i]->IsActive())
            m_apBuffs[i]->Apply(idPower);
    }
}

// 0x71004a71f0
BOOL CommunityBuffs::AnyActive() const
{
    for (int32 i = 1; i < NUM_COMMUNITY_BUFFS; ++i)
    {
        if (m_apBuffs[i]->IsActive())
            return TRUE;
    }
    return FALSE;
}

// 0x71004a73e0
BOOL CommunityBuffs::NeedsRefresh(const ActorCommonData & tACD, SNO snoPower) const
{
    for (int32 i = 1; i < NUM_COMMUNITY_BUFFS; ++i)
    {
        CommunityBuff * ptBuff = m_apBuffs[i].Get();
        BOOL bOnActor = PowerAPI_BuffFindOnACD(snoPower, ptBuff->m_nId, tACD, ACDID_INVALID, FALSE) != BUFFID_INVALID;
        if (ptBuff->IsActiveInCvar() != bOnActor)
            return TRUE;
    }
    return FALSE;
}

// 0x71004a7480
void CommunityBuffs::LoadFromCvars()
{
    for (int32 i = 1; i < NUM_COMMUNITY_BUFFS; ++i)
        m_apBuffs[i]->LoadFromCvar();
}

// 0x71004a7600: one line per active buff, each followed by a newline.
blz::string CommunityBuffs::GetActiveDescriptions() const
{
    blz::string sText;
    for (int32 i = 1; i < NUM_COMMUNITY_BUFFS; ++i)
    {
        if (m_apBuffs[i]->IsActive())
        {
            sText += m_apBuffs[i]->GetDescription();
            sText += "\n";
        }
    }
    return sText;
}

// 0x71004a7800
void CommunityBuffs::ReadConfig(const KeyValueMap & mapKeyValues)
{
    for (int32 i = 1; i < NUM_COMMUNITY_BUFFS; ++i)
        m_apBuffs[i]->ReadConfig(mapKeyValues);
}

// 0x71004a7920
void CommunityBuffs::StoreToCvars() const
{
    for (int32 i = 1; i < NUM_COMMUNITY_BUFFS; ++i)
        m_apBuffs[i]->StoreToCvar();
}
