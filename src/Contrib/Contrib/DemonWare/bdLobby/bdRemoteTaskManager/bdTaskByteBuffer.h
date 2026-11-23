// Adopted from the DWARF; constant values recovered from setupTaskData callers.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdRemoteTaskManager/bdTaskByteBuffer.h

#pragma once

#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdTaskByteBuffer : public bdByteBuffer {  // size 0x48
public:
    BD_CLASS_ALLOCATOR(bdTaskByteBuffer);
    bdTaskByteBuffer(const bdUInt, bdBool);
    bdTaskByteBuffer(const void *, const bdUInt, bdBool);
    virtual ~bdTaskByteBuffer();  // vtable slot 0
    virtual void allocateBuffer();  // vtable slot 2
    bdUInt getHeaderSize() const;
    void setHeaderSize(bdUInt size);
    bdUByte8 * getHeaderStart() const;
    bdUInt getPaddingSize() const;
    bdUByte8 * getPaddingStart() const;
    void adjustPayload(void * payloadStart, bdUInt payloadSize);
    // DWARF bdTaskByteBuffer.h:75 (inlined into bdLobbyConnection; body: the m_taskData load).
    bdUByte8 * getStartOfTaskDataBuffer()
    {
        return m_taskData;
    }
    // DWARF bdTaskByteBuffer.h:81 (inlined; body: the m_taskDataSize load).
    bdUInt getSizeOfTaskDataBuffer() const
    {
        return m_taskDataSize;
    }
    virtual bdBool expand(bdUInt increaseSize);  // vtable slot 3
    virtual void resizeToFit(bdUInt32 newDataSize);  // vtable slot 4
protected:
    void setupTaskData(bdUInt size);
    static const bdUInt maxHeaderSize = 31;
    static const bdUInt signatureSize = 24;
    bdUByte8 * m_taskData;  // 0x30
    bdUInt m_taskDataSize;  // 0x38
    bdUInt m_validHeaderSize;  // 0x3c
    bdUInt m_paddingSize;  // 0x40
};
static_assert(sizeof(bdTaskByteBuffer) == 0x48, "bdTaskByteBuffer");

typedef bdReference<bdTaskByteBuffer> bdTaskByteBufferRef;


// Functions defined in this file (bodies are not in the DWARF):
//   line    26  void bdTaskByteBuffer::operator delete(void * p)  [inlined into 1 functions]
//   line    26  void * bdTaskByteBuffer::operator new(size_t nbytes)  [inlined into 18 functions]
//   line    75  bdUByte8 * bdTaskByteBuffer::getStartOfTaskDataBuffer()  [inlined into 2 functions]
//   line    81  bdUInt bdTaskByteBuffer::getSizeOfTaskDataBuffer() const  [inlined into 1 functions]
