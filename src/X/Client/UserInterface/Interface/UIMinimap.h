// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIMinimap.h

#pragma once

#include "../Controls/UIWindowControl.h"
#include "../../../Sigma/Math/Matrix.h"

struct UIControlDataExt;
struct Rect2D;
struct FastAttribValue;
class ActorCommonData;
struct BufferedInput;
struct ImageTextureFrame;
struct Player;
struct WorldPlace;

namespace Minimap {
enum RenderLayer {
    LAYER_LOCAL_PLAYER = 0,
    LAYER_KNOWN_LIST = 1,
    LAYER_TRICKLE = 2,
    LAYER_SERVER = 3,
    LAYER_PETS = 4,
    LAYER_PLAYERS = 5,
};

}  // namespace Minimap
struct NameDrawInfo {  // size 0x4
    void Initialize() { m_nNameToDraw = -1; }  // line 60
    void Set(const int32);
    int32 m_nNameToDraw;  // 0x00
};
static_assert(sizeof(NameDrawInfo) == 0x4, "NameDrawInfo");

class UIMinimap : public UIWindow {  // size 0x280
public:
    static BOOL CanShowBuff(SNO snoPower, int32 nBuffIndex);
    static void HandleBuffCountAttributeChange(const FastAttribValue tValue, const SNO snoPower, const int32 nBuffIndex, const ActorCommonData & tACD);
    UIMinimap(const WindowCreationParams &, const Vector2D &, const Vector2D &);
    UIMinimap(const WindowCreationParams &, const Rect2D *);
    virtual ~UIMinimap() {}  // vtable slot 0; line 86
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 88
        return eType == UIT_MINIMAP || UIWindow::IsType(eType);
    }
    virtual UIID HitTest(const IVector2D & pt);  // vtable slot 15
    virtual UIID HitTestForTooltips(const IVector2D & pt, BOOL &);  // vtable slot 16
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL HandlesInput() { return TRUE; }  // vtable slot 13; line 113
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    void Initialize(const BOOL fFullScreen, const UIID & idArrowRegion);
    void MapTooltip();
    void UpdateZoom();
    BOOL UpdateTransform();
    BOOL MarkerVisibleOnMinimap(const Vector2D & vMarkerPlace) const;
    void sWorldToMapUIC(const Vector2D & pointWorld, Vector2D & pointUIC) const;
private:
    void sInit();
    enum {
        DMF_CAN_TURN_INTO_ARROW_BIT = 0,
        DMF_FORCE_SHOW_NAME_BIT = 1,
        DMF_USE_ACTOR_ORIENTATION_BIT = 2,
        DMF_FORCE_DEST_PING = 3,
        DMF_USE_PLAYER_ARROWS = 4,
        DMF_PARTY_GUIDE = 5,
        DMF_COMBAT = 6,
        DMF_COMBAT_WITH_ELITE = 7,
        DMF_TREASURE_GOBLIN = 8,
        DMF_KEY_WARDEN = 9,
    };
    void sResetScrollOffset();
    void sResetZoom();
    BOOL sInput(const BufferedInput & tInput);
    void sHandleControllerScrolling(const BufferedInput & tInput);
    void sHandleControllerZoom(const BufferedInput & tInput);
    uint8 sAlphaFromMapPosition(const Vector2D &, const Rect2D &);
    void sDrawMap(float & flZSlice);
    void sDrawMapMarkers(float & flZSlice);
    void sDrawMapObjectiveCones(float & flZSlice);
    void sDrawMarker(const SNO snoShaderMarker, const ImageTextureFrame & tImageTexFrame, const Vector2D & vecUIC, const float flZ, const uint32 dwFlags, ACDID idACDMap, uint8 nNameAlpha, float flYawOffset);
    BOOL sMarkerIsInArrowRegion(const Vector2D & vecUIC) const;
    void sDrawArrow(const Vector2D & vecUIC, const float flZ, const uint32 dwFlags, ACDID idACDMap, uint8 nNameAlpha);
    void sDrawCone(Vector2D vecUIC, Angle aRot, const float flZ);
    void sUpdate();
    void sRecalculateRegions();
    void sCenterOnWorldPlace(const WorldPlace & tPlace);
    void sCenterOnPlayer(int32);
    float sGetZoomDistance() const;
    void sDrawListMarkers(float flZ, SWorldID idSCurrentWorld, BOOL bUsePingList);
    void DrawServerMarkers(float flZ, SWorldID idSCurrentWorld);
    void DrawKnownListMarkers(float & flZ, SWorldID idSWorldCurrent);
    void DrawTrickleMarkers(float & flZ, SWorldID idSWorldCurrent);
    void DrawPlayerMarkers(float & flZ, SWorldID idSWorldCurrent);
    void DrawPetMarkers(float & flZ);
    void DrawPetMarkersForPlayer(Player * ptPlayer, float & flZ);
    void DrawLocalPlayerMarker(float & flZ);
    enum {
        UIM_DRAW_ARROWS_BIT = 0,
        UIM_ALLOW_SCROLLING_BIT = 1,
        UIM_ALWAYS_DRAW_NAMES_BIT = 2,
    };
public:
    uint32 m_dwFlags;  // 0x1cc
    UIID m_idArrowRegion;  // 0x1d0
    Rect2D m_rectMap;  // 0x1d8
    Rect2D m_rectArrowRegion;  // 0x1e8
    Vector2D m_pointScrollOffset;  // 0x1f8
    Vector2D m_pointScrollOffsetPrev;  // 0x200
    uint8 m_bAlpha;  // 0x208
    float m_flFeetPerPixel;  // 0x20c
    float m_flFeetPerPixelTarget;  // 0x210
    float m_flZoomScaleFactor;  // 0x214
    SNO m_snoTextureMask;  // 0x218
    BOOL m_bInputBleedOverCleared;  // 0x21c
    Matrix3x3 m_mWorldToUIC;  // 0x220
    Matrix3x3 m_mUICToWorld;  // 0x244
    NameDrawInfo m_tNameDrawInfo;  // 0x268
    uint32 m_dwLastTooltipTime;  // 0x26c
    BOOL m_fRound;  // 0x270
    uint32 m_nPingCount;  // 0x274
    uint32 m_nPrevPingCount;  // 0x278
};
static_assert(sizeof(UIMinimap) == 0x280, "UIMinimap");

