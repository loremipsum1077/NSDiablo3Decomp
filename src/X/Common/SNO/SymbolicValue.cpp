// Original file: X/Common/Sno/SymbolicValue.cpp
// The SymbolicEntry name tables other files refer to, and the five whose values are string hashes
// computed at start-up (they are what the static initializer consists of). Tables only this file
// uses have no symbols in the image and are not written yet.

#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "SymbolicValue.h"
#include "../AI/ActorAISwig.h"
#include "../GizmoSwig.h"
#include "../Definitions/GlobalDefinition.h"
#include "../ItemsSwig.h"
#include "../../Client/UserInterface/Console/ControllerActions.h"
#include "../../Client/Input/Input.h"
#include "../ActorCommonData.h"
#include "../Definitions/GameBalanceDefinition.h"
#include "../Attributes/FastAttribConstants.h"
#include "../Powers/PowerAPISwig.h"
#include "../DRLGTypes.h"
#include "../Definitions/WorldDefinition.h"
#include "../GameSwig.h"
#include "../CosmeticItems.h"
#include "../ActorCommonDataSwig.h"
#include "../MonsterStruct.h"
#include "../Attributes/FastAttribConstantsSwig.h"
#include "../../Client/ImageCollection.h"
#include "../Definitions/HeroDefinition.h"
#include "../../Client/Definitions/TrailDefinition.h"
#include "../AI/ActorAIStatesSwig.h"
#include "../../Server/LuaScript/ScriptEventSwig.h"
#include "../../Server/LuaScript/ScriptEventParams.h"
#include "../Definitions/LevelAreaDefinition.h"
#include "../../Client/Definitions/SoundDefinition.h"
#include "../NodePathSwig.h"
#include "../../Client/Definitions/ClothDefinition.h"
#include "../Definitions/AnimTreeDefinition.h"
#include "../Definitions/AnimationDefinition.h"
#include "../Player.h"
#include "../ConversationTreeIterator.h"
#include "../Crafting/ItemCraftingSwig.h"
#include "../Items.h"
#include "../Labels.h"
#include "../Quests/QuestsSwig.h"
#include "../Definitions/QuestDefinition.h"
#include "../Definitions/ActorDefinition.h"
#include "../Powers/PowersCommon.h"
#include "../HirelingSwig.h"
#include "../../Client/Definitions/FontDefinition.h"

// X/Common/Definitions/GameBalanceDefinition.h:34 (the ones these tables use that its adopted subset lacks)
enum {
    GB_ITEMTYPES = 1,
    GB_EXPERIENCETABLE = 3,
    GB_HIRELINGS = 4,
    GB_MONSTER_LEVELS = 5,
    GB_AFFIXES = 8,
    GB_MOVEMENT_STYLES = 10,
    GB_LABELS = 11,
    GB_LOOTDISTRIBUTION = 12,
    GB_RARE_ITEM_NAMES = 16,
    GB_MONSTER_AFFIXES = 18,
    GB_MONSTER_NAMES = 19,
    GB_HELPCODES = 24,
    GB_ITEM_DROP_TABLE = 25,
    GB_ITEM_LEVEL_MODIFIERS = 26,
    GB_ITEM_QUALITY_CLASSES = 27,
    GB_HANDICAPS = 28,
    GB_ITEM_SALVAGE_LEVELS = 29,
    GB_ITEM_TIERS = 35,
    GB_POWER_FORMULA_TABLES = 36,
    GB_SCRIPTED_ACHIEVEMENT_EVENTS = 37,
    GB_LOOTRUN_QUEST_TIERS = 39,
    GB_LEGACY_ITEM_CONVERSIONS = 45,
    GB_ENCHANT_ITEM_AFFIX_USE_COUNT_COST_SCALARS = 46,
    GB_TIERED_LOOT_RUN_LEVELS = 49,
};
// X/Common/Definitions/GameBalanceDefinition.h:491 (the ones these tables use; not adopted in src yet)
enum {
    CQL_COMMON = 1,
    CQL_UNCOMMON = 2,
    CQL_RARE = 3,
    CQL_VERY_RARE = 4,
};
// X/Common/Definitions/GameBalanceDefinition.h:294 (the ones these tables use; not adopted in src yet)
enum {
    AT_PREFIX = 0,
    AT_SUFFIX = 1,
    AT_INHERENT = 2,
    AT_TITLE = 5,
    AT_QUALITY = 6,
    AT_IMMUNITY = 7,
    AT_RANDOM = 9,
    AT_ENCHANTMENT = 10,
    AT_ADD_SOCKET = 11,
};
// X/Common/Definitions/GameBalanceDefinition.h:316 (the ones these tables use; not adopted in src yet)
enum {
    AFFIXGROUP_MAJOR = 0,
    AFFIXGROUP_MINOR = 1,
};
// X/Common/ActorCommonDataSwig.h:32 (the ones these tables use; not adopted in src yet)
enum {
    MF_NONE = 0,
    MF_FALLEN = 1,
    MF_GOATMEN = 2,
    MF_ROGUE = 3,
    MF_SKELETON = 4,
    MF_ZOMBIE = 5,
    MF_SPIDER = 6,
    MF_TRIUNE = 7,
    MF_WOODWRAITH = 8,
    MF_HUMAN = 9,
    MF_ANIMAL = 10,
    MF_TREASURE_GOBLIN = 11,
    MF_CRAZED_ANGEL = 12,
};
// X/Common/Definitions/ConversationDefinition.h:50 (the ones these tables use; not adopted in src yet)
enum {
    CONV_SPEAKER_INVALID = -1,
    CONV_SPEAKER_PLAYER = 0,
    CONV_SPEAKER_PRIMARY_NPC = 1,
    CONV_SPEAKER_ALT_NPC1 = 2,
    CONV_SPEAKER_ALT_NPC2 = 3,
    CONV_SPEAKER_ALT_NPC3 = 4,
    CONV_SPEAKER_ALT_NPC4 = 5,
    CONV_SPEAKER_FOLLOWER_TEMPLAR = 6,
    CONV_SPEAKER_FOLLOWER_SCOUNDREL = 7,
    CONV_SPEAKER_FOLLOWER_ENCHANTRESS = 8,
};
// X/Common/Definitions/ConversationDefinition.h:106 (the ones these tables use; not adopted in src yet)
enum {
    CONV_VAR_SET_OP_INVALID = -1,
    CONV_VAR_SET_OP_INC = 0,
    CONV_VAR_SET_OP_DEC = 1,
    CONV_VAR_SET_OP_SET = 2,
};
// X/Common/Definitions/ConversationDefinition.h:230 (the ones these tables use; not adopted in src yet)
enum {
    CONV_TYPE_FOLLOWER_SOUNDSET = 0,
    CONV_TYPE_PLAYER_EMOTE = 1,
    CONV_TYPE_AMBIENT_FLOAT = 2,
    CONV_TYPE_FOLLOWER_BANTER = 3,
    CONV_TYPE_FOLLOWER_CALLOUT = 4,
    CONV_TYPE_PLAYER_CALLOUT = 5,
    CONV_TYPE_GLOBAL_CHATTER = 6,
    CONV_TYPE_GLOBAL_FLOAT = 7,
    CONV_TYPE_LORE = 8,
    CONV_TYPE_AMBIENT_GOSSIP = 9,
    CONV_TYPE_TALK_MENU = 10,
    CONV_TYPE_QUEST_STANDARD = 11,
    CONV_TYPE_QUEST_FLOAT = 12,
    CONV_TYPE_QUEST_EVENT = 13,
};

uint32 StringHash(LPCSTR pszName);  // X/Sigma/TypeDescriptor/SymbolicValue.cpp:58
uint32 StringHashI(LPCSTR pszName);  // X/Sigma/TypeDescriptor/SymbolicValue.cpp:117

// X/Common/Definitions/AttachmentDefinition.cpp
extern const CHAR * DEFAULT_HARDPOINT_NAMESTR;
extern const CHAR * LEFT_FOOT_HARDPOINT_NAMESTR;
extern const CHAR * RIGHT_FOOT_HARDPOINT_NAMESTR;
extern const CHAR * LEFT_HAND_HARDPOINT_NAMESTR;
extern const CHAR * RIGHT_HAND_HARDPOINT_NAMESTR;
extern const CHAR * CHEST_HARDPOINT_NAMESTR;
extern const CHAR * BACK_HARDPOINT_NAMESTR;
extern const CHAR * AURA_HARDPOINT_NAMESTR;
extern const CHAR * CURSE_HARDPOINT_NAMESTR;
extern const CHAR * LEFT_WEAPON_HARDPOINT_NAMESTR;
extern const CHAR * RIGHT_WEAPON_HARDPOINT_NAMESTR;
extern const CHAR * LEFT_FIST_HARDPOINT_NAMESTR;
extern const CHAR * RIGHT_FIST_HARDPOINT_NAMESTR;
extern const CHAR * LIGHT_HARDPOINT_NAMESTR;
extern const CHAR * SHIELD_HARDPOINT_NAMESTR;
extern const CHAR * MOUTH_HARDPOINT_NAMESTR;
extern const CHAR * MANA_GATHER_HARDPOINT_NAMESTR;
extern const CHAR * HEAD_HARDPOINT_NAMESTR;
extern const CHAR * HELM_HARDPOINT_NAMESTR;
extern const CHAR * LEFT_SHOULDER_HARDPOINT_NAMESTR;
extern const CHAR * RIGHT_SHOULDER_HARDPOINT_NAMESTR;
extern const CHAR * CENTROID_HARDPOINT_NAMESTR;
extern const CHAR * CENTER_HARDPOINT_NAMESTR;
extern const CHAR * TRAIL_1_NAMESTR;
extern const CHAR * TRAIL_2_NAMESTR;
extern const CHAR * TRAIL_3_NAMESTR;
extern const CHAR * TRAIL_4_NAMESTR;
extern const CHAR * SHEATH_LEFT_HARDPOINT_NAMESTR;
extern const CHAR * SHEATH_RIGHT_HARDPOINT_NAMESTR;
extern const CHAR * SHEATH_LEFT_HIP_HARDPOINT_NAMESTR;
extern const CHAR * SHEATH_RIGHT_HIP_HARDPOINT_NAMESTR;
extern const CHAR * SHEATH_SHIELD_HARDPOINT_NAMESTR;
extern const CHAR * PELVIS_HARDPOINT_NAMESTR;
extern const CHAR * ANIM_PERMUTATION_HARDPOINT_NAMESTR;
extern const CHAR * EXPLOSION_HARDPOINT_NAMESTR;
extern const CHAR * BEAM_CENTER_HARDPOINT_NAMESTR;
extern const CHAR * BEAM_LEFT_HARDPOINT_NAMESTR;
extern const CHAR * BEAM_RIGHT_HARDPOINT_NAMESTR;
extern const CHAR * ATTACH_HARDPOINT_NAMESTR;
extern const CHAR * LEGENDARY_FX_HARDPOINT_NAMESTR;
extern const CHAR * PHYSICS_1_HARDPOINT_NAMESTR;
extern const CHAR * PHYSICS_2_HARDPOINT_NAMESTR;
extern const CHAR * PHYSICS_3_HARDPOINT_NAMESTR;
extern const CHAR * PHYSICS_4_HARDPOINT_NAMESTR;
extern const CHAR * LEFT_WEAPON_PHYSICS_1_HARDPOINT_NAMESTR;
extern const CHAR * LEFT_WEAPON_PHYSICS_2_HARDPOINT_NAMESTR;
extern const CHAR * LEFT_WEAPON_PHYSICS_3_HARDPOINT_NAMESTR;
extern const CHAR * LEFT_WEAPON_PHYSICS_4_HARDPOINT_NAMESTR;
extern const CHAR * LEFT_WEAPON_LEGENDARY_FX_HARDPOINT_NAMESTR;
extern const CHAR * RIGHT_WEAPON_PHYSICS_1_HARDPOINT_NAMESTR;
extern const CHAR * RIGHT_WEAPON_PHYSICS_2_HARDPOINT_NAMESTR;
extern const CHAR * RIGHT_WEAPON_PHYSICS_3_HARDPOINT_NAMESTR;
extern const CHAR * RIGHT_WEAPON_PHYSICS_4_HARDPOINT_NAMESTR;
extern const CHAR * RIGHT_WEAPON_LEGENDARY_FX_HARDPOINT_NAMESTR;

// line 122
extern const SymbolicEntry gActorBrainNames[25];
const SymbolicEntry gActorBrainNames[25] = {
    { ACTORBRAIN_UNDEFINED, "Undefined" },
    { ACTORBRAIN_NOBRAIN, "NoBrain" },
    { ACTORBRAIN_PLAYER, "Player" },
    { ACTORBRAIN_MONSTER, "Monster" },
    { ACTORBRAIN_SPAWNER, "Spawner" },
    { ACTORBRAIN_DO_NOTHING, "Do Nothing" },
    { ACTORBRAIN_SHRINE_RESURRECTION, "Resurrection Shrine" },
    { ACTORBRAIN_HIRELING, "Hireling" },
    { ACTORBRAIN_ZOMBIE, "Zombie" },
    { ACTORBRAIN_SKELETON_SUMMONER, "Skeleton Summoner" },
    { ACTORBRAIN_SIEGEBREAKER_DEMON, "Siegebreaker Demon" },
    { ACTORBRAIN_SKELETON_SHIELDBEARER, "Skeleton ShieldBearer" },
    { ACTORBRAIN_TRIUNE_SUMMONER, "Triune Summoner" },
    { ACTORBRAIN_PIG_CAGED, "Pig Caged" },
    { ACTORBRAIN_WOODWRAITH, "Wood Wraith" },
    { ACTORBRAIN_TRIUNEVESSEL, "Triune Vessel" },
    { ACTORBRAIN_TRIUNE_BERSERKER, "Triune Berserker" },
    { ACTORBRAIN_SPORE_POD, "Spore Pod" },
    { ACTORBRAIN_WIZARD_TORNADO, "Wizard Tornado" },
    { ACTORBRAIN_FALLEN_SHAMAN, "Fallen Shaman" },
    { ACTORBRAIN_SAND_TORNADO, "Sand Tornado" },
    { ACTORBRAIN_SENTRY, "Sentry" },
    { ACTORBRAIN_HIRELING_PROXY, "Hireling Proxy" },
    { ACTORBRAIN_VENDOR_WITH_ATTRACT, "Vendor With Attract" },
    { 0, NULL },
};

// line 173
extern const SymbolicEntry gActorGizmoNames[47];
const SymbolicEntry gActorGizmoNames[47] = {
    { ACTORGIZMO_UNDEFINED, "" },
    { ACTORGIZMO_DOOR, "Door" },
    { ACTORGIZMO_BREAKABLEDOOR, "Breakable Door" },
    { ACTORGIZMO_CHEST, "Chest" },
    { ACTORGIZMO_SIGN, "Sign" },
    { ACTORGIZMO_PORTAL, "Portal" },
    { ACTORGIZMO_PORTAL_DESTINATION, "Portal Destination" },
    { ACTORGIZMO_WAYPOINT, "Waypoint" },
    { ACTORGIZMO_HEALING_WELL, "HealingWell" },
    { ACTORGIZMO_POWERUP, "PowerUp" },
    { ACTORGIZMO_TOWN_PORTAL, "TownPortal" },
    { ACTORGIZMO_HEARTH_PORTAL, "HearthPortal" },
    { ACTORGIZMO_HEADSTONE, "Headstone" },
    { ACTORGIZMO_BREAKABLECHEST, "Breakable Chest" },
    { ACTORGIZMO_SHARED_STASH, "Shared Stash" },
    { ACTORGIZMO_SPAWNER, "Spawner" },
    { ACTORGIZMO_TRIGGER, "Trigger" },
    { ACTORGIZMO_SECRET_PORTAL, "Secret Portal" },
    { ACTORGIZMO_DESTROYABLE_OBJECT, "Destroyable Object" },
    { ACTORGIZMO_CHECKPOINT, "Checkpoint" },
    { ACTORGIZMO_SWITCH, "Switch" },
    { ACTORGIZMO_LOOTRUN_SWITCH, "LootRun Switch" },
    { ACTORGIZMO_WEEKLYCHALLENGE_SWITCH, "Weekly Challenge Switch" },
    { ACTORGIZMO_PRESSUREPLATE, "Pressure Plate" },
    { ACTORGIZMO_GATE, "Gate" },
    { ACTORGIZMO_DESTROY_ON_PROXIMITY, "Destroy Self When Near" },
    { ACTORGIZMO_ACT_TRANSITION_OBJECT, "Act Transition Object" },
    { ACTORGIZMO_REFORMING_DESTROYABLE_OBJECT, "Reforming Destroyable Object" },
    { ACTORGIZMO_BANNER, "Banner" },
    { ACTORGIZMO_LORE_CHEST, "Lore Chest" },
    { ACTORGIZMO_BOSS_PORTAL, "Boss Portal" },
    { ACTORGIZMO_PLACED_LOOT, "Placed Loot" },
    { ACTORGIZMO_SAVEPOINT, "Save Point" },
    { ACTORGIZMO_SAVEPOINT_PORTAL, "Return Point Portal" },
    { ACTORGIZMO_DUNGEON_PORTAL, "Dungeon Portal" },
    { ACTORGIZMO_RETURN_PORTAL, "Return Portal" },
    { ACTORGIZMO_PAGE_OF_FATE_PORTAL, "Page Of Fate Portal" },
    { ACTORGIZMO_IDENTIFY_ALL, "Identify All" },
    { ACTORGIZMO_RECREATE_GAME_WITH_PARTY, "Recreate Game With Party" },
    { ACTORGIZMO_MAILBOX, "Mailbox" },
    { ACTORGIZMO_POOL_OF_REFLECTION, "PoolOfReflection" },
    { ACTORGIZMO_MULTICLICK, "MultiClick" },
    { ACTORGIZMO_OBJECTIVE_FINDER, "Objective Finder" },
    { ACTORGIZMO_NECRO_CORPSE, "Necromancer Monster Corpse" },
    { ACTORGIZMO_WEEKLYCHALLENGE_INSPECT_ARMORRACK, "Challenge Rift Inspect ArmorRack" },
    { ACTORGIZMO_WEEKLYCHALLENGE_REWARD, "Challenge Rift Reward" },
    { 0, NULL },
};

// line 981
extern const SymbolicEntry gComponentLooks[126];
const SymbolicEntry gComponentLooks[126] = {
    { (int32)StringHashI("A"), "A" },
    { (int32)StringHashI("B"), "B" },
    { (int32)StringHashI("C"), "C" },
    { (int32)StringHashI("D"), "D" },
    { (int32)StringHashI("E"), "E" },
    { (int32)StringHashI("F"), "F" },
    { (int32)StringHashI("G"), "G" },
    { (int32)StringHashI("H"), "H" },
    { (int32)StringHashI("I"), "I" },
    { (int32)StringHashI("J"), "J" },
    { (int32)StringHashI("K"), "K" },
    { (int32)StringHashI("L"), "L" },
    { (int32)StringHashI("M"), "M" },
    { (int32)StringHashI("N"), "N" },
    { (int32)StringHashI("O"), "O" },
    { (int32)StringHashI("P"), "P" },
    { (int32)StringHashI("Q"), "Q" },
    { (int32)StringHashI("R"), "R" },
    { (int32)StringHashI("S"), "S" },
    { (int32)StringHashI("T"), "T" },
    { (int32)StringHashI("U"), "U" },
    { (int32)StringHashI("V"), "V" },
    { (int32)StringHashI("W"), "W" },
    { (int32)StringHashI("X"), "X" },
    { (int32)StringHashI("Y"), "Y" },
    { (int32)StringHashI("Z"), "Z" },
    { (int32)StringHashI("Unique01"), "Unique01" },
    { (int32)StringHashI("Unique02"), "Unique02" },
    { (int32)StringHashI("Unique03"), "Unique03" },
    { (int32)StringHashI("Unique04"), "Unique04" },
    { (int32)StringHashI("Unique05"), "Unique05" },
    { (int32)StringHashI("Unique06"), "Unique06" },
    { (int32)StringHashI("Unique07"), "Unique07" },
    { (int32)StringHashI("Unique08"), "Unique08" },
    { (int32)StringHashI("Unique09"), "Unique09" },
    { (int32)StringHashI("Unique10"), "Unique10" },
    { (int32)StringHashI("Unique11"), "Unique11" },
    { (int32)StringHashI("Unique12"), "Unique12" },
    { (int32)StringHashI("Unique13"), "Unique13" },
    { (int32)StringHashI("Unique14"), "Unique14" },
    { (int32)StringHashI("Unique15"), "Unique15" },
    { (int32)StringHashI("Unique16"), "Unique16" },
    { (int32)StringHashI("Unique17"), "Unique17" },
    { (int32)StringHashI("Unique18"), "Unique18" },
    { (int32)StringHashI("Unique19"), "Unique19" },
    { (int32)StringHashI("Unique20"), "Unique20" },
    { (int32)StringHashI("Unique21"), "Unique21" },
    { (int32)StringHashI("Unique22"), "Unique22" },
    { (int32)StringHashI("Unique23"), "Unique23" },
    { (int32)StringHashI("Unique24"), "Unique24" },
    { (int32)StringHashI("Unique25"), "Unique25" },
    { (int32)StringHashI("Unique26"), "Unique26" },
    { (int32)StringHashI("Unique27"), "Unique27" },
    { (int32)StringHashI("Unique28"), "Unique28" },
    { (int32)StringHashI("Unique29"), "Unique29" },
    { (int32)StringHashI("Unique30"), "Unique30" },
    { (int32)StringHashI("Unique31"), "Unique31" },
    { (int32)StringHashI("Unique32"), "Unique32" },
    { (int32)StringHashI("Unique33"), "Unique33" },
    { (int32)StringHashI("Unique34"), "Unique34" },
    { (int32)StringHashI("Unique35"), "Unique35" },
    { (int32)StringHashI("Unique36"), "Unique36" },
    { (int32)StringHashI("Unique37"), "Unique37" },
    { (int32)StringHashI("Unique38"), "Unique38" },
    { (int32)StringHashI("Unique39"), "Unique39" },
    { (int32)StringHashI("Unique40"), "Unique40" },
    { (int32)StringHashI("Unique41"), "Unique41" },
    { (int32)StringHashI("Unique42"), "Unique42" },
    { (int32)StringHashI("Unique43"), "Unique43" },
    { (int32)StringHashI("Unique44"), "Unique44" },
    { (int32)StringHashI("Unique45"), "Unique45" },
    { (int32)StringHashI("Unique46"), "Unique46" },
    { (int32)StringHashI("Unique47"), "Unique47" },
    { (int32)StringHashI("Unique48"), "Unique48" },
    { (int32)StringHashI("Unique49"), "Unique49" },
    { (int32)StringHashI("Unique50"), "Unique50" },
    { (int32)StringHashI("Unique51"), "Unique51" },
    { (int32)StringHashI("Unique52"), "Unique52" },
    { (int32)StringHashI("Unique53"), "Unique53" },
    { (int32)StringHashI("Unique54"), "Unique54" },
    { (int32)StringHashI("Unique55"), "Unique55" },
    { (int32)StringHashI("Unique56"), "Unique56" },
    { (int32)StringHashI("Unique57"), "Unique57" },
    { (int32)StringHashI("Unique58"), "Unique58" },
    { (int32)StringHashI("Unique59"), "Unique59" },
    { (int32)StringHashI("Unique60"), "Unique60" },
    { (int32)StringHashI("Unique61"), "Unique61" },
    { (int32)StringHashI("Unique62"), "Unique62" },
    { (int32)StringHashI("Unique63"), "Unique63" },
    { (int32)StringHashI("Unique64"), "Unique64" },
    { (int32)StringHashI("Unique65"), "Unique65" },
    { (int32)StringHashI("Unique66"), "Unique66" },
    { (int32)StringHashI("Unique67"), "Unique67" },
    { (int32)StringHashI("Unique68"), "Unique68" },
    { (int32)StringHashI("Unique69"), "Unique69" },
    { (int32)StringHashI("Unique70"), "Unique70" },
    { (int32)StringHashI("Unique71"), "Unique71" },
    { (int32)StringHashI("Unique72"), "Unique72" },
    { (int32)StringHashI("Unique73"), "Unique73" },
    { (int32)StringHashI("Unique74"), "Unique74" },
    { (int32)StringHashI("Unique75"), "Unique75" },
    { (int32)StringHashI("Unique76"), "Unique76" },
    { (int32)StringHashI("Unique77"), "Unique77" },
    { (int32)StringHashI("Unique78"), "Unique78" },
    { (int32)StringHashI("Unique79"), "Unique79" },
    { (int32)StringHashI("Unique80"), "Unique80" },
    { (int32)StringHashI("Unique81"), "Unique81" },
    { (int32)StringHashI("Unique82"), "Unique82" },
    { (int32)StringHashI("Unique83"), "Unique83" },
    { (int32)StringHashI("Unique84"), "Unique84" },
    { (int32)StringHashI("Unique85"), "Unique85" },
    { (int32)StringHashI("Unique86"), "Unique86" },
    { (int32)StringHashI("Unique87"), "Unique87" },
    { (int32)StringHashI("Unique88"), "Unique88" },
    { (int32)StringHashI("Unique89"), "Unique89" },
    { (int32)StringHashI("Unique90"), "Unique90" },
    { (int32)StringHashI("Unique91"), "Unique91" },
    { (int32)StringHashI("Unique92"), "Unique92" },
    { (int32)StringHashI("Unique93"), "Unique93" },
    { (int32)StringHashI("Unique94"), "Unique94" },
    { (int32)StringHashI("Unique95"), "Unique95" },
    { (int32)StringHashI("Unique96"), "Unique96" },
    { (int32)StringHashI("Unique97"), "Unique97" },
    { (int32)StringHashI("Unique98"), "Unique98" },
    { (int32)StringHashI("Unique99"), "Unique99" },
    { 0, NULL },
};

// line 1214
extern const SymbolicEntry gDyeValueTooltips[24];
const SymbolicEntry gDyeValueTooltips[24] = {
    { DYE_TYPE_NONE, "None" },
    { DYE_TYPE_VANISHING, "Dye_Vanishing" },
    { DYE_TYPE_01, "Dye_20" },
    { DYE_TYPE_02, "Dye_18" },
    { DYE_TYPE_03, "Dye_19" },
    { DYE_TYPE_04, "Dye_10" },
    { DYE_TYPE_05, "Dye_17" },
    { DYE_TYPE_06, "Dye_13" },
    { DYE_TYPE_07, "Dye_11" },
    { DYE_TYPE_08, "Dye_16" },
    { DYE_TYPE_09, "Dye_09" },
    { DYE_TYPE_10, "Dye_15" },
    { DYE_TYPE_11, "Dye_04" },
    { DYE_TYPE_12, "Dye_03" },
    { DYE_TYPE_13, "Dye_CE_02" },
    { DYE_TYPE_14, "Dye_14" },
    { DYE_TYPE_15, "Dye_05" },
    { DYE_TYPE_16, "Dye_12" },
    { DYE_TYPE_17, "Dye_06" },
    { DYE_TYPE_18, "Dye_01" },
    { DYE_TYPE_19, "Dye_08" },
    { DYE_TYPE_20, "Dye_CE_01" },
    { DYE_TYPE_21, "Dye_21" },
    { 0, NULL },
};

// line 1244
extern const SymbolicEntry gItemEffectTypes[16];
const SymbolicEntry gItemEffectTypes[16] = {
    { ITEM_EFFECT_TYPE_NONE, "- None -" },
    { ITEM_EFFECT_TYPE_LIGHTNING, "Lightning" },
    { ITEM_EFFECT_TYPE_COLD, "Cold" },
    { ITEM_EFFECT_TYPE_FIRE, "Fire" },
    { ITEM_EFFECT_TYPE_POISON, "Poison" },
    { ITEM_EFFECT_TYPE_ARCANE, "Arcane" },
    { ITEM_EFFECT_TYPE_WITCHDOCTOR_DAMAGE, "WitchdoctorDamage" },
    { ITEM_EFFECT_TYPE_LIFESTEAL, "LifeSteal" },
    { ITEM_EFFECT_TYPE_MANASTEAL, "ManaSteal" },
    { ITEM_EFFECT_TYPE_MAGIC_FIND, "MagicFind" },
    { ITEM_EFFECT_TYPE_GOLD_FIND, "GoldFind" },
    { ITEM_EFFECT_TYPE_ATTACK_SPEED_BONUS, "AttackSpeedBonus" },
    { ITEM_EFFECT_TYPE_CAST_SPEED_BONUS, "CastSpeedBonus" },
    { ITEM_EFFECT_TYPE_HOLY, "Holy" },
    { ITEM_EFFECT_TYPE_WIZARD_DAMAGE, "WizardDamage" },
    { 0, NULL },
};

// line 1292
extern const SymbolicEntry gControllerInputIcon[25];
const SymbolicEntry gControllerInputIcon[25] = {
    { Console::CONTROLLER_INPUT_INVALID, "" },
    { Console::CONTROLLER_INPUT_LEFT_STICK, "{icon:l_stick,2.0}" },
    { Console::CONTROLLER_INPUT_LEFT_STICK_BUTTON, "{icon:l3,2.0}" },
    { Console::CONTROLLER_INPUT_RIGHT_STICK, "{icon:r_stick,2.0}" },
    { Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON, "{icon:r3,2.0}" },
    { Console::CONTROLLER_INPUT_BUTTON_1, "{icon:button1,2.0}" },
    { Console::CONTROLLER_INPUT_BUTTON_2, "{icon:button2,2.0}" },
    { Console::CONTROLLER_INPUT_BUTTON_3, "{icon:button3,2.0}" },
    { Console::CONTROLLER_INPUT_BUTTON_4, "{icon:button4,2.0}" },
    { Console::CONTROLLER_INPUT_LEFT_SHOULDER, "{icon:l1,2.5}" },
    { Console::CONTROLLER_INPUT_RIGHT_SHOULDER, "{icon:r1,2.5}" },
    { Console::CONTROLLER_INPUT_LEFT_TRIGGER, "{icon:l2,2.0}" },
    { Console::CONTROLLER_INPUT_RIGHT_TRIGGER, "{icon:r2,2.0}" },
    { Console::CONTROLLER_INPUT_DPAD_UP, "{icon:up,2.0}" },
    { Console::CONTROLLER_INPUT_DPAD_DOWN, "{icon:down,2.0}" },
    { Console::CONTROLLER_INPUT_DPAD_LEFT, "{icon:left,2.0}" },
    { Console::CONTROLLER_INPUT_DPAD_RIGHT, "{icon:right,2.0}" },
    { Console::CONTROLLER_INPUT_DPAD_HORIZONTAL, "{icon:Dpad_H_Button,2.0}" },
    { Console::CONTROLLER_INPUT_DPAD_VERTICAL, "{icon:Dpad_V_Button,2.0}" },
    { Console::CONTROLLER_INPUT_BACK, "{icon:plus,2.0}" },
    { Console::CONTROLLER_INPUT_START, "{icon:minus,2.0}" },
    { Console::CONTROLLER_INPUT_LEFT_STICK_HORIZONTAL, "{icon:l_stick_H_Button,2.0}" },
    { Console::CONTROLLER_INPUT_LEFT_STICK_VERTICAL, "{icon:l_stick_V_Button,2.0}" },
    { Console::CONTROLLER_INPUT_OPTIONS, "{icon:options,2.0}" },
    { 0, NULL },
};

// line 1332
extern const SymbolicEntry gKeysTex[154];
const SymbolicEntry gKeysTex[154] = {
    { KEY_NONE, "" },
    { KEY_ESCAPE, "ESCAPE" },
    { KEY_1, "1" },
    { KEY_2, "2" },
    { KEY_3, "3" },
    { KEY_4, "4" },
    { KEY_5, "5" },
    { KEY_6, "6" },
    { KEY_7, "7" },
    { KEY_8, "8" },
    { KEY_9, "9" },
    { KEY_0, "0" },
    { KEY_MINUS, "MINUS" },
    { KEY_EQUALS, "EQUALS" },
    { KEY_BACK, "BACK" },
    { KEY_TAB, "TAB" },
    { KEY_Q, "Q" },
    { KEY_W, "W" },
    { KEY_E, "E" },
    { KEY_R, "R" },
    { KEY_T, "T" },
    { KEY_Y, "Y" },
    { KEY_U, "U" },
    { KEY_I, "I" },
    { KEY_O, "O" },
    { KEY_P, "P" },
    { KEY_LBRACKET, "LBRACKET" },
    { KEY_RBRACKET, "RBRACKET" },
    { KEY_RETURN, "RETURN" },
    { KEY_LCONTROL, "LCONTROL" },
    { KEY_A, "A" },
    { KEY_S, "S" },
    { KEY_D, "D" },
    { KEY_F, "F" },
    { KEY_G, "G" },
    { KEY_H, "H" },
    { KEY_J, "J" },
    { KEY_K, "K" },
    { KEY_L, "L" },
    { KEY_SEMICOLON, "SEMICOLON" },
    { KEY_APOSTROPHE, "APOSTROPHE" },
    { KEY_GRAVE, "GRAVE" },
    { KEY_LSHIFT, "LSHIFT" },
    { KEY_BACKSLASH, "BACKSLASH" },
    { KEY_Z, "Z" },
    { KEY_X, "X" },
    { KEY_C, "C" },
    { KEY_V, "V" },
    { KEY_B, "B" },
    { KEY_N, "N" },
    { KEY_M, "M" },
    { KEY_COMMA, "COMMA" },
    { KEY_PERIOD, "PERIOD" },
    { KEY_SLASH, "SLASH" },
    { KEY_RSHIFT, "RSHIFT" },
    { KEY_MULTIPLY, "MULTIPLY" },
    { KEY_LMENU, "LMENU" },
    { KEY_SPACE, "SPACE" },
    { KEY_CAPITAL, "CAPITAL" },
    { KEY_F1, "F1" },
    { KEY_F2, "F2" },
    { KEY_F3, "F3" },
    { KEY_F4, "F4" },
    { KEY_F5, "F5" },
    { KEY_F6, "F6" },
    { KEY_F7, "F7" },
    { KEY_F8, "F8" },
    { KEY_F9, "F9" },
    { KEY_F10, "F10" },
    { KEY_NUMLOCK, "NUMLOCK" },
    { KEY_SCROLL, "SCROLL" },
    { KEY_NUMPAD7, "NUMPAD7" },
    { KEY_NUMPAD8, "NUMPAD8" },
    { KEY_NUMPAD9, "NUMPAD9" },
    { KEY_SUBTRACT, "SUBTRACT" },
    { KEY_NUMPAD4, "NUMPAD4" },
    { KEY_NUMPAD5, "NUMPAD5" },
    { KEY_NUMPAD6, "NUMPAD6" },
    { KEY_ADD, "ADD" },
    { KEY_NUMPAD1, "NUMPAD1" },
    { KEY_NUMPAD2, "NUMPAD2" },
    { KEY_NUMPAD3, "NUMPAD3" },
    { KEY_NUMPAD0, "NUMPAD0" },
    { KEY_DECIMAL, "DECIMAL" },
    { KEY_OEM_102, "OEM_102" },
    { KEY_F11, "F11" },
    { KEY_F12, "F12" },
    { KEY_F13, "F13" },
    { KEY_F14, "F14" },
    { KEY_F15, "F15" },
    { KEY_KANA, "KANA" },
    { KEY_ABNT_C1, "ABNT_C1" },
    { KEY_CONVERT, "CONVERT" },
    { KEY_NOCONVERT, "NOCONVERT" },
    { KEY_YEN, "YEN" },
    { KEY_ABNT_C2, "ABNT_C2" },
    { KEY_NUMPADEQUALS, "NUMPADEQUALS" },
    { KEY_PREVTRACK, "PREVTRACK" },
    { KEY_AT, "AT" },
    { KEY_COLON, "COLON" },
    { KEY_UNDERLINE, "UNDERLINE" },
    { KEY_KANJI, "KANJI" },
    { KEY_STOP, "STOP" },
    { KEY_AX, "AX" },
    { KEY_UNLABELED, "UNLABELED" },
    { KEY_NEXTTRACK, "NEXTTRACK" },
    { KEY_NUMPADENTER, "NUMPADENTER" },
    { KEY_RCONTROL, "RCONTROL" },
    { KEY_MUTE, "MUTE" },
    { KEY_CALCULATOR, "CALCULATOR" },
    { KEY_PLAYPAUSE, "PLAYPAUSE" },
    { KEY_MEDIASTOP, "MEDIASTOP" },
    { KEY_VOLUMEDOWN, "VOLUMEDOWN" },
    { KEY_VOLUMEUP, "VOLUMEUP" },
    { KEY_WEBHOME, "WEBHOME" },
    { KEY_NUMPADCOMMA, "NUMPADCOMMA" },
    { KEY_DIVIDE, "DIVIDE" },
    { KEY_SYSRQ, "SYSRQ" },
    { KEY_RMENU, "RMENU" },
    { KEY_PAUSE, "PAUSE" },
    { KEY_HOME, "HOME" },
    { KEY_UP, "UP" },
    { KEY_PRIOR, "PRIOR" },
    { KEY_LEFT, "LEFT" },
    { KEY_RIGHT, "RIGHT" },
    { KEY_END, "END" },
    { KEY_DOWN, "DOWN" },
    { KEY_NEXT, "NEXT" },
    { KEY_INSERT, "INSERT" },
    { KEY_DELETE, "DELETE" },
    { KEY_LWIN, "LWIN" },
    { KEY_RWIN, "RWIN" },
    { KEY_APPS, "APPS" },
    { KEY_POWER, "Power" },
    { KEY_SLEEP, "SLEEP" },
    { KEY_WAKE, "WAKE" },
    { KEY_WEBSEARCH, "WEBSEARCH" },
    { KEY_WEBFAVORITES, "WEBFAVORITES" },
    { KEY_WEBREFRESH, "WEBREFRESH" },
    { KEY_WEBSTOP, "WEBSTOP" },
    { KEY_WEBFORWARD, "WEBFORWARD" },
    { KEY_WEBBACK, "WEBBACK" },
    { KEY_MYCOMPUTER, "MYCOMPUTER" },
    { KEY_MAIL, "MAIL" },
    { KEY_MEDIASELECT, "MEDIASELECT" },
    { KEY_MOUSE1, "Mouse1" },
    { KEY_MOUSE2, "Mouse2" },
    { KEY_MOUSE3, "Mouse3" },
    { KEY_MOUSE4, "Mouse4" },
    { KEY_MOUSE5, "Mouse5" },
    { KEY_MOUSEWHEELUP, "MWheelUp" },
    { KEY_MOUSEWHEELDOWN, "MWheelDown" },
    { KEY_OEM_8, "OEM_8" },
    { 0, NULL },
};

// line 1502
extern const SymbolicEntry gMonsterRarity[8];
const SymbolicEntry gMonsterRarity[8] = {
    { MR_NORMAL, "" },
    { MR_CHAMPION, "Champion" },
    { MR_RARE, "Rare" },
    { MR_RARE_MINION, "Minion" },
    { MR_UNIQUE, "Unique" },
    { MR_HIRELING, "Hireling" },
    { MR_BOSS, "Boss" },
    { 0, NULL },
};

// line 1664
extern const SymbolicEntry gGameBalanceTypes[35];
const SymbolicEntry gGameBalanceTypes[35] = {
    { GB_ITEMTYPES, "Item Types" },
    { GB_ITEMS, "Items" },
    { GB_EXPERIENCETABLE, "Experience Table" },
    { GB_HELPCODES, "Help Codes" },
    { GB_MONSTER_LEVELS, "Monster Levels" },
    { GB_HEROS, "Heros" },
    { GB_AFFIXES, "Affix List" },
    { GB_MOVEMENT_STYLES, "Movement Styles" },
    { GB_LABELS, "Labels" },
    { GB_LOOTDISTRIBUTION, "Loot Distribution" },
    { GB_RARE_ITEM_NAMES, "Rare Item Names" },
    { GB_MONSTER_AFFIXES, "Monster Affixes" },
    { GB_MONSTER_NAMES, "Monster Names" },
    { GB_SOCKETED_EFFECTS, "Socketed Effects" },
    { GB_ITEM_DROP_TABLE, "Item Drop Table" },
    { GB_ITEM_LEVEL_MODIFIERS, "Item Level Modifiers" },
    { GB_ITEM_QUALITY_CLASSES, "Quality Classes" },
    { GB_HANDICAPS, "Handicaps" },
    { GB_ITEM_SALVAGE_LEVELS, "Item Salvage Levels" },
    { GB_SCENERY, "Scenery" },
    { GB_HIRELINGS, "Hirelings" },
    { GB_SET_ITEM_BONUSES, "Set Item Bonuses" },
    { GB_ELITE_MODIFIERS, "Elite Modifiers" },
    { GB_ITEM_TIERS, "Item Tiers" },
    { GB_POWER_FORMULA_TABLES, "Power Formula Tables" },
    { GB_RECIPES, "Recipes" },
    { GB_SCRIPTED_ACHIEVEMENT_EVENTS, "Scripted Achievement Events" },
    { GB_LOOTRUN_QUEST_TIERS, "LootRun Quest Tiers" },
    { GB_PARAGON_BONUSES, "Paragon Bonuses" },
    { GB_LEGACY_ITEM_CONVERSIONS, "Legacy Item Conversions" },
    { GB_ENCHANT_ITEM_AFFIX_USE_COUNT_COST_SCALARS, "Enchant Item Affix Use Count Cost Scalars" },
    { GB_TIERED_LOOT_RUN_LEVELS, "Tiered Loot Run Levels" },
    { GB_TRANSMUTE_RECIPES, "Transmute Recipes" },
    { GB_CURRENCY_CONVERSIONS, "Currency Conversions" },
    { 0, NULL },
};

// line 1704
extern const SymbolicEntry gAttributeSources[5];
const SymbolicEntry gAttributeSources[5] = {
    { ATTRIBSOURCE_NONE, "None" },
    { ATTRIBSOURCE_DEFAULT_ACTOR, "Default Actor" },
    { ATTRIBSOURCE_TARGET, "Target Actor" },
    { ATTRIBSOURCE_GAME, "Game" },
    { 0, NULL },
};

// line 1715
extern const SymbolicEntry gAttributeValues[5];
const SymbolicEntry gAttributeValues[5] = {
    { ATTRIBVALUE_NONE, "None" },
    { ATTRIBVALUE_TOTAL, "Total" },
    { ATTRIBVALUE_AGG, "Agg" },
    { ATTRIBVALUE_BASE, "Base" },
    { 0, NULL },
};

// line 2250
extern const SymbolicEntry gDeliveryMechanismTypes[7];
const SymbolicEntry gDeliveryMechanismTypes[7] = {
    { PDM_UNKNOWN, "Unknown" },
    { PDM_MELEE, "Melee" },
    { PDM_RANGED, "Ranged" },
    { PDM_MAGIC, "Magic" },
    { PDM_DOT, "DOT" },
    { PDM_THORNS, "Thorns" },
    { 0, NULL },
};

// line 2264
extern const SymbolicEntry gDRLGTileTypes[18];
const SymbolicEntry gDRLGTileTypes[18] = {
    { DRLG_TILE_NONE, "NONE" },
    { DRLG_TILE_N, "N" },
    { DRLG_TILE_S, "S" },
    { DRLG_TILE_NS, "NS" },
    { DRLG_TILE_E, "E" },
    { DRLG_TILE_NE, "NE" },
    { DRLG_TILE_SE, "SE" },
    { DRLG_TILE_NSE, "NSE" },
    { DRLG_TILE_W, "W" },
    { DRLG_TILE_NW, "NW" },
    { DRLG_TILE_SW, "SW" },
    { DRLG_TILE_NSW, "NSW" },
    { DRLG_TILE_EW, "EW" },
    { DRLG_TILE_NEW, "NEW" },
    { DRLG_TILE_SEW, "SEW" },
    { DRLG_TILE_NSEW, "NSEW" },
    { DRLG_TILE_CUSTOM, "CUSTOM" },
    { 0, NULL },
};

// line 2289
extern const SymbolicEntry gDRLGTileSets[124];
const SymbolicEntry gDRLGTileSets[124] = {
    { GRIDDATA_TILE_SET_NONE, "None" },
    { GRIDDATA_TILE_SET_RAT_MAZE_1, "Rat Maze 1" },
    { GRIDDATA_TILE_SET_RAT_MAZE_2, "Rat Maze 2" },
    { GRIDDATA_TILE_SET_RAT_MAZE_3, "Rat Maze 3" },
    { GRIDDATA_TILE_SET_RAT_MAZE_4, "Rat Maze 4" },
    { GRIDDATA_TILE_SET_RAT_MAZE_5, "Rat Maze 5" },
    { GRIDDATA_TILE_SET_RAT_MAZE_6, "Rat Maze 6" },
    { GRIDDATA_TILE_SET_RAT_MAZE_7, "Rat Maze 7" },
    { GRIDDATA_TILE_SET_RAT_MAZE_8, "Rat Maze 8" },
    { GRIDDATA_TILE_SET_RAT_MAZE_9, "Rat Maze 9" },
    { GRIDDATA_TILE_SET_RAT_MAZE_10, "Rat Maze 10" },
    { GRIDDATA_TILE_SET_RAT_MAZE_11, "Rat Maze 11" },
    { GRIDDATA_TILE_SET_RAT_MAZE_12, "Rat Maze 12" },
    { GRIDDATA_TILE_SET_RAT_MAZE_13, "Rat Maze 13" },
    { GRIDDATA_TILE_SET_RAT_MAZE_14, "Rat Maze 14" },
    { GRIDDATA_TILE_SET_RAT_MAZE_15, "Rat Maze 15" },
    { GRIDDATA_TILE_SET_RAT_MAZE_16, "Rat Maze 16" },
    { GRIDDATA_TILE_SET_RAT_MAZE_17, "Rat Maze 17" },
    { GRIDDATA_TILE_SET_RAT_MAZE_18, "Rat Maze 18" },
    { GRIDDATA_TILE_SET_RAT_MAZE_19, "Rat Maze 19" },
    { GRIDDATA_TILE_SET_RAT_MAZE_20, "Rat Maze 20" },
    { GRIDDATA_TILE_SET_ENTRANCE_1, "Entrance 1" },
    { GRIDDATA_TILE_SET_ENTRANCE_2, "Entrance 2" },
    { GRIDDATA_TILE_SET_ENTRANCE_3, "Entrance 3" },
    { GRIDDATA_TILE_SET_ENTRANCE_4, "Entrance 4" },
    { GRIDDATA_TILE_SET_ENTRANCE_5, "Entrance 5" },
    { GRIDDATA_TILE_SET_ENTRANCE_6, "Entrance 6" },
    { GRIDDATA_TILE_SET_ENTRANCE_7, "Entrance 7" },
    { GRIDDATA_TILE_SET_ENTRANCE_8, "Entrance 8" },
    { GRIDDATA_TILE_SET_ENTRANCE_9, "Entrance 9" },
    { GRIDDATA_TILE_SET_ENTRANCE_10, "Entrance 10" },
    { GRIDDATA_TILE_SET_ENTRANCE_11, "Entrance 11" },
    { GRIDDATA_TILE_SET_ENTRANCE_12, "Entrance 12" },
    { GRIDDATA_TILE_SET_ENTRANCE_13, "Entrance 13" },
    { GRIDDATA_TILE_SET_ENTRANCE_14, "Entrance 14" },
    { GRIDDATA_TILE_SET_ENTRANCE_15, "Entrance 15" },
    { GRIDDATA_TILE_SET_ENTRANCE_16, "Entrance 16" },
    { GRIDDATA_TILE_SET_ENTRANCE_17, "Entrance 17" },
    { GRIDDATA_TILE_SET_ENTRANCE_18, "Entrance 18" },
    { GRIDDATA_TILE_SET_ENTRANCE_19, "Entrance 19" },
    { GRIDDATA_TILE_SET_ENTRANCE_20, "Entrance 20" },
    { GRIDDATA_TILE_SET_EXIT_1, "Exit 1" },
    { GRIDDATA_TILE_SET_EXIT_2, "Exit 2" },
    { GRIDDATA_TILE_SET_EXIT_3, "Exit 3" },
    { GRIDDATA_TILE_SET_EXIT_4, "Exit 4" },
    { GRIDDATA_TILE_SET_EXIT_5, "Exit 5" },
    { GRIDDATA_TILE_SET_EXIT_6, "Exit 6" },
    { GRIDDATA_TILE_SET_EXIT_7, "Exit 7" },
    { GRIDDATA_TILE_SET_EXIT_8, "Exit 8" },
    { GRIDDATA_TILE_SET_EXIT_9, "Exit 9" },
    { GRIDDATA_TILE_SET_EXIT_10, "Exit 10" },
    { GRIDDATA_TILE_SET_EXIT_11, "Exit 11" },
    { GRIDDATA_TILE_SET_EXIT_12, "Exit 12" },
    { GRIDDATA_TILE_SET_EXIT_13, "Exit 13" },
    { GRIDDATA_TILE_SET_EXIT_14, "Exit 14" },
    { GRIDDATA_TILE_SET_EXIT_15, "Exit 15" },
    { GRIDDATA_TILE_SET_EXIT_16, "Exit 16" },
    { GRIDDATA_TILE_SET_EXIT_17, "Exit 17" },
    { GRIDDATA_TILE_SET_EXIT_18, "Exit 18" },
    { GRIDDATA_TILE_SET_EXIT_19, "Exit 19" },
    { GRIDDATA_TILE_SET_EXIT_20, "Exit 20" },
    { GRIDDATA_TILE_SET_FEATURE_1, "Feature 1" },
    { GRIDDATA_TILE_SET_FEATURE_2, "Feature 2" },
    { GRIDDATA_TILE_SET_FEATURE_3, "Feature 3" },
    { GRIDDATA_TILE_SET_FEATURE_4, "Feature 4" },
    { GRIDDATA_TILE_SET_FEATURE_5, "Feature 5" },
    { GRIDDATA_TILE_SET_FEATURE_6, "Feature 6" },
    { GRIDDATA_TILE_SET_FEATURE_7, "Feature 7" },
    { GRIDDATA_TILE_SET_FEATURE_8, "Feature 8" },
    { GRIDDATA_TILE_SET_FEATURE_9, "Feature 9" },
    { GRIDDATA_TILE_SET_FEATURE_10, "Feature 10" },
    { GRIDDATA_TILE_SET_FEATURE_11, "Feature 11" },
    { GRIDDATA_TILE_SET_FEATURE_12, "Feature 12" },
    { GRIDDATA_TILE_SET_FEATURE_13, "Feature 13" },
    { GRIDDATA_TILE_SET_FEATURE_14, "Feature 14" },
    { GRIDDATA_TILE_SET_FEATURE_15, "Feature 15" },
    { GRIDDATA_TILE_SET_FEATURE_16, "Feature 16" },
    { GRIDDATA_TILE_SET_FEATURE_17, "Feature 17" },
    { GRIDDATA_TILE_SET_FEATURE_18, "Feature 18" },
    { GRIDDATA_TILE_SET_FEATURE_19, "Feature 19" },
    { GRIDDATA_TILE_SET_FEATURE_20, "Feature 20" },
    { GRIDDATA_TILE_SET_TRANSITION_1, "Transition 1" },
    { GRIDDATA_TILE_SET_TRANSITION_2, "Transition 2" },
    { GRIDDATA_TILE_SET_TRANSITION_3, "Transition 3" },
    { GRIDDATA_TILE_SET_TRANSITION_4, "Transition 4" },
    { GRIDDATA_TILE_SET_TRANSITION_5, "Transition 5" },
    { GRIDDATA_TILE_SET_TRANSITION_6, "Transition 6" },
    { GRIDDATA_TILE_SET_TRANSITION_7, "Transition 7" },
    { GRIDDATA_TILE_SET_TRANSITION_8, "Transition 8" },
    { GRIDDATA_TILE_SET_TRANSITION_9, "Transition 9" },
    { GRIDDATA_TILE_SET_TRANSITION_10, "Transition 10" },
    { GRIDDATA_TILE_SET_TRANSITION_11, "Transition 11" },
    { GRIDDATA_TILE_SET_TRANSITION_12, "Transition 12" },
    { GRIDDATA_TILE_SET_TRANSITION_13, "Transition 13" },
    { GRIDDATA_TILE_SET_TRANSITION_14, "Transition 14" },
    { GRIDDATA_TILE_SET_TRANSITION_15, "Transition 15" },
    { GRIDDATA_TILE_SET_TRANSITION_16, "Transition 16" },
    { GRIDDATA_TILE_SET_TRANSITION_17, "Transition 17" },
    { GRIDDATA_TILE_SET_TRANSITION_18, "Transition 18" },
    { GRIDDATA_TILE_SET_TRANSITION_19, "Transition 19" },
    { GRIDDATA_TILE_SET_TRANSITION_20, "Transition 20" },
    { GRIDDATA_TILE_SET_COMPOUND_1, "Compound 1" },
    { GRIDDATA_TILE_SET_COMPOUND_2, "Compound 2" },
    { GRIDDATA_TILE_SET_COMPOUND_3, "Compound 3" },
    { GRIDDATA_TILE_SET_COMPOUND_4, "Compound 4" },
    { GRIDDATA_TILE_SET_COMPOUND_5, "Compound 5" },
    { GRIDDATA_TILE_SET_COMPOUND_6, "Compound 6" },
    { GRIDDATA_TILE_SET_COMPOUND_7, "Compound 7" },
    { GRIDDATA_TILE_SET_COMPOUND_8, "Compound 8" },
    { GRIDDATA_TILE_SET_COMPOUND_9, "Compound 9" },
    { GRIDDATA_TILE_SET_COMPOUND_10, "Compound 10" },
    { GRIDDATA_TILE_SET_COMPOUND_11, "Compound 11" },
    { GRIDDATA_TILE_SET_COMPOUND_12, "Compound 12" },
    { GRIDDATA_TILE_SET_COMPOUND_13, "Compound 13" },
    { GRIDDATA_TILE_SET_COMPOUND_14, "Compound 14" },
    { GRIDDATA_TILE_SET_COMPOUND_15, "Compound 15" },
    { GRIDDATA_TILE_SET_COMPOUND_16, "Compound 16" },
    { GRIDDATA_TILE_SET_COMPOUND_17, "Compound 17" },
    { GRIDDATA_TILE_SET_COMPOUND_18, "Compound 18" },
    { GRIDDATA_TILE_SET_COMPOUND_19, "Compound 19" },
    { GRIDDATA_TILE_SET_COMPOUND_20, "Compound 20" },
    { GRIDDATA_TILE_SET_DUMMY, "Dummy" },
    { GRIDDATA_TILE_SET_PRESENT_SCENE, "Pre-Set Scene" },
    { 0, NULL },
};

// line 2591
extern const SymbolicEntry gActs[9];
const SymbolicEntry gActs[9] = {
    { ACT_INVALID, "Invalid" },
    { ACT_1, "A1" },
    { ACT_2, "A2" },
    { ACT_3, "A3" },
    { ACT_4, "A4" },
    { ACT_5, "A5" },
    { ACT_OPEN_WORLD, "OpenWorld" },
    { ACT_TEST, "Test" },
    { 0, NULL },
};

// line 2732
extern const SymbolicEntry gBodySlots[22];
const SymbolicEntry gBodySlots[22] = {
    { SLOT_PLAYER_BACKPACK, "Player:Backpack" },
    { SLOT_PLAYER_HEAD, "Player:Head" },
    { SLOT_PLAYER_TORSO, "Player:Torso" },
    { SLOT_PLAYER_RIGHT_HAND, "Player:RightHand" },
    { SLOT_PLAYER_LEFT_HAND, "Player:LeftHand" },
    { SLOT_PLAYER_HANDS, "Player:Hands" },
    { SLOT_PLAYER_WAIST, "Player:Waist" },
    { SLOT_PLAYER_FEET, "Player:Feet" },
    { SLOT_PLAYER_SHOULDERS, "Player:Shoulders" },
    { SLOT_PLAYER_LEGS, "Player:Legs" },
    { SLOT_PLAYER_BRACERS, "Player:Bracers" },
    { SLOT_PLAYER_LEFT_FINGER, "Player:LeftFinger" },
    { SLOT_PLAYER_RIGHT_FINGER, "Player:RightFinger" },
    { SLOT_PLAYER_NECK, "Player:Neck" },
    { SLOT_MERCHANT_SHOPLIST, "Merchant" },
    { SLOT_HIRELING_RIGHT_HAND, "Pet:RightHand" },
    { SLOT_HIRELING_LEFT_HAND, "Pet:LeftHand" },
    { SLOT_HIRELING_SPECIAL, "Pet:Special" },
    { SLOT_HIRELING_LEFT_FINGER, "Pet:LeftFinger" },
    { SLOT_HIRELING_RIGHT_FINGER, "Pet:RightFinger" },
    { SLOT_HIRELING_NECK, "Pet:Neck" },
    { 0, NULL },
};

// line 2804
extern const SymbolicEntry gItemQualityLevelNames[14];
const SymbolicEntry gItemQualityLevelNames[14] = {
    { IQL_INVALID, "Invalid" },
    { IQL_INFERIOR, "Inferior" },
    { IQL_NORMAL, "Normal" },
    { IQL_SUPERIOR, "Superior" },
    { IQL_MAGIC_1_AFFIX, "Magic1" },
    { IQL_MAGIC_2_AFFIX, "Magic2" },
    { IQL_MAGIC_3_AFFIX, "Magic3" },
    { IQL_RARE_4_AFFIX, "Rare4" },
    { IQL_RARE_5_AFFIX, "Rare5" },
    { IQL_RARE_6_AFFIX, "Rare6" },
    { IQL_LEGENDARY, "Legendary" },
    { IQL_SPECIAL, "Special" },
    { IQL_DISPLAY_AS_SET_ITEM, "Set" },
    { 0, NULL },
};

// line 2824
extern const SymbolicEntry gGemTypeNames[6];
const SymbolicEntry gGemTypeNames[6] = {
    { GEM_AMETHYST, "Amethyst" },
    { GEM_EMERALD, "Emerald" },
    { GEM_RUBY, "Ruby" },
    { GEM_TOPAZ, "Topaz" },
    { GEM_DIAMOND, "Diamond" },
    { 0, NULL },
};

// line 2837
extern const SymbolicEntry gCosmeticTypeNames[5];
const SymbolicEntry gCosmeticTypeNames[5] = {
    { COSMETIC_ITEM_WINGS, "CosmeticsWings" },
    { COSMETIC_ITEM_PET, "CosmeticsPets" },
    { COSMETIC_ITEM_PENNANT, "CosmeticsPennants" },
    { COSMETIC_ITEM_FRAME, "CosmeticsPortraitFrames" },
    { 0, NULL },
};

// line 2848
extern const SymbolicEntry gCraftingQualityLevelNames[5];
const SymbolicEntry gCraftingQualityLevelNames[5] = {
    { CQL_COMMON, "A" },
    { CQL_UNCOMMON, "B" },
    { CQL_RARE, "C" },
    { CQL_VERY_RARE, "D" },
    { 0, NULL },
};

// line 2859
extern const SymbolicEntry gAffixTypes[10];
const SymbolicEntry gAffixTypes[10] = {
    { AT_PREFIX, "Prefix" },
    { AT_SUFFIX, "Suffix" },
    { AT_INHERENT, "Inherent" },
    { AT_TITLE, "Title" },
    { AT_QUALITY, "Quality" },
    { AT_IMMUNITY, "Immunity" },
    { AT_RANDOM, "Random" },
    { AT_ENCHANTMENT, "Enchantment" },
    { AT_ADD_SOCKET, "Socket Enhancement" },
    { 0, NULL },
};

// line 2874
extern const SymbolicEntry gAffixGroups[3];
const SymbolicEntry gAffixGroups[3] = {
    { AFFIXGROUP_MAJOR, "Major" },
    { AFFIXGROUP_MINOR, "Minor" },
    { 0, NULL },
};

// line 2883
extern const SymbolicEntry gTransmuteRecipeActionTypes[18];
const SymbolicEntry gTransmuteRecipeActionTypes[18] = {
    { TRA_NONE, "- None -" },
    { TRA_EXTRACT_LEGENDARY_POWER, "[EXTRACT LEGENDARY POWER]" },
    { TRA_REFORGE_LEGENDARY, "[REFORGE LEGENDARY]" },
    { TRA_UPGRADE_RARE_ITEM, "[UPGRADE RARE ITEM]" },
    { TRA_CONVERT_SET_ITEM, "[CONVERT SET ITEM]" },
    { TRA_REMOVE_LEVEL_REQ, "[REMOVE LEVEL REQ]" },
    { TRA_CONVERT_CRAFTING_MATS_FROM_NORMAL, "[CONVERT CRAFTING MATS FROM NORMAL]" },
    { TRA_CONVERT_CRAFTING_MATS_FROM_MAGIC, "[CONVERT CRAFTING MATS FROM MAGIC]" },
    { TRA_CONVERT_CRAFTING_MATS_FROM_RARE, "[CONVERT CRAFTING MATS FROM RARE]" },
    { TRA_UPGRADE_ITEM_TO_PLAYER_LEVEL, "[UPGRADE ITEM TO PLAYER LEVEL]" },
    { TRA_CONVERT_GEMS, "[CONVERT GEMS]" },
    { TRA_OPEN_PORTAL_GREED, "[OPEN PORTAL TO GREED]" },
    { TRA_OPEN_PORTAL_COW, "[OPEN PORTAL TO COW]" },
    { TRA_OPEN_PORTAL_WHIMSEYSHIRE, "[OPEN PORTAL TO WHIMSEYSHIRE]" },
    { TRA_AUGMENT_ARMOR, "[AUGMENT ARMOR]" },
    { TRA_CONVERT_PAGES, "[CONVERT BLACKROCK PAGES]" },
    { TRA_INFUSE_LEGENDARY_POWER, "[INFUSE LEGENDARY POWER]" },
    { 0, NULL },
};

// line 2907
extern const SymbolicEntry gTransmuteRecipeIngredientTypes[17];
const SymbolicEntry gTransmuteRecipeIngredientTypes[17] = {
    { TRIT_ANY, "[ANY]" },
    { TRIT_LEGENDARY, "[LEGENDARY ITEM]" },
    { TRIT_LEGENDARY_WITH_POWER, "[LEGENDARY ITEM POWER]" },
    { TRIT_RARE_EQUIPPABLE_ITEM, "[RARE EQUIPPABLE ITEM]" },
    { TRIT_SET_ITEM, "[SET ITEM]" },
    { TRIT_UPGRADED_GEM_OF_EASE, "[UPGRADED GEM OF EASE]" },
    { TRIT_EQUIPPABLE, "[EQUIPPABLE ITEM]" },
    { TRIT_NORMAL_MAGIC_RARE_ITEM, "[NORMAL MAGIC RARE ITEM]" },
    { TRIT_GEM, "[GEM]" },
    { TRIT_BASIC_CRAFTING_MAT, "[BASIC CRAFTING MAT]" },
    { TRIT_ACT_BOUNTY_REAGENT, "[ACT BOUNTY REAGENT]" },
    { TRIT_GEM_CONVERSION_REAGENT, "[GEM CONVERSION REAGENT]" },
    { TRIT_UPGRADED_LEGENDARY_GEM, "[UPGRADED LEGENDARY GEM]" },
    { TRIT_ANCIENT, "[ANCIENT]" },
    { TRIT_MAX_RANK_STANDARD_GEM, "[MAX RANK STANDARD GEM]" },
    { TRIT_BLACKROCK_PAGE, "[BLACKROCK LEDGER]" },
    { 0, NULL },
};

// line 2930
extern const SymbolicEntry gMonsterAffixValidFor[5];
const SymbolicEntry gMonsterAffixValidFor[5] = {
    { VALID_FOR_ALL, "All" },
    { VALID_FOR_RARES, "Rares" },
    { VALID_FOR_SHOOTERS, "Shooters" },
    { VALID_FOR_CHAMPION, "Champions" },
    { 0, NULL },
};

// line 2941
extern const SymbolicEntry gMonsterTypes[15];
const SymbolicEntry gMonsterTypes[15] = {
    { MT_INVALID, "" },
    { MT_UNDEAD, "Undead" },
    { MT_DEMON, "Demon" },
    { MT_BEAST, "Beast" },
    { MT_HUMAN, "Human" },
    { MT_BREAKABLE, "Breakable" },
    { MT_SCENERY, "Scenery" },
    { MT_ALLY, "Ally" },
    { MT_TEAM, "Team" },
    { MT_HELPER, "Helper" },
    { MT_CORRUPTED_ANGEL, "CorruptedAngel" },
    { MT_PANDEMONIUM, "Pandemonium" },
    { MT_ADRIA, "Adria" },
    { MT_BLOOD_GOLEM, "Blood Golem" },
    { 0, NULL },
};

// line 2962
extern const SymbolicEntry gMonsterFamily[14];
const SymbolicEntry gMonsterFamily[14] = {
    { MF_NONE, "" },
    { MF_FALLEN, "Fallen" },
    { MF_GOATMEN, "GoatMen" },
    { MF_ROGUE, "Rogue" },
    { MF_SKELETON, "Skeleton" },
    { MF_ZOMBIE, "Zombie" },
    { MF_SPIDER, "Spider" },
    { MF_TRIUNE, "Triune" },
    { MF_WOODWRAITH, "WoodWraith" },
    { MF_HUMAN, "Human" },
    { MF_ANIMAL, "Animal" },
    { MF_TREASURE_GOBLIN, "TreasureGoblin" },
    { MF_CRAZED_ANGEL, "CrazedAngel" },
    { 0, NULL },
};

// line 2982
extern const SymbolicEntry gEncounterCategory[7];
const SymbolicEntry gEncounterCategory[7] = {
    { EC_NONE, "" },
    { MC_BIG, "Big" },
    { MC_STANDARD, "Standard" },
    { MC_RANGED, "Ranged" },
    { MC_SWARM, "Swarm" },
    { MC_BOSS, "Boss" },
    { 0, NULL },
};

// line 3019
extern const SymbolicEntry gHardpointNames[41];
const SymbolicEntry gHardpointNames[41] = {
    { (int32)StringHash(DEFAULT_HARDPOINT_NAMESTR), DEFAULT_HARDPOINT_NAMESTR },
    { (int32)StringHash(LEFT_FOOT_HARDPOINT_NAMESTR), LEFT_FOOT_HARDPOINT_NAMESTR },
    { (int32)StringHash(RIGHT_FOOT_HARDPOINT_NAMESTR), RIGHT_FOOT_HARDPOINT_NAMESTR },
    { (int32)StringHash(LEFT_HAND_HARDPOINT_NAMESTR), LEFT_HAND_HARDPOINT_NAMESTR },
    { (int32)StringHash(RIGHT_HAND_HARDPOINT_NAMESTR), RIGHT_HAND_HARDPOINT_NAMESTR },
    { (int32)StringHash(CHEST_HARDPOINT_NAMESTR), CHEST_HARDPOINT_NAMESTR },
    { (int32)StringHash(BACK_HARDPOINT_NAMESTR), BACK_HARDPOINT_NAMESTR },
    { (int32)StringHash(AURA_HARDPOINT_NAMESTR), AURA_HARDPOINT_NAMESTR },
    { (int32)StringHash(CURSE_HARDPOINT_NAMESTR), CURSE_HARDPOINT_NAMESTR },
    { (int32)StringHash(LEFT_WEAPON_HARDPOINT_NAMESTR), LEFT_WEAPON_HARDPOINT_NAMESTR },
    { (int32)StringHash(RIGHT_WEAPON_HARDPOINT_NAMESTR), RIGHT_WEAPON_HARDPOINT_NAMESTR },
    { (int32)StringHash(LEFT_FIST_HARDPOINT_NAMESTR), LEFT_FIST_HARDPOINT_NAMESTR },
    { (int32)StringHash(RIGHT_FIST_HARDPOINT_NAMESTR), RIGHT_FIST_HARDPOINT_NAMESTR },
    { (int32)StringHash(LIGHT_HARDPOINT_NAMESTR), LIGHT_HARDPOINT_NAMESTR },
    { (int32)StringHash(SHIELD_HARDPOINT_NAMESTR), SHIELD_HARDPOINT_NAMESTR },
    { (int32)StringHash(MOUTH_HARDPOINT_NAMESTR), MOUTH_HARDPOINT_NAMESTR },
    { (int32)StringHash(MANA_GATHER_HARDPOINT_NAMESTR), MANA_GATHER_HARDPOINT_NAMESTR },
    { (int32)StringHash(HEAD_HARDPOINT_NAMESTR), HEAD_HARDPOINT_NAMESTR },
    { (int32)StringHash(HELM_HARDPOINT_NAMESTR), HELM_HARDPOINT_NAMESTR },
    { (int32)StringHash(LEFT_SHOULDER_HARDPOINT_NAMESTR), LEFT_SHOULDER_HARDPOINT_NAMESTR },
    { (int32)StringHash(RIGHT_SHOULDER_HARDPOINT_NAMESTR), RIGHT_SHOULDER_HARDPOINT_NAMESTR },
    { (int32)StringHash(CENTROID_HARDPOINT_NAMESTR), CENTROID_HARDPOINT_NAMESTR },
    { (int32)StringHash(CENTER_HARDPOINT_NAMESTR), CENTER_HARDPOINT_NAMESTR },
    { (int32)StringHash(TRAIL_1_NAMESTR), TRAIL_1_NAMESTR },
    { (int32)StringHash(TRAIL_2_NAMESTR), TRAIL_2_NAMESTR },
    { (int32)StringHash(TRAIL_3_NAMESTR), TRAIL_3_NAMESTR },
    { (int32)StringHash(TRAIL_4_NAMESTR), TRAIL_4_NAMESTR },
    { (int32)StringHash(SHEATH_LEFT_HARDPOINT_NAMESTR), SHEATH_LEFT_HARDPOINT_NAMESTR },
    { (int32)StringHash(SHEATH_RIGHT_HARDPOINT_NAMESTR), SHEATH_RIGHT_HARDPOINT_NAMESTR },
    { (int32)StringHash(SHEATH_LEFT_HIP_HARDPOINT_NAMESTR), SHEATH_LEFT_HIP_HARDPOINT_NAMESTR },
    { (int32)StringHash(SHEATH_RIGHT_HIP_HARDPOINT_NAMESTR), SHEATH_RIGHT_HIP_HARDPOINT_NAMESTR },
    { (int32)StringHash(SHEATH_SHIELD_HARDPOINT_NAMESTR), SHEATH_SHIELD_HARDPOINT_NAMESTR },
    { (int32)StringHash(PELVIS_HARDPOINT_NAMESTR), PELVIS_HARDPOINT_NAMESTR },
    { (int32)StringHash(ANIM_PERMUTATION_HARDPOINT_NAMESTR), ANIM_PERMUTATION_HARDPOINT_NAMESTR },
    { (int32)StringHash(EXPLOSION_HARDPOINT_NAMESTR), EXPLOSION_HARDPOINT_NAMESTR },
    { (int32)StringHash(BEAM_CENTER_HARDPOINT_NAMESTR), BEAM_CENTER_HARDPOINT_NAMESTR },
    { (int32)StringHash(BEAM_LEFT_HARDPOINT_NAMESTR), BEAM_LEFT_HARDPOINT_NAMESTR },
    { (int32)StringHash(BEAM_RIGHT_HARDPOINT_NAMESTR), BEAM_RIGHT_HARDPOINT_NAMESTR },
    { (int32)StringHash(ATTACH_HARDPOINT_NAMESTR), ATTACH_HARDPOINT_NAMESTR },
    { (int32)StringHash(LEGENDARY_FX_HARDPOINT_NAMESTR), LEGENDARY_FX_HARDPOINT_NAMESTR },
    { 0, NULL },
};

// line 3070
extern const SymbolicEntry gPhysicsHardpointNames[6];
const SymbolicEntry gPhysicsHardpointNames[6] = {
    { (int32)StringHashI(PHYSICS_1_HARDPOINT_NAMESTR), PHYSICS_1_HARDPOINT_NAMESTR },
    { (int32)StringHashI(PHYSICS_2_HARDPOINT_NAMESTR), PHYSICS_2_HARDPOINT_NAMESTR },
    { (int32)StringHashI(PHYSICS_3_HARDPOINT_NAMESTR), PHYSICS_3_HARDPOINT_NAMESTR },
    { (int32)StringHashI(PHYSICS_4_HARDPOINT_NAMESTR), PHYSICS_4_HARDPOINT_NAMESTR },
    { (int32)StringHashI(LEGENDARY_FX_HARDPOINT_NAMESTR), LEGENDARY_FX_HARDPOINT_NAMESTR },
    { 0, NULL },
};

// line 3086
extern const SymbolicEntry gLeftPhysicsHardpointNames[6];
const SymbolicEntry gLeftPhysicsHardpointNames[6] = {
    { (int32)StringHashI(LEFT_WEAPON_PHYSICS_1_HARDPOINT_NAMESTR), LEFT_WEAPON_PHYSICS_1_HARDPOINT_NAMESTR },
    { (int32)StringHashI(LEFT_WEAPON_PHYSICS_2_HARDPOINT_NAMESTR), LEFT_WEAPON_PHYSICS_2_HARDPOINT_NAMESTR },
    { (int32)StringHashI(LEFT_WEAPON_PHYSICS_3_HARDPOINT_NAMESTR), LEFT_WEAPON_PHYSICS_3_HARDPOINT_NAMESTR },
    { (int32)StringHashI(LEFT_WEAPON_PHYSICS_4_HARDPOINT_NAMESTR), LEFT_WEAPON_PHYSICS_4_HARDPOINT_NAMESTR },
    { (int32)StringHashI(LEFT_WEAPON_LEGENDARY_FX_HARDPOINT_NAMESTR), LEFT_WEAPON_LEGENDARY_FX_HARDPOINT_NAMESTR },
    { 0, NULL },
};

// line 3100
extern const SymbolicEntry gRightPhysicsHardpointNames[6];
const SymbolicEntry gRightPhysicsHardpointNames[6] = {
    { (int32)StringHashI(RIGHT_WEAPON_PHYSICS_1_HARDPOINT_NAMESTR), RIGHT_WEAPON_PHYSICS_1_HARDPOINT_NAMESTR },
    { (int32)StringHashI(RIGHT_WEAPON_PHYSICS_2_HARDPOINT_NAMESTR), RIGHT_WEAPON_PHYSICS_2_HARDPOINT_NAMESTR },
    { (int32)StringHashI(RIGHT_WEAPON_PHYSICS_3_HARDPOINT_NAMESTR), RIGHT_WEAPON_PHYSICS_3_HARDPOINT_NAMESTR },
    { (int32)StringHashI(RIGHT_WEAPON_PHYSICS_4_HARDPOINT_NAMESTR), RIGHT_WEAPON_PHYSICS_4_HARDPOINT_NAMESTR },
    { (int32)StringHashI(RIGHT_WEAPON_LEGENDARY_FX_HARDPOINT_NAMESTR), RIGHT_WEAPON_LEGENDARY_FX_HARDPOINT_NAMESTR },
    { 0, NULL },
};

// line 3148
extern const SymbolicEntry gDamageTypes[9];
const SymbolicEntry gDamageTypes[9] = {
    { DAMAGE_TYPE_NONE, "None" },
    { DAMAGE_TYPE_PHYSICAL, "Physical" },
    { DAMAGE_TYPE_FIRE, "Fire" },
    { DAMAGE_TYPE_LIGHTNING, "Lightning" },
    { DAMAGE_TYPE_COLD, "Cold" },
    { DAMAGE_TYPE_POISON, "Poison" },
    { DAMAGE_TYPE_ARCANE, "Arcane" },
    { DAMAGE_TYPE_HOLY, "Holy" },
    { 0, NULL },
};

// line 3163
extern const SymbolicEntry gHealTypes[12];
const SymbolicEntry gHealTypes[12] = {
    { HEAL_TYPE_DIRECT, "Direct" },
    { HEAL_TYPE_DIRECT_OVER_TIME, "Direct Over Time" },
    { HEAL_TYPE_LIFE_STEAL, "Life Steal" },
    { HEAL_TYPE_HEALTH_GLOBE, "Health Globe" },
    { HEAL_TYPE_POTION, "Potion" },
    { HEAL_TYPE_SHRINE, "Shrine" },
    { HEAL_TYPE_RESURRECTION, "Resurrection" },
    { HEAL_TYPE_ABSORB, "Absorb" },
    { HEAL_TYPE_REGEN, "Regen" },
    { HEAL_TYPE_ON_KILL, "On Kill" },
    { HEAL_TYPE_ON_HIT, "On Hit" },
    { 0, NULL },
};

// line 3253
extern const SymbolicEntry gImageCollections[4];
const SymbolicEntry gImageCollections[4] = {
    { IMAGE_COLLECTION_ITEM, "Items" },
    { IMAGE_COLLECTION_MAP, "Minimap" },
    { IMAGE_COLLECTION_UI, "UI" },
    { 0, NULL },
};

// line 3287
extern const SymbolicEntry gItemLabels[106];
const SymbolicEntry gItemLabels[106] = {
    { ITL_INVALID, "- None -" },
    { ITL_WEAPON, "Weapon" },
    { ITL_MELEE, "Melee" },
    { ITL_SWING, "Swing" },
    { ITL_AXE, "Axe" },
    { ITL_SWORD, "Sword" },
    { ITL_MACE, "Mace" },
    { ITL_THRUST, "Thrust" },
    { ITL_DAGGER, "Dagger" },
    { ITL_POLEARM, "Polearm" },
    { ITL_1_HAND, "1-hand" },
    { ITL_2_HAND, "2-hand" },
    { ITL_RANGED, "Ranged" },
    { ITL_BOWCLASS, "BowClass" },
    { ITL_BOW, "Bow" },
    { ITL_CROSSBOW, "Crossbow" },
    { ITL_WAND, "Wand" },
    { ITL_STAFF, "Staff" },
    { ITL_THROWN, "Thrown" },
    { ITL_THROWINGAXE, "ThrowingAxe" },
    { ITL_SPEAR, "Spear" },
    { ITL_ARMOR, "Armor" },
    { ITL_SHIELD, "Shield" },
    { ITL_HELM, "Helm" },
    { ITL_GLOVES, "Gloves" },
    { ITL_BOOTS, "Boots" },
    { ITL_CHESTARMOR, "ChestArmor" },
    { ITL_BELT, "Belt" },
    { ITL_JEWELRY, "Jewelry" },
    { ITL_RING, "Ring" },
    { ITL_AMULET, "Amulet" },
    { ITL_SOCKETABLE, "Socketable" },
    { ITL_GEM, "Gem" },
    { ITL_UTILITY, "Utility" },
    { ITL_GENERALUTILITY, "GeneralUtility" },
    { ITL_POTION, "Potion" },
    { ITL_HEALTHPOTION, "HealthPotion" },
    { ITL_QUEST, "Quest" },
    { ITL_QUESTUSABLE, "QuestUsable" },
    { ITL_GOLD, "Gold" },
    { ITL_QUIVER, "Quiver" },
    { ITL_SHOULDERS, "Shoulders" },
    { ITL_GLYPH, "Glyph" },
    { ITL_LEGS, "Legs" },
    { ITL_OFFHAND, "Offhand" },
    { ITL_ORB, "Orb" },
    { ITL_SCROLL, "Scroll" },
    { ITL_TOWNPORTALSTONE, "TownPortalStone" },
    { ITL_IDENTIFYSCROLL, "IdentifyScroll" },
    { ITL_JUNK, "Junk" },
    { ITL_BOOK, "Book" },
    { ITL_ORNAMENT, "Ornament" },
    { ITL_FIST_WEAPON, "FistWeapon" },
    { ITL_COMBAT_STAFF, "CombatStaff" },
    { ITL_BRACERS, "Bracers" },
    { ITL_CRAFTING, "CraftingReagent" },
    { ITL_DYE, "Dye" },
    { ITL_CALLDOWN, "Calldown" },
    { ITL_HEALTH_GLYPH, "HealthGlyph" },
    { ITL_SHRINE_GLYPH, "ShrineGlyph" },
    { ITL_KNOWLEDGE_UTILITY, "KnowledgeUtility" },
    { ITL_HAND_CROSSBOW, "HandXbow" },
    { ITL_TOME, "Tome" },
    { ITL_FOLLOWER_SPECIAL, "FollowerSpecial" },
    { ITL_TEMPLAR_SPECIAL, "TemplarSpecial" },
    { ITL_SCOUNDREL_SPECIAL, "ScoundrelSpecial" },
    { ITL_ENCHANTRESS_SPECIAL, "EnchantressSpecial" },
    { ITL_MOJO, "Mojo" },
    { ITL_MIGHTY_WEAPON, "MightyWeapon" },
    { ITL_CEREMONIAL_DAGGER, "CeremonialDagger" },
    { ITL_WIZARD_HAT, "WizardHat" },
    { ITL_VOODOO_MASK, "VoodooMask" },
    { ITL_CLOAK, "Cloak" },
    { ITL_BARB_BELT, "BarbBelt" },
    { ITL_SPIRIT_STONE, "SpiritStone" },
    { ITL_GENERIC_SWING_WEAPON, "GenericSwingWeapon" },
    { ITL_GENERIC_THRUST_WEAPON, "GenericThrustWeapon" },
    { ITL_GENERIC_BOW_WEAPON, "GenericBowWeapon" },
    { ITL_GENERIC_RANGED_WEAPON, "GenericRangedWeapon" },
    { ITL_GENERIC_HELM, "GenericHelm" },
    { ITL_GENERIC_CHEST_ARMOR, "GenericChestArmor" },
    { ITL_GENERIC_ARMOR, "GenericBelt" },
    { ITL_GENERIC_OFFHAND, "GenericOffHand" },
    { ITL_USE_ON_PICKUP, "UseOnPickup" },
    { ITL_CRAFTING_PLAN, "CraftingPlan" },
    { ITL_CRAFTING_PLAN_LEGENDARY, "CraftingPlanLegendary" },
    { ITL_CRAFTING_PLAN_GENERIC, "CraftingPlanGeneric" },
    { ITL_FLAIL, "Flail" },
    { ITL_SHIELD_CRUSADER, "CrusaderShield" },
    { ITL_JEWEL, "Jewel" },
    { ITL_COLLECTIBLE, "Collectible" },
    { ITL_COLLECTIBLE_DEVILS_HAND, "Collectible_DevilsHand" },
    { ITL_SHARD, "Shard" },
    { ITL_GREATERSHARD, "GreaterShard" },
    { ITL_UPGRADEABLE_JEWEL, "UpgradeableJewel" },
    { ITL_BLOODSHARD, "BloodShard" },
    { ITL_COSMETIC_WINGS, "CosmeticWings" },
    { ITL_COSMETIC_PET, "CosmeticPet" },
    { ITL_COSMETIC_PENNANT, "CosmeticPennant" },
    { ITL_COSMETIC_PORTRAIT_FRAME, "CosmeticPortraitFrame" },
    { ITL_PLATINUM, "Platinum" },
    { ITL_HORADRIC_REAGENT, "HoradricReagent" },
    { ITL_TIERED_RIFT_KEY, "TieredRiftKey" },
    { ITL_SCYTHE, "Scythe" },
    { ITL_OFFHAND_NECROMANCER, "NecromancerOffhand" },
    { 0, NULL },
};

// line 3398
extern const SymbolicEntry gPrimaryDamageAttributes[5];
const SymbolicEntry gPrimaryDamageAttributes[5] = {
    { PRIMARY_DAMAGE_ATTRIBUTE_NONE, "None" },
    { PRIMARY_DAMAGE_ATTRIBUTE_STRENGTH, "Strength" },
    { PRIMARY_DAMAGE_ATTRIBUTE_DEXTERITY, "Dexterity" },
    { PRIMARY_DAMAGE_ATTRIBUTE_INTELLIGENCE, "Intelligence" },
    { 0, NULL },
};

// line 3557
extern const SymbolicEntry gTrailType[4];
const SymbolicEntry gTrailType[4] = {
    { TRAIL_LINE, "Line" },
    { TRAIL_CROSS, "Cross" },
    { TRAIL_CIRCLE, "Circle" },
    { 0, NULL },
};

// line 3644
extern const SymbolicEntry gAIStateConditionTypes[81];
const SymbolicEntry gAIStateConditionTypes[81] = {
    { AISTATE_CONDITION_NONE, "None" },
    { AISTATE_CONDITION_TARGET_IN_RANGE, "TargetInRange" },
    { AISTATE_CONDITION_TARGET_NOT_IN_RANGE, "TargetNotInRange" },
    { AISTATE_CONDITION_TARGET_HEALTH_PERCENT_IN_RANGE, "TargetHealthPercentInRange" },
    { AISTATE_CONDITION_ON_TARGETED_HIT_CHANCE, "OnChance" },
    { AISTATE_CONDITION_HAS_TARGET, "HasTarget" },
    { AISTATE_CONDITION_HAS_SKILL, "HasSkill" },
    { AISTATE_CONDITION_NO_ENEMY_TIME_IN_RANGE, "NoEnemyTimeInRange" },
    { AISTATE_CONDITION_LAST_ALLY_DEATH_TIME, "LastAllyDeathTime" },
    { AISTATE_CONDITION_LAST_ALLY_WARN_TIME, "LastAllyWarnTime" },
    { AISTATE_CONDITION_LAST_ATTACK_RECEIVED_TIME, "LastAttackReceivedTime" },
    { AISTATE_CONDITION_LAST_ATTACK_PERFORMED_TIME, "LastAttackPerformedTime" },
    { AISTATE_CONDITION_TIME_IN_CURRENT_STATE, "TimeInCurrentState" },
    { AISTATE_CONDITION_TARGET_ON_PATH_IN_RANGE, "TargetOnPathInRange" },
    { AISTATE_CONDITION_DERAILED_FROM_PATH, "DerailedFromPath" },
    { AISTATE_CONDITION_TARGET_IS_NORMAL_MONSTER, "TargetIsNormalMonster" },
    { AISTATE_CONDITION_TARGET_IS_NOT_LARGE_MONSTER, "TargetIsNotLargeMonster" },
    { AISTATE_CONDITION_TARGET_IS_OVER_WALKABLE_AREA, "TargetIsOverWalkableArea" },
    { AISTATE_CONDITION_TARGET_IS_NOT_OVER_WALKABLE_AREA, "TargetIsNotOverWalkableArea" },
    { AISTATE_CONDITION_TARGET_IS_IN_HELPER_AREA, "TargetIsInHelperArea" },
    { AISTATE_CONDITION_TARGET_IS_ROOTED, "TargetIsRooted" },
    { AISTATE_CONDITION_TARGET_IS_NOT_ROOTED, "TargetIsNotRooted" },
    { AISTATE_CONDITION_SUMMON_COUNT_LESS_THAN, "SummonCountLessThan" },
    { AISTATE_CONDITION_TOTAL_SUMMON_COUNT_NOT_REACHED, "TotalSummonCountNotReached" },
    { AISTATE_CONDITION_CONCURRENT_SUMMON_COUNT_NOT_REACHED, "ConcurrentSummonCountNotReached" },
    { AISTATE_CONDITION_TARGET_IS_OVER_SPECIAL_0, "TargetIsOverSpecial0" },
    { AISTATE_CONDITION_TARGET_IS_NOT_OVER_SPECIAL_0, "TargetIsNotOverSpecial0" },
    { AISTATE_CONDITION_TARGET_IS_OVER_SPECIAL_1, "TargetIsOverSpecial1" },
    { AISTATE_CONDITION_TARGET_IS_NOT_OVER_SPECIAL_1, "TargetIsNotOverSpecial1" },
    { AISTATE_CONDITION_TARGET_PATH_DISTANCE_IN_RANGE, "TargetPathDistanceInRange" },
    { AISTATE_CONDITION_TARGET_PATH_DISTANCE_NOT_IN_RANGE, "TargetPathDistanceNotInRange" },
    { AISTATE_CONDITION_TARGET_IS_BURROWED, "TargetIsBurrowed" },
    { AISTATE_CONDITION_TARGET_IS_NOT_BURROWED, "TargetIsNotBurrowed" },
    { AISTATE_CONDITION_TARGET_IS_STEALTHED, "TargetIsStealthed" },
    { AISTATE_CONDITION_TARGET_IS_NOT_STEALTHED, "TargetIsNotStealthed" },
    { AISTATE_CONDITION_TARGET_IS_IN_SPECIAL_STATE, "TargetIsInSpecialState" },
    { AISTATE_CONDITION_TARGET_WITHIN_FACING, "TargetWithinFacing" },
    { AISTATE_CONDITION_TARGET_IS_NOT_IN_SPECIAL_STATE, "TargetIsNotInSpecialState" },
    { AISTATE_CONDITION_TARGET_RESOURCE_PERCENT_IN_RANGE, "TargetResourcePercentInRange" },
    { AISTATE_CONDITION_TARGET_IN_RANGE_OF_OWNER, "TargetInRangeOfOwner" },
    { AISTATE_CONDITION_TARGET_NOT_IN_RANGE_OF_OWNER, "TargetNotInRangeOfOwner" },
    { AISTATE_CONDITION_BUFF_NOT_ACTIVE, "BuffNotActive" },
    { AISTATE_CONDITION_BUFF_IS_ACTIVE, "BuffIsActive" },
    { AISTATE_CONDITION_TARGET_PATH_DISTANCE_FROM_OWNER, "TargetPathDistanceFromOwner" },
    { AISTATE_CONDITION_TARGET_IS_ENRAGED, "TargetIsEnraged" },
    { AISTATE_CONDITION_TARGET_IS_NOT_ENRAGED, "TargetIsNotEnraged" },
    { AISTATE_CONDITION_NO_PATH_TO_TARGET, "NoPathToTarget" },
    { AISTATE_CONDITION_TARGET_IS_NOT_WRECKABLE, "TargetIsNotWreckable" },
    { AISTATE_CONDITION_HAS_ATTACHED_ACTOR, "HasAttachedActor" },
    { AISTATE_CONDITION_TARGET_IN_RANGE_OF_GUARD_OBJECT, "TargetInRangeOfGuardObject" },
    { AISTATE_CONDITION_TARGET_NOT_IN_RANGE_OF_GUARD_OBJECT, "TargetNotInRangeOfGuardObject" },
    { AISTATE_CONDITION_RUN_AWAY_DISTANCE_ATLEAST, "RunAwayFromTargetAtleast" },
    { AISTATE_CONDITION_TARGET_HAS_PATH, "TargetHasPath" },
    { AISTATE_CONDITION_TARGET_PLAYED_SPAWN_ANIM, "TargetPlayedSpawnAnim" },
    { AISTATE_CONDITION_TARGET_SPAWNED_BY_INACTIVE_MARKER, "TargetSpawnedByInactiveMarker" },
    { AISTATE_CONDITION_TARGET_HAS_NO_DIRECT_PATH, "TargetHasNoDirectPath" },
    { AISTATE_CONDITION_TARGET_HAS_DIRECT_PATH, "TargetHasDirectPath" },
    { AISTATE_CONDITION_KILLED_LAST_TARGET_IN_RANGE, "KilledLastTargetInRange" },
    { AISTATE_CONDITION_DOES_NOT_HAVE_TARGET, "DoesNotHaveTarget" },
    { AISTATE_CONDITION_DIRECT_PATH_TO_WITHIN_RANGE_OF_TARGET, "DirectPathToWithinRangeOfTarget" },
    { AISTATE_CONDITION_FAILED_DYNAMIC_PATH, "FailedDynamicPath" },
    { AISTATE_CONDITION_DOES_NOT_HAVE_ATTACK_SLOT_RESERVED, "DoesNotHaveAttackSlotReservedForTarget" },
    { AISTATE_CONDITION_TARGET_IS_ACTIVE, "TargetIsActive" },
    { AISTATE_CONDITION_TARGET_IS_NOT_ACTIVE, "TargetIsNotActive" },
    { AISTATE_CONDITION_PACK_ALIVE_COUNT_GREATER_THAN, "PackAliveCountGreaterThan" },
    { AISTATE_CONDITION_PACK_ALIVE_COUNT_LESS_THAN, "PackAliveCountLessThan" },
    { AISTATE_CONDITION_TARGET_IS_AHEAD_ON_PATH, "TargetIsAheadOnPath" },
    { AISTATE_CONDITION_TARGET_CAN_BE_IMMOBILIZED, "TargetCanBeImmobilized" },
    { AISTATE_CONDITION_GAME_IS_STORY_MODE, "GameIsStoryMode" },
    { AISTATE_CONDITION_GAME_IS_ADVENTURE_MODE, "GameIsAdventureMode" },
    { AISTATE_CONDITION_TARGET_IS_UP_AGAINST_WALL, "TargetIsUpAgainstWall" },
    { AISTATE_CONDITION_MONSTER_POWER_IS_AT_LEAST, "MonsterPowerIsAtLeast" },
    { AISTATE_CONDITION_MONSTER_POWER_IS_LESS_THAN, "MonsterPowerIsLessThan" },
    { AISTATE_CONDITION_TARGET_ON_WALKABLE_NOT_BURROWED_OR_STEALTHED, "TargetOnWalkableAndNotBurrowedOrStealthed" },
    { AISTATE_CONDITION_TARGET_LEVEL_IS_AT_LEAST, "TargetLevelIsAtLeast" },
    { AISTATE_CONDITION_TARGET_LEVEL_IS_LESS_THAN, "TargetLevelIsLessThan" },
    { AISTATE_CONDITION_TIME_SINCE_ENTERED_AGGRESSIVE, "TimeSinceEnteredAggressive" },
    { AISTATE_CONDITION_IS_IN_CHINA, "IsInChina" },
    { AISTATE_CONDITION_TARGET_NOT_IN_RIFT, "TargetNotInRift" },
    { AISTATE_CONDITION_TARGET_NOT_IN_BOSS_FIGHT, "TargetNotInBossFight" },
    { 0, NULL },
};

// line 3731
extern const SymbolicEntry gAIStateTargetTypes[48];
const SymbolicEntry gAIStateTargetTypes[48] = {
    { AISTATE_TARGET_NONE, "None" },
    { AISTATE_TARGET_PRIMARY_ENEMY, "PrimaryEnemy" },
    { AISTATE_TARGET_PRIMARY_ACTIVE_ENEMY, "PrimaryActiveEnemy" },
    { AISTATE_TARGET_CLOSEST_FRIEND, "ClosestFriend" },
    { AISTATE_TARGET_SELF, "Self" },
    { AISTATE_TARGET_CLOSEST_PLAYER, "ClosestPlayer" },
    { AISTATE_TARGET_RANDOM_NEARBY_GROUND, "RandomNearbyGround" },
    { AISTATE_TARGET_PLAYER_TO_FOLLOW, "PlayerToFollow" },
    { AISTATE_TARGET_CLOSEST_CORPSE, "ClosestCorpse" },
    { AISTATE_TARGET_CLOSEST_UNDEAD, "ClosestUndead" },
    { AISTATE_TARGET_CLOSEST_POWER_TARGET, "ClosestPowerTarget" },
    { AISTATE_TARGET_GUARD_OBJECT, "GuardObject" },
    { AISTATE_TARGET_OWNER, "Owner" },
    { AISTATE_TARGET_TALK_TARGET, "TalkTarget" },
    { AISTATE_TARGET_OWNER_CLOSEST_ENEMY, "OwnerClosestEnemy" },
    { AISTATE_TARGET_BROADCASTER, "Broadcaster" },
    { AISTATE_TARGET_CLOSEST_ENEMY_IN_COMBAT, "ClosestEnemyInCombat" },
    { AISTATE_TARGET_RANDOM_FAR_GROUND, "RandomFarGround" },
    { AISTATE_TARGET_RANDOM_BEHIND_ENEMY, "RandomBehindEnemy" },
    { AISTATE_TARGET_CLOSEST_ENEMY, "ClosestEnemy" },
    { AISTATE_TARGET_CUSTOM_POWER_TARGET, "CustomPowerTarget" },
    { AISTATE_TARGET_CLOSEST_GOLD, "ClosestGold" },
    { AISTATE_TARGET_CLOSEST_PLUS_ALLY, "ClosestPlusAlly" },
    { AISTATE_TARGET_HIGHEST_THREAT, "HighestThreat" },
    { AISTATE_TARGET_SWITCH_ON_ATTACK, "SwitchOnAttack" },
    { AISTATE_TARGET_CLOSEST_SAME_TEAM, "ClosestSameTeam" },
    { AISTATE_TARGET_CLOSEST_WRECKABLE, "ClosestWreckable" },
    { AISTATE_TARGET_CLOSEST_FRIENDLY_LEADER, "ClosestFriendlyLeader" },
    { AISTATE_TARGET_WRECKABLE_BLOCKER, "WreckableBlocker" },
    { AISTATE_TARGET_TETHER_POINT, "TetherPoint" },
    { AISTATE_TARGET_TAUNT_TARGET, "TauntTarget" },
    { AISTATE_TARGET_CHARM_SOURCE, "CharmSource" },
    { AISTATE_TARGET_CLOSEST_CORPSE_OF_SAME_FAMILY, "ClosestCorpseOfSameFamily" },
    { AISTATE_TARGET_CLOSEST_CORPSE_OF_SAME_FAMILY_NON_ELITE, "ClosestCorpseSameFamilyNonElite" },
    { AISTATE_TARGET_CLOSEST_IN_PACK, "ClosestInPack" },
    { AISTATE_TARGET_CENTER_OF_PACK_GROUND, "CenterOfPackGround" },
    { AISTATE_TARGET_CLOSEST_NEW_LEGENDARY, "ClosestNewLegendary" },
    { AISTATE_TARGET_CLOSEST_RESPLENDENT_CHEST, "ClosestResplendentChest" },
    { AISTATE_TARGET_CLOSEST_ELITE, "ClosestElite" },
    { AISTATE_TARGET_CLOSEST_PLAYER_IN_COMBAT_WITH_PLAYER, "ClosestPlayerInCombatWithPlayer" },
    { AISTATE_TARGET_CLOSEST_NON_PLAYER_CONTROLLED_ENEMY, "ClosestNonPlayerControlledEnemy" },
    { AISTATE_TARGET_FRONT_OF_PRIMARY_ENEMY, "FrontOfPrimaryEnemy" },
    { AISTATE_TARGET_FRONT_OF_SELF, "FrontOfSelf" },
    { AISTATE_TARGET_CLOSEST_TRASH_ITEM, "ClosestTrashItem" },
    { AISTATE_TARGET_FRONT_OF_CUSTOM_POWER_TARGET, "FrontOfCustomPowerTarget" },
    { AISTATE_TARGET_CLOSEST_PET, "ClosestPet" },
    { AISTATE_TARGET_FARTHEST_PLAYER, "FarthestPlayer" },
    { 0, NULL },
};

// line 3785
extern const SymbolicEntry gAIStateTypes[10];
const SymbolicEntry gAIStateTypes[10] = {
    { AISTATE_TYPE_NONE, "None" },
    { AISTATE_TYPE_IDLE, "Idle" },
    { AISTATE_TYPE_SUSPICIOUS, "Suspicious" },
    { AISTATE_TYPE_AGGRESSIVE, "Aggressive" },
    { AISTATE_TYPE_FLEEING, "Fleeing" },
    { AISTATE_TYPE_CUSTOM_A, "CustomA" },
    { AISTATE_TYPE_CUSTOM_B, "CustomB" },
    { AISTATE_TYPE_CUSTOM_C, "CustomC" },
    { AISTATE_TYPE_SETUP, "Setup" },
    { 0, NULL },
};

// line 3801
extern const SymbolicEntry gAIStateCooldowns[10];
const SymbolicEntry gAIStateCooldowns[10] = {
    { AICOOLDOWN_NONE, "None" },
    { AICOOLDOWN_A, "A" },
    { AICOOLDOWN_B, "B" },
    { AICOOLDOWN_C, "C" },
    { AICOOLDOWN_D, "D" },
    { AICOOLDOWN_E, "E" },
    { AICOOLDOWN_F, "F" },
    { AICOOLDOWN_G, "G" },
    { AICOOLDOWN_H, "H" },
    { 0, NULL },
};

// line 3909
extern const SymbolicEntry gScriptEventTypes[64];
const SymbolicEntry gScriptEventTypes[64] = {
    { EVENT_INVALID, "Invalid" },
    { EVENT_ACTOR_KILLED, "Actor Killed" },
    { EVENT_ACTOR_DAMAGED, "Actor Damaged" },
    { EVENT_ACTOR_SIGHTED, "Actor Sighted" },
    { EVENT_ACTOR_SCORED_CRITICAL_HIT, "Actor Scored Critical Hit" },
    { EVENT_ACTOR_SPAWNED_BY_SPAWNER, "Actor Spawned By Spawner" },
    { EVENT_ACTOR_HEALTH_ENTERED_RANGE, "Actor Health Entered Range" },
    { EVENT_ACTOR_REACHED_PATH_NODE, "Actor Reached Path Node" },
    { EVENT_ACTOR_ACTIVATED, "Actor Activated" },
    { EVENT_ACTOR_REROLLED_INVENTORY, "Actor Rerolled Inventory" },
    { EVENT_CONVERSATION_STARTED, "Conversation Started" },
    { EVENT_CONVERSATION_FINISHED, "Conversation Finished" },
    { EVENT_GROUP_KILLED, "Group Killed" },
    { EVENT_HEART_BEAT, "Heart Beat" },
    { EVENT_GAME_TIME, "Game Time" },
    { EVENT_PLAYER_ENTERED_GAME, "Player Entered Game" },
    { EVENT_PLAYER_LEFT_GAME, "Player Left Game" },
    { EVENT_PLAYER_ENTERED_WORLD, "Player Entered World" },
    { EVENT_PLAYER_ENTERED_LEVEL_AREA, "Player Entered Level Area" },
    { EVENT_PLAYER_ENTERED_HORADRIC_QUEST_LEVEL_AREA, "Player Entered Horadric Quest Level Area" },
    { EVENT_PLAYER_PICKED_UP_ITEM, "Player Picked Up Item" },
    { EVENT_PLAYER_DROPPED_ITEM, "Player Dropped Item" },
    { EVENT_PLAYER_KILL_COUNTER, "Player Kill Counter" },
    { EVENT_PLAYER_TOWN_PORTAL, "Player Town Portal" },
    { EVENT_HIRELING_EQUIPPED_ITEM, "Hireling Equipped Item" },
    { EVENT_LOOT_DROPPED, "Loot Dropped" },
    { EVENT_GIZMO_OPERATED, "Gizmo Operated" },
    { EVENT_GIZMO_CAN_OPERATE, "Gizmo Can Operate" },
    { EVENT_GATE_GIZMO_OPENED, "Gate Gizmo Opened" },
    { EVENT_DOOR_COMPLETELY_OPENED, "Door Completely Opened" },
    { EVENT_HELPER_ATTACHED, "Helper Attached" },
    { EVENT_HELPER_DISMISSED, "Helper Dismissed" },
    { EVENT_HELPER_GUARD_OBJECT, "Helper Guard Object" },
    { EVENT_ACTOR_HIRELING_LEARNED_SKILL, "Hireling Learned Skill" },
    { EVENT_USER_EVENT, "User Event" },
    { EVENT_QUEST_STEP_ASSIGNED, "Quest Step Assigned" },
    { EVENT_QUEST_STEP_COMPLETED, "Quest Step Completed" },
    { EVENT_QUEST_OBJECTIVE_PROGRESSED, "Quest Objective Progressed" },
    { EVENT_QUEST_OBJECTIVE_COMPLETED, "Quest Objective Completed" },
    { EVENT_QUESTRANGE_CHANGED, "Quest Range Changed" },
    { EVENT_QUEST_TIMER_RUNNING_OUT, "Quest Timer Running Out" },
    { EVENT_QUEST_COMPLETED, "Quest Completed" },
    { EVENT_AIQUEUE_EMPTY, "AIQueue Empty" },
    { EVENT_AIQUEUE_ACTION_COMPLETE, "AIQueue Action Complete" },
    { EVENT_PLAYER_IN_RANGE_OF_ACTOR, "Player In Range of Actor" },
    { EVENT_TRIGGER_ON_ACTOR_ENTERED, "Trigger On Actor Entered" },
    { EVENT_TRIGGER_ON_ACTOR_EXITED, "Trigger On Actor Exited" },
    { EVENT_TRIGGER_PULSE_ACTORS_IN_BOUNDS, "Trigger Pulse Actors In Bounds" },
    { EVENT_TIMED_EVENT_TIME_ELAPSED, "Timed Event Time Elapsed" },
    { EVENT_TIMED_EVENT_TIME_REMAINING, "Timed Event Time Remaining" },
    { EVENT_WORLD_DELETED, "On World Deleted" },
    { EVENT_ACTOR_HEALED, "Actor Healed" },
    { EVENT_ACTOR_RESURRECTED, "Actor Resurrected" },
    { EVENT_ACTOR_GAINED_LEVEL, "Actor Leveled" },
    { EVENT_ACTOR_USED_POWER, "Actor Used Power" },
    { EVENT_BOSS_ENCOUNTER_RESET, "Boss Encounter Reset" },
    { EVENT_SCRIPTED_SEQUENCE_SKIPPED, "Player Skipped Sequence" },
    { EVENT_CHANGED_ACT, "Changed Act" },
    { EVENT_HIRELING_HIRED, "Hireling Hired" },
    { EVENT_HIRELING_DISMISSED, "Hireling Dismissed" },
    { EVENT_NPC_VENDOR_OPERATED, "NPC Vendor Operated" },
    { EVENT_DEVILS_HAND_TRACKING, "Devil's Hand Tracking" },
    { EVENT_GIZMO_OPERATED, "Gizmo Operate" },
    { 0, NULL },
};

// line 4005
extern const SymbolicEntry gScriptEventVariantParamTypes[32];
const SymbolicEntry gScriptEventVariantParamTypes[32] = {
    { SCRIPT_EVENT_VARIANT_PARAM_NONE, "Allow Any" },
    { SCRIPT_EVENT_VARIANT_PARAM_DWORD, "Integer" },
    { SCRIPT_EVENT_VARIANT_PARAM_FLOAT, "Float" },
    { SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_ENTERED, "Float Range" },
    { SCRIPT_EVENT_VARIANT_PARAM_ACTOR_SNO, "Actor SNO" },
    { SCRIPT_EVENT_VARIANT_PARAM_MONSTER_SNO, "Monster SNO" },
    { SCRIPT_EVENT_VARIANT_PARAM_LEVEL_AREA_SNO, "Level Area SNO" },
    { SCRIPT_EVENT_VARIANT_PARAM_STRING_HASH, "String/Name" },
    { SCRIPT_EVENT_VARIANT_PARAM_PLAYER, "Player" },
    { SCRIPT_EVENT_VARIANT_PARAM_PLAYER_ALLY, "Player Ally" },
    { SCRIPT_EVENT_VARIANT_PARAM_SCENE_SNO, "Scene SNO" },
    { SCRIPT_EVENT_VARIANT_PARAM_CONVERSATION, "Conversation" },
    { SCRIPT_EVENT_VARIANT_PARAM_SELF, "Self" },
    { SCRIPT_EVENT_VARIANT_PARAM_GAME_TIME, "Game Time" },
    { SCRIPT_EVENT_VARIANT_PARAM_TIME_INTERVAL, "Time Interval" },
    { SCRIPT_EVENT_VARIANT_PARAM_ANIM_TAG, "Anim Tag" },
    { SCRIPT_EVENT_VARIANT_PARAM_QUEST_NAME, "Quest Name" },
    { SCRIPT_EVENT_VARIANT_PARAM_PRIMARY_GROUP, "Primary Group" },
    { SCRIPT_EVENT_VARIANT_PARAM_SECONDARY_GROUP, "Secondary Group" },
    { SCRIPT_EVENT_VARIANT_PARAM_ITEM_GBID, "Item GBID" },
    { SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_ANY, "Linked Marker (Any)" },
    { SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_IN_GROUP, "Linked Marker In Group" },
    { SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_INSIDE, "Float Range Inside" },
    { SCRIPT_EVENT_VARIANT_PARAM_TIMED_EVENT_SNO, "Timed Event SNO" },
    { SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_NAME, "Spawned By String/Name" },
    { SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_SELF, "Spawned By Self" },
    { SCRIPT_EVENT_VARIANT_PARAM_WORLD_SNO, "World SNO" },
    { SCRIPT_EVENT_VARIANT_PARAM_POWER_SNO, "Power SNO" },
    { SCRIPT_EVENT_VARIANT_PARAM_QUESTRANGE, "Quest Range" },
    { SCRIPT_EVENT_VARIANT_PARAM_GIZMO_TYPE, "Gizmo Type" },
    { SCRIPT_EVENT_VARIANT_PARAM_BOSS_ENCOUNTER_SNO, "Boss Encounter SNO" },
    { 0, NULL },
};

// line 4043
extern const SymbolicEntry gResourceTypes[10];
const SymbolicEntry gResourceTypes[10] = {
    { RESOURCE_TYPE_MANA, "Mana" },
    { RESOURCE_TYPE_ARCANUM, "Arcanum" },
    { RESOURCE_TYPE_FURY, "Fury" },
    { RESOURCE_TYPE_SPIRIT, "Spirit" },
    { RESOURCE_TYPE_POWER, "Power" },
    { RESOURCE_TYPE_HATRED, "Hatred" },
    { RESOURCE_TYPE_DISCIPLINE, "Discipline" },
    { RESOURCE_TYPE_FAITH, "Faith" },
    { RESOURCE_TYPE_ESSENCE, "Essence" },
    { 0, NULL },
};

// line 4059
extern const SymbolicEntry gSpawnDistributions[4];
const SymbolicEntry gSpawnDistributions[4] = {
    { DISTRIBUTION_DENSITY, "Count per 100 sq ft" },
    { DISTRIBUTION_EXACT, "Exactly" },
    { DISTRIBUTION_XP_DENSITY, "XP Density Weight" },
    { 0, NULL },
};

// line 4069
extern const SymbolicEntry gDspTypes[11];
const SymbolicEntry gDspTypes[11] = {
    { DSP_TYPE_CHORUS, "Chorus" },
    { DSP_TYPE_COMPRESSOR, "Compressor" },
    { DSP_TYPE_DISTORTION, "Distortion" },
    { DSP_TYPE_ECHO, "Echo" },
    { DSP_TYPE_FLANGE, "Flange" },
    { DSP_TYPE_HIGHPASS, "High Pass Filter" },
    { DSP_TYPE_LOWPASS, "Low Pass Filter" },
    { DSP_TYPE_LOWPASS_SIMPLE, "Low Pass Filter (Simple)" },
    { DSP_TYPE_PARAMEQ, "Parametric EQ" },
    { DSP_TYPE_SFXREVERB, "SFX Reverb" },
    { 0, NULL },
};

// line 4115
extern const SymbolicEntry gPathClasses[16];
const SymbolicEntry gPathClasses[16] = {
    { PATH_A, "Path A" },
    { PATH_B, "Path B" },
    { PATH_C, "Path C" },
    { PATH_D, "Path D" },
    { PATH_E, "Path E" },
    { PATH_F, "Path F" },
    { PATH_G, "Path G" },
    { PATH_H, "Path H" },
    { PATH_I, "Path I" },
    { PATH_J, "Path J" },
    { PATH_K, "Path K" },
    { PATH_L, "Path L" },
    { PATH_M, "Path M" },
    { PATH_N, "Path N" },
    { PATH_O, "Path O" },
    { 0, NULL },
};

// line 4149
extern const SymbolicEntry gClothPlanes[10];
const SymbolicEntry gClothPlanes[10] = {
    { CLOTH_PLANE_NONE, "None" },
    { CLOTH_PLANE_1, "HP_cloth_plane_1" },
    { CLOTH_PLANE_2, "HP_cloth_plane_2" },
    { CLOTH_PLANE_3, "HP_cloth_plane_3" },
    { CLOTH_PLANE_4, "HP_cloth_plane_4" },
    { CLOTH_PLANE_5, "HP_cloth_plane_5" },
    { CLOTH_PLANE_6, "HP_cloth_plane_6" },
    { CLOTH_PLANE_7, "HP_cloth_plane_7" },
    { CLOTH_PLANE_8, "HP_cloth_plane_8" },
    { 0, NULL },
};

// line 4278
extern const SymbolicEntry gAnimTreeNodeTypes[9];
const SymbolicEntry gAnimTreeNodeTypes[9] = {
    { ANT_ANIMATION, "Animation" },
    { ANT_PIECEWISE_LINEAR_BLEND, "PiecewiseLinearBlend" },
    { ANT_BONE_WEIGHTED_BLEND, "BoneWeightedBlend" },
    { ANT_SWITCH_BLEND, "SwitchBlend" },
    { ANT_POSE, "Pose" },
    { ANT_ADDITIVE_BLEND, "AdditiveBlend" },
    { ANT_BONE_WEIGHTS_LINEAR_BLEND, "BoneWeightsLinearBlend" },
    { ANT_BONE_WEIGHTS_MULTIPLY_BLEND, "BoneWeightsMultiplyBlend" },
    { 0, NULL },
};

// line 4293
extern const SymbolicEntry gAnimTreeControlParam[18];
const SymbolicEntry gAnimTreeControlParam[18] = {
    { ACP_FORWARD_SPEED, "ForwardSpeed" },
    { ACP_TURN_SPEED, "TurnSpeed" },
    { ACP_IS_IDLE, "IsIdle" },
    { ACP_WEAPON_CLASS, "WeaponClass" },
    { ACP_WALK_SLOW_SPEED, "WalkSlowSpeed" },
    { ACP_WALK_SPEED, "WalkSpeed" },
    { ACP_RUN_SPEED, "RunSpeed" },
    { ACP_SPRINT_SPEED, "SprintSpeed" },
    { ACP_IN_TOWN, "InTown" },
    { ACP_IS_SPECIAL_MOVE, "IsSpecialMove" },
    { ACP_ABS_TURN_SPEED, "AbsTurnSpeed" },
    { ACP_AIM_YAW, "AimYaw" },
    { ACP_AIM_BLEND, "AimBlend" },
    { ACP_IS_ALIVE, "IsAlive" },
    { ACP_IS_TURNING, "IsTurning" },
    { ACP_FORCE_WALK, "ForceWalk" },
    { ACP_NONE, "None" },
    { 0, NULL },
};

// line 4317
extern const SymbolicEntry gAnimTreeUpdateModes[3];
const SymbolicEntry gAnimTreeUpdateModes[3] = {
    { ATU_CONTINUOUS, "Continuous" },
    { ATU_INACTIVE_RESET, "InactiveReset" },
    { 0, NULL },
};

// line 4326
extern const SymbolicEntry gAnimBoneWeightSets[4];
const SymbolicEntry gAnimBoneWeightSets[4] = {
    { ABW_ACTION, "Action" },
    { ABW_GETHIT, "GetHit" },
    { ABW_NONE, "None" },
    { 0, NULL },
};

// line 4390
extern const SymbolicEntry gGameErrors[185];
const SymbolicEntry gGameErrors[185] = {
    { GAMEERROR_POWER_UNUSABLE_IN_AREA, "PowerUnusableInArea" },
    { GAMEERROR_ITEM_NOT_READY, "ItemNotReady" },
    { GAMEERROR_POWER_INVALID_TARGET, "PowerInvalidTarget" },
    { GAMEERROR_POWER_NOT_ENOUGH_HEALTH, "PowerNotEnoughHealth" },
    { GAMEERROR_POWER_NOT_ENOUGH_CHARGES, "PowerNotEnoughCharges" },
    { GAMEERROR_POWER_NOT_ENOUGH_MANA, "PowerNotEnoughMana" },
    { GAMEERROR_POWER_NOT_ENOUGH_ARCANUM, "PowerNotEnoughArcanum" },
    { GAMEERROR_POWER_NOT_ENOUGH_FURY, "PowerNotEnoughFury" },
    { GAMEERROR_POWER_NOT_ENOUGH_SPIRIT, "PowerNotEnoughSpirit" },
    { GAMEERROR_POWER_NOT_ENOUGH_HATRED, "PowerNotEnoughHatred" },
    { GAMEERROR_POWER_NOT_ENOUGH_DISCIPLINE, "PowerNotEnoughDiscipline" },
    { GAMEERROR_POWER_NOT_ENOUGH_FAITH, "PowerNotEnoughFaith" },
    { GAMEERROR_POWER_NOT_ENOUGH_ESSENCE, "PowerNotEnoughEssence" },
    { GAMEERROR_POWER_WRONG_WEAPON, "PowerWrongWeapon" },
    { GAMEERROR_POWER_IN_TOWN, "PowerInTown" },
    { GAMEERROR_POWER_TARGET_TOO_FAR, "PowerTargetTooFar" },
    { GAMEERROR_POWER_IN_COOLDOWN, "PowerInCooldown" },
    { GAMEERROR_POWER_NEEDS_LINE_OF_SIGHT, "PowerNeedsLineOfSight" },
    { GAMEERROR_POWER_UNUSABLE_IN_COMBAT, "PowerUnusableInCombat" },
    { GAMEERROR_POWER_UNUSABLE_GENERIC, "PowerUnusableGeneric" },
    { GAMEERROR_POWER_UNUSABLE_WHILE_MOVING, "PowerUnusableWhileMoving" },
    { GAMEERROR_POWER_MISSING_ITEM, "PowerMissingItem" },
    { GAMEERROR_POWER_UNUSABLE_DURING_BOSS_ENCOUNTER, "PowerUnusableDuringBossEncounter" },
    { GAMEERROR_POWER_NEED_CORPSES, "PowerNeedCorpses" },
    { GAMEERROR_POWER_NEED_TARGET, "PowerNeedTarget" },
    { GAMEERROR_IARESULT_UNKNOWN, "IAR_NotAllowedHere" },
    { GAMEERROR_IARESULT_CANT_TRADE_QUEST_ITEMS, "IAR_CantTradeQuestItems" },
    { GAMEERROR_IARESULT_CANT_EQUIP_UNIDENTIFIED_ITEM, "IAR_CantEquipUnidentifiedItem" },
    { GAMEERROR_IARESULT_PLAYER_DOESNT_MEET_ITEM_REQUIREMENTS, "IAR_PlayerDoesntMeetItemRequirements" },
    { GAMEERROR_IARESULT_PLAYER_DOESNT_MEET_ITEM_LEVEL, "IAR_PlayerDoesntMeetItemLevel" },
    { GAMEERROR_IARESULT_PLAYER_IS_WRONG_CLASS_FOR_ITEM, "IAR_PlayerIsWrongClassForItem" },
    { GAMEERROR_IARESULT_TEMPLAR_CANT_EQUIP_ITEM, "IAR_TemplarCannotUseWeapon" },
    { GAMEERROR_IARESULT_SCOUNDREL_CANT_EQUIP_ITEM, "IAR_ScoundrelCannotUseWeapon" },
    { GAMEERROR_IARESULT_ENCHANTRESS_CANT_EQUIP_ITEM, "IAR_EnchantressCannotUseWeapon" },
    { GAMEERROR_IARESULT_TEMPLAR_CANT_EQUIP_SPECIAL_ITEM, "IAR_TemplarCannotEquipSpecialItem" },
    { GAMEERROR_IARESULT_SCOUNDREL_CANT_EQUIP_SPECIAL_ITEM, "IAR_ScoundrelCannotEquipSpecialItem" },
    { GAMEERROR_IARESULT_ENCHANTRESS_CANT_EQUIP_SPECIAL_ITEM, "IAR_EnchantressCannotEquipSpecialItem" },
    { GAMEERROR_IARESULT_HIRELING_CANT_USE_ITEM, "IAR_HirelingCantUseItem" },
    { GAMEERROR_IARESULT_HIRELING_DOESNT_MEET_ITEM_LEVEL, "IAR_HirelingDoesntMeetItemLevel" },
    { GAMEERROR_IARESULT_ITEM_TYPE_DOESNT_MATCH_SLOT, "IAR_ItemTypeDoesntMatchSlot" },
    { GAMEERROR_IARESULT_NOT_ENOUGH_ROOM, "IAR_NotEnoughRoom" },
    { GAMEERROR_IARESULT_REMOVE_ONLY, "IAR_ContainerRemoveOnly" },
    { GAMEERROR_IARESULT_NO_FREE_SOCKETS, "IAR_NoFreeSockets" },
    { GAMEERROR_IARESULT_ITEM_IS_SEASONAL, "IAR_ItemIsSeasonal" },
    { GAMEERROR_IARESULT_ITEM_IS_NON_SEASONAL, "IAR_ItemIsNonSeasonal" },
    { GAMEERROR_IARESULT_ITEM_REQUIRES_SKILL, "IAR_ItemRequiresSkill" },
    { GAMEERROR_IARESULT_ITEM_REQUIRES_XBOW, "IAR_ItemRequiresXBow" },
    { GAMEERROR_ITEMCRAFTINGRESULT_NO_COMBINE_DESIGN, "ItemCrafting_NoCombineDesign" },
    { GAMEERROR_ITEMCRAFTINGRESULT_MISSINGREAGENTS, "ItemCrafting_MissingReagents" },
    { GAMEERROR_IARESULT_CANNOT_SALVAGE, "IAR_CannotSalvage" },
    { GAMEERROR_IARESULT_CANNOT_SALVAGE_GEMMED_ITEMS, "IAR_CannotSalvageGemmedItems" },
    { GAMEERROR_IARESULT_CANNOT_SALVAGE_VENDOR_ITEMS, "IAR_CannotSalvageVendorItems" },
    { GAMEERROR_SKILLS_SKILLBAR_GENERIC, "Skills_Skillbar_Generic" },
    { GAMEERROR_SKILLS_NOT_ASSIGNABLE_TO_THAT_SLOT, "Skills_NotAssignableToThatSlot" },
    { GAMEERROR_SKILLS_CANT_CHANGE_WHILE_IN_COOLDOWN, "Skills_CantChangeWhileInCooldown" },
    { GAMEERROR_SKILLS_CANT_CHANGE_WHILE_IN_COMBAT, "Skills_CantChangeWhileInCombat" },
    { GAMEERROR_SKILLS_CANT_CHANGE_WHILE_IN_BOSS_ENCOUNTER, "Skills_CantChangeWhileInBossEncounter" },
    { GAMEERROR_SKILLS_CANT_CHANGE_AT_THIS_TIME, "Skills_CantChangeAtThisTime" },
    { GAMEERROR_BUFF_LOWER_THEN_CURRENT, "BuffHigherLevelActive" },
    { GAMEERROR_IARESULT_ALREADY_IDENTIFIED, "AlreadyIdentified" },
    { GAMEERROR_IARESULT_CANNOT_SOCKET, "CannotSocket" },
    { GAMEERROR_IARESULT_ALREADY_SOCKETED, "AlreadySocketed" },
    { GAMEERROR_IARESULT_CANNOT_UNSOCKET, "CannotUnSocket" },
    { GAMEERROR_IARESULT_CANNOT_ADD_SOCKETS_WITH_THIS_ITEM, "CannotSocketWithThisItem" },
    { GAMEERROR_IARESULT_CANNOT_ACCEPT_TRADE, "CannotAcceptTrade" },
    { GAMEERROR_IARESULT_CANNOT_TRADE, "CannotTrade" },
    { GAMEERROR_IARESULT_CANNOT_TRADE_BECAUSE_OF_GEMS, "CannotTradeBecauseOfGems" },
    { GAMEERROR_ITEMUSE_UNKNOWN, "ItemUse_Unknown" },
    { GAMEERROR_ITEMUSE_NOT_ON_UNIDENTIFIED_ITEMS, "ItemUse_NotOnUnidentifiedItems" },
    { GAMEERROR_ITEMUSE_ALREADY_AT_MAX_HEALTH, "ItemUse_AlreadyAtMaxHealth" },
    { GAMEERROR_ITEMUSE_RECIPE_ALREADY_KNOWN, "ItemUse_RecipeAlreadyKnown" },
    { GAMEERROR_ITEMUSE_ONLY_AT_BLACKSMITH, "ItemUse_OnlyAtBlacksmith" },
    { GAMEERROR_ITEMUSE_ONLY_AT_JEWELER, "ItemUse_OnlyAtJeweler" },
    { GAMEERROR_ITEMUSE_ONLY_AT_MYSTIC, "ItemUse_OnlyAtMystic" },
    { GAMEERROR_ITEMUSE_DOESNT_MEET_ARTISAN_LEVEL, "ItemUse_DoesntMeetArtisanLevel" },
    { GAMEERROR_ITEMUSE_NOT_ENOUGH_PAGES_TO_CONVERT, "ItemUse_NotEnoughPagesToConvert" },
    { GAMEERROR_CANNOT_DO_THIS_WHILE_DEAD, "CannotDoWhileDead" },
    { GAMEERROR_BOSS_ENCOUNTER_SOMEONE_DECLINED, "SomeoneDeclined" },
    { GAMEERROR_PICKUP_FAILED, "Pickup_Failed" },
    { GAMEERROR_PICKUP_NO_SUITABLE_SLOT, "Pickup_NoSuitableSlot" },
    { GAMEERROR_PICKUP_ONLY_ONE_ALLOWED, "Pickup_OnlyOneAllowed" },
    { GAMEERROR_PICKUP_BELONGS_TO_OTHER, "Pickup_BelongsToOther" },
    { GAMEERROR_INSUFFICIENT_FUNDS, "InsufficientFunds" },
    { GAMEERROR_INSUFFICIENT_FUNDS_BLOODSHARDS, "InsufficientFundsSpecialCurrency" },
    { GAMEERROR_INSUFFICIENT_FUNDS_PLATINUM, "InsufficientFundsPlatinum" },
    { GAMEERROR_INSUFFICIENT_PARAGON_POINTS, "InsufficientParagonPoints" },
    { GAMERROR_PARAGON_CATEGORY_AT_CAP, "ParagonCategoryMaxed" },
    { GAMEERROR_ITEM_CANNOT_BE_REPAIRED, "ItemCannotBeRepaired" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED, "ItemCannotBeEnchanted" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_BY_TYPE, "ItemCannotBeEnchantedByType" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_BELOW_ILVL, "ItemCannotBeEnchantedBelowItemLevel" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_ABOVE_ILVL, "ItemCannotBeEnchantedAboveItemLevel" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_UNIDENTIFIED, "ItemCannotBeEnchantedUnidentified" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_SOCKETABLE, "ItemCannotBeEnchantedSocketable" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_INVALID_QUALITY, "ItemCannotBeEnchantedInvalidQuality" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_FILLED_SOCKETS, "ItemCannotBeEnchantedFilledSockets" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_CRAFTED, "ItemCannotBeEnchantedCrafted" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_NO_RANDOM_AFFIXES, "ItemCannotBeEnchantedNoRandomAffixes" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_LEGACY_ITEM, "ItemCannotBeEnchantedLegacy" },
    { GAMEERROR_ITEM_CANNOT_BE_ENCHANTED_ITEM_TYPE_ABOVE_CRAFTER_LEVEL, "ItemCannotBeEnchantedMysticLevelRequired" },
    { GAMEERROR_ITEM_CANNOT_BE_IMPROVED, "ItemCannotBeImproved" },
    { GAMEERROR_ITEM_CANNOT_BE_DYED, "ItemCannotBeDyed" },
    { GAMEERROR_ITEM_CANNOT_BE_REFORGED, "ItemCannotBeReforged" },
    { GAMEERROR_ITEM_CANNOT_BE_REFORGED_UNIDENTIFIED, "ItemCannotBeReforgedUnidentified" },
    { GAMEERROR_ITEM_CANNOT_BE_REFORGED_NOT_LEGENDARY, "ItemCannotBeReforgedNotLegendary" },
    { GAMEERROR_ITEM_CANNOT_BE_REFORGED_CANNOT_EQUIP, "ItemCannotBeReforgedCannotEquip" },
    { GAMEERROR_ITEM_CANNOT_BE_REFORGED_HAS_GEMS, "ItemCannotBeReforgedHasGems" },
    { GAMEERROR_ITEM_CANNOT_BE_USED_FOR_TRANSMOG, "ItemCannotBeUsedForTransmog" },
    { GAMEERROR_ITEM_CANNOT_BE_USED_FOR_TRANSMOG_UNIDENTIFIED, "ItemCannotBeUsedForTransmogUnidentified" },
    { GAMEERROR_ITEM_TRANSMOG_ALREADY_UNLOCKED, "ItemTransmogAlreadyUnlocked" },
    { GAMEERROR_TRADE_OTHER_BUSY, "TradeOtherBusy" },
    { GAMEERROR_TRADE_SPLIT_STACKS, "TradeSplitStacks" },
    { GAMEERROR_TRADE_DISABLED, "TradeDisabled" },
    { GAMEERROR_TRADE_OF_GOLD_DISABLED, "TradeOfGoldDisabled" },
    { GAMEERROR_NO_OTHER_WAYPOINTS, "NoOtherWaypoints" },
    { GAMEERROR_BNET_PARTY_LEADER_CANCEL_LEAVE_GAME, "Bnet_Party_LeaderCancelLeaveGame" },
    { GAMEERROR_BNET_PARTY_INVITE_REMOVED_LEAVE_GAME_CANCELED, "Bnet_Party_InviteRemoved_LeaveGameCanceled" },
    { GAMEERROR_BNET_MULTIPLAYER_GAMENOTPAUSED, "Bnet_Multiplayer_GameNotPaused" },
    { GAMEERROR_BNET_WEEKLY_CHALLENGE_GAMENOTPAUSED, "Bnet_WeeklyChallenge_GameNotPaused" },
    { GAMEERROR_BNET_NO_SEASON_JOURNEY_AVAILABLE_IN_NO_SEASONAL_GAME, "Bnet_No_Season_Journey_Available_In_No_Seasonal_Game" },
    { GAMEERROR_CANNOT_ENTER_ACTIVE_BOSS_ENCOUNTER, "CannotDoDuringBossEncounter" },
    { GAMEERROR_CANNOT_ENTER_QUEUED_BOSS_ENCOUNTER, "CannotDoDuringQueuedBossEncounter" },
    { GAMEERROR_BANNER_PLAYER_TOO_CLOSE, "BannerPlayerTooClose" },
    { GAMEERROR_BANNER_PLAYER_IS_IN_BOSS_ENCOUNTER, "BannerPlayerIsInBossEncounter" },
    { GAMEERROR_BANNER_PLAYER_IS_DEAD, "BannerPlayerIsDead" },
    { GAMEERROR_BANNER_PLAYER_IS_DUELING, "PlayerIsDueling" },
    { GAMEERROR_CANNOT_KICK_NOW, "CannotKickNow" },
    { GAMEERROR_ITEM_SOLD_NONERROR, "ItemSoldNonError" },
    { GAMEERROR_ITEM_CANNOT_BE_SOLD, "ItemCannotBeSold" },
    { GAMEERROR_ITEM_UNIQUE_EQUIPPED, "ItemIsUniqueEquipped" },
    { GAMEERROR_CANNOT_DO_THAT_RIGHT_NOW, "CannotDoThatRightNow" },
    { GAMEERROR_ITEM_SOCKET_TYPE_MISMATCH, "ItemSocketTypeMismatch" },
    { GAMEERROR_ITEM_SOCKET_UNIQUE_EQUIPPED, "ItemSocketUniqueEquipped" },
    { GAMEERROR_EQUIP_ITEM_SOCKET_UNIQUE_EQUIPPED, "EquipItemSocketUniqueEquipped" },
    { GAMEERROR_IDENTIFY_ALL_NO_ITEMS, "IdentifyAllNoItems" },
    { GAMEERROR_ITEM_CANNOT_BE_DROPPED, "ItemCannotBeDropped" },
    { GAMEERROR_WAYPOINT_TOO_CLOSE, "WaypointTooClose" },
    { GAMEERROR_LOOTRUN_PARTY_INSIDE, "NephalemRiftWarning_PartyInside" },
    { GAMEERROR_BANNER_PLAYER_IS_IN_TIERED_LOOT_RUN, "BannerPlayerIsInTieredLootRun" },
    { GAMEERROR_LOOTRUN_NEED_KEY, "NephalemRiftWarning_PlayerNeedsKey" },
    { GAMEERROR_LOOTRUN_TIERED_NEED_KEY, "NephalemRiftWarning_PlayerNeedsTieredKey" },
    { GAMEERROR_LOOTRUN_SOMEONE_DECLINED, "NephalemRiftWarning_PlayerDeclined" },
    { GAMEERROR_LOOTRUN_TIERED_ALREADY_STARTED, "NephalemRiftWarning_TieredRiftAlreadyStarted" },
    { GAMEERROR_LOOTRUN_TIERED_NOT_MAX_LEVEL, "NephalemRiftWarning_TieredRiftNotMaxLevel" },
    { GAMEERROR_LOOTRUN_TIERED_GAME_NOT_MAX_LEVEL, "NephalemRiftWarning_TieredRiftGameNotMaxLevel" },
    { GAMEERROR_LOOTRUN_TIERED_CANNOT_DO_THAT, "NephalemRiftWarning_CantDoThatDuringTieredRift" },
    { GAMEERROR_LOOTRUN_TIERED_CANNOT_CHANGE_SKILLS_OR_EQUIPMENT_OUTSIDE_GAME, "NephalemRiftWarning_CantChangeSkillsOrEquipmentOutsideGame" },
    { GAMEERROR_LOOTRUN_TIERED_CANNOT_BE_IN_SET_DUNGEON, "NephalemRiftWarning_TieredRiftCantBeInSetDungeon" },
    { GAMEERROR_DIFFICULTY_TOO_LOW, "DifficultyTooLow" },
    { GAMEERROR_ITEMS_CANNOT_BE_MOVED_AT_THIS_TIME, "ItemsCannotBeMovedAtThisTime" },
    { GAMEERROR_PLAYER_NOT_ALLOWED_IN_WORLD, "PlayerNotAllowedInWorld" },
    { GAMEERROR_TRANSMUTE_CANNOT_USE_ITEM_WITH_FILLED_SOCKETS, "TransmuteCannotUseItemWithFilledSockets" },
    { GAMEERROR_TRANSMUTE_CANNOT_USE_UNIDENTIFIED_ITEM, "TransmuteCannotUseUnidentifiedItem" },
    { GAMEERROR_TRANSMUTE_MISSING_CORE_INGREDIENTS, "TransmuteMissingCoreIngredients" },
    { GAMEERROR_TRANSMUTE_CANNOT_USE_ITEM_IN_CUBE, "TransmuteCannotUseThisItem" },
    { GAMEERROR_TRANSMUTE_FAILED, "TransmuteFailed" },
    { GAMEERROR_TRANSMUTE_FAILED_LEGENDARY_POWER_ALREADY_EXTRACTED, "TransmuteFailedLegendaryPowerAlreadyExtracted" },
    { GAMEERROR_TRANSMUTE_FAILED_LEGENDARY_POWER_BLACKLISTED, "TransmuteFailedLegendaryPowerUnavailable" },
    { GAMEERROR_TRANSMUTE_FAILED_CANNOT_EXTRACT_FROM_THIS_ITEM, "TransmuteFailedCannotExtractFromThisItem" },
    { GAMEERROR_TRANSMUTE_FAILED_CANNOT_USE_CRAFTED_ITEM, "TransmuteFailedCannotUseCraftedItem" },
    { GAMEERROR_TRANSMUTE_FAILED_CANNOT_EQUIP_ITEM, "TransmuteFailedCannotEquipItem" },
    { GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE, "TransmuteFailedCannotFindItem" },
    { GAMEERROR_TRANSMUTE_FAILED_ITEM_LEVEL_TOO_LOW, "TransmuteFailedItemLevelTooLow" },
    { GAMEERROR_TRANSMUTE_FAILED_ALREADY_REMOVED_LEVEL_REQ, "TransmuteFailedAlreadyRemovedLevelReq" },
    { GAMEERROR_TRANSMUTE_FAILED_NOT_ENOUGH_ITEMS_IN_SET, "TransmuteFailedNotEnoughItemsInSet" },
    { GAMEERROR_EMPOWERED_RIFT_CANNOT_AFFORD, "EmpoweredRiftCannotAfford" },
    { GAMEERROR_BANNER_PLAYER_IS_IN_SET_DUNGEON, "BannerPlayerIsInSetDungeon" },
    { GAMEERROR_TRANSMUTE_FAILED_LOW_JEWEL_RANK, "TransmuteFailedLowJewelRank" },
    { GAMEERROR_SET_DUNGEON_SOMEONE_DECLINED, "SetDungeonCancelled" },
    { GAMEERROR_SET_DUNGEON_CANNOT_ENTER_NOT_WORTHY, "SetDungeonCannotEnterNotWorthy" },
    { GAMEERROR_SET_DUNGEON_CANNOT_ENTER_CLOSING, "SetDungeonCannotEnterClosing" },
    { GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT, "EquipmentManagerCannotChangeLoadout" },
    { GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT_IS_IN_GRIFT, "EquipmentManagerCannotChangeLoadoutIsInGRift" },
    { GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT_SKILL_ON_COOLDOWN, "EquipmentManagerCannotChangeLoadoutSkillOnCooldown" },
    { GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT_CURRENTLY_EQUIPPED, "EquipmentManagerCannotChangeLoadoutCurrentlyEquipped" },
    { GAMEERROR_EQUIPMENT_MANAGER_CANNOT_EQUIP_POTION, "EquipmentManagerCannotEquipPotion" },
    { GAMEERROR_CHALLENGE_RIFT_NUMBER_MISMATCH, "ChallengeRift_Number_Mismatch" },
    { GAMEERROR_CHALLENGE_RIFT_ALREADY_STARTED, "NephalemRiftWarning_TieredRiftAlreadyStarted_CR" },
    { GAMEERROR_AMIIBO_POWER_ALREADY_ACTIVE, "AmiiboPowerAlreadyActive" },
    { GAMEERROR_AMIIBO_POWER_IN_COOLDOWN, "AmiiboPowerInCooldown" },
    { GAMEERROR_AMIIBO_INVALID, "AmiiboInvalid" },
    { GAMEERROR_AMIIBO_SCAN_FAILED, "AmiiboScanFailed" },
    { GAMEERROR_AMIIBO_NO_DEVICE, "AmiiboNoDevice" },
    { GAMEERROR_AMIIBO_ERROR, "AmiiboScanError" },
    { 0, NULL },
};

// line 4621
extern const SymbolicEntry gConvSpeaker[11];
const SymbolicEntry gConvSpeaker[11] = {
    { CONV_SPEAKER_INVALID, "- None -" },
    { CONV_SPEAKER_PLAYER, "Player" },
    { CONV_SPEAKER_PRIMARY_NPC, "Primary NPC" },
    { CONV_SPEAKER_ALT_NPC1, "Alt. NPC 1" },
    { CONV_SPEAKER_ALT_NPC2, "Alt. NPC 2" },
    { CONV_SPEAKER_ALT_NPC3, "Alt. NPC 3" },
    { CONV_SPEAKER_ALT_NPC4, "Alt. NPC 4" },
    { CONV_SPEAKER_FOLLOWER_TEMPLAR, "Templar Follower" },
    { CONV_SPEAKER_FOLLOWER_SCOUNDREL, "Scoundrel Follower" },
    { CONV_SPEAKER_FOLLOWER_ENCHANTRESS, "Enchantress Follower" },
    { 0, NULL },
};

// line 4640
extern const SymbolicEntry gConvVarOpCheck[8];
const SymbolicEntry gConvVarOpCheck[8] = {
    { CONV_VAR_CHECK_OP_INVALID, "- None -" },
    { CONV_VAR_CHECK_OP_EQUAL, "Equal To" },
    { CONV_VAR_CHECK_OP_LESS_THAN, "Less Than" },
    { CONV_VAR_CHECK_OP_GREATER_THAN, "Greater Than" },
    { CONV_VAR_CHECK_OP_LEQ, "Less Than Or Equal To" },
    { CONV_VAR_CHECK_OP_GEQ, "Greater Than Or Equal To" },
    { CONV_VAR_CHECK_OP_NOT_EQUAL, "Not Equal To" },
    { 0, NULL },
};

// line 4654
extern const SymbolicEntry gConvVarOpSet[5];
const SymbolicEntry gConvVarOpSet[5] = {
    { CONV_VAR_SET_OP_INVALID, "- None -" },
    { CONV_VAR_SET_OP_INC, "Incremented By" },
    { CONV_VAR_SET_OP_DEC, "Decremented By" },
    { CONV_VAR_SET_OP_SET, "Set To" },
    { 0, NULL },
};

// line 4713
extern const SymbolicEntry gActorGenderNames[3];
const SymbolicEntry gActorGenderNames[3] = {
    { ACTOR_GENDER_MALE, "Male" },
    { ACTOR_GENDER_FEMALE, "Female" },
    { 0, NULL },
};

// line 4732
extern const SymbolicEntry gPlayerClasses[9];
const SymbolicEntry gPlayerClasses[9] = {
    { PLAYER_CLASS_NONE, "- None -" },
    { PLAYER_CLASS_DEMONHUNTER, "DemonHunter" },
    { PLAYER_CLASS_BARBARIAN, "Barbarian" },
    { PLAYER_CLASS_WIZARD, "Wizard" },
    { PLAYER_CLASS_WITCHDOCTOR, "Witchdoctor" },
    { PLAYER_CLASS_MONK, "Monk" },
    { PLAYER_CLASS_CRUSADER, "Crusader" },
    { PLAYER_CLASS_NECROMANCER, "Necromancer" },
    { 0, NULL },
};

// line 4746
extern const SymbolicEntry gCrafterTypes[7];
const SymbolicEntry gCrafterTypes[7] = {
    { CRAFTER_TYPE_BLACKSMITH, "Blacksmith" },
    { CRAFTER_TYPE_JEWEL, "Jeweler" },
    { CRAFTER_TYPE_MYSTIC, "Mystic" },
    { CRAFTER_TYPE_JEWEL_UPGRADE, "Jewel Upgrade" },
    { CRAFTER_TYPE_HORADRIM, "Horadrim" },
    { CRAFTER_TYPE_INVALID, "- None -" },
    { 0, NULL },
};

// line 4762
extern const SymbolicEntry gCurrencyTypes[22];
const SymbolicEntry gCurrencyTypes[22] = {
    { CURRENCY_INVALID, "- None -" },
    { CURRENCY_GOLD, "Gold" },
    { CURRENCY_BLOODSHARDS, "BloodShards" },
    { CURRENCY_PLATINUM, "Platinum" },
    { CURRENCY_REUSABLE_PARTS, "Reusable Parts" },
    { CURRENCY_ARCANE_DUST, "Arcane Dust" },
    { CURRENCY_VEILED_CRYSTAL, "Veiled Crystal" },
    { CURRENCY_DEATHS_BREATH, "Deaths Breath" },
    { CURRENCY_FORGOTTEN_SOUL, "Forgotten Soul" },
    { CURRENCY_KHANDURAN_RUNE, "Khanduran Rune" },
    { CURRENCY_CALDEUM_NIGHTSHADE, "Caldeum Nightshade" },
    { CURRENCY_ARREAT_WAR_TAPESTRY, "Arreat War Tapestry" },
    { CURRENCY_CORRUPTED_ANGEL_FLESH, "Corrupted Angel Flesh" },
    { CURRENCY_WESTMARCH_HOLY_WATER, "Westmarch Holy Water" },
    { CURRENCY_DEMON_ORGAN_DIABLO, "Demon Organ Diablo" },
    { CURRENCY_DEMON_ORGAN_GHOM, "Demon Organ Ghom" },
    { CURRENCY_DEMON_ORGAN_SIEGE_BREAKER, "Demon Organ Siege Breaker" },
    { CURRENCY_DEMON_ORGAN_SKELETON_KING, "Demon Organ Skeleton King" },
    { CURRENCY_DEMON_ORGAN_EYE, "Demon Organ Eye" },
    { CURRENCY_DEMON_ORGAN_SPINE_CORD, "Demon Organ Spine Cord" },
    { CURRENCY_DEMON_ORGAN_TOOTH, "Demon Organ Tooth" },
    { 0, NULL },
};

// line 4824
extern const SymbolicEntry gConvTypes[15];
const SymbolicEntry gConvTypes[15] = {
    { CONV_TYPE_QUEST_EVENT, "Quest Event" },
    { CONV_TYPE_QUEST_FLOAT, "Quest Float" },
    { CONV_TYPE_QUEST_STANDARD, "Quest Standard" },
    { CONV_TYPE_TALK_MENU, "Talk Menu Gossip" },
    { CONV_TYPE_AMBIENT_GOSSIP, "Ambient Gossip" },
    { CONV_TYPE_LORE, "Lore Book" },
    { CONV_TYPE_GLOBAL_FLOAT, "Global Float" },
    { CONV_TYPE_GLOBAL_CHATTER, "Global Chatter" },
    { CONV_TYPE_PLAYER_CALLOUT, "Player Callout" },
    { CONV_TYPE_FOLLOWER_CALLOUT, "Follower Callout" },
    { CONV_TYPE_FOLLOWER_BANTER, "Follower Banter" },
    { CONV_TYPE_AMBIENT_FLOAT, "Ambient Float" },
    { CONV_TYPE_PLAYER_EMOTE, "Player Emote" },
    { CONV_TYPE_FOLLOWER_SOUNDSET, "Follower Soundset" },
    { 0, NULL },
};

// line 4901
extern const SymbolicEntry gInventorySlots[24];
const SymbolicEntry gInventorySlots[24] = {
    { SLOT_PLAYER_BACKPACK, "Backpack Items" },
    { SLOT_PLAYER_HEAD, "Head" },
    { SLOT_PLAYER_TORSO, "Torso" },
    { SLOT_PLAYER_RIGHT_HAND, "Right Hand" },
    { SLOT_PLAYER_LEFT_HAND, "Left Hand" },
    { SLOT_PLAYER_HANDS, "Hands" },
    { SLOT_PLAYER_WAIST, "Waist" },
    { SLOT_PLAYER_FEET, "Feet" },
    { SLOT_PLAYER_SHOULDERS, "Shoulders" },
    { SLOT_PLAYER_LEGS, "Legs" },
    { SLOT_PLAYER_BRACERS, "Bracers" },
    { SLOT_PLAYER_LEFT_FINGER, "Left Finger" },
    { SLOT_PLAYER_RIGHT_FINGER, "Right Finger" },
    { SLOT_PLAYER_NECK, "Neck" },
    { SLOT_PLAYER_BUYBACK, "Buyback" },
    { SLOT_PLAYER_SHARED_STASH, "Shared Stash" },
    { SLOT_PLAYER_BASE_HEALTHPOTION, "Base Health Potion" },
    { SLOT_HIRELING_RIGHT_HAND, "Hireling Right Hand" },
    { SLOT_HIRELING_LEFT_HAND, "Hireling Left Hand" },
    { SLOT_HIRELING_SPECIAL, "Hireling Special" },
    { SLOT_HIRELING_LEFT_FINGER, "Hireling Left Finger" },
    { SLOT_HIRELING_RIGHT_FINGER, "Hireling Right Finger" },
    { SLOT_HIRELING_NECK, "Hireling Neck" },
    { 0, NULL },
};

// line 5042
extern const SymbolicEntry gLabelConditionTypes[4];
const SymbolicEntry gLabelConditionTypes[4] = {
    { LABEL_CONDITION_ALWAYS, "Always" },
    { LABEL_CONDITION_LABEL_ALREADY_SET, "Label Already Set" },
    { LABEL_CONDITION_GAME_IS_OPEN_WORLD, "Game Is Open World" },
    { 0, NULL },
};

// line 5061
extern const SymbolicEntry gDifficulty[7];
const SymbolicEntry gDifficulty[7] = {
    { DL_NORMAL, "Normal" },
    { DL_NIGHTMARE, "Nightmare" },
    { DL_HELL, "Hell" },
    { DL_INFERNO, "Inferno" },
    { DL_X1, "X1" },
    { DL_DYNAMIC, "Dynamic" },
    { 0, NULL },
};

// line 5074
extern const SymbolicEntry gHandicapNames[21];
const SymbolicEntry gHandicapNames[21] = {
    { 0, "Difficulty0" },
    { 1, "Difficulty1" },
    { 2, "Difficulty2" },
    { 3, "Difficulty3" },
    { 4, "Difficulty4" },
    { 5, "Difficulty5" },
    { 6, "Difficulty6" },
    { 7, "Difficulty7" },
    { 8, "Difficulty8" },
    { 9, "Difficulty9" },
    { 10, "Difficulty10" },
    { 11, "Difficulty11" },
    { 12, "Difficulty12" },
    { 13, "Difficulty13" },
    { 14, "Difficulty14" },
    { 15, "Difficulty15" },
    { 16, "Difficulty16" },
    { 17, "Difficulty17" },
    { 18, "Difficulty18" },
    { 19, "Difficulty19" },
    { 0, NULL },
};

// line 5101
extern const SymbolicEntry gQuestStepObjectiveTypes[21];
const SymbolicEntry gQuestStepObjectiveTypes[21] = {
    { OBJECTIVE_HAD_CONVERSATION, "Had Conversation" },
    { OBJECTIVE_POSSESS_ITEM, "Possess Item" },
    { OBJECTIVE_KILL_MONSTER, "Kill Monster" },
    { OBJECTIVE_INTERACT_WITH_GIZMO, "Interact With Actor" },
    { OBJECTIVE_ENTER_LEVEL_AREA, "Enter Level Area" },
    { OBJECTIVE_ENTER_SCENE, "Enter Scene" },
    { OBJECTIVE_ENTER_WORLD, "Enter World" },
    { OBJECTIVE_ENTER_TRIGGER, "Enter Trigger" },
    { OBJECTIVE_COMPLETE_QUEST, "Complete Quest" },
    { OBJECTIVE_PLAYER_FLAG_SET, "Player Flag Set" },
    { OBJECTIVE_TIMED_EVENT_EXPIRED, "Timed Event Expired" },
    { OBJECTIVE_KILL_GROUP, "Kill Group" },
    { OBJECTIVE_GAME_FLAG_SET, "Game Flag Set" },
    { OBJECTIVE_EVENT_RECEIVED, "Event Received" },
    { OBJECTIVE_KILL_MONSTER_FROM_GROUP, "Kill Monster From Group" },
    { OBJECTIVE_KILL_MONSTER_FROM_FAMILY, "Kill Monster From Family" },
    { OBJECTIVE_KILL_ELITE_PACK_IN_LEVEL_AREA, "Kill Elite Pack In Level Area" },
    { OBJECTIVE_KILL_ANY_MONSTER_IN_LEVEL_AREA, "Kill Any Monster In Level Area" },
    { OBJECTIVE_KILL_ALL_MONSTERS_IN_WORLD, "Kill All Monsters In World" },
    { OBJECTIVE_HQ_ACT_REAGENTS_COLLECTED, "HQ Act Reagents Collected" },
    { 0, NULL },
};

// line 5127
extern const SymbolicEntry gQuestStepFailureConditionTypes[9];
const SymbolicEntry gQuestStepFailureConditionTypes[9] = {
    { FAILURE_CONDITION_MONSTER_DIED, "Monster Died" },
    { FAILURE_CONDITION_PLAYER_DIED, "Player Died" },
    { FAILURE_CONDITION_ACTOR_DIED, "Actor Died" },
    { FAILURE_CONDITION_TIMED_EVENT_EXPIRED, "Timed Event Expired" },
    { FAILURE_CONDITION_ITEM_USED, "Item Used" },
    { FAILURE_CONDITION_GAME_FLAG_SET, "Game Flag Set" },
    { FAILURE_CONDITION_PLAYER_FLAG_SET, "Player Flag Set" },
    { FAILURE_CONDITION_EVENT_RECEIVED, "Event Received" },
    { 0, NULL },
};

// line 5151
extern const SymbolicEntry gQuestTypes[10];
const SymbolicEntry gQuestTypes[10] = {
    { QUEST_TYPE_MAIN_QUEST, "Main Quest" },
    { QUEST_TYPE_EVENT, "Event" },
    { QUEST_TYPE_CHALLENGE, "Challenge" },
    { QUEST_TYPE_BOUNTY, "Bounty" },
    { QUEST_TYPE_HORADRIC_QUEST, "Horadric Quest" },
    { QUEST_TYPE_SET_DUNGEON, "Set Dungeon" },
    { QUEST_TYPE_SET_DUNGEON_BONUS, "Set Dungeon Bonus" },
    { QUEST_TYPE_SET_DUNGEON_MASTERY, "Set Dungeon Mastery" },
    { QUEST_TYPE_SET_DUNGEON_TRACKER, "Set Dungeon Tracker" },
    { 0, NULL },
};

// line 5172
extern const SymbolicEntry gBountyTypes[9];
const SymbolicEntry gBountyTypes[9] = {
    { BOUNTY_TYPE_INVALID, "- None -" },
    { BOUNTY_TYPE_KILL_UNIQUE, "Kill Unique" },
    { BOUNTY_TYPE_KILL_BOSS, "Kill Boss" },
    { BOUNTY_TYPE_COMPLETE_EVENT, "Complete Event" },
    { BOUNTY_TYPE_CLEAR_DUNGEON, "Clear Dungeon" },
    { BOUNTY_TYPE_CAMPS, "Camps" },
    { BOUNTY_TYPE_HQ_COLOR_ORBS, "HQ - Color Orbs" },
    { BOUNTY_TYPE_HQ_HOT_COLD, "HQ - Hot Cold" },
    { 0, NULL },
};

// line 5188
extern const SymbolicEntry gChallengeTypes[7];
const SymbolicEntry gChallengeTypes[7] = {
    { -1, "- None -" },
    { 0, "Timed Dungeon" },
    { 1, "Wave Fight" },
    { 2, "Horde" },
    { 3, "Zapper" },
    { 4, "Goblin Hunt" },
    { 0, NULL },
};

// line 5211
extern const SymbolicEntry gQuestRewardTypes[5];
const SymbolicEntry gQuestRewardTypes[5] = {
    { QUEST_REWARD_NO_ITEM, "- No Item -" },
    { QUEST_REWARD_SHARED_RECIPE, "Shared Recipe" },
    { QUEST_REWARD_INDIVIDUAL_RECIPE, "Class Recipe" },
    { QUEST_REWARD_TREASURE_CLASS, "Treasure Class" },
    { 0, NULL },
};

// line 5221
extern const SymbolicEntry gActorTypes[13];
const SymbolicEntry gActorTypes[13] = {
    { ACTORTYPE_INVALID, "Invalid" },
    { ACTORTYPE_MONSTER, "Monster" },
    { ACTORTYPE_GIZMO, "Gizmo" },
    { ACTORTYPE_CLIENT_EFFECT, "Client Effect" },
    { ACTORTYPE_SERVER_PROP, "Server Prop" },
    { ACTORTYPE_ENVIRONMENT, "Environment" },
    { ACTORTYPE_CRITTER, "Critter" },
    { ACTORTYPE_PLAYER, "Player" },
    { ACTORTYPE_ITEM, "Item" },
    { ACTORTYPE_AXE_SYMBOL, "Axe Symbol" },
    { ACTORTYPE_PROJECTILE, "Projectile" },
    { ACTORTYPE_CUSTOM_BRAIN, "Custom Brain" },
    { 0, NULL },
};

// line 5248
extern const SymbolicEntry gPowerFailureReasons[31];
const SymbolicEntry gPowerFailureReasons[31] = {
    { PS_CANCELABLE_SKILLS_ACTIVE, "Cancelable Skills Active" },
    { PS_UNCANCELABLE_SKILLS_ACTIVE, "Uncancelable Skills Active" },
    { PS_NOT_CLOSE_ENOUGH_BIT, "Not Close Enough" },
    { PS_NEED_COOLDOWN_BIT, "Need Cooldown" },
    { PS_NEED_HEALTH_BIT, "Need Health" },
    { PS_NEED_CHARGES_BIT, "Need Charges" },
    { PS_NEED_RESOURCE_BIT, "Need Resource" },
    { PS_NOT_IN_TOWN, "Not In Town" },
    { PS_INVALID_TARGET, "Invalid Target" },
    { PS_INVALID_SOURCE, "Invalid Source" },
    { PS_NO_LINE_OF_SIGHT, "No Line of Sight" },
    { PS_WRONG_WEAPON, "Wrong Weapon" },
    { PS_IMMOBILIZED, "Immobilized" },
    { PS_FEARED, "Feared" },
    { PS_NOT_IN_COMBAT, "Not In Combat" },
    { PS_ONLY_IN_TOWN_PORTAL_AREA, "Only In Town Portal Area" },
    { PS_TARGET_NOT_IN_TELEPORTABLE_AREA, "Target Not In Teleportable Area" },
    { PS_UNUSABLE_DURING_BOSS_ENCOUNTER, "Not During Boss Encounter" },
    { PS_NEED_GENERAL_COOLDOWN_BIT, "Need General Cooldown" },
    { PS_SILENCED, "Silenced" },
    { PS_FROZEN, "Frozen" },
    { PS_UNABLE_TO_FREECAST_BIT, "Unable to Freecast" },
    { PS_TARGET_IS_DEAD, "Target Is Dead" },
    { PS_MISSING_ITEM, "Missing Item" },
    { PS_CASTER_IS_DEAD, "Caster Is Dead" },
    { PS_CASTER_CANT_START_DISPLAYED_POWERS, "Caster Cant Start Displayed Powers" },
    { PS_CASTER_IS_STUNNED, "Caster Is Stunned" },
    { PS_POWER_DISABLED, "Disabled" },
    { PS_NEED_SKILLPOINT, "Needs Skillpoint" },
    { PS_NEED_CORPSES, "Needs Corpses" },
    { 0, NULL },
};

// line 5389
extern const SymbolicEntry gHirelingClass[5];
const SymbolicEntry gHirelingClass[5] = {
    { HC_NONE, "- None -" },
    { HC_TEMPLAR, "Templar" },
    { HC_SCOUNDREL, "Scoundrel" },
    { HC_ENCHANTRESS, "Enchantress" },
    { 0, NULL },
};

// line 5440
extern const SymbolicEntry gFontHintingModes[4];
const SymbolicEntry gFontHintingModes[4] = {
    { FHM_NO_HINTING, "Disabled" },
    { FHM_NATIVE_HINTING, "Native" },
    { FHM_AUTO_HINTING, "Auto" },
    { 0, NULL },
};

// line 5449
extern const SymbolicEntry gFontHintingStyles[5];
const SymbolicEntry gFontHintingStyles[5] = {
    { FHS_NORMAL, "Normal" },
    { FHS_LIGHT, "Light" },
    { FHS_LCD, "LCD" },
    { FHS_LCD_VERTICAL, "LCD Vertical" },
    { 0, NULL },
};
