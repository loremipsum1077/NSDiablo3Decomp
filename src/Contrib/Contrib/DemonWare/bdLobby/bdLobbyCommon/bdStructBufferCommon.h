// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdStructBufferCommon.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdStructSerializationStream {  // size 0x10
public:
    bdStructSerializationStream();
    ~bdStructSerializationStream();
    bdBool prepare(bdByteBufferRef buffer, bdUInt32 bufferSize);
    bdByteBufferRef getBuffer();
    bdUInt32 getBufferRemaining();
    bdBool isEOF();
protected:
    bdByteBufferRef m_buffer;  // 0x00
    bdUInt32 m_bufferSize;  // 0x08
};
static_assert(sizeof(bdStructSerializationStream) == 0x10, "bdStructSerializationStream");

class bdStructSerializationOutputStream : public bdStructSerializationStream {  // size 0x10
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdStructSerializationOutputStream();
    ~bdStructSerializationOutputStream();
    bdBool prepare(bdByteBufferRef, bdUInt32);
    bdUInt32 getWrittenSize();
    bdUInt32 write(const void *, bdUInt32);
    bdUInt32 write(bdUInt32 byteCount);
protected:
    bdUInt32 m_bufferWritten;  // 0x0c
};
static_assert(sizeof(bdStructSerializationOutputStream) == 0x10, "bdStructSerializationOutputStream");

class bdStructSerializationInputStream : public bdStructSerializationStream {  // size 0x10
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdStructSerializationInputStream();
    ~bdStructSerializationInputStream();
    bdBool prepare(bdByteBufferRef buffer, bdUInt32 bufferSize);
    bdUInt32 read(void * value, bdUInt32 byteCount);
    bdUInt32 read(bdUInt32 byteCount);
};
static_assert(sizeof(bdStructSerializationInputStream) == 0x10, "bdStructSerializationInputStream");

inline bdStructSerializationStream::~bdStructSerializationStream()
{
}

inline bdStructSerializationStream::bdStructSerializationStream()
    : m_buffer(NULL), m_bufferSize(0)
{
}

inline bdBool bdStructSerializationStream::prepare(bdByteBufferRef buffer, bdUInt32 bufferSize)
{
    m_buffer = buffer;
    m_bufferSize = bufferSize;
    return true;
}

inline bdByteBufferRef bdStructSerializationStream::getBuffer()
{
    return m_buffer;
}

inline bdUInt32 bdStructSerializationStream::getBufferRemaining()
{
    return m_bufferSize;
}

inline bdBool bdStructSerializationStream::isEOF()
{
    return m_bufferSize == 0;
}


// Functions defined in this file (bodies are not in the DWARF):
//   line    23  bdStructSerializationStream::~bdStructSerializationStream()  [inlined into 1 functions]
//   line    26  bdStructSerializationStream::bdStructSerializationStream()  [inlined into 1 functions]
//   line    35  bdBool bdStructSerializationStream::prepare(bdByteBufferRef buffer, bdUInt32 bufferSize)  [inlined into 1 functions]
//   line    44  bdByteBufferRef bdStructSerializationStream::getBuffer()  [inlined into 1 functions]
//   line    51  bdUInt32 bdStructSerializationStream::getBufferRemaining()  [inlined into 2 functions]
//   line    58  bdBool bdStructSerializationStream::isEOF()  [inlined into 3 functions]
