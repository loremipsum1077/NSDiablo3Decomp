// Original file: X/Common/TimedEvents.cpp

#include "TimedEvents.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the four RGBAColor normals of X/Client/Gfx/GfxVB.h:351-354,
// whose initializers the original runs in this file's static initializer (the DWARF inline tree
// of _GLOBAL__sub_I_TimedEvents.cpp lists them, ahead of this file's own statics).
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;
// Defined in X/Common/Networking/GameMessage.cpp:65; no adopted header declares it yet.
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER;

static const TypeDescriptorField ActiveEvent_Fields[5] = {  // line 14
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActiveEvent, snoTimedEvent), "", TIMEDEVENT_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveEvent, dwStartTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveEvent, dwExpirationTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActiveEvent, dwArtificiallyElapsedTime), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActiveEvent), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActiveEvent("ActiveEvent", 0, ActiveEvent_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 19
const XTypeDescriptor * DT_ACTIVE_EVENT = &TypeDescriptor_ActiveEvent;

static const TypeDescriptorField TimedEventStartedMessage_Fields[3] = {  // line 21
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(TimedEventStartedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_ACTIVE_EVENT, offsetof(TimedEventStartedMessage, tEvent), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TimedEventStartedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TimedEventStartedMessage("TimedEventStartedMessage", 0, TimedEventStartedMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 24
const XTypeDescriptor * DT_TIMED_EVENT_STARTED_MESSAGE = &TypeDescriptor_TimedEventStartedMessage;
