// Original file: X/Common/DebugChannel/XDebugChannelControlThreaded.cpp

#include "XDebugChannelControl.h"

// In this build XDebugChannelControl is an empty facade: each of the four
// out-of-line members below is a no-op and compiles to a single `ret`. The
// threaded implementation lives in XDebugChannelImpl in the same original
// translation unit.

XDebugChannelControl::XDebugChannelControl(const char * szClientName)
{
}

XDebugChannelControl::~XDebugChannelControl()
{
}

void XDebugChannelControl::StartListening(uint16 wListeningPort)
{
}

void XDebugChannelControl::Update()
{
}
