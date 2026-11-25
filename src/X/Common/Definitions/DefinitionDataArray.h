#pragma once

// The 2.7.7 regular-definition array has 18-bit indices and per-slot generations.
// Pending definitions and all other TDataArray instantiations retain the 16-bit family.
// Iterator/free-slot overlays are reconstructed ABI representations; the base and
// tracker are separate from the unchanged 16-bit family.
#include "../../Sigma/DataStructures/DefinitionArrayTracker.h"

template <>
class TDataArray<DefDataElement, DefDataElementID> : public DefinitionDataArrayBase {
public:
    DefDataElement* ptData;
    XMemoryAllocatorInterface* ptMemoryInterface;

    void DeleteAll() {
        if (this == NULL) return;
        unk_108 = 0x3ffff;
        dwTotalCount = 0;
        unk_110 = 0;
        dwFreeCount = dwMaximumCount;
        for (uint32 index = 0; index < dwMaximumCount; ++index) {
            DefDataElement* element = &ptData[index];
            element->id = DEFDATAELEMENTID_INVALID;
            element->unk_4 = index + 1;
            element->unk_8 = XHashKey<LPCSTR>(szName) & 0x1fff;
        }
        ptData[dwMaximumCount - 1].unk_4 = 0x3ffff;
    }

    DefDataElementID ElementNew() {
        if (dwFreeCount == 0 && !sGrow()) return DEFDATAELEMENTID_INVALID;
        uint32 index = unk_110;
        DefDataElement* element = &ptData[index];
        unk_110 = element->unk_4;
        uint32 generation = element->unk_8;
        SigmaMemoryClear(element, sizeof(DefDataElement));
        new (element) DefDataElement;
        --dwFreeCount;
        element->id = (DefDataElementID)((generation << 18) | (index & 0x3ffff));
        ++dwTotalCount;
        if (unk_108 == 0x3ffff || unk_108 < index) unk_108 = index;
        return element->id;
    }

    DefDataElement* ElementGet(DefDataElementID id) { return &ptData[(uint32)id & 0x3ffff]; }
    DefDataElement* ElementTryToGet(DefDataElementID id) {
        if (id == DEFDATAELEMENTID_INVALID) return NULL;
        uint32 index = (uint32)id & 0x3ffff;
        if (index >= dwMaximumCount) return NULL;
        DefDataElement* element = &ptData[index];
        return element->id == id ? element : NULL;
    }
    int32 AvailableCount() { return dwFreeCount; }

    void ElementDelete(DefDataElement* element) {
        element->~DefDataElement();
        uint32 id = (uint32)element->id;
        uint32 index = id & 0x3ffff;
        if (element->id == DEFDATAELEMENTID_INVALID) return;
        uint32 generation = ((id >> 18) + 1) & 0x3fff;
        if (generation == 0 || generation == 0x3fff) generation = 1;
        element->id = DEFDATAELEMENTID_INVALID;
        element->unk_8 = generation;
        element->unk_4 = unk_110;
        unk_110 = index;
        ++dwFreeCount;
        if (index == unk_108) {
            do {
                --element;
                unk_108 = (unk_108 - 1) & 0x3ffff;
            } while (unk_108 != 0x3ffff && element->id == DEFDATAELEMENTID_INVALID);
        }
        --dwTotalCount;
    }
    void ElementDeleteByID(DefDataElementID id) {
        DefDataElement* element = ElementTryToGet(id);
        if (element) ElementDelete(element);
    }

    DefDataElement* Iterate(uint32& index, DefDataElement*& element) {
        if (index == 0x3ffff) { index = 0; element = ptData; }
        else { ++element; ++index; }
        if (unk_108 == 0x3ffff) return NULL;
        while (index <= unk_108) {
            if (element->id != DEFDATAELEMENTID_INVALID) return element;
            ++element;
            ++index;
        }
        return NULL;
    }
    DefDataElement* Iterate(TDataArrayIterator<DefDataElement>& iterator) {
        return Iterate(iterator.unk_0, iterator.ptElement);
    }

private:
    BOOL sGrow() {
        if (!fGrowable || dwMaximumCount >= 0x3fffe) return FALSE;
        uint32 newCount = Math::Min<uint32>(dwMaximumCount * 2, 0x3fffe);
        ptData = (DefDataElement*)SigmaMemoryResize(ptData, ptMemoryInterface,
            (uint32)(newCount * sizeof(DefDataElement)), 16);
        for (uint32 index = dwMaximumCount; index < newCount; ++index) {
            DefDataElement* element = &ptData[index];
            element->id = DEFDATAELEMENTID_INVALID;
            element->unk_4 = index + 1;
            element->unk_8 = 1;
        }
        ptData[newCount - 1].unk_4 = 0x3ffff;
        unk_110 = dwMaximumCount;
        dwFreeCount = newCount - dwMaximumCount;
        dwMaximumCount = newCount;
        return TRUE;
    }
};

static_assert(sizeof(TDataArray<DefDataElement, DefDataElementID>) == 0x130, "18-bit definition array");
static_assert(__builtin_offsetof(DefinitionDataArrayBase, unk_108) == 0x108, "18-bit highest index");
static_assert(__builtin_offsetof(DefinitionDataArrayBase, unk_110) == 0x110, "18-bit first-free index");

template <>
inline TDataArray<DefDataElement, DefDataElementID>* TDataArrayCreateTracked<DefDataElement, DefDataElementID>(
    const CHAR* name, XMemoryAllocatorInterface* allocator, uint32 maximumCount) {
    typedef TDataArray<DefDataElement, DefDataElementID> Array;
    Array* array = (Array*)SigmaMemoryNew(sizeof(Array), 4, allocator, TRUE);
    Blizzard::String::Copy(array->szName, name, MAXIMUM_DATA_ARRAY_NAME_LENGTH);
    array->dwMaximumCount = maximumCount;
    array->ptData = (DefDataElement*)SigmaMemoryNew(maximumCount * sizeof(DefDataElement), 16, allocator, FALSE);
    array->dwElementSize = sizeof(DefDataElement);
    array->ptMemoryInterface = allocator;
    array->DeleteAll();
    unk_7101a46c30->AddTail_Tracked(array);
    return array;
}

template <>
inline void TDataArrayDispose<DefDataElement, DefDataElementID>(TDataArray<DefDataElement, DefDataElementID>*& array) {
    if (array != NULL) {
        POS pos = unk_7101a46c30->Find(array);
        if (pos != NULL) unk_7101a46c30->RemoveAt(pos);
        XFREE(array->ptData, array->ptMemoryInterface);
        XFREE(array, array->ptMemoryInterface);
    }
}
