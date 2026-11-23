// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/friends/friends_Types.h

#pragma once

#include "../settings/settings_Language.h"
#include "../account/account_Types.h"
#include "../nn_BitTypes.h"

namespace nn {
namespace friends {
enum PresenceStatus {
    PresenceStatus_Offline = 0,
    PresenceStatus_Online = 1,
    PresenceStatus_OnlinePlay = 2,
};

enum PresenceStatusFilter {
    PresenceStatusFilter_None = 0,
    PresenceStatusFilter_Online = 1,
    PresenceStatusFilter_OnlinePlay = 2,
    PresenceStatusFilter_OnlineOrOnlinePlay = 3,
};

enum ImageSize {
    ImageSize_64x64 = 64,
    ImageSize_128x128 = 128,
    ImageSize_256x256 = 256,
    ImageSize_Standard = 256,
};

enum NotificationType {
    NotificationType_Unkown = 0,
    NotificationType_FriendListUpdated = 1,
    NotificationType_FriendPresenceUpdated = 2,
    NotificationType_FriendRequestReceived = 101,
};

struct InAppScreenName {  // size 0x48
    char name[64];  // 0x00
    nn::settings::LanguageCode language;  // 0x40
};
static_assert(sizeof(InAppScreenName) == 0x48, "InAppScreenName");

struct FriendFilter {  // size 0x10
    nn::friends::PresenceStatusFilter presenceStatus;  // 0x00
    bool isFavoriteOnly;  // 0x04
    bool isSameAppPresenceOnly;  // 0x05
    bool isSameAppPlayedOnly;  // 0x06
    bool isArbitraryAppPlayedOnly;  // 0x07
    nn::Bit64 presenceGroupId;  // 0x08
};
static_assert(sizeof(FriendFilter) == 0x10, "FriendFilter");

struct PlayHistoryRegistrationKey {  // size 0x40
    nn::Bit8 value[64];  // 0x00
};
static_assert(sizeof(PlayHistoryRegistrationKey) == 0x40, "PlayHistoryRegistrationKey");

struct NotificationInfo {  // size 0x10
    nn::friends::NotificationType type;  // 0x00
    nn::account::NetworkServiceAccountId accountId;  // 0x08
};
static_assert(sizeof(NotificationInfo) == 0x10, "NotificationInfo");

}  // namespace friends
}  // namespace nn

// Variables defined in this file:
//   line    27  static const int nn::friends::FriendCountMax
//   line    47  static const int nn::friends::GetProfileCountMax
