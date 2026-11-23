// Original file: X/Common/Definitions/ConversationListDefinition.cpp

#include "../../Sigma/SigmaTypes.h"
typedef int32 GBID;  // X/Common/GameBalance.h
#include "ConversationListDefinition.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;  // X/Sigma/Files/XFilesCommon.cpp:20
extern const XTypeDescriptor * DT_SNO;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp
extern const XTypeDescriptor * DT_GBID;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp



#define CLE(f) offsetof(ConversationListEntry, f)
static const TypeDescriptorField ConversationListEntry_Fields[17] = {  // line 62
    FilterTypeDescriptorField("", DT_SNO, CLE(snoConv), "", CONVERSATION_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, CLE(eConditionReqs), NULL, 1),
    TypeDescriptorField("", DT_INT, CLE(dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, CLE(eCrafterType), NULL, 1),
    FilterTypeDescriptorField("", DT_GBID, CLE(gbidItem), "", 2, 0, -1),
    CharArrayTypeDescriptorField("", CLE(szLabel), "", 1, 128, -1),
    CharArrayTypeDescriptorField("", CLE(szPlayerFlag), "", 1, 128, -1),
    FilterTypeDescriptorField("", DT_SNO, CLE(snoQuestCurrent), "", QUEST_SNO, 0x200, -1),
    TypeDescriptorField("", DT_INT, CLE(nStepUIDCurrent), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, CLE(snoQuestAssigned), "", QUEST_SNO, 0x200, -1),
    FilterTypeDescriptorField("", DT_SNO, CLE(snoQuestActive), "", QUEST_SNO, 0x200, -1),
    FilterTypeDescriptorField("", DT_SNO, CLE(snoQuestComplete), "", QUEST_SNO, 0x200, -1),
    FilterTypeDescriptorField("", DT_SNO, CLE(snoQuestRange), "", QUESTRANGE_SNO, 0, -1),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, CLE(arLevelArea), NULL, 0x200, 6, LEVELAREA_SNO, 0),
    TypeDescriptorField("", DT_INT, CLE(eSpecialEventFlag), NULL, 1),
    TypeDescriptorField("", DT_INT, CLE(uClassFlags), NULL, 0x80001),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ConversationListEntry), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConversationListEntry("ConversationListEntry", 4, ConversationListEntry_Fields, 17, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 79
const XTypeDescriptor * DT_CONVERSATION_LIST_ENTRY = &TypeDescriptor_ConversationListEntry;

static const TypeDescriptorField ConversationListDefinition_Fields[3] = {  // line 81
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ConversationListDefinition, serConvList), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_CONVERSATION_LIST_ENTRY, offsetof(ConversationListDefinition, ptConvList), 0x21,
                                     (int32)offsetof(ConversationListDefinition, serConvList) - (int32)offsetof(ConversationListDefinition, ptConvList)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ConversationListDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConversationListDefinition("ConversationList", 6, ConversationListDefinition_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 83
const XTypeDescriptor * DT_CONVERSATIONLISTDEF = &TypeDescriptor_ConversationListDefinition;

ConversationListGroupDef * gptGroupDefConversationList;  // line 88

// line 90
ConversationListGroupDef::ConversationListGroupDef()
    : GroupDefinition("ConversationList", CONVERSATIONLIST_SNO,
                      SNOVersions(39, 41, 41, 41, 42),
                      SNODATATYPE_TITLE, sizeof(ConversationListDefinition), DT_CONVERSATIONLISTDEF, 0x5005, 4)
{
}

// line 108
uint32 ConversationListGroupDef::GetMaxCount()
{
    return 260;
}

// Headers the function below needs (scaffold probe, build/large3/work/CLEEval/prelude.h).
#include "../ActorCommonData.h"
#include "../../Sigma/MemoryPools.h"
#include "../Actors/ActorCollision.h"
#include "../Player.h"
#include "../../Server/Actors/Actors.h"
#include "../GameSwig.h"
#include "../GameCommonData.h"
#include "../Attributes/FastAttribute.h"
#include "../GameBalance.h"
#include "../Items.h"
#include "../Quests/Quests.h"
#include "../../Server/LuaScript/ScriptUtil.h"
#include "QuestDefinition.h"
#include "../Crafting/ItemCrafting.h"
#include "../ActorInventory.h"
#include "HeroDefinition.h"
#include "QuestRangeDefinition.h"
#include "../SceneCommon.h"
#include "../Nav/NavigationMesh.h"
#include "../../Server/System/LevelArea.h"
#include "../../Server/ConversationMgr.h"
#include "../World.h"
#include "../../Server/System/SNephalemRift.h"
#include "../TagMaps/ActorTags.h"
#include "../TagMaps/MarkerTags.h"
#include "../Crafting/ItemCraftingSwig.h"
#include "GameBalanceDefinition.h"

// Declared here because their headers do not carry them yet.
BOOL PlayerIsPlayerGameFlagSet(const Player * ptPlayer, LPCSTR szLabel);  // X/Common/Player.cpp:3461
BOOL SQuestsHasQuestItemBeenFound(GBID gbidItemRequired);  // X/Server/Quests/SQuests.cpp:1281
BOOL SPlayerHasHeardConversation(const Player * ptPlayer, const SNO snoConv);  // X/Server/SPlayer.cpp:5830
BOOL WorldIsDungeonFinderWorld(const SNO snoWorld);  // X/Common/World.cpp:886

// DWARF: X/Common/Definitions/ConversationListDefinition.h:25. Declared here instead of in the header
// (ConversationListEntry::eConditionReqs stays int32 there).
enum ConversationConditionReqs {
    NO_CONDITIONS = 0,
    ALL_CONDITIONS = 1,
    ONE_CONDITION = 2,
};
// DWARF: X/Common/Definitions/ConversationListDefinition.h:72 (anonymous enum); declared here, see above.
enum {
    CONVERSATION_LIST_MAX_LEVEL_AREAS = 6,
};

#line 293

static BOOL sACDIsNearHostiles(
    const ActorCommonData & tACD,
    const float flRadius)
{
    XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
    GetACDsInRadius(tACD, flRadius, listACDsInRadius);

    POS pos = listACDsInRadius.GetHeadPosition();
    while (pos)
    {
        ACDID idACDOther = (ACDID)listACDsInRadius.GetNext(pos);

        ActorCommonData & tACDOther = ACDGet(idACDOther);
        if (tACDOther.IsMonster() &&
            tACDOther.IsEnabled() &&
            !tACDOther.IsNeverTargetable() &&
            ACDIsHostile(tACD, tACDOther, FALSE) &&
            tACDOther.IsAlive())
        {
            return TRUE;
        }
    }

    return FALSE;
}


// The helper's body is inferred from the inlined copies: line 328 tests fValue, line 330 compares
// with ONE_CONDITION (2) on the true side, line 338 with ALL_CONDITIONS (1) on the false side.
static BOOL sRetvalFromConditionReqs(
    const BOOL fValue,
    const ConversationConditionReqs eConditionReqs,
    BOOL & fResult)
{
    if (fValue)
    {
        if (eConditionReqs == ONE_CONDITION)
        {
            fResult = TRUE;
            return TRUE;
        }
    }
    else
    {
        if (eConditionReqs == ALL_CONDITIONS)
        {
            fResult = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}





BOOL ConversationListEntryEvaluate(
    const ConversationListEntry & tEntry,
    const Player & tPlayer,
    const ActorCommonData * ptACDNPC)
{
    BOOL fResult = TRUE;


    if (!SNOFileExists(CONVERSATION_SNO, tEntry.snoConv))
    {
        return FALSE;
    }


    uint32 dwNow = GameGetServerTime();
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    int32 nCoolDown = Math::Max(tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_BANTER_COOLDOWN, tEntry.snoConv)),
        ptACDNPC ? ptACDNPC->AttributesGetInt(MakeAttribKey(ATTRIBUTE_CALLOUT_COOLDOWN, tEntry.snoConv)) : 0);
    if ((nCoolDown != -1) && (dwNow < nCoolDown))
    {
        return FALSE;
    }


    if (ptACDNPC)
    {
        Actor *ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptACDNPC->id));
        if (ptActor)
        {
            PlayerClass eKnownOnlyToClass = (PlayerClass)ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_KNOWN_ONLY_TO_CLASS, TAG_MARKER_KNOWN_ONLY_TO_CLASS);
            if (eKnownOnlyToClass != PLAYER_CLASS_NONE && eKnownOnlyToClass != PlayerGetPlayerClass(&tPlayer))
            {
                return FALSE;
            }

            ActorGender eGender = (ActorGender)ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_KNOWN_ONLY_TO_GENDER, TAG_MARKER_KNOWN_ONLY_TO_GENDER);
            if (eGender != ACTOR_GENDER_INVALID)
            {
                BOOL bIsMale = PlayerIsMale(tPlayer);
                if (eGender == ACTOR_GENDER_FEMALE && bIsMale)
                {
                    return FALSE;
                }
                if (eGender == ACTOR_GENDER_MALE && !bIsMale)
                {
                    return FALSE;
                }
            }
        }
    }


    PlayerClass eClass = PlayerGetPlayerClass(&tPlayer);
    if (eClass >= 0 && eClass < NUM_PLAYER_CLASSES)
    {

        if (!(tEntry.uClassFlags & (1 << eClass)))
        {
            return FALSE;
        }
    }

    if (tEntry.eConditionReqs == NO_CONDITIONS)
    {
        return TRUE;
    }


    if (tEntry.eCrafterType != CRAFTER_TYPE_INVALID)
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_CRAFTER_UNLOCK_NEGATE_BIT));
        BOOL fUnlocked = (GetCraftLevel((CrafterType)tEntry.eCrafterType, tPlayer) > 0);
        if (sRetvalFromConditionReqs(fUnlocked == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }

    if (tEntry.eSpecialEventFlag != SPECIAL_EVENT_FLAG_INVALID)
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_SPECIAL_EVENT_FLAG_NEGATE_BIT));
        BOOL fFlagSet = (tPlayer.tHeroStateData.tSavedData.uEventFlags >> tEntry.eSpecialEventFlag) & 1;
        if (sRetvalFromConditionReqs(fFlagSet == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }

    if (strlen(tEntry.szLabel))
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_GAME_FLAG_NEGATE_BIT));
        BOOL fSet = !!GameFlagIsSet(tEntry.szLabel);
        if (sRetvalFromConditionReqs(fSet == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }

    if (strlen(tEntry.szPlayerFlag))
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_PLAYER_FLAG_NEGATE_BIT));
        BOOL fSet = !!PlayerIsPlayerGameFlagSet(&tPlayer, tEntry.szPlayerFlag);
        if (sRetvalFromConditionReqs(fSet == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }

    if (tEntry.gbidItem != GBID_NONE)
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_HAS_ITEM_NEGATE_BIT));

        BOOL fHasItem;
        if (ItemHasLabel(tEntry.gbidItem, ITL_QUEST))
        {


            fHasItem = SQuestsHasQuestItemBeenFound(tEntry.gbidItem);
        }
        else
        {
            GBHandle hGBItem;
            hGBItem.Set(GB_ITEMS, tEntry.gbidItem);
            fHasItem = ACDInventoryGetItemCount(tACDPlayer, hGBItem, cgtDefaultPlayerInventorySlots, FALSE, FALSE) > 0;
        }

        if (sRetvalFromConditionReqs(fHasItem == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }

    if (tEntry.snoQuestCurrent != SNO_NONE)
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_QUEST_STEP_CURRENT_NEGATE_BIT));

        QuestInstance *ptQuestInstance = QuestsGetQuestInstance(tEntry.snoQuestCurrent, ACDID_INVALID);
        BOOL fIsCurrent = (ptQuestInstance && ptQuestInstance->nCurrentStepUID == tEntry.nStepUIDCurrent);

        if (sRetvalFromConditionReqs(fIsCurrent == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }
    if (tEntry.snoQuestAssigned != SNO_NONE)
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_QUEST_ASSIGNED_NEGATE_BIT));
        BOOL fAssigned;





        {
            ScopedQuestDefinitionLockR pdefQuest(tEntry.snoQuestAssigned);
            Act eAct = GameGetCurrentAct();
            if (pdefQuest->eAct == eAct)
            {
                QuestInstance *ptQuestInstance = QuestsGetQuestInstance(tEntry.snoQuestAssigned, ACDID_INVALID);
                fAssigned = (ptQuestInstance && ptQuestInstance->eInstanceState != QUEST_INSTANCE_UNASSIGNED);
            }
            else
            {
                fAssigned = (pdefQuest->eAct != ACT_INVALID && pdefQuest->eAct < eAct);
            }

            if (sRetvalFromConditionReqs(fAssigned == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
                return fResult;
        }
    }
    if (tEntry.snoQuestActive != SNO_NONE)
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_QUEST_ACTIVE_NEGATE_BIT));

        QuestInstance *ptQuestInstance = QuestsGetQuestInstance(tEntry.snoQuestActive, ACDID_INVALID);
        BOOL fActive = (ptQuestInstance && ptQuestInstance->eInstanceState == QUEST_INSTANCE_ACTIVE);

        if (sRetvalFromConditionReqs(fActive == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }
    if (tEntry.snoQuestComplete != SNO_NONE)
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_QUEST_COMPLETE_NEGATE_BIT));
        BOOL fComplete;





        {
            ScopedQuestDefinitionLockR pdefQuest(tEntry.snoQuestComplete);
            Act eAct = GameGetCurrentAct();
            if (pdefQuest->eAct == eAct)
            {
                QuestInstance *ptQuestInstance = QuestsGetQuestInstance(tEntry.snoQuestComplete, ACDID_INVALID);
                fComplete = (ptQuestInstance && ptQuestInstance->eInstanceState == QUEST_INSTANCE_COMPLETED);
            }
            else
            {
                fComplete = (pdefQuest->eAct != ACT_INVALID && pdefQuest->eAct < eAct);
            }
            if (sRetvalFromConditionReqs(fComplete == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
                return fResult;
        }
    }
    if (tEntry.snoQuestRange != SNO_NONE)
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_QUEST_RANGE_NEGATE_BIT));

        BOOL bResult = QuestRangeEvaluate(tEntry.snoQuestRange, NULL);

        if (sRetvalFromConditionReqs(bResult == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }
    // Inference: a bare scope; the DWARF block 556-584 starts with fDesiredVal and no test precedes it.
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_LEVEL_AREA_NEGATE_BIT));
        NavMeshHandle tNavMeshHandle;
        ScenesCommonNavMeshNodeFromPlace(ptACDNPC ? ptACDNPC->GetPlace() : tACDPlayer.GetPlace(), &tNavMeshHandle, FALSE, FALSE);
        if (!NavMeshHandleIsNone(tNavMeshHandle))
        {
            LevelAreaID idLevelArea = NavMeshHandleGetLevelAreaID(tNavMeshHandle, FALSE);
            SNO snoLevelArea = LevelAreaGetSNO(idLevelArea);
            if (snoLevelArea != SNO_NONE)
            {
                BOOL bValid = FALSE;
                BOOL bMatch = FALSE;
                for (int32 i = 0; i < CONVERSATION_LIST_MAX_LEVEL_AREAS; i++)
                {
                    if (tEntry.arLevelArea[i] != SNO_NONE)
                    {
                        bValid = TRUE;

                        if (snoLevelArea == tEntry.arLevelArea[i])
                        {
                            bMatch = TRUE;
                            break;
                        }
                    }
                }


                if (bValid)
                {
                    if (sRetvalFromConditionReqs(bMatch == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
                        return fResult;
                }
            }
        }
    }

    if (tEntry.dwFlags & (1 << CLE_IS_NEAR_HOSTILES_BIT))
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_NEAR_HOSTILES_NEGATE_BIT));
        BOOL fVal = sACDIsNearHostiles(ptACDNPC ? *ptACDNPC : tACDPlayer, 200.0f);

        if (sRetvalFromConditionReqs(fVal == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }
    if (tEntry.dwFlags & (1 << CLE_IS_CONVERSATION_UNLOCKED_BIT))
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_CONVERSATION_UNLOCKED_NEGATE_BIT));

        BOOL bUnlocked = tPlayer.tHeroStateData.tSavedData.tSavedConversations.IsUnlocked(tEntry.snoConv);

        if (sRetvalFromConditionReqs(bUnlocked == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }

    if (tEntry.dwFlags & (1 << CLE_CONVERSATION_HEARD_BIT))
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_CONVERSATION_HEARD_NEGATE_BIT));

        BOOL bHeard = SPlayerHasHeardConversation(&tPlayer, tEntry.snoConv);

        if (sRetvalFromConditionReqs(bHeard == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }

    if (tEntry.dwFlags & (1 << CLE_CONVERSATION_HAS_ALL_PARTICIPANTS_BIT))
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_CONVERSATION_HAS_ALL_PARTICIPANTS_NEGATE_BIT));

        BOOL bHasEveryone = ConversationMgr::ConversationHasAllParticipants(tEntry.snoConv, &tACDPlayer, ptACDNPC);

        if (sRetvalFromConditionReqs(bHasEveryone == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }

    if (tEntry.dwFlags & (1 << CLE_ADVENTURE_MODE_BIT))
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_ADVENTURE_MODE_NEGATE_BIT));
        BOOL fVal = GameIsOpenWorld();

        if (sRetvalFromConditionReqs(fVal == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }
    if (tEntry.dwFlags & (1 << CLE_DUNGEON_FINDER_BIT))
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_DUNGEON_FINDER_NEGATE_BIT));

        BOOL fVal = FALSE;
        if (ptACDNPC)
        {
            fVal = WorldIsDungeonFinderWorld(WorldCommonGetWorldSNO(ptACDNPC->GetSWorldID()));
        }
        if (sRetvalFromConditionReqs(fVal == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }
    if (tEntry.dwFlags & (1 << CLE_REQUIRES_DUNGEON_FINDER_PAYMENT_BIT))
    {
        BOOL fDesiredVal = !(tEntry.dwFlags & (1 << CLE_REQUIRES_DUNGEON_FINDER_PAYMENT_NEGATE_BIT));
        BOOL fVal = NephalemRiftManager::PlayerPaidForRift(&tPlayer);

        if (sRetvalFromConditionReqs(fVal == fDesiredVal, (ConversationConditionReqs)tEntry.eConditionReqs, fResult))
            return fResult;
    }
    return tEntry.eConditionReqs == ALL_CONDITIONS;







}
