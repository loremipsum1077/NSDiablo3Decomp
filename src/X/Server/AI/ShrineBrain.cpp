// Original file: X/Server/Ai/ShrineBrain.cpp
#include "../Actors/Actors.h"
#include "./ActorAI.h"
#include "./BrainComponents.h"
#include "../SGame.h"
#include "../Powers/Powers.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Gizmo.h"
#include "../../Common/Hireling.h"
#include "../../Common/HirelingSwig.h"
#include "../../Common/Target.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/Random.h"

enum { ACTOR_SPHERE_TREE_FLAGS_ALL = 7 };  // X/Common/Actors/ActorCollision.h
void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // ActorCollision.cpp:317
BOOL PowerActorCanResurrect(ActorID idActor);

//@ 33
static const WorldDistance cgwdResurrectionRadius = 100.0f;

//@ 38
static void sShrineResurrect(ActorID idActor, Target tTarget)
{


    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);

    SNO snoPower = tACD.AttributesGetPower(0);
    if (snoPower != SNO_NONE)
    {
        PowerStart(idACD, snoPower, tTarget);
    }
}

//@ 70
static BOOL sResurrectionShrineBrainDoChanceToStartResurrecting(ActorID idActor)
{


    float flChanceToCast = 0.5f;

    float flRandom = RandFloat_Internal(SGameGlobalsGet().rng);
    if (flRandom < flChanceToCast)
    {

        XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
        ACDID idACD = ACDFindByActor(idActor);
        ActorCommonData & tACD = ACDGet(idACD);
        WorldPlace tPlace = tACD.GetPlace();
        ACDsIntersectSphere(tPlace, cgwdResurrectionRadius, idACD, listACDs, ACTOR_SPHERE_TREE_FLAGS_ALL);
        POS pos = listACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACDCorpse = (ACDID)listACDs.GetNext(pos);

            ActorCommonData & tACDCorpse = ACDGet(idACDCorpse);
            if (tACDCorpse.IsAlive())
            {
                continue;
            }



            if (HirelingGetClass(tACDCorpse) != HC_NONE)
            {
                continue;
            }














            if (PowerActorCanResurrect((ActorID)ACDGetOwner(idACDCorpse)))
            {
                Target tTarget;
                tTarget.InitializeFromACD(idACDCorpse, FALSE, FALSE);
                sShrineResurrect(idActor, tTarget);
                return TRUE;
            }
        }
    }
    return FALSE;
}

//@ 130
void ResurrectionShrineBrainUpdate(ActorID idActor)
{



    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    int32 nGizmoState = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_GIZMO_STATE));
    if (nGizmoState == 1)
    {
        sResurrectionShrineBrainDoChanceToStartResurrecting(idActor);
    }
}
