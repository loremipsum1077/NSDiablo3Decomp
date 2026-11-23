// Original file: X/Common/Powers/PowersStruct.cpp

#include "PowersStruct.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../WorldPlace.h"
// For the RGBAColor normals, which the original also initializes here; it reaches GfxVB.h
// through an include chain the DWARF does not record.
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;

// Defined in X/Common/Definitions/ActorDefinition.cpp:87; no adopted header declares it yet.
extern const XTypeDescriptor * DT_ACTOR_COLLISION_FLAGS;

enum {
    POWERDEF_XML_VERSION = 3,
};

// The field names are "" in the binary.
static const TypeDescriptorField BuffFilterDef_Fields[3] = {  // line 34
    FilterTypeDescriptorField("", DT_SNO, offsetof(BuffFilterDef, snoPower), "", POWER_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(BuffFilterDef, dwRunes), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BuffFilterDef), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BuffFilterDef("BuffFilterDef", 0, BuffFilterDef_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 37
const XTypeDescriptor * DT_BUFF_FILTER_DEF = &TypeDescriptor_BuffFilterDef;

static const TypeDescriptorField BuffDef_Fields[3] = {  // line 39
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(BuffDef, serBuffFilters), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BUFF_FILTER_DEF, offsetof(BuffDef, arBuffFilters), 0x21,
                                     (int32)offsetof(BuffDef, serBuffFilters) - (int32)offsetof(BuffDef, arBuffFilters)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BuffDef), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BuffDef("BuffDef", 0, BuffDef_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 41
const XTypeDescriptor * DT_BUFF_DEF = &TypeDescriptor_BuffDef;

static const TypeDescriptorField PowerDef_Fields[27] = {  // line 43
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDef, serTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(PowerDef, hTagMap), 0x21,
                              (int32)offsetof(PowerDef, serTagMap) - (int32)offsetof(PowerDef, hTagMap), 0x40000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDef, serGlobalTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(PowerDef, hGeneralTagMap), 0x21,
                              (int32)offsetof(PowerDef, serGlobalTagMap) - (int32)offsetof(PowerDef, hGeneralTagMap), 0x130000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDef, serPVPGlobalTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(PowerDef, hPVPGeneralTagMap), 0x21,
                              (int32)offsetof(PowerDef, serPVPGlobalTagMap) - (int32)offsetof(PowerDef, hPVPGeneralTagMap), 0x130000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDef, serContactTagMap[0]), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(PowerDef, hContactTagMap[0]), 0x21,
                              (int32)offsetof(PowerDef, serContactTagMap[0]) - (int32)offsetof(PowerDef, hContactTagMap[0]), 0x20000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDef, serContactTagMap[1]), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(PowerDef, hContactTagMap[1]), 0x21,
                              (int32)offsetof(PowerDef, serContactTagMap[1]) - (int32)offsetof(PowerDef, hContactTagMap[1]), 0x20000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDef, serContactTagMap[2]), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(PowerDef, hContactTagMap[2]), 0x21,
                              (int32)offsetof(PowerDef, serContactTagMap[2]) - (int32)offsetof(PowerDef, hContactTagMap[2]), 0x20000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDef, serContactTagMap[3]), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(PowerDef, hContactTagMap[3]), 0x21,
                              (int32)offsetof(PowerDef, serContactTagMap[3]) - (int32)offsetof(PowerDef, hContactTagMap[3]), 0x20000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDef, serPVPContactTagMap[0]), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(PowerDef, hPVPContactTagMap[0]), 0x21,
                              (int32)offsetof(PowerDef, serPVPContactTagMap[0]) - (int32)offsetof(PowerDef, hPVPContactTagMap[0]), 0x20000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDef, serPVPContactTagMap[1]), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(PowerDef, hPVPContactTagMap[1]), 0x21,
                              (int32)offsetof(PowerDef, serPVPContactTagMap[1]) - (int32)offsetof(PowerDef, hPVPContactTagMap[1]), 0x20000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDef, serPVPContactTagMap[2]), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(PowerDef, hPVPContactTagMap[2]), 0x21,
                              (int32)offsetof(PowerDef, serPVPContactTagMap[2]) - (int32)offsetof(PowerDef, hPVPContactTagMap[2]), 0x20000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDef, serPVPContactTagMap[3]), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(PowerDef, hPVPContactTagMap[3]), 0x21,
                              (int32)offsetof(PowerDef, serPVPContactTagMap[3]) - (int32)offsetof(PowerDef, hPVPContactTagMap[3]), 0x20000),
    TypeDescriptorField("", DT_INT, offsetof(PowerDef, uFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_ACTOR_COLLISION_FLAGS, offsetof(PowerDef, arCollFlags[0]), "", 1),
    TypeDescriptorField("", DT_ACTOR_COLLISION_FLAGS, offsetof(PowerDef, arCollFlags[1]), "", 1),
    FixedArrayTypeDescriptorField("", DT_BUFF_DEF, offsetof(PowerDef, arBuff), NULL, 1, 32, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PowerDef), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PowerDef("PowerDef", POWERDEF_XML_VERSION, PowerDef_Fields, 27, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 59
const XTypeDescriptor * DT_POWER_DEF = &TypeDescriptor_PowerDef;
