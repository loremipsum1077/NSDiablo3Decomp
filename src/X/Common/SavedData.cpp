#include "X/Common/SavedData.h"


SavedDataAdapter::SavedDataAdapter(SavedDataKey * pSavedDataKey, SaveBlockChainedDataArray * ptSaveBlockArray)
    : mptSaveBlockArray(ptSaveBlockArray),
      mpSavedDataKey(pSavedDataKey),
      m_nCurrBlockId(pSavedDataKey->idFirstBlock),
      m_nCurrDataIndex(0) {
}
















void SavedDataAdapter::SaveDWORD(uint32 dwData)

{

    if (m_nCurrBlockId == SAVEBLOCKID_INVALID) {

        m_nCurrBlockId = AllocNewBlock();
        m_nCurrDataIndex = 0;
        if (mpSavedDataKey->idFirstBlock == SAVEBLOCKID_INVALID) {

            mpSavedDataKey->idFirstBlock = m_nCurrBlockId;
        }
    }


    SaveBlock & tBlock = GetBlockFromID(m_nCurrBlockId);


    tBlock.ardwData[m_nCurrDataIndex] = dwData;

    IncData();
}



void SavedDataAdapter::RestoreDWORD(uint32 & dwData)
{


    SaveBlock & tBlock = GetBlockFromID(m_nCurrBlockId);


    dwData = tBlock.ardwData[m_nCurrDataIndex];
    IncData();
}



void SavedDataAdapter::SaveUInt64(uint64 dwData)

{
    uint32 nLowerHalf = (uint32)dwData;
    uint32 nUpperHalf = (uint32)(dwData >> 32);
    SaveDWORD(nLowerHalf);
    SaveDWORD(nUpperHalf);
}




void SavedDataAdapter::RestoreUInt64(uint64 & dwData)
{
    uint32 nLowerHalf, nUpperHalf;
    RestoreDWORD(nLowerHalf);
    RestoreDWORD(nUpperHalf);
    dwData = nLowerHalf | ((uint64)nUpperHalf << 32);
}



void SavedDataAdapter::FreeAllBlocks()
{

    SaveBlockID idBlock = mpSavedDataKey->idFirstBlock;
    while (idBlock != SAVEBLOCKID_INVALID) {

        SaveBlockID idLastBlock = idBlock;
        SaveBlock & tBlock = GetBlockFromID(idBlock);
        idBlock = tBlock.idNextBlock;
        mptSaveBlockArray->ElementDeleteByID(idLastBlock);
    }

    mpSavedDataKey->idFirstBlock = SAVEBLOCKID_INVALID;
}



SaveBlockID SavedDataAdapter::AllocNewBlock()
{
    SaveBlockID idBlock;
    idBlock = mptSaveBlockArray->ElementNew();

    SaveBlock & tBlock = GetBlockFromID(idBlock);
    tBlock.idNextBlock = SAVEBLOCKID_INVALID;
    return idBlock;
}



SaveBlock & SavedDataAdapter::GetBlockFromID(SaveBlockID idBlock)
{

    SaveBlock * ptBlock = mptSaveBlockArray->ElementGet(idBlock);
    return *ptBlock;
}




void SavedDataAdapter::IncData()
{

    m_nCurrDataIndex++;
    if (m_nCurrDataIndex >= SAVE_BLOCK_NUM_DWORDS) {


        SaveBlock & tBlock = GetBlockFromID(m_nCurrBlockId);
        if (tBlock.idNextBlock == SAVEBLOCKID_INVALID) {

            tBlock.idNextBlock = AllocNewBlock();
        }
        m_nCurrBlockId = tBlock.idNextBlock;
        m_nCurrDataIndex = 0;
    }
}
