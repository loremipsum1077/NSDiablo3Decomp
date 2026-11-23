// Original file: X/Common/TagMaps/TagNumbers.cpp
#include "TagNumbers.h"
// Original include closure places blz::ignore ahead of SNOName/WorldPlace statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "Tags.h"
#include "../GlobalSNO.h"
#include "../../Client/ImageCollection.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include <strings.h>
#include "ActorTags.h"
#include "PowerTags.h"
#include "ShaderMapTags.h"
#include "VertexShaderTags.h"
#include "SurfaceTags.h"
#include "MarkerTags.h"
#include "AppearanceTags.h"
#include "AnimTags.h"
#include "../Definitions/WorldDefinition.h"
#include "MonsterTags.h"

// Original lines 40-50. Scalar/pointer values from original TAG_VALUE_* symbols;
// formula strings from the line-attributed original static initializer.
// tools/data.py: TAG_VALUE_ZERO 0x7100e57d5c = 00000000, INT_ONE 0x7100e57d60 = 01000000,
// FLOAT_ONE 0x7100e57d64 = 0000803f, FLOAT_HALF 0x7100e57d68 = 0000003f,
// NONE 0x7100e57d6c = ffffffff, NULL_SNO 0x710108d630 -> 0x7100d6a4a3 ("").
extern const int32 TAG_VALUE_ZERO = 0;
extern const int32 TAG_VALUE_INT_ONE = 1;
extern const float TAG_VALUE_FLOAT_ONE = 1.0f;
extern const float TAG_VALUE_FLOAT_HALF = 0.5f;
extern const int32 TAG_VALUE_NONE = -1;
extern const char * const TAG_VALUE_NULL_SNO = "";
extern const char * const TAG_VALUE_NULL_GBID = "";
extern const StringFormulaDefault TAG_VALUE_FORMULA_NULL("");
extern const StringFormulaDefault TAG_VALUE_FORMULA_ZERO("0");
extern const StringFormulaDefault TAG_VALUE_FORMULA_ONE("1");
extern const StringFormulaDefault TAG_VALUE_FORMULA_NEG_ONE("-1");

// SNO group enumerators come from SNOFiles.h:130.
// Original line 99.
SNOGroup AxeTagTypeToSNOGroup(const AxeTagType eType) {
    switch (eType) {
    case AXETAGTYPE_ANIM: return ANIMATION_SNO;
    case AXETAGTYPE_SHADER: return SHADER_SNO;
    case AXETAGTYPE_ACTOR: return ACTOR_SNO;
    case AXETAGTYPE_STRINGLIST: return STRINGLIST_SNO;
    case AXETAGTYPE_SOUND: return SOUND_SNO;
    case AXETAGTYPE_SOUNDBANK: return SOUNDBANK_SNO;
    case AXETAGTYPE_AMBIENT_SOUND: return AMBIENTSOUND_SNO;
    case AXETAGTYPE_POWER: return POWER_SNO;
    case AXETAGTYPE_PARTICLE: return PARTICLE_SNO;
    case AXETAGTYPE_TEXTURE: return TEXTURE_SNO;
    case AXETAGTYPE_EFFECTGROUP: return EFFECTGROUP_SNO;
    case AXETAGTYPE_WORLD: return WORLD_SNO;
    case AXETAGTYPE_CLOTH: return CLOTH_SNO;
    case AXETAGTYPE_ROPE: return ROPE_SNO;
    case AXETAGTYPE_MONSTER: return MONSTER_SNO;
    case AXETAGTYPE_SNO_LEVEL_AREA: return LEVELAREA_SNO;
    case AXETAGTYPE_WEATHER: return WEATHER_SNO;
    case AXETAGTYPE_SHAKE: return SHAKE_SNO;
    case AXETAGTYPE_ANIM2D_SNO: return ANIMATION2D_SNO;
    case AXETAGTYPE_ENCOUNTER: return ENCOUNTER_SNO;
    case AXETAGTYPE_SCRIPT: return SCRIPT_SNO;
    case AXETAGTYPE_CONVERSATION: return CONVERSATION_SNO;
    case AXETAGTYPE_CONVERSATIONLIST: return CONVERSATIONLIST_SNO;
    case AXETAGTYPE_TREASURECLASS: return TREASURECLASS_SNO;
    case AXETAGTYPE_OBSERVER: return OBSERVER_SNO;
    case AXETAGTYPE_LORE: return LORE_SNO;
    case AXETAGTYPE_AISTATE: return AISTATE_SNO;
    case AXETAGTYPE_CONDITION: return CONDITION_SNO;
    case AXETAGTYPE_CONDUCTOR: return CONDUCTOR_SNO;
    case AXETAGTYPE_TIMEDEVENT: return TIMEDEVENT_SNO;
    case AXETAGTYPE_QUESTRANGE: return QUESTRANGE_SNO;
    case AXETAGTYPE_SURFACE: return SURFACE_SNO;
    case AXETAGTYPE_REVERB: return REVERB_SNO;
    case AXETAGTYPE_MUSIC: return MUSIC_SNO;
    case AXETAGTYPE_SNO_BOSS_ENCOUNTER: return BOSS_ENCOUNTER_SNO;
    case AXETAGTYPE_VIBRATION: return VIBRATION_SNO;
    case AXETAGTYPE_SNO_QUEST: return QUEST_SNO;
    default: return SNOGROUP_NONE;
    }
}

// Original line 223.
BOOL AxeTagGetSNOName(const CTagMap * pTagMap, const uint32 & dwTag, SNOName & snoname) {
    snoname.Set(SNOGROUP_NONE, -1);
    const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwTag);
    snoname.snoGroup = AxeTagTypeReferencesSNOGroup(ptInfo->eType);
    if (snoname.snoGroup == SNOGROUP_NONE) return FALSE;
    switch (ptInfo->eType) {
    case AXETAGTYPE_GIZMO_LABEL:
    case AXETAGTYPE_GAME_ERROR_LABEL:
    case AXETAGTYPE_MINIMAP_NAME_LABEL:
    case AXETAGTYPE_SET_DUNGEON_DESCRIPTION_LABEL:
    case AXETAGTYPE_SET_DUNGEON_TITLE_LABEL:
        snoname.snoHandle = AxeTagToStringListSNO(ptInfo->eType);
        break;
    case AXETAGTYPE_ITEM_IMAGE:
    case AXETAGTYPE_MAP_IMAGE:
    case AXETAGTYPE_UI_IMAGE: {
        uint32 dwTagValue = TagMapGetDWORD(pTagMap, dwTag);
        ImageCollectionType eCollection = IMAGE_COLLECTION_INVALID;
        switch (ptInfo->eType) {
        case AXETAGTYPE_ITEM_IMAGE: eCollection = IMAGE_COLLECTION_ITEM; break;
        case AXETAGTYPE_MAP_IMAGE: eCollection = IMAGE_COLLECTION_MAP; break;
        case AXETAGTYPE_UI_IMAGE: eCollection = IMAGE_COLLECTION_UI; break;
        default: break;
        }
        ImageTextureFrame tImageTexFrame;
        ImageCollectionGetImageInfo(eCollection, dwTagValue, tImageTexFrame);
        snoname.snoHandle = tImageTexFrame.snoTexture;
        break;
    }
    default: snoname.snoHandle = TagMapGetSNO(pTagMap, dwTag); break;
    }
    return snoname.snoHandle != -1;
}
// Original line 301.
SNOGroup AxeTagTypeReferencesSNOGroup(const AxeTagType eType) {
    SNOGroup snogroup = AxeTagTypeToSNOGroup(eType);
    if (snogroup != SNOGROUP_NONE) return snogroup;
    switch (eType) {
    case AXETAGTYPE_GIZMO_LABEL:
    case AXETAGTYPE_GAME_ERROR_LABEL:
    case AXETAGTYPE_MINIMAP_NAME_LABEL:
    case AXETAGTYPE_SET_DUNGEON_DESCRIPTION_LABEL:
    case AXETAGTYPE_SET_DUNGEON_TITLE_LABEL: return STRINGLIST_SNO;
    case AXETAGTYPE_ITEM_IMAGE:
    case AXETAGTYPE_MAP_IMAGE:
    case AXETAGTYPE_UI_IMAGE: return TEXTURE_SNO;
    default: return SNOGROUP_NONE;
    case AXETAGTYPE_SNO_LEVEL_AREA: return LEVELAREA_SNO;
    case AXETAGTYPE_SNO_BOSS_ENCOUNTER: return BOSS_ENCOUNTER_SNO;
    case AXETAGTYPE_SNO_QUEST: return QUEST_SNO;
    }
}
// Original line 349.
GameBalanceType AxeTagTypeToGBType(const AxeTagType eType) {
    switch (eType) {
    case AXETAGTYPE_GBID_ITEMTYPE: return 1; // GB_ITEMTYPES, GameBalanceDefinition.h:34.
    case AXETAGTYPE_GBID_ITEM: return 2; // GB_ITEMS, GameBalanceDefinition.h:34.
    case AXETAGTYPE_GBID_MONSTERAFFIX: return 18; // GB_MONSTER_AFFIXES, GameBalanceDefinition.h:34.
    default: return -1;
    }
}
// Original line 370.
const AxeTagInfo * GetTagTypeInfo(const TagBases dwTagBase, int32 & nCount) {
    switch (dwTagBase) {
    case TAG_ACTOR: nCount = gnActorTagCount; return tActorTagInfo;
    case TAG_SHADERMAP: nCount = gnShaderMapTagCount; return tShaderMapTagInfo;
    case TAG_POWER_TUNING: nCount = gnPowerTuningTagCount; return tPowerTuningTagInfo;
    case TAG_SHADEROPTION: nCount = gnShaderOptionTagCount; return tShaderOptionTagInfo;
    case TAG_SURFACE: nCount = gnSurfaceTagCount; return tSurfaceTagInfo;
    case TAG_MARKER: nCount = gnMarkerTagCount; return tMarkerTagInfo;
    case TAG_APPEARANCE: nCount = gnAppearanceTagCount; return tAppearanceTagInfo;
    case TAG_APPEARANCE_STATE: nCount = gnAppearanceStateTagCount; return tAppearanceStateTagInfo;
    case TAG_ANIMATION: nCount = gnAnimTagCount; return tAnimTagInfo;
    case TAG_WORLD_GROUP: nCount = gnWorldLayoutGroupTagCount; return tWorldLayoutGroupTagInfo;
    case TAG_WORLD_TILE: nCount = gnWorldTileTagCount; return tWorldTileTagInfo;
    case TAG_WORLD_DRLG_COMMAND: nCount = gnWorldLayoutCommandTagCount; return tWorldLayoutCommandTagInfo;
    case TAG_MONSTER: nCount = gnMonsterTagCount; return tMonsterTagInfo;
    case TAG_POWER_GENERAL: nCount = gnPowerGeneralTagCount; return tPowerGeneralTagInfo;
    default: nCount = 0; return NULL;
    }
}
// Original line 436.
static XGrowableMap<CRefString, const CRefString &, const AxeTagInfo *, const AxeTagInfo *> s_TagXMLNameToInfoMap(16, NULL);

// Original line 501.
uint32 TagNameToTag(const CRefString & sTag, TagBases eTagBase) {
    int32 nCount;
    const AxeTagInfo * pTagInfo = GetTagTypeInfo(eTagBase, nCount);
    for (int32 i = 0; i < nCount; ++i) {
        const AxeTagInfo & tTagInfo = pTagInfo[i];
        if (strcasecmp(sTag, tTagInfo.szName) == 0) return tTagInfo.dwTagId;
    }
    return (uint32)-1;
}
// Original line 624. Body recovered from its inline site in AxeTagGetSNOName.
inline SNO AxeTagToStringListSNO(const AxeTagType eTagType) {
    switch (eTagType) {
    case AXETAGTYPE_GIZMO_LABEL: return GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS);
    case AXETAGTYPE_GAME_ERROR_LABEL: return GlobalSNOGet(GLOBALSNO_STRINGLIST_GAME_ERRORS);
    case AXETAGTYPE_MINIMAP_NAME_LABEL: return GlobalSNOGet(GLOBALSNO_STRINGLIST_MINIMAP);
    case AXETAGTYPE_SET_DUNGEON_DESCRIPTION_LABEL: return GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_DESCRIPTIONS);
    case AXETAGTYPE_SET_DUNGEON_TITLE_LABEL: return GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_TITLES);
    default: return -1;
    }
}
