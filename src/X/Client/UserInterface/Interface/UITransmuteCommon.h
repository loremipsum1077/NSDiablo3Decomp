// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UITransmuteCommon.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"

class ActorCommonData;
struct IVector2D;
struct Player;

namespace UITransmute {
void ReplaceItemsWithCopies();
void RestoreItems();
void InitializeForGame();
void DisposeForGame();
void PlatformInitializeForGame();  // X/Client/UserInterface/Console/UIConsoleTransmute.cpp:582
void PlatformDisposeForGame();  // X/Client/UserInterface/Console/UIConsoleTransmute.cpp
void Reset(ActorCommonData * ptACDPlayer);
BOOL AddItem(const ActorCommonData * ptACDItem, const IVector2D * pvGridSlot, const GBID gbidItem, const int64 nStackCount, const TransmuteRecipeIngredientType eType, const BOOL bPlaySound, const int64 nCurrencyCountNeededForRecipe);
BOOL ForceAddItem(const ActorCommonData * ptACDItem, const IVector2D * pvGridSlot, const GBID gbidItem, const int64 nStackCount, const TransmuteRecipeIngredientType eType, const BOOL bPlaySound, const int64 nCurrencyCountNeededForRecipe);
void RemoveItem(ACDID idACDItem);
void ClearItems();
BOOL DoTransmute(Player * ptPlayer);
void Lock(uint32 dwLockDuration);
void Unlock();
BOOL IsLocked();
ACDID GetItemACDIDInGridSlot(const IVector2D & vGridSlot);
GBID GetItemGBIDInGridSlot(const IVector2D & vGridSlot);
int64 GetItemGBIDStackCountInGridSlot(const IVector2D & vGridSlot);
int64 GetCurrencyCountNeededForRecipeInGridSlot(const IVector2D & vGridSlot);
TransmuteRecipeIngredientType GetItemTypeInGridSlot(const IVector2D & vGridSlot);
int64 GetItemCountForMultipleStacksInGridSlot(const IVector2D & vGridSlot);
void GetItemsInTheCube(blz::vector<ACDID, blz::allocator<ACDID> > & arItems);
bool IsUberRecipe(TransmuteRecipeAction eAction);
}  // namespace UITransmute
