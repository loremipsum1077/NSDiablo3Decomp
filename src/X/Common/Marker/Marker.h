// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/Marker.h
// Subset: Marker and MarkerLink. The full reference header pulls in the client's RActors.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "MarkerType.h"
#include "../SNO/SNOFiles.h"
#include "../SNO/SNOName.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Math/Transform.h"

class CTagMap;

struct MarkerLink {  // size 0x8
    uint32 dwHash;  // 0x00
    uint32 dwGroupHash;  // 0x04
};
static_assert(sizeof(MarkerLink) == 0x8, "MarkerLink");

struct Marker {  // size 0x58
    uint32 dwHash;  // 0x00
    uint32 dwHashPadding;  // 0x04
    MarkerType eType;  // 0x08
    PRTransform transform;  // 0x0c
    SNOName snoname;  // 0x28
    SerializeData serTagMap;  // 0x30
    XPtr64<CTagMap> hTagMap;  // 0x38
    int32 nLinkCount;  // 0x40
    SerializeData serMarkerLinks;  // 0x44
    XPtr64<MarkerLink> ptMarkerLinks;  // 0x50
};
static_assert(sizeof(Marker) == 0x58, "Marker");

// Reference Marker.h:72.
struct MarkerSetTranslation {  // size 0x1c
    Vector3D wpAnchor;  // 0x00
    Vector3D wpShift;  // 0x0c
    Angle aPivot;  // 0x18
};
static_assert(sizeof(MarkerSetTranslation) == 0x1c, "MarkerSetTranslation");

// Reference Marker.h:79.
enum MarkerSetCreateOptions {
    MCO_ENCOUNTERS_ONLY_BIT = 0,
    MCO_NO_ENCOUNTERS_BIT = 1,
    MCO_INSTANCE_IMMEDIATELY_BIT = 2,
    MCO_FOR_ADVENTURE_BIT = 3,
};

struct SceneCommonData;
struct ResolvedPortalDestination;
typedef int32 PortalResolveMethod;

uint32 MarkerStringHash(LPCSTR pszName);  // Marker.cpp:204
PortalResolveMethod MarkerResolvePortalDestination(const Marker & tMarker, const SceneCommonData & tSCD,
                                                   ResolvedPortalDestination & tResolvedDest);  // Marker.cpp:2857

BOOL MarkerCheckReference(SNO snoMarkerSet, int nMarkerIndex, const Marker & tMarker,
                          SNOGroup snoGroup, SNO snoHandle);  // Marker.cpp:608

// Marker.cpp exports continue. DWARF records only the definition DIE for
// gtransformMarkerToScene (Marker.cpp:2763, DW_OP_addr 0x18f7c48); clang 5 emits no
// extern-declaration DIE, so the DWARF cannot name a declaring header. This placement
// therefore follows the header's existing Marker.cpp-export convention.
extern PRTransform gtransformMarkerToScene;  // Marker.cpp:2763
void SetMarkerToSceneTransform(const PRTransform & transform);  // Marker.cpp

const CHAR * MarkerNameGetString(uint32 dwHash);  // Marker.cpp:176
extern LPCSTR LOCAL_MARKER_SCOPE_TOKEN;  // Marker.cpp:151

// MarkerInstance, MarkerGlobals and their container typedefs (reference Marker.h). The containers
// are only held by pointer here, so their templates are forward declared rather than included.
template <typename TYPE, typename TYPEID> class TChainedDataArray;
template <typename TYPE, typename ARG_TYPE> class XGrowableList;
template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE> class XGrowableMap;

enum {
    MARKERID_INVALID = -1,
};

typedef int32 MarkerID;  // also typedef'd in RActors.h:160 (identical redeclaration)

struct MarkerInstance {  // size 0x14
    MarkerID id;  // 0x00
    RActorID idPlaceholderRActor;  // 0x04
    int32 nMarkerIndex;  // 0x08
    SNO snoMarkerSet;  // 0x0c
    SSceneID idSScene;  // 0x10
    void DeleteRActors();  // CMarker.cpp:53
};
static_assert(sizeof(MarkerInstance) == 0x14, "MarkerInstance");
static_assert(__builtin_offsetof(MarkerInstance, idPlaceholderRActor) == 0x04, "MarkerInstance");
static_assert(__builtin_offsetof(MarkerInstance, idSScene) == 0x10, "MarkerInstance");

class GameCommonData;
const MarkerInstance & MarkerInstanceGet(GameCommonData & tGCD, const MarkerID idMarker);  // Marker.cpp:631

typedef TChainedDataArray<MarkerInstance, int> TMarkerInstanceArray;

typedef XGrowableList<MarkerInstance *, MarkerInstance *> TMarkerIntancePtrArray;

typedef XGrowableMap<int, const int &, XGrowableList<MarkerInstance *, MarkerInstance *> *, XGrowableList<MarkerInstance *, MarkerInstance *> *> TMarkerSetToMarkerInstanceMap;

struct MarkerGlobals {  // size 0x18
    TMarkerInstanceArray * ptMarkerInstances;  // 0x00
    TMarkerSetToMarkerInstanceMap * ptMarkerInstancesBySet;  // 0x08
    uint32 nMarkerSetInstances;  // 0x10
};
static_assert(sizeof(MarkerGlobals) == 0x18, "MarkerGlobals");
static_assert(__builtin_offsetof(MarkerGlobals, ptMarkerInstancesBySet) == 0x08, "MarkerGlobals");
static_assert(__builtin_offsetof(MarkerGlobals, nMarkerSetInstances) == 0x10, "MarkerGlobals");

void sFreeAllRelatedMarkerInstances(TMarkerSetToMarkerInstanceMap * ptMarkerInstancesBySet);  // Marker.cpp:572

BOOL MarkerKeepActivatedWithScene(SNO snoActor);  // Marker.cpp:1756
