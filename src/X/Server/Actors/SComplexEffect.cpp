// Original file: X/Server/Actors/SComplexEffect.cpp
// Functions sit on their original line numbers (2.6.2 DWARF).
#include "../../Common/SNO/SNOFiles.h"
#include "ActorEffect.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../Common/Actors/ComplexEffect.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/WorldPlace.h"
#include "../../Common/Networking/GameMessage.h"
#include "SComplexEffect.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"
#include "../Network/KnownList.h"
#include "../PlayerList.h"
#include "../SGame.h"
#include "../SGameManager.h"








const int32 SEEN_RECENTLY_TIME = 300;

BOOL SComplexEffectChainSeenRecently(const ActorCommonData & tACD);  // declared here (header subset lacks it); DWARF definition SComplexEffect.cpp:502




struct SComplexEffectGlobals
{
	TDataArray<SComplexEffect, SComplexEffectID> * ptServerComplexEffects;
};








static SComplexEffectGlobals & sSComplexEffectGlobalsGet()
{





	SGame & tSGame = SGameManager::GameGet();

	return *tSGame.ptSComplexEffectGlobals;
}




void SComplexEffectInitializeForGame()

{
	SGame & tSGame = SGameManager::GameGet();


	SComplexEffectGlobals *& ptSComplexEffectGlobals = tSGame.ptSComplexEffectGlobals;

	ptSComplexEffectGlobals = (SComplexEffectGlobals *)SigmaMemoryNew(sizeof(SComplexEffectGlobals), 4, tSGame.ptMemoryInterface, TRUE);


	ptSComplexEffectGlobals->ptServerComplexEffects =
		TDataArrayCreateTracked<SComplexEffect, SComplexEffectID>("SComplexEffect", tSGame.ptMemoryInterface, MAX_COMPLEX_EFFECTS);





}



void SComplexEffectDisposeForGame()

{
	SGame & tSGame = SGameManager::GameGet();


	tSGame.ptSComplexEffectGlobals->ptServerComplexEffects->DeleteAll();
	TDataArrayDispose(tSGame.ptSComplexEffectGlobals->ptServerComplexEffects);


	SigmaMemoryFree(tSGame.ptSComplexEffectGlobals, tSGame.ptMemoryInterface); tSGame.ptSComplexEffectGlobals = NULL;

}



static SComplexEffect * sSComplexEffectTryToGet(
	const SComplexEffectID & idSComplexEffect)
{





	SComplexEffectGlobals & tSComplexEffectGlobals = sSComplexEffectGlobalsGet();
	return tSComplexEffectGlobals.ptServerComplexEffects->ElementTryToGet(idSComplexEffect);
}




static SComplexEffect & sSComplexEffectGet(
	const SComplexEffectID & idSComplexEffect)
{
	SComplexEffect * ptSComplexEffect = sSComplexEffectTryToGet(idSComplexEffect);

	return *ptSComplexEffect;
}



void SComplexEffectSendAllSources(
	const PlayerList & tListPlayers,
	const ACDID idACD)
{
	const ActorCommonData & tACD = ACDGet(idACD);
	const ComplexEffectIDListPooled & listComplexEffects = tACD.tListComplexEffects;


	POS pos = listComplexEffects.GetHeadPosition();
	while (pos)
	{


		SComplexEffectID idSComplexEffect = listComplexEffects.GetNext(pos);


		const SComplexEffect * ptSComplexEffect = sSComplexEffectTryToGet(idSComplexEffect);

		// Inferred: the NULL test has no line row of its own (line 147 is indented one level deeper).
		if (ptSComplexEffect)
		{
			if (ptSComplexEffect->idACDSource == idACD &&
				ptSComplexEffect->snoEffectGroup != SNO_NONE)
			{


				ComplexEffectAddMessage tMessage;
				tMessage.idSComplexEffect = idSComplexEffect;
				tMessage.snoEffectGroup = ptSComplexEffect->snoEffectGroup;
				tMessage.annSource = ANNFromACD(ptSComplexEffect->idACDSource);
				tMessage.annDest = ANNFromACD(ptSComplexEffect->idACDDest);
				tMessage.eType = ptSComplexEffect->eType;
				tMessage.dwParam1 = ptSComplexEffect->dwParam1;
				tMessage.dwParam2 = ptSComplexEffect->dwParam2;
				tMessage.bIgnoreOwnerAlpha = ptSComplexEffect->bIgnoreOwnerAlpha;


				MessageSendToClient(tListPlayers, SMSG_SCOMPLEX_EFFECT_ADD, &tMessage, sizeof(tMessage));
			}
		}
	}




}



SComplexEffectID SComplexEffectAdd(
	ActorCommonData & tACD,
	const SNO snoEffectGroup,
	const ACDID idACDDestParam,
	ComplexEffectType eType,
	uint32 dwParam1,
	uint32 dwParam2,
	BOOL bIgnoreOwnerAlpha)
{
	SComplexEffectGlobals & tSComplexEffectGlobals = sSComplexEffectGlobalsGet();


	SComplexEffectID idSComplexEffect = tSComplexEffectGlobals.ptServerComplexEffects->ElementNew();
	if (idSComplexEffect == SCOMPLEXEFFECTID_INVALID)
	{





		int32 nNumEffects = 0;
		TDataArrayIterator<SComplexEffect> tIt;
		while (tSComplexEffectGlobals.ptServerComplexEffects->Iterate(tIt))
		{
			// Inferred: nNumEffects is only counted (DWARF const_value 0, no code); its reader is compiled out.
			nNumEffects++;
		}

		return SCOMPLEXEFFECTID_INVALID;
	}












	SComplexEffect & tSComplexEffect = sSComplexEffectGet(idSComplexEffect);
	tSComplexEffect.idACDSource = tACD.id;
	tSComplexEffect.snoEffectGroup = snoEffectGroup;
	tSComplexEffect.idACDDest = idACDDestParam;
	tSComplexEffect.eType = eType;
	tSComplexEffect.dwParam1 = dwParam1;
	tSComplexEffect.dwParam2 = dwParam2;
	tSComplexEffect.bIgnoreOwnerAlpha = bIgnoreOwnerAlpha;


	tACD.ComplexEffectTrack(idSComplexEffect);
	tACD.ModifiedMark(ACDMODIFIED_COMPLEX_EFFECTS);
	KnownListItemChanged(KET_ACD, tACD.id, FALSE);


	if (idACDDestParam != ACDID_INVALID)
	{
		ActorCommonData & tACDDest = ACDGet(idACDDestParam);
		tACDDest.ComplexEffectTrack(idSComplexEffect);
		tACDDest.ModifiedMark(ACDMODIFIED_COMPLEX_EFFECTS);
		KnownListItemChanged(KET_ACD, tACDDest.id, FALSE);
	}

	return idSComplexEffect;


}



void SComplexEffectRemove(
	const ACDID idACD,
	const SComplexEffectID idSComplexEffect)
{
	const SComplexEffect * ptSComplexEffect = sSComplexEffectTryToGet(idSComplexEffect);

	// Inferred: the NULL test has no line row of its own.
	if (ptSComplexEffect == NULL)
	{
		return;
	}


	if ((ptSComplexEffect->idACDSource != idACD) && (ptSComplexEffect->idACDDest != idACD))
	{
		return;
	}










	ActorCommonData & tACDSource = ACDGet(ptSComplexEffect->idACDSource);
	ACDPlayEffect(ptSComplexEffect->idACDSource, EFFECT_REMOVE_COMPLEX_EFFECT, idSComplexEffect, ACDID_INVALID);
	tACDSource.ComplexEffectUntrack(idSComplexEffect);


	if (ptSComplexEffect->idACDDest != ACDID_INVALID)
	{
		ActorCommonData & tACDDest = ACDGet(ptSComplexEffect->idACDDest);





		tACDDest.ComplexEffectUntrack(idSComplexEffect);
	}







	SComplexEffectGlobals & tSComplexEffetGlobals = sSComplexEffectGlobalsGet();
	tSComplexEffetGlobals.ptServerComplexEffects->ElementDeleteByID(idSComplexEffect);

}



void SComplexEffectRemoveAllOfType(
	const ACDID idACD,
	const SNO snoEffectGroup)
{


	ActorCommonData & tACD = ACDGet(idACD);
	ComplexEffectIDListPooled & tListComplexEffects = tACD.tListComplexEffects;


	POS pos = tListComplexEffects.GetHeadPosition();
	while (pos)
	{


		const SComplexEffectID idSComplexEffect = tListComplexEffects.GetNext(pos);


		const SComplexEffect & tSComplexEffect = sSComplexEffectGet(idSComplexEffect);
		// Inferred from the parameter: the original was linker-removed, no code to compare.
		if (tSComplexEffect.snoEffectGroup == snoEffectGroup)
		{
			SComplexEffectRemove(idACD, idSComplexEffect);
		}
	}








}

void SComplexEffectRemoveAll(
	const ACDID idACD)
{


	ActorCommonData & tACD = ACDGet(idACD);
	ComplexEffectIDListPooled & tListComplexEffects = tACD.tListComplexEffects;


	POS pos = tListComplexEffects.GetHeadPosition();
	while (pos)
	{


		const SComplexEffectID idSComplexEffect = tListComplexEffects.GetNext(pos);


		SComplexEffectRemove(idACD, idSComplexEffect);
	}


}



static BOOL sValidateComplexEffect(
	const SComplexEffect & tSComplexEffect,
	SWorldID idSWorld)
{
	ActorCommonData * ptACDSrc = ACDTryToGet(tSComplexEffect.idACDSource);
	ActorCommonData * ptACDDst = ACDTryToGet(tSComplexEffect.idACDDest);

	SWorldID idSWorldSrc = SWORLDID_INVALID;
	SWorldID idSWorldDst = SWORLDID_INVALID;

	if (ptACDSrc)
	{
		idSWorldSrc = ptACDSrc->GetPlace().idSWorld;
	}

	if (ptACDDst)
	{
		idSWorldDst = ptACDDst->GetPlace().idSWorld;
	}

	BOOL bValid = TRUE;
	// Inferred from lines 385/386/391/392: source and destination in two different worlds, or either one outside idSWorld.
	if (idSWorldSrc != SWORLDID_INVALID && idSWorldDst != SWORLDID_INVALID &&
		idSWorldSrc != idSWorldDst)
	{
		bValid = FALSE;
	}

	if ((idSWorldSrc != SWORLDID_INVALID && idSWorldSrc != idSWorld) ||
		(idSWorldDst != SWORLDID_INVALID && idSWorldDst != idSWorld))
	{
		bValid = FALSE;
	}

	return bValid;
}

























void SComplexEffectACDsExpand(
	ACDIDListPooled & listACDsWithConnectedComplexEffects,
	SWorldID idSWorld)
{






	POS pos = listACDsWithConnectedComplexEffects.GetHeadPosition();
	while (pos)
	{
		ComplexEffectIDListPooled listDeletedEffects(MemoryPoolsGetDATAIDListPool());

		// Inferred: a plain read of the node data (line 440 has no inlined GetAt/GetNext; the list advances on line 487).
		const ACDID idACD = (ACDID)*pos;


		ActorCommonData & tACD = ACDGet(idACD);
		ComplexEffectIDListPooled & tListComplexEffects = tACD.tListComplexEffects;
		POS posEffect = tListComplexEffects.GetHeadPosition();
		while (posEffect)
		{


			const SComplexEffectID idSComplexEffect = tListComplexEffects.GetNext(posEffect);


			const SComplexEffect & tSComplexEffect = sSComplexEffectGet(idSComplexEffect);

			if (!sValidateComplexEffect(tSComplexEffect, idSWorld))
			{
				listDeletedEffects.AddTail_Tracked(idSComplexEffect);
			}
			else
			{

				if (tSComplexEffect.idACDSource != ACDID_INVALID && tSComplexEffect.idACDSource != idACD)
				{
					if (!listACDsWithConnectedComplexEffects.Find(tSComplexEffect.idACDSource))
					{
						listACDsWithConnectedComplexEffects.AddTail_Tracked(tSComplexEffect.idACDSource);
					}
				}

				if (tSComplexEffect.idACDDest != ACDID_INVALID && tSComplexEffect.idACDDest != idACD)
				{
					if (!listACDsWithConnectedComplexEffects.Find(tSComplexEffect.idACDDest))
					{
						listACDsWithConnectedComplexEffects.AddTail_Tracked(tSComplexEffect.idACDDest);
					}
				}
			}
		}
		posEffect = listDeletedEffects.GetHeadPosition();
		while (posEffect)
		{
			SComplexEffectID idSComplexEffect = listDeletedEffects.GetNext(posEffect);
			SComplexEffectRemove(idACD, idSComplexEffect);
		}


		listACDsWithConnectedComplexEffects.GetNext(pos);
	}
}



BOOL SComplexEffectChainSeenRecently(
	const ACDID idACD)
{
	const ActorCommonData & tACD = ACDGet(idACD);
	return SComplexEffectChainSeenRecently(tACD);
}



BOOL SComplexEffectChainSeenRecently(
	const ActorCommonData & tACD)
{
	const uint32 dwNow = GameGetServerTime();
	const ComplexEffectIDListPooled & tListComplexEffects = tACD.tListComplexEffects;

	POS pos = tListComplexEffects.GetHeadPosition();
	while (pos)
	{


		const SComplexEffectID idSComplexEffect = tListComplexEffects.GetNext(pos);


		const SComplexEffect & tSComplexEffect = sSComplexEffectGet(idSComplexEffect);



		ACDID idACDSources[2] = { tSComplexEffect.idACDSource, tSComplexEffect.idACDDest };


		for (int32 i = 0; i < 2; i++)
		{
			ActorCommonData * ptACDSource = ACDTryToGet(idACDSources[i]);
			if (ptACDSource)
			{
				if (dwNow - ptACDSource->GetTimeStamp() <= SEEN_RECENTLY_TIME)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}




int32 SComplexEffectsGetTotalCount()
{

	SComplexEffectGlobals & tSComplexEffectGlobals = sSComplexEffectGlobalsGet();
	// Inferred: no inlined GetTotalCount in the DWARF, so the field is read directly.
	return tSComplexEffectGlobals.ptServerComplexEffects->dwTotalCount;
}


BOOL SComplexEffectIsValid(
	const ACDID idACD,
	const SComplexEffectID idSComplexEffect)
{
	const SComplexEffect * ptSComplexEffect = sSComplexEffectTryToGet(idSComplexEffect);

	// Inferred: the NULL test has no line row of its own.
	if (ptSComplexEffect == NULL)
	{
		return FALSE;
	}
	if (ptSComplexEffect->idACDSource != idACD && ptSComplexEffect->idACDDest != idACD)
	{
		return FALSE;
	}
	return TRUE;
}



BOOL SComplexEffectGet(
	const ACDID idACD,
	const SComplexEffectID idSComplexEffect,
	SComplexEffect & tSComplexEffect)
{
	const SComplexEffect * ptSComplexEffect = sSComplexEffectTryToGet(idSComplexEffect);

	// Inferred: the NULL test has no line row of its own.
	if (ptSComplexEffect == NULL)
	{
		return FALSE;
	}
	if (ptSComplexEffect->idACDSource != idACD && ptSComplexEffect->idACDDest != idACD)
	{
		return FALSE;
	}
	tSComplexEffect = *ptSComplexEffect;
	return TRUE;
}
