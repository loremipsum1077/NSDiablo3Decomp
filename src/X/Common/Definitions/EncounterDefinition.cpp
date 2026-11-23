// Original file: X/Common/Definitions/EncounterDefinition.cpp

#include "EncounterDefinition.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/RefString.h"
#include "../ActorCommonData.h"
#include "../ActorCommonDataSwig.h"
#include "../GameCommonData.h"
#include "../GlobalSNO.h"
#include "../Hyperlink.h"
#include "../OnlineService/OnlineService.h"
#include "../Player.h"
#include "../PlayerAccount.h"
#include <stddef.h>

// Declared here because their headers are not adopted yet (signatures from the DWARF).
extern const XTypeDescriptor * DT_SNO;  // D3DescribedType.cpp
struct LooteeInfo; BOOL ConditionEvaluate(const SNO snoCondition, const Player * ptPlayer, const ACDID idACD, const LooteeInfo * ptLooteeInfo);  // ConditionDefinition.cpp:640
BOOL SNOCheckSNOReference(SNOGroup snoGroupSrc, SNO snoHandleSrc, SNOGroup snoGroupDst, SNO snoHandleDst, LPCSTR szExtraMsg);  // SNOFiles.cpp:10239





static const uint32 PLATINUM_GOBLIN_BONUS_START_TIME = 108000;
static const uint32 PLATINUM_GOBLIN_BONUS_END_TIME = 431999;
static const uint32 PLATINUM_GOBLIN_BONUS_STEP_TIME = 18000;
static const int32 PLATINUM_GOBLIN_BONUS_STEP_WEIGHT = 5;

enum {
    ENCOUNTER_XML_VERSION = 2,
    ENCOUNTER_SPAWN_OPTION_XML_VERSION = 4,
    ENCOUNTER_SNO_COMMON_VERSION = 33,
    ENCOUNTER_SNO_PC_VERSION = 33,
    ENCOUNTER_SNO_CONSOLE_VERSION = 34,
    ENCOUNTER_SNO_AXE_VERSION = 34,
};







static const TypeDescriptorField EncounterSpawnOption_Fields[5] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(EncounterSpawnOption, snoSpawn), "", ACTOR_SNO, 0, -1), TypeDescriptorField("", DT_INT, offsetof(EncounterSpawnOption, nWeight), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(EncounterSpawnOption, eTeam), NULL, (const void *)TEAM_INVALID, (const void *)(TEAM_NUM_TEAMS - 1)),
    FilterTypeDescriptorField("", DT_SNO, offsetof(EncounterSpawnOption, snoCondition), "", CONDITION_SNO, 0, -1), TypeDescriptorField(NULL, DT_NULL, sizeof(EncounterSpawnOption), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_EncounterSpawnOption("EncounterSpawnOption", ENCOUNTER_SPAWN_OPTION_XML_VERSION, EncounterSpawnOption_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ENCOUNTER_SPAWN_OPTION = &TypeDescriptor_EncounterSpawnOption;

static const TypeDescriptorField EncounterDefinition_Fields[4] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(EncounterDefinition, snoSymbol), "", ACTOR_SNO, 0, -1), TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(EncounterDefinition, serSpawnOptions), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ENCOUNTER_SPAWN_OPTION, offsetof(EncounterDefinition, arSpawnOptions), 0x21, (int32)offsetof(EncounterDefinition, serSpawnOptions) - (int32)offsetof(EncounterDefinition, arSpawnOptions)), TypeDescriptorField(NULL, DT_NULL, sizeof(EncounterDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_EncounterDefinition("Encounter", ENCOUNTER_XML_VERSION, EncounterDefinition_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ENCOUNTER = &TypeDescriptor_EncounterDefinition;














































































































EncounterGroupDef * gptGroupDefEncounter;





EncounterGroupDef::EncounterGroupDef()
    : GroupDefinition("Encounter", ENCOUNTER_SNO,
                      SNOVersions(ENCOUNTER_SNO_PC_VERSION,
                                  ENCOUNTER_SNO_AXE_VERSION + AXE_BASE_SNO_VERSION,
                                  ENCOUNTER_SNO_CONSOLE_VERSION,
                                  ENCOUNTER_SNO_CONSOLE_VERSION,
                                  ENCOUNTER_SNO_CONSOLE_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(EncounterDefinition), DT_ENCOUNTER, 0x8005, 0xe)







{
}




uint32 EncounterGroupDef::GetMaxCount() {
    return 280;
}









void EncounterGroupDef::GetDefaultData(void * ptDefaultDef) const
{
{
    SigmaMemoryClear(ptDefaultDef, sizeof(EncounterDefinition));
    EncounterDefinition * pdef = (EncounterDefinition *)ptDefaultDef;

    pdef->snoSymbol = SNO_NONE;
    pdef->arSpawnOptions = NULL;
    pdef->serSpawnOptions.dwSize = 0;
}
}



























































static bool sPlayersHaveSeenPlatinumTreasureGoblin()
{

    bool bSawPlatinumGoblin = false;
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {


            // Inference: || form from the original keeping the flag in a register without re-normalising it (299:13).
            bSawPlatinumGoblin = bSawPlatinumGoblin || (ptPlayer->tAccount.uPartitionFlags & (1 << PARTITION_FLAG_SAW_PLATINUM_GOBLIN)) != 0;



            // Inference: the original tests the flag bit again here (tbnz at 304) and leaves the loop.
            if (ptPlayer->tAccount.uPartitionFlags & (1 << PARTITION_FLAG_SAW_PLATINUM_GOBLIN)) break;
        }
    }
    return bSawPlatinumGoblin;
}



static bool sShouldIncreasePlatinumGoblinWeight()
{

    uint32 uGameTime = GameGetServerTime();

    return OnlineService::IsFreeToPlay() &&
           !sPlayersHaveSeenPlatinumTreasureGoblin() & uGameTime >= PLATINUM_GOBLIN_BONUS_START_TIME;

}
































static uint32 sGetSpawnWeight(const EncounterSpawnOption & tSpawnOption)
{













    if (tSpawnOption.snoSpawn == GlobalSNOGet(GLOBALSNO_ACTOR_TREASURE_GOBLIN_I) && sShouldIncreasePlatinumGoblinWeight())
    {

        uint32 uTicks = Math::Min(GameGetServerTime(), PLATINUM_GOBLIN_BONUS_END_TIME);
        uint32 uStep = (uTicks - PLATINUM_GOBLIN_BONUS_START_TIME) / PLATINUM_GOBLIN_BONUS_STEP_TIME;
        return tSpawnOption.nWeight + uStep * PLATINUM_GOBLIN_BONUS_STEP_WEIGHT;
    }


    return tSpawnOption.nWeight;

}




BOOL EncounterResolveToActorSpawnInfo(SNO snoEncounter,
                                      RNG & tRNG,
                                      SNOName & tSNOName,
                                      Team & eTeam)
{
    tSNOName = SNONAME_NONE;
    eTeam = TEAM_MONSTER_FIRST;

    if (snoEncounter == SNO_NONE)
    {
        return FALSE;
    }

    const EncounterDefinition * ptEncounterDef = DefinitionLockR<EncounterDefinition>(snoEncounter);

    int32 nCount = ptEncounterDef->serSpawnOptions.dwSize / sizeof(EncounterSpawnOption);

    int32 nWeight = 0;

    for (int32 i = 0; i < nCount; i++)
    {
        SNO snoCondition = ptEncounterDef->arSpawnOptions[i].snoCondition;
        if (snoCondition == SNO_NONE || ConditionEvaluate(snoCondition, NULL, ACDID_INVALID, NULL))
        {
            nWeight += sGetSpawnWeight(ptEncounterDef->arSpawnOptions[i]);
        }
    }

    if (nWeight > 0)
    {
        int32 nPickWeight = Rand_Internal(tRNG, nWeight - 1);
        nWeight = 0;


        for (int32 i = 0; i < nCount; i++)
        {
            int32 nCurrWeight = sGetSpawnWeight(ptEncounterDef->arSpawnOptions[i]);

            SNO snoCondition = ptEncounterDef->arSpawnOptions[i].snoCondition;
            if (snoCondition != SNO_NONE && !ConditionEvaluate(snoCondition, NULL, ACDID_INVALID, NULL))
            {
                continue;
            }

            nWeight += nCurrWeight;

            if (nWeight > nPickWeight)
            {
                const EncounterSpawnOption & tSpawnOpt = ptEncounterDef->arSpawnOptions[i];
                eTeam = tSpawnOpt.eTeam;
                tSNOName.snoGroup = ACTOR_SNO;
                tSNOName.snoHandle = tSpawnOpt.snoSpawn;

                if (tSpawnOpt.snoSpawn == GlobalSNOGet(GLOBALSNO_ACTOR_TREASURE_GOBLIN_I))
                {














                    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
                    {
                        ptPlayer->tAccount.uPartitionFlags |= (1 << PARTITION_FLAG_SAW_PLATINUM_GOBLIN);


                        ptPlayer->uConsoleSyncFlags |= (1 << PCSF_PARTITION_FLAGS);
                    }
                }
                break;
            }
        }
    }
    else
    {


        CRefString sEncounter = HyperlinkCreateFromSNO(SNOName(ENCOUNTER_SNO, snoEncounter), NULL);











        return FALSE;
    }
    if (tSNOName.snoGroup != SNOGROUP_NONE &&
        SNOCheckSNOReference(ENCOUNTER_SNO, snoEncounter, tSNOName.snoGroup, tSNOName.snoHandle, NULL)) return TRUE; return FALSE;




}
