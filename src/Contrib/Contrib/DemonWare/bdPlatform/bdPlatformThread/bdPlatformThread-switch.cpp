#include "bdPlatformThread.h"
#include "../bdPlatformLog/bdPlatformLog.h"
#include "../../../../../NintendoSDK/Include/nn/nn_Result.h"
// Blank lines keep the original line numbers

namespace nn { namespace os {
Result CreateThread(ThreadType*, ThreadFunction, void*, void*, size_t, int, int);
void StartThread(ThreadType*);
void WaitThread(ThreadType*);
void DestroyThread(ThreadType*);
} }






bdBool bdPlatformThread::createThread(bdThreadProc threadProc,
                                      void* args,
                                      bdThreadHandle& handle,
                                      bdInt priority,
                                      bdUInt stackSize,
                                      void* stackPointer,
                                      bdInt coreAffinity,
                                      const bdNChar8*)
{





    const nn::Result result = nn::os::CreateThread(&handle, threadProc, args, stackPointer, stackSize, priority, coreAffinity);
    if (result.IsFailure())
    {
        bdUseVAArgs(result.GetDescription());
    }

    return result.IsSuccess();
}

bdBool bdPlatformThread::startThread(bdThreadHandle& handle, void*, bdUInt)
{
    nn::os::StartThread(&handle);
    return true;
}

bdBool bdPlatformThread::nameThread(bdThreadHandle&, const bdNChar8*)
{

    return true;
}

void bdPlatformThread::joinThread(bdThreadHandle& handle)
{
    nn::os::WaitThread(&handle);
}





void bdPlatformThread::deleteThread(bdThreadHandle& handle)
{
    nn::os::DestroyThread(&handle);
}
