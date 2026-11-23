// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/SNOPreload.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 WeaponClass;

namespace SNOPreload {
enum FilteringSNOTypes {
    FILTERSNOTYPE_ACTOR = 0,
    FILTERSNOTYPE_SCENE = 1,
    FILTERSNOTYPE_SCENELOADING = 2,
    FILTERSNOTYPE_WORLDLOADING = 3,
    FILTERSNOTYPE_LEVELAREA = 4,
    FILTERSNOTYPE_WORLD = 5,
    FILTERSNOTYPE_POWER = 6,
    FILTERSNOTYPE_UI = 7,
    FILTERSNOTYPE_QUEST = 8,
    FILTERSNOTYPE_COUNT = 9,
};

enum FilteringTypes {
    FILTERTYPE_CLIENT = 0,
    FILTERTYPE_SERVER = 1,
    FILTERTYPE_BOTH = 2,
    FILTERTYPE_COUNT = 2,
};

enum RootTypes {
    ROOTTYPE_CLIENT = 0,
    ROOTTYPE_FULL_UI = 1,
    ROOTTYPE_SERVER = 2,
    ROOTTYPE_COUNT = 3,
};

}  // namespace SNOPreload

// The public interface (X/Client/SNOPreload.cpp:2862-3469). The DWARF skeleton carries no
// declarations for them; the signatures are the mangled names.
#include "../Common/CommonTypes.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/SNO/SNOFilesAsync.h"
#include "../Common/SNO/SNOTOC.h"

typedef int32 Act;   // X/Common/Act.h
typedef int32 GBID;  // X/Common/GameBalance.h

namespace SNOPreload {
void RefreshSNOResolve();
void InitializeForGame();
BOOL IsInitializedForGame();
void DisposeForGame();
void InitializeForApp();
BOOL IsInitializedForApp();
void DisposeForApp();
void AddClientUI(const SNO snoUI);
BOOL AreAllClientUIChildrenLoaded(const SNO snoUI);
BOOL AddClientUIChildren(const SNO snoUI);
void RemoveClientUIChildren(const SNO snoUI);
void WaitForClientUIChildren(const SNO snoUI);
void WaitForAllUIRoots();
SNO GetClientWorld();
void SetClientWorld(const SNO snoWorld);
void WaitForFullClientWorld(const SNO snoWorld);
void AddFullClientWorld(const SNO snoWorld, BOOL fWaitForSNOs);
void AddNeutralClientActor(const SNO snoActor, BOOL fWaitForSNOs);
void RemoveClientSceneBySNO(const SNO snoScene);
void AddClientSceneBySNO(const SNO snoScene);
void RemoveClientWeatherBySNO(const SNO snoWeather);
void AddClientWeatherBySNO(const SNO snoWeather);
void RemoveClientScene(const SSceneID idSScene);
void WaitForClientScene(const SNO snoScene);
void AddClientScene(const SSceneID idSScene, const SNO snoScene, const SNO * ptSNOLevelAreas);
BOOL PrefetchClientWorld(const SNO snoWorld, SNOFilesAsyncPriority ePriority);
BOOL PrefetchClientScene(const SNO snoScene, SNOFilesAsyncPriority ePriority);
BOOL PrefetchClientLevelArea(const SNO snoLevelArea, SNOFilesAsyncPriority ePriority);
void AddClientTextures(const SNONameArray & snoTextures);
void RemoveClientTextures(const SNONameArray & snoTextures);
BOOL PrefetchClientWeather(const SNO snoWeather, SNOFilesAsyncPriority ePriority);
BOOL PrefetchClientActor(const SNO snoActor, const GBID * pgbidMonsterAffixes, SNOFilesAsyncPriority ePriority);
void WaitForClientActor(const ACDNetworkName ann);
BOOL IsClientActorLoaded(const ACDNetworkName ann);
BOOL IsClientActorPending(const ACDNetworkName ann);
BOOL IsClientActorRequested(const ACDNetworkName ann);
void RemoveClientActor(const ACDNetworkName ann);
void AddClientActorBySNO(const SNO snoActor);
void RemoveClientActorBySNO(const SNO snoActor);
void AddClientActor(const ACDNetworkName ann, const SNO snoActor, const WeaponClass eWeaponClass, const GBID * pgbidMonsterAffixes);
BOOL ApplyHotfix(const char * szHotfixName);
void SetAct(Act eAct);
void AddServerActorBySNO(const SNO snoActor);
void RemoveServerActorBySNO(const SNO snoActor);
void PrefetchServerActorAvenger(const SNO snoActor, const GBID * pgbidMonsterAffixes);
void RemoveServerWorld(const SNO snoWorld);
void AddServerWorld(const SNO snoWorld);
void RemoveServerWorldLoading(const SNO snoWorld, BOOL fDungeonFinder);
void AddServerWorldLoading(const SNO snoWorld, BOOL fDungeonFinder);
void PrefetchWorldLoading(const SNO snoWorld, SNOFilesAsyncPriority ePriority);
BOOL IsServerWorldLoadingDone(const SNO snoWorld, BOOL fDungeonFinder);
void PrefetchServerScene(const SNO snoScene, const SNO * ptSNOLevelAreas);
void AddServerSceneBySNO(const SNO snoScene);
void RemoveServerSceneBySNO(const SNO snoScene);
void RemoveServerLevelArea(const SNO snoLevelArea);
void AddServerLevelArea(const SNO snoLevelArea);
void RemoveServerSkillKit(const SNO snoSkillKit);
void AddServerSkillKit(const SNO snoSkillKit);
void UpdateSkills();
void UpdateEquipped();
void UpdateCrafters();
void UpdateHireling();
}  // namespace SNOPreload
