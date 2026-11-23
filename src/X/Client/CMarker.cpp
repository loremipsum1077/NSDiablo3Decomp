// X/Client/CMarker.cpp
// Client-side marker helpers. Every function sits on its original line.

#include "CMarker.h"
#include "EffectGroup.h"
#include "RActors/RActors.h"
#include "RActors/RActorAttachments.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/Definitions/AttachmentDefinition.h"
#include "../Common/Definitions/SceneGroupDefinition.h"
#include "../Common/Marker/Marker.h"
#include "../Common/Marker/SmartMarker.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/SNO/SNOName.h"
#include "../Sigma/DataStructures/ChainedDataArrayTemplated.h"




































void MarkerInstance::DeleteRActors()
{
    // :56
    if (idPlaceholderRActor != RACTORID_INVALID)
    {
        RActorDelete(idPlaceholderRActor);
    }
}




void MarkersDeleteForWorld(
    SWorldID idSWorld)
{


    GameCommonData & tGCD = GameGetActiveGameCommonData();
    MarkerGlobals * ptMarkerGlobals = tGCD.ptMarkerGlobals;

    if (ptMarkerGlobals)
    {




        if (ptMarkerGlobals->ptMarkerInstances)
        {
            TDataArrayIterator<MarkerInstance> tIt;
            while (ptMarkerGlobals->ptMarkerInstances->Iterate(tIt))
            {
                MarkerInstance * pMarkerInstance = tIt.ptElement;

                pMarkerInstance->DeleteRActors();

                ptMarkerGlobals->ptMarkerInstances->ElementDeleteByID(pMarkerInstance->id);
            }
        }


        if (ptMarkerGlobals->ptMarkerInstancesBySet)
        {
            sFreeAllRelatedMarkerInstances(ptMarkerGlobals->ptMarkerInstancesBySet);
        }
    }
}

















































































































































AttachmentID MarkerAddAttachmentsToPlaceholder(
    RActorID idRActorPlaceholder,
    MarkerType eMarkerType,
    SNO snoMarkerSource)
{
    if (eMarkerType == MARKERTYPE_EFFECTGROUP)
    {
        TriggerEvent tEffectEvent;
        TriggerEventGetDefaultData(tEffectEvent);
        tEffectEvent.dwTriggerFlags = 0;
        tEffectEvent.eType = TRIGGEREDEVENTTYPE_ADD_EFFECT_GROUP;
        tEffectEvent.snoname.Set(EFFECTGROUP_SNO, snoMarkerSource);
        tEffectEvent.dwID = 0;


        RActorID idRActors[3] = { idRActorPlaceholder, RACTORID_INVALID, RACTORID_INVALID };


        return EffectGroupAttachmentAdd(idRActorPlaceholder, (const DataID *)idRActors, 0, tEffectEvent);
    }





    AttachmentType eAttachmentType = SmartMarker::GetAttachmentType(eMarkerType, snoMarkerSource);

    if (eAttachmentType == -1)
    {
        return ATTACHMENTID_INVALID;
    }

    SNO snoAttachment = snoMarkerSource;

    if (eMarkerType == MARKERTYPE_SCENEGROUP)
    {
        ScopedSceneGroupDefinitionLockR ptSceneGrp(snoMarkerSource);

        snoAttachment = ptSceneGrp->arScenes[0].snoScene;
    }

    RActor * ptRActorPlaceholder = RActorGet(idRActorPlaceholder);
    TriggerEvent tEvent;
    TriggerEventInit(tEvent);
    tEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
    tEvent.eAttachmentType = eAttachmentType;
    tEvent.snoname.Set(TriggerEventGetSnoGroup(tEvent), snoAttachment);

    return RActorSendRMessage(*ptRActorPlaceholder, tEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
}




DataID MarkerCreateForClientGame(
    const SWorldID idSWorld,
    const SSceneID idSScene,
    const PRTransform & transform,
    const Marker & tMarker,
    const InactiveMarkerOwner & tMarkerOwner)
{



    SmartMarker sMarker(tMarker);
    SNOName snonameMarker(sMarker.GetSNOGroup(), sMarker.GetSNOHandle());

    if (snonameMarker.snoGroup != SNOGROUP_NONE &&
        (snonameMarker.snoHandle == SNO_NONE || !SNOFileExists(snonameMarker)))
    {
        return DATAID_INVALID;
    }








    return sMarker.CreateForClientGame(idSWorld, idSScene, transform, tMarkerOwner);
}
