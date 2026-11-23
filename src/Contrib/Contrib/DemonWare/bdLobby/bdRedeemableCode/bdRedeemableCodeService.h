// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdRedeemableCode/bdRedeemableCodeService.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRedeemCodeResult;
class bdRedeemPayloadRequest;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdRedeemableCodeService {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdRedeemableCodeService);
    bdRedeemableCodeService(bdRemoteTaskManager *const, const bdNChar8 *);
    ~bdRedeemableCodeService();
    bdRemoteTaskRef redeemCode(const bdNChar8 *, const bdNChar8 *, bdRedeemCodeResult *);
    bdRemoteTaskRef redeemPayload(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, bdRedeemCodeResult *);
    bdRemoteTaskRef redeemPayloadFromComms(const bdNChar8 *, const bdUByte8 *, bdUInt32, const bdUByte8 *, bdUInt32, bdRedeemCodeResult *);
protected:
    bdNChar8 m_context[16];  // 0x00
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x10
private:
    bdRemoteTaskRef redeemPayload(const bdRedeemPayloadRequest & request, bdRedeemCodeResult * result);
public:
    static void initRedeemPayloadRequestFromComms(bdRedeemPayloadRequest * request, const bdNChar8 * context, const bdNChar8 * clientTransactionId, const bdUByte8 * payload, bdUInt32 payloadSize, const bdUByte8 * payloadSignature, bdUInt32 payloadSignatureSize);
private:
    static void copyBlobToString(bdNChar8 * stringBuffer, bdUWord stringBufferSize, const bdUByte8 * blob, bdUInt32 blobSize);
};
static_assert(sizeof(bdRedeemableCodeService) == 0x18, "bdRedeemableCodeService");


// Functions defined in this file (bodies are not in the DWARF):
//   line    30  void bdRedeemableCodeService::operator delete(void * p)  [inlined into 1 functions]
//   line    30  void * bdRedeemableCodeService::operator new(size_t nbytes)  [inline]
