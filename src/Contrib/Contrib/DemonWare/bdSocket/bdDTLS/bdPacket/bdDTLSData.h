// Adopted from authenticated original DWARF; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSData.h

#pragma once

#include "../../../bdCore/bdCrypto/bdCypherSuites.h"
#include "../../../bdCore/bdMemory/bdMemory-std.h"
#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdDTLSHeader.h"
#include <stddef.h>
#include <stdint.h>

class bdCypher;
class bdHash;
class bdSequenceNumber;

class bdDTLSData : public bdDTLSHeader {  // size 0x20
public:
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void operator delete(void *, void *);
    static void operator delete[](void *);
    static void operator delete[](void *, void *);
    static void * operator new(size_t);
    static void * operator new(size_t, void *);
    static void * operator new[](size_t);
    static void * operator new[](size_t, void *);
    bdDTLSData();
    bdDTLSData(bdUInt16, bdUInt16);
    virtual ~bdDTLSData();  // vtable slot 0
    virtual bdBool serialize(void * outData, const bdUInt outDataSize, const bdUInt outDataOffset, bdUInt & outDataNewOffset, const bdSequenceNumber & lastSequenceNumber, const bdUByte8 (&keyingMaterials)[32], const bdUByte8 *const inData, const bdUInt inDataSize, bdCypher *const cypher, bdHash *const hash, const bdCypherSuite cypherSuite);  // vtable slot 4
    virtual bdBool deserialize(const void * inData, const bdUInt inDataSize, const bdUInt inDataOffset, bdUInt & inDataNewOffset, const bdSequenceNumber & lastSequenceNumber, const bdUByte8 (&keyingMaterials)[32], bdUByte8 * outData, const bdUInt outDataMaxSize, bdUInt & outDataSize, bdCypher *const cypher, bdHash *const hash, const bdCypherSuite cypherSuite);  // vtable slot 5
protected:
    bdBool setMACSize(const bdCypherSuite cypherSuite);
    bdUInt getPaddedDataSize(const bdCypherSuite cypherSuite, const bdUInt encryptedDataSize);
    bdBool generateIVNonce(const bdCypherSuite cypherSuite, const bdSequenceNumber & lastSequenceNumber, const bdUByte8 (&keyingMaterials)[32], bdUByte8 (&iv)[16], bdUInt & ivNonceSize, bdHash *const hash);
    bdBool generateMessageAuthCode(const bdCypherSuite cypherSuite, bdCypher *const cypher, const bdUByte8 (&keyingMaterials)[32], const bdUInt plainDataSize, const bdUInt encryptedPaddedDataSize, const bdUInt outDataOffset, bdUInt & outDataNewOffset, void * outData, bdUByte8 *const outPayloadStart);
    bdBool processMessageAuthCode(const bdCypherSuite cypherSuite, bdCypher *const cypher, const bdUByte8 (&keyingMaterials)[32], const bdUInt plainDataSize, const bdUInt paddedEncryptedDataSize, const bdUInt inDataOffset, bdUInt & inDataNewOffset, bdUInt & outDataSize, const void * inData, const bdUByte8 *const inPayloadStart);
    bdBool setAdditionalAuthData(const bdCypherSuite cypherSuite, bdCypher *const cypher, bdUByte8 (&aadBuffer)[1288], const bdUInt16 encryptedDataSize, const bdUInt plainDataSize, const bdUByte8 *const plainData);
    virtual bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;  // vtable slot 2
    virtual bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset);  // vtable slot 3
    bdUByte8 m_mac[12];  // 0x0e
    bdUInt m_macSize;  // 0x1c
    bdDTLSData(const bdDTLSData &);
    bdDTLSData & operator=(const bdDTLSData &);
};
static_assert(sizeof(bdDTLSData) == 0x20, "bdDTLSData");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  void bdDTLSData::operator delete(void * p)  [inlined into 1 functions]
