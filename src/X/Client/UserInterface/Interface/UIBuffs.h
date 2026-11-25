// Adopted from original DWARF layout.
// Original file: X/Client/UserInterface/Interface/UIBuffs.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../UIID.h"
#include "../../../Common/ImageHandle.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/DataStructures/XPooledList.h"
#include "../../../Sigma/SigmaTypes.h"

struct Player;

namespace UIBuffs {
struct SortedBuff {  // size 0x28
    SNO snoPower;  // 0x00
    int32 nBuffIdx;  // 0x04
    const Player * ptPlayer;  // 0x08
    int32 nDuration;  // 0x10
    int32 nStackCount;  // 0x14
    BOOL bPlayerCanCancel;  // 0x18
    BOOL fIsDebuff;  // 0x1c
    BOOL fIsHidden;  // 0x20
    int32 unk_24;  // 0x24
    void Initialize(const SNO _snoPower, const int32 _nBuffIdx, const BOOL _fIsDebuff, const BOOL _fIsHidden, const int32 _unk_24, const int32 _nStackCount, const Player * _ptPlayer, const BOOL bCreateControls);
    ImageHandle GetImage() const;
    int32 GetCanCancelTag() const;
};
static_assert(sizeof(SortedBuff) == 0x28, "SortedBuff");

typedef XPooledList<UIBuffs::SortedBuff, UIBuffs::SortedBuff> XPooledSortedBuffList;

struct UIBuffGlobals {  // size 0x150
    XListMemoryPool<UIBuffs::SortedBuff> tSortedBuffPool;  // 0x00
    UIBuffs::XPooledSortedBuffList listBeneficialBuffs;  // 0x30
    UIBuffs::XPooledSortedBuffList listHarmfulBuffs;  // 0x58
    UIBuffs::XPooledSortedBuffList listPartyBuffs;  // 0x80
    UIBuffs::XPooledSortedBuffList listMergeTooltipBuffs;  // 0xa8
    UIBuffs::XPooledSortedBuffList listSkillButtonBuffs;  // 0xd0
    UIBuffs::XPooledSortedBuffList listHiddenBuffs;  // 0xf8
    UIBuffs::XPooledSortedBuffList listMinimapBuffs;  // 0x120
    UIID idBuffHolderPlaceholderBuff;  // 0x148
};
static_assert(sizeof(UIBuffGlobals) == 0x150, "UIBuffGlobals");

UIBuffGlobals * GetBuffGlobals();
void InitializeForGame();
void DisposeForGame();
bool BuffIsAssociatedWithSkillButton(SNO snoPower, const Player * ptPlayer);
void RemoveBuff(SNO snoPower, int32 nBuffIndex, const Player * ptPlayer);
void CleanUpBuffIconsForSkillButtons();
void Update();
void AddBuff(SNO snoPower, int32 nBuffIndex, int32 nStackCount, const Player * ptPlayer);
void PlayerLeftGame(const Player * ptPlayer);
void UpdateBuffProgressBar(const UIID & idGreenBar, const UIID & idYellowBar, float flPos);
}  // namespace UIBuffs
