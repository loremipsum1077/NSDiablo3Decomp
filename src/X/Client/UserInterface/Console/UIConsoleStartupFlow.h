// Original file: X/Client/UserInterface/Console/UIConsoleStartupFlow.h
#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

namespace UIStartupFlow {
namespace Console {
enum {
    NO_STARTUP_SCREEN = -1,
    JAPANESE_KIDOJI_SCREEN = 0,
    JAPANESE_SHIYOJO_SCREEN = 1,
    JAPANESE_KENKO_SCREEN = 2,
    JAPANESE_SQEX_SCREEN = 3,
    RATINGS_SCREEN = 4,
    AUTOSAVEWARNING_SCREEN = 5,
};

typedef int32 StartupFlowScreens;

}  // namespace Console
}  // namespace UIStartupFlow
