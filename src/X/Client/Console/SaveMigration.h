// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/SaveMigration.h

#pragma once

#include "../../Common/OnlineService/Callback/Callback_Impl.h"

namespace Console {
namespace SaveMigration {
enum Result {
    SAVE_MIGRATION_SUCCESS = 0,
    SAVE_MIGRATION_SUCCESS_SKIPPED = 1,
    SAVE_MIGRATION_SUCCESS_NOT_FOUND = 2,
    SAVE_MIGRATION_FAILED = 3,
    SAVE_MIGRATION_FAILED_DATA_CORRUPT = 4,
    SAVE_MIGRATION_FAILED_SAVE_CORRUPT = 5,
    SAVE_MIGRATION_FAILED_UNOWNED_SAVE = 6,
    SAVE_MIGRATION_FAILED_NOT_SIGNED_IN = 7,
    SAVE_MIGRATION_FAILED_NO_PERMISSION = 8,
    SAVE_MIGRATION_FAILED_UNABLE_TO_CONNECT = 9,
};

enum {
    SEASONAL_ROLLOVER_ITEM_DURATION_SECONDS = 2592000,
    CHALLENGE_REWARD_DURATION_SECONDS = 2592000,
};

typedef OnlineService::Callback<void (Console::SaveMigration::Result)> SaveMigrationCallback;

}  // namespace SaveMigration
}  // namespace Console
