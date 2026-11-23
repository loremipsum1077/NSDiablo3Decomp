#include "GizmoLocs.h"
#include "SNO/SNOFiles.h"
#include "CommonTypes.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/RefString.h"
// DWARF: X/Common/Quests/Quests.h:153.
struct QuestInstance;
QuestInstance * QuestsGetQuestInstance(SNO snoQuest, ACDID idACDPlayer);

#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/Random.h"
// DWARF SWorld.h:69 and SWorld.cpp:302.
typedef XGrowableMap<int, int, int, int> TGizmoLocSpawnMap;
TGizmoLocSpawnMap & SWorldGetGizmoLocSpawnMap();
// DWARF ConditionDefinition.cpp:640 and SQuests.cpp:5410, 5476, 5510.
struct Player;
struct LooteeInfo;
BOOL ConditionEvaluate(SNO snoCondition, const Player * ptPlayer, ACDID idACD, const LooteeInfo * ptLooteeInfo);
BOOL SQuestsActorIsRequiredByBounty(SNO snoActor);
BOOL SQuestsAdventureIsRequiredByBounty(SNO snoAdventure);
BOOL SQuestsEncounterIsRequiredByBounty(SNO snoEncounter);
BOOL GameIsOpenWorld();

#include "SceneCommon.h"
#include "../Server/SGame.h"
#include "../Server/Actors/Actors.h"
#include "Definitions/ActorDefinition.h"
#include "TagMaps/ActorTags.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
// DWARF: Adventure.cpp:297, EncounterDefinition.cpp:384, SceneCommon.cpp:1620,
// SWorld.cpp:2086; Actors.cpp:5803, 5847, 5865.
BOOL AdventureSpawn(SNO snoAdventure, LPCSTR szAdventureName, const PRTransform & transform, SceneCommonData & tSCD, const PRTransform & transformSceneToWorld);
BOOL EncounterResolveToActorSpawnInfo(SNO snoEncounter, RNG & rng, SNOName & snoname, Team & eTeam);
void SceneGetSceneToWorldTransform(SSceneID idSScene, PRTransform & transform);
int32 SWorldGetRandSeedFromSSceneID(SWorldID idSWorld, SSceneID idSScene);
ActorID GizmoAddBySNO(int32 nLevel, ActorAddStruct & tActorAdd);
ActorID PropAddBySNO(ActorAddStruct & tActorAdd);
ActorID SceneryAdd(const ActorAddStruct & tActorAdd);
// DWARF XFilesCommon.cpp, D3DescribedType.cpp descriptor declarations.
extern const XTypeDescriptor * DT_SERIALIZEDATA;
extern const XTypeDescriptor * DT_SNO;
extern const XTypeDescriptor * DT_SNO_NAME;
enum { GIZMOLOC_SPAWN_ENTRY_XML_VERSION = 4 };

#include "D3TypeDescriptor/D3TypeDescriptor.h"
#include "TagMaps/Tags.h"
#include "../Client/Gfx/GfxVB.h"




// Field constructor values recovered from the original initializer; field labels are empty.
static const TypeDescriptorField GizmoLocSpawnChoice_Fields[5] = {
    TypeDescriptorField("", DT_SNO_NAME, offsetof(GizmoLocSpawnChoice, snonameToSpawn), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(GizmoLocSpawnChoice, bForceRandomFacing), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(GizmoLocSpawnChoice, flWeight), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(GizmoLocSpawnChoice, nMaxTimesPicked), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(GizmoLocSpawnChoice), NULL, 0)
};

ComplexTypeDescriptor TypeDescriptor_GizmoLocSpawnChoice("GizmoLocSpawnChoice", 0, GizmoLocSpawnChoice_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GIZMOLOC_SPAWN_CHOICE = &TypeDescriptor_GizmoLocSpawnChoice;

static const TypeDescriptorField GizmoLocSpawnEntry_Fields[9] = {
    TypeDescriptorField("", DT_INT, offsetof(GizmoLocSpawnEntry, nSpawnMin), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(GizmoLocSpawnEntry, nSpawnMax), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(GizmoLocSpawnEntry, snoRequiredQuest), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(GizmoLocSpawnEntry, snoCondition), "", CONDITION_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(GizmoLocSpawnEntry, dwFlags), NULL, 1), TypeDescriptorField("", DT_HIGHPRECISIONPERCENT, offsetof(GizmoLocSpawnEntry, flNoSpawnChance), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(GizmoLocSpawnEntry, serChoice), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_GIZMOLOC_SPAWN_CHOICE, offsetof(GizmoLocSpawnEntry, arChoice), 0x21, 8), TypeDescriptorField(NULL, DT_NULL, sizeof(GizmoLocSpawnEntry), NULL, 0)
};

ComplexTypeDescriptor TypeDescriptor_GizmoLocSpawnEntry("GizmoLocSpawnEntry", GIZMOLOC_SPAWN_ENTRY_XML_VERSION, GizmoLocSpawnEntry_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GIZMOLOC_SPAWN_ENTRY = &TypeDescriptor_GizmoLocSpawnEntry;

static const TypeDescriptorField GizmoLocSpawnType_Fields[3] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(GizmoLocSpawnType, serEntry), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_GIZMOLOC_SPAWN_ENTRY, offsetof(GizmoLocSpawnType, arEntry), 0x21, 8), TypeDescriptorField(NULL, DT_NULL, sizeof(GizmoLocSpawnType), NULL, 0)
};

ComplexTypeDescriptor TypeDescriptor_GizmoLocSpawnType("GizmoLocSpawnType", 0, GizmoLocSpawnType_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GIZMOLOC_SPAWN_TYPE = &TypeDescriptor_GizmoLocSpawnType;

static const TypeDescriptorField GizmoLocSet_Fields[2] = { FixedArrayTypeDescriptorField("", DT_GIZMOLOC_SPAWN_TYPE, offsetof(GizmoLocSet, arSpawnArray), NULL, 1, 52, 0), TypeDescriptorField(NULL, DT_NULL, sizeof(GizmoLocSet), NULL, 0)
};
ComplexTypeDescriptor TypeDescriptor_GizmoLocSet("GizmoLocSet", 0, GizmoLocSet_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GIZMOLOC_SET = &TypeDescriptor_GizmoLocSet;







































































































































void GizmoLocSpawnEntry::Initialize()

{
    SigmaMemoryClear(this, sizeof(*this));

    nSpawnMin = 0;
    nSpawnMax = 0;
    snoRequiredQuest = SNO_NONE;
    dwFlags = 0;
    flNoSpawnChance = 0.0f;
}



























static int32 sGetChoiceMapKey(int32 nEntryIdx, int32 nChoiceIdx)
{

    // Inferred key expression from original shift-by-16 and bitwise OR.
    int32 nKey = (nEntryIdx << 16) | nChoiceIdx;
    return nKey;
}




static BOOL sCanSpawnChoice(const GizmoLocSpawnChoice & tChoice, int32 nEntryIdx, int32 nChoiceIdx)
{




    TGizmoLocSpawnMap & mapSpawns = SWorldGetGizmoLocSpawnMap();
    if (!(tChoice.flWeight > 0.0f))
        return FALSE;
    if (tChoice.snonameToSpawn.snoHandle == SNO_NONE)
        return FALSE;

    if (nEntryIdx < 0)
        return TRUE;



    if (tChoice.nMaxTimesPicked == 0)
        return FALSE;
    if (tChoice.nMaxTimesPicked < 0)
        return TRUE;



    {

        int32 nStoredCount;
        BOOL bFound = mapSpawns.Lookup(sGetChoiceMapKey(nEntryIdx, nChoiceIdx), nStoredCount);
        if (bFound && nStoredCount >= tChoice.nMaxTimesPicked)
            return FALSE;
    }
    return TRUE;
}












// NON_MATCHING: 1280 vs 1276; extra null test of ptChoice. Columns 393:23/393:43 say `ptChoice->nMaxTimesPicked > 0 && nEntryIdx >= 0` with no null test, but that form restructures the loops (1296).
GizmoLocSpawnChoice * GizmoLocSpawnEntry::RollChoice(RNG & rng, int32 nEntryIdx) const


{

    if (snoCondition != SNO_NONE)
    {
        if (!ConditionEvaluate(snoCondition, NULL, ACDID_INVALID, NULL))
            return NULL;
    }


    // Inferred defaults: empty/failed selection returns NULL; selected index feeds the spawn-count key.
    GizmoLocSpawnChoice * ptChoice = NULL;
    int32 nNumChoices = serChoice.dwSize / sizeof(GizmoLocSpawnChoice);
    if (nNumChoices > 0)
    {
        float flWeightTotal = 0.0f;
        int32 nIndex = -1;

        for (int32 i = 0; i < nNumChoices; ++i)
        {
            GizmoLocSpawnChoice & tChoice = arChoice[i];
            if (sCanSpawnChoice(tChoice, nEntryIdx, i))
            {
                flWeightTotal += tChoice.flWeight;

                if (GameIsOpenWorld())
                {
                    if (tChoice.snonameToSpawn.snoGroup == ACTOR_SNO &&
                        SQuestsActorIsRequiredByBounty(tChoice.snonameToSpawn.snoHandle))
                    {
                        ptChoice = &tChoice;
                        nIndex = i;
                        break;
                    }
                    if (tChoice.snonameToSpawn.snoGroup == ADVENTURE_SNO &&
                        SQuestsAdventureIsRequiredByBounty(tChoice.snonameToSpawn.snoHandle))
                    {
                        ptChoice = &tChoice;
                        nIndex = i;
                        break;
                    }
                    if (tChoice.snonameToSpawn.snoGroup == ENCOUNTER_SNO &&
                        SQuestsEncounterIsRequiredByBounty(tChoice.snonameToSpawn.snoHandle))
                    {
                        ptChoice = &tChoice;
                        nIndex = i;
                        break;
                    }
                }
            }
        }

        if (!ptChoice && flWeightTotal > 0.0f)
        {


            if (RandFloat_Internal(rng) < flNoSpawnChance)
                return NULL;



            float flRandomValue = RandRangeFloat_Internal(rng, 0.0f, flWeightTotal);
            float flCurrentWeight = 0.0f;
            for (int32 i = 0; i < nNumChoices; ++i)
            {
                GizmoLocSpawnChoice & tChoice = arChoice[i];
                if (sCanSpawnChoice(tChoice, nEntryIdx, i))
                {
                    flCurrentWeight += tChoice.flWeight;
                    if (flRandomValue < flCurrentWeight)
                    {
                        ptChoice = &tChoice;
                        nIndex = i;
                        break;
                    }
                }
            }
        }
        if (ptChoice)
        {   // inferred: NULL paths are threaded straight to the return; 393 has columns 23 (member) and 43 (&&)
            if (ptChoice->nMaxTimesPicked > 0 && nEntryIdx >= 0)
            {


            TGizmoLocSpawnMap & mapSpawns = SWorldGetGizmoLocSpawnMap();
            int32 nKey = sGetChoiceMapKey(nEntryIdx, nIndex);
            int32 nCount = 0;
            mapSpawns.Lookup(nKey, nCount);
            ++nCount;
            mapSpawns.SetAt_Tracked(nKey, nCount);
            }
        }
    }






    return ptChoice;
}



int32 GizmoLocSpawnEntry::GetNumValidChoices() const

{
    int32 nNumValidChoices = 0;

    if (snoRequiredQuest != SNO_NONE)
    {
        if (!QuestsGetQuestInstance(snoRequiredQuest, ACDID_INVALID))
            return 0;
    }



    int32 nNumChoices = serChoice.dwSize / sizeof(GizmoLocSpawnChoice);
    for (int32 i = 0; i < nNumChoices; ++i)
    {
        GizmoLocSpawnChoice & tChoice = arChoice[i];


        if (tChoice.flWeight > 0.0f &&
            tChoice.snonameToSpawn.snoHandle != SNO_NONE)
        {
            ++nNumValidChoices;
        }
    }

    return nNumValidChoices;
}














































void GizmoLocPlaceGizmos(SceneCommonData & tSCD)

{


    if (GameActiveGameCommonDataIsClient())
        return;





    int32 nOldSeed = RandGetSeed(SGameGlobalsGet().rng);
    int32 nSeed = SWorldGetRandSeedFromSSceneID(tSCD.idSWorld, tSCD.idSScene);
    RandSetSeed_Internal(SGameGlobalsGet().rng, nSeed);

    PRTransform transformSceneToWorld;
    SceneGetSceneToWorldTransform(tSCD.idSScene, transformSceneToWorld);

    POS pos = tSCD.listGizmoLocPlacements.GetHeadPosition();
    while (pos)
    {
        GizmoLocPlacement & tPlacement = tSCD.listGizmoLocPlacements.GetNext(pos);





























        SNOName snoname = tPlacement.snoname;
        if (snoname.snoGroup == ADVENTURE_SNO)
        {
            AdventureSpawn(snoname.snoHandle, SNOToString(snoname, FALSE).str(), tPlacement.tPlace.transform, tSCD, transformSceneToWorld);
        }
        else
        {
            ActorAddStruct tActorAdd;
            if (snoname.snoGroup == ENCOUNTER_SNO)
            {
                BOOL bSpawn = EncounterResolveToActorSpawnInfo(snoname.snoHandle, SGameGlobalsGet().rng, snoname, tActorAdd.eTeam);
                if (!bSpawn)
                    continue;
            }
            else
            {


                ScopedActorDefinitionLockR pdefActor(snoname.snoHandle);
                tActorAdd.eTeam = TagMapGetDWORD(pdefActor->hTagMap, TAG_ACTOR_DEFAULT_TEAM);
            }

            if (snoname.snoGroup == ACTOR_SNO)
            {
                PRTransform transform(tPlacement.tPlace.transform);  // DWARF: copy ctor inlined at 567, operator* at 568.
                transform = transformSceneToWorld * transform;

                ScopedActorDefinitionLockR pdefActor(snoname.snoHandle);
                tActorAdd.snoActor = snoname.snoHandle; tActorAdd.dwActorAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
                tActorAdd.tSNOName = snoname;
                tActorAdd.tPlace.idSWorld = tSCD.idSWorld;
                tActorAdd.tPlace.wp = transform.wp;
                tActorAdd.qOrientation = transform.q;





                ActorType eActorType = pdefActor->eType;
                switch (eActorType)
                {
                case ACTORTYPE_MONSTER: GBAdd(tActorAdd); break;


                case ACTORTYPE_GIZMO:
                    tActorAdd.dwActorAddFlags |= 1 << ACTORADD_IS_FROM_DISTRIBUTION;
                    GizmoAddBySNO(-1, tActorAdd); break;


                case ACTORTYPE_SERVER_PROP:
                    PropAddBySNO(tActorAdd); break;


                default:
                    SceneryAdd(tActorAdd); break;
                }
            }
        }
    }

    RandSetSeed_Internal(SGameGlobalsGet().rng, nOldSeed);

}




CRefString GizmoLocSuffixFromIndex(const int32 nIndex)

{
    int32 nNum = nIndex / 26;
    int32 nLetter = nIndex % 26;
    CRefString sSuffix;
    if (nNum == 0)
    {
        sSuffix.Format("%c", (char)('A' + nLetter));
    }
    else
    {
        sSuffix.Format("%c%d", (char)('A' + nLetter), nNum);
    }
    // Inferred return: NRVO result storage is the constructor/Format receiver.
    return sSuffix;
}




