// X/Client/Crafting/CItemCrafting.cpp
//
// Byte-matching decompilation of Diablo III 2.6.2 from the original debug
// evidence (DWARF + line-attributed listing). Source order and names come from
// the DWARF skeleton build/dwarf/headers/X/Client/Crafting/CItemCrafting.cpp:
//   line  29  _ZN12_GLOBAL__N_122sClientResetDurabilityE5ACDIDiPv  [0x71003da8d0, 196 bytes]
//   line  45  _Z26CItemCrafting_TryRepairAllP6Playeri              [0x71003da680, 584 bytes]
//   line  81  _Z23CItemCrafting_TryRepair5ACDIDP6Player            [0x71003da9a0,  92 bytes]
//
// Declarations whose DWARF-owning header is not adopted in src/ are carried in
// this translation unit and cite the .cpp that defines them.

#include "../../Common/Game.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Player.h"
#include "../Sound/Sound.h"

// X/Client/Network/CGameMessage.cpp:45 (owning header X/Client/Network/CGameMessage.h
// is not adopted in src/).  _Z19MessageSendToServeriPvjPK6Player
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize,
                         const Player * ptPlayer);

// X/Client/CActorCommonData.cpp:58 (owning header X/Client/CActorCommonData.h is not
// adopted in src/).  _Z21CACDAttributeSetValueR15ActorCommonData13FastAttribKey15FastAttribValuei
void CACDAttributeSetValue(ActorCommonData & tACD, FastAttribKey tKey, FastAttribValue tValue,
                           const BOOL fBypassRActorChanges);

// X/Common/ActorInventory.cpp:2646 (the adopted ActorInventory.h does not carry it).
// _Z24ACDInventoryIterateItemsRK15ActorCommonDataijPFv5ACDIDiPvES3_
void ACDInventoryIterateItems(const ActorCommonData & tACD, InventorySlotType eSlot,
                              uint32 dwACDIIFlags,
                              void (*pfnCallback)(ACDID, InventorySlotType, void *),
                              void * pCallbackUserData);

// X/Common/ActorCommonData.cpp:2159 (inlined into this file's callers).
// _Z10ANNFromACD5ACDID
ACDNetworkName ANNFromACD(const ACDID idACD);

// X/Common/ActorInventory.cpp:104 -- const InventorySlotType cgtDefaultPlayerInventorySlots[15].
// Original data at 0x7100e4813c: int32 0,1,2,3,...,13 then -1 (60 bytes).
extern const InventorySlotType cgtDefaultPlayerInventorySlots[15];

// X/Client/Sound/Sound.cpp:50 -- const PlaySoundOptions cgtPSODefaults.
// Original data at 0x7100e43170 (128 bytes: zeros through +0x0c, -1 at +0x10 ...).
extern const PlaySoundOptions cgtPSODefaults;

namespace {

// 0x71003da8d0 -- resets one item's current durability to its maximum.
void sClientResetDurability(ACDID idACDSquare, InventorySlotType, void *) {
    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem == NULL) {
        return;
    }

    CACDAttributeSetValue(*ptACDItem,  // line 37
                          FastAttribKey(ATTRIBUTE_DURABILITY_CUR),
                          FastAttribValue(  // line 39
                              ptACDItem->AttributesGetInt(FastAttribKey(ATTRIBUTE_DURABILITY_MAX))),
                          0);
}

}  // namespace

// 0x71003da680
void CItemCrafting_TryRepairAll(Player * ptPlayer, const BOOL fEquippedOnly) {
    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

    if (fEquippedOnly) {
        for (int32 i = 1; i <= 13; ++i) {
            ACDInventoryIterateItems(tACDPlayer, i, 0, sClientResetDurability, NULL);
        }

        SimpleMessage tMessage;
        MessageSendToServer(CMSG_TRY_REPAIR_EQUIPPED, &tMessage, sizeof(tMessage), ptPlayer);
    } else {
        int32 nIndex = 0;
        for (; cgtDefaultPlayerInventorySlots[nIndex] != -1; ++nIndex) {
            ACDInventoryIterateItems(tACDPlayer, cgtDefaultPlayerInventorySlots[nIndex], 0,
                                     sClientResetDurability, NULL);
        }

        SimpleMessage tMessage;
        MessageSendToServer(CMSG_TRY_REPAIR_ALL, &tMessage, sizeof(tMessage), ptPlayer);
    }

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_REPAIR), cgtPSODefaults);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_REPAIR_ALL), cgtPSODefaults);
}

// 0x71003da9a0
void CItemCrafting_TryRepair(ACDID idACDItem, Player * ptPlayer) {
    ANNDataMessage tMessage;

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_REPAIR), cgtPSODefaults);

    tMessage.ann = ANNFromACD(idACDItem);

    MessageSendToServer(CMSG_TRY_REPAIR, &tMessage, sizeof(tMessage), ptPlayer);
}
