// X/Client/SceneAttachments.h -- provisional owning header.
//
// The 2.6.2 DWARF attributes these entry points to X/Client/SceneAttachments.cpp
// (build/dwarf/headers/X/Client/SceneAttachments.cpp:8-18); Scene.h only attests
// CSceneGet and CSceneTryToGet. CSceneSendRMessage is defined inline in the .cpp,
// is only ever inlined there and has no functions.csv entry, so it is not declared.

#pragma once

#include "Scene.h"
#include "../Common/CommonTypes.h"
#include "../Common/Definitions/AttachmentDefinition.h"

// X/Client/SceneAttachments.cpp
void CSceneAttachmentsInitialize(CScene * ptCScene);                                                                                     // line 31
void CSceneAttachmentsDispose(CScene * ptCScene);                                                                                        // line 41
BOOL CSceneGetHardpointTransformByLabel(const CSceneID idCScene, HardpointHash szHardpointLabel, PRTransform & transform);               // line 86
void CSceneAttachmentsOnMsg(CScene * ptCScene, const uint32 dwMsgKey);                                                                   // line 250
AttachmentID CSceneAddAttachment(const AttachmentType eAttachmentType, const CSceneID idCScene, const SNO sno, const uint32 dwAttachmentID, HardpointHash szHardpointName);  // line 290
