// Original file: X/Common/Networking/Network.cpp

#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "Network.h"
#include "GameMessage.h"
#include "XSocket.h"
#include "../Game.h"
#include "../ThreadGlobals.h"
#include "../Shell/CmdLine.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/stubs/common.h"
#include "../../../Contrib/Contrib/Zlib/zlib.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Networking/XDecoder.h"
#include "../../Sigma/Networking/XNetAddress.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

void ClientInitializeForApp();  // X/Client/Network/Client.cpp:679
void ClientDisposeForApp();  // X/Client/Network/Client.cpp:698
void TelnetConsoleInitializeForApp();  // X/Server/Shell/SConsoleCommand.cpp
void TelnetConsoleDisposeForApp();  // X/Server/Shell/SConsoleCommand.cpp:381
void XServerInitializeForApp();  // X/Server/Network/XServerSocket.cpp:219
void XServerDisposeForApp();  // X/Server/Network/XServerSocket.cpp:384
void ServerMonInitForApp();  // X/Server/Network/ServerMon.cpp
void ServerMonDisposeForApp();  // X/Server/Network/ServerMon.cpp:438
BOOL GetGameMessageTypeAndSize(GameMessageType eMessageType, const XTypeDescriptor *& pTypeDescriptor, uint32 & dwSize, const char *& pszMessageLabel);  // X/Common/Networking/GameMessage.cpp



// Line 54: BOOL gbKeepAliveTimeOutEnabled. No copy survives in the image (not in data_symbols.csv), so it is left out.

struct NetworkGlobals {  // size 0x28
    XNetAddress tSelfAddress;  // 0x00
    uint32 dwHeartRate;  // 0x10
    uint32 dwNetworkLatency;  // 0x14
    uint32 dwUserLatency;  // 0x18
    uint32 dwHeartbeatLatency;  // 0x1c
    uint32 dwInitialMaxGameTime;  // 0x20
    uint32 uProtocolHash;  // 0x24
};

struct TPacketBufferSize8 {  // size 0x10
    uint8 data[16];  // 0x00
};

struct TPacketBufferSize32 {  // size 0x28
    uint8 data[40];  // 0x00
};

struct TPacketBufferSize128 {  // size 0x88
    uint8 data[136];  // 0x00
};

struct TPacketBufferSize256 {  // size 0x108
    uint8 data[264];  // 0x00
};

struct TPacketBufferSize512 {  // size 0x208
    uint8 data[520];  // 0x00
};

struct TPacketBufferSize1024 {  // size 0x408
    uint8 data[1032];  // 0x00
};

typedef XChainedFixedBlockAllocator<TPacketBufferSize8> TPacketAllocator8;
typedef XChainedFixedBlockAllocator<TPacketBufferSize32> TPacketAllocator32;
typedef XChainedFixedBlockAllocator<TPacketBufferSize128> TPacketAllocator128;
typedef XChainedFixedBlockAllocator<TPacketBufferSize256> TPacketAllocator256;
typedef XChainedFixedBlockAllocator<TPacketBufferSize512> TPacketAllocator512;
typedef XChainedFixedBlockAllocator<TPacketBufferSize1024> TPacketAllocator1024;

struct PacketPool {  // size 0x120
    TPacketAllocator8 tPacketAllocator8;  // 0x00
    TPacketAllocator32 tPacketAllocator32;  // 0x30
    TPacketAllocator128 tPacketAllocator128;  // 0x60
    TPacketAllocator256 tPacketAllocator256;  // 0x90
    TPacketAllocator512 tPacketAllocator512;  // 0xc0
    TPacketAllocator1024 tPacketAllocator1024;  // 0xf0
};

uint8 * NetworkGetCompressionBuffer();











































void GoogleLogHandler(google::protobuf::LogLevel level, const char * filename, int line, const blz::string & message)
{




    if (level == google::protobuf::LOGLEVEL_FATAL)
    {











        CRefString errorString;
        errorString.Format("%s (%d) %s\n", filename, line, message.c_str());


        DisplayInternalError(NULL, "", 153, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

}




void NetworkInitializeForApp()
{


    google::protobuf::SetLogHandler(GoogleLogHandler);

    AppGlobals & tAppGlobals = sgtAppGlobals;
    NetworkGlobals *& ptNetworkGlobals = tAppGlobals.ptNetworkGlobals;

    ptNetworkGlobals = (NetworkGlobals *)SigmaMemoryNew(sizeof(NetworkGlobals), 4, NULL, TRUE);

    BOOL bResult;

    bResult = XSocketGetMyIPAddress(ptNetworkGlobals->tSelfAddress);

    if (!bResult)
    {
        ptNetworkGlobals->tSelfAddress = XNetAddress::FromString("127.0.0.1", 0);
    }




































    ClientInitializeForApp();


    TelnetConsoleInitializeForApp();
    XServerInitializeForApp();
    ServerMonInitForApp();




    ComplexTypeDescriptor * pRegistrationCurrent = ComplexTypeDescriptor::GetRegistrationHead();
    while (pRegistrationCurrent)
    {

        for (int32 i = 0; ; i++)
        {
            const TypeDescriptorField & fieldChild = pRegistrationCurrent->GetField(i);
            if (fieldChild.IsNull())
            {
                break;
            }
            const_cast<TypeDescriptorField &>(fieldChild).CalculateNetworkBitCounts();
        }

        pRegistrationCurrent = pRegistrationCurrent->GetNextRegisteredDescriptor();
    }

    uint32 uProtocolHash = 0;
    for (int32 i = 1; i < MSG_NUM_MESSAGES; i++)
    {

        if (i == CMSG_AV_CAMERA_MODE_TOGGLE || i == CMSG_AV_CAMERA_LOCATION || i == CCMSG_END_OF_TICK)
        {
            continue;
        }

        const XTypeDescriptor * pTypeDescriptor = NULL;
        uint32 dwSize = 0;
        const char * pszMessageLabel;
        GetGameMessageTypeAndSize(i, pTypeDescriptor, dwSize, pszMessageLabel);
        if (pTypeDescriptor)
        {
            TypeDescriptorField field(pTypeDescriptor, NULL);
            pTypeDescriptor->CreateFieldHash(field, uProtocolHash);
        }
    }












    ptNetworkGlobals->uProtocolHash = uProtocolHash;















}




void NetworkDisposeForApp()
{

    AppGlobals & tAppGlobals = sgtAppGlobals;
    NetworkGlobals *& ptNetworkGlobals = tAppGlobals.ptNetworkGlobals;

    if (ptNetworkGlobals)
    {

        TelnetConsoleDisposeForApp();
        ServerMonDisposeForApp();




        ClientDisposeForApp();


        XServerDisposeForApp();


        SigmaMemoryFree(ptNetworkGlobals, NULL); ptNetworkGlobals = NULL;
    }
}




uint32 NetworkGetProtocolHash()
{

    AppGlobals & tAppGlobals = sgtAppGlobals;
    return tAppGlobals.ptNetworkGlobals->uProtocolHash;
}




uint32 sNetworkUncompress(int32 eSyncMode, BOOL bResetStream, uint8 * pbInput, uint32 dwInputSize, uint8 * pbOutput, uint32 dwOutputSize)
{






    ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();
    if (tThreadGlobals.m_ptInflateZstream == NULL)
    {
        tThreadGlobals.m_ptInflateZstream = (z_streamp)SigmaMemoryNew(sizeof(z_stream), 4, NULL, TRUE);
        int32 nResult = inflateInit(tThreadGlobals.m_ptInflateZstream);
        if (nResult != Z_OK)
        {
            return 0;
        }
    }
    else if (bResetStream)
    {
        inflateReset(tThreadGlobals.m_ptInflateZstream);
    }

    z_streamp ptStream = tThreadGlobals.m_ptInflateZstream;

    ptStream->next_in = pbInput;
    ptStream->avail_in = dwInputSize;

    ptStream->next_out = pbOutput;
    ptStream->avail_out = dwOutputSize;

    int32 nResult = inflate(ptStream, eSyncMode);
    if (nResult != Z_OK)
    {
        return 0;
    }


    uint32 dwUncompressedSize = dwOutputSize - ptStream->avail_out;

    if (ptStream->avail_in != 0)
    {
        return 0;
    }

    return dwUncompressedSize;


}



// NON_MATCHING: 2 instructions swapped before the XDecoder::SetData call (the original copies pbCurrentPacket into x1 before storing the decoder's vtable pointer; ours stores first)
BOOL NetworkParseGameDataBlock(intptr idUpcallHandle, SocketReceivedData & tSocketData, NetworkGameDataBlockHandlerProc pHandlerFcn)
{





    uint8 * pMessage = tSocketData.lpMsg;

    uint32 dwBytesRemaining = tSocketData.dwSize;
    BOOL bContinueParsing = TRUE;
    tSocketData.dwConsumed = 0;

    while (dwBytesRemaining >= sizeof(uint32) && bContinueParsing)
    {
        uint8 * pbCurrentPacket = pMessage + tSocketData.dwConsumed;

        uint32 dwSize;
        XDecoder tDecoder(pbCurrentPacket, dwBytesRemaining);
        if (tDecoder.Get(dwSize, 32) != XDecoder::RESULT_VALID)
        {
            return FALSE;
        }


        BOOL bCompressed = (dwSize & (1u << NETWORK_COMPRESSION_PACKET_COMPRESSED_BIT)) != 0;
        BOOL bSyncFlush = (dwSize & (1u << NETWORK_COMPRESSION_PACKET_SYNC_FLUSH_BIT)) != 0;
        BOOL bResetStream = (dwSize & (1u << NETWORK_COMPRESSION_PACKET_RESET_STREAM_BIT)) != 0;


        dwSize &= NETWORK_COMPRESSION_SIZE_MASK;


        if (dwSize > MAX_SINGLE_MESSAGE_SIZE)
        {
            return FALSE;
        }


        if (dwSize > dwBytesRemaining)
        {
            break;
        }





        uint8 * pbPacketBuffer = pbCurrentPacket + sizeof(uint32);
        uint32 dwPacketSize = dwSize - sizeof(uint32);




        if (bCompressed)
        {
            if (!GameActiveGameCommonDataIsClient())
            {
                return FALSE;
            }


            uint8 * pCompressionBuffer = NetworkGetCompressionBuffer();

            uint32 dwUncompressedBytes = sNetworkUncompress(
                bSyncFlush ? Z_SYNC_FLUSH : Z_FULL_FLUSH, bResetStream, pbPacketBuffer, dwPacketSize, pCompressionBuffer, MAX_SINGLE_MESSAGE_SIZE);
            if (dwUncompressedBytes == 0)
            {
                return FALSE;
            }

            pbPacketBuffer = pCompressionBuffer;
            dwPacketSize = dwUncompressedBytes;
        }






        if (!pHandlerFcn(idUpcallHandle, pbPacketBuffer, dwPacketSize, bContinueParsing))
        {
            return FALSE;
        }

        tSocketData.dwConsumed += dwSize;
        dwBytesRemaining -= dwSize;
    }

    return TRUE;
}












void NetworkGetIP(XNetAddress & tAddress)
{

    AppGlobals & tAppGlobals = sgtAppGlobals;
    NetworkGlobals * ptNetworkGlobals = tAppGlobals.ptNetworkGlobals;

    tAddress = ptNetworkGlobals->tSelfAddress;
}




uint8 * NetworkGetEncodeBuffer()
{

    ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();


    if (tThreadGlobals.m_pauEncodeBuffer == NULL)
    {
        tThreadGlobals.m_pauEncodeBuffer = (uint8 *)SigmaMemoryNew(MAX_SINGLE_MESSAGE_SIZE, 4, NULL, FALSE);
    }

    return tThreadGlobals.m_pauEncodeBuffer;
}




uint8 * NetworkGetDecodeBuffer()
{

    ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();


    if (tThreadGlobals.m_pauDecodeBuffer == NULL)
    {
        tThreadGlobals.m_pauDecodeBuffer = (uint8 *)SigmaMemoryNew(MAX_SINGLE_MESSAGE_SIZE, 4, NULL, FALSE);
    }

    return tThreadGlobals.m_pauDecodeBuffer;
}




uint8 * NetworkGetCompressionBuffer()
{

    ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();


    if (tThreadGlobals.m_pauCompressionBuffer == NULL)
    {
        tThreadGlobals.m_pauCompressionBuffer = (uint8 *)SigmaMemoryNew(MAX_SINGLE_MESSAGE_SIZE, 4, NULL, FALSE);
    }

    return tThreadGlobals.m_pauCompressionBuffer;
}




void NetGetPlayerName(LPSTR sPlayerName, uint32 uLength)
{


    const CmdLineParams * pParams = CmdLineGetParams();

    if (pParams->sPlayerName[0])
    {
        Blizzard::String::Copy(sPlayerName, pParams->sPlayerName, uLength);
        return;
    }




    XNetAddress tSelfAddress;
    NetworkGetIP(tSelfAddress);
    Blizzard::String::Copy(sPlayerName, tSelfAddress.ToString(TRUE).str(), uLength);




















}




PacketPool * PacketPoolCreate(XMemoryAllocatorInterface * ptMemoryInterface)
{

    PacketPool * ptPacketPool = (PacketPool *)SigmaMemoryNew(sizeof(PacketPool), 4, ptMemoryInterface, TRUE);

    ptPacketPool->tPacketAllocator8.Initialize();
    ptPacketPool->tPacketAllocator8.SetMemoryInterface(ptMemoryInterface);
    ptPacketPool->tPacketAllocator8.SetBlockCount_Tracked(100);
    ptPacketPool->tPacketAllocator8.SetGrow(XALLOC_GROW_LINEAR, 100);
    ptPacketPool->tPacketAllocator8.SetShrinkable(TRUE);
    ptPacketPool->tPacketAllocator32.Initialize();
    ptPacketPool->tPacketAllocator32.SetMemoryInterface(ptMemoryInterface);
    ptPacketPool->tPacketAllocator32.SetBlockCount_Tracked(100);
    ptPacketPool->tPacketAllocator32.SetGrow(XALLOC_GROW_LINEAR, 100);
    ptPacketPool->tPacketAllocator32.SetShrinkable(TRUE);
    ptPacketPool->tPacketAllocator128.Initialize();
    ptPacketPool->tPacketAllocator128.SetMemoryInterface(ptMemoryInterface);
    ptPacketPool->tPacketAllocator128.SetBlockCount_Tracked(100);
    ptPacketPool->tPacketAllocator128.SetGrow(XALLOC_GROW_LINEAR, 100);
    ptPacketPool->tPacketAllocator128.SetShrinkable(TRUE);

    ptPacketPool->tPacketAllocator256.Initialize();
    ptPacketPool->tPacketAllocator256.SetMemoryInterface(ptMemoryInterface);
    ptPacketPool->tPacketAllocator256.SetBlockCount_Tracked(32);
    ptPacketPool->tPacketAllocator256.SetGrow(XALLOC_GROW_LINEAR, 32);
    ptPacketPool->tPacketAllocator256.SetShrinkable(TRUE);

    ptPacketPool->tPacketAllocator512.Initialize();
    ptPacketPool->tPacketAllocator512.SetMemoryInterface(ptMemoryInterface);
    ptPacketPool->tPacketAllocator512.SetBlockCount_Tracked(32);
    ptPacketPool->tPacketAllocator512.SetGrow(XALLOC_GROW_LINEAR, 32);
    ptPacketPool->tPacketAllocator512.SetShrinkable(TRUE);
    ptPacketPool->tPacketAllocator1024.Initialize();
    ptPacketPool->tPacketAllocator1024.SetMemoryInterface(ptMemoryInterface);
    ptPacketPool->tPacketAllocator1024.SetBlockCount_Tracked(16);
    ptPacketPool->tPacketAllocator1024.SetGrow(XALLOC_GROW_LINEAR, 16);
    ptPacketPool->tPacketAllocator1024.SetShrinkable(TRUE);








    return ptPacketPool;
}




void PacketPoolDispose(PacketPool *& ptPacketPool, XMemoryAllocatorInterface * ptMemoryInterface)
{


    ptPacketPool->tPacketAllocator8.Dispose();
    ptPacketPool->tPacketAllocator32.Dispose();
    ptPacketPool->tPacketAllocator128.Dispose();

    ptPacketPool->tPacketAllocator256.Dispose();

    ptPacketPool->tPacketAllocator512.Dispose();
    ptPacketPool->tPacketAllocator1024.Dispose();




    SigmaMemoryFree(ptPacketPool, ptMemoryInterface);

    ptPacketPool = NULL;
}




PacketBuffer * PacketBufferAlloc(PacketPool * ptPacketPool, uint32 dwSize)
{

    PacketBuffer * ptPacketBuffer;


    if (dwSize <= 8)
    {
        ptPacketBuffer = (PacketBuffer *)ptPacketPool->tPacketAllocator8.Alloc_Tracked();
        ptPacketBuffer->tHeader.dwPoolHandle = 8;
    }
    else if (dwSize <= 32)
    {
        ptPacketBuffer = (PacketBuffer *)ptPacketPool->tPacketAllocator32.Alloc_Tracked();
        ptPacketBuffer->tHeader.dwPoolHandle = 32;
    }
    else if (dwSize <= 128)
    {
        ptPacketBuffer = (PacketBuffer *)ptPacketPool->tPacketAllocator128.Alloc_Tracked();
        ptPacketBuffer->tHeader.dwPoolHandle = 128;
    }

    else if (dwSize <= 256)
    {
        ptPacketBuffer = (PacketBuffer *)ptPacketPool->tPacketAllocator256.Alloc_Tracked();
        ptPacketBuffer->tHeader.dwPoolHandle = 256;
    }

    else if (dwSize <= 512)
    {
        ptPacketBuffer = (PacketBuffer *)ptPacketPool->tPacketAllocator512.Alloc_Tracked();
        ptPacketBuffer->tHeader.dwPoolHandle = 512;
    }
    else if (dwSize <= 1024)
    {
        ptPacketBuffer = (PacketBuffer *)ptPacketPool->tPacketAllocator1024.Alloc_Tracked();
        ptPacketBuffer->tHeader.dwPoolHandle = 1024;
    }


    else
    {


        ptPacketBuffer = (PacketBuffer *)SigmaMemoryNew(dwSize + sizeof(PacketBufferHeader), 4, NULL, FALSE);
        ptPacketBuffer->tHeader.dwPoolHandle = 0x8000;
    }


    return ptPacketBuffer;
}




void PacketBufferFree(PacketPool * ptPacketPool, PacketBuffer *& ptPacketBuffer)
{


    switch (ptPacketBuffer->tHeader.dwPoolHandle)
    {
    case 8:
        ptPacketPool->tPacketAllocator8.Free((TPacketBufferSize8 *)ptPacketBuffer);
        break;
    case 32:
        ptPacketPool->tPacketAllocator32.Free((TPacketBufferSize32 *)ptPacketBuffer);
        break;
    case 128:
        ptPacketPool->tPacketAllocator128.Free((TPacketBufferSize128 *)ptPacketBuffer);
        break;

    case 256:
        ptPacketPool->tPacketAllocator256.Free((TPacketBufferSize256 *)ptPacketBuffer);
        break;

    case 512:
        ptPacketPool->tPacketAllocator512.Free((TPacketBufferSize512 *)ptPacketBuffer);
        break;
    case 1024:
        ptPacketPool->tPacketAllocator1024.Free((TPacketBufferSize1024 *)ptPacketBuffer);
        break;



    case 0x8000:
        SigmaMemoryFree(ptPacketBuffer, NULL);
        break;
    default:

        DisplayInternalError(NULL, "", 759, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    ptPacketBuffer = NULL;
}




void PacketBufferListFreeAll(PacketPool * ptPacketPool, TPacketBufferList & tPacketBufferList)
{


    while (!tPacketBufferList.IsEmpty())
    {
        PacketBuffer * ptPacketBuffer = tPacketBufferList.GetHead();
        tPacketBufferList.RemoveHead();
        PacketBufferFree(ptPacketPool, ptPacketBuffer);
    }
}
