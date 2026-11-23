#include "dmMouseJoint.h"
#include "dmJoint.h"
#include "../dmBody.h"
#include "../dmSolver.h"

#line 62
void dmMouseJoint::Create(const dmMouseJointDef* pDef) {
    m_target = pDef->m_target;
    m_localAnchor = pDef->m_localAnchor;
    m_frequencyHz = pDef->m_frequencyHz;
    m_dampingRatio = pDef->m_dampingRatio;
    m_maxForce = pDef->m_maxForce;
    m_impulse = vdupq_n_f32(0.0f);
    m_gamma = 0.0f;
    m_bias = vdupq_n_f32(0.0f);
}





#line 77
void dmMouseJoint::InitVelocityConstraints(const dmBody* pBodyB, const dmJoint* pBase, const dmSolverData* pData) {

	dm_int32 islandIndexB = pBase->m_islandIndexB;


	dm_float32 massB = pBodyB->m_mass;

	// Original folded constant is two pi, 0x40c90fdb; columns 21-32 fit "2.0f * dm_pi" but no such name is in the CU.
	dm_float32 omega = 6.28318530717958647692f * m_frequencyHz;


	dm_float32 d = 2.0f * massB * m_dampingRatio * omega;


	dm_float32 k = massB * omega * omega;


	dm_float32 h = dmF32(pData->step.dt);
	m_gamma = h * (d + h * k);
	if (m_gamma > 0.0f)
	{
		m_gamma = 1.0f / m_gamma;
	}


	dmTransform transformB = pBodyB->m_transform;
	dm_vec4 localCenterB = pBodyB->m_sweep.m_localCenter;
	// Parentheses from the two subtraction columns (24 and 40).
	dm_vec4 C = (m_target - transformB.p) - dmRotate(transformB.q, m_localAnchor);
	m_bias = dmV4(h * k * m_gamma) * C;



	dm_vec4 rB = dmRotate(transformB.q, m_localAnchor - localCenterB);
	m_J = dmMtxSetSkew(-rB);


	m_B = dmMul33(pData->aInvIs[islandIndexB], m_J);
	m_B *= -1.0f;


	dm_vec4 invMassB = dmV4(pBase->m_invMassB);
	dmMtx K = dmMul33(m_J, m_B);
	K.rx += (invMassB + dmV4(m_gamma)) * dm_vec4_unitX;
	K.ry += (invMassB + dmV4(m_gamma)) * dm_vec4_unitY;
	K.rz += (invMassB + dmV4(m_gamma)) * dm_vec4_unitZ;

	m_mass = dmInvertSym33(K);


	pData->aVelocities[islandIndexB].w *= 0.98f;


	pData->aVelocities[islandIndexB].v += invMassB * m_impulse;
	pData->aVelocities[islandIndexB].w += dmMul33(m_B, m_impulse);
}

#line 135
void dmMouseJoint::SolveVelocityConstraints(dmSolverData* pData, const dmJoint* pBase) {
    dm_int32 islandIndexB = pBase->m_islandIndexB;
    dm_vec4 v = pData->aVelocities[islandIndexB].v;
    dm_vec4 w = pData->aVelocities[islandIndexB].w;
    dm_vec4 dv = v + dmMul33(m_J, w);
    dm_vec4 impulse = dmMul33(m_mass, m_bias - dv - m_gamma * m_impulse);
    dm_vec4 oldImpulse = m_impulse;
    m_impulse += impulse;
    dm_vec4 maxImpulse = pData->step.dt * m_maxForce;
    if (dmGT(dmLengthSqr3(m_impulse), maxImpulse * maxImpulse)) {
        m_impulse = maxImpulse * dmNormalize3(m_impulse);
    }
    impulse = m_impulse - oldImpulse;
    v += pBase->m_invMassB * impulse;
    w += dmMul33(m_B, impulse);
    pData->aVelocities[islandIndexB].v = v;
    pData->aVelocities[islandIndexB].w = w;
}
