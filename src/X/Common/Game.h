// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Game.h
// Full reference layout (build/dwarf/headers/X/Common/Game.h). The free function declarations
// at the end are the external API that X/Common/Game.cpp defines (the reference .cpp lists them).

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Client/Gfx/GfxCommon.h"
// The reference also lists ../Client/OnlineService/OnlineServiceClientBase.h; it is not included
// here because it reaches CGame.h (via VisualInventory.h and RActors.h), which includes this file.
#include "Act.h"
#include "Attributes/FastAttributeStructs.h"
#include "CommonTypes.h"
#include "GameBalance.h"
#include "GameTemplate.h"
#include "OnlineService/Callback/Callback_Impl.h"
#include "OnlineService/OnlineService.h"
#include "SNO/SNOFiles.h"
#include "ThreadGlobals.h"
#include "../Sigma/DataStructures/XBitArray.h"
#include "../Sigma/Math/Shape.h"
#include "../Sigma/RGBAColor.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/Stopwatch.h"
#include "../Sigma/Unicode.h"

struct AppearanceGlobals;
struct CGame;
namespace Console { class Achievements; }
namespace Console { class ClientAchievementsInterface; }
namespace D3 { namespace OnlineService { class GameCreateParams; } }
struct DefinitionPrefetchData;
struct Diablo3NDBAppGlobals;
struct Diablo3StorageAppGlobals;
struct FastAttribAppGlobals;
struct FontGlobals;
struct GameBalanceAppGlobals;
class GameCommonData;
struct GameDebugGlobals;
struct GlobalDefinitionAppGlobals;
struct GlobalSNOEntry;  // X/Common/GlobalSNO.h
class GroupDefinition;  // X/Common/SNO/SNOGroups.h
struct HandicapLevel;  // X/Common/GameBalance.h (not adopted yet)
struct InheritedTagGlobals;
struct ItemAppGlobals;
struct LuaScriptAppGlobals;
struct NetLoungeGlobals;
struct NetworkGlobals;
struct ProfUIAppGlobals;
struct QuestAppGlobals;
struct ReplayTestGlobals;
struct ScriptEventAppGlobals;
struct TagMapAppGlobals;
struct UIAppGlobals;
struct UIChatEditGlobals;
struct UIConsoleEditGlobals;
struct UIID;  // X/Client/UserInterface/UIID.h
class XMemoryAllocatorInterface;  // X/Sigma/DataStructures/XMemoryAllocatorInterface.h
struct XServerAppGlobals;

typedef int32 XLocale;

enum {
    INITIAL_UPDATE_COUNT_DEFAULT = 1,
};

enum {
    GAME_CLOCK_TICKS_PER_SECOND = 3000,
    GAME_CLOCK_TICKS_PER_MACHINE_TICK = 3,
};

enum {
    MINIMUM_HANDICAP_LEVEL_FOR_TABLE = 0,
};

enum DebugFadeMode {
    DFM_DEFAULT = 0,
    DFM_FADE_ALL = 1,
    DFM_FADE_NEVER = 2,
    NUM_DEBUG_FADE_MODES = 3,
};

enum DebugSoundMode {
    DSM_OFF = 0,
    DSM_MINIMAL = 1,
    DSM_FULL = 2,
    NUM_DEBUG_SOUND_MODES = 3,
};

enum DebugDisplayMode {
    DDM_DISABLED = 0,
    DDM_FPS_SIMPLE = 1,
    DDM_FPS = 2,
    DDM_FPS_PROGRAMMER = 3,
    DDM_FPS_QA = 4,
    DDM_SOUND_STATUS = 5,
    DDM_MUSIC_STATUS = 6,
    DDM_PERFCOUNTERS = 7,
    DDM_PHYSICS_PROFILE = 8,
    DDM_PHYSICS_COUNTERS = 9,
    DDM_WHYSLOW = 10,
    DDM_PROF = 11,
    DDM_SCENES = 12,
    DDM_NETSTAT = 13,
    DDM_NETMESSAGES = 14,
    DDM_CLOCKS = 15,
    DDM_SERVER_UPDATES = 16,
    DDM_PREFETCH = 17,
    DDM_MEMORY_USAGE0 = 18,
    DDM_MEMORY_USAGE1 = 19,
    DDM_DEFINITION_STATS = 20,
    DDM_GAMESTATS = 21,
    DDM_SERVERSTATS = 22,
    DDM_CLIENTSTATS = 23,
    DDM_APPEARANCELOCKS = 24,
    DDM_SCENE_DEACTIVATION_REASONS = 25,
    DDM_DEBUG_ACTOR_TOOLTIP = 26,
    DDM_CONVERSATIONS = 27,
    DDM_KNOWN_LIST = 28,
    DDM_STREAMING = 29,
    DDM_BLOCKING_READS = 30,
    DDM_PATCH_ON_DEMAND = 31,
    DDM_LOOT_REST_BONUS = 32,
    DDM_IO_STATS = 33,
    DDM_SNO_MANAGER = 34,
    DDM_HEAP_STATS = 35,
    DDM_OCCLUSION_HIGHLIGHTING = 36,
};

enum {
    DRM_DISABLED = 0,
    DRM_AO = 1,
    DRM_AO_SM = 2,
    DRM_GRAYBOX = 3,
    DRM_GRAYSCALE = 4,
    DRM_MIPMAPS = 5,
    DRM_LIGHTMAP_MIPMAPS = 6,
    DRM_LIGHTMAP = 7,
    DRM_ENVMAP = 8,
    DRM_GLOSS = 9,
    DRM_DIFFUSE_LIGHTING = 10,
    DRM_SPECULAR_LIGHTING = 11,
    DRM_IRRADIANCE_LIGHTING = 12,
    DRM_DEPTH_COMPLEXITY = 13,
    DRM_PMA_DEPTH_COMPLEXITY = 14,
    DRM_ACTOR_BATCHES = 15,
    DRM_FLATSHADED = 16,
    DRM_WIREFRAME = 17,
    DRM_RETRO = 18,
    NUM_DEBUG_RENDER_MODES = 19,
};

typedef int32 DebugRenderMode;

enum DebugColorBlindMode {
    COLORBLIND_OFF = 0,
    COLORBLIND_SIM_PROTANOPIA = 1,
    COLORBLIND_SIM_PROTANOMALY = 2,
    COLORBLIND_SIM_DEUTERANOPIA = 3,
    COLORBLIND_SIM_DEUTERANOMALY = 4,
    COLORBLIND_SIM_TRITANOPIA = 5,
    COLORBLIND_SIM_TRITANOMALY = 6,
    COLORBLIND_SIM_ACHROMATOPSIA = 7,
    COLORBLIND_SIM_ACHROMATOMALY = 8,
    COLORBLIND_DALTONIZE_PROTANOPIA = 9,
    COLORBLIND_DALTONIZE_DEUTERANOPIA = 10,
    COLORBLIND_DALTONIZE_TRITANOPIA = 11,
    NUM_COLORBLIND_MODES = 12,
};

enum {
    APP_GLOBAL_NET_TRACE_BIT = 0,
    APP_GLOBAL_NET_GRAPH_BIT = 1,
    APP_GLOBAL_NET_COLLECT_DATA_BIT = 2,
    APP_GLOBAL_AI_DISABLED_BIT = 3,
    APP_GLOBAL_AI_BLIND_BIT = 4,
    APP_GLOBAL_AI_BLIND_TO_PLAYERS_BIT = 5,
    APP_GLOBAL_ALL_HITS_CRIT_BIT = 6,
    APP_GLOBAL_FADE_ALL_BIT = 7,
    APP_GLOBAL_NO_FADE_BIT = 8,
    APP_GLOBAL_SNAP_WEATHER_BIT = 9,
    APP_GLOBAL_TRACE_HESITATION_BIT = 10,
    APP_GLOBAL_TRACE_STAT_BIT = 11,
    APP_GLOBAL_TRACE_IMPULSES_BIT = 12,
    APP_GLOBAL_NO_PREFETCH_BIT = 13,
    APP_GLOBAL_DISPLAY_ALL_SKILLS = 14,
    APP_GLOBAL_DISPLAY_ITEM_AFFIXES = 15,
    APP_GLOBAL_SNAP_TO_RENDERED_MESH = 16,
    APP_GLOBAL_TRACE_SPECIAL_MARKER_CREATION = 17,
    APP_GLOBAL_TRACE_SPAWNER_BRAIN = 18,
    APP_GLOBAL_ENABLE_STATS_GATHERING_IN_AXE_GAMES = 19,
    APP_GLOBAL_FLYTHROUGH_BIT = 20,
    APP_GLOBAL_INSTANT_RESURRECTION_BIT = 21,
    APP_GLOBAL_FORCE_SPAWN_ALL_GIZMO_LOCATIONS = 22,
    APP_GLOBAL_DISPLAY_ITEM_ATTRIBUTES = 23,
    APP_GLOBAL_TRACE_PERF = 24,
    APP_GLOBAL_ALWAYS_PLAY_GETHIT_BIT = 25,
    APP_GLOBAL_FORCE_WALK_BIT = 26,
    APP_GLOBAL_LABEL_TRACE_BIT = 27,
    APP_REVEAL_MINIMAP_SUBSCENE_BIT = 28,
    APP_GLOBAL_BATTLENET_VALIDATION_DISABLED_BIT = 29,
    APP_GLOBAL_8_WAY_ROTATION_BIT = 30,
    APP_GLOBAL_REMOTE_EFFECTS_BIT = 31,
    APP_GLOBAL_COUNT = 32,
};

enum {
    APP_GLOBAL2_ALL_BANNER_LOCS_BIT = 0,
    APP_GLOBAL2_SHOW_RUNETYPES_BIT = 1,
    APP_GLOBAL2_ALWAYS_PLAY_KNOCKBACK_BIT = 2,
    APP_GLOBAL2_ALL_HITS_OVERKILL_BIT = 3,
    APP_GLOBAL2_NO_MULTI_CLIENT_UPDATE_BIT = 4,
    APP_GLOBAL2_NO_SPU_PARTICLE_UPDATE_BIT = 5,
    APP_GLOBAL2_NO_SPU_TRAIL_UPDATE_BIT = 6,
    APP_GLOBAL2_NO_SPU_CLOTH_UPDATE_BIT = 7,
    APP_GLOBAL2_NO_SPU_PARTICLE_RENDER_UPDATE_BIT = 8,
    APP_GLOBAL2_USE_CONSOLE_OBSERVER_BIT = 9,
    APP_GLOBAL2_SYNC_RENDER_BIT = 10,
    APP_GLOBAL2_ENABLE_WORLD_PRELOADING_IN_AXE_BIT = 11,
    APP_GLOBAL2_FORCE_NEW_SKILL_UI_BIT = 12,
    APP_GLOBAL2_DONT_IDENTIFY_RARES_BIT = 13,
    APP_GLOBAL2_AVENGER_ALWAYS_CREATE_BIT = 14,
    APP_GLOBAL2_AVENGER_ALWAYS_SPAWN_BIT = 15,
    APP_GLOBAL2_AVENGER_SEND_TO_SELF_BIT = 16,
    APP_GLOBAL2_AVENGER_IGNORE_DISTRO_BIT = 17,
    APP_GLOBAL2_AVENGER_EASY_SPAWN_BIT = 18,
    APP_GLOBAL2_AVENGER_QUICK_SPAWN_BIT = 19,
    APP_GLOBAL2_ALL_HITS_CRUSHING_BLOW_BIT = 20,
    APP_GLOBAL2_DISPLAY_ITEM_TARGETED_CLASS = 21,
    APP_GLOBAL2_DONT_THROTTLE_FLOATING_NUMBERS = 22,
    APP_GLOBAL2_CONVERSATION_VAR_TRACE_BIT = 23,
    APP_GLOBAL2_BATTLENET_FIRST_FLOW_ENABLED_BIT = 24,
    APP_GLOBAL2_BATTLENET_AH_READ_ONLY_ENABLED_BIT = 25,
    APP_GLOBAL2_PLAYER_RUN_REQUESTED_BIT = 26,
    APP_GLOBAL2_PLAYER_GOD_REQUESTED_BIT = 27,
    APP_GLOBAL2_PLAYER_EASYKILL_REQUESTED_BIT = 28,
    APP_GLOBAL2_DEBUG_VALID_AFFIXES = 29,
    APP_GLOBAL2_DONT_RANDOMIZE_LOADSCREEN_TIPS = 30,
    APP_GLOBAL2_FORCED_PROCS = 31,
    APP_GLOBAL2_COUNT = 32,
};

enum {
    APP_GLOBAL3_LOW_HEALTH_SCREEN_GLOW_DISABLED = 0,
    APP_GLOBAL3_CONSOLE_COLLECTORS_EDITION = 1,
    APP_GLOBAL3_RETRO_LIGHTING_DISABLED = 2,
    APP_GLOBAL3_PLAYER_MUTE_REQUESTED_BIT = 3,
    APP_GLOBAL3_ANCIENTS_ALWAYS_ROLL_PRIMAL = 4,
    APP_GLOBAL3_ANCIENT_ROLLS_WILL_ALWAYS_SUCCEED = 5,
    APP_GLOBAL3_NFP_FIGURE_DEFAULT = 6,
    APP_GLOBAL3_NFP_FIGURE_GOBLIN = 7,
    APP_GLOBAL3_COUNT = 8,
};

enum {
    APP_DRAW_BONE_BOUNDS_BIT = 0,
    APP_DRAW_BONES_BIT = 1,
    APP_DRAW_ACTOR_COLLISION_BIT = 2,
    APP_DRAW_ACTOR_BOUNDING_SPHERE = 3,
    APP_DRAW_ACTOR_PARTICLE_SPHERE = 4,
    APP_DRAW_ACTOR_RADII_BIT = 5,
    APP_DRAW_ACTOR_DEBUG_RADIUS_BIT = 6,
    APP_DRAW_SHAKE_RADII_BIT = 7,
    APP_DRAW_HARDPOINTS_BIT = 8,
    APP_DRAW_CONTACT_OFFSETS_BIT = 9,
    APP_DRAW_ACTOR_COLLISION_CAPSULE_BIT = 10,
    APP_DRAW_LIGHT_DEBUG_BIT = 11,
    APP_DRAW_ACTOR_POWER_SOURCE_BIT = 12,
    APP_DRAW_FORCE_SW_CURSOR_BIT = 13,
    APP_DRAW_DAMAGE_NUMBERS_BIT = 14,
    APP_DRAW_MICROPHONE_BIT = 15,
    APP_DRAW_NO_HOLODECK_GRID_BIT = 16,
    APP_DRAW_NO_FOG_BIT = 17,
    APP_DRAW_NO_ACTOR_SHADOWS_BIT = 18,
    APP_DRAW_NO_SCENES_BIT = 19,
    APP_DRAW_NO_ACTORS_BIT = 20,
    APP_DRAW_NO_PARTICLES_BIT = 21,
    APP_DRAW_NO_UI_BIT = 22,
    APP_DRAW_NO_UI_BUT_ITEM_TAGS_BIT = 23,
    APP_DRAW_NO_SKY_BIT = 24,
    APP_DRAW_NO_POSTFX_BIT = 25,
    APP_DRAW_NO_POSTFX_DISTORTION_BIT = 26,
    APP_DRAW_NO_POSTFX_BLOOM_BIT = 27,
    APP_DRAW_NO_POSTFX_OUTLINE_BIT = 28,
    APP_DRAW_NO_TRAILS_BIT = 29,
    APP_DRAW_NO_ROPE_BIT = 30,
    APP_DRAW_SERVER_OBJECT_HIGHLIGHT_BIT = 31,
    APP_DRAW_CLIENT_OBJECT_HIGHLIGHT_BIT = 32,
    APP_DRAW_FULL_BRIGHT_BIT = 33,
    APP_DRAW_WALKABLE_SUBOJBECTS_BIT = 34,
    APP_DRAW_WALKABLE_SUBOJBECTS_ONLY_BIT = 35,
    APP_DRAW_SURFACED_SUBOBJECTS_BIT = 36,
    APP_DRAW_SHADOWABLE_SUBOBJECTS_BIT = 37,
    APP_DRAW_COLLIDABLE_SUBOBJECTS_BIT = 38,
    APP_DRAW_FX_RECEIVING_SUBOBJECTS_BIT = 39,
    APP_DRAW_INDOOR_SUBOBJECTS_BIT = 40,
    APP_DRAW_HILIGHTED_SUBOBJECTS_ONLY_BIT = 41,
    APP_DRAW_LEVEL_BIT = 42,
    APP_DRAW_ITEM_LEVEL_BIT = 43,
    APP_DRAW_DISABLE_LOS_BIT = 44,
    APP_DRAW_DISPLAY_NAMES_BIT = 45,
    APP_DRAW_DISPLAY_MONSTER_NAMES_BIT = 46,
    APP_DRAW_DISPLAY_PARTICLE_NAMES_BIT = 47,
    APP_DRAW_DISPLAY_SHAKE_NAMES_BIT = 48,
    APP_DRAW_DISPLAY_ANIMATIONS_BIT = 49,
    APP_DRAW_DISPLAY_GROUP_BIT = 50,
    APP_DRAW_DRLG_INFO_BIT = 51,
    APP_DRAW_PHYSICS_BOUNDS_BIT = 52,
    APP_DRAW_PHYSICS_MASS_BIT = 53,
    APP_DRAW_EXPLOSION_BOUNDS_BIT = 54,
    APP_DRAW_RAGDOLL_VELOCITY_BIT = 55,
    APP_DRAW_SPHERETREE_BIT = 56,
    APP_DRAW_NO_PLAYER_ACTOR_BIT = 57,
    APP_DRAW_NO_MONSTER_ACTORS_BIT = 58,
    APP_DRAW_NO_GIZMO_ACTORS_BIT = 59,
    APP_DRAW_CLOTH_BIT = 60,
    APP_DRAW_ROPE_DEBUG_BIT = 61,
    APP_DRAW_TRIGGERS_BIT = 62,
    APP_DRAW_CHECKPOINTS_BIT = 63,
    APP_DRAW_MONSTER_AFFIXES_BIT = 64,
    APP_DRAW_SHOW_DPS_BIT = 65,
    APP_DRAW_SHOW_HPS_BIT = 66,
    APP_DRAW_SHOW_GETHIT_BIT = 67,
    APP_DRAW_SHOW_BUFFS_BIT = 68,
    APP_DRAW_SHOW_CC_RESIST_BIT = 69,
    APP_DRAW_SHOW_CURSORS_BIT = 70,
    APP_DRAW_SHOW_PAYLOADS_BIT = 71,
    APP_DRAW_COUNT_PAYLOADS_BIT = 72,
    APP_DRAW_SHOW_POWERS_BIT = 73,
    APP_DRAW_PATH_NODES_BIT = 74,
    APP_DRAW_ATTACHMENT_COUNT_BIT = 75,
    APP_DRAW_PLAYER_ENEMY_LIST_BIT = 76,
    APP_DRAW_AI_QUEUE_BIT = 77,
    APP_DRAW_CURSOR_LOCATION_BIT = 78,
    APP_DRAW_NO_DRLG_BORDER_SCENES_BIT = 79,
    APP_DRAW_DYNAMIC_LIGHT_COUNT_BIT = 80,
    APP_DRAW_PLAYER_FRUSTUM_BIT = 81,
    APP_DRAW_TILE_PARTICLE_GRIDS_BIT = 82,
    APP_DRAW_GREENSCREEN_BIT = 83,
    APP_DRAW_SCRIPT_BIT = 84,
    APP_DRAW_GIZMO_LOCATION_TYPES_BIT = 85,
    APP_DRAW_LOW_POLY_SHADOWS_BIT = 86,
    APP_DRAW_EDITMODE_PMA_BIT = 87,
    APP_DRAW_DEBUG_CALLOUTS_BIT = 88,
    APP_DRAW_DEBUG_QUESTS_BIT = 89,
    APP_DRAW_CLICK_EFFECT_BIT = 90,
    APP_DRAW_HIGH_RES_SCREENSHOT_BIT = 91,
    APP_DRAW_CONTROLLER_AUTO_TARGETING = 92,
    APP_DRAW_DEBUG_SAFE_ZONE_BIT = 93,
    APP_DRAW_BAD_UIID_BIT = 94,
    APP_DRAW_GLOBAL_CACHE_SIZE_WARNING_BIT = 95,
    APP_DRAW_HWCLASS_UI_OUTLINES_BIT = 96,
    APP_DRAW_HWCLASS_DISTORTIONS_BIT = 97,
    APP_DISABLE_RACTOR_HIGHLIGHT_BIT = 98,
    APP_DRAW_FPS_BIT = 99,
    APP_DRAW_DISPLAY_ATTRIBUTE_BIT = 100,
    APP_DRAW_SHOW_AGGRO_BIT = 101,
    APP_DRAW_TEXTURE_MEMORY_BIT = 102,
    APP_DRAW_SHOW_DAMAGE_DONE_BIT = 103,
    APP_DRAW_DEPTH_PREPASS_BIT = 104,
    APP_DRAW_NO_CONSOLE_ICONS_BIT = 105,
    APP_DRAW_AVENGER_INFO = 106,
    APP_DRAW_KANAIS_RECIPES_DEBUG_BIT = 107,
    APP_DRAW_KANAIS_HIDDEN_RECIPES_DEBUG_BIT = 108,
    APP_DRAW_NO_CLOTH_BIT = 109,
    APP_DRAW_NO_ANISOTROPIC_BIT = 110,
    APP_DRAW_NO_MARKERS_HIDDEN_IN_LOOT_RUNS_BIT = 111,
    APP_DRAW_NO_STATE_BLOCK_BIT = 112,
    APP_DRAW_NO_MULTI_RENDER_WORKER_BIT = 113,
    APP_DRAW_SSAO_BIT = 114,
    APP_DRAW_SSAO_BLUR_BIT = 115,
    APP_DRAW_SSAO_DEBUG_BIT = 116,
    APP_DRAW_SSAO_NORMAL_DEBUG_BIT = 117,
    APP_DRAW_FLAT_NORMAL_MAP_BIT = 118,
    APP_DRAW_AI_ALL_FRIENDLY_BIT = 119,
    APP_DRAW_AI_ALL_ENEMIES_BIT = 120,
    APP_DRAW_NO_VARIABLE_RES_BIT = 121,
    APP_DRAW_VARIABLE_RES_DEBUG_BIT = 122,
    APP_DRAW_ANIMATION_MEMORY_POOL_BIT = 123,
    APP_DRAW_FLAG_COUNT = 124,
};

enum GameCreateParams {
    GAMECREATEPARAM_ACTOR_VIEW = 0,
    GAMECREATEPARAM_RECORD_SERVER_REPLAY = 1,
    GAMECREATEPARAM_USE_RECORDED_AV_FRUSTUM = 2,
    GAMECREATEPARAM_PLAY_SERVER_REPLAY = 3,
    GAMECREATEPARAM_PLAY_MULTI_SERVER_REPLAY = 4,
    GAMECREATEPARAM_REPLAY_SERVER_LOAD_TEST = 5,
    GAMECREATEPARAM_REPLAY_SERVER_CPU_TEST = 6,
    GAMECREATEPARAM_PVP_BIT_DEPRECATED = 7,
    GAMECREATEPARAM_PUBLIC_BIT = 8,
    GAMECREATEPARAM_STORY_BIT = 9,
    GAMECREATEPARAM_REJOINING_BIT = 10,
    GAMECREATEPARAM_AUTO_LOAD_BIT = 11,
    GAMECREATEPARAM_PVP_FORCE_PLAYERS_TO_TEAM0_DEPRECATED = 12,
    GAMECREATEPARAM_PVP_FORCE_PLAYERS_TO_TEAM1_DEPRECATED = 13,
    GAMECREATEPARAM_PVP_ROUNDS_BIT_DEPRECATED = 14,
    GAMECREATEPARAM_UTILITY = 15,
    GAMECREATEPARAM_HARDCORE = 16,
    GAMECREATEPARAM_TRIAL = 17,
    GAMECREATEPARAM_SEASON = 18,
    GAMECREATEPARAM_AUTO_RIFT = 19,
    GAMECREATEPARAM_NUM_BITS = 20,
};

enum GameParts {
    GAME_PARTS_NONE = 0,
    GAME_PARTS_CLIENT = 1,
    GAME_PARTS_CLIENT_SERVER = 2,
    GAME_PARTS_CLIENT_NETWORK = 3,
    GAME_PARTS_CLIENT_SERVER_NETWORK = 4,
    GAME_PARTS_BATTLENET_NETWORK = 5,
    GAME_PARTS_HIDDEN_SERVER = 6,
};

enum StringDebugMode {
    STRINGDEBUG_OFF = 0,
    STRINGDEBUG_SNO_AND_LABEL = 1,
    STRINGDEBUG_SNO = 2,
    STRINGDEBUG_LABEL = 3,
};

enum AODebugMode {
    AODEBUG_NONE = 0,
    AODEBUG_NEITHER = 1,
    AODEBUG_CAST = 2,
    AODEBUG_RECEIVE = 3,
    AODEBUG_CAST_AND_RECEIVE = 4,
};

enum {
    SST_SHUTDOWN = 0,
    SST_RESTART = 1,
    SST_SEASON_ENDED = 2,
    SST_CHALLENGE_RIFTS_DISABLED = 3,
    SST_COUNT = 4,
};

enum {
    ANNIVERSARY_INVALID = -1,
    ANNIVERSARY_PRECURSOR_START = 0,
    ANNIVERSARY_EVENT_START = 1,
    ANNIVERSARY_EVENT_END = 2,
    CURRENT_DAY = 3,
};

typedef int32 AnniversaryEventStatus;

typedef int32 GameMode;

// Game.h:572. The DWARF marks GameParams::GameParams() artificial: it is the implicit default
// constructor (it only runs OnlineService::GameId::GameId() for tGameId), so none is declared.
struct GameParams {  // size 0x3c0
    BOOL bValid;  // 0x00
    SNO snoInitialWorld;  // 0x04
    SNO snoInitialQuest;  // 0x08
    int32 nInitialQuestStepUID;  // 0x0c
    SNO snoInitialHero;  // 0x10
    BOOL bResumeFromSave;  // 0x14
    uint32 dwSeed;  // 0x18
    GameType eGameType;  // 0x1c
    uint32 dwCreationFlags;  // 0x20
    uint32 dwInitialUpdateCount;  // 0x24
    uint32 dwMaxGameTime;  // 0x28
    GameMode eMode;  // 0x2c
    GameParts nGameParts;  // 0x30
    int32 nMonsterLevel;  // 0x34
    int32 nHandicapLevel;  // 0x38
    Act eAct;  // 0x3c
    GameTemplate tTemplate;  // 0x40
    uint32 uEntranceGUIDOverride;  // 0x58
    CHAR szServerAddress[260];  // 0x5c
    uint32 idGameConnection;  // 0x160
    Act ePreferredAdventureAct;  // 0x164
    uint64 arFreeKeyTieredRiftParticipatingHeroID[4];  // 0x168
    CHAR szReplayFileName[260];  // 0x188
    CHAR szGameName[260];  // 0x28c
    OnlineService::GameId tGameId;  // 0x390
    OnlineService::HeroId uHeroId;  // 0x3a8
    uint64 uServerAuthToken;  // 0x3b0
    SGameID idSGame;  // 0x3b8
    void Export(D3::OnlineService::GameCreateParams & tGameParams, LPCSTR szGameTag) const;  // Game.cpp:2327
};
static_assert(sizeof(GameParams) == 0x3c0, "GameParams");

struct ActiveGameCommonDataGlobals {  // size 0x10
    GameCommonData * ptActiveGCD;  // 0x00
    BOOL bClientActive;  // 0x08
};
static_assert(sizeof(ActiveGameCommonDataGlobals) == 0x10, "ActiveGameCommonDataGlobals");

struct GameClock {  // size 0x20
    uint32 dwLastGameClockTime;  // 0x00
    uint32 dwClockTime;  // 0x04
    uint32 dwUsedClockTime;  // 0x08
    uint32 dwUsedGameTime;  // 0x0c
    uint32 dwLastTicksPerSec;  // 0x10
    BOOL bDebugLagEnable;  // 0x14
    uint32 dwDebugLagStartGameClockTime;  // 0x18
    uint32 dwDebugLagEndGameClockTime;  // 0x1c
};
static_assert(sizeof(GameClock) == 0x20, "GameClock");

struct CollisionDebugData {  // size 0x100c
    SNO snoActorToTrack;  // 0x00
    Shape::Material eMaterialToTrack;  // 0x04
    int32 nNumSamples;  // 0x08
    float arSamples[1024];  // 0x0c
};
static_assert(sizeof(CollisionDebugData) == 0x100c, "CollisionDebugData");

struct ConstraintDebugData {  // size 0x44
    SNO snoActorToTrack;  // 0x00
    char szConstraintToTrack[64];  // 0x04
};
static_assert(sizeof(ConstraintDebugData) == 0x44, "ConstraintDebugData");

typedef blz::vector<GlobalSNOEntry, blz::allocator<GlobalSNOEntry> > GlobalSNOArray;

class FrameTimer {  // size 0x10
public:
    FrameTimer();
    void Start();
    void Stop();
    void Reset();
    Blizzard::Time::Microsecond Get();
    class IdleScope {  // size 0x8
    public:
        IdleScope(FrameTimer & tTimer);
        ~IdleScope();
    private:
        FrameTimer & m_tTimer;  // 0x00
    };
private:
    Blizzard::Time::Microsecond m_tStart;  // 0x00
    Blizzard::Time::Microsecond m_tAccum;  // 0x08
};
static_assert(sizeof(FrameTimer) == 0x10, "FrameTimer");

class FrameTimerHistory {  // size 0x18
public:
    enum Period {
        PERIOD_SINGLE = 0,
        PERIOD_SHORT = 1,
        PERIOD_LONG = 2,
        PERIOD_COUNT = 3,
    };
public:
    FrameTimerHistory();
    void Append(const Blizzard::Time::Microsecond tTime);
    void Reset();
    Blizzard::Time::Microsecond Get(const FrameTimerHistory::Period period);
private:
    Blizzard::Time::Microsecond m_atTimes[3];  // 0x00
};
static_assert(sizeof(FrameTimerHistory) == 0x18, "FrameTimerHistory");

// Game.h:940. The DWARF marks AppGlobals::AppGlobals() artificial: it is the implicit default
// constructor (the member constructors XBitArray, Stopwatch, FrameTimer, FrameTimerHistory and
// FastAttribKey are what _GLOBAL__sub_I_Game.cpp inlines), so none is declared.
struct AppGlobals {  // size 0x23a8
    BOOL fInitialized;  // 0x00
    BOOL fAppExitEnabled;  // 0x04
    int32 nGameGfxInitCount;  // 0x08
    uint32 m_dwTicksPerSec;  // 0x0c
    CGame * ptCGameActive;  // 0x10
protected:
    CGame * ptCGamePlaying;  // 0x18
    // Game.cpp:4671 and 4694 store ptCGamePlaying directly; clang records no DW_TAG_friend,
    // so how the original granted access is not known.
    friend void GameGlobalsInit();
    friend void GameGlobalsDispose();
public:
    void PlayingCGameInit();  // line 967
    CGame * PlayingCGameGet() {  // line 969; the body is the one copy inlined into AppIdle
        return ptCGamePlaying;
    }
    void SetTicksPerSec(uint32 dwTicksPerSec);  // line 983, defined in Game.cpp:5421
    uint32 GetTicksPerSec() const;  // line 986, defined in Game.cpp:5430
    GameClock tSGameClock;  // 0x20
    uint32 dwDebugFlags;  // 0x40
    uint32 dwDebugFlags2;  // 0x44
    uint32 dwDebugFlags3;  // 0x48
    XBitArray<124> tDrawFlags;  // 0x4c
    HardwareClass eHWClassOverride;  // 0x5c
    DebugRenderMode eDebugRenderMode;  // 0x60
    DebugDisplayMode eDebugDisplayMode;  // 0x64
    DebugFadeMode eDebugFadeMode;  // 0x68
    DebugSoundMode eDebugSoundMode;  // 0x6c
    DebugColorBlindMode eDebugColorBlindMode;  // 0x70
    XBitArray<27> tHideRenderLayer;  // 0x74
    int32 nDRLGSeedOverride;  // 0x78
    int32 nSubSceneOverride;  // 0x7c
    float flSpeedHack;  // 0x80
    float flDebugDrawRadius;  // 0x84
    uint32 dwDisplayAnimLeaf;  // 0x88
    SNO snoPVPWorldOverride;  // 0x8c
    uint32 dwMinFrameMs;  // 0x90
    uint32 dwFrameCounter;  // 0x94
    float flParticleScalar;  // 0x98
    float flExplosionMagnitude;  // 0x9c
    uint32 uPerfTraceInterval;  // 0xa0
    uint32 uLastPerfTrace;  // 0xa4
    int32 nTraceCategoryPage;  // 0xa8
    CollisionDebugData tCollisionDebugData;  // 0xac
    ConstraintDebugData tConstraintDebugData;  // 0x10b8
    FastAttribKey tKeyDebugAttrib;  // 0x10fc
    GlobalSNOArray * ptGlobalSNOs;  // 0x1100
    GameClock tAppClock;  // 0x1108
    uint32 dwTicksSinceAppStart;  // 0x1128
    BOOL bInAppIdle;  // 0x112c
    BOOL bInServerIdle;  // 0x1130
    GameParts nGameParts;  // 0x1134
    uint32 dwGameCreateParamFlags;  // 0x1138
    Stopwatch tStartTimeStopwatch;  // 0x1140
    StringDebugMode eStringDebugMode;  // 0x1148
    AODebugMode eAODebugMode;  // 0x114c
    BOOL fDoScheduledPrecache;  // 0x1150
    uint32 uSleepingReasons;  // 0x1154
    BOOL bAllowSleeping;  // 0x1158
    BOOL fFrameOut;  // 0x115c
    CHAR szFrameOutPath[4096];  // 0x1160
    SNO snoForceBounty;  // 0x2160
    Act eForcedDefaultAct;  // 0x2164
    TagMapAppGlobals * ptTagMapAppGlobals;  // 0x2168
    InheritedTagGlobals * ptInheritedTagGlobals;  // 0x2170
    GameBalanceAppGlobals * ptGBAppGlobals;  // 0x2178
    XServerAppGlobals * ptXServerAppGlobals;  // 0x2180
    NetLoungeGlobals * ptNetLoungeGlobals;  // 0x2188
    NetworkGlobals * ptNetworkGlobals;  // 0x2190
    ReplayTestGlobals * ptReplayTestGlobals;  // 0x2198
    FastAttribAppGlobals * ptFastAttribGlobals;  // 0x21a0
    Diablo3StorageAppGlobals * ptDiablo3StorageAppGlobals;  // 0x21a8
    Diablo3NDBAppGlobals * ptDiablo3NDBAppGlobals;  // 0x21b0
    UIConsoleEditGlobals * ptConsole;  // 0x21b8
    UIChatEditGlobals * ptChatEdit;  // 0x21c0
    GBIDPool * ptGBIDPool;  // 0x21c8
    QuestAppGlobals * ptQuestAppGlobals;  // 0x21d0
    LuaScriptAppGlobals * ptLuaScriptAppGlobals;  // 0x21d8
    ScriptEventAppGlobals * ptScriptEventAppGlobals;  // 0x21e0
    UIAppGlobals * ptUIAppGlobals;  // 0x21e8
    AppearanceGlobals * ptAppearanceGlobals;  // 0x21f0
    ProfUIAppGlobals * ptProfUIAppGlobals;  // 0x21f8
    GameDebugGlobals * ptGameDebugGlobals;  // 0x2200
    FontGlobals * ptFontGlobals;  // 0x2208
    ItemAppGlobals * ptItemAppGlobals;  // 0x2210
    DefinitionPrefetchData * ptPrefetchGlobals;  // 0x2218
    GlobalDefinitionAppGlobals * ptGlobalDefAppGlobals;  // 0x2220
    SNOArray * ptScriptDefsPurged;  // 0x2228
    SNOArray * ptConversationDefsPurged;  // 0x2230
    SNOArray * ptPowerDefsPurged;  // 0x2238
    SNOArray * ptConductorDefsPurged;  // 0x2240
    Blizzard::Lock::Mutex tScriptDefPurgeMutex;  // 0x2248
    Blizzard::Lock::Mutex tConversationDefPurgeMutex;  // 0x2268
    Blizzard::Lock::Mutex tPowerDefPurgeMutex;  // 0x2288
    Blizzard::Lock::Mutex tConductorDefPurgeMutex;  // 0x22a8
    Blizzard::Lock::Mutex tGlobalDisposeServerMutex;  // 0x22c8
    Blizzard::Lock::Semaphore tGlobalServerSemaphore;  // 0x22e8
    RGBAColor rgbaGreenscreen;  // 0x2310
    float flNormalMapFactor;  // 0x2314
    Console::Achievements * ptAchievements;  // 0x2318
    Console::ClientAchievementsInterface * ptAchievementsInterface;  // 0x2320
    BOOL bShowSidekickDebug;  // 0x2328
    FrameTimer mainTimer;  // 0x2330
    FrameTimer renderTimer;  // 0x2340
    FrameTimerHistory mainTimerHistory;  // 0x2350
    FrameTimerHistory renderTimerHistory;  // 0x2368
    FrameTimerHistory gpuTimerHistory;  // 0x2380
    uint64 nLastGameClockTime;  // 0x2398
    uint64 nLastGameClockDelta;  // 0x23a0
};
static_assert(sizeof(AppGlobals) == 0x23a8, "AppGlobals");

typedef OnlineService::Callback<void (int)> LocaleOfferCallback;

class ScopedLeaveAndReenterServerCode {  // size 0x4
public:
    ScopedLeaveAndReenterServerCode(BOOL bReturnToClient);
    ~ScopedLeaveAndReenterServerCode();
    SGameID GetSGameID() const;
private:
    SGameID m_idSGame;  // 0x00
};
static_assert(sizeof(ScopedLeaveAndReenterServerCode) == 0x4, "ScopedLeaveAndReenterServerCode");

// The constructor and destructor are defined in Game.cpp:961 and Game.cpp:978.
class ScopedEnterAndLeaveServerCode {  // size 0x4
public:
    ScopedEnterAndLeaveServerCode();
    ScopedEnterAndLeaveServerCode(SGameID idSGame);
    ~ScopedEnterAndLeaveServerCode();
private:
    BOOL m_bReturnToClient;  // 0x00
};
static_assert(sizeof(ScopedEnterAndLeaveServerCode) == 0x4, "ScopedEnterAndLeaveServerCode");

// Game.h:811-893. The bodies are not in the DWARF; they are written from the copies inlined into
// X/Client/CGame.cpp (AsyncRender::Flush and AsyncRender::AsyncRenderProc) and into
// _GLOBAL__sub_I_Game.cpp (the constructors: FrameTimer() inlines Reset() at line 813, and the
// five timers of sgtAppGlobals are zeroed by one merged memset attributed to line 836).
inline FrameTimer::FrameTimer() {  // line 811
    Reset();
}

inline void FrameTimer::Start() {  // line 816
    m_tStart = Blizzard::Time::Microseconds();
}

inline void FrameTimer::Stop() {  // line 821
    if (m_tStart) {
        const Blizzard::Time::Microsecond tEnd = Blizzard::Time::Microseconds();
        m_tAccum += tEnd - m_tStart;
    }
}

inline void FrameTimer::Reset() {  // line 833
    m_tStart = 0;
    m_tAccum = 0;
}

inline Blizzard::Time::Microsecond FrameTimer::Get() {  // line 839
    return m_tAccum;
}

inline FrameTimer::IdleScope::IdleScope(FrameTimer & tTimer) : m_tTimer(tTimer) {  // line 847
    m_tTimer.Stop();
}

inline FrameTimer::IdleScope::~IdleScope() {  // line 848
    m_tTimer.Start();
}

inline FrameTimerHistory::FrameTimerHistory() {
    Reset();
}

inline void FrameTimerHistory::Reset() {
    for (int32 i = 0; i < PERIOD_COUNT; i++) {
        m_atTimes[i] = 0;
    }
}

inline void FrameTimerHistory::Append(const Blizzard::Time::Microsecond tTime) {  // line 876
    // DWARF: static local PERIOD_WEIGHTS, const float[3], Game.h:878; values from the literal pool.
    static const float PERIOD_WEIGHTS[PERIOD_COUNT] = {1.0f, 0.1f, 0.01f};
    for (int32 i = 0; i < PERIOD_COUNT; i++) {
        m_atTimes[i] = (Blizzard::Time::Microsecond)(tTime * PERIOD_WEIGHTS[i] + m_atTimes[i] * (1.0f - PERIOD_WEIGHTS[i]));
    }
}

inline Blizzard::Time::Microsecond FrameTimerHistory::Get(const FrameTimerHistory::Period period) {  // line 893
    return m_atTimes[period];
}

inline GameCommonData & GameGetActiveGameCommonData() {  // line 1368
    return *ThreadGlobals::Get().m_ptActiveGameCommonData->ptActiveGCD;
}

// Game.h:967. The body is the one store sAppGlobalsInitialize (Game.cpp:576) merges into its stp.
inline void AppGlobals::PlayingCGameInit() {
    ptCGamePlaying = NULL;
}


// Functions defined in this file (bodies are not in the DWARF):
//   line   572  GameParams::GameParams()  [inlined into 9 functions; artificial]
//   line   811  FrameTimer::FrameTimer()  [inline]
//   line   816  void FrameTimer::Start()  [inlined into 6 functions]
//   line   821  void FrameTimer::Stop()  [inlined into 6 functions]
//   line   833  void FrameTimer::Reset()  [inlined into 2 functions]
//   line   839  Blizzard::Time::Microsecond FrameTimer::Get()  [inlined into 2 functions]
//   line   847  FrameTimer::IdleScope::IdleScope(FrameTimer & tTimer)  [inlined into 4 functions]
//   line   848  FrameTimer::IdleScope::~IdleScope()  [inlined into 4 functions]
//   line   876  void FrameTimerHistory::Append(const Blizzard::Time::Microsecond tTime)  [inlined into 2 functions]
//   line   893  Blizzard::Time::Microsecond FrameTimerHistory::Get(const FrameTimerHistory::Period period)  [inlined into 1 functions]
//   line   940  AppGlobals::AppGlobals()  [inline; artificial]
//   line   969  CGame * AppGlobals::PlayingCGameGet()  [inlined into 1 functions]
//   line  1368  GameCommonData & GameGetActiveGameCommonData()  [inlined into 3495 functions]

// Globals defined in Game.cpp (line numbers from the reference .cpp).
extern int32 s_nGameFrameCounter;  // Game.cpp:296
extern const GlobalSNO cgeGlobalSnoDefaultStartWorld;  // Game.cpp:299
extern AppGlobals sgtAppGlobals;  // Game.cpp:412
extern GroupDefinition ** s_patGroups;  // Game.cpp:413
extern BOOL sgfAppActivated;  // Game.cpp:414
extern BOOL sgbRunServerIdleThread;  // Game.cpp:419

// The external functions Game.cpp defines (line numbers from the reference .cpp). Those whose
// parameter types are not adopted yet (DifficultyLevel, CRefString-taking helpers) are left out.
void AppOverrideDRLGSeed(int32 nSeed);  // 749
int32 AppGetOverriddenDRLGSeed();  // 765
BOOL AppIsDRLGSeedOverridden();  // 774
void AppDrawFlagSet(int32 nFlag, BOOL bValue);  // 799
BOOL AppRenderLayerIsHidden(int32 nFlag);  // 826
BOOL AppDrawFlagIsSet(int32 nFlag);  // 832
void AppDrawFlagReset();  // 847
BOOL AppIsBlizzardServer();  // 868
BOOL GameCommonDataExists();  // 894
BOOL GameActiveGameCommonDataIsClient();  // 913
BOOL GameActiveGameCommonDataIsServer();  // 923
void GameSetActiveGameCommonData(GameCommonData * ptActiveGCD, BOOL bClientActive);  // 933
BOOL _GameServerCodeEnter(const SGameID idSGame);  // 987
BOOL GameServerCodeEnter_Track();  // 1045
BOOL _GameServerCodeEnter();  // 1062
BOOL GameServerCodeCanEnter();  // 1095
SGameID GameServerCodeLeave_Tracked(BOOL bReturnToClient);  // 1122
BOOL LocaleIsInstalled(const LPCSTR szLocaleString);  // 1290
void ProcessFallbackLanguage(XLocale & eXLocale, const blz::vector<int, blz::allocator<int> > & tDetectedLocales);  // 1482
XLocale FileSystemGetLocale();  // 1508
void OnConfirmMissingSystemLocaleOffer(const int32 nUserIndex, const UIID & tId);  // 1702
void OnCloseMissingSystemLocaleOffer(const int32 nUserIndex, LocaleOfferCallback pfnOnLocaleOfferComplete, const UIID & tId);  // 1711
bool ShowSystemLocaleAOCOffer(const int32 nUserIndex, LocaleOfferCallback pfnOnLocaleOfferComplete);  // 1722
bool UsePreselectedUser();  // 1779
void FileSystemInitializeForApp();  // 1832
void FileSystemDisposeForApp();  // 1891
XMemoryAllocatorInterface * AppGetCurrentMemoryInterface();  // 1938
void MemoryPoolsInitializeForApp();  // 2031
void MemoryPoolsDisposeForApp();  // 2040
void GameStringInitializeForApp();  // 2049
void GameStringDisposeForApp();  // 2061
BOOL AppInitialize();  // 2372
void AppDispose();  // 2544
GameType GameGetGameType();  // 2603
uint32 GameParamsGetGameSeed();  // 2614
void GameParamsSetDefault(GameParams & tParams, GameType eGameType);  // 2623
void GameStartMenu(Act eLoadingScreenAct);  // 2672
void GameStartServer();  // 2755
SNO GetSNOWorldFromGameType(GameType eGameType);  // 2778
void GameStartConsoleClient(uint32 idGameConnection, BOOL bIsHost, GameType eGameType, const uint32 dwGameCreateParamFlags, BOOL bResumeFromSave, Act eAct, Act ePreferredAdventureAct, SNO snoQuest, int32 nStepUID, int32 nHandicap, uint32 uEntranceGUIDOverride, int32 nFreeKeyLevel, uint64 * pArrayTieredRiftHeroForFreeKey);  // 2836
void GamePlayWorld(SNO snoWorld, uint32 dwCreationFlags, int32 nHandicap, BOOL bResumeFromSave);  // 2906
void GameQueueGameStart(GameParams & tParams);  // 2940
BOOL GameIsAGameStartQueued();  // 2962
void GameCheckForQueuedGameStart();  // 2987
void GameStop();  // 3049
void GamelessGameGlobalsInit();  // 3065
void GamelessGameGlobalsDispose();  // 3090
void GameStartGame(const GameParams & tParams);  // 3098
GroupDefinition * GameDefinitionGet(const SNOGroup snoGroup);  // 3357
void GameDisposeGames();  // 3371
BOOL GameLoopRunning();  // 3440
BOOL AppIsDrawingWorld();  // 3467
GameParts GameGetParts();  // 3493
typedef int32 DifficultyLevel;  // GameCommonData.h:129 (repeated here: that header includes this one)
DifficultyLevel GameGetDifficultyLevel();  // 3502
DifficultyLevel GameGetDifficultyParamsLevel(BOOL bIsFromTieredRift);  // 3511
int32 GameGetHandicapLevel();  // 3531
HandicapLevel * GameGetInfoForHandicapLevel(int32 nHandicapLevel);  // 3540
float GameGetHandicapDamageScalar();  // 3567
void GameGetPlayerHandicapValues(int32 nHandicapLevel, float & flMagicFindFromHandicap, float & flGoldFindFromHandicap, float & flXPBonusFromHandicap);  // 3587
int32 GameGetMonsterLevel();  // 3611

BOOL GameUsesMonsterDensityModifiers();  // 3645
BOOL GameIsStartUp();  // 3662
BOOL GameIsServerOnly();  // 3694
BOOL ServerIsLocal();  // 3713
BOOL GameIsNetworked();  // 3734
BOOL GameIsBattleNetServer();  // 3754
uint32 GameClockGetTime();  // 3771
void GameClockInitialize(GameClock & tGameClock);  // 3780
void GameClockAdvanceStart(GameClock & tGameClock, const uint32 dwGameClockTicks, uint32 & dwClockTicksToUse, uint32 & dwGameTicksToUse, const uint32 dwTicksPerSecond);  // 3793
void GameClockAdvanceComplete(GameClock & tGameClock, const BOOL bResetGameClockTime, const uint32 dwUsedClockTicks, const uint32 dwUsedGameTicks, const uint32 dwTicksPerSecond);  // 3850
BOOL GameClockDebugLag(uint32 dwGameClockTicks, GameClock & tGameClock);  // 3892
uint32 AppGetTicks();  // 3947
char * GameGetCustomCrashDescription();  // 4074
void AppIdle();  // 4192
void sCGameInit(CGame * ptCGame);  // 4600
void sCGameDispose(CGame * ptCGame);  // 4640
void GameGlobalsInit();  // 4665
void GameGlobalsDispose();  // 4691
BOOL GameModeIsAnEditMode(const GameMode eMode);  // 4706
BOOL GameModeIsAPlayingMode(const GameMode eMode);  // 4714
BOOL AppExitIsEnabled();  // 4752
int32 GameRandRangeInt(const int32 nMin, const int32 nMax);  // 4798
float GameRandRangeFloat(const float flMin, const float flMax);  // 4823
BOOL GameIsParamFlagSet(GameParamFlags eFlag);  // 5098
float GameGetMaxExpDist();  // 5107
float GameGetMinimapZoomDist();  // 5116
uint32 GameGetCurrentSeason();  // 5134
BOOL GameIsSeasonal();  // 5142
BOOL GameIsUtility();  // 5157
uint64 GameGetDailyPlatinumLimit();  // 5187
uint32 GameGetDailyPlatinumResetTime();  // 5196
AnniversaryEventStatus GameGetAnniversaryEventStatus();  // 5205
uint32 AppGetSafeTicksPerSec(uint32 dwDesiredTicksPerSec);  // 5438
BOOL AppServerThreadIsRunningGame();  // 5488
