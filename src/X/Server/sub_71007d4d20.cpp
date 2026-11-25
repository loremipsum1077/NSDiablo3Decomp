#include "../Common/ActorCommonData.h"
#include "Actors/Actors.h"
#include "SGame.h"
#include "Timers.h"
#include "../Common/GlobalSNO.h"
#include "../Common/World.h"
#include "../Sigma/Random.h"
#include "SWorld.h"
#include "../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"

SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);

// The initializer at 0x71007d51f0 creates the three five-element tables.
static blz::vector<GlobalSNO> unk_1a39aa8 = {
    GLOBALSNO_UNK_1231, GLOBALSNO_UNK_1232, GLOBALSNO_UNK_1233, GLOBALSNO_UNK_1234, GLOBALSNO_UNK_1235
};
static blz::vector<GlobalSNO> unk_1a39ac0 = {
    GLOBALSNO_UNK_689, GLOBALSNO_UNK_690, GLOBALSNO_UNK_691, GLOBALSNO_UNK_692, GLOBALSNO_UNK_693
};

static blz::vector<GlobalSNO> unk_1a39ad8 = {
    GLOBALSNO_UNK_701, GLOBALSNO_UNK_702, GLOBALSNO_UNK_703, GLOBALSNO_UNK_704, GLOBALSNO_UNK_705
};

extern "C" void sub_71007d4db0(TimerID, DataID idOwner, void *)
{
    for (GlobalSNO entry : unk_1a39ac0)
        SWorldRemovePlayersAndFlagForDeletion(WorldCommonFindBySNO(GlobalSNOGet(entry), TRUE));
    ActorFlagForDeletion((ActorID)ACDGetOwner((ACDID)idOwner));
    SGameGlobals &globals = SGameGlobalsGet();
    for (GlobalSNO entry : unk_1a39ad8)
    {
        POS pos = globals.listCompletedBossEncounters.Find(GlobalSNOGet(entry));
        if (pos)
            globals.listCompletedBossEncounters.RemoveAt(pos);
    }
}

extern "C" SNO sub_71007d4d20()
{
    RNG rng;
    RandInit(rng);
    return GlobalSNOGet(unk_1a39aa8[RandGetDWord(rng) % (uint32)unk_1a39aa8.size()]);
}

extern "C" BOOL sub_71007d50c0(SNO snoWorld)
{
    if (snoWorld == SNO_NONE)
        return FALSE;
    return blz::find_if(unk_1a39ac0.begin(), unk_1a39ac0.end(),
        [snoWorld](GlobalSNO entry) { return GlobalSNOGet(entry) == snoWorld; }) != unk_1a39ac0.end();
}

extern "C" BOOL sub_71007d5020(SWorldID idWorld)
{
    return idWorld != SWORLDID_INVALID && sub_71007d50c0(WorldCommonGetWorldSNO(idWorld));
}

extern "C" BOOL sub_71007d5160()
{
    for (GlobalSNO entry : unk_1a39ac0)
    {
        GlobalSNOGet(GLOBALSNO_WORLD_GREED_REALM);
        GlobalSNOGet(GLOBALSNO_UNK_686);
        if (SWorldFindBySNO(GlobalSNOGet(entry)) != SWORLDID_INVALID)
            return TRUE;
    }
    return FALSE;
}
