// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdHTTP/bdHTTP.h

#pragma once

#include "../bdCore/bdMemory/bdMemory-std.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdDownloadInterceptor;
class bdUploadInterceptor;

class bdHTTP {  // size 0x10
public:
    BD_CLASS_ALLOCATOR(bdHTTP)
    enum bdOperation {
        BD_NONE = 0,
        BD_GET = 1,
        BD_HEAD = 2,
        BD_POST = 3,
        BD_PUT = 4,
        BD_DELETE = 5,
        BD_COPY = 6,
    };
    enum bdStatus {
        BD_UNINITIALIZED = 0,
        BD_EMPTY = 1,
        BD_INITIALIZING = 2,
        BD_PENDING = 3,
        BD_ABORTING = 4,
        BD_DONE = 5,
        BD_FAILED = 6,
        BD_ABORTED = 7,
        BD_MAX_STATUS = 8,
    };
    bdHTTP();
    virtual ~bdHTTP() {}  // vtable slot 0; line 82
    virtual bdBool initRequest(bdHTTP::bdOperation, const bdNChar8 *const, const bdUInt32) = 0;  // vtable slot 2
    virtual bdBool setHeader(const bdNChar8 *, const bdNChar8 *) = 0;  // vtable slot 3
    virtual bdBool setHeader(const bdNChar8 * key, const bdUInt64 value);  // vtable slot 4
    virtual bdBool getHeader(const bdNChar8 *, bdNChar8 *, const bdUInt32) = 0;  // vtable slot 5
    virtual bdBool setUploadData(const bdByte8 *, bdUInt32) = 0;  // vtable slot 6
    bdBool setUploadData(const bdUByte8 *, bdUInt32);
    bdBool setUploadData(const bdNChar8 * buffer, bdUInt32 size);
    virtual bdBool setUploadHandler(bdUploadInterceptor *, bdUInt32) = 0;  // vtable slot 7
    virtual bdBool setDownloadBuffer(bdByte8 *, bdUInt32) = 0;  // vtable slot 8
    bdBool setDownloadBuffer(bdUByte8 *, bdUInt32);
    bdBool setDownloadBuffer(bdNChar8 * buffer, bdUInt32 size);
    virtual bdBool setDownloadHandler(bdDownloadInterceptor *) = 0;  // vtable slot 9
    virtual bdBool setTimeout(bdUInt32) = 0;  // vtable slot 10
    virtual bdHTTP::bdStatus sendRequest() = 0;  // vtable slot 11
    virtual bdHTTP::bdStatus getStatus() = 0;  // vtable slot 12
    virtual bdBool getResponseLength(bdUInt32 &) = 0;  // vtable slot 13
    virtual bdUInt32 getLastHTTPStatus() const = 0;  // vtable slot 14
    virtual bdInt32 getInternalError() const = 0;  // vtable slot 15
    virtual void abortOperation() = 0;  // vtable slot 16
    virtual void finalize() = 0;  // vtable slot 17
    virtual bdUInt32 getUploadProgress() const = 0;  // vtable slot 18
    virtual bdUInt32 getDownloadProgress() const = 0;  // vtable slot 19
    virtual bdFloat32 getUploadRate() const = 0;  // vtable slot 20
    virtual bdFloat32 getDownloadRate() const = 0;  // vtable slot 21
    virtual bdBool clearResult() = 0;  // vtable slot 22
    enum bdRequestState {
        BD_IDLE = 0,
        BD_CREATING = 1,
        BD_RUNNING = 2,
        BD_STOPPING = 3,
        BD_FINISHED = 4,
        BD_DESTROYED = 5,
    };
protected:
    virtual bdBool isState(bdHTTP::bdRequestState state) const;  // vtable slot 23
    bdHTTP::bdStatus m_status;  // 0x08
    bdUInt32 m_timeout;  // 0x0c
};
static_assert(sizeof(bdHTTP) == 0x10, "bdHTTP");

