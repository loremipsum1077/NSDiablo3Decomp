// Original file: X/Client/CActorInventory.cpp
#include "../Common/SNO/SNOName.h"
#include "../Common/WorldPlace.h"
#include "Gfx/GfxVB.h"
#include "ImageCollection.h"
#include "CPlayer.h"
#include "CPlayerInput.h"
#include "UserInterface/Interface/UIConfirmation.h"
#include "UserInterface/UIID.h"
#include "../Common/ActorCommonData.h"
#include "../Common/ActorInventory.h"
#include "../Common/Attributes/FastAttribute.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Common/Definitions/StringListDefinition.h"
#include "../Common/GameBalance.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Items.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Player.h"
#include "../Common/TagMaps/ActorTags.h"
#include "../Common/Target.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/RefString.h"
#include "CActorInventoryDecl.inc"

typedef int32 ItemDrawType;  // X/Client/CActorInventory.h
enum { ITEM_DRAW_INVENTORY = 0, ITEM_DRAW_SOCKETED = 1 };  // names invented; values from the code

ACDID ACDInventoryGetUltimateContainer( const ActorCommonData & tACDItem );  // X/Common/ActorInventory.cpp:4224
void ACDInventoryGetItemDrawInfoWithGBID( const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType, const PlayerClass eClass, const BOOL fIsMale );






void ACDInventoryGetItemDrawInfo( const ActorCommonData & tACDItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType, const PlayerClass eClass, const BOOL fIsMale )
{
    if ( tACDItem.IsItem() )
    {
        GBID gbidItem = tACDItem.hGB.gbid;
        ACDInventoryGetItemDrawInfoWithGBID( gbidItem, tImageTexFrame, eDrawType, eClass, fIsMale );
    }
}











void ACDInventoryGetItemDrawInfo( const ActorCommonData & tACDItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType )
{
    ACDID idACD = ACDInventoryGetUltimateContainer( tACDItem );
    Player * ptPlayer = PlayerGetByACD( idACD );
    if ( ptPlayer == NULL )
    {
        ptPlayer = LocalPlayerGet();
    }
    PlayerClass eClass = PlayerGetPlayerClass( ptPlayer );
    BOOL fIsMale = PlayerIsMale( *ptPlayer );





    ACDInventoryGetItemDrawInfo( tACDItem, tImageTexFrame, eDrawType, eClass, fIsMale );
}




ImageHandle ACDInventoryGetImageHandleFromGBID(
    const GBID gbidItem,
    const ItemDrawType eDrawType,
    const PlayerClass eClass,
    const BOOL fIsMale )
{
    GBHandle hgbItem( GB_ITEMS, gbidItem );
    SNO snoActor = GameBalanceGetActorSNO( hgbItem );





    ScopedActorDefinitionLockR pdefActor( snoActor );
    ImageHandle hImage = 0;  // initial 0 inferred: the default switch path goes straight to the fValidClass test at 117
    BOOL fValidClass = eClass != PLAYER_CLASS_NONE;
    switch (eDrawType)
    {
    case ITEM_DRAW_INVENTORY:
        if (!fIsMale && fValidClass)  // operand order from column 16 (the && after !fIsMale)
        {
            hImage = pdefActor->tInvImages[eClass].hFemaleImage;
        }
        else if (fValidClass)
        {
            hImage = pdefActor->tInvImages[eClass].hDefaultImage;
        }
        break;
    case ITEM_DRAW_SOCKETED:
        hImage = pdefActor->hSocketedImage;
        break;
    }




    if (hImage == 0)
    {
        if (fValidClass)
        {
            hImage = pdefActor->tInvImages[eClass].hDefaultImage;
        }
        // the 122 test and its 3-tab body (columns 59 at 124) sit inside the 115 block
        if (hImage == 0)
        {
            hImage = pdefActor->tInvImages[PLAYER_CLASS_BARBARIAN].hDefaultImage;  // index 1 = PLAYER_CLASS_BARBARIAN: offset 0x168, and the 22-character name fits column 59 of row 124
        }
    }

    return hImage;
}



void ACDInventoryGetItemDrawInfoWithGBID(
    const GBID gbidItem,
    ImageTextureFrame & tImageTexFrame,
    const ItemDrawType eDrawType,
    const PlayerClass eClass,
    const BOOL fIsMale )
{
    ImageHandle hImage;
    hImage = ACDInventoryGetImageHandleFromGBID( gbidItem, eDrawType, eClass, fIsMale );

    if ( !ImageCollectionGetImageInfo( (ImageCollectionType)0, hImage, tImageTexFrame ) )
    {

        ImageCollectionGetImageInfo( (ImageCollectionType)0, "ItemPlaceholderIcon", tImageTexFrame );
    }
}




void ACDInventoryGetItemDrawInfoWithGBID(
    const GBID gbidItem,
    ImageTextureFrame & tImageTexFrame,
    const ItemDrawType eDrawType )
{
    Player * ptPlayer = LocalPlayerGet();
    PlayerClass eClass = PlayerGetPlayerClass( ptPlayer );

    BOOL fIsMale = PlayerIsMale( *ptPlayer );

    ACDInventoryGetItemDrawInfoWithGBID( gbidItem, tImageTexFrame, eDrawType, eClass, fIsMale );
}




void ACDInventoryGetItemDrawInfoWithGBID(
    const ACDID idPlayer,
    const GBID gbidItem,
    ImageTextureFrame & tImageTexFrame,
    const ItemDrawType eDrawType )
{
    Player * ptPlayer = PlayerGetByACD( idPlayer );
    PlayerClass eClass = PlayerGetPlayerClass( ptPlayer );

    BOOL fIsMale = PlayerIsMale( *ptPlayer );

    ACDInventoryGetItemDrawInfoWithGBID( gbidItem, tImageTexFrame, eDrawType, eClass, fIsMale );
}



void CACDInventoryRequestDrop( ActorCommonData * ptACD,
                               const ACDID idACDOwner )

{


    if ( ptACD == NULL )
    {
        return;
    }


    CurrencyType eCurrency = CurrencyFromGBID( ptACD->hGB.gbid );
    if ( eCurrency != CURRENCY_INVALID )
    {
        CurrencyMessage tMessage;
        SigmaMemoryClear( &tMessage, sizeof( tMessage ) );
        tMessage.eCurrency = eCurrency;
        tMessage.uAmount = ptACD->GetStackCount();
        MessageSendToServer( CMSG_DROP_CRAFTING_MATERIAL, &tMessage, sizeof( tMessage ), PlayerGetByACD( idACDOwner ) );
        return;
    }

    if ( ItemIsBeingManipulated( *ptACD ) )
        return;

    ItemSetIsBeingManipulated( *ptACD, TRUE, 120 );

    ACDID idACDOwnerFromContainer = ACDInventoryGetUltimateContainer( *ptACD );

    ANNDataMessage tMessage;
    SigmaMemoryClear( &tMessage, sizeof( tMessage ) );
    tMessage.ann = ANNFromACD( ptACD->id );
    MessageSendToServer( CMSG_TRY_DROP, &tMessage, sizeof( tMessage ), PlayerGetByACD( idACDOwnerFromContainer ) );

}



void CACDInventoryRequestStackPortionDrop( const ActorCommonData * ptACD,
                                           const uint64 uAmount,
                                           const ACDID idACDOwner )

{


    if ( ptACD == NULL )
    {
        return;
    }


    CurrencyType eCurrency = CurrencyFromGBID( ptACD->hGB.gbid );
    if ( eCurrency != CURRENCY_INVALID )
    {
        CurrencyMessage tMessage;
        SigmaMemoryClear( &tMessage, sizeof( tMessage ) );
        tMessage.eCurrency = eCurrency;
        tMessage.uAmount = uAmount;
        MessageSendToServer( CMSG_DROP_CRAFTING_MATERIAL, &tMessage, sizeof( tMessage ), PlayerGetByACD( idACDOwner ) );
    }
    else
    {

        ACDID idACDOwner = ACDInventoryGetUltimateContainer( *ptACD );
        StackPortionMessage tMessage;
        SigmaMemoryClear( &tMessage, sizeof( tMessage ) );
        tMessage.annItem = ANNFromACD( ptACD->id );
        tMessage.uAmount = uAmount;
        MessageSendToServer( CMSG_TRY_INVENTORY_DROP_STACK_PORTION, &tMessage, sizeof( tMessage ), PlayerGetByACD( idACDOwner ) );
    }

    PlayerGetCursorMgr().ResetCursorState( FALSE );
}

void CACDInventoryRequestUse( ActorCommonData & tACDItem, const Target & tTarget, BOOL bPlaySound, Player * ptPlayer );

















void CACDInventoryRequestUse( ActorCommonData & tACDItem,
                              const ACDID idACDTarget,
                              BOOL bPlaySound,
                              Player * ptPlayer )

{
    Target tTarget;
    tTarget.InitializeFromACD( idACDTarget, FALSE, FALSE );

    CACDInventoryRequestUse( tACDItem, tTarget, bPlaySound, ptPlayer );
}



static ACDID sidACDInventoryRequestItem = ACDID_INVALID;
static Target stACDInventoryRequestTarget;


static void sInventoryRequestUse( const UIID & )

{
    InventoryRequestUseMessage tMessage;
    tMessage.annItemToUse = ANNFromACD( sidACDInventoryRequestItem );
    tMessage.annTarget = ANNFromACD( stACDInventoryRequestTarget.idACDTarget );
    tMessage.eType = stACDInventoryRequestTarget.eType;
    tMessage.tPlace = stACDInventoryRequestTarget.tPlace;

    MessageSendToServer( CMSG_TRY_INVENTORY_USE, &tMessage, sizeof( tMessage ), LocalPlayerGet() );

    sidACDInventoryRequestItem = ACDID_INVALID;
    stACDInventoryRequestTarget.Clear();
}


static void sInventoryCancelUse( const UIID & )

{
    sidACDInventoryRequestItem = ACDID_INVALID;
    stACDInventoryRequestTarget.Clear();
}

BOOL CACDInventoryGetUseMessagePrompt( const ActorCommonData & tACDUser, const ActorCommonData & tACDItem, const Target &, CRefString & sMsg );
void CACDInventoryRequestUse( ActorCommonData & tACDItem,
                              const Target & tTarget,
                              BOOL bPlaySound,
                              Player * ptPlayer )

{
    if ( ptPlayer == NULL )
    {
        ptPlayer = LocalPlayerGet();
    }

    if ( ptPlayer == NULL )
    {
        return;
    }

    ACDID idACDPlayer = ptPlayer->idPlayerACD;

    ActorCommonData * ptACDPlayer = ACDTryToGet( idACDPlayer );
    if ( ptACDPlayer == NULL )
    {
        return;
    }


    GameError eError = -1;
    if ( !ACDInventoryCanUseItem( *ptACDPlayer, tACDItem, &eError ) )
    {
        if ( eError != -1 )
        {
            UIGame::DisplayError( GameErrorData( eError ), FALSE, NULL );
        }
        else
        {
            uint32 uTagWarning = TAG_ACTOR_WARNING_CANT_USE;
            ItemUseType eUseType = ItemGetUseType( tACDItem );
            if ( eUseType == ITEM_USE_HEALTHPOTION )
            {
                uTagWarning = TAG_ACTOR_WARNING_POTION_IN_COOLDOWN;
            }


            CPlayerSayError( uTagWarning, ptPlayer->idPlayerRActor, FALSE );
            UIHUD::Console::FlashPotionIcon( ptPlayer->nControllerIndex );
        }

        return;
    }




    CRefString sMsg;
    if ( CACDInventoryGetUseMessagePrompt( *ptACDPlayer, tACDItem, tTarget, sMsg ) )
    {
        sidACDInventoryRequestItem = tACDItem.id;
        stACDInventoryRequestTarget = tTarget;

        UIConfirmation::Show( sMsg.str(), sInventoryRequestUse, sInventoryCancelUse, FALSE, FALSE, TRUE, -1 );
    }
    else
    {
        if ( bPlaySound )
        {
            UIInventory::PlayItemInventorySound( tACDItem );
        }

        InventoryRequestUseMessage tMessage;
        tMessage.annItemToUse = ANNFromACD( tACDItem.id );
        tMessage.annTarget = ANNFromACD( tTarget.idACDTarget );
        tMessage.eType = tTarget.eType;
        tMessage.tPlace = tTarget.tPlace;

        MessageSendToServer( CMSG_TRY_INVENTORY_USE, &tMessage, sizeof( tMessage ), ptPlayer );
    }
}


void CACDInventoryDestroy( ActorCommonData & tACDItem )

{


    ActorCommonData * ptACDContainer = ACDTryToGet( tACDItem.tLocation.tInvLocation.idACDContainer );





    if ( ptACDContainer )
    {
        ACDRemoveFromCurrentPosition( tACDItem );
    }
}


BOOL CACDInventorySetLocation( const ACDID idACDPickee,
                               ActorCommonData & tACDItem,
                               const InventoryLocation & tNewInvLocation,
                               const BOOL bPlayNewItemNotification )

{

    const InventorySlotType eSlot = tNewInvLocation.eSlot;
    const IVector2D & pt = tNewInvLocation.pt;

    InventorySlotType eCameFrom = tACDItem.GetLocationSlot();
    ACDRemoveFromCurrentPosition( tACDItem );

    tACDItem.ChangeLocation( 1 );
    InventoryLocation & tInvLocation = tACDItem.GetInventoryLocation();
    tInvLocation.eSlot = eSlot;
    tInvLocation.pt = pt;
    tInvLocation.idACDContainer = idACDPickee;

    ACDAddToInventory( tACDItem, tInvLocation, FALSE, eCameFrom );

    SNO snoTutorial = GlobalSNOGet( GLOBALSNO_TUTORIAL_INVENTORY_NEAR_FULL );
    const ActorCommonData * ptACDPickee = ACDTryToGet( idACDPickee );
    const Player * ptPlayer = LocalPlayerGet();
    if ( ptACDPickee && ptPlayer && ptPlayer->idPlayerACD == idACDPickee &&
         !Tutorial::PlayerHasSeen( *ptPlayer, snoTutorial ) )
    {

        if ( ACDInventoryGetNumEmptySlots( *ptACDPickee, 0 ) <= 10 )
        {
            gtTutorialMgr.ShowTutorial( GetUIIDNone(), snoTutorial, NULL, TRUE );
        }
    }






    if ( bPlayNewItemNotification )
    {
        UIInventory::Console::NotifyOfItemPickup( tACDItem, 0 );
    }

    return TRUE;
}




static ACDID sidACDInventoryMoveItem = ACDID_INVALID;
static InventoryLocation stInventoryMoveItemInvLocDest;

void CACDInventoryRequestMove( ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest, BOOL bIgnoreSoulbound, Player * ptPlayer ); void sHandleBindOK( const UIID & )

{
    ActorCommonData * ptACD = ACDTryToGet( sidACDInventoryMoveItem );
    if ( ptACD == NULL )
    {
        return;
    }

    CACDInventoryRequestMove( *ptACD, stInventoryMoveItemInvLocDest, TRUE, NULL );

    sidACDInventoryMoveItem = ACDID_INVALID;

}

void CACDInventoryRequestMove( ActorCommonData & tACDItem,
                               const InventoryLocation & tInvLocDest,
                               BOOL bIgnoreSoulbound,
                               Player * ptPlayer )

{


    if ( !bIgnoreSoulbound &&
         tInvLocDest.eSlot >= 1 && tInvLocDest.eSlot <= 13 &&

         ItemGetBindingLevel( tACDItem ) == IBL_ACCOUNT_EQUIP && ItemGetBoundToACD( tACDItem, FALSE ) == ACDID_INVALID )
    {
        CRefString sDisplay = StringListFetch( "H2OLayout:BindConfirmMessage" );
        sidACDInventoryMoveItem = tACDItem.id;
        stInventoryMoveItemInvLocDest = tInvLocDest;
        UIConfirmation::Show( sDisplay, sHandleBindOK, NULL, FALSE, FALSE, TRUE, -1 );

        return;
    }



    if ( ptPlayer == NULL )
    {
        ptPlayer = LocalPlayerGet();
    }

    CPlayerHandleItemMove( tACDItem, tInvLocDest, *ptPlayer );

    PlayerGetCursorMgr().ResetCursorState( FALSE );
}



void CACDInventoryRequestQuickMove( ActorCommonData & tACDItem,
                                    const InventoryLocation & tInvLocDest,
                                    const int32 * pnSlotYMinMax,
                                    const Player * ptPlayer )

{
    if ( ItemIsBeingManipulated( tACDItem ) ) return;
    ItemSetIsBeingManipulated( tACDItem, TRUE, 120 );

    InventoryRequestQuickMoveMessage tMessage;
    tMessage.annItem = ANNFromACD( tACDItem.id );
    tMessage.annOwner = ANNFromACD( tInvLocDest.idACDContainer );
    tMessage.eSlot = tInvLocDest.eSlot;

    if ( pnSlotYMinMax )
    {
        tMessage.nSlotMinY = pnSlotYMinMax[0];
        tMessage.nSlotMaxY = pnSlotYMinMax[1];
    }
    else
    {
        IVector2D tSizeSlot;
        ACDInventorySlotGetSize( tInvLocDest.eSlot, tSizeSlot );

        tMessage.nSlotMinY = 0;
        tMessage.nSlotMaxY = tSizeSlot.y;
    }


    if ( ptPlayer == NULL )
    {
        ptPlayer = LocalPlayerGet();
    }


    MessageSendToServer( CMSG_TRY_INVENTORY_QUICK_MOVE, &tMessage, sizeof( tMessage ), ptPlayer );

}



void CACDInventoryRequestToSocket( const ActorCommonData & tACDGem,
                                   const ActorCommonData & tACDItemToReceiveGem,
                                   Player * ptPlayer )

{
    InventoryRequestSocketMessage tMessage;
    tMessage.annGem = ANNFromACD( tACDGem.id );
    tMessage.annItemToReceiveGem = ANNFromACD( tACDItemToReceiveGem.id );

    MessageSendToServer( CMSG_TRY_INVENTORY_SOCKET, &tMessage, sizeof( tMessage ), ptPlayer );

    PlayerGetCursorMgr().ResetCursorState( FALSE );
}























void CACDInventoryRequestSplitStack( const ActorCommonData * ptACDStack,
                                     const InventoryLocation & tInvLocDest,
                                     const uint64 uAmount )

{
    if ( ptACDStack == NULL )
    {
        return;
    }



    ACDID idACDOwner = ACDInventoryGetUltimateContainer( *ptACDStack );

    InventorySplitStackMessage tMessage;
    tMessage.annItem = ANNFromACD( ptACDStack->id );
    tMessage.uAmount = uAmount;
    tMessage.tLocation.annOwner = ANNFromACD( tInvLocDest.idACDContainer );
    tMessage.tLocation.eSlot = tInvLocDest.eSlot;
    tMessage.tLocation.nSlotX = tInvLocDest.pt.x;
    tMessage.tLocation.nSlotY = tInvLocDest.pt.y;

    MessageSendToServer( CMSG_TRY_INVENTORY_SPLIT_STACK, &tMessage, sizeof( tMessage ), PlayerGetByACD( idACDOwner ) );

    PlayerGetCursorMgr().ResetCursorState( FALSE );
}



void CACDInventoryRequestStackTransfer( const ACDID idACDItemSrc,
                                        const ACDID idACDItemDest,
                                        const uint64 uAmount )

{
    InventoryStackTransferMessage tMessage;
    tMessage.annItemSrc = ANNFromACD( idACDItemSrc );
    tMessage.annItemDest = ANNFromACD( idACDItemDest );
    tMessage.uAmount = uAmount;
    MessageSendToServer( CMSG_TRY_INVENTORY_STACK_TRANSFER, &tMessage, sizeof( tMessage ), LocalPlayerGet() );

    PlayerGetCursorMgr().ResetCursorState( FALSE );
}



BOOL CACDInventoryGetUseMessagePrompt( const ActorCommonData & tACDUser,
                                       const ActorCommonData & tACDItem,
                                       const Target &,
                                       CRefString & sMsg )

{
    switch ( ItemGetUseType( tACDItem ) )
    {
    case ITEM_USE_POWER_SCROLL:
    case ITEM_USE_SCROLL:
        if ( tACDUser.AttributesGetInt( FastAttribKey( ATTRIBUTE_SCROLL_BUFF ) ) != 0 )
        {
            sMsg = StringListFetch( "General:ScrollUseWarning" );
            return TRUE;
        }
        break;
    }
    return FALSE;
}
