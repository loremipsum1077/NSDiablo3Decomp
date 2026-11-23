// Declarations CActorInventory.cpp needs whose headers are not adopted yet, and the message
// structs it sends (X/Common/Networking/GameMessage.h and X/Common/ActorInventory.h in the DWARF).
// Kept in this include so the .cpp keeps the original line numbers.










































// GameMessageType values this file sends (X/Common/Networking/GameMessage.h).















// X/Client/UserInterface/Interface/UITutorials.h (only the member this file calls)
class UITutorial;
#include "UserInterface/Interface/UITutorials.h"



extern UITutorialManager gtTutorialMgr;
CurrencyType CurrencyFromGBID(GBID gbid);  // X/Common/Items.cpp:10444
BOOL ItemIsBeingManipulated(const ActorCommonData & tACDItem);  // X/Client/CItems.cpp:1310
void ItemSetIsBeingManipulated(ActorCommonData & tACDItem, const BOOL fVal, const uint32 dwTimeout);  // X/Client/CItems.cpp:1319
CursorMgr & PlayerGetCursorMgr();  // X/Client/CPlayerInput.cpp:6603
BOOL ACDInventoryCanUseItem(const ActorCommonData & tACDUser, const ActorCommonData & tACDItem, GameError * peOutError);  // ActorInventory.cpp:11276
void CPlayerSayError(uint32 dwSoundTag, RActorID idRActor, BOOL bIgnoreCooldown);  // X/Client/CPlayer.cpp:3307
void ACDRemoveFromCurrentPosition(ActorCommonData & tACDItem);  // ActorInventory.cpp:3188
void ACDAddToInventory(ActorCommonData & tACDItem, const InventoryLocation & tLocation, const BOOL bMarkItemAsDirty, InventorySlotType eCameFrom);  // ActorInventory.cpp:3969
void CPlayerHandleItemMove(ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest, Player & tPlayer);  // X/Client/CPlayer.cpp:5161
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp:45
Player * LocalPlayerGet();  // X/Client/CPlayer.cpp:1612
namespace UIGame { void DisplayError(const GameErrorData & tError, BOOL fDisplayBnetError, const Player * ptPlayer); }  // Interface/UIGame.cpp
namespace UIHUD { namespace Console { void FlashPotionIcon(int32 nControllerIndex); } }  // Console/UIConsoleHUD.cpp
namespace UIInventory { void PlayItemInventorySound(const ActorCommonData & tACDItem); namespace Console { void NotifyOfItemPickup(const ActorCommonData & tACDItem, int32 nParam); } }  // Interface/UIInventory.cpp
namespace Tutorial { BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial); }  // X/Common/Tutorial.cpp
