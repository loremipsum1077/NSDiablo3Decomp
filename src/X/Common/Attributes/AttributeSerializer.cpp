// Original file: X/Common/Attributes/AttributeSerializer.cpp
// Recovered from 2.6.2 DWARF, disassembly and original constant bytes.
#include "AttributeSerializer.h"
#include "FastAttribute.h"
#include "../ActorCommonData.h"
#include "../ActorInventory.h"
#include "../GameBalance.h"
#include "../GameCommonData.h"
#include "../GlobalSNO.h"
#include "../Player.h"
#include "../Definitions/SkillKitDefinition.h"
#include "../../Sigma/RefString.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Sigma/Math/Vector.h"
#include "../Protobuf/AttributeSerializer.pb.h"

BOOL GameActiveGameCommonDataIsServer(); // Game.cpp:923

// Original line 31.
static BOOL sAttribIsSaved(const SavedAttribDef * paAttribsToSave, uint32 uAttribsToSaveCount, Attrib eAttrib) {
    for (uint32 i = 0; i < uAttribsToSaveCount; ++i)
        if (paAttribsToSave[i].eAttrib == eAttrib) return TRUE;
    return FALSE;
}

// Original line 49.
static inline FastAttribKey sNormalKeyToSaveKey(const SavedAttribDef *paAttribsToSave,
                                                const uint32 uAttribsToSaveCount,
                                                const FastAttribKey &tKey) {
    int eAttrib = KeyGetAttrib(tKey);
    for (uint32 i = 0; i < uAttribsToSaveCount; ++i) {
        if (paAttribsToSave[i].eAttrib == eAttrib)
            return MakeAttribKey(paAttribsToSave[i].nSavedAttribId, KeyGetParam(tKey));
    }
    return MakeAttribKey(-1, 0);
}

// Original line 72.
static FastAttribKey sSaveKeyToNormalKey(const SavedAttribDef * paAttribsToSave, uint32 uAttribsToSaveCount, const FastAttribKey & tKey) {
    Attrib eSaveAttrib = KeyGetAttrib(tKey);
    for (uint32 i = 0; i < uAttribsToSaveCount; ++i)
        if (paAttribsToSave[i].nSavedAttribId == eSaveAttrib)
            return MakeAttribKey(paAttribsToSave[i].eAttrib, KeyGetParam(tKey));
    return MakeAttribKey(-1, 0);
}

// Original line 96.
inline BOOL sTraitIsInAttributesAndAssignedToSkills(const ActorCommonData & tACD, SNO snoPowerTrait) {
    if (!ACDIsPlayer(tACD.id)) return FALSE;
    Player * ptPlayer = PlayerGetByACD(tACD.id);
    for (int i = 0; i < 4; ++i)
        if (ptPlayer->tHeroStateData.tSavedData.snoTraits[i] == snoPowerTrait) return TRUE;
    return FALSE;
}

// Original line 120.
inline BOOL sTraitExistsInSkillkit(const ActorCommonData & tACD, SNO snoPowerTrait) {
    if (!ACDIsPlayer(tACD.id)) return FALSE;
    SNO snoSkillKit = tACD.GetSkillKit();
    const TraitEntry * ptTraitEntry = SkillKitGetMatchingTraitEntry(snoSkillKit, snoPowerTrait);
    return ptTraitEntry && ptTraitEntry->snoPower == snoPowerTrait && !ptTraitEntry->fHide;
}

// Original line 142.
BOOL sCheckForHellfireAmulet(const ActorCommonData &tACD) {
    if (!ACDIsPlayer((ACDID)tACD.id))
        return FALSE;

    const ActorCommonData *ptACDAmulet =
        ACDTryToGet(ACDInventoryGetSquareItem(tACD, 13, cgptNull));
    if (ptACDAmulet != NULL) {
        GBID gbidHellfireAmulet = StringToGBID("x1_Amulet_norm_unique_25");
        if (ptACDAmulet->hGB.gbid == gbidHellfireAmulet)
            return TRUE;
    }
    return FALSE;
}

// Original line 165.
// NON_MATCHING: 780 vs 772; only the inlined vector<int>::push_back(T&&) growth path differs (csinc vs csel, an extra
// branch and register copy in ours). Inference: nMinTraitsForError set by an if (original 198:9 condition column).
#line 165
void AttributeSerializer_CheckAndMendHellfireExploit(
    ActorCommonData & tACD,
    BOOL bIsRestore)
{
    if (!ACDIsPlayer(tACD.id))
    {
        return;
    }

    blz::vector<SNO> arrSNOInvalidTraits;

    AttribGroupID idFastAttrib = tACD.AttributesGetAttributeGroup();
    FastAttribIterator tIt(idFastAttrib, TRUE);
    while (FastAttribIterate(tIt))
    {
        FastAttribKey tKey = tIt.tEntryBuffer.tKey;
        FastAttribValue tValue = tIt.tEntryBuffer.tValue;

        Attrib eAttrib = KeyGetAttrib(tKey);
        int32 nParam = KeyGetParam(tKey);


        if (eAttrib == ATTRIBUTE_SKILL &&
            !sTraitIsInAttributesAndAssignedToSkills(tACD, nParam) &&
            sTraitExistsInSkillkit(tACD, nParam))
        {
            arrSNOInvalidTraits.push_back(SNO(nParam));
        }
    }



    int32 nMinTraitsForError = 0;
    if (sCheckForHellfireAmulet(tACD))
    {
        nMinTraitsForError = 1;
    }

    if (nMinTraitsForError < (int32)arrSNOInvalidTraits.size())
    {
        Player * ptPlayer = PlayerGetByACD(tACD.id);
        if (!ptPlayer)
        {
            return;
        }




        CRefString sError;
        sError.Format("Player Acct(%u) Hero(%llu) on %s had %d non assigned passive traits equipped (%s). Trait SNOs were: ",
            ptPlayer->tAccount.tHandle.m_id,
            ptPlayer->uHeroId,
            bIsRestore ? "Load" : "Save",
            arrSNOInvalidTraits.size(),
            !nMinTraitsForError ? "No hellfire equipped" : "Had hellfire equipped");

        for (uint32 i = 0; i < arrSNOInvalidTraits.size(); ++i)
        {
            CRefString sAppendError;
            sAppendError.Format("%d,", arrSNOInvalidTraits[i]);
            sError += sAppendError;
        }

        sError += "\n";



    }


    for (uint32 i = 0; i < arrSNOInvalidTraits.size(); ++i)
    {
        FastAttribSetValueInt(tACD.idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL, arrSNOInvalidTraits[i]), 0);
    }
}
#line 115

// Original line 247.
BOOL sAttributeSerializer_IgnoreAttribute(const FastAttribKey &tKey) {
    const GlobalSNO snoAttributePowersToIgnore[] = {GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT, GLOBALSNO_POWERS_WEAPON_RANGED_PROJECTILE, GLOBALSNO_POWERS_WEAPON_RANGED_WAND, GLOBALSNO_POWERS_KILLELITEPACK};
    if (KeyGetAttrib(tKey) != ATTRIBUTE_SKILL)
        return FALSE;
    for (uint32 i = 0; i < 4; ++i) {
        if (KeyGetParam(tKey) == GlobalSNOGet(snoAttributePowersToIgnore[i]))
            return TRUE;
    }
    return FALSE;
}

// Original line 273.
void AttributeSerializer_Save(const SavedAttribDef * paAttribsToSave, uint32 uAttribsToSaveCount, ActorCommonData & tACD, D3::AttributeSerializer::SavedAttributes & tSavedAttributes) {
    AttribGroupID idFastAttrib = tACD.AttributesGetAttributeGroup();
    BOOL bIsServer = GameActiveGameCommonDataIsServer();
    AttributeSerializer_CheckAndMendHellfireExploit(tACD, FALSE);
    FastAttribIterator tIt(idFastAttrib, TRUE);
    while (FastAttribIterate(tIt)) {
        FastAttribKey tKey = tIt.tEntryBuffer.tKey;
        FastAttribValue tValue = tIt.tEntryBuffer.tValue;
        Attrib eAttrib = KeyGetAttrib(tKey);
        if (sAttribIsSaved(paAttribsToSave, uAttribsToSaveCount, eAttrib) && !sAttributeSerializer_IgnoreAttribute(tKey)) {
            FastAttribKey tSaveKey = sNormalKeyToSaveKey(paAttribsToSave, uAttribsToSaveCount, tKey);
            D3::AttributeSerializer::SavedAttribute * pNewAttribute = tSavedAttributes.add_attributes();
            pNewAttribute->set_key(tSaveKey.nValue);
            pNewAttribute->set_value(bIsServer ? FastAttribGetBaseValue(idFastAttrib, tKey).nValue : tValue.nValue);
        }
    }
    if (tSavedAttributes.attributes_size()) {
        D3::GameBalance::Handle * pHandle = tSavedAttributes.mutable_gb_handle();
        pHandle->set_game_balance_type(tACD.hGB.eType);
        pHandle->set_gbid(tACD.hGB.gbid);
    }
}

// Original line 331.
void AttributeSerializer_Restore(const SavedAttribDef * paAttribsToSave, uint32 uAttribsToSaveCount, const D3::AttributeSerializer::SavedAttributes & tSavedAttributes, ActorCommonData & tACD) {
    AttribGroupID idFastAttrib = tACD.AttributesGetAttributeGroup();
    for (int32 i = 0; i < tSavedAttributes.attributes_size(); ++i) {
        const D3::AttributeSerializer::SavedAttribute & tSavedAttribute = tSavedAttributes.attributes(i);
        FastAttribKey tSavedKey;
        tSavedKey.nValue = tSavedAttribute.key();
        FastAttribKey tNormalKey = sSaveKeyToNormalKey(paAttribsToSave, uAttribsToSaveCount, tSavedKey);
        Attrib eAttrib = KeyGetAttrib(tNormalKey);
        if ((uint32)eAttrib < NUM_ATTRIBUTES)
            FastAttribSetValue(idFastAttrib, tNormalKey, FastAttribValue(tSavedAttribute.value()));
    }
    AttributeSerializer_CheckAndMendHellfireExploit(tACD, TRUE);
    tACD.ModifiedMark(FALSE);
}

// Original line 361.
int32 AttributeSerializer_GetInt(const SavedAttribDef * paAttribsToSave, uint32 uAttribsToSaveCount, const D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKey, int32 nDefaultValue) {
    FastAttribKey tSaveKey = sNormalKeyToSaveKey(paAttribsToSave, uAttribsToSaveCount, tKey);
    for (int32 i = 0; i < tSavedAttributes.attributes_size(); ++i) {
        const D3::AttributeSerializer::SavedAttribute & tAttribute = tSavedAttributes.attributes(i);
        if (tAttribute.key() == tSaveKey.nValue) return tAttribute.value();
    }
    return nDefaultValue;
}

// Original line 385.
BOOL AttributeSerializer_OverwriteInt(const SavedAttribDef *paAttribsToSave,
                                      const uint32 uAttribsToSaveCount,
                                      D3::AttributeSerializer::SavedAttributes &tSavedAttributes,
                                      const FastAttribKey &tKey, const int32 nValue) {
    FastAttribKey tSaveKey = sNormalKeyToSaveKey(paAttribsToSave, uAttribsToSaveCount, tKey);
    for (int i = 0; i < tSavedAttributes.attributes_size(); ++i) {
        D3::AttributeSerializer::SavedAttribute *ptAttribute = tSavedAttributes.mutable_attributes(i);
        if (ptAttribute->key() == tSaveKey.nValue) {
            ptAttribute->set_value(nValue);
            return TRUE;
        }
    }
    return FALSE;
}

// Original line 410.
void AttributeSerializer_WriteInt(const SavedAttribDef *paAttribsToSave,
                                  const uint32 uAttribsToSaveCount,
                                  D3::AttributeSerializer::SavedAttributes &tSavedAttributes,
                                  const FastAttribKey &tKey, const int32 nValue) {
    FastAttribKey tSaveKey = sNormalKeyToSaveKey(paAttribsToSave, uAttribsToSaveCount, tKey);
    D3::AttributeSerializer::SavedAttribute *ptAttribute;
    for (int i = 0; i < tSavedAttributes.attributes_size(); ++i) {
        ptAttribute = tSavedAttributes.mutable_attributes(i);
        if (ptAttribute->key() == tSaveKey.nValue) {
            ptAttribute->set_value(nValue);
            return;
        }
    }
    ptAttribute = tSavedAttributes.mutable_attributes()->Add();
    ptAttribute->set_key(tSaveKey.nValue);
    ptAttribute->set_value(nValue);
}

// Original line 437.
void AttributeSerializer_RemoveInt(const SavedAttribDef *paAttribsToSave,
                                   const uint32 uAttribsToSaveCount,
                                   D3::AttributeSerializer::SavedAttributes &tSavedAttributes,
                                   const FastAttribKey &tKey) {
    FastAttribKey tSaveKey = sNormalKeyToSaveKey(paAttribsToSave, uAttribsToSaveCount, tKey);
    for (int i = 0; i < tSavedAttributes.attributes_size(); ++i) {
        if (tSavedAttributes.mutable_attributes(i)->key() == tSaveKey.nValue) {
            const int nLast = tSavedAttributes.attributes_size() - 1;
            if (i != nLast)
                tSavedAttributes.mutable_attributes()->SwapElements(i, nLast);
            tSavedAttributes.mutable_attributes()->RemoveLast();
            return;
        }
    }
}

// Original line 467.
uint64 AttributeSerializer_GetUInt64(const SavedAttribDef * paAttribsToSave, uint32 uAttribsToSaveCount, const D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKeyLo, const FastAttribKey & tKeyHi) {
    uint32 uLo = AttributeSerializer_GetInt(paAttribsToSave, uAttribsToSaveCount, tSavedAttributes, tKeyLo, 0);
    uint32 uHi = AttributeSerializer_GetInt(paAttribsToSave, uAttribsToSaveCount, tSavedAttributes, tKeyHi, 0);
    uint64 uValue = ((uint64)uHi << 32) | uLo;
    return uValue;
}

// Original line 493.
void AttributeSerializer_WriteUInt64(const SavedAttribDef *paAttribsToSave,
                                     const uint32 uAttribsToSaveCount,
                                     D3::AttributeSerializer::SavedAttributes &tSavedAttributes,
                                     const FastAttribKey &tKeyLo, const FastAttribKey &tKeyHi,
                                     const uint64 uValue) {
    uint32 uHi = (uint32)(uValue >> 32);
    uint32 uLo = (uint32)uValue;
    AttributeSerializer_WriteInt(paAttribsToSave, uAttribsToSaveCount, tSavedAttributes, tKeyLo,
                                 (int32)uLo);
    AttributeSerializer_WriteInt(paAttribsToSave, uAttribsToSaveCount, tSavedAttributes, tKeyHi,
                                 (int32)uHi);
}
