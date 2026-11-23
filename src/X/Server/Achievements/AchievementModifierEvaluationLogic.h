// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Achievements/AchievementModifierEvaluationLogic.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Player.h"
#include "../../Common/Definitions/TreasureClassDefinition.h"

class ActorCommonData;
namespace OnlineService { class AchievementsInterface; }
struct Player;
struct Power;
struct PowerAPIBuffInstance;
struct PowerAPIDamage;
struct PowerStartData;

typedef int32 DyeType;

typedef int32 ItemLabel;

namespace Achievement {
typedef int32 EventID;

struct EquipmentInfo {  // size 0x44
    EquipmentInfo();
    bool AreAllDyeableSlotsEquipped() const;
    bool IsFullyEquipped() const;
    bool IsAllEquipmentDyed() const;
    bool IsAllEquipmentDyedOneColor() const;
    bool IsAllEquipmentDyedColor(DyeType eDyeType) const;
    bool IsAllEquipmentDyedDifferently() const;
    bool IsAllEquipmentEnchanted() const;
    bool IsAllEquipmentTransmogrifiedToLegendary() const;
    bool IsAllEquipmentMaxLevelLegendary() const;
    bool IsAllHirelingEquipmentMaxLevelLegendary() const;
    bool HasItemLabelEquipped(ItemLabel eItemLabel) const;
    bool HasNoSetItems() const;
    ACDID tACDIDEquipmentOwner;  // 0x00
    int32 nEquipmentCount;  // 0x04
    int32 nMinRequiredLevel;  // 0x08
    int32 nNumSocketedItems;  // 0x0c
    int32 nNumDyedItems;  // 0x10
    int32 nNumEnchantedItems;  // 0x14
    int32 nNumLegendaryTransmogrifiedItems;  // 0x18
    int32 nNumMaxLevelLegendaryItems;  // 0x1c
    int32 nNumSetItems;  // 0x20
    bool bTwoHandedWeaponEquipped;  // 0x24
    bool IsDyeUsed(DyeType eDye) const;
    int32 GetDyeUsedCount() const;
    void SetDyeUsed(DyeType eDye);
    void ClearDyeUsed();
    void SetItemLabelEquipped(ItemLabel eItemLabel);
    void ClearItemLabelEquipped();
private:
    int32 setUsedDyes[1];  // 0x28
    int32 nUsedDyesCount;  // 0x2c
    int32 setItemLabels[5];  // 0x30
};
static_assert(sizeof(EquipmentInfo) == 0x44, "EquipmentInfo");

class EvaluationData {  // size 0x58
public:
    EvaluationData();
    ~EvaluationData();
    void EvaluateEvent(const Player * ptPlayer, Achievement::EventID eEventID, uint32 qty);
    void EvaluateEvent(const Player * ptPlayer, Achievement::EventID eEventID, uint32 operand, uint32 qty);
    const Player * GetPlayer() const;
    void SetPlayer(const Player * ptPlayer);
    const ActorCommonData * GetACD() const;
    void SetACD(const ActorCommonData * ptACD);
    const Power * GetPower() const;
    const SNO GetPowerSNO() const;
    const PowerAPIDamage * GetPowerDamage() const;
    const PowerAPIBuffInstance * GetBuff() const;
    void SetPower(const Power * ptPower);
    void SetPower(const PowerStartData * ptPowerStartData);
    void SetPower(const PowerAPIDamage * ptPowerDamage);
    void SetPower(SNO snoPower);
    void SetBuff(const PowerAPIBuffInstance * ptBuff);
    const Achievement::EquipmentInfo * GetEquipmentInfo();
    void SetEquipmentInfo(ACDID idACDOwner);
    const CrafterType GetCrafterType() const;
    void SetCrafterType(CrafterType eCrafterType);
    void SetCursedChestKillCount(const int32 nKillCount);
    int32 GetCursedChestKillCount() const;
    bool Evaluate(uint64 modifierID, uint64 operand);
private:
    bool EvaluateGameStateModifiers(uint64 modifierId, uint64 operand) const;
    bool EvaluatePlayerModifiers(uint64 modifierId, uint64 operand) const;
    bool EvaluateMonsterModifiers(uint64 modifierId, uint64 operand) const;
    bool EvaluateEquipmentModifiers(uint64 modifierId, uint64 operand);
    bool EvaluatePowerModifiers(uint64 modifierId, uint64 operand) const;
    bool EvaluatePetModifiers(uint64 modifierId, uint64 operand) const;
    bool EvaluateCrafterTypeModifiers(uint64 modifierId, uint64 operand) const;
    bool EvaluateBuffModifiers(uint64 modifierId, uint64 operand) const;
    bool EvaluateItemModifiers(uint64 modifierId, uint64 operand) const;
    bool EvaluateMiscellaneousModifiers(uint64 modifierId, uint64 operand) const;
    OnlineService::AchievementsInterface * m_ptAchievementsInterface;  // 0x00
    const Player * m_ptPlayer;  // 0x08
    const ActorCommonData * m_ptACD;  // 0x10
    CrafterType m_eCrafterType;  // 0x18
    Achievement::EquipmentInfo * m_ptEquipmentInfo;  // 0x20
    SNO m_snoPower;  // 0x28
    const Power * m_ptPower;  // 0x30
    const PowerAPIDamage * m_ptPowerDamage;  // 0x38
    const PowerAPIBuffInstance * m_ptBuff;  // 0x40
    const Player * m_ptEvaluatingPlayer;  // 0x48
    int32 m_nCursedChestMonsterKillCount;  // 0x50
};
static_assert(sizeof(EvaluationData) == 0x58, "EvaluationData");

}  // namespace Achievement
