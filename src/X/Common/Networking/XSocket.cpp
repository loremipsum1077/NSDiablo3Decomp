// Original file: X/Common/Networking/XSocket.cpp

#include "XSocket.h"

#include "../../../BlizzardCore/Include/Blizzard/Config/Platform/NX64.h"
#include "../../../NintendoSDK/Include/nn/nifm/nifm_Api.h"
#include "../../../NintendoSDK/Include/nn/nn_Result.h"
#include "../../../NintendoSDK/Include/nn/socket/socket_Api.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Networking/XNetAddress.h"
#include "../../Sigma/Networking/XSocketBasePrivate.h"

void WinSockInit() {
    nn::nifm::Initialize();
}

void WinSockDispose() {
}

void XSocketClose(XSocket & tSocket) {
    tSocket.tBaseSocket.Close();
    tSocket.eState = SOCKETSTATE_DISCONNECTED;
    SigmaMemoryFree(tSocket.pbReceiveBuffer, NULL);
    tSocket.pbReceiveBuffer = NULL;
    SigmaMemoryFree(tSocket.pbSendBuffer, NULL);
    tSocket.pbSendBuffer = NULL;
}

BOOL XSocketGetMyIPAddress(XNetAddress & tAddress) {
    CHAR sName[256];
    in_addr addr;
    nn::Result result = nn::nifm::GetCurrentPrimaryIpAddress(&addr);
    if (result.IsSuccess()) {
        tAddress = XNetAddress::FromString(nn::socket::InetNtoa(addr), 0);
    } else {
        tAddress.SetLoopback(0);
    }
    if (tAddress.IsAny()) {
        int32 nReturnVal = WSAGetLastError();
        ReportWinSockError("gethostbyname error", nReturnVal);
        return FALSE;
    }
    return TRUE;
}
