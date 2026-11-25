#include "../Common/ActorCommonData.h"
#include "Actors/Actors.h"
#include "SGame.h"
#include "Timers.h"
#include "SWorld.h"
#include "../Common/GlobalSNO.h"
#include "../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"

SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);

// Each entry initialized at 0x71007d4c40 holds a world and a portal GlobalSNO.
struct unk_1a39a58_entry
{
    GlobalSNO unk_0;
    GlobalSNO unk_4;
};
static blz::vector<unk_1a39a58_entry> unk_1a39a58 = {
    {GLOBALSNO_UBER_BOSS_WORLD_04, GLOBALSNO_UNK_709},
    {GLOBALSNO_UBER_BOSS_WORLD_01, GLOBALSNO_UNK_706},
    {GLOBALSNO_UBER_BOSS_WORLD_02, GLOBALSNO_UNK_707},
    {GLOBALSNO_UBER_BOSS_WORLD_03, GLOBALSNO_UNK_708}
};

static blz::vector<int32> unk_1a39a70 = {3, 0, 1, 2};

// NON_MATCHING: The flag shift moves across SGameGlobalsGet, changing five instructions in the bit clear.
extern "C" void sub_71007d4890(TimerID, DataID idOwner, void *param)
{
    uint32 index = (uint32)(uintptr_t)param;
    SWorldRemovePlayersAndFlagForDeletion(WorldCommonFindBySNO(GlobalSNOGet(unk_1a39a58[index].unk_0), TRUE));
    ActorFlagForDeletion((ActorID)ACDGetOwner((ACDID)idOwner));
    int32 bit = unk_1a39a70[index];
    SGameGlobalsGet().uPandemoniumFlags &= ~(1u << bit);
    SGameGlobals &globals = SGameGlobalsGet();
    POS pos = globals.listCompletedBossEncounters.Find(GlobalSNOGet(unk_1a39a58[index].unk_4));
    if (pos)
        globals.listCompletedBossEncounters.RemoveAt(pos);
}

extern "C" BOOL sub_71007d4af0(SNO snoWorld)
{
    if (snoWorld == SNO_NONE)
        return FALSE;
    return blz::find_if(unk_1a39a58.begin(), unk_1a39a58.end(),
        [snoWorld](const unk_1a39a58_entry &entry) { return GlobalSNOGet(entry.unk_0) == snoWorld; }) != unk_1a39a58.end();
}

extern "C" int32 sub_71007d4b90(SNO snoWorld)
{
    if (snoWorld != SNO_NONE)
    {
        auto it = blz::find_if(unk_1a39a58.begin(), unk_1a39a58.end(),
            [snoWorld](const unk_1a39a58_entry &entry) { return GlobalSNOGet(entry.unk_0) == snoWorld; });
        if (it != unk_1a39a58.end())
            return it - unk_1a39a58.begin();
    }
    return -1;
}
