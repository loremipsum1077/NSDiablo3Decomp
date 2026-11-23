// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/OnlineService/SStore.h

#pragma once

#include "../../../Contrib/Contrib/blz/include/blz/string.h"

namespace OnlineService {
enum class StoreEscrowState : uint32 {
    STORE_ESCROW_STATE_AUTHORIZED = 0,
    STORE_ESCROW_STATE_DEPOSITED = 1,
    STORE_ESCROW_STATE_CANCELED = 2,
};
}  // namespace OnlineService


































































































namespace OnlineService {
namespace StoreHelpers {

static const blz::string STATUS_METADATA_TAG = "store_status";
}  // namespace StoreHelpers
}  // namespace OnlineService

// Functions defined in this file (bodies are not in the DWARF):
//   line   116  static void __cxx_global_var_init.3()  [inline]

// Variables defined in this file:
//   line   116  static const blz::string OnlineService::StoreHelpers::STATUS_METADATA_TAG
