// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Attributes/AttributeSerializer.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "FastAttribConstantsSwig.h"
#include "FastAttributeStructs.h"
#include "../GameBalance.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"

struct ActorSavedAttribute {  // size 0x8
    FastAttribKey tKey;  // 0x00
    FastAttribValue tValue;  // 0x04
};
static_assert(sizeof(ActorSavedAttribute) == 0x8, "ActorSavedAttribute");

struct ActorSavedAttributes {  // size 0x20
    GBHandle hGB;  // 0x00
    int32 nAttributes;  // 0x08
    SerializeData serAttributes;  // 0x0c
    XPtr64<ActorSavedAttribute> ptAttributes;  // 0x18
};
static_assert(sizeof(ActorSavedAttributes) == 0x20, "ActorSavedAttributes");

struct SavedAttribDef {  // size 0x8
    int32 nSavedAttribId;  // 0x00
    Attrib eAttrib;  // 0x04
};
static_assert(sizeof(SavedAttribDef) == 0x8, "SavedAttribDef");


struct ActorCommonData;
namespace D3 { namespace AttributeSerializer { class SavedAttributes; } }
void AttributeSerializer_CheckAndMendHellfireExploit(ActorCommonData & tACD, BOOL bIsRestore);
void AttributeSerializer_Save(const SavedAttribDef * paAttribsToSave, uint32 uAttribsToSaveCount, ActorCommonData & tACD, D3::AttributeSerializer::SavedAttributes & tSavedAttributes);
void AttributeSerializer_Restore(const SavedAttribDef * paAttribsToSave, uint32 uAttribsToSaveCount, const D3::AttributeSerializer::SavedAttributes & tSavedAttributes, ActorCommonData & tACD);
int32 AttributeSerializer_GetInt(const SavedAttribDef * paAttribsToSave, uint32 uAttribsToSaveCount, const D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKey, int32 nDefaultValue);
BOOL AttributeSerializer_OverwriteInt(const SavedAttribDef *paAttribsToSave,
                                      const uint32 uAttribsToSaveCount,
                                      D3::AttributeSerializer::SavedAttributes &tSavedAttributes,
                                      const FastAttribKey &tKey, const int32 nValue);
void AttributeSerializer_WriteInt(const SavedAttribDef *paAttribsToSave,
                                  const uint32 uAttribsToSaveCount,
                                  D3::AttributeSerializer::SavedAttributes &tSavedAttributes,
                                  const FastAttribKey &tKey, const int32 nValue);
void AttributeSerializer_RemoveInt(const SavedAttribDef *paAttribsToSave,
                                   const uint32 uAttribsToSaveCount,
                                   D3::AttributeSerializer::SavedAttributes &tSavedAttributes,
                                   const FastAttribKey &tKey);
uint64 AttributeSerializer_GetUInt64(const SavedAttribDef * paAttribsToSave, uint32 uAttribsToSaveCount, const D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKeyLo, const FastAttribKey & tKeyHi);
void AttributeSerializer_WriteUInt64(const SavedAttribDef *paAttribsToSave,
                                     const uint32 uAttribsToSaveCount,
                                     D3::AttributeSerializer::SavedAttributes &tSavedAttributes,
                                     const FastAttribKey &tKeyLo, const FastAttribKey &tKeyHi,
                                     const uint64 uValue);
