// X/Client/Console/GamerProfileNX64.cpp
// Switch (NX64) gamer profiles: the signed-in users, their account/hero/profile save data and the
// save/load tasks that run on the GamerProfileInternal task thread.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"
#include "../Gfx/GfxVB.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../BlizzardCore/Include/Blizzard/File.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../NintendoSDK/Include/nn/account/account_Types.h"
#include "../../../NintendoSDK/Include/nn/friends/friends_Types.h"
#include "../../../NintendoSDK/Include/nn/nn_Result.h"
#include "GamerProfile.h"
#include "Online.h"
#include "PresenceManager.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../Common/Protobuf/Account.pb.h"
#include "../../Common/Protobuf/Console.pb.h"
#include "../../Common/Protobuf/Hero.pb.h"
#include "../../Common/Protobuf/Profile.pb.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Common/VisualInventory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Files/XFiles.h"
#include <stdlib.h>
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/blzext/fixed_vector.h"
#include "../../Sigma/Random.h"
#include "../../Common/Player.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../BlizzardCore/Include/Blizzard/Util/Crypto.h"
#include "../Definitions/PreferencesDefinition.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_minmax.h"
#include "../../../NintendoSDK/Include/nn/oe/oe_NotificationMessage.h"
#include <string.h>

// Nintendo SDK entry points (nn/os/os_ReaderWriterLock.h, nn/account/account_Api*.h,
// nn/friends/friends_Api.h, nn/pctl/pctl_Api.h); the SDK headers are not in src/.
namespace nn { namespace os {
struct ReaderWriterLockType { uint8 _storage[0x30]; };  // nn/os/os_ReaderWriterLockTypes.h, size 0x30
void InitializeReaderWriterLock(ReaderWriterLockType * pLock);
void FinalizeReaderWriterLock(ReaderWriterLockType * pLock);
void AcquireReadLock(ReaderWriterLockType * pLock);
void ReleaseReadLock(ReaderWriterLockType * pLock);
void AcquireWriteLock(ReaderWriterLockType * pLock);
void ReleaseWriteLock(ReaderWriterLockType * pLock);
} }
namespace nn { namespace account {
void Initialize();
void CloseUser(const UserHandle & handle);
nn::Result IsNetworkServiceAccountAvailable(bool * pOut, const UserHandle & handle);
nn::Result EnsureNetworkServiceAccountAvailable(const UserHandle & handle);
nn::Result GetNetworkServiceAccountId(NetworkServiceAccountId * pOut, const UserHandle & handle);
} }
namespace nn { namespace friends {
nn::Result GetPlayHistoryRegistrationKey(PlayHistoryRegistrationKey * pOut, const nn::account::Uid & uid, bool bLocalCommunication);
// nn/friends/friends_PlayHistory.h
nn::Result AddPlayHistory(const nn::account::Uid & uid, const PlayHistoryRegistrationKey & key, const InAppScreenName & inAppScreenName, const InAppScreenName & myInAppScreenName);
} }
namespace nn { namespace oe {
// nn/oe/oe_LanguageApi.h
nn::settings::LanguageCode GetDesiredLanguage();
} }
namespace nn { namespace fs {
nn::Result MountSaveData(const char * name, const nn::account::Uid & user);
void Unmount(const char * name);
nn::Result Commit(const char * name);
nn::Result EnsureSaveData(const nn::account::Uid & user);
} }
namespace nn { namespace pctl {
bool CheckFreeCommunicationPermission(bool bNotify);
} }

extern nn::account::UserHandle unk_71019079f0;  // Preselected handle opened by FileSystemGetLocale.

// X/Sigma/Files/XFiles.h (XFilesCore.cpp); prototypes from the mangled names.
void FileReferenceInit(FileReference & tRef, const char * szPath);
const char * FileReferenceGetPath(const FileReference & tRef);
void FileReferenceGetNameNoExtension(FileReference & tRef, char * szName);
void FileReferenceGetExtension(FileReference & tRef, char * szExtension);
BOOL FileFindStart(void *& ptFindFileState, const FileReference & tRef, const char * szPattern, int nFlags, int nDepth);
BOOL FileFindNext(void * ptFindFileState, FileReference & tResult);
void FileFindClose(void *& ptFindFileState);
namespace Blizzard { namespace File { bool IsFile(const char * name); } }
// BlizzardCore/Include/Blizzard/Process.h (not in src/ yet).
namespace Blizzard { namespace Process { bool Sleep(Blizzard::Time::Millisecond milliseconds); } }
void ConfigureUpdatePrivilegesThread(Blizzard::Thread::Thread tThread);  // X/Common/Shell/MainNX64.cpp:195
void AccountDefinition_ToDefaults_ForConsoleClient(D3::Account::SavedDefinition & tDefinition, int32 nUserIdx);  // X/Common/PlayerAccount.h

typedef int32 MonsterRarity;

// GamerProfile.h / GamerProfileInternal.h types. Other files that include GamerProfile.h still
// declare some of these locally, so they live here until those copies are folded into the header.
namespace Console {
namespace GamerProfile {
struct SaveTask;
class ScopedUsersReadLock {  // size 0x1
public:
    ScopedUsersReadLock();
    ~ScopedUsersReadLock();
};
class ScopedUsersWriteLock {  // size 0x1
public:
    ScopedUsersWriteLock();
    ~ScopedUsersWriteLock();
};
enum SignInResult {
    SIGNIN_CANCELLED = 0,
    SIGNIN_FAILED = 1,
    SIGNIN_FAILED_PROFILE_IN_USE = 2,
    SIGNIN_FAILED_GUEST_NOT_ALLOWED = 3,
    SIGNIN_FAILED_NO_ONLINE_PRIVILEGE = 4,
    SIGNIN_FAILED_NO_SPACE = 5,
    SIGNIN_SUCCESS = 6,
    SIGNIN_SUCCESS_NO_PROFILE = 7,
    SIGNIN_SUCCESS_NO_DEVICE = 8,
    SIGNIN_SUCCESS_GUEST = 9,
    SIGNIN_SUCCESS_UNOWNED_SAVE = 10,
    SIGNIN_SUCCESS_CORRUPT_DATA = 11,
    SIGNIN_SUCCESS_DEMO_SAVE_IMPORTED = 12,
    SIGNIN_SUCCESS_NO_SAVE = 13,
};
typedef OnlineService::Callback<void (Console::GamerProfile::SignInResult)> SignInCallback;
enum SignInMode {
    SIGNIN_MODE_ANY = 0,
    SIGNIN_MODE_GUEST_ALLOWED = 1,
    SIGNIN_MODE_ONLINE_ONLY = 2,
    SIGNIN_MODE_VIA_INVITE = 3,
};































struct HeroInfo {  // size 0x158
    HeroInfo();
    void Import(const D3::Hero::SavedDefinition & tDefinition);
    void Import(const D3::Console::HeroInfo & tHeroInfo);
    void Export(D3::Console::HeroInfo & tHeroInfo) const;
    OnlineService::HeroId uHeroId;  // 0x00
    Blizzard::Time::UnixTime uCreationTime;  // 0x08
    char szFilename[32];  // 0x0c
    char szName[49];  // 0x2c
    PlayerClass eClass;  // 0x60
    uint32 uLevel;  // 0x64
    uint32 uAltLevel;  // 0x68
    uint32 uPlayerFlags;  // 0x6c
    uint32 unk_70;  // 0x70
    Act eLastAct;  // 0x74
    Blizzard::Time::UnixTime uDeathTime;  // 0x78
    SNO snoKiller;  // 0x7c
    MonsterRarity eKillerRarity;  // 0x80
    GBID arKillerRareNameGBIDs[2];  // 0x84
    CRefString usKillerName;  // 0x90
    uint64 uMonstersKilled;  // 0xa8
    uint64 uElitesKilled;  // 0xb0
    uint64 uGoldCollected;  // 0xb8
    uint32 uTimePlayed;  // 0xc0
    uint32 uSeasonCreated;  // 0xc4
    VisualEquipment tEquipment;  // 0xc8
};
static_assert(sizeof(HeroInfo) == 0x158, "HeroInfo");
static_assert(__builtin_offsetof(HeroInfo, unk_70) == 0x70, "HeroInfo new word");
static_assert(__builtin_offsetof(HeroInfo, eLastAct) == 0x74, "HeroInfo act");
static_assert(__builtin_offsetof(HeroInfo, usKillerName) == 0x90, "HeroInfo killer name");
static_assert(__builtin_offsetof(HeroInfo, tEquipment) == 0xc8, "HeroInfo equipment");
typedef OnlineService::Callback<void (unsigned int)> FindHeroesCallback;
enum LoadHeroResult {
    LOAD_HERO_FAILED = 0,
    LOAD_HERO_FAILED_HERO_IN_USE = 1,
    LOAD_HERO_FAILED_NO_HEROES = 2,
    LOAD_HERO_FAILED_CORRUPT = 3,
    LOAD_HERO_FAILED_CONTENT_MISSING = 4,
    LOAD_HERO_SUCCESS = 5,
};
typedef OnlineService::Callback<void (Console::GamerProfile::LoadHeroResult)> LoadHeroCallback;
enum DeleteHeroResult {
    DELETE_HERO_FAILED = 0,
    DELETE_HERO_FAILED_NOT_FOUND = 1,
    DELETE_HERO_FAILED_CONTENT_MISSING = 2,
    DELETE_HERO_FAILED_NOT_OWNER = 3,
    DELETE_HERO_FAILED_IN_USE = 4,
    DELETE_HERO_SUCCESS = 5,
};
typedef OnlineService::Callback<void (Console::GamerProfile::DeleteHeroResult)> DeleteHeroCallback;
enum ExportSaveResult {
    EXPORT_SAVE_FAILED = 0,
    EXPORT_SAVE_FAILED_NOT_FOUND = 1,
    EXPORT_SAVE_FAILED_NOT_OWNER = 2,
    EXPORT_SAVE_FAILED_CORRUPT = 3,
    EXPORT_SAVE_SUCCESS = 4,
};
typedef OnlineService::Callback<void (Console::GamerProfile::ExportSaveResult)> ExportSaveCallback;
enum ImportSaveResult {
    IMPORT_SAVE_FAILED = 0,
    IMPORT_SAVE_FAILED_NO_STORAGE = 1,
    IMPORT_SAVE_FAILED_CORRUPT = 2,
    IMPORT_SAVE_SUCCESS = 3,
};
typedef OnlineService::Callback<void (Console::GamerProfile::ImportSaveResult)> ImportSaveCallback;

void CreateRandomHero(int32 nUserIdx, PlayerClass eRequiredClass);  // GamerProfileInternal.cpp:215

namespace Internal {
struct SaveLoadTask : public Blizzard::Lock::RefCounted {  // size 0x10
    SaveLoadTask() {}
    virtual ~SaveLoadTask() {}
    virtual void Process() = 0;  // vtable slot 2
    void Queue();
    virtual Console::GamerProfile::SaveTask * CastAsSaveTask() { return NULL; }  // vtable slot 3
    virtual BOOL IsCritical() const { return FALSE; }  // vtable slot 4
private:
    SaveLoadTask(const Console::GamerProfile::Internal::SaveLoadTask &);
    Console::GamerProfile::Internal::SaveLoadTask & operator=(const Console::GamerProfile::Internal::SaveLoadTask &);
};
typedef OnlineService::Callback<int (Console::GamerProfile::Internal::SaveLoadTask *)> TaskCompareFunc;
typedef OnlineService::Callback<void ()> SaveLoadTaskCallback;
int32 GetPrimaryProfileUserIndex();  // GamerProfileInternal.cpp:515
void SetPrimaryProfileUserIndex(int32 nUserIdx);  // GamerProfileInternal.cpp:524
void ClearUserNewChallengeNotifications(int32 nUserIdx);  // GamerProfileInternal.cpp:834
void QueueCallback(Console::GamerProfile::Internal::SaveLoadTaskCallback pfnCallback);  // GamerProfileInternal.cpp:493
void RemoveTasks(Console::GamerProfile::Internal::TaskCompareFunc pfnCompare);  // GamerProfileInternal.cpp:697
void NotifyBeginSaveCallback();  // GamerProfileInternal.cpp:772
void NotifyEndSaveCallback();  // GamerProfileInternal.cpp:787
}  // namespace Internal
}  // namespace GamerProfile
}  // namespace Console

enum {
    PRIVILEGE_REFRESH_INTERVAL = 1000,
};

namespace Console {
namespace GamerProfile {
struct UserRuntime {  // size 0x1e98
    UserRuntime();
    void Clear();
    nn::account::Uid m_idUser;  // 0x00
    nn::account::UserHandle m_tUserHandle;  // 0x10
    nn::account::Nickname m_tNickname;  // 0x28
    nn::friends::PlayHistoryRegistrationKey m_tPlayHistoryKey;  // 0x49
    blzext::fixed_vector<Console::GamerProfile::PlayerHistoryRecord, 4> m_atPlayersRegistered;  // 0x90
    BOOL m_bActive;  // 0x220
    BOOL m_bAccountLoadComplete;  // 0x224
    BOOL m_bGuest;  // 0x228
    int32 nGuestIndex;  // 0x22c
    BOOL m_bEnsureAccountAvailability;  // 0x230
    BOOL m_bAccountTokenIsDirty;  // 0x234
    volatile BOOL m_bNetworkServiceAccountAvailable;  // 0x238
    BOOL m_bHasNSOSubscription;  // 0x23c
    D3::Account::SavedDefinition m_tAccountDefinition;  // 0x240
    D3::Hero::SavedDefinition m_tHeroDefinition;  // 0x338
    D3::Console::Profile m_tProfile;  // 0x3c0
    int32 m_uActiveBlizzconIndex;  // 0x3f8
    D3::Hero::SavedDefinition m_tBlizzconHeroDefinitionCache[7];  // 0x400
    uint32 m_uNumHeroes;  // 0x7b8
    Console::GamerProfile::HeroInfo m_atHeroes[17];  // 0x7c0
};
static_assert(sizeof(UserRuntime) == 0x1e98, "UserRuntime");
static_assert(__builtin_offsetof(UserRuntime, m_atHeroes) == 0x7c0, "UserRuntime heroes");


struct LoadAccountTask : public Console::GamerProfile::Internal::SaveLoadTask {  // size 0x20
    LoadAccountTask(int32, Console::GamerProfile::SignInMode, Console::GamerProfile::SignInCallback);
    virtual void Process();  // vtable slot 2
    void HandleResult(Console::GamerProfile::SignInResult eResult);
    int32 m_nUserIdx;  // 0x0c
    Console::GamerProfile::SignInMode m_eSignInMode;  // 0x10
    Console::GamerProfile::SignInCallback m_pfnCallback;  // 0x18
};

struct SaveTask : public Console::GamerProfile::Internal::SaveLoadTask {  // size 0x14170
    SaveTask(int32, nn::account::Uid, Console::GamerProfile::SaveCallback);
    virtual ~SaveTask();  // vtable slot 0
    virtual void Process();  // vtable slot 2
    virtual Console::GamerProfile::SaveTask * CastAsSaveTask();  // vtable slot 3
    void * SaveFile(const char * szFilename, void * pData, int32 nDataSize);
    BOOL SaveFile(const char * szFilename, const google::protobuf::MessageLite & tMessage);
    int32 m_nUserIdx;  // 0x0c
    nn::account::Uid m_idUser;  // 0x10
    Console::GamerProfile::SaveCallback m_pfnCallback;  // 0x20
    struct File {  // size 0x1010
        File();
        ~File();
        char szFilename[4096];  // 0x00
        void * pBuffer;  // 0x1000
        int32 nSize;  // 0x1008
    };
    Console::GamerProfile::SaveTask::File m_atFiles[20];  // 0x28
    uint32 m_nNumFiles;  // 0x14168
};

struct SavePreferencesTask : public Console::GamerProfile::SaveTask {  // size 0x14170
    SavePreferencesTask(int32, nn::account::Uid, Console::GamerProfile::SaveCallback);
    virtual Console::GamerProfile::SaveTask * CastAsSaveTask();  // vtable slot 3
};

struct LoadHeroTask : public Console::GamerProfile::Internal::SaveLoadTask {  // size 0x48
    LoadHeroTask(int32 nUserIdx, nn::account::Uid idUser, const char * szFilename, Console::GamerProfile::LoadHeroCallback pfnCallback) : m_nUserIdx(nUserIdx), m_idUser(idUser), m_pfnCallback(pfnCallback) { Blizzard::String::Copy(m_szHeroFilename, szFilename, 32); }
    virtual void Process();  // vtable slot 2
    int32 m_nUserIdx;  // 0x0c
    nn::account::Uid m_idUser;  // 0x10
    Console::GamerProfile::LoadHeroCallback m_pfnCallback;  // 0x20
    char m_szHeroFilename[32];  // 0x28
};

struct DeleteHeroTask : public Console::GamerProfile::Internal::SaveLoadTask {  // size 0x48
    DeleteHeroTask(int32, nn::account::Uid, const char *, Console::GamerProfile::DeleteHeroCallback);
    virtual ~DeleteHeroTask();  // vtable slot 0
    virtual void Process();  // vtable slot 2
    int32 m_nUserIdx;  // 0x0c
    nn::account::Uid m_idUser;  // 0x10
    Console::GamerProfile::DeleteHeroCallback m_pfnCallback;  // 0x20
    char m_szHeroFilename[32];  // 0x28
};

struct ExportSaveTask : public Console::GamerProfile::Internal::SaveLoadTask {  // size 0x20
    ExportSaveTask(int32, D3::Console::MigrationData *, Console::GamerProfile::ExportSaveCallback);
    virtual void Process();  // vtable slot 2
    int32 m_nUserIdx;  // 0x0c
    D3::Console::MigrationData * m_ptMigrationData;  // 0x10
    Console::GamerProfile::ExportSaveCallback m_pfnCallback;  // 0x18
};

class MountSaveDataScope {  // size 0x10
public:
    MountSaveDataScope(const char *, const nn::account::Uid &);
    ~MountSaveDataScope();
    void Commit();
    BOOL IsSuccess() const { return m_tResult.IsSuccess(); }
    BOOL IsFailure() const { return m_tResult.IsFailure(); }
private:
    const char * m_szName;  // 0x00
    nn::Result m_tResult;  // 0x08
};

void UpdatePrivileges(BOOL bIgnoreEnsures);
void SetHero(int32 nUserIdx, const D3::Hero::SavedDefinition & tSavedDefinition);
BOOL IsNetworkServiceAccountAvailable(int32 nUserIdx);
BOOL IsNSOSubscribed(int32 nUserIdx);

}  // namespace GamerProfile
}  // namespace Console

static const int32 MAX_PREFERENCES_SIZE = 16384;
static const uint64 suEncryptionKey = 0x305f92d82ec9a01bULL;

BOOL sbIsFirstSignIn = TRUE;
const char * sszAccountFilename = "account.dat";
const char * sszPreferencesFilename = "prefs.dat";
const char * sszProfileFilename = "profile.dat";
const char * sszHeroDirectory = "heroes";

Console::GamerProfile::UserRuntime s_atRuntime[4];
BOOL s_atGuestIndices[3];

volatile BOOL sbParentalFreeCommunication = TRUE;
Blizzard::Thread::Thread stUpdatePrivilegeThread;
nn::os::ReaderWriterLockType stUserListLock;
volatile BOOL sbRunUpdatePrivilegeThread = TRUE;
Blizzard::Time::Millisecond suLastUpdatePrivilegeAttempt;
BOOL sbEnsureAvailabilityDirty;

static uint sAsyncUpdatePrivilegeThread(void * pParam);
static void sMakeHeroFileName(OnlineService::HeroId idHero, char * szFilename);
static int32 sSortHeroInfo(const void * pLeft, const void * pRight);
static BOOL sSaveTaskComparison(nn::account::Uid idUser, Console::GamerProfile::Internal::SaveLoadTask * ptTask);
static void sMakeMountName(int nUserIdx, char * szName);
static void sMakeMountedPath(int nUserIdx, const char * szFilename, char * szPath);
static void sMakeHeroFilePath(const char * szHeroFile, char * szPath);
static BOOL sReadAndParseDefinitionBuffer(const char * szFilePath, google::protobuf::MessageLite & tMessage);

namespace Console {
namespace GamerProfile {
inline DeleteHeroTask::DeleteHeroTask(int32 nUserIdx, nn::account::Uid idUser, const char * szFilename, DeleteHeroCallback pfnCallback)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    : m_nUserIdx(nUserIdx),
      m_idUser(idUser),
      m_pfnCallback(pfnCallback)
{
    nn::oe::EnterExitRequestHandlingSection();

    Blizzard::String::Copy(m_szHeroFilename, szFilename, 32);

    Internal::NotifyBeginSaveCallback();
}
}  // namespace GamerProfile
}  // namespace Console
namespace Console {
namespace GamerProfile {
DeleteHeroTask::~DeleteHeroTask()
{
    Internal::NotifyEndSaveCallback();
    nn::oe::LeaveExitRequestHandlingSection();
}
}  // namespace GamerProfile
}  // namespace Console
namespace Console {
namespace GamerProfile {
inline ExportSaveTask::ExportSaveTask(int32 nUserIdx, D3::Console::MigrationData * ptMigrationData, ExportSaveCallback pfnCallback)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    : m_nUserIdx(nUserIdx),
      m_ptMigrationData(ptMigrationData),
      m_pfnCallback(pfnCallback)
{
}
}  // namespace GamerProfile
}  // namespace Console
namespace Console {
namespace GamerProfile {
inline MountSaveDataScope::MountSaveDataScope(const char * szName, const nn::account::Uid & tUserId)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    : m_szName(szName),
      m_tResult(nn::fs::MountSaveData(szName, tUserId))
{
}

inline MountSaveDataScope::~MountSaveDataScope()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    if (m_tResult.IsSuccess())
    {
        nn::fs::Unmount(m_szName);
    }
}

inline void MountSaveDataScope::Commit()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    nn::fs::Commit(m_szName);
}
}  // namespace GamerProfile
}  // namespace Console
namespace Console {
namespace GamerProfile {
ScopedUsersReadLock::ScopedUsersReadLock()
{
    nn::os::AcquireReadLock(&stUserListLock);
}

ScopedUsersReadLock::~ScopedUsersReadLock()
{
    nn::os::ReleaseReadLock(&stUserListLock);
}

ScopedUsersWriteLock::ScopedUsersWriteLock()
{
    nn::os::AcquireWriteLock(&stUserListLock);
}

ScopedUsersWriteLock::~ScopedUsersWriteLock()
{
    nn::os::ReleaseWriteLock(&stUserListLock);
}

void PlatformInitializeForApp()
{
    nn::account::Initialize();

    nn::os::InitializeReaderWriterLock(&stUserListLock);
    UpdatePrivileges(TRUE);

    BOOL bPrivilegeThreadCreated = Blizzard::Thread::Create(sAsyncUpdatePrivilegeThread, NULL, &stUpdatePrivilegeThread, "UpdatePrivileges", 1, 0x10000);
    (void)bPrivilegeThreadCreated;

    ConfigureUpdatePrivilegesThread(stUpdatePrivilegeThread);

    sbRunUpdatePrivilegeThread = TRUE;
    Blizzard::Thread::Resume(stUpdatePrivilegeThread);

    SigmaMemoryClear(s_atGuestIndices, sizeof(s_atGuestIndices));
}
void PlatformDisposeForApp()
{
    sbRunUpdatePrivilegeThread = FALSE;
    Blizzard::Thread::WaitForever(stUpdatePrivilegeThread);
    Blizzard::Thread::Close(stUpdatePrivilegeThread);
    nn::os::FinalizeReaderWriterLock(&stUserListLock);
}

































































































































void SignOut(int32 nUserIdx)
{
    if (nUserIdx < 0 || nUserIdx >= 4)
        return;

    ScopedUsersWriteLock tScopedLock;

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];

    if (tRuntime.m_idUser != nn::account::InvalidUid)
    {
        nn::account::CloseUser(tRuntime.m_tUserHandle);
        Online::PresenceManager::Get(nUserIdx)->OnNX64UserSignedOut();
    }

    if (tRuntime.m_bGuest)
        s_atGuestIndices[tRuntime.nGuestIndex] = FALSE;

    tRuntime.Clear();

    if (GetPrimaryProfileUserIndex() == nUserIdx)
        Internal::SetPrimaryProfileUserIndex(-1);

    Internal::ClearUserNewChallengeNotifications(nUserIdx);
}











BOOL IsActive(int32 nUserIdx)
{
    if (nUserIdx < 0 || nUserIdx >= 4)
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    if (!tRuntime.m_bGuest && tRuntime.m_idUser == nn::account::InvalidUid)
        return FALSE;

    return tRuntime.m_bActive != FALSE;
}


BOOL IsSignedIn(int32 nUserIdx)
{
    if (nUserIdx < 0 || nUserIdx >= 4)
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    if (tRuntime.m_bGuest)
        return TRUE;

    return tRuntime.m_idUser != nn::account::InvalidUid;
}


BOOL IsSignedInOnline(int32 nUserIdx)
{
    if (nUserIdx < 0 || nUserIdx >= 4)
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    if (tRuntime.m_idUser == nn::account::InvalidUid)
        return FALSE;

    if (!Online::IsPhysicalNetworkConnected())
        return FALSE;

    return tRuntime.m_bNetworkServiceAccountAvailable;
}












nn::account::UserHandle GetUserHandle(int32 nUserIndex)
{
    if (nUserIndex < 0 || nUserIndex >= 4)
        return nn::account::UserHandle();

    UserRuntime & tRuntime = s_atRuntime[nUserIndex];
    if (tRuntime.m_idUser == nn::account::InvalidUid)
        return nn::account::UserHandle();

    return tRuntime.m_tUserHandle;
}





nn::account::Uid GetUserId(int32 nUserIndex)
{
    if (nUserIndex < 0 || nUserIndex >= 4)
        return nn::account::InvalidUid;

    return s_atRuntime[nUserIndex].m_idUser;
}




BOOL GetUserNetworkServiceAccountId(int32 nUserIndex, nn::account::NetworkServiceAccountId & idOutUserNSA)
{
    if (nUserIndex < 0 || nUserIndex >= 4)
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIndex];
    if (tRuntime.m_idUser == nn::account::InvalidUid)
        return FALSE;

    bool bAvailable = false;
    nn::account::IsNetworkServiceAccountAvailable(&bAvailable, tRuntime.m_tUserHandle);
    if (!bAvailable)
        return FALSE;

    return nn::account::GetNetworkServiceAccountId(&idOutUserNSA, tRuntime.m_tUserHandle).IsSuccess();
}








BOOL RefreshPlayHistoryRegistrationKey(int32 nUserIndex, BOOL bUseLocalCommunicationKey)
{
    if (nUserIndex < 0 || nUserIndex >= 4)
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIndex];
    if (tRuntime.m_idUser == nn::account::InvalidUid)
        return FALSE;

    SigmaMemoryClear(&tRuntime.m_tPlayHistoryKey, sizeof(tRuntime.m_tPlayHistoryKey));
    nn::friends::GetPlayHistoryRegistrationKey(&tRuntime.m_tPlayHistoryKey, tRuntime.m_idUser, bUseLocalCommunicationKey != FALSE);
    return TRUE;
}







BOOL GetUserPlayRegistrationKey(int32 nUserIndex, nn::friends::PlayHistoryRegistrationKey & OutRegistrationKey)
{
    if (nUserIndex < 0 || nUserIndex >= 4)
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIndex];
    if (tRuntime.m_idUser == nn::account::InvalidUid)
        return FALSE;

    OutRegistrationKey = tRuntime.m_tPlayHistoryKey;
    return TRUE;
}







inline BOOL PlayerHistoryRecord::operator==(const PlayerHistoryRecord & rhs)
{
    return memcmp(&m_tRegistrationKey, &rhs.m_tRegistrationKey, sizeof(m_tRegistrationKey)) == 0;
}

BOOL UpdatePlayHistoryRecord(int32 nUserIndex, PlayerHistoryRecord * atPlayerHistoryRecords, uint32 uNumPlayerHistoryRecords)
{
    if (nUserIndex < 0 || nUserIndex >= 4)
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIndex];

    if (tRuntime.m_idUser == nn::account::InvalidUid ||
        tRuntime.m_bGuest == TRUE)
    {
        return FALSE;
    }

    nn::friends::PlayHistoryRegistrationKey tProfilesRegistrationKey;
    if (!GetUserPlayRegistrationKey(nUserIndex, tProfilesRegistrationKey))
        return FALSE;

    for (PlayerHistoryRecord * it = tRuntime.m_atPlayersRegistered.begin(); it != tRuntime.m_atPlayersRegistered.end();)
    {
        PlayerHistoryRecord & tPlayerHistory = *it;
        BOOL bFound = FALSE;
        for (int32 nHistoryIdx = 0; !bFound && nHistoryIdx < uNumPlayerHistoryRecords; ++nHistoryIdx)
        {
            bFound = atPlayerHistoryRecords[nHistoryIdx] == tPlayerHistory;
        }

        if (!bFound)
        {
            it = tRuntime.m_atPlayersRegistered.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (uint32 nHistoryIdx = 0; nHistoryIdx < uNumPlayerHistoryRecords; ++nHistoryIdx)
    {
        BOOL bFound = FALSE;
        PlayerHistoryRecord & tNewRecordHistory = atPlayerHistoryRecords[nHistoryIdx];

        if (memcmp(&tProfilesRegistrationKey, &tNewRecordHistory.m_tRegistrationKey, sizeof(tProfilesRegistrationKey)) == 0)
            continue;

        for (PlayerHistoryRecord & tTrackedHistoryRecord : tRuntime.m_atPlayersRegistered)
        {
            bFound = tNewRecordHistory == tTrackedHistoryRecord;
            if (bFound)
                break;
        }

        if (!bFound)
        {
            nn::friends::InAppScreenName UsersScreenName;
            nn::friends::InAppScreenName OtherScreenName;
            tRuntime.m_atPlayersRegistered.emplace_back(tNewRecordHistory);

            UsersScreenName.language = nn::oe::GetDesiredLanguage();
            OtherScreenName.language = UsersScreenName.language;
            Blizzard::String::Copy(UsersScreenName.name, tRuntime.m_tNickname.name, 64);
            Blizzard::String::Copy(OtherScreenName.name, tNewRecordHistory.m_szName, 64);

            nn::Result AddPlayResult = nn::friends::AddPlayHistory(tRuntime.m_idUser, tNewRecordHistory.m_tRegistrationKey, OtherScreenName, UsersScreenName);
        }
    }

    return TRUE;
}


























nn::account::Nickname GetUserNickName(int32 nUserIndex)
{
    if (nUserIndex < 0 || nUserIndex >= 4)
        return nn::account::Nickname();

    UserRuntime & tRuntime = s_atRuntime[nUserIndex];
    if (tRuntime.m_idUser == nn::account::InvalidUid)
        return nn::account::Nickname();

    return tRuntime.m_tNickname;
}





BOOL IsGuest(int32 nUserIdx)
{
    if (nUserIdx < 0 || nUserIdx >= 4)
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    return tRuntime.m_bGuest;
}




int32 GetGuestSponsor(int32 nUserIdx)
{
    return GetPrimaryProfileUserIndex();
}


void AwardAchievement(int32 nUserIdx, uint32 nAchievementId)
{
}




void UpdateHasNSOSubscription(int32 nUserIndex, BOOL bHasNSASubscription)
{
    if (nUserIndex >= 0 && nUserIndex < 4)
    {
        UserRuntime & tRuntime = s_atRuntime[nUserIndex];
        tRuntime.m_bHasNSOSubscription = bHasNSASubscription;
    }
}





BOOL IsEnsuringAccountAvailability(int32 nUserIndex)
{
    if (nUserIndex < 0 || nUserIndex >= 4)
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIndex];
    return tRuntime.m_bEnsureAccountAvailability;
}




void ClearAccountTokenDirty(int32 nUserIndex)
{
    if (nUserIndex >= 0 && nUserIndex < 4)
    {
        UserRuntime & tRunTime = s_atRuntime[nUserIndex];
        tRunTime.m_bAccountTokenIsDirty = FALSE;
    }
}




void SetAccountTokenDirty(int32 nUserIndex)
{
    if (nUserIndex >= 0 && nUserIndex < 4)
    {
        UserRuntime & tRunTime = s_atRuntime[nUserIndex];
        tRunTime.m_bAccountTokenIsDirty = TRUE;
    }
}




BOOL IsAccountTokenDirty(int32 nUserIndex)
{
    if (nUserIndex < 0 || nUserIndex >= 4)
        return FALSE;

    UserRuntime & tRunTime = s_atRuntime[nUserIndex];
    return tRunTime.m_bAccountTokenIsDirty;
}




BOOL IsNSOSubscribed(int32 nUserIndex)
{
    if (nUserIndex < 0 || nUserIndex >= 4)
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIndex];
    return tRuntime.m_bHasNSOSubscription;
}





OnlinePlayPrivilege GetOnlinePlayPrivilege(int32 nUserIdx)
{
    if (!IsGuest(nUserIdx) && !IsSignedInOnline(nUserIdx))
        return ONLINE_PLAY_BLOCKED_NOT_SIGNED_IN_ONLINE;

    if (!sbParentalFreeCommunication)
        return ONLINE_PLAY_BLOCKED_PARENTAL_CONTROLS;

    if (!IsNSOSubscribed(nUserIdx))
        return ONLINE_PLAY_BLOCKED_REQUIRES_PREMIUM;

    return ONLINE_PLAY_ALLOWED;
}










BOOL IsNetworkServiceAccountAvailable(int32 nUserIdx)
{
    if (!IsSignedIn(nUserIdx))
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    if (tRuntime.m_bGuest)
        return FALSE;

    bool bAvailable = false;
    if (nn::account::IsNetworkServiceAccountAvailable(&bAvailable, tRuntime.m_tUserHandle).IsFailure())
        return FALSE;

    return bAvailable;
}








void UpdatePrivileges(BOOL bIgnoreEnsures)
{
    sbParentalFreeCommunication = nn::pctl::CheckFreeCommunicationPermission(false);

    ScopedUsersReadLock tScopedLock;

    for (int32 i = 0; i < 4; ++i)
    {
        UserRuntime & tRuntime = s_atRuntime[i];
        if (tRuntime.m_idUser == nn::account::InvalidUid)
            continue;

        if (!bIgnoreEnsures && tRuntime.m_bEnsureAccountAvailability && Online::IsPhysicalNetworkConnected())
        {
            nn::Result EnsureResult = nn::account::EnsureNetworkServiceAccountAvailable(tRuntime.m_tUserHandle);
            (void)EnsureResult;
            tRuntime.m_bEnsureAccountAvailability = FALSE;
        }

        tRuntime.m_bNetworkServiceAccountAvailable = IsNetworkServiceAccountAvailable(i);
    }
}





void EnsureNetworkServiceAccountForUser(int32 nUserIndex)
{
    if (nUserIndex < 0 || nUserIndex >= 4)
        return;

    UserRuntime & tRuntime = s_atRuntime[nUserIndex];
    if (tRuntime.m_idUser == nn::account::InvalidUid)
        return;

    ScopedUsersReadLock tScopedLock;
    tRuntime.m_bEnsureAccountAvailability = TRUE;
    sbEnsureAvailabilityDirty = TRUE;
}





const char * GetGamerTag(int32 nUserIdx)
{
    if (nUserIdx < 0 || nUserIdx >= 4)
        return "";

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    return tRuntime.m_tNickname.name;
}






int32 GetPrimaryProfileUserIndex()
{
    return Internal::GetPrimaryProfileUserIndex();
}


void ClearPrimaryProfileUserIndex()
{
    Internal::SetPrimaryProfileUserIndex(-1);
}




void LoadAccountDevOnly(int32 nUserIdx, PlayerClass ePlayerClass)
{
    if (nUserIdx == 0)
        Internal::SetPrimaryProfileUserIndex(0);

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    tRuntime.m_tAccountDefinition.Clear();
    AccountDefinition_ToDefaults_ForConsoleClient(tRuntime.m_tAccountDefinition, nUserIdx);
    tRuntime.m_tProfile.mutable_account_profile();
    CreateRandomHero(nUserIdx, ePlayerClass);
    tRuntime.m_bActive = TRUE;
    tRuntime.m_bAccountLoadComplete = TRUE;
}











BOOL IsAccountLoaded(int32 nUserIdx)
{
    if (nUserIdx < 0 || nUserIdx >= 4)
        return FALSE;

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    return tRuntime.m_bAccountLoadComplete;
}





D3::Account::SavedDefinition & GetAccount(int32 nUserIdx)
{
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    return tRuntime.m_tAccountDefinition;
}






D3::Console::Profile & GetProfile(int32 nUserIdx)
{
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    return tRuntime.m_tProfile;
}





const HeroInfo * GetHeroList(int32 nUserIdx, uint32 & uNumHeroes)
{
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    uNumHeroes = tRuntime.m_uNumHeroes;
    return tRuntime.m_atHeroes;
}






uint32 GetNumAvailableHeroes(int32 nUserIdx)
{
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    return tRuntime.m_uNumHeroes;
}







void FindHeroes(int32 nUserIdx, HeroInfo * ptHeroInfos, uint32 nMaxHeroes, FindHeroesCallback pfnCallback)
{
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];

    uint32 nHeroes = 0;
    for (nHeroes = 0; nHeroes < blz::min(tRuntime.m_uNumHeroes, nMaxHeroes); ++nHeroes)
    {
        ptHeroInfos[nHeroes] = tRuntime.m_atHeroes[nHeroes];
    }

    Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, FindHeroesCallback>, uint32>::bind<&OnlineService::internal::callback_base<1, FindHeroesCallback>::execute>(pfnCallback, nHeroes));
}






































































// layout from DWARF: tRuntime 1312, szMountName 1314, tMount 1316, bSuccess 1318 (function scope), eResult 1340; has_digest call_line 1333, create_time 1334
// NON_MATCHING: 4 bytes short; the original reloads has_digest (ldrb, no inline DIE, row 1346:14) for the result, ours reuses the 1333 load; rows 1328:32 and 1330:13 (whole argument on one column) suggest a macro we do not have
void LoadHeroTask::Process()
{
    UserRuntime & tRuntime = s_atRuntime[m_nUserIdx];

    char szMountName[4096];
    sMakeMountName(m_nUserIdx, szMountName);
    MountSaveDataScope tMount(szMountName, m_idUser);

    BOOL bSuccess = FALSE;

    if (tMount.IsSuccess())
    {
        char szRelativePath[4096];
        char szFilePath[4096];

        sMakeHeroFilePath(m_szHeroFilename, szRelativePath);
        sMakeMountedPath(m_nUserIdx, szRelativePath, szFilePath);

        bSuccess = sReadAndParseDefinitionBuffer(szFilePath,

            tRuntime.m_tHeroDefinition);

        if (bSuccess &&
            tRuntime.m_tHeroDefinition.has_digest() &&
            tRuntime.m_tHeroDefinition.digest().create_time() == 0)
        {
            tRuntime.m_tHeroDefinition.mutable_digest()->set_create_time(Blizzard::Time::ToUnixTime(Blizzard::Time::GetTimestamp()));
        }
    }

    LoadHeroResult eResult;
    // inferred: DWARF puts eResult at 1340 and the has_digest result test at 1346:14 (col of an "else if (" operand); the mount test here is folded into 1320
    if (!tMount.IsSuccess())
    {
        eResult = LOAD_HERO_FAILED_CONTENT_MISSING;
    }
    else if (bSuccess && tRuntime.m_tHeroDefinition.has_digest())
    {
        eResult = LOAD_HERO_SUCCESS;
    }
    else
    {
        eResult = LOAD_HERO_FAILED_CORRUPT;
    }
    Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, LoadHeroCallback>, const LoadHeroResult &>::bind<&OnlineService::internal::callback_base<1, LoadHeroCallback>::execute>(m_pfnCallback, eResult));
}


BOOL IsHeroLoaded(int32 nUserIdx)
{
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    return tRuntime.m_tHeroDefinition.has_digest();
}






D3::Hero::SavedDefinition & GetHero(int32 nUserIdx)
{
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && tRuntime.m_uActiveBlizzconIndex != -1)
        return tRuntime.m_tBlizzconHeroDefinitionCache[tRuntime.m_uActiveBlizzconIndex];

    return tRuntime.m_tHeroDefinition;
}














void SetBlizzconHero(int32 nUserIdx, OnlineService::HeroId uHeroId)
{
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];

    for (int32 i = 0; i < 7; ++i)
    {
        D3::Hero::SavedDefinition & it = tRuntime.m_tBlizzconHeroDefinitionCache[i];
        if (it.has_digest() && it.digest().hero_id().id_low() == uHeroId)
        {
            tRuntime.m_uActiveBlizzconIndex = i;
            return;
        }
    }
}



void SetRandomBlizzconHero(int32 nUserIdx)
{
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];

    D3::Hero::SavedDefinition m_tBlizzconHeroDefinitionCache[7];

    int32 validIndices[7];
    int32 numValidIndices = 0;
    for (int32 index = 0; index < 7; ++index)
    {
        if (tRuntime.m_tBlizzconHeroDefinitionCache[index].has_digest())
        {
            validIndices[numValidIndices++] = index;
        }
    }

    RNG rng;
    RandInit(rng);
    uint32 selection = RandGetDWord(rng) % numValidIndices;
    SetHero(nUserIdx, tRuntime.m_tBlizzconHeroDefinitionCache[selection]);
}





void SetHero(int32 nUserIdx, const D3::Hero::SavedDefinition & tSavedDefinition)
{
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];

    tRuntime.m_tHeroDefinition.CopyFrom(tSavedDefinition);

    OnlineService::HeroId idNewHero = tSavedDefinition.digest().hero_id().id_low();
    for (uint32 i = 0; i < tRuntime.m_uNumHeroes; ++i)
    {
        HeroInfo & tHeroInfo = tRuntime.m_atHeroes[i];
        if (idNewHero == tHeroInfo.uHeroId)
        {
            tHeroInfo.Import(tSavedDefinition);
            return;
        }
    }

    HeroInfo & tHeroInfo = tRuntime.m_atHeroes[tRuntime.m_uNumHeroes++];
    tHeroInfo.Import(tSavedDefinition);
    sMakeHeroFileName(tHeroInfo.uHeroId, tHeroInfo.szFilename);

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        if (tSavedDefinition.saved_attributes().gb_handle().game_balance_type() == GB_HEROS)
        {
            int index = PlayerClassFromGBID(tSavedDefinition.saved_attributes().gb_handle().gbid());
            tRuntime.m_tBlizzconHeroDefinitionCache[index].CopyFrom(tSavedDefinition);
        }
    }
}











void Save(int32 nUserIdx, SaveCallback pfnCallback)
{
    if (!IsSignedIn(nUserIdx))
    {
        Internal::QueueCallback(pfnCallback);
        return;
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        Internal::QueueCallback(pfnCallback);
        return;
    }

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    if (tRuntime.m_bGuest)
    {
        Internal::QueueCallback(pfnCallback);
        return;
    }

    SaveTask * ptSaveTask = new SaveTask(nUserIdx, tRuntime.m_idUser, pfnCallback);
    ptSaveTask->SaveFile(sszAccountFilename, tRuntime.m_tAccountDefinition);
    ptSaveTask->SaveFile(sszProfileFilename, tRuntime.m_tProfile);

    if (tRuntime.m_tHeroDefinition.has_digest())
    {
        OnlineService::HeroId idCurrentHero = tRuntime.m_tHeroDefinition.digest().hero_id().id_low();
        const char * szHeroFilename = NULL;
        for (uint32 i = 0; i < tRuntime.m_uNumHeroes; ++i)
        {
            HeroInfo & tHeroInfo = tRuntime.m_atHeroes[i];
            if (idCurrentHero == tHeroInfo.uHeroId)
            {
                tHeroInfo.Import(tRuntime.m_tHeroDefinition);
                szHeroFilename = tHeroInfo.szFilename;
                break;
            }
        }

        if (szHeroFilename == NULL)
        {
            ::delete ptSaveTask;
            Internal::QueueCallback(pfnCallback);
            return;
        }

        char szPath[4096];
        sMakeHeroFilePath(szHeroFilename, szPath);
        ptSaveTask->SaveFile(szPath, tRuntime.m_tHeroDefinition);
    }

    Internal::RemoveTasks(OnlineService::internal::function_binder_2<int, nn::account::Uid, Internal::SaveLoadTask *>::bind<&sSaveTaskComparison>(tRuntime.m_idUser));
    ptSaveTask->Queue();
}























void FinishDeleteHero(int32 nUserIdx, nn::account::Uid idUser, OnlineService::HeroId idHero, DeleteHeroCallback pfnCallback, DeleteHeroResult eResult)
{
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];

    if (!IsSignedIn(nUserIdx) ||
        tRuntime.m_idUser != idUser)
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, DeleteHeroCallback>, const DeleteHeroResult &>::bind<&OnlineService::internal::callback_base<1, DeleteHeroCallback>::execute>(pfnCallback, eResult));
        return;
    }

    if (tRuntime.m_tHeroDefinition.has_digest() &&
        tRuntime.m_tHeroDefinition.digest().hero_id().id_low() == idHero)
    {
        tRuntime.m_tHeroDefinition.clear_digest();
    }

    uint32 uHeroIndex;
    for (uHeroIndex = 0; uHeroIndex < tRuntime.m_uNumHeroes; ++uHeroIndex)
    {
        HeroInfo & tHero = tRuntime.m_atHeroes[uHeroIndex];
        if (tHero.uHeroId == idHero)
        {
            --tRuntime.m_uNumHeroes;
            break;
        }
    }

    for (; uHeroIndex < tRuntime.m_uNumHeroes; ++uHeroIndex)
    {
        tRuntime.m_atHeroes[uHeroIndex] = tRuntime.m_atHeroes[uHeroIndex + 1];
    }

    if (tRuntime.m_tAccountDefinition.digest().last_played_hero_id().id_low() == idHero)
    {
        tRuntime.m_tAccountDefinition.mutable_digest()->mutable_last_played_hero_id()->set_id_high(0);
        tRuntime.m_tAccountDefinition.mutable_digest()->mutable_last_played_hero_id()->set_id_low(0);
    }

    Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, DeleteHeroCallback>, const DeleteHeroResult &>::bind<&OnlineService::internal::callback_base<1, DeleteHeroCallback>::execute>(pfnCallback, eResult));
}




















void DeleteHero(int32 nUserIdx, OnlineService::HeroId uHeroId, DeleteHeroCallback pfnCallback)
{
    if (!IsSignedIn(nUserIdx))
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, DeleteHeroCallback>, const DeleteHeroResult &>::bind<&OnlineService::internal::callback_base<1, DeleteHeroCallback>::execute>(pfnCallback, DELETE_HERO_FAILED));
        return;
    }

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];

    if (tRuntime.m_bGuest)
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, DeleteHeroCallback>, const DeleteHeroResult &>::bind<&OnlineService::internal::callback_base<1, DeleteHeroCallback>::execute>(pfnCallback, DELETE_HERO_FAILED));
        return;
    }

    for (uint32 uHeroIndex = 0; uHeroIndex < tRuntime.m_uNumHeroes; ++uHeroIndex)
    {
        HeroInfo & tHero = tRuntime.m_atHeroes[uHeroIndex];
        if (tHero.uHeroId == uHeroId)
        {
            DeleteHeroTask * ptDeleteTask = new DeleteHeroTask(nUserIdx, tRuntime.m_idUser, tHero.szFilename,
                OnlineService::internal::function_binder_5<void, int32, nn::account::Uid, OnlineService::HeroId, DeleteHeroCallback, DeleteHeroResult>::bind<&FinishDeleteHero>(nUserIdx, tRuntime.m_idUser, uHeroId, pfnCallback));
            ptDeleteTask->Queue();
            return;
        }
    }

    Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, DeleteHeroCallback>, const DeleteHeroResult &>::bind<&OnlineService::internal::callback_base<1, DeleteHeroCallback>::execute>(pfnCallback, DELETE_HERO_FAILED_NOT_FOUND));
}













void DeleteHeroTask::Process()
{
    char szMountName[4096];
    char szHeroPath[4096];
    char szFilePath[4096];
    sMakeMountName(m_nUserIdx, szMountName);
    MountSaveDataScope tMount(szMountName, m_idUser);

    if (tMount.IsFailure())
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, DeleteHeroCallback>, const DeleteHeroResult &>::bind<&OnlineService::internal::callback_base<1, DeleteHeroCallback>::execute>(m_pfnCallback, DELETE_HERO_FAILED));
        return;
    }

    sMakeHeroFilePath(m_szHeroFilename, szHeroPath);
    sMakeMountedPath(m_nUserIdx, szHeroPath, szFilePath);

    BOOL bSuccess = Blizzard::File::Delete(szFilePath);
    if (bSuccess)
    {
        tMount.Commit();
    }

    Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, DeleteHeroCallback>, const DeleteHeroResult &>::bind<&OnlineService::internal::callback_base<1, DeleteHeroCallback>::execute>(m_pfnCallback, bSuccess ? DELETE_HERO_SUCCESS : DELETE_HERO_FAILED));
}











void SavePrefs()
{
    int32 nUserIdx = GetPrimaryProfileUserIndex();

    if (!IsSignedIn(nUserIdx))
        return;

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    if (tRuntime.m_bGuest)
        return;

    uint8 * pauPreferencesBuffer = (uint8 *)SigmaMemoryNew(MAX_PREFERENCES_SIZE, 4, NULL, FALSE);
    int32 nSize = Prefs::Save(pauPreferencesBuffer, MAX_PREFERENCES_SIZE);

    SaveTask * ptSaveTask = new SavePreferencesTask(nUserIdx, tRuntime.m_idUser, SaveCallback());
    ptSaveTask->SaveFile(sszPreferencesFilename, pauPreferencesBuffer, nSize);
    ptSaveTask->Queue();
}















void sImportSaveSaveCallbackWrapper(ImportSaveCallback pfnCallback)
{
    Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, ImportSaveCallback>, const ImportSaveResult &>::bind<&OnlineService::internal::callback_base<1, ImportSaveCallback>::execute>(pfnCallback, IMPORT_SAVE_SUCCESS));
}





void ImportSave(int32 nUserIdx, D3::Console::MigrationData * ptMigrationData, BOOL bDestructive, ImportSaveCallback pfnCallback)
{
    if (!IsSignedIn(nUserIdx))
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, ImportSaveCallback>, const ImportSaveResult &>::bind<&OnlineService::internal::callback_base<1, ImportSaveCallback>::execute>(pfnCallback, IMPORT_SAVE_FAILED));
        return;
    }

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];

    if (tRuntime.m_bGuest)
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, ImportSaveCallback>, const ImportSaveResult &>::bind<&OnlineService::internal::callback_base<1, ImportSaveCallback>::execute>(pfnCallback, IMPORT_SAVE_FAILED));
        return;
    }

    SaveTask * ptSaveTask = new SaveTask(nUserIdx, tRuntime.m_idUser, OnlineService::internal::function_binder_1<void, ImportSaveCallback>::bind<&sImportSaveSaveCallbackWrapper>(pfnCallback));

    tRuntime.m_tAccountDefinition = ptMigrationData->account();
    ptSaveTask->SaveFile(sszAccountFilename, tRuntime.m_tAccountDefinition);

    tRuntime.m_tProfile = ptMigrationData->profile();
    ptSaveTask->SaveFile(sszProfileFilename, tRuntime.m_tProfile);

    OnlineService::HeroId idLastPlayedHero = tRuntime.m_tAccountDefinition.digest().last_played_hero_id().id_low();

    tRuntime.m_uNumHeroes = 0;
    for (int32 i = 0; i < ptMigrationData->heroes_size() && tRuntime.m_uNumHeroes < 17; ++i)
    {
        char szFilePath[4096];
        const D3::Hero::SavedDefinition & tHeroDefinition = ptMigrationData->heroes(i);
        HeroInfo & tHeroInfo = tRuntime.m_atHeroes[tRuntime.m_uNumHeroes++];
        tHeroInfo.Import(tHeroDefinition);
        sMakeHeroFileName(tHeroInfo.uHeroId, tHeroInfo.szFilename);
        sMakeHeroFilePath(tHeroInfo.szFilename, szFilePath);
        ptSaveTask->SaveFile(szFilePath, tHeroDefinition);

        if (i == 0 || tHeroInfo.uHeroId == idLastPlayedHero)
        {
            tRuntime.m_tHeroDefinition = tHeroDefinition;
        }
    }

    if (GetPrimaryProfileUserIndex() == nUserIdx)
    {
        uint8 * pauPreferencesBuffer = (uint8 *)SigmaMemoryNew(MAX_PREFERENCES_SIZE, 4, NULL, FALSE);
        int32 nSize = Prefs::Save(pauPreferencesBuffer, MAX_PREFERENCES_SIZE);
        ptSaveTask->SaveFile(sszPreferencesFilename, pauPreferencesBuffer, nSize);
    }

    ptSaveTask->Queue();
}
















void ExportSave(int32 nUserIdx, D3::Console::MigrationData * ptMigrationData, ExportSaveCallback pfnCallback)
{
    ExportSaveTask * ptExportSaveTask;

    if (!IsSignedIn(nUserIdx))
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, ExportSaveCallback>, const ExportSaveResult &>::bind<&OnlineService::internal::callback_base<1, ExportSaveCallback>::execute>(pfnCallback, EXPORT_SAVE_FAILED));
        return;
    }

    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    if (tRuntime.m_bGuest)
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, ExportSaveCallback>, const ExportSaveResult &>::bind<&OnlineService::internal::callback_base<1, ExportSaveCallback>::execute>(pfnCallback, EXPORT_SAVE_FAILED));
        return;
    }

    ptExportSaveTask = new ExportSaveTask(nUserIdx, ptMigrationData, pfnCallback);
    ptExportSaveTask->Queue();
}































































































UserRuntime::UserRuntime()
{
    Clear();
}

void UserRuntime::Clear()
{
    m_idUser = nn::account::InvalidUid;
    SigmaMemoryClear(&m_tUserHandle, sizeof(m_tUserHandle));
    SigmaMemoryClear(&m_tNickname, sizeof(m_tNickname));
    m_bActive = FALSE;
    m_bAccountLoadComplete = FALSE;
    m_bGuest = FALSE;
    m_bNetworkServiceAccountAvailable = FALSE;
    m_bEnsureAccountAvailability = FALSE;
    m_bAccountTokenIsDirty = FALSE;
    m_bHasNSOSubscription = FALSE;
    m_tAccountDefinition.Clear();
    m_tHeroDefinition.Clear();
    m_tProfile.Clear();
    m_uNumHeroes = 0;
    m_uActiveBlizzconIndex = -1;
    for (D3::Hero::SavedDefinition & it : m_tBlizzconHeroDefinitionCache)
        it.Clear();
    nGuestIndex = -1;
}






LoadAccountTask::LoadAccountTask(int32 nUserIdx, SignInMode eMode, SignInCallback pfnCallback)
    : m_nUserIdx(nUserIdx),
      m_eSignInMode(eMode),
      m_pfnCallback(pfnCallback)
{
}




void LoadAccountTask::Process()
{
    char szMountName[4096];
    char szFilePath[4096];
    BOOL bCorruptData;
    BOOL bSuccess;
    SignInResult eSignInResult;

    UserRuntime & tRuntime = s_atRuntime[m_nUserIdx];
    tRuntime.m_tAccountDefinition.Clear();
    AccountDefinition_ToDefaults_ForConsoleClient(tRuntime.m_tAccountDefinition, m_nUserIdx);
    tRuntime.m_tHeroDefinition.Clear();
    tRuntime.m_tProfile.Clear();
    tRuntime.m_tProfile.mutable_account_profile();
    tRuntime.m_uNumHeroes = 0;

    nn::Result ensureResult = nn::fs::EnsureSaveData(tRuntime.m_idUser);
    if (ensureResult.IsFailure())
    {
        Prefs::Load(NULL);
        HandleResult(SIGNIN_FAILED_NO_SPACE);
        return;
    }

    sMakeMountName(m_nUserIdx, szMountName);
    MountSaveDataScope tMount(szMountName, tRuntime.m_idUser);
    if (tMount.IsFailure())
    {
        Prefs::Load(NULL);
        HandleResult(SIGNIN_FAILED);
        return;
    }

    if (m_nUserIdx == GetPrimaryProfileUserIndex())
    {
        sMakeMountedPath(m_nUserIdx, sszPreferencesFilename, szFilePath);
        Prefs::Load(szFilePath);
    }

    sMakeMountedPath(m_nUserIdx, sszAccountFilename, szFilePath);
    if (!Blizzard::File::IsFile(szFilePath))
    {
        HandleResult(SIGNIN_SUCCESS_NO_SAVE);
        return;
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        HandleResult(SIGNIN_SUCCESS);
        return;
    }

    bCorruptData = FALSE;

    sMakeMountedPath(m_nUserIdx, sszAccountFilename, szFilePath);
    bSuccess = sReadAndParseDefinitionBuffer(szFilePath, tRuntime.m_tAccountDefinition);
    if (!bSuccess)
    {
        AccountDefinition_ToDefaults_ForConsoleClient(tRuntime.m_tAccountDefinition, m_nUserIdx);
        bCorruptData = TRUE;
    }

    sMakeMountedPath(m_nUserIdx, sszProfileFilename, szFilePath);
    bSuccess = sReadAndParseDefinitionBuffer(szFilePath, tRuntime.m_tProfile);
    if (!bSuccess)
    {
        tRuntime.m_tProfile.Clear();
        tRuntime.m_tProfile.mutable_account_profile();
        bCorruptData = TRUE;
    }

    OnlineService::HeroId idLastPlayedHero = tRuntime.m_tAccountDefinition.digest().last_played_hero_id().id_low();

    sMakeMountedPath(m_nUserIdx, sszHeroDirectory, szFilePath);
    FileReference tRef, tResult;
    FileReferenceInit(tRef, szFilePath);
    void * ptFindFileState;
    FileFindStart(ptFindFileState, tRef, "*", 1, 0);
    while (FileFindNext(ptFindFileState, tResult))
    {
        D3::Hero::SavedDefinition tHeroDefinition;
        const char * szHeroFilePath = FileReferenceGetPath(tResult);
        BOOL bSuccess = sReadAndParseDefinitionBuffer(szHeroFilePath, tHeroDefinition);

        if (!bSuccess || !tHeroDefinition.has_digest() || tRuntime.m_uNumHeroes >= 17)
        {
            Blizzard::File::Delete(szHeroFilePath);
            bCorruptData = TRUE;
            continue;
        }

        HeroInfo & tHeroInfo = tRuntime.m_atHeroes[tRuntime.m_uNumHeroes++];
        char * szHeroFilename = tHeroInfo.szFilename;
        FileReferenceGetNameNoExtension(tResult, szHeroFilename);
        FileReferenceGetExtension(tResult, szHeroFilename + strlen(szHeroFilename));
        tHeroInfo.Import(tHeroDefinition);

        if (tHeroInfo.uHeroId == idLastPlayedHero || !tRuntime.m_tHeroDefinition.has_digest())
        {
            BOOL bIsDeceased = (tHeroInfo.uPlayerFlags & 8) != 0;
            if (!bIsDeceased)
            {
                tRuntime.m_tHeroDefinition = tHeroDefinition;
            }
        }
    }
    FileFindClose(ptFindFileState);

    if (bCorruptData)
    {
        tMount.Commit();
        eSignInResult = SIGNIN_SUCCESS_CORRUPT_DATA;
    }
    else
    {
        eSignInResult = SIGNIN_SUCCESS;
    }

    if (tRuntime.m_uNumHeroes > 1)
    {
        qsort(tRuntime.m_atHeroes, tRuntime.m_uNumHeroes, sizeof(HeroInfo), sSortHeroInfo);
    }

    HandleResult(eSignInResult);
}






























void LoadAccountTask::HandleResult(SignInResult eResult)
{
    UserRuntime & tRuntime = s_atRuntime[m_nUserIdx];
    tRuntime.m_bActive = TRUE;
    tRuntime.m_bAccountLoadComplete = TRUE;

    Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, SignInCallback>, const SignInResult &>::bind<&OnlineService::internal::callback_base<1, SignInCallback>::execute>(m_pfnCallback, eResult));
}

SaveTask::SaveTask(int32 nUserIdx, nn::account::Uid idUser, SaveCallback pfnCallback)
    : m_nUserIdx(nUserIdx),
      m_idUser(idUser),
      m_pfnCallback(pfnCallback)
{
    m_nNumFiles = 0;
    nn::oe::EnterExitRequestHandlingSection();
    Internal::NotifyBeginSaveCallback();
}

SaveTask::~SaveTask()
{
    Internal::NotifyEndSaveCallback();
    nn::oe::LeaveExitRequestHandlingSection();
}







void SaveTask::Process()
{
    char szMountName[4096];
    sMakeMountName(m_nUserIdx, szMountName);
    MountSaveDataScope tMount(szMountName, m_idUser);
    BOOL bSuccess = FALSE;

    if (tMount.IsSuccess())
    {
        char szFilePath[4096];
        sMakeMountedPath(m_nUserIdx, sszHeroDirectory, szFilePath);
        Blizzard::File::CreateDirectory(szFilePath, true);
        bSuccess = TRUE;
        for (uint32 i = 0; i < m_nNumFiles; ++i)
        {
            File * tFile = &m_atFiles[i];

            sMakeMountedPath(m_nUserIdx, tFile->szFilename, szFilePath);

            Blizzard::File::Stream tStream;
            BOOL bRes = Blizzard::File::Open(szFilePath, 0x402, tStream);  // DWARF: BOOL bRes declared at the Open line (2167); open failure leaves it 0
            if (bRes)
            {
                bRes = Blizzard::File::Write(tStream, tFile->pBuffer, tFile->nSize);
                Blizzard::File::Close(tStream);
            }








            bSuccess = bSuccess && bRes;
        }
    }

    if (bSuccess)
    {
        tMount.Commit();
    }





    Internal::QueueCallback(m_pfnCallback);
}

SaveTask * SaveTask::CastAsSaveTask()
{
    return this;
}


void * SaveTask::SaveFile(
    const char * szFilename,
    void * pData,
    int32 nDataSize)
{


    File & tFile = m_atFiles[m_nNumFiles++];

    if (szFilename)
    {
        Blizzard::String::Copy(tFile.szFilename, szFilename, sizeof(tFile.szFilename));
    }

    tFile.pBuffer = pData;
    tFile.nSize = nDataSize;
    return pData;
}


BOOL SaveTask::SaveFile(
    const char * szFilename,
    const google::protobuf::MessageLite & tMessage)
{
    int32 nSize = tMessage.ByteSize();
    uint8 * pauBuffer = (uint8 *)SigmaMemoryNew(Math::Max(nSize, 1), 4, NULL, FALSE);
    BOOL bProtoCompiled = tMessage.SerializeToArray(pauBuffer, nSize);
    if (!bProtoCompiled)
    {
        SigmaMemoryFree(pauBuffer, NULL);

        DisplayInternalError(NULL, "", 2385, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    Blizzard::Util::EncXor64(suEncryptionKey, pauBuffer, nSize);




    SaveFile(szFilename, pauBuffer, nSize);
    return TRUE;
}

SaveTask::File::File()
{
    pBuffer = NULL;
    nSize = 0;
    SigmaMemoryClear(szFilename, sizeof(szFilename));
}

SaveTask::File::~File()
{
    SigmaMemoryFree(pBuffer, NULL);
    pBuffer = NULL;
}

SavePreferencesTask::SavePreferencesTask(int32 nUserIdx, nn::account::Uid idUser, SaveCallback pfnCallback)
    : SaveTask(nUserIdx, idUser, pfnCallback)
{
}



SaveTask * SavePreferencesTask::CastAsSaveTask()
{
    return NULL;
}

}  // namespace GamerProfile
}  // namespace Console



static void sMakeHeroFileName(OnlineService::HeroId idHero, char * szFilename)
{
    Blizzard::String::Format(szFilename, 32, "%016llx.dat", idHero);
}



static void sMakeMountName(int nUserIdx, char * szName)
{
    Blizzard::String::Format(szName, 4096, "save%d", nUserIdx);
}



static void sMakeMountedPath(int nUserIdx, const char * szFilename, char * szPath)
{
    Blizzard::String::Format(szPath, 4096, "save%d:/%s", nUserIdx, szFilename);
}











static void sMakeHeroFilePath(const char * szHeroFile, char * szPath)
{
    Blizzard::String::Format(szPath, 4096, "%s/%s", sszHeroDirectory, szHeroFile);
}




static BOOL sReadAndParseDefinitionBuffer(const char * szFilePath, google::protobuf::MessageLite & tMessage)
{
    Blizzard::File::Stream tStream;
    BOOL bRes = Blizzard::File::Open(szFilePath, 0x1001, tStream);  // original line 2318: Open at the bRes declaration; failure leaves it 0
    if (bRes)
    {
        int32 nDefinitionSize = (int32)Blizzard::File::GetFileInfo(tStream)->size;
        uint8 * pauDefinitionBuffer = (uint8 *)SigmaMemoryNew(nDefinitionSize > 1 ? nDefinitionSize : 1, 4, NULL, FALSE);

        bRes = Blizzard::File::Read(tStream, pauDefinitionBuffer, nDefinitionSize);
        if (bRes)
        {
            Blizzard::Util::DecXor64(suEncryptionKey, pauDefinitionBuffer, nDefinitionSize);

            bRes = tMessage.ParseFromArray(pauDefinitionBuffer, nDefinitionSize);
        }

        SigmaMemoryFree(pauDefinitionBuffer, NULL);

        Blizzard::File::Close(tStream);
    }





    return bRes;
}


static int32 sSortHeroInfo(const void * pLeft, const void * pRight)
{
    const Console::GamerProfile::HeroInfo * ptLeftHero = (const Console::GamerProfile::HeroInfo *)pLeft;
    const Console::GamerProfile::HeroInfo * ptRightHero = (const Console::GamerProfile::HeroInfo *)pRight;

    if (ptLeftHero->uCreationTime < ptRightHero->uCreationTime)
        return -1;
    if (ptLeftHero->uCreationTime > ptRightHero->uCreationTime)
        return 1;
    return 0;
}










static BOOL sSaveTaskComparison(nn::account::Uid idUser, Console::GamerProfile::Internal::SaveLoadTask * ptTask)
{
    Console::GamerProfile::SaveTask * ptSaveTask = ptTask->CastAsSaveTask();

    return ptSaveTask && ptSaveTask->m_idUser == idUser;
}




static uint sAsyncUpdatePrivilegeThread(void * pParam)
{
    while (sbRunUpdatePrivilegeThread)
    {
        Blizzard::Process::Sleep(1);

        Blizzard::Time::Millisecond now = Blizzard::Time::Milliseconds();
        Blizzard::Time::Millisecond uTimeSinceLastUpdatePrivilege = Blizzard::Time::GetTimeElapsed(suLastUpdatePrivilegeAttempt, now);
        if (uTimeSinceLastUpdatePrivilege > PRIVILEGE_REFRESH_INTERVAL || suLastUpdatePrivilegeAttempt == 0 || sbEnsureAvailabilityDirty)
        {
            Console::GamerProfile::UpdatePrivileges(FALSE);
            suLastUpdatePrivilegeAttempt = Blizzard::Time::Milliseconds();
            sbEnsureAvailabilityDirty = FALSE;
        }
    }

    return 0;
}

void HeroDefinition_Init(D3::Hero::SavedDefinition & tSavedDefinition);
namespace Console {
namespace GamerProfile {
#line 1246
void LoadHero(int32 nUserIdx, OnlineService::HeroId idHero, LoadHeroCallback pfnCallback)
{
    if (!IsSignedIn(nUserIdx))
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, LoadHeroCallback>, const LoadHeroResult &>::bind<&OnlineService::internal::callback_base<1, LoadHeroCallback>::execute>(pfnCallback, LOAD_HERO_FAILED));
        return;
    }
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    if (tRuntime.m_bGuest)
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, LoadHeroCallback>, const LoadHeroResult &>::bind<&OnlineService::internal::callback_base<1, LoadHeroCallback>::execute>(pfnCallback, LOAD_HERO_FAILED));
        return;
    }
    if (tRuntime.m_tHeroDefinition.has_digest() && tRuntime.m_tHeroDefinition.digest().hero_id().id_low() == idHero)
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, LoadHeroCallback>, const LoadHeroResult &>::bind<&OnlineService::internal::callback_base<1, LoadHeroCallback>::execute>(pfnCallback, LOAD_HERO_SUCCESS));
        return;
    }
    tRuntime.m_tHeroDefinition.Clear();
    const char * szHeroFilename = NULL;
    for (uint32 i = 0; i < tRuntime.m_uNumHeroes; ++i)
    {
        if (tRuntime.m_atHeroes[i].uHeroId == idHero)
        {
            szHeroFilename = tRuntime.m_atHeroes[i].szFilename;
            break;
        }
    }
    if (!szHeroFilename)
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, LoadHeroCallback>, const LoadHeroResult &>::bind<&OnlineService::internal::callback_base<1, LoadHeroCallback>::execute>(pfnCallback, LOAD_HERO_FAILED));
        return;
    }
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        HeroDefinition_Init(tRuntime.m_tHeroDefinition);
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, LoadHeroCallback>, const LoadHeroResult &>::bind<&OnlineService::internal::callback_base<1, LoadHeroCallback>::execute>(pfnCallback, LOAD_HERO_SUCCESS));
        return;
    }
    LoadHeroTask * ptLoadTask = new LoadHeroTask(nUserIdx, tRuntime.m_idUser, szHeroFilename, pfnCallback);
    ptLoadTask->Queue();
}

#line 1828
void ExportSaveTask::Process()
{
    UserRuntime & tRuntime = s_atRuntime[m_nUserIdx];
    char szMountName[4096];
    sMakeMountName(m_nUserIdx, szMountName);
    MountSaveDataScope tMount(szMountName, tRuntime.m_idUser);
    if (tMount.IsFailure())
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, ExportSaveCallback>, const ExportSaveResult &>::bind<&OnlineService::internal::callback_base<1, ExportSaveCallback>::execute>(m_pfnCallback, EXPORT_SAVE_FAILED));
        return;
    }
    char szFilePath[4096];
    sMakeMountedPath(m_nUserIdx, sszAccountFilename, szFilePath);
    if (!Blizzard::File::IsFile(szFilePath))
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, ExportSaveCallback>, const ExportSaveResult &>::bind<&OnlineService::internal::callback_base<1, ExportSaveCallback>::execute>(m_pfnCallback, EXPORT_SAVE_FAILED_NOT_FOUND));
        return;
    }
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, ExportSaveCallback>, const ExportSaveResult &>::bind<&OnlineService::internal::callback_base<1, ExportSaveCallback>::execute>(m_pfnCallback, EXPORT_SAVE_FAILED_NOT_FOUND));
        return;
    }
    sMakeMountedPath(m_nUserIdx, sszAccountFilename, szFilePath);
    BOOL bSuccess = sReadAndParseDefinitionBuffer(szFilePath, *m_ptMigrationData->mutable_account());
    if (!bSuccess)
        m_ptMigrationData->clear_account();
    sMakeMountedPath(m_nUserIdx, sszProfileFilename, szFilePath);
    bSuccess = sReadAndParseDefinitionBuffer(szFilePath, *m_ptMigrationData->mutable_profile());
    if (!bSuccess)
        m_ptMigrationData->clear_profile();
    sMakeMountedPath(m_nUserIdx, sszHeroDirectory, szFilePath);
    FileReference tRef;
    FileReference tResult;
    FileReferenceInit(tRef, szFilePath);
    void * ptFindFileState;
    FileFindStart(ptFindFileState, tRef, "*", 1, 0);
    while (FileFindNext(ptFindFileState, tResult))
    {
        D3::Hero::SavedDefinition tHeroDefinition;
        const char * szHeroFilePath = FileReferenceGetPath(tResult);
        BOOL bSuccess = sReadAndParseDefinitionBuffer(szHeroFilePath, tHeroDefinition);
        if (bSuccess && tHeroDefinition.has_digest() && m_ptMigrationData->heroes_size() < 17)
            m_ptMigrationData->add_heroes()->CopyFrom(tHeroDefinition);
    }
    FileFindClose(ptFindFileState);
    Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, ExportSaveCallback>, const ExportSaveResult &>::bind<&OnlineService::internal::callback_base<1, ExportSaveCallback>::execute>(m_pfnCallback, EXPORT_SAVE_SUCCESS));
}
}
}

#include "LobbyService.h"
#include "../../Common/Definitions/StringListDefinition.h"
#include "../../Sigma/UnicodeText.h"

bool UsePreselectedUser();
namespace nn
{
namespace account
{
// DWARF account_Selector.h:30, size 0x88.
struct UserSelectionSettings
{
    Uid invalidUidList[8];
    bool isSkipEnabled;
    bool isNetworkServiceAccountRequired;
    bool showSkipButton;
    bool additionalSelect;
};
void ListAllUsers(int *, Uid *, int);
nn::Result OpenUser(UserHandle *, const Uid &);
nn::Result OpenPreselectedUser(UserHandle *);
nn::Result GetUserId(Uid *, const UserHandle &);
nn::Result GetNickname(Nickname *, const Uid &);
nn::Result ShowUserSelector(Uid *, const UserSelectionSettings &);
}
}

// Original file-local helper, GamerProfileNX64.cpp:421.
static bool LoginBlizzconUser(int32 nUserIdx, Console::GamerProfile::SignInMode eMode, Console::GamerProfile::SignInCallback pfnCallback)
{
    using namespace Console;
    using namespace Console::GamerProfile;
    nn::account::Uid outUsers[1];
    int userCount = 0;
    nn::account::UserHandle tUserHandle;
    nn::account::ListAllUsers(&userCount, outUsers, 1);
    if (userCount == 0 || nn::account::OpenUser(&tUserHandle, outUsers[0]).IsFailure())
        return false;
    nn::account::Uid idUser = outUsers[0];
    const bool bIsFirstUser = GetPrimaryProfileUserIndex() == -1;
    ScopedUsersWriteLock tScopedLock;
    UserRuntime & tRuntime = s_atRuntime[nUserIdx];
    tRuntime.Clear();
    tRuntime.m_idUser = idUser;
    tRuntime.m_tUserHandle = tUserHandle;
    nn::account::GetNickname(&tRuntime.m_tNickname, idUser);
    bool bIsLocalCommunication = true;
    RefreshPlayHistoryRegistrationKey(nUserIdx, bIsLocalCommunication);
    if (bIsFirstUser)
        Internal::SetPrimaryProfileUserIndex(nUserIdx);
    UpdatePrivileges(TRUE);
    Online::PresenceManager::Get(nUserIdx)->OnNX64UserSignedIn();
    LoadAccountTask * ptLoadAccountTask = new LoadAccountTask(nUserIdx, eMode, pfnCallback);
    ptLoadAccountTask->Queue();
    return true;
}

namespace Console
{
namespace GamerProfile
{
#line 473 "X/Client/Console/GamerProfileNX64.cpp"
// Laid out on the original rows (474-627) with columns; see build/try/draft/GamerProfileNX64.txt.
void SignIn(
    int32 nUserIdx,
    SignInMode eMode,
    SignInCallback pfnCallback)
{


    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        if (LoginBlizzconUser(nUserIdx, eMode, pfnCallback))
            return;
    }

    if (nUserIdx < 0 || nUserIdx >= 4)
    {
        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, SignInCallback>, const SignInResult &>::bind<&OnlineService::internal::callback_base<1, SignInCallback>::execute>(pfnCallback, SIGNIN_FAILED));
        return;
    }

    nn::account::Uid idUser;
    nn::account::UserHandle tUserHandle;
    bool bGuest = false;

    const bool bIsFirstUser = GetPrimaryProfileUserIndex() == -1;
    if (UsePreselectedUser() && sbIsFirstSignIn)
    {



        tUserHandle = unk_71019079f0;


        nn::Result IdResult = nn::account::GetUserId(&idUser, tUserHandle);



        sbIsFirstSignIn = FALSE;
    }
    else
    {


        nn::account::UserSelectionSettings tSettings;
        SigmaMemoryClear(&tSettings, sizeof(tSettings));
        tSettings.additionalSelect = !bIsFirstUser;
        tSettings.showSkipButton = !bIsFirstUser;
        nn::account::Uid * uInvalidUidList = tSettings.invalidUidList;

        for (int i = 0; i < 4; ++i)
        {
            UserRuntime &tRuntime = s_atRuntime[i];
            if (tRuntime.m_idUser != nn::account::InvalidUid)
            {
                *uInvalidUidList = tRuntime.m_idUser;
                uInvalidUidList++;
            }
        }

        const nn::Result selectResult = nn::account::ShowUserSelector(&idUser, tSettings);
        if (selectResult.IsFailure())
        {
            Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, SignInCallback>, const SignInResult &>::bind<&OnlineService::internal::callback_base<1, SignInCallback>::execute>(pfnCallback, SIGNIN_FAILED));
            return;
        }


        // 541/543 columns: the open is in its own block, the guest flag in the else (no row of its own).
        if (idUser != nn::account::InvalidUid)
        {
            const nn::Result openResult = nn::account::OpenUser(&tUserHandle, idUser);
            if (openResult.IsFailure())
            {
                Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, SignInCallback>, const SignInResult &>::bind<&OnlineService::internal::callback_base<1, SignInCallback>::execute>(pfnCallback, SIGNIN_FAILED));
                return;
            }
        }
        else
            bGuest = true;
    }
    ScopedUsersWriteLock tScopedLock;
    UserRuntime &tRuntime = s_atRuntime[nUserIdx];
    tRuntime.Clear();


    if (bGuest)
    {
        AccountDefinition_ToDefaults_ForConsoleClient(tRuntime.m_tAccountDefinition, nUserIdx);
        tRuntime.m_tProfile.mutable_account_profile();
        tRuntime.m_bActive = TRUE;
        tRuntime.m_bAccountLoadComplete = TRUE;
        tRuntime.m_bGuest = TRUE;


        int32 nGuestIndex = -1;
        for (int32 i = 0; i < 3; ++i)
        {
            BOOL bIndexUsed = s_atGuestIndices[i];
            if (!bIndexUsed)
            {
                nGuestIndex = i;
                s_atGuestIndices[nGuestIndex] = TRUE;
                tRuntime.nGuestIndex = nGuestIndex;
                break;
            }
        }



        strncpy(tRuntime.m_tNickname.name, GetGamerTag(GetPrimaryProfileUserIndex()), 30);
        // 587:56 (the .str() call) fits FormatNumber(nGuestIndex + 2) with the UnicodeText.h default arguments.
        CRefString LocalizedName = LocalizedSprintf(
            StringListFetch("Console:GuestLabelNX64").str(),
            tRuntime.m_tNickname.name,
            UnicodeText::FormatNumber(nGuestIndex + 2).str(),
            NULL);
        strncpy(tRuntime.m_tNickname.name, LocalizedName.str(), 32);

        Internal::QueueCallback(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, SignInCallback>, const SignInResult &>::bind<&OnlineService::internal::callback_base<1, SignInCallback>::execute>(pfnCallback, SIGNIN_SUCCESS_GUEST));
        return;
    }

    tRuntime.m_idUser = idUser;
    tRuntime.m_tUserHandle = tUserHandle;
    nn::account::GetNickname(&tRuntime.m_tNickname, idUser);

    bool bIsLocalCommunication = Online::GetNetworkMode() == Online::NET_ADHOC;

    RefreshPlayHistoryRegistrationKey(nUserIdx, bIsLocalCommunication);


    if (bIsFirstUser)
    {
        Internal::SetPrimaryProfileUserIndex(nUserIdx);
    }


    if (Online::IsPhysicalNetworkConnected())
    {
        tRuntime.m_bEnsureAccountAvailability = TRUE;
        sbEnsureAvailabilityDirty = TRUE;
    }


    UpdatePrivileges(TRUE);


    Online::PresenceManager::Get(nUserIdx)->OnNX64UserSignedIn();

    Online::LobbyService::AttemptConnectOnUser(nUserIdx);


    LoadAccountTask *ptLoadAccountTask = new LoadAccountTask(nUserIdx, eMode, pfnCallback);
    ptLoadAccountTask->Queue();
}
}
}
