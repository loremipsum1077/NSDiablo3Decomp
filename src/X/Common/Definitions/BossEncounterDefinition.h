// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/BossEncounterDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "StringListDefinition.h"
#include "../ImageHandle.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;

enum {
    BOSS_SCRIPT_RESET = 0,
    BOSS_SCRIPT_COMPLETE = 1,
    BOSS_SCRIPT_START = 2,
    NUM_BOSS_SCRIPTS = 3,
    NUM_BOSS_WORLDS = 4,
};

struct BossEncounterDefinition {  // size 0x68
    RequiredDefinitionHeader tHeader;  // 0x00
    StringLabelHandle hEncounterNameLabel;  // 0x0c
    StringLabelHandle hInformationTextLabel;  // 0x10
    StringLabelHandle hInstructionJoinLabel;  // 0x14
    StringLabelHandle hTauntTextLabel;  // 0x18
    ImageHandle hMural;  // 0x1c
    BOOL bResetOnFail;  // 0x20
    BOOL bWorkAsPortalBeforeQuestRange;  // 0x24
    BOOL bCleanupMonstersAndPowers;  // 0x28
    BOOL bResetQuestStep;  // 0x2c
    BOOL bAlwaysWorkAsPortalInOpenWorldGames;  // 0x30
    BOOL bShowUIInSinglePlayer;  // 0x34
    uint32 uEntranceGUID;  // 0x38
    float flFadeInTime;  // 0x3c
    SNO snoQuestRange;  // 0x40
    SNO arrWorlds[NUM_BOSS_WORLDS];  // 0x44
    SNO arrScripts[NUM_BOSS_SCRIPTS];  // 0x54
    SNO snoWaypointLevelArea;  // 0x60
    SNO snoActor;  // 0x64
};
static_assert(sizeof(BossEncounterDefinition) == 0x68, "BossEncounterDefinition");

class BossEncounterGroupDef : public GroupDefinition {  // size 0xa0, line 63
public:
    BossEncounterGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 68; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 72
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 74
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(BossEncounterGroupDef) == 0xa0, "BossEncounterGroupDef");

extern const XTypeDescriptor * DT_BOSSENCOUNTERDEFINITION;  // BossEncounterDefinition.cpp:51
extern BossEncounterGroupDef * gptGroupDefBossEncounter;  // BossEncounterDefinition.cpp:57

typedef ScopedDefinitionLockR<BossEncounterDefinition> ScopedBossEncounterDefinitionLockR;

// Line 101. Its callers call LockRFast and have nothing from an unlock.
template <> inline const BossEncounterDefinition * DefinitionLockR<BossEncounterDefinition>(SNO sno) {  // line 101
    return (const BossEncounterDefinition *)gptGroupDefBossEncounter->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<BossEncounterDefinition>(const BossEncounterDefinition *) {}
