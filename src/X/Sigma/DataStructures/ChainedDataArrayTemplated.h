#pragma once

#include "DataArraysTemplated.h"
#include "XGrowableArray.h"
#include "../Math/MathGeneral.h"










template <typename TYPE, typename TYPEID>
class TChainedDataArray : public CDataArrayBase {
public:

    inline void DeleteAll();

    inline TYPEID ElementNew();
    inline void ElementDelete(TYPE* lpElement, BOOL bDestroy = TRUE);
    inline void ElementDeleteByID(TYPEID id);



    inline TYPE* ElementGet(TYPEID dataID) {

        uint16 wIndex = dataID & DATA_INDEX_MASK;
        return ElementGetFromBaseIndex(wIndex);
    }

    inline TYPE* ElementTryToGet(TYPEID dataID);



    inline TYPE* ElementGetFromBaseIndex(uint16 wIndex);

    inline void SetGrowCap(uint32 uNewGrowCap);
    inline int32 AvailableCount();
    inline void PreAllocateDataBlockSlots();

    inline TYPE* Iterate(TDataArrayIterator<TYPE>& tIterator);
    inline TYPE* Iterate(uint16& wIndex, TYPE*& ptElement);

    inline void ResetIdentifier();










    XGrowableArray<TYPE*, TYPE*> m_aptDataBlocks;
    XMemoryAllocatorInterface* ptMemoryInterface;
    uint32 dwGrowSize;
    uint32 dwGrowCap;
    uint32 dwShiftCount;

private:
    inline void sGrow();
};



































template <typename TYPE, typename TYPEID>
TChainedDataArray<TYPE, TYPEID>* TChainedDataArrayCreateTracked(const CHAR* szName,
                                                                XMemoryAllocatorInterface* ptMemoryInterface,
                                                                uint32 dwGrowSize) {
    TChainedDataArray<TYPE, TYPEID>* lptDataArray;

    lptDataArray = (TChainedDataArray<TYPE, TYPEID>*)SigmaMemoryNew(sizeof(TChainedDataArray<TYPE, TYPEID>), 4,
                                                                    ptMemoryInterface, TRUE);

    Blizzard::String::Copy(lptDataArray->szName, szName, MAXIMUM_DATA_ARRAY_NAME_LENGTH);
    lptDataArray->dwGrowSize = dwGrowSize;
    lptDataArray->dwGrowCap = MAX_DATA_ARRAY_ELEMENTS;
    lptDataArray->ptMemoryInterface = ptMemoryInterface;
    lptDataArray->dwElementSize = sizeof(TYPE);
    lptDataArray->dwShiftCount = Math::LogBase2(dwGrowSize);

























    lptDataArray->m_aptDataBlocks.Initialize();
    lptDataArray->m_aptDataBlocks.SetMemoryInterface(ptMemoryInterface);
    lptDataArray->fGrowable = TRUE;

    lptDataArray->m_aptDataBlocks.SetAtGrow_Tracked(0, (TYPE*)SigmaMemoryNew(lptDataArray->dwGrowSize * sizeof(TYPE), 16, ptMemoryInterface, FALSE));
    lptDataArray->dwMaximumCount = lptDataArray->dwGrowSize;
    lptDataArray->DeleteAll();
    lptDataArray->ResetIdentifier();

    gptTrackedDAList->AddTail_Tracked(lptDataArray);

    return lptDataArray;
}




template <typename TYPE, typename TYPEID>
void TChainedDataArray<TYPE, TYPEID>::PreAllocateDataBlockSlots() {


    uint32 wBlockCount = dwGrowCap / dwGrowSize;
    uint32 wCurrentBlockIndex = dwMaximumCount >> dwShiftCount;
    for (uint32 wIndex = wCurrentBlockIndex; wIndex < wBlockCount; wIndex++) {

        m_aptDataBlocks.SetAtGrow_Tracked(wIndex, NULL);
    }
}




template <typename TYPE, typename TYPEID>
void TChainedDataArrayDispose(TChainedDataArray<TYPE, TYPEID>*& lptDataArray) {
    if (lptDataArray != NULL) {



        POS pos = gptTrackedDAList->Find(lptDataArray);
        if (pos != NULL) {

            gptTrackedDAList->RemoveAt(pos);
        }


        for (int32 i = 0; i < lptDataArray->m_aptDataBlocks.GetArraySize(); i++) {

            XFREE(lptDataArray->m_aptDataBlocks[i], lptDataArray->ptMemoryInterface);
        }
        lptDataArray->m_aptDataBlocks.Dispose();

        XFREE(lptDataArray, lptDataArray->ptMemoryInterface);
    }
}



template <typename TYPE, typename TYPEID>
void TChainedDataArray<TYPE, TYPEID>::ResetIdentifier() {






    uint16 dwHash = XHashKey<LPCSTR>(szName);

    wNextIdentifier = (dwHash & 0xFF) | 0x7700;
}








template <typename TYPE, typename TYPEID>
void TChainedDataArray<TYPE, TYPEID>::DeleteAll() {
    if (this == NULL) {
        return;
    }

    wHighestUsedIndex = DATA_INDEX_INVALID;
    dwTotalCount = 0;
    wFirstFreeItem = 0;
    dwFreeCount = dwMaximumCount;




    for (uint16 wIndex = 0; wIndex < dwMaximumCount; wIndex++) {

        TYPE* ptElement = ElementGetFromBaseIndex(wIndex);
        ptElement->id = (TYPEID)DATAID_INVALID;
        DATA_ELEMENT_NEXT_FREE(ptElement) = wIndex + 1;
    }



    TYPE* ptLastElement = ElementGetFromBaseIndex(dwMaximumCount - 1);
    DATA_ELEMENT_NEXT_FREE(ptLastElement) = DATA_INDEX_INVALID;
}




template <typename TYPE, typename TYPEID>
void TChainedDataArray<TYPE, TYPEID>::SetGrowCap(uint32 uNewGrowCap) {
    dwGrowCap = uNewGrowCap;
}








template <typename TYPE, typename TYPEID>
int32 TChainedDataArray<TYPE, TYPEID>::AvailableCount() {
    return dwFreeCount;
}








template <typename TYPE, typename TYPEID>
void TChainedDataArray<TYPE, TYPEID>::sGrow() {






    uint32 dwNewMaxCount = dwMaximumCount + dwGrowSize;








    TYPE* ptBlock = (TYPE*)SigmaMemoryNew(dwGrowSize * sizeof(TYPE), 16, ptMemoryInterface, FALSE);
    uint32 wCurrentBlockIndex = dwMaximumCount >> dwShiftCount;
    m_aptDataBlocks.SetAtGrow_Tracked(wCurrentBlockIndex, ptBlock);

    TYPE* ptElement = ptBlock;
    for (uint16 wIndex = dwMaximumCount; wIndex < dwNewMaxCount; wIndex++) {
        ptElement->id = (TYPEID)DATAID_INVALID;
        DATA_ELEMENT_NEXT_FREE(ptElement) = wIndex + 1;
        ptElement++;
    }


    TYPE* ptLastElement = &ptBlock[dwGrowSize - 1];
    DATA_ELEMENT_NEXT_FREE(ptLastElement) = DATA_INDEX_INVALID;

    wFirstFreeItem = dwMaximumCount;
    dwFreeCount = dwNewMaxCount - dwMaximumCount;
    dwMaximumCount = dwNewMaxCount;
}




template <typename TYPE, typename TYPEID>
TYPEID TChainedDataArray<TYPE, TYPEID>::ElementNew() {
    TYPE* ptElement;
    uint16 wIndex;

    if (dwFreeCount == 0) {


        sGrow();
    }

    wIndex = wFirstFreeItem;

    ptElement = ElementGetFromBaseIndex(wIndex);
    wFirstFreeItem = DATA_ELEMENT_NEXT_FREE(ptElement);
    SigmaMemoryClear(ptElement, sizeof(TYPE));
    new (ptElement) TYPE;

    dwFreeCount--;

    ptElement->id = (TYPEID)((wNextIdentifier << DATA_INDEX_BITS) | wIndex);
    if (ptElement->id == DATAID_INVALID) {
        wNextIdentifier++;
        ptElement->id = (TYPEID)((wNextIdentifier << DATA_INDEX_BITS) | wIndex);
    }
    wNextIdentifier++;

    dwTotalCount++;

    if (wHighestUsedIndex == DATA_INDEX_INVALID || wHighestUsedIndex < wIndex) {
        wHighestUsedIndex = wIndex;
    }

    return ptElement->id;
}








template <typename TYPE, typename TYPEID>
void TChainedDataArray<TYPE, TYPEID>::ElementDelete(TYPE* lpElement, BOOL bDestroy) {


    if (bDestroy) {

        lpElement->~TYPE();
    }

    if (lpElement->id != DATAID_INVALID) {



















        uint16 wIndex = lpElement->id & DATA_INDEX_MASK;

        lpElement->id = (TYPEID)DATAID_INVALID;
        DATA_ELEMENT_NEXT_FREE(lpElement) = wFirstFreeItem;
        dwFreeCount++;
        wFirstFreeItem = wIndex;

        if (wIndex == wHighestUsedIndex) {
            do {
                wHighestUsedIndex--;
                wIndex--;






                if (wHighestUsedIndex != DATA_INDEX_INVALID) {
                    lpElement = ElementGetFromBaseIndex(wIndex);
                }
            } while (wIndex != DATA_INDEX_INVALID && lpElement->id == DATAID_INVALID);
        }

        dwTotalCount--;
    }
}




template <typename TYPE, typename TYPEID>
void TChainedDataArray<TYPE, TYPEID>::ElementDeleteByID(TYPEID id) {
    TYPE* lpElement;

    lpElement = ElementTryToGet(id);

    if (lpElement != NULL) {
        ElementDelete(lpElement);
    }
}






template <typename TYPE, typename TYPEID>
TYPE* TChainedDataArray<TYPE, TYPEID>::ElementTryToGet(TYPEID dataID) {
    if (dataID == DATAID_INVALID) {
        return NULL;
    }







    uint16 wIndex = dataID & DATA_INDEX_MASK;
    if (wIndex >= dwMaximumCount) {
        return NULL;
    }


    TYPE* ptElement = ElementGetFromBaseIndex(wIndex);

    if (ptElement->id != dataID) {
        return NULL;
    }

    return ptElement;
}














































template <typename TYPE, typename TYPEID>
TYPE* TChainedDataArray<TYPE, TYPEID>::Iterate(uint16& wIndex, TYPE*& ptElement) {
    wIndex++;

    if (wHighestUsedIndex == DATA_INDEX_INVALID) {
        return NULL;
    }

    while (wIndex <= wHighestUsedIndex) {
















        ptElement = ElementGetFromBaseIndex(wIndex);

        if (ptElement->id != DATAID_INVALID) {
            return ptElement;
        }

        wIndex++;
    }

    return NULL;
}





template <typename TYPE, typename TYPEID>
TYPE* TChainedDataArray<TYPE, TYPEID>::Iterate(TDataArrayIterator<TYPE>& tIterator) {


    return Iterate(tIterator.wIndex, tIterator.ptElement);
}




template <typename TYPE, typename TYPEID>
TYPE* TChainedDataArray<TYPE, TYPEID>::ElementGetFromBaseIndex(uint16 wIndex) {





    uint32 wBlockIndex = wIndex >> dwShiftCount;
    uint32 wSubIndex = wIndex & ((1 << dwShiftCount) - 1);
    TYPE* paTypeBlock = m_aptDataBlocks[wBlockIndex];

    return &paTypeBlock[wSubIndex];
}
