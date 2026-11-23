// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/OnlineService/OnlineServiceServer.h
// License and session structs on the DWARF layouts; the ctors are defined in the .cpp.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Sigma/DataStructures/XFourCC.h"

namespace D3 { namespace OnlineServiceServer { class License; } }
namespace google { namespace protobuf { class Message; } }

namespace OnlineServiceServer {
typedef uint64 LicenseId;

struct License {  // size 0x28
    License();
    OnlineServiceServer::LicenseId id;  // 0x00
    Blizzard::Time::Timestamp expireTime;  // 0x08
    uint64 transactionId;  // 0x10
    bool consumed;  // 0x18
    bool has_consumed;  // 0x19
    bool revoked;  // 0x1a
    uint64 instanceId;  // 0x20
    void Import(const D3::OnlineServiceServer::License &);
    void Export(D3::OnlineServiceServer::License &) const;
};
static_assert(sizeof(License) == 0x28, "License");

typedef blz::vector<OnlineServiceServer::License, blz::allocator<OnlineServiceServer::License> > LicenseArray;

typedef OnlineService::Callback<void (int)> GenericCallback;

typedef OnlineService::Callback<void (int, const google::protobuf::Message *)> ProtoMessageCallback;

struct ServiceSessionInfo {  // size 0x38
    ServiceSessionInfo();
    OnlineService::Timestamp tStartTime;  // 0x00
    XFourCC tCountryCode;  // 0x08
    bool bPlayingInIGR;  // 0x0c
    bool bHasBenefactor;  // 0x0d
    bool bUsingParentalControls;  // 0x0e
    blz::string tIPAddress;  // 0x10
};
static_assert(sizeof(ServiceSessionInfo) == 0x38, "ServiceSessionInfo");

Blizzard::Time::Timestamp GetCurrentTime();  // OnlineServiceServer.cpp:1426

}  // namespace OnlineServiceServer
