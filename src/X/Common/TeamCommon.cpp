// Original file: X/Common/TeamCommon.cpp

#include "TeamCommon.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Client/CGame.h"
#include "../Client/CPlayer.h"
#include "../Server/Network/KnownList.h"
#include "../Server/SGame.h"
#include "../Server/SGameManager.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/RefString.h"
#include "ActorCommonDataSwig.h"
#include "Definitions/ActorDefinition.h"
#include "Definitions/StringListDefinition.h"
#include "Game.h"
#include "GameCommonData.h"
#include "GameTemplate.h"
#include "GlobalSNO.h"
#include "Marker/Marker.h"
#include "Marker/MarkerType.h"
#include "Networking/GameMessage.h"
#include "Player.h"
#include "TagMaps/ActorTags.h"
#include "TagMaps/MarkerTags.h"
#include "TagMaps/Tags.h"
#include "WorldPlace.h"

struct TeamData {  // size 0x10
    Team eTeam;  // 0x00
    uint32 dwTeamFlags;  // 0x04
    TeamColor eTeamColoring;  // 0x08
    uint32 uEntranceGUID;  // 0x0c
};

struct TeamGlobals {  // size 0x170
    TeamData tTeamData[23];  // 0x00
};

enum TeamRelationshipType {
    TR_NONE = -1,
    TR_FRIENDLY = 0,
    TR_HOSTILE = 1,
    TR_COUNT = 2,
};

// X/Common/Networking/GameMessage.h:1422 in the DWARF.








// Declared here because its header does not carry it yet.
BOOL SPlayerGetStartLocation(const SWorldID idSWorld, const SSceneID idSScene, const Team eTeam, const uint32 uEntranceGUID, WorldPlace & tPlace, Quaternion & q);  // X/Server/SPlayer.cpp:7938
Team TeamGetForActorSNO(SNO snoActor);
void STeamRemovePlayer(Player & tPlayer);



















static TeamData & sTeamGet(Team eTeam)

{

    if (GameActiveGameCommonDataIsClient())
    {
        return CGameGet().tGCD.ptTeamGlobals->tTeamData[eTeam];
    }
    else
    {


        return SGameManager::GameGet().tGCD.ptTeamGlobals->tTeamData[eTeam];
    }
}







void CTeamsInitializeForGame()

{
    XMemoryAllocatorInterface * ptMemInterface = AppGetCurrentMemoryInterface(); CGame & tCGame = CGameGet();

    tCGame.tGCD.ptTeamGlobals = (TeamGlobals *)SigmaMemoryNew(sizeof(TeamGlobals), 4, ptMemInterface, TRUE);
}





static void sTeamSetData(const Team eTeam, uint32 dwTeamFlags, TeamColor eTeamColor, uint32 uEntranceGUID)

{
    SGame & tSGame = SGameManager::GameGet();
    TeamData & tTeamData = tSGame.tGCD.ptTeamGlobals->tTeamData[eTeam];


    tTeamData.dwTeamFlags = dwTeamFlags;
    tTeamData.eTeamColoring = eTeamColor;

    tTeamData.uEntranceGUID = uEntranceGUID;
}

void STeamsInitializeForGame()

{
    SGame & tSGame = SGameManager::GameGet();

    tSGame.tGCD.ptTeamGlobals = (TeamGlobals *)SigmaMemoryNew(sizeof(TeamGlobals), 4, tSGame.ptMemoryInterface, TRUE);




    for (Team eTeam = TEAM_PLAYER_FIRST; eTeam <= TEAM_PLAYER_LAST; eTeam++)
    {
        BOOL bPvPTeam = GameTypeIsPvP(tSGame.tGCD.eGameType) && eTeam <= TEAM_PLAYER_1;
        sTeamSetData(eTeam, bPvPTeam ? (1 << TEAM_AFFECTED_BY_PLAYER_CHAOS_EFFECTS) : 0, bPvPTeam ? eTeam - TEAM_PLAYER_FIRST : TEAM_COLOR_NONE, (uint32)-1);
    }

    for (Team eTeam = TEAM_MONSTER_FIRST; eTeam <= TEAM_MONSTER_LAST; eTeam++)
    {
        sTeamSetData(eTeam, 1 << TEAM_AFFECTED_BY_MONSTER_CHAOS_EFFECTS, TEAM_COLOR_NONE, (uint32)-1);
    }

    sTeamSetData(TEAM_NEUTRAL, 0, TEAM_COLOR_NEUTRAL, (uint32)-1);
    sTeamSetData(TEAM_SCENERY, 0, TEAM_COLOR_NONE, (uint32)-1);
}












void TeamsDisposeForGame()

{
    GameCommonData & tGCD = GameGetActiveGameCommonData();

    SigmaMemoryFree(tGCD.ptTeamGlobals, AppGetCurrentMemoryInterface()); tGCD.ptTeamGlobals = NULL;
}












static TeamRelationshipType sTeamsGetRelationship(Team eTeam0, Team eTeam1)

{

    if (eTeam0 == eTeam1)
    {
        return TR_FRIENDLY;
    }

    if (eTeam0 == TEAM_SCENERY || eTeam1 == TEAM_SCENERY)
    {
        return TR_FRIENDLY;
    }


    if (eTeam0 == TEAM_NEUTRAL || eTeam1 == TEAM_NEUTRAL)
    {
        return TR_FRIENDLY;
    }

    if (eTeam0 == TEAM_MONSTER_FRIEND && (eTeam1 >= TEAM_MONSTER_FIRST && eTeam1 <= TEAM_MONSTER_LAST))
    {
        return TR_FRIENDLY;
    }




    if ((eTeam0 >= TEAM_MONSTER_FIRST && eTeam0 <= TEAM_MONSTER_LAST) && eTeam1 == TEAM_MONSTER_FRIEND)
    {
        return TR_FRIENDLY;
    }



    if (eTeam0 >= TEAM_HATE_ALL_EXCEPT_PLAYERS_FIRST && eTeam0 <= TEAM_HATE_ALL_EXCEPT_PLAYERS_LAST && eTeam1 >= TEAM_PLAYER_FIRST && eTeam1 <= TEAM_PLAYER_LAST)
    {
        return TR_FRIENDLY;
    }




    if (eTeam0 >= TEAM_PLAYER_FIRST && eTeam0 <= TEAM_PLAYER_LAST && eTeam1 >= TEAM_HATE_ALL_EXCEPT_PLAYERS_FIRST && eTeam1 <= TEAM_HATE_ALL_EXCEPT_PLAYERS_LAST)
    {
        return TR_FRIENDLY;
    }

    return TR_HOSTILE;
}























int32 sGetTeamPlayerCount(Team eTeam)

{
    int32 nCount = 0;

    for (Player * ptPlayer = PlayerGetFirstInGame();
         ptPlayer;
         ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->eTeam == eTeam)
        {
            nCount++;
        }
    }

    return nCount;
}





static Team sGetTeamWithLeastPlayers()

{
    Team eBestTeam = TEAM_PLAYER_FIRST;
    int32 nBestCount = sGetTeamPlayerCount(TEAM_PLAYER_FIRST);

    GameType eGameType = GameGetGameType();
    Team eLastTeam = (eGameType >= GT_PVP_DOTA && eGameType <= GT_PVP_TEAMDEATHMATCH) ? TEAM_PLAYER_1 : TEAM_PLAYER_LAST;


    for (Team eTeam = TEAM_PLAYER_FIRST + 1; eTeam <= eLastTeam; eTeam++)
    {


        int32 nCount = sGetTeamPlayerCount(eTeam);



        if (nCount < nBestCount)
        {
            nBestCount = nCount;
            eBestTeam = eTeam;
        }
    }

    return eBestTeam;
}






TeamColor TeamGetEffectGroupIndex(Team eTeam)

{
    if (eTeam == TEAM_INVALID)
    {
        return TEAM_COLOR_NONE;
    }


    return sTeamGet(eTeam).eTeamColoring;
}



uint32 TeamGetNumPlayers(Team eTeam)

{
    uint32 nCount = 0;

    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (ptPlayer->eTeam == eTeam)
        {
            nCount++;
        }
    }

    return nCount;
}




















BOOL TeamsAreFriendly(Team eTeam0, Team eTeam1)


{


    return sTeamsGetRelationship(eTeam0, eTeam1) == TR_FRIENDLY;
}




Team TeamGetForMarker(const Marker & tMarker)

{


    if (TagMapTagExists(tMarker.hTagMap, TAG_MARKER_TEAM_INDEX))
    {
        return TagMapGetInt(tMarker.hTagMap, TAG_MARKER_TEAM_INDEX);
    }
    if (tMarker.eType == MARKERTYPE_ACTOR)
    {
        return TeamGetForActorSNO(tMarker.snoname.snoHandle);
    }





    return TEAM_MONSTER_0;
}




Team TeamGetForActorSNO(SNO snoActor)

{
    ScopedActorDefinitionLockR pdefActor(snoActor);

    return TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_DEFAULT_TEAM);

}



BOOL TeamIsMonsterTeam(Team eTeam)

{
    return eTeam >= TEAM_MONSTER_FIRST && eTeam <= TEAM_MONSTER_LAST;
}





void CRevealTeamMessage(void * pMessage)

{
    RevealTeamMessage * ptMessage = (RevealTeamMessage *)pMessage;



    TeamData & tTeamData = CGameGet().tGCD.ptTeamGlobals->tTeamData[ptMessage->eTeam];
    tTeamData.eTeam = ptMessage->eTeam;
    tTeamData.dwTeamFlags = ptMessage->dwTeamFlags;
    tTeamData.eTeamColoring = ptMessage->eTeamColoring;
}




CRefString CTeamGetPvPEndGameText(Team eWinningTeam)

{
    CRefString sText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_PVP), "GameOverText");
    CRefString sTeam;

    if (eWinningTeam == TEAM_INVALID)
    {
        sText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_PVP), "GameOverDraw");
    }
    else
    {
        if (PlayerGetLocalTeam() == eWinningTeam)
        {
            sTeam = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_PVP), "TC_Blue");
        }
        else
            sTeam = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_PVP), "TC_Red");
        sText = StringListEntryReplace(sText, "{Team}", sTeam, 999);
    }


    return sText;
}







void ServerMessageSendTeamToPlayer(const PlayerList & listDestPlayers, const Team eTeam)

{
    TeamData & tTeamData = sTeamGet(eTeam);

    RevealTeamMessage tMessage;
    tMessage.eTeam = eTeam;
    tMessage.dwTeamFlags = tTeamData.dwTeamFlags;
    tMessage.eTeamColoring = tTeamData.eTeamColoring;

    MessageSendToClient(listDestPlayers, SMSG_REVEAL_TEAM, &tMessage, sizeof(tMessage));
}





Team STeamGetTeamForPlayer(Player & tPlayer)

{



    if (tPlayer.eTeam != TEAM_INVALID)
    {
        STeamRemovePlayer(tPlayer);
    }










    if (tPlayer.eJoinTeam != TEAM_INVALID)
    {
        return tPlayer.eJoinTeam;
    }

    if (SGameIsPvPFlagSet())
    {
        return sGetTeamWithLeastPlayers();
    }




    return TEAM_PLAYER_0;
}




void STeamAddTeamDataToKnownList(const Player & tPlayer, KnownListID idKnownList)

{



    for (Team eTeam = 0; eTeam < 23; eTeam++)
    {
        AddToKnownList(KET_TEAM, eTeam, idKnownList, tPlayer, FALSE, FALSE);
    }
}





void STeamRemovePlayer(Player & tPlayer)

{
    tPlayer.eTeam = TEAM_INVALID;
}




BOOL STeamGetStartLocation(const SWorldID idSWorld, const Team eTeam, WorldPlace & tPlace)



{
    SGame & tSGame = SGameManager::GameGet();

    Quaternion q;


    return SPlayerGetStartLocation(idSWorld, SSCENEID_INVALID, eTeam,
        tSGame.tGCD.ptTeamGlobals->tTeamData[eTeam].uEntranceGUID, tPlace, q);
}
