// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdUserAccountID.cpp
#include "bdUserAccountID.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString-unix.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"

bdUserAccountID::bdUserAccountID() { reset(); }
inline void bdUserAccountID::reset() {
    m_userID = 0;
    bdMemset(m_accountType, 0, sizeof(m_accountType));
}
bdUserAccountID::bdUserAccountID(const bdUserAccountID & other) : m_userID(other.m_userID) {
    bdStrlcpy(m_accountType, other.m_accountType, sizeof(m_accountType));
}
bdUserAccountID::bdUserAccountID(const bdOnlineUserID & userID) : m_userID(userID) {
    bdMemset(m_accountType, 0, sizeof(m_accountType));
}
bdUserAccountID::bdUserAccountID(const bdOnlineUserID & userID, const bdNChar8 *const accountType) : m_userID(userID) {
    bdStrlcpy(m_accountType, accountType, sizeof(m_accountType));
}
bdUserAccountID::~bdUserAccountID() {}
bdUserAccountID & bdUserAccountID::operator=(const bdUserAccountID & other) {
    bdStrlcpy(m_accountType, other.m_accountType, sizeof(m_accountType));
    m_userID = other.m_userID;
    return *this;
}
bool bdUserAccountID::operator==(const bdUserAccountID & other) const {
    bdBool equalType = bdStrncmp(m_accountType, other.m_accountType, sizeof(m_accountType)) == 0;
    return equalType && m_userID == other.m_userID;
}
bool bdUserAccountID::operator!=(const bdUserAccountID & other) const { return !(*this == other); }
bdUInt bdUserAccountID::sizeOf() { return sizeof(bdUserAccountID); }
bdBool bdUserAccountID::serialize(bdByteBuffer & buffer) const {
    bdBool ok = buffer.writeUInt64(m_userID);
    return ok && buffer.writeString(m_accountType, sizeof(m_accountType));
}
bdBool bdUserAccountID::deserialize(bdByteBufferRef buffer) {
    bdBool ok = buffer->readUInt64(m_userID);
    return ok && buffer->readString(m_accountType, sizeof(m_accountType));
}
bdUInt ContextSerialization::getUserIDSize(const bdUserAccountID & userID) {
    return 9 + 1 + bdStrnlen(userID.m_accountType, sizeof(userID.m_accountType)) + 1;
}
bdBool ContextSerialization::writeUserID(bdByteBufferRef buffer, const bdUserAccountID & userID) {
    return buffer->writeUInt64(userID.m_userID) && buffer->writeString(userID.m_accountType, sizeof(userID.m_accountType));
}
bdBool ContextSerialization::writeContext(bdByteBufferRef buffer, const bdNChar8 * context) {
    return buffer->writeString(context, 16);
}
bdBool ContextSerialization::readContext(bdByteBufferRef buffer, bdNChar8 * context, const bdUInt32 contextBufferSize) {
    return buffer->readString(context, contextBufferSize);
}
bdBool ContextSerialization::readUserID(bdByteBufferRef buffer, bdUserAccountID & userID) {
    bdBool ok = buffer->readUInt64(userID.m_userID);
    return ok && buffer->readString(userID.m_accountType, sizeof(userID.m_accountType));
}
