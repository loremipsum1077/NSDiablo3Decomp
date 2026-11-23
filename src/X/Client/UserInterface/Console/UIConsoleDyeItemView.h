// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleDyeItemView.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryView.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameBalance.h"
#include "../../../Sigma/SigmaTypes.h"

struct DyeItemMessage;
struct Player;
struct UIControlSourceKey;
struct UIID;

namespace UIMystic {
namespace Console {
class UIDyeItemView : public UIInventory::Console::UIInventoryView {  // size 0xa0
public:
    UIDyeItemView(const UIID &);
    virtual ~UIDyeItemView();  // vtable slot 0
    void GoBack();
    void Reset();
    BOOL HasDyes() const;
    void ChooseDyeForSelectedItem();
    void ApplyDye();
    void RemoveDye();
    BOOL IsChoosingDye() const;
    BOOL IsFinishedDyeing() const;
    virtual BOOL CanAcceptFocus() const;  // vtable slot 2
    GBID GetSelectedDye() const;
    bool IsSelectedItemDyed();
    void OnDyeItemResults(const DyeItemMessage & tDyeItemResults);
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnParentScreenChanged();  // vtable slot 5
    virtual void OnUpdate();  // vtable slot 12
    virtual void OnSelectedItemChanged();  // vtable slot 10
    virtual void OnGotFocus();  // vtable slot 8
    virtual void OnLostFocus();  // vtable slot 9
private:
    void SendDyeMessage(const ActorCommonData & tACDItem, DyeType eDyeType);
    // No destructor is declared: the D2 and D0 at line 93 are the implicit ones.
    class DyeDataSource : public ConsoleListBoxDataSource {  // size 0x30; line 93
    public:
        DyeDataSource();
        void SetPlayer(const Player * ptPlayer)  // line 99
        {
            m_ptPlayer = ptPlayer;
        }

        virtual int32 GetNumItems() const;  // vtable slot 2
        void SetCurrentItem(const ACDID idACDItem)  // line 108
        {
            m_idACDItem = idACDItem;
        }

        GBID GetDye(const int32 nIndex) const;
        virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
        virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
        virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
        void Clear();
        void PopulateDyes(const Player * ptPlayer, const ACDID idACDItem);
    private:
        const Player * m_ptPlayer;  // 0x08
        ACDID m_idACDItem;  // 0x10
        GBIDArray m_arDyes;  // 0x18
    };
    enum State {
        STATE_CHOOSING_ITEM = 0,
        STATE_CHOOSING_DYE = 1,
        STATE_WAITING_FOR_REPLY = 2,
        STATE_WAITING_FOR_RESULTS = 3,
        STATE_FINISHED = 4,
    };
    UIMystic::Console::UIDyeItemView::State m_eState;  // 0x60
    UIMystic::Console::UIDyeItemView::DyeDataSource m_tDataSource;  // 0x68
    DyeType m_eLastDyeType;  // 0x98
    UIConsoleListBox & GetListBox() const;
    void UpdateState();
    void UpdateUI();
    void UpdateItemVisual();
    void PopulateDyes();
    int32 GetCurrentDyeIndex() const;
    void FinishApplyingDye();
};
static_assert(sizeof(UIDyeItemView) == 0xa0, "UIDyeItemView");

}  // namespace Console
}  // namespace UIMystic

// Functions defined in this file (bodies are not in the DWARF):
//   line    93  UIMystic::Console::UIDyeItemView::DyeDataSource::~DyeDataSource()  [0x7100382420 (+1 more), inlined into 3 functions]
//   line    99  void UIMystic::Console::UIDyeItemView::DyeDataSource::SetPlayer(const Player * ptPlayer)  [inlined into 1 functions]
//   line   108  void UIMystic::Console::UIDyeItemView::DyeDataSource::SetCurrentItem(const ACDID idACDItem)  [inlined into 1 functions]
