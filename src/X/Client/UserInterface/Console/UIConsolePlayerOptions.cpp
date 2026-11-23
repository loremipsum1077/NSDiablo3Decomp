// Original file: X/Client/UserInterface/Console/UIConsolePlayerOptions.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Controls/UIConsoleListboxControl.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser
// runs them ahead of Online.h's nn::ldn constants.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/Online.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/Utilities.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/RefString.h"
#include "../../Sound/Sound.h"
#include "../../../../NintendoSDK/Include/nn/account/account_Types.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "ConsoleScreenManager.h"
#include "../../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "../Controls/UITextControl.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "UIConsoleMainMenu.h"
#include "UIConsolePartyDisplay.h"
#include "UIConsolePlayerOptions.h"
// VoiceChat.h's declarations (VoiceChat.cpp:22, :82, :116).
namespace Console { namespace Online {
typedef int32 PartyMuteState;
BOOL IsVoiceChatEnabled();
PartyMuteState IsPlayerMuted(int32 nTargetPartyMemberIndex);
void SetPlayerMuted(int32 nControllerIndexInitatedTheMuting, int32 nPartyMemberIndexToBeMuted, BOOL bMuted);
} }
namespace Console { namespace UIOnlineActions { void JoinParty(::Console::Online::PartyConnectionInfo & tConnectionInfo, ::Console::Online::PartyJoinCallback pfnCallback); } }  // OnlineActions.cpp
const UIID & UI_GetControllerFocus();  // UI.cpp
namespace UIProfileSummary { namespace Console { void OpenForController(int32 nControllerIndex); } }  // UIConsoleProfileSummary.cpp
namespace Console { namespace Online { BOOL PresenceGetFriendParty(int32 nUserIndex, uint64 idFriend, PartyInfo & tPartyInfo); } }  // Presence
void UI_ConstrainRectToViewport(Rect2D & rect, float flIndent);  // UIUtils.cpp
namespace UIPartyDisplay { namespace Console { void OnAdHocPartyJoined(::Console::Online::JoinResult eResult); } }  // UIConsolePartyDisplay.cpp
namespace UIPlayerOptions { namespace Console {
enum PlayerOptions {
    INSPECT_PLAYER = 0,
    VIEW_PROFILE_OF_PLAYER = 1,
    VIEW_GAMERCARD = 2,
    VIEW_HEROPROFILE = 3,
    INVITE_PLAYER = 4,
    JOIN_PLAYER = 5,
    MUTE_PLAYER = 6,
    UNMUTE_PLAYER = 7,
};

class PlayerOptionsDataSource : public ConsoleListBoxDataSource {
public:
    PlayerOptionsDataSource() : m_ptPlayerInfo(NULL) {}
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    virtual BOOL IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex);
    CRefString GetOptionLabel(PlayerOptions eOption) { switch (eOption) { case VIEW_GAMERCARD: return StringListFetch("Console:PlayerOptionViewUserPageNX64"); case VIEW_HEROPROFILE: return StringListFetch("Console:ViewProfile"); case INVITE_PLAYER: return StringListFetch("Console:PlayerOptionInvite"); case JOIN_PLAYER: return StringListFetch("Console:PlayerOptionJoin"); case MUTE_PLAYER: return StringListFetch("Console:PlayerOptionMute"); case UNMUTE_PLAYER: return StringListFetch("Console:PlayerOptionUnmute"); default: return CRefString(); } }  // defined in the class in the original: its DWARF definition DIE has no decl_line of its own and the symbol is weak
    void Initialize(PlayerInfo * ptPlayer);
    blz::vector<PlayerOptions> m_menuItems;
    PlayerInfo * m_ptPlayerInfo;
};

BOOL CanShowGamerCard(int32 nLocalUserIndex, ::Console::Online::PartyMember & tPartyMember);
BOOL CanShowHeroProfile(int32 nLocalUserIndex, ::Console::Online::PartyMember & tPartyMember);

int32 PlayerOptionsDataSource::GetNumItems() const
{
    return m_menuItems.size();
}

UIControlSourceKey PlayerOptionsDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsolePlayerOptions:ConsolePlayerOptions_main.PlayerOptionsListItemTemplate", -1, NULL, NULL);
}

void PlayerOptionsDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}

BOOL PartyPlayerInfo::CanInviteToGame() const
{
    return FALSE;
}

void PartyPlayerInfo::InviteToGame()
{
}

BOOL PartyPlayerInfo::IsInJoinableGame() const
{
    return FALSE;
}

BOOL PartyPlayerInfo::CanJoinGame() const
{
    return FALSE;
}

void PartyPlayerInfo::JoinGame()
{
}

BOOL PartyPlayerInfo::CanMute() const
{
    return ::Console::Online::IsVoiceChatEnabled() && ::Console::Online::IsPlayerMuted(m_nPartyMemberIndex) == 0 && !m_ptPartyMember->bIsLocal;
}

void PartyPlayerInfo::Mute()
{
    if (CanMute() && m_nPartyMemberIndex >= 0 && m_nLocalUserIndex >= 0)
    {
        ::Console::Online::SetPlayerMuted((uint8)m_nLocalUserIndex, m_nPartyMemberIndex, TRUE);
    }
}

BOOL PartyPlayerInfo::CanUnMute() const
{
    return ::Console::Online::IsVoiceChatEnabled() && ::Console::Online::IsPlayerMuted(m_nPartyMemberIndex) != 0 && !m_ptPartyMember->bIsLocal;
}

void PartyPlayerInfo::UnMute()
{
    if (CanUnMute() && m_nLocalUserIndex >= 0 && m_nPartyMemberIndex >= 0)
    {
        ::Console::Online::SetPlayerMuted((uint8)m_nLocalUserIndex, m_nPartyMemberIndex, FALSE);
    }
}

BOOL PartyPlayerInfo::CanShowGamerCard()
{
    return UIPlayerOptions::Console::CanShowGamerCard(m_nLocalUserIndex, *m_ptPartyMember);
}

BOOL PartyPlayerInfo::CanShowHeroProfile()
{
    return UIPlayerOptions::Console::CanShowHeroProfile(m_nLocalUserIndex, *m_ptPartyMember);
}

void PartyPlayerInfo::ShowGamerCard()
{
    if (CanShowGamerCard())
    {
        ::Console::Utilities::ShowProfile(m_nLocalUserIndex, m_ptPartyMember->idOnlineUser, m_ptPartyMember->szName, sizeof(m_ptPartyMember->szName));
    }
}

FriendPlayerInfo::FriendPlayerInfo(int32 nLocalUserIndex, ::Console::Online::Friend * ptFriend)
    : PlayerInfo(nLocalUserIndex), m_ptFriend(ptFriend)
{
}

BOOL FriendPlayerInfo::CanInviteToGame() const
{
    return ::Console::Online::PartyIsOnline() && ::Console::Online::PartyGetMemberCount() < 4;
}

void FriendPlayerInfo::InviteToGame()
{
    ::Console::Utilities::SendGameInvite(m_nLocalUserIndex, m_ptFriend->m_idFriend);
}

BOOL FriendPlayerInfo::IsInJoinableGame() const
{
    return m_ptFriend->m_bInGame && m_ptFriend->m_tGamePresence.m_eOnlineState > ::Console::Online::PARTY_ONLINE_INVITEONLY;
}

BOOL FriendPlayerInfo::CanJoinGame() const
{
    uint32 nNumPartyMembers = ::Console::Online::PartyGetMemberCountToJoinOnlineGame();
    return IsInJoinableGame() && 4 - m_ptFriend->m_tGamePresence.m_uNumPlayers >= nNumPartyMembers;
}

BOOL FriendPlayerInfo::CanMute() const
{
    return FALSE;
}

void FriendPlayerInfo::Mute()
{
}

BOOL FriendPlayerInfo::CanUnMute() const
{
    return FALSE;
}

void FriendPlayerInfo::UnMute()
{
}

BOOL FriendPlayerInfo::CanShowGamerCard()
{
    return TRUE;
}

BOOL FriendPlayerInfo::CanShowHeroProfile()
{
    return FALSE;
}

void LocalCommPlayerInfo::JoinGame()
{
    ::Console::UIOnlineActions::JoinParty(*m_ptPartyInfo, OnlineService::internal::function_bind_0_1<void, ::Console::Online::JoinResult, &UIPartyDisplay::Console::OnAdHocPartyJoined>());
}

void FriendPlayerInfo::ShowGamerCard()
{
    if (CanShowGamerCard())
    {
        ::Console::Utilities::ShowProfile(m_nLocalUserIndex, m_ptFriend->m_idFriend, m_ptFriend->m_szUsername, sizeof(m_ptFriend->m_szUsername));
    }
}

struct PlayerOptionsData {
    PlayerOptionsDataSource tPlayerOptionsList;
    PlayerInfo * ptPlayerInfo;
    UIID idPriorControllerFocus;
    UIID idParent;
    PlayerOptionsData();
};

inline PlayerOptionsData::PlayerOptionsData()
    : ptPlayerInfo(NULL), idPriorControllerFocus(GetUIIDNone()), idParent(GetUIIDNone())
{
}

PlayerOptionsData stPlayerOptionsData;

BOOL IsVisible()
{
    return UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS).IsActive();
}

BOOL HasParent(const UIID & idParent)
{
    return stPlayerOptionsData.idParent == idParent;
}

void OnPlayerOptionsCancel(const UIID & id, const int32 nControllerIndex)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    if (!UIPartyDisplay::Console::IsNamePlateSelected())
    {
        ClosePlayerOptions();
        return;
    }
    if (::Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex)
        UIMainMenu::Console::OnCancel(id, nControllerIndex);
    else
        UIMainMenu::Console::OnLocalLeaveButtonPressed(id, nControllerIndex);
}

BOOL CanShowGamerCard(int32 nLocalUserIndex, ::Console::Online::PartyMember & tPartyMember)
{
    if (!::Console::GamerProfile::IsSignedIn(nLocalUserIndex) || !::Console::Online::PartyIsOnline())
        return FALSE;
    if (tPartyMember.bIsGuest)
        return FALSE;
    nn::account::NetworkServiceAccountId tLocalUserNSA;
    if (!::Console::GamerProfile::GetUserNetworkServiceAccountId(nLocalUserIndex, tLocalUserNSA))
        return FALSE;
    return tPartyMember.idOnlineUser != tLocalUserNSA.id;
}

inline BOOL CanShowHeroProfile(int32 nLocalUserIndex, ::Console::Online::PartyMember & tPartyMember)
{
    return tPartyMember.bIsLocal && tPartyMember.nControllerIndex == nLocalUserIndex;
}

namespace {
void ResizeListContent();
void ShowPlayerOptions(PlayerInfo * ptPlayer, const UIID & idParent);
}

inline PartyPlayerInfo::PartyPlayerInfo(int32 nLocalUserIndex, ::Console::Online::PartyMember * ptPartyMember)
    : PlayerInfo(nLocalUserIndex), m_nPartyMemberIndex(ptPartyMember->idPlayer), m_ptPartyMember(ptPartyMember)
{
}

inline LocalCommPlayerInfo::LocalCommPlayerInfo(int32 nLocalUserIndex, int32 nRemotePlayerIndex, ::Console::Online::PartyInfo * ptParty)
    : PlayerInfo(nLocalUserIndex), m_nRemotePlayerIndex(nRemotePlayerIndex), m_ptPartyInfo(ptParty)
{
}

void ShowPartyPlayerOptions(int32 nLocalUserIndex, ::Console::Online::PartyMember * ptPartyMember, const UIID & idParent)
{
    if (ptPartyMember)
    {
        ::delete stPlayerOptionsData.ptPlayerInfo;
        stPlayerOptionsData.ptPlayerInfo = NULL;
        ShowPlayerOptions(new PartyPlayerInfo(nLocalUserIndex, ptPartyMember), idParent);
    }
}

namespace {
void ShowPlayerOptions(PlayerInfo * ptPlayer, const UIID & idParent)
{
    if (ptPlayer && !UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS).IsActive())
    {
        stPlayerOptionsData.idParent = idParent;
        stPlayerOptionsData.ptPlayerInfo = ptPlayer;
        stPlayerOptionsData.tPlayerOptionsList.Initialize(ptPlayer);
        if (!UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS).IsActive())
        {
            UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS).SetActive(TRUE);
            UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_PLAYER_OPTIONS);
            tList.SetDataSource(stPlayerOptionsData.tPlayerOptionsList);
            tList.ResetAllItems();
            ptPlayer->DisplayInfo();
            ResizeListContent();
            stPlayerOptionsData.idPriorControllerFocus = UI_GetControllerFocus();
            UI_SetControllerFocus(UILIST_CONSOLE_PLAYER_OPTIONS);
        }
    }
}
}

void ShowFriendPlayerOptions(int32 nLocalUserIndex, ::Console::Online::Friend * ptFriend, const UIID & idParent)
{
    if (ptFriend)
    {
        ::delete stPlayerOptionsData.ptPlayerInfo;
        stPlayerOptionsData.ptPlayerInfo = NULL;
        ShowPlayerOptions(new FriendPlayerInfo(nLocalUserIndex, ptFriend), idParent);
    }
}

void ShowLocalCommPlayerOptions(int32 nLocalUserIndex, int32 nRemotePlayerIndex, ::Console::Online::PartyInfo * ptParty, const UIID & idParent)
{
    if (ptParty)
    {
        ::delete stPlayerOptionsData.ptPlayerInfo;
        stPlayerOptionsData.ptPlayerInfo = NULL;
        ShowPlayerOptions(new LocalCommPlayerInfo(nLocalUserIndex, nRemotePlayerIndex, ptParty), idParent);
    }
}

void ClosePlayerOptions()
{
    if (UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS).IsActive())
    {
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_PLAYER_OPTIONS).SetSelectedIndex(-1);
        UI_SetControllerFocus(stPlayerOptionsData.idPriorControllerFocus);
        stPlayerOptionsData.idPriorControllerFocus = GetUIIDNone();
        UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_ERROR).SetActive(FALSE);
    }
    ::delete stPlayerOptionsData.ptPlayerInfo;
    stPlayerOptionsData.ptPlayerInfo = NULL;
}

void RefreshPlayerOptions(PlayerInfo * ptPlayerInfo)
{
    if (UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS).IsActive())
    {
        if (ptPlayerInfo)
        {
            ::delete stPlayerOptionsData.ptPlayerInfo;
            stPlayerOptionsData.ptPlayerInfo = ptPlayerInfo;
        }
        stPlayerOptionsData.tPlayerOptionsList.Initialize(stPlayerOptionsData.ptPlayerInfo);
        UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_PLAYER_OPTIONS);
        tList.SetDataSource(stPlayerOptionsData.tPlayerOptionsList);
        tList.RefreshVisibleItems();
        stPlayerOptionsData.ptPlayerInfo->DisplayInfo();
        ResizeListContent();
    }
}


void PlayerOptionsDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    UIID idOption = UIIDCreateFromParent(idItemUI, ".Option");
    UIID idHighlight = UIIDCreateFromParent(idItemUI, ".highlight");
    CRefString strLabel = GetOptionLabel(m_menuItems[nIndex]);
    UIText & tText = UI_GetTextFromID(idOption);
    tText.SetText(strLabel, FALSE);
    if (!IsItemEnabled(tListBox, nIndex))
        tText.SetTextColor(crgbaGray);
    else
        tText.SetTextColor(tListBox.GetSelectedIndex() == nIndex ? crgbaWhite : crgbaGold);
    UI_GetWindowFromID(idHighlight).SetActive(tListBox.GetSelectedIndex() == nIndex);
}

BOOL PlayerOptionsDataSource::IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex)
{
    switch (m_menuItems[nIndex])
    {
    case VIEW_GAMERCARD: return m_ptPlayerInfo->CanShowGamerCard();
    case VIEW_HEROPROFILE: return m_ptPlayerInfo->CanShowHeroProfile();
    case INVITE_PLAYER: return m_ptPlayerInfo->CanInviteToGame();
    case JOIN_PLAYER: return m_ptPlayerInfo->CanJoinGame();
    case MUTE_PLAYER: return m_ptPlayerInfo->CanMute();
    case UNMUTE_PLAYER: return m_ptPlayerInfo->CanUnMute();
    default: return TRUE;
    }
}















namespace {
void ResizeListContent()
{
    UIWindow & tPlayerOptionsWin = UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS);
    UIWindow & tPlayerOptionsNotification = UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_NOTIFICATION);
    UIWindow & tPlayerOptionsContent = UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_CONTENT);
    Vector2D vecContentTL;
    Vector2D vecContentDim;
    Vector2D vecListTL;
    Vector2D vecListDim;
    UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_LIST_CONTENT).GetRelativePosition(vecListTL, vecListDim);
    tPlayerOptionsContent.GetRelativePosition(vecContentTL, vecContentDim);
    vecContentDim.y = vecListDim.y;
    tPlayerOptionsContent.SetRelativePosition(vecContentTL, vecContentDim);

    Vector2D vecOptionsTL;
    Vector2D vecOptionsDim;
    Vector2D vecNotificationTL;
    Vector2D vecNotificationDim;
    tPlayerOptionsNotification.GetRelativePosition(vecNotificationTL, vecNotificationDim);
    tPlayerOptionsWin.GetRelativePosition(vecOptionsTL, vecOptionsDim);
    vecOptionsDim.y = vecNotificationDim.y;
    tPlayerOptionsWin.SetRelativePosition(vecOptionsTL, vecOptionsDim);

    AlignPosition eAlignHorizontal;
    AlignPosition eAlignVertical;
    UI_GetWindowFromID(stPlayerOptionsData.idParent).GetTooltipAlignment(eAlignHorizontal, eAlignVertical);
    Vector2D vecFinalTL;
    Rect2D rectParent = UI_GetWindowFromID(stPlayerOptionsData.idParent).GetWindowRect();
    if (eAlignHorizontal != AP_NONE || eAlignVertical != AP_NONE)
    {
        Rect2D rectOptions = tPlayerOptionsWin.GetWindowRect();
        rectOptions.AlignToRect(rectParent, eAlignHorizontal, eAlignVertical, 10.0f);
        UI_ConstrainRectToViewport(rectOptions, 10.0f);
        vecFinalTL.x = rectOptions.left;
        vecFinalTL.y = rectOptions.top;
        vecFinalTL.SnapToIntegerCoords();
    }
    else
    {
        vecFinalTL = Vector2D(rectParent.left, rectParent.top);
    }
    tPlayerOptionsWin.MoveTo(vecFinalTL);
}
}

void FriendPlayerInfo::JoinGame()
{
    if (CanJoinGame())
    {
        ::Console::Online::PartyInfo tPartyInfo;
        if (::Console::Online::PresenceGetFriendParty(::Console::GamerProfile::GetPrimaryProfileUserIndex(), m_ptFriend->m_idFriend, tPartyInfo))
        {
            ::Console::UIOnlineActions::JoinParty(tPartyInfo, ::Console::Online::PartyJoinCallback());
        }
    }
}

void OnPlayerOptionsAccept(const UIID &, const int32)
{
    if (::Console::Screens::IsScreenTransitioning())
        return;
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    UIConsoleListBox & tPlayerOptions = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_PLAYER_OPTIONS);
    int32 nSelectedIndex = tPlayerOptions.GetSelectedIndex();
    if (nSelectedIndex == -1 || nSelectedIndex >= stPlayerOptionsData.tPlayerOptionsList.GetNumItems())
        return;
    PlayerOptions eOption = stPlayerOptionsData.tPlayerOptionsList.m_menuItems[nSelectedIndex];
    switch (eOption)
    {
    case VIEW_GAMERCARD:
        stPlayerOptionsData.ptPlayerInfo->ShowGamerCard();
        break;
    case VIEW_HEROPROFILE:
    {
        int32 nLocalUserIndex = stPlayerOptionsData.ptPlayerInfo->GetLocalUserIndex();
        if (!UIPartyDisplay::Console::IsNamePlateSelected())
            ClosePlayerOptions();
        UIProfileSummary::Console::OpenForController(nLocalUserIndex);
        return;
    }
    case INVITE_PLAYER:
        stPlayerOptionsData.ptPlayerInfo->InviteToGame();
        break;
    case JOIN_PLAYER:
        stPlayerOptionsData.ptPlayerInfo->JoinGame();
        break;
    case MUTE_PLAYER:
        stPlayerOptionsData.ptPlayerInfo->Mute();
        break;
    case UNMUTE_PLAYER:
        stPlayerOptionsData.ptPlayerInfo->UnMute();
        break;
    default:
        break;
    }
    if (!UIPartyDisplay::Console::IsNamePlateSelected())
        ClosePlayerOptions();
}
} }

#include "../../../Common/Player.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Common/Definitions/ActDefinition.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Sigma/UnicodeText.h"
// Declarations recovered from original corresponding source/DWARF.
extern UIID UITEXT_CONSOLE_PLAYER_OPTIONS_INFO;
extern UIID UIWND_CONSOLE_PLAYER_OPTIONS_FOOTER;
extern UIID UIWND_CONSOLE_PLAYER_OPTIONS_PORTRAIT;
extern UIID UITEXT_CONSOLE_PLAYER_OPTIONS_ERROR;
extern UIID UITEXT_CONSOLE_PLAYER_OPTIONS_QUEST;

CRefString CPlayerGetLevelString(int32 nLevel, int32 nAltLevel);
CRefString DisplayActName(Act eAct);
CRefString DisplayFormattedHandicap(int32 nHandicap);
CRefString QuestsGetUIName(SNO snoQuest);
SNO ActSNOFromActEnum(const Act eAct);
BOOL GameIsStartUp();
namespace UIUtils {
namespace Console {
void DisplayPortrait(PlayerClass eClass, BOOL bFemale, int32 nAltLevel, const UIID& idPortrait, GBID gbidPortraitFrame);
}
}
namespace Console {
namespace UIOnlineActions {
typedef int32 HeroValidationError;
HeroValidationError ValidateHeroForPartyMember(const ::Console::Online::PartyMember& ptPartyMember);
CRefString GetHeroValidationErrorMessage(HeroValidationError eValidationError, BOOL bIncludeInstructions);
}
}
namespace UIPlayerOptions {
namespace Console {
#line 983 "X/Client/UserInterface/Console/UIConsolePlayerOptions.cpp"
void ShowGamerCard(int32 nLocalUserIndex, uint64 m_idFriend, char* nUserNickname, int32 nSizeOfNickName)
{
    ::Console::Utilities::ShowProfile(nLocalUserIndex, m_idFriend, nUserNickname, nSizeOfNickName);
}
#line 227 "X/Client/UserInterface/Console/UIConsolePlayerOptions.cpp"
void PlayerOptionsDataSource::Initialize(PlayerInfo* ptPlayer)
{
    m_menuItems.clear();
    if (ptPlayer) {
        m_ptPlayerInfo = ptPlayer;
        if (ptPlayer->CanInviteToGame()) m_menuItems.push_back(INVITE_PLAYER);
        if (ptPlayer->IsInJoinableGame()) m_menuItems.push_back(JOIN_PLAYER);
        if (ptPlayer->CanShowGamerCard()) m_menuItems.push_back(VIEW_GAMERCARD);
        if (ptPlayer->CanShowHeroProfile()) m_menuItems.push_back(VIEW_HEROPROFILE);
        if (ptPlayer->CanMute()) m_menuItems.push_back(MUTE_PLAYER);
        if (ptPlayer->CanUnMute()) m_menuItems.push_back(UNMUTE_PLAYER);
    }
}
namespace {
#line 1079 "X/Client/UserInterface/Console/UIConsolePlayerOptions.cpp"
void DisplayPlayerInfo(CRefString sName, PlayerClass eClass, BOOL bFemale, int32 nLevel, int32 nAltLevel, BOOL bHardcore, BOOL bDeceased, BOOL bIsChallengeMode, GBID gbidPortraitFrame)
{
    CRefString sClass = PlayerGetClassUIName(eClass, !bFemale);
    CRefString sLevel = CPlayerGetLevelString(nLevel, nAltLevel);
    CRefString sInfoString = bHardcore ? StringListFetch("ConsoleUI:HardcorePlayerInfo") : StringListFetch("ConsoleUI:PlayerInfo");
    if (bIsChallengeMode) {
        sInfoString = StringListFetch("ConsoleUI:WeeklyChallengePlayerInfo");
        LanguageRules::SetClientGender(!bFemale);
    }
    else {
        LanguageRules::SetClientGender(!bFemale);
        if (bDeceased) sInfoString = LocalizedSprintf(StringListFetch("ConsoleHeroSelect:DeadHeroInfo").str(), sInfoString.str(), NULL);
    }
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        sInfoString = LocalizedSprintf(StringListFetch("ConsoleUI:PlayerInfoBlizzcon").str(), sName.str(), sClass.str(), NULL);
    else if (bIsChallengeMode)
        sInfoString = LocalizedSprintf(sInfoString.str(), sName.str());
    else
        sInfoString = LocalizedSprintf(sInfoString.str(), sName.str(), sLevel.str(), sClass.str(), NULL);
    UI_GetTextFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_INFO).SetText(sInfoString, FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_FOOTER).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_PORTRAIT).SetActive(!bIsChallengeMode);
    if (!bIsChallengeMode) UIUtils::Console::DisplayPortrait(eClass, bFemale, nAltLevel, UIWND_CONSOLE_PLAYER_OPTIONS_PORTRAIT, gbidPortraitFrame);
}
}
#line 398 "X/Client/UserInterface/Console/UIConsolePlayerOptions.cpp"
void PartyPlayerInfo::DisplayInfo()
{
    if (m_ptPartyMember->tHeroDigest) {
        GBID gbidPortraitFrame = -1;
        if (m_ptPartyMember->tHeroDigest->has_visual_equipment() && m_ptPartyMember->tHeroDigest->visual_equipment().cosmetic_item_size() > 2)
            gbidPortraitFrame = m_ptPartyMember->tHeroDigest->visual_equipment().cosmetic_item(3).gbid();
        ::Console::Online::GameParams tGameParams;
        BOOL bIsChallengeMode = ::Console::Online::PartyGetGameParams(tGameParams) && tGameParams.m_eGameType == 6;
        UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_PORTRAIT).SetActive(TRUE);
        DisplayPlayerInfo(m_ptPartyMember->szName, PlayerClassFromGBID(m_ptPartyMember->tHeroDigest->gbid_class()), (m_ptPartyMember->tHeroDigest->player_flags() >> 1) & 1, m_ptPartyMember->tHeroDigest->level(), m_ptPartyMember->tHeroDigest->deprecated_alt_level(), m_ptPartyMember->tHeroDigest->player_flags() & 1, (m_ptPartyMember->tHeroDigest->player_flags() >> 3) & 1, bIsChallengeMode, gbidPortraitFrame);
        if (GameIsStartUp()) {
            ::Console::UIOnlineActions::HeroValidationError eValidationError = ::Console::UIOnlineActions::ValidateHeroForPartyMember(*m_ptPartyMember);
            UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_ERROR).SetActive(eValidationError != 0);
            if (eValidationError) {
                CRefString usError = ::Console::UIOnlineActions::GetHeroValidationErrorMessage(eValidationError, TRUE);
                UI_GetTextFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_ERROR).SetText(usError, FALSE);
            }
        }
        else UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_ERROR).SetActive(FALSE);
    }
    else {
        CRefString sUserName(m_ptPartyMember->szName);
        Set_Text_Color(sUserName, crgbaWhite);
        UI_GetTextFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_INFO).SetText(sUserName, FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_PORTRAIT).SetActive(FALSE);
    }
    UI_GetWindowFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_QUEST).SetActive(FALSE);
}

#line 587 "X/Client/UserInterface/Console/UIConsolePlayerOptions.cpp"
void FriendPlayerInfo::DisplayInfo()
{
    if (m_ptFriend->m_bInGame && m_ptFriend->m_tUserPresence.m_eClass != -1) {
        UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_PORTRAIT).SetActive(TRUE);
        DisplayPlayerInfo(m_ptFriend->m_szUsername, m_ptFriend->m_tUserPresence.m_eClass, m_ptFriend->m_tUserPresence.m_bFemale, m_ptFriend->m_tUserPresence.m_uLevel, m_ptFriend->m_tUserPresence.m_uParagonLevel, m_ptFriend->m_tGamePresence.m_bIsHardcore, FALSE, m_ptFriend->m_tGamePresence.m_bIsChallenge, m_ptFriend->m_tUserPresence.m_gbidPortrait);
        if (m_ptFriend->m_tGamePresence.m_bIsChallenge) UI_GetWindowFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_QUEST).SetActive(FALSE);
        else {
            UI_GetWindowFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_QUEST).SetActive(TRUE);
            Act eAct = m_ptFriend->m_tGamePresence.m_eAct;
            SNO snoQuest = m_ptFriend->m_tGamePresence.m_snoQuest;
            if (snoQuest == SNO_NONE) {
                SNO snoAct = ActSNOFromActEnum(eAct);
                if (snoAct != SNO_NONE) {
                    ScopedActDefinitionLockR ptDefAct(snoAct);
                    if (ptDefAct && ActGetNumQuests(ptDefAct) > 0) snoQuest = ptDefAct->arQuests[0].snoQuest;
                }
            }
            CRefString sQuest = QuestsGetUIName(snoQuest);
            CRefString szDifficulty = DisplayFormattedHandicap(m_ptFriend->m_tGamePresence.m_nHandicap);
            CRefString sAct = DisplayActName(eAct);
            if (m_ptFriend->m_tGamePresence.m_bIsSeasonal) sAct.Format("{icon:Seasonal, 1.5} %s", sAct.str());
            CRefString szFinalString = LocalizedSprintf(StringListFetch("ConsoleUI:QuestInfo").str(), sQuest.str(), sAct.str(), szDifficulty.str(), UnicodeText::FormatNumber(static_cast<int32>(m_ptFriend->m_tGamePresence.m_uNumPlayers), TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), UnicodeText::FormatNumber(4, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
            UI_GetTextFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_QUEST).SetText(szFinalString, FALSE);
        }
    }
    else {
        CRefString sUserName(m_ptFriend->m_szUsername);
        Set_Text_Color(sUserName, crgbaWhite);
        UI_GetTextFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_INFO).SetText(sUserName, FALSE);
        UI_GetWindowFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_QUEST).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_PORTRAIT).SetActive(FALSE);
    }
    UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_ERROR).SetActive(FALSE);
}

#line 681 "X/Client/UserInterface/Console/UIConsolePlayerOptions.cpp"
void LocalCommPlayerInfo::DisplayInfo()
{
    if (m_ptPartyInfo->m_tGameInfo.m_tPlayers[m_nRemotePlayerIndex].m_tUserPresence.m_eClass != -1) {
        UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_PORTRAIT).SetActive(TRUE);
        DisplayPlayerInfo(m_ptPartyInfo->m_tGameInfo.m_szUsername, m_ptPartyInfo->m_tGameInfo.m_tPlayers[m_nRemotePlayerIndex].m_tUserPresence.m_eClass, m_ptPartyInfo->m_tGameInfo.m_tPlayers[m_nRemotePlayerIndex].m_tUserPresence.m_bFemale, m_ptPartyInfo->m_tGameInfo.m_tPlayers[m_nRemotePlayerIndex].m_tUserPresence.m_uLevel, m_ptPartyInfo->m_tGameInfo.m_tPlayers[m_nRemotePlayerIndex].m_tUserPresence.m_uParagonLevel, m_ptPartyInfo->m_tGameInfo.m_bIsHardcore, FALSE, m_ptPartyInfo->m_tGameInfo.m_bIsChallenge, m_ptPartyInfo->m_tGameInfo.m_tPlayers[m_nRemotePlayerIndex].m_tUserPresence.m_gbidPortrait);
        if (m_ptPartyInfo->m_tGameInfo.m_bIsChallenge) UI_GetWindowFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_QUEST).SetActive(FALSE);
        else {
            UI_GetWindowFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_QUEST).SetActive(TRUE);
            Act eAct = m_ptPartyInfo->m_tGameInfo.m_eAct;
            SNO snoQuest = m_ptPartyInfo->m_tGameInfo.m_snoQuest;
            if (snoQuest == SNO_NONE) {
                SNO snoAct = ActSNOFromActEnum(eAct);
                if (snoAct != SNO_NONE) {
                    ScopedActDefinitionLockR ptDefAct(snoAct);
                    if (ptDefAct && ActGetNumQuests(ptDefAct) > 0) snoQuest = ptDefAct->arQuests[0].snoQuest;
                }
            }
            CRefString sQuest = QuestsGetUIName(snoQuest);
            CRefString szDifficulty = DisplayFormattedHandicap(m_ptPartyInfo->m_tGameInfo.m_nHandicap);
            CRefString sAct = DisplayActName(eAct);
            if (m_ptPartyInfo->m_tGameInfo.m_bIsSeasonal) sAct.Format("{icon:Seasonal, 1.5} %s", sAct.str());
            CRefString szFinalString = LocalizedSprintf(StringListFetch("ConsoleUI:QuestInfo").str(), sQuest.str(), sAct.str(), szDifficulty.str(), UnicodeText::FormatNumber(static_cast<int32>(m_ptPartyInfo->m_tGameInfo.m_uNumPlayers), TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), UnicodeText::FormatNumber(4, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
            UI_GetTextFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_QUEST).SetText(szFinalString, FALSE);
        }
    }
    else {
        CRefString sUserName(m_ptPartyInfo->m_tGameInfo.m_szUsername);
        Set_Text_Color(sUserName, crgbaWhite);
        UI_GetTextFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_INFO).SetText(sUserName, FALSE);
        UI_GetWindowFromID(UITEXT_CONSOLE_PLAYER_OPTIONS_QUEST).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_PORTRAIT).SetActive(FALSE);
    }
    UI_GetWindowFromID(UIWND_CONSOLE_PLAYER_OPTIONS_ERROR).SetActive(FALSE);
}
}
}
