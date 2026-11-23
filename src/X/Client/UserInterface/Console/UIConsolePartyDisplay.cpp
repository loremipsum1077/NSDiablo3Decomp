// Original file: X/Client/UserInterface/Console/UIConsolePartyDisplay.cpp.
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../../Contrib/Contrib/blz/include/blz/list.h"
#include "../../Console/Online.h"
#include "../../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "UIConsoleUtils.h"
#include "../../../Common/OnlineService/SubscriptionEvent.h"
struct UIControlSourceKey; typedef int32 EffectGroupID;  // X/Client/EffectGroup.h
#include "../UI.h"
#include "../UIIDs.h"
#include "UIConsolePlayerOptions.h"
#include "UIConsolePartyDisplay.h"
#include "../../Console/PresenceManager.h"
#include "../../../Common/Game.h"
#include <strings.h>
#include "../../Sound/Sound.h"
#include "../../../Common/GlobalSNO.h"
#include "../Controls/UITextControl.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
void EffectGroupOutro(EffectGroupID idEffectGroup);  // EffectGroup.cpp
const UIID & UI_GetControllerFocus();  // UI.cpp:540
CRefString DisplayFormattedHandicap(int32 nHandicap);  // X/Client/Battlenet/UI/Util/UIUtil.cpp
namespace Console { namespace GamerProfile { int32 GetPrimaryProfileUserIndex(); } }  // GamerProfile.cpp
namespace Console { namespace UIOnlineActions { void ClearJoinAdHocNetworkInProgress(); } }  // OnlineActions.cpp
namespace Console { namespace Utilities { void ShowGameInviteUI(int32 nUserIndex); } }  // Utilities.cpp
namespace Console { namespace Online {
BOOL PartyIsInGame();  // Online.cpp
uint32 PartyGetLocalMemberCount();  // Online.cpp
void PartyFindAdHoc(blz::list<PartyInfo, blz::allocator<PartyInfo> > & tFindResult, uint32 nMaxResults);  // Online.cpp
uint32 PresenceGetOnlineFriends(int32 nUserIndex, Friend * ptFriends, uint32 uMaxFriends);  // PresenceManager.cpp:4130
} }
namespace UIPlayerOptions { namespace Console {  // UIConsolePlayerOptions.cpp
void ShowPartyPlayerOptions(int32 nLocalUserIndex, ::Console::Online::PartyMember * ptPartyMember, const UIID & idParent);
void ShowFriendPlayerOptions(int32 nLocalUserIndex, ::Console::Online::Friend * ptFriend, const UIID & idParent);
void ShowLocalCommPlayerOptions(int32 nLocalUserIndex, int32 nRemotePlayerIndex, ::Console::Online::PartyInfo * ptParty, const UIID & idParent);
void RefreshPlayerOptions(PlayerInfo * ptPlayerInfo);
} }
namespace UIPartyDisplay { namespace Console {
void RefreshFriendsDisplay(); void RefreshPartyNameplates(); void ClearNamePlateSelection();
bool FriendListSortPred(const ::Console::Online::Friend & tFriendA, const ::Console::Online::Friend & tFriendB);
bool LocalCommPartySortPred(const ::Console::Online::PartyInfo & tPartyA, const ::Console::Online::PartyInfo & tPartyB);
enum { MAX_DISPLAYED_FRIENDS = 32, }; } }
#include "../Controls/UIConsoleScrollIndicatorControl.h"
namespace UIPartyDisplay { namespace Console { enum GroupBannerDisplay { No_Banner_Display = 0, Duo_Banner_Display = 1, Trio_Banner_Display = 2, };
void PopulateListElement(const UIID & idItemUI, int32 eClass, BOOL bFemale, CRefString sName, int32 nLevel, int32 nAltLevel, int32 eVoiceChatState, BOOL bSelected, BOOL bXBLPartyMember);
void PopulateAdHocListElement(const UIID & idItemUI, int32 eClass, BOOL bFemale, CRefString sName, int32 nLevel, int32 nAltLevel, GroupBannerDisplay eBannerDisplay, BOOL bSelected);
} } extern UIID UITEXT_CONSOLE_FRIENDS_NO_FRIENDS, UIWND_CONSOLE_FRIENDS_NO_FREINDS_HIGHLIGHT, UIWND_CONSOLE_FRIENDS_BACKGROUND;  // UIGlobals.cpp:224-227
CRefString StringListFetch(LPCSTR szLabel); CRefString LocalizedSprintf(LPCSTR cpFmtStr, ...);  // StringListDefinition.cpp
namespace UIPartyDisplay { namespace Console {
struct NamePlateIds {  // size 0x38
    const UIID * pidWindow;  // 0x00
    const UIID * pidText;  // 0x08
    const UIID * pidAnchor;  // 0x10
    const UIID * pidBackground;  // 0x18
    const UIID * pidInGame;  // 0x20
    const UIID * pidError;  // 0x28
    const UIID * pidPortrait;  // 0x30
};  static_assert(sizeof(NamePlateIds) == 0x38, "NamePlateIds");

NamePlateIds s_arNamePlates[4] = {
    { &UIWND_CONSOLE_MAIN_MENU_PARTY1, &UITEXT_CONSOLE_MAIN_MENU_PARTY1, &UIWND_CONSOLE_MAIN_MENU_PARTY1_ANCHOR, &UIWND_CONSOLE_MAIN_MENU_PARTY1_GLOW, &UIWND_CONSOLE_MAIN_MENU_PARTY1_IN_GAME, &UIWND_CONSOLE_MAIN_MENU_PARTY1_ERROR, &UIWND_CONSOLE_MAIN_MENU_PARTY1_PORTRAIT },
    { &UIWND_CONSOLE_MAIN_MENU_PARTY2, &UITEXT_CONSOLE_MAIN_MENU_PARTY2, &UIWND_CONSOLE_MAIN_MENU_PARTY2_ANCHOR, &UIWND_CONSOLE_MAIN_MENU_PARTY2_GLOW, &UIWND_CONSOLE_MAIN_MENU_PARTY2_IN_GAME, &UIWND_CONSOLE_MAIN_MENU_PARTY2_ERROR, &UIWND_CONSOLE_MAIN_MENU_PARTY2_PORTRAIT },
    { &UIWND_CONSOLE_MAIN_MENU_PARTY3, &UITEXT_CONSOLE_MAIN_MENU_PARTY3, &UIWND_CONSOLE_MAIN_MENU_PARTY3_ANCHOR, &UIWND_CONSOLE_MAIN_MENU_PARTY3_GLOW, &UIWND_CONSOLE_MAIN_MENU_PARTY3_IN_GAME, &UIWND_CONSOLE_MAIN_MENU_PARTY3_ERROR, &UIWND_CONSOLE_MAIN_MENU_PARTY3_PORTRAIT },
    { &UIWND_CONSOLE_MAIN_MENU_PARTY4, &UITEXT_CONSOLE_MAIN_MENU_PARTY4, &UIWND_CONSOLE_MAIN_MENU_PARTY4_ANCHOR, &UIWND_CONSOLE_MAIN_MENU_PARTY4_GLOW, &UIWND_CONSOLE_MAIN_MENU_PARTY4_IN_GAME, &UIWND_CONSOLE_MAIN_MENU_PARTY4_ERROR, &UIWND_CONSOLE_MAIN_MENU_PARTY4_PORTRAIT }
};


NamePlateIds s_arNamePlatesBlizzcon[4] = {{&UIWND_CONSOLE_MAIN_MENU_PARTY1, &UITEXT_CONSOLE_MAIN_MENU_PARTY1, &UIWND_CONSOLE_MAIN_MENU_PARTY1_ANCHOR, &UIWND_CONSOLE_MAIN_MENU_PARTY1_GLOW, &UIWND_CONSOLE_MAIN_MENU_PARTY1_IN_GAME, &UIWND_CONSOLE_MAIN_MENU_PARTY1_ERROR, &UIWND_CONSOLE_MAIN_MENU_PARTY1_PORTRAIT}, {&UIWND_CONSOLE_MAIN_MENU_PARTY3, &UITEXT_CONSOLE_MAIN_MENU_PARTY3, &UIWND_CONSOLE_MAIN_MENU_PARTY3_ANCHOR, &UIWND_CONSOLE_MAIN_MENU_PARTY3_GLOW, &UIWND_CONSOLE_MAIN_MENU_PARTY3_IN_GAME, &UIWND_CONSOLE_MAIN_MENU_PARTY3_ERROR, &UIWND_CONSOLE_MAIN_MENU_PARTY2_PORTRAIT}, {&UIWND_CONSOLE_MAIN_MENU_PARTY2, &UITEXT_CONSOLE_MAIN_MENU_PARTY2, &UIWND_CONSOLE_MAIN_MENU_PARTY2_ANCHOR, &UIWND_CONSOLE_MAIN_MENU_PARTY2_GLOW, &UIWND_CONSOLE_MAIN_MENU_PARTY2_IN_GAME, &UIWND_CONSOLE_MAIN_MENU_PARTY2_ERROR, &UIWND_CONSOLE_MAIN_MENU_PARTY3_PORTRAIT}, {&UIWND_CONSOLE_MAIN_MENU_PARTY4, &UITEXT_CONSOLE_MAIN_MENU_PARTY4, &UIWND_CONSOLE_MAIN_MENU_PARTY4_ANCHOR, &UIWND_CONSOLE_MAIN_MENU_PARTY4_GLOW, &UIWND_CONSOLE_MAIN_MENU_PARTY4_IN_GAME, &UIWND_CONSOLE_MAIN_MENU_PARTY4_ERROR, &UIWND_CONSOLE_MAIN_MENU_PARTY4_PORTRAIT}};
} } // Resume global scope for DWARF declarations used by the local communication display.
UIConsoleScrollIndicator & UI_GetConsoleScrollIndicatorFromID(const UIID & id); // UI.cpp:5447
namespace UIPlayerOptions { namespace Console { BOOL HasParent(const UIID & idParent); } } // UIConsolePlayerOptions.cpp:872
extern UIID UIWND_CONSOLE_LOCAL_COMMUNICATION_BACKGROUND, UIWND_CONSOLE_LOCAL_COMMUNICATION_MORE_ARROW, UIWND_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS_HIGHLIGHT; // UIGlobals.cpp:218-221
namespace UIPartyDisplay { namespace Console {
CRefString GetClassPortrait(PlayerClass eClass, BOOL bFemale); // DWARF owner:1828
} } // Global UI/SymbolicValue declarations below are recorded in their original owners.
uint32 UI_GetXMLUint32Constant(LPCSTR szConstantPath, uint32 nDefaultValue); CRefString CPlayerGetLevelString(const int32 nLevel, const int32 nAltLevel); // UI.cpp:2375; CPlayer.cpp:3166
extern const SymbolicEntry gPlayerClasses[9], gActorGenderNames[3]; LPCSTR SymbolToName(int32 value, const SymbolicEntry * pSymbols); void Set_Text_Color(CRefString & usString, const RGBAColor & rgbaColor); // X/Common/Sno/SymbolicValue.cpp tables
namespace UIPartyDisplay { namespace Console {
} }
#include "../../../Common/Player.h"
namespace UIUtils { namespace Console { RGBAColor GetPlayerColor(const int32 nPlayerIndex); } } // UIConsoleUtils.cpp:370
namespace UIPartyDisplay { namespace Console {
int32 s_arNamePlateVisualOrder[4] = {2, 0, 1, 3};  // line 88
int32 s_arNamePlateVisualOrderBlizzcon[4] = {1, 0, 2, 3};  // line 89
int32 s_arNamePlatePartyIndices[4];  // line 90
} }
#include "../../Battlenet/UI/Controls/ControlVisibilityManager.h"
namespace Console { namespace Online { int32 PartyGetHostIndex(); BOOL IsInPlatformParty(OnlineUserId tUserId); } } // Online.cpp:4323,915
namespace UIUtils { namespace Console { ImageTextureFrame GetVoiceChatTexture(VoiceChatState eState); RGBAColor GetXBLPartyColor(); } } // UIConsoleUtils.cpp:177,385
namespace UIPartyDisplay { namespace Console {
} }
#include "../../EffectGroup.h"
#include "../../RActors/RActors.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundHeroManager.h"
#include "ConsoleScreenManager.h"
#include "OnlineActions.h"
#include "../../../Sigma/UnicodeText.h"
// DWARF EffectGroup.cpp:1311; Input.cpp:1157.
EffectGroupID EffectGroupCreateTracked(const SNO, const RActorID, const RActorID, ParentType, const BOOL); uint32 InputGetNumConnectedControllers();
// DWARF UIConsoleUtils.cpp:154,1043; OnlineActions.cpp:1676,2107.
namespace UIUtils { namespace Console { CRefString GetVoiceChatTextureName(VoiceChatState); void DisplayPortrait(PlayerClass, BOOL, int32, const UIID &, GBID); } }
namespace Console { namespace UIOnlineActions { HeroValidationError ValidateHeroForPartyMember(const Online::PartyMember &); CRefString GetHeroValidationErrorMessage(HeroValidationError, BOOL); } }
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/Definitions/HeroDefinition.h"
namespace UIPartyDisplay { namespace Console {













class PartyDataSource : public ConsoleListBoxDataSource {  // size 0x8
public:

    PartyDataSource()  // empty base construction
    { }



    virtual int32 GetNumItems() const;  // vtable slot 2; defined at line 465




    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const


    { return UIControlSourceKey::Parse("ConsolePartyDisplay:ConsolePartyDisplay_main.party_list_item", -1, NULL, NULL); }  // vtable slot 3




    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)  // vtable slot 4







    { }



    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5; defined at line 482
};  static_assert(sizeof(PartyDataSource) == 0x8, "PartyDataSource");






class OnlineFriendsDataSource : public ConsoleListBoxDataSource {  // size 0x1310
public:

    OnlineFriendsDataSource()
        : m_nNumFriends(0),

          m_bShowWhenEmpty(FALSE) {}


    virtual int32 GetNumItems() const { return m_nNumFriends; }  // vtable slot 2







    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const


    { return UIControlSourceKey::Parse("ConsolePartyDisplay:ConsolePartyDisplay_main.party_list_item", -1, NULL, NULL); }  // vtable slot 3




    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)  // vtable slot 4





    { }



    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)  // vtable slot 5
    {
        PopulateListElement(idItemUI, m_arFriends[nIndex].m_tUserPresence.m_eClass, m_arFriends[nIndex].m_tUserPresence.m_bFemale,
            CRefString(m_arFriends[nIndex].m_szUsername), m_arFriends[nIndex].m_tUserPresence.m_uLevel, 0, -1,
            tListBox.GetSelectedIndex() == nIndex, m_arFriends[nIndex].m_bInPlatformParty);
    }















    void RefreshData()

    {
        m_nNumFriends = ::Console::Online::PresenceGetOnlineFriends(::Console::GamerProfile::GetPrimaryProfileUserIndex(), m_arFriends, MAX_DISPLAYED_FRIENDS);
        if (m_nNumFriends > 1)

            blz::sort(m_arFriends, m_arFriends + m_nNumFriends,
                FriendListSortPred);
    }

    ::Console::Online::Friend m_arFriends[32];  // 0x08
    uint32 m_nNumFriends;  // 0x1308
    BOOL m_bShowWhenEmpty;  // 0x130c
};  static_assert(sizeof(OnlineFriendsDataSource) == 0x1310, "OnlineFriendsDataSource");
class LocalCommunicationsDataSource : public ConsoleListBoxDataSource { public:  // size 0xc450
    virtual ~LocalCommunicationsDataSource() {}


    LocalCommunicationsDataSource()
        : m_bResultsDirty(FALSE),
          m_nNumMatchesAvailable(0),
          m_bCurrentlyExpanded(false) {}


    virtual int32 GetNumItems() const { return m_nNumMatchesAvailable; }  // vtable slot 2







    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const


    { return UIControlSourceKey::Parse("ConsolePartyDisplay:ConsolePartyDisplay_main.ad_hoc_party_list_item", -1, NULL, NULL); }  // vtable slot 3




    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)  // vtable slot 4





    { }

    virtual BOOL IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex)  // vtable slot 6
    {
        if (nIndex >= MAX_DISPLAYED_FRIENDS)
            return FALSE;
        return m_arAdHocPartyResult[nIndex].m_nPlayerIndex == 0;
    }










    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)  // vtable slot 5
    {
        PopulateAdHocListElement(idItemUI,
            m_arAdHocPartyResult[nIndex].m_tPartyinfo.m_tGameInfo.m_tPlayers[m_arAdHocPartyResult[nIndex].m_nPlayerIndex].m_tUserPresence.m_eClass,
            m_arAdHocPartyResult[nIndex].m_tPartyinfo.m_tGameInfo.m_tPlayers[m_arAdHocPartyResult[nIndex].m_nPlayerIndex].m_tUserPresence.m_bFemale,
            CRefString(m_arAdHocPartyResult[nIndex].m_tPartyinfo.m_tGameInfo.m_szUsername),
            m_arAdHocPartyResult[nIndex].m_tPartyinfo.m_tGameInfo.m_tPlayers[m_arAdHocPartyResult[nIndex].m_nPlayerIndex].m_tUserPresence.m_uLevel, 0,
            m_arAdHocPartyResult[nIndex].m_eBannerDisplay, tListBox.GetSelectedIndex() == nIndex);
    }











    void RefreshData()

    {

        if (m_bResultsDirty)
        {
            Blizzard::Lock::ScopedRMutex tLock(m_tFindMutex);


            m_tFindResult.sort(LocalCommPartySortPred);

            SigmaMemoryClear(m_arAdHocPartyResult, sizeof(m_arAdHocPartyResult));

            int32 Index = 0;
            for (::Console::Online::PartyInfo & partyInfo : m_tFindResult)
            {
                if (Index >= MAX_DISPLAYED_FRIENDS)
                    break;




                if (partyInfo.m_tGameInfo.m_uNumPlayers == 0 || partyInfo.m_tGameInfo.m_uNumPlayers == 4)
                    continue;



                GroupBannerDisplay eLeaderGroupBannerDisplay = No_Banner_Display;
                switch (partyInfo.m_tGameInfo.m_uNumPlayers)
                {
                    case 2: eLeaderGroupBannerDisplay = Duo_Banner_Display; break;
                    case 3: eLeaderGroupBannerDisplay = Trio_Banner_Display; break; // Inference: other nonempty, nonfull counts retain the no-banner value.
                    default: break;
                }





                for (int32 idx = 0; idx < partyInfo.m_tGameInfo.m_uNumPlayers; idx++, Index++)
                {
                    // Inference: 345 cols 45/73/91 fit a condition on idx alone (idx++ at 91); the Index limit is this break (no row of its own; the original tests it in the latch).
                    if (Index >= MAX_DISPLAYED_FRIENDS)
                        break;


                    SigmaMemoryCopy(&m_arAdHocPartyResult[Index].m_tPartyinfo, &partyInfo, sizeof(partyInfo));


                    m_arAdHocPartyResult[Index].m_nPlayerIndex = idx;


                    SigmaMemoryCopy(m_arAdHocPartyResult[Index].m_tPartyinfo.m_tGameInfo.m_szUsername, m_arAdHocPartyResult[Index].m_tPartyinfo.m_tGameInfo.m_tPlayers[idx].m_tUserPresence.m_szName, sizeof(m_arAdHocPartyResult[Index].m_tPartyinfo.m_tGameInfo.m_szUsername));



                    SigmaMemoryCopy(m_arAdHocPartyResult[Index].m_tPartyinfo.m_szName, m_arAdHocPartyResult[Index].m_tPartyinfo.m_tGameInfo.m_tPlayers[idx].m_tUserPresence.m_szName, sizeof(m_arAdHocPartyResult[Index].m_tPartyinfo.m_szName));




                    if (idx == 0)
                    {
                        m_arAdHocPartyResult[Index].m_eBannerDisplay = eLeaderGroupBannerDisplay;
                    }
                }
            }





            m_nNumMatchesAvailable = Math::Min(Index, (int32)MAX_DISPLAYED_FRIENDS);

            m_bResultsDirty = FALSE;
        }

    }

    void ClearData();





    void RefreshGameList()

    {
        Blizzard::Lock::ScopedRMutex tLock(m_tFindMutex);
        m_tFindResult.clear();

        ::Console::Online::PartyFindAdHoc(m_tFindResult, MAX_DISPLAYED_FRIENDS);


        m_bResultsDirty = TRUE;
    }

    Blizzard::Lock::RMutex m_tFindMutex;  // 0x08

    blz::list<::Console::Online::PartyInfo, blz::allocator<::Console::Online::PartyInfo> > m_tFindResult;  // 0x28
    BOOL m_bResultsDirty;  // 0x40

    struct PerPlayerPartyInfo {  // size 0x620
        int32 m_nPlayerIndex;  // 0x00
        UIPartyDisplay::Console::GroupBannerDisplay m_eBannerDisplay;  // 0x04
        ::Console::Online::PartyInfo m_tPartyinfo;  // 0x08
    };


    UIPartyDisplay::Console::LocalCommunicationsDataSource::PerPlayerPartyInfo m_arAdHocPartyResult[32];  // 0x48
    uint32 m_nNumMatchesAvailable;  // 0xc448

    bool m_bCurrentlyExpanded;  // 0xc44c
};  static_assert(sizeof(LocalCommunicationsDataSource) == 0xc450, "LocalCommunicationsDataSource");



struct PartyDisplayData {  // size 0xded0
    UIPartyDisplay::Console::PartyDataSource tPartyList;  // 0x00
    UIPartyDisplay::Console::OnlineFriendsDataSource tFriendList;  // 0x08
    UIPartyDisplay::Console::LocalCommunicationsDataSource tLocalCommList;  // 0x1318

    uint32 nNumPartyMembers;  // 0xd768
    uint32 nNumLocalPartyMembers;  // 0xd76c
    ::Console::Online::PartyMember arPartyMembers[4];  // 0xd770
    UIUtils::Console::PartyVoiceChatStateListener * ptVoiceListener;  // 0xdeb0
    BOOL bNamePlatesEnabled;  // 0xdeb8
    int32 nFocusedNameplate;  // 0xdebc
    EffectGroupID idHighlightEffect;  // 0xdec0






    int32 nPlayerInfoPartyMember;  // 0xdec4
    OnlineUserId tPlayerInfoFriendId;  // 0xdec8

    PartyDisplayData()
        : nNumPartyMembers(0),
          nNumLocalPartyMembers(0),
          ptVoiceListener(NULL),
          bNamePlatesEnabled(FALSE),

          nFocusedNameplate(-1),

          idHighlightEffect(-1),

          nPlayerInfoPartyMember(-1)
    {
        SigmaMemoryClear(&tPlayerInfoFriendId, sizeof(tPlayerInfoFriendId));
    }
    ~PartyDisplayData()
    {
    }
};  static_assert(sizeof(PartyDisplayData) == 0xded0, "PartyDisplayData");
PartyDisplayData sgtPartyDisplayData;


int32 PartyDataSource::GetNumItems() const {
    if (::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsOnline())
        return 4;
    return sgtPartyDisplayData.nNumPartyMembers;
}












void PartyDataSource::UpdateItemUI(
    const UIConsoleListBox & tListBox,
    const UIID & idItemUI,
    const int32 nIndex)
{ // Inference: invalid party selection starts at -1 before the conditional index assignment.
    int32 nPartyIndex = -1;
    if (::Console::Online::PartyIsInParty() && nIndex < sgtPartyDisplayData.nNumPartyMembers)
        nPartyIndex = sgtPartyDisplayData.arPartyMembers[nIndex].idPlayer;

    if (nPartyIndex >= 0 && nPartyIndex < 4)
    {



        ::Console::Online::PartyMember & tPartyMember = sgtPartyDisplayData.arPartyMembers[nIndex];

        CRefString sPlayerLabel = LocalizedSprintf(
            StringListFetch("Console:PlayerLabelShort").str(),
            UnicodeText::FormatNumber(nPartyIndex + 1, TRUE, FNS_NORMAL, 0).str(), NULL);
        Set_Text_Color(sPlayerLabel, UIUtils::Console::GetPlayerColor(nPartyIndex));
        CRefString sPlayerName;
        if (::Console::Online::PartyGetHostIndex() == nPartyIndex)
        {
            sPlayerName.Format("%s {icon:Console_PartyLeaderIcon}%s", sPlayerLabel.str(), tPartyMember.szName);
        }
        else
        {
            sPlayerName.Format("%s %s", sPlayerLabel.str(), tPartyMember.szName);
        }
        CRefString sLevel;
        int32 nLevel; // Inference: original PopulateListElement passes zero for the unused alternate level below.
        int32 nAltLevel = 0;
        if (!GameIsStartUp() && PlayerGetByPlayerIndex(nPartyIndex))
        {
            const Player * ptPlayer = PlayerGetByPlayerIndex(nPartyIndex);
            nLevel = ptPlayer->nCurrentLevel;

        }
        else if (tPartyMember.tHeroDigest)
        {
            nLevel = tPartyMember.tHeroDigest->level();
        }
        else
            nLevel = 1;
        UIUtils::Console::VoiceChatState eVoiceState = UIUtils::Console::VOICE_NONE;
        BOOL bShowVoiceIcon = sgtPartyDisplayData.nNumPartyMembers != sgtPartyDisplayData.nNumLocalPartyMembers;
        if (bShowVoiceIcon)
        {




            eVoiceState = sgtPartyDisplayData.ptVoiceListener->GetVoiceChatState(nPartyIndex);
        }

        PopulateListElement(
            idItemUI,
            PlayerClassFromGBID(tPartyMember.tHeroDigest ? tPartyMember.tHeroDigest->gbid_class() : 0),
            tPartyMember.tHeroDigest ? (tPartyMember.tHeroDigest->player_flags() & (1 << PLAYERFLAG_FEMALE)) != 0 : FALSE,
            sPlayerName,
            nLevel,
            nAltLevel,
            eVoiceState,
            tListBox.GetSelectedIndex() == nIndex,
            ::Console::Online::IsInPlatformParty(tPartyMember.idOnlineUser));
    }
    else
    {
        CRefString sName;
        if (::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsOnline())
        {
            sName = StringListFetch("Console:InviteFriend");
        }
        else
        {
            sName = "";
        }
        PopulateListElement(
            idItemUI,
            PLAYER_CLASS_NONE,
            FALSE,
            sName,
            -1,
            -1,
            UIUtils::Console::VOICE_NONE,
            tListBox.GetSelectedIndex() == nIndex,
            FALSE);
    }

}


void InitializeForGame() {
    sgtPartyDisplayData.ptVoiceListener = new UIUtils::Console::PartyVoiceChatStateListener();
}





void DisposeForGame() {
    delete sgtPartyDisplayData.ptVoiceListener;
    sgtPartyDisplayData.ptVoiceListener = NULL;
}



void RefreshPartyData()
{
    sgtPartyDisplayData.nNumPartyMembers = ::Console::Online::PartyGetMembers(sgtPartyDisplayData.arPartyMembers, 1, FALSE);
    sgtPartyDisplayData.nNumLocalPartyMembers = 0;
    for (uint32 i = 0; i < sgtPartyDisplayData.nNumPartyMembers; i++)
    {
        if (sgtPartyDisplayData.arPartyMembers[i].bIsLocal)
        {
            sgtPartyDisplayData.nNumLocalPartyMembers++;
        }
    }
}



void RefreshPartyDisplay()

{
    ::Console::Online::GameParams tGameParams;

    if (::Console::Online::PartyGetGameParams(tGameParams))
    {



        RefreshPartyData();
        UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY);
        tList.SetDataSource(sgtPartyDisplayData.tPartyList);
        tList.RefreshVisibleItems();

        CRefString szDifficulty = DisplayFormattedHandicap(tGameParams.m_nHandicap);

        ::Console::Online::PartyOnlineState eState = ::Console::Online::PartyGetOnlineState();
        CRefString sLabel = UIUtils::Console::DisplayPartyOnlineState(eState, TRUE, 2.0f);
        UI_GetTextFromID(UITEXT_CONSOLE_NETWORK_HEADER).SetText(sLabel, FALSE);

        if (UIPlayerOptions::Console::IsVisible() &&
            sgtPartyDisplayData.nPlayerInfoPartyMember != -1)
        {
            UIPlayerOptions::Console::RefreshPlayerOptions(NULL);
        }
    }
}


BOOL IsPartyDisplayVisible()
{
    return UI_GetWindowFromID(UIWND_CONSOLE_MENU_PARTY_CONTENT).IsActive();
}



BOOL IsFriendDisplayVisible()
{

    return UI_GetWindowFromID(UIWND_CONSOLE_FRIENDS_CONTENT).IsActive();
}

BOOL IsLocalCommunicationDisplayVisible()
{
    return UI_GetWindowFromID(UIWND_CONSOLE_LOCAL_COMMUNICATION_CONTENT).IsActive();
}



void SwitchFocusedList(BOOL bPressedUp)

{
    if (UI_GetControllerFocus() == UILIST_CONSOLE_MENU_PARTY &&
        IsFriendDisplayVisible() &&
        sgtPartyDisplayData.tFriendList.GetNumItems() > 0)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);

        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY).SetSelectedIndex(-1);

        int32 nSelectedIndex = bPressedUp ? sgtPartyDisplayData.tFriendList.GetNumItems() - 1 : 0;
        UIConsoleListBox & tFriendsList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST);
        tFriendsList.SetSelectedIndex(nSelectedIndex);
        tFriendsList.SimulateStickInput(bPressedUp);
        UI_SetControllerFocus(UILIST_CONSOLE_FRIENDS_LIST);
    }

    else if (UI_GetControllerFocus() == UILIST_CONSOLE_FRIENDS_LIST && IsPartyDisplayVisible())
    {

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);

        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).SetSelectedIndex(-1);

        int32 nSelectedIndex = bPressedUp ? sgtPartyDisplayData.tPartyList.GetNumItems() - 1 : 0;
        UIConsoleListBox & tPartyList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY);
        tPartyList.SetSelectedIndex(nSelectedIndex);
        tPartyList.SimulateStickInput(bPressedUp);
        UI_SetControllerFocus(UILIST_CONSOLE_MENU_PARTY);
    }

}


void OnScrolledOffListTop(const UIID & id) { SwitchFocusedList(TRUE); }






void OnScrolledOffListBottom(const UIID & id) { SwitchFocusedList(FALSE); }






void RefreshFriendsDisplay()

{
    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST);
    int32 nSelectedIndex = tList.GetSelectedIndex();


    OnlineUserId idSelectedUser = 0;





    if (nSelectedIndex != -1)
    {



        idSelectedUser = sgtPartyDisplayData.tFriendList.m_arFriends[nSelectedIndex].m_idFriend;
    }


    sgtPartyDisplayData.tFriendList.RefreshData();
    tList.SetDataSource(sgtPartyDisplayData.tFriendList);
    tList.RefreshVisibleItems();



    if (nSelectedIndex != -1)
    {
        BOOL bFoundOptionsFriend = FALSE;
        BOOL bFoundPreviousSelectedFriend = FALSE;
        UIPlayerOptions::Console::FriendPlayerInfo * ptFriendPlayerInfo = NULL;
        for (int32 i = 0; i < sgtPartyDisplayData.tFriendList.m_nNumFriends; i++)
        {
            if (UIPlayerOptions::Console::IsVisible() &&
                sgtPartyDisplayData.nPlayerInfoPartyMember == -1 &&
                sgtPartyDisplayData.tPlayerInfoFriendId == sgtPartyDisplayData.tFriendList.m_arFriends[i].m_idFriend)
            {

                if (nSelectedIndex != i && UI_GetPrimaryUIController() != -1)
                {


                    ptFriendPlayerInfo = new UIPlayerOptions::Console::FriendPlayerInfo(UI_GetPrimaryUIController(), &sgtPartyDisplayData.tFriendList.m_arFriends[i]);
                }
                bFoundOptionsFriend = TRUE;
            }
            if (idSelectedUser == sgtPartyDisplayData.tFriendList.m_arFriends[i].m_idFriend)
            {
                tList.SetSelectedIndex(i);
                bFoundPreviousSelectedFriend = TRUE;
            }

            if (bFoundOptionsFriend && bFoundPreviousSelectedFriend)
                break;
        }



        if (UIPlayerOptions::Console::IsVisible())
        {
            if (!bFoundOptionsFriend)
            {
                UIPlayerOptions::Console::ClosePlayerOptions();
            }
            else
            {
                UIPlayerOptions::Console::RefreshPlayerOptions(ptFriendPlayerInfo);
            }
        }
    }

    if (UI_GetControllerFocus() == UILIST_CONSOLE_FRIENDS_LIST &&
        sgtPartyDisplayData.tFriendList.GetNumItems() == 0)
    {
        if (IsPartyDisplayVisible())
        {
            SwitchFocusedList(TRUE);
        }
        else if (sgtPartyDisplayData.bNamePlatesEnabled)
        {
            SelectPreviousNameplate();
        }
    }

    UIText & tNoFriendsHeader = UI_GetTextFromID(UITEXT_CONSOLE_FRIENDS_NO_FRIENDS);
    UIWindow & tNoFriendsHighlight = UI_GetWindowFromID(UIWND_CONSOLE_FRIENDS_NO_FREINDS_HIGHLIGHT);

    if (::Console::Online::PartyGetOnlineState() >= ::Console::Online::PARTY_ONLINE)
    {
        tNoFriendsHeader.SetText(StringListFetch("Console:ConsoleNoFriendsFound"), FALSE);
    }
    else
    {
        tNoFriendsHeader.SetText(StringListFetch("Console:ConsoleFriendOffline"), FALSE);
    }

    UIText & tHeader = UI_GetTextFromID(UITEXT_CONSOLE_FRIENDS_HEADER);
    CRefString sHeaderText;
    if (sgtPartyDisplayData.tFriendList.GetNumItems() > 0 || sgtPartyDisplayData.tFriendList.m_bShowWhenEmpty)
    {
        CRefString sText = LocalizedSprintf(
            StringListFetch("ConsoleUI:OnlineFriends").str(),
            UnicodeText::FormatNumber(sgtPartyDisplayData.tFriendList.GetNumItems(), TRUE, FNS_NORMAL, 0).str(), NULL);
        tHeader.SetText(sText, FALSE);
    }

    if (sgtPartyDisplayData.tFriendList.m_bShowWhenEmpty)
    {


        if (UI_GetControllerFocus() == UILIST_CONSOLE_FRIENDS_LIST &&
            sgtPartyDisplayData.tFriendList.GetNumItems() == 0)
        {
            UI_SetControllerFocus(UITEXT_CONSOLE_FRIENDS_NO_FRIENDS);
        }
        else if (UI_GetControllerFocus() == UITEXT_CONSOLE_FRIENDS_NO_FRIENDS &&
                 sgtPartyDisplayData.tFriendList.GetNumItems() >= 1)
        {
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).SetSelectedIndex(0);
            UI_SetControllerFocus(UILIST_CONSOLE_FRIENDS_LIST);
        }


        tHeader.SetActive(IsFriendDisplayVisible());
        UI_GetWindowFromID(UIWND_CONSOLE_FRIENDS_BACKGROUND).SetActive(TRUE);

        if (sgtPartyDisplayData.tFriendList.GetNumItems() == 0)
        {
            tNoFriendsHeader.SetActive(TRUE);

            tNoFriendsHighlight.SetActive(UI_GetControllerFocus() == UITEXT_CONSOLE_FRIENDS_NO_FRIENDS);
        }
        else
        {
            tNoFriendsHeader.SetActive(FALSE);
            tNoFriendsHighlight.SetActive(FALSE);
        }
    }
    else
    {
        tHeader.SetActive(sgtPartyDisplayData.tFriendList.GetNumItems() >= 1 && IsFriendDisplayVisible());
        tNoFriendsHeader.SetActive(FALSE);
        tNoFriendsHighlight.SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_FRIENDS_BACKGROUND).SetActive(sgtPartyDisplayData.tFriendList.GetNumItems() > 0);
    }
}

BOOL CanDisplayLocalCommunicationDisplay()
{
    if (::Console::Online::PartyIsInGame() && ::Console::Online::PartyIsHost())
        return TRUE;
    if (::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsHost())
        return ::Console::Online::PartyGetMemberCount() == ::Console::Online::PartyGetLocalMemberCount();
    return FALSE;
}







void RefreshLocalCommunicationsDisplay()

{
    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST);
    UIWindow & tContent = UI_GetWindowFromID(UIWND_CONSOLE_LOCAL_COMMUNICATION_CONTENT);
    UIWindow & tBackground = UI_GetWindowFromID(UIWND_CONSOLE_LOCAL_COMMUNICATION_BACKGROUND);

    sgtPartyDisplayData.tLocalCommList.RefreshData();





    int32 CachedSelectedIndex = tList.GetSelectedIndex();
    tList.SetDataSource(sgtPartyDisplayData.tLocalCommList);


    CachedSelectedIndex = Math::Min(CachedSelectedIndex, sgtPartyDisplayData.tLocalCommList.GetNumItems() - 1);
    tList.SetSelectedIndex(CachedSelectedIndex);

    BOOL bCanExpand = tList.GetNumVisibleRows() < sgtPartyDisplayData.tLocalCommList.GetNumItems();

    if (UI_GetControllerFocus() == UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST)
    {

        UI_GetWindowFromID(UIWND_CONSOLE_LOCAL_COMMUNICATION_MORE_ARROW).SetActive(FALSE);

        if (bCanExpand && !sgtPartyDisplayData.tLocalCommList.m_bCurrentlyExpanded)
        {
            Vector2D vecTL;
            Vector2D vecDummyDim;
            Vector2D vecMaxDim;


            UI_GetConsoleScrollIndicatorFromID(tList.GetScrollbarIndicator()).SetScrollbarVisibility(SBV_HIDE_WHEN_NOT_NEEDED);


            tContent.GetRelativePosition(vecTL, vecDummyDim);
            tContent.GetMaxDimension(vecMaxDim);
            tContent.SetRelativePosition(vecTL, vecMaxDim);


            tBackground.GetRelativePosition(vecTL, vecDummyDim);
            tBackground.GetMaxDimension(vecMaxDim);
            tBackground.SetRelativePosition(vecTL, vecMaxDim);


            tList.GetRelativePosition(vecTL, vecDummyDim);
            tList.GetMaxDimension(vecMaxDim);
            tList.SetRelativePosition(vecTL, vecMaxDim);

            sgtPartyDisplayData.tLocalCommList.m_bCurrentlyExpanded = true;
        }
    }
    else
    {
        UI_GetWindowFromID(UIWND_CONSOLE_LOCAL_COMMUNICATION_MORE_ARROW).SetActive(bCanExpand);

        if (sgtPartyDisplayData.tLocalCommList.m_bCurrentlyExpanded)
        {
            Vector2D vecTL;
            Vector2D vecDummyDim;
            Vector2D vecOrigDim;

            UI_GetConsoleScrollIndicatorFromID(tList.GetScrollbarIndicator()).SetScrollbarVisibility(SBV_NEVER_SHOW);


            tContent.GetRelativePosition(vecTL, vecDummyDim);
            tContent.GetOriginalDimension(vecOrigDim);
            tContent.SetRelativePosition(vecTL, vecOrigDim);


            tBackground.GetRelativePosition(vecTL, vecDummyDim);
            tBackground.GetOriginalDimension(vecOrigDim);
            tBackground.SetRelativePosition(vecTL, vecOrigDim);


            tList.GetRelativePosition(vecTL, vecDummyDim);
            tList.GetOriginalDimension(vecOrigDim);
            tList.SetRelativePosition(vecTL, vecOrigDim);
            sgtPartyDisplayData.tLocalCommList.m_bCurrentlyExpanded = false;
        }
    }

    tList.RefreshVisibleItems();
    // Inference: false initializes the result on the non-ad-hoc path, whose assignment has no own row.
    bool bShouldShowMenu = false;


    if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC)
    {
        if (CanDisplayLocalCommunicationDisplay()) bShouldShowMenu = true;
    }




    if ((UI_GetControllerFocus() == UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST ||
         UI_GetControllerFocus() == UIWND_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS_HIGHLIGHT) && !CanDisplayLocalCommunicationDisplay())
    {

        if (sgtPartyDisplayData.bNamePlatesEnabled)
        {
            SelectPreviousNameplate();
        }
    }
    else if (UI_GetControllerFocus() == UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST && sgtPartyDisplayData.tLocalCommList.GetNumItems() == 0)
    {

        UI_SetControllerFocus(UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS);
    }
    else if (UI_GetControllerFocus() == UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS && sgtPartyDisplayData.tLocalCommList.GetNumItems() >= 1)
    {

        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).SetSelectedIndex(0);
        UI_SetControllerFocus(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST);
    }


    UIText & tHeader = UI_GetTextFromID(UITEXT_CONSOLE_LOCAL_COMMUNICATION_HEADER);

    bShouldShowMenu = bShouldShowMenu && IsLocalCommunicationDisplayVisible();
    tHeader.SetActive(bShouldShowMenu);
    tBackground.SetActive(bShouldShowMenu);
    UIText & tNoGamesFoundText = UI_GetTextFromID(UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS);
    UIWindow & tNoGamesFoundHighlight = UI_GetWindowFromID(UIWND_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS_HIGHLIGHT);

    if (bShouldShowMenu)
    {
        if (sgtPartyDisplayData.tLocalCommList.GetNumItems() >= 1)
        {
            tNoGamesFoundText.SetActive(FALSE);
            tList.SetActive(TRUE);
            tNoGamesFoundHighlight.SetActive(FALSE);
        }
        else
        {
            tNoGamesFoundText.SetActive(TRUE);
            tNoGamesFoundHighlight.SetActive(UI_GetControllerFocus() == UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS);
            tList.SetActive(FALSE);
        }
    }
    else
    {
        if (UIPlayerOptions::Console::IsVisible() && UIPlayerOptions::Console::HasParent(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST))
        {
            UIPlayerOptions::Console::ClosePlayerOptions();
            if (sgtPartyDisplayData.bNamePlatesEnabled)
            {
                SelectPreviousNameplate();
            }
        }


        UI_GetWindowFromID(UIWND_CONSOLE_LOCAL_COMMUNICATION_MORE_ARROW).SetActive(FALSE);
        tNoGamesFoundText.SetActive(FALSE);
        tList.SetActive(FALSE);
        tNoGamesFoundHighlight.SetActive(FALSE);
    }
}


void OnEventPartyMemberRemoved(int32 nPartyIndex) {
    BOOL bBlizzcon = CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);
    int32 * arNamePlateVisualOrder = bBlizzcon ? s_arNamePlateVisualOrderBlizzcon : s_arNamePlateVisualOrder;
    if (UIPlayerOptions::Console::IsVisible() && sgtPartyDisplayData.nPlayerInfoPartyMember == nPartyIndex) {
        UIPlayerOptions::Console::ClosePlayerOptions();
        sgtPartyDisplayData.nPlayerInfoPartyMember = -1;
    }
    if (sgtPartyDisplayData.bNamePlatesEnabled && sgtPartyDisplayData.nFocusedNameplate != -1 &&
        s_arNamePlatePartyIndices[arNamePlateVisualOrder[sgtPartyDisplayData.nFocusedNameplate]] == nPartyIndex) {
        s_arNamePlatePartyIndices[arNamePlateVisualOrder[sgtPartyDisplayData.nFocusedNameplate]] = -1;
        if (sgtPartyDisplayData.nFocusedNameplate == 0)
            SelectNextNameplate();
        else
            SelectPreviousNameplate();
    }
}













void ShowPartyDisplay() {
    RefreshPartyDisplay();
    UIConsoleListBox & tPartyList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY);
    tPartyList.SetSelectedIndex(-1);
    tPartyList.SetOnScrolledOffTopFunction(OnScrolledOffListTop);
    tPartyList.SetOnScrolledOffBottomFunction(OnScrolledOffListBottom);
    sgtPartyDisplayData.ptVoiceListener->Enable(TRUE);
    UI_GetWindowFromID(UIWND_CONSOLE_MENU_PARTY_CONTENT).SetActive(TRUE);
    UI_GetWindowFromID(UITEXT_CONSOLE_NETWORK_HEADER).SetActive(TRUE);
    ::Console::Online::GetPartyMemberRemovedEvent().Subscribe(
        OnlineService::internal::function_bind_0_1<void, int, &OnEventPartyMemberRemoved>());
}








void HidePartyDisplay() {
    UI_GetWindowFromID(UIWND_CONSOLE_MENU_PARTY_CONTENT).SetActive(FALSE);
    UI_GetWindowFromID(UITEXT_CONSOLE_NETWORK_HEADER).SetActive(FALSE);
    sgtPartyDisplayData.ptVoiceListener->Enable(FALSE);
    if (UIPlayerOptions::Console::IsVisible() && sgtPartyDisplayData.nPlayerInfoPartyMember != -1) {
        UIPlayerOptions::Console::ClosePlayerOptions();
        sgtPartyDisplayData.nPlayerInfoPartyMember = -1;
    }
    ::Console::Online::GetPartyMemberRemovedEvent().Unsubscribe(
        OnlineService::internal::function_bind_0_1<void, int, &OnEventPartyMemberRemoved>());
}







void ShowFriendDisplay(BOOL bShowWhenEmpty)
{
    ::Console::Online::PresenceManager::SetListingEnabled(TRUE);
    RefreshFriendsDisplay();
    UIConsoleListBox & tFriendsList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST);
    tFriendsList.SetSelectedIndex(-1);
    tFriendsList.SetOnScrolledOffTopFunction(OnScrolledOffListTop);
    tFriendsList.SetOnScrolledOffBottomFunction(OnScrolledOffListBottom);
    sgtPartyDisplayData.tFriendList.m_bShowWhenEmpty = bShowWhenEmpty;
    UI_GetWindowFromID(UIWND_CONSOLE_FRIENDS_CONTENT).SetActive(TRUE);
    UI_GetWindowFromID(UITEXT_CONSOLE_FRIENDS_HEADER).SetActive(TRUE);
}






void HideFriendDisplay()
{
    UI_GetWindowFromID(UIWND_CONSOLE_FRIENDS_CONTENT).SetActive(FALSE);
    UI_GetWindowFromID(UITEXT_CONSOLE_FRIENDS_HEADER).SetActive(FALSE);
    if (!GameIsStartUp())
        ::Console::Online::PresenceManager::SetListingEnabled(FALSE);
    if (UIPlayerOptions::Console::IsVisible() && sgtPartyDisplayData.nPlayerInfoPartyMember == -1)
        UIPlayerOptions::Console::ClosePlayerOptions();
}







void ShowLocalCommunicationsDisplay()
{
    UIConsoleListBox & tLocalCommList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST);
    tLocalCommList.SetSelectedIndex(-1);
    tLocalCommList.SetOnScrolledOffTopFunction(OnScrolledOffListTop);
    tLocalCommList.SetOnScrolledOffBottomFunction(OnScrolledOffListBottom);
    UI_GetWindowFromID(UIWND_CONSOLE_LOCAL_COMMUNICATION_CONTENT).SetActive(TRUE);
    UI_GetWindowFromID(UITEXT_CONSOLE_LOCAL_COMMUNICATION_HEADER).SetActive(TRUE);
}






void HideLocalCommunicationsDisplay()
{
    UI_GetWindowFromID(UIWND_CONSOLE_LOCAL_COMMUNICATION_CONTENT).SetActive(FALSE);
    UI_GetWindowFromID(UITEXT_CONSOLE_LOCAL_COMMUNICATION_HEADER).SetActive(FALSE);
    if (UIPlayerOptions::Console::IsVisible() && sgtPartyDisplayData.nPlayerInfoPartyMember == -1)
        UIPlayerOptions::Console::ClosePlayerOptions();
}






void FindLocalCommunicationGames()
{
    sgtPartyDisplayData.tLocalCommList.RefreshGameList();
}



void OnAdHocPartyJoined(::Console::Online::JoinResult eResult)
{
    ::Console::UIOnlineActions::ClearJoinAdHocNetworkInProgress();
    sgtPartyDisplayData.tLocalCommList.m_nNumMatchesAvailable = 0;
}



void ClearNameplateHighlight()

{
    EffectGroupOutro(sgtPartyDisplayData.idHighlightEffect);
    sgtPartyDisplayData.idHighlightEffect = EFFECTGROUPID_INVALID;
}


CRefString GetTextureKeyForPartyMember(
    uint8 nPartyId)
{
    return CRefString().Format("UI:Console_TextFX_GlowP%d", nPartyId + 1);
}


void RefreshPartyNameplates()

{
    RefreshPartyData();

    EffectGroup * ptHighlightEffect = EffectGroupTryToGet(sgtPartyDisplayData.idHighlightEffect);
    if (!ptHighlightEffect ||
        !RActorTryToGet(ptHighlightEffect->idRActorOwner))
    {
        ClearNameplateHighlight();
    }

    BOOL bBlizzcon = CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);
    NamePlateIds * arNamePlates = bBlizzcon ? s_arNamePlatesBlizzcon : s_arNamePlates; // Inference: fixed four-slot count folds away; original loop bound is four.
    int32 nNumNamePlates = 4;
    int32 * arNamePlateVisualOrder = bBlizzcon ? s_arNamePlateVisualOrderBlizzcon : s_arNamePlateVisualOrder;
    // Inference: bitmask starts empty; all observed stores OR in encountered player bits.
    uint32 dwTakenPartyMembers = 0;
    int32 nHeroSlot = sgtPartyDisplayData.nNumPartyMembers != 0;
    const uint32 nNumControllers = InputGetNumConnectedControllers();
    const int32 nNumInactiveLocalControllers = nNumControllers - sgtPartyDisplayData.nNumLocalPartyMembers;
    for (uint32 i = 0; i < nNumNamePlates; ++i)
    {
        NamePlateIds * ptNamePlateIds; // Inference: unselected default is shared line-zero code before both branches.
        BOOL bSelected = FALSE;
        if (i < sgtPartyDisplayData.nNumPartyMembers)
        {
            ::Console::Online::PartyMember & tPartyMember = sgtPartyDisplayData.arPartyMembers[i];

            BackgroundHeroSlot eHeroSlot;
            if (tPartyMember.bIsLocal &&
                tPartyMember.bIsPrimary)
            {
                eHeroSlot = HERO_SLOT_1;
                ptNamePlateIds = &arNamePlates[0];
                s_arNamePlatePartyIndices[0] = i;
                if (sgtPartyDisplayData.nFocusedNameplate != -1)
                {
                    bSelected = arNamePlateVisualOrder[sgtPartyDisplayData.nFocusedNameplate] == 0;
                }
            }
            else
            {
                eHeroSlot = nHeroSlot;
                if (sgtPartyDisplayData.nFocusedNameplate != -1)
                {
                    bSelected = arNamePlateVisualOrder[sgtPartyDisplayData.nFocusedNameplate] == nHeroSlot;
                }
                s_arNamePlatePartyIndices[nHeroSlot] = i;
                ptNamePlateIds = &arNamePlates[nHeroSlot++];

                nHeroSlot = Math::Min(nHeroSlot, nNumNamePlates - 1);
            }
            dwTakenPartyMembers |= 1 << tPartyMember.idPlayer;
            FrontendBackgroundHeroManager & tHeroManager = ::Console::Screens::GetFrontendBackgroundHeroManager();
            RActorID idHeroRActor = tHeroManager.GetHeroRActorId(eHeroSlot);
            if (RActorTryToGet(idHeroRActor))
            {
                if (ptHighlightEffect && ptHighlightEffect->idRActorOwner == idHeroRActor && !bSelected)
                {

                    ClearNameplateHighlight();
                    ptHighlightEffect = NULL;
                } else
                if (!ptHighlightEffect && bSelected)
                {
                    sgtPartyDisplayData.idHighlightEffect =
                        EffectGroupCreateTracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_CHARSELECT), idHeroRActor, RACTORID_INVALID, 0, FALSE);
                }
            }

            CRefString sPlayerLabel = LocalizedSprintf(
                StringListFetch("Console:PlayerLabelShort").str(),
                UnicodeText::FormatNumber(tPartyMember.idPlayer + 1, 1, FNS_NORMAL, 0).str());
            Set_Text_Color(sPlayerLabel, UIUtils::Console::GetPlayerColor(tPartyMember.idPlayer));
            if (::Console::Online::PartyGetHostIndex() == tPartyMember.idPlayer)
            {
                sPlayerLabel.Format("%s %s", "{icon:Console_PartyLeaderIcon}", sPlayerLabel.str());
            }
            if (sgtPartyDisplayData.nNumPartyMembers != sgtPartyDisplayData.nNumLocalPartyMembers)
            {




                UIUtils::Console::VoiceChatState eVoiceChatState =
                    sgtPartyDisplayData.ptVoiceListener->GetVoiceChatState(tPartyMember.idPlayer);
                if (eVoiceChatState != UIUtils::Console::VOICE_NONE)
                {
                    sPlayerLabel.Format("%s {icon:%s, 2}", sPlayerLabel.str(), UIUtils::Console::GetVoiceChatTextureName(eVoiceChatState).str());
                }
            }

            CRefString sPlayerName(tPartyMember.szName);
            if (::Console::Online::IsInPlatformParty(tPartyMember.idOnlineUser))
            {
                Set_Text_Color(sPlayerName, UIUtils::Console::GetXBLPartyColor());
            }

            sPlayerLabel.Format("%s\n%s", sPlayerLabel.str(), sPlayerName.str());
            if (ptNamePlateIds->pidText)
            {
                UI_GetTextFromID(*ptNamePlateIds->pidText).SetText(sPlayerLabel, FALSE);
            }
            if (ptNamePlateIds->pidWindow)
            {
                UI_GetWindowFromID(*ptNamePlateIds->pidWindow).SetActive(!bSelected);
            }
            if (ptNamePlateIds->pidBackground)
            {
                CRefString sTextureKey = GetTextureKeyForPartyMember(tPartyMember.idPlayer);
                ImageTextureFrame tGlow(sTextureKey);
                UI_GetWindowFromID(*ptNamePlateIds->pidBackground).SetTextureFrame(tGlow);
            }


            if (ptNamePlateIds->pidInGame)
            {
                UI_GetWindowFromID(*ptNamePlateIds->pidInGame).SetActive(tPartyMember.bIsInGame);
            }


            if (ptNamePlateIds->pidError)
            { // Inference: validation defaults to OK when no digest or already in-game; original branch skips validation.
                ::Console::UIOnlineActions::HeroValidationError eResult = ::Console::UIOnlineActions::HERO_VALIDATION_OK;
                if (tPartyMember.tHeroDigest && !tPartyMember.bIsInGame)
                {
                    eResult = ::Console::UIOnlineActions::ValidateHeroForPartyMember(tPartyMember);
                }

                UI_GetWindowFromID(*ptNamePlateIds->pidError).SetActive(eResult != ::Console::UIOnlineActions::HERO_VALIDATION_OK);
                if (eResult != ::Console::UIOnlineActions::HERO_VALIDATION_OK)
                {
                    UIID idText = UIIDCreateFromParent(*ptNamePlateIds->pidError, ".Text");
                    UI_GetTextFromID(idText).SetText(::Console::UIOnlineActions::GetHeroValidationErrorMessage(eResult, FALSE), FALSE);
                }
            }

            if (!bSelected && tPartyMember.tHeroDigest && tPartyMember.tHeroDigest->IsInitialized())
            {
                ::Console::Online::GameParams tGameParams;
                if (::Console::Online::PartyGetGameParams(tGameParams) && tGameParams.m_eGameType == GT_CHALLENGE_RIFTS)
                {
                    UI_GetWindowFromID(*ptNamePlateIds->pidPortrait).SetActive(FALSE);
                }
                else
                {
                    PlayerClass eClass = PlayerClassFromGBID(tPartyMember.tHeroDigest->gbid_class());
                    BOOL bFemale = (tPartyMember.tHeroDigest->player_flags() & (1 << PLAYERFLAG_FEMALE)) != 0;
                    // Inference: invalid GBID remains when no cosmetic portrait entry exists, as original initializes -1.
                    GBID gbidPortraitFrame = -1;
                    // Original guard accepts size >=3 before reading slot3; retain its exact boundary.
                    if (tPartyMember.tHeroDigest->has_visual_equipment() && tPartyMember.tHeroDigest->visual_equipment().cosmetic_item_size() >= VCIS_PORTRAIT)
                    {
                        gbidPortraitFrame = tPartyMember.tHeroDigest->visual_equipment().cosmetic_item(VCIS_PORTRAIT).gbid();
                    }

                    UIUtils::Console::DisplayPortrait(eClass, bFemale, tPartyMember.tHeroDigest->deprecated_alt_level(), *ptNamePlateIds->pidPortrait, gbidPortraitFrame);

                    UI_GetWindowFromID(*ptNamePlateIds->pidPortrait).SetActive(TRUE);
                }
            }
        }
        else
        {
            if (sgtPartyDisplayData.nFocusedNameplate != -1)
            {
                bSelected = arNamePlateVisualOrder[sgtPartyDisplayData.nFocusedNameplate] == nHeroSlot;
            }
            if (s_arNamePlatePartyIndices[nHeroSlot] != -1)
            {
                OnEventPartyMemberRemoved(s_arNamePlatePartyIndices[nHeroSlot]);
                s_arNamePlatePartyIndices[nHeroSlot] = -1;
            }
            ptNamePlateIds = &arNamePlates[nHeroSlot++];

            UI_GetWindowFromID(*ptNamePlateIds->pidPortrait).SetActive(FALSE);


            nHeroSlot = Math::Min(nHeroSlot, nNumNamePlates - 1);
            CRefString sName;

            const int32 nNthOpenSlot = i - sgtPartyDisplayData.nNumPartyMembers;
            if (nNthOpenSlot < nNumInactiveLocalControllers)
            {

                sName = StringListFetch("Console:PressToJoinSwitch");



            }
            else if (sgtPartyDisplayData.nNumPartyMembers > sgtPartyDisplayData.nNumLocalPartyMembers &&
                ::Console::Online::PartyIsOnline())
            {
                sName = StringListFetch("Console:InviteFriend");
            }

            if (ptNamePlateIds->pidBackground)
            { // Inference: default zero player index survives only if all four bits are occupied.
                uint8 nPartyId = 0;

                for (uint8 j = 0; j < 4; ++j)
                {
                    if (!(dwTakenPartyMembers & (1 << j)))
                    {
                        nPartyId = j;
                        dwTakenPartyMembers |= 1 << j;
                        break;
                    }
                }
                CRefString sTextureKey = GetTextureKeyForPartyMember(nPartyId);
                ImageTextureFrame tGlow(sTextureKey);
                UI_GetWindowFromID(*ptNamePlateIds->pidBackground).SetTextureFrame(tGlow);
            }
            if (sName.Length())
            {
                UI_GetTextFromID(*ptNamePlateIds->pidText).SetText(sName, FALSE);
                UI_GetWindowFromID(*ptNamePlateIds->pidWindow).SetActive(TRUE);
            }
            else
            {
                if (bSelected)
                {
                    if (sgtPartyDisplayData.nFocusedNameplate == 0)
                    {
                        SelectNextNameplate();
                    }
                    else
                    {
                        SelectPreviousNameplate();
                    }
                }
                if (ptNamePlateIds->pidWindow)
                {
                    UI_GetWindowFromID(*ptNamePlateIds->pidWindow).SetActive(FALSE);
                }
            }


            if (ptNamePlateIds->pidInGame)
            {
                UI_GetWindowFromID(*ptNamePlateIds->pidInGame).SetActive(FALSE);
            }
            if (ptNamePlateIds->pidError)
            {
                UI_GetWindowFromID(*ptNamePlateIds->pidError).SetActive(FALSE);
            }
        }

        if (sgtPartyDisplayData.nPlayerInfoPartyMember == i && !bSelected)
        {

            UIPlayerOptions::Console::ClosePlayerOptions();
            sgtPartyDisplayData.nPlayerInfoPartyMember = -1;
        }

        if (sgtPartyDisplayData.nFocusedNameplate != -1 &&
            sgtPartyDisplayData.nPlayerInfoPartyMember == -1 &&
            bSelected &&
            i < sgtPartyDisplayData.nNumPartyMembers &&
            !UIPlayerOptions::Console::IsVisible())
        {
                sgtPartyDisplayData.nPlayerInfoPartyMember = i;
                UIPlayerOptions::Console::ShowPartyPlayerOptions(
                    ::Console::GamerProfile::GetPrimaryProfileUserIndex(),
                    &sgtPartyDisplayData.arPartyMembers[i],
                    *ptNamePlateIds->pidAnchor);
        }

        if (UIPlayerOptions::Console::IsVisible() &&
            sgtPartyDisplayData.nPlayerInfoPartyMember != -1)
        {
            UIPlayerOptions::Console::RefreshPlayerOptions(NULL);
        }
    }

}


void EnablePartyNameplates(BOOL bEnable)
{
    sgtPartyDisplayData.bNamePlatesEnabled = bEnable;
    sgtPartyDisplayData.ptVoiceListener->Enable(bEnable);
    if (bEnable)
    {
        RefreshPartyNameplates();
        return;
    }
    ClearNamePlateSelection();
}





// These namespace helpers have only inline copies in the original. The loop
// index i and temporary nNamePlate names are invented: DWARF omits those locals.
inline int32 FindNextVisibleNameplate(BOOL bRight) {
    int32 nNumNamePlates = 4;
    int32 * arNamePlateVisualOrder = CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT)
        ? s_arNamePlateVisualOrderBlizzcon : s_arNamePlateVisualOrder;
    if (bRight) {
        for (int32 i = sgtPartyDisplayData.nFocusedNameplate + 1; i < nNumNamePlates; ++i) {
            uint32 nPartyDataIndex = s_arNamePlatePartyIndices[arNamePlateVisualOrder[i]];
            if (nPartyDataIndex < sgtPartyDisplayData.nNumPartyMembers &&
                UI_GetWindowFromID(*s_arNamePlates[arNamePlateVisualOrder[i]].pidWindow).IsActive())
                return i;
        }
    } else {
        for (int32 i = (sgtPartyDisplayData.nFocusedNameplate == -1 ? nNumNamePlates : sgtPartyDisplayData.nFocusedNameplate) - 1; i >= 0; --i) {
            uint32 nPartyDataIndex = s_arNamePlatePartyIndices[arNamePlateVisualOrder[i]];
            if (nPartyDataIndex < sgtPartyDisplayData.nNumPartyMembers &&
                UI_GetWindowFromID(*s_arNamePlates[arNamePlateVisualOrder[i]].pidWindow).IsActive())
                return i;
        }
    }
    return -1;
}




















BOOL IsNamePlateSelected()
{
    return sgtPartyDisplayData.bNamePlatesEnabled && sgtPartyDisplayData.nFocusedNameplate != -1;
}



void ClearNamePlateSelection()
{
    if (UIPlayerOptions::Console::IsVisible() && sgtPartyDisplayData.nPlayerInfoPartyMember != -1)
        UIPlayerOptions::Console::ClosePlayerOptions();
    sgtPartyDisplayData.nPlayerInfoPartyMember = -1;
    sgtPartyDisplayData.nFocusedNameplate = -1;
    EffectGroupOutro(sgtPartyDisplayData.idHighlightEffect);
    sgtPartyDisplayData.idHighlightEffect = -1;
}





inline BOOL SelectNamePlate(BOOL bRight) {
    if (!sgtPartyDisplayData.bNamePlatesEnabled) return FALSE;
    int32 nNamePlate = FindNextVisibleNameplate(bRight);
    if (nNamePlate == -1) return FALSE;
    sgtPartyDisplayData.nFocusedNameplate = nNamePlate;
    UI_SetControllerFocus(GetUIIDNone());
    return TRUE;
}












BOOL SelectNextNameplate() { return SelectNamePlate(TRUE); }






BOOL SelectPreviousNameplate() { return SelectNamePlate(FALSE); }






BOOL ShouldDirectFriendsForInvite()
{
    uint32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY).GetSelectedIndex();
    if (nSelectedIndex != (uint32)-1 && ::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsOnline() &&
        nSelectedIndex >= sgtPartyDisplayData.nNumPartyMembers)
        return TRUE;
    return FALSE;
}










void OnPartyListAcceptPressed(int32 nUserIndex)
{
    uint32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY).GetSelectedIndex();
    if (nSelectedIndex == (uint32)-1 || !::Console::Online::PartyIsInParty())
        return;
    if (::Console::Online::PartyIsOnline() && nSelectedIndex >= sgtPartyDisplayData.nNumPartyMembers)
    {
        ::Console::Utilities::ShowGameInviteUI(nUserIndex);
        return;
    }
    if (nSelectedIndex < sgtPartyDisplayData.nNumPartyMembers)
    {
        sgtPartyDisplayData.nPlayerInfoPartyMember = nSelectedIndex;
        UIPlayerOptions::Console::ShowPartyPlayerOptions(nUserIndex, &sgtPartyDisplayData.arPartyMembers[(int32)nSelectedIndex], UILIST_CONSOLE_MENU_PARTY);
    }
}










void OnFriendsListAcceptPressed(int32 nUserIndex)
{
    int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).GetSelectedIndex();
    if (nUserIndex >= 0 && nSelectedIndex != -1)
    {
        sgtPartyDisplayData.nPlayerInfoPartyMember = -1;
        ::Console::Online::Friend & tFriend = sgtPartyDisplayData.tFriendList.m_arFriends[nSelectedIndex];
        sgtPartyDisplayData.tPlayerInfoFriendId = tFriend.m_idFriend;
        UIPlayerOptions::Console::ShowFriendPlayerOptions(nUserIndex, &tFriend, UILIST_CONSOLE_FRIENDS_LIST);
    }
}



void OnLocalCommListAcceptPressed(int32 nUserIndex)
{
    int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).GetSelectedIndex();
    if (nUserIndex >= 0 && nSelectedIndex != -1)
    {
        sgtPartyDisplayData.nPlayerInfoPartyMember = -1;
        LocalCommunicationsDataSource::PerPlayerPartyInfo & tInfo = sgtPartyDisplayData.tLocalCommList.m_arAdHocPartyResult[nSelectedIndex];
        UIPlayerOptions::Console::ShowLocalCommPlayerOptions(nUserIndex, tInfo.m_nPlayerIndex, &tInfo.m_tPartyinfo, UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST);
    }
}






void OnPartyNamePlateAcceptPressed(int32 nUserIndex)
{
    if (sgtPartyDisplayData.nFocusedNameplate == -1)
        return;
    int32 * arNamePlateVisualOrder = CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) ? s_arNamePlateVisualOrderBlizzcon : s_arNamePlateVisualOrder;
    int32 nPartyIndex = s_arNamePlatePartyIndices[arNamePlateVisualOrder[sgtPartyDisplayData.nFocusedNameplate]];
    if (::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsOnline() && nPartyIndex == -1)
        ::Console::Utilities::ShowGameInviteUI(nUserIndex);
}















void PopulateAdHocListElement(
    const UIID & idItemUI,
    PlayerClass eClass,
    BOOL bFemale,
    CRefString sName,
    int32 nLevel,
    int32 nAltLevel,
    GroupBannerDisplay eBannerDisplay,
    BOOL bSelected)
{
    if (!UI_TestID(idItemUI)) return;
    UIID idName = UIIDCreateFromParent(idItemUI, ".PlayerName");
    UIID idLevel = UIIDCreateFromParent(idItemUI, ".PlayerLevel");
    UIID idHighlight = UIIDCreateFromParent(idItemUI, ".highlight");

    UIID idDuoBanner = UIIDCreateFromParent(idItemUI, ".GroupDuo");
    UIID idTrioBanner = UIIDCreateFromParent(idItemUI, ".GroupTrio");

    UI_GetWindowFromID(idDuoBanner).SetActive(eBannerDisplay == Duo_Banner_Display);
    UI_GetWindowFromID(idTrioBanner).SetActive(eBannerDisplay == Trio_Banner_Display);

    UI_GetWindowFromID(idHighlight).SetActive(bSelected);

    UIText & tName = UI_GetTextFromID(idName);
    UIText & tLevel = UI_GetTextFromID(idLevel);

    RGBAColor rgbaColor;

    UIColor eColor = UI_GetXMLUint32Constant("ConsolePartyDisplay:Normal", crgbaWhite.ToUIColor());
    rgbaColor.Set(eColor);

    CRefString sIcon;
    if (bSelected && UIPlayerOptions::Console::IsVisible())
    {
        sIcon = "button2";
    }
    else if (bSelected && !UIPlayerOptions::Console::IsVisible())
    {
        sIcon = "button1";
    }
    else if (nLevel > 0)
    {
        sIcon = GetClassPortrait(eClass, bFemale);
    }

    tName.SetText(sName, FALSE);
    tName.SetTextColor(rgbaColor);
    if (nLevel <= 0)
    {
        CRefString sText;
        if (sIcon.Length() > 0)
        {
            sText.Format("{icon:%s, 2}", sIcon.str());
        } // Inference: the shared line-zero SetText tail also runs when the icon is empty.
        tLevel.SetText(sText, FALSE);
    }
    else
    {
        CRefString sLevel = CPlayerGetLevelString(nLevel, nAltLevel);
        sLevel.Format("%s {icon:%s, 2}", sLevel.str(), sIcon.str()); // Inference: next SetText is the shared line-zero tail.
        tLevel.SetText(sLevel, FALSE);
    }
}

void PopulateListElement(
    const UIID & idItemUI,
    PlayerClass eClass,
    BOOL bFemale,
    CRefString sName,
    int32 nLevel,
    int32 nAltLevel,
    UIUtils::Console::VoiceChatState eVoiceChatState,
    BOOL bSelected,
    BOOL bXBLPartyMember)
{
    if (!UI_TestID(idItemUI)) return;
    UIID idName = UIIDCreateFromParent(idItemUI, ".PlayerName");
    UIID idLevel = UIIDCreateFromParent(idItemUI, ".PlayerLevel");
    UIID idHighlight = UIIDCreateFromParent(idItemUI, ".highlight");
    UIID idVoiceChat = UIIDCreateFromParent(idItemUI, ".VoiceChat");
    UIID idWizard = UIIDCreateFromParent(idItemUI, ".WizardIcon");
    UIID idMonk = UIIDCreateFromParent(idItemUI, ".MonkIcon");
    UIID idWitchDoctor = UIIDCreateFromParent(idItemUI, ".WitchdoctorIcon");
    UIID idBarb = UIIDCreateFromParent(idItemUI, ".BarbarianIcon");
    UIID idDemonHunter = UIIDCreateFromParent(idItemUI, ".DemonhunterIcon");


    UI_GetWindowFromID(idHighlight).SetActive(bSelected);

    ImageTextureFrame tTexture = UIUtils::Console::GetVoiceChatTexture(eVoiceChatState);
    UI_GetWindowFromID(idVoiceChat).SetTextureFrame(tTexture);

    UIText & tName = UI_GetTextFromID(idName);
    UIText & tLevel = UI_GetTextFromID(idLevel);

    RGBAColor rgbaColor;
    if (bXBLPartyMember)
    {
        rgbaColor = UIUtils::Console::GetXBLPartyColor();
    }
    else
    {
        UIColor eColor = UI_GetXMLUint32Constant("ConsolePartyDisplay:Normal", crgbaWhite.ToUIColor());
        rgbaColor.Set(eColor);
    }

    CRefString sIcon;
    if (bSelected && UIPlayerOptions::Console::IsVisible())
    {
        sIcon = "button2";
    }
    else if (bSelected && !UIPlayerOptions::Console::IsVisible())
    {
        sIcon = "button1";
    }
    else if (nLevel > 0)
    {
        sIcon = GetClassPortrait(eClass, bFemale);
    }

    tName.SetText(sName, FALSE);
    tName.SetTextColor(rgbaColor);
    if (nLevel <= 0)
    {
        CRefString sText;
        if (sIcon.Length() > 0)
        {
            sText.Format("{icon:%s, 2}", sIcon.str());
        } // Inference: the shared line-zero SetText tail also runs when the icon is empty.
        tLevel.SetText(sText, FALSE);
    }
    else
    {
        CRefString sLevel = CPlayerGetLevelString(nLevel, nAltLevel);
        sLevel.Format("%s {icon:%s, 2}", sLevel.str(), sIcon.str()); // Inference: next SetText is the shared line-zero tail.
        tLevel.SetText(sLevel, FALSE);
    }

    ControlVisibilityManager tClassIcons;

    tClassIcons.AddControl(idDemonHunter);
    tClassIcons.AddControl(idBarb);
    tClassIcons.AddControl(idWizard);
    tClassIcons.AddControl(idWitchDoctor);
    tClassIcons.AddControl(idMonk);
    tClassIcons.SetActive(eClass, false);
}


CRefString GetClassPortrait(
    PlayerClass eClass,
    BOOL bFemale)
{
    CRefString sPortraitKey;
    if (eClass != PLAYER_CLASS_NONE)
    { // Inference: the class-zero literal branch is line-zero code immediately before SymbolToName.
        LPCSTR szClassName = eClass == PLAYER_CLASS_DEMONHUNTER ? "Demonhunter" : SymbolToName(eClass, gPlayerClasses);
        sPortraitKey.Format("Portrait_%s_%s", szClassName, SymbolToName(bFemale, gActorGenderNames));
    }
    return sPortraitKey;
}


bool FriendListSortPred(const ::Console::Online::Friend & tFriendA, const ::Console::Online::Friend & tFriendB)
{
    if (tFriendA.m_bInPlatformParty != tFriendB.m_bInPlatformParty)
        return tFriendA.m_bInPlatformParty != FALSE;
    if (tFriendA.m_bInGame != tFriendB.m_bInGame)
        return tFriendA.m_bInGame != FALSE;
    if (tFriendA.m_bFavorite != tFriendB.m_bFavorite)
        return tFriendA.m_bFavorite != FALSE;
    return strcasecmp(tFriendA.m_szUsername, tFriendB.m_szUsername) < 0;
}














bool LocalCommPartySortPred(const ::Console::Online::PartyInfo & tPartyA, const ::Console::Online::PartyInfo & tPartyB)
{
    if (tPartyA.m_tGameInfo.m_uNumPlayers != tPartyB.m_tGameInfo.m_uNumPlayers)
        return tPartyA.m_tGameInfo.m_uNumPlayers > tPartyB.m_tGameInfo.m_uNumPlayers;
    return strcasecmp(tPartyA.m_szName, tPartyB.m_szName) != 0;
}

} }
