// Original file: X/Client/CSceneCollision.cpp

#include "CGame.h"
#include "../Common/Game.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/DataStructures/DataStructuresExternal.h"



// Inference: local declarations use DWARF GamePerf.cpp:750 and 795; original header is not adopted.
POS GamePerfCtrAdd(LPCSTR szName, const BOOL fIsCounter, XMemoryAllocatorInterface * ptMemoryInterface);
void GamePerfCtrDelete(POS pos, XMemoryAllocatorInterface * ptMemoryInterface);








// Inference: canonical layout copied locally from DWARF CSceneCollision.h:22-24 (size 8).
struct CSceneCollisionGlobals
{
    POS posSceneOctreeCounter;
};





// Inference: allocation uses the observed 8-byte size, 4-byte alignment, null allocator, and clear flag.
void CScenesCollisionInitializeForGame()

{
    CGame & tCGame = CGameGet();
    CSceneCollisionGlobals *& ptCSceneCollisionGlobals = tCGame.ptCSceneCollisionGlobals;
    ptCSceneCollisionGlobals = static_cast<CSceneCollisionGlobals *>(SigmaMemoryNew(sizeof(CSceneCollisionGlobals), 4, NULL, TRUE));
    // Inference: original selects the client/server counter name on the client-game predicate.
    if (GameActiveGameCommonDataIsClient())
    {
        ptCSceneCollisionGlobals->posSceneOctreeCounter = GamePerfCtrAdd("Client Scene Octree", TRUE, NULL);
    }
    else
    {
        ptCSceneCollisionGlobals->posSceneOctreeCounter = GamePerfCtrAdd("Server Scene Octree", TRUE, NULL);
    }
}




void CScenesCollisionDisposeForGame()

{
    CGame & tCGame = CGameGet();
    CSceneCollisionGlobals *& ptCSceneCollisionGlobals = tCGame.ptCSceneCollisionGlobals;

    GamePerfCtrDelete(ptCSceneCollisionGlobals->posSceneOctreeCounter, NULL);
    // Inference: existing XFREE spells the original free-then-null sequence; original macro name is unavailable.
    XFREE(ptCSceneCollisionGlobals, NULL);
}














































































































































































































































































































































#include "StructureOctree.h"
#include "Definitions/AppearanceDefinition.h"
#include "Definitions/MaterialDefinition.h"
#include "../Common/TagMaps/AppearanceTags.h"
#include "../Common/TagMaps/Tags.h"
// Inference: local declarations follow DWARF AppearanceDefinition.cpp:4244/4754, GfxMaterial.cpp:366, TextureDefinition.cpp:1636.
SubObjectAppearance & AppearanceGetSubObjectAppearance(const AppearanceDefinition * ptDefinition, int32 nSubObjectLook, const int32 nSubObject, const GeoSetType eGeoSetType);
void BarycentricCoordsToTextureUV(const SubObject & tSubObject, const int32 nSegmentIndex, const int32 nIndexListIndex, const int32 nUVSetIndex, const float flUTriangle, const float flVTriangle, float & flU, float & flV);
MaterialTextureEntry * MaterialLookup(UberMaterial & tUberMaterial, ShaderTex eShaderTex);
void TextureDefinitionPixelLookup(const SNO snoTexture, const float flU, const float flV, const BOOL fLinearFilter, RGBAColor & rgbaColor);
// DWARF records only an inlined helper; standalone body is not checkable against the linked original.
#line 66
static int32 sGetPredominantLayerFromAlpha(
    RGBAColor & rgbaColor)
{
// Inference: DWARF const float at69 has no linkage/storage evidence; reciprocal spells observed float0.003921568859368563.
#line 69
    const float scflInv255 = 1.0f / 255.0f;
// Inference: base layer alpha is one; four subsequent entries are the observed normalized RGBA channels.
#line 70
    float flAlpha[5] = {
        1.0f,
        rgbaColor.r * scflInv255,
        rgbaColor.g * scflInv255,
        rgbaColor.b * scflInv255,
        rgbaColor.a * scflInv255
    };
// Inference: inverse-alpha array complements each layer; element zero is unused and inferred from the uniform construction.
#line 77
    float flInvAlpha[5] = {
        1.0f - flAlpha[0],
        1.0f - flAlpha[1],
        1.0f - flAlpha[2],
        1.0f - flAlpha[3],
        1.0f - flAlpha[4]
    };
#line 85
    float flLayerPct[5] = {
        flAlpha[0] * flInvAlpha[1] * flInvAlpha[2] * flInvAlpha[3] * flInvAlpha[4],
        flAlpha[1] * flInvAlpha[2] * flInvAlpha[3] * flInvAlpha[4],
        flAlpha[2] * flInvAlpha[3] * flInvAlpha[4],
        flAlpha[3] * flInvAlpha[4],
        flAlpha[4]
    };
// Inference: strict-greater selection starts with layer zero and walks the remaining four in original comparison order.
#line 93
    float flBest = flLayerPct[0];
    int32 nLayer = 0;
    for (int32 i = 1; i < 5; ++i)
    {
        if (flLayerPct[i] > flBest)
        {
            flBest = flLayerPct[i];
            nLayer = i;
        }
    }
    return nLayer;
}
#line 110
SNO CSceneGetSurfaceFromIntersection(
    const StructureIntersectData & tStructIntersect)
{
    ScopedAppearanceDefinitionLockR pdefAppearance(tStructIntersect.snoAppearance);
    ScopedAppearanceBufferContentRetrieval tAppearanceBufferContentRetrieval(pdefAppearance);

    const SubObject & tSubObject = pdefAppearance->tStructure.tGeoSets[0].ptSubObjects[tStructIntersect.nSubobject];

    SubObjectAppearance & tSOA = AppearanceGetSubObjectAppearance(pdefAppearance, 0, tStructIntersect.nSubobject, GEOSET_HIGHPOLY);
#line 119
    if (tSOA.snoMasterMaterial != SNO_NONE)
    {
        ScopedMaterialDefinitionLockR pdefMaterial(tSOA.snoMasterMaterial);
        SNO snoSurfaces[5];
// Inference: accept the master material when at least one of its five surface tags differs from SNO_NONE.
#line 123
        BOOL fValid = FALSE;
        for (int32 i = 0; i < 5; ++i)
        {
            snoSurfaces[i] = TagMapGetSNO(pdefMaterial->hTagMap, TAG_APPEARANCE_SURFACE_LAYER0 + i);
            if (snoSurfaces[i] != SNO_NONE)
            {
                fValid = TRUE;
            }
        }

        if (fValid)
        {
// Inference: default result is SNO_NONE when the alpha texture is absent.
#line 135
            SNO snoSurface = SNO_NONE;
            const MaterialTextureEntry * ptMaterialAlpha = MaterialLookup(tSOA.tCompositeUberMaterial, SHADERTEX_ALPHA);
#line 143
            if (ptMaterialAlpha && ptMaterialAlpha->tMatTex.snoTex != SNO_NONE)
            {
#line 145
                float flUTri;
#line 145
                float flVTri;
#line 145
                float flWTri;
#line 146
                tStructIntersect.wtriIntersect.BarycentricFromPoint(tStructIntersect.wpIntersect, flUTri, flVTri, flWTri);
#line 148
                float flU;
#line 148
                float flV;
#line 149
                BarycentricCoordsToTextureUV(tSubObject, tStructIntersect.nSegment, tStructIntersect.nIndexListIndex, 0, flUTri, flVTri, flU, flV);
#line 151
                RGBAColor rgbaColor;
                TextureDefinitionPixelLookup(ptMaterialAlpha->tMatTex.snoTex, flU, flV, FALSE, rgbaColor);

                int32 nLayer = sGetPredominantLayerFromAlpha(rgbaColor);
                snoSurface = snoSurfaces[nLayer];
            }
            return snoSurface;
        }

    }

    return tSubObject.snoSurface;
}
#line 518
#include "../Common/SceneCommon.h"
#include "../Common/Definitions/SceneDefinition.h"
#include "../Sigma/Math/Intersect.h"
// Inference: local declaration follows DWARF SceneCommon.cpp:1107.
void SceneGetPhysicalAABBInWorldSpace(const SceneCommonData & tSCD, AABB & aabb);
#line 352
BOOL CSceneSCDIntersectLineSeg(
    const SceneCommonData & tSCD,
    StructureIntersectData & tStructIntersect)
{
    StructureIntersectData tStructIntersectInSceneSpace(tStructIntersect);


    float wdT;
    AABB aabbBounds;
    SceneGetPhysicalAABBInWorldSpace(tSCD, aabbBounds);
    if (!Intersect::LineSeg3DAndAABB(tStructIntersectInSceneSpace.wline, aabbBounds, wdT))
    {
        return FALSE;
    }

    tStructIntersectInSceneSpace.wline.wr.wpOrigin = tSCD.transformInvWorld * tStructIntersect.wline.wr.wpOrigin;
    tStructIntersectInSceneSpace.wline.wr.wvDirection = tSCD.transformInvWorld.Rotate(tStructIntersect.wline.wr.wvDirection);
    tStructIntersectInSceneSpace.idSScene = tSCD.idSScene;

    ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);
    SNO snoAppearance = pdefScene->GetAppearance();

    ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
    const Structure & tStructure = pdefAppearance->tStructure;
    // Inference: the following result-store group ends with snoAppearance; paired store at387 has no separate row for it.
    BOOL fIntersection = StructureOctreeIntersectLineSeg(tStructure.ptPostprocessed->tStructureOctree, tStructIntersectInSceneSpace);

    if (fIntersection)
    {
        tStructIntersect.wpIntersect = tSCD.transformWorld * tStructIntersectInSceneSpace.wpIntersect;
        tStructIntersect.wtriIntersect.wp0 = tSCD.transformWorld * tStructIntersectInSceneSpace.wtriIntersect.wp0;
        tStructIntersect.wtriIntersect.wp1 = tSCD.transformWorld * tStructIntersectInSceneSpace.wtriIntersect.wp1;
        tStructIntersect.wtriIntersect.wp2 = tSCD.transformWorld * tStructIntersectInSceneSpace.wtriIntersect.wp2;
        tStructIntersect.wvNormal = tSCD.transformWorld.Rotate(tStructIntersectInSceneSpace.wvNormal);
        tStructIntersect.tPrimitive = tStructIntersectInSceneSpace.tPrimitive;
        tStructIntersect.nSubobject = tStructIntersectInSceneSpace.nSubobject;
        tStructIntersect.nSegment = tStructIntersectInSceneSpace.nSegment;
        tStructIntersect.nIndexListIndex = tStructIntersectInSceneSpace.nIndexListIndex;
        tStructIntersect.wdIntersect = tStructIntersectInSceneSpace.wdIntersect;
        tStructIntersect.snoAppearance = snoAppearance;
    }
    // Inference: return the original intersection result; cleanup shares the closing-brace row.
    return fIntersection;
}
#line 570
#include <float.h>
#include "RActors/RActors.h"
#include "../Common/World.h"
#include "../Common/WorldPlace.h"
#include "../Sigma/MemoryPools.h"
// Inference: local declarations follow DWARF SceneCommon.cpp:3161, RActorCollision.cpp:1123, GamePerf.cpp:604.
TEnvAppDataPool * SceneCommonGetEnvAppDataPool();
void RActorsIntersectSegment(const LineSeg3D & tLineSeg, const RActorID idIgnore, SphereNodeMemoryPool * ptSphereNodePool, RActorIDListPooled & listActors, BOOL fEnvAppOnly);
void GamePerfCtrIncrement(const POS pos, const int32 nAmount);
#line 168
static BOOL sIntersectLineSegWithEnvApps(
    StructureIntersectData & tStructIntersect)
{
    WorldCommonData *ptWCD = WorldTryGetCommonData(tStructIntersect.idSWorld);
    if (!ptWCD)
    {
        return FALSE;
    }

    // Inference: placeStart/placeEnd only carry wp here (DWARF 179/180); end = origin + direction * length (operator* / operator+ inlined at 184).

    WorldPlace placeStart;
    WorldPlace placeEnd;

    placeStart.wp = tStructIntersect.wline.wr.wpOrigin;

    placeEnd.wp = tStructIntersect.wline.wr.wpOrigin + tStructIntersect.wline.wr.wvDirection * tStructIntersect.wline.wdLength;

    // Inference: ternary from columns 187:28/45/100 (pool load, then SceneCommonGetEnvAppDataPool).
    TEnvAppDataPool * ptPool = tStructIntersect.ptEnvAppDataPool ? tStructIntersect.ptEnvAppDataPool : SceneCommonGetEnvAppDataPool();
    if (!ptPool)
    {
        return FALSE;
    }

    TEnvAppDataList listEnvApps;
    // Inference: the list must use ptPool (AddTail/RemoveAll go through it); SetPool leaves no row of its own.
    listEnvApps.SetPool(ptPool);
    LineSeg3D wline;
    wline.Set(placeStart.wp, placeEnd.wp);
    {

        // Inference: fLightMapping (no location) is the INTERSECTFLAGS_LIGHT_MAPPING_BIT test folded into tst #0x20 at 207 and tbz #5 at 210.
        BOOL fLightMapping = (tStructIntersect.dwIntersectFlags & (1 << INTERSECTFLAGS_LIGHT_MAPPING_BIT));

        RActorIDListPooled listRActors;
        listRActors.SetPool(tStructIntersect.ptDataIDPool ? tStructIntersect.ptDataIDPool : MemoryPoolsGetDATAIDListPool());

        SphereNodeMemoryPool * ptSphereNodePool = tStructIntersect.ptSphereNodePool ? tStructIntersect.ptSphereNodePool : MemoryPoolsGetSphereNodeListPool();
        RActorsIntersectSegment(wline, RACTORID_INVALID, ptSphereNodePool, listRActors, !fLightMapping);

        POS pos = listRActors.GetHeadPosition();
        while (pos)
        {
            RActorID idRActor = (RActorID)listRActors.GetNext(pos);
            RActor * ptRActor = RActorTryToGet(idRActor);
            // Inference: null actor skipped (merges with RActorTryToGet's test); third && operand !fLightMapping from columns 219:64/220:65, hoisted to 210.
            if (!ptRActor)
            {
                continue;
            }
            if (ptRActor->tLocation.eActorLocation == AL_WORLD &&
                ptRActor->tMarkerOwner.snoMarkerSet != SNO_NONE &&
                !fLightMapping)
            {
                EnvAppData tAppData;
                tAppData.snoAppearance = ptRActor->snoAppearance;
                tAppData.snoPhysMesh = ptRActor->snoPhysMesh;
                tAppData.idSScene = ptRActor->idSScene;
                tAppData.transform = ptRActor->tLocation.tWorldLocation.transform;
                tAppData.flScale = ptRActor->tLocation.tWorldLocation.flScale;
                // Inference: physical-mesh test from cmn #-1 of the 225 load at 230; bodyId is never assigned (not stored in the node).
                if (tAppData.snoPhysMesh != SNO_NONE)
                {



                    listEnvApps.AddTail_Tracked(tAppData);
                }
            }
        }





    }
    // Inference: bIntersect is constant 0, then 1 after the hit block (DWARF location list); return value unused by callers.
    BOOL bIntersect = FALSE;
    POS pos = listEnvApps.GetHeadPosition();
    while (pos)
    {
        EnvAppData & tAppData = listEnvApps.GetNext(pos);

        ScopedAppearanceDefinitionLockR pdefAppearance(tAppData.snoAppearance);
        PRSTransform tTransform(tAppData.transform.q, tAppData.transform.wp, tAppData.flScale);
        PRSTransform tInvTransform = tTransform.Inverse();
        Vector3D vLocalStart = tInvTransform * placeStart.wp;
        Vector3D vLocalEnd = tInvTransform * placeEnd.wp;

        StructureIntersectData tStructIntersectActorSpace(tStructIntersect);
        tStructIntersectActorSpace.idSWorld = SWORLDID_INVALID;
        tStructIntersectActorSpace.idSScene = SSCENEID_INVALID;
        tStructIntersectActorSpace.wline.Set(vLocalStart, vLocalEnd);

        const Structure & tStructure = pdefAppearance->tStructure;
        BOOL fIntersection = StructureOctreeIntersectLineSeg(
            tStructure.ptPostprocessed->tStructureOctree, tStructIntersectActorSpace);
        // Inference: the hit test compares against the world distance scaled into actor space (fmul by 1/flScale, 268:101).
        // Inference: lines 270-281 are tab-indented in the original (columns 271:30, 273:33, 280:30, 281:35).
        if (fIntersection && tStructIntersectActorSpace.wdIntersect < (tStructIntersect.wdIntersect * tInvTransform.flScale))
        {
			tStructIntersect = tStructIntersectActorSpace;
			tStructIntersect.idACDHit = ACDID_INVALID;
			tStructIntersect.wpIntersect = tTransform * tStructIntersect.wpIntersect;
			tStructIntersect.wdIntersect = tStructIntersectActorSpace.wdIntersect * tAppData.flScale;
			tStructIntersect.wtriIntersect.wp0 = tTransform * tStructIntersect.wtriIntersect.wp0;
			tStructIntersect.wtriIntersect.wp1 = tTransform * tStructIntersect.wtriIntersect.wp1;
			tStructIntersect.wtriIntersect.wp2 = tTransform * tStructIntersect.wtriIntersect.wp2;
			PRTransform transformNoScale(tTransform.q, tTransform.wp);
			// Inference: transformNoScale has no ctor DIE; it reuses tTransform's q/wp registers, so it is built from tTransform.
			tStructIntersect.wvNormal = transformNoScale * tStructIntersectActorSpace.wvNormal;
			tStructIntersect.idSScene = tAppData.idSScene;
			tStructIntersect.snoAppearance = tAppData.snoAppearance;
			bIntersect = TRUE;
        }
    }

    return bIntersect;
}



BOOL CScenesIntersectLineSeg(
	StructureIntersectData & tStructIntersect,
	CSceneDataArrayIterateMode eIterateMode)
{

	// Inference: SceneCommonDataArrayIterate result drives the loop; scenes of other worlds are skipped (307), best hit kept by strict < (315).
	CGame & tCGame = CGameGet();
	if (tCGame.ptCSceneCollisionGlobals)
	{
		GamePerfCtrIncrement(tCGame.ptCSceneCollisionGlobals->posSceneOctreeCounter, 1);
	}

	SceneCommonData *ptSCD = NULL;
	SceneCommonDataIterator tIterator;
	while (SceneCommonDataArrayIterate(tIterator, ptSCD, eIterateMode))
	{
		if (ptSCD->idSWorld != tStructIntersect.idSWorld)
		{
			continue;
		}

		StructureIntersectData tStructIntersectCurrent(tStructIntersect);
		tStructIntersectCurrent.idSScene = ptSCD->idSScene;
		BOOL fIntersection = CSceneSCDIntersectLineSeg(*ptSCD, tStructIntersectCurrent);
		if (fIntersection && tStructIntersectCurrent.wdIntersect < tStructIntersect.wdIntersect)
		{
			tStructIntersect = tStructIntersectCurrent;
		}
	}

    sIntersectLineSegWithEnvApps(tStructIntersect);
	// Inference: if/return TRUE/FALSE from column 323:6; CSceneIntersectLineSeg returns the comparison directly (341:26).
	if (tStructIntersect.wdIntersect != FLT_MAX)
	{
		return TRUE;
	}
	return FALSE;

}



BOOL CSceneIntersectLineSeg(
	SSceneID idSScene,
	StructureIntersectData & tStructIntersect)
{
	const SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
	CSceneSCDIntersectLineSeg(tSCD, tStructIntersect);
    sIntersectLineSegWithEnvApps(tStructIntersect);

	return tStructIntersect.wdIntersect != FLT_MAX;






}
