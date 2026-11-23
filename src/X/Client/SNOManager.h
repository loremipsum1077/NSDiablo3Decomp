// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/SNOManager.h

#pragma once

#include "../Common/SNO/SNOName.h"
#include "../Sigma/DataStructures/XGrowableMap.h"

namespace SNOManager {
enum SNOCategory {
    SC_APP = 0,
    SC_GAME = 1,
    SC_SKILLS = 2,
    SC_CRAFTERS = 3,
    SC_HIRELING = 4,
    SC_ACT = 5,
    SC_GAMEACT = 6,
    SC_CLIENTUIS = 7,
    SC_CLIENTACTORS = 8,
    SC_CLIENT_ACT_WORLDS = 9,
    SC_CLIENT_ACT_SCENES = 10,
    SC_CLIENT_ACT_ACTORS = 11,
    SC_CLIENT_ACT_TEXTURES = 12,
    SC_SERVER_ACT_SCENES = 13,
    SC_SERVER_ACT_LEVELAREAS = 14,
    SC_SERVER_ACT_WORLDS = 15,
    SC_SERVER_ACT_WORLDSLOADING = 16,
    SC_SERVERACTORS = 17,
    NUMBER_OF_SNO_CATEGORIES = 18,
};

typedef XGrowableMap<CompressedSNOName, const CompressedSNOName &, int, int> SNOContainer;

}  // namespace SNOManager

// Functions defined in X/Client/SNOManager.cpp.
#include "Prefetch.h"
#include "../Common/SNO/SNOFilesAsync.h"
#include "../Common/SNO/SNOTOC.h"
struct RequiredDefinitionHeader;
namespace SNOManager {
void ReloadRequired(SNOGroup snoGroup);
void RequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, BOOL fPurgeNoLongerRequired, SNOFilesAsyncPriority ePriority);
void AddRequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, SNOFilesAsyncPriority ePriority);
void RemoveRequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, BOOL fPurgeNoLongerRequired);
BOOL WaitForSNOs(const SNONameArray & container, SNOName * ptSNONameReason, BOOL fIgnoreExpiredLoadingScreen);
void RequireSNOs(const SNOManager::SNOCategory category, const SNOManager::SNOContainer & container, BOOL fPurgeNoLongerRequired, SNOFilesAsyncPriority ePriority);
BOOL WaitForSNOs(const SNOManager::SNOContainer & container, SNOName * ptSNONameReason, BOOL fIgnoreExpiredLoadingScreen);
BOOL AreSNOsLoaded(const SNOManager::SNOContainer & container);
BOOL AnySNOsPending(const SNOManager::SNOContainer & container);
void OnSNOLoaded(const SNOName & snoName, RequiredDefinitionHeader * lpDefinition);
void AddSNOTree(SNOManager::SNOContainer & tContainer, const SNOName & snoName, Prefetch::FilterFunc ptFilterFunc);
void AddSNOsTree(SNOManager::SNOContainer & tContainer, Prefetch::ChildSNOContainer & tSNOsToProcess, Prefetch::FilterFunc ptFilterFunc);
void OnLoadingScreenCanStart();
void Update();
BOOL DisplayLoadingScreen();
void SetHasBlockedThisFrame();
BOOL HasBlockedLastFrame();
}  // namespace SNOManager
