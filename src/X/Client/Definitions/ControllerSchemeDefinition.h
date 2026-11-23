// Generated from the DWARF by tools/gen_headers.py; hand-maintained from here on.
// Original file: X/Client/Definitions/ControllerSchemeDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Input/Input.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;

typedef int32 InputButton;

struct ButtonBinding {  // size 0x4
    InputButton eButton;  // 0x00
};
static_assert(sizeof(ButtonBinding) == 0x4, "ButtonBinding");

struct ControlSchemeDefinition {  // size 0x1bc
    RequiredDefinitionHeader tHeader;  // 0x00
    ButtonBinding arBindings[98];  // 0x0c
    InputButton SkillSlotBindings[6];  // 0x194
    InputButton PotionQuickUse;  // 0x1ac
    InputButton TownPortalBinding;  // 0x1b0
    InputButton TargetLockBinding;  // 0x1b4
    InputButton TogglePlayerIndicatorBeam;  // 0x1b8
};
static_assert(sizeof(ControlSchemeDefinition) == 0x1bc, "ControlSchemeDefinition");

class ControlSchemeDefinitionGroupDef : public GroupDefinition {  // size 0xa0
public:
    ControlSchemeDefinitionGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5, line 103; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 106
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_CLIENTDATA; }  // vtable slot 7, line 108
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(ControlSchemeDefinitionGroupDef) == 0xa0, "ControlSchemeDefinitionGroupDef");

extern ControlSchemeDefinitionGroupDef * gptGroupDefControlScheme;  // ControllerSchemeDefinition.cpp

typedef ScopedDefinitionLockR<ControlSchemeDefinition> ScopedControlSchemeDefinitionLockR;

// Line 135. Its callers call LockRFast and have nothing from an unlock.
template <> inline const ControlSchemeDefinition * DefinitionLockR<ControlSchemeDefinition>(SNO sno) {
    return (const ControlSchemeDefinition *)gptGroupDefControlScheme->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<ControlSchemeDefinition>(const ControlSchemeDefinition *) {}

// Declared here from its definition in X/Client/Definitions/ControllerSchemeDefinition.cpp.
// The DWARF gives the signature but not where it was declared.
namespace Controller {
BOOL ActionButtonMatch(const int32 nActionIndex, const BufferedInput & tInput);  // ControllerSchemeDefinition.cpp
}  // namespace Controller
