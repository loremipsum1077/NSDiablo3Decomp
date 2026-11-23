// Original file: X/Common/ConversationTreeIterator.cpp
// Functions and locals sit on their DWARF lines (compile unit cu_053fc283).

#include "ConversationTreeIterator.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Server/SGame.h"
#include "../Server/SGameManager.h"
#include "../Sigma/Random.h"
#include "Definitions/ConversationDefinition.h"
#include <alloca.h>

BOOL GameActiveGameCommonDataIsServer();  // X/Common/Game.cpp:923










ConversationTreeIter::ConversationTreeIter()

{
	m_snoConversation = SNO_NONE;

	m_bForWrite = FALSE;
	m_ptConvDef = NULL;
	m_ptCurItem = NULL;
	m_ptCurNode = NULL;
	m_dwConditions = 0;
	m_dwCheckConditions = 0;
	m_ptRNG = NULL;
	m_nIterStackSize = 0;

}




ConversationTreeIter::ConversationTreeIter(
	const ConversationDefinition * pdefConv)
{
	m_snoConversation = SNO_NONE;
	m_bForWrite = FALSE;

	sInitShared(pdefConv, 0, 0, NULL);
}



void ConversationTreeIter::sInitShared(
    const ConversationDefinition * ptConv,
    const uint32 dwConditions,
    const uint32 dwCheckConditions,
    RNG * ptRNG)
{
    m_ptCurNode = NULL;
    m_ptCurItem = NULL;
    m_nIterStackSize = 0;
    m_ptConvDef = ptConv;

    m_ptRNG = ptRNG;
    m_dwConditions = dwConditions;
    m_dwCheckConditions = dwCheckConditions;

    LineGroupData tLineGroupData;
    int32 nCountToPush = ptConv->serRootTreeNodes.dwSize / sizeof(ConversationTreeNode);
    sPushArrayToStack(ptConv->arRootTreeNodes, nCountToPush, tLineGroupData);
}




void ConversationTreeIter::Init(
    const SNO snoConv,
    uint32 dwConditions,
    uint32 dwCheckConditions,
    RNG * ptRNG,
    BOOL bForWrite)
{


    m_snoConversation = snoConv;
    // Inference: the original stores 0 here whatever bForWrite is (stp w1, wzr at line 85);
    // lines 87-110 leave no code (a write-lock path compiled out of this build).
    m_bForWrite = FALSE;






















    const ConversationDefinition * ptConv = (const ConversationDefinition *)gptGroupDefConversation->LockR(snoConv, FALSE);
    sInitShared(ptConv, dwConditions, dwCheckConditions, ptRNG);



}



BOOL ConversationTreeIter::sCheckCondition(
    ConversationTreeNode * ptCurNode,
    BOOL bCheckValue)
{
    BOOL bIterate = TRUE;

    switch (ptCurNode->eConvNodeType)
    {
        case CONV_NODE_TYPE_CONDITIONAL:
        case CONV_NODE_TYPE_CONDITIONAL_PARTICIPANT:
            if (m_dwCheckConditions & (1 << ptCurNode->nBranchIndex))
            {
                bIterate = ((m_dwConditions & (1 << ptCurNode->nBranchIndex)) != 0) == bCheckValue;
            }
            break;
    }

    return bIterate;
}








static BOOL sIsNodeValidChoice(
    ConversationTreeNode * ptChildNode,
    ConversationTreeNode * ptCurNode,
    const ACDID * pidACDParticipants,
    const SNO snoConversation)
{



    if (GameActiveGameCommonDataIsServer())
    {
        if ((ptChildNode->uFlags & (1 << CTNF_RANDOM_PLAYS_ONLY_ONCE)) && snoConversation != SNO_NONE)
        {
            ConversationRandomLineKey tKey; tKey.Set(snoConversation, ptChildNode->dwLineUID);  // the original calls the (SNO, uint32) constructor (SGame.h:266), which src's SGame.h lacks
            BOOL bValue;
            if (SGameManager::GameGet().ptSGameGlobals->tConversationRandomLineMap.Lookup(tKey, bValue))
            {
                return FALSE;
            }
        }


        if (ptChildNode->eConvVarCheckOp != CONV_VAR_CHECK_OP_INVALID)
        {
            GBID gbidVar = ptChildNode->gbidConvVarCheck;
            if (gbidVar != GBID_NONE && SGameManager::GameGet().ptSGameGlobals)
            {
                int32 nValue = 0;
                SGameManager::GameGet().ptSGameGlobals->tConversationVarMap.Lookup(gbidVar, nValue);

                BOOL bResult = TRUE;
                switch (ptChildNode->eConvVarCheckOp)
                {
                    case CONV_VAR_CHECK_OP_EQUAL:
                        bResult = (nValue == ptChildNode->nConvVarCheckVal);
                        break;
                    case CONV_VAR_CHECK_OP_LESS_THAN:
                        bResult = (nValue < ptChildNode->nConvVarCheckVal);
                        break;
                    case CONV_VAR_CHECK_OP_GREATER_THAN:
                        bResult = (nValue > ptChildNode->nConvVarCheckVal);
                        break;
                    case CONV_VAR_CHECK_OP_LEQ:
                        bResult = (nValue <= ptChildNode->nConvVarCheckVal);
                        break;
                    case CONV_VAR_CHECK_OP_GEQ:
                        bResult = (nValue >= ptChildNode->nConvVarCheckVal);
                        break;
                    case CONV_VAR_CHECK_OP_NOT_EQUAL:
                        bResult = (nValue != ptChildNode->nConvVarCheckVal);
                        break;
                }


                // Lines 203-247 leave no code: a variable trace guarded by bVarTrace, not reconstructed.
                BOOL bVarTrace = FALSE;













































                if (!bResult)
                {
                    return FALSE;
                }
            }
        }
    }




    if (ptChildNode->eConvNodeType == CONV_NODE_TYPE_CONV_LINE_GROUP)
    {
        if (pidACDParticipants[ptChildNode->eLineSpeaker] == ACDID_INVALID)
        {
            return FALSE;
        }
    }
    else if (ptChildNode->eConvNodeType == CONV_NODE_TYPE_CONDITIONAL_PARTICIPANT)
    {
        if (pidACDParticipants[ptChildNode->eLineSpeaker] == ACDID_INVALID &&
            ptCurNode->serFalseNodes.dwSize == 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}




int32 ConversationTreeIter::sSelectRandomNode(
    const ACDID * pidACDParticipants,
    const SNO snoConversation,
    ConversationTreeNode * ptCurNode,
    int32 nChildNodeCount)
{
    struct NodeChoice
    {
        int32 nIndex;
        uint32 dwWeight;
    };

    NodeChoice * ptChoiceList = (NodeChoice *)alloca(sizeof(NodeChoice) * nChildNodeCount);
    int32 nNodeChoiceCount = 0;
    int32 nTotalWeight = 0;

    for (int i = 0; i < nChildNodeCount; ++i)
    {
        ConversationTreeNode * ptChildNode = &ptCurNode->arChildNodes[i];


        if (ptCurNode->nLastPlayedNode == i)
        {
            continue;
        }





        if (pidACDParticipants != NULL &&
            !sIsNodeValidChoice(ptChildNode, ptCurNode, pidACDParticipants, snoConversation))
        {
            continue;
        }

        NodeChoice & tChoice = ptChoiceList[nNodeChoiceCount];
        tChoice.nIndex = i;


        tChoice.dwWeight = ptChildNode->dwWeight;

        nTotalWeight += tChoice.dwWeight;
        nNodeChoiceCount++;
    }


    int32 nChosenNode = 0;
    if (nNodeChoiceCount > 0)
    {
        int32 nRandomWeight = Rand_Internal(*m_ptRNG, nTotalWeight - 1);


        int32 nCurWeight = 0;
        for (int i = 0; i < nNodeChoiceCount; i++)
        {
            NodeChoice & tChoice = ptChoiceList[i];

            nCurWeight += tChoice.dwWeight;
            if (nRandomWeight < nCurWeight)
            {
                nChosenNode = tChoice.nIndex;
                break;
            }
        }
    }


    ptCurNode->nLastPlayedNode = nChosenNode;
    return nChosenNode;
}





ConversationTreeNode * ConversationTreeIter::GetNext(
    const ACDID * pidACDParticipants,
    const SNO snoConversation)
{
    ConversationTreeNode * ptReturnNode = NULL;
    ConversationTreeIterItem * ptReturnItem = NULL;

    while (ptReturnNode == NULL && m_nIterStackSize > 0)
    {
        ConversationTreeIterItem * ptIterItem = &m_arIterStack[m_nIterStackSize - 1];
        ConversationTreeNode * ptCurNode = &ptIterItem->arTreeNodes[ptIterItem->nCurNode];
        BOOL fPushed = FALSE;

        ptIterItem->bVisited = TRUE;
		switch (ptCurNode->eConvNodeType)
		{
			case CONV_NODE_TYPE_RANDOM:
			{
				int32 nChildNodeCount = ptCurNode->serChildNodes.dwSize / sizeof(ConversationTreeNode);
				if (nChildNodeCount > 0)
				{
					if (m_ptRNG)
					{
						int32 nChosenNode = sSelectRandomNode(pidACDParticipants, snoConversation, ptCurNode, nChildNodeCount);

						sPushArrayToStack(&ptCurNode->arChildNodes[nChosenNode], 1, ptIterItem->m_tLineGroupData);
					}
					else
					{
						sPushArrayToStack(ptCurNode->arChildNodes, nChildNodeCount, ptIterItem->m_tLineGroupData);
					}
					fPushed = TRUE;
				}
				break;
			}

			case CONV_NODE_TYPE_CONV_LINE_GROUP:
			{
				LineGroupData tLineGroupData;
				tLineGroupData.uFlags = ptCurNode->uFlags;
				tLineGroupData.dwLineUID = ptCurNode->dwLineUID;
				tLineGroupData.eLineSpeaker = ptCurNode->eLineSpeaker;
				tLineGroupData.eSpeakerTarget = ptCurNode->eSpeakerTarget;
				tLineGroupData.dwAnimTag = ptCurNode->dwAnimTag;
				tLineGroupData.nDisplayTimeAdjustment = ptCurNode->nDisplayTimeAdjustment;

				int32 nNodeCount = ptCurNode->serChildNodes.dwSize / sizeof(ConversationTreeNode);
				if (nNodeCount > 0)
				{
					sPushArrayToStack(ptCurNode->arChildNodes, nNodeCount, tLineGroupData);
					fPushed = TRUE;
				}
				break;
			}





			case CONV_NODE_TYPE_CONDITIONAL:
			case CONV_NODE_TYPE_CONDITIONAL_PARTICIPANT:
			{

				if (ptCurNode->arTrueNodes != NULL &&
					sCheckCondition(ptCurNode, TRUE))
				{
					int32 nCountToPush = ptCurNode->serTrueNodes.dwSize / sizeof(ConversationTreeNode);
					sPushArrayToStack(ptCurNode->arTrueNodes, nCountToPush, ptIterItem->m_tLineGroupData);
					fPushed = TRUE;
				}
				if (ptCurNode->arFalseNodes != NULL &&
					sCheckCondition(ptCurNode, FALSE))
				{
					int32 nCountToPush = ptCurNode->serFalseNodes.dwSize / sizeof(ConversationTreeNode);
					sPushArrayToStack(ptCurNode->arFalseNodes, nCountToPush, ptIterItem->m_tLineGroupData);
					fPushed = TRUE;
				}
				break;
			}
		}


		// Inference: the returned node and item are taken unconditionally (x20/x21 hold ptCurNode/ptIterItem at line 450).
		ptReturnItem = ptIterItem;
		ptReturnNode = ptCurNode;


		if (!fPushed)
		{
			sAdvance();
		}
    }

    m_ptCurItem = ptReturnItem;
    m_ptCurNode = ptReturnNode;

    if ( ptReturnNode == NULL && m_ptConvDef != NULL )
    {
        Cancel();
    }

    return ptReturnNode;
}



void ConversationTreeIter::Stop()
{
    // Inference: the out-of-line copy (8 bytes, removed by the linker) leaves no evidence of its body;
    // one store and a return fit, and emptying the stack is what stopping an iterator means.
    m_nIterStackSize = 0;
}




void ConversationTreeIter::Cancel()

{
    if (m_ptConvDef)
    {
        m_ptCurNode = NULL;
        m_ptCurItem = NULL;
        m_nIterStackSize = 0;

		if (m_snoConversation != SNO_NONE)
		{
			// Lines 483-500 leave no code (a write-lock path compiled out of this build).
















			gptGroupDefConversation->UnlockR((void *&)m_ptConvDef);
		}


        m_ptConvDef = NULL;
    }
}




void ConversationTreeIter::sAdvance()
{

	while (m_nIterStackSize > 0)
	{


		ConversationTreeIterItem * ptIterItem = &m_arIterStack[m_nIterStackSize - 1];
		if (!ptIterItem->bVisited || ++ptIterItem->nCurNode != ptIterItem->nNodeCount)
		{
			break;
		}
		m_nIterStackSize -= 1;
	}
}









void ConversationTreeIter::sPushArrayToStack(
    ConversationTreeNode * arTreeNodes,
    const int32 nCount,
    const LineGroupData & tLineGroupData)
{
    if (arTreeNodes)
    {
        ConversationTreeIterItem & tStackItem = m_arIterStack[m_nIterStackSize];
        tStackItem.arTreeNodes = arTreeNodes;
        tStackItem.nCurNode = 0;
        tStackItem.nNodeCount = nCount;
        tStackItem.bVisited = FALSE;
        tStackItem.m_tLineGroupData = tLineGroupData;

        m_nIterStackSize++;
    }
}
