// X/Common/Marker/SmartAdventureMarker.cpp
// AdventureMarker is the MARKERTYPE_ADVENTURE MarkerBase implementation. It adds no data
// member to MarkerBase, so its size stays 0x18; the destructor and IsForClient are defined
// in SmartAdventureMarker.h, so GetActorForEdit, CreateForServerGame and CreateForClientGame
// are the only out-of-line bodies here.
//
// The functions sit on their original lines: GetActorForEdit 21, CreateForServerGame 37
// and CreateForClientGame 84.

#include "SmartAdventureMarker.h"

#include "../GlobalSNO.h"
#include "../Definitions/AdventureDefinition.h"

// GetActorForEdit resolves the marker's edit actor from the AdventureDefinition that the
// marker's snoSource names. The DWARF records the parameter on line 22, the definition pointer
// on line 24 and the symbol on line 25; when the adventure has no symbol actor (SNO_NONE) it
// falls back to the globally registered adventure symbol actor
// (GLOBALSNO_ACTOR_ADVENTURE_SYMBOL_ACTOR = 1150), which every adventure shares.

SNO AdventureMarker::GetActorForEdit(
    SNO snoSource)
{
    const AdventureDefinition * ptAdventureDef = (const AdventureDefinition *)gptGroupDefAdventure->LockRFast(snoSource);
    SNO snoSymbol = ptAdventureDef->snoSymbolActor;

    // 0xffffffff (SNO_NONE) means the adventure has no symbol actor of its own.
    if (snoSymbol == SNO_NONE)
    {
        return GlobalSNOGet(GLOBALSNO_ACTOR_ADVENTURE_SYMBOL_ACTOR);
    }

    return snoSymbol;
}

// CreateForServerGame returns DataID(-1); its parameters are recorded on lines 38-42.
DataID AdventureMarker::CreateForServerGame(
    const SSceneID idSScene,
    const WorldPlace & placePosition,
    const Quaternion & qOrientation,
    const Marker & tMarker,
    const InactiveMarkerOwner & tMarkerOwner)
{
    return -1;
}

// CreateForServerGame and CreateForClientGame both return DataID(-1).




































DataID AdventureMarker::CreateForClientGame(
    const SWorldID idSWorld,
    const SSceneID idSScene,
    const PRTransform & transform,
    const Marker & tMarker,
    const InactiveMarkerOwner & tMarkerOwner)
{
    return -1;
}
