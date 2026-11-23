// Original file: X/Common/HotbarButton.cpp

#include "HotbarButton.h"

// Not used directly: the original reaches it through an include chain, and its cgplaceNull static
// initializer is part of this file's _GLOBAL__sub_I.
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares them yet.
extern const XTypeDescriptor * DT_SNO;
extern const XTypeDescriptor * DT_GBID;

// _GLOBAL__sub_I_HotbarButton.cpp also runs the header statics of the includes above (cgplaceNull,
// the four RGBAColor normals of X/Client/Gfx/GfxVB.h:351-354).
//
//
//
static const TypeDescriptorField HotbarButtonData_Fields[5] = {  // line 19
    FilterTypeDescriptorField("", DT_SNO, offsetof(HotbarButtonData, m_snoPower), "", 29, 0, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(HotbarButtonData, m_eRuneType), NULL, (const void *)-1, (const void *)4),
    FilterTypeDescriptorField("", DT_GBID, offsetof(HotbarButtonData, m_gbidItem), "", 2, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(HotbarButtonData, m_annItem), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HotbarButtonData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HotbarButtonData("HotbarButtonData", 0, HotbarButtonData_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 24
const XTypeDescriptor * DT_HOTBARBUTTONDATA = &TypeDescriptor_HotbarButtonData;
