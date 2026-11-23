// Original file: X/Server/Achievements/AchievementsEngine/achievements_listener.cpp

#include "achievements_listener.h"

namespace OnlineService {
namespace AchievementsEngine {








Listener::Listener(const OnlineService::GameAccountHandle & gameAccount)
    : m_gameAccount(gameAccount)
    , m_notificationsEnabled(false)
{
}

Listener::~Listener()
{
}

void Listener::SendAccumulatedUpdates()
{
}

const bool Listener::NotificationsEnabled() const
{
    return m_notificationsEnabled;
}

void Listener::EnableNotifications()
{
    m_notificationsEnabled = true;
}

}  // namespace AchievementsEngine
}  // namespace OnlineService
