// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/NodePath.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "Definitions/EncounterDefinition.h"
#include "NodePathSwig.h"
#include "../Sigma/Math/Vector.h"

enum {
    PATH_NODE_FLAG_PATH_END = 0,
};

typedef int32 Team;

struct NodePathNode {  // size 0x1c
    uint32 tName;  // 0x00
    Vector3D wpPos;  // 0x04
    float flRadius;  // 0x10
    uint32 dwFlags;  // 0x14
    Team eTeam;  // 0x18
};
static_assert(sizeof(NodePathNode) == 0x1c, "NodePathNode");

enum {
    PATH_FLAG_DIRTY = 0,
};

struct NodePath {  // size 0x28
    PathID id;  // 0x00
    NodePathNode * arPathNodes;  // 0x08
    int32 nPathNodeCount;  // 0x10
    int32 nPathNodeMaxCount;  // 0x14
    uint32 dwFlags;  // 0x18
    PathClass ePathClass;  // 0x1c
    SWorldID idSWorld;  // 0x20
};
static_assert(sizeof(NodePath) == 0x28, "NodePath");


// Functions defined in this file (bodies are not in the DWARF):
//   line    26  NodePathNode::NodePathNode(const NodePathNode &)  [inlined into 1 functions]
//   line    26  NodePathNode & NodePathNode::operator=(const NodePathNode &)  [inlined into 1 functions]

PathID NodePathFind(PathClass ePathClass, SWorldID idSWorld);
PathNodeID NodePathGetNodeByName(PathID idPath, uint32 tName);
