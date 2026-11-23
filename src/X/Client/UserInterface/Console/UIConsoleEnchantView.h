// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleEnchantView.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryView.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Interface/UIEnchantCommon.h"
#include "../UIID.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameBalance.h"

struct ANNDataMessage;
struct EnchantAffixChoice;
struct EnchantAffixChooseMessage;
struct UIControlSourceKey;
typedef int32 PlayerClass;

namespace UIMystic {
namespace Console {
class UIEnchantView : public UIInventory::Console::UIInventoryView {  // size 0x138
    enum State {
        STATE_CHOOSING_ITEM = 0,
        STATE_CHOOSING_AFFIX = 1,
        STATE_SHOWING_POSSIBLE_REPLACEMENTS = 2,
        STATE_SHOW_PROGRESS_BAR = 3,
        STATE_SEND_ENCHANT_REQUEST = 4,
        STATE_WAITING_FOR_ENCHANT_RESPONSE = 5,
        STATE_CHOOSING_REPLACEMENT_AFFIX = 6,
        STATE_WAITING_FOR_RESULTS = 7,
        STATE_WAITING_FOR_ITEM_ACDID = 8,
        STATE_FINISHED = 9,
    };
public:
    UIEnchantView(const UIID &);
    BOOL IsChoosingItem() const;
    BOOL IsChoosingAffix() const;
    BOOL IsWaitingForEnchantResponse() const;
    BOOL IsRollingReplacementAffixes() const;
    BOOL IsChoosingReplacementAffix() const;
    BOOL IsWaitingToFinish() const;
    BOOL SelectedItemHasEnchantedAffix() const;
    int32 GetNumAffixes() const;
    void GoBack();
    void Reset();
    void ChooseAffixesForItem(const ACDID idItemACD);
    void SelectCurrentAffix();
    void ShowPossibleReplacements(BOOL bShow);
    void OnEnchantAffixChoose(const EnchantAffixChooseMessage * ptMessage);
    void OnEnchantAffixResults(const ANNDataMessage * ptMessage);
    void EnchantItem();
    BOOL IsFinishedEnchanting() const;
    BOOL IsShowingPossibleReplacements() const;
    virtual BOOL CanAcceptFocus() const;  // vtable slot 2
    virtual BOOL CanScrollWithTriggers() const;  // vtable slot 3
    virtual UIID GetScrollBoxUIID() const;  // vtable slot 4
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnUpdate();  // vtable slot 12
    virtual void OnSelectedItemChanged();  // vtable slot 10
    virtual void OnGotFocus();  // vtable slot 8
    virtual void OnLostFocus();  // vtable slot 9
    class EnchantAffixDataSource : public ConsoleListBoxDataSource {  // size 0x28
    public:
        EnchantAffixDataSource(int32);
        virtual int32 GetNumItems() const;  // vtable slot 2
        virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
        virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
        virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
        virtual BOOL IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex);  // vtable slot 6
        UIEnchant::AffixInfo GetAffixInfo(const int32 nIndex) const;
        void Clear();
        void PopulateItemAffixList(const ACDID idACDItem, const PlayerClass ePlayerClass);
        void PopulateReplacementAffixList(const ACDID idACDItem, const PlayerClass ePlayerClass, const GBID gbidSelectedAffix, const EnchantAffixChoice * arEnchantAffixChoices, int32 nEnchantAffixChoicesCount);
    private:
        UIEnchant::AffixInfoArray m_arItemAffixes;  // 0x08
        int32 m_nOriginalAffixIndex;  // 0x20
        int32 m_nMaxItemsWithoutScrolling;  // 0x24
    };
private:
    UIMystic::Console::UIEnchantView::EnchantAffixDataSource m_tAffixDataSource;  // 0x60
    UIMystic::Console::UIEnchantView::EnchantAffixDataSource m_tReplacementAffixDataSource;  // 0x88
    UIMystic::Console::UIEnchantView::State m_eState;  // 0xb0
    UIEnchant::AffixInfo m_tSelectedAffixInfo;  // 0xb8
    ACDNetworkName m_annPendingItem;  // 0xd8
    UIID m_idAffixPanel;  // 0xe0
    UIID m_idAffixListBox;  // 0xe8
    UIID m_idPossibleAffixPanel;  // 0xf0
    UIID m_idItemDataPanel;  // 0xf8
    UIID m_idPreviouslyEnchantedPanel;  // 0x100
    UIID m_idRollingReplacementPanel;  // 0x108
    UIID m_idRollingReplacementProgress;  // 0x110
    UIID m_idReplacementAffixPanel;  // 0x118
    UIID m_idReplacementAffixListBox;  // 0x120
    UIID m_idFinishedPanel;  // 0x128
    uint32 m_dwRollingReplacementsEndTime;  // 0x130
    UIID GetSelectedItemUIID() const;
    UIEnchant::AffixInfo GetSelectedAffixInfo() const;
    void UpdateState();
    void UpdatePendingItem();
    void UpdateUI();
    void UpdateFullAffixListUI();
    void UpdateAlreadyEnchantedUI();
    void UpdateRollingUI();
    void UpdateReplacementAffixListUI();
    void UpdateFinishedUI();
    void UpdatePossibleReplacementsUI();
    void UpdateItemInfo();
    void PopulateItemAffixList();
};
static_assert(sizeof(UIEnchantView) == 0x138, "UIEnchantView");

}  // namespace Console
}  // namespace UIMystic

// Functions defined in this file (bodies are not in the DWARF):
//   line    27  UIMystic::Console::UIEnchantView::~UIEnchantView()  [0x71003c7060 (+1 more), inlined into 2 functions]
//   line   127  UIMystic::Console::UIEnchantView::EnchantAffixDataSource::~EnchantAffixDataSource()  [0x71003c6f80 (+1 more), inlined into 4 functions]
