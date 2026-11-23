// Original file: X/Client/RActors/CComplexEffect.cpp
// Functions sit on their original line numbers (2.6.2 DWARF).
#include "CComplexEffect.h"
#include "../../Common/Actors/ComplexEffect.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Game.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"
#include "../CGame.h"
#include "../EffectGroup.h"
#include "../Rope.h"
#include "RActors.h"

// Declared here because their headers do not carry them yet.
EffectGroupID EffectGroupCreateTracked(const SNO snoEffectGroup, const RActorID idRActorSource, const RActorID idRActorDest, ParentType eTypeOverride, const BOOL bIgnoreOwnerAlpha);  // X/Client/EffectGroup.cpp:1311
EffectGroupID EffectGroupCreateRopeTracked(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom, const ACDID idACDTo, const RopePosition ePositionTo, const BOOL bIgnoreOwnerAlpha);  // X/Client/EffectGroup.cpp:1355
void EffectGroupOutro(const EffectGroupID idEffectGroup);  // X/Client/EffectGroup.cpp:1399
Angle ACDGetYaw(ACDID idACD);  // X/Common/ActorCommonData.cpp:2997
BOOL RActorSetACDLocationFromMessage(ActorCommonData & tACD, const ActorLocation & tNewLocation, BOOL bFirstTime, const BOOL fNewItem, const BOOL fCreateActor);  // X/Client/RActors/RActors.cpp:7118




















struct CComplexEffect {  // size 0x48
    CComplexEffectID id;  // 0x00
    SComplexEffectID idSComplexEffect;  // 0x04
    EffectGroupID idEffectGroup;  // 0x08
    ACDID idACDSource;  // 0x0c
    ACDID idACDDest;  // 0x10
    ComplexEffectType eType;  // 0x14
    uint32 dwParam1;  // 0x18
    uint32 dwParam2;  // 0x1c
    XPooledDataIDList listDataID;  // 0x20
};

struct CComplexEffectGlobals {  // size 0x8
    TDataArray<CComplexEffect, CComplexEffectID> * ptClientComplexEffects;  // 0x00
};

void sCComplexEffectRemoveByID(const ACDID idACD, const CComplexEffectID idCComplexEffect);
void CComplexEffectRemove(const ACDID idACD, const SComplexEffectID idSComplexEffect);


















static CComplexEffectGlobals & sCComplexEffectGlobalsGet()
{





    CGame & tCGame = CGameGet();

    return *tCGame.ptCComplexEffectGlobals;
}




static CComplexEffect & sCComplexEffectGet(const CComplexEffectID idCComplexEffect)
{

    CComplexEffectGlobals & tCComplexEffectGlobals = sCComplexEffectGlobalsGet();
    return *tCComplexEffectGlobals.ptClientComplexEffects->ElementGet(idCComplexEffect);
}



void CComplexEffectInitializeForGame()

{
    CGame & tCGame = CGameGet();


    CComplexEffectGlobals *& ptCComplexEffectGlobals = tCGame.ptCComplexEffectGlobals;

    ptCComplexEffectGlobals = (CComplexEffectGlobals *)SigmaMemoryNew(sizeof(CComplexEffectGlobals), 4, AppGetCurrentMemoryInterface(), TRUE);


    ptCComplexEffectGlobals->ptClientComplexEffects =
        TDataArrayCreateTracked<CComplexEffect, CComplexEffectID>("CComplexEffect", AppGetCurrentMemoryInterface(), MAX_COMPLEX_EFFECTS);






}



void CComplexEffectDisposeForGame()

{
    CGame & tCGame = CGameGet();


    TDataArrayIterator<CComplexEffect> tItCComplexEffect;
    while (tCGame.ptCComplexEffectGlobals->ptClientComplexEffects->Iterate(tItCComplexEffect))
    {
        CComplexEffect * tCComplexEffect = tItCComplexEffect.ptElement;
        CComplexEffectRemove(tCComplexEffect->idACDSource, tCComplexEffect->idSComplexEffect);
    }



    tCGame.ptCComplexEffectGlobals->ptClientComplexEffects->DeleteAll();
    TDataArrayDispose(tCGame.ptCComplexEffectGlobals->ptClientComplexEffects);


    SigmaMemoryFree(tCGame.ptCComplexEffectGlobals, AppGetCurrentMemoryInterface()); tCGame.ptCComplexEffectGlobals = NULL;

}



static CComplexEffectID sCComplexEffectNewInstance(const ACDID idACDSource, const ACDID idACDDest, const SComplexEffectID idSComplexEffect)
{



    CComplexEffectGlobals & tCComplexEffectGlobals = sCComplexEffectGlobalsGet();


    CComplexEffectID idCComplexEffect = tCComplexEffectGlobals.ptClientComplexEffects->ElementNew();


    CComplexEffect & tCComplexEffect = sCComplexEffectGet(idCComplexEffect);


    tCComplexEffect.idSComplexEffect = idSComplexEffect;
    tCComplexEffect.idEffectGroup = EFFECTGROUPID_INVALID;
    tCComplexEffect.idACDSource = idACDSource;
    tCComplexEffect.idACDDest = idACDDest;
    tCComplexEffect.listDataID.SetPool(MemoryPoolsGetDATAIDListPool());

    return idCComplexEffect;
}





static void sCComplexEffectDestroyInstance(const CComplexEffectID idCComplexEffect)
{

    CComplexEffect & tCComplexEffect = sCComplexEffectGet(idCComplexEffect);
    tCComplexEffect.listDataID.RemoveAll();

    CComplexEffectGlobals & tCComplexEffectGlobals = sCComplexEffectGlobalsGet();
    tCComplexEffectGlobals.ptClientComplexEffects->ElementDeleteByID(idCComplexEffect);
}



static CComplexEffectID sCComplexEffectTryToFindBySComplexEffectID(const ACDID idACD, const SComplexEffectID idSComplexEffect)
{


    ActorCommonData & tACD = ACDGet(idACD);
    ComplexEffectIDListPooled & tListComplexEffects = tACD.tListComplexEffects;


    POS pos = tListComplexEffects.GetHeadPosition();
    while (pos)
    {


        const CComplexEffectID idCComplexEffect = tListComplexEffects.GetNext(pos);


        CComplexEffect & tCComplexEffect = sCComplexEffectGet(idCComplexEffect);


        if (tCComplexEffect.idSComplexEffect == idSComplexEffect)
        {
            return idCComplexEffect;
        }
    }

    return CCOMPLEXEFFECTID_INVALID;
}
















CComplexEffectID CComplexEffectAdd(const SComplexEffectID idSComplexEffect, const SNO snoEffectGroup, const ACDID idACDSource, const ACDID idACDDest, ComplexEffectType eType, uint32 dwParam1, uint32 dwParam2, BOOL bIgnoreOwnerAlpha)








{



    CComplexEffectID idExistingCComplexEffect = sCComplexEffectTryToFindBySComplexEffectID(idACDSource, idSComplexEffect);
    if (idExistingCComplexEffect != CCOMPLEXEFFECTID_INVALID)
    {
        return idExistingCComplexEffect;
    }

    ActorCommonData & tACDSource = ACDGet(idACDSource);






    CComplexEffectID idCComplexEffect = sCComplexEffectNewInstance(idACDSource, idACDDest, idSComplexEffect);


    CComplexEffect & tCComplexEffect = sCComplexEffectGet(idCComplexEffect);
    tCComplexEffect.eType = eType;


    if (eType == COMPLEXEFFECT_TYPE_SPECIAL_ROPE)
    {
        tCComplexEffect.dwParam1 = dwParam1;
        tCComplexEffect.dwParam2 = dwParam2;


        tCComplexEffect.idEffectGroup = EffectGroupCreateRopeTracked(snoEffectGroup, idACDSource, dwParam1, idACDDest, dwParam2, bIgnoreOwnerAlpha);
    }
    else
    {

        RActorID idRActorPrimary = (RActorID)ACDGetOwner(idACDSource);
        RActorID idRActorSecondary = (RActorID)ACDGetOwner(idACDDest);
        tCComplexEffect.idEffectGroup = EffectGroupCreateTracked(snoEffectGroup, idRActorPrimary, idRActorSecondary, 0, bIgnoreOwnerAlpha);
    }


    tACDSource.ComplexEffectTrack(idCComplexEffect);

    return idCComplexEffect;


}



void sCComplexEffectRemoveByID(const ACDID idACD, const CComplexEffectID idCComplexEffect)

{

    CComplexEffect & tCComplexEffect = sCComplexEffectGet(idCComplexEffect);


    if (tCComplexEffect.idEffectGroup != EFFECTGROUPID_INVALID)
    {
        if (tCComplexEffect.eType != COMPLEXEFFECT_TYPE_SPECIAL_ROPE)
        {
            EffectGroupOutro(tCComplexEffect.idEffectGroup);
        }
        else
        {
            RopePlayOutro(tCComplexEffect.idEffectGroup);
        }
    }

    ACDGet(idACD).ComplexEffectUntrack(idCComplexEffect);

    sCComplexEffectDestroyInstance(idCComplexEffect);
}



void CComplexEffectRemove(const ACDID idACD, const SComplexEffectID idSComplexEffect)

{

    CComplexEffectID idCComplexEffect = sCComplexEffectTryToFindBySComplexEffectID(idACD, idSComplexEffect);


    if (idCComplexEffect != CCOMPLEXEFFECTID_INVALID)
    {
        sCComplexEffectRemoveByID(idACD, idCComplexEffect);
    }
}




void CComplexEffectRemoveAll(ActorCommonData & tACD)

{
    ComplexEffectIDListPooled & tListComplexEffects = tACD.tListComplexEffects;

    POS pos = tListComplexEffects.GetHeadPosition();
    while (pos)
    {
        CComplexEffectID idCComplexEffect = tListComplexEffects.GetNext(pos);
        CComplexEffect & tCComplexEffect = sCComplexEffectGet(idCComplexEffect);

        CComplexEffectRemove(tACD.id, tCComplexEffect.idSComplexEffect);
    }

}



void sUpdateBeamTarget(ACDID idACDSource, ACDID idACDDest)


{
    RActorID idRActorDest = (RActorID)ACDGetOwner(idACDDest);
    RActorID idRActorSource = (RActorID)ACDGetOwner(idACDSource);
    RActor * ptRActorDest = RActorTryToGet(idRActorDest);
    RActor * ptRActorSource = RActorTryToGet(idRActorSource);
    if (ptRActorDest && ptRActorSource)
    {
        ActorCommonData * ptACDSource = ACDTryToGet(idACDSource);
        ActorCommonData * ptACDDest = ACDTryToGet(idACDDest);
        if (!ptACDSource || !ptACDDest)
        {
            return;
        }

        WorldLocation & locSource = ptACDSource->GetWorldLocation();
        WorldLocation & locDest = ptACDDest->GetWorldLocation();
        Vector3D vSrc(locSource.transform.wp);
        Vector3D vDest(locDest.transform.wp);
        Vector3D vDelta = vDest - vSrc;
        float flMagSq = vDelta.MagnitudeSquared();
        if (flMagSq > 1e-12f)
        {
            Angle aYawSource = ACDGetYaw(idACDSource);
            Angle aYawTarget = Math::ATan(vDelta.y, vDelta.x);

            Quaternion qRot;
            qRot.FromYaw(aYawSource - aYawTarget);
            Vector3D vNewDelta = qRot.Rotate(vDelta);
            Vector3D vNewPos = vSrc + vNewDelta;

            ActorLocation tNewLocation;
            tNewLocation.eActorLocation = 0;
            tNewLocation.tWorldLocation.flScale = locDest.flScale;
            tNewLocation.tWorldLocation.idSWorld = locDest.idSWorld;
            tNewLocation.tWorldLocation.transform = locDest.transform;
            tNewLocation.tWorldLocation.transform.q = ptACDSource->GetOrientation();
            tNewLocation.tWorldLocation.transform.wp = vNewPos;
            RActorSetACDLocationFromMessage(*ptACDDest, tNewLocation, FALSE, FALSE, TRUE);
        }
    }
}



void CComplexEffectUpdate()

{
    CGame & tCGame = CGameGet();


    TDataArrayIterator<CComplexEffect> tItCComplexEffect;
    while (tCGame.ptCComplexEffectGlobals->ptClientComplexEffects->Iterate(tItCComplexEffect))
    {
        CComplexEffect * ptCComplexEffect = tItCComplexEffect.ptElement;
        if (ptCComplexEffect)
        {
            if (ptCComplexEffect->eType == COMPLEXEFFECT_TYPE_BEAM_TARGET)
            {

                sUpdateBeamTarget(ptCComplexEffect->idACDSource, ptCComplexEffect->idACDDest);
            }
        }
    }



}
