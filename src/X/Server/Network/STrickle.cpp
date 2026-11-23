// Original file: X/Server/Network/STrickle.cpp
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GameTemplate.h"
#include "../../Common/Gizmo.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/Player.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/TeamCommon.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../Actors/Actors.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SPlayer.h"







struct TrickleGlobals {  // size 0x4
    uint32 dwLastTrickleUpdate;  // 0x00
};


SNO ActorGetInheritedTagMapValueSNO(const Actor * ptActor, const uint32 dwActorTag,
                                    const uint32 dwMarkerTag);  // Actors.cpp



static const uint32 TRICKLE_UPDATE_DELAY = 90;




BOOL STrickleIsActorTrickled(const ActorID idActor);
BOOL STrickleIsActorTrickledToPlayer(const ActorID idActor, const Player & tPlayer);








void STrickleInitializeForGame()

{
    SGame & tSGame = SGameManager::GameGet();
    TrickleGlobals *& ptTrickleGlobals = tSGame.ptTrickleGlobals;
    ptTrickleGlobals = (TrickleGlobals *)SigmaMemoryNew(sizeof(TrickleGlobals), 4, tSGame.ptMemoryInterface, TRUE);
    ptTrickleGlobals->dwLastTrickleUpdate = 0;
}









void STrickleDisposeForGame()

{
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        SPlayerTrickleClearAll(*ptPlayer);
    }

    SGame & tSGame = SGameManager::GameGet();
    TrickleGlobals *& ptTrickleGlobals = tSGame.ptTrickleGlobals;
    SigmaMemoryFree(ptTrickleGlobals, tSGame.ptMemoryInterface); ptTrickleGlobals = NULL;
}



void STrickleRemoveFromAll(const ACDID idACD)

{
    const ActorID idActor = (ActorID)ACDGetOwner(idACD);

    // remove from every player's trickle list

    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (STrickleIsActorTrickledToPlayer(idActor, *ptPlayer))
        {
            SPlayerTrickleRemove(*ptPlayer, idACD);
        }
    }
}



BOOL STrickleIsActorTrickled(const ActorID idActor)

{
    const Actor * ptActor = ActorGet(idActor);
    return ptActor->dwTrickleBits != 0;
}



BOOL STrickleIsActorTrickledToPlayer(const ActorID idActor, const Player & tPlayer)

{

    if (tPlayer.nPlayerIndex < 0 || tPlayer.idPlayerACD == ACDID_INVALID)
    {
        return FALSE;
    }

    const Actor * ptActor = ActorGet(idActor);
    return ptActor->dwTrickleBits & (1 << tPlayer.nPlayerIndex);
}



void STrickleTriggerUpdate()

{
    TrickleGlobals & tTrickleGlobals = *SGameManager::GameGet().ptTrickleGlobals;
    tTrickleGlobals.dwLastTrickleUpdate = 0;
}



void STrickleUpdate()

{
    TrickleGlobals & tTrickleGlobals = *SGameManager::GameGet().ptTrickleGlobals;
    uint32 dwNow = GameGetServerTime();
    if (tTrickleGlobals.dwLastTrickleUpdate != 0 &&
        dwNow - tTrickleGlobals.dwLastTrickleUpdate < TRICKLE_UPDATE_DELAY)
    {
        return;
    }

    tTrickleGlobals.dwLastTrickleUpdate = dwNow;

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        SPlayerTrickleUpdate(*ptPlayer);
    }
}






BOOL sShouldTrickleGizmoType(ActorCommonData & tACD)

{
    ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
    if (pdefActor && TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_FORCE_MINIMAP_ICON))
    {
        return TRUE;
    }

    GizmoType eGizmoType = tACD.GetGizmoType();
    if (GizmoUsesStaticMinimapPing(eGizmoType))
    {
        return FALSE;
    }

    switch (eGizmoType)
    {
        case ACTORGIZMO_DOOR:
        case ACTORGIZMO_CHEST:
        case ACTORGIZMO_SIGN:
        case ACTORGIZMO_HEALING_WELL:
        case ACTORGIZMO_TOWN_PORTAL:
        case ACTORGIZMO_HEARTH_PORTAL:
        case ACTORGIZMO_SHARED_STASH:
        case ACTORGIZMO_BREAKABLEDOOR:
        case ACTORGIZMO_SWITCH:
        case ACTORGIZMO_PRESSUREPLATE:
        case ACTORGIZMO_GATE:
        case ACTORGIZMO_BANNER:
        case ACTORGIZMO_LORE_CHEST:
        case ACTORGIZMO_MAILBOX:
        case ACTORGIZMO_LOOTRUN_SWITCH:
        case ACTORGIZMO_POOL_OF_REFLECTION:
        case ACTORGIZMO_MULTICLICK:
        case ACTORGIZMO_WEEKLYCHALLENGE_SWITCH:
        case ACTORGIZMO_OBJECTIVE_FINDER:
            return TRUE;

        case ACTORGIZMO_PORTAL:
            if (pdefActor && TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_PORTAL_SETS_DYNAMIC_RETURN_LOCATION))
            {
                return TRUE;
            }
            break;
    }

    return FALSE;
}






static void sACDTryTrickleToPlayer(Player & tPlayer, const ACDID idACD)

{
    BOOL bTrickle = FALSE;
    ActorCommonData & tACD = ACDGet(idACD);
    switch (ActorGetType(SNOName(ACTOR_SNO, tACD.snoActor)))
    {
        case ACTORTYPE_MONSTER:
            if (TeamsAreFriendly(tACD.GetTeam(), tPlayer.eTeam) ||

                GameTypeIsPvP())
            {
                bTrickle = TRUE;
            }
            break;

        case ACTORTYPE_GIZMO:

            if (!sShouldTrickleGizmoType(tACD) &&

                !GameTypeIsPvP())
            {
                break;
            }

            // fall through
        case ACTORTYPE_SERVER_PROP:
        {

            const Actor * ptActor = ActorTryToGet((ActorID)tACD.idOwner);
            if (ptActor)
            {



                SNO snoIconOverride = tACD.AttributesGetInt(ATTRIBUTE_MINIMAP_ICON_OVERRIDE);
                SNO snoDefaultIcon = ActorGetInheritedTagMapValueSNO(ptActor, TAG_ACTOR_MINIMAP_ICON, TAG_MARKER_MINIMAP_ICON);

                if (SNOFileExists(TEXTURE_SNO, snoDefaultIcon) || SNOFileExists(TEXTURE_SNO, snoIconOverride))
                {
                    bTrickle = TRUE;
                }
            }
            break;
        }

        case ACTORTYPE_PLAYER:
            bTrickle = TRUE;
            break;
    }

    if (bTrickle)
    {
        SPlayerTrickleAdd(tPlayer, idACD);
    }
}

void STrickleTryTrickleToPlayers(const ACDID idACD)

{
    if (GameIsParamFlagSet(GPF_TRICKLE_ACDS_TO_PLAYER))
    {
        for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
        {
            sACDTryTrickleToPlayer(*ptPlayer, idACD);
        }
    }
}




void STrickleOnPlayerCreated(Player & tPlayer)

{
    if (GameIsParamFlagSet(GPF_TRICKLE_ACDS_TO_PLAYER))
    {
        ACDIterator tIter;
        while (ActorCommonDataIterate(tIter))
        {
            ActorCommonData & tACD = *tIter.ptElement;
            sACDTryTrickleToPlayer(tPlayer, tACD.id);
        }
    }
}




void STrickleUpdateActor(const ActorCommonData & tACD)

{
    const ActorID idActor = (ActorID)tACD.idOwner;
    if (idActor == ACTORID_INVALID)
    {
        return;
    }

    if (!STrickleIsActorTrickled(idActor))
    {
        return;
    }

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (STrickleIsActorTrickledToPlayer(idActor, *ptPlayer))
        {
            ServerTrickleList * pListOldTrickle = SPlayerGetPrevTrickleList(*ptPlayer);
            if (!pListOldTrickle)
            {
                return;
            }



            POS pos = pListOldTrickle->Find(tACD.id);
            if (pos)
            {
                pListOldTrickle->RemoveAt(pos);
            }
        }
    }
}
