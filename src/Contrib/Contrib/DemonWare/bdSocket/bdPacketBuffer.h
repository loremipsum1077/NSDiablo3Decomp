// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdPacketBuffer.h

#pragma once

#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAddr;
class bdDTLSAssociation;

class bdPacketBufferInternal {  // size 0x408
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdPacketBufferInternal();
    bdBool bufferPacket(void * data, bdUInt length);
    void clearBuffer();
    bdInt flushBuffer(bdDTLSAssociation * dtlsSocket, const bdAddr & realAddr);
    bdUInt getKey();
    void setKey(bdUInt key);
private:
    bdByte8 m_buffer[1024];  // 0x00
    bdUInt m_offset;  // 0x400
    bdUInt m_key;  // 0x404
};
static_assert(sizeof(bdPacketBufferInternal) == 0x408, "bdPacketBufferInternal");

class bdPacketBuffer {  // size 0x2854
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdPacketBuffer();
    bdBool bufferPacket(bdUInt key, void * data, bdUInt length);
    void clearBuffer(bdUInt key);
    void clearAll();
    bdInt flushBuffer(bdUInt key, bdDTLSAssociation * dtlsSocket, const bdAddr & realAddr);
private:
    bdPacketBufferInternal * findBuffer(bdUInt key, bdBool findEmpty);
    bdPacketBufferInternal m_buffers[10];  // 0x00
    bdUInt32 m_numActiveBuffers;  // 0x2850
};
static_assert(sizeof(bdPacketBuffer) == 0x2854, "bdPacketBuffer");

