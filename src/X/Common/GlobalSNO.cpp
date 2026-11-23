// Original file: X/Common/GlobalSNO.cpp
// Bootstrap generates the registration calls and GBID rows from the local 2.6.2 image.
// See tools/bootstrap_game_data.py.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/Sigma.h"
#include "../Sigma/BlzExt/BlzVectorHelper.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "Game.h"
#include "Definitions/GameBalanceDefinition.h"
#include "GlobalSNO.h"

// GameBalanceDefinition.h:49 (GameBalanceType); src's header has a subset, the rest is kept local.
enum { GB_ITEMTYPES = 1, GB_HIRELINGS = 4, GB_MONSTER_AFFIXES = 18 };
// X/Common/TagMaps/ActorTags.cpp:92 onwards; not declared in src's ActorTags.h yet.
extern const char * const ACTOR_DEFAULT_EFFECT_FROZEN_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_CHILLED_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_FIRE_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_LIGHTNING_EFFECTGROUP;
extern const char * const ACTOR_DEFAULT_EFFECT_POISONED_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_ARCANE_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_HOLY_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_POISON_DOT_EFFECTGROUP;
extern const char * const ACTOR_DEFAULT_EFFECT_HOLY_DOT_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_SLOWED_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_CHILLED_CRIT_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_FIRE_CRIT_EFFECTGROUP;
extern const char * const ACTOR_DEFAULT_EFFECT_LIGHTNING_CRIT_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_POISONED_CRIT_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_ARCANE_CRIT_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_HOLY_CRIT_EFFECTGROUP;
extern const char * const ACTOR_DEFAULT_EFFECT_CRIT_DEATH_PHYSICAL, * const ACTOR_DEFAULT_EFFECT_CRIT_DEATH_FIRE, * const ACTOR_DEFAULT_EFFECT_CRIT_DEATH_POISON, * const ACTOR_DEFAULT_EFFECT_CRIT_DEATH_ARCANE;
extern const char * const ACTOR_DEFAULT_EFFECT_CRIT_DEATH_HOLY, * const ACTOR_DEFAULT_EFFECT_CRIT_DEATH_LIGHTNING, * const ACTOR_DEFAULT_EFFECT_CRIT_DEATH_COLD, * const ACTOR_DEFAULT_EFFECT_RARE_DEATH;
extern const char * const ACTOR_DEFAULT_EFFECT_DEATH_BLEED, * const ACTOR_DEFAULT_EFFECT_DEATH_FATALITY, * const ACTOR_DEFAULT_EFFECT_DEATH_EXPLODING_PALM, * const ACTOR_DEFAULT_EFFECT_CORPSE_EXPLODE_PARTICLE;
extern const char * const ACTOR_DEFAULT_EFFECT_BURNING_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_RARITY_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_CRITICAL_HIT_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_GET_HIT_NORMAL_EFFECTGROUP;
extern const char * const ACTOR_DEFAULT_EFFECT_SHIELD_BLOCK_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_FIRE_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_ICE_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_LIGHTNING_EFFECTGROUP;
extern const char * const ACTOR_DEFAULT_EFFECT_WEAPON_POISON_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_ARCANE_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_HOLY_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_SPELL_DAMAGE_EFFECTGROUP;
extern const char * const ACTOR_DEFAULT_EFFECT_WEAPON_LIFESTEAL_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_MANASTEAL_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_MAGIC_FIND_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_GOLD_FIND_EFFECTGROUP;
extern const char * const ACTOR_DEFAULT_EFFECT_WEAPON_ATTACK_SPEED_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_CAST_SPEED_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_WITCHDOCTOR_DAMAGE_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_WEAPON_WIZARD_DAMAGE_EFFECTGROUP;
extern const char * const ACTOR_DEFAULT_EFFECT_UNSTOPPABLE_WILL_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_BATTLE_RAGE_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_BURROWED_EFFECTGROUP, * const ACTOR_DEFAULT_EFFECT_PLAGUED_EFFECTGROUP;
extern const char * const ACTOR_DEFAULT_EFFECT_TELEPORT_INVULNERABILITY_EFFECTGROUP;






static BOOL sLookupSNO(const uint32 dwSigmaPlatformBits,
                       const GlobalSNOBucket eBucket,
                       const GlobalSNO eGlobalSNO,
                       const SNOGroup snoGroup,
                       LPCSTR szPath,
                       uint32 dwFlags)

{
    if (!(dwSigmaPlatformBits & (1 << SigmaPlatformGet())))
    {
        return FALSE;
    }


    AppGlobals & tAppGlobals = sgtAppGlobals;
    blzext::vector_set_at_grow(*tAppGlobals.ptGlobalSNOs, eGlobalSNO, GlobalSNOEntry(eBucket, SNOName(snoGroup, szPath), dwFlags));

    return TRUE;
}














































void GlobalSNOListRefresh()

{









    AppGlobals & tAppGlobals = sgtAppGlobals;


    tAppGlobals.ptGlobalSNOs->clear();

    for (int32 i = 0; i < NUM_GLOBAL_SNOS; i++)
    {
        blzext::vector_set_at_grow(*tAppGlobals.ptGlobalSNOs, i, GlobalSNOEntry(BUCKET_MAIN, SNONAME_NONE, (1 << GLOBALSNO_FLAG_DO_NOT_PRELOAD_BIT) | (1 << GLOBALSNO_FLAG_DO_NOT_TRAVERSE_IN_COMMON_CPK_BIT)));
    }

#include "generated-game-data/sno_calls.inc"
#line 2640
}





































































































































































































































































































































































































struct TGlobalGBID {  // size 0x48
    GlobalGBID eId;  // 0x00
    char szName[64];  // 0x04
    GameBalanceType eType;  // 0x44
};


extern const BOOL cgfDefaultPlayerSexIsMale = FALSE;

static TGlobalGBID sgtGlobalGBIDs[] =
{
#include "generated-game-data/gbid_rows.inc"
#line 3160
};















static const int32 cnNumGlobalGBIDs = sizeof(sgtGlobalGBIDs) / sizeof(sgtGlobalGBIDs[0]);

static GBID sggbidGlobals[NUM_GLOBAL_GBIDS];



static void sGlobalGBIDListClear()
{

    for (int32 i = 0; i < NUM_GLOBAL_GBIDS; i++)
    {
        sggbidGlobals[i] = GBID_NONE;
    }
}




void GlobalGBIDListInitializeForApp()
{
    sGlobalGBIDListClear();
}




void GlobalGBIDListDisposeForApp()
{
    sGlobalGBIDListClear();
}




void GlobalGBIDListRefresh()
{


    for (int32 i = 0; i < cnNumGlobalGBIDs; i++)
    {
        GBID gbid = GBIDExists(sgtGlobalGBIDs[i].szName, sgtGlobalGBIDs[i].eType);
        sggbidGlobals[sgtGlobalGBIDs[i].eId] = gbid;
    }
}










SNO GlobalSNOGet(const GlobalSNO eGlobalSNO)
{
    if (eGlobalSNO == GLOBALSNO_INVALID)
    {
        return SNO_NONE;
    }



    AppGlobals & tAppGlobals = sgtAppGlobals;
    SNO snoHandle = (*tAppGlobals.ptGlobalSNOs)[eGlobalSNO].snoname.snoHandle;
    return snoHandle;
}






















GBID GlobalGBIDGet(GlobalGBID eGlobalGBID)
{
    if (eGlobalGBID < 0 || eGlobalGBID >= NUM_GLOBAL_GBIDS)
    {

        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return sggbidGlobals[eGlobalGBID];
}
