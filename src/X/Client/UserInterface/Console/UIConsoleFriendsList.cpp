// Original file: X/Client/UserInterface/Console/UIConsoleFriendsList.cpp
#include "UIConsoleFriendsList.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/Online.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UITextControl.h"
#include "../UI.h"
#include "../UIIDs.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"

namespace UIFriendsList { namespace Console {
static const uint32 MAX_FRIENDLIST_FRIENDS = 32;
class FriendsListUIEntry {
public:
    FriendsListUIEntry() : bOnline(FALSE) {}
    CRefString sDisplayText;
    BOOL bOnline;
};
static_assert(sizeof(FriendsListUIEntry) == 0x20, "FriendsListUIEntry");
class ListViewDataSource : public ConsoleListBoxDataSource {
public:
    ListViewDataSource();
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    FriendsListUIEntry tEntryUI[MAX_FRIENDLIST_FRIENDS];
    uint32 nCount;
};
static_assert(sizeof(ListViewDataSource) == 0x410, "ListViewDataSource");
struct UIConsoleFriendListGlobals {
    UIConsoleFriendListGlobals() : nCurrentController(-1), bActive(FALSE), nCount(0) {}
    ListViewDataSource tListViewDataSource;
    int32 nCurrentController;
    BOOL bActive;
    FriendsListAcceptedCallback pfnOnAccept;
    FriendsListCancelledCallback pfnOnCancel;
    int32 nCount;
};
static_assert(sizeof(UIConsoleFriendListGlobals) == 0x430, "UIConsoleFriendListGlobals");
ListViewDataSource::ListViewDataSource() : nCount(0) {}
int32 ListViewDataSource::GetNumItems() const { return nCount; }
UIControlSourceKey ListViewDataSource::GetSourceKeyForItem(const int32 nIndex) const {
    const LPCSTR FRIEND_LIST_ITEM_TEMPLATE = "ConsoleFriendsList:ConsoleFriendsList_main.LayoutRoot.FriendsList.FriendListItem";
    const UIControlSourceKey FRIEND_LIST_SOURCE_KEY = UIControlSourceKey::Parse(FRIEND_LIST_ITEM_TEMPLATE, -1, NULL, NULL);
    return FRIEND_LIST_SOURCE_KEY;
}
void ListViewDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex) {}
void ListViewDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex) {
    UIID idName, idSelected, idOnline;
    idName.SetFromParent(idItemUI, "Text");
    idSelected.SetFromParent(idItemUI, "Selected");
    idOnline.SetFromParent(idItemUI, "Online");
    const BOOL bSelected = tListBox.GetSelectedIndex() == nIndex;
    UI_GetWindowFromID(idSelected).SetActive(bSelected);
    UI_GetWindowFromID(idName).SetActive(TRUE);
    CRefString sListText = tEntryUI[nIndex].sDisplayText;
    UIText & tText = UI_GetTextFromID(idName);
    tText.SetText(sListText, FALSE);
    UI_GetTextFromID(idOnline).SetText(StringListFetch("Console:Online").str(), FALSE);
    UI_GetTextFromID(idOnline).SetActive(tEntryUI[nIndex].bOnline);
}
UIConsoleFriendListGlobals * sgptFriendsListGlobals;
::Console::Online::Friend * sgptFriendsArray;

void InitializeForGame() {
    sgptFriendsListGlobals = new UIConsoleFriendListGlobals;
    sgptFriendsArray = new ::Console::Online::Friend[MAX_FRIENDLIST_FRIENDS];
}
void DisposeForGame() {
    delete sgptFriendsListGlobals;
    sgptFriendsListGlobals = NULL;
    delete[] sgptFriendsArray;
}
void Close();
void OnButton1(const UIID &, const int32 nControllerIndex) {
    if (nControllerIndex != sgptFriendsListGlobals->nCurrentController || sgptFriendsListGlobals->nCount <= 0)
        return;
    Close();
    if (sgptFriendsListGlobals->pfnOnAccept) {
        UIConsoleListBox & tFriendsList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDSLIST);
        int32 nIndex = tFriendsList.GetSelectedIndex();
        if (nIndex >= 0 && nIndex < sgptFriendsListGlobals->nCount)
            sgptFriendsListGlobals->pfnOnAccept(sgptFriendsArray[nIndex]);
    }
}
void OnButton2(const UIID &, const int32 nControllerIndex) {
    if (nControllerIndex == sgptFriendsListGlobals->nCurrentController) {
        Close();
        if (sgptFriendsListGlobals->pfnOnCancel)
            sgptFriendsListGlobals->pfnOnCancel();
    }
}
BOOL IsActive() { return sgptFriendsListGlobals->bActive; }
void Close() {
    UIWindow & wnd = UI_GetWindowFromID(UIWND_CONSOLE_FRIENDSLIST);
    wnd.SetActive(FALSE);
    sgptFriendsListGlobals->bActive = FALSE;
}
} }
