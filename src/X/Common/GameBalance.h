// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/GameBalance.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/SigmaTypes.h"
#include "Protobuf/GameBalance.pb.h"

namespace D3 { namespace GameBalance { class Handle; } }

typedef int32 GBID;

static const GBID INVALID_GBID = -2;  // line 22. DW_AT_const_value -2 in all 7 CUs that record it (e.g. ScriptUtil.cpp cu_068be75a, ActorInventory.cpp cu_03df0cd8)

// line 21. DW_AT_const_value -1 in every CU that records it (120 CUs).
static const GBID GBID_NONE = -1;

typedef blz::vector<int, blz::allocator<int> > GBIDArray;

typedef XPooledList<int, const int &> GBIDPooledList;

typedef XListMemoryPool<int> GBIDPool;

typedef int32 GameBalanceType;

struct GBHandle {  // size 0x8
    GameBalanceType eType;  // 0x00
    GBID gbid;  // 0x04
    GBHandle() = default;  // DWARF line 37: declared, not artificial; no body anywhere. = default per the stores in sStartSpawn (SpawnerBrain.cpp:720)
    GBHandle(const GameBalanceType eType, const GBID gbid);
    GBHandle(const D3::GameBalance::Handle &);
    BOOL operator==(const GBHandle & arg) const;
    BOOL operator!=(const GBHandle & arg) const;
    void Set(const GameBalanceType _eType, const GBID _gbid);
    void Export(D3::GameBalance::Handle & tHandle) const;
    void Import(const D3::GameBalance::Handle & tHandle);
};
static_assert(sizeof(GBHandle) == 0x8, "GBHandle");

// line 78. Body not in the DWARF; written from ItemsValidateGeneratorParams (Items.cpp).
inline void GBHandle::Import(const D3::GameBalance::Handle & tHandle) {
    eType = (GameBalanceType)tHandle.game_balance_type();
    gbid = tHandle.gbid();
}

// line 46. The DWARF inlines Import (line 78) at line 48 of this body.
inline GBHandle::GBHandle(const D3::GameBalance::Handle & tHandle) {
    Import(tHandle);
}

// line 39. The DWARF inlines Set (line 61) at line 43 of this body.
inline GBHandle::GBHandle(const GameBalanceType eType, const GBID gbid) {
    Set(eType, gbid);
}

// line 61. Two stores, attributed to line 65 in sGetAttributeStringFromSocket (UIInventory.cpp:684)
// and sMonsterRemoveAllDefensiveAffixes (RandomMonster.cpp:498).
inline void GBHandle::Set(const GameBalanceType _eType, const GBID _gbid) {
    eType = _eType;
    gbid = _gbid;
}

// line 51. Written from the copies inlined into NephalemRift::IsRiftKey (NephalemRiftCommon.cpp:53, :82).
inline BOOL GBHandle::operator==(const GBHandle & arg) const {
    return eType == arg.eType && gbid == arg.gbid;
}

// line 56. Written from the copy inlined into sStackItems (ActorInventory.cpp:7252).
inline BOOL GBHandle::operator!=(const GBHandle & arg) const {
    return eType != arg.eType || gbid != arg.gbid;
}

// line 70. Written from the copy inlined into GrantChallengeReward (SChallengeRift.cpp:173):
// set_game_balance_type (line 73) then set_gbid (line 74).
inline void GBHandle::Export(D3::GameBalance::Handle & tHandle) const {
    tHandle.set_game_balance_type(eType);
    tHandle.set_gbid(gbid);
}

extern const GBHandle cghgbNone;

GBID StringToGBID(LPCSTR szGBID);  // GameBalance.cpp:24
typedef int32 AssetListType;  // X/Common/Definitions/GlobalDefinition.h
void PackAssetsIntoBitPackedGbidArray(D3::GameBalance::BitPackedGbidArray & tPackedArray, AssetListType eAssetList, int32 * ptAssetIdArray, int32 nNumAssets, int32 nMaxAssets);  // GameBalance.cpp:42
int32 UnpackAssetsFromBitPackedGbidArray(const D3::GameBalance::BitPackedGbidArray & tPackedArray, AssetListType eAssetList, int32 * ptAssetIdArray, int32 nMaxAssets);  // GameBalance.cpp:91

typedef XPooledList<GBHandle, const GBHandle &> GBHandleList;

class GBHandlePool : public XListMemoryPool<GBHandle> {  // size 0x30
};
static_assert(sizeof(GBHandlePool) == 0x30, "GBHandlePool");


// Functions defined in this file (bodies are not in the DWARF):
//   line    39  GBHandle::GBHandle(const GameBalanceType eType, const GBID gbid)  [inlined into 312 functions]
//   line    46  GBHandle::GBHandle(const D3::GameBalance::Handle & tHandle)  [inlined into 4 functions]
//   line    51  BOOL GBHandle::operator==(const GBHandle & arg) const  [inlined into 6 functions]
//   line    56  BOOL GBHandle::operator!=(const GBHandle & arg) const  [inlined into 1 functions]
//   line    61  void GBHandle::Set(const GameBalanceType _eType, const GBID _gbid)  [inlined into 330 functions]
//   line    70  void GBHandle::Export(D3::GameBalance::Handle & tHandle) const  [inlined into 17 functions]
//   line    78  void GBHandle::Import(const D3::GameBalance::Handle & tHandle)  [inlined into 7 functions]

// Variables defined in this file:
//   line    21  static const GBID GBID_NONE
//   line    22  static const GBID INVALID_GBID

// GameBalance.cpp: the app subsystem entry points (X/Common/Game.cpp sgtAppSubsystems).
void GameBalanceInitializeForApp();
void GameBalanceDisposeForApp();
