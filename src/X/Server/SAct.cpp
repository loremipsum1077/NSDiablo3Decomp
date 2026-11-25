// X/Server/SAct.cpp
#include "SAct.h"

#include "../Sigma/Memory/SigmaMemory.h"
#include "../Common/ActorCommonData.h"
#include "../Common/CommonTypes.h"
#include "../Common/Definitions/ActDefinition.h"
#include "../Common/GameSwig.h"
#include "../Common/Player.h"
#include "../Common/Protobuf/Account.pb.h"
#include "../Common/Protobuf/Hero.pb.h"
#include "../Common/Protobuf/Items.pb.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/World.h"
#include "SGame.h"
#include "SGameManager.h"
#include "SPlayer.h"
#include "SWorld.h"

// The per-game act block. The DWARF records its layout in this file; SGame.h
// only refers to it through SActGlobals *.
struct SActGlobals {  // size 0x4
    int32 nUnused;  // 0x00
};
static_assert(sizeof(SActGlobals) == 0x4, "SActGlobals");

void SActInitializeForGame() {
    SGame & tSGame = SGameManager::GameGet();

    tSGame.ptSActGlobals =
        (SActGlobals *)SigmaMemoryNew(sizeof(SActGlobals), 4, tSGame.ptMemoryInterface, TRUE);
}

void SActDisposeForGame() {
    SGame & tSGame = SGameManager::GameGet();

    SigmaMemoryFree(tSGame.ptSActGlobals, tSGame.ptMemoryInterface);
    tSGame.ptSActGlobals = NULL;
}

SWorldID SActTravelTo(const Act eAct, Player & tPlayer, ResolvedPortalDestination & tDest,
                      SWorldID idSWorldToUse) {
    SWorldID idSWorld = idSWorldToUse;

    if (tDest.snoWorld == SNO_NONE) {
        idSWorld = SWORLDID_INVALID;
    } else {
        if (idSWorld == SWORLDID_INVALID) {
            idSWorld = SWorldCreate(tDest.snoWorld, TRUE, FALSE, NULL, ACDID_INVALID, ACDID_INVALID,
                                    tDest.uEntranceGUID, WARP_UNKNOWN, FALSE, 0);
        }

        SPlayerEnterAct(tPlayer, eAct, FALSE);

        ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

        SWorldPlayerEnter(idSWorld, tACDPlayer.id, &tPlayer, tDest.uEntranceGUID, FALSE,
                          D3::Hero::SavedDefinition::default_instance(),
                          D3::Account::SavedDefinition::default_instance(),
                          D3::Items::Mails::default_instance(), NULL);

        SPlayerPlayActInterstitial(tPlayer, eAct, TRUE);

        SPlayerWarped(tACDPlayer, WARP_CHANGING_ACTS, NULL, ACDID_INVALID, 0.0f, TRUE);

        tACDPlayer.MarkHeroStateChanged();
    }

    return idSWorld;
}
