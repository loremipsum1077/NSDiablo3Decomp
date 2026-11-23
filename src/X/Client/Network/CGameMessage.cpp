// Reconstructed source; see docs/source_provenance.md.

#include "../../Common/CommonTypes.h"
#include "../../Common/Player.h"

// ComposeProtocolBufferMessage is defined in X/Common/Networking/GameMessage.cpp:1614; its
// declaration lives in X/Common/Networking/GameMessage.h, which is not adopted here yet.
namespace google {
namespace protobuf {
class MessageLite;
}  // namespace protobuf
}  // namespace google

uint32 ComposeProtocolBufferMessage(const google::protobuf::MessageLite & tMessage,
                                    GameMessageType eType, uint8 * pMessageBuffer,
                                    uint32 uBufferSize);

// ClientPutMessageOnOutgoingQueue is defined in X/Client/Network/Client.cpp:1150.
void ClientPutMessageOnOutgoingQueue(uint8 * pMessage);
void MessageSendToServer(
    GameMessageType eType,
    void * pMessage,
    uint32 dwMessageSize,
    int32 nControllerIndex)
{
    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;
    ptHeader->eType = eType;
    ptHeader->dwSize = (uint16)dwMessageSize;
    ptHeader->nControllerIndex = (int8)nControllerIndex;
    ClientPutMessageOnOutgoingQueue((uint8 *)pMessage);
}

void MessageSendToServer(
    GameMessageType eType,
    void * pMessage,
    uint32 dwMessageSize,
    const Player * ptPlayer)
{
    // A null player means "no controller" and is sent as -1.
    int32 nControllerIndex;
    if (ptPlayer == NULL)
        nControllerIndex = -1;
    else
        nControllerIndex = ptPlayer->nControllerIndex;

    MessageSendToServer(eType, pMessage, dwMessageSize, nControllerIndex);
}

void MessageSendToServer(
    GameMessageType eType,
    const google::protobuf::MessageLite & tMessage,
    const Player * ptPlayer)
{
    // Largest message the wire format allows, including the 8 byte header.
    uint8 auBlobMessage[0x8000];

    uint32 uTotalMessageSize;
    uTotalMessageSize = ComposeProtocolBufferMessage(tMessage, eType, auBlobMessage, 0x8000);

    MessageSendToServer(eType, auBlobMessage, uTotalMessageSize, ptPlayer);
}
