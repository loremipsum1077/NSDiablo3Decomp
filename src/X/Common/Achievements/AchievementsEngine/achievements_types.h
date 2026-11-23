#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

namespace D3 { namespace Achievements { class CriteriaUpdateRecord; } }

namespace OnlineService {

// OnlineService::CriteriaId is owned by X/Common/OnlineService/Achievements.h;
// repeated here (same underlying type) so this header is self-contained.
typedef uint64 CriteriaId;

namespace AchievementsEngine {

uint32 MakeCriteria32AndFlags8(const CriteriaId criteriaId, const uint32 flags);
CriteriaId ExtractCriteriaId64(const D3::Achievements::CriteriaUpdateRecord & criteria);
uint32 ExtractCriteriaId32(const CriteriaId criteriaId);
CriteriaId Make64BitCriteriaId(uint32 id);

}  // namespace AchievementsEngine

}  // namespace OnlineService
