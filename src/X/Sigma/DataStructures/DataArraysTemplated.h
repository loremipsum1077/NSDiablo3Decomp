#pragma once

#include "DataStructuresExternal.h"
#include "XGrowableList.h"
#include "XPooledList.h"
#include "../Math/MathGeneral.h"
#include "../RefString.h"

#include <new>

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../Memory/SigmaMemory.h"














enum {
    MAXIMUM_DATA_ARRAY_NAME_LENGTH = 256,
    DATA_INDEX_BITS = 16,
    DATA_INDEX_MASK = 65535,
    DATA_IDENTIFIER_BITS = 16,
    DATA_IDENTIFIER_MASK = 65535,
};


static const uint16 DATA_INDEX_INVALID = 0xFFFF;

typedef int32 DataID;

static const DataID DATAID_INVALID = -1;

typedef XPooledList<int, int> XPooledDataIDList;




static const uint32 MAX_DATA_ARRAY_ELEMENTS = 0xFFFE;

// A free element keeps the index of the next free element in the 16 bits that
// follow its id. Name not in the DWARF.
#define DATA_ELEMENT_NEXT_FREE(ptElement) (*(uint16*)(&(ptElement)->id + 1))



























template <typename TYPE>
struct TDataArrayIterator {

    // Reconstructed ABI storage: the definition family uses the wide index.
    union { uint16 wIndex; uint32 unk_0; };
    TYPE* ptElement;

    inline TDataArrayIterator() {
        wIndex = DATA_INDEX_INVALID;
        ptElement = NULL;
    }



    explicit TDataArrayIterator(uint32 index) : unk_0(index), ptElement(NULL) {}

    inline TYPE* operator->() { return ptElement; }
};




struct TDataArrayThreadSafeIterator {

    int32 nIndex;

    // Never instantiated in the binary: no bodies are known.
    TDataArrayThreadSafeIterator();



    void Reset();
};





class CDataArrayBase {
public:

    inline uint32 GetTotalCount() const { return dwTotalCount; }





    CHAR szName[MAXIMUM_DATA_ARRAY_NAME_LENGTH];

    uint32 dwMaximumCount;
    uint32 dwElementSize;

    uint16 wHighestUsedIndex;
    uint32 dwTotalCount;

    uint16 wNextIdentifier;
    uint16 wFirstFreeItem;
    uint32 dwFreeCount;

    BOOL fGrowable;
};

template <typename TYPE, typename TYPEID>
class TDataArray : public CDataArrayBase {
public:

    inline void DeleteAll();

    inline TYPEID ElementNew();
    void ElementDelete(TYPE* lpElement);
    inline void ElementDeleteByID(TYPEID id);



    inline TYPE* ElementGet(TYPEID dataID) {

        uint16 wIndex = dataID & DATA_INDEX_MASK;
        return &ptData[wIndex];
    }

    inline TYPE* ElementTryToGet(TYPEID dataID);

    inline int32 AvailableCount();

    inline TYPE* Iterate(TDataArrayIterator<TYPE>& tIterator);
    inline TYPE* Iterate(uint16& wIndex, TYPE*& ptElement);




    // Never instantiated in the binary: no body is known.
    TYPE* Iterate(TDataArrayThreadSafeIterator& tIterator);

    inline void ResetIdentifier();



    TYPE* ptData;
    XMemoryAllocatorInterface* ptMemoryInterface;

private:

    inline BOOL sGrow();
};

// X/Sigma/DataStructures/DataArraysTemplated.cpp
typedef XGrowableList<CDataArrayBase*, CDataArrayBase*> DataArrayList;
extern DataArrayList* gptTrackedDAList;
void DataArrayTrackerInit();
void DataArrayTrackerDispose();



























































































template <typename TYPE, typename TYPEID>
TDataArray<TYPE, TYPEID>* TDataArrayCreateTracked(const CHAR* szName, XMemoryAllocatorInterface* ptMemoryInterface,
                                                  uint32 dwMaximumCount) {
    TDataArray<TYPE, TYPEID>* lptDataArray;

    lptDataArray = (TDataArray<TYPE, TYPEID>*)SigmaMemoryNew(sizeof(TDataArray<TYPE, TYPEID>), 4, ptMemoryInterface, TRUE);

    Blizzard::String::Copy(lptDataArray->szName, szName, MAXIMUM_DATA_ARRAY_NAME_LENGTH);
    lptDataArray->dwMaximumCount = dwMaximumCount;

    lptDataArray->ptData = (TYPE*)SigmaMemoryNew(dwMaximumCount * sizeof(TYPE), 16, ptMemoryInterface, FALSE);
    lptDataArray->dwElementSize = sizeof(TYPE);
    lptDataArray->ptMemoryInterface = ptMemoryInterface;




















    lptDataArray->DeleteAll();
    lptDataArray->ResetIdentifier();

    gptTrackedDAList->AddTail_Tracked(lptDataArray);

    return lptDataArray;
}




template <typename TYPE, typename TYPEID>
TDataArray<TYPE, TYPEID>* TGrowableDataArrayCreateTracked(const CHAR* szName, XMemoryAllocatorInterface* ptMemoryInterface,
                                                          uint32 dwInitialMaximumCount) {
    TDataArray<TYPE, TYPEID>* lptDataArray =
        TDataArrayCreateTracked<TYPE, TYPEID>(szName, ptMemoryInterface, dwInitialMaximumCount);

    lptDataArray->fGrowable = TRUE;

    return lptDataArray;
}















template <typename TYPE, typename TYPEID>
void TDataArrayDispose(TDataArray<TYPE, TYPEID>*& lptDataArray) {
    if (lptDataArray != NULL) {



        POS pos = gptTrackedDAList->Find(lptDataArray);
        if (pos != NULL) {

            gptTrackedDAList->RemoveAt(pos);
        }

        XFREE(lptDataArray->ptData, lptDataArray->ptMemoryInterface);
        XFREE(lptDataArray, lptDataArray->ptMemoryInterface);
    }
}






template <typename TYPE, typename TYPEID>
void TDataArray<TYPE, TYPEID>::ResetIdentifier() {






    uint16 dwHash = XHashKey<LPCSTR>(szName);

    wNextIdentifier = (dwHash & 0xFF) | 0x7700;
}








template <typename TYPE, typename TYPEID>
void TDataArray<TYPE, TYPEID>::DeleteAll() {
    if (this == NULL) {
        return;
    }

    wHighestUsedIndex = DATA_INDEX_INVALID;
    dwTotalCount = 0;
    wFirstFreeItem = 0;
    dwFreeCount = dwMaximumCount;




    for (uint16 wIndex = 0; wIndex < dwMaximumCount; wIndex++) {

        TYPE* ptElement = &ptData[wIndex];
        ptElement->id = (TYPEID)DATAID_INVALID;
        DATA_ELEMENT_NEXT_FREE(ptElement) = wIndex + 1;
    }



    TYPE* ptLastElement = &ptData[dwMaximumCount - 1];
    DATA_ELEMENT_NEXT_FREE(ptLastElement) = DATA_INDEX_INVALID;
}





template <typename TYPE, typename TYPEID>
int32 TDataArray<TYPE, TYPEID>::AvailableCount() {
    return dwFreeCount;
}








template <typename TYPE, typename TYPEID>
BOOL TDataArray<TYPE, TYPEID>::sGrow() {
    if (!fGrowable) {
        return FALSE;
    }

    if (dwMaximumCount >= MAX_DATA_ARRAY_ELEMENTS) {
        return FALSE;
    }



    uint32 dwNewMaxCount = Math::Min<uint32>(dwMaximumCount * 2, MAX_DATA_ARRAY_ELEMENTS);

    ptData = (TYPE*)SigmaMemoryResize(ptData, ptMemoryInterface, (uint32)(dwNewMaxCount * sizeof(TYPE)), 16);






    for (uint16 wIndex = dwMaximumCount; wIndex < dwNewMaxCount; wIndex++) {

        TYPE* ptElement = &ptData[wIndex];
        ptElement->id = (TYPEID)DATAID_INVALID;
        DATA_ELEMENT_NEXT_FREE(ptElement) = wIndex + 1;
    }



    TYPE* ptLastElement = &ptData[dwNewMaxCount - 1];
    DATA_ELEMENT_NEXT_FREE(ptLastElement) = DATA_INDEX_INVALID;

    wFirstFreeItem = dwMaximumCount;
    dwFreeCount = dwNewMaxCount - dwMaximumCount;
    dwMaximumCount = dwNewMaxCount;

    return TRUE;
}




template <typename TYPE, typename TYPEID>
TYPEID TDataArray<TYPE, TYPEID>::ElementNew() {
    TYPE* ptElement;
    uint16 wIndex;

    if (dwFreeCount == 0) {





        if (!sGrow()) {
            return (TYPEID)DATAID_INVALID;
        }
    }

    wIndex = wFirstFreeItem;
    ptElement = &ptData[wIndex];
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
void TDataArray<TYPE, TYPEID>::ElementDelete(TYPE* lpElement) {


    lpElement->~TYPE();





    uint16 wIndex = lpElement->id & DATA_INDEX_MASK;



    if (lpElement->id == DATAID_INVALID) {
        return;
    }



    lpElement->id = (TYPEID)DATAID_INVALID;
    DATA_ELEMENT_NEXT_FREE(lpElement) = wFirstFreeItem;
    wFirstFreeItem = wIndex;
    dwFreeCount++;


    if (wIndex == wHighestUsedIndex) {



        do {
            lpElement--;
            wHighestUsedIndex--;
        }
        while (wHighestUsedIndex != DATA_INDEX_INVALID && lpElement->id == DATAID_INVALID);
    }

    dwTotalCount--;
}






template <typename TYPE, typename TYPEID>
void TDataArray<TYPE, TYPEID>::ElementDeleteByID(TYPEID id)

{
    TYPE* lpElement = ElementTryToGet(id);

    if (lpElement != NULL) {
        ElementDelete(lpElement);
    }


}




template <typename TYPE, typename TYPEID>
TYPE* TDataArray<TYPE, TYPEID>::ElementTryToGet(TYPEID dataID) {
    if (dataID == DATAID_INVALID) {
        return NULL;
    }







    uint16 wIndex = dataID & DATA_INDEX_MASK;
    if (wIndex >= dwMaximumCount) {
        return NULL;
    }


    TYPE* ptElement = &ptData[wIndex];

    if (ptElement->id != dataID) {
        return NULL;
    }

    return ptElement;
}















































template <typename TYPE, typename TYPEID>
TYPE* TDataArray<TYPE, TYPEID>::Iterate(uint16& wIndex, TYPE*& ptElement) {
    if (wIndex == DATA_INDEX_INVALID) {
        wIndex = 0;
        ptElement = ptData;
    } else {
        ptElement++;
        wIndex++;
    }

    if (wHighestUsedIndex == DATA_INDEX_INVALID) {
        return NULL;
    }

    while (wIndex <= wHighestUsedIndex) {
        if (ptElement->id != DATAID_INVALID) {
            return ptElement;
        }

        ptElement++;
        wIndex++;
    }

    return NULL;
}


















template <typename TYPE, typename TYPEID>
TYPE* TDataArray<TYPE, TYPEID>::Iterate(TDataArrayIterator<TYPE>& tIterator) {


    return Iterate(tIterator.wIndex, tIterator.ptElement);
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
// Not checked: TDataArrayIterator (template); TDataArray (template)
static_assert(sizeof(TDataArrayThreadSafeIterator) == 0x4, "TDataArrayThreadSafeIterator");
static_assert(offsetof(TDataArrayThreadSafeIterator, nIndex) == 0x0, "TDataArrayThreadSafeIterator");
static_assert(sizeof(CDataArrayBase) == 0x11c, "CDataArrayBase");
static_assert(offsetof(CDataArrayBase, szName) == 0x0, "CDataArrayBase");
static_assert(offsetof(CDataArrayBase, dwMaximumCount) == 0x100, "CDataArrayBase");
static_assert(offsetof(CDataArrayBase, dwElementSize) == 0x104, "CDataArrayBase");
static_assert(offsetof(CDataArrayBase, wHighestUsedIndex) == 0x108, "CDataArrayBase");
static_assert(offsetof(CDataArrayBase, dwTotalCount) == 0x10c, "CDataArrayBase");
static_assert(offsetof(CDataArrayBase, wNextIdentifier) == 0x110, "CDataArrayBase");
static_assert(offsetof(CDataArrayBase, wFirstFreeItem) == 0x112, "CDataArrayBase");
static_assert(offsetof(CDataArrayBase, dwFreeCount) == 0x114, "CDataArrayBase");
static_assert(offsetof(CDataArrayBase, fGrowable) == 0x118, "CDataArrayBase");
