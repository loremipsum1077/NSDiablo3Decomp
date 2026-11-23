#include "UILoreCommon.h"

#include "../../CGame.h"
#include "../../Gfx/GfxCommon.h"
#include "../Controls/UIBlinkerControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Player.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/Definitions.h"
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/Definitions/LoreDefinition.h"
#include "../../../Common/TagMaps/ActorTags.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/DataStructures/DataArraysTemplated.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
#include "../../../Common/Definitions/ConversationDefinition.h"
enum {
    CONV_TYPE_LORE = 8,
    CONV_TYPE_AMBIENT_GOSSIP = 9,
};
enum {
    CF_OPERATE_SPEAKER_ON_BEGIN_BIT = 11,
};
struct ConversationDefinition {  // size 0x110
    RequiredDefinitionHeader tHeader;  // 0x00
    ConversationType eConvType;  // 0x0c
    uint8 _unadopted0[0x24 - 0x10];  // 0x10 (invented name: eConvIcon .. snoConvUnlocks)
    uint32 dwFlags;  // 0x24
    CHAR szSetPlayerFlag[128];  // 0x28
    SNO snoPrimaryNpc;  // 0xa8
    uint8 _unadopted1[0x110 - 0xac];  // 0xac (invented name: snoAltNpc1 ..)
};

extern ConversationGroupDef * gptGroupDefConversation;
typedef ScopedDefinitionLockR<ConversationDefinition> ScopedConversationDefinitionLockR;
template <> inline const ConversationDefinition * DefinitionLockR<ConversationDefinition>(SNO sno) {  // line 462
    return (const ConversationDefinition *)gptGroupDefConversation->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<ConversationDefinition>(const ConversationDefinition * pdef) {  // line 463
    void * lpDef = (void *)pdef;
    gptGroupDefConversation->UnlockR(lpDef);
}
// X/Common/Definitions/Conversation.cpp
BOOL ConversationHasVO(SNO snoConversation, const Player * ptPlayer);
BOOL Conversation_GetFirstLineSound(SNO snoConversation, const Player * ptPlayer, SNO & snoSound, int32 & nPermIndex);
#include "../../Sound/Sound.h"
typedef int32 SoundID;
enum {
    SOUNDTYPE_DIALOG = 2,
};

























// X/Client/Sound/Sound.cpp
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
float GetTypeVolume(int32 eType);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
void SoundDelete(SoundID id);
void SoundSetPaused(SoundID id, BOOL fPause);
#include "../../../Common/Networking/GameMessage.h"
enum {
    CMSG_LORE_PLAYING = 184,
};




// X/Common/Definitions/StringList.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
// X/Client/ImageCollection.cpp
ImageHandle StringToImageHandle(LPCSTR szName);
BOOL ImageCollectionGetImageInfo(int32 nCollection, ImageHandle hImage, ImageTextureFrame & tFrame);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_LORENOTIFY_CONSOLE;
extern UIID UIBLINKER_LORENOTIFY_CONSOLE;
extern UIID UICONSOLELORE_PLAYER_DLG;
extern UIID UICONSOLELORE_PLAYER_TITLE;
extern UIID UICONSOLELORE_PLAYER_PORTRAIT;
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIBlinker & UI_GetBlinkerFromID(const UIID & id);
BOOL UI_TestID(const UIID & id);
const UIID & GetUIIDNone();
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
// X/Client/CPlayer.cpp
Player * LocalPlayerGet();
// X/Client/UserInterface/Console/UIConsoleEventQueue.cpp
namespace UIEventQueue {
namespace Console {
void DisplayMessageForPlayer(const CRefString & sMessage, const Player * ptPlayer, ImageHandle hImage, CRefString * psSubText, BOOL fPlaySound);
}  // namespace Console
}  // namespace UIEventQueue
// X/Client/UserInterface/Console/UIConsoleLore.cpp
namespace UILore {
namespace Console {
void Refresh();
}  // namespace Console
}  // namespace UILore
// X/Client/UserInterface/Interface/UIQuestLore.cpp
class UIQuestLore {
public:
    static void ShowLoreDisplayForSNO(SNO snoLore);
};

static const UIID & idLoreNotify = UIDLG_LORENOTIFY_CONSOLE;  // line 64
static const UIID & idLoreNotifyBlinker = UIBLINKER_LORENOTIFY_CONSOLE;  // line 65
static const UIID & idLorePlayer = UICONSOLELORE_PLAYER_DLG;  // line 66
static const UIID & idLorePlayerTitle = UICONSOLELORE_PLAYER_TITLE;  // line 67
static const UIID & idLorePlayerPortrait = UICONSOLELORE_PLAYER_PORTRAIT;  // line 68

enum {
    LORE_NOTIFICATION_FADE_START = 300,
    LORE_NOTIFICATION_FADE_TIME = 30,
    LORE_NOTIFICATION_DELAY = 0,
};

struct UILoreGlobals {  // size 0x10
    uint32 dwLoreNotifyTime;  // 0x00
    SNO snoLastLorePickedUp;  // 0x04
    SoundID idSound;  // 0x08
    BOOL fLorePlayerIsOpen;  // 0x0c
};

static UILoreGlobals * sgptUILoreGlobals;  // line 94

void UILoreCommon_InitializeForGame() {  // line 98
    sgptUILoreGlobals = (UILoreGlobals *)SigmaMemoryNew(sizeof(UILoreGlobals), 4, NULL, TRUE);
    sgptUILoreGlobals->dwLoreNotifyTime = 0;
    sgptUILoreGlobals->idSound = -1;
    sgptUILoreGlobals->fLorePlayerIsOpen = FALSE;
}

void UILoreCommon_DisposeForGame() {  // line 109
    SigmaMemoryFree(sgptUILoreGlobals, NULL);
    sgptUILoreGlobals = NULL;
}

static void sUpdateNotificationVisiblity() {  // line 117
    if (!UI_TestID(idLoreNotify)) {
        return;
    }

    UIWindow & tLoreDlg = UI_GetWindowFromID(idLoreNotify);
    uint32 dwCurrentTime = CGameGetClientTicks();

    if (sgptUILoreGlobals->dwLoreNotifyTime != 0 &&
        dwCurrentTime > sgptUILoreGlobals->dwLoreNotifyTime + LORE_NOTIFICATION_DELAY) {
        UILoreNotify::Show(sgptUILoreGlobals->snoLastLorePickedUp);
    }

    if (sgptUILoreGlobals->dwLoreNotifyTime != 0 &&
        dwCurrentTime <= sgptUILoreGlobals->dwLoreNotifyTime + LORE_NOTIFICATION_FADE_START + LORE_NOTIFICATION_FADE_TIME) {
        uint8 bAlpha = 255;
        if (dwCurrentTime > sgptUILoreGlobals->dwLoreNotifyTime + LORE_NOTIFICATION_FADE_START) {
            float flAlpha = 1.0f - (float)(dwCurrentTime - (sgptUILoreGlobals->dwLoreNotifyTime + LORE_NOTIFICATION_FADE_START)) / LORE_NOTIFICATION_FADE_TIME;
            bAlpha = (uint8)Math::FloatRoundToInt(flAlpha * 255.0f);
        }
        tLoreDlg.SetTransparency(bAlpha);
        tLoreDlg.SetActive(TRUE);
    } else {
        tLoreDlg.SetActive(FALSE);
    }
}

static void sNotifyServerOfLorePlaying(BOOL bPlaying) {  // line 153
    BoolDataMessage tMessage;
    tMessage.bVal = bPlaying;
    MessageSendToServer(CMSG_LORE_PLAYING, &tMessage, sizeof(tMessage), LocalPlayerGet());
}

void UILoreNotify::OnLoreAcquired(const SNO snoLore) {  // line 163
    if (UI_TestID(idLoreNotifyBlinker)) {
        sgptUILoreGlobals->dwLoreNotifyTime = CGameGetClientTicks();
        sgptUILoreGlobals->snoLastLorePickedUp = snoLore;
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_LORE_NEW), cgtPSODefaults);
        UI_GetBlinkerFromID(idLoreNotifyBlinker).SetDuration(BLINKER_DURATION_INFINITE);
    }
}

void UILoreNotify::Show(const SNO snoLore) {  // line 176
    if (SoundManager::GetTypeVolume(SOUNDTYPE_DIALOG) > EPSILON) {
        ScopedLoreDefinitionLockR pdefLore(snoLore);
        if (!(pdefLore->dwFlags & (1 << LOREDEF_SUPPRESS_PLAYER_BIT)) &&
            ConversationHasVO(pdefLore->snoConversation, LocalPlayerGet())) {
            UILorePlayer::OpenForLore(snoLore);
            return;
        }
    }

    Hide();

    CRefString sLabel = SNOToString(LORE_SNO, snoLore, FALSE);
    CRefString sTitle = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_UI), "NewLore"),
                                         StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_LORE), sLabel).str(), NULL);
    ImageHandle hImage = StringToImageHandle("Console_JournalRingBestiary");
    UIEventQueue::Console::DisplayMessageForPlayer(sTitle, LocalPlayerGet(), hImage, NULL, TRUE);
}

void UILoreNotify::Hide() {  // line 206
    sgptUILoreGlobals->dwLoreNotifyTime = 0;
}

void UILoreNotify::Update() {  // line 214
    sUpdateNotificationVisiblity();
}

void UILore::RefreshUI() {  // line 235
    UILore::Console::Refresh();
}

void UILore::Pause(BOOL fPause) {  // line 247
    if (sgptUILoreGlobals->idSound != -1) {
        SoundSetPaused(sgptUILoreGlobals->idSound, fPause);
    }
}

BOOL UILore::PlayLore(const SNO snoLore, const Player * ptPlayer) {  // line 263
    SNO snoSound;
    int32 nPermIndex;
    ScopedLoreDefinitionLockR pdefLore(snoLore);

    if (Conversation_GetFirstLineSound(pdefLore->snoConversation, ptPlayer, snoSound, nPermIndex)) {
        StopLore();

        PlaySoundOptions tOptions;
        tOptions.nPermutationIndex = nPermIndex;
        sgptUILoreGlobals->idSound = SoundManager::PlaySound(snoSound, tOptions);
        if (sgptUILoreGlobals->idSound != -1) {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_LORE_START), cgtPSODefaults);
            sNotifyServerOfLorePlaying(TRUE);
            return TRUE;
        }
    }

    return FALSE;
}

void UILore::StopLore() {  // line 297
    if (sgptUILoreGlobals->idSound != -1) {
        SoundDelete(sgptUILoreGlobals->idSound);
        sgptUILoreGlobals->idSound = -1;
        sNotifyServerOfLorePlaying(FALSE);
    }
}

void UILorePlayer::OpenForLore(const SNO snoLore) {  // line 313
    sgptUILoreGlobals->snoLastLorePickedUp = snoLore;

    ScopedLoreDefinitionLockR pdefLore(snoLore);
    if (!ConversationHasVO(pdefLore->snoConversation, LocalPlayerGet())) {
        return;
    }

    ScopedConversationDefinitionLockR pdefConversation(pdefLore->snoConversation);

    UI_GetWindowFromID(idLorePlayer).SetActive(TRUE);
    sgptUILoreGlobals->fLorePlayerIsOpen = TRUE;

    Player * ptLocalPlayer = LocalPlayerGet();

    CRefString sLabel = SNOToString(LORE_SNO, snoLore, FALSE);
    CRefString sTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_LORE), sLabel);
    UI_GetTextFromID(idLorePlayerTitle).SetText(sTitle, FALSE);

    UIWindow & tWndPortrait = UI_GetWindowFromID(idLorePlayerPortrait);
    SNO snoActorPortrait = pdefConversation->snoPrimaryNpc;
    if (ptLocalPlayer != NULL && snoActorPortrait == SNO_NONE) {
        snoActorPortrait = ptLocalPlayer->snoActorPortrait;
    }

    if (snoActorPortrait != SNO_NONE) {
        ImageHandle hImage = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_PORTRAIT_IMAGE, snoActorPortrait);
        ImageTextureFrame tFrame;
        ImageCollectionGetImageInfo(2, hImage, tFrame);
        tWndPortrait.SetTextureFrame(tFrame);
    } else {
        tWndPortrait.SetTextureFrame(cgtImageTextureFrameNull);
    }

    if (UILore::PlayLore(snoLore, ptLocalPlayer)) {
        UILoreNotify::Hide();
    } else {
        Close(GetUIIDNone(), TRUE);
    }

    UILore::Console::Refresh();
}

void UILorePlayer::Open(const UIID &) {  // line 395
    OpenForLore(sgptUILoreGlobals->snoLastLorePickedUp);
    UILoreNotify::Hide();
}

void UILorePlayer::OpenToLorePage(const UIID &) {  // line 405
    UIQuestLore::ShowLoreDisplayForSNO(sgptUILoreGlobals->snoLastLorePickedUp);
}

void UILorePlayer::Close(const UIID &, BOOL bCancelLore) {  // line 413
    if (UI_TestID(idLorePlayer)) {
        if (bCancelLore) {
            UILore::StopLore();
        }
        sgptUILoreGlobals->fLorePlayerIsOpen = FALSE;
        UI_GetWindowFromID(idLorePlayer).SetActive(FALSE);
    }
}

void UILorePlayer::Update() {  // line 433
    if (!UI_TestID(idLorePlayer)) {
        return;
    }

    UIWindow & tDlg = UI_GetWindowFromID(idLorePlayer);
    tDlg.SetActive(sgptUILoreGlobals->fLorePlayerIsOpen);

    if (tDlg.IsActive() && sgptUILoreGlobals->idSound != -1) {
        SoundInstance * ptSound = SoundTryToGet(sgptUILoreGlobals->idSound);
        if (ptSound == NULL) {
            Close(GetUIIDNone(), TRUE);
        }
    }
}

BOOL UILorePlayer::IsPlaying() {  // line 462
    if (!UI_TestID(idLorePlayer)) {
        return FALSE;
    }

    UIWindow & tDlg = UI_GetWindowFromID(idLorePlayer);
    if (tDlg.IsActive() && sgptUILoreGlobals->idSound != -1) {
        SoundInstance * ptSound = SoundTryToGet(sgptUILoreGlobals->idSound);
        if (ptSound != NULL) {
            return TRUE;
        }
    }

    return FALSE;
}

void UILorePlayer::OnConversationLinePlayed(const ConversationDefinition * pdefConv) {  // line 488
    if (!UI_TestID(idLorePlayer)) {
        return;
    }

    BOOL bCancel = pdefConv->eConvType > CONV_TYPE_LORE;
    if (pdefConv->eConvType == CONV_TYPE_AMBIENT_GOSSIP && (pdefConv->dwFlags & (1 << CF_OPERATE_SPEAKER_ON_BEGIN_BIT))) {
        bCancel = FALSE;
    }

    if (bCancel) {
        Close(GetUIIDNone(), TRUE);
    }
}
