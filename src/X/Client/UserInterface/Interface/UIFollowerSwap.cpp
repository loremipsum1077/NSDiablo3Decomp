// Original file: X/Client/UserInterface/Interface/UIFollowerSwap.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../../CPlayerInput.h"
#include "../../ImageCollection.h"
#include "../../Sound/Sound.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Hireling.h"
#include "../../../Common/HirelingSwig.h"
#include "../../../Common/Player.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/TagMaps/ActorTags.h"
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/ErrorManager/ErrorManager.h"
// Declared here because their headers are not adopted yet (CGameMessage.cpp, CPlayer.cpp, CPlayerInput.cpp, UI.cpp, Hireling.cpp, ImageCollection.cpp, UIHirelingCommon.cpp).
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer); ACDID LocalPlayerACDGet(); CursorMgr & PlayerGetCursorMgr(); BOOL UI_ValidateController(int32 nController);
SNO HirelingGetSNO(HirelingClass eClass, HirelingType eType); BOOL ImageCollectionGetImageInfo(const ImageCollectionType eCollection, const ImageHandle hImage, ImageTextureFrame & tImageTexFrame);
extern const uint32 MAX_FOLLOWER_SWAP_TIME; namespace UIHireling { CRefString GetNameForHirelingClass(const HirelingClass eClass, const BOOL fWithArticle); }







struct UIFollowerSwapGlobals {

    uint32 dwLastOpenTime;
};





UIFollowerSwapGlobals sgtUIFollowerSwapGlobals;

namespace UIFollowerSwap {
void Close(); BOOL IsVisible();

void InitializeForGame()
{
    sgtUIFollowerSwapGlobals.dwLastOpenTime = 0;
}





void DisposeForGame()
{
}





void OnOK(const UIID &)
{

    Close();

    SimpleMessage tMessage;
    MessageSendToServer(226, &tMessage, sizeof(tMessage), LocalPlayerGet());
}




void OnCancel(const UIID &)
{

    Close();
}




BOOL IsVisible()
{

    return UI_TestID(UIDLG_FOLLOWERSWAP) &&
        UI_GetWindowFromID(UIDLG_FOLLOWERSWAP).IsActive();
}




void Close()
{

    UI_GetWindowFromID(UIDLG_FOLLOWERSWAP).SetActive(FALSE);
}



void Show(const HirelingClass eNewClass, int32 nControllerIndex, BOOL bForAutoDismiss)
{



    if (!UI_TestID(UIDLG_FOLLOWERSWAP))
        return;
    PlayerGetCursorMgr().ResetCursorState(FALSE);



    ACDID idACDPlayer = LocalPlayerACDGet();
    if (idACDPlayer == ACDID_INVALID)
        return;



    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_HIRELING_CONFIRMATION), cgtPSODefaults);

    sgtUIFollowerSwapGlobals.dwLastOpenTime = bForAutoDismiss ? 0 : CGameGetClientTicks();

    UI_GetWindowFromID(UIDLG_FOLLOWERSWAP).SetActive(TRUE);
    {

        CRefString sNewClassName = UIHireling::GetNameForHirelingClass(eNewClass, FALSE);
        CRefString sName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_FOLLOWERS), "Name");
        sName = StringListEntryReplace(sName, "{CLASS}", sNewClassName.str(), 999);
        UI_GetTextFromID(UITXT_FOLLOWERSWAP_NAME).SetText(sName, FALSE);
    }
    {


        UIWindow & tWndPortrait = UI_GetWindowFromID(UIWND_FOLLOWERSWAP_PORTRAIT);

        SNO snoActor = HirelingGetSNO(eNewClass, HT_NORMAL);
        if (snoActor != SNO_NONE)
        {
            ImageHandle hImage = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_PORTRAIT_IMAGE, snoActor);
            ImageTextureFrame tFrame;
            ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, hImage, tFrame);

            tWndPortrait.SetTextureFrame(tFrame);
        }
        else
        {
            tWndPortrait.SetTextureFrame(cgtImageTextureFrameNull);
        }
    }
    {
        LPCSTR szFlavorLabel;
        if (bForAutoDismiss) { szFlavorLabel = "FollowerFlavorAutoDismiss"; }
        else
        {



            switch (eNewClass)
            {
            case 1:
                szFlavorLabel = "TemplarFlavor";
                break;
            case 2:
                szFlavorLabel = "ScoundrelFlavor";
                break;
            case 3:
                szFlavorLabel = "EnchantressFlavor";
                break;
            default:
                DisplayInternalError(NULL, "", 176, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
        }
        CRefString sText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_FOLLOWERS), szFlavorLabel);
        UI_GetTextFromID(UITXT_FOLLOWERSWAP_FLAVOR).SetText(sText, FALSE);
    }
    {

        CRefString sNewClassNameArticle = UIHireling::GetNameForHirelingClass(eNewClass, TRUE);
        CRefString sMsg;
        if (bForAutoDismiss)
        {
            sMsg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_FOLLOWERS), "FollowersInMultiplayer");
            sMsg = StringListEntryReplace(sMsg, "{NEW_CLASS}", sNewClassNameArticle.str(), 999);
        }
        else
        {
            ACDID idACDHireling = PlayerGetHireling(idACDPlayer);
            ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
            if (!ptACDHireling)
            {
                sMsg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_FOLLOWERS), "JoinWarning");
                sMsg = StringListEntryReplace(sMsg, "{NEW_CLASS}", sNewClassNameArticle.str(), 999);
            }
            else
            {
                HirelingClass eOldClass = HirelingGetClass(*ptACDHireling);
                CRefString sOldClassNameArticle = UIHireling::GetNameForHirelingClass(eOldClass, TRUE);

                sMsg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_FOLLOWERS), "ReplaceWarning");
                sMsg = StringListEntryReplace(sMsg, "{OLD_CLASS}", sOldClassNameArticle.str(), 999);
                sMsg = StringListEntryReplace(sMsg, "{NEW_CLASS}", sNewClassNameArticle.str(), 999);
            }
        }

        CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(sMsg.str()));
        UI_GetTextFromID(UITXT_FOLLOWERSWAP_MSG).SetText(sAfterLanguageRules, FALSE);
    }


    UI_GetButtonFromID(UIBTN_FOLLOWERSWAP_YES).SetActive(!bForAutoDismiss);
    UI_GetButtonFromID(UIBTN_FOLLOWERSWAP_NO).SetActive(!bForAutoDismiss);
    UI_GetTextFromID(UITXT_FOLLOWERSWAP_TIMER).SetActive(!bForAutoDismiss);
    UI_GetButtonFromID(UIBTN_FOLLOWERSWAP_OK).SetActive(bForAutoDismiss);


    UI_GetButtonFromID(UIBTN_FOLLOWERSWAP_YES).SetControllerIndexOverride(nControllerIndex);
    UI_GetButtonFromID(UIBTN_FOLLOWERSWAP_NO).SetControllerIndexOverride(nControllerIndex);
    UI_GetTextFromID(UITXT_FOLLOWERSWAP_TIMER).SetControllerIndexOverride(nControllerIndex);
    UI_GetButtonFromID(UIBTN_FOLLOWERSWAP_OK).SetControllerIndexOverride(nControllerIndex);



}




void Update()
{

    if (IsVisible() && sgtUIFollowerSwapGlobals.dwLastOpenTime != 0)
    {
        ACDID idACDPlayer = LocalPlayerACDGet();
        ActorCommonData * ptACDLocalPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDLocalPlayer && ptACDLocalPlayer->IsDueling())
        {
            Close();
            return;
        }

        uint32 dwCurrentTime = CGameGetClientTicks();
        int32 nTicksRemaining = sgtUIFollowerSwapGlobals.dwLastOpenTime + MAX_FOLLOWER_SWAP_TIME - dwCurrentTime;
        if (nTicksRemaining <= 0)
        {
            Close();
        }
        else
        {
            int32 nSecondsRemaining = nTicksRemaining / 60;
            CRefString sTimer;
            sTimer.Format("%d", nSecondsRemaining + 1);

            UI_GetTextFromID(UITXT_FOLLOWERSWAP_TIMER).SetText(sTimer, FALSE);
        }
    }
}






void OnButtonOK(const UIID & id, const int32 nControllerIndex)
{


    if (UI_ValidateController(nControllerIndex))
    {
        OnOK(id);
    }
}




void OnButtonCancel(const UIID & id, const int32 nControllerIndex)
{


    if (UI_ValidateController(nControllerIndex))
    {
        OnCancel(id);
    }
}

}  // namespace UIFollowerSwap
