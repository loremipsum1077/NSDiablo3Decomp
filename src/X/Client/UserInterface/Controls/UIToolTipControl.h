// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIToolTipControl.h
// Laid out on the DWARF declaration lines (blank-line padding). Only the types this
// file's functions need are present; the other *TooltipData structs are left out.

#pragma once

#include "UIWindowControl.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundHeroManager.h"
#include "../../../Common/UserInterface/UIToolTipControl.h"
#include "../../../Sigma/RefString.h"

class UIText;
class UIButton;
struct Rect2D;
typedef int32 GBID;  // X/Common/GameBalance.h
typedef int32 UIItemOverlay;  // X/Common/Items.h


typedef int32 UIJustifyHorizontal;
typedef int32 UIJustifyVertical;
typedef int32 ItemQualityLevel;
typedef int32 ItemEffectType;
typedef int32 PlayerClass;
typedef int32 LightID;

struct EnchantTooltipData;
struct BannerTooltipData;
struct FriendTooltipData;
struct SkillTooltipData;
struct TraitTooltipData;
struct TitledTooltipData;
struct PlayerNameTooltipData;
struct AchievementTooltipData;
struct RewardTooltipData;
struct BnetSkillTooltipData;

enum {
    TOOLTIP_CURRENTLY_EQUIPPED_PLAYER_BIT = 0,
    TOOLTIP_CURRENTLY_EQUIPPED_HIRELING_BIT = 1,
    TOOLTIP_CREATE_CLOSE_BUTTON = 2,
    TOOLTIP_RUNE_ONLY_BIT = 3,
    TOOLTIP_SHOW_ALT_INSTRUCTIONS_BIT = 4,
    TOOLTIP_IS_ANCIENT = 5,
    TOOLTIP_IS_PRIMAL_ANCIENT = 6,
    TOOLTIP_IS_CRAFTING_MATERIALS = 7,
};


struct RecipeTooltipData {  // size 0x110

    RecipeTooltipData();















    TooltipType eTooltipType;  // 0x00
    GBID gbidItem;  // 0x04
    uint32 dwTooltipFlags;  // 0x08
    UIItemOverlay eOverlay;  // 0x0c
    ItemQualityLevel eIQL;  // 0x10
    CRefString uszName;  // 0x18
    CRefString uszType;  // 0x30
    CRefString uszStats;  // 0x48
    CRefString uszBaseStats;  // 0x60
    CRefString uszRating;  // 0x78
    CRefString uszSetItem;  // 0x90
    CRefString uszSlot;  // 0xa8
    CRefString uszClassReqs;  // 0xc0
    CRefString uszItemLevel;  // 0xd8
    CRefString uszRequirements;  // 0xf0
    ImageTextureFrame tImageTexFrame;  // 0x108



};
struct InGamePortraitTooltipData;
struct ArmorSetRewardTooltipData;
namespace OnlineService { struct FriendInvite; struct Player; }








































struct ItemTooltipData {  // size 0x110

    ItemTooltipData();

    ACDID idACD;  // 0x00
    TooltipType eTooltipType;  // 0x04
    ItemEffectType eItemEffectType;  // 0x08
    GBID gbid;  // 0x0c
    ItemQualityLevel eIQL;  // 0x10
    uint32 dwTooltipFlags;  // 0x14
    UIItemOverlay eOverlay;  // 0x18
    ImageTextureFrame tImageTexFrame;  // 0x1c
    LPCSTR uszName;  // 0x28
    LPCSTR uszType;  // 0x30
    LPCSTR uszRating;  // 0x38
    LPCSTR uszSpecialStats;  // 0x40
    LPCSTR uszClassReqs;  // 0x48
    LPCSTR uszRequirements;  // 0x50
    LPCSTR uszItemLevel;  // 0x58
    LPCSTR uszStats;  // 0x60
    LPCSTR uszSetItem;  // 0x68
    LPCSTR uszFlavor;  // 0x70
    LPCSTR uszTradableTo;  // 0x78
    LPCSTR uszCost;  // 0x80
    LPCSTR uszDurability;  // 0x88
    LPCSTR uszInstruction;  // 0x90
    LPCSTR uszSlot;  // 0x98
    LPCSTR uszSocketCost;  // 0xa0
    LPCSTR uszRepairCost;  // 0xa8
    LPCSTR uszTransmog;  // 0xb0
    LPCSTR uszDye;  // 0xb8
    LPCSTR uszSocketStats[3];  // 0xc0
    GBID arrSocketGBIDs[3];  // 0xd8
    ImageTextureFrame arrSocketTextures[3];  // 0xe4


    BOOL fShowComparisonInfo;  // 0xfc
    float flComparisonDeltaEHP;  // 0x100
    float flComparisonDeltaDmg;  // 0x104
    float flComparisonDeltaHealing;  // 0x108

    BOOL HasSockets() const;
};
















































































































class TooltipUIElements {  // size 0x10
    // vtable pointer
public:
    TooltipUIElements();

    virtual ~TooltipUIElements();  // vtable slot 0


    void SetupWrapper(const SNO snoUI, LPCSTR szTemplate, LPCSTR szControlName, int32 & nDynamicIndex);





    virtual void HideControls();  // vtable slot 2


    virtual void SetupSubControls(const UIID & id);  // vtable slot 3


protected:
    UIWindow * m_ptWrapper;  // 0x08
};


class ItemUIElements : public TooltipUIElements {  // size 0x18
public:

    ItemUIElements();


    void SetupSubControlsData(const UIID & id, const ItemTooltipData & tData);



    void PositionSubControls(const ItemTooltipData & tData, const BOOL fEquipped, const BOOL fResetScrollPos);




    ACDID GetACD() const { return m_idACD; }





    BOOL CanScroll() const;


private:

    ACDID m_idACD;  // 0x10
};

class EnchantUIElements : public TooltipUIElements {  // size 0x10
public:

    EnchantUIElements();


    void PositionSubControls(const EnchantTooltipData &);
};


class BannerUIElements : public TooltipUIElements {  // size 0x10
public:

    BannerUIElements();


    void PositionSubControls(const BannerTooltipData &);
};


class RecipeUIElements : public TooltipUIElements {  // size 0x10
public:

    RecipeUIElements();


    void PositionSubControls(const RecipeTooltipData &);
};


class SkillUIElements : public TooltipUIElements {  // size 0x10
public:

    SkillUIElements();


    void PositionSubControls(const SkillTooltipData &);
};


class TraitUIElements : public TooltipUIElements {  // size 0x10
public:

    TraitUIElements();


    void PositionSubControls(const TraitTooltipData &);
};


class TitledUIElements : public TooltipUIElements {  // size 0x10
public:

    TitledUIElements();


    void PositionSubControls(const TitledTooltipData &);
};


class PlayerNameUIElements : public TooltipUIElements {  // size 0x10
public:

    PlayerNameUIElements();


    virtual void HideControls();  // vtable slot 2


    void PositionSubControls(const PlayerNameTooltipData &);
};


class AchievementUIElements : public TooltipUIElements {  // size 0x10
public:

    AchievementUIElements();


    void PositionSubControls(const AchievementTooltipData &);
};


class BnetSkillUIElements : public TooltipUIElements {  // size 0x10
public:

    BnetSkillUIElements();


    void PositionSubControls(const BnetSkillTooltipData &);
};


class InGamePortraitUIElements : public TooltipUIElements {  // size 0x10
public:

    InGamePortraitUIElements();


    void PositionSubControls(const InGamePortraitTooltipData &);
};


class RewardUIElements : public TooltipUIElements {  // size 0x10
public:

    RewardUIElements();


    void PositionSubControls(const RewardTooltipData &);
};


class FriendInviteUIElements : public TooltipUIElements {  // size 0x10
public:

    FriendInviteUIElements();


    void PositionSubControls(const OnlineService::FriendInvite &);
};


class BnetPlayerUIElements : public TooltipUIElements {  // size 0x10
public:

    BnetPlayerUIElements();


    void PositionSubControls(const OnlineService::Player &);
};


class FriendUIElements : public TooltipUIElements {  // size 0xf0
public:

    FriendUIElements();


    virtual void HideControls();  // vtable slot 2


    void PositionSubControls(const FriendTooltipData &);


    BackgroundHeroDescription m_heroModelDescription;  // 0x10
    LightID m_idLight;  // 0xbc

    Vector2D m_vecBottomContainerTL;  // 0xc0
    Vector2D m_vecBottomContainerDim;  // 0xc8

    Vector2D m_vecPartyContainerTL;  // 0xd0
    Vector2D m_vecPartyContainerDim;  // 0xd8

    Vector2D m_vecBodyTextureTL;  // 0xe0
    Vector2D m_vecBodyTextureDim;  // 0xe8
};

class ArmorSetRewardUIElements : public TooltipUIElements {  // size 0x10
public:

    ArmorSetRewardUIElements();


    void PositionSubControls(const ArmorSetRewardTooltipData &);
};



class UIToolTip : public UIWindow {  // size 0x400
public:

    UIToolTip(const WindowCreationParams &, const Vector2D &, const Vector2D &);



    UIToolTip(const WindowCreationParams & tParams, const Rect2D * pRect);



    virtual ~UIToolTip();  // vtable slot 0


    const UIID & GetParentID() const { return m_idActiveOwner; }






    void SetParentRect(const Rect2D & rectParent) { m_rectParent = rectParent; }






    const Rect2D & GetParentRect() const;









    BOOL OwnerIsMissing() const;



    void Update();



    void SetTextTooltipData(const UIID & idParent, LPCSTR uszTip, const UIJustifyHorizontal eJustifyHorizontal, const BOOL fForceRefresh);






    void SetItemTooltipData(const UIID & idParent, const ItemTooltipData & tData, const BOOL bResetScrollPos);




    ACDID GetItemACDID();



    void SetRecipeTooltipData(const UIID &, const RecipeTooltipData &);




    void SetEnchantTooltipData(const UIID &, const EnchantTooltipData &);




    void SetSkillTooltipData(const UIID &, const SkillTooltipData &);




    void SetTraitTooltipData(const UIID &, const TraitTooltipData &);




    void SetTitledTooltipData(const UIID &, const TitledTooltipData &);




    void SetPlayerNameTooltipData(const UIID &, const PlayerNameTooltipData &);




    void SetAchievementTooltipData(const UIID &, const AchievementTooltipData &);




    void SetBnetSkillTooltipData(const UIID &, const BnetSkillTooltipData &);




    void SetInGamePortraitTooltipData(const UIID &, const InGamePortraitTooltipData &);



    void SetRewardTooltipData(const UIID &, const RewardTooltipData &);




    void SetBannerTooltipData(const UIID &, const BannerTooltipData &);



    void SetFriendInviteTooltipData(const UIID &, const OnlineService::FriendInvite &);



    void SetBnetPlayerTooltipData(const UIID &, const OnlineService::Player &);



    void SetFriendTooltipData(const UIID &, const FriendTooltipData &);



    void SetArmorSetRewardTooltipData(const UIID &, const ArmorSetRewardTooltipData &);




    void SetJustification(UIJustifyHorizontal, UIJustifyVertical);







    void SetupCloseButton();






    void PositionCloseButton();


    void StartFadeInTimer();


    BOOL IsFading();






    TooltipType GetTooltipType() const;





    BOOL CanScroll() const;


    uint32 GetFlags() const;





    virtual void SetActive(const BOOL fActive);  // vtable slot 10


    virtual UIID HitTest(const IVector2D & pt);  // vtable slot 15


    virtual UIID HitTestForTooltips(  // vtable slot 16
        const IVector2D &,
        BOOL &)
    {
        return GetUIIDNone();
    }

    virtual BOOL IsType(  // vtable slot 17
        const UIType eType) const
    {
        return eType == UIT_TOOLTIP || UIWindow::IsType(eType);
    }








    virtual void Draw(const uint8, float & flZSlice);  // vtable slot 19



    static void OnHyperlinkClose(const UIID & id);


    static void InitializeForGame();

    // private from here in the DWARF; left public because UIToolTipManager.cpp reads the members directly.

    void sInit();


    void SetParentID(const UIID & idParent);


    uint8 sGetAlphaLevel();


    void sDisposeSubControlsForType();

    void sHideSubControlsForType();


    BOOL sChangeTooltipType(const UIID & idParent, const TooltipType eNewTooltipType);



    void sSetupTextSubControls(const UIJustifyHorizontal eJustifyHorizontal);


    void sSetupItemSubControls(const uint32);


    void sSetupSkillSubControls(const uint32, const uint32);



    UIID m_idActiveOwner;  // 0x1d0
    uint32 m_dwFadeStart;  // 0x1d8
    uint32 m_dwFadeInStart;  // 0x1dc
    uint8 m_bAlpha;  // 0x1e0
    Rect2D m_rectParent;  // 0x1e4

    TooltipType m_eTooltipType;  // 0x1f4
    BOOL m_bDirty;  // 0x1f8


    uint32 m_dwNumSocketedRunes;  // 0x1fc
    int32 m_nInvestedPoints;  // 0x200
    uint32 m_dwTooltipFlags;  // 0x204

    UIText * m_ptTextSubControl;  // 0x208

    ItemUIElements m_tItemUI;  // 0x210
    RecipeUIElements m_tRecipeUI;  // 0x228
    SkillUIElements m_tSkillUI;  // 0x238
    TraitUIElements m_tTraitUI;  // 0x248
    EnchantUIElements m_tEnchantUI;  // 0x258
    TitledUIElements m_tTitledUI;  // 0x268
    PlayerNameUIElements m_tPlayerNameUI;  // 0x278
    AchievementUIElements m_tAchievementUI;  // 0x288
    BnetSkillUIElements m_tBnetSkillUI;  // 0x298
    InGamePortraitUIElements m_tInGamePortraitUI;  // 0x2a8
    RewardUIElements m_tRewardUI;  // 0x2b8
    BannerUIElements m_tBannerUI;  // 0x2c8
    FriendInviteUIElements m_tFriendInviteUI;  // 0x2d8
    BnetPlayerUIElements m_tBnetPlayerUI;  // 0x2e8
    FriendUIElements m_tFriendUI;  // 0x2f8
    ArmorSetRewardUIElements m_tArmorSetRewardUI;  // 0x3e8

    UIButton * m_ptCloseButtonSubControl;  // 0x3f8
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(RecipeTooltipData) == 0x110, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, eTooltipType) == 0x0, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, gbidItem) == 0x4, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, dwTooltipFlags) == 0x8, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, eOverlay) == 0xc, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, eIQL) == 0x10, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, uszName) == 0x18, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, uszType) == 0x30, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, uszStats) == 0x48, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, uszBaseStats) == 0x60, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, uszRating) == 0x78, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, uszSetItem) == 0x90, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, uszSlot) == 0xa8, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, uszClassReqs) == 0xc0, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, uszItemLevel) == 0xd8, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, uszRequirements) == 0xf0, "RecipeTooltipData");
static_assert(offsetof(RecipeTooltipData, tImageTexFrame) == 0x108, "RecipeTooltipData");
static_assert(sizeof(ItemTooltipData) == 0x110, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, idACD) == 0x0, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, eTooltipType) == 0x4, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, eItemEffectType) == 0x8, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, gbid) == 0xc, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, eIQL) == 0x10, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, dwTooltipFlags) == 0x14, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, eOverlay) == 0x18, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, tImageTexFrame) == 0x1c, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszName) == 0x28, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszType) == 0x30, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszRating) == 0x38, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszSpecialStats) == 0x40, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszClassReqs) == 0x48, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszRequirements) == 0x50, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszItemLevel) == 0x58, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszStats) == 0x60, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszSetItem) == 0x68, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszFlavor) == 0x70, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszTradableTo) == 0x78, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszCost) == 0x80, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszDurability) == 0x88, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszInstruction) == 0x90, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszSlot) == 0x98, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszSocketCost) == 0xa0, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszRepairCost) == 0xa8, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszTransmog) == 0xb0, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszDye) == 0xb8, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, uszSocketStats) == 0xc0, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, arrSocketGBIDs) == 0xd8, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, arrSocketTextures) == 0xe4, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, fShowComparisonInfo) == 0xfc, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, flComparisonDeltaEHP) == 0x100, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, flComparisonDeltaDmg) == 0x104, "ItemTooltipData");
static_assert(offsetof(ItemTooltipData, flComparisonDeltaHealing) == 0x108, "ItemTooltipData");
static_assert(sizeof(TooltipUIElements) == 0x10, "TooltipUIElements");
static_assert(sizeof(ItemUIElements) == 0x18, "ItemUIElements");
static_assert(sizeof(EnchantUIElements) == 0x10, "EnchantUIElements");
static_assert(sizeof(BannerUIElements) == 0x10, "BannerUIElements");
static_assert(sizeof(RecipeUIElements) == 0x10, "RecipeUIElements");
static_assert(sizeof(SkillUIElements) == 0x10, "SkillUIElements");
static_assert(sizeof(TraitUIElements) == 0x10, "TraitUIElements");
static_assert(sizeof(TitledUIElements) == 0x10, "TitledUIElements");
static_assert(sizeof(PlayerNameUIElements) == 0x10, "PlayerNameUIElements");
static_assert(sizeof(AchievementUIElements) == 0x10, "AchievementUIElements");
static_assert(sizeof(BnetSkillUIElements) == 0x10, "BnetSkillUIElements");
static_assert(sizeof(InGamePortraitUIElements) == 0x10, "InGamePortraitUIElements");
static_assert(sizeof(RewardUIElements) == 0x10, "RewardUIElements");
static_assert(sizeof(FriendInviteUIElements) == 0x10, "FriendInviteUIElements");
static_assert(sizeof(BnetPlayerUIElements) == 0x10, "BnetPlayerUIElements");
static_assert(sizeof(FriendUIElements) == 0xf0, "FriendUIElements");
static_assert(sizeof(ArmorSetRewardUIElements) == 0x10, "ArmorSetRewardUIElements");
static_assert(sizeof(UIToolTip) == 0x400, "UIToolTip");
