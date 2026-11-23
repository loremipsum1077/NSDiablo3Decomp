// Original file: X/Client/AttachmentParent.cpp

#include "AttachmentParent.h"

#include "CGame.h"
#include "CPlayer.h"
#include "Particle.h"
#include "ParticleInternal.h"
#include "Rope.h"
#include "Scene.h"
#include "Attachments.h"
#include "Definitions/CAttachmentDefinition.h"
#include "RActors/RActors.h"
#include "RActors/RActorAttachments.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/Definitions/AttachmentDefinition.h"
#include "../Common/Player.h"
#include "../Common/SceneCommon.h"
#include "../Common/TagMaps/ActorTags.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Math/Transform.h"

// Line 26: static const WorldDistance DEFAULT_PLAYER_LIGHT_RADIUS. The DWARF records the value
// 1092616192 (0x41200000 = 10.0f); the constant is unused, so it is not emitted.
static const WorldDistance DEFAULT_PLAYER_LIGHT_RADIUS = 10.0f;

void ParentData::Init() {  // line 32
    Set(-1, -1, -1, cgtPRTransformIdentity, 1.0f);
}

float ParentData::GetAlpha() const {  // line 41
    switch (m_eParentType) {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP:
    case PARENT_LOCAL_PLAYER_CENTROID: {
        const RActor * ptRActor = RActorTryToGet((RActorID)m_idParent);
        if (ptRActor && !(ptRActor->dwFlags & (1 << RACTOR_DONT_PASS_ALPHA_TO_CHILDREN_BIT))) {
            if ((ptRActor->dwVisibilityFlags & (1 << RACTOR_NOT_IN_LOS_BIT)) ||
                (ptRActor->dwFlags & (1 << RACTOR_HIDE_ALPHA_ATTACHMENTS_BIT)))
                return 0.0f;
            return RActorGetAlpha(ptRActor) * ptRActor->flFadeAlpha * ptRActor->flTriggerFadeAlpha;
        }
        break;
    }
    case PARENT_PARTICLE_SYSTEM: {
        ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(m_idParent);
        if (ptParticleSystem)
            return ptParticleSystem->flAlpha;
        break;
    }
    case PARENT_PARTICLE: {
        Particle * ptParticle = ParticleTryToGet(m_idParent);
        if (ptParticle)
            return ptParticle->tParams.flAlpha;
        break;
    }
    case PARENT_ROPE: {
        Rope * ptRope = RopeTryToGet(m_idParent);
        if (ptRope)
            return ptRope->flRopeAlpha;
        break;
    }
    }
    return 1.0f;
}

BOOL ParentData::HasLocalPlayerAsParent() const {  // line 129
    if (ParentIsRActor(m_eParentType) && PlayerIsLocal((RActorID)m_idParent))
        return TRUE;

    switch (m_eParentType) {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP: {
        const RActor * ptRActor = RActorTryToGet((RActorID)m_idParent);
        if (ptRActor)
            return ptRActor->tParentData.HasLocalPlayerAsParent();
        break;
    }
    case PARENT_PARTICLE_SYSTEM: {
        ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(m_idParent);
        if (ptParticleSystem == NULL)
            return FALSE;
        return ptParticleSystem->tParentData.HasLocalPlayerAsParent();
    }
    case PARENT_PARTICLE: {
        Particle * ptParticle = ParticleTryToGet(m_idParent);
        if (ptParticle == NULL)
            return FALSE;
        ParticleSystem * ptParticleSystem = ParticleSystemGet(ptParticle->tParams.idParticleSystem);
        return ptParticleSystem->tParentData.HasLocalPlayerAsParent();
    }
    case PARENT_ROPE: {
        Rope * ptRope = RopeTryToGet(m_idParent);
        if (ptRope == NULL)
            return FALSE;
        return ptRope->tRopeAttachPoints[0].tParentData.HasLocalPlayerAsParent() ||
               ptRope->tRopeAttachPoints[1].tParentData.HasLocalPlayerAsParent();
    }
    case PARENT_SCENE: {
        CScene * ptCScene = CSceneTryToGet(m_idParent);
        if (ptCScene)
            return ptCScene->tParentData.HasLocalPlayerAsParent();
        break;
    }
    case PARENT_LOCAL_PLAYER_CENTROID:
        return TRUE;
    }
    return FALSE;
}

int32 ParentData::GetLocalParentPlayerIndex() const {  // line 225
    if (ParentIsRActor(m_eParentType) && PlayerIsLocal((RActorID)m_idParent))
        return PlayerGetByRActor((RActorID)m_idParent)->nPlayerIndex;

    switch (m_eParentType) {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP:
    case PARENT_LOCAL_PLAYER_CENTROID: {
        const RActor * ptRActor = RActorTryToGet((RActorID)m_idParent);
        if (ptRActor)
            return ptRActor->tParentData.GetLocalParentPlayerIndex();
        break;
    }
    case PARENT_PARTICLE_SYSTEM: {
        ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(m_idParent);
        if (ptParticleSystem)
            return ptParticleSystem->tParentData.GetLocalParentPlayerIndex();
        break;
    }
    case PARENT_PARTICLE: {
        Particle * ptParticle = ParticleTryToGet(m_idParent);
        if (ptParticle) {
            ParticleSystem * ptParticleSystem = ParticleSystemGet(ptParticle->tParams.idParticleSystem);
            return ptParticleSystem->tParentData.GetLocalParentPlayerIndex();
        }
        break;
    }
    case PARENT_ROPE: {
        Rope * ptRope = RopeTryToGet(m_idParent);
        if (ptRope) {
            for (int32 i = 0; i < 2; i++) {
                if (ptRope->tRopeAttachPoints[i].tParentData.HasLocalPlayerAsParent())
                    return ptRope->tRopeAttachPoints[i].tParentData.GetLocalParentPlayerIndex();
            }
        }
        break;
    }
    case PARENT_SCENE: {
        CScene * ptCScene = CSceneTryToGet(m_idParent);
        if (ptCScene)
            return ptCScene->tParentData.HasLocalPlayerAsParent();
        break;
    }
    }
    return -1;
}

// NON_MATCHING: the default `return -1` path is shrink-wrapped here (mov w0,#-1; ret with no
// frame); the original sets up the frame before the switch and returns -1 through the epilogue.
// Moving -1 into nDepth instead turns the case calls into tail calls, which the original lacks.
// The original looks like one shared return block (phi incl. -1) with CodeGenPrepare's dupRet not firing.
#line 324
int32 ParentData::RegisterParentObject() const
// On DWARF lines 324-391; rows and columns equal the original's (null tests at 347/359/372 precede the calls: early break).
{
	int32 nDepth = 0;
    switch (m_eParentType)
	{
	case PARENT_RACTOR:
	case PARENT_RACTOR_TOP:
	case PARENT_LOCAL_PLAYER_CENTROID:
		{
			RActor* ptRActor = RActorTryToGet((RActorID) m_idParent);
			if (!ptRActor)
			{
				break;
			}

            nDepth = RActorRegisterParentObject(ptRActor);
		}
		break;

	case PARENT_PARTICLE_SYSTEM:
		{
			ParticleSystem* ptParticleSystem = ParticleSystemTryToGet((ParticleSystemID) m_idParent);
			if (!ptParticleSystem)
			{
				break;
			}

			nDepth = ptParticleSystem->RegisterParentObject();
		}
		break;

	case PARENT_PARTICLE:
		{
			Particle* ptParticle = ParticleTryToGet((ParticleID) m_idParent);
			if (!ptParticle)
			{
				break;
			}

	        ParticleSystem* ptParticleSystem = ParticleSystemGet(ptParticle->tParams.idParticleSystem);
			nDepth = ptParticleSystem->RegisterParentObject();
		}
		break;

	case PARENT_ROPE:
		{
			Rope* ptRope = RopeTryToGet((RopeID) m_idParent);
			if (!ptRope)
			{
				break;
			}

			nDepth = RopeRegisterParentObject(ptRope->id);
		}
		break;

	case PARENT_SCENE:
		break;

	default:
		return -1;
	}

	return nDepth;


}
#line 201

void ParentData::Set(const ParentType eParentType, const DataID idParent, const Hardpoint & tHardpoint) {  // line 396
    float flAttachmentScaleFactor = 1.0f;
    if (ParentIsRActor(eParentType)) {
        const RActor * ptRActorParent = RActorTryToGet((RActorID)idParent);
        if (ptRActorParent && !(tHardpoint.tHash == LEFT_SHOULDER_HARDPOINT_NAME) &&
            !(tHardpoint.tHash == RIGHT_SHOULDER_HARDPOINT_NAME) && !(tHardpoint.tHash == HELM_HARDPOINT_NAME))
            flAttachmentScaleFactor = ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_ATTACHMENT_SCALE_FACTOR, ptRActorParent->snoActor);
    }
    Set(eParentType, idParent, tHardpoint.nBoneIndex, tHardpoint.transform, flAttachmentScaleFactor);
}

void ParentData::SetKnot(const RopeID idParent, const int32 nKnotIndex) {  // line 422
    Set(PARENT_ROPE, idParent, nKnotIndex, cgtPRTransformIdentity, 1.0f);
}

void ParentData::SetHardpointIndex(const ParentType eParentType, const DataID idParent, const int32 nHardpointIndex) {  // line 432
    m_eParentType = eParentType;
    m_idParent = idParent;
    m_nBoneIndex = -1;
    m_transform = cgtPRTransformIdentity;

    if (eParentType == PARENT_RACTOR_TOP) {
        const RActor * ptRActorParent = RActorTryToGet((RActorID)idParent);
        if (ptRActorParent) {
            Hardpoint tHardpoint;
            RActorGetHardpointByIndex(*ptRActorParent, nHardpointIndex, tHardpoint);
            m_nBoneIndex = tHardpoint.nBoneIndex;
            m_transform = ptRActorParent->transformClient;
            RActorGetTopPoint(ptRActorParent, m_transform.wp);
        }
    } else if (eParentType == PARENT_RACTOR || eParentType == PARENT_LOCAL_PLAYER_CENTROID) {
        const RActor * ptRActorParent = RActorTryToGet((RActorID)idParent);
        if (ptRActorParent) {
            Hardpoint tHardpoint;
            RActorGetHardpointByIndex(*ptRActorParent, nHardpointIndex, tHardpoint);
            m_nBoneIndex = tHardpoint.nBoneIndex;
            m_transform = tHardpoint.transform;
        }
    }
}

void ParentData::SetHardpoint(const ParentType eParentType, const DataID idParent, HardpointHash szHardpointName) {  // line 479
    m_eParentType = eParentType;
    m_idParent = idParent;
    m_nBoneIndex = -1;
    m_transform = cgtPRTransformIdentity;

    if (eParentType == PARENT_RACTOR_TOP) {
        const RActor * ptRActorParent = RActorTryToGet((RActorID)idParent);
        if (ptRActorParent) {
            int32 nHardpointIndex = HardpointIndexFromName(ptRActorParent->snoAppearance, szHardpointName);
            Hardpoint tHardpoint;
            RActorGetHardpointByIndex(*ptRActorParent, nHardpointIndex, tHardpoint);
            m_nBoneIndex = tHardpoint.nBoneIndex;
            m_transform = ptRActorParent->transformClient;
            RActorGetTopPoint(ptRActorParent, m_transform.wp);
        }
    } else if (eParentType == PARENT_RACTOR || eParentType == PARENT_LOCAL_PLAYER_CENTROID) {
        const RActor * ptRActorParent = RActorTryToGet((RActorID)idParent);
        if (ptRActorParent) {
            int32 nHardpointIndex = HardpointIndexFromName(ptRActorParent->snoAppearance, szHardpointName);
            Hardpoint tHardpoint;
            RActorGetHardpointByIndex(*ptRActorParent, nHardpointIndex, tHardpoint);
            m_nBoneIndex = tHardpoint.nBoneIndex;
            m_transform = tHardpoint.transform;
        }
    }
}

BOOL ParentData::ParentIsValid() const {  // line 529
    switch (m_eParentType) {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP:
        return RActorTryToGet((RActorID)m_idParent) != NULL;
    case PARENT_PARTICLE_SYSTEM:
        return ParticleSystemTryToGet(m_idParent) != NULL;
    case PARENT_PARTICLE:
        return ParticleTryToGet(m_idParent) != NULL;
    case PARENT_ROPE:
        return RopeTryToGet(m_idParent) != NULL;
    case PARENT_SCENE:
        return CSceneTryToGet(m_idParent) != NULL;
    default:
        return FALSE;
    case PARENT_LOCAL_PLAYER_CENTROID:
        return TRUE;
    }
}

void ParentData::GetTransform(PRTransform & transform) {  // line 564
    switch (m_eParentType) {
    case PARENT_RACTOR_TOP: {
        const RActor * ptRActor = RActorGet((RActorID)m_idParent);
        transform = ptRActor->transformClient;
        RActorGetTopPoint(ptRActor, transform.wp);
        break;
    }
    case PARENT_RACTOR: {
        const RActor * ptRActor = RActorGet((RActorID)m_idParent);
        RActorGetHardpointTransform(*ptRActor, m_nBoneIndex, m_transform, transform);
        break;
    }
    case PARENT_LOCAL_PLAYER_CENTROID: {
        int32 nLocalPlayerCounter = 0;
        Player * ptPlayer = PlayerGetFirstInGame();
        transform.wp = cgwpNull;
        transform.q = cgqIdentity;
        Vector3D lastTransform(cgwpNull);
        while (ptPlayer) {
            if (PlayerIsLocal(ptPlayer) && PlayerIsInWorld(*ptPlayer)) {
                PRTransform playerTransform;
                const RActor * ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
                if (ptRActor) {
                    RActorGetHardpointTransform(*ptRActor, m_nBoneIndex, m_transform, playerTransform);
                    lastTransform = playerTransform.wp;
                    const ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);
                    if (tACD.IsAlive()) {
                        transform.wp += playerTransform.wp;
                        nLocalPlayerCounter++;
                    }
                }
            }
            ptPlayer = PlayerGetNextInGame(ptPlayer);
        }
        if (nLocalPlayerCounter > 0)
            transform.wp *= 1.0f / nLocalPlayerCounter;
        else
            transform.wp = lastTransform;
        break;
    }
    case PARENT_PARTICLE: {
        Particle * ptParticle = ParticleGet(m_idParent);
        transform.wp = ptParticle->tParams.wpLast;
        transform.q.FromArc(cgwvXAxis, ptParticle->tParams.wvLastMotionNormalized);
        break;
    }
    case PARENT_PARTICLE_SYSTEM: {
        ParticleSystem * ptParticleSystem = ParticleSystemGet(m_idParent);
        transform.Set(ptParticleSystem->qOrientation, ptParticleSystem->wp);
        break;
    }
    case PARENT_ROPE: {
        Rope * ptRope = RopeGet(m_idParent);
        RopeGetHardpointOrientation(ptRope, m_nBoneIndex, transform);
        break;
    }
    case PARENT_SCENE: {
        CScene * ptCScene = CSceneGet(m_idParent);
        transform = ptCScene->tSCD.transformWorld * m_transform;
        break;
    }
    default:
        DisplayInternalError(NULL, "", 667, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}

float ParentData::GetScale(const PRTransform & transform) const {  // line 730
    float ftScale = 1.0f;
    if (m_eParentType == PARENT_LOCAL_PLAYER_CENTROID) {
        float ftMaxDistSq = 0.0f;
        Player * ptPlayer = PlayerGetFirstInGame();
        while (ptPlayer) {
            if (PlayerIsLocal(ptPlayer)) {
                PRTransform playerTransform;
                const RActor * ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
                if (ptRActor) {
                    RActorGetHardpointTransform(*ptRActor, m_nBoneIndex, m_transform, playerTransform);
                    const ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);
                    if (tACD.IsAlive()) {
                        float ftDistSq = Vector3D::DistanceSquared(playerTransform.wp, transform.wp);
                        if (ftDistSq > ftMaxDistSq)
                            ftMaxDistSq = ftDistSq;
                    }
                }
            }
            ptPlayer = PlayerGetNextInGame(ptPlayer);
        }
        if (ftMaxDistSq > 100.0f)
            ftScale = sqrtf(ftMaxDistSq) / 10.0f;
    }
    return ftScale;
}
