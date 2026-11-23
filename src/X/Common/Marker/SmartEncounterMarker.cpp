// Original file: X/Common/Marker/SmartEncounterMarker.cpp
#include "SmartEncounterMarker.h"
#include "Marker.h"
#include "../Attributes/FastAttribute.h"
#include "../Definitions/EncounterDefinition.h"
#include "../Definitions/QuestRangeDefinition.h"
#include "../Hyperlink.h"
#include "../SceneCommon.h"
#include "../TagMaps/Tags.h"
#include "../TagMaps/MarkerTags.h"
#include "../../Server/Actors/Actors.h"
#include "../../Server/SGame.h"

// DWARF declarations: ActorCommonData.cpp:7998, SceneCommon.cpp:1316, Actors.cpp:5723.
BOOL ACDGroupExists(uint32 uGroupHash);
LevelAreaID ScenesCommonLevelAreaIDFromPlace(SSceneID idSScene, Vector3D wp);
void EncounterAddByLevelArea(SNO snoEncounter, ActorAddStruct & tActorAdd);

#line 37
SNO EncounterMarker::GetActorForEdit(SNO snoSource)
{
#line 40
    ScopedEncounterDefinitionLockR pdefEncounter(snoSource);
    return pdefEncounter->snoSymbol;
}
#line 44
void EncounterMarker::GetAABB(const Marker & tMarker, AABB & aabbBounds)
{
#line 48
    float flScale = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_SCALE);
    PRTransform transform = gtransformMarkerToScene * tMarker.transform;

    ScopedEncounterDefinitionLockR pdefEncounter(tMarker.snoname.snoHandle);
    SNO snoSymbol = pdefEncounter->snoSymbol;
    GetActorAABB(snoSymbol, transform, flScale, aabbBounds);
}
#line 57
DataID EncounterMarker::CreateForServerGame(
    const SSceneID idSScene,
    const WorldPlace & placePosition,
    const Quaternion & qOrientation,
    const Marker & tMarker,
    const InactiveMarkerOwner & tMarkerOwner)
{
    // Original tests bit zero of dwFlags; no named mask is recorded in this CU.
#line 64
    if (SGameGlobalsGet().dwFlags & 1)
    {
        return DATAID_INVALID;
    }
#line 71
    uint32 dwAddFlags = (1 << ACTORADD_IS_FROM_MARKER);

    if (!TagMapGetInt(tMarker.hTagMap, TAG_MARKER_PERTURB))
    {
        dwAddFlags |= (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    }
#line 80
    if (TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_AI_DISABLED)) dwAddFlags |= (1 << ACTORADD_DISABLE_BRAIN_BIT);
    if (TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_AI_NO_DAMAGE)) dwAddFlags |= (1 << ACTORADD_NO_DAMAGE);
    if (TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_AI_DOES_FAKE_DAMAGE)) dwAddFlags |= (1 << ACTORADD_DOES_FAKE_DAMAGE);
    dwAddFlags |= (1 << ACTORADD_KEEP_MARKER_NAME);
    // Original line 84 explicitly clears this bit on the false path (AND 0xffefffff).
#line 84
    if (TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_GIZMO_DOOR_OPEN))
        dwAddFlags |= (1 << ACTORADD_GIZMO_DOOR_OPEN);
    else
        dwAddFlags &= ~(1 << ACTORADD_GIZMO_DOOR_OPEN);
#line 87
    SNO snoQuestRangeOpen = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_GIZMO_DOOR_OPEN_ON_QUESTRANGE);
    if (snoQuestRangeOpen != SNO_NONE && QuestRangeEvaluate(snoQuestRangeOpen, NULL))
    {
        dwAddFlags |= (1 << ACTORADD_GIZMO_DOOR_OPEN);
    }
#line 93
    MonsterRarity eMonsterRarity = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_MONSTER_RARITY);

    LevelAreaID idLevelArea = ScenesCommonLevelAreaIDFromPlace(idSScene, placePosition.wp);
    SNO snoConvList = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_CONVERSATION_LIST);
    uint32 uPrimaryGroup = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_PRIMARY_GROUP);
    uint32 uSecondaryGroup = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_SECONDARY_GROUP);

    if (uPrimaryGroup != (uint32)-1 && !ACDGroupExists(uPrimaryGroup))
    {
        CRefString sMarkerSet = HyperlinkCreateFromSNO(SNOName(MARKERSET_SNO, tMarkerOwner.snoMarkerSet), NULL);
        uPrimaryGroup = (uint32)-1;
    }
#line 114
    if (uSecondaryGroup != (uint32)-1 && !ACDGroupExists(uSecondaryGroup))
    {
        CRefString sMarkerSet = HyperlinkCreateFromSNO(SNOName(MARKERSET_SNO, tMarkerOwner.snoMarkerSet), NULL);
        uSecondaryGroup = (uint32)-1;
    }
#line 128
    if (!MarkerCheckReference(tMarkerOwner.snoMarkerSet, tMarkerOwner.nMarkerIndex, tMarker, CONVERSATIONLIST_SNO, snoConvList))
    {
        snoConvList = SNO_NONE;
    }
#line 133
    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = dwAddFlags;
    tActorAdd.eFastAttribType = FAT_SIMPLE;
    tActorAdd.tPlace = placePosition;
    tActorAdd.qOrientation = qOrientation;
    tActorAdd.flScale = 0.0f;
    tActorAdd.plistActors = NULL;
    tActorAdd.idMarkersSScene = idSScene;
    tActorAdd.tMarkersName = tMarker.dwHash;
    tActorAdd.snoEncounterSource = tMarker.snoname.snoHandle;
    tActorAdd.tMarkerOwner = tMarkerOwner;
    tActorAdd.idLevelArea = idLevelArea;
    tActorAdd.eMonsterRarity = eMonsterRarity;

    SNO snoScript = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_SCRIPT);
    if (snoScript != SNO_NONE &&
        MarkerCheckReference(tMarkerOwner.snoMarkerSet, tMarkerOwner.nMarkerIndex, tMarker, SCRIPT_SNO, snoScript))
    {
        tActorAdd.listSNOScripts.AddTail_Tracked(snoScript);
    }
    for (uint32 i = 0; i < 9; i++)
    {
        snoScript = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_SCRIPT_2 + i);
        if (snoScript != SNO_NONE &&
            MarkerCheckReference(tMarkerOwner.snoMarkerSet, tMarkerOwner.nMarkerIndex, tMarker, SCRIPT_SNO, snoScript))
        {
            tActorAdd.listSNOScripts.AddTail_Tracked(snoScript);
        }
    }
#line 164
    tActorAdd.snoConvList = snoConvList;
    tActorAdd.uPrimaryGroup = uPrimaryGroup;
    tActorAdd.uSecondaryGroup = uSecondaryGroup;

    EncounterAddByLevelArea(tMarker.snoname.snoHandle, tActorAdd);

    return DATAID_INVALID;
}
