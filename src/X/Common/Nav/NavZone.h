// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Nav/NavZone.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "NavCellHandle.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"

struct NavCell;
struct NavCellLookup;
struct NavZoneDefinition;
class SceneDefinition;
struct WorldPlace;

enum {
    NCBDSI_FLAG_INITIALIZED = 0,
};

struct NavCellBorderDataSceneInst {  // size 0xc
    SSceneID idSScene;  // 0x00
    uint8 uFlags;  // 0x04
    uint8 uBorder;  // 0x05
    uint16 uNeighborCount;  // 0x06
    uint32 uNeighborOffset;  // 0x08
};
static_assert(sizeof(NavCellBorderDataSceneInst) == 0xc, "NavCellBorderDataSceneInst");

enum {
    NCBDI_FLAG_INITIALIZED = 0,
};

struct NavCellBorderDataInst {  // size 0x6
    uint16 uFlags;  // 0x00
    uint16 uSceneCount;  // 0x02
    uint16 uSceneOffset;  // 0x04
};
static_assert(sizeof(NavCellBorderDataInst) == 0x6, "NavCellBorderDataInst");

// The DWARF does not record `mutable`; the members below marked so are written by const
// members (the lock helpers and the border caches built on demand).
struct NavZoneInstance {  // size 0x108
    SWorldID idSWorld;  // 0x00
    SSceneID idSScene;  // 0x04
    SNO snoScene;  // 0x08
    Vector3D wpWorldMin3d;  // 0x0c
    Vector2D wpWorldMin;  // 0x18
    Vector2D wpWorldMax;  // 0x20
    uint32 uBorderDataInstCount;  // 0x28
    NavCellBorderDataInst * arBorderDataInst;  // 0x30
    mutable uint32 uMaxBorderScenes;  // 0x38
    mutable uint32 uBorderSceneCount;  // 0x3c
    mutable NavCellBorderDataSceneInst * arBorderScenes;  // 0x40
    mutable uint32 uMaxBorderLookups;  // 0x48
    mutable uint32 uBorderLookupCount;  // 0x4c
    mutable NavCellLookup * arBorderLookups;  // 0x50
    void Initialize();
    void Dispose();
    void LockZoneDef() const;
    void UnlockZoneDef() const;
    static void KeepLocks();
    static void ReleaseLocks();
    const NavCellBorderDataSceneInst & GetBorderDataSceneInst(const NavCellBorderDataInst & tBorderDataInst, uint32 uIndex) const;
    const NavCellBorderDataInst & GetBorderDataInst(int nIndex) const;
    void GetBorderAABB(const NavCell & tNavCell, uint32 uBorder, Vector2D & wpMin, Vector2D & wpMax) const;
    void InitBorderDataSceneInst(const NavCellBorderDataInst & tBorderDataInst, NavCellBorderDataSceneInst & tBorderDataSceneInst) const;
    void InitBorderDataInst(int nIndex) const;
    uint32 GetChildInstances(const Vector2D & wpMin, const Vector2D & wpMax, NavZoneInstance ** _arChildInst) const;
    const NavZoneInstance * GetChildInstance(const Vector2D & wp) const;
    uint32 GetChildInstanceCount() const;
    const NavZoneDefinition * GetZoneDef() const;
    const SceneDefinition * GetSceneDef() const;
    void SetParentInstance(NavZoneInstance * ptNavZoneInst);
protected:
    mutable const NavZoneDefinition * ptNavZoneDef;  // 0x58
    mutable const SceneDefinition * ptSceneDef;  // 0x60
    mutable int nLockCount;  // 0x68
    NavZoneInstance * ptParentInst;  // 0x70
    uint32 uChildInstanceCount;  // 0x78
    NavZoneInstance * arChildInst[16];  // 0x80
    mutable const NavZoneInstance * ptNextLocked;  // 0x100
    static const NavZoneInstance * sptFirstLocked;
    static BOOL sfKeepLocks;
};
static_assert(sizeof(NavZoneInstance) == 0x108, "NavZoneInstance");

inline const NavCellBorderDataSceneInst & NavZoneInstance::GetBorderDataSceneInst(const NavCellBorderDataInst & tBorderDataInst, uint32 uIndex) const {  // line 160
    NavCellBorderDataSceneInst & tBorderDataSceneInst = arBorderScenes[tBorderDataInst.uSceneOffset + uIndex];
    if (!(tBorderDataSceneInst.uFlags & (1 << NCBDSI_FLAG_INITIALIZED))) {
        InitBorderDataSceneInst(tBorderDataInst, tBorderDataSceneInst);
    }
    return tBorderDataSceneInst;
}

inline const NavCellBorderDataInst & NavZoneInstance::GetBorderDataInst(int nIndex) const {  // line 176
    NavCellBorderDataInst & tBorderDataInst = arBorderDataInst[nIndex];
    if (!(tBorderDataInst.uFlags & (1 << NCBDI_FLAG_INITIALIZED))) {
        InitBorderDataInst(nIndex);
    }
    return tBorderDataInst;
}

inline uint32 NavZoneInstance::GetChildInstanceCount() const {  // line 194
    return uChildInstanceCount;
}

inline const NavZoneDefinition * NavZoneInstance::GetZoneDef() const {  // line 196
    return ptNavZoneDef;
}

// Invented: the DWARF declares GetSceneDef (line 197) but records no body and no inlined call.
// Defined inline so the one caller (sSweptPoint3DAndNavZone) reads ptSceneDef in place as the
// original does, instead of calling a function the original does not have.
inline const SceneDefinition * NavZoneInstance::GetSceneDef() const {  // line 197
    return ptSceneDef;
}

class ScopedNavZoneDefLock {  // size 0x8
public:
    ScopedNavZoneDefLock(const NavZoneInstance *);
    ~ScopedNavZoneDefLock();
protected:
    const NavZoneInstance * m_ptNavZoneInst;  // 0x00
};
static_assert(sizeof(ScopedNavZoneDefLock) == 0x8, "ScopedNavZoneDefLock");

inline ScopedNavZoneDefLock::ScopedNavZoneDefLock(const NavZoneInstance * ptNavZoneInst) {  // line 222
    m_ptNavZoneInst = ptNavZoneInst;
    m_ptNavZoneInst->LockZoneDef();
}

inline ScopedNavZoneDefLock::~ScopedNavZoneDefLock() {  // line 228
    m_ptNavZoneInst->UnlockZoneDef();
}

inline uint32 NavZoneGetBitArrayCount(uint32 uNavCellCount) {  // line 270
    uint32 uBitArrayCount = uNavCellCount >> 5;
    if (uNavCellCount & 31) {
        uBitArrayCount++;
    }
    return uBitArrayCount;
}

const NavCell * NavCellHandleToNavCell(const NavZoneDefinition * ptZoneDef, const NavCellHandle & hNavCell);
const NavZoneInstance * NavZoneInstanceFromSSceneID(SSceneID idSScene);
const NavZoneInstance * NavCellHandleToZoneInstance(const NavCellHandle & hNavCell);
BOOL NavCellHandleFromPoint(const NavZoneInstance * ptZoneInst, const Vector2D & wp, NavCellHandle & hNavCell);
BOOL NavCellHandleFromWorldPlace(const WorldPlace & tPlace, NavCellHandle & hNavCell);
BOOL NavCellPlaceIsOnWalkableCorner(const WorldPlace & tPlace);
uint32 NavCellGetStaticFlags(const WorldPlace & tPlace);
void NavCellGetClosestPointToNavCell(const NavCellHandle & hNavCell, const NavZoneInstance * ptNavZoneInst, const Vector2D & wpGiven, Vector2D & wpClosest);
BOOL NavCellModifyForRoundedCorners(const NavZoneInstance * ptZoneInstance, NavCellHandle & hNavCell, Vector2D & vPos, uint16 uOneRequiredFlags);


// Functions defined in this file (bodies are not in the DWARF):
//   line   160  const NavCellBorderDataSceneInst & NavZoneInstance::GetBorderDataSceneInst(const NavCellBorderDataInst & tBorderDataInst, uint32 uIndex) const  [inlined into 2 functions]
//   line   176  const NavCellBorderDataInst & NavZoneInstance::GetBorderDataInst(int nIndex) const  [inlined into 2 functions]
//   line   194  uint32 NavZoneInstance::GetChildInstanceCount() const  [inlined into 1 functions]
//   line   196  const NavZoneDefinition * NavZoneInstance::GetZoneDef() const  [inlined into 20 functions]
//   line   222  ScopedNavZoneDefLock::ScopedNavZoneDefLock(const NavZoneInstance * ptNavZoneInst)  [inlined into 19 functions]
//   line   228  ScopedNavZoneDefLock::~ScopedNavZoneDefLock()  [inlined into 19 functions]
//   line   270  uint32 NavZoneGetBitArrayCount(uint32 uNavCellCount)  [inlined into 3 functions]
