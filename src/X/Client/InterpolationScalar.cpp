// Original file: X/Client/InterpolationScalar.cpp

#include "InterpolationScalar.h"

#include "CGame.h"
#include "RWindow.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Attributes/FastAttributeStructs.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Random.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Not used directly: the original reaches these through an include chain, and their header-level
// statics (blz::ignore, SNONAME_NONE, cgplaceNull, the GfxVB.h normals) are part of this file's
// _GLOBAL__sub_I.
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../Common/SNO/SNOName.h"
#include "../Common/WorldPlace.h"
#include "Gfx/GfxVB.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function.
#include "Animation.h"




static_assert(sizeof(ProcessedBone) == 0x60, "ProcessedBone");
#include "RActors/RActorAnimation.h"





static_assert(sizeof(ActorAnim) == 0x1b8, "ActorAnim");
// X/Client/RActors/RActors.h:278 and :596. Only the members this file reads are declared.
struct RActor {  // size 0x410
    RActorID id;  // 0x00
    SSceneID idSScene;  // 0x04
    ACDID idACD;  // 0x08
    uint8 _unadopted0[0x148 - 0x0c];  // 0x0c (invented name: snoActor .. ptActorAnim)
    ActorAnim * ptActorAnim;  // 0x148
    uint8 _unadopted1[0x3c0 - 0x150];  // 0x150 (invented name: .. flBaseScale)
    float flBaseScale;  // 0x3c0
    uint8 _unadopted2[0x410 - 0x3c4];  // 0x3c4 (invented name: .. end)
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
// X/Client/RActors/RActorAnimation.cpp:3006
int32 RActorAnimationLookupBone(const RActor & tRActor, LPCSTR szBoneName);
// X/Client/CPlayer.cpp:1686
ACDID LocalPlayerACDGet();
// X/Client/Weather.cpp:1570
namespace Weather {
float GetIntensity(const WeatherRWindowData & tData);
}  // namespace Weather
// X/Common/Attributes/FastAttribute.cpp:2073
FastAttribKey MakeAttribKey(Attrib eAttrib, int32 nParam);

static const TypeDescriptorField InterpolationScalar_Fields[4] = {  // line 27
    TypeDescriptorField("", DT_INT, offsetof(InterpolationScalar, nScalarFunction), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(InterpolationScalar, flScalarLerpZero), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(InterpolationScalar, flScalarLerpOne), (const void *)0x3f800000, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(InterpolationScalar), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InterpolationScalar("InterpolationScalar", 0, InterpolationScalar_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_INTERPOLATION_SCALAR = &TypeDescriptor_InterpolationScalar;  // line 31

void InterpScalarParams::Init() {  // line 36
    flMsgScalar = 1.0f;
    flRadialDistance = 0.0f;
    flFractionalHeight = 0.0f;
    idParentACD = ACDID_INVALID;
    idParentRActor = RACTORID_INVALID;
    nBone = -1;
}

void InterpScalarParams::InitParamsFromRActor(const RActor * ptRActor) {  // line 50
    idParentACD = ptRActor->idACD;
    idParentRActor = ptRActor->id;
    nBone = RActorAnimationLookupBone(*ptRActor, "particle");
}

BOOL InterpScalarParams::Evaluate(const int32 nScalarFunction, float & flScalar) const {  // line 63
    flScalar = 1.0f;
    switch (nScalarFunction) {
        case SCALAR_FUNCTION_NONE:
            return FALSE;

        case SCALAR_FUNCTION_MSG_SCALAR:
            flScalar = flMsgScalar;
            break;

        case SCALAR_FUNCTION_FRACTIONAL_RADIAL_DIST:
            flScalar = flRadialDistance;
            break;

        case SCALAR_FUNCTION_FRACTIONAL_HEIGHT:
            flScalar = flFractionalHeight;
            break;

        case SCALAR_FUNCTION_WEATHER_INTENSITY:
            flScalar = Weather::GetIntensity(GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData());
            break;

        case SCALAR_FUNCTION_ATTACK_RATE: {
            ActorCommonData * ptACDParent = ACDTryToGet(idParentACD);
            if (!ptACDParent)
                return FALSE;

            flScalar = ptACDParent->AttributesGetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_ITEM_TOTAL);
            break;
        }

        case SCALAR_FUNCTION_ACTOR_HEALTH_PERCENT: {
            ActorCommonData * ptACDParent = ACDTryToGet(idParentACD);
            if (!ptACDParent)
                return FALSE;

            flScalar = ptACDParent->GetHitpointPercent();
            break;
        }

        case SCALAR_FUNCTION_GOLD_PICKUP_RADIUS: {
            ActorCommonData * ptACDParent = ACDTryToGet(idParentACD);
            if (!ptACDParent)
                return FALSE;

            float flGoldPickupRadius = ptACDParent->AttributesGetFloat(ATTRIBUTE_GOLD_PICKUP_RADIUS);
            const float PICKUP_SCALAR = 0.5f;
            const float OUTPUT_MAX = 20.0f;
            flGoldPickupRadius = Math::Pin(flGoldPickupRadius * PICKUP_SCALAR, 0.0f, OUTPUT_MAX);
            flScalar = flGoldPickupRadius / OUTPUT_MAX;
            break;
        }

        case SCALAR_FUNCTION_ACTOR_PRIMARY_RESOURCE_PERCENT: {
            ActorCommonData * ptACDParent = ACDTryToGet(idParentACD);
            if (!ptACDParent)
                return FALSE;

            ResourceType eResourceType = ptACDParent->GetPrimaryResourceType();
            float flResourceMax = ptACDParent->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_TOTAL, eResourceType));
            float flResourceCurrent = ptACDParent->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_CUR, eResourceType));
            float flPercent = 0.0f;
            if (flResourceMax > 0.000001f) {
                flPercent = Math::Pin(flResourceCurrent / flResourceMax, 0.0f, 1.0f);
            }
            flScalar = flPercent;
            break;
        }

        case SCALAR_FUNCTION_SPAWNER_COUNTDOWN_PERCENT: {
            ActorCommonData * ptACDParent = ACDTryToGet(idParentACD);
            if (!ptACDParent)
                return FALSE;

            flScalar = ptACDParent->AttributesGetFloat(ATTRIBUTE_SPAWNER_COUNTDOWN_PERCENT);
            break;
        }

        case SCALAR_FUNCTION_RANDOM:
            flScalar = RandFloat_Internal(CGameGlobalsGet().rngLocal);
            break;

        case SCALAR_FUNCTION_PLAYER_CLOSE_TO_DEATH: {
            ACDID idACDPlayer = LocalPlayerACDGet();
            ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
            if (ptACDPlayer) {
                const float flMinPctHealth = 0.25f;
                const float flMaxPctHealth = 0.33f;
                flScalar = (ptACDPlayer->GetHitpointPercent() - flMinPctHealth) / (flMaxPctHealth - flMinPctHealth);
                flScalar = 1.0f - Math::Pin(flScalar, 0.0f, 1.0f);
            } else {
                flScalar = 0.0f;
            }
            break;
        }

        case SCALAR_FUNCTION_ACTOR_SCALE:
            if (idParentACD != ACDID_INVALID) {
                ActorCommonData * ptACDParent = ACDTryToGet(idParentACD);
                if (!ptACDParent)
                    return FALSE;

                flScalar = ptACDParent->GetScale();
            } else {
                RActor * ptRActor = RActorTryToGet(idParentRActor);
                if (!ptRActor)
                    return FALSE;

                flScalar = ptRActor->flBaseScale;
            }
            break;

        case SCALAR_FUNCTION_VALUE_FROM_BONE_X: {
            flScalar = 0.0f;
            if (nBone >= 0) {
                RActor * ptRActor = RActorTryToGet(idParentRActor);
                if (ptRActor && ptRActor->ptActorAnim) {
                    const ActorAnim & tActorAnim = *ptRActor->ptActorAnim;
                    PRSTransform & xform = tActorAnim.ptProcessedBoneList[nBone].modelTransform;
                    flScalar = xform.wp.y;
                }
            }
            break;
        }

        default:
            DisplayInternalError(NULL, "", 237, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return TRUE;
}

BOOL InterpolationScalar::GetScalar(const InterpScalarParams & tParams, float & flFinalScalar) const {  // line 246
    flFinalScalar = 1.0f;
    float flScalar;
    if (!tParams.Evaluate(nScalarFunction, flScalar))
        return FALSE;

    Math::Lerp(flScalarLerpZero, flScalarLerpOne, flScalar, flFinalScalar);
    return TRUE;
}
