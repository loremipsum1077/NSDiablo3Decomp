#include "Labels.h"
#include "DRLG.h"
#include "SceneCommon.h"
#include "World.h"
#include "D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/SymbolicValue.h"
// Inference: GfxVB.h supplies the four normal-color constants initialized by the original owner.
#include "../Client/Gfx/GfxVB.h"
// X/Common/SNO/SymbolicValue.cpp:5042.
extern const SymbolicEntry gLabelConditionTypes[4];
#include "Definitions/SceneDefinition.h"
#include "Definitions/MarkerSetDefinition.h"
#include "Definitions/SceneGroupDefinition.h"
#include "../Sigma/Random.h"
#include "../Sigma/MemoryPools.h"
#include "../Sigma/RefString.h"
#include "../Sigma/Math/MathGeneral.h"
#include <float.h>
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "GameCommonData.h"
#include "Definitions/GameBalanceDefinition.h"
#include <string.h>
// DRLG.cpp:934 and SQuests.cpp:5329, original external callees.
TPlacedScenePtrPool * DRLGGetPlacedScenePtrPool();
BOOL SQuestsWorldAndLabelAreRequiredByBounty(SNO snoWorld, GBID gbidLabel);
// Original owner definitions770/809, needed before their original callers.
BOOL LabelAlreadySetInWorld(GBID gbidLabel, DRLGGenerator & tDRLGGenerator);
BOOL LabelAlreadySetInGame(GBID gbidLabel, DRLGGenerator * ptDRLGGenerator);








enum { LABEL_CONDITION_XML_VERSION = 1, LABEL_ENTRY_XML_VERSION = 1 };

















// Original helper is only retained inlined; no standalone body is checkable, required by sLabelsApplyRule.
static BOOL sEvaluateCondition(
    DRLGGenerator & tDRLGGenerator,
    const LabelCondition & tCondition)
{ // Inference for line 61: original false default covers unknown place values.
    BOOL bResult = FALSE;
    switch (tCondition.eType)
    {
        case LABEL_CONDITION_ALWAYS:
            bResult = TRUE;
            break;
        case LABEL_CONDITION_LABEL_ALREADY_SET:
        {
            GBID gbidLabel = tCondition.nConditionParm[0];
            LabelSetInPlace ePlace = tCondition.nConditionParm[1];
            switch (ePlace)
            { // Inference for line 73: original compares world-selection value0; no enum spelling survives in DWARF.
                case 0:
                    bResult = LabelAlreadySetInWorld(gbidLabel, tDRLGGenerator);
                    break; // Inference for line 76: original compares game-selection value1; no enum spelling survives in DWARF.
                case 1:
                    bResult = LabelAlreadySetInGame(gbidLabel, &tDRLGGenerator);
                    break;
            }
            break;
        }
        case LABEL_CONDITION_GAME_IS_OPEN_WORLD:
        {


            bResult = GameIsOpenWorld() != FALSE;
            break;
        }








        default:
        {
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }

    if (tCondition.uFlags & (1 << LABEL_CONDITION_FLAG_NOT))
    {
        bResult = !bResult;
    }

    return bResult;
}















// DWARF records bLabelTrace130 without location/value or surviving use; its initializer is unrecoverable and omitted.
static void sApplyLabelToScene(
    DRLGPlacedScene & tPlacedScene,
    GBID gbidLabel)
{ // DWARF bLabelTrace130 has no value/location or surviving use; unknown initializer omitted.
    BOOL bLabelTrace;










    tPlacedScene.listAppliedLabels.AddTail_Tracked(gbidLabel);






    ScopedSceneDefinitionLockR ptSceneDef(tPlacedScene.snoname.snoHandle);
    int32 nMarkerSets = ptSceneDef->arMarkerSets.GetArraySize();
    for (int32 i = 0; i < nMarkerSets; i++)
    {
        SNO snoMarkerSet = ptSceneDef->arMarkerSets[i];
        ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);
        if (pdefMarkerSet->nSpecialIndexCount > 0)
        {
            for (int32 j = 0; j < pdefMarkerSet->nSpecialIndexCount; j++)
            {

                int32 nIndex = pdefMarkerSet->pwSpecialIndexList[j];

                const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];
                if (tMarker.eType == MARKERTYPE_SCENEGROUP)
                {
                    ScopedSceneGroupDefinitionLockR ptSceneGroupDef(tMarker.snoname.snoHandle); // Inference for line 165: original search begins with no matching group label.
                    BOOL bLabelUsedInSceneGroup = FALSE;
                    for (int32 nScene = 0; nScene < ptSceneGroupDef->nSceneCnt; nScene++)
                    {
                        SceneGroupItem & tSceneItem = ptSceneGroupDef->arScenes[nScene];
                        if (tSceneItem.gbidLabel == gbidLabel)
                        {
                            bLabelUsedInSceneGroup = TRUE;
                            break;
                        }
                    }

                    if (bLabelUsedInSceneGroup)
                    {


                        for (int32 nScene = 0; nScene < ptSceneGroupDef->nSceneCnt; nScene++)
                        {
                            SceneGroupItem & tSceneItem = ptSceneGroupDef->arScenes[nScene];
                            if (tSceneItem.gbidLabel != GBID_NONE && tSceneItem.gbidLabel != gbidLabel)
                            {

                                POS pos = tPlacedScene.listPotentialLabels.Find(tSceneItem.gbidLabel);
                                if (pos)
                                {



                                    tPlacedScene.listPotentialLabels.RemoveAt(pos);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


// DWARF bLabelTrace211 has no location/value or surviving use; unknown initializer omitted. Original helper only inlined; standalone body not checkable, required by sApplyLabel.
static BOOL sApplyLabelToSceneListRandom(
    RNG & tRNG,
    const LabelEntry & tLabelEntry,
    TPlacedScenePtrList & listScenes,
    TPlacedScenePtrList & listAppliedScenes)
{ // DWARF bLabelTrace211 has no value/location or surviving use; unknown initializer omitted.
    BOOL bLabelTrace;

    if (listScenes.Count() <= 0)
    {
        return FALSE;
    }

    int32 nNumApplications = RandRange_Internal(tRNG, tLabelEntry.nApplyCountMin, tLabelEntry.nApplyCountMax - tLabelEntry.nApplyCountMin);

    // Inference: successful applications are counted from zero by the original increment273.
    int32 nApplyCount = 0;
    for (int32 i = 0; i < nNumApplications; i++)
    {
        int32 nSceneCount = listScenes.Count();
        if (nSceneCount > 0)
        {
            int32 nRandomIdx = RandRange_Internal(tRNG, 0, nSceneCount - 1);
 // Inference for line 229: index counter begins at zero before postincrement237.
            int32 nCurrIdx = 0;
            // Inference: shared fatal exit268 follows failure to select a scene.
            BOOL bSuccess = FALSE;
            POS pos = listScenes.GetHeadPosition();
            while (pos)
            {
                POS lastPos = pos;
                DRLGPlacedScene * ptPlacedScene = listScenes.GetNext(pos);
                if (nCurrIdx++ >= nRandomIdx)
                {
                    if (tLabelEntry.gbidLabel != GBID_NONE)
                    {
                        if (!ptPlacedScene->listAppliedLabels.Find(tLabelEntry.gbidLabel))
                        {
                            sApplyLabelToScene(*ptPlacedScene, tLabelEntry.gbidLabel);
                            if (!listAppliedScenes.Find(ptPlacedScene))
                            {
                                listAppliedScenes.AddTail_Tracked(ptPlacedScene);
                            }
                        }
                    }






                    listScenes.RemoveAt(lastPos);
                    // Inference: reaching the selected scene satisfies the later success check.
                    bSuccess = TRUE;
                    break;
                }
            }




            if (!bSuccess)
            {
                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }



            nApplyCount++;
        }
    }

    return nApplyCount >= nNumApplications;
}








// Original helper only retained inlined; standalone body not checkable, required by sApplyLabel.
static BOOL sApplyLabelToSceneListFarthest(
    RNG & tRNG,
    const LabelEntry & tLabelEntry,
    TPlacedScenePtrList & listScenes,
    TPlacedScenePtrList & listAppliedScenes)
{
    if (listScenes.Count() <= 0)
    {
        return FALSE;
    }

    int32 nNumApplications = RandRange_Internal(tRNG, tLabelEntry.nApplyCountMin, tLabelEntry.nApplyCountMax - tLabelEntry.nApplyCountMin);

    // Inference: original zeroed success counter is incremented at372.
    int32 nApplyCount = 0;
    for (int32 i = 0; i < nNumApplications; i++)
    {
        int32 nSceneCount = listScenes.Count();
        if (nSceneCount > 0)
        { // Inference for line 308: original failed-selection path defaults to false.
            BOOL bSuccess = FALSE; // Inference for line 309: original winner pointer starts null.
            DRLGPlacedScene * ptPlacedSceneWinner = NULL; // Inference for line 310: original best squared distance begins at zero.
            float flDistSqrWinner = 0.0f; // Inference for line 311: original winning position starts null.
            POS posWinner = NULL;
            POS pos = listScenes.GetHeadPosition();
            while (pos)
            {
                POS posLast = pos;
                DRLGPlacedScene * ptPlacedScene = listScenes.GetNext(pos);
                const Vector3D & vScenePos = ptPlacedScene->wp;

                POS posApplied = listAppliedScenes.GetHeadPosition();
                float flClosestDistSqrOfThisSet = FLT_MAX;
                while (posApplied)
                {
                    DRLGPlacedScene * ptPlacedSceneApplied = listAppliedScenes.GetNext(posApplied);
                    const Vector3D & vAppliedPos = ptPlacedSceneApplied->wp;
                    float flDistSqr = Vector3D::Distance2DSquared(vScenePos, vAppliedPos);
                    if (flDistSqr < flClosestDistSqrOfThisSet)
                    {
                        flClosestDistSqrOfThisSet = flDistSqr;
                    }
                }

                if (flClosestDistSqrOfThisSet > flDistSqrWinner)
                {
                    flDistSqrWinner = flClosestDistSqrOfThisSet;
                    ptPlacedSceneWinner = ptPlacedScene;
                    posWinner = posLast;
                }
            }

            if (posWinner && ptPlacedSceneWinner && tLabelEntry.gbidLabel != GBID_NONE)
            {

                if (!ptPlacedSceneWinner->listAppliedLabels.Find(tLabelEntry.gbidLabel))
                {
                    sApplyLabelToScene(*ptPlacedSceneWinner, tLabelEntry.gbidLabel);
                    if (!listAppliedScenes.Find(ptPlacedSceneWinner))
                    {
                        listAppliedScenes.AddTail_Tracked(ptPlacedSceneWinner);
                    }

                    listScenes.RemoveAt(posWinner);
                    // Inference: reaching label application satisfies the shared success check.
                    bSuccess = TRUE;
                }
            }

            if (!bSuccess)
            {
                if (ptPlacedSceneWinner)
                {
                    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                }
                else
                {

                    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                }
            }



            nApplyCount++;
        }
    }

    return nApplyCount >= nNumApplications;
}








// Original helper only retained inlined; standalone body not checkable, required by sApplyLabel.
static BOOL sApplyLabelToAllScenes(
    GBID gbidLabel,
    TPlacedScenePtrList & listScenes)
{ // Inference for line 391: empty list returns false; any visited scene sets true.
    BOOL bApplied = FALSE;
    POS pos = listScenes.GetHeadPosition();
    while (pos)
    {
        DRLGPlacedScene * ptPlacedScene = listScenes.GetNext(pos);
        if (!ptPlacedScene->listAppliedLabels.Find(gbidLabel))
        {
            sApplyLabelToScene(*ptPlacedScene, gbidLabel);
        }
        // Inference: nonempty scene traversal returns true even if all labels already existed.
        bApplied = TRUE;
    }
    return bApplied;
}



// NON_MATCHING: 2312 vs 2316 bytes; 377 differing words, loop/register/cleanup layout; all 18 call targets retained but address order differs. Tried combined/separate eligibility guards and normalized/early returns; equiv tier draft.
static BOOL sApplyLabel(
    DRLGGenerator & tDRLGGenerator,
    const LabelEntry & tEntry,
    TPlacedScenePtrList & listAppliedScenes)
{ // Inference for line 414: default failure before the selected application strategy.
    BOOL bSuccess = FALSE;

    TPlacedScenePtrList listScenes;
    listScenes.SetPool(DRLGGetPlacedScenePtrPool());



    if (tEntry.gbidLabel != GBID_NONE)
    {
        POS pos = tDRLGGenerator.m_listPlacedScenes.GetHeadPosition();
        while (pos)
        {
            DRLGPlacedScene & tPlacedScene = tDRLGGenerator.m_listPlacedScenes.GetNext(pos);
            // Inference: explicit guards preserve positive-weight, potential-label and already-chosen exclusion tests.
            if (!(tEntry.flWeight > EPSILON)) continue;

            if (!tPlacedScene.listPotentialLabels.Find(tEntry.gbidLabel)) continue;

            if (tEntry.uFlags & (1 << LABEL_ENTRY_FLAG_EXCLUDE_SCENES_ALREADY_CHOSEN))
            {
                if (listAppliedScenes.Find(&tPlacedScene))
                {
                    continue;
                }
            }

            listScenes.AddTail_Tracked(&tPlacedScene);
        }
    }



    if (tEntry.gbidLabel == GBID_NONE)
    {
        bSuccess = TRUE;
    }

    else if (tEntry.uFlags & (1 << LABEL_ENTRY_FLAG_APPLY_TO_ALL_SCENES))
    {
        bSuccess = sApplyLabelToAllScenes(tEntry.gbidLabel, listScenes);
    }
    else
    {
        if (tEntry.uFlags & (1 << LABEL_ENTRY_FLAG_APPLY_TO_FARTHEST))
        {
            bSuccess = sApplyLabelToSceneListFarthest(*tDRLGGenerator.m_ptRNG, tEntry, listScenes, listAppliedScenes);
        }
        else
        {
            bSuccess = sApplyLabelToSceneListRandom(*tDRLGGenerator.m_ptRNG, tEntry, listScenes, listAppliedScenes);
        }
    }

    if (!bSuccess && (tEntry.uFlags & (1 << LABEL_ENTRY_FLAG_MUST_SPAWN)))
    {
        return FALSE;
    }








    return TRUE;
}


// Original helper is only retained inlined; standalone body not checkable, needed by sLabelsApplyRule.
static BOOL sApplyAllEntries(
    DRLGGenerator & tDRLGGenerator,
    const LabelRule & tRule,
    int32 nNumEntries)
{

    // Inference: no entries or no failures returns the original shared true result.
    BOOL bSuccess = TRUE;
    TPlacedScenePtrList listAppliedScenes(DRLGGetPlacedScenePtrPool());

    for (int32 i = 0; i < nNumEntries; i++)
    {
        bSuccess = sApplyLabel(tDRLGGenerator, tRule.arEntries[i], listAppliedScenes);
        if (!bSuccess)
        {
            break;
        }
    }

    return bSuccess;
}


// Original helper is only retained inlined; standalone body not checkable, needed by sLabelsApplyRule.
static BOOL sApplySomeEntries(
    DRLGGenerator & tDRLGGenerator,
    const LabelRule & tRule,
    int32 nNumEntries,
    int32 nNumToApply)
{








    XPooledDataIDList listIndexes(MemoryPoolsGetDATAIDListPool());
    TPlacedScenePtrList listAppliedScenes(DRLGGetPlacedScenePtrPool());

    for (int32 i = 0; i < nNumEntries; i++)
    {

        if (GameIsOpenWorld() && tRule.arEntries[i].flWeight > 0.0f)
        {
            if (SQuestsWorldAndLabelAreRequiredByBounty(tDRLGGenerator.m_snoWorld, tRule.arEntries[i].gbidLabel))
            {
                sApplyLabel(tDRLGGenerator, tRule.arEntries[i], listAppliedScenes);
                nNumToApply--;
                continue;
            }
        }


        listIndexes.AddTail_Tracked(i);
    }
    // Inference: no failed draw preserves the shared true return; failed draw clears it579.
    BOOL bAllApplied = TRUE;
    for (int32 nApplyNum = 0; nApplyNum < nNumToApply; nApplyNum++)
    {
        // Inference: original initializes the weight accumulator to zero before each traversal.
        float flWeightTotal = 0.0f;
        POS pos = listIndexes.GetHeadPosition();
        while (pos)
        {
            int32 nIndex = listIndexes.GetNext(pos);
            flWeightTotal += tRule.arEntries[nIndex].flWeight;
        }
        // Inference: zero total weight or an exhausted traversal follows the original failed-draw path.
        BOOL bSuccess = FALSE;
        if (flWeightTotal > 0.0f)
        {
            float flRandom = RandRangeFloat_Internal(*tDRLGGenerator.m_ptRNG, 0.0f, flWeightTotal);
            // Inference: the same DWARF accumulator restarts at zero for the weighted selection traversal.
            flWeightTotal = 0.0f;
            POS posIndexes = listIndexes.GetHeadPosition();
            while (posIndexes)
            {
                POS posLast = posIndexes;
                int32 nIndex = listIndexes.GetNext(posIndexes);
                const LabelEntry & tEntry = tRule.arEntries[nIndex];
                flWeightTotal += tEntry.flWeight;
                if (flRandom <= flWeightTotal)
                {
                    bSuccess = sApplyLabel(tDRLGGenerator, tEntry, listAppliedScenes);
                    listIndexes.RemoveAt(posLast);
                    break;
                }
            }
        }




        if (!bSuccess) bAllApplied = FALSE;
    }


    return bAllApplied;
}


// Original helper is only retained inlined; standalone body not checkable, needed by LabelsApplyRuleSet.
static BOOL sLabelsApplyRule(
    DRLGGenerator & tDRLGGenerator,
    const LabelRule & tRule)
{ // Inference for line 592: unmet conditions and empty rule sets return true.
    BOOL bSuccess = TRUE;

    if (sEvaluateCondition(tDRLGGenerator, tRule.tCondition))
    {
        if (tRule.nNumEntries > 0)
        {
            int32 nNumEntries = tRule.nNumEntries;
            if (nNumEntries > 64)
            {
                nNumEntries = 64;
            }





            int32 nNumToChoose = tRule.nNumToChoose;
            if (nNumToChoose < 0 || nNumToChoose > nNumEntries)
            {
                nNumToChoose = nNumEntries;
            }


            if (nNumToChoose > 0)
            {



                if (nNumToChoose >= nNumEntries)
                {
                    bSuccess = sApplyAllEntries(tDRLGGenerator, tRule, nNumEntries);
                }
                else
                {
                    bSuccess = sApplySomeEntries(tDRLGGenerator, tRule, nNumEntries, nNumToChoose);
                }
            }
        }
    }

    return bSuccess;
}





// Original helper is only retained inlined; standalone body not checkable, needed by LabelsApplyRuleSet.
static CRefString sGetRuleName(
    const LabelRule & tLabelRule)
{
    CRefString strRuleName;
    if (strlen(tLabelRule.szRuleName) > 0)
    {
        strRuleName = tLabelRule.szRuleName;
    }
    else
    {
        for (int32 nEntry = 0; nEntry < tLabelRule.nNumEntries; nEntry++)
        {
            if (nEntry > 0)
            {
                strRuleName += ", ";
            } // Inference: original GB_LABELS filter11 has no shared enum declaration.
            strRuleName += GBIDToString(tLabelRule.arEntries[nEntry].gbidLabel, 11, FALSE).str();
        }
    }

    return strRuleName;
}


// NON_MATCHING: 2484 vs 2468 bytes; 563 differing words, stack/register/shared-tail layout; all 34 call targets retained but address order differs. Per-chunk call lists agree; tried ternary/conditional clamping; equiv tier draft.
BOOL LabelsApplyRuleSet(
    DRLGGenerator & tDRLGGenerator,
    BOOL bAllowOverrides)
{








































    // Inference: original true default covers an empty rule set, and the first failed rule stops traversal.
    BOOL bSuccess = TRUE;

    ScopedWorldDefinitionLockR pdefWorld(tDRLGGenerator.m_snoWorld);


    const LabelRuleSet & tRuleSet = pdefWorld->ptServerData->tLabelRuleSet;
    for (int32 i = 0; i < tRuleSet.nNumRules; i++)
    {
        LabelRule & tLabelRule = tRuleSet.arRules[i];
        CRefString strRuleName = sGetRuleName(tLabelRule);



        bSuccess = sLabelsApplyRule(tDRLGGenerator, tLabelRule);







        if (!bSuccess)
        {
            break;
        }
    }

    return bSuccess;
}






























// DWARF 043bc6eb: original standalone body was linker-removed and is not checkable; required by LabelAlreadySetInGame.
BOOL LabelAlreadySetInWorld(
    GBID gbidLabel,
    DRLGGenerator & tDRLGGenerator)
{
    POS pos = tDRLGGenerator.m_listPlacedScenes.GetHeadPosition();
    while (pos)
    {
        DRLGPlacedScene & tPlacedScene = tDRLGGenerator.m_listPlacedScenes.GetNext(pos);
        if (tPlacedScene.listAppliedLabels.Find(gbidLabel))
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL LabelAlreadySetInWorld(GBID gbidLabel, const WorldCommonData & tWcd)
{
    SceneCommonDataIterator tIterator;
    SceneCommonData * ptSCD = NULL;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->idSWorld != tWcd.idSWorld)
        {
            continue;
        }

        if (ptSCD->listAppliedLabels.Find(gbidLabel))
        {
            return TRUE;
        }
    }

    return FALSE;
}



BOOL LabelAlreadySetInGame(
    GBID gbidLabel,
    DRLGGenerator * ptDRLGGenerator)
{
    SceneCommonDataIterator tIterator;
    SceneCommonData * ptSCD = NULL;
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (ptSCD->listAppliedLabels.Find(gbidLabel))
        {
            return TRUE;
        }
    }
    // Inference: the shared false-result exit initializes bResult before the optional generator search.
    BOOL bResult = FALSE;
    if (ptDRLGGenerator)
    {
        bResult = LabelAlreadySetInWorld(gbidLabel, *ptDRLGGenerator);
    }

    return bResult;
}


// Original field names/default strings are empty; flags, offsets and extents follow the initializer's calls.
static const TypeDescriptorField LabelCondition_Fields[4] = {
    EnumTypeDescriptorField("", offsetof(LabelCondition, eType), NULL, gLabelConditionTypes),
    TypeDescriptorField("", DT_INT, offsetof(LabelCondition, uFlags), NULL, (1 << TDF_XML_SERIALIZATION) | (1 << TDF_BIT_FLAGS)),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(LabelCondition, nConditionParm), NULL, 1 << TDF_XML_SERIALIZATION, 4, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LabelCondition), NULL, 0) }; ComplexTypeDescriptor TypeDescriptor_LabelCondition("LabelCondition", LABEL_CONDITION_XML_VERSION, LabelCondition_Fields, 4, NULL, 1 << TD_IS_COMPLEX_TYPE, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LABEL_CONDITION = &TypeDescriptor_LabelCondition;

static const TypeDescriptorField LabelEntry_Fields[6] = { // Inference: filter value 11 is GB_LABELS (GameBalanceDefinition.cpp); its enum is unavailable in a shared header.
    FilterTypeDescriptorField("", DT_GBID, offsetof(LabelEntry, gbidLabel), "", 11, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(LabelEntry, uFlags), NULL, (1 << TDF_XML_SERIALIZATION) | (1 << TDF_BIT_FLAGS)),
    TypeDescriptorField("", DT_FLOAT, offsetof(LabelEntry, flWeight), NULL, 1 << TDF_XML_SERIALIZATION),
    TypeDescriptorField("", DT_INT, offsetof(LabelEntry, nApplyCountMin), NULL, 1 << TDF_XML_SERIALIZATION),
    TypeDescriptorField("", DT_INT, offsetof(LabelEntry, nApplyCountMax), NULL, 1 << TDF_XML_SERIALIZATION),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LabelEntry), NULL, 0) }; ComplexTypeDescriptor TypeDescriptor_LabelEntry("LabelEntry", LABEL_ENTRY_XML_VERSION, LabelEntry_Fields, 6, NULL, 1 << TD_IS_COMPLEX_TYPE, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LABEL_ENTRY = &TypeDescriptor_LabelEntry;

static const TypeDescriptorField LabelRule_Fields[7] = {
    CharArrayTypeDescriptorField("", offsetof(LabelRule, szRuleName), "", 1 << TDF_XML_SERIALIZATION, sizeof(LabelRule::szRuleName), -1),
    TypeDescriptorField("", DT_LABEL_CONDITION, offsetof(LabelRule, tCondition), NULL, 1 << TDF_XML_SERIALIZATION),
    TypeDescriptorField("", DT_INT, offsetof(LabelRule, nNumToChoose), NULL, 1 << TDF_XML_SERIALIZATION),
    TypeDescriptorField("", DT_INT, offsetof(LabelRule, nNumEntries), NULL, 1 << TDF_XML_SERIALIZATION),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LabelRule, serEntries), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_LABEL_ENTRY, offsetof(LabelRule, arEntries), (1 << TDF_XML_SERIALIZATION) | (1 << TDF_XPTR64), offsetof(LabelRule, serEntries) - offsetof(LabelRule, arEntries)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LabelRule), NULL, 0) }; ComplexTypeDescriptor TypeDescriptor_LabelRule("LabelRule", 0, LabelRule_Fields, 7, NULL, 1 << TD_IS_COMPLEX_TYPE, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LABEL_RULE = &TypeDescriptor_LabelRule;

static const TypeDescriptorField LabelRuleSet_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(LabelRuleSet, nNumRules), NULL, 1 << TDF_XML_SERIALIZATION),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LabelRuleSet, serRules), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_LABEL_RULE, offsetof(LabelRuleSet, arRules), (1 << TDF_XML_SERIALIZATION) | (1 << TDF_XPTR64), offsetof(LabelRuleSet, serRules) - offsetof(LabelRuleSet, arRules)), TypeDescriptorField(NULL, DT_NULL, sizeof(LabelRuleSet), NULL, 0) }; ComplexTypeDescriptor TypeDescriptor_LabelRuleSet("LabelRuleSet", 0, LabelRuleSet_Fields, 4, NULL, 1 << TD_IS_COMPLEX_TYPE, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LABEL_RULE_SET = &TypeDescriptor_LabelRuleSet;
