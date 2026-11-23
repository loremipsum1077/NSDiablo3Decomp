// X/Client/SNOPreload.cpp
//
// Build-time file: lines starting with "//@N" are expanded by build/try/r1-snopreload/gen.py.

#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Prefetch.h"
#include "SNOManager.h"
#include "SNOPreload.h"
#include "../Common/CommonTypes.h"
#include "../Common/ItemsSwig.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/SNO/SNOFilesAsync.h"
#include "../Common/SNO/SNOName.h"
#include "../Common/SNO/SNOTOC.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/RefString.h"
#include "../Sigma/Unicode.h"
#include "../Common/ThreadGlobals.h"
typedef int32 HirelingClass;  // X/Common/ActorCommonData.h
typedef int32 CrafterType;    // X/Common/NPC.h
typedef blz::vector<CRefString, blz::allocator<CRefString> > XStringArray;  // X/Sigma/RefString.h

// X/Client/CGame.cpp
BOOL CGameLoadingScreenCanProgress();
BOOL CGameLoadingScreenIsEnabled();
BOOL CGameLoadingScreenIsBlackOnly(); void CGameProgressLoadingScreen(BOOL fForce); BOOL CGameLoadingScreenHasExpired();


#include "../Common/Powers/PowersCommon.h"
#include "../Common/Definitions/AnimSetDefinition.h"
#include "../Common/TagMaps/Tags.h"
#include "../Common/TagMaps/AnimTagsSwig.h"
#include "../Common/HirelingSwig.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "CPlayer.h"
#include "../Common/Player.h"
#include "../Common/Hireling.h"
#include "Gfx/GfxVB.h"  // the original CU defines cgcDefaultNormal..cgcZAxisNormal (GfxVB.h:351-354)

#include "../Common/GameSwig.h"
LPCSTR SNOPackTypeToString(SNOPackType eSNOPackDestination); // X/Common/Sno/SNOFiles.cpp:7204
void SNOFilesAsyncDisableSortedRequest(); // X/Common/Sno/SNOFilesAsync.cpp:1967
void SNOFilesAsyncEnableSortedRequest(); // X/Common/Sno/SNOFilesAsync.cpp:1985
void SNOFilesAsyncSortRequests(); // X/Common/Sno/SNOFilesAsync.cpp:1976
#include "../Common/FileIO/FileIO.h"
void ConfigureClientPreloadAsyncThread(Blizzard::Thread::Thread tThread); // MainNX64.cpp:220
void ConfigureServerPreloadAsyncThread(Blizzard::Thread::Thread tThread); // MainNX64.cpp:225
#include "../Common/Crafting/ItemCrafting.h"
#include "RActors/RActors.h"
SNO PlayerGetPowerInActiveSkillSlot(const Player & tPlayer, const int32 nSlot, RuneType * peRuneType); // Player.cpp:3713
SNO PlayerGetPowerInTraitSlot(const Player & tPlayer, const int32 nSlot); // Player.cpp:3740
GBID PlayerGetLegendaryPowerInSlot(const Player & tPlayer, const int32 nSlot); // Player.cpp:3761
SNO ItemGetLegendaryPower(GBID gbidItem); // Items.cpp:10556
#include "../Common/VisualInventory.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/TagMaps/ActorTags.h"




































































namespace SNOPreload {

typedef uint32 ParentAndChildBitField[154];




struct ParentAndChild {  // size 0x8
    SNOGroup snoParent;  // 0x00
    SNOGroup snoChild;  // 0x04
};



























































































































ParentAndChild saActorServerExplore[95] = {  // line 371
    { ACTOR_SNO, ACTOR_SNO },
    { ACTOR_SNO, EFFECTGROUP_SNO },
    { ACTOR_SNO, APPEARANCE_SNO },
    { ACTOR_SNO, PARTICLE_SNO },
    { ACTOR_SNO, LIGHT_SNO },
    { ACTOR_SNO, AMBIENTSOUND_SNO },
    { ACTOR_SNO, SCRIPT_SNO },
    { ACTOR_SNO, MONSTER_SNO },
    { ACTOR_SNO, CONVERSATIONLIST_SNO },
    { ACTOR_SNO, CONVERSATION_SNO },
    { ACTOR_SNO, TRAIL_SNO },
    { ACTOR_SNO, BOSS_ENCOUNTER_SNO },
    { ACTOR_SNO, ENCOUNTER_SNO },
    { ACTOR_SNO, TREASURECLASS_SNO },
    { ACTOR_SNO, LORE_SNO },
    { ACTOR_SNO, SOUND_SNO },
    { ACTOR_SNO, POWER_SNO },
    { ENCOUNTER_SNO, ACTOR_SNO },
    { EFFECTGROUP_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, EFFECTGROUP_SNO },
    { EFFECTGROUP_SNO, SOUND_SNO },
    { EFFECTGROUP_SNO, SHAKE_SNO },
    { EFFECTGROUP_SNO, ACTOR_SNO },
    { EFFECTGROUP_SNO, AMBIENTSOUND_SNO },
    { EFFECTGROUP_SNO, ROPE_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { APPEARANCE_SNO, SURFACE_SNO },
    { APPEARANCE_SNO, MATERIAL_SNO },
    { APPEARANCE_SNO, PARTICLE_SNO },
    { MATERIAL_SNO, SURFACE_SNO },
    { MATERIAL_SNO, PARTICLE_SNO },
    { SURFACE_SNO, PARTICLE_SNO },
    { LORE_SNO, CONVERSATION_SNO },
    { POWER_SNO, EFFECTGROUP_SNO },
    { POWER_SNO, POWER_SNO },
    { POWER_SNO, ROPE_SNO },
    { POWER_SNO, ACTOR_SNO },
    { POWER_SNO, CONVERSATION_SNO },
    { POWER_SNO, AISTATE_SNO },
    { PARTICLE_SNO, SOUND_SNO },
    { PARTICLE_SNO, AMBIENTSOUND_SNO },
    { PARTICLE_SNO, ACTOR_SNO },
    { PARTICLE_SNO, PARTICLE_SNO },
    { PARTICLE_SNO, EFFECTGROUP_SNO },
    { ANIMATION_SNO, SHAKE_SNO },
    { ANIMATION_SNO, AMBIENTSOUND_SNO },
    { ANIMATION_SNO, ROPE_SNO },
    { ANIMATION_SNO, SOUND_SNO },
    { ANIMATION_SNO, ACTOR_SNO },
    { ANIMATION_SNO, TRAIL_SNO },
    { ANIMATION_SNO, LIGHT_SNO },
    { ANIMATION_SNO, PARTICLE_SNO },
    { ANIMATION_SNO, EFFECTGROUP_SNO },
    { MONSTER_SNO, AIBEHAVIOR_SNO },
    { MONSTER_SNO, TREASURECLASS_SNO },
    { MONSTER_SNO, POWER_SNO },
    { MONSTER_SNO, SKILLKIT_SNO },
    { MONSTER_SNO, LORE_SNO },
    { AIBEHAVIOR_SNO, AISTATE_SNO },
    { AISTATE_SNO, POWER_SNO },
    { SCRIPT_SNO, CONDUCTOR_SNO },
    { SCRIPT_SNO, CONVERSATION_SNO },
    { SCRIPT_SNO, CONVERSATIONLIST_SNO },
    { SCRIPT_SNO, POWER_SNO },
    { SCRIPT_SNO, ROPE_SNO },
    { SCRIPT_SNO, LORE_SNO },
    { SCRIPT_SNO, ACTOR_SNO },
    { SCRIPT_SNO, EFFECTGROUP_SNO },
    { SCRIPT_SNO, TREASURECLASS_SNO },
    { SCRIPT_SNO, MUSIC_SNO },
    { CONVERSATIONLIST_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, EFFECTGROUP_SNO },
    { CONDUCTOR_SNO, ROPE_SNO },
    { CONDUCTOR_SNO, CONVERSATIONLIST_SNO },
    { CONDUCTOR_SNO, POWER_SNO },
    { CONDUCTOR_SNO, TREASURECLASS_SNO },
    { CONDUCTOR_SNO, ACTOR_SNO },
    { CONDUCTOR_SNO, BOSS_ENCOUNTER_SNO },
    { CONDUCTOR_SNO, MUSIC_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { ROPE_SNO, PARTICLE_SNO },
    { ROPE_SNO, AMBIENTSOUND_SNO },
    { CONVERSATION_SNO, SOUND_SNO },
    { CONVERSATION_SNO, QUEST_SNO },
    { CONVERSATION_SNO, CONVERSATION_SNO },
    { CONVERSATION_SNO, STRINGLIST_SNO },
    { QUEST_SNO, TREASURECLASS_SNO },
    { QUEST_SNO, QUEST_SNO },
    { QUEST_SNO, STRINGLIST_SNO },
    { QUEST_SNO, CONVERSATION_SNO },
    { TREASURECLASS_SNO, TREASURECLASS_SNO },
    { TREASURECLASS_SNO, CONDITION_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saSceneServerExplore[84] = {  // line 399
    { SCENE_SNO, APPEARANCE_SNO },
    { SCENE_SNO, MARKERSET_SNO },
    { SCENE_SNO, ACTOR_SNO },
    { SCENE_SNO, PARTICLE_SNO },
    { SCENE_SNO, SCENE_SNO },
    { APPEARANCE_SNO, SURFACE_SNO },
    { APPEARANCE_SNO, MATERIAL_SNO },
    { APPEARANCE_SNO, PARTICLE_SNO },
    { MATERIAL_SNO, SURFACE_SNO },
    { MATERIAL_SNO, PARTICLE_SNO },
    { SURFACE_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, EFFECTGROUP_SNO },
    { EFFECTGROUP_SNO, SOUND_SNO },
    { EFFECTGROUP_SNO, SHAKE_SNO },
    { EFFECTGROUP_SNO, ACTOR_SNO },
    { EFFECTGROUP_SNO, AMBIENTSOUND_SNO },
    { EFFECTGROUP_SNO, ROPE_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { PARTICLE_SNO, SOUND_SNO },
    { PARTICLE_SNO, AMBIENTSOUND_SNO },
    { PARTICLE_SNO, ACTOR_SNO },
    { PARTICLE_SNO, PARTICLE_SNO },
    { PARTICLE_SNO, EFFECTGROUP_SNO },
    { MARKERSET_SNO, LIGHT_SNO },
    { MARKERSET_SNO, PARTICLE_SNO },
    { MARKERSET_SNO, AMBIENTSOUND_SNO },
    { MARKERSET_SNO, ENCOUNTER_SNO },
    { MARKERSET_SNO, ACTOR_SNO },
    { MARKERSET_SNO, SCRIPT_SNO },
    { MARKERSET_SNO, CONVERSATIONLIST_SNO },
    { MARKERSET_SNO, CONDUCTOR_SNO },
    { MARKERSET_SNO, CONVERSATION_SNO },
    { MARKERSET_SNO, ADVENTURE_SNO },
    { MARKERSET_SNO, WEATHER_SNO },
    { MARKERSET_SNO, MUSIC_SNO },
    { ACTOR_SNO, ACTOR_SNO },
    { ADVENTURE_SNO, ENCOUNTER_SNO },
    { ADVENTURE_SNO, MARKERSET_SNO },
    { POWER_SNO, EFFECTGROUP_SNO },
    { POWER_SNO, POWER_SNO },
    { POWER_SNO, ROPE_SNO },
    { POWER_SNO, ACTOR_SNO },
    { POWER_SNO, CONVERSATION_SNO },
    { POWER_SNO, AISTATE_SNO },
    { SCRIPT_SNO, CONDUCTOR_SNO },
    { SCRIPT_SNO, CONVERSATION_SNO },
    { SCRIPT_SNO, CONVERSATIONLIST_SNO },
    { SCRIPT_SNO, POWER_SNO },
    { SCRIPT_SNO, ROPE_SNO },
    { SCRIPT_SNO, LORE_SNO },
    { SCRIPT_SNO, ACTOR_SNO },
    { SCRIPT_SNO, EFFECTGROUP_SNO },
    { SCRIPT_SNO, TREASURECLASS_SNO },
    { SCRIPT_SNO, MUSIC_SNO },
    { LORE_SNO, CONVERSATION_SNO },
    { CONVERSATIONLIST_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, EFFECTGROUP_SNO },
    { CONDUCTOR_SNO, ROPE_SNO },
    { CONDUCTOR_SNO, CONVERSATIONLIST_SNO },
    { CONDUCTOR_SNO, POWER_SNO },
    { CONDUCTOR_SNO, TREASURECLASS_SNO },
    { CONDUCTOR_SNO, ACTOR_SNO },
    { CONDUCTOR_SNO, BOSS_ENCOUNTER_SNO },
    { CONDUCTOR_SNO, MUSIC_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { ROPE_SNO, PARTICLE_SNO },
    { ROPE_SNO, AMBIENTSOUND_SNO },
    { CONVERSATION_SNO, SOUND_SNO },
    { CONVERSATION_SNO, QUEST_SNO },
    { CONVERSATION_SNO, CONVERSATION_SNO },
    { CONVERSATION_SNO, STRINGLIST_SNO },
    { QUEST_SNO, TREASURECLASS_SNO },
    { QUEST_SNO, QUEST_SNO },
    { QUEST_SNO, STRINGLIST_SNO },
    { QUEST_SNO, CONVERSATION_SNO },
    { WEATHER_SNO, LIGHT_SNO },
    { WEATHER_SNO, PARTICLE_SNO },
    { WEATHER_SNO, SOUND_SNO },
    { TREASURECLASS_SNO, TREASURECLASS_SNO },
    { TREASURECLASS_SNO, CONDITION_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saWorldLoadingServerExplore[99] = {  // line 425
    { WORLD_SNO, MARKERSET_SNO },
    { WORLD_SNO, WEATHER_SNO },
    { WORLD_SNO, AMBIENTSOUND_SNO },
    { WORLD_SNO, MUSIC_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { WORLD_SNO, SCENEGROUP_SNO },
    { WORLD_SNO, LEVELAREA_SNO },
    { SCENEGROUP_SNO, SCENE_SNO },
    { WORLD_SNO, SCENE_SNO },
    { LEVELAREA_SNO, MARKERSET_SNO },
    { LEVELAREA_SNO, LEVELAREA_SNO },
    { LEVELAREA_SNO, ENCOUNTER_SNO },
    { LEVELAREA_SNO, CONDITION_SNO },
    { LEVELAREA_SNO, ACTOR_SNO },
    { LEVELAREA_SNO, ADVENTURE_SNO },
    { SCENE_SNO, APPEARANCE_SNO },
    { SCENE_SNO, MARKERSET_SNO },
    { SCENE_SNO, ACTOR_SNO },
    { SCENE_SNO, PARTICLE_SNO },
    { SCENE_SNO, SCENE_SNO },
    { APPEARANCE_SNO, SURFACE_SNO },
    { APPEARANCE_SNO, MATERIAL_SNO },
    { APPEARANCE_SNO, PARTICLE_SNO },
    { MATERIAL_SNO, SURFACE_SNO },
    { MATERIAL_SNO, PARTICLE_SNO },
    { SURFACE_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, EFFECTGROUP_SNO },
    { EFFECTGROUP_SNO, SOUND_SNO },
    { EFFECTGROUP_SNO, SHAKE_SNO },
    { EFFECTGROUP_SNO, ACTOR_SNO },
    { EFFECTGROUP_SNO, AMBIENTSOUND_SNO },
    { EFFECTGROUP_SNO, ROPE_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { PARTICLE_SNO, SOUND_SNO },
    { PARTICLE_SNO, AMBIENTSOUND_SNO },
    { PARTICLE_SNO, ACTOR_SNO },
    { PARTICLE_SNO, PARTICLE_SNO },
    { PARTICLE_SNO, EFFECTGROUP_SNO },
    { MARKERSET_SNO, LIGHT_SNO },
    { MARKERSET_SNO, PARTICLE_SNO },
    { MARKERSET_SNO, AMBIENTSOUND_SNO },
    { MARKERSET_SNO, ENCOUNTER_SNO },
    { MARKERSET_SNO, ACTOR_SNO },
    { MARKERSET_SNO, SCRIPT_SNO },
    { MARKERSET_SNO, CONVERSATIONLIST_SNO },
    { MARKERSET_SNO, CONDUCTOR_SNO },
    { MARKERSET_SNO, CONVERSATION_SNO },
    { MARKERSET_SNO, ADVENTURE_SNO },
    { MARKERSET_SNO, WEATHER_SNO },
    { MARKERSET_SNO, MUSIC_SNO },
    { ACTOR_SNO, ACTOR_SNO },
    { ADVENTURE_SNO, ENCOUNTER_SNO },
    { ADVENTURE_SNO, MARKERSET_SNO },
    { POWER_SNO, EFFECTGROUP_SNO },
    { POWER_SNO, POWER_SNO },
    { POWER_SNO, ROPE_SNO },
    { POWER_SNO, ACTOR_SNO },
    { POWER_SNO, CONVERSATION_SNO },
    { POWER_SNO, AISTATE_SNO },
    { SCRIPT_SNO, CONDUCTOR_SNO },
    { SCRIPT_SNO, CONVERSATION_SNO },
    { SCRIPT_SNO, CONVERSATIONLIST_SNO },
    { SCRIPT_SNO, POWER_SNO },
    { SCRIPT_SNO, ROPE_SNO },
    { SCRIPT_SNO, LORE_SNO },
    { SCRIPT_SNO, ACTOR_SNO },
    { SCRIPT_SNO, EFFECTGROUP_SNO },
    { SCRIPT_SNO, TREASURECLASS_SNO },
    { SCRIPT_SNO, MUSIC_SNO },
    { LORE_SNO, CONVERSATION_SNO },
    { CONVERSATIONLIST_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, EFFECTGROUP_SNO },
    { CONDUCTOR_SNO, ROPE_SNO },
    { CONDUCTOR_SNO, CONVERSATIONLIST_SNO },
    { CONDUCTOR_SNO, POWER_SNO },
    { CONDUCTOR_SNO, TREASURECLASS_SNO },
    { CONDUCTOR_SNO, ACTOR_SNO },
    { CONDUCTOR_SNO, BOSS_ENCOUNTER_SNO },
    { CONDUCTOR_SNO, MUSIC_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { ROPE_SNO, PARTICLE_SNO },
    { ROPE_SNO, AMBIENTSOUND_SNO },
    { CONVERSATION_SNO, SOUND_SNO },
    { CONVERSATION_SNO, QUEST_SNO },
    { CONVERSATION_SNO, CONVERSATION_SNO },
    { CONVERSATION_SNO, STRINGLIST_SNO },
    { QUEST_SNO, TREASURECLASS_SNO },
    { QUEST_SNO, QUEST_SNO },
    { QUEST_SNO, STRINGLIST_SNO },
    { QUEST_SNO, CONVERSATION_SNO },
    { WEATHER_SNO, LIGHT_SNO },
    { WEATHER_SNO, PARTICLE_SNO },
    { WEATHER_SNO, SOUND_SNO },
    { TREASURECLASS_SNO, TREASURECLASS_SNO },
    { TREASURECLASS_SNO, CONDITION_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saLevelAreaServerExplore[28] = {  // line 455
    { LEVELAREA_SNO, MARKERSET_SNO },
    { LEVELAREA_SNO, LEVELAREA_SNO },
    { LEVELAREA_SNO, ENCOUNTER_SNO },
    { LEVELAREA_SNO, CONDITION_SNO },
    { LEVELAREA_SNO, ACTOR_SNO },
    { LEVELAREA_SNO, ADVENTURE_SNO },
    { MARKERSET_SNO, LIGHT_SNO },
    { MARKERSET_SNO, PARTICLE_SNO },
    { MARKERSET_SNO, AMBIENTSOUND_SNO },
    { MARKERSET_SNO, ENCOUNTER_SNO },
    { MARKERSET_SNO, ACTOR_SNO },
    { MARKERSET_SNO, SCRIPT_SNO },
    { MARKERSET_SNO, CONVERSATIONLIST_SNO },
    { MARKERSET_SNO, CONDUCTOR_SNO },
    { MARKERSET_SNO, CONVERSATION_SNO },
    { MARKERSET_SNO, ADVENTURE_SNO },
    { MARKERSET_SNO, WEATHER_SNO },
    { MARKERSET_SNO, MUSIC_SNO },
    { ACTOR_SNO, ACTOR_SNO },
    { ADVENTURE_SNO, ENCOUNTER_SNO },
    { ADVENTURE_SNO, MARKERSET_SNO },
    { WEATHER_SNO, LIGHT_SNO },
    { WEATHER_SNO, PARTICLE_SNO },
    { WEATHER_SNO, SOUND_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saWorldServerExplore[26] = {  // line 467
    { WORLD_SNO, MARKERSET_SNO },
    { WORLD_SNO, WEATHER_SNO },
    { WORLD_SNO, AMBIENTSOUND_SNO },
    { WORLD_SNO, MUSIC_SNO },
    { MARKERSET_SNO, LIGHT_SNO },
    { MARKERSET_SNO, PARTICLE_SNO },
    { MARKERSET_SNO, AMBIENTSOUND_SNO },
    { MARKERSET_SNO, ENCOUNTER_SNO },
    { MARKERSET_SNO, ACTOR_SNO },
    { MARKERSET_SNO, SCRIPT_SNO },
    { MARKERSET_SNO, CONVERSATIONLIST_SNO },
    { MARKERSET_SNO, CONDUCTOR_SNO },
    { MARKERSET_SNO, CONVERSATION_SNO },
    { MARKERSET_SNO, ADVENTURE_SNO },
    { MARKERSET_SNO, WEATHER_SNO },
    { MARKERSET_SNO, MUSIC_SNO },
    { ACTOR_SNO, ACTOR_SNO },
    { ADVENTURE_SNO, ENCOUNTER_SNO },
    { ADVENTURE_SNO, MARKERSET_SNO },
    { WEATHER_SNO, LIGHT_SNO },
    { WEATHER_SNO, PARTICLE_SNO },
    { WEATHER_SNO, SOUND_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saPowerServerExplore[24] = {  // line 479
    { SKILLKIT_SNO, POWER_SNO },
    { POWER_SNO, EFFECTGROUP_SNO },
    { POWER_SNO, POWER_SNO },
    { POWER_SNO, ROPE_SNO },
    { POWER_SNO, ACTOR_SNO },
    { POWER_SNO, CONVERSATION_SNO },
    { POWER_SNO, AISTATE_SNO },
    { EFFECTGROUP_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, EFFECTGROUP_SNO },
    { EFFECTGROUP_SNO, SOUND_SNO },
    { EFFECTGROUP_SNO, SHAKE_SNO },
    { EFFECTGROUP_SNO, ACTOR_SNO },
    { EFFECTGROUP_SNO, AMBIENTSOUND_SNO },
    { EFFECTGROUP_SNO, ROPE_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { PARTICLE_SNO, SOUND_SNO },
    { PARTICLE_SNO, AMBIENTSOUND_SNO },
    { PARTICLE_SNO, ACTOR_SNO },
    { PARTICLE_SNO, PARTICLE_SNO },
    { PARTICLE_SNO, EFFECTGROUP_SNO },
    { ROPE_SNO, PARTICLE_SNO },
    { ROPE_SNO, AMBIENTSOUND_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saQuestServerExplore[13] = {  // line 491
    { QUEST_SNO, TREASURECLASS_SNO },
    { QUEST_SNO, QUEST_SNO },
    { QUEST_SNO, STRINGLIST_SNO },
    { QUEST_SNO, CONVERSATION_SNO },
    { CONVERSATION_SNO, SOUND_SNO },
    { CONVERSATION_SNO, QUEST_SNO },
    { CONVERSATION_SNO, CONVERSATION_SNO },
    { CONVERSATION_SNO, STRINGLIST_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { TREASURECLASS_SNO, TREASURECLASS_SNO },
    { TREASURECLASS_SNO, CONDITION_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saUIServerExplore[3] = {  // line 501
    { UI_SNO, LIGHT_SNO },
    { UI_SNO, ACTOR_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saActorClientExplore[88] = {  // line 712
    { ACTOR_SNO, ACTOR_SNO },
    { ACTOR_SNO, EFFECTGROUP_SNO },
    { ACTOR_SNO, APPEARANCE_SNO },
    { ACTOR_SNO, PARTICLE_SNO },
    { ACTOR_SNO, LIGHT_SNO },
    { ACTOR_SNO, AMBIENTSOUND_SNO },
    { ACTOR_SNO, SCRIPT_SNO },
    { ACTOR_SNO, MONSTER_SNO },
    { ACTOR_SNO, CONVERSATIONLIST_SNO },
    { ACTOR_SNO, CONVERSATION_SNO },
    { ACTOR_SNO, TRAIL_SNO },
    { ACTOR_SNO, BOSS_ENCOUNTER_SNO },
    { ACTOR_SNO, TREASURECLASS_SNO },
    { ACTOR_SNO, LORE_SNO },
    { ACTOR_SNO, POWER_SNO },
    { EFFECTGROUP_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, EFFECTGROUP_SNO },
    { EFFECTGROUP_SNO, SOUND_SNO },
    { EFFECTGROUP_SNO, SHAKE_SNO },
    { EFFECTGROUP_SNO, ACTOR_SNO },
    { EFFECTGROUP_SNO, AMBIENTSOUND_SNO },
    { EFFECTGROUP_SNO, ROPE_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { APPEARANCE_SNO, SURFACE_SNO },
    { APPEARANCE_SNO, MATERIAL_SNO },
    { APPEARANCE_SNO, PARTICLE_SNO },
    { MATERIAL_SNO, SURFACE_SNO },
    { MATERIAL_SNO, PARTICLE_SNO },
    { SURFACE_SNO, PARTICLE_SNO },
    { LORE_SNO, CONVERSATION_SNO },
    { POWER_SNO, EFFECTGROUP_SNO },
    { POWER_SNO, POWER_SNO },
    { POWER_SNO, ROPE_SNO },
    { POWER_SNO, ACTOR_SNO },
    { POWER_SNO, CONVERSATION_SNO },
    { POWER_SNO, AISTATE_SNO },
    { PARTICLE_SNO, SOUND_SNO },
    { PARTICLE_SNO, AMBIENTSOUND_SNO },
    { PARTICLE_SNO, ACTOR_SNO },
    { PARTICLE_SNO, PARTICLE_SNO },
    { PARTICLE_SNO, EFFECTGROUP_SNO },
    { ANIMATION_SNO, SHAKE_SNO },
    { ANIMATION_SNO, AMBIENTSOUND_SNO },
    { ANIMATION_SNO, ROPE_SNO },
    { ANIMATION_SNO, SOUND_SNO },
    { ANIMATION_SNO, ACTOR_SNO },
    { ANIMATION_SNO, TRAIL_SNO },
    { ANIMATION_SNO, LIGHT_SNO },
    { ANIMATION_SNO, PARTICLE_SNO },
    { ANIMATION_SNO, EFFECTGROUP_SNO },
    { MONSTER_SNO, AIBEHAVIOR_SNO },
    { MONSTER_SNO, TREASURECLASS_SNO },
    { MONSTER_SNO, POWER_SNO },
    { MONSTER_SNO, SKILLKIT_SNO },
    { MONSTER_SNO, LORE_SNO },
    { AIBEHAVIOR_SNO, AISTATE_SNO },
    { AISTATE_SNO, POWER_SNO },
    { SCRIPT_SNO, CONDUCTOR_SNO },
    { SCRIPT_SNO, CONVERSATION_SNO },
    { SCRIPT_SNO, CONVERSATIONLIST_SNO },
    { SCRIPT_SNO, POWER_SNO },
    { SCRIPT_SNO, ROPE_SNO },
    { SCRIPT_SNO, LORE_SNO },
    { SCRIPT_SNO, EFFECTGROUP_SNO },
    { SCRIPT_SNO, TREASURECLASS_SNO },
    { SCRIPT_SNO, MUSIC_SNO },
    { CONVERSATIONLIST_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, EFFECTGROUP_SNO },
    { CONDUCTOR_SNO, ROPE_SNO },
    { CONDUCTOR_SNO, CONVERSATIONLIST_SNO },
    { CONDUCTOR_SNO, POWER_SNO },
    { CONDUCTOR_SNO, MUSIC_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { ROPE_SNO, PARTICLE_SNO },
    { ROPE_SNO, AMBIENTSOUND_SNO },
    { CONVERSATION_SNO, SOUND_SNO },
    { CONVERSATION_SNO, QUEST_SNO },
    { CONVERSATION_SNO, CONVERSATION_SNO },
    { CONVERSATION_SNO, STRINGLIST_SNO },
    { QUEST_SNO, TREASURECLASS_SNO },
    { QUEST_SNO, QUEST_SNO },
    { QUEST_SNO, STRINGLIST_SNO },
    { QUEST_SNO, CONVERSATION_SNO },
    { TREASURECLASS_SNO, TREASURECLASS_SNO },
    { TREASURECLASS_SNO, CONDITION_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saSceneClientExplore[80] = {  // line 739
    { SCENE_SNO, APPEARANCE_SNO },
    { SCENE_SNO, MARKERSET_SNO },
    { SCENE_SNO, ACTOR_SNO },
    { SCENE_SNO, PARTICLE_SNO },
    { SCENE_SNO, SCENE_SNO },
    { APPEARANCE_SNO, SURFACE_SNO },
    { APPEARANCE_SNO, MATERIAL_SNO },
    { APPEARANCE_SNO, PARTICLE_SNO },
    { MATERIAL_SNO, SURFACE_SNO },
    { MATERIAL_SNO, PARTICLE_SNO },
    { SURFACE_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, EFFECTGROUP_SNO },
    { EFFECTGROUP_SNO, SOUND_SNO },
    { EFFECTGROUP_SNO, SHAKE_SNO },
    { EFFECTGROUP_SNO, ACTOR_SNO },
    { EFFECTGROUP_SNO, AMBIENTSOUND_SNO },
    { EFFECTGROUP_SNO, ROPE_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { PARTICLE_SNO, SOUND_SNO },
    { PARTICLE_SNO, AMBIENTSOUND_SNO },
    { PARTICLE_SNO, ACTOR_SNO },
    { PARTICLE_SNO, PARTICLE_SNO },
    { PARTICLE_SNO, EFFECTGROUP_SNO },
    { MARKERSET_SNO, LIGHT_SNO },
    { MARKERSET_SNO, PARTICLE_SNO },
    { MARKERSET_SNO, AMBIENTSOUND_SNO },
    { MARKERSET_SNO, ENCOUNTER_SNO },
    { MARKERSET_SNO, ACTOR_SNO },
    { MARKERSET_SNO, SCRIPT_SNO },
    { MARKERSET_SNO, CONVERSATIONLIST_SNO },
    { MARKERSET_SNO, CONDUCTOR_SNO },
    { MARKERSET_SNO, CONVERSATION_SNO },
    { MARKERSET_SNO, ADVENTURE_SNO },
    { MARKERSET_SNO, WEATHER_SNO },
    { MARKERSET_SNO, MUSIC_SNO },
    { ACTOR_SNO, ACTOR_SNO },
    { ADVENTURE_SNO, ENCOUNTER_SNO },
    { ADVENTURE_SNO, MARKERSET_SNO },
    { POWER_SNO, EFFECTGROUP_SNO },
    { POWER_SNO, POWER_SNO },
    { POWER_SNO, ROPE_SNO },
    { POWER_SNO, ACTOR_SNO },
    { POWER_SNO, CONVERSATION_SNO },
    { POWER_SNO, AISTATE_SNO },
    { SCRIPT_SNO, CONDUCTOR_SNO },
    { SCRIPT_SNO, CONVERSATION_SNO },
    { SCRIPT_SNO, CONVERSATIONLIST_SNO },
    { SCRIPT_SNO, POWER_SNO },
    { SCRIPT_SNO, ROPE_SNO },
    { SCRIPT_SNO, LORE_SNO },
    { SCRIPT_SNO, EFFECTGROUP_SNO },
    { SCRIPT_SNO, TREASURECLASS_SNO },
    { SCRIPT_SNO, MUSIC_SNO },
    { LORE_SNO, CONVERSATION_SNO },
    { CONVERSATIONLIST_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, EFFECTGROUP_SNO },
    { CONDUCTOR_SNO, ROPE_SNO },
    { CONDUCTOR_SNO, CONVERSATIONLIST_SNO },
    { CONDUCTOR_SNO, POWER_SNO },
    { CONDUCTOR_SNO, MUSIC_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { ROPE_SNO, PARTICLE_SNO },
    { ROPE_SNO, AMBIENTSOUND_SNO },
    { CONVERSATION_SNO, SOUND_SNO },
    { CONVERSATION_SNO, QUEST_SNO },
    { CONVERSATION_SNO, CONVERSATION_SNO },
    { CONVERSATION_SNO, STRINGLIST_SNO },
    { QUEST_SNO, TREASURECLASS_SNO },
    { QUEST_SNO, QUEST_SNO },
    { QUEST_SNO, STRINGLIST_SNO },
    { QUEST_SNO, CONVERSATION_SNO },
    { WEATHER_SNO, LIGHT_SNO },
    { WEATHER_SNO, PARTICLE_SNO },
    { WEATHER_SNO, SOUND_SNO },
    { TREASURECLASS_SNO, TREASURECLASS_SNO },
    { TREASURECLASS_SNO, CONDITION_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saWorldLoadingClientExplore[94] = {  // line 765
    { WORLD_SNO, MARKERSET_SNO },
    { WORLD_SNO, WEATHER_SNO },
    { WORLD_SNO, AMBIENTSOUND_SNO },
    { WORLD_SNO, MUSIC_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { WORLD_SNO, SCENEGROUP_SNO },
    { WORLD_SNO, LEVELAREA_SNO },
    { SCENEGROUP_SNO, SCENE_SNO },
    { WORLD_SNO, SCENE_SNO },
    { LEVELAREA_SNO, MARKERSET_SNO },
    { LEVELAREA_SNO, LEVELAREA_SNO },
    { LEVELAREA_SNO, ENCOUNTER_SNO },
    { LEVELAREA_SNO, CONDITION_SNO },
    { LEVELAREA_SNO, ADVENTURE_SNO },
    { SCENE_SNO, APPEARANCE_SNO },
    { SCENE_SNO, MARKERSET_SNO },
    { SCENE_SNO, ACTOR_SNO },
    { SCENE_SNO, PARTICLE_SNO },
    { SCENE_SNO, SCENE_SNO },
    { APPEARANCE_SNO, SURFACE_SNO },
    { APPEARANCE_SNO, MATERIAL_SNO },
    { APPEARANCE_SNO, PARTICLE_SNO },
    { MATERIAL_SNO, SURFACE_SNO },
    { MATERIAL_SNO, PARTICLE_SNO },
    { SURFACE_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, EFFECTGROUP_SNO },
    { EFFECTGROUP_SNO, SOUND_SNO },
    { EFFECTGROUP_SNO, SHAKE_SNO },
    { EFFECTGROUP_SNO, ACTOR_SNO },
    { EFFECTGROUP_SNO, AMBIENTSOUND_SNO },
    { EFFECTGROUP_SNO, ROPE_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { PARTICLE_SNO, SOUND_SNO },
    { PARTICLE_SNO, AMBIENTSOUND_SNO },
    { PARTICLE_SNO, ACTOR_SNO },
    { PARTICLE_SNO, PARTICLE_SNO },
    { PARTICLE_SNO, EFFECTGROUP_SNO },
    { MARKERSET_SNO, LIGHT_SNO },
    { MARKERSET_SNO, PARTICLE_SNO },
    { MARKERSET_SNO, AMBIENTSOUND_SNO },
    { MARKERSET_SNO, ENCOUNTER_SNO },
    { MARKERSET_SNO, ACTOR_SNO },
    { MARKERSET_SNO, SCRIPT_SNO },
    { MARKERSET_SNO, CONVERSATIONLIST_SNO },
    { MARKERSET_SNO, CONDUCTOR_SNO },
    { MARKERSET_SNO, CONVERSATION_SNO },
    { MARKERSET_SNO, ADVENTURE_SNO },
    { MARKERSET_SNO, WEATHER_SNO },
    { MARKERSET_SNO, MUSIC_SNO },
    { ACTOR_SNO, ACTOR_SNO },
    { ADVENTURE_SNO, ENCOUNTER_SNO },
    { ADVENTURE_SNO, MARKERSET_SNO },
    { POWER_SNO, EFFECTGROUP_SNO },
    { POWER_SNO, POWER_SNO },
    { POWER_SNO, ROPE_SNO },
    { POWER_SNO, ACTOR_SNO },
    { POWER_SNO, CONVERSATION_SNO },
    { POWER_SNO, AISTATE_SNO },
    { SCRIPT_SNO, CONDUCTOR_SNO },
    { SCRIPT_SNO, CONVERSATION_SNO },
    { SCRIPT_SNO, CONVERSATIONLIST_SNO },
    { SCRIPT_SNO, POWER_SNO },
    { SCRIPT_SNO, ROPE_SNO },
    { SCRIPT_SNO, LORE_SNO },
    { SCRIPT_SNO, EFFECTGROUP_SNO },
    { SCRIPT_SNO, TREASURECLASS_SNO },
    { SCRIPT_SNO, MUSIC_SNO },
    { LORE_SNO, CONVERSATION_SNO },
    { CONVERSATIONLIST_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, CONVERSATION_SNO },
    { CONDUCTOR_SNO, EFFECTGROUP_SNO },
    { CONDUCTOR_SNO, ROPE_SNO },
    { CONDUCTOR_SNO, CONVERSATIONLIST_SNO },
    { CONDUCTOR_SNO, POWER_SNO },
    { CONDUCTOR_SNO, MUSIC_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { ROPE_SNO, PARTICLE_SNO },
    { ROPE_SNO, AMBIENTSOUND_SNO },
    { CONVERSATION_SNO, SOUND_SNO },
    { CONVERSATION_SNO, QUEST_SNO },
    { CONVERSATION_SNO, CONVERSATION_SNO },
    { CONVERSATION_SNO, STRINGLIST_SNO },
    { QUEST_SNO, TREASURECLASS_SNO },
    { QUEST_SNO, QUEST_SNO },
    { QUEST_SNO, STRINGLIST_SNO },
    { QUEST_SNO, CONVERSATION_SNO },
    { WEATHER_SNO, LIGHT_SNO },
    { WEATHER_SNO, PARTICLE_SNO },
    { WEATHER_SNO, SOUND_SNO },
    { TREASURECLASS_SNO, TREASURECLASS_SNO },
    { TREASURECLASS_SNO, CONDITION_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saLevelAreaClientExplore[27] = {  // line 795
    { LEVELAREA_SNO, MARKERSET_SNO },
    { LEVELAREA_SNO, LEVELAREA_SNO },
    { LEVELAREA_SNO, ENCOUNTER_SNO },
    { LEVELAREA_SNO, CONDITION_SNO },
    { LEVELAREA_SNO, ADVENTURE_SNO },
    { MARKERSET_SNO, LIGHT_SNO },
    { MARKERSET_SNO, PARTICLE_SNO },
    { MARKERSET_SNO, AMBIENTSOUND_SNO },
    { MARKERSET_SNO, ENCOUNTER_SNO },
    { MARKERSET_SNO, ACTOR_SNO },
    { MARKERSET_SNO, SCRIPT_SNO },
    { MARKERSET_SNO, CONVERSATIONLIST_SNO },
    { MARKERSET_SNO, CONDUCTOR_SNO },
    { MARKERSET_SNO, CONVERSATION_SNO },
    { MARKERSET_SNO, ADVENTURE_SNO },
    { MARKERSET_SNO, WEATHER_SNO },
    { MARKERSET_SNO, MUSIC_SNO },
    { ACTOR_SNO, ACTOR_SNO },
    { ADVENTURE_SNO, ENCOUNTER_SNO },
    { ADVENTURE_SNO, MARKERSET_SNO },
    { WEATHER_SNO, LIGHT_SNO },
    { WEATHER_SNO, PARTICLE_SNO },
    { WEATHER_SNO, SOUND_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saWorldClientExplore[26] = {  // line 807
    { WORLD_SNO, MARKERSET_SNO },
    { WORLD_SNO, WEATHER_SNO },
    { WORLD_SNO, AMBIENTSOUND_SNO },
    { WORLD_SNO, MUSIC_SNO },
    { MARKERSET_SNO, LIGHT_SNO },
    { MARKERSET_SNO, PARTICLE_SNO },
    { MARKERSET_SNO, AMBIENTSOUND_SNO },
    { MARKERSET_SNO, ENCOUNTER_SNO },
    { MARKERSET_SNO, ACTOR_SNO },
    { MARKERSET_SNO, SCRIPT_SNO },
    { MARKERSET_SNO, CONVERSATIONLIST_SNO },
    { MARKERSET_SNO, CONDUCTOR_SNO },
    { MARKERSET_SNO, CONVERSATION_SNO },
    { MARKERSET_SNO, ADVENTURE_SNO },
    { MARKERSET_SNO, WEATHER_SNO },
    { MARKERSET_SNO, MUSIC_SNO },
    { ACTOR_SNO, ACTOR_SNO },
    { ADVENTURE_SNO, ENCOUNTER_SNO },
    { ADVENTURE_SNO, MARKERSET_SNO },
    { WEATHER_SNO, LIGHT_SNO },
    { WEATHER_SNO, PARTICLE_SNO },
    { WEATHER_SNO, SOUND_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { MUSIC_SNO, SOUNDBANK_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saPowerClientExplore[24] = {  // line 819
    { SKILLKIT_SNO, POWER_SNO },
    { POWER_SNO, EFFECTGROUP_SNO },
    { POWER_SNO, POWER_SNO },
    { POWER_SNO, ROPE_SNO },
    { POWER_SNO, ACTOR_SNO },
    { POWER_SNO, CONVERSATION_SNO },
    { POWER_SNO, AISTATE_SNO },
    { EFFECTGROUP_SNO, PARTICLE_SNO },
    { EFFECTGROUP_SNO, EFFECTGROUP_SNO },
    { EFFECTGROUP_SNO, SOUND_SNO },
    { EFFECTGROUP_SNO, SHAKE_SNO },
    { EFFECTGROUP_SNO, ACTOR_SNO },
    { EFFECTGROUP_SNO, AMBIENTSOUND_SNO },
    { EFFECTGROUP_SNO, ROPE_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { PARTICLE_SNO, SOUND_SNO },
    { PARTICLE_SNO, AMBIENTSOUND_SNO },
    { PARTICLE_SNO, ACTOR_SNO },
    { PARTICLE_SNO, PARTICLE_SNO },
    { PARTICLE_SNO, EFFECTGROUP_SNO },
    { ROPE_SNO, PARTICLE_SNO },
    { ROPE_SNO, AMBIENTSOUND_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saQuestClientExplore[13] = {  // line 831
    { QUEST_SNO, TREASURECLASS_SNO },
    { QUEST_SNO, QUEST_SNO },
    { QUEST_SNO, STRINGLIST_SNO },
    { QUEST_SNO, CONVERSATION_SNO },
    { CONVERSATION_SNO, SOUND_SNO },
    { CONVERSATION_SNO, QUEST_SNO },
    { CONVERSATION_SNO, CONVERSATION_SNO },
    { CONVERSATION_SNO, STRINGLIST_SNO },
    { AMBIENTSOUND_SNO, SOUND_SNO },
    { SOUND_SNO, SOUNDBANK_SNO },
    { TREASURECLASS_SNO, TREASURECLASS_SNO },
    { TREASURECLASS_SNO, CONDITION_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saUIClientExplore[4] = {  // line 841
    { UI_SNO, LIGHT_SNO },
    { UI_SNO, SOUND_SNO },
    { UI_SNO, ACTOR_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild * saShouldExploreFilter[FILTERSNOTYPE_COUNT][FILTERTYPE_COUNT] = {  // line 847
    { saActorClientExplore, saActorServerExplore },
    { saSceneClientExplore, saSceneServerExplore },
    { saSceneClientExplore, saSceneServerExplore },
    { saWorldLoadingClientExplore, saWorldLoadingServerExplore },
    { saLevelAreaClientExplore, saLevelAreaServerExplore },
    { saWorldClientExplore, saWorldServerExplore },
    { saPowerClientExplore, saPowerServerExplore },
    { saUIClientExplore, saUIServerExplore },
    { saQuestClientExplore, saQuestServerExplore },
};

ParentAndChildBitField saShouldExploreBitFilter[FILTERSNOTYPE_COUNT][FILTERTYPE_COUNT];  // line 860

ParentAndChild saActorClientUse[9] = {  // line 915
    { SNOGROUP_NONE, PHYSMESH_SNO },
    { SNOGROUP_NONE, SOUND_SNO },
    { ACTOR_SNO, APPEARANCE_SNO },
    { SNOGROUP_NONE, MUSIC_SNO },
    { ACTOR_SNO, TEXTURE_SNO },
    { PARTICLE_SNO, TEXTURE_SNO },
    { TRAIL_SNO, TEXTURE_SNO },
    { ROPE_SNO, TEXTURE_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saActorServerUse[2] = {  // line 941
    { SNOGROUP_NONE, PHYSMESH_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saSceneClientUse[8] = {  // line 958
    { SNOGROUP_NONE, SCENE_SNO },
    { SNOGROUP_NONE, PHYSMESH_SNO },
    { SNOGROUP_NONE, SOUND_SNO },
    { SCENE_SNO, APPEARANCE_SNO },
    { SNOGROUP_NONE, MUSIC_SNO },
    { WEATHER_SNO, TEXTURE_SNO },
    { SCENE_SNO, TEXTURE_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saSceneServerUse[3] = {  // line 983
    { SNOGROUP_NONE, SCENE_SNO },
    { SNOGROUP_NONE, PHYSMESH_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saSceneLoadingClientUse[1] = {  // line 999
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saSceneLoadingServerUse[1] = {  // line 1006
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saWorldLoadingClientUse[2] = {  // line 1013
    { SNOGROUP_NONE, SCENE_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saWorldLoadingServerUse[2] = {  // line 1022
    { SNOGROUP_NONE, SCENE_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saLevelAreaClientUse[8] = {  // line 1032
    { SNOGROUP_NONE, SOUND_SNO },
    { SNOGROUP_NONE, MUSIC_SNO },
    { ACTOR_SNO, TEXTURE_SNO },
    { WEATHER_SNO, TEXTURE_SNO },
    { PARTICLE_SNO, TEXTURE_SNO },
    { TRAIL_SNO, TEXTURE_SNO },
    { ROPE_SNO, TEXTURE_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saLevelAreaServerUse[1] = {  // line 1059
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saWorldClientUse[10] = {  // line 1074
    { SNOGROUP_NONE, SCENE_SNO },
    { SNOGROUP_NONE, SOUND_SNO },
    { SNOGROUP_NONE, MUSIC_SNO },
    { WORLD_SNO, TEXTURE_SNO },
    { ACTOR_SNO, TEXTURE_SNO },
    { WEATHER_SNO, TEXTURE_SNO },
    { PARTICLE_SNO, TEXTURE_SNO },
    { TRAIL_SNO, TEXTURE_SNO },
    { ROPE_SNO, TEXTURE_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saWorldServerUse[2] = {  // line 1098
    { SNOGROUP_NONE, SCENE_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saPowerClientUse[3] = {  // line 1114
    { PARTICLE_SNO, TEXTURE_SNO },
    { SNOGROUP_NONE, SOUND_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saPowerServerUse[1] = {  // line 1126
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saUIClientUse[3] = {  // line 1133
    { SNOGROUP_NONE, SOUND_SNO },
    { SNOGROUP_NONE, TEXTURE_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saUIServerUse[1] = {  // line 1144
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saQuestClientUse[2] = {  // line 1149
    { SNOGROUP_NONE, SOUND_SNO },
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild saQuestServerUse[1] = {  // line 1156
    { SNOGROUP_NONE, SNOGROUP_NONE },
};

ParentAndChild * saShouldUseFilter[FILTERSNOTYPE_COUNT][FILTERTYPE_COUNT] = {  // line 1165
    { saActorClientUse, saActorServerUse },
    { saSceneClientUse, saSceneServerUse },
    { saSceneLoadingClientUse, saSceneLoadingServerUse },
    { saWorldLoadingClientUse, saWorldLoadingServerUse },
    { saLevelAreaClientUse, saLevelAreaServerUse },
    { saWorldClientUse, saWorldServerUse },
    { saPowerClientUse, saPowerServerUse },
    { saUIClientUse, saUIServerUse },
    { saQuestClientUse, saQuestServerUse },
};

ParentAndChildBitField saShouldUseBitFilter[FILTERSNOTYPE_COUNT][FILTERTYPE_COUNT];  // line 1178

inline BOOL FilterShouldExplore(
    FilteringSNOTypes eSNOType,
    FilteringTypes eFilterType,
    SNOGroup snoParent,
    SNOGroup snoChild)
{

    uint32 dwBitIndex = (snoParent * MAX_SNO_GROUPS) + snoChild;
    uint32 dwIndex = dwBitIndex / 32;
    uint32 dwSubBit = dwBitIndex % 32;

    if (eFilterType == FILTERTYPE_BOTH)
    {
        return (saShouldExploreBitFilter[eSNOType][FILTERTYPE_CLIENT][dwIndex] & (1 << dwSubBit)) || (saShouldExploreBitFilter[eSNOType][FILTERTYPE_SERVER][dwIndex] & (1 << dwSubBit));
    }
    else
    {
        return (saShouldExploreBitFilter[eSNOType][eFilterType][dwIndex] >> dwSubBit) & 1;
    }
}






























inline BOOL FilterShouldUse(
    FilteringSNOTypes eSNOType,
    FilteringTypes eFilterType,
    SNOGroup snoParent,
    SNOGroup snoChild)
{

    uint32 dwBitIndex = (snoParent * MAX_SNO_GROUPS) + snoChild;
    uint32 dwIndex = dwBitIndex / 32;
    uint32 dwSubBit = dwBitIndex % 32;

    uint32 dwNoneBitIndex = snoChild;
    uint32 dwNoneIndex = dwNoneBitIndex / 32;
    uint32 dwNoneSubBit = dwNoneBitIndex % 32;

    if (eFilterType == FILTERTYPE_BOTH)
    {
        const ParentAndChildBitField & tClientUseBitFilter = saShouldUseBitFilter[eSNOType][FILTERTYPE_CLIENT];
        const ParentAndChildBitField & tServerUseBitFilter = saShouldUseBitFilter[eSNOType][FILTERTYPE_SERVER];

        return (tClientUseBitFilter[dwIndex] & (1 << dwSubBit)) || (tServerUseBitFilter[dwIndex] & (1 << dwSubBit)) ||
            (tClientUseBitFilter[dwNoneIndex] & (1 << dwNoneSubBit)) || (tServerUseBitFilter[dwNoneIndex] & (1 << dwNoneSubBit));
    }
    else
    {
        const ParentAndChildBitField & tUseBitFilter = saShouldUseBitFilter[eSNOType][eFilterType];

        return (tUseBitFilter[dwIndex] & (1 << dwSubBit)) || (tUseBitFilter[dwNoneIndex] & (1 << dwNoneSubBit));
    }
}






























struct ACDSNOAndRefCount {  // size 0x2c
    SNO sno;  // 0x00
    int32 nRefCount;  // 0x04
    WeaponClass eWeaponClass;  // 0x08
    GBID gbidMonsterAffixes[8];  // 0x0c
};

struct SceneSNOAndRefCount {  // size 0x18
    SNO sno;  // 0x00
    SNO snoLevelAreas[4];  // 0x04
    int32 nRefCount;  // 0x14
};



enum PreloadTaskType {
    PRELOADTASKTYPE_ADD_TREE = 0,
    PRELOADTASKTYPE_CHANGE_TREE = 1,
    PRELOADTASKTYPE_REMOVE_TREE = 2,
};

struct PreloadTask {  // size 0x68
    SNOPreload::PreloadTaskType eType;  // 0x00
    BOOL fClient;  // 0x04
    BOOL fBlocking;  // 0x08
    BOOL fPurgeNoLongerRequired;  // 0x0c
    BOOL fNeutralAnimOnly;  // 0x10
    BOOL fAddNew;  // 0x14
    BOOL fRemoveOld;  // 0x18
    SNOGroup snoGroup;  // 0x1c
    SNO sno;  // 0x20
    SNO newSno;  // 0x24
    SNOManager::SNOCategory category;  // 0x28
    SNOManager::SNOContainer * ptSNOs;  // 0x30
    SNOFilesAsyncPriority ePriority;  // 0x38
    union {
        Prefetch::FilterFunc ptFilterFunc;
        Prefetch::FilterFunc ptBeforeFilterFunc;
    };  // 0x40
    Prefetch::FilterFunc ptAfterFilterFunc;  // 0x48
    union {
        WeaponClass eWeaponClass;
        WeaponClass eWeaponClassBefore;
    };  // 0x50
    WeaponClass eWeaponClassAfter;  // 0x54
    Blizzard::Lock::REvent * m_ptTaskCompleted;  // 0x58
    SNOName * m_ptWaitReason;  // 0x60
};





struct PreloadAsyncInfo {  // size 0xa0
    Blizzard::Thread::Thread tPreloadAsyncThread;  // 0x00
    Blizzard::Lock::RMutex tRequestCrit;  // 0x08
    XGrowableList<SNOPreload::PreloadTask, const SNOPreload::PreloadTask &> pendingPreloadRequestList;  // 0x28
    BOOL fTerminateAsyncThread;  // 0x80
    Blizzard::Lock::REvent theresWorkToDoEvent;  // 0x84
    Blizzard::Lock::REvent someWorkCompletedEvent;  // 0x90
};

typedef XGrowableMap<CompressedSNOName, const CompressedSNOName &, int, int> SNONameRefMap;



class Impl {  // size 0xd38
public:
    Impl();
    void SetAct(Act eAct);
    void RemoveAllClientSNOs(BOOL fFromActChange);
    void RemoveAllServerSNOs(BOOL fFromActChange);
    void AddClientUI(const SNO sno);
    BOOL AreAllClientUIChildrenLoaded(const SNO sno);
    BOOL AddClientUIChildren(const SNO sno);
    void RemoveClientUIChildren(const SNO sno);
    void WaitForClientUIChildren(const SNO sno);
    void RemoveClientBaseUIOnly(const SNO sno);
    void WaitForAllUIRoots();
    void SetClientWorld(const SNO sno);
    SNO GetClientWorld();
    void WaitForFullClientWorld(const SNO snoWorld);
    void AddFullClientWorld(const SNO sno, BOOL fWaitForSNOs);
    void AddNeutralClientActor(const SNO sno, BOOL fWaitForSNOs);
    BOOL PrefetchClientWorld(const SNO snoWorld, SNOFilesAsyncPriority ePriority);
    BOOL PrefetchClientScene(const SNO snoScene, SNOFilesAsyncPriority ePriority);
    BOOL PrefetchClientLevelArea(const SNO snoLevelArea, SNOFilesAsyncPriority ePriority);
    void AddClientScene(const SSceneID idSScene, const SNO sno, const SNO * ptSNOLevelAreas);
    void SetPrefetchClientSceneEnabled(BOOL fEnabled);
    void WaitForClientScene(const SNO snoScene);
    void RemoveClientScene(const SSceneID idSScene);
    void AddClientTextures(const SNONameArray & snoTextures);
    void RemoveClientTextures(const SNONameArray & snoTextures);
    void AddClientSceneBySNO(const SNO sno);
    void RemoveClientSceneBySNO(const SNO sno);
    void AddClientWeatherBySNO(const SNO sno);
    void RemoveClientWeatherBySNO(const SNO sno);
    BOOL PrefetchClientWeather(const SNO sno, SNOFilesAsyncPriority ePriority);
    BOOL PrefetchClientActor(const SNO sno, const GBID * pgbidMonsterAffixes, SNOFilesAsyncPriority ePriority);
    BOOL IsClientActorRequested(const ACDNetworkName ann);
    BOOL IsClientActorLoaded(const ACDNetworkName ann);
    BOOL IsClientActorPending(const ACDNetworkName ann);
    void WaitForClientActor(const ACDNetworkName ann);
    void AddClientActor(const ACDNetworkName ann, const SNO sno, const WeaponClass eWeaponClass, const GBID * pgbidMonsterAffixes);
    void RemoveClientActor(const ACDNetworkName ann);
    void AddClientActorBySNO(const SNO sno);
    void RemoveClientActorBySNO(const SNO sno);
    void DumpRoots(SNOPreload::RootTypes, SNOGroup);
    void AddServerActorBySNO(const SNO sno);
    void RemoveServerActorBySNO(const SNO sno);
    void PrefetchServerActorAvenger(const SNO snoActor, const GBID * pgbidMonsterAffixes);
    void RemoveServerWorld(const SNO snoWorld);
    void AddServerWorld(const SNO snoWorld);
    void RemoveServerWorldLoading(const SNO snoWorld, BOOL fDungeonFinder);
    void AddServerWorldLoading(const SNO snoWorld, BOOL fDungeonFinder);
    void PrefetchWorldLoading(const SNO snoWorld, SNOFilesAsyncPriority ePriority);
    BOOL IsServerWorldLoadingDone(const SNO snoWorld, BOOL fDungeonFinder);
    void PrefetchServerScene(const SNO sno, const SNO * ptSNOLevelAreas);
    void AddServerSceneBySNO(const SNO sno);
    void RemoveServerSceneBySNO(const SNO sno);
    void RemoveServerSkillKit(const SNO sno);
    void AddServerSkillKit(const SNO sno);
    void AddServerLevelArea(const SNO sno);
    void RemoveServerLevelArea(const SNO sno);
    void WaitForServerLevelArea(const SNO);
    BOOL ApplyHotfix(const char * szHotfixName);
    void UpdateSkills();
    void UpdateEquipped();
    void UpdateCrafters();
    void UpdateHireling();
    void WaitForAllRequests();
    void InitializeForApp();
    BOOL IsInitializedForApp();
    void InitializeForGame();
    BOOL IsInitializedForGame();
    void DisposeForGame();
    void DisposeForApp();
    struct SkillInfo {  // size 0xc
        SNO snoPower;  // 0x00
        SNO snoAnimSet;  // 0x04
        WeaponClass eWeaponClass;  // 0x08
        BOOL operator==(const SNOPreload::Impl::SkillInfo & info) const;
    };
    struct ItemInfo {  // size 0x8
        SNOGroup snoGroup;  // 0x00
        SNO snoHandle;  // 0x04
        BOOL operator==(const SNOPreload::Impl::ItemInfo & info) const;
    };
    struct CrafterInfo {  // size 0xc
        CrafterType eType;  // 0x00
        SNO snoAnimSet;  // 0x04
        uint32 dwRank;  // 0x08
        BOOL operator==(const SNOPreload::Impl::CrafterInfo & info) const;
    };
private:
    BOOL PrefetchSNOTree(BOOL fClient, const SNOGroup snoGroup, const SNO sno, Prefetch::FilterFunc ptFilterFunc, BOOL fIncludeRoot, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly, SNOFilesAsyncPriority ePriority);
    void AddSNOArray(SNOPreload::SNONameRefMap & tRequiredRootNodes, SNOManager::SNOContainer & SNOs, const SNONameArray & snos, const SNOManager::SNOCategory category, BOOL fBlocking);
    BOOL AddSNOTree(BOOL fClient, const SNOGroup snoGroup, SNOPreload::SNONameRefMap & tRequiredRootNodes, SNOManager::SNOContainer & SNOs, const SNO sno, Prefetch::FilterFunc ptFilterFunc, const SNOManager::SNOCategory category, BOOL fBlocking, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly, SNOFilesAsyncPriority ePriority);
    void ChangeSNOTree(BOOL fClient, const SNOGroup snoGroup, SNOPreload::SNONameRefMap & tRequiredRootNodes, SNOManager::SNOContainer & SNOs, const SNO sno, Prefetch::FilterFunc ptBeforeFilterFunc, const SNO newSno, Prefetch::FilterFunc ptAfterFilterFunc, const SNOManager::SNOCategory category, BOOL fPurgeNoLongerRequired, BOOL fBlocking, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly, SNOFilesAsyncPriority ePriority);
    void RemoveSNOTree(BOOL fClient, const SNOGroup snoGroup, SNOPreload::SNONameRefMap & tRequiredRootNodes, SNOManager::SNOContainer & SNOs, const SNO sno, Prefetch::FilterFunc ptFilterFunc, const SNOManager::SNOCategory category, BOOL fPurgeNoLongerRequired, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly);
    void RemoveSNOArray(SNOPreload::SNONameRefMap & tRequiredRootNodes, SNOManager::SNOContainer & SNOs, const SNONameArray & snos, const SNOManager::SNOCategory category, BOOL fPurgeNoLongerRequired);
    static void sAddSNOTree(const SNOPreload::PreloadTask & task);
    static void sChangeSNOTree(const SNOPreload::PreloadTask & task);
    static void sRemoveSNOTree(const SNOPreload::PreloadTask & task);
    void WaitForSNOTree(BOOL fClient, const SNOGroup snoGroup, const SNO sno, Prefetch::FilterFunc ptFilterFunc, BOOL fIncludeRoot, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly);
    BOOL IsSNOTreeLoaded(BOOL fClient, const SNOGroup snoGroup, const SNO sno, Prefetch::FilterFunc ptFilterFunc, BOOL fIncludeRoot, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly);
    BOOL IsSNOTreePending(BOOL fClient, const SNOGroup snoGroup, const SNO sno, Prefetch::FilterFunc ptFilterFunc, BOOL fIncludeRoot, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly);
    static void sAppendRequiredAnimations(SNOManager::SNOContainer & SNOs, const WeaponClass eWeaponClass, Prefetch::FilterFunc ptFilterFunc, BOOL fNeutralAnimOnly);
    static void sGetAnimSetAnimations(const SNO snoAnimSet, const WeaponClass eWeaponClass, Prefetch::ChildSNOContainer & tAnimSNOs, BOOL fWillLoadChildren);
    void GetPowerAnimations(const SNO snoPower, const SNO snoAnimSet, const WeaponClass eWeaponClass, Prefetch::ChildSNOContainer & tAnimSNOs);
    static void sGetAnimSetNeutralAnimations(const SNO snoAnimSet, const WeaponClass eWeaponClass, Prefetch::ChildSNOContainer & tAnimSNOs);
    static void sSubmitRequest(const SNOPreload::PreloadTask & task);
    static BOOL sWaitForRequests(BOOL fClient, SNO sno, BOOL fAllowCancel);
    static void sCancelRequests(BOOL fClient);
    static BOOL sIsRequestPending(BOOL fClient, SNO sno);
    static uint PreloadAsyncThread(void * pPreloadAsyncInfo);
    static SNOPreload::PreloadAsyncInfo s_tClientPreloadAsync;
    static SNOPreload::PreloadAsyncInfo s_tServerPreloadAsync;
    SNOPreload::SNONameRefMap m_serverRequiredRootNodes;  // 0x00
    SNOPreload::SNONameRefMap m_serverPerActRequiredRootNodes;  // 0x80
    SNOManager::SNOContainer m_serverPerActSceneSNOs;  // 0x100
    SNOManager::SNOContainer m_serverPerActLevelAreaSNOs;  // 0x180
    SNOManager::SNOContainer m_serverPerActWorldSNOs;  // 0x200
    SNOManager::SNOContainer m_serverPerActWorldLoadingSNOs;  // 0x280
    SNOManager::SNOContainer m_serverActorSNOs;  // 0x300
    Blizzard::Lock::RMutex tServerCrit;  // 0x380
    XStringArray m_appliedHotfixes;  // 0x3a0
    Blizzard::Lock::REvent m_tServerTaskCompleted;  // 0x3b8
    SNOName m_tServerWaitReason;  // 0x3c4
    Act m_eCurrentAct;  // 0x3cc
    Blizzard::Lock::RMutex tClientCrit;  // 0x3d0
    SNO m_clientCrafterActorSNO[5];  // 0x3f0
    XGrowableMap<SNOPreload::Impl::SkillInfo, const SNOPreload::Impl::SkillInfo &, int, int> m_clientSkills;  // 0x408
    XGrowableMap<SNOPreload::Impl::ItemInfo, const SNOPreload::Impl::ItemInfo &, int, int> m_clientItems;  // 0x488
    XGrowableMap<SNOPreload::Impl::CrafterInfo, const SNOPreload::Impl::CrafterInfo &, int, int> m_clientCrafters;  // 0x508
    HirelingClass m_currentHireling;  // 0x588
    SNOPreload::SNONameRefMap m_clientPerActRequiredRootNodes;  // 0x590
    SNOPreload::SNONameRefMap m_clientRequiredRootNodes;  // 0x610
    SNOPreload::SNONameRefMap m_clientUIChildrenRequiredRootNodes;  // 0x690
    SNOPreload::SNONameRefMap m_clientHirelingRequiredRootNodes;  // 0x710
    XGrowableMap<ACDNetworkName, ACDNetworkName, SNOPreload::ACDSNOAndRefCount, const SNOPreload::ACDSNOAndRefCount &> m_clientANNToSNO;  // 0x790
    XGrowableMap<int, int, SNOPreload::SceneSNOAndRefCount, const SNOPreload::SceneSNOAndRefCount &> m_clientSSceneIDToSNO;  // 0x810
    SNOManager::SNOContainer m_clientPerActWorldSNOs;  // 0x890
    SNOManager::SNOContainer m_clientPerActSceneSNOs;  // 0x910
    SNOManager::SNOContainer m_clientPerActActorSNOs;  // 0x990
    SNOManager::SNOContainer m_clientPerActTextureSNOs;  // 0xa10
    SNOManager::SNOContainer m_clientUISNOs;  // 0xa90
    SNOManager::SNOContainer m_clientSkillSNOs;  // 0xb10
    SNOManager::SNOContainer m_clientCrafterSNOs;  // 0xb90
    SNOManager::SNOContainer m_clientActorSNOs;  // 0xc10
    SNOManager::SNOContainer m_clientHirelingSNOs;  // 0xc90
    Blizzard::Lock::REvent m_tClientTaskCompleted;  // 0xd10
    SNOName m_tClientWaitReason;  // 0xd1c
    SNO m_clientWorld;  // 0xd24
    BOOL m_fInitializedForApp;  // 0xd28
    BOOL m_fInitializedForGame;  // 0xd2c
    BOOL m_fPrefetchClientSceneEnabled;  // 0xd30
};
static_assert(sizeof(Impl) == 0xd38, "Impl");































inline BOOL Impl::SkillInfo::operator==(const SNOPreload::Impl::SkillInfo & info) const
{
    return snoPower == info.snoPower && snoAnimSet == info.snoAnimSet && eWeaponClass == info.eWeaponClass;
}







inline BOOL Impl::ItemInfo::operator==(const SNOPreload::Impl::ItemInfo & info) const
{
    return snoHandle == info.snoHandle;
}








inline BOOL Impl::CrafterInfo::operator==(const SNOPreload::Impl::CrafterInfo & info) const
{
    return eType == info.eType && snoAnimSet == info.snoAnimSet && dwRank == info.dwRank;
}





































































































































struct AnimSetToCrafterType {  // size 0x18
    SNOGroup snoGroup;  // 0x00
    LPCSTR szName;  // 0x08
    SNO snoHandle;  // 0x10
    CrafterType eType;  // 0x14
};

struct ActorToCrafterType {  // size 0x18
    SNOGroup snoGroup;  // 0x00
    LPCSTR szName;  // 0x08
    SNO snoHandle;  // 0x10
    CrafterType eType;  // 0x14
};

struct AnimSetToWeaponClass {  // size 0x18
    SNOGroup snoGroup;  // 0x00
    LPCSTR szName;  // 0x08
    SNO snoHandle;  // 0x10
    WeaponClass eWeaponClass;  // 0x14
};

struct ResolveableSNOName {  // size 0x18
    SNOGroup snoGroup;  // 0x00
    LPCSTR szName;  // 0x08
    SNO snoHandle;  // 0x10
};

struct ActorToHirelingClass {  // size 0x20
    SNOGroup snoGroup;  // 0x00
    LPCSTR szName;  // 0x08
    SNO snoHandle;  // 0x10
    HirelingClass eClass;  // 0x14
    BOOL fHired;  // 0x18
};

struct HirelingClassToHirelingActor {  // size 0x18
    HirelingClass eClass;  // 0x00
    SNOGroup snoGroup;  // 0x04
    LPCSTR szName;  // 0x08
    SNO snoHandle;  // 0x10
};























PreloadAsyncInfo Impl::s_tClientPreloadAsync;

PreloadAsyncInfo Impl::s_tServerPreloadAsync;








static Impl s_tPreloadImpl;









static AnimSetToCrafterType s_tCrafterAnimSet[7] = {
    { ANIMSET_SNO, "Blacksmith_Crafting_Backbone", SNO_NONE, 0 },
    { ANIMSET_SNO, "Jeweler_Crafting_Backbone", SNO_NONE, 1 },
    { ANIMSET_SNO, "Jeweler_Crafting_Backbone_Tristram", SNO_NONE, 1 },
    { ANIMSET_SNO, "Mystic_Crafting_Backbone", SNO_NONE, 2 },
    { ANIMSET_SNO, "Mystic_Crafting_Backbone_Keep", SNO_NONE, 2 },
    { ANIMSET_SNO, "Mystic_Crafting_Backbone_Tristram", SNO_NONE, 2 },
    { ANIMSET_SNO, "Mystic_Crafting_Backbone_Act5", SNO_NONE, 2 },
};









static ActorToCrafterType s_tCrafterActor[7] = {
    { ACTOR_SNO, "Blacksmith_Crafting_Backbone", SNO_NONE, 0 },
    { ACTOR_SNO, "Jeweler_Crafting_Backbone", SNO_NONE, 1 },
    { ACTOR_SNO, "Jeweler_Crafting_Backbone_Tristram", SNO_NONE, 1 },
    { ACTOR_SNO, "Mystic_Crafting_Backbone", SNO_NONE, 2 },
    { ACTOR_SNO, "Mystic_Crafting_Backbone_Keep", SNO_NONE, 2 },
    { ACTOR_SNO, "Mystic_Crafting_Backbone_Tristram", SNO_NONE, 2 },
    { ACTOR_SNO, "Mystic_Crafting_Backbone_Act5", SNO_NONE, 2 },
};









// The weapon classes are not in the binary (only the handles survive); WEAPONCLASS_NONE stands in.
static AnimSetToWeaponClass s_tWeaponClassAnimSet[3] = {
    { ANIMSET_SNO, "Ancient_Korlic", SNO_NONE, WEAPONCLASS_NONE },
    { ANIMSET_SNO, "Ancient_Mawdawc", SNO_NONE, WEAPONCLASS_NONE },
    { ANIMSET_SNO, "Ancient_Talic", SNO_NONE, WEAPONCLASS_NONE },
};







static ResolveableSNOName s_tHeroAnimSet[17] = {
    { ANIMSET_SNO, "Wizard_Male", SNO_NONE },
    { ANIMSET_SNO, "Wizard_Female", SNO_NONE },
    { ANIMSET_SNO, "Monk_Male", SNO_NONE },
    { ANIMSET_SNO, "Monk_Female", SNO_NONE },
    { ANIMSET_SNO, "Barbarian_Male", SNO_NONE },
    { ANIMSET_SNO, "Barbarian_Female", SNO_NONE },
    { ANIMSET_SNO, "WitchDoctor_Male", SNO_NONE },
    { ANIMSET_SNO, "WitchDoctor_Female", SNO_NONE },
    { ANIMSET_SNO, "DemonHunter_Male", SNO_NONE },
    { ANIMSET_SNO, "DemonHunter_Female", SNO_NONE },
    { ANIMSET_SNO, "DemonHunter_Female_backflip", SNO_NONE },
    { ANIMSET_SNO, "Barbarian_hammeroftheAncients_ancient", SNO_NONE },
    { ANIMSET_SNO, "Monk_Female_mysticAlly", SNO_NONE },
    { ANIMSET_SNO, "x1_Crusader_Male", SNO_NONE },
    { ANIMSET_SNO, "x1_Crusader_Female", SNO_NONE },
    { ANIMSET_SNO, "p6_Necro_Male", SNO_NONE },
    { ANIMSET_SNO, "p6_Necro_Female", SNO_NONE },
};

static ResolveableSNOName s_tHeroActor[15] = {
    { ACTOR_SNO, "Wizard_Male", SNO_NONE },
    { ACTOR_SNO, "Wizard_Female", SNO_NONE },
    { ACTOR_SNO, "Monk_Male", SNO_NONE },
    { ACTOR_SNO, "Monk_Female", SNO_NONE },
    { ACTOR_SNO, "Barbarian_Male", SNO_NONE },
    { ACTOR_SNO, "Barbarian_Female", SNO_NONE },
    { ACTOR_SNO, "WitchDoctor_Male", SNO_NONE },
    { ACTOR_SNO, "WitchDoctor_Female", SNO_NONE },
    { ACTOR_SNO, "DemonHunter_Male", SNO_NONE },
    { ACTOR_SNO, "DemonHunter_Female", SNO_NONE },
    { ACTOR_SNO, "DemonHunter_Female_backflip", SNO_NONE },
    { ACTOR_SNO, "x1_Crusader_Male", SNO_NONE },
    { ACTOR_SNO, "x1_Crusader_Female", SNO_NONE },
    { ACTOR_SNO, "p6_Necro_Male", SNO_NONE },
    { ACTOR_SNO, "p6_Necro_Female", SNO_NONE },
};










static ActorToHirelingClass s_tActorToHirelingClass[6] = {
    { ACTOR_SNO, "Scoundrel", SNO_NONE, 2, FALSE },
    { ACTOR_SNO, "Templar", SNO_NONE, 1, FALSE },
    { ACTOR_SNO, "Enchantress", SNO_NONE, 3, FALSE },
    { ACTOR_SNO, "Hireling_Scoundrel", SNO_NONE, 2, TRUE },
    { ACTOR_SNO, "Hireling_Templar", SNO_NONE, 1, TRUE },
    { ACTOR_SNO, "Hireling_Enchantress", SNO_NONE, 3, TRUE },
};









static HirelingClassToHirelingActor s_tHirelingClassToHirelingActor[3] = {
    { 2, ACTOR_SNO, "Hireling_Scoundrel", SNO_NONE },
    { 1, ACTOR_SNO, "Hireling_Templar", SNO_NONE },
    { 3, ACTOR_SNO, "Hireling_Enchantress", SNO_NONE },
};

static ResolveableSNOName s_tHeroAppearance[15] = {
    { APPEARANCE_SNO, "Wizard_Male", SNO_NONE },
    { APPEARANCE_SNO, "Wizard_Female", SNO_NONE },
    { APPEARANCE_SNO, "Monk_Male", SNO_NONE },
    { APPEARANCE_SNO, "Monk_Female", SNO_NONE },
    { APPEARANCE_SNO, "Barbarian_Male", SNO_NONE },
    { APPEARANCE_SNO, "Barbarian_Female", SNO_NONE },
    { APPEARANCE_SNO, "WitchDoctor_Male", SNO_NONE },
    { APPEARANCE_SNO, "WitchDoctor_Female", SNO_NONE },
    { APPEARANCE_SNO, "DemonHunter_Male", SNO_NONE },
    { APPEARANCE_SNO, "DemonHunter_Female", SNO_NONE },
    { APPEARANCE_SNO, "DemonHunter_Female_backflip", SNO_NONE },
    { APPEARANCE_SNO, "x1_Crusader_Male", SNO_NONE },
    { APPEARANCE_SNO, "x1_Crusader_Female", SNO_NONE },
    { APPEARANCE_SNO, "p6_Necro_Male", SNO_NONE },
    { APPEARANCE_SNO, "p6_Necro_Female", SNO_NONE },
};

static ResolveableSNOName s_tComplexAnimSet[33] = {
    { ANIMSET_SNO, "Wizard_Male", SNO_NONE },
    { ANIMSET_SNO, "Wizard_Female", SNO_NONE },
    { ANIMSET_SNO, "Monk_Male", SNO_NONE },
    { ANIMSET_SNO, "Monk_Female", SNO_NONE },
    { ANIMSET_SNO, "Monk_Female_mysticAlly", SNO_NONE },
    { ANIMSET_SNO, "Barbarian_Male", SNO_NONE },
    { ANIMSET_SNO, "Barbarian_Female", SNO_NONE },
    { ANIMSET_SNO, "Barbarian_hammeroftheAncients_ancient", SNO_NONE },
    { ANIMSET_SNO, "Ancient_Korlic", SNO_NONE },
    { ANIMSET_SNO, "Ancient_Mawdawc", SNO_NONE },
    { ANIMSET_SNO, "Ancient_Talic", SNO_NONE },
    { ANIMSET_SNO, "WitchDoctor_Male", SNO_NONE },
    { ANIMSET_SNO, "WitchDoctor_Female", SNO_NONE },
    { ANIMSET_SNO, "DemonHunter_Male", SNO_NONE },
    { ANIMSET_SNO, "DemonHunter_Female", SNO_NONE },
    { ANIMSET_SNO, "DemonHunter_Female_backflip", SNO_NONE },
    { ANIMSET_SNO, "x1_Crusader_Male", SNO_NONE },
    { ANIMSET_SNO, "x1_Crusader_Female", SNO_NONE },
    { ANIMSET_SNO, "p6_Necro_Male", SNO_NONE },
    { ANIMSET_SNO, "p6_Necro_Female", SNO_NONE },
    { ANIMSET_SNO, "Enchantress", SNO_NONE },
    { ANIMSET_SNO, "Templar", SNO_NONE },
    { ANIMSET_SNO, "Scoundrel", SNO_NONE },
    { ANIMSET_SNO, "OmniNPC_Male", SNO_NONE },
    { ANIMSET_SNO, "OmniNPC_Female", SNO_NONE },
    { ANIMSET_SNO, "OmniNPC_Tyrael", SNO_NONE },
    { ANIMSET_SNO, "OmniNPC_Male_TyraelPurpose", SNO_NONE },
    { ANIMSET_SNO, "OmniNPC_Male_imperius", SNO_NONE },
    { ANIMSET_SNO, "OmniNPC_Male_imperius_TyraelPurpose", SNO_NONE },
    { ANIMSET_SNO, "OmniNPC_Male_Blacksmith_Eventv", SNO_NONE },
    { ANIMSET_SNO, "OmniNPC_Male_Stranger", SNO_NONE },
    { ANIMSET_SNO, "OmniNPC_Adventurer_A_Ghost", SNO_NONE },
    { ANIMSET_SNO, "QueenLeoric", SNO_NONE },
};

static ResolveableSNOName s_tHeavyPowers[2] = {
    { POWER_SNO, "ItemPassive_Unique_Ring_600_x1", SNO_NONE },
    { POWER_SNO, "ItemPassive_Unique_Ring_920_x1", SNO_NONE },
};
// The two script handles start resolved: the original .data holds 446352 / 446764 in s_tGenericSetDungeonSnos.3.2 / .4.2.
static ResolveableSNOName s_tGenericSetDungeonSnos[5] = {
    { POWER_SNO, "p4_SetDung_Portal_Checks", SNO_NONE },
    { POWER_SNO, "p4_SetDung_Generics_Enmy", SNO_NONE },
    { POWER_SNO, "p4_SetDung_Generics_Ply", SNO_NONE },
    { SCRIPT_SNO, "p4_SetDung_Clue_Pedestal", 446352 },
    { SCRIPT_SNO, "p4_SetDung_Portal_AssignGroup", 446764 },
};

static ResolveableSNOName s_tGenericLevelAreas[2] = {
    { LEVELAREA_SNO, "X1_LR_DungeonFinder", SNO_NONE },
    { LEVELAREA_SNO, "X1_LR_DungeonFinder_GreaterRift", SNO_NONE },
};

static ResolveableSNOName s_tWhatsNewUI[1] = {
    { UI_SNO, "ConsoleWhatsNewPatches", SNO_NONE },
};


static CrafterType sGetCrafterTypeFromAnimSet(
    const SNO snoAnimSet)
{
    for (int32 i = 0; i < sizeof(s_tCrafterAnimSet) / sizeof(s_tCrafterAnimSet[0]); ++i)
    {
        if (snoAnimSet == s_tCrafterAnimSet[i].snoHandle)
        {
            return s_tCrafterAnimSet[i].eType;
        }
    }
    return -1;
}



static CrafterType sGetCrafterTypeFromActor(
    const SNO snoActor)
{
    for (int32 i = 0; i < sizeof(s_tCrafterActor) / sizeof(s_tCrafterActor[0]); ++i)
    {
        if (snoActor == s_tCrafterActor[i].snoHandle)
        {
            return s_tCrafterActor[i].eType;
        }
    }
    return -1;
}



static inline HirelingClass sGetHirelingClassFromActor(
    const SNO snoActor,
    BOOL * pfHired)
{
    for (int32 i = 0; i < 6; ++i)
    {
        if (snoActor == s_tActorToHirelingClass[i].snoHandle)
        {
            if (pfHired)
            {
                *pfHired = s_tActorToHirelingClass[i].fHired;
            }
            return s_tActorToHirelingClass[i].eClass;
        }
    }
    return 0;
}







static inline BOOL sIsHeavyPower(
    SNO snoPower)
{
    for (int32 i = 0; i < 2; ++i)
    {
        if (snoPower == s_tHeavyPowers[i].snoHandle)
        {
            return TRUE;
        }
    }
    return FALSE;
}



static inline BOOL sIsGenericSetDungeonSno(
    SNO sno)
{
    for (int32 i = 0; i < 5; ++i)
    {
        if (sno == s_tGenericSetDungeonSnos[i].snoHandle)
        {
            return TRUE;
        }
    }
    return FALSE;
}



static inline BOOL sIsGenericLevelArea(
    SNO sno)
{
    for (int32 i = 0; i < 2; ++i)
    {
        if (sno == s_tGenericLevelAreas[i].snoHandle)
        {
            return TRUE;
        }
    }
    return FALSE;
}



static inline BOOL sIsWhatsNewUI(
    SNO sno)
{
    for (int32 i = 0; i < 1; ++i)
    {
        if (sno == s_tWhatsNewUI[i].snoHandle)
        {
            return TRUE;
        }
    }
    return FALSE;
}

































static BOOL sClientActorFilter(const CompressedSNOName & snoRoot, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)




{


    if (snoChild.snoGroup == ANIMSET_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }
























    if (snoParent.snoGroup == POWER_SNO && sIsHeavyPower(snoParent.snoHandle))
    {
        fTraverseChildren = (snoChild.snoGroup == ACTOR_SNO);
        return TRUE;
    }


    if (sIsGenericSetDungeonSno(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (snoParent.snoGroup == ACTOR_SNO && (snoChild.snoGroup == MONSTER_SNO || snoChild.snoGroup == SCRIPT_SNO))
    {
        if (sGetHirelingClassFromActor(snoParent.snoHandle, NULL) != 0)
        {
            fTraverseChildren = FALSE;
            return FALSE;
        }
    }

    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_ACTOR, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
    return FilterShouldUse(FILTERSNOTYPE_ACTOR, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sClientServerItemActorFilter(
    const CompressedSNOName & snoRoot,
    const CompressedSNOName & snoParent,
    const CompressedSNOName & snoChild,
    BOOL & fTraverseChildren)
{


    if (snoChild.snoGroup == ANIMSET_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (snoParent.snoGroup == ACTOR_SNO && snoChild.snoGroup == ACTOR_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (snoParent.snoGroup == ACTOR_SNO && (snoChild.snoGroup == MONSTER_SNO || snoChild.snoGroup == SCRIPT_SNO))
    {
        if (sGetHirelingClassFromActor(snoParent.snoHandle, NULL) != 0)
        {
            fTraverseChildren = FALSE;
            return FALSE;
        }
    }

    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_ACTOR, FILTERTYPE_BOTH, snoParent.snoGroup, snoChild.snoGroup);
    return FilterShouldUse(FILTERSNOTYPE_ACTOR, FILTERTYPE_BOTH, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sClientServerActorFilter(
    const CompressedSNOName & snoRoot,
    const CompressedSNOName & snoParent,
    const CompressedSNOName & snoChild,
    BOOL & fTraverseChildren)
{


    if (snoChild.snoGroup == ANIMSET_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (snoParent.snoGroup == POWER_SNO && sIsHeavyPower(snoParent.snoHandle))
    {
        fTraverseChildren = snoChild.snoGroup == ACTOR_SNO;
        return TRUE;
    }


    if (sIsGenericSetDungeonSno(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (snoParent.snoGroup == ACTOR_SNO && (snoChild.snoGroup == MONSTER_SNO || snoChild.snoGroup == SCRIPT_SNO))
    {
        if (sGetHirelingClassFromActor(snoParent.snoHandle, NULL) != 0)
        {
            fTraverseChildren = FALSE;
            return FALSE;
        }
    }

    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_ACTOR, FILTERTYPE_BOTH, snoParent.snoGroup, snoChild.snoGroup);
    return FilterShouldUse(FILTERSNOTYPE_ACTOR, FILTERTYPE_BOTH, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sClientServerHirelingActorFilter(
    const CompressedSNOName & snoRoot,
    const CompressedSNOName & snoParent,
    const CompressedSNOName & snoChild,
    BOOL & fTraverseChildren)
{


    if (snoChild.snoGroup == ANIMSET_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (snoParent.snoGroup == ACTOR_SNO && (snoChild.snoGroup == MONSTER_SNO || snoChild.snoGroup == SCRIPT_SNO))
    {
        if (sGetHirelingClassFromActor(snoParent.snoHandle, NULL) != 0)
        {
            fTraverseChildren = FALSE;
            return FALSE;
        }
    }

    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_ACTOR, FILTERTYPE_BOTH, snoParent.snoGroup, snoChild.snoGroup);
    return FilterShouldUse(FILTERSNOTYPE_ACTOR, FILTERTYPE_BOTH, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sClientActorFilterWait(
    const CompressedSNOName & snoRoot,
    const CompressedSNOName & snoParent,
    const CompressedSNOName & snoChild,
    BOOL & fTraverseChildren)
{
    // line table: ACTOR test first (2306, returns FALSE), ANIMSET second (2314, returns TRUE)

    if (snoChild.snoGroup == ACTOR_SNO)
    {
        fTraverseChildren = FALSE;
        return FALSE;
    }

    if (snoChild.snoGroup == ANIMSET_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }

    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_ACTOR, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);

    if (snoChild.snoGroup == TEXTURE_SNO)
    {
        return FALSE;
    }

    return FilterShouldUse(FILTERSNOTYPE_ACTOR, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sClientServerSkillFilter(
    const CompressedSNOName & snoRoot,
    const CompressedSNOName & snoParent,
    const CompressedSNOName & snoChild,
    BOOL & fTraverseChildren)
{
    BOOL fUse = sClientServerActorFilter(snoRoot, snoParent, snoChild, fTraverseChildren);
    if (!fTraverseChildren)
    {
        fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_POWER, FILTERTYPE_BOTH, snoParent.snoGroup, snoChild.snoGroup);
    }











    if (snoParent.snoGroup == POWER_SNO && sIsHeavyPower(snoParent.snoHandle))
    {
        fTraverseChildren = snoChild.snoGroup == ACTOR_SNO;
        return TRUE;
    }


    if (sIsGenericSetDungeonSno(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }

    return fUse || FilterShouldUse(FILTERSNOTYPE_POWER, FILTERTYPE_BOTH, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sClientUIFilter(const CompressedSNOName &, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)
{
    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_UI, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
    return FilterShouldUse(FILTERSNOTYPE_UI, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
}






static BOOL sClientUIWithTexturesFilter(
    const CompressedSNOName & snoRoot,
    const CompressedSNOName & snoParent,
    const CompressedSNOName & snoChild,
    BOOL & fTraverseChildren)
{
    if (snoParent.snoGroup == UI_SNO && snoChild.snoGroup == TEXTURE_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }

    if (sIsWhatsNewUI(snoRoot.snoHandle) && snoParent.snoGroup == ACTOR_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }

    BOOL fUse = sClientActorFilter(snoRoot, snoParent, snoChild, fTraverseChildren);
    if (!fTraverseChildren)
    {
        fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_UI, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
    }

    return fUse || FilterShouldUse(FILTERSNOTYPE_UI, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sClientSceneFilter(const CompressedSNOName & snoRoot, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)




{
    if (snoChild.snoGroup == ANIMSET_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (snoParent.snoGroup == POWER_SNO && sIsHeavyPower(snoParent.snoHandle))
    {
        fTraverseChildren = (snoChild.snoGroup == ACTOR_SNO);
        return TRUE;
    }


    if (sIsGenericSetDungeonSno(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (sIsGenericLevelArea(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }

    BOOL fUse = sClientActorFilter(snoRoot, snoParent, snoChild, fTraverseChildren);
    if (!fTraverseChildren)
    {
        fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_SCENE, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
    }

    return fUse || FilterShouldUse(FILTERSNOTYPE_SCENE, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sClientSceneFilterWait(const CompressedSNOName & snoRoot, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)
{
    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_SCENE, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
    if (snoChild.snoGroup == ACTOR_SNO)
    {
        fTraverseChildren = FALSE;
    }
    if (snoChild.snoGroup == SCRIPT_SNO)
    {
        fTraverseChildren = FALSE;
    }
    if (snoChild.snoGroup == TEXTURE_SNO)
    {
        return FALSE;
    }
    return FilterShouldUse(FILTERSNOTYPE_SCENE, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
}












static BOOL sClientWeatherFilter(const CompressedSNOName & snoRoot, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)
{
    fTraverseChildren = FALSE;
    if (snoParent.snoGroup == WEATHER_SNO && snoChild.snoGroup == TEXTURE_SNO)
    {
        return TRUE;
    }
    return FALSE;
}









static BOOL sClientLevelAreaFilter(const CompressedSNOName & snoRoot, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)




{
    if (snoChild.snoGroup == ANIMSET_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (snoParent.snoGroup == POWER_SNO && sIsHeavyPower(snoParent.snoHandle))
    {
        fTraverseChildren = (snoChild.snoGroup == ACTOR_SNO);
        return TRUE;
    }


    if (sIsGenericSetDungeonSno(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (sIsGenericLevelArea(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }

    BOOL fUse = sClientActorFilter(snoRoot, snoParent, snoChild, fTraverseChildren);
    if (!fTraverseChildren)
    {
        fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_LEVELAREA, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
    }


    if (snoChild.snoGroup == APPEARANCE_SNO || snoChild.snoGroup == TEXTURE_SNO)
    {
        return FALSE;
    }

    return fUse || FilterShouldUse(FILTERSNOTYPE_LEVELAREA, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sClientLevelAreaPrefetchFilter(const CompressedSNOName & snoRoot, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)




{

    if (snoParent.snoGroup == POWER_SNO && sIsHeavyPower(snoParent.snoHandle))
    {
        fTraverseChildren = (snoChild.snoGroup == ACTOR_SNO);
        return TRUE;
    }


    if (sIsGenericSetDungeonSno(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (sIsGenericLevelArea(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    sClientActorFilter(snoRoot, snoParent, snoChild, fTraverseChildren);
    if (!fTraverseChildren)
    {
        fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_LEVELAREA, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
    }

    return snoChild.snoGroup == APPEARANCE_SNO || snoChild.snoGroup == TEXTURE_SNO;
}


static BOOL sClientWorldFilter(const CompressedSNOName & snoRoot, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)
{
    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_WORLD, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
    return FilterShouldUse(FILTERSNOTYPE_WORLD, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
}
























static BOOL sClientFullWorldFilter(
    const CompressedSNOName & snoRoot,
    const CompressedSNOName & snoParent,
    const CompressedSNOName & snoChild,
    BOOL & fTraverseChildren)
{
    if (snoParent.snoGroup == APPEARANCE_SNO && snoChild.snoGroup == TEXTURE_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }
    BOOL fUse = sClientActorFilter(snoRoot, snoParent, snoChild, fTraverseChildren);
    if (!fTraverseChildren)
    {
        fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_SCENE, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
        if (!fTraverseChildren)
        {
            fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_WORLDLOADING, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
        }
    }

    return fUse || FilterShouldUse(FILTERSNOTYPE_SCENE, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup) || FilterShouldUse(FILTERSNOTYPE_WORLD, FILTERTYPE_CLIENT, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sServerSkillKitFilter(const CompressedSNOName & snoRoot, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)
{
    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_POWER, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
    return FilterShouldUse(FILTERSNOTYPE_POWER, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
}









static BOOL sServerActorFilter(const CompressedSNOName &, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)




{
    if (snoChild.snoGroup == ANIMSET_SNO)
    {
        fTraverseChildren = TRUE;
        return TRUE;
    }


    if (snoParent.snoGroup == ANIMSET_SNO && snoChild.snoGroup == ANIMATION_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }



    if (snoParent.snoGroup == POWER_SNO && sIsHeavyPower(snoParent.snoHandle))
    {
        fTraverseChildren = (snoChild.snoGroup == ACTOR_SNO);
        return TRUE;
    }


    if (sIsGenericSetDungeonSno(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }

    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_ACTOR, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
    return FilterShouldUse(FILTERSNOTYPE_ACTOR, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sServerActorAvengerFilter(const CompressedSNOName &, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)




{
    if (snoChild.snoGroup == ANIMSET_SNO)
    {
        fTraverseChildren = TRUE;
        return TRUE;
    }


    if (snoParent.snoGroup == ANIMSET_SNO && snoChild.snoGroup == ANIMATION_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }



    if (snoParent.snoGroup == AISTATE_SNO && snoChild.snoGroup == POWER_SNO)
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_ACTOR, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
    return FilterShouldUse(FILTERSNOTYPE_ACTOR, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sServerSceneFilter(const CompressedSNOName & snoRoot, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)




{

    if (snoParent.snoGroup == POWER_SNO && sIsHeavyPower(snoParent.snoHandle))
    {
        fTraverseChildren = (snoChild.snoGroup == ACTOR_SNO);
        return TRUE;
    }


    if (sIsGenericSetDungeonSno(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (sIsGenericLevelArea(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }

    BOOL fUse = sServerActorFilter(snoRoot, snoParent, snoChild, fTraverseChildren);
    if (!fTraverseChildren)
    {
        fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_SCENE, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
    }

    return fUse || FilterShouldUse(FILTERSNOTYPE_SCENE, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sServerLevelAreaFilter(const CompressedSNOName & snoRoot, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)




{

    if (snoParent.snoGroup == POWER_SNO && sIsHeavyPower(snoParent.snoHandle))
    {
        fTraverseChildren = (snoChild.snoGroup == ACTOR_SNO);
        return TRUE;
    }


    if (sIsGenericSetDungeonSno(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }


    if (sIsGenericLevelArea(snoParent.snoHandle))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }

    BOOL fUse = sServerActorFilter(snoRoot, snoParent, snoChild, fTraverseChildren);
    if (!fTraverseChildren)
    {
        fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_LEVELAREA, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
    }

    return fUse || FilterShouldUse(FILTERSNOTYPE_LEVELAREA, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sServerWorldLoadingFilter(const CompressedSNOName & snoRoot, const CompressedSNOName & snoParent, const CompressedSNOName & snoChild, BOOL & fTraverseChildren)
{
    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_WORLDLOADING, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
    return FilterShouldUse(FILTERSNOTYPE_WORLDLOADING, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
}








static BOOL sServerDungeonFinderWorldLoadingFilter(
    const CompressedSNOName & snoRoot,
    const CompressedSNOName & snoParent,
    const CompressedSNOName & snoChild,
    BOOL & fTraverseChildren)
{


    fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_WORLDLOADING, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);

    if (snoChild.snoGroup == ANIMSET_SNO)
    {
        fTraverseChildren = TRUE;
        return FALSE;
    }

    if (snoChild.snoGroup == PHYSMESH_SNO || (snoParent.snoGroup == ANIMSET_SNO && snoChild.snoGroup == ANIMATION_SNO))
    {
        fTraverseChildren = FALSE;
        return TRUE;
    }

    return FilterShouldUse(FILTERSNOTYPE_WORLDLOADING, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
}


static BOOL sServerWorldFilter(
    const CompressedSNOName & snoRoot,
    const CompressedSNOName & snoParent,
    const CompressedSNOName & snoChild,
    BOOL & fTraverseChildren)
{
    BOOL fUse = sServerActorFilter(snoRoot, snoParent, snoChild, fTraverseChildren);
    if (!fTraverseChildren)
    {
        fTraverseChildren = FilterShouldExplore(FILTERSNOTYPE_WORLD, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
    }

    return fUse || FilterShouldUse(FILTERSNOTYPE_WORLD, FILTERTYPE_SERVER, snoParent.snoGroup, snoChild.snoGroup);
}
// Invented name: the thirteen loops below share lines 2863-2877, one per table.
#define SNOPRELOAD_RESOLVE_TABLE(table) \
    for (int32 i = 0; i < (int32)(sizeof(table) / sizeof(table[0])); ++i) \
    { \
        if (table[i].snoHandle == SNO_NONE) \
        { \
            table[i].snoHandle = SNOFileLookupGUID(table[i].snoGroup, table[i].szName); \
        } \
    }


void RefreshSNOResolve()
{
    SNOPRELOAD_RESOLVE_TABLE(s_tCrafterAnimSet);
    SNOPRELOAD_RESOLVE_TABLE(s_tCrafterActor);
    SNOPRELOAD_RESOLVE_TABLE(s_tActorToHirelingClass);
    SNOPRELOAD_RESOLVE_TABLE(s_tHirelingClassToHirelingActor);
    SNOPRELOAD_RESOLVE_TABLE(s_tWeaponClassAnimSet);
    SNOPRELOAD_RESOLVE_TABLE(s_tHeroAnimSet);
    SNOPRELOAD_RESOLVE_TABLE(s_tHeroActor);
    SNOPRELOAD_RESOLVE_TABLE(s_tHeroAppearance);
    SNOPRELOAD_RESOLVE_TABLE(s_tComplexAnimSet);
    SNOPRELOAD_RESOLVE_TABLE(s_tHeavyPowers);
    SNOPRELOAD_RESOLVE_TABLE(s_tGenericSetDungeonSnos);
    SNOPRELOAD_RESOLVE_TABLE(s_tGenericLevelAreas);
    SNOPRELOAD_RESOLVE_TABLE(s_tWhatsNewUI);
}


void InitializeForGame()
{
    s_tPreloadImpl.InitializeForGame();
}




BOOL IsInitializedForGame()
{
    return s_tPreloadImpl.IsInitializedForGame();
}




void DisposeForGame()
{
    s_tPreloadImpl.DisposeForGame();
}




void InitializeForApp()
{
    RefreshSNOResolve();
    s_tPreloadImpl.InitializeForApp();
}





BOOL IsInitializedForApp()
{
    return s_tPreloadImpl.IsInitializedForApp();
}












void DisposeForApp()
{
    s_tPreloadImpl.DisposeForApp();
}













void AddClientUI(const SNO snoUI)
{
    s_tPreloadImpl.AddClientUI(snoUI);
}





BOOL AreAllClientUIChildrenLoaded(const SNO snoUI)
{
    return s_tPreloadImpl.AreAllClientUIChildrenLoaded(snoUI);
}





BOOL AddClientUIChildren(const SNO snoUI)
{
    return s_tPreloadImpl.AddClientUIChildren(snoUI);
}





void RemoveClientUIChildren(const SNO snoUI)
{
    s_tPreloadImpl.RemoveClientUIChildren(snoUI);
}





void WaitForClientUIChildren(const SNO snoUI)
{
    s_tPreloadImpl.WaitForClientUIChildren(snoUI);
}





void WaitForAllUIRoots()
{
    s_tPreloadImpl.WaitForAllUIRoots();
}




SNO GetClientWorld()
{
    return s_tPreloadImpl.GetClientWorld();
}





void SetClientWorld(const SNO snoWorld)
{
    s_tPreloadImpl.SetClientWorld(snoWorld);
}





void WaitForFullClientWorld(const SNO snoWorld)
{
    s_tPreloadImpl.WaitForFullClientWorld(snoWorld);
}





void AddFullClientWorld(const SNO snoWorld, BOOL fWaitForSNOs)
{
    s_tPreloadImpl.AddFullClientWorld(snoWorld, fWaitForSNOs);
}






void AddNeutralClientActor(const SNO snoActor, BOOL fWaitForSNOs)
{
    s_tPreloadImpl.AddNeutralClientActor(snoActor, fWaitForSNOs);
}






void RemoveClientSceneBySNO(const SNO snoScene)
{
    s_tPreloadImpl.RemoveClientSceneBySNO(snoScene);
}





void AddClientSceneBySNO(const SNO snoScene)
{
    s_tPreloadImpl.AddClientSceneBySNO(snoScene);
}





void RemoveClientWeatherBySNO(const SNO snoWeather)
{
    s_tPreloadImpl.RemoveClientWeatherBySNO(snoWeather);
}





void AddClientWeatherBySNO(const SNO snoWeather)
{
    s_tPreloadImpl.AddClientWeatherBySNO(snoWeather);
}





void RemoveClientScene(const SSceneID idSScene)
{
    s_tPreloadImpl.RemoveClientScene(idSScene);
}





void WaitForClientScene(const SNO snoScene)
{
    s_tPreloadImpl.WaitForClientScene(snoScene);
}





void AddClientScene(const SSceneID idSScene, const SNO snoScene, const SNO * ptSNOLevelAreas)
{
    s_tPreloadImpl.AddClientScene(idSScene, snoScene, ptSNOLevelAreas);
}







BOOL PrefetchClientWorld(const SNO snoWorld, SNOFilesAsyncPriority ePriority)
{
    return s_tPreloadImpl.PrefetchClientWorld(snoWorld, ePriority);
}















BOOL PrefetchClientScene(const SNO snoScene, SNOFilesAsyncPriority ePriority)
{
    return s_tPreloadImpl.PrefetchClientScene(snoScene, ePriority);
}






BOOL PrefetchClientLevelArea(const SNO snoLevelArea, SNOFilesAsyncPriority ePriority)
{
    return s_tPreloadImpl.PrefetchClientLevelArea(snoLevelArea, ePriority);
}






void AddClientTextures(const SNONameArray & snoTextures)
{
    s_tPreloadImpl.AddClientTextures(snoTextures);
}





void RemoveClientTextures(const SNONameArray & snoTextures)
{
    s_tPreloadImpl.RemoveClientTextures(snoTextures);
}





BOOL PrefetchClientWeather(const SNO snoWeather, SNOFilesAsyncPriority ePriority)
{
    return s_tPreloadImpl.PrefetchClientWeather(snoWeather, ePriority);
}






BOOL PrefetchClientActor(const SNO snoActor, const GBID * pgbidMonsterAffixes, SNOFilesAsyncPriority ePriority)
{
    return s_tPreloadImpl.PrefetchClientActor(snoActor, pgbidMonsterAffixes, ePriority);
}







void WaitForClientActor(const ACDNetworkName ann)
{
    s_tPreloadImpl.WaitForClientActor(ann);
}





BOOL IsClientActorLoaded(const ACDNetworkName ann)
{
    return s_tPreloadImpl.IsClientActorLoaded(ann);
}





BOOL IsClientActorPending(const ACDNetworkName ann)
{
    return s_tPreloadImpl.IsClientActorPending(ann);
}





BOOL IsClientActorRequested(const ACDNetworkName ann)
{
    return s_tPreloadImpl.IsClientActorRequested(ann);
}





void RemoveClientActor(const ACDNetworkName ann)
{
    s_tPreloadImpl.RemoveClientActor(ann);
}





void AddClientActorBySNO(const SNO snoActor)
{
    s_tPreloadImpl.AddClientActorBySNO(snoActor);
}





void RemoveClientActorBySNO(const SNO snoActor)
{
    s_tPreloadImpl.RemoveClientActorBySNO(snoActor);
}





void AddClientActor(const ACDNetworkName ann, const SNO snoActor, const WeaponClass eWeaponClass, const GBID * pgbidMonsterAffixes)
{
    s_tPreloadImpl.AddClientActor(ann, snoActor, eWeaponClass, pgbidMonsterAffixes);
}









BOOL ApplyHotfix(const char * szHotfixName)
{
    s_tPreloadImpl.ApplyHotfix(szHotfixName);
    return TRUE;
}




void SetAct(Act eAct)
{
    s_tPreloadImpl.SetAct(eAct);
}





void AddServerActorBySNO(const SNO snoActor)
{
    s_tPreloadImpl.AddServerActorBySNO(snoActor);
}





void RemoveServerActorBySNO(const SNO snoActor)
{
    s_tPreloadImpl.RemoveServerActorBySNO(snoActor);
}





void PrefetchServerActorAvenger(const SNO snoActor, const GBID * pgbidMonsterAffixes)
{
    s_tPreloadImpl.PrefetchServerActorAvenger(snoActor, pgbidMonsterAffixes);
}






void RemoveServerWorld(const SNO snoWorld)
{
    s_tPreloadImpl.RemoveServerWorld(snoWorld);
}





void AddServerWorld(const SNO snoWorld)
{
    s_tPreloadImpl.AddServerWorld(snoWorld);
}





void RemoveServerWorldLoading(const SNO snoWorld, BOOL fDungeonFinder)
{
    s_tPreloadImpl.RemoveServerWorldLoading(snoWorld, fDungeonFinder);
}






void AddServerWorldLoading(const SNO snoWorld, BOOL fDungeonFinder)
{
    s_tPreloadImpl.AddServerWorldLoading(snoWorld, fDungeonFinder);
}






void PrefetchWorldLoading(const SNO snoWorld, SNOFilesAsyncPriority ePriority)
{
    s_tPreloadImpl.PrefetchWorldLoading(snoWorld, ePriority);
}






BOOL IsServerWorldLoadingDone(const SNO snoWorld, BOOL fDungeonFinder)
{
    return s_tPreloadImpl.IsServerWorldLoadingDone(snoWorld, fDungeonFinder);
}






void PrefetchServerScene(const SNO snoScene, const SNO * ptSNOLevelAreas)
{
    s_tPreloadImpl.PrefetchServerScene(snoScene, ptSNOLevelAreas);
}






void AddServerSceneBySNO(const SNO snoScene)
{
    s_tPreloadImpl.AddServerSceneBySNO(snoScene);
}





void RemoveServerSceneBySNO(const SNO snoScene)
{
    s_tPreloadImpl.RemoveServerSceneBySNO(snoScene);
}





void RemoveServerLevelArea(const SNO snoLevelArea)
{
    s_tPreloadImpl.RemoveServerLevelArea(snoLevelArea);
}





void AddServerLevelArea(const SNO snoLevelArea)
{
    s_tPreloadImpl.AddServerLevelArea(snoLevelArea);
}





void RemoveServerSkillKit(const SNO snoSkillKit)
{
    s_tPreloadImpl.RemoveServerSkillKit(snoSkillKit);
}





void AddServerSkillKit(const SNO snoSkillKit)
{
    s_tPreloadImpl.AddServerSkillKit(snoSkillKit);
}
















void UpdateSkills()
{
    s_tPreloadImpl.UpdateSkills();
}




void UpdateEquipped()
{
    s_tPreloadImpl.UpdateEquipped();
}




void UpdateCrafters()
{
    s_tPreloadImpl.UpdateCrafters();
}




void UpdateHireling()
{
    s_tPreloadImpl.UpdateHireling();
}













void Impl::RemoveAllServerSNOs(
    BOOL fFromActChange)
{
    SNOManager::SNOContainer tContainer;

    m_serverPerActSceneSNOs.RemoveAll();
    SNOManager::RequireSNOs(SNOManager::SC_SERVER_ACT_SCENES, tContainer, FALSE, SNOFilesAsyncPriority_Medium);

    m_serverPerActLevelAreaSNOs.RemoveAll();
    SNOManager::RequireSNOs(SNOManager::SC_SERVER_ACT_LEVELAREAS, tContainer, FALSE, SNOFilesAsyncPriority_Medium);

    m_serverPerActWorldSNOs.RemoveAll();
    SNOManager::RequireSNOs(SNOManager::SC_SERVER_ACT_WORLDS, tContainer, FALSE, SNOFilesAsyncPriority_Medium);

    m_serverPerActWorldLoadingSNOs.RemoveAll();
    SNOManager::RequireSNOs(SNOManager::SC_SERVER_ACT_WORLDSLOADING, tContainer, FALSE, SNOFilesAsyncPriority_Medium);

    m_serverPerActRequiredRootNodes.RemoveAll();

    if (!fFromActChange)
    {
        m_serverActorSNOs.RemoveAll();
        SNOManager::RequireSNOs(SNOManager::SC_SERVERACTORS, tContainer, FALSE, SNOFilesAsyncPriority_Medium);

        m_serverRequiredRootNodes.RemoveAll();
    }
}



static void sClearSNOs(
    const SNOManager::SNOCategory category,
    SNOManager::SNOContainer & tSNOs)
{
    tSNOs.RemoveAll();

    SNOManager::SNOContainer tContainer;
    SNOManager::RequireSNOs(category, tContainer, FALSE, SNOFilesAsyncPriority_Medium);
}


void Impl::RemoveAllClientSNOs(
    BOOL fFromActChange)
{
    sClearSNOs(SNOManager::SC_CLIENT_ACT_WORLDS, m_clientPerActWorldSNOs);
    sClearSNOs(SNOManager::SC_CLIENT_ACT_SCENES, m_clientPerActSceneSNOs);
    sClearSNOs(SNOManager::SC_CLIENT_ACT_ACTORS, m_clientPerActActorSNOs);
    sClearSNOs(SNOManager::SC_CLIENT_ACT_TEXTURES, m_clientPerActTextureSNOs);
    m_clientPerActRequiredRootNodes.RemoveAll();

    m_clientSSceneIDToSNO.RemoveAll();
    m_clientWorld = SNO_NONE;

    if (!fFromActChange)
    {
        sClearSNOs(SNOManager::SC_CLIENTUIS, m_clientUISNOs);
        sClearSNOs(SNOManager::SC_SKILLS, m_clientSkillSNOs);
        sClearSNOs(SNOManager::SC_CRAFTERS, m_clientCrafterSNOs);
        sClearSNOs(SNOManager::SC_CLIENTACTORS, m_clientActorSNOs);
        sClearSNOs(SNOManager::SC_HIRELING, m_clientHirelingSNOs);
        m_clientRequiredRootNodes.RemoveAll();
        m_clientHirelingRequiredRootNodes.RemoveAll();
        m_clientUIChildrenRequiredRootNodes.RemoveAll();
        m_clientSkills.RemoveAll();
        m_clientItems.RemoveAll();
        m_clientCrafters.RemoveAll();
        m_currentHireling = HC_NONE;
        m_clientANNToSNO.RemoveAll();

        for (int32 i = 0; i < sizeof(m_clientCrafterActorSNO) / sizeof(m_clientCrafterActorSNO[0]); ++i)
        {
            m_clientCrafterActorSNO[i] = SNO_NONE;
        }
    }
}


Impl::Impl()
{

    m_serverPerActSceneSNOs.SetHashTableSize_Tracked(2048);
    m_serverPerActLevelAreaSNOs.SetHashTableSize_Tracked(32);
    m_serverPerActWorldSNOs.SetHashTableSize_Tracked(64);
    m_serverPerActWorldLoadingSNOs.SetHashTableSize_Tracked(512);
    m_serverActorSNOs.SetHashTableSize_Tracked(512);
    m_serverRequiredRootNodes.SetHashTableSize_Tracked(128);
    m_serverPerActRequiredRootNodes.SetHashTableSize_Tracked(128);

    m_eCurrentAct = ACT_INVALID;


    m_clientPerActWorldSNOs.SetHashTableSize_Tracked(32);
    m_clientPerActSceneSNOs.SetHashTableSize_Tracked(4096);
    m_clientPerActActorSNOs.SetHashTableSize_Tracked(4096);
    m_clientPerActTextureSNOs.SetHashTableSize_Tracked(1024);
    m_clientUISNOs.SetHashTableSize_Tracked(64);
    m_clientSkillSNOs.SetHashTableSize_Tracked(512);
    m_clientCrafterSNOs.SetHashTableSize_Tracked(128);
    m_clientActorSNOs.SetHashTableSize_Tracked(128);
    m_clientHirelingSNOs.SetHashTableSize_Tracked(128);
    m_clientANNToSNO.SetHashTableSize_Tracked(128);
    m_clientSSceneIDToSNO.SetHashTableSize_Tracked(64);
    m_clientPerActRequiredRootNodes.SetHashTableSize_Tracked(128);
    m_clientRequiredRootNodes.SetHashTableSize_Tracked(128);
    m_clientHirelingRequiredRootNodes.SetHashTableSize_Tracked(16);
    m_clientUIChildrenRequiredRootNodes.SetHashTableSize_Tracked(128);

    m_clientWorld = SNO_NONE;
    m_fInitializedForApp = m_fInitializedForGame = FALSE;
    m_fPrefetchClientSceneEnabled = TRUE;

    for (int32 i = 0; i < sizeof(m_clientCrafterActorSNO) / sizeof(m_clientCrafterActorSNO[0]); ++i)
    {
        m_clientCrafterActorSNO[i] = SNO_NONE;
    }

    m_currentHireling = 0;
}





static BOOL sReadFile(const char * szFileName, uint8 *& pData, uint32 & nDataSize)
{





    FileIO::File tFile((XLocale)1);  // XLOCALE_GLOBAL (X/Sigma/UnicodeText.h)

    if (tFile.Open(szFileName))
    {
        nDataSize = (uint32)tFile.GetFileSize();
        pData = (uint8 *)SigmaMemoryNew(nDataSize, 4, NULL, FALSE);

        if (!tFile.ReadFile(pData, nDataSize, NULL))
        {
            SigmaMemoryFree(pData, NULL);
            pData = NULL;
            return FALSE;
        }
    }
    return TRUE;
}


static void sAddSNOsFromFile(const char * szFile, SNONameArray & tContainer)
{
    uint8 * pData = NULL;
    uint32 nDataSize = 0;
    sReadFile(szFile, pData, nDataSize);

    if (pData != NULL && nDataSize != 0)
    {




        if (*(uint32 *)pData == 0xAABB0004)
        {
            SNOName * pSNONames = (SNOName *)(pData + sizeof(uint32));
            int32 nItemCount = (nDataSize - sizeof(uint32)) / sizeof(SNOName);
            for (int32 i = 0; i < nItemCount; ++i)
            {
                tContainer.push_back(pSNONames[i]);
            }
        }
        SigmaMemoryFree(pData, NULL);
    }
}
































































































static void sGenerateAlwaysLoadedForApp(
    SNONameArray & tContainer)
{

    sAddSNOsFromFile("Misc\\Common.app.dat", tContainer);


    sAddSNOsFromFile("Misc\\CoreCommon.app.dat", tContainer);
    sAddSNOsFromFile("Misc\\ClientCommon.app.dat", tContainer);
    sAddSNOsFromFile("Misc\\ServerCommon.app.dat", tContainer);
}























































































void Impl::InitializeForApp()
{




    m_fInitializedForApp = TRUE;


    for (int32 nSNOType = 0; nSNOType < FILTERSNOTYPE_COUNT; ++nSNOType)
    {
        for (int32 nType = 0; nType < FILTERTYPE_COUNT; ++nType)
        {
            ParentAndChildBitField & tExploreBitFilter = saShouldExploreBitFilter[nSNOType][nType];
            ParentAndChild * ptExploreFilter = saShouldExploreFilter[nSNOType][nType];
            SigmaMemoryClear(tExploreBitFilter, sizeof(tExploreBitFilter));
            for (; ptExploreFilter->snoChild != SNO_NONE; ++ptExploreFilter)
            {
                uint32 dwBitIndex = ptExploreFilter->snoParent * MAX_SNO_GROUPS + ptExploreFilter->snoChild;
                uint32 dwIndex = dwBitIndex / 32;
                uint32 dwSubBit = dwBitIndex % 32;
                tExploreBitFilter[dwIndex] |= 1 << dwSubBit;
            }
        }



        for (int32 nType = 0; nType < FILTERTYPE_COUNT; ++nType)
        {
            ParentAndChildBitField & tUseBitFilter = saShouldUseBitFilter[nSNOType][nType];
            ParentAndChild * ptUseFilter = saShouldUseFilter[nSNOType][nType];
            SigmaMemoryClear(tUseBitFilter, sizeof(tUseBitFilter));
            for (; ptUseFilter->snoChild != SNO_NONE; ++ptUseFilter)
            {
                uint32 dwBitIndex = (ptUseFilter->snoParent == SNO_NONE ? 0 : ptUseFilter->snoParent * MAX_SNO_GROUPS) + ptUseFilter->snoChild;
                uint32 dwIndex = dwBitIndex / 32;
                uint32 dwSubBit = dwBitIndex % 32;
                tUseBitFilter[dwIndex] |= 1 << dwSubBit;
            }
        }
    }



    for (int32 i = 0; i < sizeof(m_clientCrafterActorSNO) / sizeof(m_clientCrafterActorSNO[0]); ++i)
    {
        m_clientCrafterActorSNO[i] = SNO_NONE;
    }

    SNONameArray tContainer;

    SNONameArray tHighContainer;
    SNONameArray tMediumContainer;


    {

        Blizzard::Time::Microsecond dwStartedUS = Blizzard::Time::Microseconds();





        sGenerateAlwaysLoadedForApp(tContainer);

        size_t nAppliedHotfixesCount = m_appliedHotfixes.size();
        for (size_t i = 0; i < nAppliedHotfixesCount; ++i)
        {
            sAddSNOsFromFile(CRefString().Format("Misc\\Hotfixes\\%s.app.dat", m_appliedHotfixes[i].str()), tContainer);
        }



        for (SNOName * it = tContainer.begin(); it != tContainer.end(); ++it)
        {
            if (it->snoGroup == FLAGSET_SNO || it->snoGroup == GAMEBALANCE_SNO || it->snoGroup == GLOBAL_SNO ||
                it->snoGroup == SOUNDBANK_SNO || it->snoGroup == UI_SNO || it->snoGroup == RECIPE_SNO)
            {




                tHighContainer.push_back(*it);
            }
            else
            {
                tMediumContainer.push_back(*it);
            }
        }


        Prefetch::BeginAdd();


        SNOFilesAsyncDisableSortedRequest();


        SNOManager::RequireSNOs(SNOManager::SC_APP, tHighContainer, FALSE, SNOFilesAsyncPriority_High);
        SNOManager::AddRequireSNOs(SNOManager::SC_APP, tMediumContainer, SNOFilesAsyncPriority_Medium);





        SNOFilesAsyncEnableSortedRequest();


        SNOFilesAsyncSortRequests();

        Prefetch::EndAdd();




        SNOManager::WaitForSNOs(tHighContainer, NULL, TRUE);



    }





    s_tClientPreloadAsync.fTerminateAsyncThread = FALSE;




    Blizzard::Thread::Create(PreloadAsyncThread, &s_tClientPreloadAsync, &s_tClientPreloadAsync.tPreloadAsyncThread, "ClientPreloadAsyncThread", 0, 0x4000);





    s_tServerPreloadAsync.fTerminateAsyncThread = FALSE;



    Blizzard::Thread::Create(PreloadAsyncThread, &s_tServerPreloadAsync, &s_tServerPreloadAsync.tPreloadAsyncThread, "ServerPreloadAsyncThread", 0, 0x4000);




    Blizzard::Thread::SetPriority(s_tClientPreloadAsync.tPreloadAsyncThread, Blizzard::Thread::PRIORITY_BELOW_NORMAL);
    Blizzard::Thread::SetPriority(s_tServerPreloadAsync.tPreloadAsyncThread, Blizzard::Thread::PRIORITY_BELOW_NORMAL);





    ConfigureClientPreloadAsyncThread(s_tClientPreloadAsync.tPreloadAsyncThread);
    ConfigureServerPreloadAsyncThread(s_tServerPreloadAsync.tPreloadAsyncThread);



}


inline BOOL Impl::IsInitializedForApp()
{
    return m_fInitializedForApp;
}


static void sGenerateAlwaysLoadedForGame(
    SNONameArray & tContainer)
{

    sAddSNOsFromFile("Misc\\Common.game.dat", tContainer);


    sAddSNOsFromFile("Misc\\CoreCommon.game.dat", tContainer);
    sAddSNOsFromFile("Misc\\ClientCommon.game.dat", tContainer);
    sAddSNOsFromFile("Misc\\ServerCommon.game.dat", tContainer);
}
















































































SNOPackType sActToSNOPackType(
    Act eAct)
{
    switch (eAct)
    {
    case ACT_1: return SNOPACKTYPE_CONSOLE_ACT1;
    case ACT_2: return SNOPACKTYPE_CONSOLE_ACT2;
    case ACT_3: return SNOPACKTYPE_CONSOLE_ACT3;
    case ACT_4: return SNOPACKTYPE_CONSOLE_ACT4;
    case ACT_5: return SNOPACKTYPE_CONSOLE_ACT5;
    default: return SNOPACKTYPE_INVALID;
    }
}








void Impl::InitializeForGame()
{
    m_fInitializedForGame = TRUE;

    SNONameArray tContainer;

    {

        Blizzard::Time::Microsecond dwStartedUS = Blizzard::Time::Microseconds();





        sGenerateAlwaysLoadedForGame(tContainer);


        size_t nAppliedHotfixesCount = m_appliedHotfixes.size();
        for (size_t i = 0; i < nAppliedHotfixesCount; ++i)
        {
            sAddSNOsFromFile(CRefString().Format("Misc\\Hotfixes\\%s.game.dat", m_appliedHotfixes[i].str()), tContainer);
        }


        Prefetch::BeginAdd();


        SNOFilesAsyncDisableSortedRequest();

        SNOManager::RequireSNOs(SNOManager::SC_GAME, tContainer, FALSE, SNOFilesAsyncPriority_Medium);


        SNOFilesAsyncEnableSortedRequest();


        SNOFilesAsyncSortRequests();

        Prefetch::EndAdd();



        SNOManager::WaitForSNOs(tContainer, NULL, TRUE);


    }



    if (m_eCurrentAct != ACT_INVALID)
    {
        tContainer.clear();




        if (m_eCurrentAct == ACT_OPEN_WORLD)
        {
            for (int32 i = SNOPACKTYPE_CONSOLE_ACT1; i <= SNOPACKTYPE_CONSOLE_ACT5; ++i)
            {
                SNOPackType eSNOPack = i;


                sAddSNOsFromFile(CRefString().Format("Misc\\%s.app.dat", SNOPackTypeToString(eSNOPack)), tContainer);
            }
        }

        else
        {
            SNOPackType eSNOPack = sActToSNOPackType(m_eCurrentAct);
            if (eSNOPack != SNOPACKTYPE_INVALID)
            {
                sAddSNOsFromFile(CRefString().Format("Misc\\%s.app.dat", SNOPackTypeToString(eSNOPack)), tContainer);
            }
        }







        SNOManager::RequireSNOs(SNOManager::SC_ACT, tContainer, FALSE, SNOFilesAsyncPriority_Medium);
        SNOManager::WaitForSNOs(tContainer, NULL, TRUE);


        tContainer.clear();




        if (m_eCurrentAct == ACT_OPEN_WORLD)
        {
            for (int32 i = SNOPACKTYPE_CONSOLE_ACT1; i <= SNOPACKTYPE_CONSOLE_ACT5; ++i)
            {
                SNOPackType eSNOPack = i;


                sAddSNOsFromFile(CRefString().Format("Misc\\%s.game.dat", SNOPackTypeToString(eSNOPack)), tContainer);
            }
        }

        else
        {
            SNOPackType eSNOPack = sActToSNOPackType(m_eCurrentAct);
            if (eSNOPack != SNOPACKTYPE_INVALID)
            {
                sAddSNOsFromFile(CRefString().Format("Misc\\%s.game.dat", SNOPackTypeToString(eSNOPack)), tContainer);
            }
        }







        SNOManager::RequireSNOs(SNOManager::SC_GAMEACT, tContainer, FALSE, SNOFilesAsyncPriority_Medium);
        SNOManager::WaitForSNOs(tContainer, NULL, TRUE);

    }

}


inline BOOL Impl::IsInitializedForGame()
{
    return m_fInitializedForGame;
}


void Impl::DisposeForGame()
{
    sCancelRequests(TRUE);
    sCancelRequests(FALSE);
    sWaitForRequests(TRUE, SNO_NONE, FALSE);
    sWaitForRequests(FALSE, SNO_NONE, FALSE);
    RemoveAllClientSNOs(FALSE);

    RemoveAllServerSNOs(FALSE);

    SNOManager::SNOContainer tContainer(16, NULL);
    SNOManager::RequireSNOs(SNOManager::SC_GAME, tContainer, FALSE, SNOFilesAsyncPriority_Medium);

    SNOManager::RequireSNOs(SNOManager::SC_GAMEACT, tContainer, FALSE, SNOFilesAsyncPriority_Medium);
    SNOManager::RequireSNOs(SNOManager::SC_ACT, tContainer, FALSE, SNOFilesAsyncPriority_Medium);


    m_fInitializedForGame = FALSE;
}



void Impl::DisposeForApp()
{
    m_fInitializedForApp = FALSE;
    m_appliedHotfixes.clear();

    s_tClientPreloadAsync.fTerminateAsyncThread = TRUE;
    s_tClientPreloadAsync.theresWorkToDoEvent.Set();
    Blizzard::Thread::WaitForever(s_tClientPreloadAsync.tPreloadAsyncThread);
    Blizzard::Thread::Close(s_tClientPreloadAsync.tPreloadAsyncThread);

    s_tServerPreloadAsync.fTerminateAsyncThread = TRUE;
    s_tServerPreloadAsync.theresWorkToDoEvent.Set();
    Blizzard::Thread::WaitForever(s_tServerPreloadAsync.tPreloadAsyncThread);
    Blizzard::Thread::Close(s_tServerPreloadAsync.tPreloadAsyncThread);
}









BOOL Impl::ApplyHotfix(const char * szHotfixName)
{
    m_appliedHotfixes.push_back(CRefString(szHotfixName));

    SNONameArray tContainer;

    if (m_fInitializedForApp)
    {





        sAddSNOsFromFile(CRefString().Format("Misc\\Hotfixes\\%s.app.dat", szHotfixName), tContainer);







        SNOManager::AddRequireSNOs(SNOManager::SC_APP, tContainer, SNOFilesAsyncPriority_Medium);
        SNOManager::WaitForSNOs(tContainer, NULL, TRUE);
    }

    tContainer.clear();

    if (m_fInitializedForGame)
    {





        sAddSNOsFromFile(CRefString().Format("Misc\\Hotfixes\\%s.game.dat", szHotfixName), tContainer);







        SNOManager::AddRequireSNOs(SNOManager::SC_GAME, tContainer, SNOFilesAsyncPriority_Medium);
        SNOManager::WaitForSNOs(tContainer, NULL, TRUE);
    }

    return TRUE;
}

void Impl::SetAct(
    Act eAct)
{
    Blizzard::Lock::ScopedMutex scopedLockC(tClientCrit);
    Blizzard::Lock::ScopedMutex scopedLockS(tServerCrit);

    if (m_fInitializedForGame)
    {
        SNONameArray tContainer;




        if (eAct == ACT_OPEN_WORLD)
        {
            for (int32 i = SNOPACKTYPE_CONSOLE_ACT1; i <= SNOPACKTYPE_CONSOLE_ACT5; ++i)
            {
                SNOPackType eSNOPack = i;


                sAddSNOsFromFile(CRefString().Format("Misc\\%s.app.dat", SNOPackTypeToString(eSNOPack)), tContainer);
            }
        }

        else
        {
            SNOPackType eSNOPack = sActToSNOPackType(eAct);
            if (eSNOPack != SNOPACKTYPE_INVALID)
            {
                sAddSNOsFromFile(CRefString().Format("Misc\\%s.app.dat", SNOPackTypeToString(eSNOPack)), tContainer);
            }
        }







        SNOManager::RequireSNOs(SNOManager::SC_ACT, tContainer, FALSE, SNOFilesAsyncPriority_Medium);
        SNOManager::WaitForSNOs(tContainer, NULL, TRUE);


        tContainer.clear();




        if (eAct == ACT_OPEN_WORLD)
        {
            for (int32 i = SNOPACKTYPE_CONSOLE_ACT1; i <= SNOPACKTYPE_CONSOLE_ACT5; ++i)
            {
                SNOPackType eSNOPack = i;


                sAddSNOsFromFile(CRefString().Format("Misc\\%s.game.dat", SNOPackTypeToString(eSNOPack)), tContainer);
            }
        }

        else
        {
            SNOPackType eSNOPack = sActToSNOPackType(eAct);
            if (eSNOPack != SNOPACKTYPE_INVALID)
            {
                sAddSNOsFromFile(CRefString().Format("Misc\\%s.game.dat", SNOPackTypeToString(eSNOPack)), tContainer);
            }
        }







        SNOManager::RequireSNOs(SNOManager::SC_GAMEACT, tContainer, FALSE, SNOFilesAsyncPriority_Medium);
        SNOManager::WaitForSNOs(tContainer, NULL, TRUE);

    }

    m_eCurrentAct = eAct;
}












































































// NON_MATCHING: 3792 vs 3796 bytes; locals/stack layout and hireling loop bound (5) now agree with the DWARF, remaining difference is register allocation: the original spills one more register pair (frame 0x170 vs 0x160) and lays out the two GetStartAssoc scans in the opposite order.
void Impl::UpdateSkills()
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);

    XGrowableMap<SkillInfo, const SkillInfo &, int, int> tNewClientSkills;
    blz::vector<SkillInfo> tAddedClientSkills;
    blz::vector<SkillInfo> tRemovedClientSkills;

    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        RActorID idRActor = ptPlayer->idPlayerRActor;
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (!ptRActor)
        {
            continue;
        }

        SkillInfo tSkillInfo;
        tSkillInfo.snoAnimSet = ActorGetAnimSetSNO(ptRActor->snoActor);
        tSkillInfo.eWeaponClass = WEAPONCLASS_NONE;

        for (int32 i = 0; i < NUM_SKILL_SLOTS; ++i)
        {
            tSkillInfo.snoPower = PlayerGetPowerInActiveSkillSlot(*ptPlayer, i, NULL);
            if (tSkillInfo.snoPower != SNO_NONE)
            {
                tNewClientSkills.SetAt_Tracked(tSkillInfo, 1);
            }
        }

        for (int32 i = 0; i < NUM_TRAIT_SLOTS; ++i)
        {
            tSkillInfo.snoPower = PlayerGetPowerInTraitSlot(*ptPlayer, i);
            if (tSkillInfo.snoPower != SNO_NONE)
            {
                tNewClientSkills.SetAt_Tracked(tSkillInfo, 1);
            }
        }

        for (int32 i = 0; i < 3; ++i)
        {
            GBID gbidLegendaryItem = PlayerGetLegendaryPowerInSlot(*ptPlayer, i);
            if (gbidLegendaryItem != GBID_NONE)
            {
                tSkillInfo.snoPower = ItemGetLegendaryPower(gbidLegendaryItem);
                if (tSkillInfo.snoPower != SNO_NONE)
                {
                    tNewClientSkills.SetAt_Tracked(tSkillInfo, 1);
                }
            }
        }
    }


    if (m_currentHireling != HC_NONE)
    {
        Player * ptPlayer = LocalPlayerGet();
        ACDID idFollowerACD = PlayerGetHireling(ptPlayer->idPlayerACD);
        if (idFollowerACD != ACDID_INVALID)
        {
            ActorCommonData * ptACD = ACDTryToGet(idFollowerACD);
            if (ptACD)
            {
                RActor * ptRActor = RActorTryToGet(RActorID(ptACD->idOwner));
                if (ptRActor)
                {
                    SkillInfo tSkillInfo;
                    tSkillInfo.snoAnimSet = ActorGetAnimSetSNO(ptACD->snoActor);
                    tSkillInfo.eWeaponClass = WEAPONCLASS_NONE;

                    for (int32 nSlot = 0; nSlot < HIRELING_TRAIN_COUNT; ++nSlot)
                    {
                        for (int32 i = 0; i < 2; ++i)
                        {
                            tSkillInfo.snoPower = HirelingGetPower(*ptACD, nSlot, i);
                            if (tSkillInfo.snoPower != SNO_NONE)
                            {
                                HirelingTrainReason eReason = HirelingCanTrainSkillReason(*ptACD, tSkillInfo.snoPower);
                                if (eReason == HIRELING_TRAIN_ALREADY_PURCHASED)
                                {
                                    tNewClientSkills.SetAt_Tracked(tSkillInfo, 1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    POS pos = tNewClientSkills.GetStartAssoc();
    SkillInfo tSkillInfo;
    while (pos)
    {
        tNewClientSkills.GetNextAssocKey(pos, tSkillInfo);

        if (!m_clientSkills.LookupPtr(tSkillInfo))
        {
            tAddedClientSkills.push_back(tSkillInfo);
        }
    }

    pos = m_clientSkills.GetStartAssoc();
    while (pos)
    {
        m_clientSkills.GetNextAssocKey(pos, tSkillInfo);

        if (!tNewClientSkills.LookupPtr(tSkillInfo))
        {
            tRemovedClientSkills.push_back(tSkillInfo);
        }
    }

    uint32 nCount = tRemovedClientSkills.size();
    for (uint32 i = 0; i < nCount; ++i)
    {
        const SkillInfo & tSkillInfoLoc = tRemovedClientSkills[i];





        RemoveSNOTree(TRUE, POWER_SNO, m_clientRequiredRootNodes, m_clientSkillSNOs, tSkillInfoLoc.snoPower, sClientServerSkillFilter, SNOManager::SC_SKILLS, FALSE, WEAPONCLASS_NONE, FALSE);

        Prefetch::ChildSNOContainer tAnimations;
        GetPowerAnimations(tSkillInfoLoc.snoPower, tSkillInfoLoc.snoAnimSet, tSkillInfoLoc.eWeaponClass, tAnimations);

        for (uint32 j = 0; j < tAnimations.size(); ++j)
        {
            SNOName snoname = tAnimations[j];
            RemoveSNOTree(TRUE, ANIMATION_SNO, m_clientRequiredRootNodes, m_clientSkillSNOs, snoname.snoHandle, sClientServerSkillFilter, SNOManager::SC_SKILLS, FALSE, WEAPONCLASS_NONE, FALSE);
        }
    }

    nCount = tAddedClientSkills.size();
    for (uint32 i = 0; i < nCount; ++i)
    {
        const SkillInfo & tSkillInfoLoc = tAddedClientSkills[i];




        AddSNOTree(TRUE, POWER_SNO, m_clientRequiredRootNodes, m_clientSkillSNOs, tSkillInfoLoc.snoPower, sClientServerSkillFilter, SNOManager::SC_SKILLS, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);

        Prefetch::ChildSNOContainer tAnimations;
        GetPowerAnimations(tSkillInfoLoc.snoPower, tSkillInfoLoc.snoAnimSet, tSkillInfoLoc.eWeaponClass, tAnimations);

        for (uint32 j = 0; j < tAnimations.size(); ++j)
        {
            SNOName snoname = tAnimations[j];
            AddSNOTree(TRUE, ANIMATION_SNO, m_clientRequiredRootNodes, m_clientSkillSNOs, snoname.snoHandle, sClientServerSkillFilter, SNOManager::SC_SKILLS, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
        }
    }

    m_clientSkills.Copy(tNewClientSkills);
}


void Impl::UpdateEquipped()
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);

    XGrowableMap<ItemInfo, const ItemInfo &, int, int> tNewClientItems;
    blz::vector<ItemInfo> tAddedClientItems;
    blz::vector<ItemInfo> tRemovedClientItems;

    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        ACDID idACD = ptPlayer->idPlayerACD;
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (!ptACD)
        {
            continue;
        }

        VisualInventory * ptVisualInventory = VisualInventoryGet(idACD);
        if (!ptVisualInventory)
        {
            continue;
        }

        for (int32 i = 0; i < VIS_NUM_SLOTS; ++i)
        {
            GBID gbidItem = ptVisualInventory->tEquipmentCurrent.tVisualItem[i].gbidItem;
            if (gbidItem == GBID_NONE)
            {
                continue;
            }

            ScopedItemDefinitionLockR pdefItem(gbidItem);
            if (!pdefItem)
            {
                continue;
            }


            if (pdefItem->snoActor != SNO_NONE)
            {
                ItemInfo tItemInfo;
                tItemInfo.snoGroup = ACTOR_SNO;
                tItemInfo.snoHandle = pdefItem->snoActor;
                tNewClientItems.SetAt_Tracked(tItemInfo, 1);

                ScopedActorDefinitionLockR pdefItemActor(pdefItem->snoActor);

                if (TagMapGetInt(pdefItemActor->hTagMap, TAG_ACTOR_VISUALINV_ENABLE_OVERRIDE))
                {
                    ActorTag eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_BARBARIAN_MALE;
                    ActorTag eVisualInvActorTag2 = TAG_ACTOR_VISUALINV_ACTOR2_BARBARIAN_MALE;
                    switch (PlayerClassFromGBID(ptACD->hGB.gbid))
                    {
                    case PLAYER_CLASS_BARBARIAN:
                        eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_BARBARIAN_MALE;
                        eVisualInvActorTag2 = TAG_ACTOR_VISUALINV_ACTOR2_BARBARIAN_MALE;
                        break;

                    case PLAYER_CLASS_WIZARD:
                        eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_WIZARD_MALE;
                        eVisualInvActorTag2 = TAG_ACTOR_VISUALINV_ACTOR2_WIZARD_MALE;
                        break;

                    case PLAYER_CLASS_WITCHDOCTOR:
                        eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_WITCHDOCTOR_MALE;
                        eVisualInvActorTag2 = TAG_ACTOR_VISUALINV_ACTOR2_WITCHDOCTOR_MALE;
                        break;

                    case PLAYER_CLASS_MONK:
                        eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_MONK_MALE;
                        eVisualInvActorTag2 = TAG_ACTOR_VISUALINV_ACTOR2_MONK_MALE;
                        break;

                    case PLAYER_CLASS_DEMONHUNTER:
                        eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_DEMONHUNTER_MALE;
                        eVisualInvActorTag2 = TAG_ACTOR_VISUALINV_ACTOR2_DEMONHUNTER_MALE;
                        break;

                    case PLAYER_CLASS_CRUSADER:
                        eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_CRUSADER_MALE;
                        eVisualInvActorTag2 = TAG_ACTOR_VISUALINV_ACTOR2_CRUSADER_MALE;
                        break;

                    case PLAYER_CLASS_NECROMANCER:
                        eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_NECROMANCER_MALE;
                        eVisualInvActorTag2 = TAG_ACTOR_VISUALINV_ACTOR2_NECROMANCER_MALE;
                        break;


                    }


                    if ((ptPlayer->tHeroStateData.uPlayerFlags >> PLAYERFLAG_FEMALE) & 1)
                    {
                        eVisualInvActorTag |= 1;
                        eVisualInvActorTag2 |= 1;
                    }

                    SNO snoAlternate = TagMapGetSNO(pdefItemActor->hTagMap, eVisualInvActorTag);
                    SNO snoAlternate2 = TagMapGetSNO(pdefItemActor->hTagMap, eVisualInvActorTag2);

                    if (snoAlternate != SNO_NONE)
                    {
                        ItemInfo tScopedItemInfo;
                        tScopedItemInfo.snoGroup = ACTOR_SNO;
                        tScopedItemInfo.snoHandle = snoAlternate;
                        tNewClientItems.SetAt_Tracked(tScopedItemInfo, 1);
                    }

                    if (snoAlternate2 != SNO_NONE)
                    {
                        ItemInfo tScopedItemInfo;
                        tScopedItemInfo.snoGroup = ACTOR_SNO;
                        tScopedItemInfo.snoHandle = snoAlternate2;
                        tNewClientItems.SetAt_Tracked(tScopedItemInfo, 1);
                    }
                }
            }


            for (int32 j = 0; j < (int32)(pdefItem->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); ++j)
            {
                if (pdefItem->ptAttributes[j].eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
                {
                    if (pdefItem->ptAttributes[j].nParam != SNO_NONE)
                    {
                        ItemInfo tItemInfo;
                        tItemInfo.snoGroup = POWER_SNO;
                        tItemInfo.snoHandle = pdefItem->ptAttributes[j].nParam;
                        tNewClientItems.SetAt_Tracked(tItemInfo, 1);
                    }
                }
            }
        }
    }


    if (m_currentHireling != HC_NONE)
    {
        Player * ptPlayer = LocalPlayerGet();
        ACDID idFollowerACD = PlayerGetHireling(ptPlayer->idPlayerACD);
        if (idFollowerACD != ACDID_INVALID)
        {
            ActorCommonData * ptACD = ACDTryToGet(idFollowerACD);
            if (ptACD)
            {
                RActor * ptRActor = RActorTryToGet(RActorID(ptACD->idOwner));
                if (ptRActor)
                {
                    VisualInventory * ptVisualInventory = VisualInventoryGet(idFollowerACD);
                    if (ptVisualInventory)
                    {
                        for (int32 i = 0; i < VIS_NUM_SLOTS; ++i)
                        {
                            GBID gbidItem = ptVisualInventory->tEquipmentCurrent.tVisualItem[i].gbidItem;
                            if (gbidItem != GBID_NONE)
                            {
                                ScopedItemDefinitionLockR pdefItem(gbidItem);
                                if (pdefItem)
                                {
                                    if (pdefItem->snoActor != SNO_NONE)
                                    {
                                        ItemInfo tItemInfo;
                                        tItemInfo.snoGroup = ACTOR_SNO;
                                        tItemInfo.snoHandle = pdefItem->snoActor;
                                        tNewClientItems.SetAt_Tracked(tItemInfo, 1);


                                        for (int32 j = 0; j < (int32)(pdefItem->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); ++j)
                                        {
                                            if (pdefItem->ptAttributes[j].eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
                                            {
                                                if (pdefItem->ptAttributes[j].nParam != SNO_NONE)
                                                {
                                                    ItemInfo tScopedItemInfo;
                                                    tScopedItemInfo.snoGroup = POWER_SNO;
                                                    tScopedItemInfo.snoHandle = pdefItem->ptAttributes[j].nParam;
                                                    tNewClientItems.SetAt_Tracked(tScopedItemInfo, 1);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    POS pos = m_clientItems.GetStartAssoc();
    ItemInfo tItemInfo;
    while (pos)
    {
        m_clientItems.GetNextAssocKey(pos, tItemInfo);

        if (!tNewClientItems.LookupPtr(tItemInfo))
        {
            tRemovedClientItems.push_back(tItemInfo);
        }
    }

    pos = tNewClientItems.GetStartAssoc();
    while (pos)
    {
        tNewClientItems.GetNextAssocKey(pos, tItemInfo);

        if (!m_clientItems.LookupPtr(tItemInfo))
        {
            tAddedClientItems.push_back(tItemInfo);
        }
    }

    uint32 nCount = tRemovedClientItems.size();
    for (uint32 i = 0; i < nCount; ++i)
    {
        const ItemInfo & tScopedItemInfo = tRemovedClientItems[i];





        switch (tScopedItemInfo.snoGroup)
        {
        case ACTOR_SNO:
            RemoveSNOTree(TRUE, ACTOR_SNO, m_clientRequiredRootNodes, m_clientActorSNOs, tScopedItemInfo.snoHandle, sClientServerItemActorFilter, SNOManager::SC_CLIENTACTORS, FALSE, WEAPONCLASS_NONE, FALSE);
            break;
        case POWER_SNO:
            RemoveSNOTree(TRUE, POWER_SNO, m_clientRequiredRootNodes, m_clientActorSNOs, tScopedItemInfo.snoHandle, sClientServerSkillFilter, SNOManager::SC_CLIENTACTORS, FALSE, WEAPONCLASS_NONE, FALSE);
            break;
        }
    }

    nCount = tAddedClientItems.size();
    for (uint32 i = 0; i < nCount; ++i)
    {
        const ItemInfo & tScopedItemInfo = tAddedClientItems[i];





        switch (tScopedItemInfo.snoGroup)
        {
        case ACTOR_SNO:
            AddSNOTree(TRUE, ACTOR_SNO, m_clientRequiredRootNodes, m_clientActorSNOs, tScopedItemInfo.snoHandle, sClientServerItemActorFilter, SNOManager::SC_CLIENTACTORS, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
            break;
        case POWER_SNO:
            AddSNOTree(TRUE, POWER_SNO, m_clientRequiredRootNodes, m_clientActorSNOs, tScopedItemInfo.snoHandle, sClientServerSkillFilter, SNOManager::SC_CLIENTACTORS, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
            break;
        }
    }

    m_clientItems.Copy(tNewClientItems);
}

// NON_MATCHING: 2624 vs 2616 bytes; crafter-map loop register allocation, spills and branch scheduling differ; retained DWARF loop/statement order (one variant).
void Impl::UpdateCrafters()
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);

    XGrowableMap<CrafterInfo, const CrafterInfo &, int, int> tNewClientCrafters;
    blz::vector<CrafterInfo> tAddedClientCrafters;
    blz::vector<CrafterInfo> tRemovedClientCrafters;

    Player * ptPlayer = LocalPlayerGet();

    for (int32 i = 0; i < sizeof(m_clientCrafterActorSNO) / sizeof(m_clientCrafterActorSNO[0]); ++i)
    {
        SNO snoActor = m_clientCrafterActorSNO[i];
        if (snoActor == SNO_NONE)
        {
            continue;
        }

        CrafterType eType = i;
        CrafterInfo tCrafterInfo;
        tCrafterInfo.eType = eType;
        tCrafterInfo.snoAnimSet = ActorGetAnimSetSNO(snoActor);
        int32 nCrafterLevel = GetCraftLevel(eType, *ptPlayer);
        tCrafterInfo.dwRank = CrafterGetRank(eType, nCrafterLevel);
        tNewClientCrafters.SetAt_Tracked(tCrafterInfo, 1);
    }

    POS pos = tNewClientCrafters.GetStartAssoc();
    CrafterInfo tCrafterInfo;
    while (pos)
    {
        tNewClientCrafters.GetNextAssocKey(pos, tCrafterInfo);

        if (!m_clientCrafters.LookupPtr(tCrafterInfo))
        {
            tAddedClientCrafters.push_back(tCrafterInfo);
        }
    }

    pos = m_clientCrafters.GetStartAssoc();
    while (pos)
    {
        m_clientCrafters.GetNextAssocKey(pos, tCrafterInfo);

        if (!tNewClientCrafters.LookupPtr(tCrafterInfo))
        {
            tRemovedClientCrafters.push_back(tCrafterInfo);
        }
    }

    uint32 nCount = tRemovedClientCrafters.size();
    for (uint32 i = 0; i < nCount; ++i)
    {
        const CrafterInfo & tScopedCrafterInfo = tRemovedClientCrafters[i];





        ScopedAnimSetDefinitionLockR pdefAnimSet(tScopedCrafterInfo.snoAnimSet);
        SNO snoAnim = TagMapGetSNO(pdefAnimSet->hTagMapAnimDefault, CrafterGetIdleAnim(tScopedCrafterInfo.dwRank));
        RemoveSNOTree(TRUE, ANIMATION_SNO, m_clientRequiredRootNodes, m_clientCrafterSNOs, snoAnim, sClientActorFilter, SNOManager::SC_CRAFTERS, FALSE, WEAPONCLASS_NONE, FALSE);
        if (tScopedCrafterInfo.dwRank + 1 < CRAFTER_RANKS)
        {
            snoAnim = TagMapGetSNO(pdefAnimSet->hTagMapAnimDefault, CrafterGetLevelUpAnim(tScopedCrafterInfo.dwRank + 1));
            RemoveSNOTree(TRUE, ANIMATION_SNO, m_clientRequiredRootNodes, m_clientCrafterSNOs, snoAnim, sClientActorFilter, SNOManager::SC_CRAFTERS, FALSE, WEAPONCLASS_NONE, FALSE);
        }
    }

    nCount = tAddedClientCrafters.size();
    for (uint32 i = 0; i < nCount; ++i)
    {
        const CrafterInfo & tScopedCrafterInfo = tAddedClientCrafters[i];





        ScopedAnimSetDefinitionLockR pdefAnimSet(tScopedCrafterInfo.snoAnimSet);
        SNO snoAnim = TagMapGetSNO(pdefAnimSet->hTagMapAnimDefault, CrafterGetIdleAnim(tScopedCrafterInfo.dwRank));
        AddSNOTree(TRUE, ANIMATION_SNO, m_clientRequiredRootNodes, m_clientCrafterSNOs, snoAnim, sClientActorFilter, SNOManager::SC_CRAFTERS, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
        if (tScopedCrafterInfo.dwRank + 1 < CRAFTER_RANKS)
        {
            snoAnim = TagMapGetSNO(pdefAnimSet->hTagMapAnimDefault, CrafterGetLevelUpAnim(tScopedCrafterInfo.dwRank + 1));
            AddSNOTree(TRUE, ANIMATION_SNO, m_clientRequiredRootNodes, m_clientCrafterSNOs, snoAnim, sClientActorFilter, SNOManager::SC_CRAFTERS, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
        }
    }

    m_clientCrafters.Copy(tNewClientCrafters);
}









void Impl::UpdateHireling()
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);

    HirelingClass eNewClass = 0;

    Player * ptPlayer = LocalPlayerGet();
    ACDID idFollowerACD = PlayerGetHireling(ptPlayer->idPlayerACD);
    if (idFollowerACD != ACDID_INVALID)
    {
        ActorCommonData * ptACD = ACDTryToGet(idFollowerACD);
        if (ptACD)
        {
            eNewClass = HirelingGetClass(*ptACD);
        }
    }

    if (eNewClass == m_currentHireling)
    {
        return;
    }

    if (m_currentHireling != 0)
    {




        SNO snoActor = SNO_NONE;
        for (int32 i = 0; i < sizeof(s_tHirelingClassToHirelingActor) / sizeof(s_tHirelingClassToHirelingActor[0]); ++i)
        {
            if (s_tHirelingClassToHirelingActor[i].eClass == m_currentHireling)
            {
                snoActor = s_tHirelingClassToHirelingActor[i].snoHandle;
                break;
            }
        }


        RemoveSNOTree(TRUE, ACTOR_SNO, m_clientHirelingRequiredRootNodes, m_clientHirelingSNOs, snoActor, sClientServerHirelingActorFilter, SNOManager::SC_HIRELING, FALSE, WEAPONCLASS_NONE, FALSE);
    }

    if (eNewClass != 0)
    {




        SNO snoActor = SNO_NONE;
        for (int32 i = 0; i < sizeof(s_tHirelingClassToHirelingActor) / sizeof(s_tHirelingClassToHirelingActor[0]); ++i)
        {
            if (s_tHirelingClassToHirelingActor[i].eClass == eNewClass)
            {
                snoActor = s_tHirelingClassToHirelingActor[i].snoHandle;
                break;
            }
        }


        AddSNOTree(TRUE, ACTOR_SNO, m_clientHirelingRequiredRootNodes, m_clientHirelingSNOs, snoActor, sClientServerHirelingActorFilter, SNOManager::SC_HIRELING, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
    }

    m_currentHireling = eNewClass;
}


inline void Impl::AddClientUI(const SNO sno)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    AddSNOTree(TRUE, UI_SNO, m_clientRequiredRootNodes, m_clientUISNOs, sno, sClientUIFilter, SNOManager::SC_CLIENTUIS, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
}




inline BOOL Impl::AreAllClientUIChildrenLoaded(const SNO sno)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    return IsSNOTreeLoaded(TRUE, UI_SNO, sno, sClientUIWithTexturesFilter, FALSE, WEAPONCLASS_NONE, FALSE);
}




inline BOOL Impl::AddClientUIChildren(const SNO sno)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    return AddSNOTree(TRUE, UI_SNO, m_clientUIChildrenRequiredRootNodes, m_clientUISNOs, sno, sClientUIWithTexturesFilter, SNOManager::SC_CLIENTUIS, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_High);
}




inline void Impl::RemoveClientUIChildren(const SNO sno)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    RemoveSNOTree(TRUE, UI_SNO, m_clientUIChildrenRequiredRootNodes, m_clientUISNOs, sno, sClientUIWithTexturesFilter, SNOManager::SC_CLIENTUIS, FALSE, WEAPONCLASS_NONE, FALSE);
}




inline void Impl::WaitForClientUIChildren(const SNO sno)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    WaitForSNOTree(TRUE, UI_SNO, sno, sClientUIWithTexturesFilter, FALSE, WEAPONCLASS_NONE, FALSE);
}

































void Impl::WaitForAllUIRoots()
{

    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);

    sWaitForRequests(TRUE, SNO_NONE, FALSE);
    SNONameArray clientUIRoots;
    POS pos = m_clientUISNOs.GetStartAssoc();
    while (pos)
    {
        CompressedSNOName snoname;
        m_clientUISNOs.GetNextAssocKey(pos, snoname);

        if (snoname.snoGroup == UI_SNO)
        {
            clientUIRoots.push_back(snoname);
        }
    }

    SNOManager::WaitForSNOs(clientUIRoots, NULL, FALSE);

}


void Impl::SetClientWorld(
    const SNO sno)
{
    if (m_clientWorld == sno)
        return;



    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);

    if (m_clientWorld == SNO_NONE)
    {
        AddSNOTree(TRUE, WORLD_SNO, m_clientPerActRequiredRootNodes, m_clientPerActWorldSNOs, sno, sClientWorldFilter, SNOManager::SC_CLIENT_ACT_WORLDS, CGameLoadingScreenIsEnabled() && !CGameLoadingScreenIsBlackOnly() && CGameLoadingScreenCanProgress(), WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
    }
    else if (sno != SNO_NONE)
    {
        ChangeSNOTree(TRUE, WORLD_SNO, m_clientPerActRequiredRootNodes, m_clientPerActWorldSNOs, m_clientWorld, sClientWorldFilter, sno, sClientWorldFilter, SNOManager::SC_CLIENT_ACT_WORLDS, FALSE, CGameLoadingScreenIsEnabled() && !CGameLoadingScreenIsBlackOnly() && CGameLoadingScreenCanProgress(), WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
    }
    else
    {
        RemoveSNOTree(TRUE, WORLD_SNO, m_clientPerActRequiredRootNodes, m_clientPerActWorldSNOs, m_clientWorld, sClientWorldFilter, SNOManager::SC_CLIENT_ACT_WORLDS, FALSE, WEAPONCLASS_NONE, FALSE);
    }

    m_clientWorld = sno;
}


inline SNO Impl::GetClientWorld()
{
    return m_clientWorld;
}




inline void Impl::AddFullClientWorld(const SNO sno, BOOL fWaitForSNOs)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    AddSNOTree(TRUE, WORLD_SNO, m_clientPerActRequiredRootNodes, m_clientPerActWorldSNOs, sno, sClientFullWorldFilter, SNOManager::SC_CLIENT_ACT_WORLDS, fWaitForSNOs, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
}





inline void Impl::WaitForFullClientWorld(const SNO snoWorld)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    if (m_clientPerActRequiredRootNodes.LookupPtr(CompressedSNOName(SNOName(WORLD_SNO, snoWorld))))
    {
        WaitForSNOTree(TRUE, WORLD_SNO, snoWorld, sClientFullWorldFilter, TRUE, WEAPONCLASS_NONE, FALSE);
    }
}




inline void Impl::AddNeutralClientActor(const SNO sno, BOOL fWaitForSNOs)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    AddSNOTree(TRUE, ACTOR_SNO, m_clientRequiredRootNodes, m_clientActorSNOs, sno, sClientActorFilter, SNOManager::SC_CLIENTACTORS, fWaitForSNOs, WEAPONCLASS_NONE, TRUE, SNOFilesAsyncPriority_Medium);
}





inline void Impl::AddClientTextures(const SNONameArray & snoTextures)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    AddSNOArray(m_clientPerActRequiredRootNodes, m_clientPerActTextureSNOs, snoTextures, SNOManager::SC_CLIENT_ACT_TEXTURES, FALSE);
}




inline void Impl::RemoveClientTextures(const SNONameArray & snoTextures)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    RemoveSNOArray(m_clientPerActRequiredRootNodes, m_clientPerActTextureSNOs, snoTextures, SNOManager::SC_CLIENT_ACT_TEXTURES, FALSE);
}





inline void Impl::WaitForClientScene(const SNO snoScene)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    if (m_clientPerActRequiredRootNodes.LookupPtr(CompressedSNOName(SNOName(SCENE_SNO, snoScene))))
    {
        WaitForSNOTree(TRUE, SCENE_SNO, snoScene, sClientSceneFilterWait, TRUE, WEAPONCLASS_NONE, FALSE);
    }
}












inline BOOL Impl::PrefetchClientWorld(const SNO snoWorld, SNOFilesAsyncPriority ePriority)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    return PrefetchSNOTree(TRUE, WORLD_SNO, snoWorld, sClientWorldFilter, TRUE, WEAPONCLASS_NONE, FALSE, ePriority);
}







inline BOOL Impl::PrefetchClientScene(const SNO snoScene, SNOFilesAsyncPriority ePriority)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    return PrefetchSNOTree(TRUE, SCENE_SNO, snoScene, sClientSceneFilter, TRUE, WEAPONCLASS_NONE, FALSE, ePriority);
}







inline BOOL Impl::PrefetchClientLevelArea(const SNO snoLevelArea, SNOFilesAsyncPriority ePriority)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    return PrefetchSNOTree(TRUE, LEVELAREA_SNO, snoLevelArea, sClientLevelAreaFilter, TRUE, WEAPONCLASS_NONE, FALSE, ePriority);
}






void Impl::AddClientScene(
    const SSceneID idSScene,
    const SNO sno,
    const SNO * ptSNOLevelAreas)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);

    SceneSNOAndRefCount snoRef;
    if (m_clientSSceneIDToSNO.Lookup(idSScene, snoRef))
    {
        ++snoRef.nRefCount;






        m_clientSSceneIDToSNO.SetAt_Tracked(idSScene, snoRef);
    }
    else
    {




        snoRef.sno = sno;
        for (int32 i = 0; i < sizeof(snoRef.snoLevelAreas) / sizeof(snoRef.snoLevelAreas[0]); ++i)
        {
            snoRef.snoLevelAreas[i] = ptSNOLevelAreas[i];
        }
        snoRef.nRefCount = 1;
        m_clientSSceneIDToSNO.SetAt_Tracked(idSScene, snoRef);
        AddSNOTree(TRUE, SCENE_SNO, m_clientPerActRequiredRootNodes, m_clientPerActSceneSNOs, sno, sClientSceneFilter, SNOManager::SC_CLIENT_ACT_SCENES, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
        for (int32 i = 0; i < sizeof(snoRef.snoLevelAreas) / sizeof(snoRef.snoLevelAreas[0]); ++i)
        {
            if (ptSNOLevelAreas[i] != SNO_NONE)
            {
                AddSNOTree(TRUE, LEVELAREA_SNO, m_clientPerActRequiredRootNodes, m_clientPerActSceneSNOs, ptSNOLevelAreas[i], sClientLevelAreaFilter, SNOManager::SC_CLIENT_ACT_SCENES, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
                PrefetchSNOTree(TRUE, LEVELAREA_SNO, ptSNOLevelAreas[i], sClientLevelAreaPrefetchFilter, TRUE, WEAPONCLASS_NONE, TRUE, SNOFilesAsyncPriority_Medium);
            }
        }
    }
}


void Impl::RemoveClientScene(
    const SSceneID idSScene)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);

    SceneSNOAndRefCount snoRef;
    if (m_clientSSceneIDToSNO.Lookup(idSScene, snoRef))
    {




        if (snoRef.nRefCount == 1)
        {
            RemoveSNOTree(TRUE, SCENE_SNO, m_clientPerActRequiredRootNodes, m_clientPerActSceneSNOs, snoRef.sno, sClientSceneFilter, SNOManager::SC_CLIENT_ACT_SCENES, FALSE, WEAPONCLASS_NONE, FALSE);
            for (int32 i = 0; i < sizeof(snoRef.snoLevelAreas) / sizeof(snoRef.snoLevelAreas[0]); ++i)
            {
                if (snoRef.snoLevelAreas[i] != SNO_NONE)
                {
                    RemoveSNOTree(TRUE, LEVELAREA_SNO, m_clientPerActRequiredRootNodes, m_clientPerActSceneSNOs, snoRef.snoLevelAreas[i], sClientLevelAreaFilter, SNOManager::SC_CLIENT_ACT_SCENES, FALSE, WEAPONCLASS_NONE, FALSE);
                }
            }
            m_clientSSceneIDToSNO.RemoveAt(idSScene);
        }
        else
        {
            snoRef.nRefCount--;
            m_clientSSceneIDToSNO.SetAt_Tracked(idSScene, snoRef);
        }
    }
}


inline void Impl::AddClientSceneBySNO(const SNO sno)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    AddSNOTree(TRUE, SCENE_SNO, m_clientPerActRequiredRootNodes, m_clientPerActSceneSNOs, sno, sClientSceneFilter, SNOManager::SC_CLIENT_ACT_SCENES, CGameLoadingScreenIsEnabled() && !CGameLoadingScreenIsBlackOnly() && CGameLoadingScreenCanProgress(), WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
}




inline void Impl::RemoveClientSceneBySNO(const SNO sno)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    RemoveSNOTree(TRUE, SCENE_SNO, m_clientPerActRequiredRootNodes, m_clientPerActSceneSNOs, sno, sClientSceneFilter, SNOManager::SC_CLIENT_ACT_SCENES, FALSE, WEAPONCLASS_NONE, FALSE);
}




inline void Impl::AddClientWeatherBySNO(const SNO sno)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    AddSNOTree(TRUE, WEATHER_SNO, m_clientPerActRequiredRootNodes, m_clientPerActSceneSNOs, sno, sClientWeatherFilter, SNOManager::SC_CLIENT_ACT_SCENES, CGameLoadingScreenIsEnabled() && !CGameLoadingScreenIsBlackOnly() && CGameLoadingScreenCanProgress(), WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
}




inline void Impl::RemoveClientWeatherBySNO(const SNO sno)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    RemoveSNOTree(TRUE, WEATHER_SNO, m_clientPerActRequiredRootNodes, m_clientPerActSceneSNOs, sno, sClientWeatherFilter, SNOManager::SC_CLIENT_ACT_SCENES, FALSE, WEAPONCLASS_NONE, FALSE);
}




inline BOOL Impl::PrefetchClientWeather(const SNO sno, SNOFilesAsyncPriority ePriority)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    return PrefetchSNOTree(TRUE, WEATHER_SNO, sno, sClientWeatherFilter, TRUE, WEAPONCLASS_NONE, FALSE, ePriority);
}





BOOL Impl::PrefetchClientActor(
    const SNO sno,
    const GBID * pgbidMonsterAffixes,
    SNOFilesAsyncPriority ePriority)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);

    BOOL fAllPrefetched = TRUE;
    if (pgbidMonsterAffixes)
    {
        for (int32 i = 0; i < NUM_MONSTER_AFFIXES; ++i)
        {
            GBID gbidAffix = pgbidMonsterAffixes[i];
            if (gbidAffix == GBID_NONE)
            {
                continue;
            }

            GBHandle hGB(18, gbidAffix);
            if (!GBIDExists(hGB))
            {
                continue;
            }

            const GameBalanceDefinition * pdefGB;
            const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);

            fAllPrefetched = PrefetchSNOTree(TRUE, POWER_SNO, ptAffix->snoOnDeathPower, sClientActorFilter, FALSE, WEAPONCLASS_NONE, FALSE, ePriority) && fAllPrefetched;
            for (int32 nOnSpawn = 0; nOnSpawn < 3; ++nOnSpawn)
            {
                fAllPrefetched = PrefetchSNOTree(TRUE, POWER_SNO, ptAffix->snoSpawnPower[nOnSpawn], sClientActorFilter, FALSE, WEAPONCLASS_NONE, FALSE, ePriority) && fAllPrefetched;
            }

            GBUnlockR(pdefGB);
        }
    }

    return PrefetchSNOTree(TRUE, ACTOR_SNO, sno, sClientActorFilter, TRUE, WEAPONCLASS_NONE, FALSE, ePriority) && fAllPrefetched;

}


void Impl::AddClientActor(
    const ACDNetworkName ann,
    const SNO sno,
    const WeaponClass eWeaponClass,
    const GBID * pgbidMonsterAffixes)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    BOOL fHero = FALSE;

    SNONameRefMap * ptRootNodes = &m_clientPerActRequiredRootNodes;
    SNOManager::SNOContainer * ptSNOContainer = &m_clientPerActActorSNOs;
    SNOManager::SNOCategory eCategory = SNOManager::SC_CLIENT_ACT_ACTORS;

    ACDSNOAndRefCount snoRef;
    if (m_clientANNToSNO.Lookup(ann, snoRef))
    {
        ++snoRef.nRefCount;






        if (snoRef.eWeaponClass != eWeaponClass)
        {
            AddSNOTree(TRUE, ACTOR_SNO, *ptRootNodes, *ptSNOContainer, sno, sClientActorFilter, eCategory, FALSE, eWeaponClass, FALSE, SNOFilesAsyncPriority_Medium);
            RemoveSNOTree(TRUE, ACTOR_SNO, *ptRootNodes, *ptSNOContainer, sno, sClientActorFilter, eCategory, FALSE, snoRef.eWeaponClass, FALSE);
            snoRef.eWeaponClass = eWeaponClass;
        }

        m_clientANNToSNO.SetAt_Tracked(ann, snoRef);
    }

    else
    {


        snoRef.sno = sno;
        snoRef.eWeaponClass = eWeaponClass;
        for (int32 i = 0; i < NUM_MONSTER_AFFIXES; ++i)
        {
            snoRef.gbidMonsterAffixes[i] = pgbidMonsterAffixes[i];
        }
        snoRef.nRefCount = 1;
        m_clientANNToSNO.SetAt_Tracked(ann, snoRef);
        AddSNOTree(TRUE, ACTOR_SNO, *ptRootNodes, *ptSNOContainer, sno, sClientActorFilter, eCategory, FALSE, eWeaponClass, FALSE, SNOFilesAsyncPriority_Medium);
        for (int32 i = 0; i < NUM_MONSTER_AFFIXES; ++i)
        {
            GBID gbidAffix = snoRef.gbidMonsterAffixes[i];
            if (gbidAffix == GBID_NONE)
            {
                continue;
            }

            GBHandle hGB(18, gbidAffix);
            if (!GBIDExists(hGB))
            {
                continue;
            }

            const GameBalanceDefinition * pdefGB;
            const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);

            AddSNOTree(TRUE, POWER_SNO, *ptRootNodes, *ptSNOContainer, ptAffix->snoOnDeathPower, sClientActorFilter, eCategory, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
            for (int32 nOnSpawn = 0; nOnSpawn < 3; ++nOnSpawn)
            {
                AddSNOTree(TRUE, POWER_SNO, *ptRootNodes, *ptSNOContainer, ptAffix->snoSpawnPower[nOnSpawn], sClientActorFilter, eCategory, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
            }

            GBUnlockR(pdefGB);
        }

        CrafterType eType = sGetCrafterTypeFromActor(sno);
        if (eType != -1)
        {
            m_clientCrafterActorSNO[eType] = sno;
            UpdateCrafters();
        }
    }
}


inline BOOL Impl::IsClientActorLoaded(const ACDNetworkName ann)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    ACDSNOAndRefCount * ptSNOAndRefCount = m_clientANNToSNO.LookupPtr(ann);
    if (ptSNOAndRefCount)
    {
        return IsSNOTreeLoaded(TRUE, ACTOR_SNO, ptSNOAndRefCount->sno, sClientActorFilterWait, TRUE, ptSNOAndRefCount->eWeaponClass, FALSE);
    }
    return FALSE;
}





inline BOOL Impl::IsClientActorPending(const ACDNetworkName ann)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    ACDSNOAndRefCount * ptSNOAndRefCount = m_clientANNToSNO.LookupPtr(ann);
    if (ptSNOAndRefCount)
    {
        return IsSNOTreePending(TRUE, ACTOR_SNO, ptSNOAndRefCount->sno, sClientActorFilterWait, TRUE, ptSNOAndRefCount->eWeaponClass, FALSE);
    }
    return FALSE;
}





inline BOOL Impl::IsClientActorRequested(const ACDNetworkName ann)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    return m_clientANNToSNO.LookupPtr(ann) != NULL;
}




inline void Impl::WaitForClientActor(const ACDNetworkName ann)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    ACDSNOAndRefCount * ptSNOAndRefCount = m_clientANNToSNO.LookupPtr(ann);
    if (ptSNOAndRefCount)
    {
        WaitForSNOTree(TRUE, ACTOR_SNO, ptSNOAndRefCount->sno, sClientActorFilterWait, TRUE, ptSNOAndRefCount->eWeaponClass, FALSE);
    }
}




void Impl::RemoveClientActor(
    const ACDNetworkName ann)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);

    ACDSNOAndRefCount snoRef;
    if (m_clientANNToSNO.Lookup(ann, snoRef))
    {
        if (snoRef.nRefCount == 1)
        {




            BOOL fHero = FALSE;
            SNONameRefMap * ptRootNodes = &m_clientPerActRequiredRootNodes;
            SNOManager::SNOContainer * ptSNOContainer = &m_clientPerActActorSNOs;
            SNOManager::SNOCategory eCategory = SNOManager::SC_CLIENT_ACT_ACTORS;
            RemoveSNOTree(TRUE, ACTOR_SNO, *ptRootNodes, *ptSNOContainer, snoRef.sno, sClientActorFilter, eCategory, FALSE, snoRef.eWeaponClass, FALSE);

            for (int32 i = 0; i < NUM_MONSTER_AFFIXES; ++i)
            {
                GBID gbidAffix = snoRef.gbidMonsterAffixes[i];
                if (gbidAffix == GBID_NONE)
                {
                    continue;
                }

                GBHandle hGB(18, gbidAffix);
                if (!GBIDExists(hGB))
                {
                    continue;
                }

                const GameBalanceDefinition * pdefGB;
                const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);

                RemoveSNOTree(TRUE, POWER_SNO, *ptRootNodes, *ptSNOContainer, ptAffix->snoOnDeathPower, sClientActorFilter, eCategory, FALSE, WEAPONCLASS_NONE, FALSE);
                for (int32 nOnSpawn = 0; nOnSpawn < 3; ++nOnSpawn)
                {
                    RemoveSNOTree(TRUE, POWER_SNO, *ptRootNodes, *ptSNOContainer, ptAffix->snoSpawnPower[nOnSpawn], sClientActorFilter, eCategory, FALSE, WEAPONCLASS_NONE, FALSE);
                }

                GBUnlockR(pdefGB);
            }

            CrafterType eType = sGetCrafterTypeFromActor(snoRef.sno);
            if (eType != -1)
            {
                m_clientCrafterActorSNO[eType] = SNO_NONE;
                UpdateCrafters();
            }

            m_clientANNToSNO.RemoveAt(ann);
        }
        else
        {




            snoRef.nRefCount--;
            m_clientANNToSNO.SetAt_Tracked(ann, snoRef);
        }
    }
}


inline void Impl::AddClientActorBySNO(const SNO sno)
{
    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    AddSNOTree(TRUE, ACTOR_SNO, m_clientPerActRequiredRootNodes, m_clientPerActActorSNOs, sno, sClientActorFilter, SNOManager::SC_CLIENT_ACT_ACTORS, CGameLoadingScreenIsEnabled() && !CGameLoadingScreenIsBlackOnly() && CGameLoadingScreenCanProgress(), WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
}









inline void Impl::RemoveClientActorBySNO(const SNO sno)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tClientCrit);
    RemoveSNOTree(TRUE, ACTOR_SNO, m_clientPerActRequiredRootNodes, m_clientPerActActorSNOs, sno, sClientActorFilter, SNOManager::SC_CLIENT_ACT_ACTORS, FALSE, WEAPONCLASS_NONE, FALSE);
}














inline void Impl::AddServerActorBySNO(const SNO sno)
{
    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    AddSNOTree(FALSE, ACTOR_SNO, m_serverRequiredRootNodes, m_serverActorSNOs, sno, sServerActorFilter, SNOManager::SC_SERVERACTORS, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_High);
}











inline void Impl::RemoveServerActorBySNO(const SNO sno)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    RemoveSNOTree(FALSE, ACTOR_SNO, m_serverRequiredRootNodes, m_serverActorSNOs, sno, sServerActorFilter, SNOManager::SC_SERVERACTORS, FALSE, WEAPONCLASS_NONE, FALSE);
}




void Impl::PrefetchServerActorAvenger(
    const SNO snoActor,
    const GBID * pgbidMonsterAffixes)
{
    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);

    PrefetchSNOTree(FALSE, ACTOR_SNO, snoActor, sServerActorAvengerFilter, TRUE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);

    if (!pgbidMonsterAffixes)
    {
        return;
    }


    for (int32 i = 0; i < NUM_MONSTER_AFFIXES; ++i)
    {
        GBID gbidAffix = pgbidMonsterAffixes[i];
        if (gbidAffix == GBID_NONE)
        {
            continue;
        }

        GBHandle hGB(18, gbidAffix);
        if (!GBIDExists(hGB))
        {
            continue;
        }

        const GameBalanceDefinition * pdefGB;
        const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);

        PrefetchSNOTree(FALSE, POWER_SNO, ptAffix->snoOnDeathPower, sServerActorFilter, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
        for (int32 nOnSpawn = 0; nOnSpawn < 3; ++nOnSpawn)
        {
            PrefetchSNOTree(FALSE, POWER_SNO, ptAffix->snoSpawnPower[nOnSpawn], sServerActorFilter, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
        }

        GBUnlockR(pdefGB);
    }

}


inline void Impl::RemoveServerWorld(const SNO snoWorld)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    RemoveSNOTree(FALSE, WORLD_SNO, m_serverPerActRequiredRootNodes, m_serverPerActWorldSNOs, snoWorld, sServerWorldFilter, SNOManager::SC_SERVER_ACT_WORLDS, FALSE, WEAPONCLASS_NONE, FALSE);
}




inline void Impl::AddServerWorld(const SNO snoWorld)
{
    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    AddSNOTree(FALSE, WORLD_SNO, m_serverPerActRequiredRootNodes, m_serverPerActWorldSNOs, snoWorld, sServerWorldFilter, SNOManager::SC_SERVER_ACT_WORLDS, TRUE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
}




inline void Impl::RemoveServerWorldLoading(const SNO snoWorld, BOOL fDungeonFinder)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    RemoveSNOTree(FALSE, WORLD_SNO, m_serverPerActRequiredRootNodes, m_serverPerActWorldLoadingSNOs, snoWorld, fDungeonFinder ? sServerDungeonFinderWorldLoadingFilter : sServerWorldLoadingFilter, SNOManager::SC_SERVER_ACT_WORLDSLOADING, FALSE, WEAPONCLASS_NONE, FALSE);
}





inline void Impl::AddServerWorldLoading(const SNO snoWorld, BOOL fDungeonFinder)
{
    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    AddSNOTree(FALSE, WORLD_SNO, m_serverPerActRequiredRootNodes, m_serverPerActWorldLoadingSNOs, snoWorld, fDungeonFinder ? sServerDungeonFinderWorldLoadingFilter : sServerWorldLoadingFilter, SNOManager::SC_SERVER_ACT_WORLDSLOADING, FALSE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
}





inline void Impl::PrefetchWorldLoading(const SNO snoWorld, SNOFilesAsyncPriority ePriority)
{
    PrefetchSNOTree(FALSE, WORLD_SNO, snoWorld, sServerWorldLoadingFilter, TRUE, WEAPONCLASS_NONE, TRUE, ePriority);
}




inline BOOL Impl::IsServerWorldLoadingDone(const SNO snoWorld, BOOL fDungeonFinder)
{
    return !IsSNOTreePending(FALSE, WORLD_SNO, snoWorld, fDungeonFinder ? sServerDungeonFinderWorldLoadingFilter : sServerWorldLoadingFilter, TRUE, WEAPONCLASS_NONE, FALSE);
}




inline void Impl::RemoveServerSkillKit(const SNO sno)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    RemoveSNOTree(FALSE, SKILLKIT_SNO, m_serverRequiredRootNodes, m_serverActorSNOs, sno, sServerSkillKitFilter, SNOManager::SC_SERVERACTORS, FALSE, WEAPONCLASS_NONE, FALSE);
}




inline void Impl::AddServerSkillKit(const SNO sno)
{
    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    AddSNOTree(FALSE, SKILLKIT_SNO, m_serverRequiredRootNodes, m_serverActorSNOs, sno, sServerSkillKitFilter, SNOManager::SC_SERVERACTORS, TRUE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
}




void Impl::PrefetchServerScene(const SNO sno, const SNO * ptSNOLevelAreas)
{


    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);





    PrefetchSNOTree(FALSE, SCENE_SNO, sno, sServerSceneFilter, TRUE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
    for (int32 i = 0; i < 4; ++i)
    {
        if (ptSNOLevelAreas[i] != SNO_NONE)
        {
            PrefetchSNOTree(FALSE, LEVELAREA_SNO, ptSNOLevelAreas[i], sServerLevelAreaFilter, TRUE, WEAPONCLASS_NONE, FALSE, SNOFilesAsyncPriority_Medium);
        }
    }
}


inline void Impl::AddServerSceneBySNO(const SNO sno)
{
    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    BOOL fLoadingScreenEnabled = CGameLoadingScreenIsEnabled();
    AddSNOTree(FALSE, SCENE_SNO, m_serverPerActRequiredRootNodes, m_serverPerActSceneSNOs, sno, sServerSceneFilter, SNOManager::SC_SERVER_ACT_SCENES, fLoadingScreenEnabled, WEAPONCLASS_NONE, FALSE, fLoadingScreenEnabled ? SNOFilesAsyncPriority_High : SNOFilesAsyncPriority_Medium);
}




inline void Impl::RemoveServerSceneBySNO(const SNO sno)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    RemoveSNOTree(FALSE, SCENE_SNO, m_serverPerActRequiredRootNodes, m_serverPerActSceneSNOs, sno, sServerSceneFilter, SNOManager::SC_SERVER_ACT_SCENES, FALSE, WEAPONCLASS_NONE, FALSE);
}




inline void Impl::AddServerLevelArea(const SNO sno)
{
    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    BOOL fLoadingScreenEnabled = CGameLoadingScreenIsEnabled();
    AddSNOTree(FALSE, LEVELAREA_SNO, m_serverPerActRequiredRootNodes, m_serverPerActLevelAreaSNOs, sno, sServerLevelAreaFilter, SNOManager::SC_SERVER_ACT_LEVELAREAS, fLoadingScreenEnabled, WEAPONCLASS_NONE, FALSE, fLoadingScreenEnabled ? SNOFilesAsyncPriority_High : SNOFilesAsyncPriority_Medium);
}




inline void Impl::RemoveServerLevelArea(const SNO sno)
{
    if (!m_fInitializedForGame)
    {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedLock(tServerCrit);
    RemoveSNOTree(FALSE, LEVELAREA_SNO, m_serverPerActRequiredRootNodes, m_serverPerActLevelAreaSNOs, sno, sServerLevelAreaFilter, SNOManager::SC_SERVER_ACT_LEVELAREAS, FALSE, WEAPONCLASS_NONE, FALSE);
}

}  // namespace SNOPreload




namespace SNOPreload {

static void sWaitOnEvent(
    Blizzard::Lock::REvent * ptEvent,
    SNOName snonameReason,
    SNOName * ptSNONameWaitReason)
{

    Blizzard::Time::Microsecond dwStartedUS = Blizzard::Time::Microseconds();
    ThreadIndex nThreadIndex = ThreadGlobals::Get().m_nThreadIndex;
    if (nThreadIndex == THREADINDEX_MAIN)
    {
        Blizzard::Time::Millisecond dwCurMS = Blizzard::Time::Milliseconds();
        BOOL fWaited = FALSE;
        BOOL fCancelWait = FALSE;
        while (!ptEvent->Wait(1) && !fCancelWait)
        {
            SNOManager::SetHasBlockedThisFrame();
            Prefetch::Update(FALSE);
            fWaited = TRUE;
            if (Blizzard::Time::Milliseconds() - dwCurMS > 16)
            {
                dwCurMS = Blizzard::Time::Milliseconds();
                if (CGameLoadingScreenIsEnabled())
                {
                    CGameProgressLoadingScreen(TRUE);






                    fCancelWait = CGameLoadingScreenHasExpired() != FALSE;
                }
            }
        }









        // Only these short-circuit side effects survive; the original diagnostic statement is unrecoverable.
        (void)(CGameLoadingScreenIsEnabled() && CGameLoadingScreenIsBlackOnly());








    }

    else
    {
        ptEvent->WaitForever();
    }







}
} // namespace SNOPreload

void SNOPreload::Impl::AddSNOArray(
    SNOPreload::SNONameRefMap & tRequiredRootNodes,
    SNOManager::SNOContainer & SNOs,
    const SNONameArray & snos,
    const SNOManager::SNOCategory category,
    BOOL fBlocking)
{




    SNONameArray tNewAdded;

    uint32 nCount = snos.size();
    for (uint32 i = 0; i < nCount; ++i)
    {
        SNOName snoname = snos[i];
        int32 * pnCounter = tRequiredRootNodes.LookupPtr(CompressedSNOName(snoname));
        if (pnCounter)
        {
            ++(*pnCounter);
        }
        else
        {
            tRequiredRootNodes.SetAt_Tracked(CompressedSNOName(snoname), 1);
            pnCounter = SNOs.LookupPtr(CompressedSNOName(snoname));


            if (pnCounter)
            {
                ++(*pnCounter);
            }
            else
            {
                SNOs.SetAt_Tracked(CompressedSNOName(snoname), 1);
                tNewAdded.push_back(snoname);
            }
        }
    }

    if (tNewAdded.size())
    {
        Prefetch::BeginAdd();
        SNOManager::AddRequireSNOs(category, tNewAdded, SNOFilesAsyncPriority_Medium);
        Prefetch::EndAdd();

        if (fBlocking)
        {
            SNOManager::WaitForSNOs(tNewAdded, NULL, FALSE);
        }
    }
}


BOOL SNOPreload::Impl::AddSNOTree(
    BOOL fClient,
    const SNOGroup snoGroup,
    SNOPreload::SNONameRefMap & tRequiredRootNodes,
    SNOManager::SNOContainer & SNOs,
    const SNO sno,
    Prefetch::FilterFunc ptFilterFunc,
    const SNOManager::SNOCategory category,
    BOOL fBlocking,
    WeaponClass eWeaponClass,
    BOOL fNeutralAnimOnly,
    SNOFilesAsyncPriority ePriority)
{




    SNOGroup snoFakeGroup = snoGroup;


    if (snoGroup == ACTOR_SNO)
    {
        snoFakeGroup = (SNOGroup)(eWeaponClass + 99);
    }

    SNOName snoname(snoFakeGroup, sno);

    int32 * pnCounter = tRequiredRootNodes.LookupPtr(CompressedSNOName(snoname));
    if (pnCounter)
    {
        ++(*pnCounter);
        return FALSE;
    }




    tRequiredRootNodes.SetAt_Tracked(CompressedSNOName(snoname), 1);





    PreloadTask task;
    task.eType = PRELOADTASKTYPE_ADD_TREE;
    task.fClient = fClient;
    task.fBlocking = fBlocking;
    task.fNeutralAnimOnly = fNeutralAnimOnly;
    task.snoGroup = snoGroup;
    task.sno = sno;
    task.category = category;
    task.ptSNOs = &SNOs;
    task.ePriority = ePriority;
    task.ptFilterFunc = ptFilterFunc;
    task.eWeaponClass = eWeaponClass;

    task.m_ptTaskCompleted = fBlocking ? (fClient ? &m_tClientTaskCompleted : &m_tServerTaskCompleted) : NULL;
    task.m_ptWaitReason = fBlocking ? (fClient ? &m_tClientWaitReason : &m_tServerWaitReason) : NULL;
    if (fClient)
    {
        m_tClientWaitReason = SNOName(snoGroup, sno);
    }
    else
    {
        m_tServerWaitReason = SNOName(snoGroup, sno);
    }






    sSubmitRequest(task);

    if (fBlocking)
    {
        sWaitOnEvent(task.m_ptTaskCompleted, SNOName(snoGroup, sno), task.m_ptWaitReason);
    }

    return TRUE;
}


void SNOPreload::Impl::sAddSNOTree(
    const SNOPreload::PreloadTask & task)
{
    BOOL fClient = task.fClient;
    SNOGroup snoGroup = task.snoGroup;
    SNOManager::SNOContainer & SNOs = *task.ptSNOs;
    SNO sno = task.sno;
    Prefetch::FilterFunc ptFilterFunc = task.ptFilterFunc;
    SNOManager::SNOCategory category = task.category;
    BOOL fBlocking = task.fBlocking;
    WeaponClass eWeaponClass = task.eWeaponClass;
    BOOL fNeutralAnimOnly = task.fNeutralAnimOnly;

    SNOManager::SNOContainer tContainer;
    if (ptFilterFunc)
    {
        tContainer.SetHashTableSize_Tracked(512);
        SNOManager::AddSNOTree(tContainer, SNOName(snoGroup, sno), ptFilterFunc);

        if (fClient)
        {
            sAppendRequiredAnimations(tContainer, eWeaponClass, ptFilterFunc, fNeutralAnimOnly);
        }
    }
    else
    {
        tContainer.SetAt_Tracked(CompressedSNOName(SNOName(snoGroup, sno)), 1);
    }







    SNONameArray tNewAdded;

    Prefetch::BeginAdd();

    {
        POS pos = tContainer.GetStartAssoc();
        CompressedSNOName snoName;
        while (pos)
        {
            tContainer.GetNextAssocKey(pos, snoName);
            int32 * pnValue = SNOs.LookupPtr(snoName);


            if (pnValue)
            {
                ++*pnValue;
            }
            else
            {
                SNOs.SetAt_Tracked(snoName, 1);
                tNewAdded.push_back(static_cast<SNOName>(snoName));
            }
        }
    }

    if (tNewAdded.size())
    {
        SNOManager::AddRequireSNOs(category, tNewAdded, task.ePriority);
        Prefetch::EndAdd();

        if (fBlocking)
        {
            SNOManager::WaitForSNOs(tNewAdded, task.m_ptWaitReason, FALSE);
        }
    }
    else
    {
        Prefetch::EndAdd();
    }

    if (fBlocking)
    {
        task.m_ptTaskCompleted->Set();
    }
}


void SNOPreload::Impl::ChangeSNOTree(
    BOOL fClient,
    const SNOGroup snoGroup,
    SNOPreload::SNONameRefMap & tRequiredRootNodes,
    SNOManager::SNOContainer & SNOs,
    const SNO sno,
    Prefetch::FilterFunc ptBeforeFilterFunc,
    const SNO newSno,
    Prefetch::FilterFunc ptAfterFilterFunc,
    const SNOManager::SNOCategory category,
    BOOL fPurgeNoLongerRequired,
    BOOL fBlocking,
    WeaponClass eWeaponClass,
    BOOL fNeutralAnimOnly,
    SNOFilesAsyncPriority ePriority)
{




    SNOGroup snoFakeGroup = snoGroup;


    if (snoGroup == ACTOR_SNO)
    {
        snoFakeGroup = (SNOGroup)(eWeaponClass + 99);
    }





    SNOName snoname(snoFakeGroup, sno);
    SNOName newsnoname(snoFakeGroup, newSno);

    if (sno != newSno)
    {
        int32 * pnCounter = tRequiredRootNodes.LookupPtr(CompressedSNOName(snoname));
        int32 * pnNewCounter = tRequiredRootNodes.LookupPtr(CompressedSNOName(newsnoname));
        if (!pnCounter)
            return;




        if (*pnCounter > 1 || pnNewCounter)
        {
            AddSNOTree(fClient, snoGroup, tRequiredRootNodes, SNOs, newSno, ptAfterFilterFunc, category, fBlocking, eWeaponClass, fNeutralAnimOnly, SNOFilesAsyncPriority_Medium);
            RemoveSNOTree(fClient, snoGroup, tRequiredRootNodes, SNOs, sno, ptBeforeFilterFunc, category, fPurgeNoLongerRequired, eWeaponClass, fNeutralAnimOnly);
            return;
        }
        tRequiredRootNodes.RemoveAt(CompressedSNOName(snoname));
        tRequiredRootNodes.SetAt_Tracked(CompressedSNOName(newsnoname), 1);
    }

    PreloadTask task;
    task.eType = PRELOADTASKTYPE_CHANGE_TREE;
    task.fClient = fClient;
    task.fBlocking = fBlocking;
    task.fPurgeNoLongerRequired = fPurgeNoLongerRequired;
    task.fNeutralAnimOnly = fNeutralAnimOnly;
    task.snoGroup = snoGroup;
    task.sno = sno;
    task.newSno = newSno;
    task.category = category;
    task.ptSNOs = &SNOs;
    task.ePriority = ePriority;
    task.ptBeforeFilterFunc = ptBeforeFilterFunc;
    task.ptAfterFilterFunc = ptAfterFilterFunc;
    task.eWeaponClass = eWeaponClass;

    task.m_ptTaskCompleted = fBlocking ? (fClient ? &m_tClientTaskCompleted : &m_tServerTaskCompleted) : NULL;
    task.m_ptWaitReason = fBlocking ? (fClient ? &m_tClientWaitReason : &m_tServerWaitReason) : NULL;
    if (fClient)
    {
        m_tClientWaitReason = SNOName(snoGroup, sno);
    }
    else
    {
        m_tServerWaitReason = SNOName(snoGroup, sno);
    }






    sSubmitRequest(task);

    if (fBlocking)
    {
        sWaitOnEvent(task.m_ptTaskCompleted, SNOName(snoGroup, sno), task.m_ptWaitReason);
    }
}


void SNOPreload::Impl::sChangeSNOTree(
    const SNOPreload::PreloadTask & task)
{
    BOOL fClient = task.fClient;
    SNOGroup snoGroup = task.snoGroup;
    SNOManager::SNOContainer & SNOs = *task.ptSNOs;
    SNO sno = task.sno;
    Prefetch::FilterFunc ptBeforeFilterFunc = task.ptBeforeFilterFunc;
    SNO newSno = task.newSno;
    Prefetch::FilterFunc ptAfterFilterFunc = task.ptAfterFilterFunc;
    SNOManager::SNOCategory category = task.category;
    BOOL fPurgeNoLongerRequired = task.fPurgeNoLongerRequired;
    BOOL fBlocking = task.fBlocking;
    WeaponClass eWeaponClass = task.eWeaponClass;
    BOOL fNeutralAnimOnly = task.fNeutralAnimOnly;

    SNOManager::SNOContainer tBeforeContainer;
    tBeforeContainer.SetHashTableSize_Tracked(512);
    SNOManager::AddSNOTree(tBeforeContainer, SNOName(snoGroup, sno), ptBeforeFilterFunc);

    if (fClient)
    {
        sAppendRequiredAnimations(tBeforeContainer, eWeaponClass, ptBeforeFilterFunc, fNeutralAnimOnly);
    }

    SNOManager::SNOContainer tAfterContainer;
    tAfterContainer.SetHashTableSize_Tracked(512);
    SNOManager::AddSNOTree(tAfterContainer, SNOName(snoGroup, newSno), ptAfterFilterFunc);

    if (fClient)
    {
        sAppendRequiredAnimations(tAfterContainer, eWeaponClass, ptAfterFilterFunc, fNeutralAnimOnly);
    }










    SNONameArray tNewAdded;
    SNONameArray tOldRemoved;

    Prefetch::BeginAdd();

    {
        POS pos = tAfterContainer.GetStartAssoc();
        CompressedSNOName snoName;
        while (pos)
        {
            tAfterContainer.GetNextAssocKey(pos, snoName);
            int32 * pnValue = SNOs.LookupPtr(snoName);


            if (pnValue)
            {
                ++*pnValue;
            }
            else
            {
                SNOs.SetAt_Tracked(snoName, 1);
                tNewAdded.push_back(static_cast<SNOName>(snoName));
            }
        }

        pos = tBeforeContainer.GetStartAssoc();
        while (pos)
        {
            tBeforeContainer.GetNextAssocKey(pos, snoName);
            int32 * pnValue = SNOs.LookupPtr(snoName);

            if (pnValue)
            {





                --*pnValue;

                if (*pnValue == 0)
                {
                    SNOs.RemoveAt(snoName);
                    tOldRemoved.push_back(static_cast<SNOName>(snoName));
                }
            }
        }
    }
    if (tOldRemoved.size())
    {
        SNOManager::RemoveRequireSNOs(category, tOldRemoved, fPurgeNoLongerRequired);
    }
    if (tNewAdded.size())
    {
        SNOManager::AddRequireSNOs(category, tNewAdded, task.ePriority);
        Prefetch::EndAdd();

        if (fBlocking)
        {
            SNOManager::WaitForSNOs(tNewAdded, task.m_ptWaitReason, FALSE);
        }
    }
    else
    {
        Prefetch::EndAdd();
    }

    if (fBlocking)
    {
        task.m_ptTaskCompleted->Set();
    }
}


void SNOPreload::Impl::RemoveSNOArray(
    SNOPreload::SNONameRefMap & tRequiredRootNodes,
    SNOManager::SNOContainer & SNOs,
    const SNONameArray & snos,
    const SNOManager::SNOCategory category,
    BOOL fPurgeNoLongerRequired)
{
    SNONameArray tOldRemoved;

    uint32 nCount = snos.size();
    for (uint32 i = 0; i < nCount; ++i)
    {
        SNOName snoname = snos[i];
        int32 * pnCounter = tRequiredRootNodes.LookupPtr(CompressedSNOName(snoname));
        if (pnCounter)
        {




            --(*pnCounter);

            if (*pnCounter == 0)
            {
                tRequiredRootNodes.RemoveAt(CompressedSNOName(snoname));
                pnCounter = SNOs.LookupPtr(CompressedSNOName(snoname));
                if (pnCounter)
                {






                    --(*pnCounter);

                    if (*pnCounter == 0)
                    {
                        SNOs.RemoveAt(CompressedSNOName(snoname));
                        tOldRemoved.push_back(snoname);
                    }
                }
            }
        } }
    if (tOldRemoved.size())
    {
        SNOManager::RemoveRequireSNOs(category, tOldRemoved, fPurgeNoLongerRequired);
    }
}


void SNOPreload::Impl::RemoveSNOTree(BOOL fClient, const SNOGroup snoGroup, SNOPreload::SNONameRefMap & tRequiredRootNodes, SNOManager::SNOContainer & SNOs, const SNO sno, Prefetch::FilterFunc ptFilterFunc, const SNOManager::SNOCategory category, BOOL fPurgeNoLongerRequired, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly)
{









    SNOGroup snoFakeGroup = (snoGroup == ACTOR_SNO) ? (SNOGroup)(eWeaponClass + 99) : snoGroup;



    SNOName snoname(snoFakeGroup, sno);





    int32 * pnCounter = tRequiredRootNodes.LookupPtr(CompressedSNOName(snoname));






    if (pnCounter)
    {
        if (--(*pnCounter) <= 0)
        {








            tRequiredRootNodes.RemoveAt(CompressedSNOName(snoname));

            PreloadTask task;
            task.eType = PRELOADTASKTYPE_REMOVE_TREE;
            task.fClient = fClient;
            task.fPurgeNoLongerRequired = fPurgeNoLongerRequired;
            task.fNeutralAnimOnly = fNeutralAnimOnly;
            task.snoGroup = snoGroup;
            task.sno = sno;
            task.category = category;
            task.ptSNOs = &SNOs;
            task.ePriority = SNOFilesAsyncPriority_Medium;
            task.ptFilterFunc = ptFilterFunc;
            task.eWeaponClass = eWeaponClass;





            sSubmitRequest(task);
        }
    }
}
void SNOPreload::Impl::sRemoveSNOTree(
    const SNOPreload::PreloadTask & task)
{
    BOOL fClient = task.fClient;
    SNOGroup snoGroup = task.snoGroup;
    SNOManager::SNOContainer & SNOs = *task.ptSNOs;
    SNO sno = task.sno;
    Prefetch::FilterFunc ptFilterFunc = task.ptFilterFunc;
    SNOManager::SNOCategory category = task.category;
    BOOL fPurgeNoLongerRequired = task.fPurgeNoLongerRequired;
    WeaponClass eWeaponClass = task.eWeaponClass;
    BOOL fNeutralAnimOnly = task.fNeutralAnimOnly;

    SNOManager::SNOContainer tContainer;
    if (ptFilterFunc)
    {
        tContainer.SetHashTableSize_Tracked(512);
        SNOManager::AddSNOTree(tContainer, SNOName(snoGroup, sno), ptFilterFunc);

        if (fClient)
        {
            sAppendRequiredAnimations(tContainer, eWeaponClass, ptFilterFunc, fNeutralAnimOnly);
        }
    }
    else
    {
        tContainer.SetAt_Tracked(CompressedSNOName(SNOName(snoGroup, sno)), 1);
    }







    SNONameArray tOldRemoved;

    {
        POS pos = tContainer.GetStartAssoc();
        CompressedSNOName snoName;
        while (pos)
        {
            tContainer.GetNextAssocKey(pos, snoName);
            int32 * pnValue = SNOs.LookupPtr(snoName);

            if (pnValue)
            {





                --*pnValue;

                if (*pnValue == 0)
                {
                    SNOs.RemoveAt(snoName);
                    tOldRemoved.push_back(static_cast<SNOName>(snoName));
                }
            }
        }
    }
    if (tOldRemoved.size())
    {
        SNOManager::RemoveRequireSNOs(category, tOldRemoved, fPurgeNoLongerRequired);
    }
}


void SNOPreload::Impl::sAppendRequiredAnimations(
    SNOManager::SNOContainer & SNOs,
    const WeaponClass eWeaponClass,
    Prefetch::FilterFunc ptFilterFunc,
    BOOL fNeutralAnimOnly)
{
    SNOManager::SNOContainer tSNOAnimSets;
    CompressedSNOName snoName;
    BOOL fAnyFound = TRUE;
    while (fAnyFound)
    {
        Prefetch::ChildSNOContainer tNeutralAnimations;
        Prefetch::ChildSNOContainer tAllAnimations;

        fAnyFound = FALSE;

        POS pos = SNOs.GetStartAssoc();
        while (pos)
        {
            int32 nValue;
            SNOs.GetNextAssoc(pos, snoName, nValue);


            if (snoName.snoGroup == ANIMSET_SNO && !tSNOAnimSets.LookupPtr(snoName))
            {
                tSNOAnimSets.SetAt_Tracked(snoName, 1);

                WeaponClass eActualWeaponClass = eWeaponClass;

                fAnyFound = TRUE;
                uint32 nOldSize = tAllAnimations.size();
                if (fNeutralAnimOnly)
                {
                    sGetAnimSetNeutralAnimations(snoName.snoHandle, eActualWeaponClass, tAllAnimations);
                }
                else
                {
                    sGetAnimSetAnimations(snoName.snoHandle, eActualWeaponClass, tAllAnimations, TRUE);
                }
                uint32 nNewSize = tAllAnimations.size();
                for (uint32 i = nOldSize; i < nNewSize; ++i)
                {
                    tNeutralAnimations.push_back(tAllAnimations[i]);
                }
            }
        }


        uint32 nCount = tAllAnimations.size();
        for (uint32 i = 0; i < nCount; ++i)
        {
            int32 * pnValue = SNOs.LookupPtr(CompressedSNOName(tAllAnimations[i]));

            if (pnValue)
            {
                ++*pnValue;
            }
            else
            {
                SNOs.SetAt_Tracked(CompressedSNOName(tAllAnimations[i]), 1);
            }
        }


        SNOManager::AddSNOsTree(SNOs, tNeutralAnimations, ptFilterFunc);
    }
}
namespace SNOPreload {

static const ActorAnimationTag sdwNeutralAnimationsTag[] = {
    TAG_ACTOR_ANIMATION_NEUTRAL,
    TAG_ACTOR_ANIMATION_WALK,
    TAG_ACTOR_ANIMATION_UNALERT_NEUTRAL,
    TAG_ACTOR_ANIMATION_CONVERSATION_NEUTRAL,
    TAG_ACTOR_ANIMATION_EMOTE_IDLE,
    TAG_ACTOR_ANIMATION_EMOTE_IDLE_2,
    TAG_ACTOR_ANIMATION_EMOTE_IDLE_LONG,
    TAG_ACTOR_ANIMATION_EMOTE_IDLE_LONG_2,
    TAG_ACTOR_ANIMATION_EMOTE_IDLE_UNARMED,
    TAG_ACTOR_ANIMATION_EMOTE_IDLE_WOUNDED,
    TAG_ACTOR_ANIMATION_SPAWN_1,
    TAG_ACTOR_ANIMATION_SPAWN_2,
    TAG_ACTOR_ANIMATION_SPAWN_3,
    TAG_ACTOR_ANIMATION_SPAWN_4,
    TAG_ACTOR_ANIMATION_SPAWN_5,
    TAG_ACTOR_ANIMATION_SPAWN_6,
    TAG_ACTOR_ANIMATION_SPAWN_7,
    TAG_ACTOR_ANIMATION_SPAWN_8,
    TAG_ACTOR_ANIMATION_SPAWN_9,
    TAG_ACTOR_ANIMATION_SPAWN_10,
    TAG_ACTOR_ANIMATION_SPAWN_FROM_GROUND,
    TAG_ACTOR_ANIMATION_GETTING_SUMMONED_START,
    TAG_ACTOR_ANIMATION_GETTING_SUMMONED,
    TAG_ACTOR_ANIMATION_GETTING_SUMMONED_END,
    TAG_ACTOR_ANIMATION_GETTING_SUMMONED_03_START,
    TAG_ACTOR_ANIMATION_GETTING_SUMMONED_03,
    TAG_ACTOR_ANIMATION_GETTING_SUMMONED_03_END,
    TAG_ACTOR_ANIMATION_GETTING_SUMMONED_04_START,
    TAG_ACTOR_ANIMATION_GETTING_SUMMONED_04,
    TAG_ACTOR_ANIMATION_GETTING_SUMMONED_04_END,
    TAG_ACTOR_SELECTION_SCREEN_IDLE,
    TAG_ACTOR_ANIMATION_DEATH_01,
    TAG_ACTOR_ANIMATION_GETHIT,
    TAG_ACTOR_ANIMATION_RUN,
    TAG_ACTOR_ANIMATION_NORMAL_ATTACK_01,
    TAG_ACTOR_EMOTE_IDLE_SHUFFLE_LEFT,
    TAG_ACTOR_EMOTE_IDLE_SHUFFLE_RIGHT,
    TAG_ACTOR_EMOTE_IDLE_SHUFFLE_LEFT_180,
    TAG_ACTOR_EMOTE_IDLE_SHUFFLE_RIGHT_180,
    TAG_ACTOR_RUN_OUTRO_LEFTFOOT,
    TAG_ACTOR_ANIMATION_SPECIAL_WALK_01,
};
} // namespace SNOPreload



void SNOPreload::Impl::sGetAnimSetNeutralAnimations(
    const SNO snoAnimSet,
    const WeaponClass eWeaponClass,
    Prefetch::ChildSNOContainer & tAnimSNOs)
{








    ScopedAnimSetDefinitionLockR pdefAnimSet(snoAnimSet);

    for (int32 i = 0; i < sizeof(sdwNeutralAnimationsTag) / sizeof(sdwNeutralAnimationsTag[0]); ++i)
    {
        SNO snoAnim = TagMapGetSNO(pdefAnimSet->hTagMapAnimDefault, sdwNeutralAnimationsTag[i]);
        if (snoAnim != SNO_NONE && snoAnim != 0)
        {
            tAnimSNOs.push_back(SNOName(ANIMATION_SNO, snoAnim));
        }


        for (int32 eWeaponClassIt = 0; eWeaponClassIt < NUM_WEAPON_CLASSES; ++eWeaponClassIt)
        {
            snoAnim = TagMapGetSNO(pdefAnimSet->tWeaponAnimTagMap[eWeaponClassIt].hTagMap, sdwNeutralAnimationsTag[i]);
            if (snoAnim != SNO_NONE)
            {
                tAnimSNOs.push_back(SNOName(ANIMATION_SNO, snoAnim));
            }
        }
    }


    if (pdefAnimSet->snoParentAnimSet != SNO_NONE)
    {
        sGetAnimSetNeutralAnimations(pdefAnimSet->snoParentAnimSet, eWeaponClass, tAnimSNOs);
    }
}

























































































namespace SNOPreload {

static const uint32 sdwPowerAnimationsTag[] = {
    TAG_POWER_ANIMATION_TAG,
    TAG_POWER_ANIMATION_TAG_RUNE_A,
    TAG_POWER_ANIMATION_TAG_RUNE_B,
    TAG_POWER_ANIMATION_TAG_RUNE_C,
    TAG_POWER_ANIMATION_TAG_RUNE_D,
    TAG_POWER_ANIMATION_TAG_RUNE_E,
    TAG_POWER_ANIMATION_TAG_2,
    TAG_POWER_OFFHAND_ANIMATION_TAG,
    TAG_POWER_COMBO_ANIMATION_1,
    TAG_POWER_COMBO_ANIMATION_2,
    TAG_POWER_COMBO_ANIMATION_3,
    TAG_POWER_COMBO_ANIMATION_1_RUNE_A,
    TAG_POWER_COMBO_ANIMATION_1_RUNE_B,
    TAG_POWER_COMBO_ANIMATION_1_RUNE_C,
    TAG_POWER_COMBO_ANIMATION_1_RUNE_D,
    TAG_POWER_COMBO_ANIMATION_1_RUNE_E,
    TAG_POWER_COMBO_ANIMATION_2_RUNE_A,
    TAG_POWER_COMBO_ANIMATION_2_RUNE_B,
    TAG_POWER_COMBO_ANIMATION_2_RUNE_C,
    TAG_POWER_COMBO_ANIMATION_2_RUNE_D,
    TAG_POWER_COMBO_ANIMATION_2_RUNE_E,
    TAG_POWER_COMBO_ANIMATION_3_RUNE_A,
    TAG_POWER_COMBO_ANIMATION_3_RUNE_B,
    TAG_POWER_COMBO_ANIMATION_3_RUNE_C,
    TAG_POWER_COMBO_ANIMATION_3_RUNE_D,
    TAG_POWER_COMBO_ANIMATION_3_RUNE_E,
};
} // namespace SNOPreload

void SNOPreload::Impl::GetPowerAnimations(
    const SNO snoPower,
    const SNO snoAnimSet,
    const WeaponClass eWeaponClass,
    Prefetch::ChildSNOContainer & tAnimSNOs)
{




    ScopedPowerDefinitionLockR pdefPower(snoPower);
    ScopedAnimSetDefinitionLockR pdefAnimSet(snoAnimSet);


    for (int32 i = 0; i < sizeof(sdwPowerAnimationsTag) / sizeof(sdwPowerAnimationsTag[0]); ++i)
    {
        uint32 dwAnimTag = PowerGeneralTagGetInt(pdefPower, sdwPowerAnimationsTag[i]);

        SNO snoAnim = TagMapGetSNO(pdefAnimSet->hTagMapAnimDefault, dwAnimTag);
        if (snoAnim != SNO_NONE)
        {
            tAnimSNOs.push_back(SNOName(ANIMATION_SNO, snoAnim));
        }


        for (int32 eWeaponClassIt = 0; eWeaponClassIt < NUM_WEAPON_CLASSES; ++eWeaponClassIt)
        {
            snoAnim = TagMapGetSNO(pdefAnimSet->tWeaponAnimTagMap[eWeaponClassIt].hTagMap, dwAnimTag);
            if (snoAnim != SNO_NONE)
            {
                tAnimSNOs.push_back(SNOName(ANIMATION_SNO, snoAnim));
            }
        }
    }

}


void SNOPreload::Impl::sGetAnimSetAnimations(
    const SNO snoAnimSet,
    const WeaponClass eWeaponClass,
    Prefetch::ChildSNOContainer & tAnimSNOs,
    BOOL fWillLoadChildren)
{








    ScopedAnimSetDefinitionLockR pdefAnimSet(snoAnimSet);

    CrafterType eType = sGetCrafterTypeFromAnimSet(snoAnimSet);

    if (!fWillLoadChildren || eType == -1)
    {

        if (pdefAnimSet->hTagMapAnimDefault)
        {
            POS pos = TagMapGetStartPosition(pdefAnimSet->hTagMapAnimDefault);
            while (pos)
            {
                uint32 dwTag = TagMapGetNextTag(pdefAnimSet->hTagMapAnimDefault, pos);
                SNO snoAnim = TagMapGetSNO(pdefAnimSet->hTagMapAnimDefault, dwTag);
                if (snoAnim != SNO_NONE && snoAnim != 0)
                {
                    tAnimSNOs.push_back(SNOName(ANIMATION_SNO, snoAnim));
                }


                for (int32 eWeaponClassIt = 0; eWeaponClassIt < NUM_WEAPON_CLASSES; ++eWeaponClassIt)
                {
                    snoAnim = TagMapGetSNO(pdefAnimSet->tWeaponAnimTagMap[eWeaponClassIt].hTagMap, dwTag);
                    if (snoAnim != SNO_NONE)
                    {
                        tAnimSNOs.push_back(SNOName(ANIMATION_SNO, snoAnim));
                    }
                }
            }
        }
    }

    if (pdefAnimSet->snoParentAnimSet != SNO_NONE)
    {
        sGetAnimSetAnimations(pdefAnimSet->snoParentAnimSet, eWeaponClass, tAnimSNOs, fWillLoadChildren);
    }

}


void SNOPreload::Impl::WaitForSNOTree(BOOL fClient, const SNOGroup snoGroup, const SNO sno, Prefetch::FilterFunc ptFilterFunc, BOOL fIncludeRoot, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly)
{
    SNOManager::SNOContainer tContainer(16, NULL);
    tContainer.SetHashTableSize_Tracked(512);
    SNOManager::AddSNOTree(tContainer, SNOName(snoGroup, sno), ptFilterFunc);
    SNOName snonameReason(snoGroup, sno);

    if (fClient)
    {
        sAppendRequiredAnimations(tContainer, eWeaponClass, ptFilterFunc, fNeutralAnimOnly);
    }
    if (!fIncludeRoot)
    {
        tContainer.RemoveAt(CompressedSNOName(SNOName(snoGroup, sno)));
    }

    Blizzard::Time::Microsecond dwStartedUS = Blizzard::Time::Microseconds();
    ThreadIndex nThreadIndex = ThreadGlobals::Get().m_nThreadIndex;
    BOOL fMainThread = nThreadIndex == THREADINDEX_MAIN;
    BOOL fServerThread = nThreadIndex == THREADINDEX_SERVERWORKER;

    sWaitForRequests(fClient, sno, TRUE);
    BOOL fWaited = SNOManager::WaitForSNOs(tContainer, &snonameReason, FALSE);

    if ((fMainThread || fServerThread) && CGameLoadingScreenIsEnabled())
    {
        CGameLoadingScreenIsBlackOnly();
    }
}

























BOOL SNOPreload::Impl::IsSNOTreeLoaded(BOOL fClient, const SNOGroup snoGroup, const SNO sno, Prefetch::FilterFunc ptFilterFunc, BOOL fIncludeRoot, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly)
{







    SNOManager::SNOContainer tContainer(16, NULL);
    tContainer.SetHashTableSize_Tracked(512);
    SNOManager::AddSNOTree(tContainer, SNOName(snoGroup, sno), ptFilterFunc);

    if (fClient)
    {
        sAppendRequiredAnimations(tContainer, eWeaponClass, ptFilterFunc, fNeutralAnimOnly);
    }

    if (!fIncludeRoot)
    {
        tContainer.RemoveAt(CompressedSNOName(SNOName(snoGroup, sno)));
    }

    return SNOManager::AreSNOsLoaded(tContainer) ? TRUE : FALSE;
}







BOOL SNOPreload::Impl::IsSNOTreePending(BOOL fClient, const SNOGroup snoGroup, const SNO sno, Prefetch::FilterFunc ptFilterFunc, BOOL fIncludeRoot, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly)
{







    if (sIsRequestPending(fClient, sno))
    {
        return TRUE;
    }

    SNOManager::SNOContainer tContainer(16, NULL);
    tContainer.SetHashTableSize_Tracked(512);
    SNOManager::AddSNOTree(tContainer, SNOName(snoGroup, sno), ptFilterFunc);

    if (fClient)
    {
        sAppendRequiredAnimations(tContainer, eWeaponClass, ptFilterFunc, fNeutralAnimOnly);
    }

    if (!fIncludeRoot)
    {
        tContainer.RemoveAt(CompressedSNOName(SNOName(snoGroup, sno)));
    }

    return SNOManager::AnySNOsPending(tContainer) ? TRUE : FALSE;
}


// X/Common/SNO/SNOName.h:118 (inlined into 16 functions); SNOManager.cpp defines it the same way.
inline CompressedSNOName::operator SNOName() {
    return SNOName(snoGroup, snoHandle);
}

BOOL SNOPreload::Impl::PrefetchSNOTree(BOOL fClient, const SNOGroup snoGroup, const SNO sno, Prefetch::FilterFunc ptFilterFunc, BOOL fIncludeRoot, WeaponClass eWeaponClass, BOOL fNeutralAnimOnly, SNOFilesAsyncPriority ePriority)
{
    SNOManager::SNOContainer tContainer(16, NULL);
    tContainer.SetHashTableSize_Tracked(512);
    SNOManager::AddSNOTree(tContainer, SNOName(snoGroup, sno), ptFilterFunc);

    if (fClient)
    {
        sAppendRequiredAnimations(tContainer, eWeaponClass, ptFilterFunc, fNeutralAnimOnly);
    }
    if (!fIncludeRoot)
    {
        tContainer.RemoveAt(CompressedSNOName(SNOName(snoGroup, sno)));
    }

    BOOL fAllPrefetched = TRUE;
    POS pos = tContainer.GetStartAssoc();
    while (pos != NULL)
    {
        CompressedSNOName snoName;
        tContainer.GetNextAssocKey(pos, snoName);
        if (!Prefetch::AddToQueue(snoName, ePriority))
        {
            fAllPrefetched = FALSE;
        }
    }
    return fAllPrefetched;
}


























inline void SNOPreload::Impl::sSubmitRequest(const SNOPreload::PreloadTask & task)
{



    PreloadAsyncInfo & info = task.fClient ? s_tClientPreloadAsync : s_tServerPreloadAsync;



    Blizzard::Lock::ScopedMutex scopedLock(info.tRequestCrit);
    info.pendingPreloadRequestList.AddTail_Tracked(task);
    info.theresWorkToDoEvent.Set();














}



inline BOOL SNOPreload::Impl::sIsRequestPending(BOOL fClient, SNO sno)
{








    PreloadAsyncInfo & info = fClient ? s_tClientPreloadAsync : s_tServerPreloadAsync;




    Blizzard::Lock::ScopedMutex scopedLock(info.tRequestCrit);
    if (sno != SNO_NONE)
    {

        for (POS pos = info.pendingPreloadRequestList.GetHeadPosition(); pos != NULL;)
        {
            const PreloadTask & task = info.pendingPreloadRequestList.GetNext(pos);
            if (task.sno == sno)
            {
                return TRUE;
            }
        }




        return FALSE;
    }
    return info.pendingPreloadRequestList.Count() > 0;
}




void SNOPreload::Impl::sCancelRequests(BOOL fClient)
{






    PreloadAsyncInfo & info = fClient ? s_tClientPreloadAsync : s_tServerPreloadAsync;




    Blizzard::Lock::ScopedMutex scopedLock(info.tRequestCrit);


    while (info.pendingPreloadRequestList.Count() > 1)
    {
        info.pendingPreloadRequestList.RemoveTail();
    }

}


BOOL SNOPreload::Impl::sWaitForRequests(
    BOOL fClient,
    SNO sno,
    BOOL fAllowCancel)
{




    BOOL fWaited = FALSE;



    PreloadAsyncInfo & info = fClient ? s_tClientPreloadAsync : s_tServerPreloadAsync;




    BOOL fPending = sIsRequestPending(fClient, sno);

    Blizzard::Time::Millisecond dwCurMS = Blizzard::Time::Milliseconds();
    BOOL fMainThread = ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_MAIN;
    BOOL fCancelWait = FALSE;

    while (fPending && !fCancelWait)
    {
        info.someWorkCompletedEvent.Wait(1);
        if (fMainThread)
        {
            SNOManager::SetHasBlockedThisFrame();
            Prefetch::Update(FALSE);
            if (CGameLoadingScreenIsEnabled() && Blizzard::Time::Milliseconds() - dwCurMS > 16)
            {
                dwCurMS = Blizzard::Time::Milliseconds();
                CGameProgressLoadingScreen(TRUE);


                if (CGameLoadingScreenHasExpired() && fAllowCancel)
                {
                    fCancelWait = TRUE;
                }
            }
        }



        fPending = sIsRequestPending(fClient, sno);
        fWaited = TRUE;
    }
    return fWaited;
}



uint SNOPreload::Impl::PreloadAsyncThread(void * pPreloadAsyncInfo)
{
    PreloadAsyncInfo * ptPreloadAsyncInfo = (PreloadAsyncInfo *)pPreloadAsyncInfo;
    ThreadGlobals::InitializeForThread();
    ThreadGlobals::Get().m_nThreadIndex = THREADINDEX_ASYNCPRELOAD;


    do
    {
        ptPreloadAsyncInfo->theresWorkToDoEvent.WaitForever();


        ptPreloadAsyncInfo->someWorkCompletedEvent.Wait(0);

        const PreloadTask * ptTask = NULL;
        {
            Blizzard::Lock::ScopedMutex scopedLock(ptPreloadAsyncInfo->tRequestCrit);
            if (ptPreloadAsyncInfo->pendingPreloadRequestList.Count() > 0)
            {
                ptTask = &ptPreloadAsyncInfo->pendingPreloadRequestList.GetHead();
            }
        }

        while (ptTask)
        {
            switch (ptTask->eType)
            {
            case PRELOADTASKTYPE_ADD_TREE:
                sAddSNOTree(*ptTask); break;

            case PRELOADTASKTYPE_CHANGE_TREE:
                sChangeSNOTree(*ptTask); break;

            case PRELOADTASKTYPE_REMOVE_TREE:
                sRemoveSNOTree(*ptTask); break;

            }



            {
                Blizzard::Lock::ScopedMutex scopedLock(ptPreloadAsyncInfo->tRequestCrit);
                if (ptPreloadAsyncInfo->pendingPreloadRequestList.Count() > 0)
                {
                    ptPreloadAsyncInfo->pendingPreloadRequestList.RemoveHead();
                }

                ptTask = NULL;
                if (ptPreloadAsyncInfo->pendingPreloadRequestList.Count() > 0)
                {
                    ptTask = &ptPreloadAsyncInfo->pendingPreloadRequestList.GetHead();
                }
            }



        }

        ptPreloadAsyncInfo->someWorkCompletedEvent.Set();
    }
    while (!ptPreloadAsyncInfo->fTerminateAsyncThread);
    ThreadGlobals::DisposeForThread();
    return 0;
}





























































template <>
inline uint32 XHashKey<const SNOPreload::Impl::SkillInfo &>(const SNOPreload::Impl::SkillInfo & key)
{
    return (key.eWeaponClass << 27) | (key.snoAnimSet << 3) | key.snoPower;
}

template <>
inline uint32 XHashKey<const SNOPreload::Impl::CrafterInfo &>(const SNOPreload::Impl::CrafterInfo & key)
{
    return (key.eType << 30) | (key.dwRank << 24) | key.snoAnimSet;
}
// DWARF decl 7516; the ItemInfo maps hash the handle.
template <>
inline uint32 XHashKey<const SNOPreload::Impl::ItemInfo &>(const SNOPreload::Impl::ItemInfo & key) { return key.snoHandle; }


