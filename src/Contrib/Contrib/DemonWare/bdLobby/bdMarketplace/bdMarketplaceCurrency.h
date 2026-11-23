// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMarketplace/bdMarketplaceCurrency.h
#pragma once
#include "../bdLobbyCommon/bdSerializable.h"
#include "../bdLobbyCommon/bdTaskResult.h"
class bdMarketplaceCurrency : public bdTaskResult, public bdSerializable {
public:
    BD_CLASS_ALLOCATOR(bdMarketplaceCurrency);
    bdMarketplaceCurrency();
    virtual ~bdMarketplaceCurrency() {}
    virtual bdBool serialize(bdByteBuffer &buffer) const;
    virtual bdBool deserialize(bdByteBufferRef buffer);
    virtual bdUInt sizeOf();
    virtual bdInt serializedSizeOf() const;
    void reset();
    bdUByte8 m_currencyId;
    bdUInt32 m_value;
};
static_assert(sizeof(bdMarketplaceCurrency) == 0x28, "bdMarketplaceCurrency");
