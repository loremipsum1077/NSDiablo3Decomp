// Original file: X/Common/Clusters.cpp

#include "Clusters.h"
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../Common/SNO/SNOName.h"
#include "../Common/WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;  // X/Sigma/Files/XFilesCommon.cpp:20
extern const XTypeDescriptor * DT_SNO;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp

#include "../Sigma/RefString.h"
#include <string.h>

// line 147
void SceneCluster::GetName(CRefString & strName) const
{
    if (strlen(szName) > 0)
    {
        strName = szName;
    }
    else
    {
        strName.Format("Cluster %d", uClusterID);
    }
}

// The field names are "" in the binary.
static const TypeDescriptorField SubSceneLabel_Fields[3] = {
    TypeDescriptorField("", DT_GBID, offsetof(SubSceneLabel, gbidLabel), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SubSceneLabel, nWeight), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SubSceneLabel), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SubSceneLabel("SubSceneLabel", 0, SubSceneLabel_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SUBSCENE_LABEL = &TypeDescriptor_SubSceneLabel;

static const TypeDescriptorField SubSceneEntry_Fields[6] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(SubSceneEntry, snoScene), "", SCENE_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(SubSceneEntry, nWeight), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SubSceneEntry, nNumLabels), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SubSceneEntry, serLabels), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SUBSCENE_LABEL, offsetof(SubSceneEntry, arLabels), 0x21, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SubSceneEntry), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SubSceneEntry("SubSceneEntry", 0, SubSceneEntry_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SUBSCENE_ENTRY = &TypeDescriptor_SubSceneEntry;

static const TypeDescriptorField SubSceneGroup_Fields[6] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(SubSceneGroup, snoCondition), "", CONDITION_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(SubSceneGroup, nNumScenesToChoose), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SubSceneGroup, nNumSubScenes), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SubSceneGroup, serSubScenes), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SUBSCENE_ENTRY, offsetof(SubSceneGroup, arSubScenes), 0x21, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SubSceneGroup), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SubSceneGroup("SubSceneGroup", 0, SubSceneGroup_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SUBSCENE_GROUP = &TypeDescriptor_SubSceneGroup;

static const TypeDescriptorField SceneCluster_Fields[7] = {
    CharArrayTypeDescriptorField("", offsetof(SceneCluster, szName), "", 1, 0x80, -1),
    TypeDescriptorField("", DT_INT, offsetof(SceneCluster, uClusterID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SceneCluster, nNumSubSceneGroups), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SceneCluster, serSubSceneGroups), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SUBSCENE_GROUP, offsetof(SceneCluster, arSubSceneGroups), 0x21, 8),
    TypeDescriptorField("", DT_SUBSCENE_GROUP, offsetof(SceneCluster, tDefaultSubSceneGroup), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SceneCluster), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SceneCluster("SceneCluster", 0, SceneCluster_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SCENE_CLUSTER = &TypeDescriptor_SceneCluster;

static const TypeDescriptorField SceneClusterSet_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(SceneClusterSet, nNumClusters), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SceneClusterSet, serClusters), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SCENE_CLUSTER, offsetof(SceneClusterSet, arClusters), 0x21, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SceneClusterSet), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SceneClusterSet("SceneClusterSet", 0, SceneClusterSet_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SCENE_CLUSTER_SET = &TypeDescriptor_SceneClusterSet;

