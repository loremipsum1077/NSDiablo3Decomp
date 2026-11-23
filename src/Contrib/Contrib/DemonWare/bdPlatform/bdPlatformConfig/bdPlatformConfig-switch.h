// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformConfig/bdPlatformConfig-switch.h

#pragma once

// NintendoSDK includes and the nn::os handle typedefs (bdThreadProc, bdSemaphoreHandle,
// bdMutexHandle, bdThreadHandle) removed until the NintendoSDK headers are adopted.
#include <stddef.h>
#include <stdint.h>

typedef long unsigned int BD_ULONG;

typedef intptr_t BD_WORD;

typedef uintptr_t BD_UWORD;

typedef signed char BD_CHAR8;

typedef unsigned char BD_UCHAR8;

typedef int64_t BD_INT_64;

typedef uint64_t BD_UINT_64;

typedef int bdDNSHandle;


#include "../../../../../NintendoSDK/Include/nn/os/os_MutexTypes.h"
typedef nn::os::MutexType bdMutexHandle;
#include <memory>
#include "../../../../../NintendoSDK/Include/nn/os/os_SemaphoreTypes.h"
#include "../../../../../NintendoSDK/Include/nn/os/os_ThreadCommon.h"
#include "../../../../../NintendoSDK/Include/nn/os/os_ThreadTypes.h"
typedef nn::os::ThreadFunction bdThreadProc;
typedef std::unique_ptr<nn::os::SemaphoreType, std::default_delete<nn::os::SemaphoreType> > bdSemaphoreHandle;
typedef nn::os::ThreadType bdThreadHandle;
