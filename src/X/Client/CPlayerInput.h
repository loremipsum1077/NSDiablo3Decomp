// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/CPlayerInput.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
// UserInterface/Interface/UISkillsHotbar.h is not adopted yet; nothing below needs it.
#include "../Common/CommonTypes.h"
#include "../Common/GameBalance.h"
#include "../Common/HotbarButton.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/Target.h"
#include "../Sigma/SigmaTypes.h"

class ActorCommonData;

typedef int32 SkillButton;

typedef int32 UIAction;

enum CursorState {
    CURSORSTATE_NORMAL = 0,
    CURSORSTATE_MOVE_ITEM = 1,
    CURSORSTATE_USE_ITEM = 2,
    CURSORSTATE_SPLIT_STACK = 3,
    CURSORSTATE_SALVAGE_ITEM = 4,
    CURSORSTATE_BUY_ITEM = 5,
    CURSORSTATE_QUEUED_SKILL = 6,
    CURSORSTATE_MOVING_HOTBAR_BUTTON = 7,
    CURSORSTATE_RESIZE_HORIZONTAL = 8,
    CURSORSTATE_RESIZE_VERTICAL = 9,
    CURSORSTATE_RESIZE_DIAGONAL_LEFT = 10,
    CURSORSTATE_RESIZE_DIAGONAL_RIGHT = 11,
    CURSORSTATE_MAP_NOT_DRAGGING = 12,
    CURSORSTATE_MAP_DRAGGING = 13,
};

struct ResolvedClick {  // size 0x30
    ResolvedClick() {  // line 47
        idRActor = RACTORID_INVALID;
        snoOriginalPower = SNO_NONE;
        snoPower = SNO_NONE;
        bPlayItemSound = FALSE;
        idACDItemToUse = ACDID_INVALID;
    }
    RActorID idRActor;  // 0x00
    SNO snoOriginalPower;  // 0x04
    SNO snoPower;  // 0x08
    BOOL bPlayItemSound;  // 0x0c
    ACDID idACDItemToUse;  // 0x10
    Target tTarget;  // 0x14
};
static_assert(sizeof(ResolvedClick) == 0x30, "ResolvedClick");

class CursorMgr {  // size 0x34
public:
    void Init();
    void ResetCursorState(const BOOL fClearManipulationTimeout);
    CursorState GetCursorState() const;
    void Update();
    void SetHotbarButtonToMove(const HotbarButtonData &, const SkillButton);
    void GetHotbarButtonToMove(HotbarButtonData & tButtonToMove, SkillButton & eSkillButton) const;
    void Draw();
    void QueueSkill(const SNO);
    SNO GetQueuedSkill() const;
    void SetItemToMove(const ActorCommonData & tACDItemToMove);
    ActorCommonData * GetItemToMove();
    void SetItemToBuy(const ActorCommonData &);
    ActorCommonData * GetItemToBuy();
    void SetSplitStack(const ActorCommonData &, const uint32);
    ActorCommonData * GetStackToSplit();
    uint32 GetStackSplitAmount() const;
    void SetItemToUse(const ActorCommonData &);
    ActorCommonData * GetItemToUse();
    void SetDialogResizingCursor(const CursorState);
    void SetMapDraggingCursor(const BOOL);
    void SalvageItem();
    void SetHotBarOverridden(const BOOL bOverridden);
    BOOL GetHotBarOverridden();
    void HandleSkillRemoval(const SNO snoPower);
private:
    void sUpdateNormalCursorImage();
    void sSetCursorState(const CursorState eState);
    CursorState m_eCursorState;  // 0x00
    HotbarButtonData m_tButtonToMove;  // 0x04
    SkillButton m_eSkillSlotToMove;  // 0x14
    SNO m_snoQueuedPower;  // 0x18
    ACDID m_idACDItem;  // 0x1c
    GBID m_gbidItem;  // 0x20
    ACDID m_idACDNPC;  // 0x24
    GBID m_gbidEnchantment;  // 0x28
    BOOL m_bHotbarOverridden;  // 0x2c
    uint32 m_dwAmount;  // 0x30
};
static_assert(sizeof(CursorMgr) == 0x34, "CursorMgr");

struct BufferedInput;
struct RActor;

// X/Client/CPlayerInput.cpp
BOOL CPlayerInputClickToPickupRActor(const RActor * ptPlayerRActor, const RActorID idRActorHit);  // line 3025
BOOL PlayingInput(const BufferedInput & tInput);  // line 5242


// Functions defined in this file (bodies are not in the DWARF):
//   line    47  ResolvedClick::ResolvedClick()  [inlined into 11 functions]

// CPlayerInput.cpp: the app subsystem entry points (X/Common/Game.cpp sgtAppSubsystems).
void CPlayerInputInitializeForApp();
void CPlayerInputDisposeForApp();

// CPlayerInput.cpp:7022,7030. Declared for X/Common/ActorCommonData.cpp.
struct Player;
int32 CPlayerInputGetComboLevel(Player * ptPlayer);
void CPlayerInputSetComboLevel(int32 nComboLevel, Player * ptPlayer);
