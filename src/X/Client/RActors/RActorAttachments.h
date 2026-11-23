// Provisional owning header for X/Client/RActors/RActorAttachments.cpp.
//
// The 2.6.2 DWARF attributes RActorGetHardpointByIndex and RActorGetHardpointTransform
// to X/Client/RActors/RActorAttachments.cpp. The full class/skeleton for this header
// has not been adopted in this checkout, so only the two entry points
// X/Client/AttachmentParent.cpp needs live here, with RActor forward declared.

#pragma once

#include "../../Common/Definitions/AttachmentDefinition.h"
#include "../../Sigma/Math/Transform.h"

struct RActor;

// X/Client/RActors/RActorAttachments.cpp
void RActorGetHardpointByIndex(const RActor & tRActor, int32 nHardpointIndex, Hardpoint & tHardpoint);
void RActorGetHardpointTransform(const RActor & tRActor, int32 nBoneIndex, const PRTransform & transformHPModelSpace, PRTransform & transform);
void RActorGetHardpointTransform(const RActor & tRActor, const Hardpoint & tHardpoint, PRTransform & transform);  // RActorAttachments.cpp:2167

// X/Client/RActors/RActorAttachments.cpp:2315
void RActorClearActorAttachment(RActor & tRActor, const uint32 dwAttachmentID);

#include "../../Common/CommonTypes.h"

typedef int32 ParentType;  // also in RActors.h:158 and EffectGroup.h (identical redeclaration)

// X/Client/RActors/RActorAttachments.cpp:849
AttachmentID RActorSendRMessage(RActor & tRActor, const TriggerEvent & tTriggerEvent, const float flImpulse, const RActorID * idRActorSources, const Hardpoint * ptDynamicHardpoints, const int32 nNumDynamicHardpoints, const float flLifetime, const BOOL bForRepeat, const float flStartOffset, ParentType eType);

struct KeyedMessage;

// X/Client/RActors/RActorAttachments.cpp:919
void RActorSendMsgKey(const RActorID idRActor, const KeyedMessage & tMsg, const MsgPassMethod eMsgPassMethod);

void RActorGetHardpointTransformByIndex(const RActor & tRActor, const int32 nHardpointIndex, PRTransform & transform);  // RActorAttachments.cpp:2248
