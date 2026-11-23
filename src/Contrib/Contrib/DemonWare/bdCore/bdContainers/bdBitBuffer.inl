// Original bdBitBuffer.inl; bodies reconstructed using DWARF inline trees.
#pragma once
#include "../bdUtilities/bdBitOperations.h"
inline bdUInt bdBitBuffer::getDataSize() const
{
    return m_data.getSize();
}
inline bdUInt bdBitBuffer::getNumBitsWritten() const {
    bdUInt numBitsWritten = m_maxWritePosition;
    return numBitsWritten ? numBitsWritten - 1 : 0;
}
inline bdUByte8* bdBitBuffer::getData()
{
    return m_data.begin();
}
inline bdUInt bdBitBuffer::getWritePosition() const
{
    return m_writePosition;
}
inline bdUInt bdBitBuffer::getReadPosition() const
{
    return m_readPosition;
}
inline void bdBitBuffer::setReadPosition(const bdUInt bitPosition) {
    m_readPosition = bitPosition > 1 ? bitPosition : 1;
}
inline void bdBitBuffer::resetReadPosition()
{
    m_readPosition = 1;
}
inline void bdBitBuffer::writeInt32(const bdInt32 i) {
    writeDataType(BD_BB_SIGNED_INTEGER32_TYPE);
    bdInt32 ni;
    bdBitOperations::endianSwap(i, ni);
    writeBits(&ni, 32);
}
inline void bdBitBuffer::writeUInt32(const bdUInt32 u) {
    writeDataType(BD_BB_UNSIGNED_INTEGER32_TYPE);
    bdUInt32 nu;
    bdBitOperations::endianSwap(u, nu);
    writeBits(&nu, 32);
}
inline bdBool bdBitBuffer::readInt32(bdInt &i) {
    bdBool ok = readDataType(BD_BB_SIGNED_INTEGER32_TYPE);
    bdInt32 ni;
    ok = ok && readBits(&ni, 32);
    if (ok) bdBitOperations::endianSwap(ni, i);
    return ok;
}
inline bdBool bdBitBuffer::readUInt32(bdUInt &u) {
    bdBool ok = readDataType(BD_BB_UNSIGNED_INTEGER32_TYPE);
    bdUInt32 nu;
    ok = ok && readBits(&nu, 32);
    if (ok) bdBitOperations::endianSwap(nu, u);
    return ok;
}

// bdBitBuffer.inl lines 124 and 283 (bodies reconstructed from bdLANDiscoveryServer::update).
inline void bdBitBuffer::writeUByte8(const bdUByte8 b)
{
    writeDataType(BD_BB_UNSIGNED_CHAR8_TYPE);
    writeBits(&b, 8);
}
inline bdBool bdBitBuffer::readUByte8(bdUByte8 & b)
{
    bdBool ok = readDataType(BD_BB_UNSIGNED_CHAR8_TYPE);
    ok = ok && readBits(&b, 8);
    return ok;
}

// bdBitBuffer.inl:93, 203, 233, 402 and 416. Written from the copies inlined into
// X/Client/Console/Online.cpp (GamePresence/UserPresence/GameInfo Serialize and Deserialize).
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString-unix.inl"

inline bdBool bdBitBuffer::writeBool(const bdBool b)
{
    writeDataType(BD_BB_BOOL_TYPE);
    if (b)
    {
        bdUByte8 byte = 0xFF;
        writeBits(&byte, 1);
    }
    else
    {
        bdUByte8 byte = 0;
        writeBits(&byte, 1);
    }
    return true;
}

inline void bdBitBuffer::writeString(const bdNChar8 *const s, const bdUWord maxLen)
{
    writeDataType(BD_BB_SIGNED_CHAR8_STRING_TYPE);
    const bdUWord length = bdStrnlen(s, maxLen);
    writeBits(s, static_cast<bdUInt>(length * 8));
    bdNChar8 null = '\0';
    writeBits(&null, 8);
}

inline bdBool bdBitBuffer::readBool(bdBool & b)
{
    bdBool ok = readDataType(BD_BB_BOOL_TYPE);
    if (ok)
    {
        bdUByte8 byte = 0;
        ok = readBits(&byte, 1);
        if (ok)
        {
            b = byte != 0;
        }
    }
    return ok;
}

inline bdBool bdBitBuffer::readString(bdNChar8 *const s, const bdUWord maxLen, bdUWord & length)
{
    bdBool ok = readDataType(BD_BB_SIGNED_CHAR8_STRING_TYPE);
    if (ok)
    {
        bdUInt i = 0;
        bdBool more;
        do
        {
            bdNChar8 c = '\0';
            ok = readBits(&c, 8);
            if (i < maxLen)
            {
                s[i] = c;
            }
            more = ok && c != '\0';
            i += more;
        } while (more);
        s[maxLen - 1] = '\0';
        length = i;
    }
    return ok;
}

inline bdBool bdBitBuffer::readString(bdNChar8 *const s, const bdUWord maxLen)
{
    bdUWord length = 0;  // initialiser: DWARF records length as const 0 in the copies inlined into Online.cpp
    bdBool ok = readString(s, maxLen, length);
    if (ok & (length >= maxLen))  // bitwise: the original computes cmp/cset/and for this test (bdBitBuffer.inl:408 in the copies inlined into Online.cpp)
    {
        bdLogWarn("bdBitBuffer", "string truncated", length + 1, maxLen);
    }
    return ok;
}

// Written from the copies inlined into X/Client/Console/ConsolePartyMessages.cpp.
#include "../bdString/bdString.h"
inline void bdBitBuffer::writeByte8(const bdByte8 b)
{
    writeDataType(BD_BB_SIGNED_CHAR8_TYPE);
    writeBits(&b, 8);
}
inline bdBool bdBitBuffer::readByte8(bdByte8 & b)
{
    bdBool ok = readDataType(BD_BB_SIGNED_CHAR8_TYPE);
    ok = ok && readBits(&b, 8);
    return ok;
}
inline void bdBitBuffer::writeUInt64(const bdUInt64 u)
{
    writeDataType(BD_BB_UNSIGNED_INTEGER64_TYPE);
    bdUInt64 nu;
    bdBitOperations::endianSwap(u, nu);
    writeBits(&nu, 64);
}
inline bdBool bdBitBuffer::readUInt64(bdUInt64 & u)
{
    bdBool ok = readDataType(BD_BB_UNSIGNED_INTEGER64_TYPE);
    bdUInt64 nu;
    ok = ok && readBits(&nu, 64);
    if (ok) bdBitOperations::endianSwap(nu, u);
    return ok;
}
inline void bdBitBuffer::writeString(const bdString & s)
{
    writeDataType(BD_BB_SIGNED_CHAR8_STRING_TYPE);
    writeBits(s.getBuffer(), (s.getLength() + 1) * 8);
}

#line 167 "Contrib/Contrib/DemonWare/bdCore/bdContainers/bdBitBuffer.inl"
inline void bdBitBuffer::writeInt64(const bdInt64 i)
{
    writeDataType(BD_BB_SIGNED_INTEGER64_TYPE);
    bdInt64 ni;
    bdBitOperations::endianSwap(i, ni);
    writeBits(&ni, 64);
}
#line 343 "Contrib/Contrib/DemonWare/bdCore/bdContainers/bdBitBuffer.inl"
inline bdBool bdBitBuffer::readInt64(bdInt64 & i)
{
    bdBool ok = readDataType(BD_BB_SIGNED_INTEGER64_TYPE);
    bdInt64 ni;
    ok = ok && readBits(&ni, 64);
    if (ok)
    {
        bdBitOperations::endianSwap(ni, i);
    }
    return ok;
}
