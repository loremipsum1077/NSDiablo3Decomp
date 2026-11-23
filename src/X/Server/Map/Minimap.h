// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Map/Minimap.h

#pragma once

// Lua binding declarations reconstructed from the owning Minimap.cpp DWARF.
// The parameter types are declared by the includes below.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"

void MinimapDeactivateMarker(const char *pszMarker);  // Minimap.cpp:1721
void MinimapActivateMarker(const char *pszMarker);  // Minimap.cpp:1728
void MinimapDeactivateMarker(const ACDID idACD);  // Minimap.cpp:1735
void MinimapActivateMarker(const ACDID idACD);  // Minimap.cpp:1742
void MinimapEnableMarkerArrow(const ACDID idACD, BOOL bEnable);  // Minimap.cpp:1789
void MinimapEnableMarkerArrow(const char *pszMarker, BOOL bEnable);  // Minimap.cpp:1797

class CRefString;
struct Actor;
class ActorCommonData;
struct Marker;
struct PRTransform;
struct SceneCommonData;

typedef int32 PlayerWarpReason;

enum {
    PINGID_INVALID = -1,
};

typedef int32 PingID;

namespace Minimap {
struct DiscoveryInfo {  // size 0x18
    uint32 m_nHashedName;  // 0x00
    WorldPlace m_Place;  // 0x04
    float m_fDiscoveryRangeSq;  // 0x14
};
static_assert(sizeof(DiscoveryInfo) == 0x18, "DiscoveryInfo");

// Declarations of the functions Minimap.cpp defines (the DWARF gives each one's signature).
void CreateMarkerListForGame();
void DisposeMarkerListForGame();
void ChangeActs();
void AddMarker(const Marker & tMarker, const SceneCommonData * ptSCD, const PRTransform * ptTransform);
void AddQuestMarker(const Marker & tMarker, const SceneCommonData * ptSCD);  // Minimap.cpp:870
void AddPortalMarker(const SNO snoMarkerSet, const Marker & tMarker, const SceneCommonData * ptSCD,
                     const PRTransform * ptTransform);
void AddSNOLocationMarker(const SNOName snoName, const WorldPlace tPlace, SSceneID idSScene);
void RemoveAllForWorld(SWorldID idSWorld);
CRefString GetMarkerNameFromPortal(const Actor * ptPortal);
void ResolveMouseOverNameForActor(const Actor * ptActor, uint32 & hStringListHandle, SNO & snoStringList);
void ActivateMarker(const char * pszMarker);
void ActivateMarker(const ACDID idACD);
void DeactivateMarker(const ACDID idACD, BOOL bFromScript);
void DeactivateMarker(const char * pszMarker, BOOL bFromScript);
void MarkerSetIcon(const SNO snoIcon, const ACDID idACD);
void MarkerSetIcon(const SNO snoIcon, const char * pszMarker);
void EnableMarkerArrow(const ACDID idACD, const BOOL bEnable);
void EnableMarkerArrow(const char * pszMarker, const BOOL bEnable);
void sPingDeleteFromTimer(int32, DataID idOwner, void *);
PingID PingLocation(const WorldPlace & placePing, int32 nDuration, OnlineService::HeroId idAssignedHero,
                    SNO snoTexture, BOOL bCanBecomeArrow, uint32 hLabel, SNO snoStringList,
                    SNO snoKnownActorOverride, float flMaxDisplayRange, SNO snoQuestSource);
void UpdatePingLocation(PingID idPing, WorldPlace tPlace);
void RemovePing(PingID idPing);
BOOL LevelAreaAutoPing(SNO snoLevelArea, SNO snoQuestSource, float flMaxDisplayRange);
void ChallengeAutoPing(SNO snoChallenge, SNO snoQuestSource);
void ActorAutoPing(ACDID idACD, SNO snoQuestSource);
void EventAutoPing(SNO snoEvent, SNO snoQuestSource);
void SNOAddAutoPingRef(SNO sno, SNOGroup snoGroup, SNO snoQuestSource, BOOL bIsBossBounty);
void SNORemoveAutoPingRef(SNO sno);
void SNOAddTrackedPing(SNO sno, PingID idPing);
BOOL SNOHasAutoPing(SNO sno);
PingID SNOGetAutoPing(SNO sno);
BOOL IsValid(PingID idPing);

void DiscoverMinimapMarkers(const ActorCommonData & tACDPlayer);  // Minimap.cpp:1063
}  // namespace Minimap

// Functions defined in this file (bodies are not in the DWARF):
//   line    33  Minimap::DiscoveryInfo & Minimap::DiscoveryInfo::operator=(const Minimap::DiscoveryInfo &)  [inlined into 2 functions]

void MinimapSetIcon(const ACDID idACD, const char * szIconName);  // X/Server/Map/Minimap.cpp:1749

void MinimapSetIcon(const char * pszMarker, const char * szIconName);  // X/Server/Map/Minimap.cpp:1769
struct Player;
namespace Minimap {
void ActivateWaypointMarker(const SNO snoLevelArea);  // Minimap.cpp:1247
void OnRemovedPlayer(const Player & tPlayer);  // Minimap.cpp:1144
}
