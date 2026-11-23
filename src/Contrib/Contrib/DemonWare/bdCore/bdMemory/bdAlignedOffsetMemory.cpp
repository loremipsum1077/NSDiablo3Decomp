// Original file: Contrib/Contrib/DemonWare/bdCore/bdMemory/bdAlignedOffsetMemory.cpp

#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include <stdlib.h>
// bdMalloc/bdFree are bdPlatformMemory.inl lines 13 and 20 (DWARF); kept here: recovery v1 excludes .inl edits.
inline void * bdMalloc(bdUWord size) { void * const ptr = malloc(size); return ptr; }
inline void bdFree(void * p) { free(p); }






void * bdAlignedOffsetMalloc(const bdUWord size,
                             const bdUWord align,
                             const bdUWord offset)
{
    if ((align & (align - 1)) != 0) return 0;






    const bdUWord padding = align + offset + sizeof(bdUWord);
    const bdUWord blockPtr = (bdUWord)bdMalloc(size + padding);

    if (blockPtr == 0) return 0;




    const bdUWord alignedPtr = (blockPtr + padding) & ~(align - 1);
    const bdUWord dataPtr = alignedPtr - offset;


    bdUWord * const headerPtr = (bdUWord *)dataPtr - 1;
    *headerPtr = blockPtr;







    return (void *)dataPtr;
}

void bdAlignedOffsetFree(void * p)
{
    const bdUWord dataPtr = (bdUWord)p;

    const bdUWord * const headerPtr = (const bdUWord *)dataPtr - 1;
    void * const blockPtr = (void *)*headerPtr;

    bdFree(blockPtr);
}

void * bdAlignedOffsetRealloc(void * p,
                              const bdUWord origSize,
                              const bdUWord size,
                              const bdUWord align,
                              const bdUWord offset)
{
    void * const dataPtr = bdAlignedOffsetMalloc(size, align, offset);
    bdMemcpy(dataPtr, p, size < origSize ? size : origSize);
    bdAlignedOffsetFree(p);
    return dataPtr;
}
