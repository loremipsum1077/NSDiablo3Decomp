// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMarketingComms/bdMarketingComms.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdCommsGetMessagesRequest;
class bdCommsGetMessagesResponse;
class bdCommsLocationMessages;
class bdCommsReportMessagesViewedRequest;
class bdCommsReportMessagesViewedResponse;
class bdMarketingMessage;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdMarketingComms {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdMarketingComms);
    bdMarketingComms(bdRemoteTaskManager *const, const bdNChar8 *);
    ~bdMarketingComms();
    bdRemoteTaskRef getMessages(const bdNChar8 *, bdMarketingMessage *, bdUInt32);
    bdRemoteTaskRef getMessages(const bdNChar8 *, bdCommsLocationMessages *, bdUInt32);
    bdRemoteTaskRef getMessages(bdCommsGetMessagesRequest *, bdCommsGetMessagesResponse *);
    bdRemoteTaskRef reportFullMessagesViewed(bdMarketingMessage *, bdUInt32);
    bdRemoteTaskRef reportMessagesViewed(bdCommsReportMessagesViewedRequest *, bdCommsReportMessagesViewedResponse *);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdNChar8 m_context[16];  // 0x08
};
static_assert(sizeof(bdMarketingComms) == 0x18, "bdMarketingComms");


// Functions defined in this file (bodies are not in the DWARF):
//   line    41  void bdMarketingComms::operator delete(void * p)  [inlined into 1 functions]
//   line    41  void * bdMarketingComms::operator new(size_t nbytes)  [inline]
