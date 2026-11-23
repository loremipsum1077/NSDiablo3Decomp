// Original file: X/Client/UserInterface/Console/UIConsoleFriendsList.h
#pragma once
#include "../../../Common/OnlineService/Callback/Callback_Impl.h"
namespace Console { namespace Online { struct Friend; } }
namespace UIFriendsList { namespace Console {
typedef OnlineService::Callback<void (::Console::Online::Friend &)> FriendsListAcceptedCallback;
typedef OnlineService::Callback<void ()> FriendsListCancelledCallback;
} }
