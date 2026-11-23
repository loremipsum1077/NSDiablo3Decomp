// Reference: X/Client/CDebugDraw.cpp
//   line   659  void CDebugDrawInitialize()  [0x7100348de0]
//   line   660  void CDebugDrawDispose()  [0x7100348df0]
//   line   818  void CDebugDrawProcessMessage(DebugDrawPrimMessage & tMessage)  [0x7100348e00]
//
// Only these three functions are attributed to this file. They are empty stubs in the
// original (each 4 bytes: a single return).

// X/Common/Networking/GameMessage.h: the adopted copy is a subset and does not yet define
// this message, so it is declared here. The body never uses the parameter.
struct DebugDrawPrimMessage;

void CDebugDrawInitialize()
{
}

void CDebugDrawDispose()
{
}

void CDebugDrawProcessMessage(DebugDrawPrimMessage & tMessage)
{
}
