// Original file: X/Common/Definitions/ConversationDefinition.cpp

#include "ConversationDefinition.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../Server/Powers/Powers.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../ActorCommonData.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Game.h"
#include "../GameSwig.h"
#include "../HirelingSwig.h"
#include "../Player.h"
#include "../TagMaps/ActorTags.h"
#include "../TagMaps/Tags.h"
#include "ActorDefinition.h"
#include "StringListDefinition.h"

// The definition structs below belong to X/Common/Definitions/ConversationDefinition.h and
// X/Common/ConversationTreeIterator.h. Several files in src/ still carry their own local copies, so
// they are kept here until those copies are removed.
enum {
    PLAYERFLAG_MAX_LENGTH = 128,
    MAX_CONV_TREE_DEPTH = 16,
    CONV_VARIABLE_MAX_LENGTH = 64,
    MAX_CONVERSATION_UNLOCKS = 3,
};

enum ConversationNodeType {
    CONV_NODE_TYPE_CONDITIONAL = 1,
    CONV_NODE_TYPE_CHOICE_DEPRECATED = 2,
    CONV_NODE_TYPE_CONV_LINE_GROUP = 3,
    CONV_NODE_TYPE_RANDOM = 4,
    CONV_NODE_TYPE_CONV_LINE = 5,
    CONV_NODE_TYPE_CONDITIONAL_PARTICIPANT = 6,
};

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

typedef int32 ConversationSpeaker;

enum {
    CONV_VAR_CHECK_OP_INVALID = -1,
    CONV_VAR_CHECK_OP_EQUAL = 0,
    CONV_VAR_CHECK_OP_LESS_THAN = 1,
    CONV_VAR_CHECK_OP_GREATER_THAN = 2,
    CONV_VAR_CHECK_OP_LEQ = 3,
    CONV_VAR_CHECK_OP_GEQ = 4,
    CONV_VAR_CHECK_OP_NOT_EQUAL = 5,
    CONV_VAR_CHECK_OP_COUNT = 6,
};

typedef int32 ConversationVarCheckOp;

enum {
    CONV_VAR_SET_OP_INVALID = -1,
    CONV_VAR_SET_OP_INC = 0,
    CONV_VAR_SET_OP_DEC = 1,
    CONV_VAR_SET_OP_SET = 2,
    CONV_VAR_SET_OP_COUNT = 3,
};

typedef int32 ConversationVarSetOp;

enum {
    CTNF_DONT_ANIMATE_BIT = 1,
    CTNF_NEEDS_REVIEW_BIT = 3,
    CTNF_PLEASE_REVISE_BIT = 4,
    CTNF_DONT_PLAY_SUBTITLE_BIT = 5,
    CTNF_NEEDS_TEXT_TO_SPEECH_BIT = 6,
    CTNF_RANDOM_PLAYS_ONLY_ONCE = 7,
};

struct ConvLocaleDisplayTimes {  // size 0x38
    uint32 dwDisplayTime[14];  // 0x00
};
static_assert(sizeof(ConvLocaleDisplayTimes) == 0x38, "ConvLocaleDisplayTimes");

struct CompressedConvLocaleDisplayTimes {  // size 0x4
    uint32 eLocale : 5;  // 0x00
    uint32 eClass : 3;  // 0x00
    uint32 eGender : 2;  // 0x00
    uint32 dwValue : 22;  // 0x00
};
static_assert(sizeof(CompressedConvLocaleDisplayTimes) == 0x4, "CompressedConvLocaleDisplayTimes");

struct ConversationTreeNode {  // size 0x88
    void Free();
    ConversationNodeType eConvNodeType;  // 0x00
    uint32 uFlags;  // 0x04
    uint32 dwLineUID;  // 0x08
    ConversationSpeaker eLineSpeaker;  // 0x0c
    ConversationSpeaker eSpeakerTarget;  // 0x10
    uint32 dwAnimTag;  // 0x14
    int32 nDisplayTimeAdjustment;  // 0x18
    int32 nLastPlayedNode;  // 0x1c
    ActorGender eGender;  // 0x20
    PlayerClass ePlayerClass;  // 0x24
    XPtr64<CompressedConvLocaleDisplayTimes> ptCompressedDisplayTimes;  // 0x28
    SerializeData serCompressedDisplayTimes;  // 0x30
    GBID gbidConvVarCheck;  // 0x38
    ConversationVarCheckOp eConvVarCheckOp;  // 0x3c
    int32 nConvVarCheckVal;  // 0x40
    GBID gbidConvVarSet;  // 0x44
    ConversationVarSetOp eConvVarSetOp;  // 0x48
    int32 nConvVarSetVal;  // 0x4c
    int32 nBranchIndex;  // 0x50
    uint32 dwWeight;  // 0x54
    XPtr64<ConversationTreeNode> arTrueNodes;  // 0x58
    SerializeData serTrueNodes;  // 0x60
    XPtr64<ConversationTreeNode> arFalseNodes;  // 0x68
    SerializeData serFalseNodes;  // 0x70
    XPtr64<ConversationTreeNode> arChildNodes;  // 0x78
    SerializeData serChildNodes;  // 0x80
};
static_assert(sizeof(ConversationTreeNode) == 0x88, "ConversationTreeNode");

enum {
    CONV_TYPE_FOLLOWER_SOUNDSET = 0,
    CONV_TYPE_PLAYER_EMOTE = 1,
    CONV_TYPE_AMBIENT_FLOAT = 2,
    CONV_TYPE_FOLLOWER_BANTER = 3,
    CONV_TYPE_FOLLOWER_CALLOUT = 4,
    CONV_TYPE_PLAYER_CALLOUT = 5,
    CONV_TYPE_GLOBAL_CHATTER = 6,
    CONV_TYPE_GLOBAL_FLOAT = 7,
    CONV_TYPE_LORE = 8,
    CONV_TYPE_AMBIENT_GOSSIP = 9,
    CONV_TYPE_TALK_MENU = 10,
    CONV_TYPE_QUEST_STANDARD = 11,
    CONV_TYPE_QUEST_FLOAT = 12,
    CONV_TYPE_QUEST_EVENT = 13,
    NUM_CONVERSATION_TYPES = 14,
};

typedef int32 ConversationType;

typedef int32 ConversationPriority;

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
    CONV_ICON_INVALID = -1,
    CONV_ICON_NONE = 0,
    CONV_ICON_EXCLAMATION = 1,
    CONV_ICON_QUESTION = 2,
    CONV_ICON_ASTERISK = 3,
    CONV_ICON_GOSSIP = 4,
    CONV_ICON_EXCLAMATION_BONUS = 5,
    NUM_CONVERSATION_ICONS = 6,
};

typedef int32 ConversationIcon;

enum {
    _DEPRECATED_CF_ACTIVATE_QUEST_BIT = 0,
    _DEPRECATED_CF_GRANT_REWARD_BIT = 1,
    _DEPRECATED_CF_IMPORTANT_BIT = 2,
    _DEPRECATED_CF_QUESTION_BIT = 3,
    _DEPRECATED_CF_ASTERISK_BIT = 4,
    _DEPRECATED_CF_REOPERATE_NPC_BIT = 5,
    _DEPRECATED_CF_TALK_MENU_ONLY_BIT = 6,
    _DEPRECATED_CF_GOSSIP_BIT = 7,
    CF_NO_VOICEOVER_BIT = 8,
    _DEPRECATED_CF_PLAY_FOR_ALL_PLAYERS_BIT = 9,
    _DEPRECATED_CF_ZOOM_BIT = 10,
    CF_OPERATE_SPEAKER_ON_BEGIN_BIT = 11,
    CF_READY_TO_RECORD_BIT = 12,
    CF_OPERATE_SPEAKER_ON_END_BIT = 13,
    CF_NEVER_REPEAT_BIT = 14,
    CF_CANCEL_FOR_COMBAT_BIT = 15,
    _DEPRECATED_CF_READY_FOR_LOCALIZATION_BIT = 16,
    _DEPRECATED_CF_RECORDING_HAS_BEGUN_BIT = 17,
    _DEPRECATED_CF_LOCALIZATION_HAS_BEGUN_BIT = 18,
    CF_READY_FOR_REVIEW_BIT = 19,
    CF_PERSIST_THROUGH_SPEAKER_DEATH = 20,
    CF_ALWAYS_IMPORTANT = 21,
    CF_CONSOLE_HIDE_CLOSE_BUTTON = 22,
    CF_SHOW_ALL_LINES_IN_RECORDING_SCRIPT = 23,
};

struct ConversationDefinition {  // size 0x110
    RequiredDefinitionHeader tHeader;  // 0x00
    ConversationType eConvType;  // 0x0c
    ConversationIcon eConvIcon;  // 0x10
    SNO snoConvPiggyback;  // 0x14
    SNO snoConvUnlocks[3];  // 0x18
    uint32 dwFlags;  // 0x24
    CHAR szSetPlayerFlag[128];  // 0x28
    SNO snoPrimaryNpc;  // 0xa8
    SNO snoAltNpc1;  // 0xac
    SNO snoAltNpc2;  // 0xb0
    SNO snoAltNpc3;  // 0xb4
    SNO snoAltNpc4;  // 0xb8
    uint32 dwLineUID;  // 0xbc
    XPtr64<ConversationTreeNode> arRootTreeNodes;  // 0xc0
    SerializeData serRootTreeNodes;  // 0xc8
    XPtr64<unsigned char> pCompiledScript;  // 0xd0
    SerializeData serCompiledScript;  // 0xd8
    SNO snoScript;  // 0xe0
    BOOL fSpeakerUsed[9];  // 0xe4
    SNO snoBossEncounter;  // 0x108
    uint32 dwPad;  // 0x10c
};
static_assert(sizeof(ConversationDefinition) == 0x110, "ConversationDefinition");

typedef ScopedDefinitionLockR<ConversationDefinition> ScopedConversationDefinitionLockR;

template <> inline const ConversationDefinition * DefinitionLockR<ConversationDefinition>(SNO sno) {  // line 462
    return (const ConversationDefinition *)gptGroupDefConversation->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<ConversationDefinition>(const ConversationDefinition * pdef) {  // line 463
    void * lpDef = (void *)pdef;
    gptGroupDefConversation->UnlockR(lpDef);
}

// X/Common/ConversationTreeIterator.h.
struct LineGroupData {  // size 0x18
    LineGroupData() : uFlags(0), dwLineUID(0), eLineSpeaker(CONV_SPEAKER_INVALID), eSpeakerTarget(CONV_SPEAKER_INVALID), dwAnimTag(0), nDisplayTimeAdjustment(0) {}  // line 17
    uint32 uFlags;  // 0x00
    uint32 dwLineUID;  // 0x04
    ConversationSpeaker eLineSpeaker;  // 0x08
    ConversationSpeaker eSpeakerTarget;  // 0x0c
    uint32 dwAnimTag;  // 0x10
    int32 nDisplayTimeAdjustment;  // 0x14
};
static_assert(sizeof(LineGroupData) == 0x18, "LineGroupData");

struct ConversationTreeIterItem {  // size 0x30
    ConversationTreeIterItem() {}  // line 35
    int32 nCurNode;  // 0x00
    int32 nNodeCount;  // 0x04
    BOOL bVisited;  // 0x08
    ConversationTreeNode * arTreeNodes;  // 0x10
    LineGroupData m_tLineGroupData;  // 0x18
};
static_assert(sizeof(ConversationTreeIterItem) == 0x30, "ConversationTreeIterItem");

class ConversationTreeIter {  // size 0x338
public:
    ConversationTreeIter();
    ConversationTreeIter(const SNO snoConv) { Init(snoConv, 0, 0, NULL, FALSE); }  // line 51
    ConversationTreeIter(const SNO, BOOL);
    ConversationTreeIter(const ConversationDefinition *);
    void Init(const SNO snoConv, uint32 dwConditions, uint32 dwCheckConditions, RNG * ptRNG, BOOL bForWrite);
    ConversationTreeNode * GetNext(const ACDID * pidACDParticipants, const SNO snoConversation);
    void Stop();
    void Cancel();
    ConversationTreeNode * GetCurrentNode() const { return m_ptCurNode; }  // line 84
    const LineGroupData * GetLineGroupData() const { return &m_ptCurItem->m_tLineGroupData; }  // line 90
private:
    SNO m_snoConversation;  // 0x00
    BOOL m_bForWrite;  // 0x04
    const ConversationDefinition * m_ptConvDef;  // 0x08
    ConversationTreeIterItem * m_ptCurItem;  // 0x10
    ConversationTreeNode * m_ptCurNode;  // 0x18
    uint32 m_dwConditions;  // 0x20
    uint32 m_dwCheckConditions;  // 0x24
    RNG * m_ptRNG;  // 0x28
    int32 m_nIterStackSize;  // 0x30
    ConversationTreeIterItem m_arIterStack[16];  // 0x38
};
static_assert(sizeof(ConversationTreeIter) == 0x338, "ConversationTreeIter");

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;  // X/Sigma/Files/XFilesCommon.cpp:20
extern const XTypeDescriptor * DT_SNO;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp
extern const SymbolicEntry gConvSpeaker[11];  // X/Common/SNO/SymbolicValue.cpp:4621
extern const SymbolicEntry gConvVarOpCheck[8];  // :4640
extern const SymbolicEntry gConvVarOpSet[];  // X/Common/SNO/SymbolicValue.cpp
extern const SymbolicEntry gConvTypes[15];  // :4824
struct ScriptMessageMap;
SNO ScriptDefinitionCreateScript(uint8 * ptScript, uint32 dwScriptSize, ScriptMessageMap * arMessageMap, int32 nMessageMapCount, SNOName & snonameOwner, LPCSTR szChildFileName);  // X/Server/Definitions/ScriptDefinition.cpp:871
void ScriptDefinitionDisposeScript(SNO snoScript);  // :906
SNO HirelingGetSNO(HirelingClass eClass, HirelingType eType);  // X/Common/Hireling.cpp:252
StringTableEntry * ConversationChooseStringTableEntry(const StringListDefinition * pdefStringList, const uint32 uLineUID, const ConversationSpeaker eLineSpeaker, const PlayerClass eLineClass, const ActorGender eLineGender, const PlayerClass ePlayerClass, const ActorGender ePlayerGender);  // X/Client/ConversationUtils.cpp:172
BOOL ConversationChooseSoundForLine(SNO & snoSound, int32 & nPermIndex, const SNO snoConversation, const uint32 uLineUID, const ConversationSpeaker eLineSpeaker, const PlayerClass eLineClass, const ActorGender eLineGender, const PlayerClass ePlayerClass, const ActorGender ePlayerGender, const BOOL fReportErrors);  // :365
extern const XTypeDescriptor * DT_CONVERSATION_TREE_NODE;
SNO ConversationGetStringListSNO(SNO snoConv);
HirelingClass ConversationSpeakerToHirelingClass(ConversationSpeaker eSpeaker);
BOOL Conversation_GetFirstLineSound(const SNO snoConv, const Player * ptPlayer, SNO & snoSound, int32 & nPermIndex);


const float MAX_CONVERSATION_DISTANCE = 20.0f;  // line 34

LPCSTR gszConvConditionalRootFuncName = "ConvConditional";  // line 71
LPCSTR gszConvConditionFuncName = "ConvCondition";
LPCSTR gszConvMenuTextLabel = "MenuLabel";
LPCSTR gszConvStringListPrefix = "Conv_";
LPCSTR gszConvRootLineLabel = "Line";

static const TypeDescriptorField ConvLocaleDisplayTimes_Fields[2] = {  // line 81
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(ConvLocaleDisplayTimes, dwDisplayTime), NULL, 1, 14, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ConvLocaleDisplayTimes), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConvLocaleDisplayTimes("ConvLocaleDisplayTimes", 0, ConvLocaleDisplayTimes_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 83
const XTypeDescriptor * DT_CONV_LOCALE_DISPLAY_TIMES = &TypeDescriptor_ConvLocaleDisplayTimes;

#define TN(f) offsetof(ConversationTreeNode, f)
static const TypeDescriptorField ConversationTreeNode_Fields[27] = {  // line 86
    TypeDescriptorField("", DT_INT, TN(eConvNodeType), NULL, 1),
    TypeDescriptorField("", DT_INT, TN(uFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, TN(dwLineUID), NULL, 1),
    EnumTypeDescriptorField("", TN(eLineSpeaker), NULL, gConvSpeaker),
    EnumTypeDescriptorField("", TN(eSpeakerTarget), (const void *)-1, gConvSpeaker),
    TypeDescriptorField("", DT_INT, TN(dwAnimTag), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, TN(eGender), NULL, 1),
    TypeDescriptorField("", DT_INT, TN(ePlayerClass), NULL, 1),
    TypeDescriptorField("", DT_TIME, TN(nDisplayTimeAdjustment), NULL, 1),
    TypeDescriptorField("", DT_INT, TN(nLastPlayedNode), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, TN(serCompressedDisplayTimes), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_INT, TN(ptCompressedDisplayTimes), 0x700021, (int32)TN(serCompressedDisplayTimes) - (int32)TN(ptCompressedDisplayTimes)),
    TypeDescriptorField("", DT_INT, TN(gbidConvVarCheck), NULL, 0),
    EnumTypeDescriptorField("", TN(eConvVarCheckOp), NULL, gConvVarOpCheck),
    TypeDescriptorField("", DT_INT, TN(nConvVarCheckVal), NULL, 1),
    TypeDescriptorField("", DT_INT, TN(gbidConvVarSet), NULL, 0),
    EnumTypeDescriptorField("", TN(eConvVarSetOp), NULL, gConvVarOpSet),
    TypeDescriptorField("", DT_INT, TN(nConvVarSetVal), NULL, 1),
    TypeDescriptorField("", DT_INT, TN(nBranchIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, TN(dwWeight), (const void *)100, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, TN(serTrueNodes), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_CONVERSATION_TREE_NODE, TN(arTrueNodes), 0x21, (int32)TN(serTrueNodes) - (int32)TN(arTrueNodes)),
    TypeDescriptorField("", DT_SERIALIZEDATA, TN(serFalseNodes), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_CONVERSATION_TREE_NODE, TN(arFalseNodes), 0x21, (int32)TN(serFalseNodes) - (int32)TN(arFalseNodes)),
    TypeDescriptorField("", DT_SERIALIZEDATA, TN(serChildNodes), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_CONVERSATION_TREE_NODE, TN(arChildNodes), 0x21, (int32)TN(serChildNodes) - (int32)TN(arChildNodes)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ConversationTreeNode), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConversationTreeNode("ConversationTreeNode", 3, ConversationTreeNode_Fields, 27, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 115
const XTypeDescriptor * DT_CONVERSATION_TREE_NODE = &TypeDescriptor_ConversationTreeNode;

#define CD(f) offsetof(ConversationDefinition, f)
static const TypeDescriptorField ConversationDefinition_Fields[19] = {  // line 118
    EnumTypeDescriptorField("", CD(eConvType), NULL, gConvTypes),
    TypeDescriptorField("", DT_INT, CD(eConvIcon), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, CD(snoConvPiggyback), "", CONVERSATION_SNO, 0, -1),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, CD(snoConvUnlocks), NULL, 0, MAX_CONVERSATION_UNLOCKS, CONVERSATION_SNO, 0),
    TypeDescriptorField("", DT_INT, CD(dwFlags), NULL, 0x80001),
    CharArrayTypeDescriptorField("", CD(szSetPlayerFlag), "", 1, PLAYERFLAG_MAX_LENGTH, -1),
    FilterTypeDescriptorField("", DT_SNO, CD(snoPrimaryNpc), "", ACTOR_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, CD(snoAltNpc1), "", ACTOR_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, CD(snoAltNpc2), "", ACTOR_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, CD(snoAltNpc3), "", ACTOR_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, CD(snoAltNpc4), "", ACTOR_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, CD(dwLineUID), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, CD(serRootTreeNodes), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_CONVERSATION_TREE_NODE, CD(arRootTreeNodes), 0x21, (int32)CD(serRootTreeNodes) - (int32)CD(arRootTreeNodes)),
    TypeDescriptorField("", DT_SERIALIZEDATA, CD(serCompiledScript), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BYTE, CD(pCompiledScript), 0x20, (int32)CD(serCompiledScript) - (int32)CD(pCompiledScript)),
    FilterTypeDescriptorField("", DT_SNO, CD(snoBossEncounter), "", BOSS_ENCOUNTER_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, CD(dwPad), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ConversationDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConversationDefinition("Conversation", 19, ConversationDefinition_Fields, 19, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 146
const XTypeDescriptor * DT_CONVERSATIONDEF = &TypeDescriptor_ConversationDefinition;






















































ConversationGroupDef * gptGroupDefConversation;




ConversationGroupDef::ConversationGroupDef()
    : GroupDefinition("Conversation", CONVERSATION_SNO,

                      SNOVersions(99, 103, 99, 101, 100),
                      SNODATATYPE_TITLE, sizeof(ConversationDefinition), DT_CONVERSATIONDEF, 0x4441, 0x500e)
{
}



















uint32 ConversationGroupDef::GetMaxCount()
{
    return 32;
}



























































































































































































BOOL ConversationGroupDef::Write(void * lpDefinition, FileReference & tRef) const
{




    XASSERT(FALSE);
    return FALSE;
}






void ConversationGroupDef::GetDefaultData(void * ptDefaultDef) const
{
    ConversationDefinition * ptConvDef = (ConversationDefinition *)ptDefaultDef;
    SigmaMemoryClear(ptConvDef, sizeof(ConversationDefinition));



    ptConvDef->snoBossEncounter = SNO_NONE;
}




void ConversationGroupDef::Postprocess(void * lpDefinition)
{

    GroupDefinition::Postprocess(lpDefinition);

    ConversationDefinition * ptConvDef = (ConversationDefinition *)lpDefinition;


    ptConvDef->snoScript = SNO_NONE;

    if (!(ptConvDef->tHeader.dwFlags & (1 << 5)))
    {
        SNOName snoname(CONVERSATION_SNO, ptConvDef->tHeader.snoHandle);




        ptConvDef->snoScript = ScriptDefinitionCreateScript(ptConvDef->pCompiledScript, ptConvDef->serCompiledScript.dwSize, NULL, 0, snoname, "0000.lua");
    }





    SigmaMemoryClear(ptConvDef->fSpeakerUsed, sizeof(ptConvDef->fSpeakerUsed));

    ConversationTreeIter tIter(ptConvDef);
    while (tIter.GetNext(NULL, SNO_NONE))
    {
        ConversationTreeNode * ptCurNode = tIter.GetCurrentNode();
        ptCurNode->nLastPlayedNode = -1;

        if (ptCurNode->eConvNodeType == CONV_NODE_TYPE_CONV_LINE_GROUP)
        {




            ptConvDef->fSpeakerUsed[ptCurNode->eLineSpeaker] = TRUE;
        }
    }
    tIter.Cancel();
}











































































































































































void ConversationGroupDef::Delete(void * lpDefinition) const
{
    ConversationDefinition * ptConvDef = (ConversationDefinition *)lpDefinition;


    if (ptConvDef->snoScript != SNO_NONE)
    {



        if (ServerIsLocal() && AppServerThreadIsRunningGame())
        {
            Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.tConversationDefPurgeMutex);
            sgtAppGlobals.ptConversationDefsPurged->Add_Tracked(ptConvDef->snoScript);
        }
        else
        {
            ScriptDefinitionDisposeScript(ptConvDef->snoScript);
        }



        ptConvDef->snoScript = SNO_NONE;
    }










    GroupDefinition::Delete(lpDefinition);
}




















































































































void ConversationGetTitleLabel(SNO snoConv, CRefString & sLabel, SNO & snoStringList)



{
    snoStringList = ConversationGetStringListSNO(snoConv);
    sLabel = gszConvMenuTextLabel;
}




SNO ConversationGetStringListSNO(SNO snoConv)

{
    CRefString sSnoName = SNOToString(CONVERSATION_SNO, snoConv, FALSE);
    CRefString sStringListSnoName(gszConvStringListPrefix);
    sStringListSnoName += sSnoName;

    SNO snoStringList = SNOFileLookupGUID(STRINGLIST_SNO, sStringListSnoName.str());







    return snoStringList;
}



CRefString ConversationGetSoundName(const SNO snoConversation,
                                    const LPCSTR szSpeaker)

{

    return CRefString().Format("D_%s_%s", SNOToString(CONVERSATION_SNO, snoConversation, FALSE).str(), szSpeaker);
}
















































































































































SNO ConversationSpeakerToSNO(const ConversationDefinition * ptConvDef,
                             ConversationSpeaker eSpeaker)

{
    switch (eSpeaker)
    {

    case CONV_SPEAKER_PRIMARY_NPC: return ptConvDef->snoPrimaryNpc;
    case CONV_SPEAKER_ALT_NPC1: return ptConvDef->snoAltNpc1;
    case CONV_SPEAKER_ALT_NPC2: return ptConvDef->snoAltNpc2;
    case CONV_SPEAKER_ALT_NPC3: return ptConvDef->snoAltNpc3;
    case CONV_SPEAKER_ALT_NPC4: return ptConvDef->snoAltNpc4;

    case CONV_SPEAKER_FOLLOWER_TEMPLAR:
    case CONV_SPEAKER_FOLLOWER_SCOUNDREL:
    case CONV_SPEAKER_FOLLOWER_ENCHANTRESS:
        return HirelingGetSNO(ConversationSpeakerToHirelingClass(eSpeaker), 0);
    }


    return SNO_NONE;
}



HirelingClass ConversationSpeakerToHirelingClass(ConversationSpeaker eSpeaker)

{
    switch (eSpeaker)
    {
    case CONV_SPEAKER_FOLLOWER_TEMPLAR:
        return 1;
    case CONV_SPEAKER_FOLLOWER_SCOUNDREL:
        return 2;
    case CONV_SPEAKER_FOLLOWER_ENCHANTRESS:
        return 3;
    }
    return 0;
}





SNO ConversationSpeakerToSNO(SNO snoConv,
                             ConversationSpeaker eSpeaker)

{
    ScopedConversationDefinitionLockR ptConvDef(snoConv);

    return ConversationSpeakerToSNO(ptConvDef, eSpeaker);
}




uint32 ConversationTypeGetUsageFlags(const ConversationType eConvType)

{
    switch (eConvType)
    {
    case CONV_TYPE_FOLLOWER_SOUNDSET:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT) | (1 << CTU_NEVER_ANIMATES_BIT);
    case CONV_TYPE_PLAYER_EMOTE:
        return (1 << CTU_IS_GLOBAL_BIT) | (1 << CTU_DOES_NOT_CANCEL_OTHERS_BIT) | (1 << CTU_INTERRUPTS_EQUAL_TYPE_BIT) | (1 << CTU_NEVER_SILENCE_AUDIO_BIT) | (1 << CTU_NEVER_ANIMATES_BIT);
    case CONV_TYPE_AMBIENT_FLOAT:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_CANCELS_BEYOND_LONG_RANGE_BIT) | (1 << CTU_SHOWN_IN_TALK_MENU_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT);
    case CONV_TYPE_FOLLOWER_BANTER:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_CANCELS_BEYOND_LONG_RANGE_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT);
    case CONV_TYPE_FOLLOWER_CALLOUT:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_CANCELS_BEYOND_LONG_RANGE_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT);
    case CONV_TYPE_PLAYER_CALLOUT:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_CANCELS_BEYOND_LONG_RANGE_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT);
    case CONV_TYPE_GLOBAL_CHATTER:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_CANCELS_BEYOND_LONG_RANGE_BIT) | (1 << CTU_IS_GLOBAL_BIT) | (1 << CTU_DOES_NOT_CANCEL_OTHERS_BIT) | (1 << CTU_STARTS_IMMEDIATELY_ON_NPC_OPERATE_BIT) | (1 << CTU_SHOWN_IN_TALK_MENU_BIT) | (1 << CTU_NPC_UNUSABLE_DURING_CONVERSATION_BIT) | (1 << CTU_TRIGGER_FOLLOWER_CONVERSATION_COOLDOWN_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT) | (1 << CTU_NEVER_SILENCE_AUDIO_BIT);
    case CONV_TYPE_GLOBAL_FLOAT:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_CANCELS_BEYOND_LONG_RANGE_BIT) | (1 << CTU_IS_GLOBAL_BIT) | (1 << CTU_STARTS_IMMEDIATELY_ON_NPC_OPERATE_BIT) | (1 << CTU_SHOWN_IN_TALK_MENU_BIT) | (1 << CTU_NPC_UNUSABLE_DURING_CONVERSATION_BIT) | (1 << CTU_TRIGGER_FOLLOWER_CONVERSATION_COOLDOWN_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT);
    case CONV_TYPE_LORE:
        return 0;
    case CONV_TYPE_AMBIENT_GOSSIP:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_CANCELS_BEYOND_CLOSE_RANGE_BIT) | (1 << CTU_STARTS_IMMEDIATELY_ON_NPC_OPERATE_BIT) | (1 << CTU_ALWAYS_PLAY_IMMEDIATELY_BIT) | (1 << CTU_NPC_UNUSABLE_DURING_CONVERSATION_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT);
    case CONV_TYPE_TALK_MENU:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_ALLOWS_MANUAL_ADVANCE_BIT) | (1 << CTU_CANCELS_BEYOND_CLOSE_RANGE_BIT) | (1 << CTU_SHOWN_IN_TALK_MENU_BIT) | (1 << CTU_NPC_UNUSABLE_DURING_CONVERSATION_BIT) | (1 << CTU_TRIGGER_FOLLOWER_CONVERSATION_COOLDOWN_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT);
    case CONV_TYPE_QUEST_STANDARD:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_ALLOWS_MANUAL_ADVANCE_BIT) | (1 << CTU_CANCELS_BEYOND_CLOSE_RANGE_BIT) | (1 << CTU_STARTS_IMMEDIATELY_ON_NPC_OPERATE_BIT) | (1 << CTU_SHOWN_IN_TALK_MENU_BIT) | (1 << CTU_NPC_UNUSABLE_DURING_CONVERSATION_BIT) | (1 << CTU_TRIGGER_FOLLOWER_CONVERSATION_COOLDOWN_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT) | (1 << CTU_NO_AUDIO_ROLLOFF_BIT);
    case CONV_TYPE_QUEST_FLOAT:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_ALLOWS_MANUAL_ADVANCE_BIT) | (1 << CTU_IS_GLOBAL_BIT) | (1 << CTU_STARTS_IMMEDIATELY_ON_NPC_OPERATE_BIT) | (1 << CTU_SHOWN_IN_TALK_MENU_BIT) | (1 << CTU_NPC_UNUSABLE_DURING_CONVERSATION_BIT) | (1 << CTU_TRIGGER_FOLLOWER_CONVERSATION_COOLDOWN_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT) | (1 << CTU_NO_AUDIO_ROLLOFF_BIT);
    case CONV_TYPE_QUEST_EVENT:
        return (1 << CTU_ALLOWS_MULTIPLE_SPEAKERS_BIT) | (1 << CTU_ALLOWS_MANUAL_ADVANCE_BIT) | (1 << CTU_IS_GLOBAL_BIT) | (1 << CTU_STARTS_IMMEDIATELY_ON_NPC_OPERATE_BIT) | (1 << CTU_SHOWN_IN_TALK_MENU_BIT) | (1 << CTU_NPC_UNUSABLE_DURING_CONVERSATION_BIT) | (1 << CTU_TRIGGER_FOLLOWER_CONVERSATION_COOLDOWN_BIT) | (1 << CTU_YIELDS_TO_PLAYER_INITIATED_BIT) | (1 << CTU_NO_AUDIO_ROLLOFF_BIT);
    }


















































































    return 0;
}






static const ConversationPriority gcnTypeToPriority[NUM_CONVERSATION_TYPES] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 5, 6, 6 };

















ConversationPriority ConversationTypeGetPriority(const ConversationType eConvType)

{
    return gcnTypeToPriority[eConvType];
}




LPCSTR ConversationGetPlayerClassShortString(PlayerClass ePlayerClass)

{
    if (ePlayerClass != -1)
    {
        return PlayerGetShortClassName(ePlayerClass);
    }
    return "";
}













LPCSTR ConversationGetGenderString(ActorGender eGender)

{
    return eGender == 0 ? "M" : eGender == 1 ? "F" : "";
}

























CRefString ConversationGetSpeakerNameForPlayer(PlayerClass ePlayerClass,
                                               ActorGender eGender)

{
    CRefString sPlayerName(ConversationGetPlayerClassShortString(ePlayerClass));
    sPlayerName += ConversationGetGenderString(eGender);
    return sPlayerName;
}
















































































CRefString Conversation_GetFirstLine(const SNO snoConv,
                                     const Player * ptPlayer)

{
    if (snoConv == SNO_NONE)
    {
        return "";
    }
    PlayerClass ePlayerClass = -1;
    ActorGender ePlayerGender = -1;

    if (ptPlayer)
    {
        ePlayerGender = PlayerIsMale(*ptPlayer) ? 0 : 1;
        ePlayerClass = ptPlayer->ePlayerClassPortrait;
    }
    uint32 dwLastUID = 0;
    ConversationSpeaker eLastSpeaker = CONV_SPEAKER_INVALID;


    CRefString sLineText;
    ConversationTreeIter tIter(snoConv);
    while (tIter.GetNext(NULL, SNO_NONE))
    {
        const ConversationTreeNode * ptCurNode = tIter.GetCurrentNode();


        if (ptCurNode->eConvNodeType == CONV_NODE_TYPE_CONV_LINE_GROUP)
        {
            dwLastUID = ptCurNode->dwLineUID;
            eLastSpeaker = ptCurNode->eLineSpeaker;
            continue;
        }
        if (ptCurNode->eConvNodeType != CONV_NODE_TYPE_CONV_LINE)
            continue;
        if ((ptCurNode->ePlayerClass == -1 || ptCurNode->ePlayerClass == ePlayerClass) &&
            (ptCurNode->eGender == -1 || ptCurNode->eGender == ePlayerGender))
        {
            SNO snoStringList = ConversationGetStringListSNO(snoConv);
            ScopedStringListDefinitionLockR pdefStringList(snoStringList);
            StringTableEntry * ptTableEntry = ConversationChooseStringTableEntry(pdefStringList, dwLastUID, eLastSpeaker, ptCurNode->ePlayerClass, ptCurNode->eGender, ePlayerClass, ePlayerGender);

            if (ptTableEntry)
            {
                sLineText = ptTableEntry->szText;
                tIter.Cancel();
                break;
            }
        }
    }
    tIter.Cancel();

    return sLineText;
}




























BOOL Conversation_GetFirstLineSound(const SNO snoConv,
                                    const Player * ptPlayer,
                                    SNO & snoSound,
                                    int32 & nPermIndex)
{
    snoSound = SNO_NONE;
    nPermIndex = -1;
    if (snoConv == SNO_NONE)
    {
        return FALSE;
    }
    PlayerClass ePlayerClass = -1;
    ActorGender ePlayerGender = -1;



    if (ptPlayer)
    {
        ePlayerGender = PlayerIsMale(*ptPlayer) ? 0 : 1;
        ePlayerClass = ptPlayer->ePlayerClassPortrait;
    }

    ConversationTreeIter tIter(snoConv);
    while (tIter.GetNext(NULL, SNO_NONE))
    {
        const ConversationTreeNode * ptCurNode = tIter.GetCurrentNode();
        const LineGroupData * ptLineGroupData = tIter.GetLineGroupData();

        if (ptCurNode->eConvNodeType == CONV_NODE_TYPE_CONV_LINE &&
            (ptCurNode->ePlayerClass == -1 || ptCurNode->ePlayerClass == ePlayerClass) &&
            (ptCurNode->eGender == -1 || ptCurNode->eGender == ePlayerGender))
        {












            ScopedConversationDefinitionLockR pdefConv(snoConv);
            BOOL fSuccess = ConversationChooseSoundForLine(snoSound, nPermIndex, snoConv, ptLineGroupData->dwLineUID, ptLineGroupData->eLineSpeaker, ptCurNode->ePlayerClass, ptCurNode->eGender, ePlayerClass, ePlayerGender, FALSE);











            tIter.Cancel();
            return fSuccess;
        }
    }
    tIter.Cancel();
    return FALSE;
}


BOOL ConversationHasVO(const SNO snoConversation,
                       const Player * ptPlayer)
{
    int32 nPermIndex;
    ScopedConversationDefinitionLockR pdefConversation(snoConversation);
    if (pdefConversation->dwFlags & (1 << CF_NO_VOICEOVER_BIT))
    {
        return FALSE;
    }
    SNO snoSound;
    return Conversation_GetFirstLineSound(snoConversation, ptPlayer, snoSound, nPermIndex) != FALSE;
}






























BOOL ConversationPlayerNearNPC(ActorCommonData & tACDPlayer,
                               ActorCommonData & tACDNpc)
{
    const WorldLocation & tLocPlayer = tACDPlayer.GetWorldLocation();
    WorldLocation & tLocNPC = tACDNpc.GetWorldLocation();

    if (tLocPlayer.idSWorld != tLocNPC.idSWorld)
    {
        return FALSE;
    }



    if (ACDGetPetOwner(tACDNpc.id) == tACDPlayer.id)
    {
        return TRUE;
    }


    SNO snoSpeaker = tACDNpc.snoActor;
    ScopedActorDefinitionLockR pdefActor(snoSpeaker);
    BOOL fVOPlaysOnPlayer = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_VO_PLAYS_ON_PLAYER);
    if (fVOPlaysOnPlayer)
    {
        return TRUE;
    }

    return Vector3D::DistanceSquared(tLocPlayer.transform.wp, tLocNPC.transform.wp) <= MAX_CONVERSATION_DISTANCE * MAX_CONVERSATION_DISTANCE;
}
