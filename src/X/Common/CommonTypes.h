// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/CommonTypes.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"

enum {
    ATTACHMENTID_INVALID = -1,
};

typedef int32 AttachmentID;

typedef int32 SGameID;

enum {
    SWORLDID_INVALID = -1,
};

typedef int32 SWorldID;

enum RActorID {
    RACTORID_INVALID = -1,
};

enum ActorID {
    ACTORID_INVALID = -1,
};

enum ACDID {
    ACDID_INVALID = -1,
};

enum {
    SSCENEID_INVALID = -1,
};

typedef int32 SSceneID;

enum ACDNetworkName {
    ANN_INVALID = -1,
};

typedef int32 GameMessageType;

struct RequiredMessageHeader {  // size 0x8
    uint16 dwSize;  // 0x00
    int8 nControllerIndex;  // 0x02
    int8 nPadding;  // 0x03
    GameMessageType eType;  // 0x04
};
static_assert(sizeof(RequiredMessageHeader) == 0x8, "RequiredMessageHeader");

struct NavMeshHandle {  // size 0x8
    int32 nSquareIndex;  // 0x00
    SSceneID idSScene;  // 0x04
    BOOL operator==(const NavMeshHandle & rhs) const;
};
static_assert(sizeof(NavMeshHandle) == 0x8, "NavMeshHandle");

enum {
    MAX_HERO_NAME_LENGTH = 12,
    MAX_HERO_NAME_UTF8_BYTECOUNT = 49,
    MAX_ACCOUNT_NAME_BYTECOUNT = 128,
    MAX_GAME_NAME_LENGTH = 33,
    MAX_STRING_LABEL_LENGTH = 128,
    MAX_PLAYERS = 4,
    MAX_COOP_PLAYERS = 4,
    NUM_RARE_NAMES = 2,
    NUM_MONSTER_AFFIXES = 8,
    MAX_WAYPOINTS_PER_ACT = 100,
    MAX_SAVED_WAYPOINTS = 32,
    BASE_HERO_COUNT = 4,
    MAX_HEROES_VANILLA = 10,
    MAX_HEROES = 12,
    MAX_HEROES_X1_COLLECTORS = 15,
    MAX_NUMBER_FALLEN_HEROES = 10,
    EXTRA_HEROES_NECROMANCER = 2,
    MAX_HEROES_X1_COLLECTORS_WITH_NECROMANCER = 17,
    MAX_HEROES_CONSOLE = 17,
    MAX_LOCAL_PLAYERS = 4,
};


// Functions defined in this file (bodies are not in the DWARF):
//   line    78  BOOL NavMeshHandle::operator==(const NavMeshHandle & rhs) const  [inlined into 1 functions]

// Variables defined in this file:
//   line    27  static const SGameID SGAMEID_INVALID
//   line    86  static const NavMeshHandle NAVMESH_HANDLE_NONE

// Original CommonTypes.h:27 and :86; constant bytes verified in the wrapper.
static const SGameID SGAMEID_INVALID = -1;
static const NavMeshHandle NAVMESH_HANDLE_NONE = {-1, SSCENEID_INVALID};

// Original CommonTypes.h:78, inlined in _wrap_NavMeshHandle___eq.
inline BOOL NavMeshHandle::operator==(const NavMeshHandle &rhs) const
{
    return nSquareIndex != -1 && nSquareIndex == rhs.nSquareIndex && idSScene == rhs.idSScene;
}

// CommonTypes.cpp: reflected message header used by network message descriptors.
class XTypeDescriptor;
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER;
