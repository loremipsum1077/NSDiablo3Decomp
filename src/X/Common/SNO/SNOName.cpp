// Original file: X/Common/SNO/SNOName.cpp

#include "SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"

#include "../ThreadGlobals.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp:532 and :339; no adopted header
// declares them yet.
extern const XTypeDescriptor * DT_SNO_GROUP;
extern const XTypeDescriptor * DT_SNONAME_HANDLE;

struct SNONameThreadGlobals {  // size 0x30
    TSnoNameListPool tSnoNameListPool;  // 0x00
};
static_assert(sizeof(SNONameThreadGlobals) == 0x30, "SNONameThreadGlobals");

// _GLOBAL__sub_I_SNOName.cpp also runs the header statics of the includes above (cgplaceNull,
// the four RGBAColor normals of X/Client/Gfx/GfxVB.h:351-354).
// The field names are "" in the binary.
//
//
//
//
//
static const TypeDescriptorField SNOName_Fields[3] = {  // line 20
    TypeDescriptorField("", DT_SNO_GROUP, offsetof(SNOName, snoGroup), NULL, 1),
    TypeDescriptorField("", DT_SNONAME_HANDLE, offsetof(SNOName, snoHandle), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SNOName), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SNOName("SNOName", 0, SNOName_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 23
const XTypeDescriptor * DT_SNO_NAME = &TypeDescriptor_SNOName;

inline SNONameThreadGlobals * SNONameThreadGlobalsGet() {  // line 29
    SNONameThreadGlobals *& ptSNONameThreadGlobals = ThreadGlobals::Get().m_ptSNONameThreadGlobals;

    if (ptSNONameThreadGlobals == NULL) {
        ptSNONameThreadGlobals =
            (SNONameThreadGlobals *)SigmaMemoryNew(sizeof(SNONameThreadGlobals), 4, NULL, TRUE);

        ptSNONameThreadGlobals->tSnoNameListPool.Initialize();
        ptSNONameThreadGlobals->tSnoNameListPool.SetBlockCount_Tracked(100);
    }

    return ptSNONameThreadGlobals;
}

void SNONameDisposeThreadGlobals() {  // line 48
    SNONameThreadGlobals *& ptSNONameThreadGlobals = ThreadGlobals::Get().m_ptSNONameThreadGlobals;

    if (ptSNONameThreadGlobals != NULL) {
        ptSNONameThreadGlobals->tSnoNameListPool.Dispose();
        XFREE(ptSNONameThreadGlobals, NULL);
    }
}

TSnoNameListPool * GetSnoNameListPool() {  // line 63
    SNONameThreadGlobals * ptSNONameThreadGlobals = SNONameThreadGlobalsGet();
    return &ptSNONameThreadGlobals->tSnoNameListPool;
}

void SNOName::Set(const SNOGroup _snoGroup, LPCSTR sSNOName) {  // line 74
    snoGroup = _snoGroup;
    snoHandle = SNOFileLookupGUID(_snoGroup, sSNOName);
}
