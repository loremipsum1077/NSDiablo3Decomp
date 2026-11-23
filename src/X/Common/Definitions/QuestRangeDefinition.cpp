// Original file: X/Common/Definitions/QuestRangeDefinition.cpp

#include "QuestRangeDefinition.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../Act.h"
#include "../Game.h"
#include "../GameCommonData.h"
#include "../GameSwig.h"
#include "../Quests/Quests.h"
#include "../Quests/QuestsSwig.h"
#include "ActDefinition.h"
#include "QuestDefinition.h"
#include <stddef.h>

// Declared here because their headers are not adopted yet.
extern const XTypeDescriptor * DT_SNO;  // D3DescribedType.cpp
QuestInstance * GetLeadingMainLineQuest();  // X/Common/Quests/Quests.cpp:1020

enum {
    QUESTRANGE_XML_VERSION = 3,
    QUESTRANGE_SNO_COMMON_VERSION = 22,
    QUESTRANGE_SNO_PC_VERSION = 22,
    QUESTRANGE_SNO_CONSOLE_VERSION = 23,
    QUESTRANGE_SNO_AXE_VERSION = 23,
};








static const TypeDescriptorField QuestTime_Fields[3] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestTime, snoQuest), "", QUEST_SNO, 0x200, -1), TypeDescriptorField("", DT_INT, offsetof(QuestTime, nStepUID), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(QuestTime), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestTime("QuestTime", 0, QuestTime_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_QUESTTIME = &TypeDescriptor_QuestTime;

static const TypeDescriptorField QuestRange_Fields[3] = {
    TypeDescriptorField("", DT_QUESTTIME, offsetof(QuestRange, tStart), NULL, 1), TypeDescriptorField("", DT_QUESTTIME, offsetof(QuestRange, tEnd), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(QuestRange), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestRange("QuestRange", 0, QuestRange_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_QUESTRANGE = &TypeDescriptor_QuestRange;

static const TypeDescriptorField QuestRangeDefinition_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(QuestRangeDefinition, nQuestRangeCount), NULL, 1), TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestRangeDefinition, serQuestRanges), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_QUESTRANGE, offsetof(QuestRangeDefinition, arQuestRanges), 0x21, (int32)offsetof(QuestRangeDefinition, serQuestRanges) - (int32)offsetof(QuestRangeDefinition, arQuestRanges)), TypeDescriptorField(NULL, DT_NULL, sizeof(QuestRangeDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestRangeDefinition("QuestRange", QUESTRANGE_XML_VERSION, QuestRangeDefinition_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_QUESTRANGE_DEFINITION = &TypeDescriptor_QuestRangeDefinition;























































































QuestRangeGroupDef * gptGroupDefQuestRange;




QuestRangeGroupDef::QuestRangeGroupDef()
    : GroupDefinition("QuestRange", QUESTRANGE_SNO,
                      SNOVersions(QUESTRANGE_SNO_PC_VERSION,
                                  QUESTRANGE_SNO_AXE_VERSION + AXE_BASE_SNO_VERSION,
                                  QUESTRANGE_SNO_CONSOLE_VERSION,
                                  QUESTRANGE_SNO_CONSOLE_VERSION,
                                  QUESTRANGE_SNO_CONSOLE_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(QuestRangeDefinition), DT_QUESTRANGE_DEFINITION, 0x8005, 0xe)







{
}



uint32 QuestRangeGroupDef::GetMaxCount() {
    return 1100;
}









void QuestRangeGroupDef::GetDefaultData(void * ptDefaultDef) const
{

    SigmaMemoryClear(ptDefaultDef, sizeof(QuestRangeDefinition));

    QuestRangeDefinition * pdef = (QuestRangeDefinition *)ptDefaultDef;

    pdef->nQuestRangeCount = 0;

    pdef->arQuestRanges = NULL;
    pdef->serQuestRanges.dwSize = 0;
}








void QuestTime::Initialize()
{

    snoQuest = SNO_NONE;

    nStepUID = -1;
}


static BOOL sQuestIsCurrentStepInRange(const SNO & snoQuest,
                                       const int32 & nCurrentStepUID,
                                       const QuestTime & tStart,
                                       const QuestTime & tEnd,
                                       QuestRangeResult * pResult)
{
    if (tStart.snoQuest == snoQuest && !QuestStepIsReachableByStep(snoQuest, tStart.nStepUID, nCurrentStepUID))
    {
        if (pResult)
        {
            *pResult = QRR_NOT_REACHED_YET;
        }
        return FALSE;
    }

    if (tEnd.snoQuest == snoQuest && !QuestStepIsReachableByStep(snoQuest, nCurrentStepUID, tEnd.nStepUID))
    {
        if (pResult)
        {
            *pResult = QRR_EXPIRED;
        }
        return FALSE;
    }

    if (pResult)
    {
        *pResult = QRR_ACTIVE;
    }
    return TRUE;
}



static BOOL sQuestStepIsAnActEndpoint(
    const SNO snoQuest,
    const int32 nStepUID)
{
    if (snoQuest == SNO_NONE)
    {
        return FALSE;
    }

    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    SNO snoAct = ActSNOFromActEnum(pdefQuest->eAct);
    if (snoAct == SNO_NONE)
    {
        return FALSE;
    }

    ScopedActDefinitionLockR pdefAct(snoAct);
    int32 nNumQuests = ActGetNumQuests(pdefAct);
    if (nNumQuests == 0)
    {
        return FALSE;
    }

    if (pdefAct->arQuests[nNumQuests - 1].snoQuest != snoQuest)
    {
        return FALSE;
    }

    return QuestStepIsCompletionStep(snoQuest, nStepUID);
}






BOOL QuestRangeEvaluate(const SNO snoQuestRange,
                        QuestRangeResult * pResult)
{

    if (snoQuestRange == SNO_NONE)
    {
        if (pResult) *pResult = QRR_NO_QUEST_RANGE;
        return TRUE;
    }



    QuestInstance * ptQuest = GetLeadingMainLineQuest();

    if (!ptQuest)
    {



        QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
        if (ptQuestGlobals && ptQuestGlobals->tQuestInstanceList.Count() > 0)
        {
            ptQuest = &ptQuestGlobals->tQuestInstanceList.GetTail();
        }

        if (!ptQuest)
        {
            if (pResult) *pResult = QRR_INVALID;
            return FALSE;
        }
    }

    return QuestRangeEvaluate(snoQuestRange, ptQuest->snoQuest, ptQuest->nCurrentStepUID, pResult);
}


static BOOL sQuestRangeEvaluate(
    const SNO snoQuestRange,
    const SNO snoCurrentQuest,
    const int32 nRangeIdx,
    const int32 nCurrentStepUID,
    QuestRangeResult * pResult)
{





    ScopedQuestRangeDefinitionLockR pdefQuestRange(snoQuestRange);
    if (nRangeIdx >= pdefQuestRange->nQuestRangeCount)
    {
        return FALSE;
    }

    const SNO & snoStartQuest = pdefQuestRange->arQuestRanges[nRangeIdx].tStart.snoQuest;
    const SNO & snoEndQuest = pdefQuestRange->arQuestRanges[nRangeIdx].tEnd.snoQuest;


    if ( snoStartQuest == SNO_NONE && snoEndQuest == SNO_NONE )
    {
        if (pResult) *pResult = QRR_NO_ENDPOINTS;
        return TRUE;
    }


    Act eAct;
    {
        ScopedQuestDefinitionLockR pdefQuest(snoCurrentQuest);
        eAct = pdefQuest->eAct;
    }
    switch (eAct)
    {
    case ACT_OPEN_WORLD:
        // Inference: both eAct compares share row 349:13 (the column of eAct in `switch (eAct)`), so a switch.





        if (sQuestStepIsAnActEndpoint(snoEndQuest, pdefQuestRange->arQuestRanges[nRangeIdx].tEnd.nStepUID))
        {
            return TRUE;
        }
        break;
    case ACT_INVALID:
        if (pResult) *pResult = QRR_INVALID; return FALSE;
    }

    Act eStartingAct = ACT_INVALID;
    if (snoStartQuest != SNO_NONE)
    {
        if (SNOFileExists(QUEST_SNO, snoStartQuest))
        {
            ScopedQuestDefinitionLockR pdefQuest(snoStartQuest);
            eStartingAct = pdefQuest->eAct;
        }

        if (eStartingAct == ACT_INVALID)
        {
            eStartingAct = eAct;
        }


        if (eStartingAct > eAct)
        {
            if (pResult) *pResult = QRR_NOT_REACHED_YET;
            return FALSE;
        }
    }

    BOOL bInRange = (snoStartQuest == SNO_NONE) || (eStartingAct < eAct);


    Act eEndingAct = ACT_INVALID;
    if (snoEndQuest != SNO_NONE)
    {
        if (SNOFileExists(QUEST_SNO, snoEndQuest))
        {
            ScopedQuestDefinitionLockR pdefQuest(snoEndQuest);
            eEndingAct = pdefQuest->eAct;
        }

        if (eEndingAct == ACT_INVALID)
        {
            eEndingAct = eAct;
        }




        if (eEndingAct < eAct)
        {
            if (pResult) *pResult = QRR_EXPIRED;
            return FALSE;
        }
    }



    if ( bInRange && ( snoEndQuest == SNO_NONE || eEndingAct > eAct ) )
    {
        if (pResult) *pResult = QRR_ACTIVE;
        return TRUE;
    }


    SNO snoAct = ActSNOFromActEnum(eAct);
    ScopedActDefinitionLockR pdefAct(snoAct);
    int32 nActMainQuests = ActGetNumQuests(pdefAct);

    for (int32 i = 0; i < nActMainQuests; i++)
    {
        ActQuestInfo & tInfo = pdefAct->arQuests[i];
        SNO snoQuest = tInfo.snoQuest;
        if (snoQuest == SNO_NONE)
        {
            continue;
        }


        if (QuestGetQuestType(snoQuest) != QUEST_TYPE_MAIN_QUEST)
        {
            continue;
        }




        if ( !bInRange && snoQuest == snoStartQuest )
        {
            bInRange = TRUE;
        }


        if (snoQuest == snoCurrentQuest)
        {
            if (!bInRange)
            {
                if (pResult) *pResult = QRR_NOT_REACHED_YET;
                return FALSE;
            }

            return sQuestIsCurrentStepInRange(snoCurrentQuest,
                                              nCurrentStepUID,
                                              pdefQuestRange->arQuestRanges[nRangeIdx].tStart,
                                              pdefQuestRange->arQuestRanges[nRangeIdx].tEnd,
                                              pResult);
        }




        if ( snoQuest == snoEndQuest )
        {
            if (pResult)
            {
                if (bInRange)
                {
                    *pResult = QRR_EXPIRED;
                }
                else
                {
                    *pResult = QRR_INVALID;
                }
            }
            return FALSE;
        }
    }

    if (pResult) *pResult = QRR_INVALID;
    return FALSE;
}

// NON_MATCHING: 1244 vs 1224 bytes. Same calls and blocks; the original tests ACT_OPEN_WORLD before ACT_INVALID in the
// eAct dispatch (ours, as switch or if/else, tests -1 first) and tail-merges the QRR_NO_ENDPOINTS/QRR_ACTIVE stores.
// Tried: switch / if-else / switch inside the lock block; ternary, if/else and break forms of the end-quest result.

BOOL QuestRangeEvaluate(
    const SNO snoQuestRange,
    const SNO snoCurrentQuest,
    const int32 nCurrentStepUID,
    QuestRangeResult * pResult)
{

    if (snoCurrentQuest == SNO_NONE)
    {
        if (pResult) *pResult = QRR_INVALID;
        return FALSE;
    }


    if (snoQuestRange == SNO_NONE)
    {
        if (pResult) *pResult = QRR_NO_QUEST_RANGE;
        return TRUE;
    }


    if (!SNOFileExists(QUESTRANGE_SNO, snoQuestRange))
    {
        if (pResult) *pResult = QRR_INVALID;
        return FALSE;
    }

    ScopedQuestRangeDefinitionLockR pdefQuestRange(snoQuestRange);
    for (int32 i = 0; i < pdefQuestRange->nQuestRangeCount; i++)
    {
        if (sQuestRangeEvaluate(snoQuestRange, snoCurrentQuest, i, nCurrentStepUID, pResult))
        {
            return TRUE;
        }
    }

    return FALSE;
}











































BOOL QuestRangeStillPending(
    const SNO snoQuestRange)
{

    QuestInstance * ptQuest = GetLeadingMainLineQuest();
    if (snoQuestRange == SNO_NONE || !ptQuest)
    {
        return FALSE;
    }








    ScopedQuestDefinitionLockR pdefQuestCurrent(ptQuest->snoQuest);
    Act eAct = pdefQuestCurrent->eAct;
    if (eAct == ACT_INVALID)
    {
        return TRUE;
    }

    ScopedQuestRangeDefinitionLockR pdefQuestRange(snoQuestRange);
    for (int32 nRangeIdx = 0; nRangeIdx < pdefQuestRange->nQuestRangeCount; nRangeIdx++)
    {
        const SNO snoEndQuest = pdefQuestRange->arQuestRanges[nRangeIdx].tEnd.snoQuest;
        if (snoEndQuest == SNO_NONE)
        {
            return TRUE;
        }

        {
            Act eEndingAct = ACT_INVALID;
            if (SNOFileExists(QUEST_SNO, snoEndQuest))
            {
                ScopedQuestDefinitionLockR pdefQuest(snoEndQuest);
                eEndingAct = pdefQuest->eAct;
            }

            if (eEndingAct == ACT_INVALID)
            {
                eEndingAct = eAct;
            }



            if (eEndingAct > eAct)
            {
                return TRUE;
            }



            if (eEndingAct < eAct)
            {
                continue;
            }
        }
        BOOL bPassedCurrentQuest = FALSE;
        SNO snoAct = ActSNOFromActEnum(eAct);
        ScopedActDefinitionLockR pdefAct(snoAct);
        int32 nActMainQuests = ActGetNumQuests(pdefAct);
        for (int32 i = 0; i < nActMainQuests; i++)
        {
            ActQuestInfo & tInfo = pdefAct->arQuests[i];
            SNO snoQuest = tInfo.snoQuest;
            if (snoQuest == SNO_NONE)
            {
                continue;
            }


            if (QuestGetQuestType(snoQuest) != QUEST_TYPE_MAIN_QUEST)
            {
                continue;
            }

            if (snoQuest == ptQuest->snoQuest)
            {
                bPassedCurrentQuest = TRUE;
                if (snoQuest == snoEndQuest)
                {

                    if (QuestStepIsReachableByStep(snoEndQuest, ptQuest->nCurrentStepUID, pdefQuestRange->arQuestRanges[nRangeIdx].tEnd.nStepUID))
                    {
                        return TRUE;
                    }
                }
            }
            else if (bPassedCurrentQuest && snoQuest == snoEndQuest)
            {
                return TRUE;
            }
        }
    }

    return FALSE;



}
