#include "UIWindowControl.h"

#include <string.h>
#include <strings.h>

#include "../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Sigma/ErrorManager/ErrorManager.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../CGame.h"
#include "../../Definitions/TextureDefinition.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "UIEditControl.h"

// Declarations of the functions this file calls whose headers are not adopted yet;
// the DWARF gives their signatures but not where they were declared.
#include "../../Input/Input.h"
typedef int32 InputMsg;
























// X/Client/Input/Input.cpp
BOOL InputIsController(const BufferedInput & tInput);
BOOL InputIsGesture(const BufferedInput & tInput);
// X/Client/RActors/RActors.h:278 and :596. Only the members this file touches are declared.
struct RActor {  // size 0x410
    RActorID id;  // 0x00
    uint8 _unadopted0[0x410 - 0x04];  // 0x04 (invented name: idSScene .. end)
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
// X/Client/RActors/RActors.cpp
void RActorGetTopPoint(const RActor * ptRActor, Vector3D & wpTop);
// X/Client/Camera.cpp
BOOL WorldPointToScreenPointUIC(const Vector3D & wp, Vector2D & wpUIC, const int32 nCameraIndex);

















// X/Client/UserInterface/UI.cpp
UI & UIGet();
BOOL UI_TestID(const UIID & id);
UIWindow & UI_GetWindowFromID(const UIID & id);
UIEdit & UI_GetEditFromID(const UIID & id);
void UI_RegisterInputListener(UIControl * pControl);
// X/Client/UserInterface/Interface/UIFunctions.cpp
UIFunctionPtr UIFunction_Lookup(LPCSTR szFunction);
UIMsgFunctionPtr UIMsgFunction_Lookup(LPCSTR szFunction);
UIConsoleFunctionPtr UIConsoleFunction_Lookup(LPCSTR szFunction);
// X/Client/UserInterface/UIUtils.cpp
void ConvertUICToPixels(const Rect2D & rectUIC, Rect2D & rectPixels);
void ConvertUICToPixelsDoNotSnap(const Rect2D & rectUIC, Rect2D & rectPixels);
void ConvertPixelsToUIC(const IVector2D & vecPixels, Vector2D & vecUIC);
void ConvertPixelsToUIC(const Vector2D & vecPixels, Vector2D & vecUIC);
// X/Client/Gfx/ImageCollection.cpp
BOOL ImageCollectionGetImageInfo(LPCSTR szDecoratedLabel, ImageTextureFrame & tImageTexFrame);
// X/Client/Gfx/Texture.cpp
void TextureGetFrameRectPixels(IRect2D & rectTexturePixels, const SNO snoTexture, const uint32 dwFrameIndex);
void TextureGetFrameRectUIScaledPixels(Rect2D & rectTexturePixels, const SNO snoTexture, const uint32 dwFrameIndex);
void TextureGetFrameRectUVs(Rect2D & rectTextureUVs, const TextureDefinition * pdefTexture, const uint32 dwFrameIndex);
void TextureDefinitionPixelLookup(const SNO snoTexture, const float flU, const float flV, const BOOL fLinearFilter, RGBAColor & rgbaColor);
// X/Client/Gfx/Gfx.cpp
void GfxGetCurrentDimensions(uint32 & dwWidth, uint32 & dwHeight);
float GfxGetAspectRatio();
// X/Client/Sprite.cpp
void SpriteDrawDeferred(const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
void SpriteDrawDeferred(const ImageTextureFrame & tImageTexFrame, const ImageTextureFrame & tImageTexFrameMask, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const Rect2D * const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
void SpriteDrawDeferredRotated(const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const Angle aYaw, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride);
void SpriteDrawDeferredMirrored(const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const BOOL bMirrorX, const BOOL bMirrorY, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride);
void SpriteDrawSimple(const SNO snoTexture, const SNO snoShader, const Rect2D & wrScreen, const Vector2D * wpUVs, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const RenderLayer eRenderLayerOverride);
// X/Client/Animation2D.cpp
void Animation2DSet(AnimState & tAnimState, SNO & snoAnim2D, const SNO snoAnim2DNew, const BOOL fForceReset);
void Animation2DDraw(const SNO snoAnim2D, const AnimState & tAnimState, const Rect2D & wrRect, const Rect2D & wrRectClipping, const uint8 bAlpha, const float flZ, const RGBAColor & rgbaColor, const SNO snoShader, const RenderLayer eRenderLayerOverride);
void Animation2DUpdate(AnimState & tAnimState, const SNO snoAnim2D, const float flDeltaTime);
void Animation2DReset(AnimState & tAnimState, const SNO snoAnim2D);
// X/Client/Prefetch.h
typedef int32 SNOFilesAsyncPriority;
namespace Prefetch {
BOOL AddToQueue(const SNOName & snoname, SNOFilesAsyncPriority ePriority);
}  // namespace Prefetch
// X/Common/StringList.cpp
LPCSTR StringListFetchLPCSTR(const SNO snoStringList, LPCSTR szLabel);
// APP_DRAW_NO_UI_BIT comes from X/Common/Game.h.



// X/Client/App.cpp
BOOL AppDrawFlagIsSet(int32 nFlag);
// X/Client/UserInterface/Controls/UIControlControl.cpp
extern uint32 s_controlDrawCount;

UIAttributeType sgtConsoleButtonAttributes[18] = {
    ATT_ONBUTTON1, ATT_ONBUTTON2, ATT_ONBUTTON3, ATT_ONBUTTON4,
    ATT_ONL1, ATT_ONR1, ATT_ONL2, ATT_ONR2,
    ATT_ONDPADUP, ATT_ONDPADDOWN, ATT_ONDPADLEFT, ATT_ONDPADRIGHT,
    ATT_ONBACK, ATT_ONSTART, ATT_ONL3, ATT_ONR3,
    ATT_ONTOUCHPADBUTTON, ATT_ONOPTIONSBUTTON,
};

UIAttributeType sgtConsoleButtonHeldAttributes[18] = {
    ATT_ONBUTTON1_HELD, ATT_ONBUTTON2_HELD, ATT_ONBUTTON3_HELD, ATT_ONBUTTON4_HELD,
    ATT_ONL1_HELD, ATT_ONR1_HELD, ATT_ONL2_HELD, ATT_ONR2_HELD,
    ATT_ONDPADUP_HELD, ATT_ONDPADDOWN_HELD, ATT_ONDPADLEFT_HELD, ATT_ONDPADRIGHT_HELD,
    ATT_ONBACK_HELD, ATT_ONSTART_HELD, ATT_ONL3_HELD, ATT_ONR3_HELD,
    ATT_ONTOUCHPADBUTTON_HELD, ATT_ONOPTIONSBUTTON_HELD,
};

const float UIDC_DIALOG_Z_INCREMENT = 0.0001f;
const float UIDC_DIALOG_Z_START = 0.9999f;
const float UIDC_CONTROL_Z_INCREMENT = 0.00001f;

struct WindowPieces {  // size 0x18
    GlobalSNO eBkgrnd;  // 0x00
    GlobalSNO eTopBorder;  // 0x04
    GlobalSNO eRightBorder;  // 0x08
    GlobalSNO eBottomBorder;  // 0x0c
    GlobalSNO eLeftBorder;  // 0x10
    GlobalSNO eCorner;  // 0x14
};

struct VertTiledWindowPieces {  // size 0x18
    LPCSTR szPieceTop;  // 0x00
    LPCSTR szPieceBottom;  // 0x08
    GlobalSNO eMiddle;  // 0x10
};

enum {
    TP_START = 0,
    TP_END = 1,
    TP_MIDDLE = 2,
    NUM_TRACKPIECES = 3,
};

struct TrackPieces {  // size 0x18
    LPCSTR szPieceFrame[NUM_TRACKPIECES];  // 0x00
};

struct TiledWindowPieces {  // size 0x38
    LPCSTR szCornerFrames[4];  // 0x00
    LPCSTR szBackground;  // 0x20
    GlobalSNO eTopBorder;  // 0x28
    GlobalSNO eRightBorder;  // 0x2c
    GlobalSNO eBottomBorder;  // 0x30
    GlobalSNO eLeftBorder;  // 0x34
};

static TiledWindowPieces scgtTiledPiecesDefault = {
    { "UI:FrameCornerUpperLeft", "UI:FrameCornerUpperRight", "UI:FrameCornerLowerRight", "UI:FrameCornerLowerLeft" },
    "UI:FrameCornerFill",
    GLOBALSNO_TILEDWINDOW_DEFAULT_BORDER_TOP, GLOBALSNO_TILEDWINDOW_DEFAULT_BORDER_RIGHT, GLOBALSNO_TILEDWINDOW_DEFAULT_BORDER_BOTTOM, GLOBALSNO_TILEDWINDOW_DEFAULT_BORDER_LEFT,
};

static TiledWindowPieces scgtTiledPiecesTextTooltips = {
    { "UI:TextWindowCorner_UpperLeft", "UI:TextWindowCorner_UpperRight", "UI:TextWindowCorner_LowerRight", "UI:TextWindowCorner_LowerLeft" },
    "UI:TextWindowCorner_Fill",
    GLOBALSNO_TILEDWINDOW_TEXTTOOLTIP_BORDER_TOP, GLOBALSNO_TILEDWINDOW_TEXTTOOLTIP_BORDER_RIGHT, GLOBALSNO_TILEDWINDOW_TEXTTOOLTIP_BORDER_BOTTOM, GLOBALSNO_TILEDWINDOW_TEXTTOOLTIP_BORDER_LEFT,
};

static TiledWindowPieces scgtTiledPiecesSkillTooltips = {
    { "UI:Frame2CornerUpperLeft", "UI:Frame2CornerUpperRight", "UI:Frame2CornerLowerRight", "UI:Frame2CornerLowerLeft" },
    "UI:FrameCornerFill",
    GLOBALSNO_TILEDWINDOW_SKILLTOOLTIP_BORDER_TOP, GLOBALSNO_TILEDWINDOW_SKILLTOOLTIP_BORDER_RIGHT, GLOBALSNO_TILEDWINDOW_SKILLTOOLTIP_BORDER_BOTTOM, GLOBALSNO_TILEDWINDOW_SKILLTOOLTIP_BORDER_LEFT,
};

static TiledWindowPieces scgtTiledPiecesChatBalloon = {
    { "UI:ChatBlackCornerUpperLeft", "UI:ChatBlackCornerUpperRight", "UI:ChatBlackCornerLowerRight", "UI:ChatBlackCornerLowerLeft" },
    "UI:ChatBlackFill",
    GLOBALSNO_TILEDWINDOW_CHATBALLOON_BORDER_TOP, GLOBALSNO_TILEDWINDOW_CHATBALLOON_BORDER_RIGHT, GLOBALSNO_TILEDWINDOW_CHATBALLOON_BORDER_BOTTOM, GLOBALSNO_TILEDWINDOW_CHATBALLOON_BORDER_LEFT,
};

static TrackPieces scgtPiecesArrowVertical = {
    { "UI:SkillTree_Arrow_Top", "UI:SkillTree_Arrow_Bottom", "UI:SkillTree_Arrow_Middle" },
};

static TrackPieces scgtPiecesItemTooltipBody = {
    { "UI:Tooltip_Frame_Tile_Top", "UI:Tooltip_Frame_Tile_Bottom", "UI:Tooltip_Frame_Tile_Middle" },
};

static TrackPieces scgtPiecesItemTooltipBody_Ancient = {
    { "UI:Tooltip_Frame_Tile_Top_Ancient", "UI:Tooltip_Frame_Tile_Bottom_Ancient", "UI:Tooltip_Frame_Tile_Middle_Ancient" },
};

static TrackPieces scgtPiecesItemTooltipBody_PrimalAncient = {
    { "UI:Tooltip_Frame_Tile_Top_Primal", "UI:Tooltip_Frame_Tile_Bottom_Primal", "UI:Tooltip_Frame_Tile_Middle_Primal" },
};

static TrackPieces scgtPiecesItemTooltipCost = {
    { "", "", "UI:Tooltip_Frame_Cost" },
};

static TrackPieces scgtPiecesItemTooltipCost_Ancient = {
    { "", "", "UI:Tooltip_Frame_Cost_Ancient" },
};

static TrackPieces scgtPiecesItemTooltipCost_PrimalAncient = {
    { "", "", "UI:Tooltip_Frame_Cost_Primal" },
};

static TrackPieces scgtPiecesItemTooltipInstruction = {
    { "", "UI:Tooltip_Frame_Instruction_Bottom", "UI:Tooltip_Frame_Instruction_Tile" },
};

static TrackPieces scgtPiecesItemTooltipInstruction_Ancient = {
    { "", "UI:Tooltip_Frame_Instruction_Bottom_Ancient", "UI:Tooltip_Frame_Instruction_Tile_Ancient" },
};

static TrackPieces scgtPiecesItemTooltipInstruction_PrimalAncient = {
    { "", "UI:Tooltip_Frame_Instruction_Bottom_Primal", "UI:Tooltip_Frame_Instruction_Tile_Primal" },
};

static TrackPieces scgtPiecesTraitTooltip = {
    { "UI:TraitList_Tooltip_Top", "UI:TraitList_Tooltip_Bottom", "UI:TraitList_Tooltip_Middle" },
};

static VertTiledWindowPieces scgtPiecesConfirmationDlg = {
    "UI:Confirmation_Top",
    "UI:Confirmation_Bottom",
    GLOBALSNO_TEXTURE_CONFIRMATION_DLG_MIDDLE,
};

static VertTiledWindowPieces scgtPiecesConversationDlg = {
    "UI:TopMenuTile_Top",
    "UI:TopMenuTile_Bottom",
    GLOBALSNO_TEXTURE_CONV_DLG_MIDDLE,
};

static VertTiledWindowPieces scgtPiecesFollowerSwapDlg = {
    "UI:FollowerSwap_Tile_Top",
    "UI:FollowerSwap_Tile_Bottom",
    GLOBALSNO_TEXTURE_FOLLOWER_MIDDLE,
};

static VertTiledWindowPieces scgtPiecesTutorialTL = {
    "UI:TutorialTooltip_TopLeft",
    "UI:TutorialTooltip_Bottom",
    GLOBALSNO_TEXTURE_TUTORIAL_MIDDLE,
};

static VertTiledWindowPieces scgtPiecesTutorialTR = {
    "UI:TutorialTooltip_TopRight",
    "UI:TutorialTooltip_Bottom",
    GLOBALSNO_TEXTURE_TUTORIAL_MIDDLE,
};

static VertTiledWindowPieces scgtPiecesTutorialBL = {
    "UI:TutorialTooltip_Top",
    "UI:TutorialTooltip_BottomLeft",
    GLOBALSNO_TEXTURE_TUTORIAL_MIDDLE,
};

static VertTiledWindowPieces scgtPiecesTutorialBR = {
    "UI:TutorialTooltip_Top",
    "UI:TutorialTooltip_BottomRight",
    GLOBALSNO_TEXTURE_TUTORIAL_MIDDLE,
};

static VertTiledWindowPieces scgtPiecesTutorialNoArrow = {
    "UI:TutorialTooltip_Top",
    "UI:TutorialTooltip_Bottom",
    GLOBALSNO_TEXTURE_TUTORIAL_MIDDLE,
};

static VertTiledWindowPieces scgtPiecesBoss = {
    "UI:BossWindow_Top",
    "UI:BossWindow_Bottom",
    GLOBALSNO_TEXTURE_BOSSWINDOW_MIDDLE,
};

static VertTiledWindowPieces scgtPiecesTooltipTitle = {
    "UI:Tooltip_Frame_Title_Top",
    "UI:Tooltip_Frame_Title_Bottom",
    GLOBALSNO_TEXTURE_TOOLTIP_TITLE_MIDDLE,
};

static VertTiledWindowPieces scgtPiecesTooltipTitle_Ancient = {
    "UI:Tooltip_Frame_Title_Top_Ancient",
    "UI:Tooltip_Frame_Title_Bottom_Ancient",
    GLOBALSNO_TEXTURE_TOOLTIP_TITLE_MIDDLE_ANCIENT,
};

static VertTiledWindowPieces scgtPiecesTooltipTitle_PrimalAncient = {
    "UI:Tooltip_Frame_Title_Top_Primal",
    "UI:Tooltip_Frame_Title_Bottom_Primal",
    GLOBALSNO_TEXTURE_TOOLTIP_TITLE_MIDDLE_ANCIENT_PRIMAL,
};

static const WindowPieces scgtWindowPieces[TILEDSTYLE_COUNT] = {
    { GLOBALSNO_GLOBALS, GLOBALSNO_GLOBALS, GLOBALSNO_GLOBALS, GLOBALSNO_GLOBALS, GLOBALSNO_GLOBALS, GLOBALSNO_GLOBALS },
    { GLOBALSNO_TILEDEDITBOX_DEFAULT_BKGRND, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_CORNER },
    { GLOBALSNO_TILEDEDITBOX_DEFAULT_BKGRND, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_CORNER },
    { GLOBALSNO_TILEDEDITBOX_DEFAULT_BKGRND, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_BORDER, GLOBALSNO_TILEDEDITBOX_DEFAULT_CORNER },
    { GLOBALSNO_INVALID, GLOBALSNO_INVALID, GLOBALSNO_TILEDWINDOW_DEFAULT_BORDER, GLOBALSNO_INVALID, GLOBALSNO_TILEDWINDOW_DEFAULT_BORDER, GLOBALSNO_INVALID },
};

struct CornerData {  // size 0x18
    ImageTextureFrame tImageTexFrame;  // 0x00
    Rect2D rectPixels;  // 0x08
};

struct TrackPieceData {  // size 0x18
    ImageTextureFrame tImageTexFrame;  // 0x00
    Rect2D rectUIC;  // 0x08
};

BOOL WindowCreationParams::HasAnyFunctionsPtr() const
{
    if (pfnOnClick != NULL || pfnOnRightClick != NULL || pfnOnDoubleClick != NULL || pfnOnLButtonStillDown != NULL ||
        pfnTooltip != NULL || pfnOnDragStart != NULL || pfnOnDrag != NULL || pfnOnContextMenu != NULL)
    {
        return TRUE;
    }

    for (int32 i = 0; i < 18; i++)
    {
        if (pfnConsoleButtonDown[i] != NULL || pfnConsoleButtonHeld[i] != NULL)
        {
            return TRUE;
        }
    }

    if (pfnLStickHandler != NULL || pfnRStickHandler != NULL)
    {
        return TRUE;
    }

    return FALSE;
}

inline void WindowCreationParams::sInit()
{
    bAlpha = 255;
    snoCustomShader = SNO_NONE;
    eRenderLayerOverride = RENDERLAYER_INVALID;
    snoAnim2D = SNO_NONE;
    tImageTexBkgrnd = cgtImageTextureFrameNull;
    tImageTexMask = cgtImageTextureFrameNull;
    eHorizontalAnchor = UI_ANCHOR_CENTER;
    eVerticalAnchor = UI_VERT_ANCHOR_CENTER;
    dwFlags = 0;
    eTiledStyle = TILEDSTYLE_NONE;
    eEatsClicks = EC_IGNORE;
    szContextMenu[0] = '\0';
    szTargetParentContextMenu[0] = '\0';
    szTooltipLabel[0] = '\0';
    eTooltipTextJustify = UIJH_CENTER;
    vecStandardDefOffset.Set(0.0f, 0.0f);
    for (int32 i = 0; i < 18; i++)
    {
        pfnConsoleButtonDown[i] = NULL;
        pfnConsoleButtonHeld[i] = NULL;
    }
    pfnLStickHandler = NULL;
    pfnRStickHandler = NULL;
    pfnOnClick = NULL;
    pfnOnRightClick = NULL;
    pfnOnDoubleClick = NULL;
    pfnOnLButtonStillDown = NULL;
    pfnTooltip = NULL;
    pfnOnDragStart = NULL;
    pfnOnDrag = NULL;
    pfnOnContextMenu = NULL;
    fAbsoluteCoordinates = FALSE;
    vecTL.x = 0.0f;
    vecTL.y = 0.0f;
    vecDim.x = 0.0f;
    vecDim.y = 0.0f;
    vecMaxDim.Set(-1.0f, -1.0f);
    vecMinDim.Set(-1.0f, -1.0f);
    aRot = 0.0f;
    bMirrorX = FALSE;
    bMirrorY = FALSE;
    bNoSnap = FALSE;
    nToolTipWidth = -1;
    vecTooltipOffset.Set(0.0f, 0.0f);
    eToolTipAlignHorizontal = AP_NONE;
    eToolTipAlignVertical = AP_NONE;
    eCenterOnParent = CP_DISABLED;
    nInputFocusOrder = (uint32)-1;
    dwParentAnchorFlags = (1 << PARENT_ANCHOR_LEFT_BIT) | (1 << PARENT_ANCHOR_TOP_BIT);
}

WindowCreationParams::WindowCreationParams(const UIType eType, const UIID & id, const UIID & idParent)
    : ControlCreationParams(eType, id, idParent)
{
    sInit();
}

WindowCreationParams::WindowCreationParams(const UIType eType, LPCSTR szName, const UIID & idParent)
    : ControlCreationParams(eType, szName, idParent)
{
    sInit();
}

void WindowCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{
    ControlCreationParams::Load(snoUI, pControlData);

    bAlpha = (uint8)pControlData->GetInt32Value(ATT_ALPHA);
    snoCustomShader = SNOFileLookupGUID(SHADER_SNO, pControlData->GetAttributeValue(ATT_SHADER));
    eRenderLayerOverride = (RenderLayer)pControlData->GetInt32Value(ATT_RENDERLAYER_OVERRIDE);
    pControlData->GetImageAttribute(ATT_IMAGEFRAME, tImageTexBkgrnd);
    pControlData->GetImageAttribute(ATT_MASKIMAGE, tImageTexMask);
    snoAnim2D = SNOFileLookupGUID(ANIMATION2D_SNO, pControlData->GetAttributeValue(ATT_BG_ANIMSNO));

    LPCSTR szAnchorPoint = pControlData->GetAttributeValue(ATT_ANCHOR_POINT);
    eHorizontalAnchor = UI_ANCHOR_CENTER;
    if (szAnchorPoint != NULL)
    {
        if (strcasecmp(szAnchorPoint, "LEFT") == 0)
            eHorizontalAnchor = UI_ANCHOR_LEFT;
        else if (strcasecmp(szAnchorPoint, "RIGHT") == 0)
            eHorizontalAnchor = UI_ANCHOR_RIGHT;
        else if (strcasecmp(szAnchorPoint, "STRETCH") == 0)
            eHorizontalAnchor = UI_ANCHOR_STRETCH_LEFT_AND_RIGHT;
    }

    szAnchorPoint = pControlData->GetAttributeValue(ATT_VERTICAL_ANCHOR_POINT);
    eVerticalAnchor = UI_VERT_ANCHOR_CENTER;
    if (szAnchorPoint != NULL)
    {
        if (strcasecmp(szAnchorPoint, "TOP") == 0)
            eVerticalAnchor = UI_VERT_ANCHOR_TOP;
        else if (strcasecmp(szAnchorPoint, "BOTTOM") == 0)
            eVerticalAnchor = UI_VERT_ANCHOR_BOTTOM;
    }

    dwFlags = !pControlData->GetBoolValue(ATT_WIN_DRAGGABLE) ? (dwFlags & ~(1 << UIWINDOW_DRAGGABLE_BIT)) : (dwFlags | (1 << UIWINDOW_DRAGGABLE_BIT));
    dwFlags = !pControlData->GetBoolValue(ATT_DRAGWIN_CONSTRAIN_HORIZONTAL) ? (dwFlags & ~(1 << UIWINDOW_DRAG_CONSTRAIN_HORIZONTAL_BIT)) : (dwFlags | (1 << UIWINDOW_DRAG_CONSTRAIN_HORIZONTAL_BIT));
    dwFlags = !pControlData->GetBoolValue(ATT_DRAGWIN_CONSTRAIN_VERTICAL) ? (dwFlags & ~(1 << UIWINDOW_DRAG_CONSTRAIN_VERTICAL_BIT)) : (dwFlags | (1 << UIWINDOW_DRAG_CONSTRAIN_VERTICAL_BIT));
    dwFlags = !pControlData->GetBoolValue(ATT_WIN_IGNORE_CLIPPING) ? (dwFlags & ~(1 << UIWINDOW_IGNORES_CLIPPING_BIT)) : (dwFlags | (1 << UIWINDOW_IGNORES_CLIPPING_BIT));
    dwFlags = !pControlData->GetBoolValue(ATT_CLEAR_HIGHLIGHT_LAYER) ? (dwFlags & ~(1 << UIWINDOW_CLEAR_HIGHLIGHT_LAYER_BIT)) : (dwFlags | (1 << UIWINDOW_CLEAR_HIGHLIGHT_LAYER_BIT));
    dwFlags = !pControlData->GetBoolValue(ATT_ROLLEDUP) ? (dwFlags & ~(1 << UIWINDOW_ROLLED_UP_BIT)) : (dwFlags | (1 << UIWINDOW_ROLLED_UP_BIT));
    dwFlags = !pControlData->GetBoolValue(ATT_CENTER_IN_STACK) ? (dwFlags & ~(1 << UIWINDOW_CENTER_IN_STACK_BIT)) : (dwFlags | (1 << UIWINDOW_CENTER_IN_STACK_BIT));
    dwFlags = !pControlData->GetBoolValue(ATT_ENCLOSE_CHILDREN) ? (dwFlags & ~(1 << UIWINDOW_RESIZE_TO_ENCLOSE_CHILDREN_BIT)) : (dwFlags | (1 << UIWINDOW_RESIZE_TO_ENCLOSE_CHILDREN_BIT));
    dwFlags = !pControlData->GetBoolValue(ATT_CONTEXTMENU_LEFTCLICK) ? (dwFlags & ~(1 << UIWINDOW_CONTEXT_MENU_LEFT_CLICK_BIT)) : (dwFlags | (1 << UIWINDOW_CONTEXT_MENU_LEFT_CLICK_BIT));
    dwFlags = !pControlData->GetBoolValue(ATT_CONTEXTMENU_TOGGLE) ? (dwFlags & ~(1 << UIWINDOW_CONTEXT_MENU_TOGGLE_BIT)) : (dwFlags | (1 << UIWINDOW_CONTEXT_MENU_TOGGLE_BIT));
    dwFlags = !pControlData->GetBoolValue(ATT_WINDOW_IS_INPUT_FOCUS_GROUP) ? (dwFlags & ~(1 << UIWINDOW_INPUT_FOCUS_GROUP_BIT)) : (dwFlags | (1 << UIWINDOW_INPUT_FOCUS_GROUP_BIT));
    nInputFocusOrder = pControlData->GetInt32Value(ATT_INPUT_FOCUS_ORDER);

    eTiledStyle = (TiledStyle)pControlData->GetInt32Value(ATT_TILED_STYLE);
    eEatsClicks = (EatsClicksValue)pControlData->GetInt32Value(ATT_WINDOW_EATS_CLICKS);
    if (eEatsClicks == -1)
        eEatsClicks = EC_IGNORE;

    Blizzard::String::Copy(szContextMenu, pControlData->GetAttributeValue(ATT_CONTEXTMENU), sizeof(szContextMenu));
    Blizzard::String::Copy(szTargetParentContextMenu, pControlData->GetAttributeValue(ATT_TARGETPARENT_CONTEXTMENU), sizeof(szTargetParentContextMenu));
    Blizzard::String::Copy(szTooltipLabel, pControlData->GetAttributeValue(ATT_TOOLTIP_TEXT), sizeof(szTooltipLabel));
    eTooltipTextJustify = pControlData->GetInt32Value(ATT_TOOLTIP_TEXT_JUSTIFY);

    pfnOnClick = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONCLICK));
    pfnOnLButtonStillDown = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONLBUTTONSTILLDOWN));
    pfnOnDoubleClick = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONDOUBLECLICK));
    pfnOnRightClick = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONRIGHTCLICK));
    pfnTooltip = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_TOOLTIP));
    pfnOnDragStart = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ON_DRAG_START));
    pfnOnDrag = UIMsgFunction_Lookup(pControlData->GetAttributeValue(ATT_ON_DRAG));
    pfnOnContextMenu = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONCONTEXTMENU));

    int32 nStandardDefOffsetX = pControlData->GetInt32Value(ATT_STANDARD_DEF_OFFSET_X);
    int32 nStandardDefOffsetY = pControlData->GetInt32Value(ATT_STANDARD_DEF_OFFSET_Y);
    vecStandardDefOffset.Set((float)nStandardDefOffsetX, (float)nStandardDefOffsetY);

    for (int32 i = 0; i < 18; i++)
    {
        pfnConsoleButtonDown[i] = UIConsoleFunction_Lookup(pControlData->GetAttributeValue(sgtConsoleButtonAttributes[i]));
        pfnConsoleButtonHeld[i] = UIConsoleFunction_Lookup(pControlData->GetAttributeValue(sgtConsoleButtonHeldAttributes[i]));
    }
    if (pfnConsoleButtonDown[13] == UIConsoleFunction_Lookup("UIConsole_MainMenu_Accept"))
        pfnConsoleButtonDown[13] = NULL;
    pfnLStickHandler = UIConsoleFunction_Lookup(pControlData->GetAttributeValue(ATT_LSTICKHANDLER));
    pfnRStickHandler = UIConsoleFunction_Lookup(pControlData->GetAttributeValue(ATT_RSTICKHANDLER));

    nToolTipWidth = pControlData->GetInt32Value(ATT_TOOLTIP_WIDTH);
    eToolTipAlignHorizontal = (AlignPosition)pControlData->GetInt32Value(ATT_TOOLTIP_ALIGN_HORIZONTAL);
    eToolTipAlignVertical = (AlignPosition)pControlData->GetInt32Value(ATT_TOOLTIP_ALIGN_VERTICAL);
    int32 nTooltipOffsetX = pControlData->GetInt32Value(ATT_TOOLTIP_OFFSET_X);
    int32 nTooltipOffsetY = pControlData->GetInt32Value(ATT_TOOLTIP_OFFSET_Y);
    vecTooltipOffset.Set((float)nTooltipOffsetX, (float)nTooltipOffsetY);
    eCenterOnParent = (UICenterOnParent)pControlData->GetInt32Value(ATT_CENTER_ON_PARENT);

    int32 nMaxWidth = pControlData->GetInt32Value(ATT_MAX_WIDTH);
    int32 nMaxHeight = pControlData->GetInt32Value(ATT_MAX_HEIGHT);
    vecMaxDim.Set((float)nMaxWidth, (float)nMaxHeight);

    int32 nMinWidth = pControlData->GetInt32Value(ATT_MIN_WIDTH);
    int32 nMinHeight = pControlData->GetInt32Value(ATT_MIN_HEIGHT);
    vecMinDim.Set((float)nMinWidth, (float)nMinHeight);

    int32 nWidth = pControlData->GetInt32Value(ATT_WIDTH);
    int32 nHeight = pControlData->GetInt32Value(ATT_HEIGHT);
    int32 nLeft;
    int32 nTop;
    if (pControlData->AttributeExists(ATT_L_ABS) || pControlData->AttributeExists(ATT_T_ABS))
    {
        fAbsoluteCoordinates = TRUE;
        nLeft = pControlData->GetInt32Value(ATT_L_ABS);
        nTop = pControlData->GetInt32Value(ATT_T_ABS);
        if (pControlData->AttributeExists(ATT_R_ABS))
        {
            int32 nRight = pControlData->GetInt32Value(ATT_R_ABS);
            nWidth = nRight - nLeft;
        }
        if (pControlData->AttributeExists(ATT_B_ABS))
        {
            int32 nBottom = pControlData->GetInt32Value(ATT_B_ABS);
            nHeight = nBottom - nTop;
        }
    }
    else
    {
        fAbsoluteCoordinates = FALSE;
        nLeft = pControlData->GetInt32Value(ATT_L);
        nTop = pControlData->GetInt32Value(ATT_T);
    }
    vecTL.Set((float)nLeft, (float)nTop);
    vecDim.Set((float)nWidth, (float)nHeight);

    aRot = Math::DegToAngle(pControlData->GetFloat32Value(ATT_ROTATION));
    bMirrorX = pControlData->GetBoolValue(ATT_MIRRORX);
    bMirrorY = pControlData->GetBoolValue(ATT_MIRRORY);

    dwParentAnchorFlags = 0;
    if (pControlData->AttributeExists(ATT_ANCHOREDGES))
    {
        LPCSTR szAnchorEdges = pControlData->GetAttributeValue(ATT_ANCHOREDGES);
        for (int32 i = 0; ; i++)
        {
            // The original loads the character zero-extended (ldrb), so the switch is on an unsigned value.
            switch ((uint8)szAnchorEdges[i])
            {
            case 'L':
                dwParentAnchorFlags |= (1 << PARENT_ANCHOR_LEFT_BIT);
                break;
            case 'B':
                dwParentAnchorFlags |= (1 << PARENT_ANCHOR_BOTTOM_BIT);
                break;
            case 'T':
                dwParentAnchorFlags |= (1 << PARENT_ANCHOR_TOP_BIT);
                break;
            case 'R':
                dwParentAnchorFlags |= (1 << PARENT_ANCHOR_RIGHT_BIT);
                break;
            case '\0':
                if (dwParentAnchorFlags != 0)
                {
                    if ((dwParentAnchorFlags & ((1 << PARENT_ANCHOR_LEFT_BIT) | (1 << PARENT_ANCHOR_RIGHT_BIT))) == 0)
                        dwParentAnchorFlags |= (1 << PARENT_ANCHOR_LEFT_BIT);
                    if ((dwParentAnchorFlags & ((1 << PARENT_ANCHOR_TOP_BIT) | (1 << PARENT_ANCHOR_BOTTOM_BIT))) == 0)
                        dwParentAnchorFlags |= (1 << PARENT_ANCHOR_TOP_BIT);
                }
                return;
            default:
                DisplayInternalError(NULL, "", 847, ERROR_INTERNAL_FATAL);
                ExitProgram(ERROR_INTERNAL_FATAL);
            }
        }
    }
}

void UIWindow::sComputeCascadedAnchor()
{
    UIHorizontalAnchorPoint eHorizontalAnchor = GetHorizontalAnchor();
    UIVerticalAnchorPoint eVerticalAnchor = GetVerticalAnchor();
    m_eCascadedHorizontalAnchor = UI_ANCHOR_CENTER;
    m_eCascadedVerticalAnchor = UI_VERT_ANCHOR_CENTER;

    UIID idParent = GetParent();
    while (UI_TestID(idParent) && (eHorizontalAnchor == UI_ANCHOR_CENTER || eVerticalAnchor == UI_VERT_ANCHOR_CENTER))
    {
        UIWindow & tWndParent = UI_GetWindowFromID(idParent);
        if (eHorizontalAnchor == UI_ANCHOR_CENTER)
            eHorizontalAnchor = tWndParent.GetHorizontalAnchor();
        if (eVerticalAnchor == UI_VERT_ANCHOR_CENTER)
            eVerticalAnchor = tWndParent.GetVerticalAnchor();
        idParent = tWndParent.GetParent();
    }

    if (eHorizontalAnchor != UI_ANCHOR_STRETCH_LEFT_AND_RIGHT)
        m_eCascadedHorizontalAnchor = eHorizontalAnchor;
    m_eCascadedVerticalAnchor = eVerticalAnchor;
}

void UIWindow::sComputeCascadedSDOffset()
{
    m_vecCascadedStandardDefOffset = m_vecStandardDefOffset;

    UIID idParent = GetParent();
    while (UI_TestID(idParent))
    {
        UIWindow & tWndParent = UI_GetWindowFromID(idParent);
        m_vecCascadedStandardDefOffset += tWndParent.m_vecStandardDefOffset;
        idParent = tWndParent.GetParent();
    }
}

void UIWindow::sInit(const WindowCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim)
{
    m_rgbaWindowColor = crgbaWhite;
    SetTransparency(tParams.bAlpha);
    m_snoCustomShader = tParams.snoCustomShader;
    m_eRenderLayerOverride = tParams.eRenderLayerOverride;
    m_tImageTexBkgrnd = tParams.tImageTexBkgrnd;
    m_tImageTexMask = tParams.tImageTexMask;
    SetBackgroundAnimation(tParams.snoAnim2D);
    m_eHorizontalAnchor = tParams.eHorizontalAnchor;
    m_eVerticalAnchor = tParams.eVerticalAnchor;
    sComputeCascadedAnchor();
    m_dwFlags = tParams.dwFlags;
    m_vecTexOffset.Set(0.0f, 0.0f);
    m_vecTexScale.Set(1.0f, 1.0f);
    SetTiledStyle(tParams.eTiledStyle);
    m_eEatsClicks = tParams.eEatsClicks;

    m_pFunctionsPtr = NULL;
    if (tParams.HasAnyFunctionsPtr())
    {
        AllocateFunctionsPtrIfNeeded();
        m_pFunctionsPtr->pfnOnClick = tParams.pfnOnClick;
        m_pFunctionsPtr->pfnOnLButtonStillDown = tParams.pfnOnLButtonStillDown;
        m_pFunctionsPtr->pfnOnDoubleClick = tParams.pfnOnDoubleClick;
        m_pFunctionsPtr->pfnOnRightClick = tParams.pfnOnRightClick;
        SigmaMemoryCopy(m_pFunctionsPtr->pfnConsoleButtonDown, tParams.pfnConsoleButtonDown, sizeof(m_pFunctionsPtr->pfnConsoleButtonDown));
        SigmaMemoryCopy(m_pFunctionsPtr->pfnConsoleButtonHeld, tParams.pfnConsoleButtonHeld, sizeof(m_pFunctionsPtr->pfnConsoleButtonHeld));
        m_pFunctionsPtr->pfnLStickHandler = tParams.pfnLStickHandler;
        m_pFunctionsPtr->pfnRStickHandler = tParams.pfnRStickHandler;
    }

    m_vecStandardDefOffset = tParams.vecStandardDefOffset;
    sComputeCascadedSDOffset();

    SetTooltipTextLabel(tParams.szTooltipLabel);
    m_eTooltipTextJustify = tParams.eTooltipTextJustify;
    m_nToolTipWidth = tParams.nToolTipWidth;
    m_vecTooltipOffset = tParams.vecTooltipOffset;
    m_eToolTipAlignHorizontal = tParams.eToolTipAlignHorizontal;
    m_eToolTipAlignVertical = tParams.eToolTipAlignVertical;
    m_eCenterOnParent = tParams.eCenterOnParent;
    m_nInputFocusOrder = tParams.nInputFocusOrder;

    m_rect = cgrectNull;
    m_rectUnshifted = cgrectNull;
    m_vecTL.Set(0.0f, 0.0f);
    m_vecDim.Set(0.0f, 0.0f);
    m_vecMaxDim = tParams.vecMaxDim;
    m_vecMinDim = tParams.vecMinDim;
    m_rectClipping = cgrectNull;
    m_rectClippingUnshifted = cgrectNull;
    m_vecClippingTL.Set(0.0f, 0.0f);
    m_vecClippingDim.Set(0.0f, 0.0f);
    SetWindowRotation(tParams.aRot);
    SetWindowMirroring(tParams.bMirrorX, tParams.bMirrorY);
    SetNoSnap(tParams.bNoSnap);
    m_rectStoredClipping = cgrectNull;
    m_flStereoDepth = 1.0f;
    m_dwParentAnchorFlags = tParams.dwParentAnchorFlags;

    if (m_eHorizontalAnchor == UI_ANCHOR_STRETCH_LEFT_AND_RIGHT)
    {
        m_vecMaxDim.Set(-1.0f, -1.0f);
        m_vecDim.Set(960.0f, 720.0f);
        m_vecMinDim.Set(-1.0f, -1.0f);
        m_vecClippingDim = m_vecDim;
        m_rectUnshifted.Set(0.0f, 0.0f, m_vecDim.x, m_vecDim.y);
        m_rectClippingUnshifted = m_rectUnshifted;
        UpdateAbsolutePositions();
    }
    else
    {
        SetRelativePosition(vecTL, vecDim);
    }

    m_vecOriginalDim = m_vecDim;

    if (m_dwFlags & (1 << UIWINDOW_ROLLED_UP_BIT))
    {
        SetActive(FALSE);
    }
}

void UIWindow::SetBackgroundAnimation(SNO snoBackgroundAnimation2D)
{
    if (m_snoBackgroundAnimation != snoBackgroundAnimation2D)
    {
        Animation2DSet(m_tBackgroundAnimState, m_snoBackgroundAnimation, snoBackgroundAnimation2D, TRUE);
    }
}

UIWindow::UIWindow(const WindowCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim)
    : UIControlGroup(tParams), m_snoBackgroundAnimation(SNO_NONE), m_szTooltipLabel(NULL)
{
    sInit(tParams, vecTL, vecDim);
}

UIWindow::UIWindow(const WindowCreationParams & tParams, const Rect2D * pRect)
    : UIControlGroup(tParams), m_snoBackgroundAnimation(SNO_NONE), m_szTooltipLabel(NULL)
{
    Vector2D vecTL;
    Vector2D vecDim;
    sRelativeCoordsFromCreationParams(vecTL, vecDim, pRect, tParams);
    sInit(tParams, vecTL, vecDim);
}

inline void UIWindow::sRelativeFromAbsoluteRect(Vector2D & vecTL, Vector2D & vecDim, const Rect2D & rectUIC) const
{
    Rect2D rectParentUIC = cgrectNull;
    const UIID & idParent = GetParent();
    if (idParent != GetUIIDNone())
    {
        UI_GetWindowFromID(idParent).GetUnshiftedWindowRect(rectParentUIC);
    }
    vecTL.Set(rectUIC.left - rectParentUIC.left, rectUIC.top - rectParentUIC.top);
    vecDim.Set(rectUIC.Width(), rectUIC.Height());
}

void UIWindow::sRelativeCoordsFromCreationParams(Vector2D & vecTL, Vector2D & vecDim, const Rect2D * pRect, const WindowCreationParams & tParams) const
{
    if (pRect != NULL)
    {
        sRelativeFromAbsoluteRect(vecTL, vecDim, *pRect);
    }
    else if (tParams.fAbsoluteCoordinates)
    {
        Rect2D rectAbs(tParams.vecTL.x, tParams.vecTL.y, tParams.vecTL.x + tParams.vecDim.x, tParams.vecTL.y + tParams.vecDim.y);
        sRelativeFromAbsoluteRect(vecTL, vecDim, rectAbs);
    }
    else
    {
        vecTL = tParams.vecTL;
        vecDim = tParams.vecDim;
    }
}

UIWindow::~UIWindow()
{
    SigmaMemoryFree(m_pFunctionsPtr, NULL);
}

void UIWindow::GetTextureFrameSize(Vector2D & vecSize) const
{
    IRect2D rectPixels;
    TextureGetFrameRectPixels(rectPixels, m_tImageTexBkgrnd.snoTexture, m_tImageTexBkgrnd.nFrame);
    vecSize.Set((float)rectPixels.Width(), (float)rectPixels.Height());
}

void UIWindow::SetTextureFrame(LPCSTR szDecoratedLabel)
{
    ImageTextureFrame tImageTextureFrame;
    ImageCollectionGetImageInfo(szDecoratedLabel, tImageTextureFrame);
    SetTextureFrame(tImageTextureFrame);
}

BOOL UIWindow::IsInside(const IVector2D & pt)
{
    if (!IsActive())
    {
        return FALSE;
    }

    Vector2D vecUIC;
    ConvertPixelsToUIC(pt, vecUIC);

    Rect2D rectWindow;
    GetWindowRect(rectWindow);
    const Rect2D & rectClipping = GetCascadedClippingRect();
    Rect2D rectExtents;
    Rect2D::Intersect(rectWindow, rectClipping, &rectExtents);

    return rectExtents.ContainsPoint(vecUIC);
}

void UIWindow::Move(const Vector2D & vecDelta)
{
    if (Math::Abs(vecDelta.x) < EPSILON &&
        Math::Abs(vecDelta.y) < EPSILON)
    {
        return;
    }

    m_vecTL += vecDelta;
    m_vecClippingTL += vecDelta;
    UpdateAbsolutePositions();
}

Vector2D UIWindow::GetAnchorShiftUIC() const
{
    uint32 dwWidth;
    uint32 dwHeight;
    GfxGetCurrentDimensions(dwWidth, dwHeight);
    Rect2D rectSafeZone = UIGet().tSafeZone.GetMargins();

    Vector2D vecShiftUIC(0.0f, 0.0f);
    if (m_eCascadedHorizontalAnchor == UI_ANCHOR_LEFT)
    {
        Vector2D vecPixels((float)dwWidth * rectSafeZone.left, 0.0f);
        Vector2D vecUIC;
        ConvertPixelsToUIC(vecPixels, vecUIC);
        vecShiftUIC.x = vecUIC.x;
    }
    else if (m_eCascadedHorizontalAnchor == UI_ANCHOR_RIGHT)
    {
        Vector2D vecPixels((float)dwWidth * (1.0f - rectSafeZone.right), 0.0f);
        Vector2D vecUIC;
        ConvertPixelsToUIC(vecPixels, vecUIC);
        vecShiftUIC.x = vecUIC.x - 960.0f;
    }

    if (m_eCascadedVerticalAnchor == UI_VERT_ANCHOR_TOP)
    {
        Vector2D vecUnsafePixels(0.0f, (float)dwHeight * rectSafeZone.top);
        Vector2D vecUIC;
        ConvertPixelsToUIC(vecUnsafePixels, vecUIC);
        vecShiftUIC.y = vecUIC.y;
    }
    else if (m_eCascadedVerticalAnchor == UI_VERT_ANCHOR_BOTTOM)
    {
        Vector2D vecPixels(0.0f, (float)dwHeight * (1.0f - rectSafeZone.bottom));
        Vector2D vecUIC;
        ConvertPixelsToUIC(vecPixels, vecUIC);
        vecShiftUIC.y = vecUIC.y - 720.0f;
    }

    const float FOUR_BY_THREE = 4.0f / 3.0f;
    if (GfxGetAspectRatio() - FOUR_BY_THREE <= EPSILON)
    {
        vecShiftUIC += m_vecCascadedStandardDefOffset;
    }

    return vecShiftUIC;
}

inline void UIWindow::sRelativeToAbsoluteRect(const Vector2D & vecTL, const Vector2D & vecDim, const Vector2D & vecParentTL, Rect2D & rectUIC) const
{
    rectUIC.left = vecParentTL.x + vecTL.x;
    rectUIC.top = vecParentTL.y + vecTL.y;
    rectUIC.right = rectUIC.left + vecDim.x;
    rectUIC.bottom = rectUIC.top + vecDim.y;
}

void UIWindow::UpdateAbsolutePositions()
{
    if (GetHorizontalAnchor() == UI_ANCHOR_STRETCH_LEFT_AND_RIGHT)
    {
        Rect2D rectFullScreenUIC;
        CGameGetLetterboxedViewportUIC(rectFullScreenUIC);
        rectFullScreenUIC.right += 0.5f;
        rectFullScreenUIC.bottom += 0.5f;
        m_rect = rectFullScreenUIC;
        m_rectClipping = rectFullScreenUIC;
    }
    else
    {
        const UIID & idParent = GetParent();
        Rect2D rectParentUIC = cgrectNull;
        if (idParent != GetUIIDNone())
        {
            UI_GetWindowFromID(idParent).GetUnshiftedWindowRect(rectParentUIC);
        }
        Vector2D vecParentTL(rectParentUIC.left, rectParentUIC.top);

        sRelativeToAbsoluteRect(m_vecTL, m_vecDim, vecParentTL, m_rectUnshifted);
        sRelativeToAbsoluteRect(m_vecClippingTL, m_vecClippingDim, vecParentTL, m_rectClippingUnshifted);

        if (m_eCascadedHorizontalAnchor == UI_ANCHOR_CENTER && m_eCascadedVerticalAnchor == UI_VERT_ANCHOR_CENTER)
        {
            m_rect = m_rectUnshifted;
            m_rectClipping = m_rectClippingUnshifted;
        }
        else
        {
            Vector2D vecShiftUIC = GetAnchorShiftUIC();
            m_rectUnshifted.Offset(vecShiftUIC, m_rect);
            m_rectClippingUnshifted.Offset(vecShiftUIC, m_rectClipping);
        }
    }

    if (m_rectClipping != m_rectStoredClipping)
    {
        m_dwFlags &= ~(1 << UIWINDOW_CLIPPING_STORED_BIT);
    }

    for (uint32 i = 0; i < m_listChildren.size(); i++)
    {
        UI_GetControlFromID(m_listChildren[i].id).UpdateAbsolutePositions();
    }
}

void UIWindow::MoveTo(const Vector2D & vecTL)
{
    Rect2D rectWnd;
    GetWindowRect(rectWnd);
    Vector2D vecDelta(vecTL.x - rectWnd.left, vecTL.y - rectWnd.top);
    Move(vecDelta);
}

void UIWindow::SetWindowRect(const Rect2D & rectUIC)
{
    if (m_rect == rectUIC)
    {
        return;
    }

    Vector2D vecTL;
    Vector2D vecDim;
    sRelativeFromAbsoluteRect(vecTL, vecDim, rectUIC);
    SetRelativePosition(vecTL, vecDim);
}

void UIWindow::RecalculateLayout(int32 nStartingChildIndex)
{
    if (m_dwFlags & (1 << UIWINDOW_RESIZE_TO_ENCLOSE_CHILDREN_BIT))
    {
        Vector2D vecDimEnclose(0.0f, 0.0f);
        for (uint32 i = 0; i < m_listChildren.size(); i++)
        {
            UIID idChild = m_listChildren[i].id;
            if (UI_GetControlFromID(idChild).IsType(UIT_WINDOW))
            {
                UIWindow & tWindow = UI_GetWindowFromID(idChild);
                if (tWindow.IsItselfActive() && !(tWindow.m_dwFlags & (1 << UIWINDOW_IGNORES_CLIPPING_BIT)))
                {
                    Vector2D vecTLChild;
                    Vector2D vecDimChild;
                    tWindow.GetRelativePosition(vecTLChild, vecDimChild);
                    vecDimEnclose.x = Math::Max(vecDimEnclose.x, vecTLChild.x + vecDimChild.x);
                    vecDimEnclose.y = Math::Max(vecDimEnclose.y, vecTLChild.y + vecDimChild.y);
                }
            }
        }
        SetRelativePosition(m_vecTL, vecDimEnclose);
    }
}

void UIWindow::SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim)
{
    Vector2D vecClampedDim = vecDim;
    if (m_vecMaxDim.x > -1.0f)
        vecClampedDim.x = Math::Min(m_vecMaxDim.x, vecClampedDim.x);
    if (m_vecMaxDim.y > -1.0f)
        vecClampedDim.y = Math::Min(m_vecMaxDim.y, vecClampedDim.y);
    if (m_vecMinDim.x > -1.0f)
        vecClampedDim.x = Math::Max(m_vecMinDim.x, vecClampedDim.x);
    if (m_vecMinDim.y > -1.0f)
        vecClampedDim.y = Math::Max(m_vecMinDim.y, vecClampedDim.y);

    BOOL fDimensionsChanged = FALSE;
    if (Math::Abs(vecClampedDim.x - m_vecDim.x) > EPSILON ||
        Math::Abs(vecClampedDim.y - m_vecDim.y) > EPSILON)
    {
        fDimensionsChanged = TRUE;
    }

    Vector2D oldVecTL = m_vecTL;
    Vector2D oldVecDim = m_vecDim;
    m_vecClippingDim = m_vecDim = vecClampedDim;
    m_vecTL = vecTL;

    if (fDimensionsChanged)
    {
        if (m_dwFlags & (1 << UIWINDOW_DIMENSION_INITIALIZED))
        {
            if (m_dwParentAnchorFlags & (1 << PARENT_ANCHOR_RIGHT_BIT))
                m_vecTL.x -= m_vecDim.x - oldVecDim.x;
            if (m_dwParentAnchorFlags & (1 << PARENT_ANCHOR_BOTTOM_BIT))
                m_vecTL.y -= m_vecDim.y - oldVecDim.y;
        }
        else
        {
            m_dwFlags |= (1 << UIWINDOW_DIMENSION_INITIALIZED);
        }
    }

    const UIID & idParent = GetParent();
    if (idParent != GetUIIDNone() && m_eCenterOnParent != CP_DISABLED)
    {
        UIWindow & tWndParent = UI_GetWindowFromID(idParent);
        Vector2D vecTLParent;
        Vector2D vecDimParent;
        tWndParent.GetRelativePosition(vecTLParent, vecDimParent);
        switch (m_eCenterOnParent)
        {
        case CP_X:
            m_vecTL.x = (vecDimParent.x - m_vecDim.x) * 0.5f;
            break;
        case CP_X_RIGHT:
            m_vecTL.x = vecDimParent.x - m_vecDim.x;
            break;
        case CP_Y:
            m_vecTL.y = (vecDimParent.y - m_vecDim.y) * 0.5f;
            break;
        case CP_XY:
            m_vecTL.x = (vecDimParent.x - m_vecDim.x) * 0.5f;
            m_vecTL.y = (vecDimParent.y - m_vecDim.y) * 0.5f;
            break;
        default:
            break;
        }
    }

    m_vecClippingTL = m_vecTL;
    UpdateAbsolutePositions();

    if (fDimensionsChanged)
    {
        if (idParent != GetUIIDNone())
        {
            UI_GetWindowFromID(idParent).RecalculateLayoutForElement(GetID());
        }

        for (uint32 i = 0; i < m_listChildren.size(); i++)
        {
            UIID idChild = m_listChildren[i].id;
            UIControl & tControl = UI_GetControlFromID(idChild);
            if (tControl.IsType(UIT_WINDOW))
            {
                UIWindow & tWindowChild = UI_GetWindowFromID(idChild);
                if (tWindowChild.IsAnchored())
                {
                    uint32 dwParentAnchorFlags = tWindowChild.m_dwParentAnchorFlags;
                    Vector2D vecTLScoped;
                    Vector2D vecDimScoped;
                    tWindowChild.GetRelativePosition(vecTLScoped, vecDimScoped);
                    if (dwParentAnchorFlags & (1 << PARENT_ANCHOR_RIGHT_BIT))
                    {
                        float deltaWidth = m_vecDim.x - oldVecDim.x;
                        if (dwParentAnchorFlags & (1 << PARENT_ANCHOR_LEFT_BIT))
                            vecDimScoped.x += deltaWidth;
                        else
                            vecTLScoped.x += deltaWidth;
                    }
                    if (dwParentAnchorFlags & (1 << PARENT_ANCHOR_BOTTOM_BIT))
                    {
                        float deltaHeight = m_vecDim.y - oldVecDim.y;
                        if (dwParentAnchorFlags & (1 << PARENT_ANCHOR_TOP_BIT))
                            vecDimScoped.y += deltaHeight;
                        else
                            vecTLScoped.y += deltaHeight;
                    }
                    tWindowChild.SetRelativePosition(vecTLScoped, vecDimScoped);
                }
            }
        }
    }
}

void UIWindow::SetClipRect(const Rect2D & rectClipUIC)
{
    sRelativeFromAbsoluteRect(m_vecClippingTL, m_vecClippingDim, rectClipUIC);
    UpdateAbsolutePositions();
}

void UIWindow::SetRelativeClipRect(const Vector2D & vecTL, const Vector2D & vecDim)
{
    m_vecClippingTL = vecTL;
    m_vecClippingDim = vecDim;
    UpdateAbsolutePositions();
}

const Rect2D & UIWindow::GetCascadedClippingRect()
{
    if (m_dwFlags & (1 << UIWINDOW_IGNORES_CLIPPING_BIT))
    {
        return m_rect;
    }

    if (!(m_dwFlags & (1 << UIWINDOW_CLIPPING_STORED_BIT)))
    {
        Rect2D rectClipping;
        GetClippingRect(rectClipping);
        const UIID & idParent = GetParent();
        if (idParent != cgUIIDRoot && idParent != GetUIIDNone())
        {
            UIWindow & tWindow = UI_GetWindowFromID(idParent);
            const Rect2D & rectClippingParent = tWindow.GetCascadedClippingRect();
            Rect2D::Intersect(rectClippingParent, rectClipping, &rectClipping);
        }
        m_rectStoredClipping = rectClipping;
        m_dwFlags |= (1 << UIWINDOW_CLIPPING_STORED_BIT);
    }

    return m_rectStoredClipping;
}

BOOL UIWindow::IsClipped()
{
    const Rect2D rectWindowUIC = m_rect;
    const Rect2D & rectClippingUIC = GetCascadedClippingRect();
    Rect2D rectVisible;
    if (Rect2D::Intersect(rectWindowUIC, rectClippingUIC, &rectVisible) && !rectVisible.IsEmpty())
    {
        return FALSE;
    }
    return TRUE;
}

LPCSTR UIWindow::GetTooltipText()
{
    if (m_szTooltipLabel != NULL && m_szTooltipLabel[0] != '\0')
    {
        LPCSTR pszColon = strchr(m_szTooltipLabel, ':');
        if (pszColon == NULL)
        {
            return NULL;
        }

        CHAR szLeft[128];
        CHAR szRight[128];
        strncpy(szLeft, m_szTooltipLabel, pszColon - m_szTooltipLabel);
        szLeft[pszColon - m_szTooltipLabel] = '\0';
        Blizzard::String::Copy(szRight, pszColon + 1, sizeof(szRight));

        SNO snoStringList = SNOFileLookupGUID(STRINGLIST_SNO, szLeft);
        if (snoStringList != SNO_NONE)
        {
            return StringListFetchLPCSTR(snoStringList, szRight);
        }
    }

    return NULL;
}

static void sDrawPiece(const SNO snoTexturePiece, const SNO snoShader, const Rect2D & rectPixels, const Rect2D & rectUVs, const Rect2D & rectClippingPixels, const float flZ, const int32 nRotations, uint8 uAlphaVal, const RenderLayer eRenderLayerOverride)
{
    Rect2D wrUVsClipped;
    Rect2D rectScreenClipped;
    Rect2D rectFrameSpace;
    Vector2D wpUVs[4];
    BOOL bVisible = Rect2D::Intersect(rectPixels, rectClippingPixels, &rectScreenClipped);
    if (bVisible)
    {
        rectScreenClipped.ToUnitCoords(rectPixels, rectFrameSpace);
        rectFrameSpace.FromUnitCoords(rectUVs, wrUVsClipped);
        wrUVsClipped.ToPoints(wpUVs, nRotations);
        RGBAColor rgbaColor(crgbaWhite);
        rgbaColor.a = uAlphaVal;
        SpriteDrawSimple(snoTexturePiece, snoShader, rectScreenClipped, wpUVs, rgbaColor, crgbaBlack, flZ, 1.0f, eRenderLayerOverride);
    }
}

static void sDrawFramePiece(const SNO snoTexturePiece, const SNO snoShader, const Rect2D & rectPixels, const Rect2D & rectUVs, const float flZ, uint8 uAlphaVal, const RenderLayer eRenderLayerOverride)
{
    Vector2D wpUVs[4];
    rectUVs.ToPoints(wpUVs, 0);
    RGBAColor rgbaColor(crgbaWhite);
    rgbaColor.a = uAlphaVal;
    SpriteDrawSimple(snoTexturePiece, snoShader, rectPixels, wpUVs, rgbaColor, crgbaBlack, flZ, 1.0f, eRenderLayerOverride);
}

static BOOL sInitCornerData(LPCSTR szDecoratedLabel, CornerData * ptCorners, const uint32 nCornerIndex, const Rect2D & rectWindowPixels)
{
    CornerData & tCornerData = ptCorners[nCornerIndex];
    ImageCollectionGetImageInfo(szDecoratedLabel, tCornerData.tImageTexFrame);
    if (tCornerData.tImageTexFrame.snoTexture != SNO_NONE)
    {
        if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tCornerData.tImageTexFrame.snoTexture), 2))
        {
            return FALSE;
        }

        Rect2D rectUVCornerPixels;
        TextureGetFrameRectUIScaledPixels(rectUVCornerPixels, tCornerData.tImageTexFrame.snoTexture, tCornerData.tImageTexFrame.nFrame);

        // flWidth and flHeight are invented names: the DWARF drops these locals.
        float flWidth = rectUVCornerPixels.Width(), flHeight = rectUVCornerPixels.Height();
        switch (nCornerIndex)
        {
        case 0:
            tCornerData.rectPixels.Set(rectWindowPixels.left, rectWindowPixels.top, flWidth + rectWindowPixels.left, flHeight + rectWindowPixels.top);
            break;
        case 1:
            tCornerData.rectPixels.Set(rectWindowPixels.right - flWidth, rectWindowPixels.top, rectWindowPixels.right, flHeight + rectWindowPixels.top);
            break;
        case 3:
            tCornerData.rectPixels.Set(rectWindowPixels.left, rectWindowPixels.bottom - flHeight, flWidth + rectWindowPixels.left, rectWindowPixels.bottom);
            break;
        case 2:
            tCornerData.rectPixels.Set(rectWindowPixels.right - flWidth, rectWindowPixels.bottom - flHeight, rectWindowPixels.right, rectWindowPixels.bottom);
            break;
        }
    }
    return TRUE;
}

static void sDrawCornerPiece(const CornerData & tCorner, const SNO snoShader, const float flZ, RGBAColor & rgbaColor, const RenderLayer eRenderLayerOverride)
{
    SpriteDrawDeferred(tCorner.tImageTexFrame, snoShader, tCorner.rectPixels, rgbaColor, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, eRenderLayerOverride, FALSE);
}

static BOOL sInitTrackPieceData(LPCSTR szDecoratedLabel, TrackPieceData * ptPieceData, const uint32 nPieceIndex, const Rect2D & rectWindowUIC)
{
    TrackPieceData & tPieceData = ptPieceData[nPieceIndex];
    if (!ImageCollectionGetImageInfo(szDecoratedLabel, tPieceData.tImageTexFrame))
    {
        tPieceData.tImageTexFrame = cgtImageTextureFrameNull;
    }

    // flSizeY is an invented name: the DWARF drops this local, as it drops nPieceIndex.
    float flSizeY = 0.0f;
    if (tPieceData.tImageTexFrame.snoTexture != SNO_NONE)
    {
        if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tPieceData.tImageTexFrame.snoTexture), 2))
        {
            return FALSE;
        }

        IRect2D rectUVCorner;
        TextureGetFrameRectPixels(rectUVCorner, tPieceData.tImageTexFrame.snoTexture, tPieceData.tImageTexFrame.nFrame);
        flSizeY = (float)rectUVCorner.Height();
    }

    switch (nPieceIndex)
    {
    case TP_START:
        tPieceData.rectUIC.Set(rectWindowUIC.left, rectWindowUIC.top, rectWindowUIC.right, rectWindowUIC.top + flSizeY);
        break;
    case TP_END:
        tPieceData.rectUIC.Set(rectWindowUIC.left, rectWindowUIC.bottom - flSizeY, rectWindowUIC.right, rectWindowUIC.bottom);
        break;
    case TP_MIDDLE:
        tPieceData.rectUIC.Set(rectWindowUIC.left, ptPieceData[TP_START].rectUIC.bottom, rectWindowUIC.right, ptPieceData[TP_END].rectUIC.top);
        break;
    }
    return TRUE;
}

static void sDrawTrackPiece(const TrackPieceData & tPiece, const SNO snoShader, const float flZ, RGBAColor & rgbaColor, const RenderLayer eRenderLayerOverride)
{
    if (tPiece.tImageTexFrame.snoTexture != SNO_NONE)
    {
        Rect2D rectPixels;
        ConvertUICToPixels(tPiece.rectUIC, rectPixels);
        SpriteDrawDeferred(tPiece.tImageTexFrame, snoShader, rectPixels, rgbaColor, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, eRenderLayerOverride, FALSE);
    }
}

void UIWindow::sDrawTrack(const TrackPieces & tPieces, Rect2D & rectWindowUIC, float flZ, uint8 bAlphaVal) const
{
    TrackPieceData tPieceData[NUM_TRACKPIECES];
    for (int32 i = 0; i < NUM_TRACKPIECES; i++)
    {
        if (!sInitTrackPieceData(tPieces.szPieceFrame[i], tPieceData, i, rectWindowUIC))
        {
            return;
        }
    }

    const SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_CLAMP);
    flZ -= UIDC_CONTROL_Z_INCREMENT;
    RGBAColor rgbaColor(crgbaWhite);
    rgbaColor.a = bAlphaVal;
    for (int32 i = 0; i < NUM_TRACKPIECES; i++)
    {
        sDrawTrackPiece(tPieceData[i], snoShader, flZ, rgbaColor, m_eRenderLayerOverride);
    }
}

inline BOOL sGetEndPieceImage(LPCSTR szDecoratedLabel, ImageTextureFrame & tImageTexFrame, float & flSizeY)
{
    if (!ImageCollectionGetImageInfo(szDecoratedLabel, tImageTexFrame))
    {
        tImageTexFrame = cgtImageTextureFrameNull;
    }

    flSizeY = 0.0f;
    if (tImageTexFrame.snoTexture != SNO_NONE)
    {
        if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrame.snoTexture), 2))
        {
            return FALSE;
        }

        IRect2D rectUVCorner;
        TextureGetFrameRectPixels(rectUVCorner, tImageTexFrame.snoTexture, tImageTexFrame.nFrame);
        flSizeY = (float)rectUVCorner.Height();
    }
    return TRUE;
}

void UIWindow::sDrawVertTiledWindow(const VertTiledWindowPieces & tPieces, Rect2D & rectWindowUIC, float flZ, uint8 bAlphaVal) const
{
    flZ -= UIDC_CONTROL_Z_INCREMENT;

    RGBAColor rgbaColor(crgbaWhite);
    rgbaColor.a = bAlphaVal;

    Rect2D rectWindowMiddle = rectWindowUIC;

    {
        float flSizeY;
        ImageTextureFrame tImageTexFrame;
        if (!sGetEndPieceImage(tPieces.szPieceTop, tImageTexFrame, flSizeY))
        {
            return;
        }

        Rect2D rectPieceUIC;
        rectPieceUIC.Set(rectWindowUIC.left, rectWindowUIC.top, rectWindowUIC.right, rectWindowUIC.top + flSizeY);
        rectWindowMiddle.top = rectPieceUIC.bottom;

        Rect2D rectPixels;
        ConvertUICToPixels(rectPieceUIC, rectPixels);
        SpriteDrawDeferred(tImageTexFrame, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_CLAMP), rectPixels, rgbaColor, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, m_eRenderLayerOverride, FALSE);
    }

    {
        float flSizeY;
        ImageTextureFrame tImageTexFrame;
        if (!sGetEndPieceImage(tPieces.szPieceBottom, tImageTexFrame, flSizeY))
        {
            return;
        }

        Rect2D rectPieceUIC;
        rectPieceUIC.Set(rectWindowUIC.left, rectWindowUIC.bottom - flSizeY, rectWindowUIC.right, rectWindowUIC.bottom);
        rectWindowMiddle.bottom = rectPieceUIC.top;

        Rect2D rectPixels;
        ConvertUICToPixels(rectPieceUIC, rectPixels);
        SpriteDrawDeferred(tImageTexFrame, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_CLAMP), rectPixels, rgbaColor, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, m_eRenderLayerOverride, FALSE);
    }

    SNO snoMiddle = GlobalSNOGet(tPieces.eMiddle);
    if (snoMiddle != SNO_NONE)
    {
        if (Prefetch::AddToQueue(SNOName(TEXTURE_SNO, snoMiddle), 2))
        {
            Rect2D rectPixels;
            IRect2D rectUVBorder;
            Rect2D rectUVs;
            Vector2D wpUVs[4];
            ConvertUICToPixels(rectWindowMiddle, rectPixels);
            TextureGetFrameRectPixels(rectUVBorder, snoMiddle, (uint32)-1);
            rectUVs.Set(0.0f, 0.0f, 1.0f, rectWindowMiddle.Height() / (float)rectUVBorder.Height());
            rectUVs.ToPoints(wpUVs, 0);
            SpriteDrawSimple(snoMiddle, SNO_NONE, rectPixels, wpUVs, rgbaColor, crgbaBlack, flZ, 1.0f, m_eRenderLayerOverride);
        }
    }
}

void UIWindow::sDrawTiledWindowDefault(const TiledWindowPieces & tPieces, const Rect2D & rectWindowPixels, float flZ, const uint8 bAlphaVal, const BOOL fInsetBackground) const
{
    CornerData tCorners[4];
    if (!sInitCornerData(tPieces.szCornerFrames[0], tCorners, 0, rectWindowPixels) ||
        !sInitCornerData(tPieces.szCornerFrames[1], tCorners, 1, rectWindowPixels) ||
        !sInitCornerData(tPieces.szCornerFrames[3], tCorners, 3, rectWindowPixels) ||
        !sInitCornerData(tPieces.szCornerFrames[2], tCorners, 2, rectWindowPixels))
    {
        return;
    }

    {
        Rect2D rectWindowBkgrnd = rectWindowPixels;
        if (fInsetBackground)
        {
            rectWindowBkgrnd.Set(tCorners[0].rectPixels.right, tCorners[0].rectPixels.bottom, tCorners[2].rectPixels.left, tCorners[2].rectPixels.top);
        }
        ImageTextureFrame tTexBkgrnd;
        ImageCollectionGetImageInfo(tPieces.szBackground, tTexBkgrnd);
        const SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT);
        RGBAColor rgbaColor(crgbaWhite);
        rgbaColor.a = bAlphaVal;
        SpriteDrawDeferred(tTexBkgrnd, snoShader, rectWindowBkgrnd, rgbaColor, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, m_eRenderLayerOverride, FALSE);
    }

    flZ -= UIDC_CONTROL_Z_INCREMENT;

    {
        const SNO snoEdgeHShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_EDGE_H);
        const SNO snoEdgeVShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_EDGE_V);

        SNO snoBorder = GlobalSNOGet(tPieces.eTopBorder);
        if (snoBorder != SNO_NONE && Prefetch::AddToQueue(SNOName(TEXTURE_SNO, snoBorder), 2))
        {
            Rect2D rectUVBorderPixels;
            TextureGetFrameRectUIScaledPixels(rectUVBorderPixels, snoBorder, (uint32)-1);
            Rect2D rectBorder;
            rectBorder.Set(tCorners[0].rectPixels.right, tCorners[0].rectPixels.top, tCorners[1].rectPixels.left, tCorners[0].rectPixels.bottom);
            Rect2D rectTextureUVs;
            rectTextureUVs.Set(0.0f, 0.0f, rectBorder.Width() / rectUVBorderPixels.Width(), 1.0f);
            sDrawFramePiece(snoBorder, snoEdgeVShader, rectBorder, rectTextureUVs, flZ, bAlphaVal, m_eRenderLayerOverride);
        }

        snoBorder = GlobalSNOGet(tPieces.eRightBorder);
        if (snoBorder != SNO_NONE && Prefetch::AddToQueue(SNOName(TEXTURE_SNO, snoBorder), 2))
        {
            Rect2D rectUVBorderPixels;
            TextureGetFrameRectUIScaledPixels(rectUVBorderPixels, snoBorder, (uint32)-1);
            Rect2D rectBorder;
            rectBorder.Set(tCorners[1].rectPixels.left, tCorners[1].rectPixels.bottom, tCorners[1].rectPixels.right, tCorners[2].rectPixels.top);
            Rect2D rectTextureUVs;
            rectTextureUVs.Set(0.0f, 0.0f, 1.0f, rectBorder.Height() / rectUVBorderPixels.Height());
            sDrawFramePiece(snoBorder, snoEdgeHShader, rectBorder, rectTextureUVs, flZ, bAlphaVal, m_eRenderLayerOverride);
        }

        snoBorder = GlobalSNOGet(tPieces.eBottomBorder);
        if (snoBorder != SNO_NONE && Prefetch::AddToQueue(SNOName(TEXTURE_SNO, snoBorder), 2))
        {
            Rect2D rectUVBorderPixels;
            TextureGetFrameRectUIScaledPixels(rectUVBorderPixels, snoBorder, (uint32)-1);
            Rect2D rectBorder;
            rectBorder.Set(tCorners[3].rectPixels.right, tCorners[2].rectPixels.top, tCorners[2].rectPixels.left, tCorners[2].rectPixels.bottom);
            Rect2D rectTextureUVs;
            rectTextureUVs.Set(0.0f, 0.0f, rectBorder.Width() / rectUVBorderPixels.Width(), 1.0f);
            sDrawFramePiece(snoBorder, snoEdgeVShader, rectBorder, rectTextureUVs, flZ, bAlphaVal, m_eRenderLayerOverride);
        }

        snoBorder = GlobalSNOGet(tPieces.eLeftBorder);
        if (snoBorder != SNO_NONE && Prefetch::AddToQueue(SNOName(TEXTURE_SNO, snoBorder), 2))
        {
            Rect2D rectUVBorderPixels;
            TextureGetFrameRectUIScaledPixels(rectUVBorderPixels, snoBorder, (uint32)-1);
            Rect2D rectBorder;
            rectBorder.Set(tCorners[0].rectPixels.left, tCorners[0].rectPixels.bottom, tCorners[0].rectPixels.right, tCorners[3].rectPixels.top);
            Rect2D rectTextureUVs;
            rectTextureUVs.Set(0.0f, 0.0f, 1.0f, rectBorder.Height() / rectUVBorderPixels.Height());
            sDrawFramePiece(snoBorder, snoEdgeHShader, rectBorder, rectTextureUVs, flZ, bAlphaVal, m_eRenderLayerOverride);
        }
    }

    const SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_CLAMP);
    flZ -= UIDC_CONTROL_Z_INCREMENT;

    RGBAColor rgbaColor(crgbaWhite);
    rgbaColor.a = bAlphaVal;

    if (tCorners[0].tImageTexFrame.snoTexture != SNO_NONE)
    {
        sDrawCornerPiece(tCorners[0], snoShader, flZ, rgbaColor, m_eRenderLayerOverride);
    }
    if (tCorners[1].tImageTexFrame.snoTexture != SNO_NONE)
    {
        sDrawCornerPiece(tCorners[1], snoShader, flZ, rgbaColor, m_eRenderLayerOverride);
    }
    if (tCorners[3].tImageTexFrame.snoTexture != SNO_NONE)
    {
        sDrawCornerPiece(tCorners[3], snoShader, flZ, rgbaColor, m_eRenderLayerOverride);
    }
    if (tCorners[2].tImageTexFrame.snoTexture != SNO_NONE)
    {
        sDrawCornerPiece(tCorners[2], snoShader, flZ, rgbaColor, m_eRenderLayerOverride);
    }
}

void UIWindow::sDrawTiledWindow(Rect2D & rectWindowPixels, const Rect2D & rectClippingPixels, float flZ, uint8 bAlphaVal) const
{
    BOOL bVisible = Rect2D::Intersect(rectWindowPixels, rectClippingPixels, NULL);
    if (!bVisible)
    {
        return;
    }

    float flCornerWidth = 0.0f;
    float flCornerHeight = 0.0f;
    SNO snoCorner = GlobalSNOGet(scgtWindowPieces[m_eTiledStyle].eCorner);
    Rect2D rectUVCornerPixels;
    if (snoCorner != SNO_NONE)
    {
        TextureGetFrameRectUIScaledPixels(rectUVCornerPixels, snoCorner, (uint32)-1);
        flCornerWidth = rectUVCornerPixels.Width();
        flCornerHeight = rectUVCornerPixels.Height();
    }

    SNO snoBkgrnd = GlobalSNOGet(scgtWindowPieces[m_eTiledStyle].eBkgrnd);
    if (snoBkgrnd != SNO_NONE)
    {
        const SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT);
        Rect2D rectUVBkgrndPixels;
        TextureGetFrameRectUIScaledPixels(rectUVBkgrndPixels, snoBkgrnd, (uint32)-1);
        float flWidthFactor = 1.0f / rectUVBkgrndPixels.Width();
        float flHeightFactor = 1.0f / rectUVBkgrndPixels.Height();
        Rect2D wrBkgrnd;
        Rect2D wrInnerUVs;
        Rect2D wrTextureUVs;

        wrInnerUVs.Set(flCornerWidth * flWidthFactor,
                       flCornerHeight * flHeightFactor,
                       flWidthFactor * (rectWindowPixels.Width() - flCornerWidth),
                       flHeightFactor * (rectWindowPixels.Height() - flCornerHeight));

        wrBkgrnd.Set(flCornerWidth + rectWindowPixels.left, flCornerHeight + rectWindowPixels.top, rectWindowPixels.right - flCornerWidth, rectWindowPixels.bottom - flCornerHeight);
        sDrawPiece(snoBkgrnd, snoShader, wrBkgrnd, wrInnerUVs, rectClippingPixels, flZ, 0, bAlphaVal, m_eRenderLayerOverride);

        wrBkgrnd.Set(flCornerWidth + rectWindowPixels.left, rectWindowPixels.top, rectWindowPixels.right - flCornerWidth, flCornerHeight + rectWindowPixels.top);
        wrTextureUVs.Set(wrInnerUVs.left, 0.0f, wrInnerUVs.right, wrInnerUVs.top);
        sDrawPiece(snoBkgrnd, snoShader, wrBkgrnd, wrTextureUVs, rectClippingPixels, flZ, 0, bAlphaVal, m_eRenderLayerOverride);

        wrBkgrnd.Set(flCornerWidth + rectWindowPixels.left, rectWindowPixels.bottom - flCornerHeight, rectWindowPixels.right - flCornerWidth, rectWindowPixels.bottom);
        wrTextureUVs.Set(wrInnerUVs.left, wrInnerUVs.bottom, wrInnerUVs.right, flHeightFactor * rectWindowPixels.Height());
        sDrawPiece(snoBkgrnd, snoShader, wrBkgrnd, wrTextureUVs, rectClippingPixels, flZ, 0, bAlphaVal, m_eRenderLayerOverride);

        wrBkgrnd.Set(rectWindowPixels.left, flCornerHeight + rectWindowPixels.top, flCornerWidth + rectWindowPixels.left, rectWindowPixels.bottom - flCornerHeight);
        wrTextureUVs.Set(0.0f, wrInnerUVs.top, wrInnerUVs.left, wrInnerUVs.bottom);
        sDrawPiece(snoBkgrnd, snoShader, wrBkgrnd, wrTextureUVs, rectClippingPixels, flZ, 0, bAlphaVal, m_eRenderLayerOverride);

        wrBkgrnd.Set(rectWindowPixels.right - flCornerWidth, flCornerHeight + rectWindowPixels.top, rectWindowPixels.right, rectWindowPixels.bottom - flCornerHeight);
        wrTextureUVs.Set(wrInnerUVs.right, wrInnerUVs.top, flWidthFactor * rectWindowPixels.Width(), wrInnerUVs.bottom);
        sDrawPiece(snoBkgrnd, snoShader, wrBkgrnd, wrTextureUVs, rectClippingPixels, flZ, 0, bAlphaVal, m_eRenderLayerOverride);
    }

    flZ -= UIDC_CONTROL_Z_INCREMENT;

    {
        const SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT);
        SNO snoBorder = GlobalSNOGet(scgtWindowPieces[m_eTiledStyle].eTopBorder);
        if (snoBorder != SNO_NONE)
        {
            Rect2D rectUVBorderPixels;
            TextureGetFrameRectUIScaledPixels(rectUVBorderPixels, snoBorder, (uint32)-1);
            float flVertThickness = rectUVBorderPixels.Height();
            float flHorFactor = 1.0f / rectUVBorderPixels.Width();
            Rect2D wrBorder;
            wrBorder.Set(flCornerWidth + rectWindowPixels.left, rectWindowPixels.top, rectWindowPixels.right - flCornerWidth, flVertThickness + rectWindowPixels.top);
            Rect2D wrTextureUVs;
            wrTextureUVs.Set(0.0f, 0.0f, flHorFactor * wrBorder.Width(), 1.0f);
            sDrawPiece(snoBorder, snoShader, wrBorder, wrTextureUVs, rectClippingPixels, flZ, 0, bAlphaVal, m_eRenderLayerOverride);
        }

        snoBorder = GlobalSNOGet(scgtWindowPieces[m_eTiledStyle].eRightBorder);
        if (snoBorder != SNO_NONE)
        {
            Rect2D rectUVBorderPixels;
            TextureGetFrameRectUIScaledPixels(rectUVBorderPixels, snoBorder, (uint32)-1);
            float flHorThickness = rectUVBorderPixels.Height();
            float flVertFactor = 1.0f / rectUVBorderPixels.Width();
            Rect2D wrBorder;
            wrBorder.Set(rectWindowPixels.right - flHorThickness, flCornerHeight + rectWindowPixels.top, rectWindowPixels.right, rectWindowPixels.bottom - flCornerHeight);
            Rect2D wrTextureUVs;
            wrTextureUVs.Set(0.0f, 0.0f, flVertFactor * wrBorder.Height(), 1.0f);
            sDrawPiece(snoBorder, snoShader, wrBorder, wrTextureUVs, rectClippingPixels, flZ, 1, bAlphaVal, m_eRenderLayerOverride);
        }

        snoBorder = GlobalSNOGet(scgtWindowPieces[m_eTiledStyle].eBottomBorder);
        if (snoBorder != SNO_NONE)
        {
            Rect2D rectUVBorderPixels;
            TextureGetFrameRectUIScaledPixels(rectUVBorderPixels, snoBorder, (uint32)-1);
            float flVertThickness = rectUVBorderPixels.Height();
            float flHorFactor = 1.0f / rectUVBorderPixels.Width();
            Rect2D wrBorder;
            wrBorder.Set(flCornerWidth + rectWindowPixels.left, rectWindowPixels.bottom - flVertThickness, rectWindowPixels.right - flCornerWidth, rectWindowPixels.bottom);
            Rect2D wrTextureUVs;
            wrTextureUVs.Set(0.0f, 0.0f, flHorFactor * wrBorder.Width(), 1.0f);
            sDrawPiece(snoBorder, snoShader, wrBorder, wrTextureUVs, rectClippingPixels, flZ, 2, bAlphaVal, m_eRenderLayerOverride);
        }

        snoBorder = GlobalSNOGet(scgtWindowPieces[m_eTiledStyle].eLeftBorder);
        if (snoBorder != SNO_NONE)
        {
            Rect2D rectUVBorderPixels;
            TextureGetFrameRectUIScaledPixels(rectUVBorderPixels, snoBorder, (uint32)-1);
            float flHorThickness = rectUVBorderPixels.Height();
            float flVertFactor = 1.0f / rectUVBorderPixels.Width();
            Rect2D wrBorder;
            wrBorder.Set(rectWindowPixels.left, flCornerHeight + rectWindowPixels.top, flHorThickness + rectWindowPixels.left, rectWindowPixels.bottom - flCornerHeight);
            Rect2D wrTextureUVs;
            wrTextureUVs.Set(0.0f, 0.0f, flVertFactor * wrBorder.Height(), 1.0f);
            sDrawPiece(snoBorder, snoShader, wrBorder, wrTextureUVs, rectClippingPixels, flZ, 3, bAlphaVal, m_eRenderLayerOverride);
        }
    }

    if (snoCorner != SNO_NONE)
    {
        flZ -= UIDC_CONTROL_Z_INCREMENT;
        const SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_CLAMP);
        Rect2D wrTextureUVs;
        wrTextureUVs.Set(0.0f, 0.0f, 1.0f, 1.0f);
        Rect2D wrCorner;

        wrCorner.Set(rectWindowPixels.left, rectWindowPixels.top, flCornerWidth + rectWindowPixels.left, flCornerHeight + rectWindowPixels.top);
        sDrawPiece(snoCorner, snoShader, wrCorner, wrTextureUVs, rectClippingPixels, flZ, 0, bAlphaVal, m_eRenderLayerOverride);

        wrCorner.Set(rectWindowPixels.right - flCornerWidth, rectWindowPixels.top, rectWindowPixels.right, flCornerHeight + rectWindowPixels.top);
        sDrawPiece(snoCorner, snoShader, wrCorner, wrTextureUVs, rectClippingPixels, flZ, 1, bAlphaVal, m_eRenderLayerOverride);

        wrCorner.Set(rectWindowPixels.right - flCornerWidth, rectWindowPixels.bottom - flCornerHeight, rectWindowPixels.right, rectWindowPixels.bottom);
        sDrawPiece(snoCorner, snoShader, wrCorner, wrTextureUVs, rectClippingPixels, flZ, 2, bAlphaVal, m_eRenderLayerOverride);

        wrCorner.Set(rectWindowPixels.left, rectWindowPixels.bottom - flCornerHeight, flCornerWidth + rectWindowPixels.left, rectWindowPixels.bottom);
        sDrawPiece(snoCorner, snoShader, wrCorner, wrTextureUVs, rectClippingPixels, flZ, 3, bAlphaVal, m_eRenderLayerOverride);
    }
}

void UIWindow::sDrawSelf(const uint8 bAlphaCascaded, float & flZSlice)
{
    float flZ = UIDC_DIALOG_Z_START - flZSlice * UIDC_DIALOG_Z_INCREMENT;
    flZSlice += 1.0f;

    Rect2D rectWindowUIC = m_rect;
    const Rect2D & rectClippingUIC = GetCascadedClippingRect();

    rectWindowUIC.left += m_vecTexOffset.x;
    rectWindowUIC.right += m_vecTexOffset.x;
    rectWindowUIC.top += m_vecTexOffset.y;
    rectWindowUIC.bottom += m_vecTexOffset.y;
    rectWindowUIC.right = rectWindowUIC.left + m_vecTexScale.x * (rectWindowUIC.right - rectWindowUIC.left);
    rectWindowUIC.bottom = rectWindowUIC.top + m_vecTexScale.y * (rectWindowUIC.bottom - rectWindowUIC.top);

    Rect2D rectWindowPixels;
    Rect2D rectClippingPixels;
    if (m_bNoSnap)
    {
        ConvertUICToPixelsDoNotSnap(rectWindowUIC, rectWindowPixels);
        ConvertUICToPixelsDoNotSnap(rectClippingUIC, rectClippingPixels);
    }
    else
    {
        ConvertUICToPixels(rectWindowUIC, rectWindowPixels);
        ConvertUICToPixels(rectClippingUIC, rectClippingPixels);
    }

    switch (m_eTiledStyle)
    {
    case TILEDSTYLE_NONE:
    {
        SNO snoShaderToUse = m_snoCustomShader;
        if (snoShaderToUse == SNO_NONE && m_rgbaWindowColor.a != 0 && m_rgbaWindowColor.a != 255)
        {
            snoShaderToUse = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT);
        }

        if (m_tImageTexBkgrnd.snoTexture != SNO_NONE)
        {
            if (bAlphaCascaded != 0)
            {
                RGBAColor rgbaColor = m_rgbaWindowColor;
                rgbaColor.a = bAlphaCascaded;
                if (Math::Abs(m_aRot) > EPSILON)
                {
                    SpriteDrawDeferredRotated(m_tImageTexBkgrnd, snoShaderToUse, rectWindowPixels, rgbaColor, crgbaBlack, flZ, NULL, m_aRot, SNO_NONE, NULL, FALSE, m_eRenderLayerOverride);
                }
                else if (m_bMirrorX || m_bMirrorY)
                {
                    SpriteDrawDeferredMirrored(m_tImageTexBkgrnd, snoShaderToUse, rectWindowPixels, rgbaColor, crgbaBlack, flZ, NULL, m_bMirrorX, m_bMirrorY, SNO_NONE, NULL, FALSE, m_eRenderLayerOverride);
                }
                else
                {
                    SpriteDrawDeferred(m_tImageTexBkgrnd, m_tImageTexMask, snoShaderToUse, rectWindowPixels, rgbaColor, crgbaBlack, flZ, GetStereoDepth(), &rectClippingPixels, m_eRenderLayerOverride, FALSE);
                }
            }
        }
        else
        {
            Animation2DDraw(m_snoBackgroundAnimation, m_tBackgroundAnimState, rectWindowPixels, rectClippingPixels, bAlphaCascaded, flZ, m_rgbaWindowColor, snoShaderToUse, m_eRenderLayerOverride);
        }
        break;
    }

    case TILEDSTYLE_DEFAULT:
        sDrawTiledWindowDefault(scgtTiledPiecesDefault, rectWindowPixels, flZ, bAlphaCascaded, FALSE);
        break;

    case TILEDSTYLE_TEXT_TOOLTIP:
        sDrawTiledWindowDefault(scgtTiledPiecesTextTooltips, rectWindowPixels, flZ, bAlphaCascaded, FALSE);
        break;

    case TILEDSTYLE_SKILL_TOOLTIP:
        sDrawTiledWindowDefault(scgtTiledPiecesSkillTooltips, rectWindowPixels, flZ, bAlphaCascaded, FALSE);
        break;

    case TILEDSTYLE_CHATBALLOON:
        sDrawTiledWindowDefault(scgtTiledPiecesChatBalloon, rectWindowPixels, flZ, bAlphaCascaded, TRUE);
        break;

    case TILEDSTYLE_ARROW_VERTICAL:
        sDrawTrack(scgtPiecesArrowVertical, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_ITEM_TOOLTIP_BODY:
        sDrawTrack(scgtPiecesItemTooltipBody, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_ITEM_TOOLTIP_BODY_ANCIENT:
        sDrawTrack(scgtPiecesItemTooltipBody_Ancient, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_ITEM_TOOLTIP_BODY_PRIMAL_ANCIENT:
        sDrawTrack(scgtPiecesItemTooltipBody_PrimalAncient, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_ITEM_TOOLTIP_COST:
        sDrawTrack(scgtPiecesItemTooltipCost, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_ITEM_TOOLTIP_COST_ANCIENT:
        sDrawTrack(scgtPiecesItemTooltipCost_Ancient, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_ITEM_TOOLTIP_COST_PRIMAL_ANCIENT:
        sDrawTrack(scgtPiecesItemTooltipCost_PrimalAncient, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_ITEM_TOOLTIP_INSTRUCTION:
        sDrawTrack(scgtPiecesItemTooltipInstruction, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_ITEM_TOOLTIP_INSTRUCTION_ANCIENT:
        sDrawTrack(scgtPiecesItemTooltipInstruction_Ancient, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_ITEM_TOOLTIP_INSTRUCTION_PRIMAL_ANCIENT:
        sDrawTrack(scgtPiecesItemTooltipInstruction_PrimalAncient, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_TRAIT_TOOLTIP:
        sDrawTrack(scgtPiecesTraitTooltip, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_CONFIRMATION_DLG:
        sDrawVertTiledWindow(scgtPiecesConfirmationDlg, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_CONVERSATION_DLG:
        sDrawVertTiledWindow(scgtPiecesConversationDlg, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_FOLLOWERSWAP_DLG:
        sDrawVertTiledWindow(scgtPiecesFollowerSwapDlg, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_TUTORIAL_TL:
        sDrawVertTiledWindow(scgtPiecesTutorialTL, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_TUTORIAL_TR:
        sDrawVertTiledWindow(scgtPiecesTutorialTR, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_TUTORIAL_BL:
        sDrawVertTiledWindow(scgtPiecesTutorialBL, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_TUTORIAL_BR:
        sDrawVertTiledWindow(scgtPiecesTutorialBR, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_TUTORIAL_NO_ARROW:
        sDrawVertTiledWindow(scgtPiecesTutorialNoArrow, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_BOSS_DLG:
        sDrawVertTiledWindow(scgtPiecesBoss, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_TOOLTIP_TITLE:
        sDrawVertTiledWindow(scgtPiecesTooltipTitle, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_TOOLTIP_TITLE_ANCIENT:
        sDrawVertTiledWindow(scgtPiecesTooltipTitle_Ancient, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    case TILEDSTYLE_TOOLTIP_TITLE_PRIMAL_ANCIENT:
        sDrawVertTiledWindow(scgtPiecesTooltipTitle_PrimalAncient, rectWindowUIC, flZ, bAlphaCascaded);
        break;

    default:
        sDrawTiledWindow(rectWindowPixels, rectClippingPixels, flZ, bAlphaCascaded);
        break;
    }
}

void UIWindow::DrawChildren(const uint8 bAlphaCascaded, float & flZSlice)
{
    UIControl * ptControl = GetFirstActiveChild();
    while (ptControl != NULL)
    {
        UIControl * ptNextSibling = ptControl->GetNextActiveSibling();
        if (ptControl->GetType() == UIT_WINDOW)
        {
            static_cast<UIWindow *>(ptControl)->UIWindow::Draw(bAlphaCascaded, flZSlice);
        }
        else
        {
            ptControl->Draw(bAlphaCascaded, flZSlice);
        }
        ptControl = ptNextSibling;
    }
}

void UIWindow::Draw(const uint8 bLayerAlpha, float & flZSlice)
{
    if (AppDrawFlagIsSet(APP_DRAW_NO_UI_BIT))
    {
        return;
    }

    s_controlDrawCount++;

    uint8 bAlphaCascaded = MultiplyAlpha(bLayerAlpha, m_rgbaWindowColor.a);
    if (bAlphaCascaded == 0)
    {
        return;
    }

    sDrawSelf(bAlphaCascaded, flZSlice);
    DrawChildren(bAlphaCascaded, flZSlice);
}

inline void UIWindow::sUpdate()
{
    float flDeltaTime = CGameGetRealDeltaTime();
    Animation2DUpdate(m_tBackgroundAnimState, m_snoBackgroundAnimation, flDeltaTime);
}

BOOL UIWindow::sInput(const BufferedInput & tInput)
{
    switch (tInput.eInputMsg)
    {
    case INPUTMSG_LEFTSTICK:
        if (m_pFunctionsPtr != NULL && m_pFunctionsPtr->pfnLStickHandler != NULL)
        {
            m_pFunctionsPtr->pfnLStickHandler(GetID(), tInput.tBufControllerData.nControllerIndex);
            return TRUE;
        }
        break;

    case INPUTMSG_RIGHTSTICK:
        if (m_pFunctionsPtr != NULL && m_pFunctionsPtr->pfnRStickHandler != NULL)
        {
            m_pFunctionsPtr->pfnRStickHandler(GetID(), tInput.tBufControllerData.nControllerIndex);
            return TRUE;
        }
        break;

    case INPUTMSG_LEFTTRIGGER:
        if (m_pFunctionsPtr != NULL && m_pFunctionsPtr->pfnConsoleButtonDown[6] != NULL && tInput.tBufControllerData.nLeftTriggerPos > 5)
        {
            UI & ui = UIGet();
            if (!ui.bLeftTriggerBounceForUI)
            {
                m_pFunctionsPtr->pfnConsoleButtonDown[6](GetID(), tInput.tBufControllerData.nControllerIndex);
                ui.bLeftTriggerBounceForUI = TRUE;
            }
            return TRUE;
        }
        break;

    case INPUTMSG_RIGHTTRIGGER:
        if (m_pFunctionsPtr != NULL && m_pFunctionsPtr->pfnConsoleButtonDown[7] != NULL && tInput.tBufControllerData.nRightTriggerPos > 5)
        {
            UI & ui = UIGet();
            if (!ui.bRightTriggerBounceForUI)
            {
                m_pFunctionsPtr->pfnConsoleButtonDown[7](GetID(), tInput.tBufControllerData.nControllerIndex);
                ui.bRightTriggerBounceForUI = TRUE;
            }
            return TRUE;
        }
        break;

    case INPUTMSG_CONTROLLERBUTTONDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT:
        if (m_pFunctionsPtr != NULL && m_pFunctionsPtr->pfnConsoleButtonDown[tInput.tBufControllerData.eInputButton] != NULL)
        {
            m_pFunctionsPtr->pfnConsoleButtonDown[tInput.tBufControllerData.eInputButton](GetID(), tInput.tBufControllerData.nControllerIndex);
            return TRUE;
        }
        break;

    case INPUTMSG_CONTROLLERBUTTONSTILLDOWN:
    case INPUTMSG_CONTROLLERBUTTONUP:
        if (m_pFunctionsPtr != NULL && m_pFunctionsPtr->pfnConsoleButtonDown[tInput.tBufControllerData.eInputButton] != NULL)
        {
            return TRUE;
        }
        break;

    case INPUTMSG_CONTROLLERBUTTONHELDDOWN:
        if (m_pFunctionsPtr != NULL && m_pFunctionsPtr->pfnConsoleButtonHeld[tInput.tBufControllerData.eInputButton] != NULL)
        {
            m_pFunctionsPtr->pfnConsoleButtonHeld[tInput.tBufControllerData.eInputButton](GetID(), tInput.tBufControllerData.nControllerIndex);
            return TRUE;
        }
        break;
    }

    if ((InputIsController(tInput) || InputIsGesture(tInput)) && IsActive() && EatsClicks() != EC_IGNORE)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL UIWindow::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{
    if (ForwardMessage(msg, dwParam))
    {
        return TRUE;
    }

    switch (msg)
    {
    case UI_UPDATE:
        sUpdate();
        break;

    case UI_INPUT:
    {
        BOOL bRetval = sInput(*(const BufferedInput *)dwParam);
        if (bRetval)
        {
            return bRetval;
        }
        break;
    }

    case UI_REGISTER_INPUT_LISTENER:
        if (HandlesInput())
        {
            UI_RegisterInputListener(this);
        }
        break;

    case UI_RESET:
        Animation2DReset(m_tBackgroundAnimState, m_snoBackgroundAnimation);
        break;
    }

    return UIControl::HandleMessage(msg, dwParam);
}

BOOL UIWindow::HandlesInput()
{
    // bHandlesInput is an invented name: the DWARF records only i. Returning TRUE from inside the
    // loop instead lets block placement duplicate the return into every check.
    BOOL bHandlesInput = FALSE;
    if (m_pFunctionsPtr != NULL)
    {
        for (int32 i = 0; i < 18; i++)
        {
            if (m_pFunctionsPtr->pfnConsoleButtonDown[i] != NULL || m_pFunctionsPtr->pfnConsoleButtonHeld[i] != NULL)
            {
                bHandlesInput = TRUE;
                break;
            }
        }
        if (!bHandlesInput && (m_pFunctionsPtr->pfnLStickHandler != NULL || m_pFunctionsPtr->pfnRStickHandler != NULL))
        {
            bHandlesInput = TRUE;
        }
    }

    if (!bHandlesInput && EatsClicks() != EC_IGNORE)
    {
        bHandlesInput = TRUE;
    }

    return bHandlesInput;
}

BOOL UIWindow::ClickIsEaten(const IVector2D & p) const
{
    EatsClicksValue ecValue = EatsClicks();
    if (ecValue == EC_EAT)
    {
        return TRUE;
    }

    if (ecValue == EC_EAT_MASKED)
    {
        if (m_tImageTexBkgrnd.snoTexture == SNO_NONE)
        {
            return TRUE;
        }

        Vector2D vecUIC;
        ConvertPixelsToUIC(p, vecUIC);
        Rect2D rectWindow;
        GetWindowRect(rectWindow);
        if (rectWindow.Width() == 0.0f || rectWindow.Height() == 0.0f)
        {
            return FALSE;
        }

        Vector2D uvWindow((vecUIC.x - rectWindow.left) / rectWindow.Width(), (vecUIC.y - rectWindow.top) / rectWindow.Height());
        Rect2D rectFrame;
        {
            ScopedTextureDefinitionLockR pdefTexture(m_tImageTexBkgrnd.snoTexture);
            if (m_tImageTexBkgrnd.nFrame == -1)
            {
                rectFrame.Set(0.0f, 0.0f, 1.0f, 1.0f);
            }
            else
            {
                TextureGetFrameRectUVs(rectFrame, pdefTexture, m_tImageTexBkgrnd.nFrame);
            }
        }
        Vector2D uvTex(uvWindow.x * rectFrame.Width() + rectFrame.left, uvWindow.y * rectFrame.Height() + rectFrame.top);
        RGBAColor rgbaColor;
        TextureDefinitionPixelLookup(m_tImageTexBkgrnd.snoTexture, uvTex.x, uvTex.y, FALSE, rgbaColor);
        return rgbaColor.a >= 240;
    }

    return FALSE;
}

UIID UIWindow::HitTest(const IVector2D & pt)
{
    if (!IsActive())
    {
        return GetUIIDNone();
    }

    UIID idHit = HitTestChildren(pt);
    if (idHit != GetUIIDNone())
    {
        return idHit;
    }

    if (IsInside(pt) && ClickIsEaten(pt))
    {
        return GetID();
    }

    return GetUIIDNone();
}

UIID UIWindow::HitTestForTooltips(const IVector2D & pt, BOOL & bContinueTesting)
{
    if (!IsActive())
    {
        return GetUIIDNone();
    }

    UIID idHit = HitTestChildrenForTooltips(pt, bContinueTesting);
    if (idHit != GetUIIDNone() || !bContinueTesting)
    {
        return idHit;
    }

    if (IsInside(pt))
    {
        LPCSTR pszTooltipText = GetTooltipText();
        if (pszTooltipText != NULL && pszTooltipText[0] != '\0')
        {
            return GetID();
        }

        if (ClickIsEaten(pt))
        {
            bContinueTesting = FALSE;
        }
    }

    return GetUIIDNone();
}

void UIWindow::SetInitialEditText()
{
    for (uint32 i = 0; i < m_listChildren.size(); i++)
    {
        const UIID & idControl = m_listChildren[i].id;
        if (UI_GetControlFromID(idControl).IsType(UIT_EDIT))
        {
            UI_GetEditFromID(idControl).Clear();
        }
    }
}

void UIWindow::ResizeToFitChildren()
{
    Vector2D vecTL;
    Vector2D vecDim;
    GetRelativePosition(vecTL, vecDim);
    vecDim.Set(0.0f, 0.0f);
    for (uint32 i = 0; i < m_listChildren.size(); i++)
    {
        const UIID & idControl = m_listChildren[i].id;
        UIControl & tControl = UI_GetControlFromID(idControl);
        if (tControl.IsActive() && tControl.IsType(UIT_WINDOW))
        {
            Vector2D vecTLControl;
            Vector2D vecDimControl;
            UI_GetWindowFromID(idControl).GetRelativePosition(vecTLControl, vecDimControl);
            vecDim.x = Math::Max(vecDim.x, vecTLControl.x + vecDimControl.x);
            vecDim.y = Math::Max(vecDim.y, vecTLControl.y + vecDimControl.y);
        }
    }
    SetRelativePosition(vecTL, vecDim);
}

void UIWindow::Scroll(const Vector2D & vecScroll)
{
    Rect2D rectClip;
    GetUnshiftedClippingRect(rectClip);
    Move(vecScroll);
    SetClipRect(rectClip);
}

BOOL UIWindow::HandleContextMenu(const BufferedInput & tInput)
{
    return FALSE;
}

void UIWindow::CenterOverACD(const ACDID idACD)
{
    Vector2D vecTL;
    Vector2D vecDim;
    GetRelativePosition(vecTL, vecDim);

    Rect2D rectViewportUIC;
    CGameGetSafeViewportUIC(rectViewportUIC);
    vecTL.x = rectViewportUIC.left + (rectViewportUIC.Width() - vecDim.x) * 0.5f;
    vecTL.y = rectViewportUIC.top + (rectViewportUIC.Height() - vecDim.y) * 0.5f;
    Rect2D rectWndUIC;
    rectWndUIC.Set(vecTL.x, vecTL.y, vecTL.x + vecDim.x, vecTL.y + vecDim.y);

    RActorID idRActor = (RActorID)ACDGetOwner(idACD);
    const RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor != NULL)
    {
        Vector3D wpLoc;
        RActorGetTopPoint(ptRActor, wpLoc);
        Vector2D wpTopScreenUIC;
        if (WorldPointToScreenPointUIC(wpLoc, wpTopScreenUIC, -1) == TRUE)
        {
            const float cflGapBetweenTopOfACDAndBottomOfMenu = 35.0f;
            vecTL.x = wpTopScreenUIC.x - vecDim.x * 0.5f;
            vecTL.y = wpTopScreenUIC.y - cflGapBetweenTopOfACDAndBottomOfMenu - vecDim.y;
            rectWndUIC.Set(vecTL.x, vecTL.y, vecTL.x + vecDim.x, vecTL.y + vecDim.y);
            rectWndUIC.ConstrainToRect(rectViewportUIC, 0.0f);
        }
    }

    SetRelativePosition(Vector2D(rectWndUIC.left, rectWndUIC.top), vecDim);
}
