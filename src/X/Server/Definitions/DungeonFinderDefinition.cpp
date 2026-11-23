// Original file: X/Server/Definitions/DungeonFinderDefinition.cpp

#include "DungeonFinderDefinition.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../System/SNephalemRift.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;  // X/Sigma/Files/XFilesCommon.cpp:20
extern const XTypeDescriptor * DT_SNO;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp
#include "../../Common/Definitions/LevelAreaDefinition.h"
#include "../../Common/Definitions/WorldDefinition.h"
#include <math.h>
#include <string.h>
static const int32 MAX_DUNGEON_FINDER_LEVELS = 10;  // X/Server/Definitions/DungeonFinderDefinition.h:18 (not in src's copy of that header)










































void DungeonFinderRefreshTotalWeights(DungeonFinderDefinition * ptDefDungeonFinder);




static const TypeDescriptorField WeightedTransition_Fields[5] = {  // line 36
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeightedTransition, snoNextWorld), "", WORLD_SNO, 0, -1),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeightedTransition, flWeight), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WeightedTransition, bExcludeFromTieredRift), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WeightedTransition, nTableIndex), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WeightedTransition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WeightedTransition("WeightedTransition", 0, WeightedTransition_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 45
const XTypeDescriptor * DT_WEIGHTED_TRANSITION = &TypeDescriptor_WeightedTransition;

static const TypeDescriptorField WeightedPopulation_Fields[6] = {  // line 47
    FilterTypeDescriptorField("", DT_SNO, offsetof(WeightedPopulation, snoLevelArea), "", LEVELAREA_SNO, 0, -1),
    CharArrayTypeDescriptorField("", offsetof(WeightedPopulation, szPopulationName), "", 1, 64, -1),
    TypeDescriptorField("", DT_FLOAT, offsetof(WeightedPopulation, flWeight), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WeightedPopulation, bPlaystationOnly), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WeightedPopulation, bExcludeFromTieredRift), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WeightedPopulation), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WeightedPopulation("WeightedPopulation", 0, WeightedPopulation_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 55
const XTypeDescriptor * DT_WEIGHTED_POPULATION = &TypeDescriptor_WeightedPopulation;

#define LT(f) offsetof(DungeonFinderLevelTable, f)
static const TypeDescriptorField DungeonFinderLevelTable_Fields[15] = {  // line 57
    FilterTypeDescriptorField("", DT_SNO, LT(snoWorldTileset), "", WORLD_SNO, 0, -1),
    TypeDescriptorField("", DT_FLOAT, LT(flWeight), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, LT(serTransitions), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_WEIGHTED_TRANSITION, LT(arTransitions), 0x21, (int32)LT(serTransitions) - (int32)LT(arTransitions)),
    TypeDescriptorField("", DT_SERIALIZEDATA, LT(serPopulations), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_WEIGHTED_POPULATION, LT(arPopulations), 0x21, (int32)LT(serPopulations) - (int32)LT(arPopulations)),
    TypeDescriptorField("", DT_FLOAT, LT(flSpawnGroupDensityMultiplier), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, LT(flEliteSpawnGroupDensityMultiplier), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, LT(flGizmoDensityMultiplier), NULL, 1),
    TypeDescriptorField("", DT_INT, LT(bExcludeFromTieredRift), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, LT(flTotalTransitionWeights), NULL, 0),
    TypeDescriptorField("", DT_FLOAT, LT(flTotalPopulationWeights), NULL, 0),
    FilterTypeDescriptorField("", DT_SNO, LT(snoBossEncounter), "", ENCOUNTER_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, LT(snoElitePopulation), "", LEVELAREA_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DungeonFinderLevelTable), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DungeonFinderLevelTable("DungeonFinderLevelTable", 0, DungeonFinderLevelTable_Fields, 15, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 78
const XTypeDescriptor * DT_DUNGEON_FINDER_LEVEL_TABLE = &TypeDescriptor_DungeonFinderLevelTable;

static const TypeDescriptorField DungeonFinderDefinition_Fields[4] = {  // line 80
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(DungeonFinderDefinition, serDungeonFinderLevelTables), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_DUNGEON_FINDER_LEVEL_TABLE, offsetof(DungeonFinderDefinition, arDungeonFinderLevelTables), 0x21,
                                     (int32)offsetof(DungeonFinderDefinition, serDungeonFinderLevelTables) - (int32)offsetof(DungeonFinderDefinition, arDungeonFinderLevelTables)),
    TypeDescriptorField("", DT_FLOAT, offsetof(DungeonFinderDefinition, flTotalDungeonFinderLevelWeights), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DungeonFinderDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DungeonFinderDefinition("DungeonFinder", 6, DungeonFinderDefinition_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 85
const XTypeDescriptor * DT_DUNGEON_FINDER = &TypeDescriptor_DungeonFinderDefinition;

DungeonFinderGroupDef * gptGroupDefDungeonFinder;  // line 91

// line 97
DungeonFinderGroupDef::DungeonFinderGroupDef()
    : GroupDefinition("DungeonFinder", DUNGEONFINDER_SNO,
                      SNOVersions(33, 35, 33, 33, 34),
                      SNODATATYPE_TITLE, sizeof(DungeonFinderDefinition), DT_DUNGEON_FINDER, 0x8005, 4)
{
}

// line 112
uint32 DungeonFinderGroupDef::GetMaxCount()
{
    return 6;
}

// line 118
void DungeonFinderGroupDef::GetDefaultData(void * ptDefaultDef) const
{
    SigmaMemoryClear(ptDefaultDef, sizeof(DungeonFinderDefinition));
}

// line 126
void DungeonFinderGroupDef::Postprocess(void * lpDefinition)
{
    DungeonFinderDefinition * ptDefDungeonFinder = (DungeonFinderDefinition *)lpDefinition;
    GroupDefinition::Postprocess(lpDefinition);
    DungeonFinderRefreshTotalWeights(ptDefDungeonFinder);
}

// line 138
void DungeonFinderRefreshTotalWeights(DungeonFinderDefinition * ptDefDungeonFinder)
{
    int32 nDungeonFinderLevels = ptDefDungeonFinder->serDungeonFinderLevelTables.ElementCount(sizeof(DungeonFinderLevelTable));
    ptDefDungeonFinder->flTotalDungeonFinderLevelWeights = 0.0f;

    for (int32 i = 0; i < nDungeonFinderLevels; i++)
    {
        DungeonFinderLevelTable & tDungeonFinderLevel = ptDefDungeonFinder->arDungeonFinderLevelTables[i];
        ptDefDungeonFinder->flTotalDungeonFinderLevelWeights += tDungeonFinderLevel.flWeight;

        int32 nNumTransitions = tDungeonFinderLevel.serTransitions.ElementCount(sizeof(WeightedTransition));
        tDungeonFinderLevel.flTotalTransitionWeights = 0.0f;
        for (int32 j = 0; j < nNumTransitions; j++)
        {
            tDungeonFinderLevel.flTotalTransitionWeights += tDungeonFinderLevel.arTransitions[j].flWeight;
            tDungeonFinderLevel.arTransitions[j].nTableIndex = -1;
            for (int32 k = 0; k < nDungeonFinderLevels; k++)
            {
                if (tDungeonFinderLevel.arTransitions[j].snoNextWorld == ptDefDungeonFinder->arDungeonFinderLevelTables[k].snoWorldTileset)
                {
                    tDungeonFinderLevel.arTransitions[j].nTableIndex = k;
                    break;
                }
            }
        }

        int32 nNumPopulations = tDungeonFinderLevel.serPopulations.ElementCount(sizeof(WeightedPopulation));
        tDungeonFinderLevel.flTotalPopulationWeights = 0.0f;
        for (int32 j = 0; j < nNumPopulations; j++)
        {
            tDungeonFinderLevel.flTotalPopulationWeights += tDungeonFinderLevel.arPopulations[j].flWeight;
        }
    }
}

// line 190
const DungeonFinderLevelTable * DungeonFinderPickLevel(const DungeonFinderDefinition * ptDefDungeonFinder, const DungeonFinderOverrides * ptOverrides, RNG & rng)
{
    BOOL bPickForTieredRift = NephalemRiftManager::IsTieredRiftActive();
    float flTotalDungeonFinderLevelWeights = ptDefDungeonFinder->flTotalDungeonFinderLevelWeights;
    int32 nDungeonFinderLevels = ptDefDungeonFinder->serDungeonFinderLevelTables.ElementCount(sizeof(DungeonFinderLevelTable));

    if (bPickForTieredRift || (ptOverrides && ptOverrides->nMaxFloorSize > 0))
    {
        const DungeonFinderLevelTable * ptDungeonFinderLevel;
        for (int32 i = 0; i < nDungeonFinderLevels; i++)
        {
            ptDungeonFinderLevel = &ptDefDungeonFinder->arDungeonFinderLevelTables[i];
            if (bPickForTieredRift && ptDungeonFinderLevel->bExcludeFromTieredRift)
            {
                flTotalDungeonFinderLevelWeights -= ptDungeonFinderLevel->flWeight;
            }
            else if (ptOverrides && ptOverrides->nMaxFloorSize > 0 && ptDungeonFinderLevel->flSpawnGroupDensityMultiplier > ptOverrides->nMaxFloorSize)
            {
                flTotalDungeonFinderLevelWeights -= ptDungeonFinderLevel->flWeight;
            }
        }
    }

    float flRand = RandRangeFloat_Internal(rng, 0.0f, flTotalDungeonFinderLevelWeights);
    const DungeonFinderLevelTable * ptDungeonFinderLevel = NULL;

    for (int32 i = 0; i < nDungeonFinderLevels; i++)
    {
        ptDungeonFinderLevel = &ptDefDungeonFinder->arDungeonFinderLevelTables[i];

        if (bPickForTieredRift && ptDungeonFinderLevel->bExcludeFromTieredRift)
        {
            continue;
        }

        if (ptOverrides && ptOverrides->nMaxFloorSize > 0 && ptDungeonFinderLevel->flSpawnGroupDensityMultiplier > ptOverrides->nMaxFloorSize)
        {
            continue;
        }

        if (flRand < ptDungeonFinderLevel->flWeight)
        {
            break;
        }
        flRand -= ptDungeonFinderLevel->flWeight;
    }
    return ptDungeonFinderLevel;
}

// line 249
BOOL DungeonFinderPickPopulation(const DungeonFinderDefinition * ptDefDungeonFinder, const DungeonFinderLevelTable * ptDungeonFinderLevel, RNG & rng, SNO & snoLevelArea, LPSTR lpszPopulationName, int32 nNumFloors, const DungeonFinderOverrides * ptOverrides)
{
    BOOL bExcludePlaystationPopulations = TRUE;
    BOOL bPickForTieredRift = NephalemRiftManager::IsTieredRiftActive();
    int32 nNumPopulations = ptDungeonFinderLevel->serPopulations.ElementCount(sizeof(WeightedPopulation));
    WeightedPopulation * ptPopulation = NULL;

    if (ptOverrides)
    {
        if (ptOverrides->arPopulationHashes[nNumFloors] != 0)
        {
            for (int32 i = 0; i < nNumPopulations; i++)
            {
                if (XHashKey<const char *>(ptDungeonFinderLevel->arPopulations[i].szPopulationName) == ptOverrides->arPopulationHashes[nNumFloors])
                {
                    ptPopulation = &ptDungeonFinderLevel->arPopulations[i];
                    break;
                }
            }
        }
        if (!ptPopulation && ptOverrides->dwMasterPopulationHash != 0)
        {
            for (int32 i = 0; i < nNumPopulations; i++)
            {
                if (XHashKey<const char *>(ptDungeonFinderLevel->arPopulations[i].szPopulationName) == ptOverrides->dwMasterPopulationHash)
                {
                    ptPopulation = &ptDungeonFinderLevel->arPopulations[i];
                    break;
                }
            }
        }
    }

    float flTotalPopulationWeights = ptDungeonFinderLevel->flTotalPopulationWeights;
    {
        int32 nNumPopulations2 = ptDungeonFinderLevel->serPopulations.ElementCount(sizeof(WeightedPopulation));
        for (int32 j = 0; j < nNumPopulations2; j++)
        {
            WeightedPopulation tWeightedPopulation = ptDungeonFinderLevel->arPopulations[j];
            if (tWeightedPopulation.bPlaystationOnly != FALSE || tWeightedPopulation.bExcludeFromTieredRift != FALSE)
            {
                flTotalPopulationWeights -= tWeightedPopulation.flWeight;
            }
        }
    }

    float flRand = RandRangeFloat_Internal(rng, 0.0f, flTotalPopulationWeights);
    if (!ptPopulation)
    {
        for (int32 i = 0; i < nNumPopulations; i++)
        {
            if (bExcludePlaystationPopulations && ptDungeonFinderLevel->arPopulations[i].bPlaystationOnly)
            {
                continue;
            }
            ptPopulation = &ptDungeonFinderLevel->arPopulations[i];
            if (flRand < ptPopulation->flWeight)
            {
                break;
            }
            flRand -= ptPopulation->flWeight;
        }
    }

    if (!ptPopulation)
    {
        snoLevelArea = SNO_NONE;
        Blizzard::String::Copy(lpszPopulationName, "", 64);
        return FALSE;
    }
    snoLevelArea = ptPopulation->snoLevelArea;
    Blizzard::String::Copy(lpszPopulationName, ptPopulation->szPopulationName, 64);
    return TRUE;
}

// line 350
const DungeonFinderLevelTable * DungeonFinderPickNextLevel(const DungeonFinderDefinition * ptDefDungeonFinder, const DungeonFinderLevelTable * ptDungeonFinderLevel, const DungeonFinderOverrides * ptOverrides, RNG & rng)
{
    BOOL bPickForTieredRift = NephalemRiftManager::IsTieredRiftActive();
    float flTotalLevelTransitionWeights = ptDungeonFinderLevel->flTotalTransitionWeights;
    int32 nNumTransitions = ptDungeonFinderLevel->serTransitions.ElementCount(sizeof(WeightedTransition));

    for (int32 i = 0; i < nNumTransitions; i++)
    {
        WeightedTransition * ptTransition = &ptDungeonFinderLevel->arTransitions[i];
        if (ptTransition->nTableIndex == -1 || (bPickForTieredRift && ptTransition->bExcludeFromTieredRift))
        {
            flTotalLevelTransitionWeights -= ptTransition->flWeight;
        }
        else if (ptOverrides && ptOverrides->nMaxFloorSize > 0 && ptDefDungeonFinder->arDungeonFinderLevelTables[ptTransition->nTableIndex].flSpawnGroupDensityMultiplier > ptOverrides->nMaxFloorSize)
        {
            flTotalLevelTransitionWeights -= ptTransition->flWeight;
        }
    }

    float flRand = RandRangeFloat_Internal(rng, 0.0f, flTotalLevelTransitionWeights);
    WeightedTransition * ptBestTransition = NULL;
    for (int32 i = 0; i < nNumTransitions; i++)
    {
        WeightedTransition * ptTransition = &ptDungeonFinderLevel->arTransitions[i];
        if (ptTransition->nTableIndex == -1)
        {
            continue;
        }
        if (bPickForTieredRift && ptTransition->bExcludeFromTieredRift)
        {
            continue;
        }
        if (ptOverrides && ptOverrides->nMaxFloorSize > 0 &&
            ptDefDungeonFinder->arDungeonFinderLevelTables[ptTransition->nTableIndex].flSpawnGroupDensityMultiplier > ptOverrides->nMaxFloorSize)
        {
            continue;
        }
        if (flRand < ptTransition->flWeight)
        {
            ptBestTransition = ptTransition;
            break;
        }
        flRand -= ptTransition->flWeight;
    }

    if (ptBestTransition)
    {
        return &ptDefDungeonFinder->arDungeonFinderLevelTables[ptBestTransition->nTableIndex];
    }
    return NULL;
}

// line 639
void DungeonFinderOverridesInit(DungeonFinderOverrides & tDungeonFinderOverrides)
{
    tDungeonFinderOverrides.snoDungeonFinderEngine = SNO_NONE;
    tDungeonFinderOverrides.nSeed = 0;
    tDungeonFinderOverrides.snoMasterTileset = SNO_NONE;
    tDungeonFinderOverrides.dwMasterPopulationHash = 0;

    for (int32 i = 0; i < 10; i++)
    {
        tDungeonFinderOverrides.arSnoTilesets[i] = SNO_NONE;
        tDungeonFinderOverrides.arPopulationHashes[i] = 0;
    }
    tDungeonFinderOverrides.snoDungeonFinderBossOverride = SNO_NONE;
    tDungeonFinderOverrides.nMaxFloorSize = 0;
    tDungeonFinderOverrides.nWeeklyChallengeRiftLevel = -1;
    tDungeonFinderOverrides.nWeeklyChallengeCompletionMilliseconds = -1;

    tDungeonFinderOverrides.bUsePylonOverrides = FALSE;

    for (int32 i = 0; i < 5; i++)
    {
        tDungeonFinderOverrides.arPylonOverrides[i].snoActorPylon = SNO_NONE;
    }
}





















































// DungeonFinderPickWeather: lines 450-482 (DWARF decl 450, params 451-453, prologue 454).
// The nWeathers == 1 guard at line 474 has no Random.h row of its own and Rand_Internal is
// called with nWeathers - 2 (mask nMax - 1 = nWeathers - 2, modulo by nWeathers - 1): the
// original probably used a Rand(rng, n) macro (0 when n == 0, else Rand_Internal(rng, n - 1)) that src lacks.
SNO DungeonFinderPickWeather(
	RNG & rng,
	SNO snoLevelArea,
	LPCSTR lpszPopulationName)
{
	ScopedLevelAreaDefinitionLockR ptDefLevelArea(snoLevelArea);

	SNO snoWeather = SNO_NONE;
	if (!ptDefLevelArea->ptServerData)
	{
		return snoWeather;
	}

	int32 nPopulations = ptDefLevelArea->ptServerData->serSpawnPopulations.ElementCount(sizeof(LevelAreaSpawnPopulation));
	for (int32 i = 0; i < nPopulations; i++)
	{
		const LevelAreaSpawnPopulation & tSpawnPopulation = ptDefLevelArea->ptServerData->arSpawnPopulations[i];
		if (strcmp(tSpawnPopulation.szName, lpszPopulationName) == 0)
		{
			int32 nWeathers = tSpawnPopulation.serDungeonFinderWeatherSNOs.ElementCount(sizeof(SNO));
			if (nWeathers > 0)
			{
				// inferred, see the note above the function
				int32 nWeatherIndex = (nWeathers - 1) ? Rand_Internal(rng, nWeathers - 2) : 0;
				snoWeather = tSpawnPopulation.arDungeonFinderWeatherSNOs[nWeatherIndex];
			}
			break;
		}
	}

	return snoWeather;
}





SNO DungeonFinderGetLevelAreaBossEncounterOverride(
	SNO snoLevelArea,
	LPCSTR lpszPopulationName)
{
	ScopedLevelAreaDefinitionLockR ptDefLevelArea(snoLevelArea);

	if (!ptDefLevelArea->ptServerData)
	{
		return SNO_NONE;
	}

	int32 nPopulations = ptDefLevelArea->ptServerData->serSpawnPopulations.ElementCount(sizeof(LevelAreaSpawnPopulation));
	for (int32 i = 0; i < nPopulations; i++)
	{
		if (strcmp(ptDefLevelArea->ptServerData->arSpawnPopulations[i].szName, lpszPopulationName) == 0)
		{
			return ptDefLevelArea->ptServerData->arSpawnPopulations[i].snoDungeonFinderOverrideBossEncounter;
		}
	}

	return SNO_NONE;
}




void DungeonFinderCreate(
	SNO snoDungeonFinder,
	RNG & rng,
	DungeonFinderLevelInstance * tDungeonFinderLevels,
	uint32 & nNumDungeonFinderLevels,
	const DungeonFinderOverrides * ptOverrides)
{
	nNumDungeonFinderLevels = 0;

	ScopedDungeonFinderDefinitionLockR ptDefDungeonFinder(snoDungeonFinder);




	const DungeonFinderLevelTable * ptDungeonFinderLevel = DungeonFinderPickLevel(ptDefDungeonFinder, ptOverrides, rng);

	if (ptDungeonFinderLevel == NULL)
	{
		return;
	}



	int32 nTotalFloorPoints = 0;

	do
	{
		if (ptOverrides)
		{
			SNO snoTilesetOverride = ptOverrides->arSnoTilesets[nNumDungeonFinderLevels];


			if (snoTilesetOverride == SNO_NONE)
			{
				snoTilesetOverride = ptOverrides->snoMasterTileset;
			}


			if (snoTilesetOverride != SNO_NONE)
			{
				int32 nDungeonFinderLevels = ptDefDungeonFinder->serDungeonFinderLevelTables.ElementCount(sizeof(DungeonFinderLevelTable));
				for (int32 i = 0; i < nDungeonFinderLevels; i++)
				{
					if (snoTilesetOverride == ptDefDungeonFinder->arDungeonFinderLevelTables[i].snoWorldTileset)
					{
						ptDungeonFinderLevel = &ptDefDungeonFinder->arDungeonFinderLevelTables[i];
						break;
					}
				}
			}
		}

		int32 nFloorPoints = 0;
		ScopedWorldDefinitionLockR ptDefWorld(ptDungeonFinderLevel->snoWorldTileset);
		if ((ptDefWorld->uFlags >> WDF_DUNGEON_FINDER_TILESET) & 1)
		{
			DungeonFinderLevelInstance & tDungeonFinderFloor = tDungeonFinderLevels[nNumDungeonFinderLevels];
			SigmaMemoryClear(&tDungeonFinderFloor, sizeof(DungeonFinderLevelInstance));

			tDungeonFinderFloor.snoTilesetWorld = ptDungeonFinderLevel->snoWorldTileset;
			tDungeonFinderFloor.flSpawnGroupDensityMultiplier = ptDungeonFinderLevel->flSpawnGroupDensityMultiplier;
			tDungeonFinderFloor.flEliteSpawnGroupDensityMultiplier = ptDungeonFinderLevel->flEliteSpawnGroupDensityMultiplier;
			tDungeonFinderFloor.flGizmoDensityMultiplier = ptDungeonFinderLevel->flGizmoDensityMultiplier;
			tDungeonFinderFloor.snoBossEncounter = ptDungeonFinderLevel->snoBossEncounter;
			tDungeonFinderFloor.snoElitePopulation = ptDungeonFinderLevel->snoElitePopulation;
			tDungeonFinderFloor.dwWorldSeed = rng.dwSeed;

			SNO snoLevelArea = SNO_NONE;
			DungeonFinderPickPopulation(ptDefDungeonFinder, ptDungeonFinderLevel, rng, snoLevelArea, tDungeonFinderFloor.szPopulationName, nNumDungeonFinderLevels, ptOverrides);


			tDungeonFinderFloor.snoWeather = DungeonFinderPickWeather(rng, snoLevelArea, tDungeonFinderFloor.szPopulationName);

			SNO snoBossEncounterOverride = DungeonFinderGetLevelAreaBossEncounterOverride(snoLevelArea, tDungeonFinderFloor.szPopulationName);
			if (snoBossEncounterOverride != SNO_NONE)
			{
				tDungeonFinderFloor.snoBossEncounter = snoBossEncounterOverride;
			}












			nFloorPoints = (int32)ceilf(ptDungeonFinderLevel->flSpawnGroupDensityMultiplier);


			nNumDungeonFinderLevels++;
		}





		// line 615 tests nFloorPoints (not the total) against 9; the ptNextDungeonFinderLevel block is lines 618-628
		nTotalFloorPoints += nFloorPoints;
		if (nFloorPoints < MAX_DUNGEON_FINDER_LEVELS)
		{

			const DungeonFinderLevelTable * ptNextDungeonFinderLevel = DungeonFinderPickNextLevel(ptDefDungeonFinder, ptDungeonFinderLevel, ptOverrides, rng);

			if (ptNextDungeonFinderLevel)
			{
				ptDungeonFinderLevel = ptNextDungeonFinderLevel;
			}
			// inferred from line 628 (nFloorPoints tested first, then the tileset flag) and the cleanup
			// selector the original keeps across the ptDefWorld unlock: a break out of the loop
			else if (nFloorPoints == 0 || !((ptDefWorld->uFlags >> WDF_DUNGEON_FINDER_TILESET) & 1))
			{
				break;
			}
		}



	} while (nTotalFloorPoints < MAX_DUNGEON_FINDER_LEVELS);
}
