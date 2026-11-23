// Original file: X/Server/Actors/ActorEffect.cpp
#include "ActorEffect.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/CosmeticItems.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../../Common/VisualInventory.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../Network/KnownMessage.h"
#include "../PlayerList.h"


















static const DamageType stElementalDamageTypes[] =
{
    DAMAGE_TYPE_PHYSICAL,
    DAMAGE_TYPE_FIRE,
    DAMAGE_TYPE_LIGHTNING,
    DAMAGE_TYPE_COLD,
    DAMAGE_TYPE_POISON,
    DAMAGE_TYPE_ARCANE,
    DAMAGE_TYPE_HOLY,
};
static int32 snElementalDamageTableSize = sizeof(stElementalDamageTypes) / sizeof(stElementalDamageTypes[0]);

DamageType GetPredominantDamageType(const float * flDamageDone, BOOL bExcludePhysical)

{

    if (!flDamageDone)
    {
        return DAMAGE_TYPE_NONE;
    }

    DamageType eWinningDamageType = DAMAGE_TYPE_PHYSICAL;
    float flWinningDamage = EPSILON;

    for (int32 i = bExcludePhysical ? 1 : 0; i < snElementalDamageTableSize; i++)
    {
        if (flDamageDone[stElementalDamageTypes[i]] > flWinningDamage)
        {
            flWinningDamage = flDamageDone[stElementalDamageTypes[i]];
            eWinningDamageType = stElementalDamageTypes[i];
        }
    }



    return eWinningDamageType;
}


void ACDPlayHitEffect(const ACDID idACD, const ACDID idACDAttacker, const DamageType eDamageType, const BOOL fCrit)

{

    PlayHitEffectMessage tMessage;
    ActorCommonData & tACD = ACDGet(idACD);

    tMessage.ann = ANNFromACD(tACD.id);
    tMessage.annAttacker = ANNFromACD(ACDGetControllingActor(idACDAttacker));
    tMessage.eDamageType = eDamageType;
    tMessage.fCrit = fCrit;
    KnownMessageAdd(tACD, SMSG_PLAY_HIT_EFFECT, &tMessage, sizeof(tMessage), NULL);
}





void ACDPlayHitEffectOverride(const ACDID idACD, const ACDID idACDAttacker, const SNO snoEffectGroup)

{

    PlayHitEffectOverrideMessage tMessage;
    ActorCommonData & tACD = ACDGet(idACD);

    tMessage.ann = ANNFromACD(tACD.id);
    tMessage.annAttacker = ANNFromACD(ACDGetControllingActor(idACDAttacker));
    tMessage.snoEffectGroup = snoEffectGroup;
    KnownMessageAdd(tACD, SMSG_PLAY_HIT_EFFECT_OVERRIDE, &tMessage, sizeof(tMessage), NULL);
}



void ACDPlayEffect(const ACDID idACD, const EffectType eEffectType, const int32 nEffectParameter, const ACDID idACDOwner)

{



    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        PlayEffectMessage tMessage;
        tMessage.ann = ANNFromACD(idACD);
        tMessage.eEffectType = eEffectType;

        if (nEffectParameter != 0)
        {
            tMessage.nEffectParameter.Set(nEffectParameter);
        }

        ACDID idACDPlayer = ACDGetControllingActor(idACDOwner);
        if (ACDIsPlayer(idACDPlayer))
        {
            tMessage.annPlayer.Set(ANNFromACD(idACDPlayer));
        }

        KnownMessageAdd(*ptACD, SMSG_PLAY_EFFECT, &tMessage, sizeof(tMessage), NULL);


        ptACD->dwFlags &= ~(1 << ACD_EDIT_MODE_ONLY_BIT);
    }
}



void ACDPlayEffectForPlayer(const ACDID idACDPlayer, const ACDID idACD, const EffectType eEffectType, const int32 nEffectParameter)

{

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);
    ACDPlayEffectForPlayers(listDestPlayers, idACD, eEffectType, nEffectParameter);
}





void ACDPlayEffectForPlayers(const PlayerList & listDestPlayers, const ACDID idACD, const EffectType eEffectType, const int32 nEffectParameter)

{



    ActorCommonData & tACD = ACDGet(idACD);
    PlayEffectMessage tMessage;

    tMessage.ann = ANNFromACD(tACD.id);

    tMessage.eEffectType = eEffectType;
    if (nEffectParameter != 0)
    {
        tMessage.nEffectParameter.Set(nEffectParameter);
    }

    ACDID idACDPlayer = ACDGetControllingActor(idACD);
    if (ACDIsPlayer(idACDPlayer))
    {
        tMessage.annPlayer.Set(ANNFromACD(idACDPlayer));
    }

    KnownMessageAdd(tACD, SMSG_PLAY_EFFECT, &tMessage, sizeof(tMessage), &listDestPlayers);
}



























void ACDPlayEffectChangeActor(const ACDID idACD, const SNO snoActor)

{

    ActorCommonData & tACD = ACDGet(idACD);
    ACDChangeActorMessage tMessage;

    tMessage.ann = ANNFromACD(tACD.id);
    tMessage.snoActor = snoActor;

    KnownMessageAdd(tACD, SMSG_CHANGE_ACTOR, &tMessage, sizeof(tMessage), NULL);
}



void ACDPlayEffectGroupACDToACD(const SNO snoEffectGroup, const ACDID idACDFrom, const ACDID idACDTo)

{



    if (snoEffectGroup == SNO_NONE)
    {
        return;
    }




    ActorCommonData & tACDFrom = ACDGet(idACDFrom);
    EffectGroupACDToACDMessage tMessage;
    tMessage.snoEffectGroup = snoEffectGroup;
    tMessage.annFrom = ANNFromACD(tACDFrom.id);
    tMessage.annTo = ANNFromACD(idACDTo);


    KnownMessageAdd(tACDFrom, SMSG_EFFECT_GROUP_ACD_TO_ACD, &tMessage, sizeof(tMessage), NULL);
}


void ACDPlayEffectRopeACDToACD(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom, const ACDID idACDTo, const RopePosition ePositionTo, const BOOL bIgnoreOwnerAlpha)

{



    ActorCommonData & tACDFrom = ACDGet(idACDFrom);
    RopeEffectMessageACDToACD tMessage;
    tMessage.snoRope = snoRope;
    tMessage.annFrom = ANNFromACD(tACDFrom.id);
    tMessage.ePositionFrom = ePositionFrom;
    tMessage.annTo = ANNFromACD(idACDTo);
    tMessage.ePositionTo = ePositionTo;
    tMessage.bIgnoreOwnerAlpha = bIgnoreOwnerAlpha;


    KnownMessageAdd(tACDFrom, SMSG_ROPE_EFFECT_ACD_TO_ACD, &tMessage, sizeof(tMessage), NULL);
}





void ACDPlayEffectRopeACDToPlace(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom, const WorldPlace & tPlaceTo, const BOOL bIgnoreOwnerAlpha)

{





    ActorCommonData & tACDFrom = ACDGet(idACDFrom);



    if (tACDFrom.GetPlace().idSWorld != tPlaceTo.idSWorld)
    {
        return;
    }

    RopeEffectMessageACDToPlace tMessage;
    tMessage.snoRope = snoRope;
    tMessage.annFrom = ANNFromACD(tACDFrom.id);
    tMessage.ePositionFrom = ePositionFrom;
    tMessage.tPlaceTo = tPlaceTo;
    tMessage.bIgnoreOwnerAlpha = bIgnoreOwnerAlpha;



    KnownMessageAdd(tACDFrom, SMSG_ROPE_EFFECT_ACD_TO_PLACE, &tMessage, sizeof(tMessage), NULL);
}
void ACDActivateCosmeticEffect(ACDID idACD, GBID gbidItem, CosmeticItemType eType)

{

    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {

        VisualCosmeticItemSlot eSlot;
        switch (eType)
        {
            case COSMETIC_ITEM_WINGS:
                eSlot = VCIS_WINGS;
                break;
            case COSMETIC_ITEM_PENNANT:
                eSlot = VCIS_PENNANT;
                break;
            default:
                eSlot = VCIS_INVALID;
                break;
        }

        if (eSlot != VCIS_INVALID)
        {
            VisualInventorySetCosmeticItemInSlot(idACD, eSlot, gbidItem);
        }
    }
}
