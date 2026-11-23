// X/Server/AI/BrainTranslate.cpp: the "walk to a place" sub-state machine.
// Its per-game block is a single word, so the whole TU's globals fit in the four bytes
// SigmaMemoryNew hands back.

#include "BrainTranslate.h"

#include "../SGameManager.h"
#include "../Actors/Actors.h"
#include "../Movement/SActorMovement.h"
#include "../../Common/ActorCommonData.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "ActorAI.h"

struct BrainTranslateGlobals {  // size 0x4
    uint32 dwGlobalNextLongPathTime;  // 0x00
};
static_assert(sizeof(BrainTranslateGlobals) == 0x4, "BrainTranslateGlobals");

void BrainTranslateInitForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    BrainTranslateGlobals *& ptTransGlobals = tSGame.ptBrainTranslateGlobals;

    ptTransGlobals = (BrainTranslateGlobals *)SigmaMemoryNew(sizeof(BrainTranslateGlobals), 4,
                                                             tSGame.ptMemoryInterface, 1);
    ptTransGlobals->dwGlobalNextLongPathTime = 0;
}

void BrainTranslateDisposeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    BrainTranslateGlobals *& ptTransGlobals = tSGame.ptBrainTranslateGlobals;

    SigmaMemoryFree(ptTransGlobals, tSGame.ptMemoryInterface);
    ptTransGlobals = NULL;
}

void BrainTranslateCancel(BrainComponentTranslate & tTranslate, BOOL bForceByPopping)
{
    ActorID idActor = ACTORID_INVALID;

    tTranslate.dwDoneWaitingTime = 0;
    tTranslate.dwDoneSteppingAwayTime = 0;
    tTranslate.idACDTarget = ACDID_INVALID;

    AITrace(tTranslate.idACD, "brain translate cancel");

    if (tTranslate.idACD != ACDID_INVALID) {
        idActor = (ActorID)ACDGetOwner(tTranslate.idACD);
    }

    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor != NULL && ptActor->ptSActorMovement != NULL && ptActor->ptSActorMovement->IsMoving()) {
        if (!bForceByPopping || ptActor->ptSActorMovement->IsInArcingMovement()) {
            ptActor->ptSActorMovement->StopSoon();
        } else {
            ptActor->ptSActorMovement->StopNow();
        }
    }
}
