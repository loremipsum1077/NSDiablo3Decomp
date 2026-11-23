// Original file: X/Common/Definitions/BossEncounterDefinition.cpp

#include "BossEncounterDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;

enum {
    BOSSENCOUNTER_XML_VERSION = 5,
    BOSSENCOUNTER_SNO_VERSION = 23,
};

// The field names are "" in the binary.
static const TypeDescriptorField BossEncounterDefinition_Fields[19] = {  // line 32
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, hEncounterNameLabel), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, hInformationTextLabel), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, hInstructionJoinLabel), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, hTauntTextLabel), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, hMural), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, bResetOnFail), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, bWorkAsPortalBeforeQuestRange), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, bAlwaysWorkAsPortalInOpenWorldGames), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, bResetQuestStep), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, bCleanupMonstersAndPowers), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, bShowUIInSinglePlayer), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterDefinition, uEntranceGUID), (const void *)-1, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BossEncounterDefinition, snoQuestRange), "", QUESTRANGE_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BossEncounterDefinition, snoWaypointLevelArea), "", LEVELAREA_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BossEncounterDefinition, snoActor), "", ACTOR_SNO, 0, -1),
    TypeDescriptorField("", DT_FLOAT, offsetof(BossEncounterDefinition, flFadeInTime), NULL, 1),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(BossEncounterDefinition, arrWorlds), NULL, 0, NUM_BOSS_WORLDS, WORLD_SNO, 0),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(BossEncounterDefinition, arrScripts), NULL, 0, NUM_BOSS_SCRIPTS, SCRIPT_SNO, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BossEncounterDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BossEncounterDefinition("BossEncounter", BOSSENCOUNTER_XML_VERSION, BossEncounterDefinition_Fields, 19, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 51
const XTypeDescriptor * DT_BOSSENCOUNTERDEFINITION = &TypeDescriptor_BossEncounterDefinition;

BossEncounterGroupDef * gptGroupDefBossEncounter;  // line 57

// line 63
BossEncounterGroupDef::BossEncounterGroupDef()
    : GroupDefinition("BossEncounter", BOSS_ENCOUNTER_SNO, SNOVersions(BOSSENCOUNTER_SNO_VERSION), SNODATATYPE_TITLE,
                      sizeof(BossEncounterDefinition), DT_BOSSENCOUNTERDEFINITION, 0x8005, 14) {
}

// line 84
uint32 BossEncounterGroupDef::GetMaxCount() {
    return 32;
}

// line 96
void BossEncounterGroupDef::GetDefaultData(void * ptDefaultDef) const {
    BossEncounterDefinition * pdef = (BossEncounterDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdef, sizeof(BossEncounterDefinition));

    pdef->hEncounterNameLabel = (StringLabelHandle)-1;
    pdef->hInformationTextLabel = (StringLabelHandle)-1;
    pdef->hInstructionJoinLabel = (StringLabelHandle)-1;
    pdef->hTauntTextLabel = (StringLabelHandle)-1;

    pdef->hMural = 0;

    pdef->snoQuestRange = SNO_NONE;
    for (int32 i = 0; i < NUM_BOSS_WORLDS; i++)
        pdef->arrWorlds[i] = SNO_NONE;
    pdef->arrScripts[BOSS_SCRIPT_RESET] = SNO_NONE;
    pdef->arrScripts[BOSS_SCRIPT_COMPLETE] = SNO_NONE;
    pdef->arrScripts[BOSS_SCRIPT_START] = SNO_NONE;

    pdef->bResetOnFail = TRUE;
    pdef->bWorkAsPortalBeforeQuestRange = FALSE;
    pdef->bAlwaysWorkAsPortalInOpenWorldGames = FALSE;
    pdef->bShowUIInSinglePlayer = FALSE;

    pdef->uEntranceGUID = (uint32)-1;
}
