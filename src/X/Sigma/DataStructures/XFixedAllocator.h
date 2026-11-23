#pragma once

#include "../SigmaTypes.h"
#include "XAllocator.h"















template <int SIZE, int COUNT>
class XFixedAllocator {
public:
    inline XFixedAllocator();
    inline ~XFixedAllocator();

    inline void Initialize();
    inline void Dispose();
    inline BOOL IsDisposed();

    inline void SetUseSystemAllocator(BOOL bValue);
    inline void SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam);

    inline void* Reserve_Tracked(int32 nSize);
    inline void* SetSize_Tracked(int32 nSize);
    inline int32 GetSize();

protected:
    uint8 m_Data[SIZE * COUNT];
};

// The members from here up to Reserve_Tracked compile to nothing and leave no
// trace in the binary; their original lines are not known.
template <int SIZE, int COUNT>
XFixedAllocator<SIZE, COUNT>::XFixedAllocator() {
}

template <int SIZE, int COUNT>
XFixedAllocator<SIZE, COUNT>::~XFixedAllocator() {
}

template <int SIZE, int COUNT>
void XFixedAllocator<SIZE, COUNT>::Initialize() {
}

template <int SIZE, int COUNT>
void XFixedAllocator<SIZE, COUNT>::Dispose() {
}

template <int SIZE, int COUNT>
BOOL XFixedAllocator<SIZE, COUNT>::IsDisposed() {
    return FALSE;
}

template <int SIZE, int COUNT>
void XFixedAllocator<SIZE, COUNT>::SetUseSystemAllocator(BOOL bValue) {
}

template <int SIZE, int COUNT>
void XFixedAllocator<SIZE, COUNT>::SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam) {
}
































































template <int SIZE, int COUNT>
void* XFixedAllocator<SIZE, COUNT>::Reserve_Tracked(int32 nSize) {










    return m_Data;
}




template <int SIZE, int COUNT>
void* XFixedAllocator<SIZE, COUNT>::SetSize_Tracked(int32 nSize) {










    return m_Data;
}

// Not instantiated anywhere in the binary; original line not known.
template <int SIZE, int COUNT>
int32 XFixedAllocator<SIZE, COUNT>::GetSize() {
    return SIZE * COUNT;
}
