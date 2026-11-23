#include "UIStoryboard.h"

#include "../CGame.h"
#include "../Definitions/UIDialogDefinition.h"
#include "Controls/UIBlinkerControl.h"
#include "Controls/UIWindowControl.h"
#include "UIAnimation.h"
#include "UIAttributes.h"
#include "UIID.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/Unicode.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
struct PlaySoundOptions;
typedef uint32 SoundID;
// X/Client/Sound/Sound.cpp
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIBlinker & UI_GetBlinkerFromID(const UIID & id);
BOOL UI_TestID(const UIID & id);
// X/Client/UserInterface/Interface/UIFunctions.cpp
UIFunctionPtr UIFunction_Lookup(LPCSTR szFunction);

struct UIManagedAnimationNode {  // size 0x10
    UIAnimationType m_eType;  // 0x00
    void * m_pAnimation;  // 0x08

    ~UIManagedAnimationNode()
    {
        if (m_pAnimation)
        {
            switch (m_eType)
            {
                case 0:
                    ::delete (UIAnimation<int> *)m_pAnimation;
                    break;
                case 1:
                    ::delete (UIAnimation<float> *)m_pAnimation;
                    break;
                case 2:
                    ::delete (UIAnimation<Vector2D> *)m_pAnimation;
                    break;
            }
        }
    }
};

struct UIStoryboardGlobals {  // size 0x80
    XGrowableMap<unsigned long, unsigned long, UIManagedAnimationNode *, UIManagedAnimationNode *> tManagedAnimations;  // 0x00




    UIStoryboardGlobals() : tManagedAnimations(16, NULL) {}

    ~UIStoryboardGlobals()
    {
        POS pos = tManagedAnimations.GetStartAssoc();
        while (pos)
        {
            uint64 nKey;
            UIManagedAnimationNode * pNode;
            tManagedAnimations.GetNextAssoc(pos, nKey, pNode);





            tManagedAnimations.RemoveAt(nKey);
            delete pNode;
        }
    }
};

static UIStoryboardGlobals * sgpUIStoryboardGlobals;

static const uint32 sgearAnimPropertyTypes[9] = {
    3,  // APROP_LEFT: int32 or float
    3,  // APROP_TOP
    3,  // APROP_WIDTH
    3,  // APROP_HEIGHT
    4,  // APROP_LEFT_AND_TOP: Vector2D
    4,  // APROP_WIDTH_AND_HEIGHT
    1,  // APROP_ALPHA: int32
    2,  // APROP_BLINKER_ALPHA_SPEED: float
    2,  // APROP_BLINKER_ROTATION_SPEED
};

template <typename C, int eAnimType>
class ManagedAnimation : public UIAnimation<C> {
    ManagedAnimation(LPCSTR);
    ManagedAnimation(const UIID & id) : m_id(id) {}
public:
    const UIID & GetID() const;
    static ManagedAnimation<C, eAnimType> * CreateAnimation(const UIID & id);
    static ManagedAnimation<C, eAnimType> * CreateAnimation(LPCSTR);
    static ManagedAnimation<C, eAnimType> * GetAnimation(const UIID & id);
    static ManagedAnimation<C, eAnimType> * GetAnimation(LPCSTR);
    virtual ~ManagedAnimation();
private:
    UIID m_id;  // 0x60
};

void UI_StoryboardsInitializeForGame()
{
    sgpUIStoryboardGlobals = new UIStoryboardGlobals;
}

void UI_StoryboardsDisposeForGame()
{
    delete sgpUIStoryboardGlobals;
    sgpUIStoryboardGlobals = NULL;
}

static void sHandleInt32Animation(float flT, UIInt32Animation & tInt32Animation, UIAnimationProperty eProperty, const UIID & tControlId, float flScaleFactor)
{
    int32 nValue;
    if (!tInt32Animation.Evaluate(flT, nValue))
    {
        return;
    }

    nValue = Math::FloatToInt(flScaleFactor * nValue);

    UIWindow & tControl = UI_GetWindowFromID(tControlId);

    switch (eProperty)
    {
        case APROP_LEFT:
        {
            Vector2D vecTL, vecDim;
            tControl.GetRelativePosition(vecTL, vecDim);
            vecTL.x = (float)nValue;
            tControl.SetRelativePosition(vecTL, vecDim);
        }
        break;

        case APROP_TOP:
        {
            Vector2D vecTL, vecDim;
            tControl.GetRelativePosition(vecTL, vecDim);
            vecTL.y = (float)nValue;
            tControl.SetRelativePosition(vecTL, vecDim);
        }
        break;

        case APROP_WIDTH:
        {
            Vector2D vecTL, vecDim;
            tControl.GetRelativePosition(vecTL, vecDim);
            vecDim.x = (float)nValue;
            tControl.SetRelativePosition(vecTL, vecDim);
        }
        break;

        case APROP_HEIGHT:
        {
            Vector2D vecTL, vecDim;
            tControl.GetRelativePosition(vecTL, vecDim);
            vecDim.y = (float)nValue;
            tControl.SetRelativePosition(vecTL, vecDim);
        }
        break;

        case APROP_ALPHA:
            tControl.SetTransparency((uint8)nValue);
            break;

        default:
            break;
    }
}

static void sHandleFloatAnimation(float flT, UIFloatAnimation & tFloatAnimation, UIAnimationProperty eProperty, const UIID & tControlId, float flScaleFactor)
{
    float flValue;
    if (!tFloatAnimation.Evaluate(flT, flValue))
    {
        return;
    }

    UIWindow & tControl = UI_GetWindowFromID(tControlId);

    flValue *= flScaleFactor;

    switch (eProperty)
    {
        case APROP_LEFT:
        {
            Vector2D vecTL, vecDim;
            tControl.GetRelativePosition(vecTL, vecDim);
            vecTL.x = flValue;
            tControl.SetRelativePosition(vecTL, vecDim);
        }
        break;

        case APROP_TOP:
        {
            Vector2D vecTL, vecDim;
            tControl.GetRelativePosition(vecTL, vecDim);
            vecTL.y = flValue;
            tControl.SetRelativePosition(vecTL, vecDim);
        }
        break;

        case APROP_WIDTH:
        {
            Vector2D vecTL, vecDim;
            tControl.GetRelativePosition(vecTL, vecDim);
            vecDim.x = flValue;
            tControl.SetRelativePosition(vecTL, vecDim);
        }
        break;

        case APROP_HEIGHT:
        {
            Vector2D vecTL, vecDim;
            tControl.GetRelativePosition(vecTL, vecDim);
            vecDim.y = flValue;
            tControl.SetRelativePosition(vecTL, vecDim);
        }
        break;

        case APROP_BLINKER_ALPHA_SPEED:
            if (tControl.IsType(UIT_BLINKER))
            {
                UI_GetBlinkerFromID(tControlId).SetAlphaSpeed(flValue);
            }
            break;

        case APROP_BLINKER_ROTATION_SPEED:
            if (tControl.IsType(UIT_BLINKER))
            {
                UI_GetBlinkerFromID(tControlId).SetRotationSpeed(flValue);
            }
            break;

        default:
            break;
    }
}

static void sHandleVector2DAnimation(float flT, UIVector2DAnimation & tVector2dAnimation, UIAnimationProperty eProperty, const UIID & tControlId, float flScaleFactor)
{
    Vector2D vecValue;
    if (!tVector2dAnimation.Evaluate(flT, vecValue))
    {
        return;
    }

    vecValue *= flScaleFactor;

    UIWindow & tControl = UI_GetWindowFromID(tControlId);
    Vector2D vecTL, vecDim;
    tControl.GetRelativePosition(vecTL, vecDim);

    switch (eProperty)
    {
        case APROP_LEFT_AND_TOP:
            tControl.SetRelativePosition(vecValue, vecDim);
            break;

        case APROP_WIDTH_AND_HEIGHT:
            tControl.SetRelativePosition(vecTL, vecValue);
            break;

        default:
            break;
    }
}

StoryboardCreationParams::StoryboardCreationParams(const UIType eType, const UIID & id, const UIID & idParent)
    : ControlCreationParams(eType, id, idParent)
{
    sInit();
}

void StoryboardCreationParams::sInit()
{
    dwDuration = 0;
    snoPlaybackSound = -1;
    pfnOnEnd = NULL;
}

void StoryboardCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{
    ControlCreationParams::Load(snoUI, pControlData);

    dwDuration = (uint32)(pControlData->GetFloat32Value(ATT_DURATION) * 60.0f);
    snoPlaybackSound = SNOFileLookupGUID(SOUND_SNO, pControlData->GetAttributeValue(ATT_PLAYBACK_SOUND));
    pfnOnEnd = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONEND));
}

UIStoryboard::UIStoryboard(const StoryboardCreationParams & tParams)
    : UIControl(tParams),
      m_flElapsedTime(0.0f),
      m_bLooping(FALSE),
      m_bPlaying(FALSE)
{
    m_flTotalDurationTime = tParams.dwDuration * (1.0f / 60.0f);
    m_snoPlaybackSound = tParams.snoPlaybackSound;
    m_pfnOnEnd = tParams.pfnOnEnd;

    sLoadAnimations();
}

UIStoryboard::~UIStoryboard()
{
    if (m_bPlaying)
    {
        m_bPlaying = FALSE;
    }

    ClearAnimationBindings();
}

void UIStoryboard::sLoadAnimations()
{
    const UIControlSourceKey & tSrcKey = GetUISrcKey();
    if (tSrcKey.snoUI == -1)
    {
        return;
    }

    ScopedUIDialogDefinitionLockR pdefUI(tSrcKey.snoUI);

    UIControlData * pControlData;
    BOOL fSuccess = UI_GetControlData(tSrcKey, pdefUI, pControlData);
    if (!fSuccess)
    {
        return;
    }

    UIAnimationElements & tAnimElements = pControlData->tAnimationElements;
    const UIID & id = GetID();

    for (uint32 i = 0; i < tAnimElements.dwNumAnimationDescriptions; ++i)
    {
        UIAnimationDescription & tAnimationDesc = tAnimElements.ptAnimationDescriptions[i];

        UIID idFinal;
        idFinal.SetFromParent(id, tAnimationDesc.szName);

        switch (tAnimationDesc.eType)
        {
            case 0:
            {
                UIInt32Animation * pAnimation = UIInt32Animation::CreateAnimation(idFinal);
                if (pAnimation)
                {
                    for (int32 j = tAnimationDesc.nNodeRange[0]; j < tAnimationDesc.nNodeRange[1]; ++j)
                    {
                        const UIAnimationElementInt32 & tElement = tAnimElements.ptInt32Elements[j];
                        pAnimation->AddNode(tElement.flT, tElement.nValue, 2);
                    }
                }
            }
            break;

            case 1:
            {
                UIFloatAnimation * pAnimation = UIFloatAnimation::CreateAnimation(idFinal);
                if (pAnimation)
                {
                    for (int32 j = tAnimationDesc.nNodeRange[0]; j < tAnimationDesc.nNodeRange[1]; ++j)
                    {
                        const UIAnimationElementFloat & pElement = tAnimElements.ptFloatElements[j];
                        pAnimation->AddNode(pElement.flT, pElement.flValue, 2);
                    }
                }
            }
            break;

            case 2:
            {
                UIVector2DAnimation * pAnimation = UIVector2DAnimation::CreateAnimation(idFinal);
                if (pAnimation)
                {
                    for (int32 j = tAnimationDesc.nNodeRange[0]; j < tAnimationDesc.nNodeRange[1]; ++j)
                    {
                        const UIAnimationElementVector2D & pElement = tAnimElements.ptVector2DElements[j];
                        pAnimation->AddNode(pElement.flT, Vector2D(pElement.flVector[0], pElement.flVector[1]), 2);
                    }
                }
            }
            break;
        }
    }
}

BOOL UIStoryboard::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{
    switch (msg)
    {
        case UI_UPDATE:
        {
            const UIID & id = GetID();
            float flDeltaTime = CGameGetRealDeltaTime();

            if (m_pControlFunctionsPtr != NULL && m_pControlFunctionsPtr->pfnOnUpdate != NULL)
            {
                m_pControlFunctionsPtr->pfnOnUpdate(id);
            }

            if (m_bPlaying && !Advance(flDeltaTime))
            {
                Stop();
            }
        }
        break;
    }

    return UIControl::HandleMessage(msg, dwParam);
}

BOOL UIStoryboard::Play(const BOOL bLoop, const float flInitialElapsedTime)
{
    if (m_animationBindings.IsEmpty())
    {
        return FALSE;
    }

    m_bLooping = bLoop;
    if (!m_bPlaying)
    {
        m_flElapsedTime = flInitialElapsedTime;
        m_bPlaying = TRUE;
        Advance(0.0f);
    }

    return TRUE;
}

BOOL UIStoryboard::Stop()
{
    if (m_bPlaying)
    {
        if (m_pfnOnEnd)
        {
            m_pfnOnEnd(GetID());
        }

        m_bPlaying = FALSE;
        return TRUE;
    }

    return FALSE;
}

void UIStoryboard::SkipToEnd()
{
    if (!m_bLooping)
    {
        Advance(m_flTotalDurationTime);
        Stop();
    }
}

BOOL UIStoryboard::BindAnimation(const UIID & idControl, LPCSTR szAnimId, UIAnimationProperty eAnimProperty, float flScaleFactor)
{
    UIID id;
    id.FromLabel(szAnimId);
    return BindAnimation(idControl, id, eAnimProperty, flScaleFactor);
}

BOOL UIStoryboard::BindAnimation(const UIID & tUIControlId, const UIID & tAnimId, UIAnimationProperty eAnimProperty, float flScaleFactor)
{
    UIControl & tControl = UI_GetControlFromID(tUIControlId);
    if (!tControl.IsType(UIT_WINDOW))
    {
        return FALSE;
    }

    UIManagedAnimationNode * pNode;
    if (!sgpUIStoryboardGlobals->tManagedAnimations.Lookup(tAnimId, pNode))
    {
        return FALSE;
    }

    if (!(sgearAnimPropertyTypes[eAnimProperty] & (1 << pNode->m_eType)))
    {
        return FALSE;
    }

    AnimationBinding * binding = new AnimationBinding;
    binding->nAnimationId = tAnimId;
    binding->eAnimatedProperty = eAnimProperty;
    binding->idAnimatedControl = tUIControlId;
    binding->flScaleFactor = flScaleFactor;

    m_animationBindings.AddTail_Tracked(binding);
    return TRUE;
}

void UIStoryboard::ClearAnimationBindings()
{
    POS pos = m_animationBindings.GetHeadPosition();
    while (pos)
    {
        AnimationBinding * binding = m_animationBindings.GetNext(pos);
        delete binding;
    }

    m_animationBindings.RemoveAll();
}

// NON_MATCHING: 1760 vs 1752 bytes. Same instructions apart from the tail merge: the original
// shares one SetRelativePosition call between the int32/float handlers and both Vector2D cases,
// ours keeps the two Vector2D calls separate, which changes the block layout.
BOOL UIStoryboard::Advance(float flDeltaTime)
{
    if (m_animationBindings.IsEmpty() || m_flTotalDurationTime == 0.0f)
    {
        return FALSE;
    }

    BOOL fPlaySound = (m_flElapsedTime == 0.0f);
    BOOL fKeepGoing;

    m_flElapsedTime += flDeltaTime;

    if (m_flElapsedTime >= m_flTotalDurationTime)
    {
        if (m_bLooping)
        {
            float flTrunc = Math::FloatTrunc(m_flElapsedTime / m_flTotalDurationTime);
            m_flElapsedTime -= flTrunc;
            fKeepGoing = TRUE;
            fPlaySound = TRUE;
        }
        else
        {
            m_flElapsedTime = m_flTotalDurationTime;
            fKeepGoing = FALSE;
        }
    }
    else
    {
        fKeepGoing = TRUE;
    }

    if (fPlaySound)
    {
        SoundManager::PlaySound(m_snoPlaybackSound, cgtPSODefaults);
    }

    float flT = m_flElapsedTime / m_flTotalDurationTime;

    POS pos = m_animationBindings.GetHeadPosition();
    while (pos)
    {
        POS posOld = pos;
        AnimationBinding * ptBinding = m_animationBindings.GetNext(pos);

        if (ptBinding != NULL && UI_TestID(ptBinding->idAnimatedControl))
        {
            UIManagedAnimationNode * pNode;
            if (sgpUIStoryboardGlobals->tManagedAnimations.Lookup(ptBinding->nAnimationId, pNode))
            {
                switch (pNode->m_eType)
                {
                    case 0:
                    {
                        UIInt32Animation * pInt32Animation = (UIInt32Animation *)pNode->m_pAnimation;
                        sHandleInt32Animation(flT, *pInt32Animation, ptBinding->eAnimatedProperty, ptBinding->idAnimatedControl, ptBinding->flScaleFactor);
                    }
                    break;

                    case 1:
                    {
                        UIFloatAnimation * pFloatAnimation = (UIFloatAnimation *)pNode->m_pAnimation;
                        sHandleFloatAnimation(flT, *pFloatAnimation, ptBinding->eAnimatedProperty, ptBinding->idAnimatedControl, ptBinding->flScaleFactor);
                    }
                    break;

                    case 2:
                    {
                        UIVector2DAnimation * pVector2dAnimation = (UIVector2DAnimation *)pNode->m_pAnimation;
                        sHandleVector2DAnimation(flT, *pVector2dAnimation, ptBinding->eAnimatedProperty, ptBinding->idAnimatedControl, ptBinding->flScaleFactor);
                    }
                    break;
                }
            }
        }
        else
        {
            m_animationBindings.RemoveAt(posOld);
            delete ptBinding;
        }
    }

    return fKeepGoing;
}

template <typename C, int eAnimType>
ManagedAnimation<C, eAnimType> * ManagedAnimation<C, eAnimType>::CreateAnimation(const UIID & id)
{
    UIManagedAnimationNode * ptNode;
    if (sgpUIStoryboardGlobals->tManagedAnimations.Lookup(id, ptNode))
    {
        return NULL;
    }

    ManagedAnimation<C, eAnimType> * ptNewAnimation = new ManagedAnimation<C, eAnimType>(id);

    ptNode = new UIManagedAnimationNode;
    ptNode->m_eType = eAnimType;
    ptNode->m_pAnimation = ptNewAnimation;
    sgpUIStoryboardGlobals->tManagedAnimations.SetAt_Tracked(id, ptNode);

    return ptNewAnimation;
}

template <typename C, int eAnimType>
ManagedAnimation<C, eAnimType>::~ManagedAnimation()
{
    UIManagedAnimationNode * pNode;
    if (sgpUIStoryboardGlobals->tManagedAnimations.Lookup(m_id, pNode))
    {
        sgpUIStoryboardGlobals->tManagedAnimations.RemoveAt(m_id);
        pNode->m_pAnimation = NULL;
        delete pNode;
    }
}

template class ManagedAnimation<int, 0>;
template class ManagedAnimation<float, 1>;
template class ManagedAnimation<Vector2D, 2>;
