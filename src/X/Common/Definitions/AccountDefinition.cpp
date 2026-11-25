// Original file: X/Common/Definitions/AccountDefinition.cpp

#include "AccountDefinition.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../Crafting/ItemCrafting.h"
#include "../ActorCommonData.h"
#include "../Player.h"
#include "../GlobalSNO.h"
#include "GlobalDefinition.h"
#include "../OnlineService/OnlineService.h"
#include "../../Client/Console/GamerProfile.h"
#include "../../Sigma/Random.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Attributes/AttributeSerializer.h"
#include "../Protobuf/Account.pb.h"
#include "../Protobuf/AttributeSerializer.pb.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../OnlineService/Achievements.h"  // DWARF: OnlineService::Achievements / RewardDefinition locals
// Declared here because its header does not carry it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;  // X/Sigma/Files/XFilesCommon.cpp:20

// X/Common/Definitions/AccountDefinition.h, which only carries the group definition class so far.
struct AccountDefinition {  // size 0x20
    RequiredDefinitionHeader tHeader;  // 0x00
    SerializeData serProtoData;  // 0x0c
    XPtr64<unsigned char> pProtoData;  // 0x18
};
static_assert(sizeof(AccountDefinition) == 0x20, "AccountDefinition");

void AccountDefinition_BannerToDefaults(D3::Account::BannerConfiguration & banner);  // line 593

static XVarScope s_scopeAccountDefinition("AccountDefinition", NULL);  // line 33
XVarUint32 s_varInitialPlatinumQuantity(s_scopeAccountDefinition, "InitialPlatinumQuantity", 0);  // line 34

CrafterSavedData stDefaultCrafterSavedData;  // line 45

static const TypeDescriptorField AccountDefinition_Fields[3] = {  // line 57
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AccountDefinition, serProtoData), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(AccountDefinition, pProtoData), 0x20,
                                     (int32)offsetof(AccountDefinition, serProtoData) - (int32)offsetof(AccountDefinition, pProtoData)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AccountDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AccountDefinition("Account", 61, AccountDefinition_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 59
const XTypeDescriptor * DT_ACCOUNT_DEFINITION = &TypeDescriptor_AccountDefinition;

AccountGroupDef * gptGroupDefAccount;  // line 64

// line 69
AccountGroupDef::AccountGroupDef()
    : GroupDefinition("Account", ACCOUNT_SNO,
                      SNOVersions(116, 116, 116, 116, 116),
                      SNODATATYPE_USERDATA, sizeof(AccountDefinition), DT_ACCOUNT_DEFINITION, 0, 0x142e)
{
}

// line 92
void AccountInitializeForApp()
{
    POS pos;
    PooledSNONameList listSNOFiles(GetSnoNameListPool());
    SNOFileEnumerateSNOsForGroup(ACCOUNT_SNO, &listSNOFiles);

    pos = listSNOFiles.GetHeadPosition();
    while (pos)
    {
        SNOName & snonameFile = listSNOFiles.GetNext(pos);

        gptGroupDefAccount->DeleteIfVersionIsWrong(snonameFile.snoHandle);
    }

    CrafterSavedDataSetDefaults(stDefaultCrafterSavedData);
}

// line 111
void AccountDisposeForApp()
{
}

// line 119
uint32 AccountGroupDef::GetMaxCount()
{
    return 4;
}

// line 126
void AccountGroupDef::GetDefaultData(void * ptDefaultDef) const
{
    SigmaMemoryClear(ptDefaultDef, sizeof(AccountDefinition));
}

// line 261. Rows read from the original image (0x7100e53ed4, 0x7100e53f7c, 0x7100e53fcc).
static const SavedAttribDef cgarAccountSaveAttribsDefs[23] = {
#include "../generated-game-data/account_attribs.inc"
#line 124
};

// line 288
static const SavedAttribDef cgarAccountSaveAttribsDefs_AccountWide[10] = {
#include "../generated-game-data/account_wide_attribs.inc"
#line 138
};

// line 303
static const SavedAttribDef cgarAccountSaveAttribsDefs_PartitionOnly[13] = {
#include "../generated-game-data/partition_attribs.inc"
#line 153
};

// line 320
void AccountDefinition_SerializeAttributes(ActorCommonData & tACDPlayer, D3::AttributeSerializer::SavedAttributes & tSavedAttributes, AccountSerializeMode eMode)
{
    switch (eMode)
    {
    case ASM_ALL:
        AttributeSerializer_Save(cgarAccountSaveAttribsDefs, sizeof(cgarAccountSaveAttribsDefs) / sizeof(cgarAccountSaveAttribsDefs[0]), tACDPlayer, tSavedAttributes);
        break;

    case ASM_ACCOUNT_ONLY:
        AttributeSerializer_Save(cgarAccountSaveAttribsDefs_AccountWide, sizeof(cgarAccountSaveAttribsDefs_AccountWide) / sizeof(cgarAccountSaveAttribsDefs_AccountWide[0]), tACDPlayer, tSavedAttributes);
        break;

    case ASM_PARITION_ONLY:
        AttributeSerializer_Save(cgarAccountSaveAttribsDefs_PartitionOnly, sizeof(cgarAccountSaveAttribsDefs_PartitionOnly) / sizeof(cgarAccountSaveAttribsDefs_PartitionOnly[0]), tACDPlayer, tSavedAttributes);
        break;
    }

    tACDPlayer.hGB.Export(*tSavedAttributes.mutable_gb_handle());
}

// line 358
int32 AccountDefinitionSavedAttributeGetInt(const D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKey, int32 nDefaultValue)
{
    return AttributeSerializer_GetInt(cgarAccountSaveAttribsDefs, sizeof(cgarAccountSaveAttribsDefs) / sizeof(cgarAccountSaveAttribsDefs[0]), tSavedAttributes, tKey, nDefaultValue);
}

// line 373
void AccountDefinitionSavedAttributeWriteInt(D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKey, const int32 nValue)
{
    AttributeSerializer_WriteInt(cgarAccountSaveAttribsDefs, sizeof(cgarAccountSaveAttribsDefs) / sizeof(cgarAccountSaveAttribsDefs[0]), tSavedAttributes, tKey, nValue);
}

// line 388
void AccountDefinitionSavedAttributeRemoveInt(D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKey)
{
    AttributeSerializer_RemoveInt(cgarAccountSaveAttribsDefs, sizeof(cgarAccountSaveAttribsDefs) / sizeof(cgarAccountSaveAttribsDefs[0]), tSavedAttributes, tKey);
}

// line 401
uint64 AccountDefinitionSavedAttributeGetUInt64(const D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKeyLo, const FastAttribKey & tKeyHi)
{
    return AttributeSerializer_GetUInt64(cgarAccountSaveAttribsDefs, sizeof(cgarAccountSaveAttribsDefs) / sizeof(cgarAccountSaveAttribsDefs[0]), tSavedAttributes, tKeyLo, tKeyHi);
}

// line 416
void AccountDefinitionSavedAttributeWriteUInt64(D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKeyLo, const FastAttribKey & tKeyHi, const uint64 uValue)
{
    AttributeSerializer_WriteUInt64(cgarAccountSaveAttribsDefs, sizeof(cgarAccountSaveAttribsDefs) / sizeof(cgarAccountSaveAttribsDefs[0]), tSavedAttributes, tKeyLo, tKeyHi, uValue);
}

// line 432
void AccountDefinition_RestoreAttributes(ActorCommonData & tACDPlayer, const D3::AttributeSerializer::SavedAttributes & tSavedAttributes)
{
    AttributeSerializer_Restore(cgarAccountSaveAttribsDefs, sizeof(cgarAccountSaveAttribsDefs) / sizeof(cgarAccountSaveAttribsDefs[0]), tSavedAttributes, tACDPlayer);
}

// line 567
void AccountDefinition_DigestToDefaults(D3::Account::Digest & tDigest)
{
    tDigest.set_version(gptGroupDefAccount->GetCurrentVersion(SIGMAPLATFORM_UNKNOWN));

    uint32 dwFlags = 0;  // 2.7.7 stores 0 (2.6.2: 0x294, bits 2, 4, 7 and 9)

    tDigest.set_flags(dwFlags);

    AccountDefinition_BannerToDefaults(*tDigest.mutable_banner_configuration());
}

// Matched (x13) by laying it on DWARF lines 593-714 with the two DWARF-only locals ptAchievements (640) and tReward (652).
// (Before: 12 bytes longer, register and stack-slot choice in the colour-set loop.)
// line 593
#line 593
void AccountDefinition_BannerToDefaults(
    D3::Account::BannerConfiguration & banner)
{
    RNG tRNG;
    RandInit(tRNG);

    ScopedGlobalDefinitionLockR tGlobals;
    uint32 uHash = 0;




    banner.set_pattern(0);






    banner.set_banner_shape(0);






    banner.set_sigil_main(0);






    banner.set_sigil_accent(0);



    int32 nNumPlacements = tGlobals->tBannerParams.serSigilPlacements.ElementCount(sizeof(BannerSigilPlacement));
    uHash = nNumPlacements > 0 ? XHashKey(tGlobals->tBannerParams.arSigilPlacements[0].szName) : 0;
    banner.set_sigil_placement(uHash);

    banner.set_use_sigil_variant(true);

    uint32 nNumColorSets = tGlobals->tBannerParams.serColorSets.ElementCount(sizeof(BannerColorSet));

    blz::vector<int> arValidSigilColors, arValidShapeColors, arValidPatternColors;
    // DWARF local without location or code (640); declared, unused.
    OnlineService::Achievements * ptAchievements;









    for (uint32 i = 0; i < nNumColorSets; i++)
    {
        OnlineService::RewardDefinition tReward;  // DWARF local without location or code; unused.
        const BannerColorSet & tColorSet = tGlobals->tBannerParams.arColorSets[i];





        if (tColorSet.dwFlags & 0x1)
        {
            arValidSigilColors.push_back(i);
        }
        if (tColorSet.dwFlags & 0x2)
        {
            arValidShapeColors.push_back(i);
        }
        if (tColorSet.dwFlags & 0x4)
        {
            arValidPatternColors.push_back(i);
        }
    }

    int32 nSigilColorIndex;
    if (arValidSigilColors.size() > 0)
    {
        nSigilColorIndex = RandGetDWord(tRNG) % arValidSigilColors.size();
        nSigilColorIndex = arValidSigilColors[nSigilColorIndex];
    }
    else
    {
        nSigilColorIndex = RandGetDWord(tRNG) % nNumColorSets;
    }
    const BannerColorSet & tSigilColorSet = tGlobals->tBannerParams.arColorSets[nSigilColorIndex];
    uHash = BannerColorSet::XHash(tSigilColorSet);
    banner.set_sigil_color(uHash);

    int32 nShapeColorIndex;
    if (arValidShapeColors.size() > 0)
    {
        nShapeColorIndex = RandGetDWord(tRNG) % arValidShapeColors.size();
        nShapeColorIndex = arValidShapeColors[nShapeColorIndex];
    }
    else
    {
        nShapeColorIndex = RandGetDWord(tRNG) % nNumColorSets;
    }
    const BannerColorSet & tShapeColorSet = tGlobals->tBannerParams.arColorSets[nShapeColorIndex];
    uHash = BannerColorSet::XHash(tShapeColorSet);
    banner.set_background_color(uHash);

    int32 nPatternColorIndex;
    if (arValidPatternColors.size() > 0)
    {
        nPatternColorIndex = RandGetDWord(tRNG) % arValidPatternColors.size();
        nPatternColorIndex = arValidPatternColors[nPatternColorIndex];
    }
    else
    {
        nPatternColorIndex = RandGetDWord(tRNG) % nNumColorSets;
    }
    const BannerColorSet & tPatternColorSet = tGlobals->tBannerParams.arColorSets[nPatternColorIndex];
    uHash = BannerColorSet::XHash(tPatternColorSet);
    banner.set_pattern_color(uHash);
}
#line 315

// line 719
void AccountDefinition_ToDefaults(D3::Account::SavedDefinition & tDefinition)
{
    tDefinition = D3::Account::SavedDefinition::default_instance();
    tDefinition.set_version(gptGroupDefAccount->GetCurrentVersion(geSigmaPlatform));

    *tDefinition.mutable_saved_attributes() = D3::AttributeSerializer::SavedAttributes::default_instance();
    tDefinition.mutable_saved_attributes()->mutable_gb_handle()->set_game_balance_type(D3::GameBalance::Handle::default_instance().game_balance_type());
    tDefinition.mutable_saved_attributes()->mutable_gb_handle()->set_gbid(D3::GameBalance::Handle::default_instance().gbid());

    if (OnlineService::IsFreeToPlay() && s_varInitialPlatinumQuantity.Get() > 0)
    {
        D3::Items::CurrencyData & tCurrency = *tDefinition.mutable_account_wide_currency_data()->add_currency();
        tCurrency.set_id(2);
        tCurrency.set_count(s_varInitialPlatinumQuantity.Get());
        tCurrency.set_data_persist_flags(0);
    }

    D3::Account::AccountPartition * ptPartition = tDefinition.mutable_partitions()->Add();


    D3::AttributeSerializer::SavedAttribute * pNewAttribute = ptPartition->mutable_saved_attributes()->add_attributes();
    // The DWARF records this first set_key/set_value pair (lines 740-741); it is overwritten below, so its values are NOT recoverable and the ones here are stand-ins.
    pNewAttribute->set_key(1);
    pNewAttribute->set_value(0);

    pNewAttribute->set_key(1);
    pNewAttribute->set_value(0);

    *ptPartition->mutable_saved_attributes() = D3::AttributeSerializer::SavedAttributes::default_instance();
    ptPartition->mutable_saved_attributes()->mutable_gb_handle()->set_game_balance_type(D3::GameBalance::Handle::default_instance().game_balance_type());
    ptPartition->mutable_saved_attributes()->mutable_gb_handle()->set_gbid(D3::GameBalance::Handle::default_instance().gbid());
    ptPartition->clear_items();

    stDefaultCrafterSavedData.Export(*ptPartition->mutable_crafter_data(), NULL);

    tDefinition.mutable_digest()->mutable_last_played_hero_id()->set_id_high(0);
    tDefinition.mutable_digest()->mutable_last_played_hero_id()->set_id_low(0);

    AccountDefinition_DigestToDefaults(*tDefinition.mutable_digest());


    tDefinition.mutable_console_data()->set_nfp_default_time(0);
    tDefinition.mutable_console_data()->set_nfp_goblin_time(0);
}

// line 768
void AccountDefinition_ToDefaults_ForConsoleClient(D3::Account::SavedDefinition & tDefinition, int32 nUserIndex)
{
    AccountDefinition_ToDefaults(tDefinition);

    uint32 nSeed = XHashKey<LPCSTR>(Console::GamerProfile::GetGamerTag(nUserIndex));
    nSeed ^= (uint32)Blizzard::Time::ToUnixTime(Blizzard::Time::GetTimestamp());
    tDefinition.mutable_console_data()->set_console_cube_base_seed(nSeed);


    D3::Account::AccountPartition & tBasePartition = *tDefinition.mutable_partitions(0);
    tBasePartition.set_partition_id(0);


    for (uint32 i = 1; i < 6; i++)  // ACCOUNT_PARTITION_COUNT (Partition.h); 2.6.2 had 4
    {
        D3::Account::AccountPartition * ptPartition = tDefinition.add_partitions();
        *ptPartition = tBasePartition;

        ptPartition->set_partition_id(i);

        D3::AttributeSerializer::SavedAttribute * pNewAttribute = ptPartition->mutable_saved_attributes()->add_attributes();
        // The DWARF records this first set_key/set_value pair (lines 794-795); it is overwritten below, so its values are NOT recoverable and the ones here are stand-ins.
        pNewAttribute->set_key(1);
        pNewAttribute->set_value(0);

        pNewAttribute->set_key(1);
        pNewAttribute->set_value(0);

        *ptPartition->mutable_saved_attributes() = D3::AttributeSerializer::SavedAttributes::default_instance();
        ptPartition->mutable_saved_attributes()->mutable_gb_handle()->set_game_balance_type(D3::GameBalance::Handle::default_instance().game_balance_type());
        ptPartition->mutable_saved_attributes()->mutable_gb_handle()->set_gbid(D3::GameBalance::Handle::default_instance().gbid());
        ptPartition->clear_items();
    }
}
