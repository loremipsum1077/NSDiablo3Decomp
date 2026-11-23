// Checks the inline bodies in X/Common/Definitions/HeroDefinition.h through small functions of
// the binary that inline them, written here from their disassembly (llvm-objdump -l) with the
// line structure of the original files. Each one is compared by try_file.py:
//   SWorldPlayerWarpToLastSavepoint   SavePointData::IsValid / HasValidSavePoint / HasValidPortalDestination
//   SPlayerHasLearnedLore             LearnedLore::Find
//   PlayerHasUnlockedConversation     SavedConversations::IsUnlocked
//   PlayerSaveConversation            SavedConversations::Save(sno, TRUE)
//   PlayerUnlockConversation          SavedConversations::Save(sno, FALSE)
//   ConversationMgr::MarkSavedConversationsAsHeard   GetNumConversations, HasPlayed(index, sno)
//   SPlayerInitGameSavePointFromPlayer               SavePointData::Import
// The callees and the owning types that are not in src/ yet are stand-ins (declared below).
#include "X/Common/Definitions/HeroDefinition.h"
#include "X/Common/Player.h"

typedef int32 WarpToSceneMethod;
void sSWorldPlayerWarpToScene(ACDID idACD, SNO snoScene, WarpToSceneMethod eMethod, ACDID idACDPortal, const void * pvData);
BOOL GameIsWeeklyChallenge();
SNO SNOFileLookupGUID(SNOGroup eGroup, const char * szName);
void LuaVerifySnosAreReferenced(SNO, SNO, SNO, SNO, SNO, SNO, SNO);
void SPlayerMarkConversationAsHeard(Player * ptPlayer, SNO snoConv);

// X/Server/SWorld.cpp:983
void SWorldPlayerWarpToLastSavepoint(const Player & tPlayer)
{
    const SavePointData & tData = tPlayer.tHeroStateData.tSavedData.tLastSavePoint;
    if (tData.IsValid())
    {
        sSWorldPlayerWarpToScene(tPlayer.idPlayerACD, SNO_NONE, 2, ACDID_INVALID, &tData);
    }
}

// X/Server/SPlayer.cpp:7349
BOOL SPlayerHasLearnedLore(ACDID idACD, SNO snoLore)
{
    if (GameIsWeeklyChallenge())
    {
        return TRUE;
    }
    if (snoLore == SNO_NONE)
    {
        return FALSE;
    }
    Player * ptPlayer = PlayerGetByACD(idACD);
    if (!ptPlayer)
    {
        return FALSE;
    }
    int32 nIndex = ptPlayer->tHeroStateData.tSavedData.tLearnedLore.Find(snoLore);
    if (nIndex != -1)
    {
        return TRUE;
    }
    return FALSE;
}

// X/Server/LuaScript/ScriptUtil.cpp:2563
BOOL PlayerHasUnlockedConversation(ACDID idACD, const char * szConv)
{
    Player * ptPlayer = PlayerGetByACD(idACD);
    if (!ptPlayer)
    {
        return FALSE;
    }
    SNO snoConv = SNOFileLookupGUID(12, szConv);
    if (snoConv == SNO_NONE)
    {
        return FALSE;
    }
    LuaVerifySnosAreReferenced(snoConv, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    return ptPlayer->tHeroStateData.tSavedData.tSavedConversations.IsUnlocked(snoConv);
}

// X/Server/LuaScript/ScriptUtil.cpp:2586
void PlayerUnlockConversation(ACDID idACD, const char * szConv)
{
    Player * ptPlayer = PlayerGetByACD(idACD);
    if (!ptPlayer)
    {
        return;
    }
    SNO snoConv = SNOFileLookupGUID(12, szConv);
    if (snoConv == SNO_NONE)
    {
        return;
    }
    LuaVerifySnosAreReferenced(snoConv, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    ptPlayer->tHeroStateData.tSavedData.tSavedConversations.Save(snoConv, FALSE);
    ptPlayer->uConsoleSyncFlags |= 8;
}

// X/Server/LuaScript/ScriptUtil.cpp:2613
void PlayerSaveConversation(ACDID idACD, const char * szConv)
{
    Player * ptPlayer = PlayerGetByACD(idACD);
    if (!ptPlayer)
    {
        return;
    }
    SNO snoConv = SNOFileLookupGUID(12, szConv);
    if (snoConv == SNO_NONE)
    {
        return;
    }
    LuaVerifySnosAreReferenced(snoConv, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    ptPlayer->tHeroStateData.tSavedData.tSavedConversations.Save(snoConv, TRUE);
    ptPlayer->uConsoleSyncFlags |= 8;
}

// X/Server/ConversationMgr.cpp:3269
struct ConversationMgr {
    static void MarkSavedConversationsAsHeard(Player & tPlayer);
};
void ConversationMgr::MarkSavedConversationsAsHeard(Player & tPlayer)
{
    for (int32 i = 0; i < tPlayer.tHeroStateData.tSavedData.tSavedConversations.GetNumConversations(); i++)
    {
        SNO snoConv;
        if (tPlayer.tHeroStateData.tSavedData.tSavedConversations.HasPlayed(i, snoConv))
        {
            SPlayerMarkConversationAsHeard(&tPlayer, snoConv);
        }
    }
}

// X/Server/SGame.h: SGameGlobals is not adopted; only tGameSavePoint (0x1fc) is placed.
struct SGameGlobals {
    uint8 _unadopted0[0x1fc];
    SavePointData tGameSavePoint;  // 0x1fc
};
SGameGlobals * SGameGlobalsGet();

// X/Server/SPlayer.cpp:7238
void SPlayerInitGameSavePointFromPlayer(const D3::Hero::SavedDefinition & tSavedDef)
{
    SGameGlobals * ptGlobals = SGameGlobalsGet();
    ptGlobals->tGameSavePoint.Import(tSavedDef.saved_data().save_point());
}
