#include "UIItemButtonControl.h"

#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttributeStructs.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/Protobuf/Items.pb.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../Font/Font.h"
#include "../../Font/FormattedText.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../Prefetch.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../UIAttributes.h"

// Declarations of the functions this file calls whose headers are not adopted yet.
// GB_ITEMS comes from GameBalanceDefinition.h.

#include "../../../Common/Definitions/TreasureClassDefinition.h"







// X/Common/Definitions/RecipeDefinition.h
struct RecipeDefinition {  // size 0x48
    RequiredDefinitionHeader tHeader;  // 0x00
    ItemSpecifierData tSpecificItem;  // 0x0c
    uint32 dwPad;  // 0x44
};
static_assert(sizeof(RecipeDefinition) == 0x48, "RecipeDefinition");
class RecipeGroupDef;
extern RecipeGroupDef * gptGroupDefRecipe;
typedef ScopedDefinitionLockR<RecipeDefinition> ScopedRecipeDefinitionLockR;
template <> inline const RecipeDefinition * DefinitionLockR<RecipeDefinition>(SNO sno) {  // line 70
    return (const RecipeDefinition *)((GroupDefinition *)gptGroupDefRecipe)->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<RecipeDefinition>(const RecipeDefinition *) {}
// X/Common/Hyperlink.h
CRefString HyperlinkCreateFromSNO(const SNOName & snoname, LPCSTR szLinkName);
// X/Common/Definitions/GameBalanceDefinition.cpp:2444
GBID GBIDExists(LPCSTR szGBID, GameBalanceType eGBType);
// X/Common/Items.h







BOOL ItemIsSetItem(GBID gbid);
BOOL ItemIsLegendary(GBID gbid);
ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACDItem);
ItemQualityLevel ItemGetDisplayedQualityLevel(const D3::Items::Generator & tItem);
BOOL ItemIsUnidentified(const ActorCommonData & tACDItem);
extern "C" BOOL sub_710050e910(GBID gbidItem);
extern "C" BOOL sub_710050ea10(const ActorCommonData * ptACD);
extern "C" BOOL sub_710050ecb0(const ActorCommonData * ptACD);
ItemQualityLevel ItemDetermineQualityLevel(LPCSTR szDebugStr, GBHandle hGBItem, const GBID * ptGBIDAffixes, const int32 nAffixCount, const int32 nBonusRandomAffixes);
// X/Client/Definitions/TextureDefinition.h
void TextureGetFrameRectPixels(IRect2D & rectTexturePixels, const SNO snoTexture, const uint32 dwFrameIndex);
// X/Client/CActorInventory.h
typedef int32 ItemDrawType;
void ACDInventoryGetItemDrawInfo(const ActorCommonData & tACDItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType, const PlayerClass eClass, const BOOL fIsMale);
void ACDInventoryGetItemDrawInfo(const ActorCommonData & tACDItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType);
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType);
// X/Common/ActorInventory.h
ACDID ACDInventoryGetSquareItem(const ActorCommonData & tACD, const InventorySlotType eSlot, const IVector2D & tPoint);
// X/Client/ImageCollection.h
enum {
    IMAGE_COLLECTION_UI = 2,
};
BOOL ImageCollectionGetImageInfo(const int32 eCollection, LPCSTR szLabel, ImageTextureFrame & tImageTexFrame);
// X/Client/Sprite.h
void SpriteDrawDeferred(const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
void SpriteDrawDeferred(const ImageTextureFrame & tImageTexFrame, const ImageTextureFrame & tImageTexFrameMask, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const Rect2D * const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
// X/Client/UserInterface/UIUtils.h
void ConvertUICToPixels(const Rect2D & rectUIC, Rect2D & rectPixels);
// X/Common/Game.h
BOOL GameIsStartUp();
// X/Client/CGame.cpp
struct Player;
Player * LocalPlayerGet();
// X/Common/Items.h
BOOL ItemIsDrawnAsLargeSlotItem(const GBID gbidItem);
BOOL ItemIsUpgradeableJewel(const GBID gbidItem);
// X/Client/UserInterface/Interface/UIInventory.h
namespace UIInventory {
BOOL GetItemBkgrndImage(int32 eQuality, ImageTextureFrame & tImageTexFrame, int32 fEquipped, int32 fLarge, int32 unk_4, int32 fSeasonOnly, int32 fSanctified, int32 fPrimalized);
void DrawItemOverlay(const Rect2D & rectItemUIC, int32 eOverlay, float flAlpha, float & flZ, const Rect2D * prectClippingUIC);
void DrawLegendaryGemRank(int32 nRank, float & flZ, const Rect2D & rectItemUIC, const Rect2D * prectClippingUIC);
void DrawItemRarityIcon(GBID gbidItem, float flAlpha, float & flZ, SNO snoShader, const Rect2D & rectPixels, const Vector2D & vecOffsetPixels, const Rect2D * prectClippingPixels, BOOL fPrimal);
}  // namespace UIInventory

// The quality levels SetGBID forces; their enumerator names are not in the DWARF.
static const ItemQualityLevel scnIQLLegendary = 9;
static const ItemQualityLevel scnIQLSet = 11;

void ItemButtonCreationParams::sInit() {  // line 36
    fDrawBackground = TRUE;
    fDrawSockets = TRUE;
    fForceLargeBackground = TRUE;
    gbidItem = -1;
    fDrawStackCount = FALSE;
    nStackCount = 0;
}

ItemButtonCreationParams::ItemButtonCreationParams(const UIType eType, const UIID & id, const UIID & idParent)  // line 50
    : ButtonCreationParams(eType, id, idParent) {
    sInit();
}

void ItemButtonCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData) {  // line 74
    if (snoUI == SNO_NONE)
        return;

    ButtonCreationParams::Load(snoUI, pControlData);

    fDrawBackground = pControlData->GetBoolValue(ATT_DRAW_BACKGROUND);
    fDrawSockets = pControlData->GetBoolValue(ATT_DRAW_SOCKETS);
    fForceLargeBackground = pControlData->GetBoolValue(ATT_FORCE_LARGE_BACKGROUND);
    LPCSTR szItemName = pControlData->GetAttributeValue(ATT_ITEM_NAME);
    gbidItem = GBIDExists(szItemName, GB_ITEMS);
    fDrawStackCount = pControlData->GetBoolValue(ATT_ITEM_DRAW_STACK_COUNT);
    nStackCount = pControlData->GetInt32Value(ATT_ITEM_STACK_COUNT);
}

void UIItemButton::sInit(const ItemButtonCreationParams & tParams) {  // line 98
    m_eIQL = -1;
    m_dwItemButtonFlags = 0;
    m_eOverlay = ITEMOVERLAY_NONE;
    m_gbid = -1;
    m_idACDItem = ACDID_INVALID;
    m_nStackCount = tParams.nStackCount;
    unk_4e8 = 0;
    unk_4ec = 0;

    m_dwItemButtonFlags = tParams.fDrawBackground ? (m_dwItemButtonFlags & ~(1 << IBF_NO_BACKGROUND)) : (m_dwItemButtonFlags | (1 << IBF_NO_BACKGROUND));
    m_dwItemButtonFlags = tParams.fDrawSockets ? (m_dwItemButtonFlags & ~(1 << IBF_NO_SOCKETS)) : (m_dwItemButtonFlags | (1 << IBF_NO_SOCKETS));
    m_dwItemButtonFlags = tParams.fForceLargeBackground ? (m_dwItemButtonFlags | (1 << IBF_FORCE_LARGE_BACKGROUND)) : (m_dwItemButtonFlags & ~(1 << IBF_FORCE_LARGE_BACKGROUND));
    m_dwItemButtonFlags = tParams.fDrawStackCount ? (m_dwItemButtonFlags | (1 << IBF_DRAW_STACK_COUNT)) : (m_dwItemButtonFlags & ~(1 << IBF_DRAW_STACK_COUNT));

    if (tParams.gbidItem != -1) {
        SetGBID(tParams.gbidItem);
        m_dwItemButtonFlags |= (1 << IBF_DELAYED_TEXTURE_INIT);
    }
}

UIItemButton::UIItemButton(const ItemButtonCreationParams & tParams, const Rect2D * pRect)  // line 136
    : UIButton(tParams, pRect) {
    sInit(tParams);
}

void UIItemButton::SetACDID(const ACDID idACD) {  // line 146
    m_idACDItem = idACD;

    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD != NULL && ptACD->IsItem()) {
        SetGBID(ptACD->hGB.gbid);
        SetQualityLevel(ItemGetDisplayedQualityLevel(*ptACD));
        unk_4e8 = sub_710050ea10(ptACD);
        unk_4ec = sub_710050ecb0(ptACD);
        UIItemOverlay eOverlay = ItemIsUnidentified(*ptACD) ? ITEMOVERLAY_UNIDENTIFIED : ITEMOVERLAY_NONE;
        SetOverlay(eOverlay);
    }
}

void UIItemButton::SetGBID(const GBID gbid) {  // line 166
    m_gbid = gbid;

    if (gbid != -1) {
        if (ItemIsSetItem(gbid))
            m_eIQL = scnIQLSet;
        else if (ItemIsLegendary(gbid))
            m_eIQL = scnIQLLegendary;
    }
}

void UIItemButton::SetQualityLevel(const ItemQualityLevel eIQL) {  // line 187
    m_eIQL = eIQL;
}

void UIItemButton::GetItemRect(const ImageTextureFrame & tImageTexFrame, const Rect2D & rectSlot, Rect2D & rectItem) {  // line 196
    IRect2D rect;
    TextureGetFrameRectPixels(rect, tImageTexFrame.snoTexture, tImageTexFrame.nFrame);

    Vector2D vecTexDim((float)rect.Width(), (float)rect.Height());
    Vector2D vecSlotDim(rectSlot.right - rectSlot.left, rectSlot.bottom - rectSlot.top);
    Vector2D vecTexSizeToUse;
    vecTexSizeToUse = vecTexDim;

    if (vecTexDim.x > vecSlotDim.x || vecTexDim.y > vecSlotDim.y) {
        float flSlotAspectRatio = vecSlotDim.x / vecSlotDim.y;
        float flTexAspectRatio = vecTexDim.x / vecTexDim.y;
        if (flTexAspectRatio > flSlotAspectRatio) {
            vecTexSizeToUse.x = vecSlotDim.x;
            vecTexSizeToUse.y = vecSlotDim.x / flTexAspectRatio;
        } else {
            vecTexSizeToUse.x = flTexAspectRatio * vecSlotDim.y;
            vecTexSizeToUse.y = vecSlotDim.y;
        }
    }

    rectItem.left = rectSlot.left + (vecSlotDim.x - vecTexSizeToUse.x) * 0.5f;
    rectItem.right = vecTexSizeToUse.x + rectItem.left;
    rectItem.top = rectSlot.top + (vecSlotDim.y - vecTexSizeToUse.y) * 0.5f;
    rectItem.bottom = vecTexSizeToUse.y + rectItem.top;
}

void UIItemButton::DrawItemSockets(const ActorCommonData * ptACD, const Rect2D & rectItemImage, const BOOL fEquipped, float & flZ) {  // line 237
    if (m_dwItemButtonFlags & (1 << IBF_NO_SOCKETS))
        return;

    int32 nNumSockets = ptACD->AttributesGetInt(ATTRIBUTE_SOCKETS);

    ImageTextureFrame tImageTexFrameSocket;
    ImageCollectionGetImageInfo(2, "GemSocket32", tImageTexFrameSocket);

    Vector2D vecSocketDim = fEquipped ? Vector2D(32.0f, 32.0f) : Vector2D(28.0f, 28.0f);

    Vector2D vecSocketCenter(rectItemImage.left + rectItemImage.Width() * 0.5f, rectItemImage.top + rectItemImage.Height() * 0.5f);

    float flSocketLeft = vecSocketCenter.x - vecSocketDim.x * 0.5f;
    float flSocketTop = vecSocketCenter.y - vecSocketDim.y * nNumSockets * 0.5f;

    const Rect2D & rectClippingUIC = GetCascadedClippingRect();
    Rect2D rectClippingPixels;
    ConvertUICToPixels(rectClippingUIC, rectClippingPixels);

    for (int32 nSocket = 0; nSocket < nNumSockets; nSocket++) {
        Rect2D rectSocket;
        rectSocket.left = flSocketLeft;
        rectSocket.top = flSocketTop + vecSocketDim.y * nSocket;
        rectSocket.right = vecSocketDim.x + rectSocket.left;
        rectSocket.bottom = vecSocketDim.y + rectSocket.top;

        Rect2D rectSocketScreen;
        ConvertUICToPixels(rectSocket, rectSocketScreen);

        SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT);
        SpriteDrawDeferred(tImageTexFrameSocket, snoShader, rectSocketScreen, crgbaWhite, crgbaBlack, flZ, &rectClippingPixels, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
        flZ -= UIDC_CONTROL_Z_INCREMENT;

        IVector2D tPoint(0, nSocket);
        ACDID idACDSocketedItem = ACDInventoryGetSquareItem(*ptACD, SLOT_ITEM_SOCKETS, tPoint);
        ActorCommonData * ptACDSocketedItem = ACDTryToGet(idACDSocketedItem);
        if (ptACDSocketedItem != NULL) {
            ImageTextureFrame tImageTexFrame = cgtImageTextureFrameNull;
            if (GameIsStartUp())
                ACDInventoryGetItemDrawInfo(*ptACDSocketedItem, tImageTexFrame, 1, 1, TRUE);
            else
                ACDInventoryGetItemDrawInfo(*ptACDSocketedItem, tImageTexFrame, 1);

            SpriteDrawDeferred(tImageTexFrame, cgtImageTextureFrameNull, snoShader, rectSocketScreen, crgbaWhite, crgbaBlack, flZ, 1.0f, &rectClippingPixels, RENDERLAYER_INVALID, FALSE);
            flZ -= UIDC_CONTROL_Z_INCREMENT;
        }
    }
}

static const int32 FONT_ALPHA_TINY_SIZE = 10;  // Font.h:85




void UIItemButton::Draw(const uint8 bLayerAlpha, float & flZSlice) {  // line 317
    Rect2D rectWindowUIC;
    Rect2D rectWindowPixels;
    Rect2D rectDrawClippingPixels;

    GetWindowRect(rectWindowUIC);
    const Rect2D & rectClippingUIC = GetCascadedClippingRect();

    uint8 bAlphaCascaded = MultiplyAlpha(bLayerAlpha, GetWindowColor().a);
    if (bAlphaCascaded == 0)
        return;

    if ((m_dwItemButtonFlags & (1 << IBF_DELAYED_TEXTURE_INIT)) &&
        LocalPlayerGet()) {
        ImageTextureFrame tImageTexFrame;
        ACDInventoryGetItemDrawInfoWithGBID(m_gbid, tImageTexFrame, 0);
        SetAllTextureFrames(tImageTexFrame);
        m_dwItemButtonFlags &= ~(1 << IBF_DELAYED_TEXTURE_INIT);
    }

    ConvertUICToPixels(rectWindowUIC, rectWindowPixels);
    ConvertUICToPixels(rectClippingUIC, rectDrawClippingPixels);

    RGBAColor rgbaColor = GetWindowColor();
    rgbaColor.a = bAlphaCascaded;

    SNO snoShaderToUse = GetShader();
    if (snoShaderToUse == SNO_NONE)
        snoShaderToUse = (bAlphaCascaded != 255) ? GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT) : SNO_NONE;

    BOOL fLargeItem = (m_dwItemButtonFlags & (1 << IBF_FORCE_LARGE_BACKGROUND)) || ItemIsDrawnAsLargeSlotItem(m_gbid);

    if (m_eIQL != -1 && !(m_dwItemButtonFlags & (1 << IBF_NO_BACKGROUND))) {
        BOOL fEquipped = (m_dwItemButtonFlags & (1 << IBF_EQUIPPED_BIT)) != 0;
        ImageTextureFrame tImageTexFrameQualityBkgrnd;
        BOOL fSeasonOnly = sub_710050e910(m_gbid);
        BOOL fFoundImage = UIInventory::GetItemBkgrndImage(m_eIQL, tImageTexFrameQualityBkgrnd, fEquipped, fLargeItem, FALSE, fSeasonOnly, unk_4e8, unk_4ec);
        if (fFoundImage) {
            float flLayerZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
            flZSlice += 1.0f;
            SpriteDrawDeferred(tImageTexFrameQualityBkgrnd, cgtImageTextureFrameNull, snoShaderToUse, rectWindowPixels, rgbaColor, crgbaBlack, flLayerZ, 1.0f, &rectDrawClippingPixels, RENDERLAYER_INVALID, FALSE);
        }
    }

    ImageTextureFrame & tImageTexFrame = m_tTextureFrame[BS_NORMAL];
    if (tImageTexFrame.snoTexture != SNO_NONE) {
        if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrame.snoTexture), SNOFilesAsyncPriority_High))
            return;

        Rect2D rectItemPixels;
        Rect2D rectItemUIC;
        GetItemRect(tImageTexFrame, rectWindowUIC, rectItemUIC);
        ConvertUICToPixels(rectItemUIC, rectItemPixels);
        {
            float flLayerZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
            flZSlice += 1.0f;
            SpriteDrawDeferred(tImageTexFrame, cgtImageTextureFrameNull, snoShaderToUse, rectItemPixels, rgbaColor, crgbaBlack, flLayerZ, 1.0f, &rectDrawClippingPixels, RENDERLAYER_INVALID, FALSE);
        }

        ActorCommonData * ptACDItem = ACDTryToGet(m_idACDItem);
        if (ptACDItem != NULL && ptACDItem->IsItem()) {
            float flLayerZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
            flZSlice += 1.0f;
            DrawItemSockets(ptACDItem, rectWindowUIC, (m_dwItemButtonFlags & (1 << IBF_EQUIPPED_BIT)) != 0, flLayerZ);
        }

        if (m_eOverlay != ITEMOVERLAY_NONE) {
            float flLayerZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
            flZSlice += 1.0f;
            UIInventory::DrawItemOverlay(rectItemUIC, m_eOverlay, (float)bAlphaCascaded / 255.0f, flLayerZ, &rectClippingUIC);
        }
    }

    DrawText(bLayerAlpha, flZSlice);

    if (m_dwItemButtonFlags & (1 << IBF_DRAW_HIGHLIGHT_BIT)) {
        RGBAColor rgbaColorLocal = crgbaWhite;
        rgbaColorLocal.a = Math::Min(rgbaColorLocal.a, bAlphaCascaded);
        CRefString sLabel(fLargeItem ? "ItemSelectLarge" : "ItemSelectSmall");
        ImageTextureFrame tImageTexFrameLocal = cgtImageTextureFrameNull;
        if (!ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, sLabel, tImageTexFrameLocal))
            return;

        float flLayerZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
        flZSlice += 1.0f;
        SpriteDrawDeferred(tImageTexFrameLocal, snoShaderToUse, rectWindowPixels, rgbaColorLocal, crgbaBlack, flLayerZ, &rectDrawClippingPixels, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
    }

    if (m_dwItemButtonFlags & (1 << IBF_DRAW_STACK_COUNT)) {
        uint64 uStackSize = GetStackCount();
        if (uStackSize != 0) {
            float flLayerZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
            flZSlice += 1.0f;

            Rect2D rectUIC;
            GetWindowRect(rectUIC);

            CHAR uzTemp[32];
            Blizzard::String::Format(uzTemp, sizeof(uzTemp), "%llu", uStackSize);

            TextRenderProperties tProps(GlobalSNOGet(GLOBALSNO_FONT_SERIF), FONT_ALPHA_TINY_SIZE, 1.0f, 1.0f);
            Vector2D vecTextSizeUIC;
            FontStringGetRenderedSizeUIC(tProps, FormattedText(uzTemp, PARSE_DEFAULT, FALSE), vecTextSizeUIC);

            Vector2D vecUIC;
            vecUIC.Set(rectUIC.right - (vecTextSizeUIC.x + 4.0f), rectUIC.bottom - (vecTextSizeUIC.y + 4.0f));
            FontStringDrawUIC(tProps, FormattedText(uzTemp, PARSE_DEFAULT, FALSE), vecUIC, crgbaWhite, &crgbaBlack, flLayerZ, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
        }
    }

    if (ItemIsUpgradeableJewel(m_gbid) && (m_dwItemButtonFlags & (1 << IBF_DRAW_LEGENDARY_GEM_RANK))) {
        ActorCommonData * ptACDItem = ACDTryToGet(m_idACDItem);
        if (ptACDItem != NULL) {
            float flZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
            flZSlice += 1.0f;
            int32 nRank = ptACDItem->AttributesGetInt(ATTRIBUTE_JEWEL_RANK);
            UIInventory::DrawLegendaryGemRank(nRank, flZ, rectWindowUIC, &rectClippingUIC);
        }
    }

    if (Prefs::GetGameplayPrefs().GetItemRarityIcons() && !(m_dwItemButtonFlags & (1 << IBF_NO_BACKGROUND))) {
        Vector2D vecOffsetPixels(-3.0f, -3.0f);
        float flAlpha = (float)Math::Min(rgbaColor.a, bAlphaCascaded) / 255.0f;
        float flZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
        flZSlice += 1.0f;
        if (m_eIQL != -1)
            UIInventory::DrawItemRarityIcon(m_gbid, flAlpha, flZ, snoShaderToUse, rectWindowPixels, vecOffsetPixels, &rectDrawClippingPixels, unk_4ec);
    }
}

void UIItemButton::SetFromRecipe(const SNO snoRecipe) {  // line 531
    ScopedRecipeDefinitionLockR pdefRecipe(snoRecipe);
    const ItemSpecifierData & tSpecificItem = pdefRecipe->tSpecificItem;

    SetGBID(tSpecificItem.gbidItem);

    GBHandle hgbItem;
    hgbItem.eType = GB_ITEMS;
    hgbItem.gbid = tSpecificItem.gbidItem;

    ScopedItemDefinitionLockR pdefItem(hgbItem.gbid);
    ItemQualityLevel eIQL = pdefItem ? pdefItem->eDisplayedQualityLevel : -1;
    if (eIQL == -1) {
        if (ItemIsSetItem(hgbItem.gbid)) {
            eIQL = scnIQLSet;
        } else {
            eIQL = ItemDetermineQualityLevel(HyperlinkCreateFromSNO(SNOName(RECIPE_SNO, snoRecipe), NULL).str(), hgbItem,
                                             tSpecificItem.gbidAffixes, tSpecificItem.nNumAffixes, tSpecificItem.nAdditionalRandomAffixes);
        }
    }

    SetQualityLevel(eIQL);

    ImageTextureFrame tImageTexFrame;
    ACDInventoryGetItemDrawInfoWithGBID(tSpecificItem.gbidItem, tImageTexFrame, 0);
    SetAllTextureFrames(tImageTexFrame);
}

void UIItemButton::SetFromItemGenerator(const D3::Items::Generator & tItem) {  // line 569
    GBID gbidItem = tItem.gb_handle().gbid();
    if (gbidItem != -1) {
        SetGBID(gbidItem);
        SetQualityLevel(ItemGetDisplayedQualityLevel(tItem));
        SetStackCount(tItem.stack_size());
        ImageTextureFrame tImageTexFrame;
        ACDInventoryGetItemDrawInfoWithGBID(gbidItem, tImageTexFrame, 0);
        SetAllTextureFrames(tImageTexFrame);
    }
}

extern "C" void sub_710039d4d0(UIItemButton * button, int32 value)
{
    button->unk_4e8 = value;
}

extern "C" void sub_710039d4e0(UIItemButton * button, int32 value)
{
    button->unk_4ec = value;
}
