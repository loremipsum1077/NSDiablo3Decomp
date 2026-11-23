// Original file: X/Common/Callouts.cpp

#include "Callouts.h"
#include "../Server/Powers/PowerAPI.h"
#include "../Server/Powers/Powers.h"
#include "../Sigma/Math/MathGeneral.h"
#include "ActorCommonData.h"
#include "Definitions/GlobalDefinition.h"
#include "Definitions/PowerDefinition.h"
#include "Game.h"
#include "GameCommonData.h"
#include "GlobalSNO.h"
#include "Player.h"
#include "Powers/PowersCommon.h"
#include "TagMaps/PowerTags.h"
#include "Target.h"

namespace Callouts {




static const uint32 NUM_MASSACRE_LEVELS = 10;


static MassacreDef gstMassacreDefs[NUM_MASSACRE_LEVELS] = {
    { 0, 10, 10, 1.0f },
    { 1, 20, 20, 1.0f },
    { 2, 30, 30, 1.0f },
    { 3, 40, 40, 1.0f },
    { 4, 50, 50, 1.0f },
    { 5, 75, 75, 1.0f },
    { 6, 100, 100, 1.0f },
    { 7, 150, 150, 1.0f },
    { 8, 200, 200, 1.0f },
    { 9, 300, 300, 1.0f },
};



















MassacreDef GetMassacreDef(const uint32 nKills)
{
    uint32 nLevel = 0;

    while (nLevel + 1 < NUM_MASSACRE_LEVELS)
    {
        uint32 nMinKillCount = gstMassacreDefs[nLevel + 1].nMinKillCount;
        if (PlayerGetNumPlayers() > 1)
        {
            nMinKillCount = gstMassacreDefs[nLevel + 1].nMinKillCountMultiplayer;
        }


        if (nKills < nMinKillCount)
            break;
        nLevel++;
    }
    return gstMassacreDefs[nLevel];
}

float GetMassacreXPMultiplier(const uint32 nKills)

{
    uint32 nMassacreLevel = GetMassacreDef(nKills).nLevel;
    ScopedGlobalDefinitionLockR pdefGlobals;
    switch (nMassacreLevel)
    {
    case 0:
        return pdefGlobals->flMassacreXPBonusLevel0;
    case 1:
        return pdefGlobals->flMassacreXPBonusLevel1;
    case 2:
        return pdefGlobals->flMassacreXPBonusLevel2;
    case 3:
        return pdefGlobals->flMassacreXPBonusLevel3;
    case 4:
        return pdefGlobals->flMassacreXPBonusLevel4;
    case 5:
        return pdefGlobals->flMassacreXPBonusLevel5;
    case 6:
        return pdefGlobals->flMassacreXPBonusLevel6;
    case 7:
        return pdefGlobals->flMassacreXPBonusLevel7;
    case 8:
        return pdefGlobals->flMassacreXPBonusLevel8;
    case 9:
        return pdefGlobals->flMassacreXPBonusLevel9;
    }


    return 1.0f;
}
BOOL IsSharedCounterType(const KillCounterType eType)

{
    return eType == 0 || eType == 1;
}






uint32 GetMassacreMinCount()
{
    ScopedGlobalDefinitionLockR pdefGlobals;

    if (PlayerGetNumPlayers() > 1)
    {
        return pdefGlobals->nMassacreMinCountCoop;
    }


    return pdefGlobals->nMassacreMinCountSolo;
}



int32 ApplyEnvironmentKillBuff(Player & tPlayer,
                               uint32 nBonus)
{
    int32 nDuration = 0;






    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return 0;
    }

    SNO snoEnvironmentKillBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_ENVIRONMENTKILL_BUFF_RESOURCE_REGEN);
    ScopedPowerDefinitionLockR pdefEnvironmentKillBuffPower(snoEnvironmentKillBuffPower);

    BuffID idBuff = PowerAPI_BuffFindOnACD(snoEnvironmentKillBuffPower, 0, *ptACDPlayer, ACDID_INVALID, FALSE);
    if (idBuff == BUFFID_INVALID)
    {
        PowerStart(ptACDPlayer->id, snoEnvironmentKillBuffPower, cgTargetNone);
        idBuff = PowerAPI_BuffFindOnACD(snoEnvironmentKillBuffPower, 0, *ptACDPlayer, ACDID_INVALID, FALSE);
    }
    else
    {

        nBonus++;
    }

    if (idBuff != BUFFID_INVALID)
    {
        const uint32 nBonusDuration = 60 * Math::FloatToInt(PowerGetFormulaValue(ptACDPlayer, pdefEnvironmentKillBuffPower, TAG_POWER_SCRIPT_FORMULA_2, -1, -1, 0));
        const uint32 nDurationMinimum = 60 * Math::FloatToInt(PowerGetFormulaValue(ptACDPlayer, pdefEnvironmentKillBuffPower, TAG_POWER_SCRIPT_FORMULA_1, -1, -1, 0));
        const uint32 nDurationMaximum = 60 * Math::FloatToInt(PowerGetFormulaValue(ptACDPlayer, pdefEnvironmentKillBuffPower, TAG_POWER_SCRIPT_FORMULA_3, -1, -1, 0));


        uint32 nDurationDelta = nBonus * nBonusDuration;

        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        uint32 dwNow = GameGetServerTime();
        uint32 nTicksLeft = ptBuff->dwDuration - (dwNow - ptBuff->dwStartTick);
        uint32 nNewDuration = nTicksLeft + nDurationDelta;


        if (nNewDuration < nDurationMinimum)
        {
            nDurationDelta = nDurationMinimum - nTicksLeft;
        }
        else if (nNewDuration > nDurationMaximum)
        {
            nDurationDelta = nDurationMaximum - nTicksLeft;
        }


        PowerAPI_BuffModifyDuration(idBuff, nDurationDelta, FALSE, TRUE);
        nNewDuration = nTicksLeft + nDurationDelta;
        nDuration = (int32)nNewDuration / 60;
    }


    return nDuration;
}

void ApplyDestructionBuff(Player & tPlayer,
                          uint32 nCount)
{




    SNO snoDestructionBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DESTRUCTIONSTREAK_BUFF_RUN_SPEED);
    ScopedPowerDefinitionLockR pdefDestructionBuffPower(snoDestructionBuffPower);
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    uint32 nMinimumDestructionCount = Math::FloatToInt(PowerGetFormulaValue(&tACDPlayer, pdefDestructionBuffPower, TAG_POWER_SCRIPT_FORMULA_3, -1, -1, 0));
    if (nCount == nMinimumDestructionCount)
    {
        for (Player * ptPotentialLocalPlayer = PlayerGetFirstInGame(); ptPotentialLocalPlayer; ptPotentialLocalPlayer = PlayerGetNextInGame(ptPotentialLocalPlayer))
        {
            if (PlayersAreLocalToEachOther(tPlayer, *ptPotentialLocalPlayer))
            {
                ActorCommonData * ptACDPlayerWithBuff = ACDTryToGet(ptPotentialLocalPlayer->idPlayerACD);


                if (ptACDPlayerWithBuff && PowerAPI_BuffFindOnACD(snoDestructionBuffPower, ptACDPlayerWithBuff, ACDID_INVALID) == BUFFID_INVALID)
                {
                    PowerStart(ptPotentialLocalPlayer->idPlayerACD, snoDestructionBuffPower, cgTargetNone);
                }
            }
        }
    }
}
}  // namespace Callouts
