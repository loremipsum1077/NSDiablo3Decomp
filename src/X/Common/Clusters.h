// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Clusters.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "GameBalance.h"
#include "SNO/SNOFiles.h"
#include "../Sigma/DataStructures/XPtr64.h"
#include "../Sigma/Files/XSerializeData.h"
#include "../Sigma/Unicode.h"

class CRefString;

struct SubSceneLabel {  // size 0x8
    GBID gbidLabel;  // 0x00
    int32 nWeight;  // 0x04
    void Initialize();
    void InitializeFrom(const SubSceneLabel *);
};
static_assert(sizeof(SubSceneLabel) == 0x8, "SubSceneLabel");

struct SubSceneEntry {  // size 0x20
    SNO snoScene;  // 0x00
    int32 nWeight;  // 0x04
    int32 nNumLabels;  // 0x08
    XPtr64<SubSceneLabel> arLabels;  // 0x10
    SerializeData serLabels;  // 0x18
    void Initialize();
    void InitializeFrom(const SubSceneEntry *);
};
static_assert(sizeof(SubSceneEntry) == 0x20, "SubSceneEntry");

struct SubSceneGroup {  // size 0x20
    SNO snoCondition;  // 0x00
    int32 nNumScenesToChoose;  // 0x04
    int32 nNumSubScenes;  // 0x08
    XPtr64<SubSceneEntry> arSubScenes;  // 0x10
    SerializeData serSubScenes;  // 0x18
    void Initialize();
    void InitializeFrom(const SubSceneGroup *);
};
static_assert(sizeof(SubSceneGroup) == 0x20, "SubSceneGroup");

struct SceneCluster {  // size 0xb8
    CHAR szName[128];  // 0x00
    uint32 uClusterID;  // 0x80
    int32 nNumSubSceneGroups;  // 0x84
    XPtr64<SubSceneGroup> arSubSceneGroups;  // 0x88
    SerializeData serSubSceneGroups;  // 0x90
    SubSceneGroup tDefaultSubSceneGroup;  // 0x98
    void Initialize(uint32);
    void GetName(CRefString & strName) const;
};
static_assert(sizeof(SceneCluster) == 0xb8, "SceneCluster");

struct SceneClusterSet {  // size 0x18
    int32 nNumClusters;  // 0x00
    XPtr64<SceneCluster> arClusters;  // 0x08
    SerializeData serClusters;  // 0x10
};
static_assert(sizeof(SceneClusterSet) == 0x18, "SceneClusterSet");

