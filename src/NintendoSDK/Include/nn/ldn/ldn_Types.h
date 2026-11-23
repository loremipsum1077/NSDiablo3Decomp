// Adopted from the DWARF reference; hand-maintained.
// Original file: NintendoSDK/Include/nn/ldn/ldn_Types.h

#pragma once

#include "ldn_Ipv4Address.h"
#include "ldn_MacAddress.h"
#include "ldn_NetworkId.h"
#include "ldn_Ssid.h"
#include "../nn_BitTypes.h"
#include <stddef.h>
#include <stdint.h>

namespace nn {
namespace ldn {
enum State {
    State_None = 0,
    State_Initialized = 1,
    State_AccessPoint = 2,
    State_AccessPointCreated = 3,
    State_Station = 4,
    State_StationConnected = 5,
    State_Error = 6,
};

struct NetworkConfig {  // size 0x20
    nn::ldn::IntentId intentId;  // 0x00
    int16_t channel;  // 0x10
    int8_t nodeCountMax;  // 0x12
    nn::Bit8 _reserved1[1];  // 0x13
    int16_t localCommunicationVersion;  // 0x14
    nn::Bit8 _reserved2[10];  // 0x16
};
static_assert(sizeof(NetworkConfig) == 0x20, "NetworkConfig");

struct UserConfig {  // size 0x30
    char userName[33];  // 0x00
    nn::Bit8 _reserved[15];  // 0x21
};
static_assert(sizeof(UserConfig) == 0x30, "UserConfig");

enum SecurityMode {
    SecurityMode_Any = 0,
    SecurityMode_Product = 1,
    SecurityMode_Debug = 2,
    SecurityMode_SystemDebug = 3,
};

struct SecurityConfig {  // size 0x44
    nn::Bit16 securityMode;  // 0x00
    uint16_t passphraseSize;  // 0x02
    nn::Bit8 passphrase[64];  // 0x04
};
static_assert(sizeof(SecurityConfig) == 0x44, "SecurityConfig");

enum ScanFilterFlag {
    ScanFilterFlag_LocalCommunicationId = 1,
    ScanFilterFlag_SessionId = 2,
    ScanFilterFlag_NetworkType = 4,
    ScanFilterFlag_Bssid = 8,
    ScanFilterFlag_Ssid = 16,
    ScanFilterFlag_SceneId = 32,
    ScanFilterFlag_IntentId = 33,
    ScanFilterFlag_NetworkId = 35,
    ScanFilterFlag_All = 63,
};

struct ScanFilter {  // size 0x60
    nn::ldn::NetworkId networkId;  // 0x00
    nn::Bit32 networkType;  // 0x20
    nn::ldn::MacAddress bssid;  // 0x24
    nn::ldn::Ssid ssid;  // 0x2a
    nn::Bit8 _reserved[16];  // 0x4c
    nn::Bit32 flag;  // 0x5c
};
static_assert(sizeof(ScanFilter) == 0x60, "ScanFilter");

struct CommonNetworkInfo {  // size 0x30
    nn::ldn::MacAddress bssid;  // 0x00
    nn::ldn::Ssid ssid;  // 0x06
    int16_t channel;  // 0x28
    int8_t linkLevel;  // 0x2a
    nn::Bit8 networkType;  // 0x2b
    nn::Bit8 _reserved[4];  // 0x2c
};
static_assert(sizeof(CommonNetworkInfo) == 0x30, "CommonNetworkInfo");

struct NodeInfo {  // size 0x40
    nn::ldn::Ipv4Address ipv4Address;  // 0x00
    nn::ldn::MacAddress macAddress;  // 0x04
    int8_t nodeId;  // 0x0a
    bool isConnected;  // 0x0b
    char userName[33];  // 0x0c
    nn::Bit8 _reserved1[1];  // 0x2d
    int16_t localCommunicationVersion;  // 0x2e
    nn::Bit8 _reserved2[16];  // 0x30
};
static_assert(sizeof(NodeInfo) == 0x40, "NodeInfo");

struct LdnNetworkInfo {  // size 0x430
    nn::Bit8 serverRandom[16];  // 0x00
    nn::Bit16 securityMode;  // 0x10
    nn::Bit8 stationAcceptPolicy;  // 0x12
    nn::Bit8 _reserved1[3];  // 0x13
    int8_t nodeCountMax;  // 0x16
    int8_t nodeCount;  // 0x17
    nn::ldn::NodeInfo nodes[8];  // 0x18
    nn::Bit8 _reserved2[2];  // 0x218
    uint16_t advertiseDataSize;  // 0x21a
    nn::Bit8 advertiseData[384];  // 0x21c
    nn::Bit8 _reserved3[148];  // 0x39c
};
static_assert(sizeof(LdnNetworkInfo) == 0x430, "LdnNetworkInfo");

struct NetworkInfo {  // size 0x480
    nn::ldn::NetworkId networkId;  // 0x00
    nn::ldn::CommonNetworkInfo common;  // 0x20
    nn::ldn::LdnNetworkInfo ldn;  // 0x50
};
static_assert(sizeof(NetworkInfo) == 0x480, "NetworkInfo");

}  // namespace ldn
}  // namespace nn

// Variables defined in this file:
//   line    44  static const int nn::ldn::AutoChannel
//   line    64  static const size_t nn::ldn::UserNameBytesMax
//   line    84  static const int nn::ldn::ScanResultCountMax
