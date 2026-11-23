// Original file: X/Common/Achievements/AchievementDefines.cpp
#include "AchievementDefines.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../Sigma/DataStructures/XVar.h"

namespace Achievement {

//@ 24
static XVarScope s_scopeAchievement("Achievement", NULL);

//@ 28
XVarUint32 arTimestamps[10] = {
    {s_scopeAchievement, "Blizzcon2014", 1414652400},
    {s_scopeAchievement, "LegacyOfTheVoid", 1446537600},
    {s_scopeAchievement, "Blizzcon2015", 1446422400},
    {s_scopeAchievement, "Legion", 1452470400},
    {s_scopeAchievement, "Overwatch", 1461024000},
    {s_scopeAchievement, "Blizzcon2016", 1477267260},
    {s_scopeAchievement, "EventWeArentDoingNow", 1495411200},
    {s_scopeAchievement, "Blizzcon2017", 1508832000},
    {s_scopeAchievement, "StarCraft20thAnniversary", 1520323200},
    {s_scopeAchievement, "Blizzcon2018", 1540368000},
};

// The heal-type and pet-type enums are not in this file's DWARF; the values are the ones the
// original tests (pet values 10..16 are PETTYPE_GENERIC_PET_0 and up).
//@ 158
BOOL IsHealTypeEvaluable(uint32 eHealType)
{
    switch (eHealType)
    {
    case 0:
    case 3:
    case 4:
    case 5:
        return TRUE;
    }
    return eHealType == 1;
}

//@ 170
BOOL IsPetTypeEvaluable(uint32 ePetType)
{
    switch (ePetType)
    {
    case 0:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        return TRUE;
    }
    return ePetType == 16;
}

}  // namespace Achievement
