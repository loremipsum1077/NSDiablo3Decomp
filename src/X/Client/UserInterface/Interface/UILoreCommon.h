// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UILoreCommon.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOFiles.h"

typedef int32 ConversationType;

struct Player;
struct UIID;
struct ConversationDefinition;

void UILoreCommon_InitializeForGame();
void UILoreCommon_DisposeForGame();

namespace UILoreNotify {
void OnLoreAcquired(const SNO snoLore);
void Show(const SNO snoLore);
void Hide();
void Update();
}  // namespace UILoreNotify

namespace UILore {
void RefreshUI();
void Pause(BOOL fPause);
BOOL PlayLore(const SNO snoLore, const Player * ptPlayer);
void StopLore();
}  // namespace UILore

namespace UILorePlayer {
void OpenForLore(const SNO snoLore);
void Open(const UIID &);
void OpenToLorePage(const UIID &);
void Close(const UIID &, BOOL bCancelLore);
void Update();
BOOL IsPlaying();
void OnConversationLinePlayed(const ConversationDefinition * pdefConv);
}  // namespace UILorePlayer
