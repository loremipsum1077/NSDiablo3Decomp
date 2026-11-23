// Original file: X/Client/UserInterface/Console/UIConsoleGameSettings.h
#pragma once

#include "../../../Common/OnlineService/Callback/Callback_Impl.h"
namespace UIGameSettings { namespace Console {
typedef OnlineService::Callback<void(int)> ScreenClosedCallback;
void OpenForTags(int32 nControllerIndex, ScreenClosedCallback cb);  // UIConsoleGameSettings.cpp:839
} }
namespace UIGameSettings { namespace Console {
void OpenForDifficultySelect(int32 nControllerIndex, ScreenClosedCallback cb);  // UIConsoleGameSettings.cpp:816
void OpenForMatchmaking(int32 nControllerIndex, ScreenClosedCallback cb);  // UIConsoleGameSettings.cpp:828
void Close();  // UIConsoleGameSettings.cpp:857
BOOL IsVisible();  // UIConsoleGameSettings.cpp:900
BOOL GameSettingsViewed();  // UIConsoleGameSettings.cpp:961
} }
