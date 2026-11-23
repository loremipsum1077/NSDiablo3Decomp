// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/DRLG.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "DRLGTypes.h"
#include "Definitions/WorldDefinition.h"
#include "GameBalance.h"
#include "../Server/SWorld.h"
#include "../Sigma/DataStructures/XTemplate.h"

struct RNG;
struct SceneCluster;
struct SubSceneEntry;

typedef int32 LevelAreaID;

class DRLGGenerator {  // size 0x150
    // vtable pointer
public:
    uint32 m_dwSeed;  // 0x08
    RNG * m_ptRNG;  // 0x10
    TPlacedSceneList m_listPlacedScenes;  // 0x18
    TPooledSpawnGroupExactInstanceList m_listSpawnGroupExact;  // 0x40
    SNO m_snoWorld;  // 0x68
    SWorldID m_idSWorld;  // 0x6c
    SceneSpecification m_tDefaultSceneSpec;  // 0x70
    DRLGGenerator();
    void Initialize(RNG & tRNG, SNO snoWorld, SWorldID idSWorld);
    BOOL PickSubScenes();
    BOOL PickSubScenesFromClusters();
protected:
    virtual void InitSpecific() = 0;  // vtable slot 0
    BOOL PickSubSceneForPlacedScene(POS posParentScene, BOOL bAbortOnError);
    BOOL ApplyClusterSubScene(SubSceneEntry & tSubSceneEntry, TPlacedSceneList & listSubScenes, GBID gbidForceLabel);
    void SelectDebugOverrideSubScenes(SceneCluster & tCluster, TPlacedSceneList & listSubScenes, int32 nOverrideIndex);
};
static_assert(sizeof(DRLGGenerator) == 0x150, "DRLGGenerator");

typedef int32 GridDataTileSets;


// Functions defined in this file (bodies are not in the DWARF):
//   line    23  DRLGGenerator::~DRLGGenerator()  [0x7100430320]
