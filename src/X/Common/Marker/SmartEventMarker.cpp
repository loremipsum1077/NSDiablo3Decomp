// X/Common/Marker/SmartEventMarker.cpp
// MARKERTYPE_EVENT's MarkerBase implementation. ~EventMarker is implicit (the
// DWARF marks it artificial) and the constructor is only declared in the header;
// the three out-of-line bodies below sit on their original lines (21, 29, 50).

#include "SmartEventMarker.h"

#include "../GlobalSNO.h"

// X/Server/Marker/SMarker.cpp:406. The declaration belongs to
// X/Server/Marker/SMarker.h, which is not adopted in src/ yet, so it is
// repeated here (the sibling decomp of the 1.0 build declares it locally too).
void MarkerLoadQuestForServerGame(const SSceneID idSScene, const WorldPlace & placePosition,
                                  const Quaternion & qOrientation, const Marker & tMarker,
                                  const InactiveMarkerOwner & tMarkerOwner);





SNO EventMarker::GetActorForEdit(
    SNO snoSource)
{
    return GlobalSNOGet(GLOBALSNO_ACTOR_EVENT);
}



DataID EventMarker::CreateForServerGame(
    const SSceneID idSScene,
    const WorldPlace & placePosition,
    const Quaternion & qOrientation,
    const Marker & tMarker,
    const InactiveMarkerOwner & tMarkerOwner)
{
    MarkerLoadQuestForServerGame(idSScene, placePosition, qOrientation, tMarker, tMarkerOwner);






    return -1;
}





DataID EventMarker::CreateForClientGame(
    const SWorldID idSWorld,
    const SSceneID idSScene,
    const PRTransform & transform,
    const Marker & tMarker,
    const InactiveMarkerOwner & tMarkerOwner)
{
    return -1;
}
