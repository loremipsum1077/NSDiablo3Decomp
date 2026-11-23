// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/TimedEventDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;
class XTypeDescriptor;

struct TimedEventDefinition {  // size 0x10
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwDuration;  // 0x0c
};
static_assert(sizeof(TimedEventDefinition) == 0x10, "TimedEventDefinition");

class TimedEventGroupDef : public GroupDefinition {  // size 0xa0, line 28
public:
    TimedEventGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 33; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 36
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 38
        return SNOPACKTYPE_SERVERDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(TimedEventGroupDef) == 0xa0, "TimedEventGroupDef");

extern const XTypeDescriptor * DT_TIMEDEVENT_DEFINITION;  // TimedEventDefinition.cpp:25
extern TimedEventGroupDef * gptGroupDefTimedEvent;  // TimedEventDefinition.cpp:30

typedef ScopedDefinitionLockR<TimedEventDefinition> ScopedTimedEventDefinitionLockR;

// Line 65. Its callers call LockRFast and have nothing from an unlock, so the unlock is taken to
// be empty.
template <> inline const TimedEventDefinition * DefinitionLockR<TimedEventDefinition>(SNO sno) {
    return (const TimedEventDefinition *)gptGroupDefTimedEvent->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<TimedEventDefinition>(const TimedEventDefinition *) {}
