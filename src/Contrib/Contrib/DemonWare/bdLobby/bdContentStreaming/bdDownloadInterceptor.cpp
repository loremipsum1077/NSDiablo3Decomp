// Original file: Contrib/Contrib/DemonWare/bdLobby/bdContentStreaming/bdDownloadInterceptor.cpp
#include "bdDownloadInterceptor.h"












bdDownloadInterceptor::~bdDownloadInterceptor()
{
}

// LINKER-REMOVED with the vtable (the original keeps no vtable for this class and DemonWare has no
// DWARF). Invented body: accept any length; bdHTTPCurl stops the transfer when this returns false.
bdBool bdDownloadInterceptor::handleContentLength(bdUInt32, bdBool)
{
    return true;
}
