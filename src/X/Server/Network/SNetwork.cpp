#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/SigmaTypes.h"

// Minimal declarations of the server-socket entry points this file calls. They
// belong in X/Server/Network/XServerSocket.h, which is not adopted into src/
// yet; the mangled names match the originals used by this TU.
BOOL XServerStartListener();
void XServerStopListener();
BOOL XServerIsListening();

// SNetworkIsRunning is defined at the end of this file (original line 87) but
// used first by SNetworkStop (original line 72), so it needs a prior
// declaration. It is fully inlined (no out-of-line copy in the original).
BOOL SNetworkIsRunning();

// Original line 44.
void SNetworkStart()
{
    BOOL bStarted = XServerStartListener();
    (void)bStarted;
}

// Original line 69.
void SNetworkStop()
{
    if (SNetworkIsRunning())
        XServerStopListener();
}

// Original line 87.
BOOL SNetworkIsRunning()
{
    return XServerIsListening();
}
