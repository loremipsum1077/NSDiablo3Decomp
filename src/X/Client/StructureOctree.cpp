// Original file: X/Client/StructureOctree.cpp

#include "StructureOctree.h"

#include "CGame.h"
#include "SubObject.h"
#include "Definitions/AppearanceDefinition.h"
#include "Definitions/SurfaceDefinition.h"
#include "../Common/TagMaps/AppearanceTags.h"
#include "../Common/TagMaps/Tags.h"
#include "../Sigma/DataStructures/Octree.h"
#include "../Sigma/Math/Intersect.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Ray.h"
#include "../Sigma/Math/Triangle.h"

#include <float.h>

// DebugFadeMode (X/Common/Game.h:70) comes from Game.h.







struct LineIntersectData {  // size 0x60
    LineSeg3D wlineIntersect;  // 0x00
    WorldDistance wdIntersect;  // 0x1c
    Triangle3D wtriIntersect;  // 0x20
    OctreePrimitive tPrimitive;  // 0x44
    uint32 dwIntersectFlags;  // 0x4c
    const Octree * ptOctree;  // 0x50
    const AppearanceDefinition * ptAppearance;  // 0x58
};
static_assert(sizeof(LineIntersectData) == 0x60, "LineIntersectData");

static const int USERDATA_INDEX0_BITS = 16;  // line 62

static BOOL sOctreeLeafIntersectLine(const OctreeLeaf * const ptLeaf, void * lpData, BOOL & fEndWalk);

// line 77
inline void StructureOctreeGetIndexFromUserData(uint32 uUserData, int32 & nIndex0, int32 & nIndex1) {
    nIndex0 = uUserData & ((1 << USERDATA_INDEX0_BITS) - 1);
    nIndex1 = uUserData >> USERDATA_INDEX0_BITS;
}

// line 267
static BOOL sSurfaceIsLiquid(const SNO snoSurface) {
    if (snoSurface == SNO_NONE)
        return FALSE;

    ScopedSurfaceDefinitionLockR pdefSurface(snoSurface);
    switch (pdefSurface->eType) {
    case SURFACETYPE_LAVA:
    case SURFACETYPE_WATER:
    case SURFACETYPE_SLIME:
        return TRUE;
    default:
        return FALSE;
    }
}

// line 290
static BOOL sStructureOctreeIgnorePrimitive(const AppearanceDefinition * ptAppearance, const GeoSetType eGeoSetType,
                                            int32 nSubObjectIndex, uint32 dwIntersectFlags) {
    const GeoSet & tGeoSet = ptAppearance->tStructure.tGeoSets[eGeoSetType];
    const SubObject * ptSubObject = &((SubObject *)tGeoSet.ptSubObjects)[nSubObjectIndex];
    const SubObjectAppearance & tSOA = *(SubObjectAppearance *)ptSubObject->ptPostprocessed->ptMaterial->ptSOAs;

    if (dwIntersectFlags & (1 << INTERSECTFLAGS_IGNORE_NOT_WALKABLE)) {
        if ((ptSubObject->dwFlags & (1 << SUBOBJECT_NOT_WALKABLE_BIT)) ||
            TagMapGetInt(tSOA.hTagMapMasterMaterial, TAG_APPEARANCE_FORCE_NONWALKABLE))
            return TRUE;
    }

    if (dwIntersectFlags & (1 << INTERSECTFLAGS_IGNORE_NOT_RENDERED)) {
        if (!(tSOA.dwFlags & (1 << SUBOBJECTAPPEARANCE_RENDER_BIT)))
            return TRUE;
    }

    if (dwIntersectFlags & (1 << INTERSECTFLAGS_IGNORE_NON_COLLIDABLE_BIT)) {
        if (ptSubObject->dwFlags & (1 << SUBOBJECT_NOT_COLLIDABLE_BIT))
            return TRUE;
    }

    if (dwIntersectFlags & (1 << INTERSECTFLAGS_IGNORE_NON_LIQUID_SURFACES_BIT)) {
        if (!sSurfaceIsLiquid(ptSubObject->snoSurface))
            return TRUE;
    }

    if (dwIntersectFlags & (1 << INTERSECTFLAGS_IGNORE_FADE_SURFACES_BIT)) {
        if (ptSubObject->nFadeGroup >= 0)
            return TRUE;
    }

    return ptSubObject->nFadeGroup >= 0 && sgtAppGlobals.eDebugFadeMode == DFM_FADE_ALL;
}

// line 350
BOOL StructureOctreeIntersectLineSeg(const StructureOctree & tStructOctree, StructureIntersectData & tStructIntersect) {
    if (!tStructOctree.ptOctree || tStructOctree.ptOctree->nNodeCount <= 0)
        return FALSE;

    if (tStructOctree.ptOctree->nPrimitiveCount == 0)
        return FALSE;

    LineIntersectData tIntersectData;
    BOOL fIntersection = FALSE;

    tIntersectData.wlineIntersect = tStructIntersect.wline;
    tIntersectData.wdIntersect = FLT_MAX;
    tIntersectData.ptOctree = tStructOctree.ptOctree;
    tIntersectData.ptAppearance = tStructOctree.ptAppearance;
    tIntersectData.dwIntersectFlags = tStructIntersect.dwIntersectFlags;

    tStructOctree.ptOctree->WalkLinear(tStructIntersect.wline, &tIntersectData, sOctreeLeafIntersectLine);

    if (tIntersectData.wdIntersect != FLT_MAX) {
        int32 nIndex0, nIndex1;
        tIntersectData.wlineIntersect.wr.GetEndpoint(tIntersectData.wdIntersect, tStructIntersect.wpIntersect);
        tIntersectData.wtriIntersect.GetFaceNormal(tStructIntersect.wvNormal);
        tStructIntersect.tPrimitive = tIntersectData.tPrimitive;
        tStructIntersect.wtriIntersect = tIntersectData.wtriIntersect;
        StructureOctreeGetIndexFromUserData(tIntersectData.tPrimitive.dwUserData, nIndex0, nIndex1);
        tStructIntersect.nSubobject = nIndex0;
        tStructIntersect.nSegment = nIndex1;
        tStructIntersect.nIndexListIndex = tIntersectData.tPrimitive.dwIndexListIndex;
        tStructIntersect.wdIntersect = tIntersectData.wdIntersect;
        fIntersection = TRUE;
    }

    return fIntersection;
}

// line 414
static BOOL sOctreeLeafIntersectLine(const OctreeLeaf * const ptLeaf, void * lpData, BOOL & fEndWalk) {
    LineIntersectData * ptIntersectData = (LineIntersectData *)lpData;
    BOOL fContinueWalk = TRUE;
    int32 nLastSubObjectIndex = -1;
    BOOL bIgnore = FALSE;

    if (ptLeaf->nPrimitiveCount == 0)
        return fContinueWalk;

    ScopedAppearanceBufferContentRetrieval tAppearanceBufferContentRetrieval(ptIntersectData->ptAppearance);
    const Structure & tStructure = ptIntersectData->ptAppearance->tStructure;
    OctreePrimitive * ptPrimitiveList;
    const int32 nPrimitiveCount = ptLeaf->nPrimitiveCount;
    const int32 nPrimitiveIndex = ptLeaf->nPrimitiveIndex;
    BOOL fCull;

    ptPrimitiveList = &((OctreePrimitive *)ptIntersectData->ptOctree->ptPrimitives)[nPrimitiveIndex];
    fCull = !(ptIntersectData->dwIntersectFlags & (1 << INTERSECTFLAGS_NO_CULL));

    for (int32 i = 0; i < ptLeaf->nPrimitiveCount; i++) {
        int32 nIndex0, nIndex1;
        StructureOctreeGetIndexFromUserData(ptPrimitiveList[i].dwUserData, nIndex0, nIndex1);

        Triangle3D wtri;
        {
            const GeoSet & tGeoSet = tStructure.tGeoSets[GEOSET_HIGHPOLY];
            const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
            int32 nSubObjectIndex = nIndex0;

            if (nSubObjectIndex != nLastSubObjectIndex) {
                bIgnore = sStructureOctreeIgnorePrimitive(ptIntersectData->ptAppearance, eGeoSetType, nSubObjectIndex,
                                                          ptIntersectData->dwIntersectFlags);
                nLastSubObjectIndex = nSubObjectIndex;
            }

            if (bIgnore)
                continue;

            int32 nIndexListIndex = ptPrimitiveList[i].dwIndexListIndex;
            SubObject & tSubObject = ((SubObject *)tGeoSet.ptSubObjects)[nSubObjectIndex];
            uint16 wIndex0;
            uint16 wIndex1;
            uint16 wIndex2;
            int32 nSegmentIndex = nIndex1;

            SubObjectGetSegmentTriangle(tSubObject, nSegmentIndex, nIndexListIndex, wIndex0, wIndex1, wIndex2);

            if (fCull) {
                if (!(tSubObject.dwFlags & (1 << SUBOBJECT_UNSTRIPIFIED_BIT)) && (nIndexListIndex & 1))
                    Math::Swap(wIndex1, wIndex2);
            }

            SubObjectGetSegmentVertexPos(tSubObject, nSegmentIndex, wIndex0, wtri.wp0);
            SubObjectGetSegmentVertexPos(tSubObject, nSegmentIndex, wIndex1, wtri.wp1);
            SubObjectGetSegmentVertexPos(tSubObject, nSegmentIndex, wIndex2, wtri.wp2);
        }

        BOOL fRes;
        WorldDistance wdT;
        Vector3D wpIntersect;

        if (fCull)
            fRes = Intersect::LineSeg3DAndTriangle3DCull(ptIntersectData->wlineIntersect, wtri, wdT, &wpIntersect);
        else
            fRes = Intersect::LineSeg3DAndTriangle3D(ptIntersectData->wlineIntersect, wtri, wdT, &wpIntersect);

        if (fRes) {
            if (wdT < ptIntersectData->wdIntersect) {
                fContinueWalk = FALSE;
                ptIntersectData->wdIntersect = wdT;
                ptIntersectData->wtriIntersect = wtri;
                ptIntersectData->tPrimitive = ptPrimitiveList[i];
            }
        }
    }

    return fContinueWalk;
}
