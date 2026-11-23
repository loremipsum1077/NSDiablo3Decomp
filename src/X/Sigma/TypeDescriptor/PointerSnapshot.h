// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/TypeDescriptor/PointerSnapshot.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"

class TypeDescriptorField;
class XTypeDescriptor;

class PointerSnapshot {  // size 0x30
    // vtable pointer
public:
    // The DWARF does not record access for nested types; PointerVector is used by MergeableSerializeDataInfo.
    struct Pointer {  // size 0x10
        void * pPointer;  // 0x00
        void * pPointerValue;  // 0x08
    };
    typedef blz::vector<PointerSnapshot::Pointer, blz::allocator<PointerSnapshot::Pointer> > PointerVector;
public:
    PointerSnapshot(const PointerSnapshot::PointerVector &);
    PointerSnapshot(void *, const XTypeDescriptor *);
    PointerSnapshot(void *, const XTypeDescriptor *, int32, int32);
    virtual ~PointerSnapshot();  // vtable slot 0
    void ClearAllPointers();
    void RestoreAllPointers();
    const PointerSnapshot::PointerVector & GetPointers() const;
protected:
    static void sDefinitionFieldCallback(const XTypeDescriptor *, const TypeDescriptorField *, const void *, void *);
    void StorePointer(void * pPointer, void * pPointerValue);
    PointerSnapshot::PointerVector m_tPointerValues;  // 0x08
    void * m_pDef;  // 0x20
    const XTypeDescriptor * m_pTypeDescriptor;  // 0x28
};
static_assert(sizeof(PointerSnapshot) == 0x30, "PointerSnapshot");

class ScopedPointerSnapshot : public PointerSnapshot {  // size 0x30
public:
    ScopedPointerSnapshot(const PointerSnapshot::PointerVector &);
    ScopedPointerSnapshot(void *, const XTypeDescriptor *);
    ScopedPointerSnapshot(void *, const XTypeDescriptor *, int32, int32);
    virtual ~ScopedPointerSnapshot();  // vtable slot 0
};
static_assert(sizeof(ScopedPointerSnapshot) == 0x30, "ScopedPointerSnapshot");

