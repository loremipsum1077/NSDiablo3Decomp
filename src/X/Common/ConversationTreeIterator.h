// Original file: X/Common/ConversationTreeIterator.h (declarations sit on their DWARF lines).
// Only X/Common/ConversationTreeIterator.cpp includes it so far; other files keep local copies.
#pragma once
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "GameBalance.h"
#include "GameSwig.h"
#include "SNO/SNOFiles.h"
#include "Definitions/Definitions.h"
#include "../Sigma/Random.h"
#include "../Sigma/DataStructures/XPtr64.h"
#include "../Sigma/Files/XSerializeData.h"
struct ConversationDefinition; struct ConversationTreeNode;
typedef int32 ConversationSpeaker;  // X/Common/Definitions/ConversationDefinition.h
struct LineGroupData
{
    LineGroupData()
    {
        uFlags = 0;
        dwLineUID = 0;
        eLineSpeaker = -1;  // CONV_SPEAKER_INVALID (ConversationDefinition.h, declared further down)
        eSpeakerTarget = -1;  // CONV_SPEAKER_INVALID
        dwAnimTag = 0;
        nDisplayTimeAdjustment = 0;
    }

    uint32 uFlags;  // 0x00
    uint32 dwLineUID;  // 0x04
    ConversationSpeaker eLineSpeaker;  // 0x08
    ConversationSpeaker eSpeakerTarget;  // 0x0c
    uint32 dwAnimTag;  // 0x10
    int32 nDisplayTimeAdjustment;  // 0x14
};

struct ConversationTreeIterItem
{
    int32 nCurNode;  // 0x00
    int32 nNodeCount;  // 0x04
    BOOL bVisited;  // 0x08
    ConversationTreeNode * arTreeNodes;  // 0x10

    LineGroupData m_tLineGroupData;  // 0x18
};

class ConversationTreeIter
{
public:
    ConversationTreeIter();


    ConversationTreeIter(const SNO snoConv) { Init(snoConv, 0, 0, NULL, FALSE); }  // DWARF line 51; body from the out-of-line copy _ZN20ConversationTreeIterC2Ei (0x6116e0)





    ConversationTreeIter(const SNO snoConv, BOOL bForWrite);  // defined elsewhere (not in this CU)






    ConversationTreeIter(const ConversationDefinition * pdefConv);


    void Init(const SNO snoConv, uint32 dwConditions, uint32 dwCheckConditions, RNG * ptRNG, BOOL bForWrite);






    ConversationTreeNode * GetNext(const ACDID * pidACDParticipants, const SNO snoConversation);



    void Stop();


    void Cancel();


    ConversationTreeNode * GetCurrentNode() const { return m_ptCurNode; }  // body inferred from the name (copy in ConversationDefinition.cpp)





    const LineGroupData * GetLineGroupData() const { return &m_ptCurItem->m_tLineGroupData; }  // body inferred likewise




private:

    void sInitShared(const ConversationDefinition * ptConv, const uint32 dwConditions, const uint32 dwCheckConditions, RNG * ptRNG);




    void sAdvance();

    void sPushArrayToStack(ConversationTreeNode * arTreeNodes, const int32 nCount, const LineGroupData & tLineGroupData);



    BOOL sCheckCondition(ConversationTreeNode * ptCurNode, BOOL bCheckValue);



    int32 sSelectRandomNode(const ACDID * pidACDParticipants, const SNO snoConversation, ConversationTreeNode * ptCurNode, int32 nChildNodeCount);





    SNO m_snoConversation;  // 0x00
    BOOL m_bForWrite;  // 0x04
    const ConversationDefinition * m_ptConvDef;  // 0x08
    ConversationTreeIterItem * m_ptCurItem;  // 0x10
    ConversationTreeNode * m_ptCurNode;  // 0x18

    uint32 m_dwConditions;  // 0x20
    uint32 m_dwCheckConditions;  // 0x24
    RNG * m_ptRNG;  // 0x28

    int32 m_nIterStackSize;  // 0x30
    ConversationTreeIterItem m_arIterStack[16];  // 0x38, MAX_CONV_TREE_DEPTH
};
static_assert(sizeof(ConversationTreeIter) == 0x338, "ConversationTreeIter");
static_assert(sizeof(ConversationTreeIterItem) == 0x30, "ConversationTreeIterItem");
static_assert(sizeof(LineGroupData) == 0x18, "LineGroupData");

// The types below belong to X/Common/Definitions/ConversationDefinition.h, whose src copy has not
// adopted them yet (several .cpp files carry their own copies, so adding them there would clash).
// Copied from src/X/Common/Definitions/ConversationDefinition.cpp; layout checked by the asserts.
enum ConversationNodeType {
    CONV_NODE_TYPE_CONDITIONAL = 1,
    CONV_NODE_TYPE_CHOICE_DEPRECATED = 2,
    CONV_NODE_TYPE_CONV_LINE_GROUP = 3,
    CONV_NODE_TYPE_RANDOM = 4,
    CONV_NODE_TYPE_CONV_LINE = 5,
    CONV_NODE_TYPE_CONDITIONAL_PARTICIPANT = 6,
};

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
typedef int32 ConversationVarSetOp;

enum {
    CTNF_DONT_ANIMATE_BIT = 1,
    CTNF_NEEDS_REVIEW_BIT = 3,
    CTNF_PLEASE_REVISE_BIT = 4,
    CTNF_DONT_PLAY_SUBTITLE_BIT = 5,
    CTNF_NEEDS_TEXT_TO_SPEECH_BIT = 6,
    CTNF_RANDOM_PLAYS_ONLY_ONCE = 7,
};

struct CompressedConvLocaleDisplayTimes;

struct ConversationTreeNode {  // size 0x88
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

typedef int32 ConversationType;
typedef int32 ConversationIcon;

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
