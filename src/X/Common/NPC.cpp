// Original file: X/Common/NPC.cpp

#include "NPC.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../Sigma/RefString.h"
#include "ActorCommonData.h"
#include "Attributes/FastAttribute.h"
#include "Attributes/FastAttribConstantsSwig.h"
#include "Crafting/ItemCrafting.h"
#include "Crafting/ItemCraftingSwig.h"
#include "Definitions/ActorDefinition.h"
#include "Definitions/StringListDefinition.h"
#include "Game.h"
#include "GlobalSNO.h"
#include "HirelingSwig.h"
#include "Monsters.h"
#include "Player.h"
#include "Shell/CmdLine.h"
#include "TagMaps/ActorTags.h"
#include <stddef.h>

// Declared here because its header is not adopted yet.
const CmdLineParams * CmdLineGetParams();  // X/Common/Shell/CmdLine.cpp
CrafterType NPCGetCrafterType(const ActorCommonData & tACDVendor);


















static const TypeDescriptorField NPCInteraction_Fields[5] = {
    BoundedTypeDescriptorField("", DT_INT, offsetof(NPCInteraction, eType), NULL, (const void *)NIT_INVALID, (const void *)11 /* 2.7.7 NPC interaction upper bound */),
    TypeDescriptorField("", DT_INT, offsetof(NPCInteraction, dwData), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(NPCInteraction, dwData2), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NPCInteraction, dwFlags), NULL, (const void *)0, (const void *)((1 << NPC_OPTION_COUNT) - 1)), TypeDescriptorField(NULL, DT_NULL, sizeof(NPCInteraction), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_NPCInteraction("NPCInteraction", 0, NPCInteraction_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_NPC_INTERACTION = &TypeDescriptor_NPCInteraction;

static const TypeDescriptorField NPCInteractOptionsMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(NPCInteractOptionsMessage, tHeader), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(NPCInteractOptionsMessage, annTalkingTo), NULL, 1),
    FixedArrayTypeDescriptorField("tNPCInteraction", DT_NPC_INTERACTION, offsetof(NPCInteractOptionsMessage, tNPCInteraction), NULL, 8, 30, (int32)offsetof(NPCInteractOptionsMessage, nNumInteractions) - (int32)offsetof(NPCInteractOptionsMessage, tNPCInteraction)),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NPCInteractOptionsMessage, ePreferredMenu), NULL, (const void *)NM_DEFAULT, (const void *)NM_COUNT), TypeDescriptorField(NULL, DT_NULL, sizeof(NPCInteractOptionsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_NPCInteractOptionsMessage("NPCInteractOptionsMessage", 0, NPCInteractOptionsMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_NPC_INTERACT_OPTIONS_MESSAGE = &TypeDescriptor_NPCInteractOptionsMessage;












BOOL NPCCanOperate(const ActorCommonData & tACDNPC,
                   const ActorCommonData & tACDOperator,
                   BOOL bFromDirectClick)
{
    if (!ACDIsFriendly(tACDNPC, tACDOperator, FALSE))
    {
        return FALSE;
    }

    if (!tACDNPC.IsNPC()) return TRUE;

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {

        CrafterType eCrafterType = NPCGetCrafterType(tACDNPC);
        if (eCrafterType != CRAFTER_TYPE_INVALID ||

            ACDGetHirelingClass(tACDNPC) != HC_NONE ||

            tACDNPC.IsMerchant())
        {
            return FALSE;
        }
    }



    if (!tACDNPC.IsAlive())
    {
        return FALSE;
    }

    if (!tACDNPC.AttributesGetInt(FastAttribKey(ATTRIBUTE_NPC_IS_OPERATABLE)))
    {
        return FALSE;
    }


    Player * ptNPCOwnerPlayer = tACDNPC.GetPetOwnerPlayer();
    if (!ptNPCOwnerPlayer && tACDNPC.AttributesGetInt(FastAttribKey(ATTRIBUTE_NPC_IS_ESCORTING)))
        return FALSE;
    if (ptNPCOwnerPlayer && bFromDirectClick)
    {
        return FALSE;
    }



    if (ptNPCOwnerPlayer && ptNPCOwnerPlayer->idPlayerACD != tACDOperator.id)
    {
        return FALSE;
    }


    return TRUE;

}




BOOL NPCIsOperableVendorOrCrafter(const ActorCommonData & tACDNpc,
                                  const ActorCommonData & tACDPlayer)
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return FALSE;
    }


    if (tACDNpc.AttributesGetInt(FastAttribKey(ATTRIBUTE_NPC_DISABLE_OPERATION)))
    {
        return FALSE;
    }

    if (!NPCCanTrade(tACDNpc))
    {
        return FALSE;
    }

    CrafterType eCrafterType = NPCGetCrafterType(tACDNpc);
    if (eCrafterType == CRAFTER_TYPE_INVALID || eCrafterType == CRAFTER_TYPE_JEWEL_UPGRADE)
        return TRUE;

    const Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    return ptPlayer && GetCraftLevel(eCrafterType, *ptPlayer) > 0;
}










BOOL NPCIsOpenWorldModeChange(const ActorCommonData & tACDNPC)
{

    if (tACDNPC.AttributesGetInt(FastAttribKey(ATTRIBUTE_NPC_DISABLE_OPERATION)))
    {
        return FALSE;
    }

    return ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_OPEN_WORLD_MODE, tACDNPC.snoActor) != -1;
}








BOOL NPCCanTrade(const ActorCommonData & tACD)
{

    return ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_TRADE, tACD.snoActor);
}











BOOL NPCIsCosmeticsVendor(const ActorCommonData & tACD)
{

    return ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_IS_COSMETICS_NPC, tACD.snoActor);
}



BOOL NPCIsEquipmentManagerVendor(const ActorCommonData & tACD)
{

    return tACD.snoActor == GlobalSNOGet(GLOBALSNO_ACTOR_EQUIPMENT_MANAGER_NPC);
}



extern "C" BOOL sub_710051b9d0(const ActorCommonData & tACD)
{
    return ActorDefinitionGetTagMapValueInt(0x109a3, tACD.snoActor);
}

BOOL NPCCanIdentify(const ActorCommonData & tACD)
{

    return ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_CAN_IDENTIFY, tACD.snoActor);
}


















ACDID NPCIsAnyPlayerTalkingTo(ACDID idACDTalkingTo)
{

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->tTalkData.idACDTalkingTo == idACDTalkingTo)
        {
            return ptPlayer->idPlayerACD;
        }
    }

    return ACDID_INVALID;
}



BOOL NPCUpdateConversationIconForPlayer(ActorCommonData & tACDWithInfo,
                                        const Player & tPlayer,
                                        const ConversationIcon eConvIcon)
{
    FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_CONVERSATION_ICON, tPlayer.nPlayerIndex);
    ConversationIcon ePrev = tACDWithInfo.AttributesGetInt(tKey);
    if (ePrev != eConvIcon)
    {
        tACDWithInfo.AttributesSetInt(tKey, eConvIcon);
        return TRUE;
    }

    return FALSE;
}





BOOL ActorCommonData::IsOperatableNPC(const Player * ptPlayer,
                                      BOOL bFromDirectClick) const
{



    if (!IsNPC())
    {
        return FALSE;
    }



    if (GameActiveGameCommonDataIsClient())
    {
        const ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        if (!NPCCanOperate(*this, tACDPlayer, bFromDirectClick))
        {
            return FALSE;
        }
    }


    if (IsHelper())
    {
        return TRUE;
    }



    return AttributesGetInt(MakeAttribKey(ATTRIBUTE_NPC_HAS_INTERACT_OPTIONS, ptPlayer->nPlayerIndex)) != 0;
}










BOOL NPCGetUIName(const ActorCommonData & tACD,
                  CRefString & usText)
{
    BOOL bGotName = FALSE;
    if (tACD.tSNOName.snoGroup == ACTOR_SNO)
    {
        usText = MonsterGetName(&tACD, FALSE); bGotName = TRUE;

        CrafterType eCrafterType = NPCGetCrafterType(tACD);
        if (eCrafterType == CRAFTER_TYPE_INVALID)
        {
            if (NPCCanTrade(tACD))
            {
                usText += "\n";
                usText += StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "VendorSubtitle");
            }
        }
        else
        {
            usText += "\n";
            usText += StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "ArtisanSubtitle");
        }
    }

    return bGotName;
}






CrafterType NPCGetCrafterType(const ActorCommonData & tACDVendor)
{

    int32 nCrafterType = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_CRAFTER_TYPE, tACDVendor.snoActor);


    if (nCrafterType < 0 || nCrafterType >= CRAFTER_TYPE_COUNT)
    {
        return CRAFTER_TYPE_INVALID;
    }
    return nCrafterType;
}


OpenWorldMode NPCGetOpenWorldMode(const ActorCommonData & tACDNPC)
{

    return ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_OPEN_WORLD_MODE, tACDNPC.snoActor);
}
