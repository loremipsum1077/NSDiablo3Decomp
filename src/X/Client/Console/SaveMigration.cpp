// Original file: X/Client/Console/SaveMigration.cpp
// Console save migration: cross-platform import and export of saves, and upgrading
// older console save data to the current version.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../Battlenet/Subsystems/ModalNotificationManager.h"
#include "Online.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdStorage/bdContextUserStorageFileInfo.h"
#include "SaveMigration.h"
#include "../../Common/Crafting/ItemCrafting.h"

extern "C" const CrafterSavedData & sub_71006b3690();
#include "GamerProfile.h"
#include "../../Common/Protobuf/Account.pb.h"
#include "../../Common/Protobuf/Console.pb.h"
#include "../../Common/Protobuf/CS.pb.h"
#include "../../Common/Protobuf/Hero.pb.h"
#include "../../Common/Protobuf/Achievements.pb.h"
#include "../../Common/Protobuf/AccountMigration.pb.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Partition.h"
#include "../../Common/GameCommonData.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../Common/Protobuf/Items.pb.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../UserInterface/Console/ConsoleScreenManager.h"
#include "../UserInterface/Console/ConsoleModalNotificationManager.h"
#include "../../Common/Game.h"
#include "../../Common/Definitions/StringListDefinition.h"
#include "../../Sigma/RefString.h"
#include "../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLinkedAccounts/bdLinkedAccountsResult.h"
#include "../../../Contrib/Contrib/blz/include/blz/mem_shared_ptr.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include <string.h>
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdByteBuffer.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdByteBuffer.inl"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdMemory/bdMemory.inl"
#include "../../../Contrib/Contrib/Zlib/zlib.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Common/ItemsSwig.h"
namespace OnlineService { namespace AccountMigration {
void Migrate_110_111(D3::CS::Snapshot & tSnapshot); void Migrate_111_112(D3::CS::Snapshot & tSnapshot); void Migrate_112_113(D3::CS::Snapshot & tSnapshot);
void Migrate_113_114(const D3::CS::Snapshot & tIn, D3::CS::Snapshot & tOut); void Migrate_115_116(D3::CS::Snapshot & tSnapshot);
void Migrate_114_115(const D3::AccountMigration::ReadData_114 & tReadData, D3::AccountMigration::WriteData_115 & tWriteData);
void Migrate_109_110(const D3::AccountMigration::ReadData_109 & tReadData, D3::AccountMigration::WriteData_110 & tWriteData);
} }
int32 AccountDefinitionSavedAttributeGetInt(const D3::AttributeSerializer::SavedAttributes & tAttributes, const FastAttribKey & tKey, int32 nDefault);
void AccountDefinitionSavedAttributeWriteInt(D3::AttributeSerializer::SavedAttributes & tAttributes, const FastAttribKey & tKey, int32 nValue);
void AccountDefinitionSavedAttributeRemoveInt(D3::AttributeSerializer::SavedAttributes & tAttributes, const FastAttribKey & tKey);
void AccountDefinition_ToDefaults(D3::Account::SavedDefinition & tDefinition);
void AccountDefinition_ToDefaults_ForConsoleClient(D3::Account::SavedDefinition & tDefinition, int32 nUserIndex);
int32 HeroDefinitionSavedAttributeGetInt(const D3::AttributeSerializer::SavedAttributes & tAttributes, const FastAttribKey & tKey, int32 nDefault);
#include "../../Sigma/DataStructures/XBaseMap.h"
#include "../../Common/GameBalance.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
BOOL ItemIsCosmeticItem(const GBID gbidItem);
#include "../../Common/Definitions/HeroDefinition.h"
void HeroDefinition_Init(D3::Hero::SavedDefinition & tDefinition);
namespace Console { namespace GamerProfile { BOOL IsAccountLoaded(int32 nUserIdx); } }
struct LegacyItemConversion {  // size 0x28; GameBalanceDefinition.h:1333
    GBHeader tHeader;  // 0x00
    GBID gbidOldItem;  // 0x18
    GBID gbidNewItem;  // 0x1c
    BOOL bConsoleIgnore;  // 0x20
    uint32 dwPad;  // 0x24
};
typedef ScopedGBLockR<LegacyItemConversion, 45> ScopedLegacyItemConversionLockR;
enum { GB_LEGACY_ITEM_CONVERSIONS = 45 };
extern "C" BOOL sub_7100501ab0(const D3::Items::Generator & tGenerator);
extern "C" BOOL sub_710050e800(GBID gbidItem);
BOOL ItemHasLabel(const int32 gbidItem, const int32 eItemLabel);  // Items.h:90
int32 StringToGBID(const char * szGBID);  // GameBalance.h:75
namespace OnlineService { namespace AccountMigration { BOOL SeasonCleanSeasonalAchievements(D3::Achievements::Snapshot & tSnapshot);
struct SeasonMigrationData {  // AccountMigration.h:68
    SeasonMigrationData(D3::Account::AccountPartition * ptNonSeasonPartition, D3::Account::AccountPartition * ptSeasonPartition, const char * szLogHeader)
        : m_ptNonSeasonPartition(ptNonSeasonPartition), m_ptSeasonPartition(ptSeasonPartition), m_szLogHeader(szLogHeader), m_republishPresence(false) {}
    D3::Account::AccountPartition * m_ptNonSeasonPartition;
    D3::Account::AccountPartition * m_ptSeasonPartition;
    const char * m_szLogHeader;
    bool m_republishPresence;
};
BOOL SeasonMigratePartition(SeasonMigrationData & tData);
} }
namespace Console { namespace Online { BOOL IsSeasonsInitialized(); Blizzard::Time::Timestamp GetNetworkTimestamp();
void StorageFindUserFile(int32 nUserIndex, const char * szContext, const char * szFilename, StorageFindUserFileCallback pfnCallback, bdUserAccountID tOwner);
void StorageGetUserFile(int32 nUserIndex, const char * szContext, const bdContextUserStorageFileInfo & tFileInfo, StorageGetFileCallback pfnCallback); } }
namespace Console { namespace GamerProfile { enum ImportSaveResult { IMPORT_SAVE_FAILED, IMPORT_SAVE_FAILED_NO_STORAGE, IMPORT_SAVE_FAILED_CORRUPT, IMPORT_SAVE_SUCCESS }; typedef OnlineService::Callback<void (ImportSaveResult)> ImportSaveCallback; void ImportSave(int32 nUserIdx, D3::Console::MigrationData * ptMigrationData, BOOL bDestructive, ImportSaveCallback pfnCallback); } }
namespace Console { namespace UIOnlineActions { void SetGameParamsForHero(const D3::Account::SavedDefinition & tAccount, const D3::Hero::SavedDefinition & tHero, Online::GameParams & tParams); } }
namespace SHero { void DigestUItoDigest(D3::Hero::Digest & digest, const D3::Hero::DigestUI & digestUI); void DigesttoDigestUI(D3::Hero::DigestUI & digestUI, const D3::Hero::Digest & digest); }  int32 ActorInventoryTranslateFromSavedSlot(int32 eSavedSlotType);  // DWARF: X/Server/SHero.cpp:2557,2586; X/Common/ActorInventory.cpp:419
namespace Console {
namespace SaveMigration {
enum Status { SAVE_MIGRATION_STATUS_LOADING, SAVE_MIGRATION_STATUS_UPLOADING, SAVE_MIGRATION_STATUS_DOWNLOADING, SAVE_MIGRATION_STATUS_SAVING, SAVE_MIGRATION_STATUS_FINISHED };
void SaveMigrationUpdateStatus(int32 nUserIndex, Status eStatus);

bool PerformSeasonMigration(int32 nUserIdx);
void sCleanupExpiredSeasonalRolloverAndChallengeItems(D3::Account::SavedDefinition & tAccountDefinition);
void MigrateSavedItemsToSeasonRolloverCache(const D3::Items::ItemList & tItems, D3::Account::AccountPartition & tDestinationPartition);
BOOL HealMigrationData(int32 nUserIndex, D3::Console::MigrationData & tMigrationData);
void MergeLegacyLicenses(D3::Console::MigrationData & tMigrationData);
void _Migrate_Vanilla_RoS(D3::Console::MigrationData & tMigrationData); void _Migrate_108_109(D3::Console::MigrationData & tMigrationData); void _Migrate_109_110(D3::Console::MigrationData & tMigrationData);
void _Migrate_110_111(D3::Console::MigrationData & tMigrationData); void _Migrate_111_112(D3::Console::MigrationData & tMigrationData); void _Migrate_112_113(D3::Console::MigrationData & tMigrationData);
void _Migrate_113_114(D3::Console::MigrationData & tMigrationData); void _Migrate_114_115(D3::Console::MigrationData & tMigrationData); void _Migrate_115_116(D3::Console::MigrationData & tMigrationData);
void _Migrate_RoS_RoS212(D3::Console::MigrationData & tMigrationData); void _Migrate_220_to_221(D3::Console::MigrationData & tMigrationData); void _Migrate_221_to_243(D3::Console::MigrationData & tMigrationData);
void _Migrate_243_to_250a(D3::Console::MigrationData & tMigrationData); void _Migrate_250a_to_260(int32 nUserIndex, D3::Console::MigrationData & tMigrationData);
void sOnImportSaveComplete(int32 nUserIndex, SaveMigrationCallback pfnCallback, GamerProfile::ImportSaveResult eImportResult);
typedef blz::shared_ptr<blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> > > FileDataPtr;
void sOnFindFile(int32 nUserIndex, SaveMigrationCallback pfnCallback, Online::StorageResult eResult, const bdContextUserStorageFileInfo & tUserFile);
void sOnDownloadFileComplete(int32 nUserIndex, SaveMigrationCallback pfnCallback, Online::StorageResult eResult, FileDataPtr pszFileData);
void sOnCrossBrandConfirmation(int32 nUserIndex, SaveMigrationCallback pfnCallback, D3::Console::MigrationData * ptMigrationData, BOOL bConfirmed, const UIID &);
void sOnImportConfirmation(int32 nUserIndex, SaveMigrationCallback pfnCallback, D3::Console::MigrationData * ptMigrationData, BOOL bConfirmed, const UIID &);



const char * STORAGE_CONTEXT = "diablo3";
const char * STORAGE_FILENAME = "savemigration";

ModalNotificationId sidMigrationNotification = INVALID_MODAL_NOTIFICATION_ID;
BOOL s_bInProgress = FALSE;
bdContextUserStorageFileInfo s_tNewestFile;
int32 s_nFindFilesInProgress = 0;
BOOL s_bCrossPlatformSaveMigrationEnabled = FALSE;
} }
// DWARF X/Server/OnlineService/AccountMigration.cpp:132.
namespace OnlineService { namespace AccountMigration { BOOL Migrate_108_109(const D3::AccountMigration::ReadData_108 & tReadData, D3::AccountMigration::WriteData_109 & tWriteData); } }
namespace Console { namespace SaveMigration {
}
}
// DWARF X/Server/OnlineService/AccountMigration.cpp:121.
namespace OnlineService {
namespace AccountMigration {
BOOL Migrate_ConsoleVanilla_108(const D3::AccountMigration::ReadData_107 & tReadData, D3::AccountMigration::WriteData_108 & tWriteData);
}
}
namespace Console {
namespace SaveMigration {






















































void ExportSave(int32 nUserIndex, SaveMigrationCallback pfnCallback)
{
    SaveMigrationCallback pfnExportCallback = pfnCallback;

    if (GamerProfile::GetPrimaryProfileUserIndex() != nUserIndex)
    {
        pfnExportCallback(SAVE_MIGRATION_FAILED);
        return;
    }

    if (!GamerProfile::IsSignedInOnline(nUserIndex))
    {
        pfnExportCallback(SAVE_MIGRATION_FAILED_NOT_SIGNED_IN);
        return;
    }

    SaveMigrationUpdateStatus(nUserIndex, SAVE_MIGRATION_STATUS_LOADING);
}





















































































































































void ImportSave(int32 nUserIndex, SaveMigrationCallback pfnCallback)
{
    if (GamerProfile::IsGuest(nUserIndex))
    {
        pfnCallback(SAVE_MIGRATION_FAILED);
        return;
    }

    if (!GamerProfile::IsSignedInOnline(nUserIndex))
    {
        pfnCallback(SAVE_MIGRATION_FAILED_NOT_SIGNED_IN);
        return;
    }

    SaveMigrationUpdateStatus(nUserIndex, SAVE_MIGRATION_STATUS_DOWNLOADING);
    s_bInProgress = TRUE;

    s_tNewestFile = bdContextUserStorageFileInfo();
    ++s_nFindFilesInProgress;
    Online::StorageFindUserFile(nUserIndex, STORAGE_CONTEXT, STORAGE_FILENAME, OnlineService::internal::function_binder_4<void, int32, SaveMigrationCallback, Online::StorageResult, const bdContextUserStorageFileInfo &>::bind<&sOnFindFile>(nUserIndex, pfnCallback), bdUserAccountID());
}

























































void sOnFindFile(int32 nUserIndex, SaveMigrationCallback pfnCallback, Online::StorageResult eResult, const bdContextUserStorageFileInfo & tUserFile)
{
    if (eResult == Online::STORAGE_SUCCESS && tUserFile.m_modifedTime > s_tNewestFile.m_modifedTime)
        s_tNewestFile = tUserFile;

    if (--s_nFindFilesInProgress != 0)
        return;

    if (s_tNewestFile.m_modifedTime == 0)
    {
        sOnDownloadFileComplete(nUserIndex, pfnCallback, eResult, FileDataPtr());
        return;
    }

    Online::StorageGetUserFile(nUserIndex, STORAGE_CONTEXT, s_tNewestFile, OnlineService::internal::function_binder_4<void, int32, SaveMigrationCallback, Online::StorageResult, FileDataPtr>::bind<&sOnDownloadFileComplete>(nUserIndex, pfnCallback));
}


















void sOnDownloadFileComplete(int32 nUserIndex, SaveMigrationCallback pfnCallback, Online::StorageResult eDownloadResult, FileDataPtr pszFileData)
{
    s_bInProgress = FALSE;

    if (eDownloadResult != Online::STORAGE_SUCCESS)
    {
        Result eMigrationResult = SAVE_MIGRATION_FAILED;
        switch (eDownloadResult)
        {
        case Online::STORAGE_FAILED_NOT_CONNECTED: eMigrationResult = SAVE_MIGRATION_FAILED_UNABLE_TO_CONNECT; break;
        case Online::STORAGE_FAILED_FILE_NOT_FOUND: eMigrationResult = SAVE_MIGRATION_SUCCESS_NOT_FOUND; break;
        default: eMigrationResult = SAVE_MIGRATION_FAILED; break;
        }

        SaveMigrationUpdateStatus(nUserIndex, SAVE_MIGRATION_STATUS_FINISHED);
        pfnCallback(eMigrationResult);
        return;
    }

    // The file is the uncompressed size followed by a zlib-compressed MigrationData blob.
    uint32 uExpectedUncompressedSize = 0;
    uint8 * pauCompressedData = NULL;
    uint32 uCompressedSize = 0;
    bdByteBuffer tBuffer(pszFileData->modify_data(), (bdUInt)pszFileData->size(), false);
    bdBool bOk = tBuffer.readUInt32(uExpectedUncompressedSize);
    if (bOk)
        bOk = tBuffer.readAndAllocateBlob(pauCompressedData, uCompressedSize);

    uint8 * pauBuffer = (uint8 *)SigmaMemoryNew(uExpectedUncompressedSize, 4, NULL, FALSE);
    uLongf uUncompressedSize = uExpectedUncompressedSize;
    int nResult = uncompress(pauBuffer, &uUncompressedSize, pauCompressedData, uCompressedSize);
    bdDeallocate(pauCompressedData);

    if (nResult != Z_OK || uUncompressedSize != uExpectedUncompressedSize)
    {
        SigmaMemoryFree(pauBuffer, NULL);
        SaveMigrationUpdateStatus(nUserIndex, SAVE_MIGRATION_STATUS_FINISHED);
        pfnCallback(SAVE_MIGRATION_FAILED_DATA_CORRUPT);
        return;
    }

    D3::Console::MigrationData * ptMigrationData = new D3::Console::MigrationData;
    BOOL bParsed = ptMigrationData->ParseFromArray(pauBuffer, (int)uUncompressedSize);
    SigmaMemoryFree(pauBuffer, NULL);

    if (!bParsed ||
        !ptMigrationData->account().console_data().has_version_required() ||
        ptMigrationData->account().console_data().version_required() >= 202 ||
        ptMigrationData->title_version() >= 202 ||
        ptMigrationData->account().version() >= 109)
    {
        ::delete ptMigrationData;
        SaveMigrationUpdateStatus(nUserIndex, SAVE_MIGRATION_STATUS_FINISHED);
        pfnCallback(SAVE_MIGRATION_FAILED_DATA_CORRUPT);
        return;
    }

    s_bInProgress = TRUE;

    NotificationCreationParams tParams;
    tParams.nControllerIndex = nUserIndex;
    tParams.uszMessage = StringListFetch("Console:SaveTransferImportConfirm");
    if (ptMigrationData->export_platform() == 3)
    {
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_5<void, int32, SaveMigrationCallback, D3::Console::MigrationData *, BOOL, const UIID &>::bind<&sOnImportConfirmation>(nUserIndex, pfnCallback, ptMigrationData, TRUE);
        tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_binder_5<void, int32, SaveMigrationCallback, D3::Console::MigrationData *, BOOL, const UIID &>::bind<&sOnImportConfirmation>(nUserIndex, pfnCallback, ptMigrationData, FALSE);
    }
    else
    {
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_5<void, int32, SaveMigrationCallback, D3::Console::MigrationData *, BOOL, const UIID &>::bind<&sOnCrossBrandConfirmation>(nUserIndex, pfnCallback, ptMigrationData, TRUE);
        tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_binder_5<void, int32, SaveMigrationCallback, D3::Console::MigrationData *, BOOL, const UIID &>::bind<&sOnCrossBrandConfirmation>(nUserIndex, pfnCallback, ptMigrationData, FALSE);
    }

    SaveMigrationUpdateStatus(nUserIndex, SAVE_MIGRATION_STATUS_FINISHED);
    Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}




























void sOnCrossBrandConfirmation(int32 nUserIndex, SaveMigrationCallback pfnCallback, D3::Console::MigrationData * ptMigrationData, BOOL bConfirmed, const UIID &)
{
    s_bInProgress = FALSE;

    if (!bConfirmed)
    {
        pfnCallback(SAVE_MIGRATION_SUCCESS_SKIPPED);
        return;
    }

    if (!GamerProfile::IsSignedIn(nUserIndex))
    {
        pfnCallback(SAVE_MIGRATION_FAILED_NOT_SIGNED_IN);
        return;
    }

    // The save came from another brand of console; confirm once more before importing it.
    s_bInProgress = TRUE;

    NotificationCreationParams tParams;
    tParams.nControllerIndex = nUserIndex;
    tParams.uszMessage = StringListFetch("Console:SaveTransferImportOtherBrandConfirm");
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_5<void, int32, SaveMigrationCallback, D3::Console::MigrationData *, BOOL, const UIID &>::bind<&sOnImportConfirmation>(nUserIndex, pfnCallback, ptMigrationData, TRUE);
    tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_binder_5<void, int32, SaveMigrationCallback, D3::Console::MigrationData *, BOOL, const UIID &>::bind<&sOnImportConfirmation>(nUserIndex, pfnCallback, ptMigrationData, FALSE);

    SaveMigrationUpdateStatus(nUserIndex, SAVE_MIGRATION_STATUS_FINISHED);
    Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}








void sOnImportConfirmation(int32 nUserIndex, SaveMigrationCallback pfnCallback, D3::Console::MigrationData * ptMigrationData, BOOL bConfirmed, const UIID &)
{
    s_bInProgress = FALSE;

    if (!bConfirmed)
    {
        pfnCallback(SAVE_MIGRATION_SUCCESS_SKIPPED);
    }
    else if (!GamerProfile::IsSignedIn(nUserIndex))
    {
        pfnCallback(SAVE_MIGRATION_FAILED_NOT_SIGNED_IN);
    }
    else
    {
        SaveMigrationUpdateStatus(nUserIndex, SAVE_MIGRATION_STATUS_SAVING);
        HealMigrationData(nUserIndex, *ptMigrationData);

        // Achievements and the avenger are per-platform; start them fresh.
        ptMigrationData->mutable_account()->mutable_console_data()->mutable_achievement_snapshot()->Clear();
        ptMigrationData->mutable_account()->mutable_console_data()->clear_avenger_data();
        MergeLegacyLicenses(*ptMigrationData);

        if (GamerProfile::GetAccount(nUserIndex).console_data().has_bnet_account())
            ptMigrationData->mutable_account()->mutable_console_data()->set_has_bnet_account(true);

        s_bInProgress = TRUE;
        GamerProfile::ImportSave(nUserIndex, ptMigrationData, TRUE, OnlineService::internal::function_binder_3<void, int32, SaveMigrationCallback, GamerProfile::ImportSaveResult>::bind<&sOnImportSaveComplete>(nUserIndex, pfnCallback));
    }

    ::delete ptMigrationData;
}














void sOnImportSaveComplete(int32 nUserIndex, SaveMigrationCallback pfnCallback, GamerProfile::ImportSaveResult eImportResult)
{
    s_bInProgress = FALSE;
    Screens::GetModalNotificationManager().RemoveNotification(sidMigrationNotification);
    sidMigrationNotification = INVALID_MODAL_NOTIFICATION_ID;

    if (eImportResult != GamerProfile::IMPORT_SAVE_SUCCESS)
    {
        Result eMigrationResult = eImportResult == GamerProfile::IMPORT_SAVE_FAILED_CORRUPT ? SAVE_MIGRATION_FAILED_SAVE_CORRUPT : SAVE_MIGRATION_FAILED;
        pfnCallback(eMigrationResult);
        return;
    }

    pfnCallback(SAVE_MIGRATION_SUCCESS);
    sgtAppGlobals.ptAchievementsInterface->OnUserSignedIn(nUserIndex);
    PerformSeasonMigration(nUserIndex);

    if (Online::PartyIsInParty(nUserIndex))
    {
        Online::PartyUpdatePlayerHero(nUserIndex);
        if (Online::PartyIsHost() && GamerProfile::GetPrimaryProfileUserIndex() == nUserIndex && GameIsStartUp())
        {
            const D3::Account::SavedDefinition & tAccountDefinition = GamerProfile::GetAccount(nUserIndex);
            const D3::Hero::SavedDefinition & tHeroDefinition = GamerProfile::GetHero(nUserIndex);
            Online::GameParams tGameParams;
            Online::PartyGetGameParams(tGameParams);
            UIOnlineActions::SetGameParamsForHero(tAccountDefinition, tHeroDefinition, tGameParams);
            Online::PartySetGameParams(tGameParams);
        }
    }
}


























void SaveMigrationUpdateStatus(int32 nUserIndex, Status eStatus)
{
    const char * szStatus = "Console:SaveTransferExportReading";
    switch (eStatus)
    {
    case SAVE_MIGRATION_STATUS_LOADING:
        break;
    case SAVE_MIGRATION_STATUS_UPLOADING:
        szStatus = "Console:SaveTransferExportUploading";
        break;
    case SAVE_MIGRATION_STATUS_DOWNLOADING:
        szStatus = "Console:SaveTransferImportDownloading";
        break;
    case SAVE_MIGRATION_STATUS_SAVING:
        szStatus = "Console:SaveTransferImportWriting";
        break;
    case SAVE_MIGRATION_STATUS_FINISHED:
        Screens::GetModalNotificationManager().RemoveNotification(sidMigrationNotification);
        sidMigrationNotification = INVALID_MODAL_NOTIFICATION_ID;
        return;
    default:
        szStatus = "";
        break;
    }

    if (sidMigrationNotification == INVALID_MODAL_NOTIFICATION_ID)
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nUserIndex;
        tParams.uszMessage = StringListFetch(szStatus);
        sidMigrationNotification = Screens::GetModalNotificationManager().SetMessageOnlyNotification(tParams);
    }
    else
    {
        Screens::GetModalNotificationManager().ChangeMessage(sidMigrationNotification, StringListFetch(szStatus));
    }
}








BOOL IsMigrationRequired(const D3::Account::SavedDefinition & tDefinition)
{
    return tDefinition.version() < 115 || tDefinition.console_data().version_required() < 277;
}





extern "C" void sub_71000b0300(int32, D3::Console::MigrationData & tMigrationData)
{
    D3::Account::SavedDefinition & tAccount = *tMigrationData.mutable_account();
    for (uint32 i = tAccount.partitions_size(); i < ACCOUNT_PARTITION_COUNT; ++i)
    {
        D3::Account::AccountPartition * ptPartition = tAccount.add_partitions();
        ptPartition->set_partition_id(i);
        D3::AttributeSerializer::SavedAttribute * ptAttribute = ptPartition->mutable_saved_attributes()->add_attributes();
        ptAttribute->set_key(1);
        ptAttribute->set_value(0);
        *ptPartition->mutable_saved_attributes() = D3::AttributeSerializer::SavedAttributes::default_instance();
        ptPartition->mutable_saved_attributes()->mutable_gb_handle()->set_game_balance_type(D3::GameBalance::Handle::default_instance().game_balance_type());
        ptPartition->mutable_saved_attributes()->mutable_gb_handle()->set_gbid(D3::GameBalance::Handle::default_instance().gbid());
        ptPartition->clear_items();
        sub_71006b3690().Export(*ptPartition->mutable_crafter_data(), NULL);
    }
}

BOOL HealMigrationData(int32 nUserIndex, D3::Console::MigrationData & tMigrationData)
{
    BOOL bDidHeal = FALSE;

    if (tMigrationData.title_version() < 200)
    {
        _Migrate_Vanilla_RoS(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.account().version() == 108)
    {
        _Migrate_108_109(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.account().version() == 109)
    {
        _Migrate_109_110(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.account().version() == 110)
    {
        _Migrate_110_111(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.account().version() == 111)
    {
        _Migrate_111_112(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.account().version() == 112)
    {
        _Migrate_112_113(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.account().version() == 113)
    {
        _Migrate_113_114(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.account().version() == 114)
    {
        _Migrate_114_115(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.account().version() == 115)
    {
        _Migrate_115_116(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.title_version() < 212)
    {
        _Migrate_RoS_RoS212(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.title_version() < 221)
    {
        _Migrate_220_to_221(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.title_version() < 250)
    {
        _Migrate_221_to_243(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.title_version() < 251)
    {
        _Migrate_243_to_250a(tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.title_version() < 260)
    {
        _Migrate_250a_to_260(nUserIndex, tMigrationData);
        bDidHeal = TRUE;
    }

    if (tMigrationData.title_version() < 277)
    {
        sub_71000b0300(nUserIndex, tMigrationData);
        bDidHeal = TRUE;
    }

    return bDidHeal;
}














void _Migrate_Vanilla_RoS(
    D3::Console::MigrationData & tMigrationData)
{

    D3::Account::SavedDefinition & tAccount = *tMigrationData.mutable_account();

    tAccount.mutable_console_data()->set_version_required(277);

    tAccount.mutable_console_data()->mutable_achievement_snapshot()->Clear();


    D3::AccountMigration::ReadData_107 tReadData;
    D3::AccountMigration::WriteData_108 tWriteData;

    D3::AccountMigration::AccountReadData_107 & tAccountReadData = *tReadData.mutable_account_data();
    tAccountReadData.set_version(107);
    tAccountReadData.set_flags(tMigrationData.account().digest().flags());

    for (int32 nHero = 0; nHero < tMigrationData.heroes_size(); ++nHero)
    {
        const D3::Hero::SavedDefinition & tHero = tMigrationData.heroes(nHero);
        D3::AccountMigration::HeroReadData_107 & tHeroReadData = *tReadData.add_heroes_data();

        *tHeroReadData.mutable_hero_id() = tHero.digest().hero_id();
        tHeroReadData.set_level(tHero.digest().level());
        tHeroReadData.set_alt_level(tHero.digest().deprecated_alt_level());
        tHeroReadData.set_player_flags(tHero.digest().player_flags());
        *tHeroReadData.mutable_saved_attributes() = tHero.saved_attributes();
        SHero::DigesttoDigestUI(*tHeroReadData.mutable_digest_ui(), tHero.digest());
        tHeroReadData.mutable_quest_history()->MergeFrom(tHero.digest().quest_history());
        tHeroReadData.set_deleted(tHero.digest().has_delete_time());
    }

    OnlineService::AccountMigration::Migrate_ConsoleVanilla_108(tReadData, tWriteData);

    tAccount.mutable_digest()->set_flags(tWriteData.account_data().flags());

    uint32 uLegacyLicenseBits = 0;
    for (int32 nHero = 0; nHero < tMigrationData.heroes_size(); ++nHero)
    {
        D3::Hero::SavedDefinition & tHero = *tMigrationData.mutable_heroes(nHero);
        const D3::AccountMigration::HeroWriteData_108 & tHeroWriteData = tWriteData.heroes_data(nHero);


        SHero::DigestUItoDigest(*tHero.mutable_digest(), tHeroWriteData.digest_ui());
        tHero.mutable_digest()->clear_quest_history();
        tHero.mutable_digest()->mutable_quest_history()->MergeFrom(tHeroWriteData.quest_history());
        tHero.clear_quest_reward_history();

        if (tHero.digest().highest_unlocked_act() == ACT_5)
        {




            BOOL bHasCompletedFinalAct4Quest = FALSE;
            // Inference: the loop body leaves no code beyond mutable_digest(); nothing sets the flag in the binary.
            for (int32 nQuest = 0; nQuest < tHero.digest().quest_history_size(); ++nQuest)
            {
                D3::Hero::QuestHistoryEntry & tQuestHistory = *tHero.mutable_digest()->mutable_quest_history(nQuest);
            }





            // Inference: this test has no code of its own (the flag is constant FALSE); the add is unconditional in the binary.
            if (!bHasCompletedFinalAct4Quest)
            {
                D3::Hero::QuestHistoryEntry & tQuestHistory = *tHero.mutable_digest()->add_quest_history();
                tQuestHistory.set_sno_quest(114901);  // literal from the binary (0x1c0d5); no named SNO constant found
            }
        }


        if (tHero.has_saved_data())
        {
            SavedConversations tSavedConversations;
            tSavedConversations.Import(tHero.saved_data().saved_conversations());
            tSavedConversations.Export(*tHero.mutable_saved_data()->mutable_saved_conversations());
        }


        tHero.mutable_console_data()->set_last_played_handicap(0);

        uLegacyLicenseBits |= tHero.accepted_license_bits();


        int32 nHeroLevel = tHero.digest().level();

        D3::Items::ItemList & tItems = *tHero.mutable_items();
        for (int32 nItem = 0; nItem < tItems.items_size(); ++nItem)
        {
            D3::Items::SavedItem & tSavedItem = *tItems.mutable_items(nItem);

            InventorySlotType eSlot = ActorInventoryTranslateFromSavedSlot(tSavedItem.item_slot());
            if ((eSlot >= SLOT_PLAYER_BODYSLOT_START && eSlot < SLOT_PLAYER_BODYSLOT_END) ||
                (eSlot >= SLOT_HIRELING_START && eSlot < SLOT_HIRELING_END))
            {

                tSavedItem.mutable_generator()->set_console_max_level(nHeroLevel);
            }
            else
            {
                tSavedItem.mutable_generator()->set_console_max_level(60);
            }

            if(tSavedItem.mutable_generator()->has_item_binding_level() && tSavedItem.mutable_generator()->item_binding_level() != 0)
            {
                tSavedItem.mutable_generator()->set_console_promo_item(true);
            }
        }
    }

    for (int32 nPartition = 0; nPartition < tAccount.partitions_size(); ++nPartition)
    {
        D3::Account::AccountPartition & tPartition = *tAccount.mutable_partitions(nPartition);

        D3::Items::ItemList & tItems = *tPartition.mutable_items();
        for (int32 nItem = 0; nItem < tItems.items_size(); ++nItem)
        {
            D3::Items::SavedItem & tSavedItem = *tItems.mutable_items(nItem);

            tSavedItem.mutable_generator()->set_console_max_level(60);

            if(tSavedItem.mutable_generator()->has_item_binding_level() && tSavedItem.mutable_generator()->item_binding_level() != 0)
            {
                tSavedItem.mutable_generator()->set_console_promo_item(true);
            }
        }
    }




    const uint32 uLegacyLicenseBitsMask = 3;





    tMigrationData.mutable_account()->mutable_console_data()->set_legacy_license_bits(uLegacyLicenseBits & uLegacyLicenseBitsMask);
}



void _Migrate_108_109(
    D3::Console::MigrationData & tMigrationData)
{



    D3::AccountMigration::ReadData_108 tReadData;
    D3::AccountMigration::AccountReadData_108 & tAccountReadData = *tReadData.mutable_account_data();
    tAccountReadData.set_version(tMigrationData.account().version());
    *tAccountReadData.mutable_saved_attributes() = tMigrationData.account().saved_attributes();
    *tAccountReadData.mutable_saved_attributes_hardcore() = tMigrationData.account().deprecated_saved_attributes_hardcore();
    *tAccountReadData.mutable_deprecated_stash_icons_normal() = tMigrationData.account().deprecated_stash_icons_normal();
    *tAccountReadData.mutable_deprecated_stash_icons_hardcore() = tMigrationData.account().deprecated_stash_icons_hardcore();

    // Inference: ascending index loop and partition 0/1 branches follow the original compares and back edge.
    for (int32 i=0; i < tMigrationData.account().partitions_size(); i++)
    {
        const D3::Account::AccountPartition & tPartition = tMigrationData.account().partitions(i);
        if (tPartition.partition_id() == 0)
        {
            *tAccountReadData.mutable_partition_0_saved_attributes() = tPartition.saved_attributes();
        }
        else if (tPartition.partition_id() == 1)
        {
            *tAccountReadData.mutable_partition_1_saved_attributes() = tPartition.saved_attributes();
        }
    }
    // Inference: ascending hero loop follows the original increment and signed bound comparison.
    for (int32 i=0; i < tMigrationData.heroes_size(); i++)
    {
        const D3::Hero::SavedDefinition & tHeroSnapshot = tMigrationData.heroes(i);
        D3::AccountMigration::HeroReadData_108 & tHeroReadData = *tReadData.add_heroes_data();

        *tHeroReadData.mutable_hero_id() = tHeroSnapshot.digest().hero_id();
        tHeroReadData.set_hero_name(tHeroSnapshot.digest().hero_name());
        tHeroReadData.set_gbid_class(tHeroSnapshot.digest().gbid_class());
        tHeroReadData.set_level(tHeroSnapshot.digest().level());
        tHeroReadData.set_player_flags(tHeroSnapshot.digest().player_flags());
        tHeroReadData.mutable_visual_equipment()->MergeFrom(tHeroSnapshot.digest().visual_equipment());
    }

    D3::AccountMigration::WriteData_109 tWriteData;
    OnlineService::AccountMigration::Migrate_108_109(tReadData, tWriteData);


    tMigrationData.mutable_account()->set_version(tWriteData.account_data().version());

    // Inference: ascending partition loop and partition 0/1 branches follow the original compares and back edge.
    for (int32 i=0; i < tMigrationData.account().partitions_size(); i++)
    {
        D3::Account::AccountPartition & tPartition = *tMigrationData.mutable_account()->mutable_partitions(i);
        if (tPartition.partition_id() == 0)
        {
            *tPartition.mutable_stash_icons() = tWriteData.account_data().partition_0_stash_icons();
            *tPartition.mutable_saved_attributes() = tWriteData.account_data().partition_0_saved_attributes();
        }
        else if (tPartition.partition_id() == 1)
        {
            *tPartition.mutable_stash_icons() = tWriteData.account_data().partition_1_stash_icons();
            *tPartition.mutable_saved_attributes() = tWriteData.account_data().partition_1_saved_attributes();
        }
    }
    // Inference: append all generated minis in index order; original signed loop bound and MergeFrom call.
    D3::Profile::AccountProfile & profile = *tMigrationData.mutable_profile()->mutable_account_profile();
    for(int i=0; i < tWriteData.minis_size(); i++)
    {
        profile.add_heroes()->MergeFrom(tWriteData.minis(i));
    }


    tMigrationData.mutable_account()->clear_deprecated_stash_icons_normal();
    tMigrationData.mutable_account()->clear_deprecated_stash_icons_hardcore();
}



void _Migrate_109_110(D3::Console::MigrationData & tMigrationData)
{
    D3::AccountMigration::ReadData_109 tReadData;
    D3::AccountMigration::AccountReadData_109 * tAccountReadData = tReadData.mutable_account_data();
    tAccountReadData->set_version(tMigrationData.account().version());
    *tAccountReadData->mutable_saved_attributes() = tMigrationData.account().saved_attributes();
    *tAccountReadData->mutable_saved_attributes_hardcore() = tMigrationData.account().deprecated_saved_attributes_hardcore();

    D3::AccountMigration::WriteData_110 tWriteData;
    OnlineService::AccountMigration::Migrate_109_110(tReadData, tWriteData);

    tMigrationData.mutable_account()->set_version(tWriteData.account_data().version());
    *tMigrationData.mutable_account()->mutable_saved_attributes() = tWriteData.account_data().saved_attributes();
    if (tMigrationData.account().has_deprecated_saved_attributes_hardcore())
        *tMigrationData.mutable_account()->mutable_deprecated_saved_attributes_hardcore() = tWriteData.account_data().saved_attributes_hardcore();
}











void _Migrate_110_111(D3::Console::MigrationData & tMigrationData)
{
    D3::CS::Snapshot tSnapshot;
    *tSnapshot.mutable_definition() = tMigrationData.account();
    OnlineService::AccountMigration::Migrate_110_111(tSnapshot);
    *tMigrationData.mutable_account() = tSnapshot.definition();
}









void _Migrate_111_112(
    D3::Console::MigrationData & tMigrationData)
{


    D3::CS::Snapshot tSnapshot;
    *tSnapshot.mutable_definition() = tMigrationData.account();

    OnlineService::AccountMigration::Migrate_111_112(tSnapshot);



    // One achievement was re-issued under a new id; move old records over to it.

    D3::Achievements::Snapshot & tAchievements = *tSnapshot.mutable_definition()->mutable_console_data()->mutable_achievement_snapshot();

    BOOL bHasOldAchievement = FALSE;
    BOOL bHasNewAchievement = FALSE;

    const uint64 idOldAchievement = 0x443354fb3cdaULL;
    const uint64 idNewAchievement = 0x4433541dc4a1ULL;

    for (int32 nAchievement = 0; nAchievement < tAchievements.achievement_snapshot_size(); ++nAchievement)
    {
        D3::Achievements::AchievementUpdateRecord * ptRecord = tAchievements.mutable_achievement_snapshot(nAchievement);
        if (ptRecord->achievement_id() == idOldAchievement)
        {
            bHasOldAchievement = TRUE;
        }
        else if (ptRecord->achievement_id() == idNewAchievement)
        {
            bHasNewAchievement = TRUE;
        }
    }

    if (bHasOldAchievement)
    {
        D3::Achievements::Snapshot tAchvSnapshot;
        for (int32 nAchievement = 0; nAchievement < tAchievements.achievement_snapshot_size(); ++nAchievement)
        {
            D3::Achievements::AchievementUpdateRecord tRecord = tAchievements.achievement_snapshot(nAchievement);
            if (tRecord.achievement_id() == idOldAchievement)
            {
                if (bHasNewAchievement)
                    continue;

                tRecord.set_achievement_id(idNewAchievement);
            }

            *tAchvSnapshot.add_achievement_snapshot() = tRecord;
        }

        *tAchievements.mutable_achievement_snapshot() = tAchvSnapshot.achievement_snapshot();
    }

    *tMigrationData.mutable_account() = tSnapshot.definition();
}



void _Migrate_112_113(D3::Console::MigrationData & tMigrationData)
{
    D3::CS::Snapshot tSnapshot;
    *tSnapshot.mutable_definition() = tMigrationData.account();
    OnlineService::AccountMigration::Migrate_112_113(tSnapshot);
    *tMigrationData.mutable_account() = tSnapshot.definition();
}









void _Migrate_113_114(D3::Console::MigrationData & tMigrationData)
{
    D3::CS::Snapshot tSnapshot;
    *tSnapshot.mutable_definition() = tMigrationData.account();
    D3::CS::Snapshot tOldSnapshot;
    OnlineService::AccountMigration::Migrate_113_114(tOldSnapshot, tSnapshot);
    *tMigrationData.mutable_account() = tSnapshot.definition();
}










void _Migrate_114_115(D3::Console::MigrationData & tMigrationData)
{
    D3::AccountMigration::ReadData_114 tReadData;
    D3::AccountMigration::WriteData_115 tWriteData;
    *tReadData.mutable_account_data()->mutable_snapshot()->mutable_definition() = tMigrationData.account();
    OnlineService::AccountMigration::Migrate_114_115(tReadData, tWriteData);
    *tMigrationData.mutable_account() = tWriteData.account_data().snapshot().definition();
}










void _Migrate_115_116(D3::Console::MigrationData & tMigrationData)
{
    D3::CS::Snapshot tSnapshot;
    *tSnapshot.mutable_definition() = tMigrationData.account();
    OnlineService::AccountMigration::Migrate_115_116(tSnapshot);
    *tMigrationData.mutable_account() = tSnapshot.definition();
}









void _Migrate_220_to_221(D3::Console::MigrationData & tMigrationData)
{
    D3::Account::SavedDefinition * tAccount = tMigrationData.mutable_account();
    D3::CS::Snapshot tSnapshot;
    *tSnapshot.mutable_definition() = tMigrationData.account();

    // Platinum-earning time used to live in its own attribute; fold it into the solo rift record.
    for (int32 partitionId = 0; partitionId < tSnapshot.definition().partitions_size(); ++partitionId)
    {
        D3::Account::AccountPartition * tPartition = tSnapshot.mutable_definition()->mutable_partitions(partitionId);
        int32 nPlatinumGainTime = AccountDefinitionSavedAttributeGetInt(tPartition->saved_attributes(), FastAttribKey(ATTRIBUTE_PLATINUM_LAST_SIGNIFICANT_KILL), 0);
        if (nPlatinumGainTime > 0)
        {
            int32 nGreateRiftMax = AccountDefinitionSavedAttributeGetInt(tPartition->saved_attributes(), FastAttribKey(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL), 0);
            if (nGreateRiftMax <= 0)
                AccountDefinitionSavedAttributeWriteInt(*tPartition->mutable_saved_attributes(), FastAttribKey(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL), nPlatinumGainTime);
            AccountDefinitionSavedAttributeRemoveInt(*tPartition->mutable_saved_attributes(), FastAttribKey(ATTRIBUTE_PLATINUM_LAST_SIGNIFICANT_KILL));
        }
    }


    *tMigrationData.mutable_account() = tSnapshot.definition();
}











void _Migrate_221_to_243(D3::Console::MigrationData & tMigrationData)
{
    D3::Account::SavedDefinition & tAccount = *tMigrationData.mutable_account();

    D3::CS::Snapshot tSnapshot;
    *tSnapshot.mutable_definition() = tMigrationData.account();

    for (int32 i = 0; i < tMigrationData.heroes_size(); ++i)
    {
        D3::Hero::SavedDefinition & tHero = *tMigrationData.mutable_heroes(i);
        D3::Items::ItemList & tHeroItems = *tHero.mutable_items();
        D3::Items::ItemList tNewItemList;
        for (int32 nItem = 0; nItem < tHeroItems.items_size(); ++nItem)
        {
            BOOL bConverted = FALSE;
            D3::Items::SavedItem & tSavedItem = *tHeroItems.mutable_items(nItem);
            if (tSavedItem.has_generator() && tSavedItem.generator().has_gb_handle())
            {
                GBHandle hGB(tSavedItem.generator().gb_handle());
                GBID gbidItem = hGB.gbid;

                // Legacy cosmetic items become account-wide cosmetic unlocks.
                if (ItemIsCosmeticItem(gbidItem) && GBIDExists(gbidItem, GB_LEGACY_ITEM_CONVERSIONS))
                {
                    ScopedLegacyItemConversionLockR pdefLegacyItemConversion(gbidItem);
                    if (pdefLegacyItemConversion != NULL && pdefLegacyItemConversion->gbidOldItem == gbidItem)
                    {
                        tSnapshot.mutable_definition()->mutable_account_wide_cosmetic_item_data()->mutable_cosmetic_items()->add_elements(pdefLegacyItemConversion->gbidNewItem);
                        bConverted = TRUE;
                    }
                }
            }

            if (!bConverted)
            {

                D3::Items::SavedItem * ptNewSavedItem = tNewItemList.add_items();
                ptNewSavedItem->CopyFrom(tSavedItem);
            }
        }

        tHeroItems.clear_items();
        tHeroItems.CopyFrom(tNewItemList);
    }

    for (int32 partitionId = 0; partitionId < tSnapshot.definition().partitions_size(); ++partitionId)
    {
        D3::Account::AccountPartition & tPartition = *tSnapshot.mutable_definition()->mutable_partitions(partitionId);

        if (tPartition.has_items())
        {
            D3::Items::ItemList & tStashItems = *tPartition.mutable_items();
            D3::Items::ItemList tNewItemList;
            for (int32 i = 0; i < tPartition.items().items_size(); ++i)
            {
                BOOL bConverted = FALSE;
                D3::Items::SavedItem & tSavedItem = *tStashItems.mutable_items(i);
                if (tSavedItem.has_generator() && tSavedItem.generator().has_gb_handle())
                {
                    GBHandle hGB(tSavedItem.generator().gb_handle());
                    GBID gbidItem = hGB.gbid;

                    if (ItemIsCosmeticItem(gbidItem) && GBIDExists(gbidItem, GB_LEGACY_ITEM_CONVERSIONS))
                    {
                        ScopedLegacyItemConversionLockR pdefLegacyItemConversion(gbidItem);
                        if (pdefLegacyItemConversion != NULL && pdefLegacyItemConversion->gbidOldItem == gbidItem)
                        {
                            tSnapshot.mutable_definition()->mutable_account_wide_cosmetic_item_data()->mutable_cosmetic_items()->add_elements(pdefLegacyItemConversion->gbidNewItem);
                            bConverted = TRUE;
                        }
                    }
                }

                if (!bConverted)
                {

                    D3::Items::SavedItem * ptNewSavedItem = tNewItemList.add_items();
                    ptNewSavedItem->CopyFrom(tSavedItem);
                }
            }

            tStashItems.clear_items();
            tStashItems.CopyFrom(tNewItemList);
        }
    }

    *tMigrationData.mutable_account() = tSnapshot.definition();
}










void _Migrate_243_to_250a(D3::Console::MigrationData & tMigrationData)
{
    tMigrationData.mutable_account()->mutable_console_data()->set_version_required(277);

    // Seasons restarted with 2.5.0a; nothing carries over.
    for (D3::Hero::SavedDefinition & tHero : *tMigrationData.mutable_heroes())
        tHero.mutable_digest()->set_season_created(0);

    tMigrationData.mutable_account()->mutable_digest()->set_season_id(0);

    D3::Account::SavedDefinition tCleanDefinition;
    AccountDefinition_ToDefaults(tCleanDefinition);

    for (D3::Account::AccountPartition & tPartition : *tMigrationData.mutable_account()->mutable_partitions())
    {
        D3AccountPartition idPartition = (D3AccountPartition)tPartition.partition_id();
        if (idPartition >= ACCOUNT_PARTITION_SEASON_NORMAL && idPartition <= ACCOUNT_PARTITION_UNK_5)
        {
            tPartition.CopyFrom(tCleanDefinition.partitions(0));
            tPartition.set_partition_id(idPartition);
        }
    }

    OnlineService::AccountMigration::SeasonCleanSeasonalAchievements(*tMigrationData.mutable_account()->mutable_console_data()->mutable_achievement_snapshot());
}













void _Migrate_250a_to_260(int32 nUserIndex, D3::Console::MigrationData & tMigrationData)
{
    D3::Account::SavedDefinition * tAccount = tMigrationData.mutable_account();
    tAccount->mutable_console_data()->set_version_required(277);

    // Challenge rift progress is reset by this update.
    tMigrationData.mutable_account()->mutable_digest()->clear_challenge_rift_account_data();
    tMigrationData.mutable_account()->mutable_console_data()->clear_challenge_rift_reward();
    tMigrationData.mutable_account()->mutable_console_data()->clear_challenge_rift_personal_best_rift_number();
    tMigrationData.mutable_account()->mutable_console_data()->clear_challenge_rift_personal_best();

    D3::Account::SavedDefinition tDefaultDefinition;
    AccountDefinition_ToDefaults_ForConsoleClient(tDefaultDefinition, nUserIndex);
    uint32 nSeed = tDefaultDefinition.console_data().console_cube_base_seed();

    bool bCompletedSoloRift = false;
    for (const D3::Account::AccountPartition & tPartition : tMigrationData.account().partitions())
    {
        bCompletedSoloRift = bCompletedSoloRift || AccountDefinitionSavedAttributeGetInt(tPartition.saved_attributes(), FastAttribKey(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL), 0) > 0;
    }
    for (const D3::Hero::SavedDefinition & tHero : tMigrationData.heroes())
    {
        bCompletedSoloRift = bCompletedSoloRift || HeroDefinitionSavedAttributeGetInt(tHero.saved_attributes(), FastAttribKey(ATTRIBUTE_HIGHEST_HERO_SOLO_RIFT_LEVEL), 0) > 0;
        nSeed ^= XHashKey<uint64>(tHero.digest().hero_id().id_low());
    }

    tAccount->mutable_digest()->set_completed_solo_rift(bCompletedSoloRift);
    tAccount->mutable_console_data()->set_console_cube_base_seed(nSeed);
}










void _Migrate_RoS_RoS212(D3::Console::MigrationData & tMigrationData)
{
    D3::Account::SavedDefinition * tAccount = tMigrationData.mutable_account();
    tAccount->mutable_console_data()->set_version_required(277);

    // Two console items were corrected in 2.1.2; swap any old copies for the corrected ones.
    int32 arOldItems[2] = { StringToGBID("Unique_Shield_009_x1"), StringToGBID("Unique_Amulet_003_x1") };
    int32 arNewItems[2] = { StringToGBID("Unique_Shield_009_x1_ConsoleCorrection"), StringToGBID("Unique_Amulet_003_x1_ConsoleCorrection") };

    typedef google::protobuf::RepeatedPtrField<D3::Account::AccountPartition>::iterator PartitionIter;
    typedef google::protobuf::RepeatedPtrField<D3::Hero::SavedDefinition>::iterator HeroesIterator;
    typedef google::protobuf::RepeatedPtrField<D3::Items::SavedItem>::iterator ItemIterator;

    for (PartitionIter partitionIter = tAccount->mutable_partitions()->begin(), partitionEnd = tAccount->mutable_partitions()->end(); partitionIter != partitionEnd; ++partitionIter)
    {
        if (!partitionIter->has_items())
            continue;

        D3::Items::ItemList * tItemList = partitionIter->mutable_items();
        for (ItemIterator itemIter = tItemList->mutable_items()->begin(), itemEnd = tItemList->mutable_items()->end(); itemIter != itemEnd; ++itemIter)
        {
            if (!itemIter->has_generator())
                continue;

            for (int32 i = 0; i < 2; ++i)
            {
                if (arOldItems[i] == itemIter->generator().gb_handle().gbid())
                {
                    itemIter->mutable_generator()->mutable_gb_handle()->set_gbid(arNewItems[i]);
                    break;
                }
            }
        }
    }

    for (HeroesIterator heroIter = tMigrationData.mutable_heroes()->begin(), heroEnd = tMigrationData.mutable_heroes()->end(); heroIter != heroEnd; ++heroIter)
    {
        if (!heroIter->has_items())
            continue;

        D3::Items::ItemList * tItemList = heroIter->mutable_items();
        for (ItemIterator itemIter = tItemList->mutable_items()->begin(), itemEnd = tItemList->mutable_items()->end(); itemIter != itemEnd; ++itemIter)
        {
            if (!itemIter->has_generator())
                continue;

            for (int32 i = 0; i < 2; ++i)
            {
                if (arOldItems[i] == itemIter->generator().gb_handle().gbid())
                {
                    itemIter->mutable_generator()->mutable_gb_handle()->set_gbid(arNewItems[i]);
                    break;
                }
            }
        }
    }
}























void MergeLegacyLicenses(D3::Console::MigrationData & tMigrationData)
{
    D3::Account::SavedDefinition * tAccount = tMigrationData.mutable_account();

    uint32 uLegacyLicenseBits = tAccount->console_data().has_legacy_license_bits() ? tAccount->console_data().legacy_license_bits() : 0;
    if (tMigrationData.has_licenses())
        uLegacyLicenseBits |= tMigrationData.licenses();

    if (tMigrationData.export_platform() != 3)
        uLegacyLicenseBits = 0x20;

    tAccount->mutable_console_data()->set_legacy_license_bits(uLegacyLicenseBits);
}



























BOOL IsInProgress()
{
    return s_bInProgress;
}





void SetCrossPlatformSaveMigrationEnabled(BOOL bEnabled)
{
    s_bCrossPlatformSaveMigrationEnabled = bEnabled;
}






void PerformSeasonMigration()
{
    for (int32 i = 0; i < 4; ++i)
    {
        if (GamerProfile::IsActive(i) && PerformSeasonMigration(i) && Online::PartyIsInParty())
        {
            Online::PartyUpdatePlayerHero(i);
            if (GamerProfile::GetPrimaryProfileUserIndex() == i && Online::PartyIsHost())
            {
                // The host's hero may have changed partition; refresh the party's game params,
                // keeping any challenge rift setup.
                const D3::Account::SavedDefinition & tAccountDefinition = GamerProfile::GetAccount(i);
                const D3::Hero::SavedDefinition & tHeroDefinition = GamerProfile::GetHero(i);
                Online::GameParams tGameParams;
                Online::PartyGetGameParams(tGameParams);
                bool bIsChallengeRift = tGameParams.m_eGameType == GT_CHALLENGE_RIFTS;
                D3::ChallengeRifts::ChallengeData tChallengeConfig(tGameParams.m_tWeeklyChallengeConfig);
                D3::Leaderboard::WeeklyChallengeData tChallengeData(tGameParams.m_tWeeklyChallengeData);
                UIOnlineActions::SetGameParamsForHero(tAccountDefinition, tHeroDefinition, tGameParams);
                if (bIsChallengeRift)
                {
                    tGameParams.m_eGameType = GT_CHALLENGE_RIFTS;
                    tGameParams.m_tWeeklyChallengeConfig.CopyFrom(tChallengeConfig);
                    tGameParams.m_tWeeklyChallengeData.CopyFrom(tChallengeData);
                }
                Online::PartySetGameParams(tGameParams);
            }
        }
    }
}























bool PerformSeasonMigration(int32 nUserIdx)
{
    if (!GamerProfile::IsActive(nUserIdx))
        return false;

    D3::Account::SavedDefinition & tAccountDefinition = GamerProfile::GetAccount(nUserIdx);
    sCleanupExpiredSeasonalRolloverAndChallengeItems(tAccountDefinition);

    if (!Online::IsSeasonsInitialized())
        return false;

    uint32 uActiveSeasonNum = OnlineService::GetSeasonNum();
    if (OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ENDED)
        ++uActiveSeasonNum;

    if (tAccountDefinition.digest().season_id() >= uActiveSeasonNum)
        return false;

    // A new season: roll each seasonal partition into its non-seasonal counterpart and reset it.
    D3::Account::SavedDefinition tCleanDefinition;
    AccountDefinition_ToDefaults(tCleanDefinition);

    for (D3::Account::AccountPartition & tPartition : *tAccountDefinition.mutable_partitions())
    {
        D3AccountPartition idPartition = (D3AccountPartition)tPartition.partition_id();
        if (idPartition >= ACCOUNT_PARTITION_SEASON_NORMAL && idPartition <= ACCOUNT_PARTITION_UNK_5)
        {
            D3AccountPartition idNonSeasonalPartition = GetPartition(idPartition == ACCOUNT_PARTITION_HARDCORE || idPartition == ACCOUNT_PARTITION_SEASON_HARDCORE || idPartition == ACCOUNT_PARTITION_UNK_5, FALSE);
            D3::Account::AccountPartition * ptNonSeasonalPartition = NULL;
            for (D3::Account::AccountPartition & tNonSeasonalPartition : *tAccountDefinition.mutable_partitions())
            {
                if (tNonSeasonalPartition.partition_id() == (uint32)idNonSeasonalPartition)
                {
                    ptNonSeasonalPartition = &tNonSeasonalPartition;
                    break;
                }
            }

            OnlineService::AccountMigration::SeasonMigrationData tData(ptNonSeasonalPartition, &tPartition, "");
            OnlineService::AccountMigration::SeasonMigratePartition(tData);
            MigrateSavedItemsToSeasonRolloverCache(tPartition.items(), *ptNonSeasonalPartition);

            tPartition.CopyFrom(tCleanDefinition.partitions(0));
            tPartition.set_partition_id(idPartition);
        }
    }

    tAccountDefinition.mutable_console_data()->clear_leaderboard_heal_achievements();
    tAccountDefinition.mutable_console_data()->clear_leaderboard_heal_conquests();
    AccountDefinitionSavedAttributeRemoveInt(*tAccountDefinition.mutable_saved_attributes(), FastAttribKey(ATTRIBUTE_LAST_POSTED_SEASONAL_ACHIEVEMENT_POINTS));

    if (OnlineService::AccountMigration::SeasonCleanSeasonalAchievements(*tAccountDefinition.mutable_console_data()->mutable_achievement_snapshot()) &&
        sgtAppGlobals.ptAchievementsInterface != NULL)
    {
        sgtAppGlobals.ptAchievementsInterface->OnUserSignedIn(nUserIdx);
    }

    tAccountDefinition.mutable_digest()->set_season_id(uActiveSeasonNum);
    return true;
}


































void sCleanupExpiredSeasonalRolloverAndChallengeItems(D3::Account::SavedDefinition & tAccountDefinition)
{
    Blizzard::Time::UnixTime64 uNow = Blizzard::Time::ToUnixTime(Online::GetNetworkTimestamp());

    for (D3::Account::AccountPartition & tPartition : *tAccountDefinition.mutable_partitions())
    {
        bool bAnItemHasExpired = false;
        for (const D3::Account::ConsolePartitionData_SeasonalRolloverItem & tItem : tPartition.console_partition_data().seasonal_rollover_item())
        {
            if (uNow > tItem.create_time() && uNow - tItem.create_time() >= SEASONAL_ROLLOVER_ITEM_DURATION_SECONDS)
            {
                bAnItemHasExpired = true;
                break;
            }
        }

        if (bAnItemHasExpired)
        {
            // Keep only the items that are still within their rollover window.
            D3::Account::ConsolePartitionData tTempPartitionData;
            for (D3::Account::ConsolePartitionData_SeasonalRolloverItem & tItem : *tPartition.mutable_console_partition_data()->mutable_seasonal_rollover_item())
            {
                if (uNow < tItem.create_time() || uNow - tItem.create_time() < SEASONAL_ROLLOVER_ITEM_DURATION_SECONDS)
                    tTempPartitionData.add_seasonal_rollover_item()->Swap(&tItem);
            }
            tPartition.mutable_console_partition_data()->mutable_seasonal_rollover_item()->Swap(tTempPartitionData.mutable_seasonal_rollover_item());
        }
    }

    BOOL bNeedCleanup = FALSE;
    for (const D3::Account::ConsoleChallengeRiftReward & tReward : tAccountDefinition.console_data().challenge_rift_reward())
    {
        if (tReward.create_time() + CHALLENGE_REWARD_DURATION_SECONDS < uNow)
        {
            bNeedCleanup = TRUE;
            break;
        }
    }

    if (bNeedCleanup)
    {
        D3::Account::ConsoleData tTempConsoleData;
        for (D3::Account::ConsoleChallengeRiftReward tReward : tAccountDefinition.console_data().challenge_rift_reward())
        {
            if (uNow < tReward.create_time() || uNow - tReward.create_time() < CHALLENGE_REWARD_DURATION_SECONDS)
                tTempConsoleData.add_challenge_rift_reward()->Swap(&tReward);
        }
        tAccountDefinition.mutable_console_data()->mutable_challenge_rift_reward()->Swap(tTempConsoleData.mutable_challenge_rift_reward());
    }
}

























void MigrateSavedItemsToSeasonRolloverCache(const D3::Items::ItemList & tItemList, D3::Account::AccountPartition & tDestinationPartition)
{
    Blizzard::Time::UnixTime uNow = Blizzard::Time::ToUnixTime(Online::GetNetworkTimestamp());

    // Rollover items get fresh ids, continuing on from the last item already in the cache.
    uint32 uNumExistingRolloverItems = tDestinationPartition.console_partition_data().seasonal_rollover_item_size();
    uint64 uTopId = 0;
    if (uNumExistingRolloverItems != 0)
        uTopId = tDestinationPartition.console_partition_data().seasonal_rollover_item(uNumExistingRolloverItems - 1).item().id().id_low();

    for (const D3::Items::SavedItem & tItem : tItemList.items())
    {
        const GBID gbidItem = tItem.generator().gb_handle().gbid();
        if (ItemHasLabel(gbidItem, 46) || sub_710050e800(gbidItem) || sub_7100501ab0(tItem.generator()))
            continue;

        D3::Account::ConsolePartitionData_SeasonalRolloverItem * tRolloverItem = tDestinationPartition.mutable_console_partition_data()->add_seasonal_rollover_item();
        tRolloverItem->mutable_item()->CopyFrom(tItem);
        tRolloverItem->set_create_time(uNow);
        tRolloverItem->mutable_item()->mutable_id()->set_id_high(0);
        tRolloverItem->mutable_item()->mutable_id()->set_id_low(++uTopId);
    }
}













void RebirthHero(int32 nUserIdx, uint32 uRebirthIntoSeason, BOOL bUnk)
{
    if (!GamerProfile::IsAccountLoaded(nUserIdx))
        return;
    if (!GamerProfile::IsHeroLoaded(nUserIdx))
        return;






    D3::Account::SavedDefinition & tAccount = GamerProfile::GetAccount(nUserIdx);
    D3::Hero::SavedDefinition & tHero = GamerProfile::GetHero(nUserIdx);

    uint32 dwPlayerFlags = tHero.digest().player_flags();






    D3::Account::AccountPartition * ptPartition = (dwPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) ? tAccount.mutable_partitions(ACCOUNT_PARTITION_HARDCORE) : tAccount.mutable_partitions(ACCOUNT_PARTITION_NORMAL);




    MigrateSavedItemsToSeasonRolloverCache(tHero.items(), *ptPartition);

    D3::Hero::SavedDefinition tRebirthedHeroDefinition;
    HeroDefinition_Init(tRebirthedHeroDefinition);



    dwPlayerFlags = tHero.digest().player_flags() & ((1 << PLAYERFLAG_HARDCORE) | (1 << PLAYERFLAG_FEMALE));
    uint32 dwPlayerFlags2 = bUnk ? 1 : 0;


    D3::Hero::Digest & tDigest = *tRebirthedHeroDefinition.mutable_digest();
    *tDigest.mutable_hero_id() = tHero.digest().hero_id();
    tDigest.set_hero_name(tHero.digest().hero_name());
    tDigest.set_gbid_class(tHero.digest().gbid_class());
    tDigest.set_player_flags(dwPlayerFlags);
    tDigest.set_player_flags2(dwPlayerFlags2);
    tDigest.set_create_time(tHero.digest().create_time());
    tDigest.set_season_created(uRebirthIntoSeason);

    D3::Hero::SavedData & tSavedData = *tRebirthedHeroDefinition.mutable_saved_data();
    tSavedData.set_activated_waypoints(0);
    tSavedData.mutable_hireling_saved_data()->set_active_hireling(0);
    tSavedData.mutable_hireling_saved_data()->set_available_hirelings_bitfield(0);
    tSavedData.set_last_level_time(0);
    tSavedData.mutable_learned_lore();
    tSavedData.mutable_saved_conversations()->set_played_conversations_bitfield("");
    tSavedData.mutable_save_point()->set_sno_world(-1);
    tSavedData.mutable_save_point()->set_savepoint_number(-1);
    tSavedData.mutable_save_point()->set_creates_portal(false);
    tSavedData.set_gbid_potion_button(-1);
    tSavedData.set_skill_slot_ever_assigned("");
    tSavedData.set_skill_version(0);
    tSavedData.set_time_played(tHero.saved_data().time_played());


    tRebirthedHeroDefinition.mutable_digest()->set_time_played(tHero.digest().time_played());

    *tRebirthedHeroDefinition.mutable_saved_attributes()->mutable_gb_handle() = tHero.saved_attributes().gb_handle();


    tHero = tRebirthedHeroDefinition;


    GamerProfile::Save(nUserIdx, GamerProfile::SaveCallback());
    Online::PartyUpdatePlayerHero(nUserIdx);
}





}  // namespace SaveMigration
}  // namespace Console
