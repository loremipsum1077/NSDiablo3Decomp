#include "bdSecurityID.h"
#include "../bdCrypto/bdCryptoUtils.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
// Blank lines keep the original line numbers













bdSecurityID::bdSecurityID()
{
    bdMemset(ab, 1, sizeof(ab));
}

bdSecurityID::bdSecurityID(const bdSecurityID& other)
{
    bdMemcpy(ab, other.ab, sizeof(ab));
}

bdSecurityID::~bdSecurityID()
{
    bdCryptoUtils::zeroBuffer(ab, sizeof(ab));
}

bdBool bdSecurityID::operator==(const bdSecurityID& other) const
{
    return bdCryptoUtils::constTimeCompare(ab, other.ab, sizeof(ab));
}
