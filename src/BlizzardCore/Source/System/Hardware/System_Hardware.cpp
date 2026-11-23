// Original file: BlizzardCore/Source/System/Hardware/System_Hardware.cpp
// The original is a unity file that includes Hardware_NX64.cpp (the DWARF decl_file of every
// function). The bodies are written here at their lines in that file.
#include "../../../Include/Blizzard/Hardware.h"
#include <string.h>

// NintendoSDK declaration (nn/os/os_Tick.h).
namespace nn {
namespace os {
int64_t GetSystemTickFrequency();
}  // namespace os
}  // namespace nn

namespace Blizzard {
namespace Hardware {




uint64 CpuSpeed()
{
    const int64_t result = nn::os::GetSystemTickFrequency();
    return result;
}


































void GetSystemManufacturer(char * manufacturer, int length)
{
    memset(manufacturer, 0, length);
}

void GetSystemModel(char * model, int length)
{
    memset(model, 0, length);
}

void GetProcessorVendorString(char * vendor, int length)
{
    memset(vendor, 0, length);
}

void GetProcessorBrandString(char * brand, int length)
{
    memset(brand, 0, length);
}

bool ExtendedProcessorFeatures(ProcessorFeatures & features)
{
    static ProcessorFeatures s_features;

    s_features.numCores = 3;
    s_features.numHWThreads = 3;
    s_features.processorVendor = PROCESSOR_VENDOR_ARM;
    s_features.processorFeatures = 0;
    s_features.packageCount = 1;

    features = s_features;
    return true;
}

}  // namespace Hardware
}  // namespace Blizzard
