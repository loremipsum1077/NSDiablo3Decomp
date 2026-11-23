// Adopted from the DWARF reference; hand-maintained.
// Original file: NintendoSDK/Include/nn/ldn/ldn_Ssid.h

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace nn {
namespace ldn {
struct Ssid {  // size 0x22
    uint8_t length;  // 0x00
    char raw[33];  // 0x01
};
static_assert(sizeof(Ssid) == 0x22, "Ssid");

}  // namespace ldn
}  // namespace nn
