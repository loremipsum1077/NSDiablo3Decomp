// Original file: X/Common/Definitions/RecipeDefinition.cpp

#include "RecipeDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Common/Definitions/TreasureClassDefinition.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_ITEMSPECIFIERDATA;

enum {
    RECIPE_XML_VERSION = 3,
    RECIPE_SNO_COMMON_VERSION = 35,
    RECIPE_SNO_PC_VERSION = 35,
    RECIPE_SNO_CONSOLE_VERSION = 36,
    RECIPE_SNO_AXE_VERSION = 36,
};

// The field names are "" in the binary.
static const TypeDescriptorField RecipeDefinition_Fields[3] = {  // line 32
    TypeDescriptorField("", DT_ITEMSPECIFIERDATA, offsetof(RecipeDefinition, tSpecificItem), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RecipeDefinition, dwPad), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RecipeDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RecipeDefinition("Recipe", RECIPE_XML_VERSION, RecipeDefinition_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 36
const XTypeDescriptor * DT_RECIPE_DEFINITION = &TypeDescriptor_RecipeDefinition;

RecipeGroupDef * gptGroupDefRecipe;  // line 41

// line 46
RecipeGroupDef::RecipeGroupDef()
    : GroupDefinition("Recipe", RECIPE_SNO,
                      SNOVersions(RECIPE_SNO_COMMON_VERSION, 38, RECIPE_SNO_CONSOLE_VERSION, RECIPE_SNO_AXE_VERSION, 37),
                      SNODATATYPE_TITLE, sizeof(RecipeDefinition), DT_RECIPE_DEFINITION, 0x4005, 14) {
}

// line 66
uint32 RecipeGroupDef::GetMaxCount() {
    return 340;
}

// line 77
void RecipeGroupDef::GetDefaultData(void * ptDefaultDef) const {
    RecipeDefinition * pdef = (RecipeDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdef, sizeof(RecipeDefinition));

    pdef->tSpecificItem.gbidItem = (GBID)-1;
    pdef->tSpecificItem.nNumAffixes = 0;
    pdef->tSpecificItem.nAdditionalRandomAffixes = 0;
    pdef->tSpecificItem.nAdditionalRandomAffixesDelta = 0;
    pdef->tSpecificItem.nMinLegendaryAffixes = 0;
    pdef->tSpecificItem.bAccountBound = FALSE;
    pdef->tSpecificItem.nAdditionalMajorAffixes = 0;
    pdef->tSpecificItem.nAdditionalMinorAffixes = 0;
}
