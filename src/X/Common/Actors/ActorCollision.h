// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Actors/ActorCollision.h

#pragma once

enum ActorSphereTreeTypes {
    ACTOR_SPHERE_TREE_NORMAL = 0,
    ACTOR_SPHERE_TREE_FAST_MOVER = 1,
    ACTOR_SPHERE_TREE_ITEMS_IN_WORLD = 2,
    ACTOR_SPHERE_TREE_NUM_TYPES = 3,
    ACTOR_SPHERE_TREE_FLAGS_ALL = 7,
    ACTOR_SPHERE_TREE_FLAGS_NO_ITEMS = 3,
};


#include "../../Sigma/DataStructures/DataArraysTemplated.h"
class ActorCommonData;
void GetACDsInRadius(const ActorCommonData & tACD, const float flRadius, XPooledDataIDList & listACDs);  // ActorCollision.cpp:601

#include "../WorldPlace.h"
void ACDsCylinderIntersectSphere(const WorldPlace & placeCenter, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // ActorCollision.cpp:349

// No header declarations recovered; owning-header placement inferred from ActorCollision.cpp:185 and :205.
void ActorCollisionInitialize(ActorCommonData & tACD);
void ActorCollisionDispose(ActorCommonData & tACD);
