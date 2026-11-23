// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Include/Blizzard/Hardware.h

#pragma once

#include "Types.h"

namespace Blizzard {
namespace Hardware {
enum ProcessorVendor {
    PROCESSOR_VENDOR_UNKNOWN = 0,
    PROCESSOR_VENDOR_INTEL = 1,
    PROCESSOR_VENDOR_AMD = 2,
    PROCESSOR_VENDOR_PPC = 3,
    PROCESSOR_VENDOR_VIA = 4,
    PROCESSOR_VENDOR_ARM = 5,
};

struct ProcessorFeatures {  // size 0x2c
    uint32 model;  // 0x00
    uint32 extmodel;  // 0x04
    uint32 family;  // 0x08
    uint32 extfamily;  // 0x0c
    uint32 stepping;  // 0x10
    uint32 signature;  // 0x14
    int32 processorVendor;  // 0x18
    uint32 processorFeatures;  // 0x1c
    int32 numCores;  // 0x20
    int32 numHWThreads;  // 0x24
    int32 packageCount;  // 0x28
};
static_assert(sizeof(ProcessorFeatures) == 0x2c, "ProcessorFeatures");

uint64 CpuSpeed();
void GetSystemManufacturer(char * manufacturer, int length);
void GetSystemModel(char * model, int length);
void GetProcessorVendorString(char * vendor, int length);
void GetProcessorBrandString(char * brand, int length);
bool ExtendedProcessorFeatures(ProcessorFeatures & features);

}  // namespace Hardware
}  // namespace Blizzard
