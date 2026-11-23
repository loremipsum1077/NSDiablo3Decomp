// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/ActorCommon.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "SNO/SNOFiles.h"

struct InactiveMarkerOwner;

struct MinimapMarkerSetInfo {  // size 0x8
    SNO snoMarkerSet;  // 0x00
    int16 nMarkerIndex;  // 0x04
    // Lines 24 and 29. Bodies written from the copy inlined into Minimap::AddStaticGizmoMarker.
    void Initialize() {
        snoMarkerSet = SNO_NONE;
        nMarkerIndex = -1;
    }
    MinimapMarkerSetInfo() {
        Initialize();
    }
    MinimapMarkerSetInfo(const InactiveMarkerOwner &);
};
static_assert(sizeof(MinimapMarkerSetInfo) == 0x8, "MinimapMarkerSetInfo");

struct Marker;
typedef int32 GBID;  // X/Common/GameBalance.h

// Declared here from their definitions in ActorCommon.cpp.
float ActorGetInheritedTagMapValueFloat(const SNO snoActor, const Marker & tMarker, const uint32 dwActorTag,
                                        const uint32 dwMarkerTag);  // ActorCommon.cpp:147
int32 ActorGetInheritedTagMapValueInt(const SNO snoActor, const Marker & tMarker, const uint32 dwActorTag,
                                      const uint32 dwMarkerTag);  // ActorCommon.cpp:163
SNO ActorGetInheritedTagMapValueSNO(const SNO snoActor, const Marker & tMarker, const uint32 dwActorTag,
                                    const uint32 dwMarkerTag);  // ActorCommon.cpp:195
SNO ActorGetInheritedTagMapValueSNO(const SNO snoActor, const MinimapMarkerSetInfo & tMarkerOwner,
                                    const uint32 dwActorTag, const uint32 dwMarkerTag);  // ActorCommon.cpp:102
float ActorGetInheritedTagMapValueFloat(const SNO snoActor, const MinimapMarkerSetInfo & tMarkerOwner,
                                        const uint32 dwActorTag, const uint32 dwMarkerTag);  // ActorCommon.cpp:30
int32 ActorGetInheritedTagMapValueInt(const SNO snoActor, const MinimapMarkerSetInfo & tMarkerOwner,
                                      const uint32 dwActorTag, const uint32 dwMarkerTag);  // ActorCommon.cpp:54
uint32 ActorGetInheritedTagMapValueDWORD(const SNO snoActor, const MinimapMarkerSetInfo & tMarkerOwner,
                                         const uint32 dwActorTag, const uint32 dwMarkerTag);  // ActorCommon.cpp:78
GBID ActorGetInheritedTagMapValueGBID(const SNO snoActor, const MinimapMarkerSetInfo & tMarkerOwner,
                                      const uint32 dwActorTag, const uint32 dwMarkerTag);  // ActorCommon.cpp:125
uint32 ActorGetInheritedTagMapValueDWORD(const SNO snoActor, const Marker & tMarker, const uint32 dwActorTag,
                                         const uint32 dwMarkerTag);  // ActorCommon.cpp:179
GBID ActorGetInheritedTagMapValueGBID(const SNO snoActor, const Marker & tMarker, const uint32 dwActorTag,
                                      const uint32 dwMarkerTag);  // ActorCommon.cpp:211
float ActorGetHandicapHealthScalar(int32 nHandicapLevel);  // ActorCommon.cpp:228
float ActorGetHandicapHealthScalar();  // ActorCommon.cpp:246
BOOL ActorIsKeywarden(SNO snoActor);  // ActorCommon.cpp:255

enum {
    ACTOR_INITIAL_COUNT = 512,
    DEFAULT_ACTOR_SOFT_CAP = 7680,
    MAX_ACTORS = 8192,
};

enum {
    ACTOR_POOL_PAD_SIZE = 24,
    ANN_POOL_SIZE = 11676,
};


// Functions defined in this file (bodies are not in the DWARF):
//   line    24  void MinimapMarkerSetInfo::Initialize()  [inlined into 3 functions]
//   line    29  MinimapMarkerSetInfo::MinimapMarkerSetInfo()  [inlined into 2 functions]

// Variables defined in this file:
//   line    93  static const float MAX_MONSTER_VIEW_DISTANCE
