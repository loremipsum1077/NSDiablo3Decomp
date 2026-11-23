// X/Server/Network/KnownMessage.cpp -- messages queued against an ACD
// (ActorCommonData::listACDMessages) and delivered to the players that already
// know it.
#include "KnownMessage.h"

#include "../../Common/ActorCommonData.h"
#include "../../Common/Player.h"
#include "../../Common/Networking/GameMessage.h"
#include "../PlayerList.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"

// Declared in X/Common/Networking/ByteswapMessage.h (its header is not adopted yet).
BOOL GetGameMessageTypeAndSize(GameMessageType eMessageType, const XTypeDescriptor *& pTypeDescriptor,
                               uint32 & dwSize, const char *& pszMessageLabel);  // ByteswapMessage.cpp:74

void KnownMessageSend(const PlayerList & listDestPlayers, const KnownMessage & tKnownMessage)  // :26
{
    const void * ptMessage = tKnownMessage.byteBuffer;                            // :30
    const RequiredMessageHeader & tHeader = *(const RequiredMessageHeader *)ptMessage;
    const XTypeDescriptor * pTypeDescriptor = NULL;
    uint32 dwSize;
    const char * pszLabel;

    GetGameMessageTypeAndSize(tHeader.eType, pTypeDescriptor, dwSize, pszLabel);  // :36

    MessageSendToClient(listDestPlayers, tHeader.eType, (void *)ptMessage, dwSize);  // :38
}

void KnownMessageAdd(ActorCommonData & tACD, const GameMessageType eMessageType, const void * ptMessage,
                     const uint32 dwSize, const PlayerList * ptDestPlayers)  // :43
{
    uint32 dwPlayerMask = 0;                                                      // :52

    if (ptDestPlayers != NULL) {                                                  // :53
        POS pos = ptDestPlayers->GetHeadPosition();                               // :60

        while (pos != NULL) {                                                     // :61
            const Player * ptPlayer = ptDestPlayers->GetNext(pos);                // :63

            if (!ptPlayer->bIsPrimaryPlayerServerOnly) {                          // :67
                ptPlayer = PlayerGetPrimaryPlayerForGameConnection(ptPlayer->idGameConnection);  // :69
                if (ptPlayer == NULL) {                                               // :70
                    continue;
                }
            }

            dwPlayerMask |= 1 << ptPlayer->nPlayerIndex;                          // :77
        }

        if (dwPlayerMask == 0) {                                                  // :82
            return;
        }
    } else {
        dwPlayerMask = 0xffffffff;
    }

    KnownMessage tKnownMessage;                                                   // :88
    tKnownMessage.dwDestinationsPlayerIndexMask = dwPlayerMask;                   // :91

    RequiredMessageHeader & tHeader = *(RequiredMessageHeader *)ptMessage;        // :106
    tHeader.dwSize = dwSize;                                                      // :107
    tHeader.eType = eMessageType;                                                 // :108

    SigmaMemoryCopy(tKnownMessage.byteBuffer, ptMessage, dwSize);                 // :111

    tACD.listACDMessages.AddTail_Tracked(tKnownMessage);                          // :114
    tACD.ModifiedMark(1);                                                         // :118
}
