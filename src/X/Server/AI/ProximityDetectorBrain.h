// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/AI/ProximityDetectorBrain.h
// Subset: the brain data the ActorAI data array holds.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ActorAI.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/Math/Rect.h"

enum {
    AI_PROXIMITY_SHAPE_NONE = -1,
    AI_PROXIMITY_SHAPE_SPHERE = 0,
    AI_PROXIMITY_SHAPE_BOX = 1,
};

typedef int32 ProximityShape;

typedef void (*ProximityBrainCallback)(ActorID, ACDID);

typedef void (*ProximityBrainCountChangedCallback)(const ActorID, const int32, const int32, const BOOL);

enum {
    PROX_FLAG_DO_EFFECT = 0,
    PROX_FLAG_ONE_SHOT = 1,
    PROX_FLAG_SET_CHECKPOINT = 2,
    PROX_FLAG_PULSE = 3,
};

enum {
    PROXIMITYDETECTORID_INVALID = -1,
};

struct ProximityShapeData {  // size 0x30
    ProximityShape eShape;  // 0x00
    float flRadius;  // 0x04
    OBB wobb;  // 0x08
};
static_assert(sizeof(ProximityShapeData) == 0x30, "ProximityShapeData");

struct ProximityDetectorBrainData {  // size 0x90
    ProximityDetectorDataID id;  // 0x00
    uint32 dwFlags;  // 0x04
    ProximityShapeData tShape;  // 0x08
    uint32 dwNextPulseTime;  // 0x38
    uint32 dwPulseInterval;  // 0x3c
    uint32 dwPulseIntervalDelta;  // 0x40
    ProximityBrainCallback pOnPlayerEnteredCallback;  // 0x48
    ProximityBrainCallback pOnPlayerExitedCallback;  // 0x50
    ProximityBrainCountChangedCallback pOnCountChangedCallback;  // 0x58
    SNO snoTriggerActor;  // 0x60
    XPooledDataIDList listACDs;  // 0x68
};
static_assert(sizeof(ProximityDetectorBrainData) == 0x90, "ProximityDetectorBrainData");

void ProximityDetectorGetActorsInBoundsList(ActorID idTrigger, XPooledDataIDList & tListDetectedACDs, BOOL bPlayersOnly);  // ProximityDetectorBrain.cpp:594
