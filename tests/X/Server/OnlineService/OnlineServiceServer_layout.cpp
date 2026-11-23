// Layout check for X/Server/OnlineService/OnlineServiceServer.h against the DWARF. Compiled,
// not compared.
#include "X/Server/OnlineService/OnlineServiceServer.h"

#include <cstddef>

using namespace OnlineServiceServer;

static_assert(sizeof(License) == 0x28, "License");
static_assert(offsetof(License, expireTime) == 0x08, "License::expireTime");
static_assert(offsetof(License, transactionId) == 0x10, "License::transactionId");
static_assert(offsetof(License, consumed) == 0x18, "License::consumed");
static_assert(offsetof(License, has_consumed) == 0x19, "License::has_consumed");
static_assert(offsetof(License, revoked) == 0x1a, "License::revoked");
static_assert(offsetof(License, instanceId) == 0x20, "License::instanceId");

static_assert(sizeof(ServiceSessionInfo) == 0x38, "ServiceSessionInfo");
static_assert(offsetof(ServiceSessionInfo, tCountryCode) == 0x08, "ServiceSessionInfo::tCountryCode");
static_assert(offsetof(ServiceSessionInfo, bPlayingInIGR) == 0x0c, "ServiceSessionInfo::bPlayingInIGR");
static_assert(offsetof(ServiceSessionInfo, bHasBenefactor) == 0x0d, "ServiceSessionInfo::bHasBenefactor");
static_assert(offsetof(ServiceSessionInfo, bUsingParentalControls) == 0x0e, "ServiceSessionInfo::bUsingParentalControls");
static_assert(offsetof(ServiceSessionInfo, tIPAddress) == 0x10, "ServiceSessionInfo::tIPAddress");
