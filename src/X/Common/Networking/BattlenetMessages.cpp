// Original file: X/Common/Networking/BattlenetMessages.cpp

// blz/tuple.h for blz::ignore, which sits at the front of the merged statics block. It must precede
// BattlenetMessages.h, whose include chain reaches SNONAME_NONE (the original's include chain had
// blz/tuple.h first too).
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

#include "BattlenetMessages.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// For the eXLocale bounds below (XLOCALE_LOCALE_START, NUM_XLOCALES).
#include "../../Sigma/UnicodeText.h"

#include <stddef.h>

// Defined in X/Common/Networking/GameMessage.cpp:65; no adopted header declares it yet.
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER;

// The field names are "" in the binary.
static const TypeDescriptorField JoinBNetGameMessage_Fields[8] = {  // line 18
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(JoinBNetGameMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(JoinBNetGameMessage, uHeroId), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(JoinBNetGameMessage, idSGame), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(JoinBNetGameMessage, uAuthToken), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(JoinBNetGameMessage, eXLocale), NULL, (const void *)XLOCALE_LOCALE_START, (const void *)(NUM_XLOCALES - 1)),
    TypeDescriptorField("", DT_INT, offsetof(JoinBNetGameMessage, uProtocolHash), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(JoinBNetGameMessage, dwSNOCoreDataHash), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(JoinBNetGameMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_JoinBNetGameMessage("JoinBNetGameMessage", 0, JoinBNetGameMessage_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 26
const XTypeDescriptor * DT_JOIN_BNET_GAME_MESSAGE = &TypeDescriptor_JoinBNetGameMessage;
