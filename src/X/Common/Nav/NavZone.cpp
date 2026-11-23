// Original file: X/Common/Nav/NavZone.cpp

#include "NavZone.h"
#include "NavMeshStitch.h"
#include "NavZoneIntersect.h"
#include "NavZoneIntersectPrim.h"
#include "../Definitions/SceneDefinition.h"
#include "../Game.h"
#include "../SceneCommon.h"
#include "../ThreadGlobals.h"
#include "../World.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"

const NavZoneInstance * NavZoneInstance::sptFirstLocked;
BOOL NavZoneInstance::sfKeepLocks;









void NavZoneInstance::KeepLocks() {
    sfKeepLocks = TRUE;
}



void NavZoneInstance::ReleaseLocks() {
    const NavZoneInstance * ptLocked = sptFirstLocked;
    while (ptLocked) {
        ptLocked->UnlockZoneDef();
        const NavZoneInstance * ptNextLocked = ptLocked->ptNextLocked;
        ptLocked->ptNextLocked = NULL;
        ptLocked = ptNextLocked;
    }
    sptFirstLocked = NULL;
    sfKeepLocks = FALSE;
}





void NavZoneInstance::Initialize() {
}


void NavZoneInstance::Dispose() {
    if (nLockCount == 1 && sfKeepLocks) {
        const NavZoneInstance * ptLocked = sptFirstLocked;
        const NavZoneInstance * ptPrevious = NULL;
        while (ptLocked) {
            if (ptLocked == this) {
                if (ptPrevious)
                    ptPrevious->ptNextLocked = ptNextLocked;
                else
                    sptFirstLocked = ptNextLocked;
                break;
            }
            ptPrevious = ptLocked;
            ptLocked = ptLocked->ptNextLocked;
        }
        UnlockZoneDef();
        ptNextLocked = NULL;
    }

    while (uChildInstanceCount)
        arChildInst[0]->SetParentInstance(NULL);

    SetParentInstance(NULL);
}















void NavZoneInstance::LockZoneDef() const {
    if (nLockCount == 0) {
        // The server worker parks its locks on a chain that ReleaseLocks() drops.
        if (sfKeepLocks && ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_SERVERWORKER) {
            ptNextLocked = sptFirstLocked;
            sptFirstLocked = this;
            nLockCount++;
        }
        ptSceneDef = (const SceneDefinition *)gptGroupDefScene->LockR(snoScene, FALSE);
        ptNavZoneDef = &ptSceneDef->tNavZoneDef;
    }
    nLockCount++;
}







void NavZoneInstance::UnlockZoneDef() const {
    nLockCount--;
    if (nLockCount == 0) {
        ptNavZoneDef = NULL;
        gptGroupDefScene->UnlockR((void *&)ptSceneDef);
        ptSceneDef = NULL;
    }
}





inline void NavZoneInstance::GetBorderAABB(const NavCell & tNavCell, uint32 uBorder, Vector2D & wpMin, Vector2D & wpMax) const {
    switch (uBorder) {
    case NCBD_MIN_X:
        wpMin.Set(tNavCell.tMin.x - 1.25f, tNavCell.tMin.y + 1.25f);
        wpMax.Set(tNavCell.tMin.x - 1.25f, tNavCell.tMax.y - 1.25f);
        break;
    case NCBD_MAX_X:
        wpMin.Set(tNavCell.tMax.x + 1.25f, tNavCell.tMin.y + 1.25f);
        wpMax.Set(tNavCell.tMax.x + 1.25f, tNavCell.tMax.y - 1.25f);
        break;
    case NCBD_MIN_Y:
        wpMin.Set(tNavCell.tMin.x + 1.25f, tNavCell.tMin.y - 1.25f);
        wpMax.Set(tNavCell.tMax.x - 1.25f, tNavCell.tMin.y - 1.25f);
        break;
    case NCBD_MAX_Y:
        wpMin.Set(tNavCell.tMin.x + 1.25f, tNavCell.tMax.y + 1.25f);
        wpMax.Set(tNavCell.tMax.x - 1.25f, tNavCell.tMax.y + 1.25f);
        break;
    }

    wpMin += wpWorldMin;
    wpMax += wpWorldMin;
}








void NavZoneInstance::InitBorderDataSceneInst(const NavCellBorderDataInst & tBorderDataInst, NavCellBorderDataSceneInst & tBorderDataSceneInst) const {
    if (tBorderDataSceneInst.uFlags & (1 << NCBDSI_FLAG_INITIALIZED))
        return;

    SceneCommonData * ptSCD = SceneCommonDataTryToGet(tBorderDataSceneInst.idSScene);
    if (ptSCD == NULL || ptSCD->ptNavZoneInst == NULL)
        return;

    tBorderDataSceneInst.uFlags |= (1 << NCBDSI_FLAG_INITIALIZED);
    tBorderDataSceneInst.uNeighborOffset = 0;
    tBorderDataSceneInst.uNeighborCount = 0;

    const NavZoneDefinition * ptNavZoneDefLoc = GetZoneDef();
    int32 nIndex = &tBorderDataInst - arBorderDataInst;
    const NavCellBorderData & tBorderData = ptNavZoneDefLoc->arBorderData[nIndex];
    const NavCell & tNavCell = ptNavZoneDefLoc->arNavCells[tBorderData.uCellIndex];

    Vector2D wpBoxMin;
    Vector2D wpBoxMax;
    GetBorderAABB(tNavCell, tBorderDataSceneInst.uBorder, wpBoxMin, wpBoxMax);

    const NavZoneInstance * ptNavZoneInstBorder = ptSCD->ptNavZoneInst;
    ScopedNavZoneDefLock tLockZone(ptNavZoneInstBorder);
    const NavZoneDefinition * ptBorderZoneDef = ptNavZoneInstBorder->GetZoneDef();
    if (ptBorderZoneDef->uNavCellCount == 0)
        return;

    uint32 uBitArrayCount = NavZoneGetBitArrayCount(ptBorderZoneDef->uNavCellCount);
    uint32 uBitArraySize = uBitArrayCount * sizeof(uint32);
    uint32 * arCellIsect = (uint32 *)alloca(uBitArraySize);
    SigmaMemoryClear(arCellIsect, uBitArraySize);

    wpBoxMin -= ptNavZoneInstBorder->wpWorldMin;
    wpBoxMax -= ptNavZoneInstBorder->wpWorldMin;

    uint32 uCellCount;
    BOOL bHitChild;
    NavZoneCellQueryAABB(ptNavZoneInstBorder, wpBoxMin, wpBoxMax, 0, 0, 0, arCellIsect, uCellCount, bHitChild);
    if (uCellCount == 0)
        return;

    tBorderDataSceneInst.uNeighborOffset = uBorderLookupCount;
    if (uBorderLookupCount + uCellCount >= uMaxBorderLookups) {
        uMaxBorderLookups = Math::Max(uBorderLookupCount + uCellCount, uMaxBorderLookups * 2);
        arBorderLookups = (NavCellLookup *)SigmaMemoryResize(arBorderLookups, NULL, uMaxBorderLookups * sizeof(NavCellLookup), 4);
    }

    // Walk the set bits; stop once every cell the query reported has been seen.
    uint32 uIndexOffset = 0;
    uint32 uCount = 0;
    NavCellLookup * ptLookup = &arBorderLookups[tBorderDataSceneInst.uNeighborOffset];
    for (uint i = 0; i < uBitArrayCount; i++) {
        uint32 uBlocks = arCellIsect[i];
        uint32 uMask = 1;
        uint32 uIndex = uIndexOffset;
        while (uBlocks) {
            if (uBlocks & uMask) {
                const NavCell & tNavCellLoc = ptBorderZoneDef->arNavCells[uIndex];
                if (AABB2DAndAABB2D((const Vector2D &)tNavCellLoc.tMin, (const Vector2D &)tNavCellLoc.tMax, wpBoxMin, wpBoxMax)) {
                    ptLookup->uCellIndex = uIndex;
                    ptLookup->uCellFlags = tNavCellLoc.uFlags;
                    ptLookup++;
                    tBorderDataSceneInst.uNeighborCount++;
                }
                uCount++;
                uBlocks &= ~uMask;
            }
            uMask <<= 1;
            uIndex++;
        }
        if (uCount == uCellCount)
            break;
        uIndexOffset += 32;
    }

    uBorderLookupCount += tBorderDataSceneInst.uNeighborCount;
}















































void NavZoneInstance::InitBorderDataInst(int nIndex) const {
    const int nMaxSceneIdCount = 64;
    SSceneID arSceneIds[nMaxSceneIdCount];

    NavCellBorderDataInst & tBorderDataInst = arBorderDataInst[nIndex];
    const NavCellBorderData & tBorderData = GetZoneDef()->arBorderData[nIndex];
    const NavCell & tNavCell = GetZoneDef()->arNavCells[tBorderData.uCellIndex];

    tBorderDataInst.uSceneCount = 0;
    tBorderDataInst.uSceneOffset = uBorderSceneCount;

    // On the client, wait until every child scene of the neighbouring scenes exists.
    if (GameActiveGameCommonDataIsClient()) {
        for (uint32 i = 0; i < 4; i++) {
            if (!(tBorderData.uBorderFlags & (1 << i)))
                continue;

            Vector2D wpBoxMin;
            Vector2D wpBoxMax;
            GetBorderAABB(tNavCell, i, wpBoxMin, wpBoxMax);
            uint16 uCount = NavMeshStitchScenesInAABB(idSWorld, wpBoxMin, wpBoxMax, arSceneIds, nMaxSceneIdCount);

            for (int j = 0; j < uCount; j++) {
                SSceneID idSSceneLoc = arSceneIds[j];
                SceneCommonData & tSCD = SceneCommonDataGet(idSSceneLoc);
                POS pos = tSCD.tChildSceneIDs.GetHeadPosition();
                while (pos) {
                    SSceneID idSSceneChild = tSCD.tChildSceneIDs.GetNext(pos);
                    SceneCommonData * ptSCDChild = SceneCommonDataTryToGet(idSSceneChild);
                    if (ptSCDChild == NULL)
                        return;
                }
            }
        }
    }

    for (uint32 i = 0; i < 4; i++) {
        if (!(tBorderData.uBorderFlags & (1 << i)))
            continue;

        NavCellBorderDataSceneInst * ptCellBorderScene;
        Vector2D wpBoxMin;
        Vector2D wpBoxMax;
        GetBorderAABB(tNavCell, i, wpBoxMin, wpBoxMax);
        uint16 uCount = NavMeshStitchScenesInAABB(idSWorld, wpBoxMin, wpBoxMax, arSceneIds, nMaxSceneIdCount);

        for (int j = 0; j < uCount; j++) {
            SSceneID idSSceneLoc = arSceneIds[j];
            SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSSceneLoc);
            if (ptSCD) {
                POS pos = ptSCD->tChildSceneIDs.GetHeadPosition();
                while (pos) {
                    SSceneID idSSceneChild = ptSCD->tChildSceneIDs.GetNext(pos);
                    SceneCommonData & tSCDChild = SceneCommonDataGet(idSSceneChild);
                    Vector2D wpMin(tSCDChild.transformWorld.wp.x, tSCDChild.transformWorld.wp.y);
                    Vector2D wpMax = wpMin;
                    wpMax += Vector2D(tSCDChild.tSceneSpec.tCachedValues.nNavmeshSizeX * 2.5f, tSCDChild.tSceneSpec.tCachedValues.nNavmeshSizeY * 2.5f);
                    if (AABB2DAndAABB2D(wpBoxMin, wpBoxMax, wpMin, wpMax))
                        arSceneIds[uCount++] = idSSceneChild;
                }

                // Never border ourselves.
                if (arSceneIds[j] == idSScene) {
                    arSceneIds[j] = arSceneIds[uCount - 1];
                    uCount--;
                    j--;
                }
            }
        }

        if (GameActiveGameCommonDataIsClient() && uCount == 0)
            return;

        if (uBorderSceneCount + uCount >= uMaxBorderScenes) {
            uMaxBorderScenes = Math::Max(uMaxBorderScenes * 2, uBorderSceneCount + uCount);
            arBorderScenes = (NavCellBorderDataSceneInst *)SigmaMemoryResize(arBorderScenes, NULL, uMaxBorderScenes * sizeof(NavCellBorderDataSceneInst), 4);
        }

        ptCellBorderScene = &arBorderScenes[uBorderSceneCount];
        for (int j = 0; j < uCount; j++) {
            ptCellBorderScene->idSScene = arSceneIds[j];
            ptCellBorderScene->uBorder = i;
            ptCellBorderScene->uFlags = 0;
            ptCellBorderScene->uNeighborCount = 0;
            ptCellBorderScene->uNeighborOffset = 0;
            ptCellBorderScene++;
        }

        tBorderDataInst.uSceneCount += uCount;
        uBorderSceneCount += uCount;
    }

    tBorderDataInst.uFlags |= (1 << NCBDI_FLAG_INITIALIZED);
}



























































uint32 NavZoneInstance::GetChildInstances(const Vector2D & wpMin, const Vector2D & wpMax, NavZoneInstance ** _arChildInst) const {
    uint32 uChildCount = 0;
    for (uint32 i = 0; i < uChildInstanceCount; i++) {
        NavZoneInstance * ptChildInst = arChildInst[i];
        if (AABB2DAndAABB2D(ptChildInst->wpWorldMin, ptChildInst->wpWorldMax, wpMin, wpMax))
            _arChildInst[uChildCount++] = ptChildInst;
    }
    return uChildCount;
}













inline const NavZoneInstance * NavZoneInstance::GetChildInstance(const Vector2D & wp) const {
    const NavZoneInstance * ptChildInst = NULL;
    for (uint32 i = 0; i < uChildInstanceCount; i++) {
        NavZoneInstance * ptChildInstCur = arChildInst[i];
        if (Point2DInsideAABB2D(wp, ptChildInstCur->wpWorldMin, ptChildInstCur->wpWorldMax)) {
            ptChildInst = ptChildInstCur;
            break;
        }
    }
    return ptChildInst;
}









void NavZoneInstance::SetParentInstance(NavZoneInstance * ptNavZoneInst) {
    if (ptParentInst != ptNavZoneInst) {
        if (ptParentInst) {
            for (uint32 i = 0; i < ptParentInst->uChildInstanceCount; i++) {
                if (ptParentInst->arChildInst[i] == this) {
                    ptParentInst->arChildInst[i] = ptParentInst->arChildInst[ptParentInst->uChildInstanceCount - 1];
                    ptParentInst->uChildInstanceCount--;
                    break;
                }
            }
        }
        if (ptNavZoneInst) {
            ptNavZoneInst->arChildInst[ptNavZoneInst->uChildInstanceCount] = this;
            ptNavZoneInst->uChildInstanceCount++;
        }
        ptParentInst = ptNavZoneInst;
    }
}


















const NavCell * NavCellHandleToNavCell(const NavZoneDefinition * ptZoneDef, const NavCellHandle & hNavCell) {
    NavCell * ptNavCell = NULL;
    if (hNavCell.uCellIndex < ptZoneDef->uNavCellCount)
        ptNavCell = &ptZoneDef->arNavCells[hNavCell.uCellIndex];
    return ptNavCell;
}










const NavZoneInstance * NavZoneInstanceFromSSceneID(SSceneID idSScene) {
    const NavZoneInstance * ptZoneInst = NULL;
    if (idSScene != -1) {
        SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
        if (ptSCD)
            ptZoneInst = ptSCD->ptNavZoneInst;
    }
    return ptZoneInst;
}











const NavZoneInstance * NavCellHandleToZoneInstance(const NavCellHandle & hNavCell) {
    const NavZoneInstance * ptZoneInst = NULL;
    if (hNavCell.uCellIndex != (uint32)-1)
        ptZoneInst = NavZoneInstanceFromSSceneID(hNavCell.idSScene);
    return ptZoneInst;
}














BOOL NavCellHandleFromPoint(const NavZoneInstance * ptZoneInst, const Vector2D & wp, NavCellHandle & hNavCell) {
    const NavZoneDefinition * ptZoneDef = ptZoneInst->GetZoneDef();
    hNavCell.idSScene = -1;
    hNavCell.uCellIndex = (uint32)-1;

    if (!ptZoneDef->arGridSquares)
        return FALSE;

    Vector2D wpZonePos = wp - ptZoneInst->wpWorldMin;
    IVector2D tSquareCoords;
    tSquareCoords.x = Math::FloatFloorToInt(wpZonePos.x * ptZoneDef->flOOGridSizeFeet);
    tSquareCoords.y = Math::FloatFloorToInt(wpZonePos.y * ptZoneDef->flOOGridSizeFeet);

    // A point on the far edge belongs to the last square.
    if (tSquareCoords.x == ptZoneDef->tGridSize.x)
        tSquareCoords.x--;
    if (tSquareCoords.y == ptZoneDef->tGridSize.y)
        tSquareCoords.y--;

    if (tSquareCoords.x >= ptZoneDef->tGridSize.x || tSquareCoords.y >= ptZoneDef->tGridSize.y)
        return FALSE;
    if (tSquareCoords.x < 0 || tSquareCoords.y < 0)
        return FALSE;

    const NavGridSquare & tSquare = ptZoneDef->arGridSquares[tSquareCoords.x + tSquareCoords.y * ptZoneDef->tGridSize.x];

    // The square overlaps a child zone: look there first.
    if (tSquare.uCellFlags & 0x20) {
        const NavZoneInstance * ptChildZoneInst = ptZoneInst->GetChildInstance(wp);
        if (ptChildZoneInst) {
            ScopedNavZoneDefLock tZoneDefLocker(ptChildZoneInst);
            if (!NavCellHandleFromPoint(ptChildZoneInst, wp, hNavCell))
                return FALSE;
        }
    }

    if (hNavCell.uCellIndex == (uint32)-1) {
        const NavCellLookup * arLookups = ptZoneDef->arCellLookups;
        if (!arLookups)
            return FALSE;

        arLookups += tSquare.uLookupOffset;
        for (uint16 i = 0; i < tSquare.uCellCount; i++) {
            const NavCellLookup & tLookup = arLookups[i];
            const NavCell & tNavCell = ptZoneDef->arNavCells[tLookup.uCellIndex];
            if (Point2DInsideAABB2D(wpZonePos, (const Vector2D &)tNavCell.tMin, (const Vector2D &)tNavCell.tMax)) {
                hNavCell.idSScene = ptZoneInst->idSScene;
                hNavCell.uCellIndex = tLookup.uCellIndex;
                break;
            }
        }
    }

    return hNavCell.uCellIndex != (uint32)-1;
}













































































































BOOL NavCellHandleFromWorldPlace(const WorldPlace & tPlace, NavCellHandle & hNavCell) {
    hNavCell.idSScene = -1;
    hNavCell.uCellIndex = (uint32)-1;

    WorldCommonData * ptWCD = WorldTryGetCommonData(tPlace.idSWorld);
    if (!ptWCD)
        return FALSE;

    SSceneID idSSceneStitch = NavMeshStitchWorldPlaceToScene(*ptWCD, tPlace, TRUE);
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSSceneStitch);
    if (!ptSCD)
        return FALSE;

    const NavZoneInstance * ptZoneInst = ptSCD->ptNavZoneInst;
    if (!ptZoneInst)
        return FALSE;

    ScopedNavZoneDefLock tZoneDefLocker(ptZoneInst);
    NavCellHandleFromPoint(ptZoneInst, (const Vector2D &)tPlace.wp, hNavCell);
    return TRUE;
}









BOOL NavCellPlaceIsOnWalkableCorner(const WorldPlace & tPlace) {
    NavCellHandle hNavCell;
    NavCellHandleFromWorldPlace(tPlace, hNavCell);

    const NavZoneInstance * ptZoneInstance = NavCellHandleToZoneInstance(hNavCell);
    if (ptZoneInstance) {
        ScopedNavZoneDefLock tZoneDefLock(ptZoneInstance);
        const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();
        const NavCell * ptCell = NavCellHandleToNavCell(ptZoneDef, hNavCell);
        if (ptCell) {
            Vector2D vWorldMin = (const Vector2D &)ptCell->tMin + ptZoneInstance->wpWorldMin;
            Vector2D vWorldMax = ptZoneInstance->wpWorldMin + (const Vector2D &)ptCell->tMax;

            // Only cells with a rounded corner can leave a point outside the walkable area.
            if (ptCell->uFlags >= 0x1000 && !Point2DInsideRoundedAABB2D((const Vector2D &)tPlace.wp, vWorldMin, vWorldMax, ptCell->uFlags))
                return TRUE;
        }
    }
    return FALSE;
}
















uint32 NavCellGetStaticFlags(const WorldPlace & tPlace) {
    uint32 uFlags = 0;
    NavCellHandle hNavCell;
    NavCellHandleFromWorldPlace(tPlace, hNavCell);

    const NavZoneInstance * ptZoneInstance = NavCellHandleToZoneInstance(hNavCell);
    if (ptZoneInstance) {
        ScopedNavZoneDefLock tZoneDefLock(ptZoneInstance);
        const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();
        const NavCell * ptCell = NavCellHandleToNavCell(ptZoneDef, hNavCell);
        if (ptCell)
            uFlags = ptCell->uFlags;
    }
    return uFlags;
}









static const float CLOSE_POINT_EPSILON = 0.1f;
void NavCellGetClosestPointToNavCell(const NavCellHandle & hNavCell, const NavZoneInstance * ptNavZoneInst, const Vector2D & wpGiven, Vector2D & wpClosest) {
    if (ptNavZoneInst) {
        ptNavZoneInst->LockZoneDef();
        const NavCell * ptNavCell = NavCellHandleToNavCell(ptNavZoneInst->GetZoneDef(), hNavCell);
        if (ptNavCell == NULL)
            ptNavCell = ptNavZoneInst->GetZoneDef()->arNavCells;

        Vector2D wpMin = ptNavZoneInst->wpWorldMin + (const Vector2D &)ptNavCell->tMin;
        Vector2D wpMax = ptNavZoneInst->wpWorldMin + (const Vector2D &)ptNavCell->tMax;

        wpClosest.x = Math::Pin(wpGiven.x, wpMin.x + CLOSE_POINT_EPSILON, wpMax.x - CLOSE_POINT_EPSILON);
        wpClosest.y = Math::Pin(wpGiven.y, wpMin.y + CLOSE_POINT_EPSILON, wpMax.y - CLOSE_POINT_EPSILON);
        ptNavZoneInst->UnlockZoneDef();
    }
}













inline void NavCellGetClosestPointToNavCell(const NavCellHandle & hNavCell, const Vector2D & wpGiven, Vector2D & wpClosest) {
    NavCellGetClosestPointToNavCell(hNavCell, NavCellHandleToZoneInstance(hNavCell), wpGiven, wpClosest);
}






BOOL NavCellModifyForRoundedCorners(const NavZoneInstance * ptZoneInstance, NavCellHandle & hNavCell, Vector2D & vPos, uint16 uOneRequiredFlags) {
    BOOL bModified = FALSE;
    if (!ptZoneInstance)
        return FALSE;

    ScopedNavZoneDefLock tZoneDefLock(ptZoneInstance);
    const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();
    const NavCell * ptCell = NavCellHandleToNavCell(ptZoneDef, hNavCell);
    if (ptCell && !(ptCell->uFlags & uOneRequiredFlags)) {
        float flWinningDist = 2.5f;
        const NavCellLookup * arNeighbors = &ptZoneDef->arNavCellNeighbors[ptCell->uNeighborOffset];
        for (int32 i = 0; i < ptCell->uNeighborCount; i++) {
            const NavCellLookup & tLookup = arNeighbors[i];
            if ((int16)tLookup.uCellIndex >= 0) {
                if (tLookup.uCellFlags & uOneRequiredFlags) {
                    NavCellHandle hNavCellNeighbor;
                    Vector2D vClosestPoint;
                    hNavCellNeighbor.idSScene = ptZoneInstance->idSScene;
                    hNavCellNeighbor.uCellIndex = tLookup.uCellIndex;
                    NavCellGetClosestPointToNavCell(hNavCellNeighbor, vPos, vClosestPoint);
                    float flDistSqr = Vector2D::DistanceSquared(vClosestPoint, vPos);
                    if (flDistSqr < flWinningDist) {
                        hNavCell = hNavCellNeighbor;
                        vPos = vClosestPoint;
                        bModified = TRUE;
                        flWinningDist = flDistSqr;
                    }
                }
            } else {
                // A border link: the neighbours live in other scenes.
                uint16 uIndex = tLookup.uCellIndex & 0x7fff;
                const NavCellBorderDataInst & tBorderDataInst = ptZoneInstance->GetBorderDataInst(uIndex);
                for (int j = 0; j < tBorderDataInst.uSceneCount; j++) {
                    const NavCellBorderDataSceneInst & tSceneInst = ptZoneInstance->GetBorderDataSceneInst(tBorderDataInst, j);
                    SceneCommonData * ptSCD = SceneCommonDataTryToGet(tSceneInst.idSScene);
                    if (ptSCD && (ptSCD->dwSceneFlags & (1 << SCENE_ACTIVATED)) && ptSCD->ptNavZoneInst) {
                        const NavCellLookup * arBorderNeighbors = &ptZoneInstance->arBorderLookups[tSceneInst.uNeighborOffset];
                        for (int32 k = 0; k < tSceneInst.uNeighborCount; k++) {
                            const NavCellLookup & tBorderLookup = arBorderNeighbors[k];
                            if (tBorderLookup.uCellFlags & uOneRequiredFlags) {
                                NavCellHandle hNavCellNeighbor;
                                Vector2D vClosestPoint;
                                hNavCellNeighbor.idSScene = tSceneInst.idSScene;
                                hNavCellNeighbor.uCellIndex = tBorderLookup.uCellIndex;
                                NavCellGetClosestPointToNavCell(hNavCellNeighbor, vPos, vClosestPoint);
                                float flDistSqr = Vector2D::DistanceSquared(vClosestPoint, vPos);
                                if (flDistSqr < flWinningDist) {
                                    hNavCell = hNavCellNeighbor;
                                    vPos = vClosestPoint;
                                    bModified = TRUE;
                                    flWinningDist = flDistSqr;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return bModified;
}
