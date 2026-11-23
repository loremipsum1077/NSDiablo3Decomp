// Original file: X/Server/Ai/HirelingBrain.cpp
#include "../Actors/Actors.h"
#include "./ActorAI.h"
#include "./BrainComponents.h"
#include "./HirelingBrain.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SNPC.h"
#include "../Powers/Powers.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/AI/ActorAISwig.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Hireling.h"
#include "../../Common/HirelingSwig.h"
#include "../../Common/Player.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"

void ActorSetBrain(ActorID idActor, ActorBrain nBrainType, ActorSavedAI * ptSavedAI);  // X/Server/Ai/ActorAI.cpp
ActorBrain ActorGetBrainType(ActorID idActor);  // X/Server/Ai/ActorAI.cpp
void NPCHandleDismissal(ActorCommonData & tACDHireling, BOOL bPlayerDismissed, BOOL bDisplayChatMessage);  // X/Server/SNPC.cpp:747

struct TUpdateFunction {  // size 0x10
    HirelingClass eClass;  // 0x00
    HirelingClassUpdateFunc pfnUpdateFunc;  // 0x08
};

static void sPaladinUpdate(ActorID idActor);
static void sScoundrelUpdate(ActorID idActor);

extern const float MAX_CONVERSATION_DISTANCE;

//@ 98
static void sBrainDataInitialize(HirelingBrainData & tData)
{
//@ 102
    tData.idLeader = ACTORID_INVALID;

    tData.pfnChooseAttack = NULL;

//@ 108
    tData.eBrainState = HBS_NONE;
    for (int32 i = 0; i < MAX_HIRELING_SKILLS; i++)
    {
        tData.dwLastUseSkill[i] = 0;
    }
    tData.fAttacking = FALSE;
    tData.dwCurrentAttackStartTime = 0;
//@ 116
    tData.snoCurrentAttack = SNO_NONE;
    tData.idACDCurrentTarget = ACDID_INVALID;
    tData.idACDLastTarget = ACDID_INVALID;
    tData.idACDLastAttacker = ACDID_INVALID;
    tData.idACDLastPlayerTarget = ACDID_INVALID;
//@ 124
    tData.flDefaultSpeed = 1.0f;
    tData.flCurrentSpeed = 1.0f;
    tData.fChangedLastTime = FALSE;
    tData.dwDeathPowerNextTime = 0;
}

//@ 134
void HirelingBrainInitialize(const ActorID idHireling)
{
//@ 138
    ActorAI & tAI = ActorGetAI(idHireling);

    HirelingBrainData * ptData = (HirelingBrainData *)SigmaMemoryNew(sizeof(HirelingBrainData), 4, SGameManager::GameGet().ptMemoryInterface, TRUE);
    sBrainDataInitialize(*ptData);

//@ 149
    ActorCommonData & tACD = ACDGet(ActorGet(idHireling)->idACD);

    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_INVULNERABLE), 0);

//@ 154
    tAI.pBrainData = ptData;
}

//@ 161
void HirelingBrainDispose(const ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    SigmaMemoryFree(tAI.pBrainData, SGameManager::GameGet().ptMemoryInterface); tAI.pBrainData = NULL;
}

//@ 171
static HirelingBrainData * sGetBrainData(ActorID idActor)
{
//@ 174
    ActorAI & tAI = ActorGetAI(idActor);

    return (HirelingBrainData *)tAI.pBrainData;
}

//@ 198
static TUpdateFunction sgtUpdateFunctions[] = {
    { HC_TEMPLAR, sPaladinUpdate },
    { HC_SCOUNDREL, sScoundrelUpdate },
};
static const int32 sgnNumUpdateFunctions = sizeof(sgtUpdateFunctions) / sizeof(sgtUpdateFunctions[0]);

//@ 207
void HirelingBrainCreateHireling(ActorID idHireling, ActorID idLeader)
{
//@ 212
    ActorSetBrain(idHireling, ACTORBRAIN_HIRELING, NULL);

    if (ActorGetBrainType(idHireling) == ACTORBRAIN_DO_NOTHING)
    {
        return;
    }

    HirelingBrainData * ptData = sGetBrainData(idHireling);
//@ 222
    ptData->idLeader = idLeader;

//@ 226
    Actor * ptActor = ActorGet(idHireling);
    if (ptActor == NULL)
    {
        return;
    }
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    HirelingClass eClass = HirelingGetClass(tACD);

//@ 234
    ptData->pfnUpdateFunction = NULL;

    for (int32 i = 0; i < sgnNumUpdateFunctions; i++)
    {
        if (sgtUpdateFunctions[i].eClass == eClass)
        {
            ptData->pfnUpdateFunction = sgtUpdateFunctions[i].pfnUpdateFunc;
            break;
        }
    }
}

//@ 1770
void HirelingBrainClearAttack(ActorID idHireling)
{
    HirelingBrainData * ptData = sGetBrainData(idHireling);

//@ 1775
    if (ptData == NULL)
    {
        return;
    }
    ptData->fAttacking = FALSE;
    ptData->dwCurrentAttackStartTime = 0;
    ptData->snoCurrentAttack = SNO_NONE;
    ptData->idACDCurrentTarget = ACDID_INVALID;
}

//@ 1789
BOOL HirelingBrainOnDeath(const ActorID idActor, const ActorID idKillerActor)
{
    if (idActor == ACTORID_INVALID)
    {
        return FALSE;
    }

//@ 1798
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD == ACDID_INVALID)
    {
        return FALSE;
    }

    HirelingBrainData * ptData = sGetBrainData(idActor);
    if (ptData == NULL)
    {
        return FALSE;
    }

    BOOL bKillerWasOwner = FALSE;

//@ 1812
    if (idKillerActor != ACTORID_INVALID)
    {
        ACDID idACDKiller = ACDFindByActor(idKillerActor);
        if (idACDKiller != ACDID_INVALID && ACDIsPlayer(idACDKiller))
        {
            bKillerWasOwner = PlayerGetHireling(idACDKiller) == idACD;
        }
    }

//@ 1821
    ActorCommonData & tACD = ACDGet(idACD);
    HirelingClass eClass = HirelingGetClass(tACD);
    uint32 dwNow = GameGetServerTime();

    if (eClass == HC_TEMPLAR)
    {
        if (bKillerWasOwner || ptData->dwDeathPowerNextTime > dwNow)
        {
            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_RESURRECTION_POWER), SNO_NONE);
        }
        else
        {
//@ 1833
            SNO snoResurrectPower = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_RESURRECTION_POWER));
            if (snoResurrectPower != SNO_NONE)
            {
                float flCooldownInSeconds = PowerGetFormulaValue(&tACD, AutoPowerDefinitionR(snoResurrectPower), TAG_POWER_PALADIN_RESURRECTION_COOLDOWN_TIME, -1, -1, 0);
                flCooldownInSeconds = Math::Max(0.0f, flCooldownInSeconds);
                uint32 dwCooldownInTicks = Math::FloatToInt(flCooldownInSeconds * 60.0f);
                ptData->dwDeathPowerNextTime = dwNow + dwCooldownInTicks;
            }
        }
    }

//@ 1845
    return FALSE;
}

//@ 1849
static void sPaladinUpdate(ActorID idActor)
{
    ACDID idACD = ACDFindByActor(idActor);
    HirelingBrainData * ptData = sGetBrainData(idActor);

//@ 1856
    if (ptData == NULL)
    {
        return;
    }

    ActorCommonData & tACD = ACDGet(idACD);
//@ 1862
    if (!tACD.IsAlive())
    {
        return;
    }
}

//@ 1870
static void sScoundrelUpdate(ActorID idActor)
{
    ACDID idACD = ACDFindByActor(idActor);
}

//@ 1898
void HirelingProxyBrainUpdate(ActorID idActor)
{
//@ 1901
    Actor * ptActor = ActorGet(idActor);



    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);

    ACDID idACDPlayerOwner = ACDGetPetOwner(idACD);
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayerOwner);
    if (ptACDPlayer != NULL)
    {
//@ 1913
        WorldPlace placeOwner = ptACDPlayer->GetPlace();
        WorldPlace placeHireling = tACD.GetPlace();

        if (WorldPlaceInSameWorld(placeOwner, placeHireling))
        {
            float flDistSq = WorldPlaceDistanceSquared(placeOwner, placeHireling);
            if (flDistSq < MAX_CONVERSATION_DISTANCE * MAX_CONVERSATION_DISTANCE)
            {
                return;
            }
        }
    }

//@ 1928
    NPCHandleDismissal(tACD, FALSE, TRUE);
}
