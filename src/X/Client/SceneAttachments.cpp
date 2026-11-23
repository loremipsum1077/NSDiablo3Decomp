// Original file: X/Client/SceneAttachments.cpp

#include "Scene.h"
#include "SceneAttachments.h"
#include "Attachments.h"
#include "Definitions/AppearanceDefinition.h"
#include "Definitions/CAttachmentDefinition.h"
#include "../Common/Definitions/SceneDefinition.h"
#include "../Sigma/MemoryPools.h"

static void sCSceneHandleRMsg(const DataID idOwner, RMessage & msg);





















void CSceneAttachmentsInitialize(CScene * ptCScene)
{
    ptCScene->tAttachmentList.SetPool(MemoryPoolsGetDATAIDListPool());
    ptCScene->tRMsgList.Initialize(ptCScene->id, sCSceneHandleRMsg);
}




void CSceneAttachmentsDispose(CScene * ptCScene)
{
    AttachmentListHandleParentDeletion(ptCScene->tAttachmentList);
    ptCScene->tRMsgList.Dispose();
}




static void sCSceneGetHardpointOrientation(const CScene * ptCScene, const Hardpoint & tHardpoint, PRTransform & transform)
{



    transform = ptCScene->tSCD.transformWorld * tHardpoint.transform;
}




























BOOL CSceneGetHardpointTransformByLabel(const CSceneID idCScene, HardpointHash szHardpointLabel, PRTransform & transform)
{


    const CScene * ptCScene = CSceneGet(idCScene);
    ScopedSceneDefinitionLockR pdefScene(ptCScene->tSCD.snoScene);
    ScopedAppearanceDefinitionLockR pdefAppearance(pdefScene->GetAppearance());
    const Structure & tStructure = pdefAppearance->tStructure;
    const Hardpoint * ptHardpoint = NULL;

    for (int32 i = 0; i < tStructure.nHardpointCount; i++)
    {
        const Hardpoint & tCurrentHardpoint = tStructure.ptHardpoints[i];
        if (szHardpointLabel == tCurrentHardpoint.tHash)
        {
            ptHardpoint = &tCurrentHardpoint;
            break;
        }
    }

    if (ptHardpoint != NULL)
    {
        sCSceneGetHardpointOrientation(ptCScene, *ptHardpoint, transform);
        return TRUE;
    }


    return FALSE;
}




static AttachmentID sCSceneAttachmentCreate(const CSceneID idCScene, RMessage & msg)
{


    CScene * ptCScene = CSceneTryToGet(idCScene);
    if (ptCScene == NULL)
    {
        return ATTACHMENTID_INVALID;
    }

    if (msg.tHardpoints[0].tHash == 0u)
    {
        return ATTACHMENTID_INVALID;
    }



    PRTransform transform;
    sCSceneGetHardpointOrientation(ptCScene, msg.tHardpoints[0], transform);

    ParentData tParentData;
    tParentData.Set(PARENT_SCENE, idCScene, msg.tHardpoints[0]);

    return AttachmentCreate(msg, tParentData, transform, 1.0f, FALSE);
}




static void sResolveHardpointLinks(const CSceneID idCScene, RMessage & msg)
{


    const CScene * ptCScene = CSceneGet(idCScene);


    for (int32 i = 0; i < MAX_EVENT_HARDPOINT_LINKS; i++)
    {


        ScopedSceneDefinitionLockR pdefScene(ptCScene->tSCD.snoScene);
        ScopedAppearanceDefinitionLockR pdefAppearance(pdefScene->GetAppearance());






        int32 nHardpointIndex = HardpointIndexFromName(pdefScene->GetAppearance(), msg.tTriggerEvent.tHardpointLinks[i].tHash);

        if (nHardpointIndex != -1)
        {
            msg.tHardpoints[i] = pdefAppearance->tStructure.ptHardpoints[nHardpointIndex];
        }
        else
        {
            msg.tHardpoints[i].transform = cgtPRTransformIdentity;
            msg.tHardpoints[i].nBoneIndex = -1;
            msg.tHardpoints[i].tHash = 0u;
        }
    }
}




static AttachmentID sHandleMsg(const DataID id, RMessage & msg)
{

    const CSceneID idCScene = (CSceneID)id;
    CScene * ptCScene = CSceneGet(idCScene);

    const TriggerConditions & tConditions = msg.tTriggerEvent.tConditions;

    if (tConditions.dwRepeatTimeMin != 0 || tConditions.dwRepeatTimeDelta != 0)
    {

        BOOL bRemote = (msg.tTriggerEvent.dwTriggerFlags & (1 << TRIGGERFLAG_DISABLED_IF_REMOTE_PLAYER)) != 0;
        ptCScene->tRMsgList.SendMessage(PARENT_SCENE, id, TRUE, msg.tTriggerEvent, 0.0f, bRemote);
    }

    sResolveHardpointLinks(idCScene, msg);

    AttachmentID idAttachment = ATTACHMENTID_INVALID;
    switch (msg.tTriggerEvent.eType)
    {
    case TRIGGEREDEVENTTYPE_ADD_OBJECT:
        idAttachment = sCSceneAttachmentCreate(idCScene, msg);
        break;





    case TRIGGEREDEVENTTYPE_DETACH_OBJECT:
        AttachmentFindAndDetach(ptCScene->tAttachmentList, msg.tTriggerEvent.dwID);
        break;

    case TRIGGEREDEVENTTYPE_DELETE_OBJECT:
        AttachmentFindAndDelete(ptCScene->tAttachmentList, msg.tTriggerEvent.dwID, FALSE);
        break;

    case TRIGGEREDEVENTTYPE_OUTRO_OBJECT:
        AttachmentFindAndDelete(ptCScene->tAttachmentList, msg.tTriggerEvent.dwID, TRUE);
        break;

    default:
        break;
    }


    return idAttachment;
}






static void sCSceneHandleRMsg(const DataID idOwner, RMessage & msg)
{


    sHandleMsg(idOwner, msg);
}




void CSceneAttachmentsOnMsg(CScene * ptCScene, const uint32 dwMsgKey)
{




    ScopedSceneDefinitionLockR pdefScene(ptCScene->tSCD.snoScene);

    for (int32 i = 0; i < pdefScene->nMsgTriggeredEventCount; i++)
    {
        const MsgTriggeredEvent & tEvent = pdefScene->ptMsgTriggeredEvents[i];
        if (tEvent.dwMsgKey == dwMsgKey)
        {
            ptCScene->tRMsgList.SendMessage(PARENT_SCENE, ptCScene->id, FALSE, tEvent.tTriggerEvent, 0.0f, FALSE);
        }
    }



}




inline AttachmentID CSceneSendRMessage(CScene * ptCScene, const TriggerEvent & tTriggerEvent)
{


    RMessage msg;
    if (!msg.Initialize(tTriggerEvent, 0.0f, PARENT_SCENE, ptCScene->id, -1.0f, FALSE, FALSE, 0.0f))
    {
        return ATTACHMENTID_INVALID;
    }

    return sHandleMsg(ptCScene->id, msg);
}




AttachmentID CSceneAddAttachment(const AttachmentType eAttachmentType, const CSceneID idCScene, const SNO sno, const uint32 dwAttachmentID, HardpointHash szHardpointName)
{





    CScene * ptCScene = CSceneTryToGet(idCScene);
    if (ptCScene == NULL)
    {
        return ATTACHMENTID_INVALID;
    }

    TriggerEvent tTriggerEvent;
    TriggerEventInit(tTriggerEvent);
    tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
    tTriggerEvent.eAttachmentType = eAttachmentType;
    tTriggerEvent.snoname.Set(TriggerEventGetSnoGroup(tTriggerEvent), sno);
    tTriggerEvent.dwID = dwAttachmentID;
    tTriggerEvent.tHardpointLinks[0].tHash = szHardpointName;

    return CSceneSendRMessage(ptCScene, tTriggerEvent);
}
