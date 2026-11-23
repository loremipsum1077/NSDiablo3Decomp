#include "UIConversations.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Sound/Sound.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/Definitions.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "UIConfirmation.h"
#include "../Console/ConsoleScreenManager.h"
#include "../Console/ConsoleModalNotificationManager.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/Player.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include "../../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../RActors/RActors.h"
#include "../../RActors/RActorAttachments.h"
#include "../../../Common/Definitions/AttachmentDefinition.h"
#include "../../Attachments.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../../Common/Monsters.h"
#include "../../../Common/Gizmo.h"
#include "../../../Common/Marker/Marker.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/TagMaps/ActorTags.h"
#include "../../ImageCollection.h"
#include "../Controls/UIAutoScrollingTextControl.h"
#include "../Controls/UITextControl.h"
#include "UIQuestComplete.h"
void UI_Print(LPCSTR szText, const BOOL fEchoToConsole); RActorID CinematographyGetFocus();  // X/Client/UserInterface/UI.cpp:3860, X/Client/Camera/Cinematography.cpp:457
typedef int32 ConversationType;  // X/Common/Definitions/ConversationDefinition.h
typedef int32 PlayerClass;  // X/Common/ConversationCommon.h
typedef int32 ConversationSpeaker;  // X/Common/ConversationCommon.h
typedef uint32 StringLabelHandle;  // X/Common/Labels.h
enum { CONV_TYPE_PLAYER_EMOTE = 1, CONV_TYPE_GLOBAL_CHATTER = 6, CONV_TYPE_LORE = 8, CONV_TYPE_TALK_MENU = 10, CONV_TYPE_QUEST_STANDARD = 11, CONV_TYPE_QUEST_FLOAT = 12, CONV_TYPE_QUEST_EVENT = 13, };  // X/Common/Definitions/ConversationDefinition.h
enum { CTNF_DONT_PLAY_SUBTITLE_BIT = 5 }; struct ConversationDefinition; SNO ConversationSpeakerToSNO(const ConversationDefinition * ptConvDef, ConversationSpeaker eSpeaker); BOOL ConversationChooseSoundForLine(SNO & snoSound, int32 & nPermIndex, const SNO snoConversation, const uint32 uLineUID, const ConversationSpeaker eLineSpeaker, const PlayerClass eLineClass, const ActorGender eLineGender, const PlayerClass ePlayerClass, const ActorGender ePlayerGender, const BOOL fReportErrors);  // ConversationDefinition.h, ConversationDefinition.cpp:1251, X/Client/ConversationUtils.cpp:365
struct PlayLineParams {  // size 0x4c, X/Common/ConversationCommon.h
    SNO snoConversation; int32 nSpeakingPlayerIndex; BOOL bZoom; BOOL bFirstLine; BOOL bHostile; BOOL bPlayerInitiated; uint32 dwLineUID; ConversationSpeaker eLineSpeaker; ActorGender eLineGender; PlayerClass eLineClass;
    ConversationSpeaker eSpeakerTarget; SNO snoSpeakerActor; ActorGender ePlayerGender; PlayerClass ePlayerClass; uint32 dwLineFlags; uint32 dwLineAnimTag; uint32 dwLineDisplayTime; int32 nLineGUID; int32 nPriority;
};
void SoundSetPaused(SoundID id, BOOL fPause);  // X/Client/Sound/Sound.cpp
SNO ConversationGetStringListSNO(SNO snoConversation); StringTableEntry * ConversationChooseStringTableEntry(const StringListDefinition * pdefStringList, uint32 dwLineUID, ConversationSpeaker eLineSpeaker, PlayerClass eLineClass, ActorGender eLineGender, PlayerClass ePlayerClass, ActorGender ePlayerGender);  // ConversationCommon.cpp
SNO ConversationSpeakerToSNO(SNO snoConversation, ConversationSpeaker eSpeaker); CRefString PlayerGetHeroDisplayName(const Player & tPlayer, BOOL fDisplayPound);  // ConversationCommon.cpp, Player.cpp
extern UIID UITXT_CONVERSATION_TEXT, UIWND_CONVERSATION_PORTRAIT, UITXT_CONVERSATION_PORTRAIT_NAME, UITXT_CONVERSATION_ACCEPT; extern UIQuestComplete theQuestCompleteUI; UIAutoScrollingText & UI_GetAutoScrollingTextFromID(const UIID & id);
void SoundFadeOut(SoundID id); Attachment * AttachmentTryToGet(const AttachmentID idAttachment);  // Sound.cpp, Attachments.cpp:106
#include "../../CPlayer.h"

uint32 ConversationTypeGetUsageFlags(const ConversationType eConvType);  // ConversationDefinition.cpp:1307
enum { CTU_ZOOMS_FOR_TARGET_PLAYER_BIT = 1, CTU_ALLOWS_MANUAL_ADVANCE_BIT = 2, CTU_NPC_UNUSABLE_DURING_CONVERSATION_BIT = 11, CTU_NO_AUDIO_ROLLOFF_BIT = 15, CTU_NEVER_SILENCE_AUDIO_BIT = 16 };  // X/Common/Definitions/ConversationDefinition.h
BOOL UI_CloseAllTabs();  // X/Client/UserInterface/UI.cpp:884
namespace UIChatBalloons { void CancelBalloonForRActor(RActorID idRActor); void AddBalloon(RActorID idRActor, SNO snoStringList, uint32 hLabel, uint32 dwTicksToDisplay, float flHeightOffset, int32 nControllerIndex); }  // UIChatBalloons.cpp
struct AdvanceConvMessage {  // size 0x10, X/Common/ConversationCommon.h
    RequiredMessageHeader tHeader; SNO snoConv; int32 nLineUID;
};
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
extern UIID UIDLG_CONVERSATION;  // 0x71017d80d8
namespace UIWaypointMap { void SetZKConversationComplete(); }  // 0x710017a210
SNO ConversationGetEndGameConversation(uint32 uContentFlags);  // X/Common/ConversationCommon.cpp:89
#include "../../../Common/Definitions/ConversationDefinition.h"
float SNOPackChunkGetActLoadPercentComplete(Act eAct); void SNOPackChunkRequestLoad(Act eAct); BOOL SNOPackChunkLoadAct(Act eAct);  // SNOFiles.cpp
enum { CF_OPERATE_SPEAKER_ON_END_BIT = 13, CF_CONSOLE_HIDE_CLOSE_BUTTON = 22 };  // X/Common/Definitions/ConversationDefinition.h
struct ConversationDefinition {  // size 0x110, X/Common/Definitions/ConversationDefinition.h
    RequiredDefinitionHeader tHeader; ConversationType eConvType; uint8 _unadopted0[0x24 - 0x10]; uint32 dwFlags; uint8 _unadopted1[0x110 - 0x28];
};
 extern ConversationGroupDef * gptGroupDefConversation;
typedef ScopedDefinitionLockR<ConversationDefinition> ScopedConversationDefinitionLockR;
template <> inline const ConversationDefinition * DefinitionLockR<ConversationDefinition>(SNO sno) { return (const ConversationDefinition *)gptGroupDefConversation->LockR(sno, FALSE); }
template <> inline void DefinitionUnlockR<ConversationDefinition>(const ConversationDefinition * pdef) { void * lpDef = (void *)pdef; gptGroupDefConversation->UnlockR(lpDef); }
#include "UILoreCommon.h"
extern const uint32 gcdwConvAutoAdvanceTicks;  // X/Common/ConversationCommon.cpp:19 (loaded through the GOT in Update, so extern here)
extern UIID UITXT_SCRIPTED_SEQUENCE_SUBTITLE;  // X/Client/UserInterface/UIGlobals.cpp:1411
void UIFunction_SetConversationCameraZoom(const BOOL fZoomed);  // X/Client/UserInterface/Interface/UIFunctions.cpp:634
BOOL ConversationPlayerNearNPC(ActorCommonData & tACDPlayer, ActorCommonData & tACDNpc);  // X/Common/Definitions/ConversationDefinition.cpp:1788
struct UpdateConvAutoAdvanceMessage {  // size 0x14, X/Common/ConversationCommon.h:73
    RequiredMessageHeader tHeader; SNO snoConv; int32 nLineUID; uint32 dwNextAdvanceTime;
};
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Common/TagMaps/Tags.h"
#include "../../../Common/UserInterface/UITextControl.h"
namespace UIConversation {

struct ConversationLine {  // size 0x88
    BOOL fMarkedForRemoval;  // 0x00
    ACDID idACDParticipants[9];  // 0x04
    ACDID idACDSpeaker;  // 0x28
    SoundID idSound;  // 0x2c
    ConversationType eConvType;  // 0x30
    PlayLineParams tParams;  // 0x34
    uint32 dwTimeToDisplay;  // 0x80
    BOOL fStarted;  // 0x84
};

enum {
    MAX_TALK_BUBBLES = 8,
};

class TalkBubbleMgr {  // size 0x24
public:
    void Initialize();
    void Dispose();
    void AddTalkBubble(RActorID idRActor);
    void DeleteTalkBubblesForRActor(RActorID idRActor);
    BOOL RActorHasTalkBubble(RActorID idRActor) const;
private:
    int32 m_nTalkBubbleCount;  // 0x00
    RActorID m_idTalkBubbles[MAX_TALK_BUBBLES];  // 0x04
};

class ConvDlgMgr {  // size 0x4
public:
    void Show(const ActorCommonData * ptACDSpeaker, const PlayLineParams & tParams, const SNO snoStringList, StringLabelHandle hLabel, const uint32 dwTicksToDisplay, const BOOL fCanManuallyAdvance);
    void Hide();
    BOOL m_bCanBeCancelled;  // 0x00
};

typedef XGrowableList<UIConversation::ConversationLine, UIConversation::ConversationLine &> ConvLineList;





















struct ConversationGlobals {  // size 0xa0
    UIConversation::ConvLineList tConvLineList;  // 0x00
    UIConversation::TalkBubbleMgr tTalkBubbleMgr;  // 0x58
    UIConversation::ConvDlgMgr tConvDlgMgr;  // 0x7c
    BOOL bInZoomedConversation;  // 0x80
    SNO sno;  // 0x84
    uint32 dwCurrentSubtitleTimeToDisplay;  // 0x88
    SNO snoCurrentSubtitleStringlist;  // 0x8c
    StringLabelHandle hCurrentSubtitleLabel;  // 0x90
    Blizzard::Time::Millisecond dwConversationCancelTime;  // 0x94
    Blizzard::Time::Millisecond dwConversationSkipAllTime;  // 0x98
};






static ConversationGlobals sgtConversationGlobals;


static const Blizzard::Time::Millisecond CONVERSATION_CANCEL_COOLDOWN_TIME_MS = 1500;

































void TalkBubbleMgr::Initialize()
{


    m_nTalkBubbleCount = 0;
    for ( int32 i = 0; i < MAX_TALK_BUBBLES; i++ )
    {
        m_idTalkBubbles[i] = RACTORID_INVALID;
    }
}

void TalkBubbleMgr::Dispose()
{


    Initialize();
}

void TalkBubbleMgr::AddTalkBubble( RActorID idRActor )
{

    RActor * ptRActor = RActorTryToGet( idRActor );
    if ( !ptRActor )
    {
        return;
    }

    if ( m_nTalkBubbleCount >= MAX_TALK_BUBBLES )
    {
        return;
    }

    if ( RActorHasTalkBubble( idRActor ) )
    {
        return;
    }

    m_idTalkBubbles[m_nTalkBubbleCount++] = idRActor;
}






void TalkBubbleMgr::DeleteTalkBubblesForRActor( RActorID idRActor )
{


    for ( int32 i = 0; i < m_nTalkBubbleCount; i++ )
    {
        if ( m_idTalkBubbles[i] == idRActor )
        {

            m_idTalkBubbles[i] = m_idTalkBubbles[m_nTalkBubbleCount - 1];
            m_idTalkBubbles[m_nTalkBubbleCount - 1] = RACTORID_INVALID;
            m_nTalkBubbleCount--;
            return;
        }
    }
}

BOOL TalkBubbleMgr::RActorHasTalkBubble( RActorID idRActor ) const
{


    for ( int32 i = 0; i < m_nTalkBubbleCount; i++ )
    {
        if ( m_idTalkBubbles[i] == idRActor )
            return TRUE;
    }
    return FALSE;
}




BOOL RActorHasTalkBubble( const RActorID idRActor )
{


    return sgtConversationGlobals.tTalkBubbleMgr.RActorHasTalkBubble( idRActor );
}

void InitializeForGame()
{


    sgtConversationGlobals.tTalkBubbleMgr.Initialize();
    sgtConversationGlobals.tConvDlgMgr.m_bCanBeCancelled = FALSE;
    sgtConversationGlobals.bInZoomedConversation = FALSE;



    sgtConversationGlobals.dwCurrentSubtitleTimeToDisplay = 0;
    sgtConversationGlobals.snoCurrentSubtitleStringlist = SNO_NONE;
    sgtConversationGlobals.hCurrentSubtitleLabel = 0;
    sgtConversationGlobals.dwConversationCancelTime = 0;
    sgtConversationGlobals.dwConversationSkipAllTime = 0;
}


void DisposeForGame()
{


    sgtConversationGlobals.tConvLineList.RemoveAll();
    sgtConversationGlobals.tTalkBubbleMgr.Dispose();
    sgtConversationGlobals.bInZoomedConversation = FALSE;
}

static CRefString sACDGetName( const ActorCommonData * ptACDSpeaker, const PlayLineParams & tParams )
{


    if ( tParams.eLineSpeaker == 0 )
    {
        Player * ptPlayer = PlayerGetByPlayerIndex( tParams.nSpeakingPlayerIndex );
        if ( ptPlayer )
        {
            if ( PlayerIsLocal( ptPlayer ) &&
                 !GameIsWeeklyChallenge() )
            {

                return PlayerGetHeroDisplayName( *ptPlayer, FALSE );
            }



            return CPlayerGetServiceName( *ptPlayer, FALSE );
        }


        return "";
    }

    if ( ptACDSpeaker )
    {
        if ( ptACDSpeaker->IsMonster() )
        {
            return MonsterGetName( ptACDSpeaker, FALSE );
        }

        if ( ptACDSpeaker->IsGizmo() )
        {
            CRefString sName;
            if ( GizmoGetUIName( *ptACDSpeaker, sName ) )
            {
                return sName;
            }
        }

        if ( ActorGetType( ptACDSpeaker->tSNOName ) == ACTORTYPE_SERVER_PROP )
        {
            CRefString sName;
            if ( GizmoGetUINameFromLabel( ptACDSpeaker->tSNOName.snoHandle, sName ) )
            {
                return sName;
            }
        }

        RActorID idRActor = (RActorID)ACDGetOwner( ptACDSpeaker->id );
        RActor * ptRActor = RActorTryToGet( idRActor );
        if ( ptRActor )
        {
            return MarkerNameGetString( ptRActor->tName );
        }
    }
    else
    {




        SNO snoActor = ConversationSpeakerToSNO( tParams.snoConversation, tParams.eLineSpeaker );
        ScopedActorDefinitionLockR pdefActor( snoActor );

        if ( pdefActor->eType == ACTORTYPE_MONSTER )
        {

            SNOName tSNOName( ACTORTYPE_MONSTER, snoActor );
            return MonsterGetName( tSNOName, NULL, 0, FALSE );
        }
        else if ( pdefActor->eType == ACTORTYPE_SERVER_PROP || pdefActor->eType == ACTORTYPE_GIZMO )
        {
            CRefString sName;
            if ( GizmoGetUINameFromLabel( snoActor, sName ) )
            {
                return sName;
            }
        }
    }

    return "Unknown";
}







static void sWriteTextToLog(
    const ActorCommonData * ptACDSpeaker,
    LPCSTR szText,
    const PlayLineParams & tParams )
{
    FontSpecifier eSpecifier;
    CRefString sConversationLabel;
    if (tParams.bHostile)
    {

        sConversationLabel = "ConversationLine";
        eSpecifier = FONTCOLOR_YELL;
    }
    else
    {
        sConversationLabel = "ConversationLineSays";
        eSpecifier = FONTCOLOR_QUEST_LOG;
    }
    CRefString sName = sACDGetName(ptACDSpeaker, tParams);
    CRefString usMessage = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), sConversationLabel.str()),
        sName.str(), szText, NULL);

    AppGlobals & tAppGlobals = sgtAppGlobals;  // DWARF local (line 425); no instructions of its own, bound to sgtAppGlobals (loaded via GOT at PlayLine+0x858)
    if (AppDrawFlagIsSet(APP_DRAW_DEBUG_CALLOUTS_BIT) || tAppGlobals.eDebugDisplayMode == DDM_CONVERSATIONS)
    {
        usMessage += " (";
        usMessage += SNOToString(SNOName(CONVERSATION_SNO, tParams.snoConversation), FALSE);
        usMessage += ")";
        usMessage += CRefString().Format("\n%s\n", HyperlinkCreateFromSNO(SNOName(CONVERSATION_SNO, tParams.snoConversation), "LINK").str());
    }

    Set_Text_Color(usMessage, eSpecifier);
    UI_Print(usMessage, TRUE);
}


static AttachmentID sAttachSound(
    RActor * ptRActor,
    SNO snoSound,
    int32 nPermIndex,
    BOOL bPlayAs2D )
{
    TriggerEvent tTriggerEvent;
    TriggerEventInit( tTriggerEvent );
    tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
    tTriggerEvent.eAttachmentType = ATTACHMENTTYPE_SOUND;
    tTriggerEvent.snoname.Set( 0x28, snoSound );
    tTriggerEvent.dwID = 69;
    tTriggerEvent.dwPinFlags = nPermIndex;  // was 0; PlayLine+0x450 stores nPermIndex here (451:36); ReattachConversationLinesForSpeaker passes 0, unchanged
    if ( bPlayAs2D ) tTriggerEvent.dwTriggerFlags |= ( 1 << TRIGGERFLAG_SOUND_PLAY_AS_2D ); else tTriggerEvent.dwTriggerFlags &= ~( 1 << TRIGGERFLAG_SOUND_PLAY_AS_2D );
    return RActorSendRMessage( *ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0 );
}


static SoundID sPlaySoundForLine(
    const ACDID idACDSpeaker,
    const PlayLineParams & tParams )
{
    SNO snoSound = SNO_NONE;
    int32 nPermIndex = -1;

    ConversationChooseSoundForLine(
        snoSound,
        nPermIndex,
        tParams.snoConversation,
        tParams.dwLineUID,
        tParams.eLineSpeaker,
        tParams.eLineClass,
        tParams.eLineGender,
        tParams.ePlayerClass,
        tParams.ePlayerGender,
        TRUE );

    if (snoSound == SNO_NONE ||
        nPermIndex == -1)
    {
        return SOUNDID_INVALID;
    }


    BOOL fSkipTempVO = CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);



    if (fSkipTempVO)
    {
        ScopedSoundDefinitionLockR pdefSound(snoSound);
        SNO snoSoundBank = pdefSound->ptPermutations[nPermIndex].snoValidatedSoundBank;
        if (snoSoundBank != SNO_NONE)
        {
            ScopedSoundBankDefinitionLockR pdefSoundBank(snoSoundBank);
            int32 nSampleIndex = pdefSound->ptPermutations[nPermIndex].nSampleIndex;
            if (pdefSoundBank->ptSoundSamples[nSampleIndex].IsTemporaryVO())
            {
                return SOUNDID_INVALID;  // inferred: the single ~pdefSoundBank (500) / two ~pdefSound (501) rows fit an early return here
            }

        }
    }

    ScopedConversationDefinitionLockR pdefConv(tParams.snoConversation);
    uint32 dwUsageFlags = ConversationTypeGetUsageFlags(pdefConv->eConvType);




    BOOL bPlayAs2D = (dwUsageFlags & (1 << CTU_NO_AUDIO_ROLLOFF_BIT)) || (CinematographyGetFocus() != RACTORID_INVALID);  // columns 77 (||), 81 (call), 106 (!=)

    if (!bPlayAs2D)
    {


        SNO snoSpeaker = ConversationSpeakerToSNO(pdefConv, tParams.eLineSpeaker);
        if (snoSpeaker != SNO_NONE)
        {
            ScopedActorDefinitionLockR pdefActor(snoSpeaker);
            bPlayAs2D = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_VO_PLAYS_ON_PLAYER);
        }
    }

    SoundID idSound = SOUNDID_INVALID;
    RActor * ptRActor = NULL;


    if (!bPlayAs2D)
    {
        RActorID idRActor = (RActorID)ACDGetOwner(idACDSpeaker);
        ptRActor = RActorTryToGet(idRActor);
    }


    if (!ptRActor)
    {
        Player * ptPlayer = LocalPlayerGet();
        ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
    }

    if (ptRActor)
    {
        AttachmentID idAttachment = sAttachSound(ptRActor, snoSound, nPermIndex, bPlayAs2D);
        Attachment * ptAttachment = AttachmentTryToGet(idAttachment);
        if (ptAttachment)
        {
            idSound = ptAttachment->idAttachedObject;

            SoundInstance * ptSound = SoundTryToGet(idSound);
            if (ptSound)
            {
                ptSound->flFadeOutTime = 1.5f;
            }
        }
    }


    return idSound;
}

static void sCancelConvLine( ConversationLine & tConv, const BOOL fStoppedByUser )
{

    if ( tConv.fMarkedForRemoval )
    {
        return;
    }

    if ( tConv.idSound != SOUNDID_INVALID )
    {
        SoundInstance * ptSound = SoundTryToGet( tConv.idSound );

        if ( fStoppedByUser && ptSound )
        {


            ptSound->flFadeOutTime = 0.25f;
        }


        SoundFadeOut( tConv.idSound );
    }

    tConv.idSound = SOUNDID_INVALID;

    if ( tConv.eConvType == CONV_TYPE_TALK_MENU || tConv.eConvType == CONV_TYPE_QUEST_STANDARD || tConv.eConvType == CONV_TYPE_QUEST_EVENT )
    {


        sgtConversationGlobals.tConvDlgMgr.Hide();
    }
    else
    {
        RActorID idRActor = (RActorID)ACDGetOwner( tConv.idACDSpeaker );
        if ( idRActor != RACTORID_INVALID )
        {

            CancelLineForRActor( idRActor );
        }
    }

    tConv.idACDSpeaker = ACDID_INVALID;

    tConv.fMarkedForRemoval = TRUE;
}



static void sQuietConvLine(
    ConversationLine & tConv,
    BOOL bQuiet )
{
    if ( tConv.fMarkedForRemoval )
    {
        return;
    }

    SoundInstance * ptSound = SoundTryToGet( tConv.idSound );
    if ( ptSound )
    {
        ptSound->SetControlledVolumeAtten( bQuiet ? 0.0f : 1.0f );
    }
}


static SoundID sPlayLine(
    const ACDID idACDSpeaker,
    const PlayLineParams & tParams,
    const uint32 dwTimeToDisplay )
{
    SNO snoStringList = ConversationGetStringListSNO(tParams.snoConversation);
    ScopedStringListDefinitionLockR pdefStringList(snoStringList);
    StringTableEntry * ptTableEntry = ConversationChooseStringTableEntry(pdefStringList,

        tParams.dwLineUID,
        tParams.eLineSpeaker,

        tParams.eLineClass,
        tParams.eLineGender,
        tParams.ePlayerClass,
        tParams.ePlayerGender );
    if (ptTableEntry == NULL)
    {
        return SOUNDID_INVALID;
    }

    ScopedConversationDefinitionLockR pdefConv(tParams.snoConversation);
    SoundID idSound = sPlaySoundForLine(idACDSpeaker, tParams);
    ActorCommonData * ptACDSpeaker = ACDTryToGet(idACDSpeaker);

    switch (pdefConv->eConvType)
    {
    case CONV_TYPE_TALK_MENU:
    case CONV_TYPE_QUEST_STANDARD:
    case CONV_TYPE_QUEST_EVENT:
        {
            uint32 dwUsageFlags = ConversationTypeGetUsageFlags(pdefConv->eConvType);
            BOOL fCanManuallyAdvance = ( ( dwUsageFlags & ( 1 << CTU_ALLOWS_MANUAL_ADVANCE_BIT ) ) != 0 );
            sgtConversationGlobals.tConvDlgMgr.Show(ptACDSpeaker, tParams, snoStringList, ptTableEntry->hLabel, dwTimeToDisplay, fCanManuallyAdvance);
        }
        break;

    case CONV_TYPE_GLOBAL_CHATTER:
    case CONV_TYPE_LORE:
        break;

    case CONV_TYPE_PLAYER_EMOTE:
        {


            RActorID idRActor = (RActorID)ACDGetOwner(idACDSpeaker);
            if (idRActor != RACTORID_INVALID)
            {
                UIChatBalloons::AddBalloon(idRActor, snoStringList, ptTableEntry->hLabel, dwTimeToDisplay, 0.0f, -1);
            }
        }
        break;

    default:
        {





            RActorID idRActor = (RActorID)ACDGetOwner(idACDSpeaker);
            if (idRActor != RACTORID_INVALID)
            {


                BOOL fInScriptedSequence = CGameInScriptedSequence();
                if ((fInScriptedSequence && !Prefs::GetSoundPrefs().CinematicsSubtitlesEnabled()) ||
                    (!fInScriptedSequence && !Prefs::GetSoundPrefs().QuestSubtitlesEnabled()))
                {

                    sgtConversationGlobals.tTalkBubbleMgr.AddTalkBubble(idRActor);
                }
                else
                {

                    UIChatBalloons::AddBalloon(idRActor, snoStringList, ptTableEntry->hLabel, dwTimeToDisplay, 0.0f, -1);
                }
            }
        }
        break;
    }


    if (!(tParams.dwLineFlags & (1 << CTNF_DONT_PLAY_SUBTITLE_BIT)))
    {
        if (CGameInScriptedSequence())
        {
            sgtConversationGlobals.dwCurrentSubtitleTimeToDisplay = CGameGetRealTicks() + dwTimeToDisplay;
            sgtConversationGlobals.snoCurrentSubtitleStringlist = snoStringList;
            sgtConversationGlobals.hCurrentSubtitleLabel = ptTableEntry->hLabel;
        }

        if (Prefs::GetChatPrefs().EchoQuestDialogToChat())
        {
            sWriteTextToLog(ptACDSpeaker, ptTableEntry->szText, tParams);
        }
    }

    return idSound;
}



void PlayLine(
    const ACDID idACDSpeaker,
    const ACDID * pidACDParticipants,
    const PlayLineParams & tParams,
    const uint32 dwTimeToDisplay )
{
    ScopedConversationDefinitionLockR pdefConv(tParams.snoConversation);























    ConversationLine tConv;
    tConv.idSound = sPlayLine(idACDSpeaker, tParams, dwTimeToDisplay);
    tConv.eConvType = pdefConv->eConvType;
    tConv.tParams = tParams;
    tConv.fMarkedForRemoval = FALSE;
    tConv.idACDSpeaker = idACDSpeaker;
    tConv.dwTimeToDisplay = dwTimeToDisplay;
    tConv.fStarted = FALSE;  // inferred: merged into the stp of line 764 (PlayLine+0x9bc)

    SigmaMemoryCopy(tConv.idACDParticipants, pidACDParticipants, sizeof(tConv.idACDParticipants));

    sgtConversationGlobals.tConvLineList.AddTail_Tracked(tConv);



    if (tParams.bFirstLine)
    {
        UILorePlayer::OnConversationLinePlayed(pdefConv);
    }


    Update();











}


void StopLine( const int32 nLineGUID, const BOOL fStoppedByUser )
{


    POS pos = sgtConversationGlobals.tConvLineList.GetHeadPosition();
    while ( pos )
    {

        ConversationLine & tConv = sgtConversationGlobals.tConvLineList.GetNext( pos );
        if ( tConv.tParams.nLineGUID == nLineGUID )
        {
            sCancelConvLine( tConv, fStoppedByUser );
            return;
        }
    }
}

void EndGameConversation( const UIID & )
{


    SNODataMessage tEndMessage;
    tEndMessage.snoData = sgtConversationGlobals.sno;
    MessageSendToServer( CMSG_GAME_END_REQUEST, &tEndMessage, sizeof( tEndMessage ), LocalPlayerGetPrimary() );
}

void ActTransitionConversation( const UIID & )
{


    if ( !SNOPackChunkLoadAct( ACT_5 ) )
    {

        NotificationCreationParams tParams;
        tParams.uszMessage = LocalizedSprintf( StringListFetch( "ConsoleErrors:CannotProceedGenericContentNotReady" ).str(),
            UnicodeText::FormatNumber( SNOPackChunkGetActLoadPercentComplete( ACT_5 ) * 100.0f, TRUE, FNS_NORMAL, 2, 2, 0 ).str(), NULL );



        SNOPackChunkRequestLoad( ACT_5 );

        Console::Screens::GetModalNotificationManager().SetOkNotification( tParams );
    }
    else
    {

        SNODataMessage tEndMessage;
        tEndMessage.snoData = sgtConversationGlobals.sno;
        MessageSendToServer( CMSG_ACT_TRANSITION_REQUEST, &tEndMessage, sizeof( tEndMessage ), LocalPlayerGetPrimary() );
    }
}
void EndConversation( const SNO snoConversation, const ACDNetworkName annSpeaker, const int32 nPlayerTalkingIndex )
{



    if ( snoConversation == GlobalSNOGet( GLOBALSNO_ZK_CUBE_INTRO_CONVERSATION ) )
    {
        UIWaypointMap::SetZKConversationComplete();
    }

    ScopedConversationDefinitionLockR pdefConv( snoConversation );

    if ( pdefConv->dwFlags & ( 1 << CF_OPERATE_SPEAKER_ON_END_BIT ) )
    {

        ANNDataMessage tMessage;
        tMessage.ann = annSpeaker;

        Player * ptPlayer = PlayerGetByPlayerIndex( nPlayerTalkingIndex );




        MessageSendToServer( CMSG_NPC_VENDOR_REQUEST, &tMessage, sizeof( tMessage ), ptPlayer );
    }

















    uint32 uContentFlags = 0xffffffff;
    SNO snoEndGameConversation = ConversationGetEndGameConversation( uContentFlags );
    if ( snoEndGameConversation == snoConversation )
    {

        CRefString sDisplay = StringListFetch( "H2OLayout:PromptToEndGame" );
        sgtConversationGlobals.sno = snoConversation;
        UIConfirmation::Show( sDisplay.str(), EndGameConversation, NULL, FALSE, FALSE, TRUE, -1 );
    }

    else if ( snoConversation == ConversationGetEndGameConversation( 8 ) )
    {


        CRefString sDisplay = StringListFetch( "Gizmos:ActSwitchConfirm" );
        sgtConversationGlobals.sno = snoConversation;
        UIConfirmation::Show( sDisplay.str(), ActTransitionConversation, NULL, FALSE, FALSE, TRUE, -1 );
    }
}



void Advance( const UIID &, const Player * ptPlayer )
{















    if ( CanCancel() || UI_CloseAllTabs() )
    {
        SNO snoConv = SNO_NONE;
        int32 nLineUID = -1;





        POS pos = sgtConversationGlobals.tConvLineList.GetHeadPosition();
        while ( pos )
        {

            ConversationLine & tConv = sgtConversationGlobals.tConvLineList.GetNext( pos );
            uint32 dwUsageFlags = ConversationTypeGetUsageFlags( tConv.eConvType );

            if ( dwUsageFlags & ( 1 << CTU_ALLOWS_MANUAL_ADVANCE_BIT ) )
            {
                snoConv = tConv.tParams.snoConversation;
                nLineUID = tConv.tParams.nLineGUID;
                break;
            }
        }
        if ( snoConv != SNO_NONE )
        {
            AdvanceConvMessage tMessage;
            tMessage.snoConv = snoConv;
            tMessage.nLineUID = nLineUID;
            if ( ptPlayer == NULL )
            {
                ptPlayer = LocalPlayerGetPrimary();
            }
            MessageSendToServer( CMSG_REQUEST_CONV_ADVANCE, &tMessage, sizeof( tMessage ), ptPlayer );
        }
    }
}



void ConsoleAdvance( const UIID & id, const int32 nControllerIndex )
{






    Player * ptPlayer = PlayerGetByControllerIndex( nControllerIndex );
    if ( ptPlayer == NULL )
    {
        return;
    }

    Advance( id, ptPlayer );
    SNO sound = GlobalSNOGet( GLOBALSNO_SOUND_CLICK_GENERIC );
    SoundManager::PlaySound( sound, cgtPSODefaults );





    sgtConversationGlobals.dwConversationCancelTime = Blizzard::Time::Milliseconds();
}

void ConsoleCancel( const UIID & id, const int32 nControllerIndex )
{



    if ( PlayerGetByControllerIndex( nControllerIndex ) == NULL )
    {
        return;
    }

    Cancel( id );
    SNO sound = GlobalSNOGet( GLOBALSNO_SOUND_CLICK_GENERIC );
    SoundManager::PlaySound( sound, cgtPSODefaults );





    sgtConversationGlobals.dwConversationSkipAllTime = Blizzard::Time::Milliseconds();
}




void SendCancelRequest()
{



    SimpleMessage tMessage;
    MessageSendToServer( CMSG_REQUEST_CONV_CANCEL, &tMessage, sizeof( tMessage ), LocalPlayerGetPrimary() );
}
BOOL CanCancel()
{


    UIWindow & tWnd = UI_GetWindowFromID( UIDLG_CONVERSATION );
    if ( tWnd.IsActive() && sgtConversationGlobals.tConvDlgMgr.m_bCanBeCancelled )
        return TRUE;
    return FALSE;
}


static BOOL sConversationShouldZoom( ConversationLine & tConv );  // defined at line 1183


void Cancel( const UIID & )
{


    if ( CanCancel() )
    {

        SendCancelRequest();
    }
}
void Update(
    )
{
    BOOL fZoom = FALSE;
    ConversationLine * ptConvBest = NULL;
    BOOL bLorePlaying = UILorePlayer::IsPlaying();

    POS pos = sgtConversationGlobals.tConvLineList.GetHeadPosition();
    while ( pos )
    {
        POS posOld = pos;
        ConversationLine & tConv = sgtConversationGlobals.tConvLineList.GetNext( pos );


        if ( !tConv.fStarted )
        {

            SoundInstance * ptSound = SoundTryToGet( tConv.idSound );
            if ( ptSound && ptSound->HasStarted() )
            {
                tConv.fStarted = TRUE;




                UpdateConvAutoAdvanceMessage tMessage;
                tMessage.snoConv = tConv.tParams.snoConversation;
                tMessage.nLineUID = tConv.tParams.nLineGUID;
                tMessage.dwNextAdvanceTime = CGameGetPredictedServerTime() + tConv.dwTimeToDisplay + gcdwConvAutoAdvanceTicks;
                MessageSendToServer( CMSG_UPDATE_CONV_AUTO_ADVANCE, &tMessage, sizeof( tMessage ), LocalPlayerGetPrimary() );
            }
        }


        if ( tConv.fMarkedForRemoval )
        {
            sgtConversationGlobals.tConvLineList.RemoveAt( posOld );
        }
        else
        {
            uint32 dwUsageFlags = ConversationTypeGetUsageFlags( tConv.eConvType );
            if ( ( dwUsageFlags & ( 1 << CTU_ZOOMS_FOR_TARGET_PLAYER_BIT ) ) &&
                 sConversationShouldZoom( tConv ) )
            {
                fZoom = TRUE;
            }



            if ( !bLorePlaying && ( ptConvBest == NULL || ptConvBest->eConvType < tConv.eConvType ||
                 ( ptConvBest->eConvType == tConv.eConvType && ptConvBest->tParams.nPriority < tConv.tParams.nPriority ) ) )
            {
                ptConvBest = &tConv;
            }
        }
    }


    pos = sgtConversationGlobals.tConvLineList.GetHeadPosition();
    while ( pos )
    {
        ConversationLine & tConv = sgtConversationGlobals.tConvLineList.GetNext( pos );
        uint32 dwUsageFlags = ConversationTypeGetUsageFlags( tConv.eConvType );

        if ( ( ptConvBest && ptConvBest->tParams.nLineGUID == tConv.tParams.nLineGUID ) || ( dwUsageFlags & ( 1 << CTU_NEVER_SILENCE_AUDIO_BIT ) ) )  // operand order chosen by matching; the other order does not fold the loads above the tbnz
        {
            sQuietConvLine( tConv, FALSE );
        }
        else
        {
            sQuietConvLine( tConv, TRUE );
        }
    }

    if ( sgtConversationGlobals.bInZoomedConversation != fZoom )
    {
        sgtConversationGlobals.bInZoomedConversation = fZoom;
        UIFunction_SetConversationCameraZoom( fZoom );
    }

    if ( CGameInScriptedSequence() && Prefs::GetSoundPrefs().CinematicsSubtitlesEnabled() )
    {
        if ( sgtConversationGlobals.dwCurrentSubtitleTimeToDisplay > CGameGetRealTicks() &&
             sgtConversationGlobals.snoCurrentSubtitleStringlist != SNO_NONE )
        {
            CRefString sText = StringListFetchByLabel(
                sgtConversationGlobals.snoCurrentSubtitleStringlist,
                sgtConversationGlobals.hCurrentSubtitleLabel );

            UI_GetTextFromID(UITXT_SCRIPTED_SEQUENCE_SUBTITLE).SetText(sText.str(), FALSE);
        }
        else
        {
            UI_GetTextFromID(UITXT_SCRIPTED_SEQUENCE_SUBTITLE).SetText("", FALSE);  // inferred: the call at Update+0x604 is line 0, i.e. two identical copies merged (here and line 1145)
        }
    }
    else
    {
        sgtConversationGlobals.snoCurrentSubtitleStringlist = SNO_NONE;
        UI_GetTextFromID(UITXT_SCRIPTED_SEQUENCE_SUBTITLE).SetText("", FALSE);
    }
}


static BOOL sACDIsInConversation( const ConversationLine & tConv, const ACDID idACD )
{
    for ( int32 i = 0; i < 9; i++ )
    {
        if ( tConv.idACDParticipants[i] == idACD )
        {
            return TRUE;
        }
    }
    return FALSE;
}

static void sShowSubtitleText( ConversationLine & tConv, RActorID idRActor );




















static BOOL sConversationShouldZoom(
    ConversationLine & tConv )
{
    if ( tConv.tParams.bZoom )
    {


        ActorCommonData * ptACDNpc = ACDTryToGet( tConv.idACDParticipants[1] );
        if ( ptACDNpc )
        {
            ACDID idACDLocalPlayer = LocalPlayerACDGet();
            ActorCommonData * ptACDPlayer = ACDTryToGet( idACDLocalPlayer );

            if ( ptACDPlayer && ConversationPlayerNearNPC( *ptACDPlayer, *ptACDNpc ) )
            {
                return TRUE;
            }
        }



        tConv.tParams.bZoom = FALSE;
    }

    return FALSE;
}









BOOL IsImportantConversationPlaying()
{

    POS pos = sgtConversationGlobals.tConvLineList.GetHeadPosition();
    while ( pos )
    {


        ConversationLine & tConv = sgtConversationGlobals.tConvLineList.GetNext( pos );
        switch ( tConv.eConvType )
        {
        case CONV_TYPE_TALK_MENU:
        case CONV_TYPE_QUEST_STANDARD:
        case CONV_TYPE_QUEST_FLOAT:
        case CONV_TYPE_QUEST_EVENT:
            return TRUE;
        }
    }
    return FALSE;
}
BOOL IsUIBlockingConversationPlaying()
{


    if ( UI_GetWindowFromID( UIDLG_CONVERSATION ).IsActive() )
    {
        return TRUE;
    }

    POS pos = sgtConversationGlobals.tConvLineList.GetHeadPosition();
    while ( pos )
    {
        ConversationLine & tConv = sgtConversationGlobals.tConvLineList.GetNext( pos );
        if ( !tConv.fMarkedForRemoval && tConv.eConvType == CONV_TYPE_TALK_MENU )
        {
            return TRUE;
        }
    }
    return FALSE;
}


BOOL ACDIsInConversation( const ACDID idACD, BOOL bConvDialogOnly, BOOL bUnusableOnly )
{

    POS pos = sgtConversationGlobals.tConvLineList.GetHeadPosition();
    while ( pos )
    {
        ConversationLine & tConv = sgtConversationGlobals.tConvLineList.GetNext( pos );

        if ( bConvDialogOnly &&
             tConv.eConvType != CONV_TYPE_TALK_MENU &&
             tConv.eConvType != CONV_TYPE_QUEST_STANDARD &&
             tConv.eConvType != CONV_TYPE_QUEST_EVENT )
        {
            continue;
        }

        if ( bUnusableOnly )
        {
            uint32 dwUsageFlags = ConversationTypeGetUsageFlags( tConv.eConvType );
            if ( !( dwUsageFlags & ( 1 << CTU_NPC_UNUSABLE_DURING_CONVERSATION_BIT ) ) )
            {
                continue;
            }
        }

        if ( sACDIsInConversation( tConv, idACD ) )
        {
            return TRUE;
        }
    }
    return FALSE;
}



















void Pause( BOOL fPause )
{




    POS pos = sgtConversationGlobals.tConvLineList.GetHeadPosition();
    while ( pos )
    {
        ConversationLine & tConv = sgtConversationGlobals.tConvLineList.GetNext( pos );
        SoundSetPaused( tConv.idSound, fPause );
    }
}




BOOL IsCancelButtonOnCooldown()
{


    Blizzard::Time::Millisecond dwNow = Blizzard::Time::Milliseconds();
    return ( dwNow - sgtConversationGlobals.dwConversationCancelTime ) < CONVERSATION_CANCEL_COOLDOWN_TIME_MS;
}

BOOL IsSkipAllButtonOnCooldown()
{


    Blizzard::Time::Millisecond dwNow = Blizzard::Time::Milliseconds();
    return ( dwNow - sgtConversationGlobals.dwConversationSkipAllTime ) < CONVERSATION_CANCEL_COOLDOWN_TIME_MS;
}





















void CancelLineForRActor( const RActorID idRActor )
{

    UIChatBalloons::CancelBalloonForRActor( idRActor );
    sgtConversationGlobals.tTalkBubbleMgr.DeleteTalkBubblesForRActor( idRActor );
}


void OnWorldLeave()
{

    ACDID idACDLocalPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet( idACDLocalPlayer );
    if ( !ptACDPlayer )
    {
        return;
    }

    WorldPlace place = ptACDPlayer->GetPlace();


    POS pos = sgtConversationGlobals.tConvLineList.GetHeadPosition();
    while ( pos )
    {
        ConversationLine & tConv = sgtConversationGlobals.tConvLineList.GetNext( pos );
        ACDID idACDSpeaker = tConv.idACDSpeaker;


        if ( idACDSpeaker == idACDLocalPlayer ||
             PlayerIsLocal( ACDGetPetOwner( idACDSpeaker ) ) )
        {



            SoundInstance * ptSound = SoundTryToGet( tConv.idSound );
            if ( ptSound )
            {

                RActorID idRActor = (RActorID)ACDGetOwner( idACDSpeaker );
                RActor * ptRActor = RActorTryToGet( idRActor );
                if ( ptRActor )
                {
                    TriggerEvent tTriggerEvent;
                    TriggerEventInit( tTriggerEvent );
                    tTriggerEvent.eType = TRIGGEREDEVENTTYPE_DETACH_OBJECT;
                    tTriggerEvent.dwID = 69;
                    RActorSendRMessage( *ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, (ParentType)0 );
                }


                ptSound->SetPosition( place.wp, 0.0f );
            }
        }
    }
}

void ReattachConversationLinesForSpeaker( ActorCommonData & tACDSpeaker )
{

    POS pos = sgtConversationGlobals.tConvLineList.GetHeadPosition();
    while ( pos )
    {
        ConversationLine & tConv = sgtConversationGlobals.tConvLineList.GetNext( pos );


        if ( tACDSpeaker.id != tConv.idACDSpeaker )
        {
            continue;
        }


        SoundInstance * ptSound = SoundTryToGet( tConv.idSound );
        if ( !ptSound )
        {
            continue;
        }


        RActorID idRActor = (RActorID)ACDGetOwner( tACDSpeaker.id );
        RActor * ptRActor = RActorTryToGet( idRActor );
        if ( !ptRActor )
        {
            return;
        }



        SNO snoSound = GlobalSNOGet( GLOBALSNO_SOUND_PLACEHOLDER_SILENCE );
        AttachmentID idAttachment = sAttachSound( ptRActor, snoSound, 0, FALSE );
        Attachment * ptAttachment = AttachmentTryToGet( idAttachment );
        if ( ptAttachment )
        {
            SoundID idNewSound = ptAttachment->idAttachedObject;
            SoundInstance * ptNewSound = SoundTryToGet( idNewSound );
            if ( ptNewSound )
            {


                ptSound->SetParent( ptNewSound->tParentData );
                ptAttachment->idAttachedObject = tConv.idSound;
                SoundDelete( idNewSound );
            }
        }



        switch ( tConv.eConvType )
        {
        case CONV_TYPE_GLOBAL_CHATTER:
        case CONV_TYPE_LORE:
        case CONV_TYPE_TALK_MENU:
        case CONV_TYPE_QUEST_STANDARD:
        case CONV_TYPE_QUEST_EVENT:
            break;

        case CONV_TYPE_PLAYER_EMOTE:
            sShowSubtitleText( tConv, idRActor );
            break;

        default:
            if ( Prefs::GetSoundPrefs().QuestSubtitlesEnabled() )
            {
                sShowSubtitleText( tConv, idRActor );
            }
            else
            {
                sgtConversationGlobals.tTalkBubbleMgr.AddTalkBubble( idRActor );
            }
            break;
        }
    }
}








void ConvDlgMgr::Show( const ActorCommonData * ptACDSpeaker, const PlayLineParams & tParams, const SNO snoStringList, StringLabelHandle hLabel, const uint32 dwTicksToDisplay, const BOOL fCanManuallyAdvance )
{






    theQuestCompleteUI.Show( FALSE, NULL, FALSE );


    UI_GetWindowFromID( UIDLG_CONVERSATION ).SetActive( TRUE );
    UIAutoScrollingText & tScrollingText = UI_GetAutoScrollingTextFromID( UITXT_CONVERSATION_TEXT );
    tScrollingText.SetText( StringListFetchByLabel( snoStringList, hLabel ) );
    tScrollingText.SetScrollRateFromDuration( dwTicksToDisplay );
    tScrollingText.StartTimers();

    {
        ImageHandle hImage = 0;
        if ( tParams.snoSpeakerActor != SNO_NONE )
        {

            if ( ActorDefinitionTagMapValueExists( TAG_ACTOR_PORTRAIT_IMAGE, tParams.snoSpeakerActor ) )
            {
                hImage = ActorDefinitionGetTagMapValueInt( TAG_ACTOR_PORTRAIT_IMAGE, tParams.snoSpeakerActor );
            }
        }


        ImageTextureFrame tFrame;
        ImageCollectionGetImageInfo( 2, hImage, tFrame );
        UI_GetWindowFromID( UIWND_CONVERSATION_PORTRAIT ).SetTextureFrame( tFrame );


        CRefString sName( "Default" );

        if ( ptACDSpeaker )
        {
            if ( ACDIsPlayer( ptACDSpeaker->id ) )
            {
                Player * ptPlayer = PlayerGetByACD( ptACDSpeaker->id );
                if ( ptPlayer )
                {
                    sName = PlayerGetHeroDisplayName( *ptPlayer, FALSE );
                }
            }
            else
            {
                sName = sACDGetName( ptACDSpeaker, tParams );
            }
        }
        else
        {
            sName = sACDGetName( NULL, tParams );
        }



        UI_GetTextFromID( UITXT_CONVERSATION_PORTRAIT_NAME ).SetText( sName, FALSE );
    }






    ScopedConversationDefinitionLockR pdefConv( tParams.snoConversation );
    UIText & acceptBtn = UI_GetTextFromID( UITXT_CONVERSATION_ACCEPT );
    acceptBtn.SetActive( fCanManuallyAdvance && !( pdefConv->dwFlags & ( 1 << CF_CONSOLE_HIDE_CLOSE_BUTTON ) ) );

    Player * ptPlayer = PlayerGetByPlayerIndex( tParams.nSpeakingPlayerIndex );
    if ( ptPlayer )
    {

        acceptBtn.SetResizeToFit( RESIZETOFIT_VERTICAL_ONLY );
        acceptBtn.SetControllerIndexOverride( ptPlayer->nControllerIndex );
    }


    sgtConversationGlobals.tConvDlgMgr.m_bCanBeCancelled = fCanManuallyAdvance;



    sgtConversationGlobals.dwConversationCancelTime = 0;
    sgtConversationGlobals.dwConversationSkipAllTime = 0;
}



void ConvDlgMgr::Hide()
{


    UI_GetWindowFromID( UIDLG_CONVERSATION ).SetActive( FALSE );
}

static void sShowSubtitleText( ConversationLine & tConv, RActorID idRActor )
{

    SNO snoStringList = ConversationGetStringListSNO( tConv.tParams.snoConversation );
    ScopedStringListDefinitionLockR pdefStringList( snoStringList );
    StringTableEntry * ptTableEntry = ConversationChooseStringTableEntry( pdefStringList,
        tConv.tParams.dwLineUID,
        tConv.tParams.eLineSpeaker,

        tConv.tParams.eLineClass,
        tConv.tParams.eLineGender,
        tConv.tParams.ePlayerClass,
        tConv.tParams.ePlayerGender );

    if ( ptTableEntry )
    {

        UIChatBalloons::AddBalloon( idRActor, snoStringList, ptTableEntry->hLabel, tConv.dwTimeToDisplay, 0.0f, -1 );
    }
}

}  // namespace UIConversation
