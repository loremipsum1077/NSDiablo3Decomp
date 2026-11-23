#include "SChallengeRift.h"

#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/ChallengeRift.h"
#include "../../Common/Game.h"
#include "../../Common/GameBalance.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Player.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Items.h"
#include "../../Common/Definitions/TreasureClassDefinition.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/Protobuf/Account.pb.h"
#include "../../Common/Protobuf/Items.pb.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../LuaScript/LuaScript.h"
#include "../PlayerList.h"
#include "../SGame.h"
#include "../SPlayer.h"
#include "../../Sigma/Random.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
BOOL GrantChallengeReward(Player * ptPlayer, GBHandle rewardGBHandle);




// Original line 30: the original condition rejects a nonzero, already rewarded challenge.
inline BOOL sPlayerEligibleforReward(Player * ptPlayer)
{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();


    if (ptPlayer->nLastRewardChallengeNumber != 0 && ptPlayer->nLastRewardChallengeNumber >= tSGameGlobals.tChallengeRift.nChallengeNumber)
        return FALSE;
    return TRUE;
}







void ChallengeRift::HandleChallengeRewards(Player * ptPlayer, int32 nTotalTimeMS, RewardScreenInfo & tInfo)
{



    ActorCommonData * ptPlayerACD = ACDTryToGet(ptPlayer->idPlayerACD);
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    uint32 nCurrentChallengeNumber = tSGameGlobals.tChallengeRift.nChallengeNumber;
    DungeonFinderOverrides * ptOverrides = &tSGameGlobals.tChallengeRift.tDungeonFinderOverrides;
    GBHandle challengeRewardHandle = GetChallengeReward();
    tInfo.challengeRiftRewardGbid = challengeRewardHandle.gbid;


    tInfo.challengeRiftTimeToBeatMs = ptOverrides->nWeeklyChallengeCompletionMilliseconds;
    tInfo.challengeRiftNum = nCurrentChallengeNumber;

    if (ptPlayerACD)
    {




        ptPlayerACD->AttributesSetInt(ATTRIBUTE_IN_CHALLENGE_RIFT, 0);



        if (!sPlayerEligibleforReward(ptPlayer))
        {


            tInfo.challengeRiftRewardState = CRRS_REWARD_PREVIOUSLY_EARNED;
        } else {
            tInfo.challengeRiftRewardState = (nTotalTimeMS < ptOverrides->nWeeklyChallengeCompletionMilliseconds) ? CRRS_REWARD_JUST_EARNED : CRRS_REWARD_UNEARNED;










            if (tInfo.challengeRiftRewardState == CRRS_REWARD_JUST_EARNED)
            {
                GrantChallengeReward(ptPlayer, challengeRewardHandle);
            }
        }
    }










}



GBHandle ChallengeRift::GetChallengeReward()
{
    SNO snoTreasureClass;
    GBHandle invalid;
    invalid.eType = -1;  // GB_INVALID, GameBalanceDefinition.h:34
    invalid.gbid = GBID_NONE;
    snoTreasureClass = GlobalSNOGet(GLOBALSNO_CHALLENGE_RIFTS_REWARD_TREASURE_CLASS);
    if (snoTreasureClass == SNO_NONE)
    {
        return invalid;
    }



    LuaVerifySnosAreReferenced(snoTreasureClass, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);

    ScopedTreasureClassDefinitionLockR pdefTreasureClass(snoTreasureClass);

    for (int32 n = 0; n < pdefTreasureClass->nModifierCount; n++)
    {
        LootDropModifier & tLootDropModifier = pdefTreasureClass->ptModifierList[n];
        GBHandle result;
        result.eType = 2;  // GB_ITEMS, GameBalanceDefinition.h:34
        result.gbid = tLootDropModifier.tSpecificItem.gbidItem;
        return result;
    }

    return invalid;
}



BOOL GrantChallengeReward(Player * ptPlayer, GBHandle rewardGBHandle)
{

    D3::Items::Mail & mail = ptPlayer->tChallengeMail;
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    uint32 nCurrentChallengeNumber = tSGameGlobals.tChallengeRift.nChallengeNumber;



















    D3::Items::MailAttachments & tMailAttachments = *mail.mutable_attachments();

    D3::Items::SavedItem & tItem = *tMailAttachments.mutable_items()->add_items();

    D3::Items::Generator & tGenerator = *tItem.mutable_generator();
    ItemGeneratorToDefaults(tGenerator);
    rewardGBHandle.Export(*tGenerator.mutable_gb_handle());


    tGenerator.set_seed(RandGetSeed(SGameGlobalsGet().rng));
    tGenerator.set_flags(tGenerator.flags() | 1);

    OnlineService::ItemId::INVALID.Export(*tItem.mutable_id());
    tItem.set_hireling_class(0);
    tItem.set_item_slot(0);
    tItem.set_square_index(0);
    tItem.set_used_socket_count(0);

































    D3::Account::ConsoleChallengeRiftReward tConsoleReward;

    tConsoleReward.mutable_items()->Swap(mail.mutable_attachments()->mutable_items()->mutable_items());
    tConsoleReward.set_challenge_rift(nCurrentChallengeNumber);

    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(ptPlayer);

    MessageSendToClient(listDestPlayers, SMSG_CHALLENGE_RIFT_REWARD_CONSOLE, tConsoleReward);



    ptPlayer->nLastRewardChallengeNumber = nCurrentChallengeNumber;

    SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_ChallengeR, PLAYER_SAVE_MODE_END_OF_FRAME);

    return TRUE;
}

void ChallengeRift::OpenChallengeRift()
{
    uint32 numPlayers = 0;
    Player * ptInstigator = NULL;


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->dwNephalemRiftFlags & 0x1)
        {
            numPlayers++;


            if (ptPlayer->dwNephalemRiftFlags & 0x20)
            {
                ptInstigator = ptPlayer;
            }
        }
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    DungeonFinderOverrides * ptOverrides = &tSGameGlobals.tChallengeRift.tDungeonFinderOverrides;

    ptOverrides->nWeeklyChallengeTargetMilliseconds = ptOverrides->nWeeklyChallengeCompletionMilliseconds;

    int32 nInstigatorIndex = -1;


    if (ptInstigator)
    {
        ChallengeRiftPersonalBest best = NumPlayersToChallengeRiftPersonalBest(numPlayers);
        if (best != CRPB_COUNT &&
            ptInstigator->nChallengeBestTimesMilliseconds[best] > 0 &&
            ptInstigator->nChallengeBestTimesMilliseconds[best] < ptOverrides->nWeeklyChallengeTargetMilliseconds)
        {
            ptOverrides->nWeeklyChallengeTargetMilliseconds = ptInstigator->nChallengeBestTimesMilliseconds[best];
            nInstigatorIndex = ptInstigator->nPlayerIndex;
        }
    }

    tSGameGlobals.tChallengeRift.nInstigatorIndex = nInstigatorIndex;

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_IN_CHALLENGE_RIFT, 1);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_ELIGIBLE_FOR_WEEKLY_CHALLENGE_REWARD, sPlayerEligibleforReward(ptPlayer));
    }
}


void ChallengeRift::GrantRewardToPlayerIfEligible(Player * ptPlayer)
{

    blz::list_iterator<D3::Account::ConsoleChallengeRiftReward> it = blz::find_if(ptPlayer->tChallengeRiftRewards.begin(), ptPlayer->tChallengeRiftRewards.end(),
        [](D3::Account::ConsoleChallengeRiftReward & tReward)
        {
            return !GameIsSeasonal() || tReward.season_earned() == OnlineService::GetSeasonNum();
        });

    if (it == ptPlayer->tChallengeRiftRewards.end())
    {
        return;
    }

    DWordDataMessage tMsg;
    tMsg.dwData = (uint32)-1;

    D3::Account::ConsoleChallengeRiftReward & tReward = *it;

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer != NULL &&
        ACDInventoryGetNumEmptySlots(*ptACDPlayer, 0) >= tReward.items_size())
    {
        tMsg.dwData = tReward.challenge_rift();

        for (int32 i = 0; i < tReward.items_size(); i++)
        {
            SPlayerRetrieveSeasonalStashItemOrChallengeRiftReward(*ptPlayer, *tReward.mutable_items(i)->mutable_generator(), 0);
        }

        ptPlayer->tChallengeRiftRewards.erase(it);
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(ptPlayer);
    MessageSendToClient(listDestPlayers, SMSG_CHALLENGE_RIFT_CLAIM_REWARD_CONSOLE, &tMsg, sizeof(tMsg));
}
