// Original file: X/Common/ChallengeRift.cpp

#include "ChallengeRift.h"

#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Attributes/FastAttribConstantsSwig.h"
#include "Definitions/AttachmentDefinition.h"
#include "Definitions/HeroDefinition.h"
#include "Game.h"
#include "GameCommonData.h"
#include "Player.h"
#include "SNO/SNOFiles.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include <new>

// Defined in X/Common/Game.cpp; no adopted header declares it yet.
XMemoryAllocatorInterface * AppGetCurrentMemoryInterface();

struct SkillAndRune {  // size 0x8
    SNO snoSkill;  // 0x00
    RuneType eRuneType;  // 0x04
};

struct ChallengeRiftGlobals {  // size 0x18 (line 24)
    blz::vector<SkillAndRune, blz::allocator<SkillAndRune> > tSkillsSnapshot;  // 0x00
};

namespace ChallengeRift {

void InitializeForGame()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    ChallengeRiftGlobals *& ptChallengeRiftGlobals = tGCD.ptChallengeRiftGlobals;
    ptChallengeRiftGlobals = new (SigmaMemoryNew(sizeof(ChallengeRiftGlobals), 4, ptMemoryInterface, FALSE)) ChallengeRiftGlobals();
}

void DisposeForGame()
{
    ChallengeRiftGlobals *& ptChallengeRiftGlobals = GameGetActiveGameCommonData().ptChallengeRiftGlobals;
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    ptChallengeRiftGlobals->~ChallengeRiftGlobals();
    SigmaMemoryFree(ptChallengeRiftGlobals, ptMemoryInterface);
    ptChallengeRiftGlobals = NULL;
}

ChallengeRiftPersonalBest LeaderboardIdToChallengeRiftPersonalBest(LeaderboardId leaderboardId)
{
    switch (leaderboardId)
    {
    case LB_CHALLENGE_RIFT_TEAM_1:
        return CRPB_TEAM_1;
    case LB_CHALLENGE_RIFT_TEAM_2:
        return CRPB_TEAM_2;
    case LB_CHALLENGE_RIFT_TEAM_3:
        return CRPB_TEAM_3;
    case LB_CHALLENGE_RIFT_TEAM_4:
        return CRPB_TEAM_4;
    default:
        return CRPB_COUNT;
    }
}

ChallengeRiftPersonalBest NumPlayersToChallengeRiftPersonalBest(uint32 numPlayers)
{
    switch (numPlayers)
    {
    case 1:
        return CRPB_TEAM_1;
    case 2:
        return CRPB_TEAM_2;
    case 3:
        return CRPB_TEAM_3;
    case 4:
        return CRPB_TEAM_4;
    default:
        return CRPB_COUNT;
    }
}

void SnapshotPlayerSkills(const Player & tPlayer)
{
    ChallengeRiftGlobals * ptChallengeRiftGlobals = GameGetActiveGameCommonData().ptChallengeRiftGlobals;
    if (ptChallengeRiftGlobals == NULL)
    {
        return;
    }

    ptChallengeRiftGlobals->tSkillsSnapshot.clear();
    for (int i = 0; i < NUM_SKILL_SLOTS; ++i)
    {
        SNO snoSkill = tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].snoSkill;
        if (snoSkill != SNO_NONE)
        {
            SkillAndRune tSkill;
            tSkill.snoSkill = snoSkill;
            tSkill.eRuneType = tPlayer.tHeroStateData.tSavedData.tActiveSkills[i].eRuneType;
            ptChallengeRiftGlobals->tSkillsSnapshot.push_back(tSkill);
        }
    }
}

BOOL IsValidChallengeRiftSkill(SNO snoSkill)
{
    ChallengeRiftGlobals * ptChallengeRiftGlobals = GameGetActiveGameCommonData().ptChallengeRiftGlobals;
    if (ptChallengeRiftGlobals == NULL)
    {
        return FALSE;
    }

    for (SkillAndRune * iter = ptChallengeRiftGlobals->tSkillsSnapshot.begin(); iter != ptChallengeRiftGlobals->tSkillsSnapshot.end(); ++iter)
    {
        if (iter->snoSkill == snoSkill)
        {
            return TRUE;
        }
    }
    return FALSE;
}

RuneType GetRuneTypeForSkill(SNO snoSkill)
{
    ChallengeRiftGlobals * ptChallengeRiftGlobals = GameGetActiveGameCommonData().ptChallengeRiftGlobals;
    if (ptChallengeRiftGlobals == NULL)
    {
        return RUNE_TYPE_NONE;
    }

    for (SkillAndRune * iter = ptChallengeRiftGlobals->tSkillsSnapshot.begin(); iter != ptChallengeRiftGlobals->tSkillsSnapshot.end(); ++iter)
    {
        if (iter->snoSkill == snoSkill)
        {
            return iter->eRuneType;
        }
    }
    return RUNE_TYPE_NONE;
}

}  // namespace ChallengeRift
