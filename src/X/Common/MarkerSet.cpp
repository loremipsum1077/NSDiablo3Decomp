// Original file: X/Common/MarkerSet.cpp

#include "MarkerSet.h"
#include "Definitions/MarkerSetDefinition.h"

void MarkerSetInitialize(MarkerSet & tMarkerSet) {
    tMarkerSet.Initialize();
}

BOOL MarkerSetIterateMarkers(SNO snoMarkerSet, MarkerIterateCb pfuncIterate, void * ptUser) {
    BOOL bContinueSearch = TRUE;
    ScopedMarkerSetDefinitionLockR ptMarkerSetDef(snoMarkerSet);

    for (int32 i = 0; i < ptMarkerSetDef->tMarkerSet.GetArraySize(); i++) {
        const Marker & tMarker = ptMarkerSetDef->tMarkerSet[i];
        bContinueSearch = (*pfuncIterate)(&tMarker, ptUser);
        if (!bContinueSearch) {
            break;
        }
    }

    return bContinueSearch;
}
