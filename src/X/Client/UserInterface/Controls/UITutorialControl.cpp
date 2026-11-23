// Original file: X/Client/UserInterface/Controls/UITutorialControl.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "UITutorialControl.h"
#include "UITextControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIUtils.h"
#include "../../CGame.h"
#include "../../RActors/RActors.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/Definitions/TutorialDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/Math/MathGeneral.h"
// Declared here because their headers are not adopted yet (UIChatBalloons.cpp, UIConsoleHUD.cpp, UI.cpp, UIUtils.cpp, SNOFiles.cpp).
namespace UIChatBalloons { void CancelBalloonForRActor(RActorID idRActor); void AddBalloon(RActorID idRActor, SNO snoStringList, uint32 hLabel, uint32 dwTicksToDisplay, float flHeightOffset, int32 nControllerIndex); }
namespace UIHUD { namespace Console { int32 PlayerGetHUDSlot(const Player * ptPlayer); } }
UIID UI_HitTest(const IVector2D & pt); void ConvertUICToPixels(const Vector2D & vecUIC, Vector2D & vecPixels); CRefString SNOFileGetName(const SNOName & snoname);









static const uint32 TUTORIAL_FADE_OUT_TIME = 60;
static const uint32 TUTORIAL_FADE_IN_TIME = 30;







void UITutorial::sInit()
{

    m_idActiveOwner = GetUIIDNone();
    m_idRActorOwner = RACTORID_INVALID;
    m_bAlpha = 0xff;

    m_dwFlags |= (1 << UIWINDOW_IGNORES_CLIPPING_BIT);
    m_dwFlags |= (1 << UIWINDOW_RESIZE_TO_ENCLOSE_CHILDREN_BIT);

    m_snoTutorial = SNO_NONE;
    m_fIsFreestanding = FALSE;
    m_fIsChatBalloon = FALSE;
    m_dwStartTime = 0;
    m_dwTimeBeforeFading = 0;


    m_eStyleOverride = TILEDSTYLE_NONE;
}




































UITutorial::UITutorial(const WindowCreationParams & tParams, const Rect2D * pRect)
    : UIWindow(tParams, pRect)
{


    sInit();
}




UITutorial::~UITutorial()
{
}




void UITutorial::SetTutorial(const Rect2D & parentRect, const RActorID idRActorOwner, SNO snoTutorial, const Player * ptPlayer)
{






    m_snoTutorial = snoTutorial;
    if (snoTutorial == SNO_NONE)
    {
        SetActive(FALSE);
        return;
    }

    SetActive(TRUE);


    if (m_fIsChatBalloon)
    {



        UIChatBalloons::CancelBalloonForRActor(m_idRActorOwner);
    }


    m_ParentRect = parentRect;
    m_idRActorOwner = idRActorOwner;

    m_bAlpha = 0xff;
    SetTransparency(m_bAlpha);

    ScopedTutorialDefinitionLockR pdefTutorial(snoTutorial);
    m_dwStartTime = CGameGetRealTicks();
    m_dwTimeBeforeFading = pdefTutorial->dwTimeBeforeFading;
    m_fIsFreestanding = (pdefTutorial->dwFlags & 1);
    const BOOL fUsesFreestandingUI = m_fIsFreestanding || pdefTutorial->eArrowPosition == TILEDSTYLE_TUTORIAL_NO_ARROW;
    m_fIsChatBalloon = (m_idRActorOwner != RACTORID_INVALID);

    TiledStyle eTiledStyle = (TiledStyle)pdefTutorial->eArrowPosition;


    if (pdefTutorial->dwFlags & 0x20)
    {
        switch (UIHUD::Console::PlayerGetHUDSlot(ptPlayer))
        {
        case 0: m_eStyleOverride = TILEDSTYLE_TUTORIAL_BL; break;
        case 1: m_eStyleOverride = TILEDSTYLE_TUTORIAL_BR; break;
        case 2: m_eStyleOverride = TILEDSTYLE_TUTORIAL_TL; break;
        case 3: m_eStyleOverride = TILEDSTYLE_TUTORIAL_TR; break;
        }












        eTiledStyle = m_eStyleOverride;
    }
    else
    {
        m_eStyleOverride = TILEDSTYLE_NONE;
    }


    CRefString sLabelBase = SNOFileGetName(SNOName(TUTORIAL_SNO, snoTutorial));
    const SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_TUTORIALS);

    SetParentID(GetUIIDNone());
    UIWindow & tWndAnchored = UI_GetWindowFromID(UIIDCreateFromParent(GetID(), ".anchored"));


    UIWindow & tWndFreestanding = UI_GetWindowFromID(UIIDCreateFromParent(GetID(), ".floating"));




    tWndFreestanding.SetActive(fUsesFreestandingUI ? !m_fIsChatBalloon : FALSE);
    tWndAnchored.SetActive(fUsesFreestandingUI ? FALSE : !m_fIsChatBalloon);



    const UIJustifyHorizontal eJustifyHorizontal = (pdefTutorial->dwFlags & 0x100) ? 1 : 2;
    const UIJustifyVertical eJustifyVertical = 2;



    if (m_fIsChatBalloon)
    {
        const StringLabelHandle hLabel = StringListGetLabelHandle(sLabelBase.str());

        UIChatBalloons::AddBalloon(m_idRActorOwner, snoStringList, hLabel, m_dwTimeBeforeFading, (float)pdefTutorial->vecOffset.y, ptPlayer->nControllerIndex);
    }
    else if (!fUsesFreestandingUI)
    {












        {
            UIText & tTextTitle = UI_GetTextFromID(UIIDCreateFromParent(GetID(), ".anchored.stack.wrapper.title"));
            CRefString sLabel = sLabelBase + CRefString("_title");
            tTextTitle.SetText(StringListFetch(snoStringList, sLabel.str()), FALSE);
        }
        {
            UIText & tTextBody = UI_GetTextFromID(UIIDCreateFromParent(GetID(), ".anchored.stack.wrapper.body"));
            if (ptPlayer)
            {
                tTextBody.SetControllerIndexOverride(ptPlayer->nControllerIndex);
            }

            tTextBody.SetText(StringListFetch(snoStringList, sLabelBase.str()), FALSE);
            tTextBody.SetJustification(eJustifyHorizontal, eJustifyVertical);
        }






        tWndAnchored.SetTiledStyle(eTiledStyle);
    }
    else
    {

















        {
            UIText & tTextTitle = UI_GetTextFromID(UIIDCreateFromParent(GetID(), ".floating.stack.wrapper.title"));
            CRefString sLabel = sLabelBase + CRefString("_title");
            tTextTitle.SetText(StringListFetch(snoStringList, sLabel.str()), FALSE);
        }
        {
            UIText & tTextBody = UI_GetTextFromID(UIIDCreateFromParent(GetID(), ".floating.stack.wrapper.body"));
            if (ptPlayer)
            {
                tTextBody.SetControllerIndexOverride(ptPlayer->nControllerIndex);
            }

            tTextBody.SetText(StringListFetch(snoStringList, sLabelBase.str()), FALSE);
            tTextBody.SetJustification(eJustifyHorizontal, eJustifyVertical);
        }
        tWndFreestanding.SetTiledStyle(TILEDSTYLE_TUTORIAL_NO_ARROW);
    }


    Update();
}


void UITutorial::SetTutorial(const UIID & idParent, const SNO snoTutorial, const Player * ptPlayer)
{



    if (UI_TestID(idParent))
    {




        Rect2D parentRect;
        UI_GetWindowFromID(idParent).GetWindowRect(parentRect);

        SetTutorial(parentRect, RACTORID_INVALID, snoTutorial, ptPlayer);
        SetParentID(idParent);
    }
}



void UITutorial::SetParentID(const UIID & idParent)
{

    m_idActiveOwner = idParent;
}




BOOL UITutorial::OwnerIsMissing() const
{


    if (m_fIsChatBalloon)
    {
        return m_idRActorOwner == RACTORID_INVALID || RActorTryToGet(m_idRActorOwner) == NULL;
    }
    if (m_idActiveOwner == GetUIIDNone())
    {
        return FALSE;
    }



    if (!UI_TestID(m_idActiveOwner))
    {
        return TRUE;
    }

    UIWindow & tParentWindow = UI_GetWindowFromID(m_idActiveOwner);
    if (!tParentWindow.IsActive())
    {
        return TRUE;
    }



    if (!m_fIsFreestanding)
    {

        Rect2D rectParent;
        tParentWindow.GetWindowRect(rectParent);
        Vector2D vecRectCenter = rectParent.Center();
        Vector2D vecRecCenterPixels;

        ConvertUICToPixels(vecRectCenter, vecRecCenterPixels);

        IVector2D tCenterPixels(Math::FloatToInt(vecRecCenterPixels.x), Math::FloatToInt(vecRecCenterPixels.y));

        UIID idVisibleControl = UI_HitTest(tCenterPixels);
        if (idVisibleControl != GetUIIDNone() && idVisibleControl != m_idActiveOwner)
        {
            UIWindow & tVisibleWindow = UI_GetWindowFromID(idVisibleControl);



            if (!tVisibleWindow.HasParent(m_idActiveOwner) && !tParentWindow.HasParent(idVisibleControl) &&


                tVisibleWindow.GetWindowColor().a == 255)
            {
                return TRUE;
            }
        }
    }


    return FALSE;
}



void UITutorial::SetActive(const BOOL fActive)
{

    if (IsActive() == fActive)
    {
        return;
    }

    if (!fActive)
    {
        if (m_fIsChatBalloon)
        {



            UIChatBalloons::CancelBalloonForRActor(m_idRActorOwner);
        }


        m_idActiveOwner = GetUIIDNone();
        m_idRActorOwner = RACTORID_INVALID;
    }
    UIWindow::SetActive(fActive);
}




void UITutorial::sUpdatePosition()
{

    if (!IsActive())
    {
        return;
    }



    ScopedTutorialDefinitionLockR pdefTutorial(m_snoTutorial);


    if ((pdefTutorial->dwFlags & 1) || pdefTutorial->eArrowPosition == TILEDSTYLE_TUTORIAL_NO_ARROW)
    {
        return;
    }













    UIWindow & tWndAnchored = UI_GetWindowFromID(UIIDCreateFromParent(GetID(), ".anchored"));
    Vector2D vecTutorialTL, vecTutorialDim, vecPos;
    tWndAnchored.GetRelativePosition(vecTutorialTL, vecTutorialDim);
    Rect2D rectParent;
    if (GetParentID() != GetUIIDNone())
    {
        UIWindow & tParent = UI_GetWindowFromID(GetParentID());
        tParent.GetWindowRect(rectParent);
    }
    else
    {
        GetParentRect(rectParent);
    }

    uint32 eTiledStyle = pdefTutorial->eArrowPosition;
    Vector2D vecRectCenter = rectParent.Center();

    if (pdefTutorial->dwFlags & 0x40)
    {
        rectParent.left = rectParent.right = vecRectCenter.x;
    }

    if (pdefTutorial->dwFlags & 0x80)
    {
        rectParent.top = rectParent.bottom = vecRectCenter.y;
    }

    Vector2D vecUIC((float)pdefTutorial->vecOffset.x, (float)pdefTutorial->vecOffset.y);




    if (m_eStyleOverride != TILEDSTYLE_NONE)
    {
        switch (m_eStyleOverride)
        {
        case TILEDSTYLE_TUTORIAL_TL:
            vecUIC.y = -vecUIC.y;
            break;
        case TILEDSTYLE_TUTORIAL_TR:
            vecUIC.x = -vecUIC.x;
            vecUIC.y = -vecUIC.y;
            break;
        case TILEDSTYLE_TUTORIAL_BR:
            vecUIC.x = -vecUIC.x;
            break;
        }
        eTiledStyle = m_eStyleOverride;
    }





    switch (eTiledStyle)
    {
    case TILEDSTYLE_TUTORIAL_TL:
        vecPos.Set(rectParent.right, rectParent.bottom);
        break;
    case TILEDSTYLE_TUTORIAL_TR:
        vecPos.Set(rectParent.left - vecTutorialDim.x, rectParent.bottom);
        break;
    case TILEDSTYLE_TUTORIAL_BL:
        vecPos.Set(rectParent.right, rectParent.top - vecTutorialDim.y);
        break;
    case TILEDSTYLE_TUTORIAL_BR:
        vecPos.Set(rectParent.left - vecTutorialDim.x, rectParent.top - vecTutorialDim.y);
        break;
    }

    vecPos += vecUIC;
    tWndAnchored.SetRelativePosition(vecPos, vecTutorialDim);
}





void UITutorial::Update()
{

    sUpdatePosition();

    m_bAlpha = 0xff;
    float flAlpha;

    const uint32 dwElapsedTime = CGameGetRealTicks() - m_dwStartTime;


    if (m_dwTimeBeforeFading && dwElapsedTime >= m_dwTimeBeforeFading)
    {

        const uint32 dwTimeIntoFade = dwElapsedTime - m_dwTimeBeforeFading;
        if (dwTimeIntoFade > TUTORIAL_FADE_OUT_TIME) {

            SetActive(FALSE); }


        flAlpha = 1.0f - (float)dwTimeIntoFade / (float)TUTORIAL_FADE_OUT_TIME;
    }
    else if (dwElapsedTime < TUTORIAL_FADE_IN_TIME)
    {
        const uint32 dwTimeIntoFade = TUTORIAL_FADE_IN_TIME - dwElapsedTime;
        flAlpha = 1.0f - (float)dwTimeIntoFade / (float)TUTORIAL_FADE_IN_TIME;
    }
    else { flAlpha = 1.0f; }
    flAlpha = Math::Pin(flAlpha, 0.0f, 1.0f);
    m_bAlpha = (uint8)Math::FloatToInt(flAlpha * 255.0f);
    SetTransparency(m_bAlpha);
}



void UITutorial::Draw(const uint8, float & flZSlice)
{


    if (m_bAlpha)
    {



        UIWindow::Draw(m_bAlpha, flZSlice);
    }
}
