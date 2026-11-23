// Original file: X/Client/EffectGroup.cpp
// Functions sit on their original line numbers (2.6.2 DWARF).
// EffectGroupAttachmentAdd (1152) inlines every static pick helper (lines 322-1150) except
// sEffectGroupCreate (146) and sEffectGroupPickOne (244); the helpers are written from its line
// table and the local decl lines in the CU dump.
#include "EffectGroup.h"
#include "CGame.h"
#include "RActors/RActors.h"
#include "RActors/RActorACDEffects.h"
#include "RActors/RActorAttachments.h"
#include "../Common/Definitions/AttachmentDefinition.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/SNO/SNOName.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "Gfx/GfxVB.h"

struct EffectGroupGlobals {  // size 0x8
    TDataArray<EffectGroup, EffectGroupID> * ptClientTrackedEffectGroups;  // 0x00
};

// The same macro X/Common/Definitions/AttachmentDefinition.cpp uses (name invented there): the
// DWARF records the hash and its string on one line.
#define DEFINE_HARDPOINT_NAME(name, str) \
    const CHAR * name##STR = str;        \
    const HardpointHash name(str)
DECLARE_HARDPOINT_NAME(HARDPOINT_DONT_OVERRIDE);
DECLARE_HARDPOINT_NAME(HARDPOINT_CLOSEST_DAMAGE);
#include "CPlayer.h"
#include "Particle.h"
#include "Rope.h"
#include "Definitions/AppearanceDefinition.h"
#include "Definitions/EffectGroupDefinition.h"
#include "Battlenet/UI/Util/UIUtil.h"
#include "../Common/Player.h"
#include "../Common/TeamCommon.h"
#include "../Common/ItemsSwig.h"
#include "../Common/ActorCommonDataSwig.h"
#include "../Common/GameTemplate.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Common/TagMaps/ActorTags.h"
#include "../Common/Attributes/FastAttribute.h"
HardwareClass GfxGetHWClass();  // X/Client/Gfx/Gfx.cpp:3922; no adopted header declares it yet
TeamColor TeamGetEffectGroupIndex(Team eTeam);  // X/Common/TeamCommon.cpp:309; not declared in TeamCommon.h yet
static const int32 MAX_EFFECT_GROUPS = 256;




DEFINE_HARDPOINT_NAME(HARDPOINT_DONT_OVERRIDE, "Don't Override");
DEFINE_HARDPOINT_NAME(HARDPOINT_CLOSEST_DAMAGE, "Closest Damage HP");















static EffectGroupGlobals & sEffectGroupGlobalsGet()
{





    CGame & tCGame = CGameGet();

    return *tCGame.ptEffectGroupGlobals;
}




EffectGroup & EffectGroupGet(const EffectGroupID idEffectGroup)
{

    EffectGroupGlobals & tEffectGroupGlobals = sEffectGroupGlobalsGet();
    return *tEffectGroupGlobals.ptClientTrackedEffectGroups->ElementGet(idEffectGroup);
}


EffectGroup * EffectGroupTryToGet(const EffectGroupID idEffectGroup)
{

    EffectGroupGlobals & tEffectGroupGlobals = sEffectGroupGlobalsGet();
    return tEffectGroupGlobals.ptClientTrackedEffectGroups->ElementTryToGet(idEffectGroup);
}



void EffectGroupInitializeForGame()

{
    CGame & tCGame = CGameGet();


    EffectGroupGlobals *& ptEffectGroupGlobals = tCGame.ptEffectGroupGlobals;

    ptEffectGroupGlobals = (EffectGroupGlobals *)SigmaMemoryNew(sizeof(EffectGroupGlobals), 4, AppGetCurrentMemoryInterface(), TRUE);


    ptEffectGroupGlobals->ptClientTrackedEffectGroups =
        TDataArrayCreateTracked<EffectGroup, EffectGroupID>("EffectGroup", AppGetCurrentMemoryInterface(), MAX_EFFECT_GROUPS);





}



void EffectGroupDisposeForGame()


{
    CGame & tCGame = CGameGet();


    TDataArrayIterator<EffectGroup> tItEffectGroup;
    while (tCGame.ptEffectGroupGlobals->ptClientTrackedEffectGroups->Iterate(tItEffectGroup))
    {
        EffectGroup & tEffectGroup = *tItEffectGroup.ptElement;
        EffectGroupDelete(tEffectGroup.id);
    }


    TDataArrayDispose(tCGame.ptEffectGroupGlobals->ptClientTrackedEffectGroups);


    SigmaMemoryFree(tCGame.ptEffectGroupGlobals, AppGetCurrentMemoryInterface()); tCGame.ptEffectGroupGlobals = NULL;

}



static AttachmentID sEffectGroupCreate(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType, const int32 nEffectIndex)






{
    if (nEffectIndex < 0 || nEffectIndex >= ptEffectGroupDef->nEffectCount)
    {
        return ATTACHMENTID_INVALID;
    }



    const DataID idAttachParentPrimary = idAttachParents[0];
    const EffectItem & tEffectItem = ptEffectGroupDef->ptEffects[nEffectIndex];

    const TriggerEvent & tEffectItemEvent = tEffectItem.tMsgTriggeredEvents.tTriggerEvent;

    if ((tEffectItemEvent.dwTriggerFlags & (1 << TRIGGERFLAG_DISABLED_IF_REMOTE_PLAYER))
        &&
        (tEffectEvent.dwTriggerFlags & (1 << TRIGGERFLAG_DISABLED_IF_REMOTE_PLAYER)))
    {
        return ATTACHMENTID_INVALID;
    }



    TriggerEvent tEvent;
    SigmaMemoryCopy(&tEvent, &tEffectItemEvent, sizeof(TriggerEvent));

    if (!(ptEffectGroupDef->dwEffectFlags & 2))
    {



        tEvent.dwID = tEffectEvent.dwID;
    }

    for (int32 i = 0; i < 2; i++)
    {
        HardpointLink & tHardpointLink = tEvent.tHardpointLinks[i];



        if (tHardpointLink.tHash == HARDPOINT_DONT_OVERRIDE)
        {
            tHardpointLink.tHash = tEffectEvent.tHardpointLinks[i].tHash;
        }
    }

    AttachmentID idAttachment = ATTACHMENTID_INVALID;
    switch (eParentType)
    {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP:
        {
            RActor * ptRActorPrimary = RActorGet((RActorID)idAttachParentPrimary);
            idAttachment = RActorSendRMessage(*ptRActorPrimary, tEvent, 0.0f, (const RActorID *)idAttachParents, NULL, 0, -1.0f, FALSE, 0.0f, eParentType);
        }
        break;
    case PARENT_PARTICLE_SYSTEM:
    case PARENT_PARTICLE:
        {
            ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(idOwner);
            if (ptParticleSystem)
            {
                BOOL bRemote = (tEffectEvent.dwTriggerFlags & (1 << TRIGGERFLAG_DISABLED_IF_REMOTE_PLAYER)) != 0;
                ptParticleSystem->tRMsgList.SendMessage(eParentType, idAttachParentPrimary, FALSE, tEvent, 0.0f, bRemote);
            }
        }
        break;
    case PARENT_ROPE:
        {


            Rope * ptRope = RopeTryToGet(idAttachParentPrimary);
            if (ptRope)
            {
                BOOL bRemote = (tEffectEvent.dwTriggerFlags & (1 << TRIGGERFLAG_DISABLED_IF_REMOTE_PLAYER)) != 0;
                ptRope->tRMsgList.SendMessage(PARENT_ROPE, idAttachParentPrimary, FALSE, tEvent, 0.0f, bRemote);
            }
        }
        break;
    default:


        DisplayInternalError(NULL, "", 234, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }

    return idAttachment;
}




static AttachmentID sEffectGroupPickOne(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)





{
    if (ptEffectGroupDef->nEffectCount == 0)
    {
        return ATTACHMENTID_INVALID;
    }

    uint32 dwTotalWeight = 0;
    int32 nViablePermCount = 0;
    int32 nViablePermList[64];

    int32 i;
    for (i = 0; i < ptEffectGroupDef->nEffectCount; i++)
    {
        if ((ptEffectGroupDef->dwEffectFlags & 1) &&
            (ptEffectGroupDef->fPlayedBit[i >> 5] & (1 << (i & 31))))
        {
            continue;
        }

        EffectItem & tEffect = ptEffectGroupDef->ptEffects[i];
        dwTotalWeight += tEffect.dwWeight;

        nViablePermList[nViablePermCount++] = i;
    }

    if (!dwTotalWeight)
    {
        dwTotalWeight = 1;
    }

    uint32 dwPickWeight = RandRange_Internal(CGameGlobalsGet().rngLocal, 0, dwTotalWeight - 1);

    uint32 dwCurWeight = 0;
    int32 nChoice = 0;
    for (i = 0; i < nViablePermCount; i++)
    {
        int32 nIndex = nViablePermList[i];

        EffectItem & tEffect = ptEffectGroupDef->ptEffects[nIndex];
        dwCurWeight += tEffect.dwWeight;




        if (dwPickWeight < dwCurWeight) { nChoice = nIndex; break; }
    }


    if (ptEffectGroupDef->dwEffectFlags & 1)
    {
        if (nViablePermCount == 1)
        {
            SigmaMemoryClear((void *)ptEffectGroupDef->fPlayedBit, sizeof(ptEffectGroupDef->fPlayedBit) / sizeof(ptEffectGroupDef->fPlayedBit[0]));
        }
        else
        {
            ((EffectGroupDefinition *)ptEffectGroupDef)->fPlayedBit[nChoice >> 5] |= (1 << (nChoice & 31));
        }
    }

    return sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nChoice);






}




static AttachmentID sEffectGroupPickSome(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)





{
    int32 nDelta = Math::Max(ptEffectGroupDef->nMaxToPick - ptEffectGroupDef->nMinToPick, 0);

    int32 nCount = RandRange_Internal(CGameGlobalsGet().rngLocal, ptEffectGroupDef->nMinToPick, nDelta);

    AttachmentID idAttachment = ATTACHMENTID_INVALID;
    for (int32 i = 0; i < nCount; i++)
    {
        AttachmentID idLastAttachment = sEffectGroupPickOne(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);






        if (idLastAttachment != ATTACHMENTID_INVALID)
        {
            idAttachment = idLastAttachment;
        }
    }

    return idAttachment;
}





static AttachmentID sEffectGroupPickAll(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)





{
    AttachmentID idAttachment = ATTACHMENTID_INVALID;

    for (int32 i = 0; i < ptEffectGroupDef->nEffectCount; i++)
    {
        AttachmentID idLastAttachment = sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, i);







        if (idLastAttachment != ATTACHMENTID_INVALID)
        {
            idAttachment = idLastAttachment;
        }
    }

    return idAttachment;
}


























static AttachmentID sEffectGroupPickByFriendlyStatus(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)





{
    if (!RActorTryToGet((RActorID)idOwner))
    {
        return ATTACHMENTID_INVALID;
    }

    RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)idOwner, TRUE, FALSE);

    const RActor * ptTopmost = RActorGet(idRActorTopmost);

    if (ptTopmost->idACD == ACDID_INVALID)
    {
        return ATTACHMENTID_INVALID;
    }

    ActorCommonData & tACDTopmost = ACDGet(ptTopmost->idACD);
    Team eTeam = tACDTopmost.GetTeam();

    Team eLocalTeam = eTeam;

    ACDID idACDLocalPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDLocalPlayer = ACDTryToGet(idACDLocalPlayer);

    if (ptACDLocalPlayer)
    {
        eLocalTeam = ptACDLocalPlayer->GetTeam();
    }

    int32 nIndex = TeamsAreFriendly(eTeam, eLocalTeam) ? 0 : 1;

    return sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nIndex);
}




















static AttachmentID sEffectGroupPickByTeam(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)





{
    if (!RActorTryToGet((RActorID)idOwner))
    {
        return ATTACHMENTID_INVALID;
    }

    RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)idOwner, TRUE, FALSE);

    const RActor * ptTopmost = RActorGet(idRActorTopmost);

    if (ptTopmost->idACD == ACDID_INVALID)
    {
        return ATTACHMENTID_INVALID;
    }

    ActorCommonData & tACDTopmost = ACDGet(ptTopmost->idACD);

    Team eTeam = TEAM_NEUTRAL;








    if (tACDTopmost.GetTeam() != TEAM_NEUTRAL)
    {


        eTeam = ACDIsFriendly(tACDTopmost, ACDGet(LocalPlayerACDGet()), FALSE) ? TEAM_PLAYER_1 : TEAM_PLAYER_0;
    }










    int32 nIndex = TeamGetEffectGroupIndex(eTeam);

    if (nIndex == TEAM_COLOR_NONE)
    {
        return ATTACHMENTID_INVALID;
    }












    return sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nIndex);
}










static AttachmentID sEffectGroupPickByResourceLevel(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)





{
    RActor * ptRActor = RActorTryToGet((RActorID)idOwner);
    if (!ptRActor)
    {
        return ATTACHMENTID_INVALID;
    }

    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (!ptACD)
    {
        return ATTACHMENTID_INVALID;
    }

    uint32 dwScalar = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_RESOURCE_EFFECT_LEVEL));

    return sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, dwScalar);
}









static AttachmentID sEffectGroupPickByRarity(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)






{
    const RActor * ptRActor = RActorTryToGet((RActorID)idOwner);
    if (!ptRActor)
    {
        return ATTACHMENTID_INVALID;
    }

    RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)idOwner, TRUE, FALSE);
    const RActor * ptTopmost = RActorGet(idRActorTopmost);

    ActorCommonData * ptACDTopmost = ACDTryToGet(ptTopmost->idACD);
    if (!ptACDTopmost)
    {
        return ATTACHMENTID_INVALID;
    }

    int32 nEffectIndex;  // MonsterRarity has no enumerators in the DWARF: literal values

    switch (ptACDTopmost->eMonsterRarity)
    {
    case 0: nEffectIndex = 0; break;
    case 1: nEffectIndex = 1; break;
    case 2: nEffectIndex = 2; break;
    case 3: nEffectIndex = 3; break;
    case 4: nEffectIndex = 5; break;
    case 5: nEffectIndex = 4; break;
    case 7: nEffectIndex = 6; break;
    default:
        return ATTACHMENTID_INVALID;
    }

    if (nEffectIndex >= ptEffectGroupDef->nEffectCount)
    {
        return ATTACHMENTID_INVALID;
    }

    sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nEffectIndex);
    return ATTACHMENTID_INVALID;
}











static AttachmentID sEffectGroupPickByTopology(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)





{
    const RActor * ptRActor = RActorTryToGet((RActorID)idOwner);
    if (!ptRActor)
    {
        return ATTACHMENTID_INVALID;
    }

    RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)idOwner, TRUE, FALSE);
    const RActor * ptTopmost = RActorGet(idRActorTopmost);

    ActorTopology eTopology = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_TOPOLOGY, ptTopmost->snoActor);

    int32 nEffectIndex = eTopology;

    if (nEffectIndex >= ptEffectGroupDef->nEffectCount)
    {
        return ATTACHMENTID_INVALID;
    }

    sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nEffectIndex);
    return ATTACHMENTID_INVALID;
}









static ActorGender sResolvePlayerGenderFromActorSNO(SNO snoActor)
{

    for (int i = 0; i < NUM_PLAYER_CLASSES; i++)
    {
        PlayerClass eClass = (PlayerClass)i;
        GBID gbidClass = PlayerClassToGBID(eClass);

        if (GetFrontendCharacterSNO(eClass, FALSE) == snoActor)
        {
            return ACTOR_GENDER_MALE;
        }
        if (GetFrontendCharacterSNO(eClass, TRUE) == snoActor)
        {
            return ACTOR_GENDER_FEMALE;
        }

        const GameBalanceDefinition * pdefGB;
        const HeroData * pHeroData = (const HeroData *)GBLockR(GBHandle(GB_HEROS, gbidClass), pdefGB, TRUE);
        if (pHeroData)
        {
            if (pHeroData->snoActorMale == snoActor)
            {
                return ACTOR_GENDER_MALE;
            }
            if (pHeroData->snoActorFemale == snoActor)
            {
                return ACTOR_GENDER_FEMALE;
            }
        }
    }

    return ACTOR_GENDER_INVALID;
}



static AttachmentID sEffectGroupPickByGender(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)





{
    const RActor * ptRActor = RActorTryToGet((RActorID)idOwner);
    if (!ptRActor)
    {
        return ATTACHMENTID_INVALID;
    }

    RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)idOwner, TRUE, FALSE);
    const RActor * ptTopmost = RActorGet(idRActorTopmost);

    ActorGender eGender;
    Player * ptPlayer = PlayerGetByACD(ptTopmost->idACD);
    if (ptPlayer)
    {
        eGender = PlayerIsMale(*ptPlayer) ? ACTOR_GENDER_MALE : ACTOR_GENDER_FEMALE;
    }
    else
    {

        eGender = sResolvePlayerGenderFromActorSNO(ptRActor->snoActor);
    }

    if (eGender == ACTOR_GENDER_INVALID)
    {
        return ATTACHMENTID_INVALID;
    }

    int32 nEffectIndex = eGender;
    if (ptEffectGroupDef->nEffectCount <= nEffectIndex)
    {
        return ATTACHMENTID_INVALID;
    }

    sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nEffectIndex);
    return ATTACHMENTID_INVALID;
}







static PlayerClass sResolvePlayerClassFromActorSNO(SNO snoActor)
{

    for (int i = 0; i < NUM_PLAYER_CLASSES; i++)
    {
        PlayerClass eClass = (PlayerClass)i;
        GBID gbidClass = PlayerClassToGBID(eClass);

        if (GetFrontendCharacterSNO(eClass, FALSE) == snoActor ||
            GetFrontendCharacterSNO(eClass, TRUE) == snoActor)
        {
            return eClass;
        }

        const GameBalanceDefinition * pdefGB;
        const HeroData * pHeroData = (const HeroData *)GBLockR(GBHandle(GB_HEROS, gbidClass), pdefGB, TRUE);
        if (pHeroData)
        {
            if (pHeroData->snoActorMale == snoActor ||
                pHeroData->snoActorFemale == snoActor)
            {
                return eClass;
            }
        }
    }

    return PLAYER_CLASS_NONE;
}




static AttachmentID sEffectGroupPickByClass(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)





{
    const RActor * ptRActor = RActorTryToGet((RActorID)idOwner);
    if (!ptRActor)
    {
        return ATTACHMENTID_INVALID;
    }

    RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)idOwner, TRUE, FALSE);
    const RActor * ptTopmost = RActorGet(idRActorTopmost);

    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptTopmost->idACD);
    if (ePlayerClass == PLAYER_CLASS_NONE)
    {
        ePlayerClass = sResolvePlayerClassFromActorSNO(ptRActor->snoActor);
    }

    if (ePlayerClass == PLAYER_CLASS_NONE)
    {
        return ATTACHMENTID_INVALID;
    }

    int32 nEffectIndex = ePlayerClass;
    if (nEffectIndex >= ptEffectGroupDef->nEffectCount)
    {
        return ATTACHMENTID_INVALID;
    }

    sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nEffectIndex);
    return ATTACHMENTID_INVALID;
}







static AttachmentID sEffectGroupPickByShaderHWClass(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)





{
    if (ptEffectGroupDef->nEffectCount == 0)
    {
        return ATTACHMENTID_INVALID;
    }

    int32 nEffectIndex = GfxGetHWClass();
    if (nEffectIndex >= ptEffectGroupDef->nEffectCount)
    {
        nEffectIndex = ptEffectGroupDef->nEffectCount - 1;
    }

    sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nEffectIndex);
    return ATTACHMENTID_INVALID;
}










static AttachmentID sEffectGroupPickByLook(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)





{
    if (ptEffectGroupDef->nEffectCount == 0 || idOwner == DATAID_INVALID)
    {
        return ATTACHMENTID_INVALID;
    }

    const RActor * ptRActor = RActorTryToGet((RActorID)idOwner);
    if (!ptRActor)
    {
        return ATTACHMENTID_INVALID;
    }

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);



    uint32 dwLookHash = pdefAppearance->ptAppearanceLooks[ptRActor->nLookActive].dwLookHash;




    AttachmentID idAttachment = ATTACHMENTID_INVALID;
    for (int32 i = 0; i < ptEffectGroupDef->nEffectCount; i++)
    {
        const EffectItem & tEffectItem = ptEffectGroupDef->ptEffects[i];


        if (dwLookHash == tEffectItem.dwHash)
        {






            AttachmentID idLastAttachment = sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, i);
        }
    }

    return idAttachment;











}



static AttachmentID sEffectGroupPickByWeaponDamageType(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)






{
    if (ptEffectGroupDef->nEffectCount < 1 || idOwner == DATAID_INVALID)
    {
        return ATTACHMENTID_INVALID;
    }


    const RActor * ptRActor = RActorTryToGet((RActorID)idOwner);
    if (!ptRActor)
    {
        return ATTACHMENTID_INVALID;
    }



    int nNumEffects = ptEffectGroupDef->nEffectCount;










    int32 nEffectIndex = Math::Pin(ptRActor->eItemEffectType, 0, NUM_ITEM_EFFECT_TYPES - 1);

    sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nEffectIndex);
    return ATTACHMENTID_INVALID;
}









static AttachmentID sEffectGroupPickByWeaponDamage(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)






{
    if (ptEffectGroupDef->nEffectCount < 1 || idOwner == DATAID_INVALID)
    {
        return ATTACHMENTID_INVALID;
    }


    const RActor * ptRActor = RActorTryToGet((RActorID)idOwner);
    if (!ptRActor)
    {
        return ATTACHMENTID_INVALID;
    }

    int32 nItemEffectLevel = ptRActor->nItemEffectLevel;
    int32 nEffectCount = ptEffectGroupDef->nEffectCount;




















    int32 nEffectIndex = Math::Pin(nItemEffectLevel, 0, nEffectCount - 1);

    sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nEffectIndex);
    return ATTACHMENTID_INVALID;
}









static AttachmentID sEffectGroupPickByRune(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)






{
    if (ptEffectGroupDef->nEffectCount < 1 || idOwner == DATAID_INVALID)
    {
        return ATTACHMENTID_INVALID;
    }


    const RActor * ptRActor = RActorTryToGet((RActorID)idOwner);
    if (!ptRActor)
    {
        return ATTACHMENTID_INVALID;
    }


    RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)idOwner, TRUE, FALSE);
    const RActor * ptTopmost = RActorGet(idRActorTopmost);

    if (ptTopmost->idACD == ACDID_INVALID)
    {
        return ATTACHMENTID_INVALID;
    }

    ActorCommonData & tACDTopmost = ACDGet(ptTopmost->idACD);




    int32 nEffectIndex = 0;

    if (tEffectEvent.bUseRuneType)
    {

        nEffectIndex = (tEffectEvent.eRuneType >= RUNE_TYPE_A && tEffectEvent.eRuneType < RUNE_TYPE_COUNT) ? tEffectEvent.eRuneType + 1 : 0;
    }
    else
    {









        SNO snoPower = ptEffectGroupDef->snoPower;
        if (tACDTopmost.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, snoPower)))
        {
            nEffectIndex = 1;
        }
        else if (tACDTopmost.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, snoPower)))
        {
            nEffectIndex = 2;
        }
        else if (tACDTopmost.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, snoPower)))
        {
            nEffectIndex = 3;
        }
        else if (tACDTopmost.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, snoPower)))
        {
            nEffectIndex = 4;
        }
        else if (tACDTopmost.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, snoPower)))
        {
            nEffectIndex = 5;
        }
    }

    sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nEffectIndex);
    return ATTACHMENTID_INVALID;
}














static AttachmentID sEffectGroupPickByGameType(const EffectGroupDefinition * ptEffectGroupDef, const TriggerEvent & tEffectEvent, const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType)






{
    if (ptEffectGroupDef->nEffectCount < 1)
    {
        return ATTACHMENTID_INVALID;
    }

    int32 nEffectIndex = GameTypeIsPvP() ? 1 : 0;

    sEffectGroupCreate(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType, nEffectIndex);
    return ATTACHMENTID_INVALID;
}















AttachmentID EffectGroupAttachmentAdd(const DataID idOwner, const DataID * idAttachParents, const ParentType eParentType, const TriggerEvent & tEffectEvent)




{
    if (tEffectEvent.snoname.snoHandle == SNO_NONE)
    {
        return ATTACHMENTID_INVALID;
    }

    ScopedEffectGroupDefinitionLockR ptEffectGroupDef(tEffectEvent.snoname.snoHandle);

    AttachmentID idAttachment = ATTACHMENTID_INVALID;

    switch (ptEffectGroupDef->nEffectWeightMethod)
    {
    case EFFECT_WEIGHT_PICK_ONE:
        idAttachment = sEffectGroupPickOne(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_PICK_SOME:
        idAttachment = sEffectGroupPickSome(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_PICK_ALL:
        idAttachment = sEffectGroupPickAll(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_RESOURCE_LEVEL:
        idAttachment = sEffectGroupPickByResourceLevel(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_TEAM:
        idAttachment = sEffectGroupPickByTeam(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_FRIENDLY_STATUS:
        idAttachment = sEffectGroupPickByFriendlyStatus(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_RARITY:
        idAttachment = sEffectGroupPickByRarity(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_TOPOLOGY:
        idAttachment = sEffectGroupPickByTopology(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_GENDER:
        idAttachment = sEffectGroupPickByGender(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_CLASS:
        idAttachment = sEffectGroupPickByClass(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_SHADER_HW_CLASS:
        idAttachment = sEffectGroupPickByShaderHWClass(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_WEAPON_DAMAGE_TYPE:
        idAttachment = sEffectGroupPickByWeaponDamageType(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_WEAPON_DAMAGE:
        idAttachment = sEffectGroupPickByWeaponDamage(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_LOOK:
        idAttachment = sEffectGroupPickByLook(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_RUNE:
        idAttachment = sEffectGroupPickByRune(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;

    case EFFECT_WEIGHT_BY_GAMETYPE:
        idAttachment = sEffectGroupPickByGameType(ptEffectGroupDef, tEffectEvent, idOwner, idAttachParents, eParentType);
        break;
    }

    return idAttachment;
}






static void sEffectGroupAttach(const SNO snoEffectGroup, const RActorID idRActorPrimary, const RActorID idRActorSecondary, const uint32 dwAttachmentID, ParentType eType, BOOL bIgnoreOwnerAlpha, BOOL bRemote)







{

    TriggerEvent tEffectEvent;
    TriggerEventGetDefaultData(tEffectEvent);
    tEffectEvent.dwTriggerFlags = 0;
    tEffectEvent.eType = TRIGGEREDEVENTTYPE_ADD_EFFECT_GROUP;
    tEffectEvent.snoname.Set(EFFECTGROUP_SNO, snoEffectGroup);
    tEffectEvent.dwID = dwAttachmentID;
    tEffectEvent.fIgnoreOwnerAlpha = bIgnoreOwnerAlpha;
    for (int32 i = 0; i < 2; i++)
    {
        tEffectEvent.tHardpointLinks[i].tHash = DEFAULT_HARDPOINT_NAME;
    }
    if (bRemote)
    {
        tEffectEvent.dwTriggerFlags |= (1 << TRIGGERFLAG_DISABLED_IF_REMOTE_PLAYER);
    }


    RActorID idRActors[3] = { idRActorPrimary, idRActorSecondary, RACTORID_INVALID };


    EffectGroupAttachmentAdd(idRActorPrimary, (const DataID *)idRActors, eType, tEffectEvent);





}



void EffectGroupCreateUntracked(const SNO snoEffectGroup, const RActorID idRActorSource, const RActorID idRActorDest, const uint32 dwAttachmentID, const BOOL bIgnoreOwnerAlpha, const BOOL bRemote)






{




    sEffectGroupAttach(snoEffectGroup, idRActorSource, idRActorDest, dwAttachmentID, 0, bIgnoreOwnerAlpha, bRemote);
}



void EffectGroupCreateUntracked(const SNO snoEffectGroup, const RActorID idRActorSource, const uint32 dwAttachmentID, const BOOL bIgnoreOwnerAlpha, const BOOL bRemote)





{
    sEffectGroupAttach(snoEffectGroup, idRActorSource, RACTORID_INVALID, dwAttachmentID, 0, bIgnoreOwnerAlpha, bRemote);
}



EffectGroupID EffectGroupCreateTracked(const SNO snoEffectGroup, const RActorID idRActorSource, const RActorID idRActorDest, ParentType eTypeOverride, const BOOL bIgnoreOwnerAlpha)





{
    RActor * ptRActor = RActorTryToGet(idRActorSource);
    if (ptRActor == NULL)
    {
        return EFFECTGROUPID_INVALID;
    }

    EffectGroupGlobals & tEffectGroupGlobals = sEffectGroupGlobalsGet();


    EffectGroupID idEffectGroup = tEffectGroupGlobals.ptClientTrackedEffectGroups->ElementNew();
    if (idEffectGroup != EFFECTGROUPID_INVALID)
    {
        EffectGroup & tEffectGroup = EffectGroupGet(idEffectGroup);


        tEffectGroup.idRActorOwner = idRActorSource;

        ptRActor->nTrackedEffectGroupID++;
        if (ptRActor->nTrackedEffectGroupID >= 1100)
        {
            ptRActor->nTrackedEffectGroupID = 1000;
        }
        tEffectGroup.dwTrackingID = ptRActor->nTrackedEffectGroupID;

        sEffectGroupAttach(snoEffectGroup, idRActorSource, idRActorDest, tEffectGroup.dwTrackingID, eTypeOverride, bIgnoreOwnerAlpha, FALSE);
    }

    return idEffectGroup;





}



EffectGroupID EffectGroupCreateRopeTracked(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom, const ACDID idACDTo, const RopePosition ePositionTo, const BOOL bIgnoreOwnerAlpha)
{






    return RActorACDRopeEffect(snoRope, idACDFrom, ePositionFrom, idACDTo, ePositionTo, bIgnoreOwnerAlpha);
}



void EffectGroupDelete(const EffectGroupID idEffectGroup)

{
    EffectGroupGlobals & tEffectGroupGlobals = sEffectGroupGlobalsGet();

    EffectGroup * ptEffectGroup = tEffectGroupGlobals.ptClientTrackedEffectGroups->ElementTryToGet(idEffectGroup);
    if (ptEffectGroup == NULL)
    {
        return;
    }






    RActor * ptRActor = RActorTryToGet(ptEffectGroup->idRActorOwner);
    if (ptRActor)
    {
        TriggerEvent tTriggerEvent;
        TriggerEventInit(tTriggerEvent);
        tTriggerEvent.eType = TRIGGEREDEVENTTYPE_DELETE_OBJECT;
        tTriggerEvent.dwID = ptEffectGroup->dwTrackingID;
        RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
    }

    tEffectGroupGlobals.ptClientTrackedEffectGroups->ElementDeleteByID(idEffectGroup);
}



void EffectGroupOutro(const EffectGroupID idEffectGroup)

{
    EffectGroupGlobals & tEffectGroupGlobals = sEffectGroupGlobalsGet();

    EffectGroup * ptEffectGroup = tEffectGroupGlobals.ptClientTrackedEffectGroups->ElementTryToGet(idEffectGroup);
    if (ptEffectGroup == NULL)
    {
        return;
    }






    RActor * ptRActor = RActorTryToGet(ptEffectGroup->idRActorOwner);
    if (ptRActor)
    {
        TriggerEvent tTriggerEvent;
        TriggerEventInit(tTriggerEvent);
        tTriggerEvent.eType = TRIGGEREDEVENTTYPE_OUTRO_OBJECT;
        tTriggerEvent.dwID = ptEffectGroup->dwTrackingID;
        RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
    }

    tEffectGroupGlobals.ptClientTrackedEffectGroups->ElementDeleteByID(idEffectGroup);
}
