// Original file: X/Common/DebugChannel/XDebugChannelConsole.cpp
//
// The original file also carried these members, none of which is an out-of-line
// function assigned to this file (functions.csv lists only the two below):
//   line  28  static void sDebugChannelConsoleSend(const CHAR * szString)      [inline]
//   line  52  InitializedBOOL::InitializedBOOL()                              [inline]
//   line  61  static Blizzard::Thread::LocalObject<InitializedBOOL> tl_bDebugChannelLogInProgress
// They are file-local inline helpers / state used by the console command path;
// neither out-of-line function here references them, so they are not needed for
// the two assigned functions to match.

#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/DebugChannelInterface/XDebugChannelInterface.h"

void XDebugChannelConsoleInit()
{
    XDebugMessage tMsg(XDM_CONSOLE_ENABLE);
}

void XDebugChannelConsoleDispose()
{
}
