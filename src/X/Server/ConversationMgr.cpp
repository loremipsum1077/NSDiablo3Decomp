// Original file: X/Server/ConversationMgr.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "ConversationMgr.h"
#include "../Common/CommonTypes.h"
#include "SGame.h"
#include "SGameManager.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Common/Game.h"
#include "../Common/Player.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Definitions/HeroDefinition.h"
#include "../Common/Definitions/AnimationDefinition.h"
#include "../Common/AnimPlayer.h"
#include "../../BlizzardCore/Include/Blizzard/Lock.h"

void ScriptDefinitionDisposeScript(SNO snoScript);  // X/Server/Definitions/ScriptDefinition.cpp:906

// X/Common/Definitions/ConversationDefinition.h
typedef int32 ConversationType;
typedef int32 ConversationSpeaker;
uint32 ConversationTypeGetUsageFlags(const ConversationType eConvType);  // ConversationDefinition.cpp:1307

#include "../Common/ConversationTreeIterator.h"
#include "PlayerList.h"
#include "SPlayer.h"
#include "../Common/Networking/GameMessage.h"
#include "Achievements/AchievementModifierEvaluationLogic.h"
#include "../Common/TagMaps/Tags.h"
#include "../Common/TagMaps/ActorTags.h"
#include "../Common/TagMaps/AnimTags.h"
#include "../Common/TagMaps/AnimTagsSwig.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/Definitions/GlobalDefinition.h"
#include "../Common/WorldPlace.h"
#include "AI/AIStates.h"
#include "LuaScript/ScriptEvent.h"
#include "../Sigma/UnicodeText.h"
#include "../Common/Achievements/AchievementDefines.h"
#include "SGameConstants.h"
#include "../Common/NPC.h"
#include "../Common/Hireling.h"
#include "../Common/HirelingSwig.h"
#include <float.h>
#include "LuaScript/LuaScriptUtil.h"
#include "../../Contrib/Contrib/lua/src/lua.h"
#include "../Common/SNO/SNOFiles.h"
lua_State * LuaScriptGetLuaStateWithSNO(SNO snoScript);  // X/Server/LuaScript/LuaScript.cpp:3353
int32 LuaSNOScriptCall(SNO snoScript, lua_State * L, int32 nArgumentsCnt, int32 nReturnValCnt, LPCSTR szFuncName);  // X/Server/LuaScript/LuaScriptUtil.cpp:1084
BOOL LuaCheckReturnValue(SNO snoScript, lua_State * L, int32 nExpectedLuaType, LPCSTR szFuncName);  // X/Server/LuaScript/LuaScriptUtil.cpp:1130
extern LPCSTR gszConvConditionalRootFuncName;  // X/Common/Definitions/ConversationDefinition.cpp:71






// X/Server/SPlayer.h
void SPlayerMarkConversationAsHeard(Player * ptPlayer, SNO snoConversation);  // SPlayer.cpp:5816

enum ConversationAdvancedReturn {
    CONV_LINE = 0,
    CONV_DONE = 1,
};

enum PlayerParticipationStatus {
    PARTICIPATION_STATUS_NOT_INCLUDED = 0,
    PARTICIPATION_STATUS_OPTED_IN = 1,
    PARTICIPATION_STATUS_OPTED_OUT = 2,
    PARTICIPATION_STATUS_FINISHED = 3,
    NUM_PARTICIPATION_STATUS = 4,
};

struct ActiveConversation {  // size 0x3d8
    BOOL fMarkedForRemoval;  // 0x00
    SNO snoConversation;  // 0x04
    ConversationType eConvType;  // 0x08
    int32 nPriority;  // 0x0c
    ACDID idACDParticipants[9];  // 0x10
    uint32 dwAnimIdleTags[9];  // 0x34
    BOOL bZoom;  // 0x58
    BOOL bPlayerInitiated;  // 0x5c
    int32 nSpeakingPlayerIndex;  // 0x60
    SNO snoPlayerActor;  // 0x64
    PlayerClass ePlayerClass;  // 0x68
    ActorGender ePlayerGender;  // 0x6c
    PlayerParticipationStatus eStatus[4];  // 0x70
    uint32 dwLastAutoAdvanceTime;  // 0x80
    uint32 dwEarliestNextAutoAdvanceTime;  // 0x84
    uint32 dwNextAdvanceTime;  // 0x88
    ConversationTreeIter tTreeIter;  // 0x90
    int32 nLastLineGUIDSentToClient;  // 0x3c8
    uint32 dwLastLineUID;  // 0x3cc
    ACDID idACDLastSpeaker;  // 0x3d0
    BOOL bAnimChanged;  // 0x3d4
};
static_assert(sizeof(ActiveConversation) == 0x3d8, "ActiveConversation");

typedef XGrowableList<ActiveConversation, ActiveConversation &> ConvList;

struct ConversationMgrGlobals {  // size 0x80
    UpdateData tUpdateData;  // 0x00
    ConvList tConversationList;  // 0x20
    int32 nLineGUID;  // 0x78
};
static_assert(sizeof(ConversationMgrGlobals) == 0x80, "ConversationMgrGlobals");

static void sCancelFloatConversation(ActiveConversation & tConv, int32 nPlayerIndex, BOOL fStopImmediately);
static void sAdvanceFloatConversation(ActiveConversation & tConv, const BOOL fManuallyAdvanced);
static BOOL sStartConversation(int32 nPlayerTalkingIndex, const SNO snoConversation, uint32 dwConvConditions, uint32 dwConvCheckConditions, const ACDID idACDTalkingTo, const ACDID idACDAlt1, const ACDID idACDAlt2, const ACDID idACDAlt3, const ACDID idACDAlt4, const int32 nPriority, const BOOL bForceInterrupt, const BOOL bPlayerInitiated);
BOOL sACDIsInActiveConversation(const ACDID idACD, BOOL bCheckUsable, const Player * ptPlayer);
static void sActiveConversationDispose(ActiveConversation & tActiveConv);





#include "../Common/Definitions/ConversationDefinition.h"
// X/Common/Definitions/ConversationDefinition.h (subset; src's header does not carry these yet,
// see build/dwarf/headers/X/Common/Definitions/ConversationDefinition.h)
enum {
    CONV_SPEAKER_INVALID = -1,
    CONV_SPEAKER_PLAYER = 0,
    CONV_SPEAKER_PRIMARY_NPC = 1,
    CONV_SPEAKER_ALT_NPC1 = 2,
    CONV_SPEAKER_ALT_NPC2 = 3,
    CONV_SPEAKER_ALT_NPC3 = 4,
    CONV_SPEAKER_ALT_NPC4 = 5,
    CONV_SPEAKER_FOLLOWER_START = 6,
    CONV_SPEAKER_FOLLOWER_TEMPLAR = 6,
    CONV_SPEAKER_FOLLOWER_SCOUNDREL = 7,
    CONV_SPEAKER_FOLLOWER_ENCHANTRESS = 8,
    CONV_SPEAKER_FOLLOWER_END = 9,
    NUM_CONV_SPEAKERS = 9,
};
enum {
    CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT = 0,
    CTU_ZOOMS_FOR_TARGET_PLAYER_BIT = 1,
    CTU_ALLOWS_MANUAL_ADVANCE_BIT = 2,
    CTU_CANCELS_BEYOND_CLOSE_RANGE_BIT = 3,
    CTU_CANCELS_BEYOND_LONG_RANGE_BIT = 4,
    CTU_IS_GLOBAL_BIT = 5,
    CTU_DOES_NOT_CANCEL_OTHERS_BIT = 6,
    CTU_INTERRUPTS_EQUAL_TYPE_BIT = 7,
    CTU_STARTS_IMMEDIATELY_ON_NPC_OPERATE_BIT = 8,
    CTU_ALWAYS_PLAY_IMMEDIATELY_BIT = 9,
    CTU_SHOWN_IN_TALK_MENU_BIT = 10,
    CTU_NPC_UNUSABLE_DURING_CONVERSATION_BIT = 11,
    CTU_TRIGGER_FOLLOWER_CONVERSATION_COOLDOWN_BIT = 12,
    CTU_DISABLES_PLAYER_INTERACTION_BIT = 13,
    CTU_YIELDS_TO_PLAYER_INITIATED_BIT = 14,
    CTU_NO_AUDIO_ROLLOFF_BIT = 15,
    CTU_NEVER_SILENCE_AUDIO_BIT = 16,
    CTU_NEVER_ANIMATES_BIT = 17,
};
enum {
    CF_NO_VOICEOVER_BIT = 8,
    CF_OPERATE_SPEAKER_ON_BEGIN_BIT = 11,
    CF_READY_TO_RECORD_BIT = 12,
    CF_OPERATE_SPEAKER_ON_END_BIT = 13,
    CF_NEVER_REPEAT_BIT = 14,
    CF_CANCEL_FOR_COMBAT_BIT = 15,
    CF_READY_FOR_REVIEW_BIT = 19,
    CF_PERSIST_THROUGH_SPEAKER_DEATH = 20,
    CF_ALWAYS_IMPORTANT = 21,
    CF_CONSOLE_HIDE_CLOSE_BUTTON = 22,
    CF_SHOW_ALL_LINES_IN_RECORDING_SCRIPT = 23,
};
// ConversationDefinition itself comes from ../Common/ConversationTreeIterator.h (src keeps it there for now).
























template <> inline const ConversationDefinition * DefinitionLockR<ConversationDefinition>(SNO sno) {  // ConversationDefinition.h:462
    return (const ConversationDefinition *)gptGroupDefConversation->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<ConversationDefinition>(const ConversationDefinition * pdef) {  // ConversationDefinition.h:463
    void * lpDef = (void *)pdef;
    gptGroupDefConversation->UnlockR(lpDef);
}
typedef ScopedDefinitionLockR<ConversationDefinition> ScopedConversationDefinitionLockR;

// X/Common/ConversationCommon.h (not adopted in src; copy of src/X/Common/ConversationCommon.cpp:22-80)
struct PlayLineParams {  // size 0x4c
    SNO snoConversation; int32 nSpeakingPlayerIndex; BOOL bZoom; BOOL bFirstLine; BOOL bHostile; BOOL bPlayerInitiated; uint32 dwLineUID; ConversationSpeaker eLineSpeaker; ActorGender eLineGender; PlayerClass eLineClass; ConversationSpeaker eSpeakerTarget; SNO snoSpeakerActor; ActorGender ePlayerGender; PlayerClass ePlayerClass; uint32 dwLineFlags; uint32 dwLineAnimTag; uint32 dwLineDisplayTime; int32 nLineGUID; int32 nPriority;
};
struct PlayConvLineMessage {  // size 0x80
    RequiredMessageHeader tHeader; ACDNetworkName annSpeaker; ACDNetworkName annParticipants[9]; PlayLineParams tParams; uint32 dwTimeToDisplay;
};
struct StopConvLineMessage {  // size 0x10
    RequiredMessageHeader tHeader; int32 nConvLineGUID; BOOL fStoppedByUser;
};
struct EndConversationMessage {  // size 0x14
    RequiredMessageHeader tHeader; SNO snoConversation; ACDNetworkName annSpeaker; int8 nPlayerTalkingIndex;
};
static_assert(sizeof(PlayConvLineMessage) == 0x80 && sizeof(StopConvLineMessage) == 0x10 && sizeof(EndConversationMessage) == 0x14, "conv messages");
extern const uint32 gcdwConvAutoAdvanceTicks;  // ConversationCommon.h; defined (external, DWARF) at X/Common/ConversationCommon.cpp:19; loaded through the GOT at 1168
// X/Common/Definitions/ConversationDefinition.h
struct ConvLocaleDisplayTimes {  // size 0x38
    uint32 dwDisplayTime[14];
};
struct CompressedConvLocaleDisplayTimes {  // size 0x4
    uint32 eLocale : 5; uint32 eClass : 3; uint32 eGender : 2; uint32 dwValue : 22;
};
enum { CONV_TYPE_FOLLOWER_SOUNDSET = 0, CONV_TYPE_PLAYER_EMOTE = 1, CONV_TYPE_AMBIENT_FLOAT = 2, CONV_TYPE_FOLLOWER_BANTER = 3, CONV_TYPE_FOLLOWER_CALLOUT = 4, CONV_TYPE_PLAYER_CALLOUT = 5, CONV_TYPE_GLOBAL_CHATTER = 6, CONV_TYPE_GLOBAL_FLOAT = 7, CONV_TYPE_LORE = 8, CONV_TYPE_AMBIENT_GOSSIP = 9, CONV_TYPE_TALK_MENU = 10, CONV_TYPE_QUEST_STANDARD = 11, CONV_TYPE_QUEST_FLOAT = 12, CONV_TYPE_QUEST_EVENT = 13, NUM_CONVERSATION_TYPES = 14 };
enum { CONV_VAR_SET_OP_INVALID = -1, CONV_VAR_SET_OP_INC = 0, CONV_VAR_SET_OP_DEC = 1, CONV_VAR_SET_OP_SET = 2, CONV_VAR_SET_OP_COUNT = 3 };
typedef int32 ConversationPriority;
ConversationPriority ConversationTypeGetPriority(const ConversationType eConvType);  // ConversationDefinition.cpp:1449
BOOL ConversationPlayerNearNPC(ActorCommonData & tACDPlayer, ActorCommonData & tACDNpc);  // ConversationDefinition.cpp:1788
SNO ConversationSpeakerToSNO(const ConversationDefinition * ptConvDef, ConversationSpeaker eSpeaker);  // ConversationDefinition.cpp:1251
HirelingClass ConversationSpeakerToHirelingClass(ConversationSpeaker eSpeaker);  // ConversationDefinition.cpp:1276
SNO ConversationGetEndGameConversation(uint32 uContentFlags);  // X/Common/ConversationCommon.cpp:89
// other owners (declared locally; DWARF file:line given)
BOOL SPlayerInScriptedSequence(const Player & tPlayer);  // X/Server/SPlayer.cpp:10849
BOOL SPlayerHasHeardConversation(const Player * ptPlayer, const SNO snoConv);  // X/Server/SPlayer.cpp:5830
void SPlayerSaveConversation(const PlayerList & listDestPlayers, const SNO snoConv, const BOOL bPlayed);  // X/Server/SPlayer.cpp:7378
uint32 AnimPlayerGetIdleAnimation(ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp:1323
uint32 AnimPlayerGetCurrentAnimTag(ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp:1856
BOOL NPCIsOperableVendorOrCrafter(const ActorCommonData & tACDNpc, const ActorCommonData & tACDPlayer);  // X/Common/NPC.cpp:137
void NPCUpdateConversationIconAndInteraction(ActorCommonData & tACDNPC, const Player & tPlayer);  // X/Server/SNPC.cpp:438
void NPCOperate(ActorCommonData & tACDNPC, ActorCommonData & tACDPlayer, const NPCMenu ePreferredMenu, const BOOL fFromPower, const BOOL fSkipImportantConversations, const BOOL fForceOperateVendor);  // X/Server/SNPC.cpp:650
void AISetCalloutCooldown(ActorCommonData & tACD, SNO snoConversation, float flCooldown, float flGlobalCooldown);  // X/Server/AI/ActorAI.cpp:2873
void AISetBanterCooldown(ActorCommonData & tACD, SNO snoConversation, float flCooldown, float flGlobalCooldown);  // X/Server/AI/ActorAI.cpp
void SQuestsOnConversationEnded(ACDID idACDPlayer, SNO snoConversation);  // X/Server/Quests/SQuests.cpp:4671
void SGameEvaluateActCompletedAchievement(Act eActCompleted);  // X/Server/SGame.cpp:7122
namespace BossEncounterManager { void TriggerEncounter(SNO snoEncounter, Player * ptPlayer, SWorldID idSWorldOrigin, BOOL bForceImmediateStart); }  // X/Server/System/BossEncounter.cpp:1087
ACDID SHirelingGetNearby(ACDID idACDOrigin, float flRangeSq, HirelingClass eHirelingClass);  // X/Server/SHireling.cpp:1027
int32 SGameStatsEncodeSNODataToID(SNO sno, int32 nData);  // X/Server/SGameStats.cpp:1774
// forward declarations of this file's functions (defined further down)
static void sGetPlayersOptedIntoConversation(const ActiveConversation & tConv, PlayerList & listDestPlayers, const GameConnectionID idGameConnection);
static void sFinishConversation(ActiveConversation & tConv);
static void sFinishConversationForPlayer(ActiveConversation & tConv, const PlayerList & listDestPlayers);
static void sReturnLastSpeakerToNeutralAnim(ActiveConversation & tConv);
static void sGetPlayersForConversation(const ConversationDefinition * pdefConv, Player * ptPlayer, PlayerList & listDestPlayers);
static void sClearConversationTalkTarget(ActorCommonData & tACDSpeaker);
static BOOL sIsInRangeOfConversation(const GameConnectionID idGameConnection, const ActiveConversation & tConv);
static BOOL sIsInRangeOfConversation(const Player & tPlayer, const ActiveConversation & tConv);
static void sAddPlayerToActiveConversation(const Player * ptPlayer, ActiveConversation & tConv);
static BOOL sAnyParticipantIsInConversation(const ActiveConversation & tConv, const ACDID * idACDParticipants);
static ConversationAdvancedReturn sActiveConversationAdvance(ActiveConversation & tActiveConv, PlayLineParams & tParams);
static void sActiveConversationInitialize(ActiveConversation & tConv, const SNO snoConversation, Player * ptStartingPlayer, const ACDID * idACDParticipants, const uint32 dwConvConditions, const uint32 dwConvCheckConditions, const int32 nPriority, const BOOL bPlayerInitiated);
void sGetParticipantsForConversation(const ConversationDefinition * pdefConv, const ActorCommonData * ptACDPlayer, ACDID * idACDParticipants, BOOL bSuppressWarning);
const float gcflConvHearingRadius = 60;  // DWARF line 130
const float gcflConvMaxDistance = 90;  // DWARF line 131
const uint32 gcdwConvAutoAdvancePaddingTicks = 300;  // DWARF line 134
namespace ConversationMgr { BOOL StartDebugConversation(const int32 nPlayerTalkingIndex, const SNO snoConversation, const uint32 dwConvConditions, const uint32 dwConvCheckConditions, const ACDID idACDTalkingTo, const ACDID idACDAlt1, const ACDID idACDAlt2, const ACDID idACDAlt3, const ACDID idACDAlt4, const int32 nPriority, const BOOL bForceInterrupt); }  // ConversationMgr.h (src header lacks it)




void ConversationMgr::InitializeForGame()
{

    SGame & tSGame = SGameManager::GameGet();

    ConversationMgrGlobals * ptConvGlobals = (ConversationMgrGlobals *)SigmaMemoryNew(sizeof(ConversationMgrGlobals), 4, tSGame.ptMemoryInterface, TRUE);
    tSGame.ptConversationMgrGlobals = ptConvGlobals;

    UpdateFunctionInitialize(UPDATE_CONVERSATIONS, DATAID_INVALID, ptConvGlobals->tUpdateData);
    ServerQueueUpdateFunction(1, ptConvGlobals->tUpdateData);

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    ptConvGlobals->tConversationList.Initialize(ptMemoryInterface);
    ptConvGlobals->nLineGUID = 69;
}






static void sSConversationPurgeUnusedScripts()
{
    {




        Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.tConversationDefPurgeMutex);

        int32 nCount = sgtAppGlobals.ptConversationDefsPurged->GetArraySize();
        for (int32 i = 0; i < nCount; i++)
        {
            SNO snoScript = (*sgtAppGlobals.ptConversationDefsPurged)[i];
            ScriptDefinitionDisposeScript(snoScript);
        }

        sgtAppGlobals.ptConversationDefsPurged->EmptyArray();
    }
}



void ConversationMgr::DisposeForGame()
{

    SGame & tSGame = SGameManager::GameGet();

    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;

    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation & tConv = ptConvGlobals->tConversationList.GetNext(pos);
        sActiveConversationDispose(tConv);
    }
    ptConvGlobals->tConversationList.RemoveAll();
    ptConvGlobals->tConversationList.Dispose();





    sSConversationPurgeUnusedScripts();

    XFREE(tSGame.ptConversationMgrGlobals, tSGame.ptMemoryInterface);
    tSGame.ptConversationMgrGlobals = NULL;
}

















uint32 sACDGetCurrentConversationIdle(const ACDID idACD)
{

    SGame & tSGame = SGameManager::GameGet();

    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;
    if (ptConvGlobals == NULL)
    {
        return 0;
    }


    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation & tConv = ptConvGlobals->tConversationList.GetNext(pos);
        if (tConv.fMarkedForRemoval)
        {
            continue;
        }


        for (int32 i = 0; i < 9; i++)
        {
            if (tConv.idACDParticipants[i] == idACD && tConv.dwAnimIdleTags[i] != 0)
            {
                return tConv.dwAnimIdleTags[i];
            }
        }
    }

    return 0;
}


inline uint32 sACDGetCurrentIdleAnim(  // inline: DWARF has only inlined instances and no concrete DIE for this external function
    ActorCommonData& tACD)
{


    uint32 dwAnimTag = sACDGetCurrentConversationIdle(tACD.id);
    if (dwAnimTag != 0)
    {
        return dwAnimTag;
    }


    return AnimPlayerGetIdleAnimation(tACD);
}



static void sPlayAnimOnACDID(SNO snoConversation, ActorCommonData & tACDSpeaker, const uint32 dwAnimTag, BOOL fSetIdle)
{




    if (fSetIdle)
    {
        AnimPlayerSetIdleAnimation(tACDSpeaker, dwAnimTag);
    }
    else
    {
        SNO snoAnim = tACDSpeaker.GetSpecificAnim(dwAnimTag);
        if (snoAnim != SNO_NONE)
        {









            ScopedAnimationDefinitionLockR pdefAnim(snoAnim);

            if (pdefAnim->ePlaybackMode == 2)
            {
                AnimPlayerPlayAnimation(tACDSpeaker, dwAnimTag, ANIM_PLAY_FOREVER, TRUE, ANIM_REASON_FLOAT_CONVERSATION, 0.0f, FALSE, 1.0f);
            }
            else
            {
                AnimPlayerPlayAnimation(tACDSpeaker, dwAnimTag, ANIM_USE_ANIM_LENGTH, TRUE, ANIM_REASON_FLOAT_CONVERSATION, 0.0f, FALSE, 1.0f);
            }
        }
    }
}







static uint32 sGetNeutralAnimTag(
    ActiveConversation & tConv,
    ActorCommonData & tACDLastSpeaker)
{
    uint32 dwNeutralAnimTag = TAG_ACTOR_ANIMATION_CONVERSATION_NEUTRAL;

    if (AnimPlayerAnimExists(tACDLastSpeaker, dwNeutralAnimTag))
    {
        return dwNeutralAnimTag;
    }


    for (int32 i = 0; i < 9; i++)
    {
        if (tConv.idACDParticipants[i] == tACDLastSpeaker.id && tConv.dwAnimIdleTags[i] != 0)
            return tConv.dwAnimIdleTags[i];
    }



    return TAG_ACTOR_ANIMATION_NEUTRAL;
}






static BOOL sActiveConversationIsFinished(
    const ActiveConversation & tConv)
{

    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        if (tConv.eStatus[i] == PARTICIPATION_STATUS_OPTED_IN)
        {
            return FALSE;
        }
    }

    return TRUE;
}



static void sTellClientToStopCurrentLine(
    ActiveConversation & tConv,
    const PlayerList & listDestPlayers,
    const BOOL fStoppedByUser)
{
    if (tConv.nLastLineGUIDSentToClient == 0)
    {
        return;
    }


    StopConvLineMessage tMessage;
    tMessage.nConvLineGUID = tConv.nLastLineGUIDSentToClient;
    tMessage.fStoppedByUser = fStoppedByUser;
    MessageSendToClient(listDestPlayers, SMSG_STOP_CONV_LINE, &tMessage, sizeof(tMessage));
}





static void sTellClientConversationHasEnded(
    ActiveConversation & tConv,
    const PlayerList & listDestPlayers)
{
    if (tConv.nLastLineGUIDSentToClient == 0)
    {
        return;
    }


    EndConversationMessage tMessage;
    tMessage.snoConversation = tConv.snoConversation;
    tMessage.annSpeaker = ANNFromACD(tConv.idACDLastSpeaker);
    tMessage.nPlayerTalkingIndex = static_cast<int8>(tConv.nSpeakingPlayerIndex);

    MessageSendToClient(listDestPlayers, SMSG_END_CONV, &tMessage, sizeof(tMessage));
}




static void sUpdateConversationForPlayer(
    ActiveConversation & tConv,
    int32 nPlayerIndex,
    const PlayerList & listDestPlayers,
    PlayerParticipationStatus eStatus)
{

    if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYERS)
    {
        for (int32 i = 0; i < MAX_PLAYERS; i++)
        {
            tConv.eStatus[i] = eStatus;
        }
    }
    else
    {
        POS pos = listDestPlayers.GetHeadPosition();
        while (pos != NULL)
        {
            Player * ptPlayer = listDestPlayers.GetNext(pos);
            tConv.eStatus[ptPlayer->nPlayerIndex] = eStatus;
        }
    }
}

static void sAddConversationParticipants(
    ActiveConversation & tConv)
{

    if (tConv.fMarkedForRemoval)
    {
        return;
    }


    uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);
    if ((dwUsageFlags & (1 << CTU_IS_GLOBAL_BIT)) == 0)
    {
        return;
    }

    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        if (tConv.eStatus[i] == PARTICIPATION_STATUS_NOT_INCLUDED)
        {
            Player * ptPlayer = PlayerGetByPlayerIndex(i);


            if (ptPlayer && sIsInRangeOfConversation(ptPlayer->idGameConnection, tConv))
            {



                sAddPlayerToActiveConversation(ptPlayer, tConv);
            }
        }
    }
}




static void sRemoveConversationParticipants(
    ActiveConversation & tConv)
{

    if (tConv.fMarkedForRemoval)
    {
        return;
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    sGetPlayersOptedIntoConversation(tConv, listDestPlayers, -1);

    POS pos = listDestPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);

        if (ptPlayer && !sIsInRangeOfConversation(ptPlayer->idGameConnection, tConv))
        {



            sCancelFloatConversation(tConv, ptPlayer->nPlayerIndex, FALSE);
        }
    }
}




static void sFinishConversationForPlayer(
    ActiveConversation & tConv,
    const PlayerList & listDestPlayers)
{
    POS pos = listDestPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACD)
        {

            ptACD->AttributesSetInt(ATTRIBUTE_IN_CONVERSATION, 0);


            for (int32 i = 0; i < 9; i++)
            {
                ActorCommonData * ptACDNPC = ACDTryToGet(tConv.idACDParticipants[i]);
                if (ptACDNPC)
                {
                    NPCUpdateConversationIconAndInteraction(*ptACDNPC, *ptPlayer);
                }
            }


            uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);
            if (dwUsageFlags & (1 << CTU_TRIGGER_FOLLOWER_CONVERSATION_COOLDOWN_BIT))
            {
                ScopedGlobalDefinitionLockR pdefGlobal;

                float flCalloutDelayAfterConv = (float)pdefGlobal->ptServerData->nCalloutDelayAfterConv;


                AISetCalloutCooldown(*ptACD, SNO_NONE, 0.0f, flCalloutDelayAfterConv);
                AISetBanterCooldown(*ptACD, SNO_NONE, 0.0f, flCalloutDelayAfterConv);


                ACDID idACDHireling = PlayerGetHireling(ptACD->id);
                ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
                if (ptACDHireling)
                {
                    AISetCalloutCooldown(*ptACDHireling, SNO_NONE, 0.0f, flCalloutDelayAfterConv);
                }
            }
        }
    }
}


static void sEvaluatePlayerConverseWithNPCAchievement(
    const ActorCommonData& tACDNPC,
    const Player& tPlayer)
{




    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.SetACD(&tACDNPC);

        tEvaluationData.EvaluateEvent(
            &tPlayer,
            Achievement::ACH_EVENT_CONVERSE_WITH_NPC,
            tACDNPC.GetActorSNO(),
            1, (uint32)sub_71007b5e70());
    }
}


static void sEvaluateConversationLineHeardAchievement(
    const PlayerList & listDestPlayers,
    const PlayLineParams & tPlayLineParams,
    const ActorCommonData & tACDSpeaker)
{




    {

        POS pos = listDestPlayers.GetHeadPosition();
        while (pos != NULL)
        {
            Achievement::EvaluationData tEvaluationData;

            Player * ptPlayer = listDestPlayers.GetNext(pos);
            if (ptPlayer)
            {
                tEvaluationData.SetPlayer(ptPlayer);
                tEvaluationData.SetACD(&tACDSpeaker);

                tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_PLAYER_HEARD_CONVERSATION_LINE,


                    SGameStatsEncodeSNODataToID(tPlayLineParams.snoConversation, tPlayLineParams.dwLineUID),
                    1, (uint32)sub_71007b5e70());
            }
        }
    }
}


static void sEvaluatePlayerConversationAchievement(
    SNO snoConversation,
    Achievement::EventID eventID,
    const Player& tPlayer)
{




    if (!tPlayer.tAccount.tHandle.IsInvalid())
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);

        tEvaluationData.EvaluateEvent(&tPlayer, eventID, snoConversation, 1, (uint32)sub_71007b5e70());




    }
}






static void sFinishConversation(
    ActiveConversation & tConv)
{

    sReturnLastSpeakerToNeutralAnim(tConv);

    Player * ptPlayer = PlayerGetByPlayerIndex(tConv.nSpeakingPlayerIndex);
    ACDID idACDPlayerSpeaking = ptPlayer ? ptPlayer->idPlayerACD : ACDID_INVALID;

    ScopedConversationDefinitionLockR pdefConv(tConv.snoConversation);
    PlayerList listPlayers(PlayerListGetElementPool());
    sGetPlayersForConversation(pdefConv, ptPlayer, listPlayers);


    for (int32 i = 0; i < 3; i++)
    {
        if (pdefConv->snoConvUnlocks[i] != SNO_NONE)
        {
            SPlayerSaveConversation(listPlayers, pdefConv->snoConvUnlocks[i], FALSE);
        }
    }


    for (int32 i = 0; i < 9; i++)
    {
        ActorCommonData * ptACD = ACDTryToGet(tConv.idACDParticipants[i]);
        if (ptACD != NULL)
        {

            if (ptACD->IsAlive() &&
                tConv.dwAnimIdleTags[i] != 0)
            {



                if (sACDGetCurrentConversationIdle(ptACD->id) == 0)
                {
                    sPlayAnimOnACDID(tConv.snoConversation, *ptACD, tConv.dwAnimIdleTags[i], TRUE);
                }
            }


            if (ConversationMgr::ACDIsInActiveConversation(ptACD->id, NULL) == FALSE)
            {
                sClearConversationTalkTarget(*ptACD);
            }
        }
    }



    SQuestsOnConversationEnded(idACDPlayerSpeaking, tConv.snoConversation);
    ScriptEventOnConversationFinished(idACDPlayerSpeaking, tConv.idACDParticipants[CONV_SPEAKER_PRIMARY_NPC], tConv.snoConversation);


    POS pos = listPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        Player * ptPlayerToEvaluate = listPlayers.GetNext(pos);
        if (ptPlayerToEvaluate != NULL)
        {
            sEvaluatePlayerConversationAchievement(tConv.snoConversation, Achievement::ACH_EVENT_COMPLETE_CONVERSATION, *ptPlayerToEvaluate);
        }
    }


    GameCommonData & tGCD = GameGetActiveGameCommonData();
    Act eCurrentAct = tGCD.tGameSyncedData.eAct;
    SGame & tSGame = SGameManager::GameGet();
    if (tConv.snoConversation == ConversationGetEndGameConversation(8))
    {
        tSGame.ptSGameGlobals->dwFlags |= (1 << SGAMEGLOBALS_END_GAME_QUEUED_VANILLA);
        SGameEvaluateActCompletedAchievement(eCurrentAct);
    }
    else if (tConv.snoConversation == ConversationGetEndGameConversation(0x10))
    {
        tSGame.ptSGameGlobals->dwFlags |= (1 << SGAMEGLOBALS_END_GAME_QUEUED_X1);
        SGameEvaluateActCompletedAchievement(eCurrentAct);
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayerSpeaking);
    BossEncounterManager::TriggerEncounter(pdefConv->snoBossEncounter, ptPlayer, ptACDPlayer ? ptACDPlayer->GetSWorldID() : SWORLDID_INVALID, FALSE);



    if ((pdefConv->snoConvPiggyback != SNO_NONE && tConv.nSpeakingPlayerIndex != -1 &&


            ptACDPlayer && ConversationMgr::ConversationHasAllParticipants(pdefConv->snoConvPiggyback, ptACDPlayer, NULL)))
    {
        ConversationMgr::StartConversation(
                tConv.nSpeakingPlayerIndex,
                pdefConv->snoConvPiggyback,
                ACDID_INVALID,
                ACDID_INVALID,
                ACDID_INVALID,
                ACDID_INVALID,
                ACDID_INVALID,
                tConv.nPriority,
                FALSE,
                FALSE);
    }
}



static void sCancelFloatConversation(
    ActiveConversation & tConv,
    int32 nPlayerIndex,
    BOOL fStopImmediately)
{
    if (tConv.fMarkedForRemoval)
    {
        return;
    }


    if (tConv.nSpeakingPlayerIndex == nPlayerIndex) nPlayerIndex = -1;




    PlayerList listDestPlayers(PlayerListGetElementPool());
    if (nPlayerIndex == -1)
    {
        sGetPlayersOptedIntoConversation(tConv, listDestPlayers, -1);
    }
    else
    {

        Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
        if (ptPlayer == NULL)
        {
            return;
        }

        sGetPlayersOptedIntoConversation(tConv, listDestPlayers, ptPlayer->idGameConnection);
    }



    sTellClientToStopCurrentLine(tConv, listDestPlayers, fStopImmediately);
    sTellClientConversationHasEnded(tConv, listDestPlayers);
    sFinishConversationForPlayer(tConv, listDestPlayers);
    sUpdateConversationForPlayer(tConv, nPlayerIndex, listDestPlayers, PARTICIPATION_STATUS_OPTED_OUT);



    if (sActiveConversationIsFinished(tConv))
    {
        sActiveConversationDispose(tConv);

        sFinishConversation(tConv);
    }
}



static void sEndFloatConversation(
    ActiveConversation & tConv)
{
    if (tConv.fMarkedForRemoval)
    {
        return;
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    sGetPlayersOptedIntoConversation(tConv, listDestPlayers, -1);

    sTellClientToStopCurrentLine(tConv, listDestPlayers, FALSE);
    sTellClientConversationHasEnded(tConv, listDestPlayers);
    sFinishConversationForPlayer(tConv, listDestPlayers);
    sUpdateConversationForPlayer(tConv, -1, listDestPlayers, PARTICIPATION_STATUS_FINISHED);

    sActiveConversationDispose(tConv);

    sFinishConversation(tConv);
}




static void sReturnLastSpeakerToNeutralAnim(
    ActiveConversation & tConv)
{
    ActorCommonData * ptACDLastSpeaker = ACDTryToGet(tConv.idACDLastSpeaker);
    // inference: the NULL test has no row of its own (folded into ACDTryToGet's checks)
    if (ptACDLastSpeaker == NULL)
    {
        return;
    }

    if (!ptACDLastSpeaker->IsAlive())
    {
        return;
    }


    uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);
    if (dwUsageFlags & (1 << CTU_NEVER_ANIMATES_BIT))
    {
        return;
    }




    if (!tConv.bAnimChanged)
    {
        return;
    }
    tConv.bAnimChanged = FALSE;

    uint32 dwCurrentAnim = AnimPlayerGetCurrentAnimTag(*ptACDLastSpeaker);

    uint32 dwStartTag;
    uint32 dwEndTag;
    AnimTagGetStartAndEndTags(dwCurrentAnim, dwStartTag, dwEndTag);

    uint32 dwNeutralAnimTag = sGetNeutralAnimTag(tConv, *ptACDLastSpeaker);

    if (AnimPlayerAnimExists(*ptACDLastSpeaker, dwEndTag))
    {
        sPlayAnimOnACDID(tConv.snoConversation, *ptACDLastSpeaker, dwEndTag, FALSE);
        AnimPlayerSetIdleAnimation(*ptACDLastSpeaker, dwNeutralAnimTag);
    }
    else
    {
        AnimPlayerSetIdleAnimation(*ptACDLastSpeaker, dwNeutralAnimTag);
        AnimPlayerPlayIdle(*ptACDLastSpeaker);
    }
}



static void sSetConversationTalkTarget(
    ActiveConversation & tConv,
    ActorCommonData & tACDSpeaker,
    ConversationSpeaker eSpeakerTarget)
{
    ACDNetworkName idANNTarget = ANN_INVALID;
    ACDID idACDTarget = tConv.idACDParticipants[eSpeakerTarget];

    if (ACDIsPlayer(tACDSpeaker.id))
    {


        ActorCommonData * ptACDNPC = ACDTryToGet(idACDTarget);
        if (ptACDNPC)
        {
            idANNTarget = ANNFromACD(tACDSpeaker.id);
            ptACDNPC->AttributesSetInt(ATTRIBUTE_NPC_CONV_TARGET_ANN, idANNTarget);
        }
    }
    else
    {

        if (eSpeakerTarget == CONV_SPEAKER_PLAYER && idACDTarget == ACDID_INVALID)
        {
            Player * ptPlayer = PlayerGetByPlayerIndex(tConv.nSpeakingPlayerIndex);
            idACDTarget = (ptPlayer ? ptPlayer->idPlayerACD : ACDID_INVALID);
        }

        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget)
        {
            idANNTarget = ANNFromACD(ptACDTarget->id);
        }

        tACDSpeaker.AttributesSetInt(ATTRIBUTE_NPC_CONV_TARGET_ANN, idANNTarget);
    }
}



static void sClearConversationTalkTarget(
    ActorCommonData & tACDSpeaker)
{
    tACDSpeaker.AttributesSetInt(ATTRIBUTE_NPC_CONV_TARGET_ANN, ANN_INVALID);
}




static void sAdvanceFloatConversation(
    ActiveConversation & tConv,
    const BOOL fManuallyAdvanced)
{

    if (tConv.fMarkedForRemoval)
    {
        return;
    }




    PlayerList listDestPlayers(PlayerListGetElementPool());
    sGetPlayersOptedIntoConversation(tConv, listDestPlayers, -1);
    sTellClientToStopCurrentLine(tConv, listDestPlayers, fManuallyAdvanced);

    CRefString sNPCLabel;

    PlayLineParams tParams;
    ConversationAdvancedReturn eConvReturn = sActiveConversationAdvance(tConv, tParams);
    // inference: the CONV_LINE / else split has no row of its own
    if (eConvReturn == CONV_LINE)
    {


        uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);

        uint32 dwTimeToDisplay = 0;
        BOOL bAnimChanged = FALSE;

        ACDID idACDSpeaker = tConv.idACDParticipants[tParams.eLineSpeaker];
        ActorCommonData * ptACDSpeaker = ACDTryToGet(idACDSpeaker);
        if (ptACDSpeaker || tParams.eLineSpeaker == CONV_SPEAKER_PLAYER)
        {
            dwTimeToDisplay = tParams.dwLineDisplayTime;

            tParams.snoSpeakerActor = (tParams.eLineSpeaker == CONV_SPEAKER_PLAYER ? tConv.snoPlayerActor :
                (ptACDSpeaker ? ptACDSpeaker->snoActor : SNO_NONE));

            PlayConvLineMessage tPlayLineMessage;
            tPlayLineMessage.annSpeaker = ANNFromACD(idACDSpeaker);
            for (int32 i = 0; i < 9; i++)
            {
                tPlayLineMessage.annParticipants[i] = ANNFromACD(tConv.idACDParticipants[i]);
            }
            tPlayLineMessage.tParams = tParams;
            tPlayLineMessage.dwTimeToDisplay = dwTimeToDisplay;
            MessageSendToClient(listDestPlayers, SMSG_PLAY_CONV_LINE, &tPlayLineMessage, sizeof(tPlayLineMessage));

            SGame & tSGame = SGameManager::GameGet();
            if (tParams.dwLineFlags & (1 << CTNF_RANDOM_PLAYS_ONLY_ONCE))
            {
                ConversationRandomLineKey tKey; tKey.Set(tConv.snoConversation, tParams.dwLineUID);  // inference: DWARF uses a (SNO, uint32) constructor (SGame.h:242) that src SGame.h lacks
                tSGame.ptSGameGlobals->tConversationRandomLineMap.SetAt_Tracked(tKey, TRUE);
            }


            tConv.nLastLineGUIDSentToClient = tParams.nLineGUID;

            if (ptACDSpeaker)
            {


                if (ptACDSpeaker->IsAlive() && !(dwUsageFlags & (1 << CTU_NEVER_ANIMATES_BIT)) && !(tParams.dwLineFlags & (1 << CTNF_DONT_ANIMATE_BIT)))
                {
                    WeaponClass eWeaponClass = ptACDSpeaker->GetWeaponClassForAnim(NULL);
                    uint32 dwAnimTag = TAG_ACTOR_ANIMATION_TALK2;

                    if (ActorGetAnim(tParams.snoSpeakerActor, eWeaponClass, dwAnimTag) == SNO_NONE)
                    {
                        dwAnimTag = TAG_ACTOR_ANIMATION_TALK1;

                        if (ActorGetAnim(tParams.snoSpeakerActor, eWeaponClass, dwAnimTag) == SNO_NONE)
                            dwAnimTag = TAG_ACTOR_ANIMATION_INVALID;  // inference: sentinel, see notes
                    }


                    if (dwAnimTag != TAG_ACTOR_ANIMATION_INVALID)
                    {
                        sPlayAnimOnACDID(tConv.snoConversation, *ptACDSpeaker, dwAnimTag, TRUE);
                        bAnimChanged = TRUE;
                    }

                    uint32 dwGestureAnim = tParams.dwLineAnimTag;
                    if (dwGestureAnim != TAG_ACTOR_ANIMATION_INVALID)
                    {
                        sPlayAnimOnACDID(tConv.snoConversation, *ptACDSpeaker, dwGestureAnim, FALSE);
                        bAnimChanged = TRUE;
                    }
                    else
                    {

                        AnimPlayerPlayIdle(*ptACDSpeaker);
                    }
                }


                sSetConversationTalkTarget(tConv, *ptACDSpeaker, tParams.eSpeakerTarget);
            }

            sEvaluateConversationLineHeardAchievement(listDestPlayers, tParams, *ptACDSpeaker);
        }






        if (tConv.idACDLastSpeaker != idACDSpeaker)
        {
            sReturnLastSpeakerToNeutralAnim(tConv);
        }

        tConv.idACDLastSpeaker = idACDSpeaker;
        tConv.bAnimChanged = tConv.bAnimChanged || bAnimChanged;



        uint32 dwNow = GameGetActiveGameCommonData().dwServerTime;
        tConv.dwEarliestNextAutoAdvanceTime = dwNow + dwTimeToDisplay + gcdwConvAutoAdvanceTicks;







        tConv.dwNextAdvanceTime = tConv.dwEarliestNextAutoAdvanceTime + gcdwConvAutoAdvancePaddingTicks;
    }
    else
    {

        sEndFloatConversation(tConv);
    }
}



static ACDID sACDFindNearestActor(
    const SNO snoActor,
    const WorldPlace & place)
{

    ACDIterator tIt;
    ACDID idBestACD = ACDID_INVALID;
    float flBestDist = FLT_MAX;

    while (ActorCommonDataIterate(tIt))
    {
        const ActorCommonData* ptACD = tIt.ptElement;
        if ( ptACD->snoActor == snoActor )
        {
            const WorldPlace& placeCompare = ptACD->GetPlace();
            if (placeCompare.idSWorld == place.idSWorld)
            {
                float flDist = WorldPlaceDistanceSquared(place, placeCompare);
                if (flDist < flBestDist)
                {
                    flBestDist = flDist;
                    idBestACD = ptACD->id;
                }
            }
        }
    }

    return idBestACD;
}



static ACDID sFindACDFromActorSNO(
    const SNO snoConversation,
    const WorldPlace & place,
    const SNO snoActor,
    BOOL bSuppressWarning)
{
    ACDID idACD = ACDID_INVALID;

    if ( snoActor != SNO_NONE )
    {
        idACD = sACDFindNearestActor(snoActor, place);

        // inference: the original warns here (body compiled out); the test keeps bSuppressWarning live for dead-argument elimination
        if (idACD == ACDID_INVALID && !bSuppressWarning)
        {
        }
    }

    return idACD;
}



static BOOL sACDIsInConversation(const ActiveConversation & tConv, const ACDID idACD);
static BOOL sAnyParticipantIsInConversation(
    const ActiveConversation & tConv,
    const ACDID * idACDParticipants)
{
    if (tConv.fMarkedForRemoval)
    {
        return FALSE;
    }


    for (int32 i = 0; i < 9; i++)
    {
        if (idACDParticipants[i] != ACDID_INVALID &&
            sACDIsInConversation(tConv, idACDParticipants[i]))
        {
            return TRUE;
        }
    }

    return FALSE;
}



static ACDID sGetClosestPlayerForConversation(
    ConversationType eConvType,
    ACDID idACDNpc)
{
    ActorCommonData* ptACDNpc = ACDTryToGet(idACDNpc);

    if (ptACDNpc == NULL)
    {
        return ACDID_INVALID;
    }
    const WorldPlace& tPlace = ptACDNpc->GetPlace();
    Player* ptPlayerClosest = NULL;
    WorldDistance wdPlayerClosestDistSq = 0.0f;


    for (Player* ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL;

        ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData& tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        WorldPlace tPlacePlayer = tACDPlayer.GetPlace();


        if (WorldPlaceInSameWorld(tPlace, tPlacePlayer) == FALSE)
        {
            continue;
        }

        WorldDistance wdDistSq = WorldPlaceDistanceSquared(tPlace, tPlacePlayer);


        SNO snoSpeaker = ptACDNpc->snoActor;
        ScopedActorDefinitionLockR pdefActor(snoSpeaker);
        BOOL fVOPlaysOnPlayer = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_VO_PLAYS_ON_PLAYER);

        if (!fVOPlaysOnPlayer)
        {



            BOOL bInRange = (SPlayerInScriptedSequence(*ptPlayer) && ptACDNpc->IsKnownByPlayer(*ptPlayer));

            if (!bInRange)
            {

                uint32 dwUsageFlags = ConversationTypeGetUsageFlags(eConvType);
                if ((dwUsageFlags & ((1 << CTU_CANCELS_BEYOND_CLOSE_RANGE_BIT) | (1 << CTU_CANCELS_BEYOND_LONG_RANGE_BIT))) != 0)
                {

                    if (wdDistSq >= gcflConvHearingRadius * gcflConvHearingRadius)
                    {
                        continue;
                    }
                }
                else
                {

                    if (wdDistSq >= gcflConvMaxDistance * gcflConvMaxDistance)
                    {
                        continue;
                    }
                }
            }
        }


        if (!ptPlayerClosest || wdDistSq < wdPlayerClosestDistSq)
        {
            ptPlayerClosest = ptPlayer;
            wdPlayerClosestDistSq = wdDistSq;
        }
    }

    return (ptPlayerClosest ? ptPlayerClosest->idPlayerACD : ACDID_INVALID);
}





static BOOL sIsInRangeOfConversation(
    const GameConnectionID idGameConnection,
    const ActiveConversation & tConv)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, TRUE, idGameConnection);

    POS pos = listDestPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        if (sIsInRangeOfConversation(*ptPlayer, tConv))
        {
            return TRUE;
        }
    }

    return FALSE;
}
static BOOL sGetLocationForConversation(ACDID idACDPrimaryNPC, const ActorCommonData * ptACDPlayer, ACDID & idACDCenter, WorldPlace & placeCenter);




static BOOL sIsInRangeOfConversation(
    const Player & tPlayer,
    const ActiveConversation & tConv)
{
    ActorCommonData* ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return FALSE;
    }
    const WorldPlace& placePlayer = ptACDPlayer->GetPlace();


    ACDID idACDNpc = ACDID_INVALID;
    for (int32 eSpeaker = CONV_SPEAKER_PRIMARY_NPC; eSpeaker < CONV_SPEAKER_FOLLOWER_START; eSpeaker++)
    {
        idACDNpc = tConv.idACDParticipants[eSpeaker];
        if (idACDNpc != ACDID_INVALID)
        {
            break;
        }
    }

    ActorCommonData* ptACDNpc = ACDTryToGet(idACDNpc);
    if (ptACDNpc)
    {

        const WorldPlace& placeNPC = ptACDNpc->GetPlace();
        if (!WorldPlaceInSameWorld(placeNPC, placePlayer))
        {
            return FALSE;
        }


        SNO snoSpeaker = ptACDNpc->snoActor;
        ScopedActorDefinitionLockR pdefActor(snoSpeaker);
        BOOL fVOPlaysOnPlayer = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_VO_PLAYS_ON_PLAYER);
        if (fVOPlaysOnPlayer ||







            SPlayerInScriptedSequence(tPlayer) && ptACDNpc->IsKnownByPlayer(tPlayer))
        {
            return TRUE;
        }
    }


    Player* ptSpeakingPlayer = PlayerGetByPlayerIndex(tConv.nSpeakingPlayerIndex);
    ACDID idACDPlayerSpeaking = (ptSpeakingPlayer ? ptSpeakingPlayer->idPlayerACD : ACDID_INVALID);
    ActorCommonData* ptACDSpeakingPlayer = ACDTryToGet(idACDPlayerSpeaking);

    ACDID idACDCenter;
    WorldPlace placeCenter;
    if (!sGetLocationForConversation(idACDNpc, ptACDSpeakingPlayer, idACDCenter, placeCenter))
    {
        return FALSE;
    }


    if (!WorldPlaceInSameWorld(placeCenter, placePlayer))
    {
        return FALSE;
    }


    WorldDistance wdDistSq = WorldPlaceDistanceSquared(placeCenter, placePlayer);


    uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);
    if ((dwUsageFlags & (1 << CTU_CANCELS_BEYOND_CLOSE_RANGE_BIT)) || (dwUsageFlags & (1 << CTU_CANCELS_BEYOND_LONG_RANGE_BIT)))
    {

        if (wdDistSq >= gcflConvHearingRadius * gcflConvHearingRadius)
        {
            return FALSE;
        }
    }
    else
    {
        if (wdDistSq >= gcflConvMaxDistance * gcflConvMaxDistance)
        {
            return FALSE;
        }
    }

    return TRUE;
}




static BOOL sGetLocationForConversation(
    ACDID idACDPrimaryNPC,
    const ActorCommonData * ptACDPlayer,
    ACDID & idACDCenter,
    WorldPlace & placeCenter)
{


    ActorCommonData* ptACDPrimaryNPC = ACDTryToGet(idACDPrimaryNPC);
    if (ptACDPrimaryNPC == NULL)
    {
        if (ptACDPlayer == NULL)
        {
            return FALSE;
        }
        idACDCenter = ptACDPlayer->id;
        placeCenter = ptACDPlayer->GetPlace();
        return TRUE;
    }
    idACDCenter = ptACDPrimaryNPC->id;
    placeCenter = ptACDPrimaryNPC->GetPlace();
    return TRUE;
}








struct ConvCancelData
{
    ActiveConversation* ptConv;
};

static BOOL sCancelLowerPriorityConversationsForParticipants(
    const ConversationDefinition* pdefConv,
    const int32 nPlayerTalkingIndex,
    const ACDID* idACDParticipants,
    const int32 nPriority,
    const BOOL bForceInterrupt,
    const BOOL bPlayerInitiated)
{
    SGame& tSGame = SGameManager::GameGet();
    ConversationMgrGlobals* ptConvGlobals = tSGame.ptConversationMgrGlobals;


    XGrowableList<ConvCancelData, ConvCancelData&> tCancelList;

    uint32 dwUsageFlags = ConversationTypeGetUsageFlags(pdefConv->eConvType);
    if ((dwUsageFlags & (1 << CTU_IS_GLOBAL_BIT)) && !(dwUsageFlags & (1 << CTU_DOES_NOT_CANCEL_OTHERS_BIT)))
    {
        return TRUE;
    }






    if (nPlayerTalkingIndex == -1)
    {
        return FALSE;
    }

    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation& tConv = ptConvGlobals->tConversationList.GetNext(pos);
        if (tConv.fMarkedForRemoval)
        {
            continue;
        }

        uint32 dwUsageFlagsCur = ConversationTypeGetUsageFlags(tConv.eConvType);
        BOOL bForceCancel = bForceInterrupt;
























        if (tConv.eStatus[nPlayerTalkingIndex] != PARTICIPATION_STATUS_OPTED_IN)
        {
            continue;
        }



        if (bPlayerInitiated && (dwUsageFlagsCur & (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT)))
        {
            bForceCancel = TRUE;
        }

        BOOL bInterruptEqualType = (dwUsageFlagsCur & (1 << CTU_INTERRUPTS_EQUAL_TYPE_BIT)) != 0;

        if (!bForceCancel && (ConversationTypeGetPriority(pdefConv->eConvType) < ConversationTypeGetPriority(tConv.eConvType) ||
            (ConversationTypeGetPriority(pdefConv->eConvType) == ConversationTypeGetPriority(tConv.eConvType) && !bInterruptEqualType && tConv.nPriority >= nPriority)))
        {


            if (sAnyParticipantIsInConversation(tConv, idACDParticipants))
            {
                return FALSE;
            }


            if (pdefConv->eConvType == 1 || pdefConv->eConvType == 6)
            {
                continue;
            }

            return FALSE;
        }











        ConvCancelData tConvCancelData;
        tConvCancelData.ptConv = &tConv;



        if (sAnyParticipantIsInConversation(tConv, idACDParticipants))
        {
            tCancelList.AddTail_Tracked(tConvCancelData);
        }
        else
        {

            if (tConv.eConvType != 1 && tConv.eConvType != 6)
            {

                tCancelList.AddTail_Tracked(tConvCancelData);
            }
        }
    }












    POS posCancel = tCancelList.GetHeadPosition();
    while (posCancel != NULL)
    {
        ConvCancelData& tConvCancelData = tCancelList.GetNext(posCancel);
        sCancelFloatConversation(*tConvCancelData.ptConv, nPlayerTalkingIndex, TRUE);
    }

    return TRUE;
}



static BOOL sShouldCancelForCombat(
    const ConversationDefinition* pdefConv,
    ACDID idACDNPC)
{
    if ((pdefConv->dwFlags & (1 << CF_CANCEL_FOR_COMBAT_BIT)) != 0)
    {
        ActorID idActor = (ActorID)ACDGetOwner(idACDNPC);
        switch (AIStateGetCurrState(idActor))
        {
        case AISTATE_TYPE_NONE:
        case AISTATE_TYPE_IDLE:
        case AISTATE_TYPE_SETUP:
            break;
        default:
            return TRUE;
        }
    }

    return FALSE;
}






static void sMarkConversationStartedForPlayer(
    const ConversationDefinition* pdefConv,
    Player* ptPlayer)
{
    if (pdefConv->szSetPlayerFlag[0] != '\0')
    {
        PlayerSetPlayerGameFlag(ptPlayer, pdefConv->szSetPlayerFlag, TRUE);
    }

    SPlayerMarkConversationAsHeard(ptPlayer, pdefConv->tHeader.snoHandle);
}




void sGetParticipantsForConversation(
    const ConversationDefinition * pdefConv,
    const ActorCommonData * ptACDPlayer,
    ACDID * idACDParticipants,
    BOOL bSuppressWarning)
{

    uint32 dwUsageFlags = ConversationTypeGetUsageFlags(pdefConv->eConvType);
    BOOL bIsGlobal = ((dwUsageFlags & (1 << CTU_IS_GLOBAL_BIT)) != 0);


    ACDID idACDCurrentHireling = (ptACDPlayer ? PlayerGetHireling(ptACDPlayer->id) : ACDID_INVALID);
    ActorCommonData* ptACDCurrentHireling = ACDTryToGet(idACDCurrentHireling);


    float flRadius = gcflConvHearingRadius;
    ACDID idACDCenter;
    WorldPlace placeCenter;

    if (!sGetLocationForConversation(idACDParticipants[CONV_SPEAKER_PRIMARY_NPC], ptACDPlayer, idACDCenter, placeCenter))
    {
        return;
    }



    for (int32 i = CONV_SPEAKER_PRIMARY_NPC; i < NUM_CONV_SPEAKERS; i++)
    {

        if (idACDParticipants[i] != ACDID_INVALID)
        {
            continue;
        }


        if (!pdefConv->fSpeakerUsed[i])
        {
            continue;
        }



        int32 nFollowerIndex = CONV_SPEAKER_INVALID;
        for (int32 j = CONV_SPEAKER_FOLLOWER_START; j < CONV_SPEAKER_FOLLOWER_END; j++)
        {
            if (ConversationSpeakerToSNO(pdefConv, i) ==
                ConversationSpeakerToSNO(pdefConv, j))
            {
                nFollowerIndex = j;
                break;
            }
        }

        if (nFollowerIndex != CONV_SPEAKER_INVALID)
        {
            HirelingClass eHirelingClass = ConversationSpeakerToHirelingClass(nFollowerIndex);

            if (bIsGlobal)
            {
                idACDParticipants[i] = SHirelingGetNearby(idACDCenter, flRadius * flRadius, eHirelingClass);
            }
            else if (ptACDCurrentHireling && eHirelingClass == HirelingGetClass(*ptACDCurrentHireling))
            {

                if ((pdefConv->dwFlags & (1 << CF_PERSIST_THROUGH_SPEAKER_DEATH)) || ptACDCurrentHireling->IsAlive())
                {
                    idACDParticipants[i] = idACDCurrentHireling;
                }
                else
                {
                    idACDParticipants[i] = ACDID_INVALID;
                }
            }


            idACDParticipants[nFollowerIndex] = idACDParticipants[i];
        }
        else
        {

            idACDParticipants[i] = sFindACDFromActorSNO(pdefConv->tHeader.snoHandle, placeCenter, ConversationSpeakerToSNO(pdefConv, i), bSuppressWarning);


            if (ptACDPlayer && idACDParticipants[i] != ACDID_INVALID)
            {
                ActorCommonData* ptACD = ACDTryToGet(idACDParticipants[i]);
                if (ptACD && ACDGetHirelingClass(*ptACD) != HC_NONE)
                {
                    Player* ptPlayer = PlayerGetByACD(ptACDPlayer->id);
                    if (ptPlayer && !ptACD->IsKnownByPlayer(*ptPlayer))
                    {
                        idACDParticipants[i] = ACDID_INVALID;
                    }
                }
            }
        }
    }
}





static BOOL sStartConversation(
    int32 nPlayerTalkingIndex,
    const SNO snoConversation,
    uint32 dwConvConditions,
    uint32 dwConvCheckConditions,
    const ACDID idACDTalkingTo,
    const ACDID idACDAlt1,
    const ACDID idACDAlt2,
    const ACDID idACDAlt3,
    const ACDID idACDAlt4,
    const int32 nPriority,
    const BOOL bForceInterrupt,
    const BOOL bPlayerInitiated)
{
    SGame& tSGame = SGameManager::GameGet();
    ConversationMgrGlobals* ptConvGlobals = tSGame.ptConversationMgrGlobals;

    if (ptConvGlobals == NULL)
    {
        return FALSE;
    }

    ScopedConversationDefinitionLockR pdefConv(snoConversation);

    Player* ptPlayer = PlayerGetByPlayerIndex(nPlayerTalkingIndex);
    ACDID idACDPlayer = (ptPlayer ? ptPlayer->idPlayerACD : ACDID_INVALID);


    if (idACDPlayer == ACDID_INVALID)
    {
        idACDPlayer = sGetClosestPlayerForConversation(pdefConv->eConvType, idACDTalkingTo);
        if (idACDPlayer == ACDID_INVALID)
        {

            uint32 dwUsageFlags = ConversationTypeGetUsageFlags(pdefConv->eConvType);
            if (!(dwUsageFlags & (1 << CTU_IS_GLOBAL_BIT)))
            {
                return FALSE;
            }
        }
        else
        {


            ptPlayer = PlayerGetByACD(idACDPlayer);
            nPlayerTalkingIndex = (ptPlayer ? ptPlayer->nPlayerIndex : -1);
        }
    }

    ACDID idACDParticipants[9] =
    {
        idACDPlayer, idACDTalkingTo, idACDAlt1, idACDAlt2, idACDAlt3, idACDAlt4, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID,
    };









    PlayerList listDestPlayers(PlayerListGetElementPool());
    sGetPlayersForConversation(pdefConv, ptPlayer, listDestPlayers);


    if (sShouldCancelForCombat(pdefConv, idACDTalkingTo))
    {
        return FALSE;
    }


    if ((pdefConv->dwFlags & (1 << CF_NEVER_REPEAT_BIT)) != 0)
    {

        int32 nPlayersHeardConversation = 0;

        POS posPlayer = listDestPlayers.GetHeadPosition();
        while (posPlayer != NULL)
        {
            Player* ptPlayerToCheck = listDestPlayers.GetNext(posPlayer);
            if (SPlayerHasHeardConversation(ptPlayerToCheck, pdefConv->tHeader.snoHandle))
            {

                nPlayersHeardConversation++;
                if (nPlayersHeardConversation >= listDestPlayers.Count() ||



                    (pdefConv->eConvType == CONV_TYPE_AMBIENT_FLOAT && nPlayersHeardConversation > 0))
                {
                    return FALSE;
                }
            }
        }
    }





    ActorCommonData* ptACDPlayer = ACDTryToGet(idACDPlayer);
    sGetParticipantsForConversation(pdefConv, ptACDPlayer, idACDParticipants, TRUE);


    for (int32 i = 0; i < NUM_CONV_SPEAKERS; i++)
    {
        if (i != CONV_SPEAKER_PRIMARY_NPC && !pdefConv->fSpeakerUsed[i])
        {
            idACDParticipants[i] = ACDID_INVALID;
        }
    }

    if (!sCancelLowerPriorityConversationsForParticipants(pdefConv, nPlayerTalkingIndex, idACDParticipants, nPriority, bForceInterrupt, bPlayerInitiated))
    {
        return FALSE;
    }




    ConversationMgr::EvaluateConversationConditions(idACDTalkingTo, idACDPlayer, idACDParticipants, snoConversation, dwConvConditions, dwConvCheckConditions);







    POS posNew = ptConvGlobals->tConversationList.AddTail_NoAssign_Tracked();

    ActiveConversation& tActiveConv = ptConvGlobals->tConversationList.GetAt(posNew);
    sActiveConversationInitialize(tActiveConv, snoConversation, ptPlayer, idACDParticipants, dwConvConditions, dwConvCheckConditions, nPriority, bPlayerInitiated);




    if (sActiveConversationIsFinished(tActiveConv))
    {
        sCancelFloatConversation(tActiveConv, -1, TRUE);
        return FALSE;
    }




    ScriptEventOnConversationStarted(idACDPlayer, idACDParticipants[CONV_SPEAKER_PRIMARY_NPC], snoConversation);





    {
        ActorCommonData* ptACDNPC = ACDTryToGet(idACDTalkingTo);

        POS pos = listDestPlayers.GetHeadPosition();
        while (pos != NULL)
        {
            Player* ptPlayerToSet = listDestPlayers.GetNext(pos);


            ActorCommonData* ptPlayerACD = ACDTryToGet(ptPlayerToSet->idPlayerACD);
            if (ptACDNPC && ptPlayerACD && NPCIsOperableVendorOrCrafter(*ptACDNPC, *ptPlayerACD))
            {
                if (ptPlayerToSet->nPlayerIndex != ptPlayer->nPlayerIndex)
                {
                    continue;
                }
            }



            sMarkConversationStartedForPlayer(pdefConv, ptPlayerToSet);


            if (pdefConv->eConvType == CONV_TYPE_TALK_MENU)
            {
                SPlayerSaveConversation(listDestPlayers, snoConversation, TRUE);
            }


            for (int32 i = 0; i < NUM_CONV_SPEAKERS; i++)
            {
                ActorCommonData* ptACDNPC2 = ACDTryToGet(idACDParticipants[i]);
                if (ptACDNPC2 != NULL)
                {
                    NPCUpdateConversationIconAndInteraction(*ptACDNPC2, *ptPlayerToSet);
                    sEvaluatePlayerConverseWithNPCAchievement(*ptACDNPC2, *ptPlayerToSet);
                }
            }

            sEvaluatePlayerConversationAchievement(snoConversation, Achievement::ACH_EVENT_INITIATE_CONVERSATION, *ptPlayerToSet);


            ActorCommonData* ptACD = ACDTryToGet(ptPlayerToSet->idPlayerACD);
            if (ptACD != NULL)
            {
                ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_IN_CONVERSATION), 1);

                if (ptACDNPC && (pdefConv->dwFlags & (1 << CF_OPERATE_SPEAKER_ON_BEGIN_BIT)))
                {
                    NPCOperate(*ptACDNPC, *ptACD, 0, TRUE, TRUE, TRUE);
                }
            }
        }
    }





















    sAdvanceFloatConversation(tActiveConv, FALSE);
    return TRUE;

}




BOOL ConversationMgr::StartConversation(const int32 nPlayerTalkingIndex, const SNO snoConversation, const ACDID idACDTalkingTo, const ACDID idACDAlt1, const ACDID idACDAlt2, const ACDID idACDAlt3, const ACDID idACDAlt4, const int32 nPriority, const BOOL bForceInterrupt, const BOOL bPlayerInitiated)
{
    return sStartConversation(nPlayerTalkingIndex, snoConversation, 0, 0, idACDTalkingTo, idACDAlt1, idACDAlt2, idACDAlt3, idACDAlt4, nPriority, bForceInterrupt, bPlayerInitiated);
}















BOOL ConversationMgr::StartDebugConversation(
    const int32 nPlayerTalkingIndex,
    const SNO snoConversation,
    const uint32 dwConvConditions,
    const uint32 dwConvCheckConditions,
    const ACDID idACDTalkingTo,
    const ACDID idACDAlt1,
    const ACDID idACDAlt2,
    const ACDID idACDAlt3,
    const ACDID idACDAlt4,
    const int32 nPriority,
    const BOOL bForceInterrupt)
{
    return sStartConversation(nPlayerTalkingIndex, snoConversation, dwConvConditions, dwConvCheckConditions, idACDTalkingTo, idACDAlt1, idACDAlt2, idACDAlt3, idACDAlt4, nPriority, bForceInterrupt, FALSE);  // inference: last argument FALSE; the body was linker-dropped (concrete DIE at low_pc 0), no bytes to check
}





BOOL ConversationMgr::ConversationHasAllParticipants(const SNO snoConversation, const ActorCommonData * ptACDPlayer, const ActorCommonData * ptACDNpc)
{
    ACDID idACDParticipants[9] = {
        ptACDPlayer ? ptACDPlayer->id : ACDID_INVALID,
        ptACDNpc ? ptACDNpc->id : ACDID_INVALID,
        ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID,
    };

    ScopedConversationDefinitionLockR pdefConv(snoConversation);
    sGetParticipantsForConversation(pdefConv, ptACDPlayer, idACDParticipants, TRUE);
    for (int32 i = 0; i < 6; i++)
    {
        if (pdefConv->fSpeakerUsed[i] && idACDParticipants[i] == ACDID_INVALID)
        {
            return FALSE;
        }
    }
    return TRUE;
}

















static BOOL sShouldCancelConversation(
    ActiveConversation & tConv)
{
    uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);
    BOOL fCancelConv = FALSE;

    if ((dwUsageFlags & (1 << CTU_CANCELS_BEYOND_CLOSE_RANGE_BIT)) ||
        ((dwUsageFlags & (1 << CTU_ZOOMS_FOR_TARGET_PLAYER_BIT)) && tConv.bZoom))
    {
        Player * ptPlayer = PlayerGetByPlayerIndex(tConv.nSpeakingPlayerIndex);
        ActorCommonData * ptACDPlayer = ptPlayer ? ACDTryToGet(ptPlayer->idPlayerACD) : NULL;
        if (ptACDPlayer == NULL)
        {
            tConv.bZoom = FALSE;
            fCancelConv = ((dwUsageFlags & (1 << CTU_CANCELS_BEYOND_CLOSE_RANGE_BIT)) != 0);
        }
        else
        {
            BOOL fStartedWithValidNPC = (tConv.idACDParticipants[CONV_SPEAKER_PRIMARY_NPC] != ACDID_INVALID);
            if (fStartedWithValidNPC)
            {
                ActorCommonData * ptACDNpc = ACDTryToGet(tConv.idACDParticipants[CONV_SPEAKER_PRIMARY_NPC]);
                if (!(ptACDNpc != NULL && ConversationPlayerNearNPC(*ptACDPlayer, *ptACDNpc)))
                {
                    tConv.bZoom = FALSE;
                    fCancelConv = ((dwUsageFlags & (1 << CTU_CANCELS_BEYOND_CLOSE_RANGE_BIT)) != 0);
                }
            }
        }
    }

    ScopedConversationDefinitionLockR pdefConv(tConv.snoConversation);
    if (sShouldCancelForCombat(pdefConv, tConv.idACDParticipants[CONV_SPEAKER_PRIMARY_NPC]))
    {
        fCancelConv = TRUE;
    }

    return fCancelConv;
}



// NON_MATCHING: 44/573 insns; the inlined sShouldCancelConversation tests CTU bit 3 as tbnz/and where the original uses lsr+tbnz/ubfx
void ConversationMgr::Update()

{

    SGame & tSGame = SGameManager::GameGet();
    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;
    if (ptConvGlobals == NULL)
    {
        return;
    }

    ServerUpdateWasCalled(ptConvGlobals->tUpdateData);
    ServerQueueUpdateFunction(1, ptConvGlobals->tUpdateData);

    uint32 dwNow = GameGetActiveGameCommonData().dwServerTime;





    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        POS posOld = pos;
        ActiveConversation & tConv = ptConvGlobals->tConversationList.GetNext(pos);

        if (tConv.fMarkedForRemoval)
        {
            ptConvGlobals->tConversationList.RemoveAt(posOld);
        }
        else
        {
            if (sShouldCancelConversation(tConv))
            {
                sCancelFloatConversation(tConv, -1, FALSE);
                continue;
            }


            if (dwNow >= tConv.dwNextAdvanceTime)
            {

                tConv.dwLastAutoAdvanceTime = dwNow;


                sAddConversationParticipants(tConv);

                sAdvanceFloatConversation(tConv, FALSE);
            }





            sRemoveConversationParticipants(tConv);
        }
    }

    sSConversationPurgeUnusedScripts();
}




void ConversationMgr::CancelConversation(const SNO snoConv)
{

    SGame & tSGame = SGameManager::GameGet();
    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;
    if (ptConvGlobals == NULL)
    {
        return;
    }

    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation & tConv = ptConvGlobals->tConversationList.GetNext(pos);

        if (tConv.snoConversation == snoConv)
        {
            sCancelFloatConversation(tConv, -1, TRUE);
        }
    }
}



BOOL ConversationMgr::ConversationIsActive(const SNO snoConv)
{

    SGame & tSGame = SGameManager::GameGet();
    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;
    if (ptConvGlobals == NULL)
    {
        return FALSE;
    }

    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation & tConv = ptConvGlobals->tConversationList.GetNext(pos);
        if (tConv.snoConversation == snoConv && !tConv.fMarkedForRemoval)
        {
            return TRUE;
        }
    }

    return FALSE;
}




static BOOL sACDIsInConversation(const ActiveConversation & tConv, const ACDID idACD)
{

    if (tConv.fMarkedForRemoval)
    {
        return FALSE;
    }

    for (int32 i = 0; i < 9; i++)
    {

        if (tConv.idACDParticipants[i] == idACD)
        {
            return TRUE;
        }
    }

    return FALSE;
}




BOOL sACDIsInActiveConversation(const ACDID idACD, BOOL bCheckUsable, const Player * ptPlayer)
{

    SGame & tSGame = SGameManager::GameGet();

    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;
    if (ptConvGlobals == NULL)
    {
        return FALSE;
    }


    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation & tConv = ptConvGlobals->tConversationList.GetNext(pos);
        if (tConv.fMarkedForRemoval)
        {
            continue;
        }

        uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);
        if (bCheckUsable && !(dwUsageFlags & (1 << 11)))
        {
            continue;
        }






        if (ptPlayer != NULL && !(dwUsageFlags & (1 << 5)) && tConv.eStatus[ptPlayer->nPlayerIndex] != PARTICIPATION_STATUS_OPTED_IN)
        {
            continue;
        }





        if (sACDIsInConversation(tConv, idACD))
        {
            return TRUE;
        }
    }

    return FALSE;
}


BOOL ConversationMgr::ACDIsUnusableDuringConversation(const ACDID idACD, const Player * ptPlayer)
{
    return sACDIsInActiveConversation(idACD, TRUE, ptPlayer);
}






BOOL ConversationMgr::ACDIsInActiveConversation(const ACDID idACD, const Player * ptPlayer)
{
    return sACDIsInActiveConversation(idACD, FALSE, ptPlayer);
}






BOOL ConversationMgr::PlayerOptOutOfAllConversations(
    const Player & tPlayer,
    ActiveConversation * ptConv)
{
    SGame& tSGame = SGameManager::GameGet();
    ConversationMgrGlobals* ptConvGlobals = tSGame.ptConversationMgrGlobals;
    if (ptConvGlobals == NULL)
    {
        return FALSE;
    }

    XGrowableList<ConvCancelData, ConvCancelData&> tCancelList;

    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation& tConv = ptConvGlobals->tConversationList.GetNext(pos);


        if (tConv.eStatus[tPlayer.nPlayerIndex] != PARTICIPATION_STATUS_OPTED_IN)
        {
            continue;
        }

        if (ptConv != NULL)
        {
            if (&tConv == ptConv)
            {
                continue;
            }





            ScopedConversationDefinitionLockR pdefConv(ptConv->snoConversation);
            if (ConversationTypeGetPriority(tConv.eConvType) > ConversationTypeGetPriority(pdefConv->eConvType) ||
                ((tConv.eConvType == CONV_TYPE_AMBIENT_GOSSIP ||
                  tConv.eConvType == CONV_TYPE_TALK_MENU ||
                  tConv.eConvType == CONV_TYPE_QUEST_STANDARD) &&
                 tConv.eStatus[ptConv->nSpeakingPlayerIndex] != PARTICIPATION_STATUS_OPTED_IN))
            {





                return FALSE;
            }





            uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);
            if (dwUsageFlags & (1 << CTU_NEVER_SILENCE_AUDIO_BIT))
            {
                continue;
            }
        }


        ConvCancelData tConvCancelData;
        tConvCancelData.ptConv = &tConv;
        tCancelList.AddTail_Tracked(tConvCancelData);
    }


    POS posCancel = tCancelList.GetHeadPosition();
    while (posCancel != NULL)
    {
        ConvCancelData& tConvCancelData = tCancelList.GetNext(posCancel);
        sCancelFloatConversation(*tConvCancelData.ptConv, tPlayer.nPlayerIndex, TRUE);
    }

    return TRUE;
}




void ConversationMgr::UpdateIdleForActor(ACDID idACD, const uint32 dwAnimTag)
{

    SGame & tSGame = SGameManager::GameGet();
    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;
    if (ptConvGlobals == NULL)
    {
        return;
    }

    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation & tConv = ptConvGlobals->tConversationList.GetNext(pos);
        for (int32 i = 0; i < 9; i++)
        {
            if (tConv.idACDParticipants[i] == idACD && tConv.dwAnimIdleTags[i] != 0)
            {
                tConv.dwAnimIdleTags[i] = dwAnimTag;
            }
        }
    }
}





























































































































































static void sAddPlayerToActiveConversation(
    const Player* ptPlayer,
    ActiveConversation& tConv)
{






    {
        uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);
        if ((dwUsageFlags & (1 << CTU_IS_GLOBAL_BIT)) && !(dwUsageFlags & (1 << CTU_DOES_NOT_CANCEL_OTHERS_BIT)))
        {


            if (!ConversationMgr::PlayerOptOutOfAllConversations(*ptPlayer, &tConv))
            {
                return;
            }
        }


        tConv.eStatus[ptPlayer->nPlayerIndex] = PARTICIPATION_STATUS_OPTED_IN;
    }
}





static void sActiveConversationInitialize(
    ActiveConversation& tConv,
    const SNO snoConversation,
    Player* ptStartingPlayer,
    const ACDID* idACDParticipants,
    const uint32 dwConvConditions,
    const uint32 dwConvCheckConditions,
    const int32 nPriority,
    const BOOL bPlayerInitiated)
{
    SGame& tSGame = SGameManager::GameGet();
    ConversationMgrGlobals* ptConvGlobals = tSGame.ptConversationMgrGlobals;


    ScopedConversationDefinitionLockR pdefConv(snoConversation);
    uint32 dwUsageFlags = ConversationTypeGetUsageFlags(pdefConv->eConvType);

    tConv.fMarkedForRemoval = FALSE;
    tConv.snoConversation = snoConversation;

    tConv.eConvType = pdefConv->eConvType;
    tConv.nPriority = nPriority;
    tConv.bPlayerInitiated = bPlayerInitiated;


    for (int32 i = 0; i < NUM_CONV_SPEAKERS; i++)
    {
        tConv.dwAnimIdleTags[i] = 0;
        tConv.idACDParticipants[i] = idACDParticipants[i];
    }


    for (int32 i = 0; i < NUM_CONV_SPEAKERS; i++)
    {
        ActorCommonData* ptACD = ACDTryToGet(idACDParticipants[i]);
        if (ptACD && ptACD->IsAlive() && !(dwUsageFlags & (1 << CTU_NEVER_ANIMATES_BIT)))
        {
            tConv.dwAnimIdleTags[i] = sACDGetCurrentIdleAnim(*ptACD);
        }
    }


    tConv.bZoom = bPlayerInitiated && (dwUsageFlags & (1 << CTU_ZOOMS_FOR_TARGET_PLAYER_BIT));

    if (ptStartingPlayer != NULL)
    {
        tConv.nSpeakingPlayerIndex = ptStartingPlayer->nPlayerIndex;
        tConv.snoPlayerActor = ptStartingPlayer->snoActorPortrait;
        tConv.ePlayerClass = ptStartingPlayer->ePlayerClassPortrait;
        tConv.ePlayerGender = (PlayerIsMale(*ptStartingPlayer) ? 0 : 1);
    }
    else
    {
        tConv.nSpeakingPlayerIndex = -1;
        tConv.snoPlayerActor = -1;
        tConv.ePlayerClass = -1;
        tConv.ePlayerGender = -1;
    }

    tConv.dwLastAutoAdvanceTime = 0;
    tConv.dwNextAdvanceTime = 0;

    tConv.tTreeIter.Init(tConv.snoConversation, dwConvConditions, dwConvCheckConditions, &SGameGlobalsGet().rng, FALSE);
    tConv.nLastLineGUIDSentToClient = 0;
    tConv.dwLastLineUID = 0xFFFFFFFF;

    tConv.idACDLastSpeaker = ACDID_INVALID;
    tConv.bAnimChanged = FALSE;


    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        tConv.eStatus[i] = PARTICIPATION_STATUS_NOT_INCLUDED;
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    sGetPlayersForConversation(pdefConv, ptStartingPlayer, listDestPlayers);

    POS posPlayer = listDestPlayers.GetHeadPosition();
    while (posPlayer != NULL)
    {
        Player* ptPlayer = listDestPlayers.GetNext(posPlayer);
        if (sIsInRangeOfConversation(*ptPlayer, tConv))
        {

            sAddPlayerToActiveConversation(ptPlayer, tConv);
        }
    }
}



static void sActiveConversationDispose(ActiveConversation & tActiveConv)
{


    tActiveConv.fMarkedForRemoval = TRUE;
    tActiveConv.tTreeIter.Cancel();
}



static void sDecompressConvDisplayTimes(
    ConvLocaleDisplayTimes & tTimes,
    CompressedConvLocaleDisplayTimes * ptCompTimes,
    int32 nCount,
    XLocale eActualLocale)
{
    for (int32 i = 0; i < nCount; i++)
    {
        CompressedConvLocaleDisplayTimes & info = ptCompTimes[i];

        if ((info.eLocale != eActualLocale) && info.eLocale != NUM_XLOCALES)
        {
            continue;
        }


        if (info.eClass == NUM_PLAYER_CLASSES)
        {
            for (int32 eClass = 0; eClass < NUM_PLAYER_CLASSES; eClass++)
            {
                if (info.eGender == NUM_ACTOR_GENDERS)
                {
                    for (int32 eGender = 0; eGender < NUM_ACTOR_GENDERS; eGender++)
                    {
                        tTimes.dwDisplayTime[eClass * NUM_ACTOR_GENDERS + eGender] = info.dwValue;
                    }
                }
                else
                {
                    tTimes.dwDisplayTime[eClass * NUM_ACTOR_GENDERS + info.eGender] = info.dwValue;
                }
            }
        }
        else
        {
            if (info.eGender == NUM_ACTOR_GENDERS)
            {
                for (int32 eGender = 0; eGender < NUM_ACTOR_GENDERS; eGender++)
                {
                    tTimes.dwDisplayTime[info.eClass * NUM_ACTOR_GENDERS + eGender] = info.dwValue;
                }
            }
            else
            {
                tTimes.dwDisplayTime[info.eClass * NUM_ACTOR_GENDERS + info.eGender] = info.dwValue;
            }
        }
    }
}



static ConversationAdvancedReturn sActiveConversationAdvance(
    ActiveConversation & tActiveConv,
    PlayLineParams & tParams)
{
    SGame & tSGame = SGameManager::GameGet();
    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;

    while (tActiveConv.tTreeIter.GetNext(tActiveConv.idACDParticipants, tActiveConv.snoConversation))
    {
        const ConversationTreeNode * ptCurNode = tActiveConv.tTreeIter.GetCurrentNode();

        if (ptCurNode->eConvNodeType != CONV_NODE_TYPE_CONV_LINE)
        {
            continue;
        }

        if (ptCurNode->ePlayerClass != PLAYER_CLASS_NONE &&
            ptCurNode->ePlayerClass != tActiveConv.ePlayerClass)
        {
            continue;
        }
        if (ptCurNode->eGender != ACTOR_GENDER_INVALID &&
            ptCurNode->eGender != tActiveConv.ePlayerGender)
        {
            continue;
        }

        const LineGroupData * ptLineGroupData = tActiveConv.tTreeIter.GetLineGroupData();




        if (tActiveConv.dwLastLineUID == ptLineGroupData->dwLineUID)
        {
            continue;
        }
        tActiveConv.dwLastLineUID = ptLineGroupData->dwLineUID;


        if (ptCurNode->eConvVarSetOp != CONV_VAR_SET_OP_INVALID)
        {
            GBID gbidVar = ptCurNode->gbidConvVarSet;
            if (gbidVar != GBID_NONE && tSGame.ptSGameGlobals)
            {
                int32 nValue = 0;

                switch (ptCurNode->eConvVarSetOp)
                {
                    case CONV_VAR_SET_OP_INC:
                        tSGame.ptSGameGlobals->tConversationVarMap.Lookup(gbidVar, nValue);
                        nValue += ptCurNode->nConvVarSetVal;
                        break;
                    case CONV_VAR_SET_OP_DEC:
                        tSGame.ptSGameGlobals->tConversationVarMap.Lookup(gbidVar, nValue);
                        nValue -= ptCurNode->nConvVarSetVal;
                        break;
                    case CONV_VAR_SET_OP_SET:
                        nValue = ptCurNode->nConvVarSetVal;
                        break;
                }



                tSGame.ptSGameGlobals->tConversationVarMap.SetAt_Tracked(gbidVar, nValue);

                BOOL bVarTrace = FALSE;












            }
        }

        ScopedConversationDefinitionLockR pdefConv(tActiveConv.snoConversation);

        tParams.snoConversation = tActiveConv.snoConversation;
        tParams.nSpeakingPlayerIndex = tActiveConv.nSpeakingPlayerIndex;
        tParams.dwLineUID = ptLineGroupData->dwLineUID;
        tParams.eLineSpeaker = ptLineGroupData->eLineSpeaker;
        tParams.eSpeakerTarget = ptLineGroupData->eSpeakerTarget;
        tParams.eLineClass = ptCurNode->ePlayerClass;
        tParams.eLineGender = ptCurNode->eGender;
        tParams.ePlayerClass = tActiveConv.ePlayerClass;
        tParams.ePlayerGender = tActiveConv.ePlayerGender;
        tParams.dwLineFlags = ptLineGroupData->uFlags | ptCurNode->uFlags;
        tParams.dwLineAnimTag = ptLineGroupData->dwAnimTag;
        tParams.nPriority = tActiveConv.nPriority;
        tParams.bZoom = tActiveConv.bZoom;
        tParams.bFirstLine = (tActiveConv.nLastLineGUIDSentToClient == 0);
        tParams.bPlayerInitiated = tActiveConv.bPlayerInitiated;

        ACDID idACDSpeaker = tActiveConv.idACDParticipants[tParams.eLineSpeaker];
        Player * ptPlayer = PlayerGetByPlayerIndex(tActiveConv.nSpeakingPlayerIndex);
        ACDID idACDPlayer = (ptPlayer ? ptPlayer->idPlayerACD : ACDID_INVALID);
        ActorCommonData * ptACDSpeaker = ACDTryToGet(idACDSpeaker);
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        tParams.bHostile = ((ptACDSpeaker && ptACDPlayer) ? ACDIsHostile(*ptACDSpeaker, *ptACDPlayer, FALSE) : FALSE);  // columns 29 '(', 43 '&&', 61 call; BOOL result stored unconverted



        uint32 dwDisplayTime = 0;
        {
            PlayerList listDestPlayers(PlayerListGetElementPool());
            sGetPlayersOptedIntoConversation(tActiveConv, listDestPlayers, -1);

            POS posPlayer = listDestPlayers.GetHeadPosition();
            while (posPlayer != NULL)
            {
                Player * ptPlayer2 = listDestPlayers.GetNext(posPlayer);
                XLocale eActualLocale = XLocaleGetBaseLocale(ptPlayer2->eLocale);


                ConvLocaleDisplayTimes tTimes;
                sDecompressConvDisplayTimes(tTimes, ptCurNode->ptCompressedDisplayTimes, ptCurNode->serCompressedDisplayTimes.dwSize / sizeof(CompressedConvLocaleDisplayTimes), eActualLocale);



                int32 nIndex = Math::Max(0, tActiveConv.ePlayerClass * NUM_ACTOR_GENDERS + tActiveConv.ePlayerGender);
                dwDisplayTime = Math::Max(dwDisplayTime, tTimes.dwDisplayTime[nIndex]);
            }
        }
        tParams.dwLineDisplayTime = Math::Max<int>(0, dwDisplayTime + ptLineGroupData->nDisplayTimeAdjustment);


        tParams.nLineGUID = ptConvGlobals->nLineGUID++;

        return CONV_LINE;
    }

    return CONV_DONE;
}




static BOOL sEvaluateCondition(
    const ACDID idACDTalkingTo,
    const ACDID idACDPlayer,
    const SNO snoConversation,
    SNO snoScript,
    int32 nCondIndex)
{
    BOOL bCond = TRUE;

    CRefString sConv = SNOToString(CONVERSATION_SNO, snoConversation);
    CRefString sFuncName;

    sFuncName.Format("%s%d", gszConvConditionalRootFuncName, nCondIndex);

    lua_State* ptScript = LuaScriptGetLuaStateWithSNO(snoScript);
    LuaStackChecker tStackCheck(ptScript);

    if (LuaScriptGetFunction(ptScript, sFuncName))
    {
        lua_pushnumber(ptScript, idACDTalkingTo);
        lua_pushnumber(ptScript, idACDPlayer);
        lua_pushstring(ptScript, sConv.str());

        int32 nError = LuaSNOScriptCall(snoScript, ptScript, 3, 1, sFuncName);

        if (nError == 0)
        {
            if ( LuaCheckReturnValue(snoScript, ptScript, LUA_TBOOLEAN, sFuncName.str()) )
            {
                bCond = lua_toboolean(ptScript, -1);
            }

            lua_pop(ptScript, 1);
        }
    }

    return bCond;
}




void ConversationMgr::AdvanceConversationForPlayer(const Player & tPlayer, SNO snoConv, int32 nLineUID)
{

    SGame & tSGame = SGameManager::GameGet();
    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;

    int32 nPlayerIndex = tPlayer.nPlayerIndex;
    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation & tConv = ptConvGlobals->tConversationList.GetNext(pos);

        if (!tConv.fMarkedForRemoval && tConv.snoConversation == snoConv && tConv.eStatus[nPlayerIndex] == PARTICIPATION_STATUS_OPTED_IN)
        {
            uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);
            BOOL fCanAdvance = (dwUsageFlags & (1 << 2)) != 0;
            if (fCanAdvance && tConv.nLastLineGUIDSentToClient == nLineUID)
            {
                sAdvanceFloatConversation(tConv, TRUE);
            }
            break;
        }
    }
}





































void ConversationMgr::UpdateConversationAutoAdvanceForPlayer(const Player & tPlayer, SNO snoConv, int32 nLineUID, uint32 dwNextAdvanceTime)
{

    SGame & tSGame = SGameManager::GameGet();
    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;

    int32 nPlayerIndex = tPlayer.nPlayerIndex;
    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation & tConv = ptConvGlobals->tConversationList.GetNext(pos);

        if (!tConv.fMarkedForRemoval && tConv.snoConversation == snoConv && tConv.eStatus[nPlayerIndex] == PARTICIPATION_STATUS_OPTED_IN)
        {
            if (tConv.nLastLineGUIDSentToClient == nLineUID)
            {
                tConv.dwNextAdvanceTime = Math::Pin(dwNextAdvanceTime, tConv.dwEarliestNextAutoAdvanceTime, tConv.dwNextAdvanceTime);
            }
            break;
        }
    }
}






























void ConversationMgr::CancelConversationForPlayer(const Player & tPlayer)
{

    SGame & tSGame = SGameManager::GameGet();
    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;

    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation & tConv = ptConvGlobals->tConversationList.GetNext(pos);

        if (tConv.fMarkedForRemoval || tConv.eStatus[tPlayer.nPlayerIndex] != PARTICIPATION_STATUS_OPTED_IN)
        {
            continue;
        }

        uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);
        BOOL fCanCancel = (dwUsageFlags & (1 << 2)) != 0;
        if (fCanCancel)
        {
            sCancelFloatConversation(tConv, -1, TRUE);
            return;
        }
    }
}












BOOL ConversationMgr::PlayerIsInQuestConversation(const Player & tPlayer)
{

    SGame & tSGame = SGameManager::GameGet();
    ConversationMgrGlobals * ptConvGlobals = tSGame.ptConversationMgrGlobals;

    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation & tConv = ptConvGlobals->tConversationList.GetNext(pos);
        if (tConv.fMarkedForRemoval)
        {
            continue;
        }

        uint32 dwUsageFlags = ConversationTypeGetUsageFlags(tConv.eConvType);
        if ((dwUsageFlags & (1 << 13)) &&
            ((dwUsageFlags & (1 << 5)) || tConv.eStatus[tPlayer.nPlayerIndex] == PARTICIPATION_STATUS_OPTED_IN))
        {
            return TRUE;
        }
    }

    return FALSE;
}






void ConversationMgr::EvaluateConversationConditions(
    const ACDID idACDTalkingTo,
    const ACDID idACDPlayer,
    const ACDID * idACDParticipants,
    const SNO snoConversation,
    uint32 & dwConditions,
    uint32 & dwCheckConditions)
{
    dwConditions = 0;
    dwCheckConditions = 0;

    ScopedConversationDefinitionLockR ptConv(snoConversation);
    SNO snoScript = ptConv->snoScript;

    ConversationTreeIter tIter(snoConversation);
    while (tIter.GetNext(NULL, SNO_NONE))
    {
        const ConversationTreeNode* ptNode = tIter.GetCurrentNode();

        if ( ptNode->eConvNodeType == CONV_NODE_TYPE_CONDITIONAL )
        {

            if (!(dwCheckConditions & (1 << ptNode->nBranchIndex)))
            {
                BOOL bCond = sEvaluateCondition(idACDTalkingTo, idACDPlayer, snoConversation, snoScript, ptNode->nBranchIndex);
                // Inferred: the original has a single row (3194:17) for the whole set/clear and check update, as from a macro whose name is unknown.
                if (bCond) dwConditions |= (1 << ptNode->nBranchIndex); else dwConditions &= ~(1 << ptNode->nBranchIndex); dwCheckConditions |= (1 << ptNode->nBranchIndex);
            }
        }
        else if (ptNode->eConvNodeType == CONV_NODE_TYPE_CONDITIONAL_PARTICIPANT)
        {


            BOOL bCond = (idACDParticipants[ptNode->eLineSpeaker] != ACDID_INVALID);
            // Inferred: single original row 3203:13, as at 3194.
            if (bCond) dwConditions |= (1 << ptNode->nBranchIndex); else dwConditions &= ~(1 << ptNode->nBranchIndex); dwCheckConditions |= (1 << ptNode->nBranchIndex);
        }
    }

}














static void sGetPlayersForConversation(
    const ConversationDefinition* pdefConv,
    Player* ptPlayer,
    PlayerList& listDestPlayers)
{
    uint32 dwUsageFlags = ConversationTypeGetUsageFlags(pdefConv->eConvType);
    if (dwUsageFlags & (1 << CTU_IS_GLOBAL_BIT))
    {

        PlayerListGetAllInGame(listDestPlayers, -1, TRUE, -1);
    }
    else if (ptPlayer != NULL)
    {

        PlayerListGetAllInGame(listDestPlayers, -1, TRUE, ptPlayer->idGameConnection);
    }
}





static void sGetPlayersOptedIntoConversation(
    const ActiveConversation & tConv,
    PlayerList & listDestPlayers,
    const GameConnectionID idGameConnection)
{
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, idGameConnection);

    POS pos = listDestPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        POS posCur = pos;
        Player * ptPlayer = listDestPlayers.GetNext(pos);

        if (tConv.eStatus[ptPlayer->nPlayerIndex] != PARTICIPATION_STATUS_OPTED_IN)
        {
            listDestPlayers.RemoveAt(posCur);
        }
    }
}




void ConversationMgr::MarkSavedConversationsAsHeard(Player & tPlayer)
{

    for (int32 i = 0; i < tPlayer.tHeroStateData.tSavedData.tSavedConversations.GetNumConversations(); i++)
    {
        SNO snoConv;
        if (tPlayer.tHeroStateData.tSavedData.tSavedConversations.HasPlayed(i, snoConv))
        {
            SPlayerMarkConversationAsHeard(&tPlayer, snoConv);
        }
    }
}





void ConversationMgr::CancelConversationsForDeadParticipant(
    const ACDID idACD)
{
    SGame& tSGame = SGameManager::GameGet();
    ConversationMgrGlobals* ptConvGlobals = tSGame.ptConversationMgrGlobals;
    if (ptConvGlobals == NULL)
    {
        return;
    }

    Player* ptPlayer = PlayerGetByACD(idACD);
    int32 nPlayerIndex = (ptPlayer ? ptPlayer->nPlayerIndex : -1);

    XGrowableList<ConvCancelData, ConvCancelData&> tCancelList;

    POS pos = ptConvGlobals->tConversationList.GetHeadPosition();
    while (pos != NULL)
    {
        ActiveConversation& tConv = ptConvGlobals->tConversationList.GetNext(pos);
        if (tConv.fMarkedForRemoval)
        {
            continue;
        }

        if (!sACDIsInConversation(tConv, idACD))
        {
            continue;
        }


        ScopedConversationDefinitionLockR pdefConv(tConv.snoConversation);
        if (pdefConv->dwFlags & (1 << CF_PERSIST_THROUGH_SPEAKER_DEATH))
        {
            continue;
        }

        ConvCancelData tConvCancelData;
        tConvCancelData.ptConv = &tConv;

        tCancelList.AddTail_Tracked(tConvCancelData);
    }

    POS posCancel = tCancelList.GetHeadPosition();
    while (posCancel != NULL)
    {
        ConvCancelData& tConvCancelData = tCancelList.GetNext(posCancel);
        sCancelFloatConversation(*tConvCancelData.ptConv, nPlayerIndex, TRUE);
    }
}
