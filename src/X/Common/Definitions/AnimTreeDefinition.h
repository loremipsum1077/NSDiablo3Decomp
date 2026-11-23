// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/AnimTreeDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "AnimationDefinition.h"
#include "AttachmentDefinition.h"
#include "Definitions.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
struct PRSTransform;
class XMemoryAllocatorInterface;

enum {
    ATN_LEAF_BRANCH_BIT = 31,
    ATN_MAX_CHILDREN = 16,
    ATN_MAX_BLEND_CASES = 16,
};

enum {
    ANT_ANIMATION = 0,
    ANT_PIECEWISE_LINEAR_BLEND = 1,
    ANT_BONE_WEIGHTED_BLEND = 2,
    ANT_SWITCH_BLEND = 3,
    ANT_POSE = 4,
    ANT_ADDITIVE_BLEND = 5,
    ANT_BONE_WEIGHTS_LINEAR_BLEND = 6,
    ANT_BONE_WEIGHTS_MULTIPLY_BLEND = 7,
    ANT_MAX = 8,
};

typedef int32 AnimNodeType;

enum {
    ACP_FORWARD_SPEED = 0,
    ACP_TURN_SPEED = 1,
    ACP_IS_IDLE = 2,
    ACP_WEAPON_CLASS = 3,
    ACP_WALK_SLOW_SPEED = 4,
    ACP_WALK_SPEED = 5,
    ACP_RUN_SPEED = 6,
    ACP_SPRINT_SPEED = 7,
    ACP_IN_TOWN = 8,
    ACP_IS_SPECIAL_MOVE = 9,
    ACP_ABS_TURN_SPEED = 10,
    ACP_AIM_YAW = 11,
    ACP_AIM_BLEND = 12,
    ACP_IS_ALIVE = 13,
    ACP_IS_TURNING = 14,
    ACP_FORCE_WALK = 15,
    ACP_MAX = 16,
    ACP_NONE = 17,
};

typedef int32 AnimControlParam;

enum {
    ATU_CONTINUOUS = 0,
    ATU_INACTIVE_RESET = 1,
    ATU_MAX = 2,
};

typedef int32 AnimTreeUpdateMode;

struct AnimBoneStack {  // size 0x18
    uint32 dwSize;  // 0x00
    uint32 dwTop;  // 0x04
    PRSTransform * ptTransform;  // 0x08
    float * pflWeight;  // 0x10
};
static_assert(sizeof(AnimBoneStack) == 0x18, "AnimBoneStack");

struct AnimTreeLayerName {  // size 0x4
    uint32 dwHash;  // 0x00
};
static_assert(sizeof(AnimTreeLayerName) == 0x4, "AnimTreeLayerName");

struct AnimTreeBlendRamp {  // size 0x18
    float flRampInTimeSec;  // 0x00
    float flRampOutTimeSec;  // 0x04
    float flDurationSec;  // 0x08
    float flBlendStart;  // 0x0c
    float flBlendMid;  // 0x10
    float flBlendEnd;  // 0x14
};
static_assert(sizeof(AnimTreeBlendRamp) == 0x18, "AnimTreeBlendRamp");

struct AnimTreeLeaf {  // size 0x24
    AnimNodeType eNodeType;  // 0x00
    SNO snoAnim;  // 0x04
    uint32 dwTagAnimation;  // 0x08
    float flPoseFrame;  // 0x0c
    float flPlaySpeed;  // 0x10
    int32 nSyncGroupIndex;  // 0x14
    AnimTreeUpdateMode eUpdateMode;  // 0x18
    AnimBoneWeightSet eBoneWeightSet;  // 0x1c
    AnimTreeLayerName tLayerName;  // 0x20
};
static_assert(sizeof(AnimTreeLeaf) == 0x24, "AnimTreeLeaf");

struct AnimTreeBlendCase {  // size 0xc
    AnimControlParam eParam;  // 0x00
    int32 nLiteral;  // 0x04
    float flLiteral;  // 0x08
};
static_assert(sizeof(AnimTreeBlendCase) == 0xc, "AnimTreeBlendCase");

struct AnimTreeNode {  // size 0x12c
    AnimNodeType eNodeType;  // 0x00
    uint32 dwNumChildren;  // 0x04
    uint32 pdwChildren[16];  // 0x08
    AnimControlParam eBlendParam;  // 0x48
    AnimTreeBlendCase tBlendCase[16];  // 0x4c
    AnimTreeBlendRamp tBlendRamp;  // 0x10c
    int32 nSyncGroupIndex;  // 0x124
    AnimTreeLayerName tLayerName;  // 0x128
};
static_assert(sizeof(AnimTreeNode) == 0x12c, "AnimTreeNode");

struct AnimTreeControlParam {  // size 0x8
    float flValue;  // 0x00
    int32 nValue;  // 0x04
};
static_assert(sizeof(AnimTreeControlParam) == 0x8, "AnimTreeControlParam");

struct AnimTreeControl {  // size 0x90
    AnimTreeControlParam tParam[16];  // 0x00
    OutroAnimationType eWalkOutro;  // 0x80
    float flWalkOutroTimer;  // 0x84
    OutroAnimationType eRunOutro;  // 0x88
    float flRunOutroTimer;  // 0x8c
};
static_assert(sizeof(AnimTreeControl) == 0x90, "AnimTreeControl");

struct AnimTreeDefinition {  // size 0x60
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwMachineTime;  // 0x0c
    uint32 dwFlags;  // 0x10
    uint32 dwNumLeafs;  // 0x14
    SerializeData serLeafTable;  // 0x18
    XPtr64<AnimTreeLeaf> ptLeaf;  // 0x20
    uint32 dwNumNodes;  // 0x28
    SerializeData serNodeTable;  // 0x2c
    XPtr64<AnimTreeNode> ptNode;  // 0x38
    uint32 dwNumSyncGroups;  // 0x40
    SerializeData serSyncGroupTable;  // 0x44
    XPtr64<int> ptSyncGroup;  // 0x50
    uint32 dwRootNode;  // 0x58
};
static_assert(sizeof(AnimTreeDefinition) == 0x60, "AnimTreeDefinition");

class AnimTreeGroupDef : public GroupDefinition {  // size 0xa0, line 176
public:
    AnimTreeGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 181; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 186
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 188
        return SNOPACKTYPE_COREDATA;
    }
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(AnimTreeGroupDef) == 0xa0, "AnimTreeGroupDef");

extern AnimTreeGroupDef * gptGroupDefAnimTree;  // AnimTreeDefinition.cpp:101

typedef ScopedDefinitionLockR<AnimTreeDefinition> ScopedAnimTreeDefinitionLockR;

// Line 234. Its callers (RActorAnimTreeLayerIsActive and others) call LockRFast and have
// nothing from an unlock.
template <> inline const AnimTreeDefinition * DefinitionLockR<AnimTreeDefinition>(SNO sno) {  // line 234
    return (const AnimTreeDefinition *)gptGroupDefAnimTree->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<AnimTreeDefinition>(const AnimTreeDefinition *) {}


// Functions defined in this file (bodies are not in the DWARF):
//   line   176  AnimTreeGroupDef::~AnimTreeGroupDef()  [0x710042a4a0]
//   line   181  void * AnimTreeGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x710042a4d0 (+1 more)]
//   line   181  void AnimTreeGroupDef::MemFree(void *& ptr) const  [0x710042a520 (+1 more)]
//   line   186  BOOL AnimTreeGroupDef::CanReferenceOtherSNOs() const  [0x710042a5b0]
//   line   188  SNOPackType AnimTreeGroupDef::GetSNOPackDestination(const void *) const  [0x710042a5a0]
//   line   234  const AnimTreeDefinition * DefinitionLockR<AnimTreeDefinition>(SNO sno)  [inlined into 22 functions]
