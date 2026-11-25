// Original file: X/Client/UserInterface/Console/UIConsoleUtils.cpp
//
// Small helpers shared by the console screens: voice-chat icons, party state strings,
// hero panels, recipe requirements, scroll boxes, paragon levels and item dropping.
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "UIConsoleUtils.h"
#include "ConsoleScreenManager.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../CPlayer.h"
#include "../../Console/Online.h"
#include "../../../Common/Player.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../Gfx/GfxCommon.h"
#include "../Controls/UIStackPanelControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Crafting/ItemCraftingSwig.h"
#include "../../../Common/Crafting/ItemCrafting.h"
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/TagMaps/ActorTags.h"
#include "../../../Common/Partition.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/NephalemRiftCommon.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/Game.h"
#include "../../../Common/Attributes/FastAttribute.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Interface/UIGame.h"
#include "../../Sound/Sound.h"
#include "../../../Sigma/UnicodeText.h"
// Declared here because their headers are not adopted yet. Each comment names the defining file.
CRefString StringListFetch(LPCSTR szLabel);  // X/Common/Definitions/StringList.cpp
CRefString PlayerGetHeroDisplayName(const Player & tPlayer, BOOL fDisplayPound);  // X/Common/Player.cpp
CRefString CPlayerGetLevelString(const int32 nLevel, const int32 nAltLevel);  // X/Client/CPlayer.cpp
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
uint32 UI_GetXMLUint32Constant(LPCSTR szName, uint32 dwDefault);  // X/Client/UserInterface/UI.cpp
namespace Console { namespace Online {  // X/Client/Console/VoiceChat*.cpp
typedef int32 PartyMuteState;
BOOL IsVoiceChatRestricted();
PartyMuteState IsPlayerMuted(int32 nTargetPartyMemberIndex);
BOOL IsPartyMemberMicConnected(int32 nPartyMemberIndex);
} }
namespace UILeaderboardInspect { namespace Console { void OpenForChallengeRift(int32 nControllerIndex); } }  // UIConsoleLeaderboardInspect.cpp
namespace Console { namespace GamerProfile { int32 GetPrimaryProfileUserIndex(); } }  // GamerProfileNX64.cpp
extern UIID UITXT_CONSOLE_PLAYER_LABEL_RING;
extern UIID UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL;
UIStackPanel & UI_GetStackPanelFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
CrafterType NPCGetCrafterType(const ActorCommonData & tACD);  // X/Common/NPC.cpp  // X/Client/UserInterface/UIIDs.cpp

namespace UIUtils {
namespace Console {

void YieldFocus(const UIID & idYielding);
BOOL PlayerIsInActiveTieredRift(const Player * ptPlayer);
void sDropStack(const ACDID idACDStack, const uint64 uAmount);
void sUpdateEmptyRecipeRequirementsUI(const UIID & idParent);
void UpdateRecipeRequirementsUI(const UIID & idParent, const GBID gbidRecipe, const Player & tPlayer);
void sUpdateNormalRecipeRequirementsUI(const UIID & idParent, const ACDID idACDPlayer, uint64 nCost, const RecipeIngredient * arRecipeIngredients, uint32 nNumIngredients);
void UpdateRecipeRequirementsUI(const UIID & idParent, const Player & tPlayer, uint64 nCost, const RecipeIngredient * arRecipeIngredients, uint32 nNumIngredients);

enum {
    VOICE_CHAT_ICON_DISPLAY_TIME = 60,
};

class UIUtilsGlobals {
public:
    UIUtilsGlobals() : m_idACDPlayerDroppingStack(ACDID_INVALID) {}
    ACDID m_idACDPlayerDroppingStack;
};




static UIUtilsGlobals sgtUIUtilsGlobals;


PartyVoiceChatStateListener::PartyVoiceChatStateListener()
: m_bEnabled(FALSE)
{
    SigmaMemoryClear(m_arLastTalkTime, sizeof(m_arLastTalkTime));
}

PartyVoiceChatStateListener::~PartyVoiceChatStateListener()
{
    if (m_bEnabled)
        Enable(FALSE);
}




UIUtils::Console::VoiceChatState PartyVoiceChatStateListener::GetVoiceChatState(int32 nPartyIndex) const
{
    UIUtils::Console::VoiceChatState eVoiceState;
    if (::Console::Online::IsPlayerMuted(nPartyIndex))
        eVoiceState = VOICE_MUTED;
    else if (AppGetTicks() - m_arLastTalkTime[nPartyIndex] <= VOICE_CHAT_ICON_DISPLAY_TIME)
        eVoiceState = VOICE_TALKING;
    else if (::Console::Online::IsPartyMemberMicConnected(nPartyIndex))
        eVoiceState = VOICE_CONNECTED;
    else
        eVoiceState = VOICE_NONE;
    return eVoiceState;
}
// Online.cpp:5242; canonical event accessor signature from DWARF.
} }
namespace Console { namespace Online { PartyMemberVoiceChatTalkingEvent & GetPartyMemberVoiceChatTalkingEvent(); } }
namespace UIUtils { namespace Console {



void PartyVoiceChatStateListener::Enable(
    BOOL bEnabled)
{
    if (bEnabled)
    {
        ::Console::Online::GetPartyMemberVoiceChatTalkingEvent().Subscribe(OnlineService::internal::method_bind_0_1<void, PartyVoiceChatStateListener, PartyVoiceChatStateListener *, int32, &PartyVoiceChatStateListener::OnEventPartyMemberSpoke>(this));
    }
    else
    {
        ::Console::Online::GetPartyMemberVoiceChatTalkingEvent().Unsubscribe(OnlineService::internal::method_bind_0_1<void, PartyVoiceChatStateListener, PartyVoiceChatStateListener *, int32, &PartyVoiceChatStateListener::OnEventPartyMemberSpoke>(this));
    }
    m_bEnabled = bEnabled;
}

// Required callback helper: concrete DWARF02192bf8 low_pc0; standalone body is not checkable.
void PartyVoiceChatStateListener::OnEventPartyMemberSpoke(
    int32 nPartyIndex)
{
    if (!::Console::Online::IsVoiceChatRestricted())
    {
        m_arLastTalkTime[nPartyIndex] = AppGetTicks();
    }
}


CRefString GetVoiceChatTextureName(UIUtils::Console::VoiceChatState eVoiceChatState)
{
    switch (eVoiceChatState)
    {
    case VOICE_TALKING:
        return CRefString("Console_VoiceChat_Speaking");
    case VOICE_CONNECTED:
        return CRefString("Console_VoiceChat_HeadSet");
    case VOICE_MUTED:
        return CRefString("Console_VoiceChat_Muted");
    case VOICE_NONE:
    default:
        return CRefString();
    }
}








ImageTextureFrame GetVoiceChatTexture(UIUtils::Console::VoiceChatState eVoiceChatState)
{
    CRefString sTextureName = GetVoiceChatTextureName(eVoiceChatState);
    if (sTextureName.Length())
    {
        CRefString sFormat;
        return ImageTextureFrame(sFormat.Format("UI:%s", sTextureName.str()));
    }
    return ImageTextureFrame();
}
// UIConsoleUtils.cpp:370, original DWARF signature.
RGBAColor GetPlayerColor(const int32 nPlayerIndex);















// NON_MATCHING: 732/732 bytes; two PinWrap compare/select instructions differ in inherited GetPlayerColor; restored fallback count, no header variants.
CRefString PlayerGetDecoratedPlayerNum(
    const int32 nControllerIndex)
{
    int32 nPlayerNum = -1;

    if (!GameIsStartUp())
    {
        Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
        if (ptPlayer)
        {
            nPlayerNum = ptPlayer->nPlayerIndex;
        }
    }

    if (nPlayerNum == -1 && ::Console::Online::PartyIsInParty())
    {
        ::Console::Online::PartyMember tPartyMembers[4];
        int32 nNumPartyMembers = ::Console::Online::PartyGetMembers(tPartyMembers, 0, FALSE); // Inference: +0xe4 retains the returned count as the fallback player number.
        nPlayerNum = nNumPartyMembers; // Inference: the next line scans members in ascending index order.
        for (int32 i = 0; i < nNumPartyMembers; i++)
        {
            if (tPartyMembers[i].bIsLocal && tPartyMembers[i].nControllerIndex == nControllerIndex)
            {
                nPlayerNum = tPartyMembers[i].idPlayer; // Inference: the matched-member branch joins the shared cleanup after this assignment.
                break;
            }
        }
    }

    CRefString sPlayerNum = LocalizedSprintf(
        StringListFetch("Console:PlayerLabelShort").str(),
        UnicodeText::FormatNumber(nPlayerNum + 1, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);

    if (nPlayerNum >= 0)
    {
        Set_Text_Color(sPlayerNum, GetPlayerColor(nPlayerNum));
    } // Inference: the existing result object returns through the shared epilogue.
    return sPlayerNum;
}


CRefString DisplayPartyOnlineStateIcon(::Console::Online::PartyOnlineState eState, BOOL bEnabled, float flScale)
{
    CRefString sIcon;
    switch (eState)
    {
    case 0:
        if (bEnabled)
            sIcon.Format("{icon:network_offline,%f}", flScale);
        else
            sIcon.Format("{icon:network_offlineSelected,%f}", flScale);
        break;
    case 1:
    case 2:
        if (bEnabled)
            sIcon.Format("{icon:network_local,%f}", flScale);
        else
            sIcon.Format("{icon:network_localSelected,%f}", flScale);
        break;
    case 3:
        if (bEnabled)
            sIcon.Format("{icon:network_inviteOnly,%f}", flScale);
        else
            sIcon.Format("{icon:network_inviteOnlySelected,%f}", flScale);
        break;
    case 4:
        if (bEnabled)
            sIcon.Format("{icon:network_friends,%f}", flScale);
        else
            sIcon.Format("{icon:network_friendsSelected,%f}", flScale);
        break;
    case 5:
        if (bEnabled)
            sIcon.Format("{icon:network_public,%f}", flScale);
        else
            sIcon.Format("{icon:network_publicSelected,%f}", flScale);
        break;
    }
    return sIcon;
}









































CRefString DisplayPartyOnlineState(::Console::Online::PartyOnlineState eState, BOOL bEnabled, float flScale)
{
    CRefString sIcon = DisplayPartyOnlineStateIcon(eState, bEnabled, flScale);
    CRefString sLabel;
    switch (eState)
    {
    case 0:
        sLabel = StringListFetch("Console:PartyOffline");
        break;
    case 1:
        sLabel = StringListFetch("Console:PartyLocalCommunicationNX64");
        break;
    case 2:
        sLabel = StringListFetch("Console:PS3PartyLAN");
        break;
    case 3:
        sLabel = StringListFetch("Console:PartyOnlineInviteOnly");
        break;
    case 4:
        sLabel = StringListFetch("Console:PartyOnlineFriends");
        break;
    case 5:
        sLabel = StringListFetch("Console:PartyOnlinePublic");
        break;
    }
    CRefString sResult;
    return CRefString(sResult.Format("%s %s", sIcon.str(), sLabel.str()));
}
















RGBAColor GetPlayerColor(const int32 nPlayerIndex)
{
    RGBAColor arColors[4] =
    {
        RGBAColor(0xff, 0x7d, 0x00),
        RGBAColor(0x00, 0xa0, 0xff),
        RGBAColor(0x00, 0xff, 0x00),
        RGBAColor(0xe6, 0x00, 0xff),
    };
    int32 nIndex = Math::PinWrap(nPlayerIndex, 0, 4);
    return arColors[nIndex];
}



RGBAColor GetXBLPartyColor()
{
    RGBAColor rgbaColor;
    UIColor eColor = UI_GetXMLUint32Constant("ConsolePartyDisplay:XBLPartyMember", crgbaBlue.ToUIColor());
    rgbaColor.Set(eColor);
    return rgbaColor;
}








}  // namespace Console
}  // namespace UIUtils

// X/Common/Networking/GameMessage.h (kept local: other files define their own subsets)
#include "UIConsoleNewItemIndicators.h"  // NewItemIndicatorManager::MarkAsNew for UpdateRecipes
#include "../../../Common/UserInterface/UIToolTips.h"  // TTVC_* and UIToolTips_ProcessTip for GetStatsFromAffix
#include <float.h>  // FLT_EPSILON for GetStatsFromAffix
#include "../Interface/UITutorials.h"  // UITutorialManager for UpdateRecipes
extern UITutorialManager gtTutorialMgr;  // X/Client/UserInterface/Interface/UITutorials.cpp
namespace Tutorial { BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial); }  // X/Common/Tutorial.cpp (signature from the ProximityDetectorBrain.cpp declaration)
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
GameError ItemCanDrop(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
// X/Client/CACDInventory.cpp
void CACDInventoryRequestDrop(ActorCommonData * ptACD, ACDID idACDOwner);
void CACDInventoryRequestStackPortionDrop(const ActorCommonData * ptACD, const uint64 uAmount, const ACDID idACDOwner);  // signature from the definition in X/Client/CActorInventory.cpp:224
// X/Common/AccountDefinition.cpp
uint64 AccountDefinitionSavedAttributeGetUInt64(const D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKeyLo, const FastAttribKey & tKeyHi);
// X/Client/UserInterface/Console/UIConsoleStackSplit.cpp
namespace UIStackSplit {
namespace Console {
enum StackSplitType {
    STACKSPLIT_GENERIC = 0,
};
typedef void (*UIStackSplitCallback)(const ACDID, const uint64);
void Open(const ACDID idACDStack, const uint64 uInitialVal, const uint64 uStackMax, UIStackSplitCallback pfnCallback, StackSplitType eType);
}  // namespace Console
}  // namespace UIStackSplit
namespace UIInventory {  // DWARF X/Client/UserInterface/Interface/UIInventoryCommon.h:22 (members 24-28); no adopted header
struct HeroModifierArray { float flBase; float flTotal; float flWeapons; float flArmor; float flJewelry; };
void GetHeroModifiers(ActorCommonData & tACD, FastAttribKey tKey, FastAttribKey tKeyTotal, HeroModifierArray & tDestArray, const BOOL fIncludeAllStatsBonus);  // UIInventoryCommon.cpp:128
}  // namespace UIInventory
CRefString FormatTruncatedNumber(const float flValue, const BOOL bUseMillions, const BOOL bIncludeSpecialChar);  // X/Client/UserInterface/UIUtils.cpp:845

namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }  // UIGame.cpp


namespace UIUtils {
namespace Console {
// Canonical declarations from GameBalanceDefinition.cpp:3313 and Player.cpp:5467.
} }
SNO GameBalanceGetHeroSNO(const GBHandle & hGB, const uint32 dwPlayerFlags);
GBID PlayerGetPortraitGBIDFromParagonLevel(int32 nLevel);
// UIPortraits.cpp:3478,3522,3544; no adopted header exists.
namespace UIPortraits {
ImageTextureFrame GetPortraitBackgroundImage(GBID gbidPortrait);
ImageTextureFrame GetPortraitBannerImage(GBID gbidPortrait);
ImageTextureFrame GetPortraitFrameImage(GBID gbidPortrait);
}
typedef int32 ItemDrawType; // CActorInventory.cpp:153 signature uses ItemDrawType (int32).
#include "../../ImageCollection.h"
#include "../../../Common/Definitions/GlobalDefinition.h"
#include "../Controls/UIRActorControl.h"
#include "../../RActors/RActors.h"
#include "../../Movement/CActorMovement.h"
// UI.cpp:5757; original signature from owner CU.
UIRActor & UI_GetRActorFromID(const UIID & id);
namespace UIUtils { namespace Console {
// Canonical inventory/UI declarations: CActorInventory.cpp:153, Items.cpp:152, UI.cpp:5384.
} }
#include "../../../Common/ActorInventory.h"
#include "../../../Common/Items.h"
#include "../Controls/UIItemButtonControl.h"
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType);
extern const InventorySlotType cgtCraftingPlayerInventorySlots[3];
UIItemButton & UI_GetItemButtonFromID(const UIID & id);
namespace UIUtils { namespace Console {
































void YieldFocusHandler(const UIID & id, const int32)
{
    YieldFocus(id);
}





void YieldFocus(const UIID & idYielding)
{
    UI_SetControllerFocus(GetUIIDNone());
}



void EatButton(const UIID &, const int32)
{
}






void UpdateHeroNameAndLevel(const UIID & idParentPanel, const Player & tPlayer)
{
    UIID idName;
    idName.SetFromParent(idParentPanel, "PlayerName");
    UI_GetTextFromID(idName).SetText(PlayerGetHeroDisplayName(tPlayer, FALSE).str(), FALSE);

    UIID idLevel;
    idLevel.SetFromParent(idParentPanel, "PlayerLevel");
    CRefString sLevel = CPlayerGetLevelString(tPlayer.nCurrentLevel, tPlayer.nCurrentAltLevel);
    UI_GetTextFromID(idLevel).SetText(LocalizedSprintf(StringListFetch("ConsoleInventory:Level").str(), sLevel.str(), NULL).str(), FALSE);
}












void UpdateHeroStats(
    const UIID & idParentPanel,
    const Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer == NULL)  // Inference: null check has no row of its own (folded into the inlined ACDTryToGet ccmp); early return shape assumed.
    {
        return;
    }


    struct HeroStatDisplay
    {
        Attrib eAttribute;
        LPCSTR szID;
    };

    HeroStatDisplay tStatList[] = {
        { ATTRIBUTE_STRENGTH_TOTAL, "StrengthValue" },
        { ATTRIBUTE_DEXTERITY_TOTAL, "DexterityValue" },
        { ATTRIBUTE_INTELLIGENCE_TOTAL, "IntelligenceValue" },
        { ATTRIBUTE_VITALITY_TOTAL, "VitalityValue" },
    };
    const int32 nStatSize = sizeof(tStatList) / sizeof(tStatList[0]);  // DWARF const 4; sizeof spelling inferred.


    for (int32 i = 0; i < nStatSize; i++)
    {
        UIID idStatText = GetUIIDNone();
        idStatText.SetFromParent(idParentPanel, tStatList[i].szID);


        UIInventory::HeroModifierArray tStatArray;
        UIInventory::GetHeroModifiers(*ptACDPlayer, tStatList[i].eAttribute, tStatList[i].eAttribute, tStatArray, TRUE);
        CRefString usStatValue = FormatTruncatedNumber(tStatArray.flTotal, TRUE, TRUE);


        if ((tStatArray.flTotal - tStatArray.flBase) > 0.000001f)  // Inference: inner parentheses from columns 22/42/63; 1e-6f as in UIConsoleCharacterDetails GetBaseStatString.
        {
            Set_Text_Color(usStatValue, FONTCOLOR_BONUS);
        }
        else if ((tStatArray.flBase - tStatArray.flTotal) > 0.000001f)
        {
            Set_Text_Color(usStatValue, FONTCOLOR_ORANGE);
        }
        else if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL) >= 2)
        {
            Set_Text_Color(usStatValue, FONTCOLOR_SIDEKICKED);
        }

        UI_GetTextFromID(idStatText).SetText(usStatValue, FALSE);
    }


    float flEHP = ptACDPlayer->GetEHPValue();
    CRefString sSurvivability = FormatTruncatedNumber(flEHP, TRUE, TRUE);
    if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL) > 1)  // Bytes: cmp #1/b.le here versus cmp #2/b.lt at 591/612/621, hence > 1 here and >= 2 there.
    {
        Set_Text_Color(sSurvivability, FONTCOLOR_SIDEKICKED);
    }
    UI_GetTextFromID(UIIDCreateFromParent(idParentPanel, ".ArmorValue")).SetText(sSurvivability, FALSE);


    BOOL bIsDualWielding = ACDIsDualWielding(*ptACDPlayer, TRUE);
    float flDPS = ptACDPlayer->GetDPSValue(bIsDualWielding);
    CRefString sDPS = FormatTruncatedNumber(flDPS, TRUE, TRUE);
    if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL) >= 2)
    {
        Set_Text_Color(sDPS, FONTCOLOR_SIDEKICKED);
    }
    UI_GetTextFromID(UIIDCreateFromParent(idParentPanel, ".DPSValue")).SetText(sDPS, FALSE);


    float flHealing = ptACDPlayer->GetHealingValue(bIsDualWielding, NULL);
    CRefString sHealing = FormatTruncatedNumber(flHealing, TRUE, TRUE);
    if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL) >= 2)
    {
        Set_Text_Color(sHealing, FONTCOLOR_SIDEKICKED);
    }
    UI_GetTextFromID(UIIDCreateFromParent(idParentPanel, ".LifeValue")).SetText(sHealing, FALSE);
}



void UpdatePlayerIndicator(const Player & tPlayer)
{
    UIID idPlayerNumber = UITXT_CONSOLE_PLAYER_LABEL_RING;
    CRefString sNumber;
    if (PlayerGetLocalPlayerCount() > 1 && PlayerIsLocal(&tPlayer))
        sNumber = CPlayerGetIndicatorString(tPlayer);
    UI_GetTextFromID(idPlayerNumber).SetText(sNumber.str(), FALSE);
}







SNO GetObserverForPlayerClass(PlayerClass ePlayerClass)
{
    GlobalSNO eObserver;
    switch (ePlayerClass)
    {
    case PLAYER_CLASS_DEMONHUNTER:
        eObserver = GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_DEMONHUNTER;
        break;
    case PLAYER_CLASS_BARBARIAN:
        eObserver = GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_BARBARIAN;
        break;
    case PLAYER_CLASS_WIZARD:
        eObserver = GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_WIZARD;
        break;
    case PLAYER_CLASS_WITCHDOCTOR:
        eObserver = GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_WITCHDOCTOR;
        break;
    case PLAYER_CLASS_MONK:
        eObserver = GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_MONK;
        break;
    case PLAYER_CLASS_CRUSADER:
        eObserver = GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_CRUSADER;
        break;
    case PLAYER_CLASS_NECROMANCER:
        eObserver = GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_NECROMANCER;
        break;
    default:
        eObserver = GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_DEFAULT;
        break;
    }
    return GlobalSNOGet(eObserver);
}

void OpenArtisanScreen(ActorCommonData & tACDArtisan, const int32 nControllerIndex)
{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (!ptPlayer)
        return;

    CrafterType eCrafterType = NPCGetCrafterType(tACDArtisan);
    int32 nTab = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_VENDOR_DEFAULT_TAB, tACDArtisan.snoActor);
    switch (eCrafterType)
    {
    case CRAFTER_TYPE_BLACKSMITH:
    {
        ::Console::Screens::ScreenID idScreen;
        if (nTab == 2)
            idScreen = 2;
        else if (nTab == 1)
            idScreen = 1;
        else
            idScreen = 0;
        ::Console::Screens::TryOpenToScreen(::Console::Screens::BLACKSMITH_SCREEN_SET, idScreen, nControllerIndex, FALSE);
        break;
    }
    case CRAFTER_TYPE_JEWEL:
    {
        ::Console::Screens::ScreenID idScreen;
        if (nTab == 2)
            idScreen = 2;
        else
            idScreen = 0;
        ::Console::Screens::TryOpenToScreen(::Console::Screens::JEWELER_SCREEN_SET, idScreen, nControllerIndex, FALSE);
        break;
    }
    case CRAFTER_TYPE_MYSTIC:
    {
        ::Console::Screens::ScreenID idScreen;
        if (nTab == 1)
            idScreen = 1;
        else
            idScreen = 0;
        ::Console::Screens::TryOpenToScreen(::Console::Screens::MYSTIC_SCREEN_SET, idScreen, nControllerIndex, FALSE);
        break;
    }
    case CRAFTER_TYPE_HORADRIM:
        ::Console::Screens::TryOpenToScreen(::Console::Screens::KANAIS_CUBE_SCREEN_SET, 0, nControllerIndex, FALSE);
        break;
    case CRAFTER_TYPE_JEWEL_UPGRADE:
        ::Console::Screens::TryOpenToScreen(::Console::Screens::JEWEL_UPGRADE_SCREEN_SET, 0, nControllerIndex, FALSE);
        break;
    }
}









































void UpdateTrainingUI(const CrafterType eCrafterType, const Player & tPlayer)
{
    GBID gbidRecipe = ItemCrafting_GetTrainingRecipe(eCrafterType, &tPlayer);
    UpdateRecipeRequirementsUI(UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL, gbidRecipe, tPlayer);
}






void UpdateRecipeRequirementsUI(const UIID & idParent, const GBID gbidRecipe, const Player & tPlayer)
{
    if (gbidRecipe != GBID_NONE)
    {
        ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
        int32 nCost = ItemCrafting_GetRecipeCost(gbidRecipe);
        UpdateRecipeRequirementsUI(idParent, tPlayer, nCost, pdefRecipeEntry->tIngredient, pdefRecipeEntry->nNumIngredients);
    }
    else
    {
        sUpdateEmptyRecipeRequirementsUI(idParent);
    }
}







void UpdateRecipeRequirementsUI(const UIID & idParent, const Player & tPlayer, uint64 nCost, const RecipeIngredient * arRecipeIngredients, uint32 nNumIngredients)
{
    sUpdateNormalRecipeRequirementsUI(idParent, tPlayer.idPlayerACD, nCost, arRecipeIngredients, nNumIngredients);
}









void sUpdateEmptyRecipeRequirementsUI(const UIID & idParent)
{
    UIID idRequirementsContainer;
    idRequirementsContainer.SetFromParent(idParent, "RequiredMaterials");
    const UIControlItemArray & arChildren = UI_GetStackPanelFromID(idRequirementsContainer).GetChildList();
    for (uint32 i = 0; i < arChildren.size(); i++)
    {
        UI_GetWindowFromID(arChildren[i].id).SetActive(FALSE);
    }

    UIID idCraftingCost;
    idCraftingCost.SetFromParent(idParent, "CraftingCost");
    UI_GetTextFromID(idCraftingCost).SetText("", FALSE);

    UIID idTitle = UIIDCreateFromParent(UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL, ".RequiredMaterialsTitle");
    UI_GetWindowFromID(idTitle).SetActive(FALSE);
    UIID idSeparator = UIIDCreateFromParent(UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL, ".CraftingSeparator");
    UI_GetWindowFromID(idSeparator).SetActive(FALSE);
    UIID idCostBackground = UIIDCreateFromParent(UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL, ".CraftingCostBackground");
    UI_GetWindowFromID(idCostBackground).SetActive(FALSE);
}








void sUpdateNormalRecipeRequirementsUI(
    const UIID & idParent,
    const ACDID idACDPlayer,
    uint64 nCost,
    const RecipeIngredient * arRecipeIngredients,
    uint32 nNumIngredients)
{

    UIID idRequirementsContainer;
    idRequirementsContainer.SetFromParent(idParent, "RequiredMaterials");

    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);

    const UIControlItemArray & arChildren = UI_GetStackPanelFromID(idRequirementsContainer).GetChildList();
    for (uint32 i = 0; i < arChildren.size(); i++)
    {

        UI_GetWindowFromID(arChildren[i].id).SetActive(i < nNumIngredients);
        if (i < nNumIngredients)
        {
            const RecipeIngredient & tIngredient = arRecipeIngredients[i];

            const UIID & idIngredient = UIIDCreateFromParent(idRequirementsContainer, CRefString().Format(".Material%02d", i + 1));


            UIID idIcon;
            idIcon.SetFromParent(idIngredient, "MaterialIcon");

            UIItemButton & tBtnIcon = UI_GetItemButtonFromID(idIcon);
            ImageTextureFrame tImageTexFrame; // Inference: draw-type0 below follows the original argument; no authenticated enumerator name.
            ACDInventoryGetItemDrawInfoWithGBID(tIngredient.gbidItem, tImageTexFrame, 0);
            tBtnIcon.SetGBID(tIngredient.gbidItem);
            // Inference: the null/invalid-definition fallback selects normal quality (constant1).
            ItemQualityLevel eIQL = IQL_NORMAL;
            ScopedItemDefinitionLockR pdefItem(tIngredient.gbidItem);
            if (pdefItem != NULL && pdefItem->eDisplayedQualityLevel != IQL_INVALID)
                eIQL = pdefItem->eDisplayedQualityLevel;



            tBtnIcon.SetQualityLevel(eIQL);

            tBtnIcon.SetAllTextureFrames(tImageTexFrame);


            UIID idCount;
            idCount.SetFromParent(idIngredient, "MaterialCount");

            int32 nRequired = tIngredient.nQuantity;
            GBHandle hgbItem(GB_ITEMS, tIngredient.gbidItem);
            int32 nOnHand = ACDInventoryGetItemCount(tACDPlayer, hgbItem, cgtCraftingPlayerInventorySlots, TRUE, FALSE);

            CRefString sText = LocalizedSprintf(
                StringListFetch("ConsoleCrafting:RequiredAmount").str(),
                UnicodeText::FormatNumber(nOnHand, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                UnicodeText::FormatNumber(nRequired, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                NULL);

            UIText & tText = UI_GetTextFromID(idCount);
            tText.SetText(sText.str(), FALSE);
            tText.SetTextColor(nOnHand >= nRequired ? crgbaWhite : crgbaRed);
        }
    }

    UIID idTitle = UIIDCreateFromParent(UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL, ".RequiredMaterialsTitle");
    UI_GetWindowFromID(idTitle).SetActive(TRUE);

    UIID idSeparator = UIIDCreateFromParent(UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL, ".CraftingSeparator");
    UI_GetWindowFromID(idSeparator).SetActive(TRUE);

    UIID idCostBackground = UIIDCreateFromParent(UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL, ".CraftingCostBackground");
    UI_GetWindowFromID(idCostBackground).SetActive(TRUE);


    UIID idCraftingCost;
    idCraftingCost.SetFromParent(idParent, "CraftingCost");

    CurrencyType eCurrencyType = CURRENCY_GOLD;
    CRefString sCostText = UIText_ValueToGameCurrency(nCost, eCurrencyType); // Inference: the next call narrows nCost to int32 before sign-extending (original +0x3b8).
    if (!tACDPlayer.CanAffordCurrencyCost((int32)nCost, eCurrencyType))
    {
        Set_Text_Color(sCostText, crgbaRed);
    }

    UI_GetTextFromID(idCraftingCost).SetText(sCostText.str(), FALSE);
}



void ScrollContentInScrollBox(const UIID & idScrollBox, float fAmount)
{
    UIConsoleScrollBox & scrollbox = UI_GetConsoleScrollBoxFromID(idScrollBox);
    scrollbox.Scroll(fAmount);
}





void ResetScrollBox(const UIID & idScrollBox)
{
    UIConsoleScrollBox & scrollbox = UI_GetConsoleScrollBoxFromID(idScrollBox);
    scrollbox.SetScrollPos(0.0f);
}




BOOL CanScrollContentInScrollBox(const UIID & idScrollBox)
{
    UIConsoleScrollBox & scrollbox = UI_GetConsoleScrollBoxFromID(idScrollBox);
    return scrollbox.IsActive() && scrollbox.GetScrollLimit() > 1e-06f;
}



CRefString GetTagName(OnlineService::GameTagId idTag)
{
    OnlineService::XGameTagArray arGameTags;
    OnlineService::GetDefaultGameTags(arGameTags);
    CRefString sTagName;
    for (uint32 i = 0; i < arGameTags.size(); i++)
    {
        if (arGameTags[i].m_tagId == idTag)
        {
            CRefString sStringKey(CRefString().Format("Bnet_QuestSelection:GameTag_%s", arGameTags[i].m_tag.str()));
            sTagName = StringListFetch(sStringKey);
            break;
        }
    }
    return sTagName;
}



BOOL GameTagIsValid(
    const OnlineService::GameTagId & tTag,
    BOOL bAdventureMode)
{
    if (tTag == OnlineService::GAME_TAG_ID_NONE)
        return FALSE;


    if (bAdventureMode)
    {
        return (tTag == OnlineService::GAME_TAG_ID_RIFT) ||
            (tTag == OnlineService::GAME_TAG_ID_GREATER_RIFT) ||
            (tTag == OnlineService::GAME_TAG_ID_BOUNTIES) ||
            (tTag == OnlineService::GAME_TAG_ID_KEYWARDEN);
    }
    else
    {
        return (tTag == OnlineService::GAME_TAG_ID_DEFAULT) ||
            (tTag == OnlineService::GAME_TAG_ID_BRAWLING);
    }
}


int32 GetParagonLevel(const D3::Account::SavedDefinition & tAccount, BOOL bIsHardcore, BOOL bIsSeasonal, BOOL bUnk)
{
    int32 nParagonLevel = 0;
    D3AccountPartition nPartitionId = GetPartition(bIsHardcore, bIsSeasonal, bUnk);
    for (int32 i = 0; i < tAccount.partitions_size(); i++)
    {
        if (tAccount.partitions(i).partition_id() == nPartitionId)
        {
            nParagonLevel = tAccount.partitions(i).alt_level();
            break;
        }
    }
    return nParagonLevel;
}







uint64 GetParagonExperience(const D3::Account::SavedDefinition & tAccount, BOOL bIsHardcore, BOOL bIsSeasonal, BOOL bUnk)
{
    uint64 nParagonXP = 0;
    D3AccountPartition nPartitionId = GetPartition(bIsHardcore, bIsSeasonal, bUnk);
    for (int32 i = 0; i < tAccount.partitions_size(); i++)
    {
        if (tAccount.partitions(i).partition_id() == nPartitionId)
        {
            const D3::Account::AccountPartition & tPartition = tAccount.partitions(i);
            nParagonXP = AccountDefinitionSavedAttributeGetUInt64(tPartition.saved_attributes(), FastAttribKey(ATTRIBUTE_ALT_EXPERIENCE_NEXT_LO), FastAttribKey(ATTRIBUTE_ALT_EXPERIENCE_NEXT_HI));
            break;
        }
    }
    return nParagonXP;
}







void DisplayPortrait(
    PlayerClass eClass,
    BOOL bFemale,
    int32 nAltLevel,
    const UIID & idPortrait,
    GBID gbidPortrait)
{
    UIID idClassPortrait = UIIDCreateFromParent(idPortrait, ".icon");
    UIID idPortraitAltLevel = UIIDCreateFromParent(idPortrait, ".alt_level");
    UIID idPortraitBanner = UIIDCreateFromParent(idPortrait, ".Banner");
    UIID idPortraitBackground = UIIDCreateFromParent(idPortrait, ".Background");
    UIID idPortraitFrame = UIIDCreateFromParent(idPortrait, ".Frame");


    uint32 dwPlayerFlags = 0;
    if (bFemale) dwPlayerFlags |= 1 << PLAYERFLAG_FEMALE;
    SNO snoHero = GameBalanceGetHeroSNO(GBHandle(GB_HEROS, PlayerClassToGBID(eClass)), dwPlayerFlags);
    ImageTextureFrame tFrame;
    ImageHandle hImage = 0;
    if (snoHero != SNO_NONE)
    {
        if (ActorDefinitionTagMapValueExists(TAG_ACTOR_PORTRAIT_IMAGE, snoHero))
        {
            hImage = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_PORTRAIT_IMAGE, snoHero);
        }
    }
    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, hImage, tFrame);
    UI_GetWindowFromID(idClassPortrait).SetTextureFrame(tFrame);

    if (gbidPortrait == GBID_NONE)
    {
        gbidPortrait = PlayerGetPortraitGBIDFromParagonLevel(nAltLevel);
    }

    UI_GetWindowFromID(idPortraitFrame).SetTextureFrame(UIPortraits::GetPortraitFrameImage(gbidPortrait));
    UI_GetWindowFromID(idPortraitBackground).SetTextureFrame(UIPortraits::GetPortraitBackgroundImage(gbidPortrait));

    UIWindow & tWndBanner = UI_GetWindowFromID(idPortraitBanner);
    UIText & tTxtAltLevel = UI_GetTextFromID(idPortraitAltLevel);
    if (nAltLevel > 0)
    {
        tWndBanner.SetTextureFrame(UIPortraits::GetPortraitBannerImage(gbidPortrait));
        tWndBanner.SetActive(TRUE);

        CRefString sLevel = UnicodeText::FormatNumber(nAltLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str();
        tTxtAltLevel.SetText(sLevel, FALSE);
        tTxtAltLevel.SetActive(TRUE);
    }
    else
    {
        tWndBanner.SetActive(FALSE);
        tTxtAltLevel.SetActive(FALSE);
    }
}

// DWARF low_pc=0 (DIE 02161ca4); required by rotation caller; standalone body is not checkable.
void sRotateRactor(
    RActorID idRActor,
    Angle aYawDelta)
{
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor)
    {
        if (ptRActor->ptCActorMovement)
        {
            ptRActor->ptCActorMovement->SetNewFacing(ptRActor->aYaw + aYawDelta, TRUE, TRUE);
        }
        else
        { // Inference: the original constant is 0x3e860a92 (15 degrees); its source expression is unavailable.
            ptRActor->aYaw += 0.2617993950843811f;
        }
    }
}

// DWARF low_pc=0 (DIE 02161df1); required by rotation caller; standalone body is not checkable.
void sRotateUIRactor(
    const UIID & id,
    Angle aYawDelta)
{
    UIRActor & tHero = UI_GetRActorFromID(id);
    sRotateRactor(tHero.GetRActor(), aYawDelta);
}


void HandleUIRactorRotation(
    const UIID & idRActor,
    Vector2D vecStick)
{ // DWARF02161fbd location is constant0x3e490fdb for aRotationSpeed below.
    Angle aRotationSpeed = 0.19634954631328583f;
    ScopedGlobalDefinitionLockR ptGlobals;
    float flT = vecStick.x; // Inference: 1e-6f below matches original0x7100e3bea0; original literal spelling is unavailable.
    if (ptGlobals != NULL && Math::Abs(flT) > 1e-6f)
    {
        flT = (Math::Abs(vecStick.x) - ptGlobals->flControllerDeadZone) / (1.0f - ptGlobals->flControllerDeadZone);
        if (vecStick.x < 0.0f)
        {
            flT = -flT;
        }
    }
    sRotateUIRactor(idRActor, flT * aRotationSpeed);
}



CRefString GetStatsFromAffix(
    GBID gbidAffix,
    GBID gbidItem,
    const PlayerClass ePlayerClass)
{
    CRefString usAffixStats;

    GBHandle hGBAffix;
    ScopedItemAffixLockR ptAffix(gbidAffix);
    if (ptAffix == NULL)
    {
        return usAffixStats;
    }


    RNG tRNG;
    RandSetSeed_Internal(tRNG, 0);

    AttribGroupID idAttribGroupMin = FastAttribGroupCreate(FAT_ITEM, -1);

    AttribGroupID idAttribGroupMax = FastAttribGroupCreate(FAT_ITEM, -1);

    AttribGroupID idAttribGroupItemPassivesMin = FastAttribGroupCreate(FAT_ITEM, -1);

    AttribGroupID idAttribGroupItemPassivesMax = FastAttribGroupCreate(FAT_ITEM, -1);


    for (uint32 i = 0; i < 4; i++)  // 4 = ARRAYSIZE(AffixTableEntry::ptAffixAttributes); literal as in Items.cpp sibling loops.
    {
        const AttributeSpecifier & tAttribute = ptAffix->ptAffixAttributes[i];

        if (tAttribute.eAttribute == ATTRIBUTE_NONE)
        {
            continue;
        }

        int32 nAttributeParam = tAttribute.nParam;
        FastAttribKey tKey = MakeAttribKey(tAttribute.eAttribute, nAttributeParam);
        AttributeDataType eAttribType = KeyGetDataType(tKey);
        if (eAttribType == ATTRIBUTEDT_UNKNOWN)
        {
            continue;
        }

        float flMinAffixValue = AffixGetFormulaMinValue(tAttribute, NULL, gbidItem, &tRNG, NULL);





        float flMaxAffixValue = AffixGetFormulaMaxValue(tAttribute, NULL, gbidItem, &tRNG, NULL);





        if (flMinAffixValue < FLT_EPSILON && flMaxAffixValue < FLT_EPSILON)
        {
            continue;
        }


        if (KeyGetDataType(tKey) == ATTRIBUTEDT_FLOAT)
        {
            if (tAttribute.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
            {
                FastAttribSetValueFloat(idAttribGroupItemPassivesMin, tKey, flMinAffixValue);
                FastAttribSetValueFloat(idAttribGroupItemPassivesMax, tKey, flMaxAffixValue);
            }
            else
            {
                FastAttribSetValueFloat(idAttribGroupMin, tKey, flMinAffixValue);
                FastAttribSetValueFloat(idAttribGroupMax, tKey, flMaxAffixValue);
            }
        }
        else
        {
            if (tAttribute.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
            {
                FastAttribSetValueInt(idAttribGroupItemPassivesMin, tKey, (int32)flMinAffixValue);
                FastAttribSetValueInt(idAttribGroupItemPassivesMax, tKey, (int32)flMaxAffixValue);
            }
            else
            {
                FastAttribSetValueInt(idAttribGroupMin, tKey, (int32)flMinAffixValue);
                FastAttribSetValueInt(idAttribGroupMax, tKey, (int32)flMaxAffixValue);
            }
        }
    }




    usAffixStats = ItemGetAttributeTip("{icon:bullet} ", idAttribGroupMin, idAttribGroupMax, TTVC_NORMAL_COLORING);

    usAffixStats += ItemGetPowerBonusAttributeDescriptions("{icon:bullet} ", NULL, idAttribGroupMin, idAttribGroupMax, ePlayerClass, TTVC_NORMAL_COLORING, NULL, NULL);

    FastAttribGroupDelete(idAttribGroupMax);
    FastAttribGroupDelete(idAttribGroupMin);

    SNO snoItemPassivePowerStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_PASSIVE_POWER_DESCRIPTIONS);
    for (uint32 i = 0; i < 4; i++)
    {

        const AttributeSpecifier & tAttribute = ptAffix->ptAffixAttributes[i];

        if (tAttribute.eAttribute != ATTRIBUTE_ITEM_POWER_PASSIVE)
        {
            continue;
        }

        CRefString sStringListLabel = AttribToString(tAttribute.eAttribute);

        usAffixStats += "{icon:bullet} ";

        StringLabelHandle hLabel = StringListGetLabelHandle(SNOToString(SNOName(POWER_SNO, tAttribute.nParam), FALSE).str());

        CRefString usAttributeFormatSpecifier;
        if (!StringListFetchByLabel(snoItemPassivePowerStringList, hLabel, usAttributeFormatSpecifier))
        {
            CRefString usErrorTip;
            usErrorTip.Format("Missing item passive power label (%s):(%d),(%s)\n", SNOToString(STRINGLIST_SNO, snoItemPassivePowerStringList, FALSE).str(), hLabel, sStringListLabel.str());
            usAffixStats += usErrorTip;
            continue;  // Inference: usErrorTip dtor at 1271:17 and the 1273 statement at the same indent as 1265 imply continue rather than else.
        }

        usAffixStats += UIToolTips_ProcessTip(NULL, idAttribGroupItemPassivesMin, idAttribGroupItemPassivesMax, tAttribute.nParam, 1, usAttributeFormatSpecifier, TTVC_NO_COLORING, ItemResolveValueTagForItemPassivePowerDescription, (void *)(intptr_t)tAttribute.eAttribute, NULL);

        usAffixStats += "\n";
    }

    FastAttribGroupDelete(idAttribGroupItemPassivesMin);
    FastAttribGroupDelete(idAttribGroupItemPassivesMax);

    return usAffixStats;

}




void UpdateRecipes(
    const Player * ptPlayer,
    const CrafterType eType,
    const BOOL fInitialInfo,
    const D3::ItemCrafting::CrafterData & tNewCrafterData,
    const CrafterData & tOldCrafterData)
{
    if (!ptPlayer || !ptPlayer->ptNewRecipeManager || fInitialInfo)  // Inference: columns 27 (||) and 41 (ptNewRecipeManager) fit this order; early-return form inferred.
    {
        return;
    }



    if (tNewCrafterData.level() != tOldCrafterData.nLevel)
    {
        GBHandleList listGBHandles(GBGetHandlePool());
        GBEnumerate(GB_RECIPES, listGBHandles);

        POS pos = listGBHandles.GetHeadPosition();
        while (pos)
        {
            GBHandle hGB = listGBHandles.GetNext(pos);
            ScopedRecipeEntryLockR pdefRecipeEntry(hGB.gbid);
            if (pdefRecipeEntry &&  // Columns 45/68/80 are the && operators of lines 1312-1314; the dwFlags test (operand 4) is attributed to 1312:29.
                pdefRecipeEntry->eCrafterType == eType &&
                pdefRecipeEntry->nLevel == tNewCrafterData.level() &&
                !(pdefRecipeEntry->dwFlags & (1 << RECIPE_DROPPED_BIT)))
            {
                ptPlayer->ptNewRecipeManager->MarkAsNew(hGB.gbid);
            }
        }
    }

    if (tNewCrafterData.recipes_size() > tOldCrafterData.nNumRecipes)
    {
        SNO snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_X1_SPECIALCRAFTINGMATS);

        for (int32 i = tOldCrafterData.nNumRecipes; i < tNewCrafterData.recipes_size(); i++)
        {
            GBID gbidRecipe = tNewCrafterData.recipes(i);
            ptPlayer->ptNewRecipeManager->MarkAsNew(gbidRecipe);


            if (!Tutorial::PlayerHasSeen(*ptPlayer, snoTutorial))
            {
                ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);

                if (pdefRecipeEntry->nLevel != CrafterGetMaxLevel(pdefRecipeEntry->eCrafterType))
                {
                    continue;
                }


                for (int32 nIngredientIndex = 0; nIngredientIndex < pdefRecipeEntry->nNumIngredients; ++nIngredientIndex)  // Prefix increment: column 115 is the operator position.
                {
                    GBID gbidIngredient = pdefRecipeEntry->tIngredient[nIngredientIndex].gbidItem;
                    ScopedItemDefinitionLockR pdefIngredient(gbidIngredient);


                    if (pdefIngredient->eDisplayedQualityLevel >= IQL_LEGENDARY &&
                        gbidIngredient != GlobalGBIDGet(GLOBALGBID_FORGOTTEN_SOUL))
                    {
                        gtTutorialMgr.ShowTutorial(GetUIIDNone(), snoTutorial, ptPlayer, TRUE);
                        break;
                    }
                }
            }
        }
    }

}




void SetJunkFlag(ActorCommonData & ptACD, BOOL bJunk, Player * ptPlayer)
{
    if (ptACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_ITEM_FLAGGED_AS_JUNK)) != bJunk)
    {
        ptACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_ITEM_FLAGGED_AS_JUNK), bJunk);

        ItemJunkFlagChangedMessage tMessage;
        tMessage.ann = ANNFromACD(ptACD.id);
        tMessage.bJunkFlag = bJunk;
        MessageSendToServer(CMSG_ITEM_JUNK_FLAG_CHANGED, &tMessage, sizeof(tMessage), ptPlayer);
    }
}









void sDropStack(
    const ACDID idACDStack,
    const uint64 uAmount)
{
    ActorCommonData * ptACDStack = ACDTryToGet(idACDStack);
    if (ptACDStack == NULL)
    {
        return;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(sgtUIUtilsGlobals.m_idACDPlayerDroppingStack);
    if (ptACDPlayer == NULL)
    {
        return;
    }

    CACDInventoryRequestStackPortionDrop(ptACDStack, uAmount, ptACDPlayer->id);  // Inference: no separate ACDID_INVALID tests (the original has only ACDTryToGet's inlined ones).
}




void DropItem(ACDID idSelectedItem, Player * ptPlayer)
{
    if (idSelectedItem != ACDID_INVALID)
    {
        ActorCommonData * ptSelectedItemACD = ACDTryToGet(idSelectedItem);
        if (ptSelectedItemACD)
        {
            if (ItemCanDrop(*ptSelectedItemACD) == GAMEERROR_NONE)
            {
                uint64 uStackSize = ptSelectedItemACD->GetStackCount();
                if (uStackSize > 1)
                {
                    sgtUIUtilsGlobals.m_idACDPlayerDroppingStack = ptPlayer->idPlayerACD;
                    UIStackSplit::Console::Open(ptSelectedItemACD->id, uStackSize, uStackSize, sDropStack, UIStackSplit::Console::STACKSPLIT_GENERIC);
                }
                else
                {
                    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
                    CACDInventoryRequestDrop(ptSelectedItemACD, ptPlayer->idPlayerACD);
                }
            }
            else
            {
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_INVALID_SELECTION), cgtPSODefaults);
                UIGame::DisplayError(GameErrorData(GAMEERROR_ITEM_CANNOT_BE_DROPPED), FALSE, ptPlayer);
            }
        }
    }
}







BOOL IsJapaneseVersion()
{
    return FALSE;
}








BOOL PlayerIsInActiveTieredRift(const Player * ptPlayer)
{
    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    if (tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_GOD)) ||
        tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALLOW_SKILL_CHANGES)))
        return FALSE;

    if (NephalemRift::IsTieredRiftActive())  // DWARF: bParticipating (line 1469) sits in a lexical block under this test
    {
        BOOL bParticipating = FastAttribGetValueInt(tACDPlayer.idFastAttrib, FastAttribKey(ATTRIBUTE_PARTICIPATING_IN_TIERED_LOOT_RUN));
        if (bParticipating)
        {
            GameSyncedData & tGameSyncedData = GameGetActiveGameCommonData().tGameSyncedData;
            int32 nRiftTier = tGameSyncedData.nOpenLootRunLevel;
            if (nRiftTier >= 0 && !tGameSyncedData.bLootRunBossDead)
                return TRUE;
        }
    }

    return FALSE;
}











BOOL ShowTieredRiftMessage(const Player * ptPlayer)
{
    if (PlayerIsInActiveTieredRift(ptPlayer))
    {
        UIGame::DisplayErrorString(StringListFetch("ConsoleErrors:TieredRiftsNoInventoryOrSkills"));
        return TRUE;
    }
    return FALSE;
}

















































void OpenInspectLeaderboardForChallengeRift()
{
    int32 nProfileUserIdx = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    UILeaderboardInspect::Console::OpenForChallengeRift(nProfileUserIdx);
}





void UpdateNavButtons(const UIID & id)
{
    UIID idLeft = UIIDCreateFromParent(id, ".KeyBinding1");
    UIID idRight = UIIDCreateFromParent(id, ".KeyBinding2");
    UIText & tLeftButton = UI_GetTextFromID(idLeft);
    UIText & tRightButton = UI_GetTextFromID(idRight);
    tLeftButton.SetText("{icon:l1,1.25}", FALSE);
    tRightButton.SetText("{icon:r1,1.25}", FALSE);
}

}  // namespace Console
}  // namespace UIUtils
