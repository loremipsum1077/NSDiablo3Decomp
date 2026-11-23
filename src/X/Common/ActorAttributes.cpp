// X/Common/ActorAttributes.cpp
// Functions sit on their original line numbers.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "ActorAttributes.h"
#include "ActorCommonData.h"
#include "ActorInventory.h"
#include "CommonTypes.h"
#include "ItemsSwig.h"
#include "Monsters.h"
#include "Attributes/FastAttribute.h"
#include "Definitions/ActorDefinition.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Definitions/PowerDefinition.h"
#include "Powers/PowersCommon.h"
#include "SNO/SNOFiles.h"
#include "TagMaps/ActorTags.h"
#include "TagMaps/Tags.h"
#include "../Sigma/MemoryPools.h"











void ActorCommonData::AttributesUpdateCached()
{
    if (idFastAttrib == ATTRIBGROUPID_INVALID)
    {
        return;
    }



    eTeamOverride = AttributesGetInt(FastAttribKey(ATTRIBUTE_TEAM_OVERRIDE));
    eTeam = AttributesGetInt(FastAttribKey(ATTRIBUTE_TEAM));
    flHitPointsCurrent = AttributesGetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_CUR));
    fIsAlive = flHitPointsCurrent > 0.0f;
    fIsUntargetable = AttributesGetInt(FastAttribKey(ATTRIBUTE_UNTARGETABLE)) != 0;
    fIsStealthed = AttributesGetInt(FastAttribKey(ATTRIBUTE_STEALTHED)) != 0;
    nPetOwner = AttributesGetInt(FastAttribKey(ATTRIBUTE_PET_OWNER));
    fIsNPC = AttributesGetInt(FastAttribKey(ATTRIBUTE_IS_NPC)) != 0;
    fIsDisabled = AttributesGetInt(FastAttribKey(ATTRIBUTE_DISABLED)) != 0;
    fIsCharmed = AttributesGetInt(FastAttribKey(ATTRIBUTE_CHARMED)) > 0;
    fIsStunned = AttributesGetInt(FastAttribKey(ATTRIBUTE_STUNNED)) > 0 || AttributesGetInt(FastAttribKey(ATTRIBUTE_FROZEN)) > 0 || AttributesGetInt(FastAttribKey(ATTRIBUTE_FORCE_GRIPPED)) > 0;
    fHasAssignedHeroID = GetAssignedHeroIDLow() != 0;
    eHirelingClass = AttributesGetInt(FastAttribKey(ATTRIBUTE_HIRELING_CLASS));


}




void ActorCommonData::SetAttributeGroup(const AttribGroupID idFastAttributeGroup)

{
    idFastAttrib = idFastAttributeGroup;

    ptFastAttribGroup = FastAttribGroupGet(idFastAttributeGroup);

}




float ActorCommonData::AttributesGetFloat(FastAttribKey tKey) const
{




    return FastAttribGetValueFloat(ptFastAttribGroup, tKey);
}







float ActorCommonData::AttributesGetBaseFloat(FastAttribKey tKey) const
{




    return FastAttribGetBaseValueFloat(ptFastAttribGroup, tKey);
}







float ActorCommonData::AttributesSetFloat(FastAttribKey tKey, float flValue)
{



    float flValuePrevious = FastAttribGetValueFloat(ptFastAttribGroup, tKey);
    FastAttribSetValueFloat(ptFastAttribGroup, tKey, flValue);




    if (flValuePrevious != flValue && (uAttributeSyncMask & KeyGetSyncFlags(tKey)))
    {
        ModifiedMark(ACDMODIFIED_ATTRIBUTES);
    }
    return flValuePrevious;
}




int32 ActorCommonData::AttributesGetInt(FastAttribKey tKey) const
{




    return FastAttribGetValueInt(ptFastAttribGroup, tKey);
}






















void ActorCommonData::AttributesSetInt(FastAttribKey tKey, int32 nValue)
{



    int32 nValuePrevious = FastAttribGetValueInt(ptFastAttribGroup, tKey);
    FastAttribSetValueInt(ptFastAttribGroup, tKey, nValue);




    if (nValuePrevious != nValue && (uAttributeSyncMask & KeyGetSyncFlags(tKey)))
    {
        ModifiedMark(ACDMODIFIED_ATTRIBUTES);
    }
}


















SNO ActorCommonData::WhichAttackIsBetter(SNO snoAttackPower0, SNO snoAttackPower1) const
{


    if (snoAttackPower0 == SNO_NONE)
    {
        return snoAttackPower1;
    }

    if (snoAttackPower1 == SNO_NONE)
    {
        return snoAttackPower0;
    }

    if (PowerIsPunch(snoAttackPower0) && ACDGetCurrentWeaponClass(*this) != WEAPONCLASS_NONE)
    {
        return snoAttackPower1;
    }

    ACDID idACDWeapon = ACDInventoryGetPrimaryWeapon(id);
    if (idACDWeapon != ACDID_INVALID)
    {
        ActorCommonData & tACDWeapon = ACDGet(idACDWeapon);

        SNO snoWeaponPower = tACDWeapon.AttributesGetPower(0);
        if (snoWeaponPower == snoAttackPower0 || snoWeaponPower == snoAttackPower1)
        {
            return snoWeaponPower;
        }
    }

    return snoAttackPower0;
}




SNO ActorCommonData::AttributesGetPower(const int32 nIndex) const
{
    SNO snoPower = SNO_NONE;


    if (IsMonster())
    {

        ScopedActorDefinitionLockR pdefActor(snoActor);
        if (pdefActor->snoMonster != SNO_NONE)
        {







            SkillDeclaration tSkillDec = MonsterGetSkill(tSNOName, nIndex);
            snoPower = tSkillDec.snoPower;
        }
    }
    else if (hGB.eType == GB_ITEMS)
    {
        const GameBalanceDefinition * pdefItems;


        const ItemDefinition * pItem = (const ItemDefinition *)GBLockR(hGB, pdefItems, TRUE);
        snoPower = pItem->tSkills[nIndex].snoPower;
        GBUnlockR(pdefItems);
    }
    else if (IsGizmo())
    {

        ScopedActorDefinitionLockR ptActorDef(snoActor);
        snoPower = TagMapGetSNO(ptActorDef->hTagMap, TAG_ACTOR_GIZMO_POWER_ON_OPERATE);
    }

    return snoPower;
}



SNO ActorCommonData::AttributeGetBasicAttack() const
{
    SNO snoBasicAttack;

    SNOPooledList listSkills(MemoryPoolsGetDATAIDListPool());
    AttributesGetPowers(0, listSkills, &snoBasicAttack);

    return snoBasicAttack;
}




void ActorCommonData::AttributesGetPowers(uint32 dwSkillQueryFlags, SNOPooledList & listSkills, SNO * psnoAttackPower) const
{
    SNO snoAttackPower = SNO_NONE;



    FastAttribIterator tIt(idFastAttrib, TRUE);
    while (FastAttribIterate(tIt))
    {
        Attrib eAttrib = KeyGetAttrib(tIt.tEntryBuffer.tKey);
        if (eAttrib == ATTRIBUTE_SKILL_TOTAL)
        {
            SNO snoHandle = PowerSNOFromAttributeKey(tIt.tEntryBuffer.tKey);

            if (snoHandle == SNO_NONE)
            {
                continue;
            }

            int32 nPtsTotal = tIt.tEntryBuffer.tValue.nValue;
            if (nPtsTotal <= 0)
            {
                continue;
            }










            if (dwSkillQueryFlags & (1 << SKILL_QUERY_EXCLUDE_STANDARD_ATTACK))
            {
                if (PowerIsBasicAttack(snoHandle))
                {
                    continue;
                }
            }











            if (dwSkillQueryFlags & (1 << SKILL_QUERY_EXCLUDE_TOGGLEABLES))
            {
                if (PowerIsToggleable(snoHandle))
                {
                    continue;
                }
            }

            if (dwSkillQueryFlags & (1 << SKILL_QUERY_EXCLUDE_NON_TOGGLEABLE))
            {
                if (!PowerIsToggleable(snoHandle))
                {
                    continue;
                }
            }


























            if (PowerIsBasicAttack(snoHandle))
            {
                snoAttackPower = WhichAttackIsBetter(snoAttackPower, snoHandle);
                continue;
            }


            if (PowerIsDisplayed(snoHandle))
            {
                listSkills.AddTail_Tracked(snoHandle);
            }
        }
    }


    if (snoAttackPower != SNO_NONE)
    {

        listSkills.AddHead_Tracked(snoAttackPower);
    }

    if (psnoAttackPower)
    {
        *psnoAttackPower = snoAttackPower;
    }
}




BOOL ActorCommonData::AttributesAttributeExists(FastAttribKey tKey) const
{




    return FastAttribExists(ptFastAttribGroup, tKey);
}







AttribGroupID ActorCommonData::AttributesGetAttributeGroup() const
{

    return idFastAttrib;
}




void ActorCommonData::AttributesUnmergeAttributeGroup(AttribGroupID idFastAttribUnmerge)
{


    FastAttribUnmerge(idFastAttrib, idFastAttribUnmerge);
    ModifiedMark(ACDMODIFIED_ATTRIBUTES);
}





void ActorCommonData::AttributesMergeBaseAttributeGroup(AttribGroupID idFastAttribMerge)
{


    FastAttribMergePermanent(idFastAttrib, idFastAttribMerge);
    ModifiedMark(ACDMODIFIED_ATTRIBUTES);
}





void ActorCommonData::AttributesMergeAttributeGroup(AttribGroupID idFastAttribMerge)
{


    FastAttribMerge(idFastAttrib, idFastAttribMerge);
    ModifiedMark(ACDMODIFIED_ATTRIBUTES);
}
