#include "CGame.h"
#include "RActors/RActors.h"
#include "Particle.h"
#include "Rope.h"
#include "RMessage.h"
#include "Physics/DominoClient.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/JobManager/JobManager.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include <new>
// DWARF RActors.cpp:5118, 5251, 5302, 5532, 5716.
void RActorPreThreadSafeUpdate(RActor * ptRActor, ActorCommonData * ptACD, float flDeltaTime, const ActorDefinition * ptDefActor);
void RActorThreadSafeUpdate(RActor * ptRActor, Vector3D & wpActor, ActorCommonData * ptACD, float flDeltaTime, const ActorDefinition * ptDefActor);
void RActorPostThreadSafeUpdate(RActor * ptRActor, Vector3D & wpActor, ActorCommonData * ptACD, float flDeltaTime, const ActorDefinition * ptDefActor);
void RActorUpdate(RActor * ptRActor, float flDeltaTime);
void RActorsRegisterParentObjects(BOOL fFixedUpdateOnly, float flDeltaTime);
// DWARF Particle.cpp:2310, Rope.cpp:1092,1119.
void ParticlesRegisterParentObjects(BOOL fFixedUpdateOnly);
void RopeRegisterParentObjects(BOOL fFixedUpdateOnly);
void RopeUpdate(Rope * ptRope, float flDeltaTime);


























struct ParentObjectUpdateRecord
{
    ParentType eType;
    DataID idObject;
    ACDID idACD;
    ActorCommonData * ptACD;
    union
    {
        RActor * ptRActor;
        ParticleSystem * ptParticleSystem;
        Rope * ptRope;
        void * pObject;
    };

    Vector3D wpActor;
    BOOL fLocked;
    const ActorDefinition * pdefActor;
};








typedef blz::vector<ParentObjectUpdateRecord> ParentObjectUpdateList;

enum { MAX_ALLOWED_DEPTH = 8 };















struct ParentObjectGlobals
{
    int nMaxDepth;
    ParentObjectUpdateList tUpdateLists[MAX_ALLOWED_DEPTH];
};









void ParentObjectsInitialize()

{
    ParentObjectGlobals *& ptParentObjectGlobals = CGameGet().ptParentObjectGlobals;
    ptParentObjectGlobals = (ParentObjectGlobals *)SigmaMemoryNew(sizeof(ParentObjectGlobals), 4, AppGetCurrentMemoryInterface(), TRUE);
    for (int32 i = 0; i < MAX_ALLOWED_DEPTH; ++i)
    {
        new (&ptParentObjectGlobals->tUpdateLists[i]) ParentObjectUpdateList;
    }
}








void ParentObjectsDispose()

{
    ParentObjectGlobals *& ptParentObjectGlobals = CGameGet().ptParentObjectGlobals;
    for (int32 i = 0; i < MAX_ALLOWED_DEPTH; ++i)
    {
        ptParentObjectGlobals->tUpdateLists[i].~ParentObjectUpdateList();
    }
    SigmaMemoryFree(ptParentObjectGlobals, AppGetCurrentMemoryInterface());
    ptParentObjectGlobals = NULL;
}








static void sUpdateParentRecord(
    ParentObjectUpdateRecord & tRecord,
    float flDeltaTime)
{
    switch (tRecord.eType)
    {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP:
        tRecord.ptRActor = RActorTryToGet((RActorID)tRecord.idObject);
        if (tRecord.ptRActor)
            RActorUpdate(tRecord.ptRActor, flDeltaTime);
        break;
    case PARENT_PARTICLE_SYSTEM:
    {







        ParticleSystemID idParticleSystem = tRecord.idObject;
        if (tRecord.ptParticleSystem && tRecord.ptParticleSystem->id == idParticleSystem)
            tRecord.ptParticleSystem->Update(flDeltaTime);
        break;
    }
    case PARENT_ROPE:
    {



        RopeID idRope = tRecord.idObject;
        if (tRecord.ptRope && tRecord.ptRope->id == idRope)
            RopeUpdate(tRecord.ptRope, flDeltaTime);
        break;
    }
    }
}







static void sPrepareRecordInit(
    ParentObjectUpdateRecord & tRecord,
    BOOL fFirst)
{
    switch (tRecord.eType)
    {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP:
        if (tRecord.ptRActor && tRecord.ptRActor->id == tRecord.idObject)
        {

            tRecord.ptACD = ACDTryToGet(tRecord.idACD);
            static const ActorDefinition * spdefActor = NULL;
            static SNO ssnoActor = SNO_NONE;
            if (fFirst || ssnoActor != tRecord.ptRActor->snoActor)
            {
                spdefActor = DefinitionLockR<ActorDefinition>(tRecord.ptRActor->snoActor);
                ssnoActor = tRecord.ptRActor->snoActor;
                tRecord.fLocked = TRUE;
            }
            else
                tRecord.fLocked = FALSE;
            tRecord.pdefActor = spdefActor;
        }
        break;
    }
}







static void sPreThreadSafeUpdateParentRecord(
    ParentObjectUpdateRecord & tRecord,
    float flDeltaTime)
{
    switch (tRecord.eType)
    {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP:
        if (tRecord.ptRActor && tRecord.ptRActor->id == tRecord.idObject)
            RActorPreThreadSafeUpdate(tRecord.ptRActor, tRecord.ptACD, flDeltaTime, tRecord.pdefActor);
        break;
    case PARENT_PARTICLE_SYSTEM:
        if (tRecord.ptParticleSystem && tRecord.ptParticleSystem->id == tRecord.idObject)
        {
            if (!tRecord.ptParticleSystem->PreThreadSafeUpdate(flDeltaTime))
                tRecord.ptParticleSystem = NULL;
        }
        break;
    case PARENT_ROPE:
        if (tRecord.ptRope && tRecord.ptRope->id == tRecord.idObject)
            RopeUpdate(tRecord.ptRope, flDeltaTime);
        break;
    }
}

























static void sThreadSafeUpdateParentRecord(
    ParentObjectUpdateRecord & tRecord,
    float flDeltaTime)
{
    switch (tRecord.eType)
    {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP:
        if (tRecord.ptRActor && tRecord.ptRActor->id == tRecord.idObject)
            RActorThreadSafeUpdate(tRecord.ptRActor, tRecord.wpActor, tRecord.ptACD, flDeltaTime, tRecord.pdefActor);
        break;
    case PARENT_PARTICLE_SYSTEM:
        if (tRecord.ptParticleSystem && tRecord.ptParticleSystem->id == tRecord.idObject)
            tRecord.ptParticleSystem->ThreadSafeUpdate(flDeltaTime);
        break;
    }
}





















static void sPostThreadSafeUpdateParentRecord(
    ParentObjectUpdateRecord & tRecord,
    float flDeltaTime)
{
    switch (tRecord.eType)
    {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP:
        if (tRecord.ptRActor && tRecord.ptRActor->id == tRecord.idObject)
            RActorPostThreadSafeUpdate(tRecord.ptRActor, tRecord.wpActor, tRecord.ptACD, flDeltaTime, tRecord.pdefActor);
        if (tRecord.pdefActor && tRecord.fLocked)
            gptGroupDefActor->UnlockR((void *&)tRecord.pdefActor);
        break;
    case PARENT_PARTICLE_SYSTEM:
        if (tRecord.ptParticleSystem && tRecord.ptParticleSystem->id == tRecord.idObject)
            tRecord.ptParticleSystem->PostThreadSafeUpdate(flDeltaTime);
        break;
    }
}
























class ParentUpdateJob
{
public:
    static float sflDeltaTime;

    ParentUpdateJob(
        ParentObjectUpdateList * ptList,
        int32 nStartingIndex,
        int32 nCount)
        : m_ptList(ptList), m_nStartingIndex(nStartingIndex), m_nCount(nCount)
    {
    }




    void Execute()
    {
        GameSetActiveGameCommonData(&CGameGet().tGCD, TRUE);

        int32 nCount = m_nCount;
        int32 nStartingIndex = m_nStartingIndex;










        float flDeltaTime = sflDeltaTime;

        for (int32 i = 0; i < nCount; ++i)
        {




            ParentObjectUpdateRecord & tRecord = (*m_ptList)[nStartingIndex + i];
            sThreadSafeUpdateParentRecord(tRecord, flDeltaTime);
        }
    }

    static void JobFcn(void * pParam)
    {
        ParentUpdateJob * pThis = (ParentUpdateJob *)pParam;
        pThis->Execute();
    }

private:
    ParentObjectUpdateList * m_ptList;
    int32 m_nStartingIndex;
    int32 m_nCount;
};
float ParentUpdateJob::sflDeltaTime;















































































































































void ParentObjectsUpdate(
    float flDeltaTime)
{


    ParentObjectGlobals * ptParentObjectGlobals = CGameGet().ptParentObjectGlobals;










    for (int32 i = 0; i <= ptParentObjectGlobals->nMaxDepth; ++i)
        ptParentObjectGlobals->tUpdateLists[i].clear();
    ptParentObjectGlobals->nMaxDepth = 0;
    RActorsRegisterParentObjects(FALSE, flDeltaTime);
    ParticlesRegisterParentObjects(FALSE);
    RopeRegisterParentObjects(FALSE);






    for (int32 nList = 0; nList <= ptParentObjectGlobals->nMaxDepth; ++nList)
    {
        ParentObjectUpdateList & tList = ptParentObjectGlobals->tUpdateLists[nList];

        int32 nCount = tList.size();
        if (!nCount)
            break;
        ParentUpdateJob::sflDeltaTime = flDeltaTime;





        char arJobBuffer[2048];

        int32 nIncrement = nCount / 128 + 1;
        int32 nJobIndex = 0;
        sPrepareRecordInit(tList[0], TRUE);
        if (nCount > 1)
            sPrepareRecordInit(tList[1], FALSE);










        for (int32 i = 0; i < nCount; ++i)
        {
            if (i + 2 < nCount)
                sPrepareRecordInit(tList[i + 2], FALSE);



            ParentObjectUpdateRecord & tRecord = tList[i];
            sPreThreadSafeUpdateParentRecord(tRecord, flDeltaTime);
        }
        gptPhysicsAPIImpl->Lock();









        for (int32 i = 0; i < nCount; i += nIncrement)
        {
            void * pBuffer = &arJobBuffer[nJobIndex++ * sizeof(ParentUpdateJob)];
            ParentUpdateJob * ptJob = new (pBuffer) ParentUpdateJob(&tList, i, Math::Min(nIncrement, nCount - i));
            JobManager::Get()->PushJob(ParentUpdateJob::JobFcn, ptJob);
        }




        JobManager::Get()->WaitForIdle();



        gptPhysicsAPIImpl->Unlock();










        for (int32 i = 0; i < nCount; ++i)
        {




            ParentObjectUpdateRecord & tRecord = tList[i];
            sPostThreadSafeUpdateParentRecord(tRecord, flDeltaTime);
        }
    }
}
















void ParentObjectsFixedUpdate(
    float flDeltaTime)
{


    ParentObjectGlobals * ptParentObjectGlobals = CGameGet().ptParentObjectGlobals;





    for (int32 i = 0; i <= ptParentObjectGlobals->nMaxDepth; ++i)
        ptParentObjectGlobals->tUpdateLists[i].clear();
    ptParentObjectGlobals->nMaxDepth = 0;
    RActorsRegisterParentObjects(TRUE, flDeltaTime);
    ParticlesRegisterParentObjects(TRUE);
    RopeRegisterParentObjects(TRUE);






    for (int32 nList = 0; nList <= ptParentObjectGlobals->nMaxDepth; ++nList)
    {
        ParentObjectUpdateList & tList = ptParentObjectGlobals->tUpdateLists[nList];

        uint nCount = tList.size();

        for (uint32 i = 0; i < nCount; ++i)
        {
            ParentObjectUpdateRecord & tRecord = tList[i];
            sUpdateParentRecord(tRecord, flDeltaTime);
        }
    }
}




void ParentObjectRegister(
    int nDepth,
    ParentType eType,
    DataID idObject,
    void * pObject,
    ACDID idACD)
{
    ParentObjectGlobals * ptParentObjectGlobals = CGameGet().ptParentObjectGlobals;


    const CGame & tClientData = CGameGet();

    if (!tClientData.bLastClientUpdate)
        nDepth = 0;
















    ptParentObjectGlobals->nMaxDepth = Math::Max(nDepth, ptParentObjectGlobals->nMaxDepth);

    ParentObjectUpdateRecord tRecord;
    tRecord.eType = eType;
    tRecord.idObject = idObject;
    tRecord.pObject = pObject;
    tRecord.idACD = idACD;
    tRecord.ptACD = NULL;

    tRecord.pdefActor = NULL;
    tRecord.wpActor.Set(0.0f, 0.0f, 0.0f);








    ptParentObjectGlobals->tUpdateLists[nDepth].push_back(tRecord);
}




