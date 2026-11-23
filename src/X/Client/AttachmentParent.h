// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/AttachmentParent.h
// Members sit on their DWARF decl_line. The ctor body comes from the copy inlined into
// CPlayer.cpp's sGenerateParticleSystem (a call to Init at line 28); Set's from the out-of-line
// ParentData::Init and ParentData::SetKnot in AttachmentParent.cpp, which inline it.
// The copy assignment at line 23 is the implicit one.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "RMessage.h"
#include "../Common/ThreadGlobals.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/SigmaTypes.h"

class CRefString;
struct Hardpoint;
struct HardpointHash;

typedef int32 ParentType;
typedef int32 RopeID;

class ParentData {  // size 0x2c
public:

    ParentData()
    {
        Init();
    }

    void Init();  // AttachmentParent.cpp:33


    void Set(const ParentType eParentType, const DataID idParent, const int32 nBoneIndex, const PRTransform & transform, const float flAttachmentScaleFactor)
    {





        m_eParentType = eParentType;
        m_idParent = idParent;
        m_nBoneIndex = nBoneIndex;
        m_transform = transform;
        m_flAttachmentScaleFactor = flAttachmentScaleFactor;
    }

    void Set(const ParentType eParentType, const DataID idParent, const Hardpoint & tHardpoint);




    void SetKnot(const RopeID idParent, const int32 nKnotIndex);



    void SetHardpointIndex(const ParentType eParentType, const DataID idParent, const int32 nHardpointIndex);




    void SetHardpoint(const ParentType eParentType, const DataID idParent, HardpointHash szHardpointName);




    BOOL HasLocalPlayerAsParent() const;


    int32 GetLocalParentPlayerIndex() const;


    BOOL ParentIsValid() const;


    void GetTransform(PRTransform & transform);


    float GetAlpha() const;


    int32 RegisterParentObject() const;


    CRefString GetDebugName() const;


    float GetScale(const PRTransform & transform) const;


    ParentType m_eParentType;  // 0x00
    DataID m_idParent;  // 0x04
    int32 m_nBoneIndex;  // 0x08
    PRTransform m_transform;  // 0x0c
    float m_flAttachmentScaleFactor;  // 0x28
};
static_assert(sizeof(ParentData) == 0x2c, "ParentData");
