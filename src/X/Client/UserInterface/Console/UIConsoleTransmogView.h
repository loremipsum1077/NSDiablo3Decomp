// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleTransmogView.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryView.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Interface/UITransmogrifyCommon.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameBalance.h"

struct GBIDDataMessage;
struct Player;
struct TransmogItemMessage;
struct UIControlSourceKey;
struct UIID;

namespace UIMystic {
namespace Console {
class UITransmogView : public UIInventory::Console::UIInventoryView {  // size 0xa0
public:
    UITransmogView(const UIID &);
    void GoBack();
    void Reset();
    BOOL HasTransmogs() const;
    void ChooseTransmogForSelectedItem();
    void Transmogrify();
    BOOL IsChoosingTransmog() const;
    BOOL IsFinishedTransmogrifying() const;
    virtual BOOL CanAcceptFocus() const;  // vtable slot 2
    GBID GetSelectedTransmog() const;
    void OnTransmogResults(const TransmogItemMessage & tTransmogResults);
    void OnUnlockTransmogResults(const GBIDDataMessage * ptMessage);
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnParentScreenChanged();  // vtable slot 5
    virtual void OnUpdate();  // vtable slot 12
    virtual void OnSelectedItemChanged();  // vtable slot 10
    virtual void OnGotFocus();  // vtable slot 8
    virtual void OnLostFocus();  // vtable slot 9
    class TransmogDataSource : public ConsoleListBoxDataSource {  // size 0x30
    public:
        inline TransmogDataSource();  // defined in the .cpp (line 460); no out-of-line copy
        void SetPlayer(const Player * ptPlayer)  // line 92
        {
            m_ptPlayer = ptPlayer;
        }
        virtual int32 GetNumItems() const;  // vtable slot 2
        void SetCurrentItem(const ACDID idACDItem)  // line 101
        {
            m_idACDItem = idACDItem;
        }
        inline GBID GetTransmog(const int32 nIndex) const;  // .cpp line 477
        virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
        virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
        virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
        inline void Clear();  // .cpp line 553
        void PopulateTransmogs(const Player * ptPlayer, const ACDID idACDItem);
    private:
        const Player * m_ptPlayer;  // 0x08
        ACDID m_idACDItem;  // 0x10
        UITransmogrify::DisplayedTransmogArray m_arTransmogs;  // 0x18
    };
    enum State {
        STATE_CHOOSING_ITEM = 0,
        STATE_CHOOSING_TRANSMOG = 1,
        STATE_WAITING_FOR_REPLY = 2,
        STATE_WAITING_FOR_RESULTS = 3,
        STATE_FINISHED = 4,
    };
private:
    UIMystic::Console::UITransmogView::State m_eState;  // 0x60
    UIMystic::Console::UITransmogView::TransmogDataSource m_tDataSource;  // 0x68
    GBID m_gbidLastTransmog;  // 0x98
    inline UIConsoleListBox & GetListBox() const;  // .cpp line 271
    void UpdateState();
    void UpdateUI();
    inline void UpdateItemVisual();  // .cpp line 381
    inline void PopulateTransmogs();  // .cpp line 400
    int32 GetCurrentTransmogIndex() const;
    void FinishTransmog();
};
static_assert(sizeof(UITransmogView) == 0xa0, "UITransmogView");

}  // namespace Console
}  // namespace UIMystic

// Functions defined in this file (bodies are not in the DWARF):
//   line    27  UIMystic::Console::UITransmogView::~UITransmogView()  [0x7100173170 (+1 more), inlined into 2 functions]
//   line    86  UIMystic::Console::UITransmogView::TransmogDataSource::~TransmogDataSource()  [0x71001730f0 (+1 more), inlined into 4 functions]
//   line    92  void UIMystic::Console::UITransmogView::TransmogDataSource::SetPlayer(const Player * ptPlayer)  [inlined into 1 functions]
//   line   101  void UIMystic::Console::UITransmogView::TransmogDataSource::SetCurrentItem(const ACDID idACDItem)  [inlined into 1 functions]
