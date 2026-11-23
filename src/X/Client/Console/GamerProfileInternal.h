// Original file: X/Client/Console/GamerProfileInternal.h
// Subset: the save/load task thread and callback queue shared by GamerProfileInternal.cpp and the
// platform file (GamerProfileNX64.cpp still declares its own copy of this block).
#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"

namespace Console { namespace GamerProfile {
struct SaveTask;
typedef OnlineService::Callback<void ()> BeginSaveTaskCallback;
typedef OnlineService::Callback<void ()> EndSaveTaskCallback;
void RegisterSaveTaskCallbacks(Console::GamerProfile::BeginSaveTaskCallback pfnBeginSaveCallback, Console::GamerProfile::EndSaveTaskCallback pfnEndSaveCallback);
void PlatformInitializeForApp();  // GamerProfileNX64.cpp
void PlatformDisposeForApp();  // GamerProfileNX64.cpp
} }

namespace Console { namespace GamerProfile { namespace Internal {
typedef OnlineService::Callback<void ()> SaveLoadTaskCallback;
int32 GetPrimaryProfileUserIndex();
void SetPrimaryProfileUserIndex(int32 nUserIdx);
void HandleUserSignedOut(int32 nUserIdx);
void ClearUserNewChallengeNotifications(int32 nUserIdx);
void QueueCallback(Console::GamerProfile::Internal::SaveLoadTaskCallback pfnCallback);
void NotifyBeginSaveCallback();
void NotifyEndSaveCallback();
uint32 sAsyncSaveLoadThread(void * pParam);
} } }

void ConfigureConsoleSaveLoadThread(Blizzard::Thread::Thread tThread);  // X/Client/NX64 platform code



namespace Console { namespace GamerProfile { namespace Internal {
struct SaveLoadTask : public Blizzard::Lock::RefCounted {  // size 0x10
    SaveLoadTask() {}
    virtual ~SaveLoadTask() {}
    virtual void Process() = 0;  // vtable slot 2
    void Queue();
    virtual Console::GamerProfile::SaveTask * CastAsSaveTask() { return NULL; }  // vtable slot 3
    virtual BOOL IsCritical() const { return FALSE; }  // vtable slot 4
private:
    SaveLoadTask(const Console::GamerProfile::Internal::SaveLoadTask &);
    Console::GamerProfile::Internal::SaveLoadTask & operator=(const Console::GamerProfile::Internal::SaveLoadTask &);
};
typedef OnlineService::Callback<int (Console::GamerProfile::Internal::SaveLoadTask *)> TaskCompareFunc;
void RemoveTasks(Console::GamerProfile::Internal::TaskCompareFunc pfnCompare);
} } }

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(Console::GamerProfile::Internal::SaveLoadTask) == 0x10, "Console::GamerProfile::Internal::SaveLoadTask");
