#include "UIEquipLegendaryPowersCommon.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../../../Contrib/Contrib/blz/include/blz/iter_range.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/UserInterface/FontDefinition.h"
// Declared here because their headers are not adopted yet; each comment names the defining file.
// X/Client/CActorInventory.cpp, X/Client/UserInterface/UI.cpp
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const int32 eDrawType);
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const int32 eDrawType, const PlayerClass eClass, const BOOL fIsMale);
UIWindow & UI_GetWindowFromID(const UIID & id);
// X/Common/Items.cpp
void ItemsGetExtractableLegendaryPowers(GBIDArray & arExtractableLegendaryPowers);
bool ItemCompareExtractableLegendaryPowers(GBID gbidPower1, GBID gbidPower2);  SNO ItemGetLegendaryPower(GBID gbidItem);  float ItemGetLegendaryPowerValue(GBID gbidItem);
BOOL ItemIsDrawnAsLargeSlotItem(const GBID gbidItem);
BOOL ItemResolveValueTagForItemPassivePowerDescription(const ActorCommonData *, AttribGroupID, AttribGroupID, LPCSTR, float &, float *, CRefString &, SNO, int32, void *, void *, FontSpecifier &);
typedef BOOL (*TagResolutionCallback)(const ActorCommonData *, AttribGroupID, AttribGroupID, LPCSTR, float &, float *, CRefString &, SNO, int32, void *, void *, FontSpecifier &);
// X/Common/UserInterface/UIToolTips.cpp, UITextControl.cpp, X/Common/Attributes/FastAttribute.cpp
CRefString UIToolTips_ProcessTip(const ActorCommonData * ptACDItem, const AttribGroupID idFastAttribMin, const AttribGroupID idFastAttribMax, const SNO snoPower, const int32 nLevel, LPCSTR szTip, const int32 eColoringType, TagResolutionCallback pTagCallback, void * pTagData0, void * pTagData1);
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
void Set_Text_Color(CRefString & usString, FontSpecifier efcDisplayColor);
AttribGroupID FastAttribGroupCreate(int32 eType, int32 nDefaultEntryIndex);  void FastAttribGroupDelete(AttribGroupID idAttribGroup);
void FastAttribSetValueFloat(AttribGroupID idAttribGroup, FastAttribKey tKey, float flValue);
FastAttribKey MakeAttribKey(Attrib eAttrib, int32 nParam);

void UIEquipLegendaryPowers::GetAllPowers(GBIDArray & arrPowers) {
    GBIDArray arrAllPowers;
    ItemsGetExtractableLegendaryPowers(arrAllPowers);
    for (int * iter = blz::begin(arrAllPowers); iter != blz::end(arrAllPowers); ++iter) {
        GBID gbidPower = *iter;
        if (gbidPower != -1) {
            arrPowers.push_back(gbidPower);
        }
    }
    blz::sort(blz::begin(arrPowers), blz::end(arrPowers), ItemCompareExtractableLegendaryPowers);
}











CRefString UIEquipLegendaryPowers::GetPowerTooltipText(GBID gbidPower, bool bUseBullets) {
    SNO snoPower = ItemGetLegendaryPower(gbidPower);
    if (snoPower == -1) {
        return "";
    }
    CRefString sPowerName = SNOToString(POWER_SNO, snoPower, FALSE);
    CRefString sPowerDescription;
    if (bUseBullets) {
        sPowerDescription = "{icon:bullet2} ";
    }
    sPowerDescription += StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_PASSIVE_POWER_DESCRIPTIONS), sPowerName.str());
    Set_Text_Color(sPowerDescription, 15);
    float flPowerValue = ItemGetLegendaryPowerValue(gbidPower);
    AttribGroupID idAttribGroup = FastAttribGroupCreate(2, -1);
    FastAttribSetValueFloat(idAttribGroup, MakeAttribKey(ATTRIBUTE_ITEM_POWER_PASSIVE, snoPower), flPowerValue);
    CRefString sPowerTooltip = UIToolTips_ProcessTip(NULL, idAttribGroup, -1, snoPower, 1, sPowerDescription, 1, ItemResolveValueTagForItemPassivePowerDescription, (void *)ATTRIBUTE_ITEM_POWER_PASSIVE, NULL);
    FastAttribGroupDelete(idAttribGroup);
    return sPowerTooltip;
}

















void UIEquipLegendaryPowers::ShowPowerIcon(const UIID & idItemUI, GBID gbidPower, BOOL bUnlocked) {
    UIWindow & tWndLarge = UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".Large"));
    UIWindow & tWndSmall = UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".Small"));
    if (gbidPower == -1) {
        tWndLarge.SetActive(FALSE);
        tWndSmall.SetActive(FALSE);
        return;
    }
    ImageTextureFrame tTexFrame;
    ACDInventoryGetItemDrawInfoWithGBID(gbidPower, tTexFrame, 0);
    if (ItemIsDrawnAsLargeSlotItem(gbidPower)) {
        tWndLarge.SetTextureFrame(tTexFrame);
        tWndLarge.SetShader(bUnlocked ? -1 : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
        tWndLarge.SetActive(TRUE);
        tWndSmall.SetActive(FALSE);
    } else {
        tWndSmall.SetTextureFrame(tTexFrame);
        tWndSmall.SetShader(bUnlocked ? -1 : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
        tWndSmall.SetActive(TRUE);
        tWndLarge.SetActive(FALSE);
    }
}













void UIEquipLegendaryPowers::ShowPowerIcon(const UIID & idItemUI, GBID gbidPower, PlayerClass ePlayerClass, BOOL bIsMale) {
    UIWindow & tWndLarge = UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".Large"));
    UIWindow & tWndSmall = UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".Small"));
    if (gbidPower == -1) {
        tWndLarge.SetActive(FALSE);
        tWndSmall.SetActive(FALSE);
        return;
    }
    ImageTextureFrame tTexFrame;
    ACDInventoryGetItemDrawInfoWithGBID(gbidPower, tTexFrame, 0, ePlayerClass, bIsMale);
    if (ItemIsDrawnAsLargeSlotItem(gbidPower)) {
        tWndLarge.SetTextureFrame(tTexFrame);
        tWndLarge.SetShader(-1);
        tWndLarge.SetActive(TRUE);
        tWndSmall.SetActive(FALSE);
    } else {
        tWndSmall.SetTextureFrame(tTexFrame);
        tWndSmall.SetShader(-1);
        tWndSmall.SetActive(TRUE);
        tWndLarge.SetActive(FALSE);
    }
}
