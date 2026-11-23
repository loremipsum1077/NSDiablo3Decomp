#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCryptoUtils.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityKey.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"


bdSecurityKey::bdSecurityKey()
{
    bdMemset(ab, 1, sizeof(ab));
}

bdSecurityKey::bdSecurityKey(const bdSecurityKey & other)
{
    bdMemcpy(ab, other.ab, sizeof(ab));
}

bdSecurityKey::~bdSecurityKey()
{
    bdCryptoUtils::zeroBuffer(ab, sizeof(ab));
}

bdBool bdSecurityKey::operator==(const bdSecurityKey & other) const
{
    return bdCryptoUtils::constTimeCompare(ab, other.ab, sizeof(ab));
}
