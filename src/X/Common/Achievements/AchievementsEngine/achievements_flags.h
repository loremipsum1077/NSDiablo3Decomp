// Adopted from the DWARF.
// Original file: X/Common/Achievements/AchievementsEngine/achievements_flags.h

#pragma once

namespace OnlineService {
namespace AchievementsEngine {
enum AchievementStaticFlags {
    ACHIEVEMENT_FLAG_STATISTIC = 1,
    ACHIEVEMENT_FLAG_CUMULATIVE = 2,
    ACHIEVEMENT_FLAG_TRACK_LOCAL_UNTIL_AWARDED = 4,
};

enum CriteriaFlag {
    CRITERIA_FLAG_FAIL_ACHIEVEMENT = 1,
    CRITERIA_FLAG_RESET_ON_START = 2,
};

}  // namespace AchievementsEngine
}  // namespace OnlineService
