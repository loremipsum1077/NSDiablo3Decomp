// Original file: X/Client/UserInterface/Controls/UIActorTagControl.cpp
#include "UIActorTagControl.h"
#include "X/Client/Camera/CameraManager.h"
#include "X/Client/Definitions/CAttachmentDefinition.h"
#include "X/Client/Font/Font.h"
#include "X/Client/RActors/RActors.h"
#include "X/Common/ActorCommonData.h"
#include "X/Sigma/Math/Rect.h"
#include "X/Sigma/Math/Transform.h"

// Defined in X/Client/RActors/RActorAttachments.cpp:2248.
void RActorGetHardpointTransformByIndex(const RActor & tRActor, int32 nHardpointIndex,
                                       PRTransform & transform);

void UIActorTag::sInit() {
    m_idACD = ACDID_INVALID;
    m_dwExpirationTime = 0;
    m_offset.Set(0.0f, 0.0f);
}

UIActorTag::UIActorTag(const TextCreationParams & tParams, const Vector2D & vecTL,
                       const Vector2D & vecDim)
    : UIText(tParams, vecTL, vecDim) {
    sInit();
}

UIActorTag::UIActorTag(const TextCreationParams & tParams, const Rect2D * pRect)
    : UIText(tParams, pRect) {
    sInit();
}

void UIActorTag::Draw(const uint8 bLayerAlpha, float & flZSlice) {
    const RActorID idRActor = (RActorID)ACDGetOwner(m_idACD);
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor == NULL)
        return;

    int32 nHardpointIndex = HardpointIndexFromName(ptRActor->snoAppearance, AURA_HARDPOINT_NAME);
    if (nHardpointIndex == -1)
        return;

    PRTransform transform;
    RActorGetHardpointTransformByIndex(*ptRActor, nHardpointIndex, transform);
    Vector2D wpScreenUIC;
    if (!WorldPointToScreenPointUIC(transform.wp, wpScreenUIC, -1))
        return;

    wpScreenUIC += m_offset;
    Vector2D vecTextSizeUIC;
    TextRenderProperties tProps(GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar());
    FontStringGetRenderedSizeUIC(tProps, GetText(), vecTextSizeUIC);
    Rect2D rectUIC;
    rectUIC.Set(wpScreenUIC.x - vecTextSizeUIC.x * 0.5f, wpScreenUIC.y,
                wpScreenUIC.x + vecTextSizeUIC.x * 0.5f, wpScreenUIC.y + vecTextSizeUIC.y);
    SetWindowRect(rectUIC);
    UIText::Draw(bLayerAlpha, flZSlice);
}
