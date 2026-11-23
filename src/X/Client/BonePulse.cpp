// Original file: X/Client/BonePulse.cpp
#include "BonePulse.h"
#include "CGame.h"
#include "Definitions/AppearanceDefinition.h"
#include "./RActors/RActors.h"
#include "./RActors/RActorAnimation.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Transform.h"

//@ 17
BonePulse::BonePulse(RActorID idRActor, const BonePulseData * ptData)


    : m_idRActor(idRActor)
    , m_flTime(0.0f)
{
    if (ptData->flHertz > 0.0f)
    {
        m_flOmega = 6.2831855f / ptData->flHertz;
    }
    else
    {
        m_flOmega = 0.0f;
    }

    m_flPhase = ptData->aPhase;

    const float cflMaxAmplitude = 0.99f;
    m_flAmplitude = Math::Min(cflMaxAmplitude, ptData->flAmplitude);

    RActor * ptRActor = RActorGet(m_idRActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);

    const BoneStructure * ptBoneStuctures = pdefAppearance->tStructure.ptBoneStructure;

    m_nBoneIndex = -1;
    for (int32 i = 0; i < pdefAppearance->tStructure.nBoneCount; i++)
    {
        const BoneStructure * ptBoneStructure = &ptBoneStuctures[i];
        if (ptData->dwBoneHash == ptBoneStructure->dwHash)
        {
            m_nBoneIndex = i;
            break;
        }
    }

















    if (m_nBoneIndex != -1)
    {
        ptRActor->ptActorAnim->ptBoneAnimControllerList[m_nBoneIndex] = this;
    }
}

//@ 73
BonePulse::~BonePulse()
{
    if (m_nBoneIndex != -1)
    {
        ActorAnim * ptAnim;




        RActor * ptRActor = RActorGet(m_idRActor);
        ptAnim = ptRActor->ptActorAnim;

        if (ptAnim->ptBoneAnimControllerList[m_nBoneIndex] == this)
        {
            ptAnim->ptBoneAnimControllerList[m_nBoneIndex] = NULL;
        }
    }
}

//@ 90
void BonePulse::ComputeBone(RActor * ptRActor, PRSTransform & transform, const PRSTransform & parentTransform, const PRSTransform & relativeTransform, const PRSTransform & defaultRelativeTransform, int32 nBoneIndex, float flDeltaTime)







{





    PRSTransform delta(relativeTransform);
    float flFactor = 1.0f + m_flAmplitude * Math::Sin(m_flOmega * m_flTime + m_flPhase);
    delta.flScale *= flFactor;
    transform = parentTransform * delta;




    GameMode eMode = CGameGet().tGameStartParams.eMode;
    BOOL bAlive = TRUE;
    if (eMode != GAMEMODE_HOLODECK)
    {
        bAlive = FALSE;


        ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
        if (ptACD)
        {
            bAlive = ptACD->GetHitPointsCurrent() > 0.0f;
        }
    }
    if (bAlive)
    {
        m_flTime += flDeltaTime;
    }
}
