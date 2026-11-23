#pragma once
#include "Definitions.h"
#include "TreasureClassDefinition.h"

struct RecipeDefinition { // original size 0x48
    RequiredDefinitionHeader tHeader;
    ItemSpecifierData tSpecificItem;
    uint32 dwPad;
};
static_assert(sizeof(RecipeDefinition) == 0x48, "RecipeDefinition");
class RecipeGroupDef : public GroupDefinition {  // size 0xa0, line 23
public:
    RecipeGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13
    virtual SNOPackType GetSNOPackDestination(const void *) const {
        return SNOPACKTYPE_COREDATA;
    }  // vtable slot 7
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(RecipeGroupDef) == 0xa0, "RecipeGroupDef");
extern RecipeGroupDef * gptGroupDefRecipe;  // RecipeDefinition.cpp
template <> inline const RecipeDefinition * DefinitionLockR<RecipeDefinition>(SNO sno) {
    return (const RecipeDefinition *)((GroupDefinition *)gptGroupDefRecipe)->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<RecipeDefinition>(const RecipeDefinition *) {}
typedef ScopedDefinitionLockR<RecipeDefinition> ScopedRecipeDefinitionLockR;
