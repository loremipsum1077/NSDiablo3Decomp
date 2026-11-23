// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Attachments.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "RMessage.h"
#include "../Common/CommonTypes.h"
#include "../Common/Definitions/AttachmentDefinition.h"
#include "../Common/SNO/SNOName.h"
#include "../Common/ThreadGlobals.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/Shape.h"

typedef int32 ParticleSystemID;

typedef int32 ParentType;

struct KeyedMessage {  // size 0xc
    KeyedMessage();
    KeyedMessage(const uint32, const float, const Shape::Material);
    uint32 dwMsgKey;  // 0x00
    float flImpulseMagnitude;  // 0x04
    Shape::Material eMaterial;  // 0x08
};
static_assert(sizeof(KeyedMessage) == 0xc, "KeyedMessage");

// line 29
inline KeyedMessage::KeyedMessage(const uint32 dwMsgKey, const float flImpulseMagnitude, const Shape::Material eMaterial) {
    this->dwMsgKey = dwMsgKey;
    this->flImpulseMagnitude = flImpulseMagnitude;
    this->eMaterial = eMaterial;
}

struct Attachment {  // size 0x3c
    AttachmentID id;  // 0x00
    DataID idAttachedObject;  // 0x04
    SNOName snoname;  // 0x08
    uint32 dwTriggerFlags;  // 0x10
    uint32 dwID;  // 0x14
    HardpointLink tHP[2];  // 0x18
    ParentType nParentType;  // 0x30
    DataID idParent;  // 0x34
    DataID idListOwner;  // 0x38
};
static_assert(sizeof(Attachment) == 0x3c, "Attachment");

struct AttachmentGlobals {  // size 0x8
    TDataArray<Attachment, int> * ptAttachments;  // 0x00
};
static_assert(sizeof(AttachmentGlobals) == 0x8, "AttachmentGlobals");

class ParentData;

// X/Client/Attachments.cpp:1636
bool ParentIsRActor(ParentType eType);

AttachmentID AttachmentCreate(const RMessage & msg, const ParentData & tParentData, const PRTransform & transform, const float flScale, const BOOL fOverrideScale);
void AttachmentFindAndDelete(XPooledDataIDList & tAttachmentList, const uint32 dwID, const BOOL fOutro);
void AttachmentFindAndDetach(XPooledDataIDList & tAttachmentList, const uint32 dwID);
void AttachmentListHandleParentDeletion(XPooledDataIDList & tAttachmentList);
void AttachmentListSendMsgToChildren(XPooledDataIDList & tAttachmentList, const KeyedMessage & tMsg);  // Attachments.cpp:1376


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  KeyedMessage::KeyedMessage(const uint32 dwMsgKey, const float flImpulseMagnitude, const Shape::Material eMaterial)  [inlined into 21 functions]
//   line    44  Attachment::Attachment()  [inlined into 1 functions]
