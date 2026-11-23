#include "bdStructBufferSerializable.h"












bdStructBufferSerializable::bdStructBufferSerializable() {}



bdStructBufferSerializable::~bdStructBufferSerializable() {}



bdStructBufferSerializable::bdStructBufferSerializable(const bdStructBufferSerializable &) {}



bdBool bdStructBufferSerializable::serialize(bdStructBufferSerializer &) const
{
    return false;
}



bdBool bdStructBufferSerializable::deserialize(bdStructBufferDeserializer &)
{
    return false;
}



bdBool bdStructBufferSerializable::serializeWithLobbyService(bdStructBufferSerializer & serializer, bdLobbyService *) const
{
    return serialize(serializer);
}




bdBool bdStructBufferSerializable::deserializeWithLobbyService(bdStructBufferDeserializer & deserializer, bdLobbyService *)
{
    return deserialize(deserializer);
}
