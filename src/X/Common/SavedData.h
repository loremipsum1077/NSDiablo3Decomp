// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Common/SavedData.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/DataStructures/ChainedDataArrayTemplated.h"

enum {
    SAVEBLOCKID_INVALID = -1,
};

typedef int32 SaveBlockID;

static const int32 SAVE_BLOCK_NUM_DWORDS = 4;

struct SaveBlock {  // size 0x18
    SaveBlockID id;  // 0x00
    uint32 ardwData[SAVE_BLOCK_NUM_DWORDS];  // 0x04
    SaveBlockID idNextBlock;  // 0x14
};
static_assert(sizeof(SaveBlock) == 0x18, "SaveBlock");

typedef TChainedDataArray<SaveBlock, int> SaveBlockChainedDataArray;

struct SavedDataKey {  // size 0x4
    SaveBlockID idFirstBlock;  // 0x00
};
static_assert(sizeof(SavedDataKey) == 0x4, "SavedDataKey");

class SavedDataAdapter {  // size 0x18
public:
    SavedDataAdapter(SavedDataKey *, SaveBlockChainedDataArray *);
    void SaveDWORD(uint32 dwData);
    void RestoreDWORD(uint32 & dwData);
    void SaveUInt64(uint64 dwData);
    void RestoreUInt64(uint64 & dwData);
    void FreeAllBlocks();
private:
    SaveBlockID AllocNewBlock();
    SaveBlock & GetBlockFromID(SaveBlockID idBlock);
    void IncData();
    SaveBlockChainedDataArray * mptSaveBlockArray;  // 0x00
    SavedDataKey * mpSavedDataKey;  // 0x08
    SaveBlockID m_nCurrBlockId;  // 0x10
    int32 m_nCurrDataIndex;  // 0x14
};
static_assert(sizeof(SavedDataAdapter) == 0x18, "SavedDataAdapter");


// Variables defined in this file:
//   line    18  static const int32 SAVE_BLOCK_NUM_DWORDS
