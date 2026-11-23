// Original file: X/Common/GameDebug.cpp

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "../Sigma/Files/XFiles.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/Unicode.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Server/SGameManager.h"

// Defined in X/Sigma/Files/XFilesCore.cpp; no adopted header declares it yet.
void FileClose(FileReference & tFileRef);

struct GameDebugGlobals {  // size 0x1480
    ACDID atDebugUpdatedACDID[8];  // 0x00
    uint32 uDebugUpdateACDIDIndex;  // 0x20
    SSceneID atDeactivatedSceneID[8];  // 0x24
    SGameID atDeactivateSceneIDGames[8];  // 0x44
    uint32 uDebugDeactivatedSceneIndex;  // 0x64
    FileReference tCombatLog;  // 0x68
    BOOL bLogEnabled;  // 0x1078
    ACDID idACDDebugged;  // 0x107c
    CHAR szACDDebugInfo[1024];  // 0x1080

    ~GameDebugGlobals() {  // line 113
        if (bLogEnabled == TRUE) {
            FileClose(tCombatLog);
        }
    }
};
static_assert(sizeof(GameDebugGlobals) == 0x1480, "GameDebugGlobals");

// AppGlobals (X/Common/Game.h:940) comes from X/Common/Game.h via SGameManager.h.





extern AppGlobals sgtAppGlobals;

void GameDebugInitializeForApp() {  // line 313
}

void GameDebugDisposeForApp() {  // line 328
    AppGlobals & tAppGlobals = sgtAppGlobals;
    GameDebugGlobals *& ptGameDebugGlobals = tAppGlobals.ptGameDebugGlobals;

    if (ptGameDebugGlobals != NULL) {
        delete ptGameDebugGlobals;
    }
    ptGameDebugGlobals = NULL;
}

void GameDebugInitializeForGame() {  // line 339
}

void GameDebugDisposeForGame() {  // line 353
    SGame & tSGame = SGameManager::GameGet();

    if (tSGame.ptGameDebugGlobals != NULL) {
        tSGame.ptGameDebugGlobals->~GameDebugGlobals();
        SigmaMemoryFree(tSGame.ptGameDebugGlobals, tSGame.ptMemoryInterface);
    }
}

void GameDebugSetCurrentUpdatingActor(ACDID idACD) {  // line 368
}

void GameDebugSceneDeActivating(SSceneID idSScene) {  // line 394
}

void GameDebugDumpInfo() {  // line 815
}
