// Original file: X/Client/UserInterface/Interface/UIInventoryCommon.cpp
#include "../../CPlayerInput.h"
#include "../../Definitions/TextureDefinition.h"
#include "../../Font/Font.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Input/Input.h"
#include "../../Sound/Sound.h"
#include "../Controls/UITextControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../Font/FormattedText.h"
#include "../../../../BlizzardCore/Include/Blizzard/String.h"
// Additional declarations retain their original DWARF types and names.
CursorMgr & PlayerGetCursorMgr(); // CPlayerInput.cpp:6603
BOOL ItemUsesQuiver(const ActorCommonData & tACDItem); // Items.cpp:3467
BOOL ItemIsUniqueEquipped(const ActorCommonData & tACDItem); // Items.cpp:5219
GameError ACDInventoryCanMove(const ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest, ACDID * pidACDItemInSlot); // ActorInventory.cpp:10573
void CACDInventoryRequestMove(ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest, BOOL bIgnoreSoulbound, Player * ptPlayer); // CActorInventory.cpp:484
float UIGetBaseResolutionScaleFrac(); // UIUtils.cpp:419
void TextureGetFrameRectPixels(IRect2D & rectTexturePixels, const TextureDefinition * pdefTexture, const uint32 dwFrameIndex); // TextureDefinition.cpp:2728
InventorySlotType ACDInventoryGetOppositeTwoHandedSlot(InventorySlotType eSlot);  // ActorInventory.cpp:532
BOOL ACDInventoryFindNonFullStackInSlot(const ActorCommonData & tACDItem, const ActorCommonData & tACDContainer, const InventorySlotType eCheckSlot, InventoryLocation & tNewLocation, const int32 * pnSlotYMinMax);  // ActorInventory.cpp:2289
BOOL ACDInventoryCanHold(const ActorCommonData & tACD, const ActorCommonData & tACDItem, const InventorySlotType eSlot, IVector2D & tWhereCanHold, const int32 * pnSlotYMinMax);  // ActorInventory.cpp:869
void ACDInventoryLocationSet(InventoryLocation & tInvLoc, const ACDID idACDContainer, const InventorySlotType eSlot, const int32 nSlotX, const int32 nSlotY);  // ActorInventory.cpp:9743
void ACDInventoryWhereCanThisGoHireling(const GBID gbidItem, int32 & nNumSlots, InventorySlotType * eValidSlots);  // ActorInventory.cpp:11712
BOOL ItemIsTwoHanded(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:3403
static FastAttribKey sgtCurrentKey;
BOOL ItemMeetsOwnerClassRestrictions(const GBID gbidItem, const PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight);  // Items.cpp:1883
PlayerClass PlayerGetPlayerClass(const ACDID idACDPlayer);  // X/Common/Player.cpp
BOOL InputAltKeyIsDown(InputContext eContext);  // X/Client/Input/Input.cpp:799
void CACDInventoryRequestQuickMove(ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest, const int32 * pnSlotYMinMax, const Player * ptPlayer);  // X/Client/CActorInventory.cpp:519
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp:45
namespace UIGame { void DisplayError(const GameErrorData & tError, BOOL fDisplayBnetError, const Player * ptPlayer); }  // Interface/UIGame.cpp

namespace UIInventory {
void PlayItemInventorySound(const ActorCommonData & tACDItem);
void QuickMoveToEmptySlot(const ActorCommonData & tACD, ActorCommonData & tACDItem, int32 & nNumSlots, InventorySlotType * eValidSlots, BOOL bUseOffhand, Player * ptPlayer);

BOOL SlotIsEmpty(const ActorCommonData & tACD, const InventorySlotType eSlot, const IVector2D & pt);
// UIInventoryCommon.h:22-29; owner-local declaration avoids changing shared headers.
struct HeroModifierArray {
    float flBase;
    float flTotal;
    float flWeapons;
    float flArmor;
    float flJewelry;
};
}
// Sprite.cpp:2538, original vector-position overload.
void SpriteDrawDeferred(const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Vector2D & pCoords, const float flScaleX, const float flScaleY, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
SNO ActorInventory_GetItemUseSound(const ActorCommonData * ptACDItem); // ActorInventory.cpp:12348

static void sGetStatsFloatItem(ACDID idACDSquare, InventorySlotType, void * pCallBackData)
{



    float * pflValue = static_cast<float *>(pCallBackData);

    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem)
    {
        *pflValue += ptACDItem->AttributesGetFloat(sgtCurrentKey);
    }
}









static float sGetInventoryStatsFloatForKey(const ActorCommonData & tACD, const int32 * pnSlots, FastAttribKey tKey)
{



    float flReturn = 0.0f;
    int32 nPosition = 0;
    sgtCurrentKey = tKey;
    IVector2D tSize;
    while (pnSlots[nPosition] != SLOT_NONE)
    {

        InventorySlotType eSlot = pnSlots[nPosition++];
        ACDInventoryIterateItems(tACD, eSlot, 0, sGetStatsFloatItem, &flReturn);
    }



    return flReturn;
}




static float sGetInventoryStatsFloat(const ActorCommonData & tACD, const int32 * pnSlots, FastAttribKey tKey, const BOOL fIncludeAllStatsBonus)
{




    float flReturn = sGetInventoryStatsFloatForKey(tACD, pnSlots, tKey);
    if (fIncludeAllStatsBonus)
    {
        flReturn += sGetInventoryStatsFloatForKey(tACD, pnSlots, FastAttribKey(ATTRIBUTE_STATS_ALL_BONUS));
    }


    return flReturn;
}



namespace UIInventory {
void GetHeroModifiers(ActorCommonData & tACD, FastAttribKey tKey, FastAttribKey tKeyTotal, HeroModifierArray & tDestArray, const BOOL fIncludeAllStatsBonus)
{








    // Array values follow the original unrolled slot calls; SLOT_NONE terminators follow the helper loop and DWARF array lengths.
    const int32 snWeaponsList[3] = { SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_RIGHT_HAND, SLOT_NONE };







    const int32 snArmorList[8] = { SLOT_PLAYER_HEAD, SLOT_PLAYER_TORSO, SLOT_PLAYER_HANDS, SLOT_PLAYER_FEET, SLOT_PLAYER_WAIST, SLOT_PLAYER_SHOULDERS, SLOT_PLAYER_LEGS, SLOT_NONE };



    const int32 snJewelryList[4] = { SLOT_PLAYER_NECK, SLOT_PLAYER_LEFT_FINGER, SLOT_PLAYER_RIGHT_FINGER, SLOT_NONE };



    tDestArray.flTotal = tACD.AttributesGetFloat(tKeyTotal);


    tDestArray.flWeapons = sGetInventoryStatsFloat(tACD, snWeaponsList, tKey, fIncludeAllStatsBonus);
    tDestArray.flArmor = sGetInventoryStatsFloat(tACD, snArmorList, tKey, fIncludeAllStatsBonus);
    tDestArray.flJewelry = sGetInventoryStatsFloat(tACD, snJewelryList, tKey, fIncludeAllStatsBonus);
    tDestArray.flBase = tDestArray.flTotal - tDestArray.flWeapons - tDestArray.flArmor - tDestArray.flJewelry;
}







BOOL IsMagicColor(const ItemQualityLevel eItemQualityLevel)

{
    return (eItemQualityLevel >= IQL_MAGIC_AFFIX_START && eItemQualityLevel <= IQL_MAGIC_AFFIX_END) ||
           eItemQualityLevel == IQL_SPECIAL;
}



BOOL IsRareColor(const ItemQualityLevel eItemQualityLevel)

{
    return eItemQualityLevel >= IQL_RARE_AFFIX_START && eItemQualityLevel <= IQL_RARE_AFFIX_END;
}



BOOL IsLegendaryColor(const ItemQualityLevel eItemQualityLevel)

{
    return eItemQualityLevel == IQL_LEGENDARY;
}



void PlayItemInventorySound(const ActorCommonData & tACDItem)

{

    SNO snoInventorySound = ActorInventory_GetItemUseSound(&tACDItem);


    if (snoInventorySound != SNO_NONE)
    {
        SoundManager::PlaySound(snoInventorySound, cgtPSODefaults);
    }
}


















void PlayItemPickupSound()

{
    SNO snoPickupSound = GlobalSNOGet(GLOBALSNO_SOUND_PICKUP);


    if (snoPickupSound != SNO_NONE)
    {
        SoundManager::PlaySound(snoPickupSound, cgtPSODefaults);
    }
}
}  // namespace UIInventory


static GameErrorData sFindOpenSlot(const ActorCommonData & tACD, const ActorCommonData & tACDItem, int32 & nNumSlots, InventorySlotType * eValidSlots, InventoryLocation & tInvLocDest, BOOL bUseOffhand)
{






    GameErrorData eErrorData;


    InventorySlotType eSlotToUse = SLOT_NONE;

    if (bUseOffhand && nNumSlots != 1)
    {



        eSlotToUse = eValidSlots[1];
    }
    else if (ItemIsUniqueEquipped(tACDItem) &&
             nNumSlots == 1)
    {
        eSlotToUse = eValidSlots[0];
    }
    else
    {


        for (int32 i = 0; i < nNumSlots; ++i)
        {
            if (UIInventory::SlotIsEmpty(tACD, eValidSlots[i], cgptNull))
            {
                eSlotToUse = eValidSlots[i];
                break;
            }
        }
    }

    if (eSlotToUse == SLOT_NONE)
    {

        eSlotToUse = eValidSlots[0];
    }

    IVector2D vecPt(0, 0);
    ACDInventoryLocationSet(tInvLocDest, tACD.id, eSlotToUse, vecPt.x, vecPt.y);
    eErrorData.eError = ACDInventoryCanMove(tACDItem, tInvLocDest, NULL);

    eErrorData.dwVal = tInvLocDest.idACDContainer;

    return eErrorData;
}



namespace UIInventory {
void QuickMoveToEmptySlot(const ActorCommonData & tACD, ActorCommonData & tACDItem, int32 & nNumSlots, InventorySlotType * eValidSlots, BOOL bUseOffhand, Player * ptPlayer)






{
    if (nNumSlots < 1) return;

    InventoryLocation tInvLocDest;
    GameErrorData eErrorData = sFindOpenSlot(tACD, tACDItem, nNumSlots, eValidSlots, tInvLocDest, bUseOffhand);
    if (eErrorData.eError != GAMEERROR_NONE)
    {
        UIGame::DisplayError(eErrorData, FALSE, NULL);
        return;
    }
        // Expanded original InventoryLocation::operator== (ActorCommonData.h:92), absent from adopted header.
    const InventoryLocation * ptInvLocItem = tACDItem.TryToGetInventoryLocation();
    if (ptInvLocItem)
    {
        if (ptInvLocItem->idACDContainer == tInvLocDest.idACDContainer && ptInvLocItem->eSlot == tInvLocDest.eSlot && ptInvLocItem->pt == tInvLocDest.pt)
        {
            PlayerGetCursorMgr().ResetCursorState(TRUE);
            return;
        }




        if (ItemIsTwoHanded(tACDItem))
        {
            if ((tInvLocDest.eSlot == SLOT_PLAYER_RIGHT_HAND && ptInvLocItem->eSlot == SLOT_PLAYER_LEFT_HAND) ||
                (tInvLocDest.eSlot == SLOT_HIRELING_RIGHT_HAND && ptInvLocItem->eSlot == SLOT_HIRELING_LEFT_HAND))
            {
                PlayerGetCursorMgr().ResetCursorState(TRUE);
                return;
            }
        }
    }

    CACDInventoryRequestMove(tACDItem, tInvLocDest, FALSE, ptPlayer);
    PlayItemInventorySound(tACDItem);
}
}




static BOOL sIsTwoHandedSlot(InventorySlotType eSlot)

{
    return eSlot == 3 || eSlot == 4 || eSlot == 20 || eSlot == 21;
}

namespace UIInventory {











BOOL SlotIsEmpty(const ActorCommonData & tACD,
                 const InventorySlotType eSlot,
                 const IVector2D & pt)

{
    ACDID idACDItem = ACDInventoryGetSquareItem(tACD, eSlot, pt);
    if (idACDItem != ACDID_INVALID)
    {
        return FALSE;
    }


    if (sIsTwoHandedSlot(eSlot))
    {
        InventorySlotType eOppositeSlot = ACDInventoryGetOppositeTwoHandedSlot(eSlot);
        ACDID idACDOppositeItem = ACDInventoryGetSquareItem(tACD, eOppositeSlot, cgptNull);
        const ActorCommonData * ptACDOppositeItem = ACDTryToGet(idACDOppositeItem);
        if (ptACDOppositeItem &&
            ItemIsTwoHanded(*ptACDOppositeItem))
        {
            PlayerClass ePlayerClass = PlayerGetPlayerClass(tACD.id);
            if (ItemUsesQuiver(*ptACDOppositeItem) &&
                ItemMeetsOwnerClassRestrictions(GlobalGBIDGet(GLOBALGBID_ITEM_QUIVER), ePlayerClass, FALSE))
            {
                return TRUE;
            }

            return FALSE;
        }
    }





    return TRUE;
}



uint32 TryIdentifyWithCast(const ACDID idACDItem,
                           const ActorCommonData & tACDPlayer,
                           const Player * ptPlayer)

{
    uint32 uFailureReason = 0x200;

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem)
    {

        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(idACDItem);
        MessageSendToServer(CMSG_TRY_IDENTIFY_WITH_CAST, &tMessage, sizeof(tMessage), ptPlayer);
        uFailureReason = 0;
    }





















    return uFailureReason;
}




void TryQuickMoveToLocation(ActorCommonData & tACDOwner,
                            ActorCommonData & tACDItem,
                            const InventorySlotType eSlot)

{
    Player * ptPlayer = PlayerGetByACD(tACDOwner.id);

    InventoryLocation tInvLoc;
    if (!ACDInventoryFindNonFullStackInSlot(tACDItem, tACDOwner, eSlot, tInvLoc, NULL))
    {





        IVector2D tPoint(0, 0);
        if (ACDInventoryCanHold(tACDOwner, tACDItem, eSlot, tPoint, NULL))
        {
            ACDInventoryLocationSet(tInvLoc, tACDOwner.id, eSlot, tPoint.x, tPoint.y);
        }
        else
        {
            UIGame::DisplayError(GameErrorData(GAMEERROR_IARESULT_NOT_ENOUGH_ROOM), FALSE, NULL); return;
        }
    }
    CACDInventoryRequestQuickMove(tACDItem, tInvLoc, NULL, ptPlayer); PlayItemInventorySound(tACDItem);
}





void UpdateGoldAmount(const UIID & idText,
                      const ACDID idPlayerACD)

{
    if (!UI_TestID(idText))
        return;
    ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayerACD);
    if (ptACDPlayer == NULL)
        return;
    CurrencyType eCurrencyType = CURRENCY_GOLD;
    uint64 uGold = ptACDPlayer->GetCurrencyAmount(eCurrencyType);
    CRefString usAmount = UIText_ValueToGameCurrency(uGold, eCurrencyType);
    UI_GetTextFromID(idText).SetText(usAmount, FALSE);


}




void HandleQuickMoveToHireling(ActorCommonData & tACDHireling,
                               ActorCommonData & tACDItem)

{


    int32 nNumSlots = 0;
    InventorySlotType eValidSlots[4];
    ACDInventoryWhereCanThisGoHireling(tACDItem.hGB.gbid, nNumSlots, eValidSlots);

    Player * ptPlayer = ACDGetControllingPlayer(tACDHireling.id);

    QuickMoveToEmptySlot(tACDHireling, tACDItem, nNumSlots, eValidSlots, InputAltKeyIsDown(INPUTCONTEXT_DEFAULT), ptPlayer);
}



CRefString GetDPSString(float flDPS)

{
    int32 nSigDigits = 2;
    if (flDPS >= 10000.0f)
    {
        nSigDigits = 0;
    }
    else if (flDPS >= 1000.0f)
    {
        nSigDigits = 1;
    }

    CRefString sDPS = UnicodeText::FormatNumber(flDPS, TRUE, FNS_NORMAL, nSigDigits, nSigDigits, XLOCALE_INVALID);
    return sDPS;
}


void DrawItemRarityIcon(ItemQualityLevel eIQL, float flAlpha, float & flZ, SNO snoShader, const Rect2D & rectDestPixels, const Vector2D & vecOffsetPixels, const Rect2D * prectDrawClippingPixels)
{







    float flBaseResScale = UIGetBaseResolutionScaleFrac();
    float flIconScale = flBaseResScale * 0.4f;
    Vector2D vecScaledOffsetPixels = vecOffsetPixels * flBaseResScale;

    SNO snoIcon = SNO_NONE;
    if (eIQL == IQL_LEGENDARY)
    {
        snoIcon = GlobalSNOGet(GLOBALSNO_TEXTURE_MARKER_LEGENDARY_PING);
    }
    else if (eIQL == IQL_DISPLAY_AS_SET_ITEM)
    {
        snoIcon = GlobalSNOGet(GLOBALSNO_TEXTURE_MARKER_SET_ITEM_PING);
    }


    SNO snoBkgnd = GlobalSNOGet(GLOBALSNO_TEXTURE_COLORBLIND_ICON_BACKGROUND);

    if (snoIcon != SNO_NONE && snoBkgnd != SNO_NONE)
    {
        ImageTextureFrame tImageTexFrame(snoBkgnd, 0);
        ScopedTextureDefinitionLockR pdefTexFrame(snoBkgnd);
        IRect2D rectTexFrame;
        TextureGetFrameRectPixels(rectTexFrame, pdefTexFrame, 0);


        Vector2D vecIconPixels(rectDestPixels.right - flIconScale * rectTexFrame.Width() - vecScaledOffsetPixels.x, rectDestPixels.bottom - flIconScale * rectTexFrame.Height() - vecScaledOffsetPixels.y);
        RGBAColor rgbaColor(255, 255, 255, uint8(flAlpha * 255.0f));
        SpriteDrawDeferred(tImageTexFrame, snoShader, vecIconPixels, flIconScale, flIconScale, rgbaColor, crgbaBlack, flZ, prectDrawClippingPixels, RENDERLAYER_INVALID, FALSE);

        flZ -= UIDC_CONTROL_Z_INCREMENT;
    }
    if (snoIcon != SNO_NONE)
    {

        ImageTextureFrame tImageTexFrame(snoIcon, 0);
        ScopedTextureDefinitionLockR pdefTexFrame(snoIcon);
        IRect2D rectTexFrame;
        TextureGetFrameRectPixels(rectTexFrame, pdefTexFrame, 0);


        Vector2D vecIconPixels(rectDestPixels.right - flIconScale * rectTexFrame.Width() - vecScaledOffsetPixels.x, rectDestPixels.bottom - flIconScale * rectTexFrame.Height() - vecScaledOffsetPixels.y);
        RGBAColor rgbaColor(255, 255, 255, uint8(flAlpha * 255.0f));
        SpriteDrawDeferred(tImageTexFrame, snoShader, vecIconPixels, flIconScale, flIconScale, rgbaColor, crgbaBlack, flZ, prectDrawClippingPixels, RENDERLAYER_INVALID, FALSE);

        flZ -= UIDC_CONTROL_Z_INCREMENT;
    }
}



void DrawLegendaryGemRank(int32 nRank, float & flZ, const Rect2D & rectWindowUIC, const Rect2D * prectClipUIC)
{
    if (nRank == 0) return;









    CHAR uzTemp[32];
    Blizzard::String::Format(uzTemp, sizeof(uzTemp), "%d", nRank);

    TextRenderProperties tProps(GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF), 16, 1.0f, 1.0f);
    Vector2D vecTextSizeUIC;
    FontStringGetRenderedSizeUIC(tProps, FormattedText(uzTemp, PARSE_DEFAULT, FALSE), vecTextSizeUIC);

    Vector2D vecUIC;
    vecUIC.Set(rectWindowUIC.left + 5.0f, rectWindowUIC.top + 4.0f);

    FontStringDrawUIC(tProps, FormattedText(uzTemp, PARSE_DEFAULT, FALSE), vecUIC, RGBAColor(255, 215, 158, 255), &crgbaBlack, flZ, TRUE, prectClipUIC, RENDERLAYER_INVALID, FALSE);
    flZ -= UIDC_CONTROL_Z_INCREMENT;
}
}
