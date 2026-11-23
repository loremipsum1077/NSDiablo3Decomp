// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/RMessage.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/Definitions/AttachmentDefinition.h"
#include "../Common/ThreadGlobals.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/SigmaTypes.h"

enum {
    PARENT_NONE = -1,
    PARENT_RACTOR = 0,
    PARENT_PARTICLE_SYSTEM = 1,
    PARENT_PARTICLE = 2,
    PARENT_ROPE = 3,
    PARENT_SCENE = 4,
    PARENT_RACTOR_TOP = 5,
    PARENT_LOCAL_PLAYER_CENTROID = 6,
    NUM_PARENT_TYPES = 7,
};

typedef int32 ParentType;

struct RMessage {  // size 0x128
    float flDelay;  // 0x00
    float flStartOffset;  // 0x04
    TriggerEvent tTriggerEvent;  // 0x08
    float flImpulse;  // 0xc4
    Hardpoint tHardpoints[2];  // 0xc8
    ParentType nParentType;  // 0x118
    DataID idParent;  // 0x11c
    float flExpireTime;  // 0x120
    BOOL bRemote;  // 0x124
    BOOL Initialize(const TriggerEvent & _tTriggerEvent, const float _flImpulse, const ParentType _eParentType, const DataID _idParent, const float _flExpireTime, const BOOL _bForRepeat, const BOOL _bRemote, const float _flStartOffset);
};
static_assert(sizeof(RMessage) == 0x128, "RMessage");

typedef void (*RMessageHandlerFunc)(const DataID, RMessage &);

class RMessageList {  // size 0x38
public:
    void Initialize(const DataID _idOwner, RMessageHandlerFunc _pfnHandleMsg);
    void Dispose();
    void DeleteAll();
    void DeleteforEventID(uint32 dwID);
    void DeleteforFlags(uint32 dwFlags);
    void Update(float flDeltaTime);
    void SendMessage(const ParentType nParentType, const DataID idParent, const BOOL bForRepeat, const TriggerEvent & tTriggerEvent, const float flImpulse, const BOOL bRemote);
    void QueueDelayedMessage(RMessage & msg);
    int32 Count() const;
protected:
    RMessageHandlerFunc pfnHandleMsg;  // 0x00
    XPooledDataIDList tList;  // 0x08
    DataID idOwner;  // 0x30
};
static_assert(sizeof(RMessageList) == 0x38, "RMessageList");


// Functions defined in this file (bodies are not in the DWARF):
//   line    33  RMessage::RMessage()  [inlined into 6 functions]
//   line    33  RMessage & RMessage::operator=(const RMessage &)  [inlined into 1 functions]
//   line    67  RMessageList::RMessageList()  [inlined into 4 functions]
//   line    67  RMessageList::~RMessageList()  [inlined into 4 functions]
//   line   100  int32 RMessageList::Count() const  [inlined into 2 functions]

#line 100
inline int32 RMessageList::Count() const
{

    return tList.Count();
}
