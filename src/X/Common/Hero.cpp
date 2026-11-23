// Original file: X/Common/Hero.cpp
//

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Act.h"
#include "ActorCommonData.h"
#include "CommonTypes.h"
#include "GameCommonData.h"
#include "GameSwig.h"
#include "Player.h"
#include "WorldPlace.h"
#include "Definitions/ActDefinition.h"
#include "Definitions/Definitions.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Protobuf/HeroCommon.pb.h"
#include "Quests/QuestsSwig.h"
#include "SNO/SNOFiles.h"
#include "../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Memory/SigmaMemory.h"

























void HeroResetSavedData(
    Player & tPlayer)
{

    SigmaMemoryClear(&tPlayer.tHeroStateData, sizeof(tPlayer.tHeroStateData));
    SigmaMemoryClear(&tPlayer.tCrafterSavedData, sizeof(tPlayer.tCrafterSavedData));
    SigmaMemoryClear(&tPlayer.tCurrencySavedData, sizeof(tPlayer.tCurrencySavedData));

    tPlayer.tHeroStateData.SetDefaults();
    CrafterSavedDataSetDefaults(tPlayer.tCrafterSavedData);
    tPlayer.tCurrencySavedData.SetToDefaults();

    PlayerInitializeWaypoints(&tPlayer);

    tPlayer.tHeroStateData.tSavedData.uSecondsPlayed = 0;
    tPlayer.tHeroStateData.tSavedData.uEventFlags = 0;
}



float HeroGetAwarenessRadius(
    ACDID idACDPlayer)
{
    float flRadius = 0.0f;
    if (ACDIsPlayer(idACDPlayer))
    {
        ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
        if (ptACD)
        {


            const GameBalanceDefinition * pdefGB;
            const HeroData * pCharData = (const HeroData *)GBLockR(ptACD->hGB, pdefGB, TRUE);
            if (pCharData)
            {
                flRadius = pCharData->flPlayerAwarenessRadius;
                GBUnlockR(pdefGB);
            }
        }
    }
    return flRadius;
}




BOOL HeroIsRanged(
    ACDID idACDPlayer)
{
    BOOL bIsRanged = FALSE;
    if (ACDIsPlayer(idACDPlayer))
    {
        ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
        if (ptACD)
        {


            const GameBalanceDefinition * pdefGB;
            const HeroData * pCharData = (const HeroData *)GBLockR(ptACD->hGB, pdefGB, TRUE);
            if (pCharData)
            {
                bIsRanged = pCharData->bIsRanged;
                GBUnlockR(pdefGB);
            }
        }
    }
    return bIsRanged;
}




BOOL HeroIsAwareOfPlayer(
    ACDID idACDPlayer,
    ACDID idACDTarget)
{
    BOOL bAware = FALSE;

    WorldPlace placePlayer = ACDGet(idACDPlayer).GetPlace();
    WorldPlace placeTarget = ACDGet(idACDTarget).GetPlace();

    if (placePlayer.idSWorld == placeTarget.idSWorld)
    {
        float flRadius = HeroGetAwarenessRadius(idACDPlayer);
        float flRadiusSqr = flRadius * flRadius;

        if (Vector3D::Distance2DSquared(placePlayer.wp, placeTarget.wp) <= flRadiusSqr)
        {
            bAware = TRUE;
        }
    }
    return bAware;
}
















































































































static bool sQuestCompleted(const D3::Hero::QuestHistoryList & questHistoryList, SNO quest)
{
    for (int32 i = 0; i < questHistoryList.quest_history_size(); ++i)
    {
        const D3::Hero::QuestHistoryEntry & questEntry = questHistoryList.quest_history(i);

        if (questEntry.sno_quest() == quest)
        {
            return questEntry.highest_played_quest_step() == QUEST_ANY_COMPLETION_STEP_UID;
        }
    }

    return false;
}






// External linkage in the original (_Z28sHeroCompletedAllQuestsInActiRKN2D34Hero16QuestHistoryListE),
// emitted out of line in the original CU too (unreferenced, dropped by the linker).
bool sHeroCompletedAllQuestsInAct(Act act, const D3::Hero::QuestHistoryList & questHistoryList)
{
    SNO snoAct = ActSNOFromActEnum(act);
    if (snoAct == SNO_NONE)
    {
        return false;
    }
    ScopedActDefinitionLockR ptDefAct(snoAct);
    if (!ptDefAct)
    {
        return false;
    }

    int32 nNumQuests = ActGetNumQuests(ptDefAct);
    int32 nCompletedQuest = 0;
    for (int32 quest = 0; quest < nNumQuests; ++quest)
    {
        SNO snoQuest = ptDefAct->arQuests[quest].snoQuest;
        if (snoQuest != SNO_NONE)
        {
            if (sQuestCompleted(questHistoryList, snoQuest))
            {
                ++nCompletedQuest;
            }
        }
    }

    return nCompletedQuest == nNumQuests;
}

bool HeroCompletedAllQuests(
    const D3::Hero::QuestHistoryList & questHistoryList,
    BOOL bIncludeActV)
{
    if (!sHeroCompletedAllQuestsInAct(ACT_1, questHistoryList))
    {
        return false;
    }

    if (!sHeroCompletedAllQuestsInAct(ACT_2, questHistoryList))
    {
        return false;
    }

    if (!sHeroCompletedAllQuestsInAct(ACT_3, questHistoryList))
    {
        return false;
    }

    if (!sHeroCompletedAllQuestsInAct(ACT_4, questHistoryList))
    {
        return false;
    }

    if (bIncludeActV && !sHeroCompletedAllQuestsInAct(ACT_5, questHistoryList))
    {
        return false;
    }

    return true;
}
