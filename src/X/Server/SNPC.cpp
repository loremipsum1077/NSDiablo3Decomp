// Original file: X/Server/SNPC.cpp

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Player.h"
#include "../Common/Game.h"
#include "Actors/Actors.h"
#include "STalk.h"
#include "ConversationMgr.h"
#include "LuaScript/ScriptEvent.h"
#include "LuaScript/ScriptEventParams.h"
#include "LuaScript/ScriptEventSwig.h"
#include "SNPC.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Target.h"
#include "../Common/AI/ActorAIStatesSwig.h"
#include "Powers/Powers.h"
#include "Powers/PowerAPI.h"
#include "System/LevelArea.h"
#include "../Common/Shell/CmdLine.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/TagMaps/ActorTags.h"
#include "../Common/TagMaps/Tags.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/Definitions/ConversationDefinition.h"
#include "../Common/NPC.h"
#include "../Common/GameCommonData.h"
#include "../Common/GameTemplate.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "AI/AIStates.h"
#include "AI/BrainComponents.h"

extern "C" BOOL sub_710051b9d0(const ActorCommonData & tACD);  // NPC.cpp
extern "C" BOOL sub_710051b060();  // NephalemRiftCommon.cpp

// Declared here from their definitions elsewhere (headers not adopted yet).
void DeleteLastHeadstone(ActorCommonData & tACD); void SHirelingMoveToStable(ACDID idACDPlayer, ActorCommonData & tACDHireling, BOOL bPlayerDismissed); void PlayerRemovePet(ACDID idACDPlayer, ACDID idACDPet, BOOL bDisplayChatMessage);
ACDID PlayerGetHirelingProxy(ACDID idACDPlayer, HirelingClass eClass); void DismissPet(ActorCommonData & tACDPlayer, ActorCommonData & tACDPet, BOOL bPlayerDismissed); void NPCHandleDismissal(ActorCommonData & tACDHireling, BOOL bPlayerDismissed, BOOL bDisplayChatMessage);
typedef int32 NPCInteractionType; BOOL ACDIsFollower(const ActorCommonData & tACD); BOOL ACDIsFollowerProxy(const ActorCommonData & tACD);
BOOL ACDIsHirelingProxy(const ActorCommonData & tACD); ACDID PlayerGetPet(ACDID idACDPlayer, int32 nIndex); void SHirelingSetActive(ACDID idACDPlayer, BOOL bActive, BOOL bForce);
uint32 ConversationTypeGetUsageFlags(const int32 eConvType); BOOL SPlayerHasHeardConversation(const Player * ptPlayer, SNO snoConv); SNO NPCGetConversationsFromList(SNO snoConvList, ACDID idACDPlayer, ACDID idACDNPC, ConversationArray * ptConvArray);
SNO ACDGetInventory(const ActorCommonData * ptACD, BOOL bSecondary); void SACDInventoryReload(ACDID idACD, SNO snoInventory); CrafterType NPCGetCrafterType(const ActorCommonData & tACD);
enum { CTU_STARTS_IMMEDIATELY_ON_NPC_OPERATE_BIT = 8, CTU_ALWAYS_PLAY_IMMEDIATELY_BIT = 9, CTU_SHOWN_IN_TALK_MENU_BIT = 10 }; enum { CF_ALWAYS_IMPORTANT = 21 };  // ConversationDefinition.h
// ConversationDefinition is kept local (as in SPlayer.cpp): the shared header does not adopt it.
struct ConversationDefinition {  // size 0x110
    RequiredDefinitionHeader tHeader;  // 0x00
    int32 eConvType;  // 0x0c (ConversationType)
    ConversationIcon eConvIcon;  // 0x10
    uint8 _unadopted0[0x24 - 0x14];  // 0x14
    uint32 dwFlags;  // 0x24
    uint8 _unadopted1[0x110 - 0x28];  // 0x28
};
template <> inline const ConversationDefinition * DefinitionLockR<ConversationDefinition>(SNO sno) { return (const ConversationDefinition *)gptGroupDefConversation->LockR(sno, FALSE); }
template <> inline void DefinitionUnlockR<ConversationDefinition>(const ConversationDefinition * pdef) { void * lpDef = (void *)pdef; gptGroupDefConversation->UnlockR(lpDef); }
typedef ScopedDefinitionLockR<ConversationDefinition> ScopedConversationDefinitionLockR;




static BOOL sConversationStartsImmediately(const SNO snoConv, const Player & tPlayer)
{
    ScopedConversationDefinitionLockR pdefConv(snoConv);
    uint32 dwUsageFlags = ConversationTypeGetUsageFlags(pdefConv->eConvType);
    if (dwUsageFlags & (1 << CTU_ALWAYS_PLAY_IMMEDIATELY_BIT))
        return TRUE;
    if (dwUsageFlags & (1 << CTU_STARTS_IMMEDIATELY_ON_NPC_OPERATE_BIT))
    {
        if (pdefConv->dwFlags & (1 << CF_ALWAYS_IMPORTANT))
            return TRUE;
        return !SPlayerHasHeardConversation(&tPlayer, snoConv);
    }
    return FALSE;
}

#include "../Common/TagMaps/MarkerTags.h"
#include "../Common/HirelingSwig.h"
#include "../Common/Attributes/FastAttribute.h"
BOOL NPCCanOperate(const ActorCommonData & tACDNPC, const ActorCommonData & tACDPlayer, BOOL bIgnoreDistance);  // X/Common/NPC.cpp
BOOL NPCIsOperableVendorOrCrafter(const ActorCommonData & tACDNPC, const ActorCommonData & tACDPlayer);  // X/Common/NPC.cpp
BOOL NPCUpdateConversationIconForPlayer(ActorCommonData & tACDNPC, const Player & tPlayer, ConversationIcon eIcon);  // X/Common/NPC.cpp
BOOL NPCIsCosmeticsVendor(const ActorCommonData & tACD);  // X/Common/NPC.cpp
BOOL NPCIsEquipmentManagerVendor(const ActorCommonData & tACD);  // X/Common/NPC.cpp
void STrickleUpdateActor(const ActorCommonData & tACD);  // X/Server/Network/STrickle.cpp:305
BOOL SHirelingCanHire(const Player & tPlayer);  // X/Server/SHireling.cpp
int32 GetCraftLevel(const CrafterType eType, const Player & tPlayer);  // X/Common/Crafting/ItemCrafting.h
BOOL NPCGetConversations(ACDID idACDPlayer, ACDID idACDNPC, ConversationArray & tConvArray);
void NPCStartConversation(ActorCommonData & tACDPlayer, ActorCommonData & tACDTalkingTo, const SNO snoConversation, const NPCInteractionType);



static BOOL sGetImportantConversation(const Player & tPlayer, ActorCommonData & tACDNPC, const ConversationArray & tConvArray, SNO & snoConv)
{
    if (!tACDNPC.IsAlive() || tConvArray.size() == 0)
        return FALSE;

    snoConv = tConvArray[0];
    return sConversationStartsImmediately(snoConv, tPlayer);
}












static BOOL sNPCGetGossipConversation(
    ACDID idACDPlayer,
    const ConversationArray & tConvArray,
    SNO & outSnoConv)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);

    for (uint32 i = 0; i < tConvArray.size(); i++)
    {
        // inferred: early continue; DWARF puts pdefConv in the for-body block and its destructor on line 145
        if (tConvArray[i] == SNO_NONE)
            continue;




        ScopedConversationDefinitionLockR pdefConv(tConvArray[i]);
        uint32 dwUsageFlags = ConversationTypeGetUsageFlags(pdefConv->eConvType);
        if (!(dwUsageFlags & (1 << CTU_SHOWN_IN_TALK_MENU_BIT)) ||
            (dwUsageFlags & (1 << CTU_STARTS_IMMEDIATELY_ON_NPC_OPERATE_BIT)) ||
            (dwUsageFlags & (1 << CTU_ALWAYS_PLAY_IMMEDIATELY_BIT)))
            continue;  // inferred: line 140 is at the same depth as line 127 (columns 13/14)









        if (!SPlayerHasHeardConversation(ptPlayer, tConvArray[i]))
        {
            outSnoConv = tConvArray[i];
            return TRUE;
        }
    }

    return FALSE;
}




static BOOL sGetGossipConversation(const Player & tPlayer, ActorCommonData & tACDNPC, const ConversationArray & tConvArray, SNO & snoConv)
{




    if (!tACDNPC.IsAlive())
        return FALSE;
    return sNPCGetGossipConversation(tPlayer.idPlayerACD, tConvArray, snoConv);
}
#include "Actors/ActorEffect.h"
#include "../Common/Definitions/ConversationDefinition.h"
BOOL AnimPlayerShouldTurnToFace(const ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp
BOOL NPCIsOpenWorldModeChange(const ActorCommonData & tACD);  // X/Common/NPC.cpp
OpenWorldMode NPCGetOpenWorldMode(const ActorCommonData & tACD);  // X/Common/NPC.cpp
inline void NPCGetInteractOptions(ActorCommonData & tACDPlayer, ActorCommonData & tACDNPC, NPCInteraction * ptNPCInteractions, uint32 & dwNumInteractions);

static BOOL sHandleImportantConversation(ActorCommonData & tACDNPC, ActorCommonData & tACDPlayer)
{




    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer)
    {
        return FALSE;
    }
    BOOL bTalked = FALSE;




    ConversationArray tConvArray;
    NPCGetConversations(ptPlayer->idPlayerACD, tACDNPC.id, tConvArray);

    SNO snoConv;
    if (sGetImportantConversation(*ptPlayer, tACDNPC, tConvArray, snoConv))
    {




        NPCStartConversation(tACDPlayer, tACDNPC, snoConv, NIT_TALK_IMPORTANT);
        bTalked = TRUE;
    }


    return bTalked;
}
static BOOL sShowConversationIcon(const SNO snoConv, const ActorCommonData & tACDNPC, const Player & tPlayer)
{




    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (!NPCCanOperate(tACDNPC, tACDPlayer, TRUE))
    {
        return FALSE;
    }


    ScopedConversationDefinitionLockR pdefConv(snoConv);
    if (pdefConv->dwFlags & (1 << CF_ALWAYS_IMPORTANT))
    {
        return TRUE;
    }


    if (ConversationMgr::ACDIsInActiveConversation(tACDNPC.id, &tPlayer))
    {
        return FALSE;
    }


    return !SPlayerHasHeardConversation(&tPlayer, snoConv);
}








static ConversationIcon sNPCGetConversationIcon(const ActorCommonData & tACDNPC, const Player & tPlayer, const SNO snoConv)
{



    if (!sShowConversationIcon(snoConv, tACDNPC, tPlayer))
    {
        return 0;
    }

    ScopedConversationDefinitionLockR pdefConv(snoConv);
    return pdefConv->eConvIcon;
}



static void sNPCAddInteractOption(const ActorCommonData & tACDNPC, const NPCInteractionType eType, const uint32 dwData, const uint32 dwData2, const BOOL bEnabled, const BOOL bHeard, NPCInteraction * ptNPCInteractions, uint32 & dwNumInteractions)
{














    ptNPCInteractions[dwNumInteractions].eType = eType;
    ptNPCInteractions[dwNumInteractions].dwData = dwData;
    ptNPCInteractions[dwNumInteractions].dwData2 = dwData2;
    ptNPCInteractions[dwNumInteractions].dwFlags = 0;
    if (bEnabled)
    {

        ptNPCInteractions[dwNumInteractions].dwFlags |= (1 << NPC_OPTION_ENABLED);
    }

    if (bHeard == TRUE)
    {
        ptNPCInteractions[dwNumInteractions].dwFlags |= (1 << NPC_OPTION_CONVERSATION_HEARD);
    }

    dwNumInteractions++;
}



static void sNPCGetInteractOptions(ActorCommonData & tACDPlayer, ActorCommonData & tACDNPC, NPCInteraction * ptNPCInteractions, uint32 & dwNumInteractions, const ConversationArray & tConvArray)
{








    dwNumInteractions = 0;



    for (uint32 i = 0; i < tConvArray.size(); i++)
    {
        SNO snoConv = tConvArray[i];
        if (snoConv == SNO_NONE)
            continue;



        ScopedConversationDefinitionLockR ptConv(snoConv);

        uint32 dwUsageFlags = ConversationTypeGetUsageFlags(ptConv->eConvType);
        if (dwUsageFlags & (1 << CTU_SHOWN_IN_TALK_MENU_BIT))
        {
            sNPCAddInteractOption(tACDNPC, NIT_TALK_DIALOG, snoConv, (uint32)-1, TRUE,
                SPlayerHasHeardConversation(PlayerGetByACD(tACDPlayer.id), snoConv), ptNPCInteractions, dwNumInteractions);
        }
    }












    if (NPCIsCosmeticsVendor(tACDNPC))
    {


        sNPCAddInteractOption(tACDNPC, NIT_CUSTOMIZATION, 1, (uint32)-1, TRUE, FALSE, ptNPCInteractions, dwNumInteractions);
    }









    else if (NPCIsEquipmentManagerVendor(tACDNPC))
    {
        sNPCAddInteractOption(tACDNPC, NIT_EQUIPMENT_MANAGER, (uint32)-1, (uint32)-1, TRUE, FALSE, ptNPCInteractions, dwNumInteractions);
    }
    else if (sub_710051b9d0(tACDNPC))
    {
        sNPCAddInteractOption(tACDNPC, 11, (uint32)-1, (uint32)-1, sub_710051b060() == TRUE, FALSE, ptNPCInteractions, dwNumInteractions);
    }











    CrafterType eCrafterType = NPCGetCrafterType(tACDNPC);
    if (eCrafterType != -1)
    {
        Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
        if (ptPlayer && GetCraftLevel(eCrafterType, *ptPlayer) > 0)
        {
            sNPCAddInteractOption(tACDNPC, NIT_CRAFT, eCrafterType, (uint32)-1, TRUE, FALSE, ptNPCInteractions, dwNumInteractions);
        }
    }




























    if (ACDGetHirelingClass(tACDNPC) != HC_NONE && !tACDNPC.GetPetOwnerPlayer())
    {

        int32 nNumPlayers = PlayerGetNumPlayers();


        if (nNumPlayers == 1 && SHirelingCanHire(*PlayerGetByACD(tACDPlayer.id)))
        {

            sNPCAddInteractOption(tACDNPC, NIT_HIRE, (uint32)-1, (uint32)-1, TRUE, FALSE, ptNPCInteractions, dwNumInteractions);
        }
    }
}
























void NPCUpdateConversationIconAndInteraction(ActorCommonData & tACDNPC, const Player & tPlayer)
{

    BOOL bCanInteract = TRUE;
    ConversationIcon eConvIcon = 0;


    ConversationArray tConvArray;
    NPCGetConversations(tPlayer.idPlayerACD, tACDNPC.id, tConvArray);


    SNO snoConv = SNO_NONE;
    if (sGetImportantConversation(tPlayer, tACDNPC, tConvArray, snoConv))
    {
        eConvIcon = sNPCGetConversationIcon(tACDNPC, tPlayer, snoConv);
    }

    else if (sGetGossipConversation(tPlayer, tACDNPC, tConvArray, snoConv))
    {
        eConvIcon = sNPCGetConversationIcon(tACDNPC, tPlayer, snoConv);
        bCanInteract = FALSE;
    }
    else
        bCanInteract = FALSE;
    BOOL bUpdated = NPCUpdateConversationIconForPlayer(tACDNPC, tPlayer, eConvIcon);

    if (!bCanInteract)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer)
        {
            if (NPCIsOperableVendorOrCrafter(tACDNPC, *ptACDPlayer))
            {
                bCanInteract = TRUE;
            }
            else
            {
                NPCInteraction tNPCInteraction[30];
                uint32 dwNumInteractions = 0;
                sNPCGetInteractOptions(*ptACDPlayer, tACDNPC, tNPCInteraction, dwNumInteractions, tConvArray);

                bCanInteract = dwNumInteractions > 0;
            }
        }
    }




    tACDNPC.AttributesSetInt(MakeAttribKey(ATTRIBUTE_NPC_HAS_INTERACT_OPTIONS, tPlayer.nPlayerIndex), bCanInteract);

    if (bCanInteract || bUpdated)
    {
        STrickleUpdateActor(tACDNPC);
    }
}
#include "PlayerList.h"
#include "../Common/Networking/GameMessage.h"
void NPCVendorStart(Player & tPlayer, ActorCommonData & tACDVendor);
BOOL GameTypeIsPvP();  // X/Common/Game.cpp

void NPCOperateVendor(ActorCommonData & tACD, ActorCommonData & tACDOperator)
{


    Player * ptPlayer = PlayerGetByACD(tACDOperator.id);





    NPCVendorStart(*ptPlayer, tACD);

    PlayerList listDestPlayers(PlayerListGetElementPool());
    if (GameTypeIsPvP())
    {



        for (Player * ptPlayerLoc = PlayerGetFirstAll(); ptPlayerLoc; ptPlayerLoc = PlayerGetNextAll(ptPlayerLoc))
        {
            if (ptPlayerLoc->idACDActiveVendor == tACD.id)
            {
                listDestPlayers.AddTail_Tracked(ptPlayerLoc);
            }
        }
    }
    else
    {

        listDestPlayers.AddTail_Tracked(ptPlayer);
    }


    CrafterType eCrafterType = NPCGetCrafterType(tACD);
    if (eCrafterType != -1)
    {

        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(tACD.id);
        MessageSendToClient(listDestPlayers, 21, &tMessage, sizeof(tMessage));  // SMSG_CRAFTER_OPEN
    }
    else
    {

        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(tACD.id);
        MessageSendToClient(listDestPlayers, 23, &tMessage, sizeof(tMessage));  // SMSG_SHOP_OPEN
    }



    ScriptEventOnNPCVendorOperated(tACD.id, tACDOperator.id);
}

static void sHandleNPCMenu(ActorCommonData & tACDNPC, ActorCommonData & tACDPlayer, const NPCMenu ePreferredMenu, const BOOL fSkipConversations)
{
    NPCInteraction tNPCInteraction[30];
    uint32 dwNumInteractions = 0;
    NPCGetInteractOptions(tACDPlayer, tACDNPC, tNPCInteraction, dwNumInteractions);
    const Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (NPCIsOpenWorldModeChange(tACDNPC) && fSkipConversations)
    {
        OpenWorldMode eOpenWorldMode = NPCGetOpenWorldMode(tACDNPC);
        PlayerList listPlayer(PlayerListGetElementPool());
        PlayerListGetSingle(listPlayer, *ptPlayer);
        Act eAct = (Act)-1;
        Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(tACDNPC.id));
        if (ptActor)
        {
            eAct = (Act)ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_NPC_OPEN_WORLD_MODE_ACT, TAG_MARKER_NPC_OPEN_WORLD_MODE_ACT);
        }
        OpenWorldModeChangeMessage tMessage;
        tMessage.eOpenWorldMode = eOpenWorldMode;
        tMessage.eAct = eAct;
        tMessage.nParam = -1;
        MessageSendToClient(listPlayer, SMSG_OPEN_WORLD_MODE_CHANGE_PROMPT, &tMessage, sizeof(tMessage));
    }
    else if (NPCIsOperableVendorOrCrafter(tACDNPC, tACDPlayer) && (fSkipConversations || dwNumInteractions <= 1))
    {
        NPCOperateVendor(tACDNPC, tACDPlayer);
    }
    else if (NPCIsCosmeticsVendor(tACDNPC) && dwNumInteractions <= 1)
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tACDPlayer.id);
        RequiredMessageHeader tMessage;
        MessageSendToClient(listDestPlayers, SMSG_OPEN_COSMETICS, &tMessage, sizeof(tMessage));
    }
    else if (NPCIsEquipmentManagerVendor(tACDNPC) && dwNumInteractions <= 1)
    {
        STalkSetTalkingTo(tACDPlayer, tACDNPC);
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tACDPlayer.id);
        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(tACDNPC.id);
        MessageSendToClient(listDestPlayers, SMSG_EQUIPMENT_MANAGER_OPEN, &tMessage, sizeof(tMessage));
    }
    else if (!fSkipConversations && dwNumInteractions > 0)
    {
        NPCInteractOptionsMessage tMessage;
        tMessage.annTalkingTo = ANNFromACD(tACDNPC.id);
        tMessage.ePreferredMenu = ePreferredMenu;
        tMessage.nNumInteractions = dwNumInteractions;
        for (uint32 dwIndex = 0; dwIndex < dwNumInteractions; dwIndex++)
        {
            tMessage.tNPCInteraction[dwIndex] = tNPCInteraction[dwIndex];
        }
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tACDPlayer.id);
        MessageSendToClient(listDestPlayers, SMSG_NPC_INTERACT, &tMessage, sizeof(tMessage));
        STalkSetTalkingTo(tACDPlayer, tACDNPC);
    }
}






































void NPCOperate(ActorCommonData & tACDNPC, ActorCommonData & tACDPlayer, const NPCMenu ePreferredMenu, const BOOL fFromPower, const BOOL fSkipImportantConversations, const BOOL fForceOperateVendor)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer)
        return;
    if (!NPCCanOperate(tACDNPC, tACDPlayer, fFromPower))
        return;
    {
        WorldLocation & tLocPlayer = tACDPlayer.GetWorldLocation();
        WorldLocation & tLocNPC = tACDNPC.GetWorldLocation();
        float flDistSqr = Vector3D::DistanceSquared(tLocPlayer.transform.wp, tLocNPC.transform.wp);
        if (flDistSqr >= MAX_CONVERSATION_DISTANCE * MAX_CONVERSATION_DISTANCE)
            return;
    }
    if (!tACDNPC.IsMonster())
        return;
    if (ConversationMgr::PlayerIsInQuestConversation(*ptPlayer))
        return;
    BOOL bTurnToFace = TRUE;
    BOOL bSkipAllConversations = fForceOperateVendor;
    if (ConversationMgr::ACDIsUnusableDuringConversation(tACDNPC.id, ptPlayer))
    {
        bSkipAllConversations = TRUE;
        bTurnToFace = NPCIsOperableVendorOrCrafter(tACDNPC, tACDPlayer);
    }
    if (bTurnToFace && AnimPlayerShouldTurnToFace(tACDNPC))
    {
        tACDNPC.SetFacing(tACDPlayer.GetPlace(), FALSE);
    }
    if (ptPlayer->idACDActiveVendor != tACDNPC.id)
    {
        ACDPlayEffectForPlayer(tACDPlayer.id, tACDNPC.id, 0x25, 0);
    }
    if (STalkIsTalking(tACDPlayer.id))
    {
        STalkResetTalkingTo(*ptPlayer);
    }
    if (!bSkipAllConversations && !fSkipImportantConversations)
    {
        if (sHandleImportantConversation(tACDNPC, tACDPlayer))
            return;
    }
    sHandleNPCMenu(tACDNPC, tACDPlayer, ePreferredMenu, bSkipAllConversations);
}





















































void NPCHandleDismissal(ActorCommonData & tACDHireling, BOOL bPlayerDismissed, BOOL bDisplayChatMessage)
{



    DeleteLastHeadstone(tACDHireling);

    ACDID idACDOwner = ACDGetPetOwner(tACDHireling.id);
    if (idACDOwner != ACDID_INVALID)
    {
        ACDIDParam tParam1(tACDHireling.id);
        ACDIDParam tParam2(idACDOwner);
        ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
        ScriptEventDispatchEvent(EVENT_HIRELING_DISMISSED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);

        SHirelingMoveToStable(idACDOwner, tACDHireling, bPlayerDismissed);
        PlayerRemovePet(idACDOwner, tACDHireling.id, bDisplayChatMessage);
    }


    const ActorID idActorHireling = (ActorID)tACDHireling.idOwner;
    ActorFlagForDeletion(idActorHireling);
}




void NPCDismiss(ActorCommonData & tACDPlayer, BOOL bPlayerDismissed, BOOL bDisplayChatMessage)
{

    ACDID idACDHireling = PlayerGetHireling(tACDPlayer.id);
    if (idACDHireling != ACDID_INVALID)
    {
        ActorCommonData & tACDHireling = ACDGet(idACDHireling);
        NPCHandleDismissal(tACDHireling, bPlayerDismissed, bDisplayChatMessage);
    }
}









void NPCDismissProxy(ActorCommonData & tACDPlayer, HirelingClass eClass)
{

    ACDID idACDHirelingProxy = PlayerGetHirelingProxy(tACDPlayer.id, eClass);
    if (idACDHirelingProxy != ACDID_INVALID)
    {
        ActorCommonData & tACDHirelingProxy = ACDGet(idACDHirelingProxy);
        DismissPet(tACDPlayer, tACDHirelingProxy, FALSE);
    }
}









void DismissPet(ActorCommonData & tACDPlayer, ActorCommonData & tACDPet, BOOL bPlayerDismissed)
{




    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer == NULL)
        return;



    ptPlayer = tACDPet.GetPetOwnerPlayer();
    if (ptPlayer == NULL)
        return;





    for (int32 iPet = 0; iPet < 64; iPet++)
    {

        if (PlayerGetPet(tACDPlayer.id, iPet) == tACDPet.id)
        {

            ActorID idPet = (ActorID)ACDGetOwner(tACDPet.id);



            if (ACDIsFollower(tACDPet) || ACDIsFollowerProxy(tACDPet))
                return;






            if (ACDIsHirelingProxy(tACDPet))
            {


                NPCHandleDismissal(tACDPet, bPlayerDismissed, TRUE);
                return;
            }



            if (ACDIsHireling(tACDPet))
            {
                Target tTarget;

                tTarget.InitializeFromACD(tACDPet.id, FALSE, FALSE);
                PowerStart(tACDPet.id, GlobalSNOGet(GLOBALSNO_POWERS_HIRELING_DISMISS), tTarget);


                SHirelingSetActive(tACDPlayer.id, FALSE, TRUE);
                return;
            }



            PowerAPI_BuffFlagAllForDeletion(tACDPet);


            PowerAPI_BuffApplyOnPetDismissEffect(tACDPlayer, tACDPlayer.id, tACDPet.id);
            PowerAPI_BuffApplyOnPetDismissEffect(tACDPet, tACDPlayer.id, tACDPet.id);


            BrainAPIDoDeath(idPet, (ActorID)ACDGetOwner(tACDPlayer.id), TRUE, TRUE, TRUE, NULL, NULL);
            return;
        }
    }
}
void NPCHelperDetach(ActorCommonData & tACDPlayer, ActorCommonData & tACDHelper)
{


    Player * ptPlayer = tACDHelper.GetPetOwnerPlayer();
    if (ptPlayer == NULL)
        return;


    PlayerRemovePet(*ptPlayer, tACDHelper.id, TRUE);

    AIStateSetStateData((ActorID)ACDGetOwner(tACDHelper.id), 0);
    tACDHelper.AttributesSetInt(ATTRIBUTE_GUARD_OBJECT_ACDID, ACDID_INVALID);

    ScriptEventOnHelperDismissed(tACDHelper.id, tACDPlayer.id);

}




static void sHelperProcessLevelAreaChange(ActorCommonData & tACDHelper, ActorCommonData & tACDPlayer, SNO snoLevelAreaNew)
{

    if (!HelperIsLevelAreaAllowed(tACDHelper, snoLevelAreaNew))
    {


        BOOL bSuccess = AIStateSetStateData((ActorID)ACDGetOwner(tACDHelper.id), AIBEHAVIOR_TYPE_STALLED);
        if (bSuccess)
        {
            Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
            ConversationMgr::StartConversation(ptPlayer->nPlayerIndex, GlobalSNOGet(GLOBALSNO_CONVERSATION_HELPER_CANNOT_FOLLOW), tACDHelper.id, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, 0x7fffffff, FALSE, FALSE);
        }
    }
}



















void HelperProcessPlayerLevelAreaChange(ActorCommonData & tACDPlayer, const LevelAreaID idLevelAreaNew)
{

    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    SNO snoLevelAreaNew = LevelAreaGetSNO(idLevelAreaNew);

    for (int32 i = 0; i <= ptPlayer->nMaxUsedPetIndex; i++)
    {
        ACDID idACDPet = ptPlayer->tPets[i].idACD;
        if (idACDPet != ACDID_INVALID && ptPlayer->tPets[i].eType == PETTYPE_HELPER)
        {
            ActorCommonData & tACDPet = ACDGet(idACDPet);
            sHelperProcessLevelAreaChange(tACDPet, tACDPlayer, snoLevelAreaNew);
        }
    }
}










static SNO sHelperGetAllowedLevelArea(const ActorCommonData & tACDHelper, int nIndex)
{
    uint32 uTag = TAG_ACTOR_HELPER_LEVEL_AREA_0 + nIndex * (TAG_ACTOR_HELPER_LEVEL_AREA_1 - TAG_ACTOR_HELPER_LEVEL_AREA_0);






















    ScopedActorDefinitionLockR pdefActor(tACDHelper.snoActor);
    SNO snoLevelArea = TagMapGetSNO(pdefActor->hTagMap, uTag);
    return snoLevelArea;
}




BOOL HelperIsLevelAreaAllowed(const ActorCommonData & tACDHelper, SNO snoLevelArea)
{

    BOOL bAllowed = FALSE;
    int nValidCount = 0;
    for (int i = 0; i < 4; i++)
    {

        SNO snoAllowedLevelArea = sHelperGetAllowedLevelArea(tACDHelper, i);
        if (snoAllowedLevelArea != SNO_NONE)
        {
            nValidCount++;

            if (snoAllowedLevelArea == snoLevelArea)
            {
                bAllowed = TRUE;
                break;
            }
        }
    }

    if (nValidCount == 0)
        bAllowed = TRUE;
    return bAllowed;
}







void NPCSetTalkTarget(ActorCommonData & tACDNPC, ActorCommonData * ptACDTarget)
{


    ActorID idActorNPC = (ActorID)ACDGetOwner(tACDNPC.id);


    ACDNetworkName idANNTarget = ANN_INVALID;
    if (ptACDTarget)
    {
        idANNTarget = ANNFromACD(ptACDTarget->id);
    }

    tACDNPC.AttributesSetInt(ATTRIBUTE_NPC_TALK_TARGET_ANN, idANNTarget);
    BrainAPIDoIdle(idActorNPC);
}


void NPCStartConversation(ActorCommonData & tACDPlayer, ActorCommonData & tACDTalkingTo, const SNO snoConversation, const NPCInteractionType)
{

    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer)
    {
        const ACDID idACDParticipants[9] = { tACDTalkingTo.id, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID };
        ConversationMgr::StartConversation(ptPlayer->nPlayerIndex, snoConversation, idACDParticipants[0], idACDParticipants[1], idACDParticipants[2], idACDParticipants[3], idACDParticipants[4], 0, FALSE, TRUE);
    }
}















































void NPCVendorStart(Player & tPlayer, ActorCommonData & tACDVendor)
{



    uint32 dwNow = GameGetServerTime();
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    if (NPCCanTrade(tACDVendor))
    {
        if (GameIsParamFlagSet(GPF_RESTOCK_SHOPS_UPON_TALK))
        {


            SNO snoShopInventory = ACDGetInventory(&tACDVendor, FALSE);
            SACDInventoryReload(tACDVendor.id, snoShopInventory);
        }

        tACDVendor.AttributesSetInt(ATTRIBUTE_LAST_TICK_SHOP_ENTERED, dwNow);
    }



    tPlayer.idACDActiveVendor = tACDVendor.id;
    STalkSetTalkingTo(tACDPlayer, tACDVendor);



    tPlayer.nBusyMessagesSinceNPCInteract = 0;

    CrafterType eCrafterType = NPCGetCrafterType(tACDVendor);
    if (eCrafterType != -1)
    {
        tPlayer.eActiveCrafterType = eCrafterType;
    }
    tPlayer.tVendorData.VendorReset();
}




void NPCVendorStop(Player & tPlayer)
{
    tPlayer.idACDActiveVendor = ACDID_INVALID;
    STalkResetTalkingTo(tPlayer);
    tPlayer.tVendorData.VendorReset();
}






inline void NPCGetInteractOptions(ActorCommonData & tACDPlayer, ActorCommonData & tACDNPC, NPCInteraction * ptNPCInteractions, uint32 & dwNumInteractions)
{
    ConversationArray tConvArray;
    NPCGetConversations(tACDPlayer.id, tACDNPC.id, tConvArray);
    sNPCGetInteractOptions(tACDPlayer, tACDNPC, ptNPCInteractions, dwNumInteractions, tConvArray);
}




#include "../Common/Definitions/ConversationListDefinition.h"



SNO NPCGetConversationsFromList(SNO snoConvList, ACDID idACDPlayer, ACDID idACDNPC, ConversationArray * ptConvArray)
{
    ActorCommonData * ptACDNPC = ACDTryToGet(idACDNPC);
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);

    if (!SNOFileExists(CONVERSATIONLIST_SNO, snoConvList))
    {
        return SNO_NONE;
    }

    ScopedConversationListDefinitionLockR ptConvListDef(snoConvList);

    int nCount = ptConvListDef->serConvList.dwSize / sizeof(ConversationListEntry);
    for (int32 i = 0; i < nCount; i++)
    {

        ConversationListEntry & tEntry = ptConvListDef->ptConvList[i];
        if (tEntry.dwFlags & (1 << CLE_DIVIDER_BIT))
        {

            if (ptConvArray && ptConvArray->size() > 0)
            {
                break;
            }
            continue;
        }

        if (tEntry.snoConv == SNO_NONE)
        {
            continue;
        }

        if (ConversationListEntryEvaluate(tEntry, *ptPlayer, ptACDNPC))
        {
            if (ptConvArray)
            {

                ptConvArray->push_back(tEntry.snoConv);
            }
            else
            {

                return tEntry.snoConv;
            }
        }
    }

    return SNO_NONE;
}






BOOL NPCGetConversations(ACDID idACDPlayer, ACDID idACDNPC, ConversationArray & tConvArray)
{

    tConvArray.clear();

    ActorCommonData & tACDNPC = ACDGet(idACDNPC);
    if (CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
    {

        if (tACDNPC.snoActor == SNOFileLookupGUID(1, "PVP_Dueling_NPC"))
            return FALSE;
    }





    Actor * ptActor = ActorTryToGet((ActorID)tACDNPC.idOwner);
    if (ptActor && ptActor->snoConvList != SNO_NONE)
    {
        NPCGetConversationsFromList(ptActor->snoConvList, idACDPlayer, idACDNPC, &tConvArray);
    }

    ScopedActorDefinitionLockR pdefActor(tACDNPC.snoActor);
    SNO snoConvList = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_NPC_BACKSTORY_CONVERSATION_LIST);
    if (snoConvList != SNO_NONE)
    {
        NPCGetConversationsFromList(snoConvList, idACDPlayer, idACDNPC, &tConvArray);
    }

    return tConvArray.size() != 0;
}



















#include "../Common/Definitions/MarkerSetDefinition.h"
#include "../Common/Hireling.h"
#include "../Common/HirelingSwig.h"
#include "../Common/GizmoSwig.h"
BOOL QuestRangeEvaluate(const SNO snoQuestRange, QuestRangeResult * pResult);  // X/Common/Definitions/QuestRangeDefinition.cpp:280
int32 GetCraftLevel(const CrafterType eType, const Player & tPlayer);  // X/Common/Crafting/ItemCrafting.h
uint32 CrafterGetRank(const CrafterType eType, const uint32 nLevel);  // X/Common/Crafting/ItemCrafting.h





// LINKER-REMOVED: DWARF SNPC.cpp:1293 (low_pc 0, 0x78 bytes); body from line rows 1297/1299/1301/1305; its out-of-line call keeps sNPCGetGossipConversation out of line; the FALSE return is inferred
BOOL NPCGetGossipConversation(
    ACDID idACDPlayer,
    ACDID idACDNPC,
    SNO & outSnoConv)
{
    ConversationArray tConvArray;
    if (NPCGetConversations(idACDPlayer, idACDNPC, tConvArray))
    {
        return sNPCGetGossipConversation(idACDPlayer, tConvArray, outSnoConv);
    }
    // inferred: no line-table row; the FALSE return merges into the shared epilogue (DWARF return type BOOL)
    return FALSE;
}

// NON_MATCHING: 1 instruction; the current-hireling class test is folded into the crafter cset (b instead of b.ne to return FALSE)
BOOL NPCShouldBeHiddenForClient(const ActorCommonData & tACD, const Player * ptPlayer)
{


    if (!ptPlayer)
        return FALSE;




    if (!tACD.IsEnabled())
        return TRUE;



    if (tACD.IsGizmo() && tACD.GetGizmoType() == ACTORGIZMO_HEARTH_PORTAL)
    {
        if (!ptPlayer->tHearthPortalInfo.IsValid())
            return TRUE;
    }



    HirelingClass eHirelingClass = ACDGetHirelingClass(tACD);
    if (eHirelingClass == HC_NONE && !tACD.IsMerchant())
        return FALSE;



    BOOL bHideUntilUnlocked = FALSE;
    SNO snoQuestRangeUnlock = SNO_NONE;

    Actor * ptActor = ActorTryToGet((ActorID)tACD.idOwner);
    if (ptActor && ptActor->tMarkerOwner.snoMarkerSet != SNO_NONE)
    {

        ScopedMarkerSetDefinitionLockR pdefMarkerSet(ptActor->tMarkerOwner.snoMarkerSet);
        bHideUntilUnlocked = TagMapGetInt(pdefMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex].hTagMap, TAG_MARKER_HIDDEN_UNTIL_UNLOCKED);



        if (bHideUntilUnlocked && !GameIsOpenWorld())
        {
            snoQuestRangeUnlock = TagMapGetSNO(pdefMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex].hTagMap, TAG_MARKER_UNLOCK_QUESTRANGE);
        }
    }

    if (eHirelingClass != HC_NONE)
    {

        Player * ptOwner = tACD.GetPetOwnerPlayer();
        if (ptOwner)
            return FALSE;
        if (bHideUntilUnlocked)
        {

            if (snoQuestRangeUnlock == SNO_NONE || !QuestRangeEvaluate(snoQuestRangeUnlock, NULL))
            {

                if (!HirelingTestTypeAvailable(ptPlayer->idPlayerACD, eHirelingClass))
                    return TRUE;
            }
        }
        {



            ACDID idACDCurrentHireling = PlayerGetHireling(ptPlayer->idPlayerACD);
            ActorCommonData * ptACDCurrentHireling = ACDTryToGet(idACDCurrentHireling);
            if (ptACDCurrentHireling)
            {
                HirelingClass eClass = HirelingGetClass(*ptACDCurrentHireling); if (eClass == eHirelingClass) return TRUE;
            }
        }
    }
    else if (bHideUntilUnlocked)
    {




        CrafterType eType = NPCGetCrafterType(tACD);
        if (eType != -1)
        {
            if (snoQuestRangeUnlock == SNO_NONE || !QuestRangeEvaluate(snoQuestRangeUnlock, NULL))
            {


                int32 nCrafterLevel = GetCraftLevel(eType, *ptPlayer);
                return CrafterGetRank(eType, nCrafterLevel) == 0;
            }
        }
    }



    return FALSE;
}
