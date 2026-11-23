// Original file: X/Server/Achievements/AchievementsEngine/achievements_listener.h

#pragma once

#include "../../../Common/OnlineService/OnlineService.h"
#include "achievements_update_structs.h"

namespace OnlineService {
namespace AchievementsEngine {
// CriteriaDeleteRecord and AchievementUpdateRecord (a typedef of Achievement) come from
// achievements_update_structs.h.

class Listener {  // size 0x18
    // vtable pointer
public:
    virtual void OnCriteriaUpdate(const OnlineService::AchievementsEngine::CriteriaUpdateRecord &) = 0;  // vtable slot 0
    virtual void OnCriteriaDeleted(const OnlineService::AchievementsEngine::CriteriaDeleteRecord &) = 0;  // vtable slot 1
    virtual void OnAchievementUpdate(const OnlineService::AchievementsEngine::AchievementUpdateRecord &) = 0;  // vtable slot 2
    virtual void SendAccumulatedUpdates();  // vtable slot 3
    virtual void EnableNotifications();  // vtable slot 4
    virtual const bool NotificationsEnabled() const;  // vtable slot 5
    const OnlineService::GameAccountHandle & GetGameAccount() const;
protected:
    Listener(const OnlineService::GameAccountHandle &);
    virtual ~Listener();  // vtable slot 0
    OnlineService::GameAccountHandle m_gameAccount;  // 0x08
    bool m_notificationsEnabled;  // 0x14
};
static_assert(sizeof(Listener) == 0x18, "Listener");

}  // namespace AchievementsEngine
}  // namespace OnlineService
