#include "CTrickle.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Definitions/LevelAreaDefinition.h"
#include "../../Common/World.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../CGame.h"
#include "../CPlayer.h"
#include "../ImageCollection.h"

// Local globals/layout recorded by this CU's DWARF.
typedef XListMemoryPool<ClientTrickleEntry> ClientTrickleEntryPool;
struct ClientTrickleGlobals {
    ClientTrickleEntryPool* ptClientTrickleEntryPool;
    ClientTrickleList* pListTrickle;
};
static ClientTrickleGlobals* sgptClientTrickleGlobals;
static const int32 CLIENT_TRICKLE_ENTRY_POOL_SIZE = 32;
BOOL WorldIsDungeonFinderWorld(const SNO snoWorld);
#line 49 "X/Client/Network/CTrickle.cpp"
void CTrickleInitializeForApp()
{
    sgptClientTrickleGlobals = (ClientTrickleGlobals*)SigmaMemoryNew(sizeof(ClientTrickleGlobals), 4, 0, TRUE);
    sgptClientTrickleGlobals->ptClientTrickleEntryPool = new ClientTrickleEntryPool;
    sgptClientTrickleGlobals->ptClientTrickleEntryPool->SetBlockCount_Tracked(CLIENT_TRICKLE_ENTRY_POOL_SIZE);
    sgptClientTrickleGlobals->pListTrickle = new ClientTrickleList(sgptClientTrickleGlobals->ptClientTrickleEntryPool);
}
#line 68 "X/Client/Network/CTrickle.cpp"
void CTrickleDisposeForApp()
{
    sgptClientTrickleGlobals->pListTrickle->RemoveAll();
    delete sgptClientTrickleGlobals->pListTrickle;
    delete sgptClientTrickleGlobals->ptClientTrickleEntryPool;
    SigmaMemoryFree(sgptClientTrickleGlobals, 0);
    sgptClientTrickleGlobals = 0;
}
#line 89 "X/Client/Network/CTrickle.cpp"
void CTrickleInitializeForGame()
{
}
#line 98 "X/Client/Network/CTrickle.cpp"
void CTrickleDisposeForGame()
{
    sgptClientTrickleGlobals->pListTrickle->RemoveAll();
}
#line 108 "X/Client/Network/CTrickle.cpp"
static POS sCTrickleEntryFind(
    const ACDNetworkName ann)
{
    ClientTrickleList & listTrickle = *sgptClientTrickleGlobals->pListTrickle;

    POS pos = listTrickle.GetHeadPosition();
    POS posCurrent = NULL;

    while (pos)
    {
        posCurrent = pos;
        const ClientTrickleEntry & tTrickleEntry = listTrickle.GetNext(pos);

        if (tTrickleEntry.ann == ann)
            return posCurrent;
    }
    return NULL;
}
#line 132 "X/Client/Network/CTrickle.cpp"
void CTrickleUpdate(
    const TrickleMessage * pTrickleMessage)
{
    int32 nPlayerIndexOwner = pTrickleMessage->nPlayerIndexOwner.Exists() ? pTrickleMessage->nPlayerIndexOwner.Get() : -1;
    float flHealthPercent = pTrickleMessage->flHealthPercent.Exists() ? pTrickleMessage->flHealthPercent.Get() : 0.0f;
    int32 dwHeadstoneCorpseReviveTime = pTrickleMessage->dwHeadstoneCorpseReviveTime.Exists() ? pTrickleMessage->dwHeadstoneCorpseReviveTime.Get() : 0;
    float flMinDistSq = pTrickleMessage->fMinDisplayRangeSq.Exists() ? pTrickleMessage->fMinDisplayRangeSq.Get() : 0.0f;
    float flMaxDistSq = pTrickleMessage->fMaxDisplayRangeSq.Exists() ? pTrickleMessage->fMaxDisplayRangeSq.Get() : 0.0f;
    SNO snoStringList = pTrickleMessage->snoStringList.Exists() ? pTrickleMessage->snoStringList.Get() : -1;
    uint32 hStringListHandle = pTrickleMessage->hMinimapStringLabel.Exists() ? pTrickleMessage->hMinimapStringLabel.Get() : 0xffffffff;
    uint32 hMinimapIconHandle = pTrickleMessage->hMinimapIconLabel.Exists() ? pTrickleMessage->hMinimapIconLabel.Get() : 0xffffffff;
    int32 nAction = pTrickleMessage->nAction.Exists() ? pTrickleMessage->nAction.Get() : 0;
    GBID gbidPortraitFrame = pTrickleMessage->gbidPortraitFrame.Exists() ? pTrickleMessage->gbidPortraitFrame.Get() : -1;

    ImageTextureFrame tIcon;
    tIcon.snoTexture = pTrickleMessage->snoMinimapIcon.Exists() ? pTrickleMessage->snoMinimapIcon.Get() : -1;
    if (hMinimapIconHandle != 0xffffffff && tIcon.snoTexture == -1)
    {
        if (!ImageCollectionGetImageInfo(1, hMinimapIconHandle, tIcon))
        {
            ImageCollectionGetImageInfo(2, hMinimapIconHandle, tIcon);
        }
    }

    POS pos = sCTrickleEntryFind(pTrickleMessage->ann);
    if (pos)
    {
        ClientTrickleEntry & tTrickleEntry = sgptClientTrickleGlobals->pListTrickle->GetAt(pos);
        tTrickleEntry.snoActor = pTrickleMessage->snoActor;
        tTrickleEntry.tPlace = pTrickleMessage->tPlace;
        tTrickleEntry.nPlayerIndexOwner = nPlayerIndexOwner;
        tTrickleEntry.eType = pTrickleMessage->eTrickleInfoType;
        tTrickleEntry.snoLevelArea = pTrickleMessage->snoLevelArea;

        if (flHealthPercent < tTrickleEntry.flHealthPercent)
        {
            tTrickleEntry.flLastInjuredTime = CGameGetClientTime();
        }

        tTrickleEntry.dwHeadstoneCorpseReviveTime = dwHeadstoneCorpseReviveTime;
        tTrickleEntry.flHealthPercent = flHealthPercent;
        tTrickleEntry.dwTrickleFlags = pTrickleMessage->dwTrickleFlags;
        tTrickleEntry.snoMinimapStringlist = snoStringList;
        tTrickleEntry.hStringListLabel = hStringListHandle;
        tTrickleEntry.tIcon = tIcon;
        tTrickleEntry.fMinMinimapDistSq = flMinDistSq;
        tTrickleEntry.fMaxMinimapDistSq = flMaxDistSq;

        if (pTrickleMessage->uEngagedWithRareTime.Exists())
        {
            tTrickleEntry.uEngagedWithRareTime = pTrickleMessage->uEngagedWithRareTime.Get();
        }

        if (pTrickleMessage->uEngagedWithGoblinTime.Exists())
        {
            tTrickleEntry.uEngagedWithGoblinTime = pTrickleMessage->uEngagedWithGoblinTime.Get();
        }

        if (pTrickleMessage->bInCombat.Exists())
        {
            tTrickleEntry.bInCombat = pTrickleMessage->bInCombat.Get();
        }

        if (pTrickleMessage->nTieredRiftRank.Exists())
        {
            tTrickleEntry.nTieredRiftRank = pTrickleMessage->nTieredRiftRank.Get();
        }

        if (pTrickleMessage->bBrawling.Exists())
        {
            tTrickleEntry.bBrawling = pTrickleMessage->bBrawling.Get();
        }

        if (pTrickleMessage->bInUberFight.Exists())
        {
            tTrickleEntry.bInUberFight = pTrickleMessage->bInUberFight.Get();
        }
        if (pTrickleMessage->unk_b4.Exists()) tTrickleEntry.unk_68 = pTrickleMessage->unk_b4.Get();
        tTrickleEntry.gbidPortraitFrame = gbidPortraitFrame;
    }
    else
    {

        ClientTrickleEntry tTrickleEntry;
        tTrickleEntry.ann = pTrickleMessage->ann;
        tTrickleEntry.snoActor = pTrickleMessage->snoActor;
        tTrickleEntry.tPlace = pTrickleMessage->tPlace;
        tTrickleEntry.nPlayerIndexOwner = nPlayerIndexOwner;
        tTrickleEntry.eType = pTrickleMessage->eTrickleInfoType;
        tTrickleEntry.snoLevelArea = pTrickleMessage->snoLevelArea;
        tTrickleEntry.flHealthPercent = flHealthPercent;
        tTrickleEntry.flLastInjuredTime = -1.0f;
        tTrickleEntry.dwHeadstoneCorpseReviveTime = dwHeadstoneCorpseReviveTime;
        tTrickleEntry.dwTrickleFlags = pTrickleMessage->dwTrickleFlags;
        tTrickleEntry.snoMinimapStringlist = snoStringList;
        tTrickleEntry.hStringListLabel = hStringListHandle;
        tTrickleEntry.tIcon = tIcon;
        tTrickleEntry.fMinMinimapDistSq = flMinDistSq;
        tTrickleEntry.fMaxMinimapDistSq = flMaxDistSq;
        tTrickleEntry.uEngagedWithRareTime = pTrickleMessage->uEngagedWithRareTime.Exists() ? pTrickleMessage->uEngagedWithRareTime.Get() : 0;
        tTrickleEntry.uEngagedWithGoblinTime = pTrickleMessage->uEngagedWithGoblinTime.Exists() ? pTrickleMessage->uEngagedWithGoblinTime.Get() : 0;
        tTrickleEntry.bInCombat = pTrickleMessage->bInCombat.Exists() ? pTrickleMessage->bInCombat.Get() : FALSE;
        tTrickleEntry.nTieredRiftRank = pTrickleMessage->nTieredRiftRank.Exists() ? pTrickleMessage->nTieredRiftRank.Get() : -1;
        tTrickleEntry.bBrawling = pTrickleMessage->bBrawling.Exists() ? pTrickleMessage->bBrawling.Get() : FALSE;
        tTrickleEntry.bInUberFight = pTrickleMessage->bInUberFight.Exists() ? pTrickleMessage->bInUberFight.Get() : FALSE;
        tTrickleEntry.gbidPortraitFrame = pTrickleMessage->gbidPortraitFrame.Exists() ? pTrickleMessage->gbidPortraitFrame.Get() : -1;
        tTrickleEntry.unk_68 = pTrickleMessage->unk_b4.Exists() ? pTrickleMessage->unk_b4.Get() : 0;
        sgptClientTrickleGlobals->pListTrickle->AddTail_Tracked(tTrickleEntry);
    }


    if (pTrickleMessage->eTrickleInfoType == 1 || pTrickleMessage->eTrickleInfoType == 3 || pTrickleMessage->eTrickleInfoType == 6)
    {


        Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndexOwner);
        if (ptPlayer)
        {
            ptPlayer->flHealthPercent = flHealthPercent;
            ptPlayer->snoAreaLocation = pTrickleMessage->snoLevelArea;
            ptPlayer->nPortraitAction = nAction;

            if (ptPlayer == LocalPlayerGet())
            {
                if (GameIsOpenWorld())
                {
                    OpenWorldStatus eOpenWorldStatusPrevious = ptPlayer->eOpenWorldStatus;
                    // DWARF: local with no location; its initialiser is not recoverable.
                    int32 nRiftTier;
                    if (LevelAreaIsTown(ptPlayer->snoAreaLocation))
                        ptPlayer->eOpenWorldStatus = 0;


                    else if (WorldIsDungeonFinderWorld(WorldCommonGetWorldSNO(pTrickleMessage->tPlace.idSWorld)))
                        ptPlayer->eOpenWorldStatus = 2;









                    else if (pTrickleMessage->bBrawling.Exists() && pTrickleMessage->bBrawling.Get())
                        ptPlayer->eOpenWorldStatus = 3;


                    else if (pTrickleMessage->bInUberFight.Exists() && pTrickleMessage->bInUberFight.Get())
                        ptPlayer->eOpenWorldStatus = 4;
                    else if (pTrickleMessage->unk_b4.Exists() && pTrickleMessage->unk_b4.Get())
                        ptPlayer->eOpenWorldStatus = 5;
                    else
                        ptPlayer->eOpenWorldStatus = 1;
                }
            }
        }
    }















}
#line 309 "X/Client/Network/CTrickle.cpp"
ClientTrickleEntry* CTrickleEntryFindForANN(const ACDNetworkName ann)
{
    POS pos = sCTrickleEntryFind(ann);
    return &sgptClientTrickleGlobals->pListTrickle->GetAt(pos);
}

#line 323 "X/Client/Network/CTrickle.cpp"
void CTrickleRemove(const ACDNetworkName ann)
{
    POS pos = sCTrickleEntryFind(ann);
    if (pos)
        sgptClientTrickleGlobals->pListTrickle->RemoveAt(pos);
}
#line 336 "X/Client/Network/CTrickle.cpp"
ClientTrickleList& CTrickleGetList()
{
    return *sgptClientTrickleGlobals->pListTrickle;
}
#line 347 "X/Client/Network/CTrickle.cpp"
void CTrickleRemoveAll()
{
    sgptClientTrickleGlobals->pListTrickle->RemoveAll();
}
