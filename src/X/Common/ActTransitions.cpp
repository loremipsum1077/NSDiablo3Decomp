// Original file: X/Common/ActTransitions.cpp

#include "ActTransitions.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the four RGBAColor normals of X/Client/Gfx/GfxVB.h:351-354,
// whose initializers the original runs in this file's static initializer (the DWARF inline tree
// of _GLOBAL__sub_I_ActTransitions.cpp lists them, ahead of this file's own two statics).
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Common/Networking/GameMessage.cpp:65; no adopted header declares it yet.
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER;

static const TypeDescriptorField ActTransitionStartedMessage_Fields[5] = {  // line 14
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ActTransitionStartedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActTransitionStartedMessage, dwActTransitionTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActTransitionStartedMessage, bRecreateGameWithParty), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActTransitionStartedMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActTransitionStartedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActTransitionStartedMessage("ActTransitionStartedMessage", 0, ActTransitionStartedMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 19
const XTypeDescriptor * DT_ACT_TRANSITION_STARTED_MESSAGE = &TypeDescriptor_ActTransitionStartedMessage;
