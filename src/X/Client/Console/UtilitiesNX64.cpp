// X/Client/Console/UtilitiesNX64.cpp
// Switch (NX64) console utilities: keyboard, profile and store UI, game invites and the system language.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"
#include "../Gfx/GfxVB.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../NintendoSDK/Include/nn/account/account_Types.h"
#include "../../../NintendoSDK/Include/nn/friends/friends_Types.h"
#include "../../../NintendoSDK/Include/nn/settings/settings_Language.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/UnicodeText.h"
#include "Utilities.h"
#include <string.h>

// Nintendo SDK entry points (nn/oe/oe_LanguageApi.h, nn/nifm/nifm_Api.h, nn/friends/friends_Api.h,
// nn/settings/settings_Language.h); the SDK headers are not in src/.
namespace nn { namespace oe { nn::settings::LanguageCode GetDesiredLanguage(); } }
namespace nn { namespace nifm { void HandleNetworkRequestResult(); } }
namespace nn { namespace friends {
void ShowUserDetailInfo(const nn::account::Uid & uid, const nn::account::NetworkServiceAccountId & id, const nn::friends::InAppScreenName & userName, const nn::friends::InAppScreenName & profileOwnerName);
} }
// settings_Language.h:105 and :119 are DWARF-inline operators over this SDK import.
namespace nn { namespace settings {
bool operator==(const LanguageCode & lhs, const LanguageCode & rhs);
inline bool operator==(const Language & lhs, const LanguageCode & rhs) { return LanguageCode::Make(lhs) == rhs; }
inline bool operator==(const LanguageCode & lhs, const Language & rhs) { return rhs == lhs; }
} }
// GamerProfileNX64.cpp:734 and :910.
namespace Console { namespace GamerProfile {
nn::account::Uid GetUserId(int32 nUserIndex);
nn::account::Nickname GetUserNickName(int32 nUserIndex);
} }

namespace Console {
namespace Utilities {

















































void CloseKeyboard()
{


}




void ShowProfile(int32 nUserIndex,
                 uint64 idTarget,
                 const char * pUserNickname,
                 int32 nSizeOfNickname)

{
    nn::friends::InAppScreenName UserName;
    nn::friends::InAppScreenName ProfileOwnerName;
    const nn::settings::LanguageCode tLanguage = nn::oe::GetDesiredLanguage();
    UserName.language = tLanguage;
    ProfileOwnerName.language = tLanguage;

    nn::account::Nickname tUsersNickname = GamerProfile::GetUserNickName(nUserIndex);
    memcpy(UserName.name, tUsersNickname.name, sizeof(tUsersNickname.name));  // INFERENCE: direct memcpy of 0x21 bytes (0x67500), not Blizzard::String::Copy



    int32 nSizeToCopy = Math::Min(nSizeOfNickname, 64);  // INFERENCE: cap 64 = sizeof(InAppScreenName::name), from cmp w19,#0x40
    memcpy(ProfileOwnerName.name, pUserNickname, nSizeToCopy);

    nn::account::NetworkServiceAccountId idTargetNSA;
    idTargetNSA.id = idTarget;

    nn::friends::ShowUserDetailInfo(GamerProfile::GetUserId(nUserIndex), idTargetNSA, UserName, ProfileOwnerName);
}









































































void ShowSystemOnlineErrorDialog()
{
    nn::nifm::HandleNetworkRequestResult();
}




BOOL ShowGameInviteUI(int32 nControllerIndex)
{




    return FALSE;  // INFERENCE: whole body is mov w0,wzr (0x67890); the original may hold platform-specific code that compiled away
}







































XLocale GetSystemLanguage()
{

    XLocale languageOut = XLOCALE_INVALID;  // INFERENCE: default 0 from the final csel against wzr (0x67cfc); locale per language from the immediates in each arm
    const nn::settings::LanguageCode languageIn = nn::oe::GetDesiredLanguage();

    if (languageIn == nn::settings::Language_Japanese)
    {
        languageOut = XLOCALE_JAPANESE;
    }
    else if (languageIn == nn::settings::Language_AmericanEnglish)
    {
        languageOut = XLOCALE_ENGLISH_US;
    }
    else if (languageIn == nn::settings::Language_French)
    {
        languageOut = XLOCALE_FRENCH;
    }
    else if (languageIn == nn::settings::Language_German)
    {
        languageOut = XLOCALE_GERMAN;
    }
    else if (languageIn == nn::settings::Language_Italian)
    {
        languageOut = XLOCALE_ITALIAN;
    }
    else if (languageIn == nn::settings::Language_Spanish)
    {
        languageOut = XLOCALE_SPANISH_EU;
    }
    else if (languageIn == nn::settings::Language_Chinese)
    {
        languageOut = XLOCALE_CHINESE_CHINA;
    }
    else if (languageIn == nn::settings::Language_Korean)
    {
        languageOut = XLOCALE_KOREAN;
    }
    else if (languageIn == nn::settings::Language_Dutch)
    {
        languageOut = XLOCALE_ENGLISH_EU;
    }
    else if (languageIn == nn::settings::Language_Portuguese)
    {
        languageOut = XLOCALE_PORTUGUESE_BRAZIL;
    }
    else if (languageIn == nn::settings::Language_Russian)
    {
        languageOut = XLOCALE_RUSSIAN;
    }
    else if (languageIn == nn::settings::Language_Taiwanese)
    {
        languageOut = XLOCALE_CHINESE_TAIWAN;
    }
    else if (languageIn == nn::settings::Language_BritishEnglish)
    {
        languageOut = XLOCALE_ENGLISH_EU;
    }
    else if (languageIn == nn::settings::Language_CanadianFrench)
    {
        languageOut = XLOCALE_FRENCH;
    }
    else if (languageIn == nn::settings::Language_LatinAmericanSpanish)
    {
        languageOut = XLOCALE_SPANISH_LATAM;
    }
    else if (languageIn == nn::settings::Language_SimplifiedChinese)
    {
        languageOut = XLOCALE_CHINESE_CHINA;
    }
    else if (languageIn == nn::settings::Language_TraditionalChinese)
    {
        languageOut = XLOCALE_CHINESE_TAIWAN;
    }
    return languageOut;
}
}  // namespace Utilities
}  // namespace Console

#include "../../../NintendoSDK/Include/nn/nn_Result.h"
#include "GamerProfileInternal.h"
#include "LobbyService.h"
#include "PresenceManager.h"
#include "../UserInterface/Console/ConsoleScreenManager.h"
#include "../UserInterface/Console/ConsoleModalNotificationManager.h"
// StringList.cpp original declaration.
CRefString StringListFetch(LPCSTR szStringName);
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdBitBuffer.h"
// nn/swkbd SDK layouts recovered completely from original DWARF, kept local.
namespace nn {
namespace swkbd {
enum KeyboardMode {
    KeyboardMode_Full = 0,
    KeyboardMode_Numeric = 1,
    KeyboardMode_ASCII = 2,
    KeyboardMode_FullLatin = 3,
    KeyboardMode_Max = 4,
    KeyboardMode_Force32 = -1,
};
enum InitialCursorPos {
    InitialCursorPos_First = 0,
    InitialCursorPos_Last = 1,
    InitialCursorPos_Max = 2,
    InitialCursorPos_Force32 = -1,
};
enum PasswordMode {
    PasswordMode_Show = 0,
    PasswordMode_Hide = 1,
    PasswordMode_Max = 2,
    PasswordMode_Force32 = -1,
};
enum InputFormMode {
    InputFormMode_OneLine = 0,
    InputFormMode_MultiLine = 1,
    InputFormMode_Separate = 2,
    InputFormMode_Max = 3,
    InputFormMode_Force32 = -1,
};
enum TextCheckResult {
    TextCheckResult_Success = 0,
    TextCheckResult_ShowFailureDialog = 1,
    TextCheckResult_ShowConfirmDialog = 2,
    TextCheckResult_Max = 3,
    TextCheckResult_Force32 = -1,
};
enum Preset {
    Preset_Default = 0,
    Preset_Password = 1,
    Preset_UserName = 2,
    Preset_DownloadCode = 3,
    Preset_Max = 4,
    Preset_Force32 = -1,
};
struct String;
typedef TextCheckResult (*TextCheckCallback)(void*, size_t*, String*);
struct String {
    void * ptr;
    size_t bufSize;
};
static_assert(sizeof(String) == 0x10, "String");
struct KeyboardConfig {
    KeyboardMode keyboardMode;
    char16_t okText[9];
    char16_t leftOptionalSymbolKey;
    char16_t rightOptionalSymbolKey;
    bool isPredictionEnabled;
    uint32_t invalidCharFlag;
    InitialCursorPos initialCursorPos;
    char16_t headerText[65];
    char16_t subText[129];
    char16_t guideText[257];
    int32_t textMaxLength;
    int32_t textMinLength;
    PasswordMode passwordMode;
    InputFormMode inputFormMode;
    bool isUseNewLine;
    bool isUseUtf8;
    bool isUseBlurBackground;
    int32_t _initialStringOffset;
    int32_t _initialStringLength;
    int32_t _userDictionaryOffset;
    int32_t _userDictionaryNum;
    bool _isUseTextCheck;
    TextCheckCallback _textCheckCallback;
    int32_t separateTextPos[8];
};
static_assert(sizeof(KeyboardConfig) == 0x0400, "KeyboardConfig");
struct ShowKeyboardArg {
    KeyboardConfig keyboardConfig;
    void * workBuf;
    size_t workBufSize;
    void * textCheckWorkBuf;
    size_t textCheckWorkBufSize;
};
static_assert(sizeof(ShowKeyboardArg) == 0x0420, "ShowKeyboardArg");
void MakePreset(KeyboardConfig*, Preset);
void SetHeaderTextUtf8(KeyboardConfig*, const char*);
void SetSubTextUtf8(KeyboardConfig*, const char*);
void SetGuideTextUtf8(KeyboardConfig*, const char*);
size_t GetRequiredWorkBufferSize(bool);
size_t GetRequiredStringBufferSize();
void SetInitialTextUtf8(ShowKeyboardArg*, const char*);
nn::Result ShowKeyboard(String*, const ShowKeyboardArg&);
}
namespace account {
nn::Result ShowLicenseRequirementsForNetworkService(const UserHandle&);
}
}
namespace Console {
namespace GamerProfile {
nn::account::UserHandle GetUserHandle(int32);
void ClearAccountTokenDirty(int32);
void SetAccountTokenDirty(int32);
BOOL IsAccountTokenDirty(int32);
}
namespace Utilities {
typedef OnlineService::Callback<void(int, CRefString)> StringInputCallback;
typedef OnlineService::Callback<void()> ShowStoreCallback;
// textMaxLength is stored before keyboardMode: original rows 63:46 then 64:45 (store order 0x3f4 then 0x48).
#line 55 "X/Client/Console/UtilitiesNX64.cpp"
void ShowKeyboardForUser(int32 nUserIndex, LPCSTR szDefaultText, LPCSTR szDialogTitle, LPCSTR szDialogMessage, uint32 dwMaxTextLength, StringInputCallback cb)
{
    nn::swkbd::ShowKeyboardArg arg;
    nn::swkbd::MakePreset(&arg.keyboardConfig, nn::swkbd::Preset_Default);
    nn::swkbd::SetHeaderTextUtf8(&arg.keyboardConfig, szDialogTitle);
    if (szDialogMessage[0]) nn::swkbd::SetSubTextUtf8(&arg.keyboardConfig, szDialogMessage);
    nn::swkbd::SetGuideTextUtf8(&arg.keyboardConfig, szDefaultText);
    arg.keyboardConfig.inputFormMode = nn::swkbd::InputFormMode_OneLine;
    arg.keyboardConfig.isUseNewLine = false;
    arg.keyboardConfig.isUseUtf8 = true;
    arg.keyboardConfig.textMaxLength = dwMaxTextLength;
    arg.keyboardConfig.keyboardMode = nn::swkbd::KeyboardMode_Full;
    arg.workBufSize = nn::swkbd::GetRequiredWorkBufferSize(false);
    void* workBuffer = SigmaMemoryNew(arg.workBufSize, 4096, 0, FALSE);
    arg.workBuf = workBuffer;
    nn::swkbd::String outString;
    outString.bufSize = nn::swkbd::GetRequiredStringBufferSize();
    void* outBuffer = SigmaMemoryNew(outString.bufSize, 4096, 0, FALSE);
    outString.ptr = outBuffer;
    SigmaMemoryClear(outBuffer, outString.bufSize);
    nn::swkbd::SetInitialTextUtf8(&arg, szDefaultText);
    nn::Result result = nn::swkbd::ShowKeyboard(&outString, arg);
    {
        // DWARF: method_binder_2::bind<&execute, Callback, bool, const char *> inlined at line 82 inside QueueCallback(...), no named local.
        GamerProfile::Internal::QueueCallback(OnlineService::internal::method_binder_2<void, OnlineService::internal::callback_base<2, StringInputCallback>, int, const CRefString&>::bind<&OnlineService::internal::callback_base<2, StringInputCallback>::execute>(cb, result.IsSuccess(), static_cast<const char*>(outBuffer)));

    }
    SigmaMemoryFree(workBuffer, 0);
    SigmaMemoryFree(outBuffer, 0);
}
#line 138 "X/Client/Console/UtilitiesNX64.cpp"
void NSOSubscriptionInfoReceived(int32 nControllerIndex, ModalNotificationId modId, OnUpsellCallback callback)
{
    Screens::GetModalNotificationManager().RemoveNotification(modId);
    callback();
}
#line 146 "X/Client/Console/UtilitiesNX64.cpp"
void NSOVerifySubscriptionAfterUpsell(int32 nControllerIndex, OnUpsellCallback callback)
{
    GamerProfile::ClearAccountTokenDirty(nControllerIndex);
    NotificationCreationParams tParams;
    tParams.nControllerIndex = nControllerIndex;
    tParams.uszMessage = StringListFetch("ConsoleErrors:ValidatingNSO");
    ModalNotificationId ModId = Screens::GetModalNotificationManager().SetMessageOnlyNotification(tParams);
    Online::LobbyService::VerifyNSOSubscriptionAsync(nControllerIndex, OnlineService::internal::function_binder_3<void, int, int, OnUpsellCallback>::bind<NSOSubscriptionInfoReceived>(nControllerIndex, ModId, callback));
    Online::LobbyService::RunTokenAuthentication(nControllerIndex);
}
#line 166 "X/Client/Console/UtilitiesNX64.cpp"
        void ShowNX64Upsell(
            int32 nControllerIndex,
            OnUpsellCallback successCB,
            OnUpsellCallback cancelCB)
        {
            BOOL bRefreshTokenAuthentication = GamerProfile::IsAccountTokenDirty(nControllerIndex);

            if (bRefreshTokenAuthentication)
            {
                NSOVerifySubscriptionAfterUpsell(nControllerIndex, successCB);
            }
            else
            {
                nn::account::UserHandle tUserHandle = GamerProfile::GetUserHandle(nControllerIndex);

                GamerProfile::SetAccountTokenDirty(nControllerIndex);
                const nn::Result selectResult = nn::account::ShowLicenseRequirementsForNetworkService(tUserHandle);

                if (selectResult.IsSuccess())
                {
                    NSOVerifySubscriptionAfterUpsell(nControllerIndex, successCB);
                }
                else
                {
                    cancelCB();
                }
            }
        }
#line 217 "X/Client/Console/UtilitiesNX64.cpp"
BOOL SendGameInvite(int32 nControllerIndex, uint64 idPlayer)
{
    Online::PresenceManager* ptPresenceManager = Online::PresenceManager::Get(nControllerIndex);
    const Online::PartyConnectionInfo& tConnectionInfo = ptPresenceManager->GetConnectionInfo();
    bdReference<bdBitBuffer> ptBuffer = ::new bdBitBuffer(0, false);
    ptBuffer->writeUInt32(0x109);
    ptBuffer->writeUInt32(1);
    tConnectionInfo.Serialize(ptBuffer);
    BOOL bStartedTask = Online::LobbyService::Get(nControllerIndex)->SendGlobalInstantMessage(idPlayer, ptBuffer->getData(), ptBuffer->getDataSize());
    return bStartedTask;
}
#line 246 "X/Client/Console/UtilitiesNX64.cpp"
void ShowStoreUI(int32 nControllerIndex, const char* szTarget, bool isCategory, ShowStoreCallback cb)
{
    GamerProfile::Internal::QueueCallback(cb);
}
}
}
