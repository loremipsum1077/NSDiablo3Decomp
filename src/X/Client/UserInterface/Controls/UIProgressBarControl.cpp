#include "UIProgressBarControl.h"

#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../CGame.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "UIRActorControl.h"

// Declarations of the functions this file calls whose headers are not adopted yet;
// the DWARF gives their signatures but not where they were declared.
#include "../../Input/Input.h"
typedef int32 InputMsg;






#include "../../TexAnimation.h"



// X/Client/RActors/RActors.h:278 and :596. Only the members this file touches are declared.
struct RActor {  // size 0x410
    RActorID id;  // 0x00
    uint8 _unadopted0[0x3d4 - 0x04];  // 0x04 (invented name: idSScene .. ePlayerClass)
    ObjectTexAnimParam tObjTexAnimParam;  // 0x3d4
    uint8 _unadopted1[0x410 - 0x3d8];  // 0x3d8 (invented name: dwDebugFlags .. end)
};
static_assert(sizeof(RActor) == 0x410, "RActor");

inline RActor * RActorTryToGet(const RActorID idRActor) {  // line 596
    if (idRActor == RACTORID_INVALID)
        return NULL;
    CGame & tCGame = CGameGet();
    if (!tCGame.ptRActors)
        return NULL;
    return tCGame.ptRActors->ElementTryToGet(idRActor);
}






// X/Client/UserInterface/UI.cpp
UI & UIGet();
UIID UIIDCreateFromControlAttribute(const UIControlSourceKey & tSrcKey, const UIID & id, const UIAttributeType eAttributeType);
UIWindow & UI_GetWindowFromID(const UIID & id);
UIRActor & UI_GetRActorFromID(const UIID & id);
void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);
// X/Client/UserInterface/UIUtils.cpp
enum AlignmentType {
    ALIGN_CENTER_VERTICAL = 0,
    ALIGN_CENTER_HORIZONTAL = 1,
    ALIGN_CENTER_HORIZONTAL_VERTICAL = 2,
};
void AlignRect(const Vector2D & vecSourceTL, const Vector2D & vecSourceDim, const AlignmentType eAlignmentType, const float flStart, const Vector2D & vecToAlignDim, Vector2D & vecTL);

inline void ProgressBarCreationParams::sInit()
{
    flMinPos = 0.0f;
    flMaxPos = 0.0f;
    fIsVertical = FALSE;
    fDrawReversed = FALSE;
    tImageTexProgressBackground = cgtImageTextureFrameNull;
    tImageTexProgressForeground = cgtImageTextureFrameNull;
}

ProgressBarCreationParams::ProgressBarCreationParams(const UIType eType, const UIID & id, const UIID & idParent)
    : WindowCreationParams(eType, id, idParent)
{
    sInit();
}

void ProgressBarCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{
    WindowCreationParams::Load(snoUI, pControlData);

    fIsVertical = pControlData->GetBoolValue(ATT_VERTICAL);
    fDrawReversed = pControlData->GetBoolValue(ATT_PROGRESS_REVERSED);
    flMinPos = pControlData->GetFloat32Value(ATT_PROGRESS_MIN);
    flMaxPos = pControlData->GetFloat32Value(ATT_PROGRESS_MAX);
    pControlData->GetImageAttribute(ATT_PROGRESS_BACKGROUND_IMAGE, tImageTexProgressBackground);
    pControlData->GetImageAttribute(ATT_PROGRESS_FOREGROUND_IMAGE, tImageTexProgressForeground);
}

void UIProgressBar::sInit(const ProgressBarCreationParams & tParams, const Vector2D &, const Vector2D & vecDim)
{
    m_bUseCustomExtents = FALSE;
    m_flCustomMin = 0.0f;
    m_flCustomMax = 0.0f;
    m_fIsVertical = tParams.fIsVertical;
    m_fDrawReversed = tParams.fDrawReversed;
    m_flMinPos = tParams.flMinPos;
    m_flCurrentPos = tParams.flMaxPos;
    m_flMaxPos = m_flCurrentPos;
    m_flTargetPos = m_flCurrentPos;

    const UIID & id = GetID();
    const UIControlSourceKey & tSrcKey = GetUISrcKey();
    m_idLeftEndcapChild = UIIDCreateFromControlAttribute(tSrcKey, id, ATT_PROGRESS_LEFT_ENDCAP);
    m_idRightEndcapChild = UIIDCreateFromControlAttribute(tSrcKey, id, ATT_PROGRESS_RIGHT_ENDCAP);
    m_idTargetChild = UIIDCreateFromControlAttribute(tSrcKey, id, ATT_PROGRESS_TARGET);

    m_idBackgroundChild = GetUIIDNone();
    if (tParams.tImageTexProgressBackground.snoTexture != SNO_NONE)
    {
        WindowCreationParams tWindowParams(UIT_WINDOW, "background", id);
        tWindowParams.tImageTexBkgrnd = tParams.tImageTexProgressBackground;
        UIWindow & tBackground = *new UIWindow(tWindowParams, Vector2D(0.0f, 0.0f), vecDim);
        m_idBackgroundChild = tBackground.GetID();
    }

    m_idForegroundChild = UIIDCreateFromControlAttribute(tSrcKey, id, ATT_PROGRESS_FOREGROUND);
    if (m_idForegroundChild == GetUIIDNone() && tParams.tImageTexProgressForeground.snoTexture != SNO_NONE)
    {
        WindowCreationParams tWindowParams(UIT_WINDOW, "foreground", id);
        tWindowParams.tImageTexBkgrnd = tParams.tImageTexProgressForeground;
        UIWindow & tForeground = *new UIWindow(tWindowParams, Vector2D(0.0f, 0.0f), vecDim);
        m_idForegroundChild = tForeground.GetID();
    }
}

UIProgressBar::UIProgressBar(const ProgressBarCreationParams & tParams, const Rect2D * pRect)
    : UIWindow(tParams, pRect)
{
    Vector2D vecTL;
    Vector2D vecDim;
    sRelativeCoordsFromCreationParams(vecTL, vecDim, pRect, tParams);
    sInit(tParams, vecTL, vecDim);
}

UIProgressBar::~UIProgressBar()
{
    UI_DeleteControl(m_idBackgroundChild, TRUE);
    UI_DeleteControl(m_idForegroundChild, TRUE);
}

inline float UIProgressBar::sGetMinPos()
{
    if (m_bUseCustomExtents)
        return m_flCustomMin;
    return m_flMinPos;
}

inline float UIProgressBar::sGetMaxPos()
{
    if (m_bUseCustomExtents)
        return m_flCustomMax;
    return m_flMaxPos;
}

// On DWARF lines (#line): the deviations (283/284) precede GetRelativePosition (287); each
// m_fDrawReversed choice is `if (!m_fDrawReversed)` (columns 7/8 at 317/368, 8/9 at 333).
#line 203
void UIProgressBar::sOnReposition()

{




	UIControl * pControl;
	if (!UIGet().pmapElements->Lookup(m_idForegroundChild, pControl) || !pControl)
		return;




	const float flPos = GetPos();
	const float flMin = sGetMinPos();
	const float flMax = sGetMaxPos();
	const float flTarget = GetTargetPos();

	if (pControl->IsType(UIT_RACTOR))
	{

		UIRActor & tRActorForeground = UI_GetRActorFromID(m_idForegroundChild);
		RActor * ptRActor = RActorTryToGet(tRActorForeground.GetRActor());
		if (ptRActor)
		{
			if (flMax < EPSILON)
				ptRActor->tObjTexAnimParam.flValue = 0.0f;
			else



				ptRActor->tObjTexAnimParam.flValue = Math::Pin(flPos / flMax, 0.0f, 1.0f);
		}


		if (m_idTargetChild != GetUIIDNone())
		{
			UIRActor & tRActorTarget = UI_GetRActorFromID(m_idTargetChild);
			RActor * ptRActor = RActorTryToGet(tRActorTarget.GetRActor());
			if (ptRActor)
			{
				if (flMax < EPSILON)
					ptRActor->tObjTexAnimParam.flValue = 0.0f;
				else



					ptRActor->tObjTexAnimParam.flValue = Math::Pin(flTarget / flMax, 0.0f, 1.0f);
			}
		}

		return;
	}


	UIWindow & tWindowForeground = static_cast<UIWindow &>(*pControl);

	const float flDelta = flMax - flMin;
	if (flPos - flMin < EPSILON || flDelta < EPSILON)
	{


		tWindowForeground.SetRelativeClipRect(Vector2D(0.0f, 0.0f), Vector2D(0.0f, 0.0f));
		if (m_idLeftEndcapChild != GetUIIDNone())

			UI_GetWindowFromID(m_idLeftEndcapChild).SetRelativeClipRect(Vector2D(0.0f, 0.0f), Vector2D(0.0f, 0.0f));

		if (m_idRightEndcapChild != GetUIIDNone())

			UI_GetWindowFromID(m_idRightEndcapChild).SetRelativeClipRect(Vector2D(0.0f, 0.0f), Vector2D(0.0f, 0.0f));

		if (m_idTargetChild != GetUIIDNone())

			UI_GetWindowFromID(m_idTargetChild).SetRelativeClipRect(Vector2D(0.0f, 0.0f), Vector2D(0.0f, 0.0f));

		return;
	}


	const float flDeviation = (flPos - flMin) / flDelta;
	const float flTargetDeviation = (flTarget - flMin) / flDelta;

	Vector2D vecTL, vecDim;
	GetRelativePosition(vecTL, vecDim);

	if (m_idLeftEndcapChild != GetUIIDNone())
	{
		UIWindow & tWindow = UI_GetWindowFromID(m_idLeftEndcapChild);
		Vector2D vecEndcapTL, vecEndcapDim;
		tWindow.GetRelativePosition(vecEndcapTL, vecEndcapDim);
		tWindow.SetRelativeClipRect(vecEndcapTL, vecEndcapDim);
	}

	Vector2D vecTargetTL, vecTargetDim;
	if (m_idTargetChild != GetUIIDNone())
	{
		UIWindow & tWindow = UI_GetWindowFromID(m_idTargetChild);
		tWindow.GetRelativePosition(vecTargetTL, vecTargetDim);
		tWindow.SetRelativeClipRect(vecTargetTL, vecTargetDim);
	}

	if (m_idBackgroundChild != GetUIIDNone())

		UI_GetWindowFromID(m_idBackgroundChild).SetRelativePosition(Vector2D(0.0f, 0.0f), vecDim);

	tWindowForeground.SetRelativePosition(Vector2D(0.0f, 0.0f), vecDim);

	Vector2D vecForegroundClipTL, vecForegroundClipDim;
	Vector2D vecTargetClipTL, vecTargetClipDim;
	if (m_fIsVertical)
	{
		vecForegroundClipDim.Set(vecDim.x, flDeviation * vecDim.y);
		vecForegroundClipTL.x = 0.0f;
		if (!m_fDrawReversed)
		{

			vecForegroundClipTL.y = vecDim.y - vecForegroundClipDim.y;
		}
		else
		{
			vecForegroundClipTL.y = 0.0f;
		}



		if (m_idTargetChild != GetUIIDNone())
		{
			vecTargetClipDim.Set(vecTargetDim.x, flTargetDeviation * vecTargetDim.y);
			vecTargetClipTL.x = vecTargetTL.x;
			if (!m_fDrawReversed)
			{

				vecTargetClipTL.y = vecTargetTL.y + vecTargetDim.y - vecTargetClipDim.y;
			}
			else
			{
				vecTargetClipTL.y = vecTargetTL.y;
			}
		}


		if (m_idRightEndcapChild != GetUIIDNone())
		{
			UIWindow & tRightEndCap = UI_GetWindowFromID(m_idRightEndcapChild);
			Vector2D vecRightEndcapTL, vecRightEndcapDim;
			tRightEndCap.GetRelativePosition(vecRightEndcapTL, vecRightEndcapDim);
			Vector2D vecClippingDim = vecRightEndcapDim;
			vecClippingDim.y = Math::Min(vecRightEndcapDim.y, vecForegroundClipDim.y);
			tRightEndCap.SetRelativeClipRect(vecRightEndcapTL, vecClippingDim);
			Rect2D rectWindow;
			GetWindowRect(rectWindow);
			Vector2D vecNewPos;
			const float yPos = m_fDrawReversed ? rectWindow.top + vecForegroundClipDim.y - vecRightEndcapDim.y : rectWindow.bottom - vecForegroundClipDim.y;
			AlignRect(Vector2D(rectWindow.left, rectWindow.top), vecDim, ALIGN_CENTER_HORIZONTAL, yPos, vecRightEndcapDim, vecNewPos);
			Rect2D rectRightEndCap;
			tRightEndCap.GetWindowRect(rectRightEndCap);
			tRightEndCap.Move(vecNewPos - Vector2D(rectRightEndCap.left, rectRightEndCap.top));
		}
	}
	else
	{
		vecForegroundClipDim.Set(flDeviation * vecDim.x, vecDim.y);
		vecForegroundClipTL.y = 0.0f;

		if (!m_fDrawReversed)
		{
			vecForegroundClipTL.x = 0.0f;
		}
		else
		{


			vecForegroundClipTL.x = vecDim.x - vecForegroundClipDim.x;
		}


		if (m_idTargetChild != GetUIIDNone())
		{
			vecTargetClipTL = vecTargetTL;
			vecTargetClipDim.Set(flTargetDeviation * vecTargetDim.x, vecTargetDim.y);
		}

		if (m_idRightEndcapChild != GetUIIDNone())
		{
			UIWindow & tRightEndCap = UI_GetWindowFromID(m_idRightEndcapChild);
			Vector2D vecRightEndcapTL, vecRightEndcapDim;
			tRightEndCap.GetRelativePosition(vecRightEndcapTL, vecRightEndcapDim);
			Vector2D vecClippingDim = vecRightEndcapDim;
			vecClippingDim.x = Math::Min(vecRightEndcapDim.x, vecForegroundClipDim.x);
			tRightEndCap.SetRelativeClipRect(vecRightEndcapTL, vecClippingDim);
			Rect2D rectWindow;
			GetWindowRect(rectWindow);
			Vector2D vecNewPos;
			const float xPos = m_fDrawReversed ? rectWindow.right - vecForegroundClipDim.x : rectWindow.left + vecForegroundClipDim.x - vecRightEndcapDim.x;


			AlignRect(Vector2D(rectWindow.left, rectWindow.top), vecDim, ALIGN_CENTER_VERTICAL, xPos, vecRightEndcapDim, vecNewPos);
			Rect2D rectRightEndCap;
			tRightEndCap.GetWindowRect(rectRightEndCap);
			tRightEndCap.Move(vecNewPos - Vector2D(rectRightEndCap.left, rectRightEndCap.top));
		}
	}

	tWindowForeground.SetRelativeClipRect(vecForegroundClipTL, vecForegroundClipDim);
	if (m_idTargetChild != GetUIIDNone())

		UI_GetWindowFromID(m_idTargetChild).SetRelativeClipRect(vecTargetClipTL, vecTargetClipDim);

}
#line 313

void UIProgressBar::SetPos(const float flPos, const float flTarget)
{
    const BOOL fPosChanged = Math::Abs(m_flCurrentPos - flPos) > EPSILON;
    if (!fPosChanged && (flTarget == -1.0f || Math::Abs(m_flTargetPos - flTarget) < EPSILON))
        return;

    m_flCurrentPos = flPos;
    m_flTargetPos = flTarget < 0.0f ? flPos : flTarget;
    sOnReposition();
}

void UIProgressBar::PostCreate()
{
    UIWindow::PostCreate();

    if (m_idTargetChild != GetUIIDNone())
        UI_GetWindowFromID(m_idTargetChild).SetShader(GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT));
}

BOOL UIProgressBar::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{
    switch (msg)
    {
    case UI_INPUT:
    {
        BufferedInput * tInput = (BufferedInput *)dwParam;
        if (tInput->eInputMsg == INPUTMSG_LEFTMOUSEUP)
        {
            if (EatsClicks() == EC_IGNORE)
                return FALSE;

            if (m_pFunctionsPtr && m_pFunctionsPtr->pfnOnClick)
                m_pFunctionsPtr->pfnOnClick(GetID());
        }
        break;
    }

    case UI_INIT_FOR_WORLD:
        UIWindow::HandleMessage(msg, dwParam);
        sOnReposition();
        return FALSE;

    default:
        break;
    }

    return UIWindow::HandleMessage(msg, dwParam);
}

void UIProgressBar::SetExtents(const float flMin, const float flMax)
{
    m_bUseCustomExtents = TRUE;

    if (Math::Abs(m_flCustomMin - flMin) > EPSILON || Math::Abs(m_flCustomMax - flMax) > EPSILON)
    {
        m_flCustomMin = flMin;
        m_flCustomMax = flMax;
        sOnReposition();
    }
}

void UIProgressBar::sSetRActor(const UIID & id, const SNO snoActor, const RenderContext eContext)
{
    if (id == GetUIIDNone())
        return;

    if (!UI_GetControlFromID(id).IsType(UIT_RACTOR))
        return;

    UIRActor & tUIRActor = UI_GetRActorFromID(id);
    tUIRActor.CreateRActor(eContext, snoActor);
    sOnReposition();
}

UIID UIProgressBar::HitTestForTooltips(const IVector2D & pt, BOOL &)
{
    if (!IsActive() || !IsInside(pt))
        return GetUIIDNone();

    LPCSTR pszTooltipText = GetTooltipText();
    if (pszTooltipText && pszTooltipText[0])
        return GetID();

    return GetUIIDNone();
}

BOOL UIProgressBar::UpdateCachedActiveState()
{
    const BOOL fWasActive = UIWindow::UpdateCachedActiveState();
    if (IsActive() && fWasActive != IsActive())
        sOnReposition();
    return fWasActive;
}
