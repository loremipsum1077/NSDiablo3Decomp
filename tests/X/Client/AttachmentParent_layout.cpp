// Layout check for X/Client/AttachmentParent.h against the DWARF (ParentData: byte_size 0x2c,
// members at 0x00/0x04/0x08/0x0c/0x28), and a replay of the binary's ParentData::Init
// (X/Client/AttachmentParent.cpp:33, 0x710033f470) and SetKnot (line 424, 0x710033fc50),
// which inline ParentData::Set, so try_file.py compares the header's Set body. The parent-type
// values are literals here: the ParentType enumerators are not in the DWARF.
#include "X/Client/AttachmentParent.h"

#include <cstddef>

static_assert(sizeof(ParentData) == 0x2c, "ParentData");
static_assert(offsetof(ParentData, m_eParentType) == 0x00, "m_eParentType");
static_assert(offsetof(ParentData, m_idParent) == 0x04, "m_idParent");
static_assert(offsetof(ParentData, m_nBoneIndex) == 0x08, "m_nBoneIndex");
static_assert(offsetof(ParentData, m_transform) == 0x0c, "m_transform");
static_assert(offsetof(ParentData, m_flAttachmentScaleFactor) == 0x28, "m_flAttachmentScaleFactor");

void ParentData::Init()
{
    Set(-1, -1, -1, cgtPRTransformIdentity, 1.0f);
}

void ParentData::SetKnot(const RopeID idParent, const int32 nKnotIndex)
{
    Set(3, idParent, nKnotIndex, cgtPRTransformIdentity, 1.0f);
}
