// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/MarkerSetDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../GameBalance.h"
#include "../MarkerSet.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/SigmaTypes.h"

struct FileReference;

enum {
    MARKER_SET_BOUNDS_VALID_BIT = 0,
    MARKER_SET_WORLD_MARKER_BIT = 1,
};

struct MarkerSetDefinition {  // size 0xc8
    RequiredDefinitionHeader tHeader;  // 0x00
    SerializeData serMarkers;  // 0x0c
    MarkerSet tMarkerSet;  // 0x18
    SerializeData serNoSpawns;  // 0x50
    NoSpawnSet tNoSpawnSet;  // 0x58
    AABB aabbBounds;  // 0x90
    uint32 dwMarkerSetFlags;  // 0xa8
    GBID nLabel;  // 0xac
    int32 nSpecialIndexCount;  // 0xb0
    SerializeData serSpecialIndexList;  // 0xb4
    XPtr64<unsigned short> pwSpecialIndexList;  // 0xc0
};
static_assert(sizeof(MarkerSetDefinition) == 0xc8, "MarkerSetDefinition");

class MarkerSetGroupDef : public GroupDefinition {  // size 0xa0
public:
    MarkerSetGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 53; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 56
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 58
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
};
static_assert(sizeof(MarkerSetGroupDef) == 0xa0, "MarkerSetGroupDef");

extern MarkerSetGroupDef * gptGroupDefMarkerSet;  // MarkerSetDefinition.cpp:70

typedef ScopedDefinitionLockR<MarkerSetDefinition> ScopedMarkerSetDefinitionLockR;

// Lines 104-105. Written from the copy inlined into Minimap::ResolveMouseOverNameForActor
// (LockR with FALSE, UnlockR through a local copy of the pointer), as for ActorDefinition.h.
template <> inline const MarkerSetDefinition * DefinitionLockR<MarkerSetDefinition>(SNO sno) {  // line 104
    return (const MarkerSetDefinition *)gptGroupDefMarkerSet->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<MarkerSetDefinition>(const MarkerSetDefinition * pdef) {  // line 105
    void * lpDef = (void *)pdef;
    gptGroupDefMarkerSet->UnlockR(lpDef);
}
