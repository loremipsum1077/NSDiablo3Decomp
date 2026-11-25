extern "C" void sub_7100a244b0();
extern "C" void sub_7100a24520();
#include "Sigma.h"

#include "../../BlizzardCore/Include/Blizzard/Using.h"
#include "DataStructures/DataArraysTemplated.h"
#include "DataStructures/XVar.h"
#include "ErrorManager/ErrorManager.h"
#include "Memory/SigmaMemory.h"

// These belong to headers in other areas that do not exist in src/ yet.
void XRegistryDispose();  // X/Sigma/Files/XRegistryStub.cpp
void SFileShutdown();     // X/Common/Console/IO/SFileWrapper.cpp

SigmaGlobals gtSigmaGlobals;

SigmaPlatform geSigmaPlatform;

void SigmaInit(LPCSTR szApplicationName, uint32 dwInspectorProjectID) {
    SigmaMemoryInit();
    XVarInitialize();
    DataArrayTrackerInit();
    sub_7100a244b0();
    ErrorManagerInitialize(szApplicationName, dwInspectorProjectID);
    Blizzard::Using::StartUsing("ErrorManager::Logging");
}

void SigmaDispose() {
    XRegistryDispose();
    ErrorManagerDispose();
    sub_7100a24520();
    DataArrayTrackerDispose();
    SFileShutdown();
    XVarDispose();
    SigmaMemoryDispose();
    Blizzard::Using::StopUsing("ErrorManager::Logging");
}
