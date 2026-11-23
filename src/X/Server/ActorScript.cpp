// Original file: X/Server/ActorScript.cpp
#include "ActorScript.h"
#include "SGame.h"
#include "SPlayer.h"
#include "LuaScript/LuaScriptUtil.h"
#include "Powers/PowerAPI.h"
#include "Powers/Powers.h"
#include "../Common/ActorCommonData.h"
#include "../Common/ActorInventory.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/Attributes/FastAttributeStructs.h"
#include "../Common/Definitions/ActDefinition.h"
#include "../Common/Definitions/HeroDefinition.h"
#include "../Common/GameCommonData.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Player.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"

struct CachedWaypoint {  // size 0x2c (X/Common/Act.h:13)
    Act eSourceAct;  // 0x00
    WaypointInfo tInfo;  // 0x04
};
typedef blz::vector<CachedWaypoint, blz::allocator<CachedWaypoint> > CachedWaypointArray;  // X/Common/Act.h:19
const CachedWaypointArray & ActGetCachedWaypointArray();  // X/Common/Act.cpp:146
namespace GameTesting { void SetLevel(const ACDID idACDPlayer, const int32 nLevel, const BOOL fUpdateClient); }  // X/Common/GameTesting.cpp:2104
lua_State * LuaScriptGetLuaStateWithSNO(SNO snoScript);  // X/Server/LuaScript/LuaScript.cpp:3353
int32 LuaSNOScriptCall(SNO snoScript, lua_State * L, int32 nArgumentsCnt, int32 nReturnValCnt, LPCSTR szFuncName);  // X/Server/LuaScript/LuaScriptUtil.cpp:1084
void ACDInventoryUpdateSkillButtons(ActorCommonData & tACD);  // X/Common/ActorInventory.cpp:8692
void ACDInventoryCreateBaseHealthPotion(ActorCommonData & tACDPlayer);  // X/Common/ActorInventory.cpp:6401
BOOL PlayerAssignBaseHealthPotion(Player * ptPlayer);  // X/Common/Player.cpp:1703

//@ 49
static void sDeletePlayerItem(ACDID idACDSquare, InventorySlotType, void * pUserData)



{



    Player * ptPlayer = (Player *)pUserData;
    ActorCommonData & tACDItem = ACDGet(idACDSquare);
    uint64 uStackSize = tACDItem.GetStackCount();

    SACDInventoryDeleteItem(tACDItem, uStackSize, TRUE, FALSE);
}

//@ 64
void sDestroyEquipment(Player * ptPlayer)
{

    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    ACDInventoryIterateItems(tACDPlayer, -1, 0, sDeletePlayerItem, ptPlayer);
}

//@ 78
void ActorBoostPvP(ActorCommonData & tACDPlayer, const int32 nLevel, LPCSTR szTemplate, LPCSTR szTemplateParam, const uint32 dwPlayerBoostOptionBits)





{






    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);



    uint32 dwOldFlags = ptPlayer->dwServerSidePrefFlags;
    ptPlayer->dwServerSidePrefFlags |= (1 << SSPREF_AUTOEQUIP_ITEMS);


    int32 nPlayerLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    if (nPlayerLevel < nLevel)
    {
        GameTesting::SetLevel(tACDPlayer.id, nLevel, FALSE);
    }


    sDestroyEquipment(ptPlayer);

    SNO snoBoostScript = GlobalSNOGet(GLOBALSNO_SCRIPT_BOOST);
    lua_State * ptLuaState = LuaScriptGetLuaStateWithSNO(snoBoostScript);



    PowerResetSkills(tACDPlayer, FALSE);
    PowerAPI_BuffFlagAllForDeletion(tACDPlayer);
    LPCSTR szBoostFuncName = "OnPlayerBoost";
    if (LuaScriptGetFunction(ptLuaState, szBoostFuncName))
    {
        lua_pushinteger(ptLuaState, tACDPlayer.id);
        lua_pushinteger(ptLuaState, nLevel);
        lua_pushinteger(ptLuaState, nLevel - nPlayerLevel);
        lua_pushstring(ptLuaState, szTemplate);
        lua_pushstring(ptLuaState, szTemplateParam);

        int nError = LuaSNOScriptCall(snoBoostScript, ptLuaState, 5, 0, szBoostFuncName);
    }







    tACDPlayer.ResetLifeStats(TRUE, FALSE, TRUE, TRUE, FALSE);


    ptPlayer->dwServerSidePrefFlags = dwOldFlags;

    ptPlayer->tHeroStateData.uPlayerFlags |= (1u << PLAYERFLAG_BOOSTED);

    BOOL bNotifyClient = (dwPlayerBoostOptionBits & (1 << PLAYER_BOOST_NOTIFY_CLIENT_BIT)) != 0;
    ACDInventoryUpdateSkillButtons(tACDPlayer);
    if (bNotifyClient)
    {

        tACDPlayer.MarkHeroStatePlayerFlagsChanged();
    }
}

//@ 148
void ActorBoostToLevel(ActorCommonData & tACDPlayer, const int32 nLevel, LPCSTR szTemplate, LPCSTR szTemplateParam, const uint32 dwPlayerBoostOptionBits)





{






    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);


    uint32 dwOldFlags = ptPlayer->dwServerSidePrefFlags;
    ptPlayer->dwServerSidePrefFlags |= (1 << SSPREF_AUTOEQUIP_ITEMS);

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    uint32 dwFlags2 = tSGameGlobals.dwFlags2;
    tSGameGlobals.dwFlags2 |= (1 << 1);

    BOOL bNotifyClient = (dwPlayerBoostOptionBits & (1 << PLAYER_BOOST_NOTIFY_CLIENT_BIT)) != 0;
    BOOL bGiveWaypoints = (dwPlayerBoostOptionBits & (1 << PLAYER_BOOST_GIVE_WAYPOINTS_BIT)) != 0; if (bGiveWaypoints)
    {
        const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
        size_t nCount = tWaypoints.size();
        for (uint32 i = 0; i < nCount; i++)
        {
            SPlayerActivateWaypoint(i, FALSE);
        }
    }


    int32 nPlayerLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));

    if (nPlayerLevel < nLevel)
    {

        BOOL bRemoveEquipment = (dwPlayerBoostOptionBits & (1 << PLAYER_BOOST_REMOVE_EQUIPMENT_FIRST_BIT)) != 0;
        if (bRemoveEquipment)
        {
            sDestroyEquipment(ptPlayer);


            ACDInventoryCreateBaseHealthPotion(tACDPlayer);
            PlayerAssignBaseHealthPotion(ptPlayer);
        }

        BOOL bShowLevelUpUI = (dwPlayerBoostOptionBits & (1 << PLAYER_BOOST_SHOW_LEVEL_UP_UI_BIT)) != 0;
        GameTesting::SetLevel(tACDPlayer.id, nLevel, bShowLevelUpUI);

        SNO snoBoostScript = GlobalSNOGet(GLOBALSNO_SCRIPT_BOOST);
        lua_State * ptLuaState = LuaScriptGetLuaStateWithSNO(snoBoostScript);


        LPCSTR szBoostFuncName = "OnPlayerBoost";
        if (LuaScriptGetFunction(ptLuaState, szBoostFuncName))
        {
            lua_pushinteger(ptLuaState, tACDPlayer.id);
            lua_pushinteger(ptLuaState, nLevel);
            lua_pushinteger(ptLuaState, nLevel - nPlayerLevel);
            lua_pushstring(ptLuaState, szTemplate);
            lua_pushstring(ptLuaState, szTemplateParam);

            int nError = LuaSNOScriptCall(snoBoostScript, ptLuaState, 5, 0, szBoostFuncName);
        }
    }









//@ 222
    tACDPlayer.ResetLifeStats(TRUE, TRUE, TRUE, TRUE, FALSE);


    ptPlayer->dwServerSidePrefFlags = dwOldFlags;

    tSGameGlobals.dwFlags2 = dwFlags2;

    ptPlayer->tHeroStateData.uPlayerFlags |= (1u << PLAYERFLAG_BOOSTED);


    ACDInventoryUpdateSkillButtons(tACDPlayer);
    if (bNotifyClient)
    {

        tACDPlayer.MarkHeroStatePlayerFlagsChanged();
    }
}
