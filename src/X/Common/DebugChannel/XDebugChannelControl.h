// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/DebugChannel/XDebugChannelControl.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/SigmaTypes.h"

class XDebugChannelControl {  // size 0x1
public:
    XDebugChannelControl(const char *);
    ~XDebugChannelControl();
    void StartListening(uint16 wListeningPort);
    void StopListening();
    BOOL IsListening();
    void Update();
};
static_assert(sizeof(XDebugChannelControl) == 0x1, "XDebugChannelControl");

