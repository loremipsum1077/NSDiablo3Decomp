// X/Server/AI/MirrorImageBrain.cpp. Reconstructed from DWARF and original line evidence.
#include "ActorAI.h"
#include "BrainComponents.h"
#include "../Actors/Actors.h"
#include "../Actors/ActorPowers.h"
#include "../SGame.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"

// Original line 28, 0x71007a4890.
void MirrorImageBrainActionUpdate(ActorID idActor)
{
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    ACDID idACDOwner = (ACDID)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMONED_BY_ACDID));
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (!ptACDOwner || !tACD.IsAlive()) {
        BrainAPIDoDeath(idActor, ACTORID_INVALID, 1, 1, 1, 0, 0);
        return;
    }

    if (ActorPowersGetPrimaryPower((ActorID)ptACDOwner->idOwner) == GlobalSNOGet(GLOBALSNO_POWERS_WIZARD_MIRROR_IMAGE))
        return;

    // Original relocated table at 0x7101097e08, three pointers in this order.
    const char * szAvailableAttacks[3] = {
#include "../../Common/generated-game-data/mirror_attacks.inc"
    };
    SNO snoAttackToUse = SNOFileLookupGUID(POWER_SNO, szAvailableAttacks[RandRange_Internal(SGameGlobalsGet().rng, 0, 2)]);
    ActorID idActorTarget = BrainAPIActorChooseTarget(idActor, snoAttackToUse, 0, 0);
    if (idActorTarget != ACTORID_INVALID) {
        BrainAttackSettings tAttackSettings;
        BrainAPIGetAttackSettings(idActor, tAttackSettings);
        tAttackSettings.snoPower = snoAttackToUse;
        tAttackSettings.tTarget.InitializeFromACD(ACDFindByActor(idActorTarget), 0, 0);
        BrainAPIDoAttack(idActor, tAttackSettings);
    } else {
        WorldPlace placeActor = tACD.GetPlace();
        WorldPlace placeLeader = ptACDOwner->GetPlace();
        if (placeLeader.idSWorld != placeActor.idSWorld) {
            BrainAPIDoDeath(idActor, ACTORID_INVALID, 1, 1, 1, 0, 0);
        } else {
            WorldDistance wdToLeader = WorldPlaceDistance(placeActor, placeLeader);
            if (wdToLeader > 30.0f) {
                BrainAPIDoWalk(idActor, placeLeader, 0.0f, TRANSLATE_TYPE_WALKING, TAG_ACTOR_ANIMATION_RUN);
            } else if (wdToLeader <= 15.0f) {
                if (wdToLeader > EPSILON) {
                    Vector3D vToDest = placeActor.wp - placeLeader.wp;
                    vToDest.Normalize();
                    vToDest *= 22.5f; // Original float bytes 0000b441 at 0x7100e3c328.
                    WorldPlace placeTarget = placeLeader;
                    placeTarget.wp += vToDest;
                    BrainAPIDoWalk(idActor, placeTarget, 0.0f, TRANSLATE_TYPE_WALKING, TAG_ACTOR_ANIMATION_RUN);
                }
            } else {
                WorldPlace placeWander = placeLeader;
                BrainWanderSettings tSettings;
                BrainAPIGetWanderSettings(idActor, tSettings);
                tSettings.dwTagAnimationWalking = TAG_ACTOR_ANIMATION_WALK;
                tSettings.dwTagAnimationIdling = TAG_ACTOR_ANIMATION_NEUTRAL;
                tSettings.bResetWanderCenter = 1;
                tSettings.placeWander = placeWander;
                tSettings.dwTimerMin = 120;
                tSettings.dwTimerDelta = 240;
                BrainAPIDoWander(idActor, tSettings);
                BrainSetStatus(idActor, "Wander");
            }
        }
    }
}

// Original line 141, 0x71007a4c70. The sibling's matching 1.0 body agrees.
void MirrorImageBrainDeadUpdate(const ActorID idActor)
{
    BrainAPIDoDeath(idActor, ACTORID_INVALID, 1, 1, 1, 0, 0);
}
