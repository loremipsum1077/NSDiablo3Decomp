#include "UITransmuteCommon.h"

#include "../../CGame.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/Items.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../CPlayer.h"
#include "../../../Common/Networking/GameMessage.h"

// Declared here because their headers are not adopted yet; each comment names the defining file.
void ACDDelete(const ACDID idACD, BOOL bWorldDeleted);  // X/Common/ActorCommonData.cpp:996
const uint32 DWORD_NONE = 0xffffffff;  // X/Sigma/Sigma.h:331






#include "../../../Common/Crafting/ItemCrafting.h"


void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp














struct GridSquare {  // size 0x30
    ACDID id;  // 0x00
    GBID gbid;  // 0x04
    ACDID idACDFakeItem;  // 0x08
    int64 nCountForGBID;  // 0x10
    int64 nCurrencyCountNeededForRecipe;  // 0x18
    int64 nCountForMultipleStacks;  // 0x20
    TransmuteRecipeIngredientType eType;  // 0x28
    bool bNeedsToBeDestroyed;  // 0x2c





    GridSquare()
        : id( ACDID_INVALID ), gbid( GBID_NONE ), idACDFakeItem( ACDID_INVALID )
        , eType( -1 )
        , bNeedsToBeDestroyed( false )
        , nCountForGBID( 0 ), nCurrencyCountNeededForRecipe( 0 ), nCountForMultipleStacks( 0 )
    {
    }

    void Clear()
    {

        if ( bNeedsToBeDestroyed )
        {
            DestroyItem( id );
        }
        DestroyItem( idACDFakeItem );

        id = ACDID_INVALID;
        gbid = GBID_NONE;
        idACDFakeItem = ACDID_INVALID;

        nCountForGBID = nCurrencyCountNeededForRecipe = nCountForMultipleStacks = 0;

        eType = -1;
        bNeedsToBeDestroyed = false;
    }

    void DestroyItem( ACDID idACDItem )
    {

        ActorCommonData * ptACDItem = ACDTryToGet( idACDItem );
        if ( ptACDItem )
        {
            if ( ItemGetNumFilledSockets( *ptACDItem ) > 0 )
            {
                ACDInventoryIterateSlot( *ptACDItem, SLOT_ITEM_SOCKETS, NULL, DestroySocketedItem, NULL, FALSE, FALSE );
            }

            ACDDelete( idACDItem, FALSE );
        }
    }

    static BOOL DestroySocketedItem( const InventoryLocation & tInvLoc, void * pCallBackData )
    {


        ActorCommonData & tACD = ACDGet( tInvLoc.idACDContainer );
        ACDID idACDItem = ACDInventoryGetSquareItem( tACD, tInvLoc.eSlot, tInvLoc.pt );
        ACDDelete( idACDItem, FALSE );

        return TRUE;
    }
};



struct TransmuteGlobals {  // size 0x370

    void Initialize()
    {
        ClearItems();
        dwLockedTimeout = 0;
        ptACDPlayer = NULL;
    }

    void ClearItems()
    {
        for ( int32 x = 0; x < 3; x++ )
        {
            for ( int32 y = 0; y < 3; y++ )
            {
                tGridItems[x][y].Clear();
            }
        }
    }

    GridSquare tGridItems[3][3];  // 0x00
    GridSquare tBackupGridItems[3][3];  // 0x1b0
    uint32 dwLockedTimeout;  // 0x360
    ActorCommonData * ptACDPlayer;  // 0x368
};






TransmuteGlobals * sgptTransmuteGlobals = NULL;






static void sRemoveItemFromGridSlot( const IVector2D & vGridSlot )
{

    GBID gbid = sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y].gbid;

    if ( gbid == GBID_NONE )
    {
        if ( vGridSlot.y < 2 &&
             sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y + 1].id == sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y].id )
        {
            sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y + 1].Clear();
        }
    }
    else
    {
        if ( vGridSlot.y < 2 &&
             sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y + 1].gbid == gbid )
        {
            sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y + 1].Clear();
        }
    }
    sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y].Clear();
}


















































































































void UITransmute::RestoreItems()
{


    for ( int32 x = 0; x < 3; x++ )
    {
        for ( int32 y = 0; y < 3; y++ )
        {

            sgptTransmuteGlobals->tGridItems[x][y].Clear();
            sgptTransmuteGlobals->tGridItems[x][y] = sgptTransmuteGlobals->tBackupGridItems[x][y];


            sgptTransmuteGlobals->tBackupGridItems[x][y].bNeedsToBeDestroyed = false;
            sgptTransmuteGlobals->tBackupGridItems[x][y].Clear();
        }
    }
}



void UITransmute::InitializeForGame()
{

    sgptTransmuteGlobals = new TransmuteGlobals;
    sgptTransmuteGlobals->Initialize();

    PlatformInitializeForGame();
}


void UITransmute::DisposeForGame()
{

    delete sgptTransmuteGlobals;
    sgptTransmuteGlobals = NULL;

    PlatformDisposeForGame();
}


void UITransmute::Reset( ActorCommonData * ptACDPlayer )
{

    sgptTransmuteGlobals->Initialize();
    sgptTransmuteGlobals->ptACDPlayer = ptACDPlayer;
}


BOOL UITransmute::AddItem( const ActorCommonData * ptACDItem, const IVector2D * pvGridSlot, const GBID gbidItem, const int64 nStackCount, const TransmuteRecipeIngredientType eType, const BOOL bPlaySound, const int64 nCurrencyCountNeededForRecipe )
{








    if ( IsLocked() )
    {


        return FALSE;
    }

    return ForceAddItem( ptACDItem, pvGridSlot, gbidItem, nStackCount, eType, bPlaySound, nCurrencyCountNeededForRecipe );
}

























































































































































































// NON_MATCHING: x20/x21 swapped (GOT address vs value of sgptTransmuteGlobals); fail paths of the inlined sRemoveItemFromGridSlot branch to the post-Clear reload instead of past it
void UITransmute::RemoveItem( ACDID idACDItem )
{

    if ( IsLocked() )
    {
        return;
    }

    for ( int32 x = 0; x < 3; x++ )
    {
        for ( int32 y = 0; y < 3; y++ )
        {
            if ( sgptTransmuteGlobals->tGridItems[x][y].id == idACDItem )
            {
                sRemoveItemFromGridSlot( IVector2D( x, y ) );
                return;
            }
        }
    }
}


void UITransmute::ClearItems()
{

    sgptTransmuteGlobals->ClearItems();
}














































BOOL UITransmute::DoTransmute( Player * ptPlayer )
{

    if ( !ptPlayer || !PlayerIsLocal( ptPlayer ) )
    {
        return FALSE;
    }

    if ( IsLocked() )
    {
        return FALSE;
    }

    TransmuteItemsMessage tMessage;

    tMessage.nItemsCount = 0; tMessage.nStackedItemsBitField = 0;
    tMessage.nCurrenciesCount = 0;

    for ( int32 x = 0; x < 3; x++ )
    {
        for ( int32 y = 0; y < 3; y++ )
        {

            IVector2D vItemGridSlot( x, y );
            ACDID idACDItem = GetItemACDIDInGridSlot( vItemGridSlot );
            ActorCommonData * ptACDItem = ACDTryToGet( idACDItem );
            if ( ptACDItem )
            {

                if ( y > 0 &&
                     GetItemACDIDInGridSlot( IVector2D( x, y - 1 ) ) == idACDItem )
                {
                    continue;
                }



                tMessage.annItems[tMessage.nItemsCount] = ANNFromACD( idACDItem );
                if ( GetItemCountForMultipleStacksInGridSlot( vItemGridSlot ) > 0 )
                {
                    tMessage.nStackedItemsBitField |= ( 1 << tMessage.nItemsCount );
                }
                tMessage.nItemsCount++;
            }
            else
            {

                GBID gbidItem = GetItemGBIDInGridSlot( vItemGridSlot );
                if ( ItemIsCurrency( gbidItem ) )
                {

                    tMessage.gbidCurrencies[tMessage.nCurrenciesCount] = gbidItem;
                    tMessage.nCurrenciesCount++;
                }
            }
        }
    }




    if ( tMessage.nItemsCount <= 0 && tMessage.nCurrenciesCount <= 0 )
    {
        return FALSE;
    }

    MessageSendToServer( CMSG_TRY_TRANSMUTE, &tMessage, sizeof( tMessage ), ptPlayer );
    return TRUE;
}
void UITransmute::Lock( uint32 dwLockDuration )
{
    if ( dwLockDuration == DWORD_NONE )
    {
        sgptTransmuteGlobals->dwLockedTimeout = DWORD_NONE;
        return;
    }
    uint32 dwNow = CGameGetClientTicks();
    sgptTransmuteGlobals->dwLockedTimeout = dwNow + dwLockDuration;
}




void UITransmute::Unlock()
{
    sgptTransmuteGlobals->dwLockedTimeout = 0;
}



BOOL UITransmute::IsLocked()
{
    uint32 dwNow = CGameGetClientTicks();
    return dwNow < sgptTransmuteGlobals->dwLockedTimeout;
}



ACDID UITransmute::GetItemACDIDInGridSlot( const IVector2D & vGridSlot )
{

    if ( vGridSlot.x < 0 || vGridSlot.x >= 3 )
        return ACDID_INVALID;
    if ( vGridSlot.y < 0 || vGridSlot.y >= 3 )
        return ACDID_INVALID;


    return sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y].id;
}


GBID UITransmute::GetItemGBIDInGridSlot( const IVector2D & vGridSlot )
{

    if ( vGridSlot.x < 0 || vGridSlot.x >= 3 )
        return GBID_NONE;
    if ( vGridSlot.y < 0 || vGridSlot.y >= 3 )
        return GBID_NONE;


    return sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y].gbid;
}















int64 UITransmute::GetItemGBIDStackCountInGridSlot( const IVector2D & vGridSlot )
{

    if ( vGridSlot.x < 0 || vGridSlot.x >= 3 )
        return 0;
    if ( vGridSlot.y < 0 || vGridSlot.y >= 3 )
        return 0;


    return sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y].nCountForGBID;
}


int64 UITransmute::GetCurrencyCountNeededForRecipeInGridSlot( const IVector2D & vGridSlot )
{

    if ( vGridSlot.x < 0 || vGridSlot.x >= 3 )
        return 0;
    if ( vGridSlot.y < 0 || vGridSlot.y >= 3 )
        return 0;


    return sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y].nCurrencyCountNeededForRecipe;
}


TransmuteRecipeIngredientType UITransmute::GetItemTypeInGridSlot( const IVector2D & vGridSlot )
{

    if ( vGridSlot.x < 0 || vGridSlot.x >= 3 )
        return -1;
    if ( vGridSlot.y < 0 || vGridSlot.y >= 3 )
        return -1;


    return sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y].eType;
}


int64 UITransmute::GetItemCountForMultipleStacksInGridSlot( const IVector2D & vGridSlot )
{

    if ( vGridSlot.x < 0 || vGridSlot.x >= 3 )
        return 0;
    if ( vGridSlot.y < 0 || vGridSlot.y >= 3 )
        return 0;


    return sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y].nCountForMultipleStacks;
}


void UITransmute::GetItemsInTheCube( blz::vector<ACDID, blz::allocator<ACDID> > & arItems )
{

    arItems.clear();

    for ( int32 x = 0; x < 3; x++ )
    {
        for ( int32 y = 0; y < 3; y++ )
        {
            IVector2D vItemGridSlot( x, y );
            ACDID idACDItem = GetItemACDIDInGridSlot( vItemGridSlot );
            ActorCommonData * ptACDItem = ACDTryToGet( idACDItem );
            if ( !ptACDItem )
            {
                continue;
            }



            if ( y > 0 &&
                 GetItemACDIDInGridSlot( IVector2D( x, y - 1 ) ) == idACDItem )
            {
                continue;
            }




            arItems.push_back( ptACDItem->id );
        }
    }
}





























































































































































































































bool UITransmute::IsUberRecipe( TransmuteRecipeAction eAction )
{


    switch ( eAction )
    {
    case TRA_OPEN_PORTAL_GREED:
    case TRA_OPEN_PORTAL_COW:
    case TRA_OPEN_PORTAL_WHIMSEYSHIRE:
    case 16:
    case 17:
        return true;
    default:
        return false;
    }
}

#include "../../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../Common/Protobuf/Items.pb.h"
#include "../../../Common/Player.h"
BOOL ItemIsDrawnAsLargeSlotItem(const GBID gbidItem); // Items.cpp:1858
BOOL PlayerCanManipulateInventory(const ActorCommonData & tACDPlayer); // Player.cpp:4878
// ActorInventory.h, UIGame.h, UIInventory.h: original DWARF declarations.
void ACDAddToInventory(ActorCommonData & tACDItem, const InventoryLocation & tLocation, const BOOL bMarkItemAsDirty, InventorySlotType eCameFrom);
namespace UIGame {
void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer);
}
namespace UIInventory {
void PlayItemInventorySound(const ActorCommonData & tACDItem);
}
typedef blz::unordered_map<ACDID, ACDID> RealToDummyItemMap;
// Item copies and their socketed items share the network-name range starting at 0x2f44.
// No matching named enumerators occur in DWARF; casts retain ACDNetworkName typing.
#line 174 "X/Client/UserInterface/Interface/UITransmuteCommon.cpp"
static ACDID sDuplicateItem(ACDID idACDItem, int32 & nNextItemIndex)
{
#line 178
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem)
        return ACDID_INVALID;
#line 184
    D3::Items::Generator tGenerator;
    ItemGeneratorPopulateFromACDID(*ptACDItem, tGenerator, TRUE);
#line 187
    ACDID idACDDuplicate = CItemGenerate(static_cast<ACDNetworkName>(0x2f44 + nNextItemIndex), tGenerator, NULL);
    if (idACDDuplicate != ACDID_INVALID)
    {
        ++nNextItemIndex;
#line 192
        ActorCommonData & tACDDuplicate = ACDGet(idACDDuplicate);
#line 195
        int32 nSocketsFilled = 0;
        for (int32 i = 0; i < tGenerator.contents_size(); ++i)
        {
            const D3::Items::EmbeddedGenerator & tEmbedded = tGenerator.contents(i);
#line 200
            ACDID idACDSocketed = CItemGenerate(static_cast<ACDNetworkName>(0x2f44 + nNextItemIndex), tEmbedded.generator(), NULL);
            ActorCommonData * ptACDSocketed = ACDTryToGet(idACDSocketed);
            if (ptACDSocketed)
            {
                ++nNextItemIndex;
#line 206
                InventoryLocation tLocation;
                tLocation.eSlot = SLOT_ITEM_SOCKETS;
                tLocation.pt.Set(0, nSocketsFilled);
                tLocation.idACDContainer = tACDDuplicate.id;
                ACDAddToInventory(*ptACDSocketed, tLocation, FALSE, SLOT_NONE);
                ItemAddSocketedItemAttributes(*ptACDSocketed, tACDDuplicate);
                FastAttribMerge(tACDDuplicate.idFastAttrib, ptACDSocketed->idFastAttrib);
                ++nSocketsFilled;
            }
        }
        tACDDuplicate.AttributesSetInt(FastAttribKey(ATTRIBUTE_SOCKETS_FILLED), nSocketsFilled);
    }
    return idACDDuplicate;
}
#line 224
void UITransmute::ReplaceItemsWithCopies()
{
#line 230
    for (int32 x = 0; x < 3; ++x)
    {
        for (int32 y = 0; y < 3; ++y)
            sgptTransmuteGlobals->tBackupGridItems[x][y] = sgptTransmuteGlobals->tGridItems[x][y];
    }
#line 239
    GridSquare tDummyGridItems[3][3];
    RealToDummyItemMap tItemMapping;
    int32 nNextItemIndex = 0;
#line 243
    for (int32 x = 0; x < 3; ++x)
    {
        for (int32 y = 0; y < 3; ++y)
        {
            GridSquare & tSquare = sgptTransmuteGlobals->tBackupGridItems[x][y];
            if (tSquare.gbid != GBID_NONE)
            {
                if (y < 2 && sgptTransmuteGlobals->tBackupGridItems[x][y + 1].gbid == tSquare.gbid)
                    sgptTransmuteGlobals->tGridItems[x][y + 1] = tSquare;
                sgptTransmuteGlobals->tGridItems[x][y] = tSquare;
            }
            else
            {
#line 260
                RealToDummyItemMap::const_iterator iter = tItemMapping.find(tSquare.id);
                if (iter == tItemMapping.end())
                {
                    ACDID idDuplicate = sDuplicateItem(tSquare.id, nNextItemIndex);
                    if (idDuplicate != ACDID_INVALID)
                    {
                        sgptTransmuteGlobals->tGridItems[x][y].id = idDuplicate;
                        sgptTransmuteGlobals->tGridItems[x][y].bNeedsToBeDestroyed = true;
                        tItemMapping[tSquare.id] = idDuplicate;
                    }
                }
                else
                {
                    sgptTransmuteGlobals->tGridItems[x][y].id = iter->second;
                    sgptTransmuteGlobals->tGridItems[x][y].bNeedsToBeDestroyed = false;
                }
            }
        }
    }
}
// Inference: early returns express original common exit branches; the found-slot
// break preserves the original first available column/row selection.
#line 352
// NON_MATCHING: Clang eliminates the initial grid-square type reset before the replacement type is stored.
BOOL UITransmute::ForceAddItem(const ActorCommonData * ptACDItem, const IVector2D * pvGridSlot, const GBID gbidItem, const int64 nStackCount, const TransmuteRecipeIngredientType eType, const BOOL bPlaySound, const int64 nCurrencyCountNeededForRecipe)
{
#line 361
    BOOL bRealItem = ptACDItem != NULL;
#line 364
    for (int32 x = 0; x < 3; ++x)
    {
        for (int32 y = 0; y < 3; ++y)
        {
            if (bRealItem ? ptACDItem->id == sgptTransmuteGlobals->tGridItems[x][y].id : sgptTransmuteGlobals->tGridItems[x][y].gbid == gbidItem)
                return FALSE;
        }
    }
    if (bRealItem && ItemGetMaxStackSize(ptACDItem->hGB, NULL, NULL))
    {
#line 381
        for (int32 x = 0; x < 3; ++x)
        {
            for (int32 y = 0; y < 3; ++y)
            {
                ActorCommonData * ptACDGridItem = ACDTryToGet(sgptTransmuteGlobals->tGridItems[x][y].id);
                if (ptACDGridItem && ptACDGridItem->hGB.gbid == ptACDItem->hGB.gbid)
                {
                    // Original truncates both stack counts to signed 32 bits before accumulating.
                    if (sgptTransmuteGlobals->tGridItems[x][y].nCountForMultipleStacks == 0)
                        sgptTransmuteGlobals->tGridItems[x][y].nCountForMultipleStacks = (int32)ptACDGridItem->GetStackCount();
                    sgptTransmuteGlobals->tGridItems[x][y].nCountForMultipleStacks += (int32)ptACDItem->GetStackCount();
                    return TRUE;
                }
            }
        }
    }
#line 403
    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    if (!ptACDPlayer || !PlayerCanManipulateInventory(*ptACDPlayer))
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_ITEMS_CANNOT_BE_MOVED_AT_THIS_TIME), FALSE, NULL);
        return FALSE;
    }
    if (bRealItem && ItemIsUnidentified(*ptACDItem))
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_TRANSMUTE_CANNOT_USE_UNIDENTIFIED_ITEM), FALSE, NULL);
        return FALSE;
    }
    if (bRealItem && ItemGetUseType(*ptACDItem) == ITEM_USE_TREASURE_BAG)
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_TRANSMUTE_CANNOT_USE_ITEM_IN_CUBE), FALSE, NULL);
        return FALSE;
    }
#line 422
    BOOL bLargeItem = ItemIsDrawnAsLargeSlotItem(bRealItem ? ptACDItem->hGB.gbid : gbidItem);
#line 425
    IVector2D vGridSlot;
    if (pvGridSlot)
    {
        vGridSlot = *pvGridSlot;
        if (GetItemACDIDInGridSlot(vGridSlot) != ACDID_INVALID)
            return FALSE;
        if (bLargeItem && GetItemACDIDInGridSlot(vGridSlot + IVector2D(0, 1)) != ACDID_INVALID)
            return FALSE;
    }
    else
    {
#line 439
        int32 nMaxY = bLargeItem ? 2 : 3;
#line 445
        BOOL bFoundGridSlot = FALSE;
        for (vGridSlot.x = 0; vGridSlot.x < 3; ++vGridSlot.x)
        {
            for (vGridSlot.y = 0; vGridSlot.y < nMaxY; ++vGridSlot.y)
            {
#line 450
                BOOL bNoRealItemInSlot = GetItemACDIDInGridSlot(vGridSlot) == ACDID_INVALID && (!bLargeItem || GetItemACDIDInGridSlot(vGridSlot + IVector2D(0, 1)) == ACDID_INVALID);
#line 452
                BOOL bNoFakeItemInSlot = GetItemGBIDInGridSlot(vGridSlot) == GBID_NONE && (!bLargeItem || GetItemGBIDInGridSlot(vGridSlot + IVector2D(0, 1)) == GBID_NONE);
                if (bNoRealItemInSlot && bNoFakeItemInSlot)
                {
                    bFoundGridSlot = TRUE;
                    break;
                }
            }
            if (bFoundGridSlot)
                break;
        }
        if (!bFoundGridSlot)
            return FALSE;
    }
#line 475
    GridSquare & tSquare = sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y];
    ACDID idACDFakeItem = ACDID_INVALID;
    if (bRealItem)
    {
        tSquare.Clear();
        tSquare.id = ptACDItem->id;
        tSquare.eType = eType;
    }
    else
    {
        if (ItemIsCurrency(gbidItem))
        {
#line 486
            int32 nGridIndex = vGridSlot.x + vGridSlot.y * 3;
            GBHandle hGBItem(GB_ITEMS, gbidItem);
#line 489
            D3::Items::Generator tItem;
            ItemGeneratorToDefaults(tItem);
            hGBItem.Export(*tItem.mutable_gb_handle());
            tItem.set_flags(tItem.flags() | 1);
            idACDFakeItem = CItemGenerate(static_cast<ACDNetworkName>(0x2f74 + nGridIndex), tItem, NULL);
        }
        tSquare.Clear();
        tSquare.gbid = gbidItem;
        tSquare.idACDFakeItem = idACDFakeItem;
        tSquare.nCountForGBID = nStackCount;
        tSquare.nCurrencyCountNeededForRecipe = nCurrencyCountNeededForRecipe;
        tSquare.eType = eType;
    }
    if (bLargeItem)
    {
#line 510
        GridSquare & tSquareBelow = sgptTransmuteGlobals->tGridItems[vGridSlot.x][vGridSlot.y + 1];
        tSquareBelow.Clear();
        if (bRealItem)
            tSquareBelow.id = ptACDItem->id;
        else
        {
            tSquareBelow.gbid = gbidItem;
            tSquareBelow.idACDFakeItem = idACDFakeItem;
            tSquareBelow.nCountForGBID = nStackCount;
            tSquareBelow.nCurrencyCountNeededForRecipe = nCurrencyCountNeededForRecipe;
        }
        tSquareBelow.eType = eType;
    }
    if (bRealItem && bPlaySound)
        UIInventory::PlayItemInventorySound(*ptACDItem);
    return TRUE;
}

extern "C" int32 sub_710024d7e0()
{
    ActorCommonData * ptACD = ACDTryToGet(LocalPlayerGet()->idPlayerACD);
    return ptACD->AttributesGetInt(ATTRIBUTE_FOURTH_KANAIS_CUBE_SLOT);
}
