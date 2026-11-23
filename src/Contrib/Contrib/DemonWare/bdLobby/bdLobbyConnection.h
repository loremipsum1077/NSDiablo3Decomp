// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyConnection.h
#pragma once

#include "../bdCore/bdContainers/bdBitBuffer.h"
#include "../bdCore/bdContainers/bdByteBuffer.h"
#include "../bdCore/bdContainers/bdQueue.h"
#include "../bdCore/bdReference/bdReferencable.h"
#include "../bdCore/bdReference/bdReference.h"
#include "../bdCore/bdSocket/bdAddr.h"
#include "../bdCore/bdSocket/bdStreamSocket.h"
#include "../bdCore/bdTiming/bdStopwatch.h"
#include "../bdCore/bdUtilities/bdTokenBucket.h"
#include "bdAuthTicket.h"
#include "bdRemoteTaskManager/bdTaskByteBuffer.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdCypher;
class bdCypherAES;
class bdLobbyConnectionListener;
class bdPendingBufferTransfer;
class bdLobbyConnectionMigrationHeldBackTask;

typedef bdReference<bdPendingBufferTransfer> bdPendingBufferTransferRef;

class bdLobbyConnection : public bdReferencable {  // size 0x720
public:
    BD_CLASS_ALLOCATOR(bdLobbyConnection);
    enum Status {
        BD_NOT_CONNECTED = 0,
        BD_CONNECTING = 1,
        BD_CONNECTED = 2,
        BD_DISCONNECTING = 3,
        BD_DISCONNECTED = 4,
    };
    enum RecvState {
        BD_READ_INIT = 0,
        BD_READ_SIZE = 1,
        BD_READ_ENCRYPT = 2,
        BD_READ_MESSAGE = 3,
        BD_READ_COMPLETE = 4,
    };
    enum Crypto210State {
        BD_CRYPTO210_INVALID = 0,
        BD_CRYPTO210_WAIT_SERVER_ACK = 1,
        BD_CRYPTO210_WAIT_SERVER_AUTH_DONE = 2,
        BD_CRYPTO210_READY = 3,
    };
    enum Migration220State {
        BD_MIGRATION220_IDLE = 0,
        BD_MIGRATION220_INIT_ACK_SENT = 1,
        BD_MIGRATION220_RECONNECTING = 2,
        BD_MIGRATION220_RECONNECTED = 3,
    };
    bdLobbyConnection(bdAddr lobbyAddr, bdLobbyConnectionListener * connectionListener, bdBool isLSGConnection, bdBool isPlaintext, bdUWord sendBufSize, bdUWord recvBufSize);
    virtual ~bdLobbyConnection();  // vtable slot 0
    virtual bdBool sendTask(bdTaskByteBufferRef message, bdUInt messageSize, const bdBool encrypt);  // vtable slot 2
    bdUInt getMaxSendMessageSizeWithoutOverhead() const;
    bdBool setSessionKey(const bdUByte8 *, const bdUInt);
    bdLobbyConnection::Status getStatus() const;
    void sendRaw(void *, const bdUInt32);
    bdBool getAuthFramedMessage(bdUByte8 &, bdBitBufferRef &);
    bdBool getMessageToDispatch(bdUByte8 & type, bdByteBufferRef & payload);
    bdUInt32 getReceiveBufferSize() const;
    bdBool connect(bdAuthInfo & authInfo);
    void disconnect();
    void close();
    bdBool pump();
    void getHandleInfo(bdInt &, bdBool &, bdBool &, bdFloat32 &);
    bdTokenBucket & getThrottleBucket();
    void setTasksAreThrottled(bdBool areThrottled);
    bdBool isConnectionMigrationSupported() const;
protected:
    void onTcpConnect(const bdBool success);
    void callListenersDisconnect();
    void sendFramedMessage(const void * buffer, const bdUInt32 bufferSize);
    bdBool recvMessageData();
    bdInt recvMessageSize();
    bdInt recvEncryptType();
    bdBool receivedFullMessage();
    bdBool send200ClientHeader();
    static bdUInt getSendMessageOverheadSize();
    bdBool getMessageToDispatch200(bdUByte8 &, bdByteBufferRef &);
    bdBool getMessageToDispatch210(bdUByte8 & type, bdByteBufferRef & payload);
    bdBool sendTask200(bdTaskByteBufferRef, bdUInt, const bdBool);
    bdBool sendTask210(bdTaskByteBufferRef message, bdUInt messageSize, const bdBool, const bdUByte8 payloadType);
    bdBool send210ClientHeader();
    bdBool process210ExpectedServerHeader(const bdUByte8 *const readPtr, const bdUInt readOffset, const bdUInt validBytes);
    bdBool process210ExpectedServerAuthDone(const bdUByte8 *const readPtr, const bdUInt readOffset, const bdUInt validBytes);
    bdBool extract210ExpectedServerError(const bdUByte8 *const readPtr, const bdUInt readOffset, const bdUInt validBytes, bdUInt32 & returnErrorReason);
    bdBool process210ExpectedRecord(bdUByte8 *const readPtr, const bdUInt readOffset, const bdUInt validBytes, bdUByte8 & returnMsgType, bdByteBufferRef & returnPayload);
    bdBool process220MigrateInit(bdUByte8 *const readPtr, const bdUInt validBytes, bdUInt & offset);
    bdBool parse220MigrateAck(bdUByte8 *const readPtr, const bdUInt validBytes, bdUInt & offset);
    bdBool process220MigrateAck(bdUByte8 *const readPtr, const bdUInt validBytes, bdUInt & offset);
    bdBool send220MigrateInitAck();
    bdBool process220MigrationServerAuthDone();
    bdBool prepare2x0ClientAuthAndDeriveCrypto(bdUByte8 *const replyPacketPtr, bdUInt & replyPacketOffset, const bdUInt replyPacketMaxSize, const bdUByte8 *const serverHeaderAckReadPtr, const bdUInt serverHeaderAckReadOffset, const bdUInt serverHeaderAckEndOffset);
    bdAddr m_lobbyAddr;  // 0x0c
    bdBool m_isLSGConnection;  // 0x94
    bdBool m_isPlaintext;  // 0x95
    bdUInt m_maxSendMessageSize;  // 0x98
    bdUInt m_maxRecvMessageSize;  // 0x9c
    bdLobbyConnection::RecvState m_recvState;  // 0xa0
    bdUByte8 m_msgSizeBuffer[4];  // 0xa4
    bdUInt m_recvCount;  // 0xa8
    bdUByte8 m_recvEncryptType;  // 0xac
    bdUInt32 m_messageSize;  // 0xb0
    bdTaskByteBufferRef m_recvMessage;  // 0xb8
    bdPendingBufferTransferRef m_recvTransfer;  // 0xc0
    bdQueue<bdPendingBufferTransfer> m_outgoingBuffers;  // 0xc8
    bdStreamSocket m_socket;  // 0xe0
    bdLobbyConnection::Status m_status;  // 0xe4
    bdLobbyConnectionListener * m_connectionListener;  // 0xe8
    bdUInt m_encryptionType;  // 0xf0
    bdCypher * m_cypher;  // 0xf8
    bdUByte8 m_sessionKey[24];  // 0x100
    bdUInt32 m_messageCount;  // 0x118
    bdStopwatch m_keepAliveTimer;  // 0x120
    bdStopwatch m_lastReceivedTimer;  // 0x128
    bdStopwatch m_asyncConnectTimer;  // 0x130
    bdTokenBucket m_throttleBucket;  // 0x138
    bdBool m_tasksAreThrottled;  // 0x158
    bdAuthInfo m_authInfo;  // 0x160
    bdLobbyConnection::Crypto210State m_cryptoState;  // 0x210
    bdUInt32 m_cypher210RecvCounter;  // 0x214
    bdUInt32 m_cypher210SendCounter;  // 0x218
    bdUByte8 m_clientNonce[8];  // 0x21c
    bdUByte8 m_expectedServerVerify[8];  // 0x224
    bdCypherAES * m_cypher210Client;  // 0x230
    bdCypherAES * m_cypher210Server;  // 0x238
    bdUByte8 m_cypher210ClientMacKey[20];  // 0x240
    bdUByte8 m_cypher210ServerMacKey[20];  // 0x254
    bdUInt64 m_cypher210ConnID;  // 0x268
    bdUInt32 m_serverSelectedProto;  // 0x270
    bdLobbyConnection::Migration220State m_migrationState;  // 0x274
    bdAddr m_lobbyMigrationAddr;  // 0x278
    bdQueue<bdLobbyConnectionMigrationHeldBackTask> m_migrationOutgoingTaskQueue;  // 0x300
    bdUInt m_migrationTicketLen;  // 0x318
    bdUByte8 m_migrationTicket[1024];  // 0x31c
    bdFloat32 m_migrationReconnectTimeout;  // 0x71c
};
static_assert(sizeof(bdLobbyConnection) == 0x720, "bdLobbyConnection");
typedef bdReference<bdLobbyConnection> bdLobbyConnectionRef;

class bdPendingBufferTransfer : public bdReferencable {  // size 0x28
public:
    BD_CLASS_ALLOCATOR(bdPendingBufferTransfer);
    bdPendingBufferTransfer(bdTaskByteBufferRef buffer, bdUInt totalSize, bdBool isThrottled);
    bdPendingBufferTransfer(bdByteBufferRef buffer, bdUInt totalSize, bdBool isThrottled);
    bdPendingBufferTransfer(const bdPendingBufferTransfer & other);
    virtual ~bdPendingBufferTransfer();  // vtable slot 0
    bdUByte8 * getData() const;
    bdUInt32 getAvail() const;
    bdUInt32 updateTransfer(bdUInt32 amountTransfered);
    bdBool getIsThrottled() const;
protected:
    bdByteBufferRef m_buffer;  // 0x10
    bdUByte8 * m_txPtr;  // 0x18
    bdUInt32 m_txAvail;  // 0x20
    bdBool m_isThrottled;  // 0x24
};
static_assert(sizeof(bdPendingBufferTransfer) == 0x28, "bdPendingBufferTransfer");

class bdLobbyConnectionMigrationHeldBackTask {  // size 0x10
public:
    BD_CLASS_ALLOCATOR(bdLobbyConnectionMigrationHeldBackTask);
    bdLobbyConnectionMigrationHeldBackTask(const bdLobbyConnectionMigrationHeldBackTask & other)
        : m_task(other.m_task), m_messageSize(other.m_messageSize)
    {
    }
    bdLobbyConnectionMigrationHeldBackTask(bdTaskByteBufferRef task, bdUInt messageSize)
        : m_task(task), m_messageSize(messageSize)
    {
    }
    ~bdLobbyConnectionMigrationHeldBackTask()
    {
    }
    bdTaskByteBufferRef m_task;  // 0x00
    bdUInt m_messageSize;  // 0x08
};
static_assert(sizeof(bdLobbyConnectionMigrationHeldBackTask) == 0x10, "bdLobbyConnectionMigrationHeldBackTask");
