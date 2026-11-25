#include "achievements_types.h"

#include "../../Protobuf/Achievements.pb.h"

namespace OnlineService {
namespace AchievementsEngine {

static const uint64 PRODUCT_AND_TEAM_HEADER = 0x443354000000;

uint32 MakeCriteria32AndFlags8(const CriteriaId criteriaId, const uint32 flags) {
    const uint32 criteriaId24 = (uint32)(criteriaId & 0xffffff);
    const uint32 flags8 = flags;
    return criteriaId24 | (flags8 << 24);
}

CriteriaId ExtractCriteriaId64(const D3::Achievements::CriteriaUpdateRecord & criteria) {
    const uint32 criteriaId32 = criteria.criteria_id_32_and_flags_8() & 0xffffff;
    return Make64BitCriteriaId(criteriaId32);
}

uint32 ExtractCriteriaId32(const CriteriaId criteriaId) {
    const uint32 criteriaId24 = (uint32)(criteriaId & 0xffffff);
    return criteriaId24;
}

CriteriaId Make64BitCriteriaId(uint32 id) {
    const CriteriaId achievementId = PRODUCT_AND_TEAM_HEADER | ExtractCriteriaId32(id);
    return achievementId;
}

}  // namespace AchievementsEngine
}  // namespace OnlineService
