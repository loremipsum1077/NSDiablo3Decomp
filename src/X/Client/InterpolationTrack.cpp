// Original file: X/Client/InterpolationTrack.cpp

#include "InterpolationTrack.h"

#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Random.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

// Not used directly: the original reaches these through an include chain, and their header-level
// statics (blz::ignore, SNONAME_NONE, cgplaceNull, the GfxVB.h normals) are part of this file's
// _GLOBAL__sub_I.
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../Common/SNO/SNOName.h"
#include "../Common/WorldPlace.h"
#include "Gfx/GfxVB.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Client/InterpolationScalar.cpp:31; no adopted header declares it yet.
extern const XTypeDescriptor * DT_INTERPOLATION_SCALAR;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares them yet.
extern const XTypeDescriptor * DT_TIME;
extern const XTypeDescriptor * DT_VELOCITY;
extern const XTypeDescriptor * DT_ACCEL;
extern const XTypeDescriptor * DT_VELOCITYVECTOR3D;
extern const XTypeDescriptor * DT_ACCELVECTOR3D;
extern const XTypeDescriptor * DT_ANGULARVELOCITY;

enum {
    INTERPOLATION_PATH_XML_VERSION = 1,
};

typedef IntPath TimePath;
typedef IntNode TimeNode;
typedef FloatPath VelocityPath;
typedef FloatNode VelocityNode;
typedef FloatPath AccelPath;
typedef FloatNode AccelNode;
typedef VectorPath VelocityVectorPath;
typedef VectorNode VelocityVectorNode;
typedef VectorPath AccelVectorPath;
typedef VectorNode AccelVectorNode;
typedef AnglePath AngularVelocityPath;
typedef AngleNode AngularVelocityNode;

InterpolationPathHeader::InterpolationPathHeader(const int32 nNodeCount_)
    : nNodeCount(nNodeCount_), tLoopPointStart(0.0f), tLoopPointEnd(1.0f), nDistributionFunction(DISTRIBUTION_FUNCTION_UNIFORM),
      tInterpScalar(cgInterpolationScalarDefault) {
}

// Invented name. Every path node type registers the same fatal LoadFromDefaultData hook; the
// original also emits a VersionPath<type> function from the same line, which nothing kept.
#define DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(TYPE, LINE)                                \
    void LoadFromDefaultData##TYPE(const TypeDescriptorField &, void *) {                  \
        DisplayInternalError(NULL, "", LINE, ERROR_INTERNAL_FATAL);                        \
        ExitProgram(ERROR_INTERNAL_FATAL);                                                 \
    }

static const TypeDescriptorField InterpolationPathHeader_Fields[6] = {  // line 120
    TypeDescriptorField("", DT_INT, offsetof(InterpolationPathHeader, nNodeCount), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(InterpolationPathHeader, tLoopPointStart), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(InterpolationPathHeader, tLoopPointEnd), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(InterpolationPathHeader, nDistributionFunction), NULL, 1),
    TypeDescriptorField("", DT_INTERPOLATION_SCALAR, offsetof(InterpolationPathHeader, tInterpScalar), &cgInterpolationScalarDefault, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(InterpolationPathHeader), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InterpolationPathHeader("InterpolationPathHeader", 0, InterpolationPathHeader_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_INTERPOLATION_PATH_HEADER = &TypeDescriptor_InterpolationPathHeader;  // line 126

// Invented name: the node and path descriptors of every path type follow one pattern.
#define DEFINE_PATH_NODE_FIELDS(NAME, NODE, VALUETYPE)                                              \
    static const TypeDescriptorField NAME##Node_Fields[4] = {                                       \
        TypeDescriptorField("", VALUETYPE, offsetof(NODE, tValueMin), NULL, 1),                     \
        TypeDescriptorField("", VALUETYPE, offsetof(NODE, tValueMax), NULL, 1),                     \
        TypeDescriptorField("", DT_FLOAT, offsetof(NODE, tTime), NULL, 1),                          \
        TypeDescriptorField(NULL, DT_NULL, sizeof(NODE), NULL, 0),                                  \
    };

#define DEFINE_PATH_FIELDS(NAME, PATH, NODEDT)                                                      \
    static const TypeDescriptorField NAME##Path_Fields[4] = {                                       \
        TypeDescriptorField("", DT_INTERPOLATION_PATH_HEADER, offsetof(PATH, tHeader), NULL, 1),    \
        TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PATH, serNodes), NULL, 0),               \
        VariableArrayTypeDescriptorField("", NODEDT, offsetof(PATH, ptArrayNodes), 0x21,            \
                                         (int32)offsetof(PATH, serNodes) - (int32)offsetof(PATH, ptArrayNodes)), \
        TypeDescriptorField(NULL, DT_NULL, sizeof(PATH), NULL, 0),                                  \
    };

DEFINE_PATH_NODE_FIELDS(Float, FloatNode, DT_FLOAT)  // line 130
ComplexTypeDescriptor TypeDescriptor_FloatNode("FloatNode", 0, FloatNode_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_FLOAT_NODE = &TypeDescriptor_FloatNode;  // line 134
DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(DT_FLOAT_NODE, 136)
DEFINE_PATH_FIELDS(Float, FloatPath, DT_FLOAT_NODE)  // line 138
ComplexTypeDescriptor TypeDescriptor_FloatPath("FloatPath", INTERPOLATION_PATH_XML_VERSION, FloatPath_Fields, 4, NULL, 0x80, NULL, NULL, NULL, LoadFromDefaultDataDT_FLOAT_NODE); const XTypeDescriptor * DT_FLOAT_PATH = &TypeDescriptor_FloatPath;  // line 141

const InterpolationScalar cgInterpolationScalarDefault(SCALAR_FUNCTION_NONE, 0.0f, 1.0f);  // line 143

static InterpolationNode<float> sgpathNullFloatPathNodes[1] = {  // line 147
    { 0.0f, 0.0f, 0.0f },
};
const FloatPath cgpathNullFloatPath(InterpolationPathHeader(1), sgpathNullFloatPathNodes);  // line 152

static InterpolationNode<float> sgpathUnitFloatPathNodes[1] = {  // line 157
    { 1.0f, 1.0f, 0.0f },
};
const FloatPath cgpathUnitFloatPath(InterpolationPathHeader(1), sgpathUnitFloatPathNodes);  // line 162

DEFINE_PATH_NODE_FIELDS(Int, IntNode, DT_INT)  // line 169
ComplexTypeDescriptor TypeDescriptor_IntNode("IntNode", 0, IntNode_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_INT_NODE = &TypeDescriptor_IntNode;  // line 173
DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(DT_INT_NODE, 175)
DEFINE_PATH_FIELDS(Int, IntPath, DT_INT_NODE)  // line 177
ComplexTypeDescriptor TypeDescriptor_IntPath("IntPath", INTERPOLATION_PATH_XML_VERSION, IntPath_Fields, 4, NULL, 0x80, NULL, NULL, NULL, LoadFromDefaultDataDT_INT_NODE); const XTypeDescriptor * DT_INT_PATH = &TypeDescriptor_IntPath;  // line 180

static InterpolationNode<int> sgpathNullIntPathNodes[1] = {  // line 182
    { 0, 0, 0.0f },
};
const IntPath cgpathNullIntPath(InterpolationPathHeader(1), sgpathNullIntPathNodes);  // line 187

static InterpolationNode<int> sgpathUnitIntPathNodes[1] = {  // line 192
    { 1, 1, 0.0f },
};
const IntPath cgpathUnitIntPath(InterpolationPathHeader(1), sgpathUnitIntPathNodes);  // line 197

DEFINE_PATH_NODE_FIELDS(Vector, VectorNode, DT_VECTOR3D)  // line 204
ComplexTypeDescriptor TypeDescriptor_VectorNode("VectorNode", 0, VectorNode_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_VECTOR_NODE = &TypeDescriptor_VectorNode;  // line 208
DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(DT_VECTOR_NODE, 210)
DEFINE_PATH_FIELDS(Vector, VectorPath, DT_VECTOR_NODE)  // line 212
ComplexTypeDescriptor TypeDescriptor_VectorPath("VectorPath", INTERPOLATION_PATH_XML_VERSION, VectorPath_Fields, 4, NULL, 0x80, NULL, NULL, NULL, LoadFromDefaultDataDT_VECTOR_NODE); const XTypeDescriptor * DT_VECTOR_PATH = &TypeDescriptor_VectorPath;  // line 215

static InterpolationNode<Vector3D> sgpathNullVectorPathNodes[1] = {  // line 217
    { Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f), 0.0f },
};
const VectorPath cgpathNullVectorPath(InterpolationPathHeader(1), sgpathNullVectorPathNodes);  // line 222

static InterpolationNode<Vector3D> sgpathZAxisVectorPathNodes[1] = {  // line 227
    { Vector3D(0.0f, 0.0f, 1.0f), Vector3D(0.0f, 0.0f, 1.0f), 0.0f },
};
const VectorPath cgpathZAxisVectorPath(InterpolationPathHeader(1), sgpathZAxisVectorPathNodes);  // line 232

DEFINE_PATH_NODE_FIELDS(Color, ColorNode, DT_RGBACOLOR)  // line 240
ComplexTypeDescriptor TypeDescriptor_ColorNode("ColorNode", 0, ColorNode_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_COLOR_NODE = &TypeDescriptor_ColorNode;  // line 244
DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(DT_COLOR_NODE, 246)
DEFINE_PATH_FIELDS(Color, ColorPath, DT_COLOR_NODE)  // line 248
ComplexTypeDescriptor TypeDescriptor_ColorPath("ColorPath", INTERPOLATION_PATH_XML_VERSION, ColorPath_Fields, 4, NULL, 0x80, NULL, NULL, NULL, LoadFromDefaultDataDT_COLOR_NODE); const XTypeDescriptor * DT_COLOR_PATH = &TypeDescriptor_ColorPath;  // line 251

static InterpolationNode<RGBAColor> sgpathNullColorPathNodes[1] = {  // line 253
    { RGBAColor(255, 255, 255, 255), RGBAColor(255, 255, 255, 255), 0.0f },
};
const ColorPath cgpathNullColorPath(InterpolationPathHeader(1), sgpathNullColorPathNodes);  // line 258

DEFINE_PATH_NODE_FIELDS(Angle, AngleNode, DT_ANGLE)  // line 265
ComplexTypeDescriptor TypeDescriptor_AngleNode("AngleNode", 0, AngleNode_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ANGLE_NODE = &TypeDescriptor_AngleNode;  // line 269
DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(DT_ANGLE_NODE, 271)
DEFINE_PATH_FIELDS(Angle, AnglePath, DT_ANGLE_NODE)  // line 273
ComplexTypeDescriptor TypeDescriptor_AnglePath("AnglePath", INTERPOLATION_PATH_XML_VERSION, AnglePath_Fields, 4, NULL, 0x80, NULL, NULL, NULL, LoadFromDefaultDataDT_ANGLE_NODE); const XTypeDescriptor * DT_ANGLE_PATH = &TypeDescriptor_AnglePath;  // line 276

static InterpolationNode<float> sgpathNullAnglePathNodes[1] = {  // line 278
    { 0.0f, 0.0f, 0.0f },
};
const AnglePath cgpathNullAnglePath(InterpolationPathHeader(1), sgpathNullAnglePathNodes);  // line 283

DEFINE_PATH_NODE_FIELDS(Time, TimeNode, DT_TIME)  // line 293
ComplexTypeDescriptor TypeDescriptor_TimeNode("TimeNode", 0, TimeNode_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TIME_NODE = &TypeDescriptor_TimeNode;  // line 297
DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(DT_TIME_NODE, 299)
DEFINE_PATH_FIELDS(Time, TimePath, DT_TIME_NODE)  // line 301
ComplexTypeDescriptor TypeDescriptor_TimePath("TimePath", INTERPOLATION_PATH_XML_VERSION, TimePath_Fields, 4, NULL, 0x80, NULL, NULL, NULL, LoadFromDefaultDataDT_TIME_NODE); const XTypeDescriptor * DT_TIME_PATH = &TypeDescriptor_TimePath;  // line 304

DEFINE_PATH_NODE_FIELDS(Velocity, VelocityNode, DT_VELOCITY)  // line 311
ComplexTypeDescriptor TypeDescriptor_VelocityNode("VelocityNode", 0, VelocityNode_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_VELOCITY_NODE = &TypeDescriptor_VelocityNode;  // line 315
DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(DT_VELOCITY_NODE, 317)
DEFINE_PATH_FIELDS(Velocity, VelocityPath, DT_VELOCITY_NODE)  // line 319
ComplexTypeDescriptor TypeDescriptor_VelocityPath("VelocityPath", INTERPOLATION_PATH_XML_VERSION, VelocityPath_Fields, 4, NULL, 0x80, NULL, NULL, NULL, LoadFromDefaultDataDT_VELOCITY_NODE); const XTypeDescriptor * DT_VELOCITY_PATH = &TypeDescriptor_VelocityPath;  // line 322

DEFINE_PATH_NODE_FIELDS(Accel, AccelNode, DT_ACCEL)  // line 328
ComplexTypeDescriptor TypeDescriptor_AccelNode("AccelNode", 0, AccelNode_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACCEL_NODE = &TypeDescriptor_AccelNode;  // line 332
DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(DT_ACCEL_NODE, 334)
DEFINE_PATH_FIELDS(Accel, AccelPath, DT_ACCEL_NODE)  // line 336
ComplexTypeDescriptor TypeDescriptor_AccelPath("AccelPath", INTERPOLATION_PATH_XML_VERSION, AccelPath_Fields, 4, NULL, 0x80, NULL, NULL, NULL, LoadFromDefaultDataDT_ACCEL_NODE); const XTypeDescriptor * DT_ACCEL_PATH = &TypeDescriptor_AccelPath;  // line 339

DEFINE_PATH_NODE_FIELDS(VelocityVector, VelocityVectorNode, DT_VELOCITYVECTOR3D)  // line 346
ComplexTypeDescriptor TypeDescriptor_VelocityVectorNode("VelocityVectorNode", 0, VelocityVectorNode_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_VELOCITYVECTOR_NODE = &TypeDescriptor_VelocityVectorNode;  // line 350
DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(DT_VELOCITYVECTOR_NODE, 352)
DEFINE_PATH_FIELDS(VelocityVector, VelocityVectorPath, DT_VELOCITYVECTOR_NODE)  // line 354
ComplexTypeDescriptor TypeDescriptor_VelocityVectorPath("VelocityVectorPath", INTERPOLATION_PATH_XML_VERSION, VelocityVectorPath_Fields, 4, NULL, 0x80, NULL, NULL, NULL, LoadFromDefaultDataDT_VELOCITYVECTOR_NODE); const XTypeDescriptor * DT_VELOCITYVECTOR_PATH = &TypeDescriptor_VelocityVectorPath;  // line 357

DEFINE_PATH_NODE_FIELDS(AccelVector, AccelVectorNode, DT_ACCELVECTOR3D)  // line 364
ComplexTypeDescriptor TypeDescriptor_AccelVectorNode("AccelVectorNode", 0, AccelVectorNode_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACCELVECTOR_NODE = &TypeDescriptor_AccelVectorNode;  // line 368
DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(DT_ACCELVECTOR_NODE, 370)
DEFINE_PATH_FIELDS(AccelVector, AccelVectorPath, DT_ACCELVECTOR_NODE)  // line 372
ComplexTypeDescriptor TypeDescriptor_AccelVectorPath("AccelVectorPath", INTERPOLATION_PATH_XML_VERSION, AccelVectorPath_Fields, 4, NULL, 0x80, NULL, NULL, NULL, LoadFromDefaultDataDT_ACCELVECTOR_NODE); const XTypeDescriptor * DT_ACCELVECTOR_PATH = &TypeDescriptor_AccelVectorPath;  // line 375

DEFINE_PATH_NODE_FIELDS(AngularVelocity, AngularVelocityNode, DT_ANGULARVELOCITY)  // line 382
ComplexTypeDescriptor TypeDescriptor_AngularVelocityNode("AngularVelocityNode", 0, AngularVelocityNode_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ANGULARVELOCITY_NODE = &TypeDescriptor_AngularVelocityNode;  // line 386
DEFINE_PATH_NODE_LOAD_FROM_DEFAULT_DATA(DT_ANGULARVELOCITY_NODE, 388)
DEFINE_PATH_FIELDS(AngularVelocity, AngularVelocityPath, DT_ANGULARVELOCITY_NODE)  // line 390
ComplexTypeDescriptor TypeDescriptor_AngularVelocityPath("AngularVelocityPath", INTERPOLATION_PATH_XML_VERSION, AngularVelocityPath_Fields, 4, NULL, 0x80, NULL, NULL, NULL, LoadFromDefaultDataDT_ANGULARVELOCITY_NODE); const XTypeDescriptor * DT_ANGULARVELOCITY_PATH = &TypeDescriptor_AngularVelocityPath;  // line 393

template <typename TYPE>
void sInsertNode(InterpolationPath<TYPE> & tPath, int32 i, const InterpolationNode<TYPE> tNode) {  // line 399
    int32 nNumNodesToMove = tPath.tHeader.nNodeCount - i;

    uint32 dwSize = (tPath.tHeader.nNodeCount + 1) * sizeof(InterpolationNode<TYPE>);

    if (tPath.ptArrayNodes == NULL) {
        tPath.ptArrayNodes = (InterpolationNode<TYPE> *)SigmaMemoryNew(dwSize, 4, NULL, FALSE);
    } else {
        tPath.ptArrayNodes = (InterpolationNode<TYPE> *)SigmaMemoryResize(tPath.ptArrayNodes, NULL, dwSize, 4);
    }

    if (nNumNodesToMove != 0) {
        SigmaMemoryMove(&tPath.ptArrayNodes[i + 1], &tPath.ptArrayNodes[i], (tPath.tHeader.nNodeCount - i) * sizeof(InterpolationNode<TYPE>));
    }

    tPath.ptArrayNodes[i] = tNode;
    tPath.tHeader.nNodeCount++;
    tPath.serNodes.dwSize = dwSize;
}

// The class templates are explicitly instantiated at the end of the file. Members that nothing
// called were stripped by the linker, so GetNode, GetIntraLoopValue, SetLinearPath,
// SetLoopPoints, the operator== family, the TickTime min/max and Vector2D/Vector3D variants and
// most ResolvedInterpolationPath members have no bytes to check; their bodies follow the inline
// trees and line numbers the DWARF keeps for them.
template <typename TYPE>
void InterpolationPath<TYPE>::GetNode(const int32 i, InterpolationNode<TYPE> & tNode) const {  // line 431
    tNode = ptArrayNodes[i];
}

template <typename TYPE>
void InterpolationPath<TYPE>::AddNode(PathTime tTime, const TYPE & tValueMin, const TYPE & tValueMax) {  // line 442
    int32 i = FindNode(tTime);

    InterpolationNode<TYPE> tNode;
    tNode.tValueMin = tValueMin;
    tNode.tValueMax = tValueMax;
    tNode.tTime = tTime;

    sInsertNode(*this, i, tNode);
}

template <typename TYPE>
void InterpolationPath<TYPE>::GetIntraLoopValue(TYPE & tValue) const {  // line 474
    for (int32 i = 0; i < tHeader.nNodeCount; i++) {
        if (ptArrayNodes[i].tTime >= tHeader.tLoopPointStart) {
            tValue = ptArrayNodes[i].tValueMin;
            return;
        }
    }
}

template <typename TYPE>
void InterpolationPath<TYPE>::SetConstantValue(const TYPE & tValue) {  // line 495
    RemoveAllNodes();
    AddNode(0.0f, tValue);
}

template <typename TYPE>
void InterpolationPath<TYPE>::SetLinearPath(const TYPE & tValue0, const TYPE & tValue1) {  // line 506
    RemoveAllNodes();
    AddNode(0.0f, tValue0);
    AddNode(1.0f, tValue1);
}

template <typename TYPE>
void InterpolationPath<TYPE>::SetLoopPoints(PathTime tLoopPointStart, PathTime tLoopPointEnd) {  // line 519
    tHeader.tLoopPointStart = tLoopPointStart;
    tHeader.tLoopPointEnd = tLoopPointEnd;
}

template <typename TYPE>
void InterpolationPath<TYPE>::RemoveAllNodes() {  // line 531
    tHeader.nNodeCount = 0;
    // Both on line 535 in the original.
    SigmaMemoryFree(ptArrayNodes, NULL); ptArrayNodes = NULL;
    serNodes.dwSize = 0;
}

template <typename TYPE>
BOOL InterpolationNode<TYPE>::operator==(const InterpolationNode<TYPE> & OtherNode) const {  // line 543
    if (tValueMin != OtherNode.tValueMin)
        return FALSE;
    if (tValueMax != OtherNode.tValueMax)
        return FALSE;
    return tTime == OtherNode.tTime;
}

inline BOOL InterpolationPathHeader::operator==(const InterpolationPathHeader & OtherPathHeader) const {  // line 558
    return nNodeCount == OtherPathHeader.nNodeCount && tLoopPointStart == OtherPathHeader.tLoopPointStart &&
           tLoopPointEnd == OtherPathHeader.tLoopPointEnd && nDistributionFunction == OtherPathHeader.nDistributionFunction &&
           tInterpScalar.nScalarFunction == OtherPathHeader.tInterpScalar.nScalarFunction &&
           tInterpScalar.flScalarLerpZero == OtherPathHeader.tInterpScalar.flScalarLerpZero &&
           tInterpScalar.flScalarLerpOne == OtherPathHeader.tInterpScalar.flScalarLerpOne;
}

template <typename TYPE>
BOOL InterpolationPath<TYPE>::operator==(const InterpolationPath<TYPE> & OtherPath) const {  // line 577
    if (!(tHeader == OtherPath.tHeader))
        return FALSE;

    for (int32 nNode = 0; nNode < tHeader.nNodeCount; nNode++) {
        if (!(ptArrayNodes[nNode] == OtherPath.ptArrayNodes[nNode]))
            return FALSE;
    }

    return TRUE;
}

template <typename TYPE>
InterpolationPath<TYPE> & InterpolationPath<TYPE>::operator=(const InterpolationPath<TYPE> & rhs) {  // line 597
    uint32 dwSize = rhs.tHeader.nNodeCount * sizeof(InterpolationNode<TYPE>);

    if (rhs.tHeader.nNodeCount > 0) {
        if (tHeader.nNodeCount != rhs.tHeader.nNodeCount) {
            if (ptArrayNodes == NULL) {
                ptArrayNodes = (InterpolationNode<TYPE> *)SigmaMemoryNew(dwSize, 4, NULL, FALSE);
            } else {
                ptArrayNodes = (InterpolationNode<TYPE> *)SigmaMemoryResize(ptArrayNodes, NULL, dwSize, 4);
            }
        }
        SigmaMemoryCopy(ptArrayNodes, rhs.ptArrayNodes, dwSize);
    } else {
        SigmaMemoryFree(ptArrayNodes, NULL); ptArrayNodes = NULL;
    }

    tHeader = rhs.tHeader;
    serNodes.dwSize = dwSize;

    return *this;
}

static void Lerp_Vector3DIndependent(const Vector3D & wvMin, const Vector3D & wvMax, const Vector3D & wvT, Vector3D & wvResult) {  // line 632
    Math::Lerp(wvMin.x, wvMax.x, wvT.x, wvResult.x);
    Math::Lerp(wvMin.y, wvMax.y, wvT.y, wvResult.y);
    Math::Lerp(wvMin.z, wvMax.z, wvT.z, wvResult.z);
}

template <typename TYPE>
static void sMultiplyInterpValueByScalar(float flScalar, TYPE & tValue);

template <>
void sMultiplyInterpValueByScalar<Vector3D>(float flScalar, Vector3D & tValue) {  // line 647
    tValue *= flScalar;
}

template <>
void sMultiplyInterpValueByScalar<int>(float flScalar, int32 & tValue) {  // line 658
    tValue = Math::FloatRoundToInt(tValue * flScalar);
}

template <>
void sMultiplyInterpValueByScalar<RGBAColor>(float flScalar, RGBAColor & tValue) {  // line 669
    tValue.r = (uint8)Math::FloatRoundToInt(tValue.r * flScalar);
    tValue.g = (uint8)Math::FloatRoundToInt(tValue.g * flScalar);
    tValue.b = (uint8)Math::FloatRoundToInt(tValue.b * flScalar);
    tValue.a = (uint8)Math::FloatRoundToInt(tValue.a * flScalar);
}

template <typename TYPE>
static void sMultiplyInterpValueByScalar(float flScalar, TYPE & tValue) {  // line 683
    tValue = flScalar * tValue;
}

// Walks a node pointer rather than indexing: after RemoveAllNodes the inlined copy in
// SetConstantValue sees a null array, and indexing lets the compiler fold the loop away.
template <typename TYPE>
int32 InterpolationPath<TYPE>::FindNode(PathTime tTime) const {  // line 694
    int32 nCount = tHeader.nNodeCount;
    InterpolationNode<TYPE> * _ptArrayNodes = ptArrayNodes;
    for (int32 i = 0; i < nCount; i++, _ptArrayNodes++) {
        if (_ptArrayNodes->tTime > tTime)
            return i;
    }
    return nCount;
}

template <typename TYPE>
int32 ResolvedInterpolationPath<TYPE>::FindNode(PathTime tTime, InterpCursor & tInterpCursor) const {  // line 713
    int32 nCount = tHeader.nNodeCount;
    ResolvedInterpolationNode<TYPE> * _ptArrayNodes = ptArrayNodes;
    if (tInterpCursor.tLastTime <= tTime) {
        int32 i = tInterpCursor.nLastNode + 1;
        for (_ptArrayNodes += i; i < nCount; i++, _ptArrayNodes++) {
            if (_ptArrayNodes->tTime > tTime)
                return i;
        }
    } else {
        for (int32 i = 0; i < nCount; i++, _ptArrayNodes++) {
            if (_ptArrayNodes->tTime > tTime)
                return i;
        }
    }
    return nCount;
}

template <typename TYPE>
int32 ResolvedInterpolationPath<TYPE>::FindNode(PathTime tTime) const {  // line 747
    int32 nCount = tHeader.nNodeCount;
    ResolvedInterpolationNode<TYPE> * _ptArrayNodes = ptArrayNodes;
    for (int32 i = 0; i < nCount; i++, _ptArrayNodes++) {
        if (_ptArrayNodes->tTime > tTime)
            return i;
    }
    return nCount;
}

float InterpolationPathHeader::GetInterpolantFromDistributionFunction(RNG & rng) const {  // line 765
    float flUniform = RandFloat_Internal(rng);

    switch (nDistributionFunction) {
        case DISTRIBUTION_FUNCTION_UNIFORM:
            return flUniform;

        case DISTRIBUTION_FUNCTION_LOW: {
            float flResult = flUniform * flUniform;
            return Math::Pin(flResult, 0.0f, 1.0f);
        }

        case DISTRIBUTION_FUNCTION_MID: {
            float flCenter = flUniform - 0.5f;
            float flCurve = flCenter * flCenter * 2.0f;
            float flResult = (flCenter < 0.0f ? -flCurve : flCurve) + 0.5f;
            return Math::Pin(flResult, 0.0f, 1.0f);
        }

        case DISTRIBUTION_FUNCTION_HIGH: {
            float flResult = flUniform * flUniform;
            flResult = 1.0f - flResult;
            return Math::Pin(flResult, 0.0f, 1.0f);
        }

        case DISTRIBUTION_FUNCTION_LOW_AND_HIGH: {
            float flCenter = flUniform - 0.5f;
            float flCurve = flCenter * flCenter * 2.0f;
            float flResult;
            if (flCenter < 0.0f) {
                flResult = flCurve;
            } else {
                flResult = 1.0f - flCurve;
            }
            return Math::Pin(flResult, 0.0f, 1.0f);
        }

        case DISTRIBUTION_FUNCTION_SMOOTH_LOW: {
            float flResult = Math::Sqrt(flUniform);
            flResult = 1.0f - flResult;
            return Math::Pin(flResult, 0.0f, 1.0f);
        }

        case DISTRIBUTION_FUNCTION_SMOOTH_MID: {
            float flCenter = flUniform - 0.5f;
            float flCurve = Math::Sqrt(Math::Abs(flCenter) * 2.0f) * 0.5f;
            float flResult;
            if (flCenter < 0.0f) {
                flResult = flCurve;
            } else {
                flResult = 1.0f - flCurve;
            }
            return Math::Pin(flResult, 0.0f, 1.0f);
        }

        case DISTRIBUTION_FUNCTION_SMOOTH_HIGH: {
            float flResult = Math::Sqrt(flUniform);
            return Math::Pin(flResult, 0.0f, 1.0f);
        }

        case DISTRIBUTION_FUNCTION_SMOOTH_LOW_AND_HIGH: {
            float flCenter = flUniform - 0.5f;
            float flCurve = Math::Sqrt(Math::Abs(flCenter) * 2.0f) * 0.5f;
            float flResult = (flCenter < 0.0f ? -flCurve : flCurve) + 0.5f;
            return Math::Pin(flResult, 0.0f, 1.0f);
        }
    }

    return 0.0f;
}

void InterpolationPathHeader::GetInterpolantVectorFromDistributionFunction(RNG & rng, Vector3D & vec) const {  // line 880
    vec.Set(GetInterpolantFromDistributionFunction(rng), GetInterpolantFromDistributionFunction(rng), GetInterpolantFromDistributionFunction(rng));
}

// The node values are compared as raw 32-bit words (lines 907-908 and 945-948 compile to integer
// compares with no inlined helper). memcmp() == 0 gives the same compare but a different block
// layout.
template <typename TYPE>
void InterpolationPath<TYPE>::GetInterpolatedValueByTime(const uint32 dwInterpSeed, const int32 nInterp, const InterpTime & tInterpTime, TYPE & tResult) const {  // line 894
    const InterpolationNode<TYPE> & tNode = ptArrayNodes[0];
    float flInterp = 0.0f;

    if (!(tHeader.nNodeCount == 1 && *(const uint32 *)&tNode.tValueMin == *(const uint32 *)&tNode.tValueMax)) {
        uint32 dwSeed = nInterp * dwInterpSeed;
        RNG rng;
        RandSetSeed_Internal(rng, dwSeed);
        flInterp = tHeader.GetInterpolantFromDistributionFunction(rng);
    }

    GetInterpolatedValueByTime(flInterp, tInterpTime, tResult);
}

void VectorPath::GetInterpolatedValueByTime(const uint32 dwInterpSeed, const int32 nInterp, const InterpTime & tInterpTime, Vector3D & tResult) const {  // line 932
    const VectorNode & tNode = ptArrayNodes[0];
    Vector3D vecInterp;

    if (tHeader.nNodeCount == 1 &&
        *(const uint32 *)&tNode.tValueMin.x == *(const uint32 *)&tNode.tValueMax.x &&
        *(const uint32 *)&tNode.tValueMin.y == *(const uint32 *)&tNode.tValueMax.y &&
        *(const uint32 *)&tNode.tValueMin.z == *(const uint32 *)&tNode.tValueMax.z) {
        vecInterp = Vector3D(0.0f, 0.0f, 0.0f);
    } else {
        uint32 dwSeed = nInterp * dwInterpSeed;
        RNG rng;
        RandSetSeed_Internal(rng, dwSeed);
        tHeader.GetInterpolantVectorFromDistributionFunction(rng, vecInterp);
    }

    GetInterpolatedValueByTime(vecInterp, tInterpTime, tResult);
}

void VectorPath::GetInterpolatedValueByTickTime(const uint32 dwInterpSeed, const int32 nInterp, const InterpTickTime & tInterpTime, Vector3D & tResult) const {  // line 969
    const VectorNode & tNode = ptArrayNodes[0];
    Vector3D vecInterp;

    if (tHeader.nNodeCount == 1 &&
        *(const uint32 *)&tNode.tValueMin.x == *(const uint32 *)&tNode.tValueMax.x &&
        *(const uint32 *)&tNode.tValueMin.y == *(const uint32 *)&tNode.tValueMax.y &&
        *(const uint32 *)&tNode.tValueMin.z == *(const uint32 *)&tNode.tValueMax.z) {
        vecInterp = Vector3D(0.0f, 0.0f, 0.0f);
    } else {
        uint32 dwSeed = nInterp * dwInterpSeed;
        RNG rng;
        RandSetSeed_Internal(rng, dwSeed);
        tHeader.GetInterpolantVectorFromDistributionFunction(rng, vecInterp);
    }

    GetInterpolatedValueByTickTime(vecInterp, tInterpTime, tResult);
}

inline PathTime InterpolationPathHeader::GetTimeFromInterpTime(const InterpTime & tInterpTime) const {  // line 1049
    if (tInterpTime.flScale == 0.0f || tInterpTime.flScale == 1.0f / 60.0f) {
        return 0.0f;
    }

    if (tInterpTime.nTimeType == TICK_TIME_NON_LOOPING) {
        return tInterpTime.flTime / tInterpTime.flScale;
    }

    if (tLoopPointStart < 1e-6f && tLoopPointEnd > 0.999999f) {
        float flNonLoopingPathTime = tInterpTime.flTime / tInterpTime.flScale;
        return Math::FloatFrac(flNonLoopingPathTime);
    }

    float tLoopTimeEnd = tInterpTime.flScale * tLoopPointEnd;
    float tCurrentTime = tInterpTime.flTime;

    if (tCurrentTime > tLoopTimeEnd) {
        float tLoopTimeStart = tInterpTime.flScale * tLoopPointStart;
        float tLoopLength = tLoopTimeEnd - tLoopTimeStart;

        if (tLoopLength == 0.0f) {
            tCurrentTime = tLoopTimeStart;
        } else {
            float tTimePassedStart = tCurrentTime - tLoopTimeStart;
            float tLoopCount = Math::FloatTrunc(tTimePassedStart / tLoopLength);
            tCurrentTime = tLoopTimeStart + Math::Max(tTimePassedStart - tLoopLength * tLoopCount, 0.0f);
        }
    }

    return tCurrentTime / tInterpTime.flScale;
}

inline PathTime InterpolationPathHeader::GetTimeFromInterpTimeOutro(const InterpTime & tInterpTime) const {  // line 1105
    float flOutroTime;
    Math::Lerp(tLoopPointEnd, 1.0f, tInterpTime.flOutro, flOutroTime);
    return flOutroTime;
}

inline PathTime InterpolationPathHeader::GetTimeFromInterpTickTime(const InterpTickTime & tInterpTime) const {  // line 1120
    if (tInterpTime.tScale == 0) {
        return 0.0f;
    }

    if (tInterpTime.nTimeType == TICK_TIME_NON_LOOPING) {
        return (float)tInterpTime.tTickTime / (float)tInterpTime.tScale;
    }

    if (tLoopPointStart < 1e-6f && tLoopPointEnd > 0.999999f) {
        float flNonLoopingPathTime = (float)tInterpTime.tTickTime / (float)tInterpTime.tScale;
        return Math::FloatFrac(flNonLoopingPathTime);
    }

    TickTime tLoopTimeEnd = Math::FloatToInt(tLoopPointEnd * tInterpTime.tScale);
    TickTime tCurrentTime = tInterpTime.tTickTime;

    if (tCurrentTime > tLoopTimeEnd) {
        TickTime tLoopTimeStart = Math::FloatToInt(tLoopPointStart * tInterpTime.tScale);
        TickTime tLoopLength = tLoopTimeEnd - tLoopTimeStart;

        if (tLoopLength == 0) {
            tCurrentTime = tLoopTimeStart;
        } else {
            TickTime tTimePassedStart = tCurrentTime - tLoopTimeStart;
            tCurrentTime = tTimePassedStart % tLoopLength + tLoopTimeStart;
        }
    }

    return (float)tCurrentTime / (float)tInterpTime.tScale;
}

inline PathTime InterpolationPathHeader::GetTimeFromInterpTickTimeOutro(const InterpTickTime & tInterpTime) const {  // line 1169
    float flOutroTime;
    Math::Lerp(tLoopPointEnd, 1.0f, tInterpTime.flOutro, flOutroTime);
    return flOutroTime;
}

template <typename TYPE>
void InterpolationPath<TYPE>::GetInterpolatedMinMaxValues(PathTime tTime, TYPE & tMin, TYPE & tMax) const {  // line 1185
    PathTime t;
    int32 i = FindNode(tTime);

    if (i == tHeader.nNodeCount) {
        tMin = ptArrayNodes[i - 1].tValueMin;
        tMax = ptArrayNodes[i - 1].tValueMax;
        return;
    }

    const InterpolationNode<TYPE> & tNode0 = ptArrayNodes[i - 1];
    const InterpolationNode<TYPE> & tNode1 = ptArrayNodes[i];

    t = (tTime - tNode0.tTime) / (tNode1.tTime - tNode0.tTime);
    Math::Lerp(tNode0.tValueMin, tNode1.tValueMin, t, tMin);
    Math::Lerp(tNode0.tValueMax, tNode1.tValueMax, t, tMax);
}

template <typename TYPE>
void InterpolationPath<TYPE>::GetInterpolatedValue(const float rangeInterpolant, PathTime tTime, TYPE & tResult) const {  // line 1220
    PathTime t;
    int32 i = FindNode(tTime);

    if (i == tHeader.nNodeCount) {
        Math::Lerp(ptArrayNodes[i - 1].tValueMin, ptArrayNodes[i - 1].tValueMax, rangeInterpolant, tResult);
        return;
    }

    InterpolationNode<TYPE> & tNode0 = ptArrayNodes[i - 1];
    InterpolationNode<TYPE> & tNode1 = ptArrayNodes[i];

    TYPE tValue0;
    TYPE tValue1;
    Math::Lerp(tNode0.tValueMin, tNode0.tValueMax, rangeInterpolant, tValue0);
    Math::Lerp(tNode1.tValueMin, tNode1.tValueMax, rangeInterpolant, tValue1);

    t = (tTime - tNode0.tTime) / (tNode1.tTime - tNode0.tTime);
    Math::Lerp(tValue0, tValue1, t, tResult);
}

template <typename TYPE>
void ResolvedInterpolationPath<TYPE>::GetInterpolatedValue(PathTime tTime, TYPE & tResult, InterpCursor & tInterpCursor) const {  // line 1260
    int32 i = FindNode(tTime, tInterpCursor);

    tInterpCursor.nLastNode = i - 1;
    tInterpCursor.tLastTime = tTime;

    ResolvedInterpolationNode<TYPE> & tNode0 = ptArrayNodes[i - 1];

    if (i == tHeader.nNodeCount) {
        tResult = tNode0.tValue;
        return;
    }

    ResolvedInterpolationNode<TYPE> & tNode1 = ptArrayNodes[i];
    TYPE tValue0 = tNode0.tValue;
    TYPE tValue1 = tNode1.tValue;

    PathTime t = (tTime - tNode0.tTime) / (tNode1.tTime - tNode0.tTime);
    Math::Lerp(tValue0, tValue1, t, tResult);
}

template <typename TYPE>
void ResolvedInterpolationPath<TYPE>::GetInterpolatedValue(PathTime tTime, TYPE & tResult) const {  // line 1301
    int32 i = FindNode(tTime);

    ResolvedInterpolationNode<TYPE> & tNode0 = ptArrayNodes[i - 1];

    if (i == tHeader.nNodeCount) {
        tResult = tNode0.tValue;
        return;
    }

    ResolvedInterpolationNode<TYPE> & tNode1 = ptArrayNodes[i];
    TYPE tValue0 = tNode0.tValue;
    TYPE tValue1 = tNode1.tValue;

    PathTime t = (tTime - tNode0.tTime) / (tNode1.tTime - tNode0.tTime);
    Math::Lerp(tValue0, tValue1, t, tResult);
}

void VectorPath::GetInterpolatedValue(const Vector3D & rangeInterpolant, PathTime tTime, Vector3D & wvResult) const {  // line 1337
    PathTime t;
    int32 i = FindNode(tTime);

    if (i == tHeader.nNodeCount) {
        Lerp_Vector3DIndependent(ptArrayNodes[i - 1].tValueMin, ptArrayNodes[i - 1].tValueMax, rangeInterpolant, wvResult);
        return;
    }

    VectorNode & tNode0 = ptArrayNodes[i - 1];
    VectorNode & tNode1 = ptArrayNodes[i];

    Vector3D wvValue0;
    Vector3D wvValue1;
    Lerp_Vector3DIndependent(tNode0.tValueMin, tNode0.tValueMax, rangeInterpolant, wvValue0);
    Lerp_Vector3DIndependent(tNode1.tValueMin, tNode1.tValueMax, rangeInterpolant, wvValue1);

    t = (tTime - tNode0.tTime) / (tNode1.tTime - tNode0.tTime);
    Math::Lerp(wvValue0, wvValue1, t, wvResult);
}

template <typename TYPE>
static void sModifyInterpValueByScalar(const InterpolationPathHeader & tHeader, const InterpTime & tInterpTime, TYPE & tValue) {  // line 1375
    float flFinalScalar;
    if (tHeader.tInterpScalar.GetScalar(tInterpTime.tCachedScalarParams, flFinalScalar)) {
        sMultiplyInterpValueByScalar(flFinalScalar, tValue);
    }
}

template <typename TYPE>
static void sModifyInterpValueByScalar(const InterpolationPathHeader & tHeader, const InterpTickTime & tInterpTime, TYPE & tValue) {  // line 1391
    float flFinalScalar;
    if (tHeader.tInterpScalar.GetScalar(tInterpTime.tCachedScalarParams, flFinalScalar)) {
        sMultiplyInterpValueByScalar(flFinalScalar, tValue);
    }
}

template <typename TYPE>
void InterpolationPath<TYPE>::GetMinMaxValuesByTime(const InterpTime & tInterpTime, TYPE & tMin, TYPE & tMax) const {  // line 1407
    TYPE tCurrentMin;
    TYPE tCurrentMax;

    if (tHeader.nNodeCount == 1 && tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE) {
        const InterpolationNode<TYPE> & tNode = ptArrayNodes[0];
        tMin = tNode.tValueMin;
        tMax = tNode.tValueMax;
        return;
    }

    PathTime tTime = tHeader.GetTimeFromInterpTime(tInterpTime);

    GetInterpolatedMinMaxValues(tTime, tCurrentMin, tCurrentMax);

    if (tInterpTime.nTimeType == TICK_TIME_OUTRO_CROSSFADE && tInterpTime.flRatio > 1e-6f) {
        PathTime tTimeOutro = tHeader.GetTimeFromInterpTimeOutro(tInterpTime);
        TYPE tOutroMin;
        TYPE tOutroMax;
        GetInterpolatedMinMaxValues(tTimeOutro, tOutroMin, tOutroMax);
        Math::Lerp(tCurrentMin, tOutroMin, tInterpTime.flRatio, tMin);
        Math::Lerp(tCurrentMax, tOutroMax, tInterpTime.flRatio, tMax);
    } else {
        tMin = tCurrentMin;
        tMax = tCurrentMax;
    }

    sModifyInterpValueByScalar(tHeader, tInterpTime, tMin);
    sModifyInterpValueByScalar(tHeader, tInterpTime, tMax);
}

template <typename TYPE>
void InterpolationPath<TYPE>::GetInterpolatedValueByTime(const float rangeInterpolant, const InterpTime & tInterpTime, TYPE & tResult) const {  // line 1459
    if (tHeader.nNodeCount == 1 && tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE) {
        const InterpolationNode<TYPE> & tNode = ptArrayNodes[0];
        if (*(const uint32 *)&tNode.tValueMin == *(const uint32 *)&tNode.tValueMax) {
            tResult = tNode.tValueMin;
        } else {
            Math::Lerp(tNode.tValueMin, tNode.tValueMax, rangeInterpolant, tResult);
        }
        return;
    }

    PathTime tTime = tHeader.GetTimeFromInterpTime(tInterpTime);

    if (tInterpTime.nTimeType == TICK_TIME_OUTRO_CROSSFADE && tInterpTime.flRatio > 1e-6f) {
        TYPE tCurrentValue;
        GetInterpolatedValue(rangeInterpolant, tTime, tCurrentValue);
        PathTime tTimeOutro = tHeader.GetTimeFromInterpTimeOutro(tInterpTime);
        TYPE tOutroValue;
        GetInterpolatedValue(rangeInterpolant, tTimeOutro, tOutroValue);
        Math::Lerp(tCurrentValue, tOutroValue, tInterpTime.flRatio, tResult);
    } else {
        GetInterpolatedValue(rangeInterpolant, tTime, tResult);
    }

    sModifyInterpValueByScalar(tHeader, tInterpTime, tResult);
}

template <typename TYPE>
void ResolvedInterpolationPath<TYPE>::GetInterpolatedValueByTime(const InterpTime & tInterpTime, TYPE & tResult, InterpCursor & tInterpCursor) const {  // line 1515
    if (tHeader.nNodeCount == 1 && tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE) {
        const ResolvedInterpolationNode<TYPE> & tNode = ptArrayNodes[0];
        tResult = tNode.tValue;
        return;
    }

    PathTime tTime = tHeader.GetTimeFromInterpTime(tInterpTime);

    if (tInterpTime.nTimeType == TICK_TIME_OUTRO_CROSSFADE && tInterpTime.flRatio > 1e-6f) {
        TYPE tCurrentValue;
        GetInterpolatedValue(tTime, tCurrentValue, tInterpCursor);
        PathTime tTimeOutro = tHeader.GetTimeFromInterpTimeOutro(tInterpTime);
        TYPE tOutroValue;
        GetInterpolatedValue(tTimeOutro, tOutroValue);
        Math::Lerp(tCurrentValue, tOutroValue, tInterpTime.flRatio, tResult);
    } else {
        GetInterpolatedValue(tTime, tResult, tInterpCursor);
    }

    sModifyInterpValueByScalar(tHeader, tInterpTime, tResult);
}

template <typename TYPE>
void ResolvedInterpolationPath<TYPE>::GetInterpolatedValueByTime(const InterpTime & tInterpTime, TYPE & tResult) const {  // line 1562
    if (tHeader.nNodeCount == 1 && tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE) {
        const ResolvedInterpolationNode<TYPE> & tNode = ptArrayNodes[0];
        tResult = tNode.tValue;
        return;
    }

    PathTime tTime = tHeader.GetTimeFromInterpTime(tInterpTime);

    if (tInterpTime.nTimeType == TICK_TIME_OUTRO_CROSSFADE && tInterpTime.flRatio > 1e-6f) {
        TYPE tCurrentValue;
        GetInterpolatedValue(tTime, tCurrentValue);
        PathTime tTimeOutro = tHeader.GetTimeFromInterpTimeOutro(tInterpTime);
        TYPE tOutroValue;
        GetInterpolatedValue(tTimeOutro, tOutroValue);
        Math::Lerp(tCurrentValue, tOutroValue, tInterpTime.flRatio, tResult);
    } else {
        GetInterpolatedValue(tTime, tResult);
    }

    sModifyInterpValueByScalar(tHeader, tInterpTime, tResult);
}

void VectorPath::GetInterpolatedValueByTime(const Vector3D & rangeInterpolant, const InterpTime & tInterpTime, Vector3D & wvResult) const {  // line 1609
    if (tHeader.nNodeCount == 1 && tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE) {
        const VectorNode & tNode = ptArrayNodes[0];
        if (*(const uint32 *)&tNode.tValueMin.x == *(const uint32 *)&tNode.tValueMax.x &&
            *(const uint32 *)&tNode.tValueMin.y == *(const uint32 *)&tNode.tValueMax.y &&
            *(const uint32 *)&tNode.tValueMin.z == *(const uint32 *)&tNode.tValueMax.z) {
            wvResult = tNode.tValueMin;
        } else {
            Lerp_Vector3DIndependent(tNode.tValueMin, tNode.tValueMax, rangeInterpolant, wvResult);
        }
        return;
    }

    PathTime tTime = tHeader.GetTimeFromInterpTime(tInterpTime);

    if (tInterpTime.nTimeType == TICK_TIME_OUTRO_CROSSFADE && tInterpTime.flRatio > 1e-6f) {
        Vector3D wvCurrentValue;
        GetInterpolatedValue(rangeInterpolant, tTime, wvCurrentValue);
        PathTime tTimeOutro = tHeader.GetTimeFromInterpTimeOutro(tInterpTime);
        Vector3D wvOutroValue;
        GetInterpolatedValue(rangeInterpolant, tTimeOutro, wvOutroValue);
        Math::Lerp(wvCurrentValue, wvOutroValue, tInterpTime.flRatio, wvResult);
    } else {
        GetInterpolatedValue(rangeInterpolant, tTime, wvResult);
    }

    sModifyInterpValueByScalar(tHeader, tInterpTime, wvResult.x);
    sModifyInterpValueByScalar(tHeader, tInterpTime, wvResult.y);
    sModifyInterpValueByScalar(tHeader, tInterpTime, wvResult.z);
}

template <typename TYPE>
void InterpolationPath<TYPE>::GetMinMaxValuesByTickTime(const InterpTickTime & tInterpTime, TYPE & tMin, TYPE & tMax) const {  // line 1668
    TYPE tCurrentMin;
    TYPE tCurrentMax;

    if (tHeader.nNodeCount == 1 && tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE) {
        const InterpolationNode<TYPE> & tNode = ptArrayNodes[0];
        tMin = tNode.tValueMin;
        tMax = tNode.tValueMax;
        return;
    }

    PathTime tTime = tHeader.GetTimeFromInterpTickTime(tInterpTime);

    GetInterpolatedMinMaxValues(tTime, tCurrentMin, tCurrentMax);

    if (tInterpTime.nTimeType == TICK_TIME_OUTRO_CROSSFADE && tInterpTime.flRatio > 1e-6f) {
        PathTime tTimeOutro = tHeader.GetTimeFromInterpTickTimeOutro(tInterpTime);
        TYPE tOutroMin;
        TYPE tOutroMax;
        GetInterpolatedMinMaxValues(tTimeOutro, tOutroMin, tOutroMax);
        Math::Lerp(tCurrentMin, tOutroMin, tInterpTime.flRatio, tMin);
        Math::Lerp(tCurrentMax, tOutroMax, tInterpTime.flRatio, tMax);
    } else {
        tMin = tCurrentMin;
        tMax = tCurrentMax;
    }

    sModifyInterpValueByScalar(tHeader, tInterpTime, tMin);
    sModifyInterpValueByScalar(tHeader, tInterpTime, tMax);
}

template <typename TYPE>
void InterpolationPath<TYPE>::GetInterpolatedValueByTickTime(const float rangeInterpolant, const InterpTickTime & tInterpTime, TYPE & tResult) const {  // line 1720
    if (tHeader.nNodeCount == 1 && tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE) {
        const InterpolationNode<TYPE> & tNode = ptArrayNodes[0];
        if (*(const uint32 *)&tNode.tValueMin == *(const uint32 *)&tNode.tValueMax) {
            tResult = tNode.tValueMin;
        } else {
            Math::Lerp(tNode.tValueMin, tNode.tValueMax, rangeInterpolant, tResult);
        }
        return;
    }

    PathTime tTime = tHeader.GetTimeFromInterpTickTime(tInterpTime);

    if (tInterpTime.nTimeType == TICK_TIME_OUTRO_CROSSFADE && tInterpTime.flRatio > 1e-6f) {
        TYPE tCurrentValue;
        GetInterpolatedValue(rangeInterpolant, tTime, tCurrentValue);
        PathTime tTimeOutro = tHeader.GetTimeFromInterpTickTimeOutro(tInterpTime);
        TYPE tOutroValue;
        GetInterpolatedValue(rangeInterpolant, tTimeOutro, tOutroValue);
        Math::Lerp(tCurrentValue, tOutroValue, tInterpTime.flRatio, tResult);
    } else {
        GetInterpolatedValue(rangeInterpolant, tTime, tResult);
    }

    sModifyInterpValueByScalar(tHeader, tInterpTime, tResult);
}

template <typename TYPE>
void ResolvedInterpolationPath<TYPE>::GetInterpolatedValueByTickTime(const InterpTickTime & tInterpTime, TYPE & tResult, InterpCursor & tInterpCursor) const {  // line 1776
    if (tHeader.nNodeCount == 1 && tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE) {
        const ResolvedInterpolationNode<TYPE> & tNode = ptArrayNodes[0];
        tResult = tNode.tValue;
        return;
    }

    PathTime tTime = tHeader.GetTimeFromInterpTickTime(tInterpTime);

    if (tInterpTime.nTimeType == TICK_TIME_OUTRO_CROSSFADE && tInterpTime.flRatio > 1e-6f) {
        TYPE tCurrentValue;
        GetInterpolatedValue(tTime, tCurrentValue, tInterpCursor);
        PathTime tTimeOutro = tHeader.GetTimeFromInterpTickTimeOutro(tInterpTime);
        TYPE tOutroValue;
        GetInterpolatedValue(tTimeOutro, tOutroValue);
        Math::Lerp(tCurrentValue, tOutroValue, tInterpTime.flRatio, tResult);
    } else {
        GetInterpolatedValue(tTime, tResult, tInterpCursor);
    }

    sModifyInterpValueByScalar(tHeader, tInterpTime, tResult);
}

template <typename TYPE>
void ResolvedInterpolationPath<TYPE>::GetInterpolatedValueByTickTime(const InterpTickTime & tInterpTime, TYPE & tResult) const {  // line 1823
    if (tHeader.nNodeCount == 1 && tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE) {
        const ResolvedInterpolationNode<TYPE> & tNode = ptArrayNodes[0];
        tResult = tNode.tValue;
        return;
    }

    PathTime tTime = tHeader.GetTimeFromInterpTickTime(tInterpTime);

    if (tInterpTime.nTimeType == TICK_TIME_OUTRO_CROSSFADE && tInterpTime.flRatio > 1e-6f) {
        TYPE tCurrentValue;
        GetInterpolatedValue(tTime, tCurrentValue);
        PathTime tTimeOutro = tHeader.GetTimeFromInterpTickTimeOutro(tInterpTime);
        TYPE tOutroValue;
        GetInterpolatedValue(tTimeOutro, tOutroValue);
        Math::Lerp(tCurrentValue, tOutroValue, tInterpTime.flRatio, tResult);
    } else {
        GetInterpolatedValue(tTime, tResult);
    }

    sModifyInterpValueByScalar(tHeader, tInterpTime, tResult);
}

void VectorPath::GetInterpolatedValueByTickTime(const Vector3D & rangeInterpolant, const InterpTickTime & tInterpTime, Vector3D & wvResult) const {  // line 1870
    if (tHeader.nNodeCount == 1 && tHeader.tInterpScalar.nScalarFunction == SCALAR_FUNCTION_NONE) {
        const VectorNode & tNode = ptArrayNodes[0];
        if (*(const uint32 *)&tNode.tValueMin.x == *(const uint32 *)&tNode.tValueMax.x &&
            *(const uint32 *)&tNode.tValueMin.y == *(const uint32 *)&tNode.tValueMax.y &&
            *(const uint32 *)&tNode.tValueMin.z == *(const uint32 *)&tNode.tValueMax.z) {
            wvResult = tNode.tValueMin;
        } else {
            Lerp_Vector3DIndependent(tNode.tValueMin, tNode.tValueMax, rangeInterpolant, wvResult);
        }
        return;
    }

    PathTime tTime = tHeader.GetTimeFromInterpTickTime(tInterpTime);

    if (tInterpTime.nTimeType == TICK_TIME_OUTRO_CROSSFADE && tInterpTime.flRatio > 1e-6f) {
        Vector3D wvCurrentValue;
        GetInterpolatedValue(rangeInterpolant, tTime, wvCurrentValue);
        PathTime tTimeOutro = tHeader.GetTimeFromInterpTickTimeOutro(tInterpTime);
        Vector3D wvOutroValue;
        GetInterpolatedValue(rangeInterpolant, tTimeOutro, wvOutroValue);
        Math::Lerp(wvCurrentValue, wvOutroValue, tInterpTime.flRatio, wvResult);
    } else {
        GetInterpolatedValue(rangeInterpolant, tTime, wvResult);
    }

    sModifyInterpValueByScalar(tHeader, tInterpTime, wvResult.x);
    sModifyInterpValueByScalar(tHeader, tInterpTime, wvResult.y);
    sModifyInterpValueByScalar(tHeader, tInterpTime, wvResult.z);
}

template <typename TYPE>
void InterpolationPath<TYPE>::GetMinMaxValues(TYPE & tValueMin, TYPE & tValueMax) const {  // line 1929
    tValueMin = ptArrayNodes[0].tValueMin;
    tValueMax = ptArrayNodes[0].tValueMax;

    for (int32 i = 1; i < tHeader.nNodeCount; i++) {
        tValueMin = Math::Min(tValueMin, ptArrayNodes[i].tValueMin);
        tValueMax = Math::Max(tValueMax, ptArrayNodes[i].tValueMax);
    }
}

template <>
void InterpolationPath<RGBAColor>::GetMinMaxValues(RGBAColor & tValueMin, RGBAColor & tValueMax) const {  // line 1950
    tValueMin = ptArrayNodes[0].tValueMin;
    tValueMax = ptArrayNodes[0].tValueMax;

    for (int32 i = 1; i < tHeader.nNodeCount; i++) {
        tValueMin.r = Math::Min(tValueMin.r, ptArrayNodes[i].tValueMin.r);
        tValueMax.r = Math::Max(tValueMax.r, ptArrayNodes[i].tValueMax.r);
        tValueMin.g = Math::Min(tValueMin.g, ptArrayNodes[i].tValueMin.g);
        tValueMax.g = Math::Max(tValueMax.g, ptArrayNodes[i].tValueMax.g);
        tValueMin.b = Math::Min(tValueMin.b, ptArrayNodes[i].tValueMin.b);
        tValueMax.b = Math::Max(tValueMax.b, ptArrayNodes[i].tValueMax.b);
        tValueMin.a = Math::Min(tValueMin.a, ptArrayNodes[i].tValueMin.a);
        tValueMax.a = Math::Max(tValueMax.a, ptArrayNodes[i].tValueMax.a);
    }
}

template <>
void InterpolationPath<Vector2D>::GetMinMaxValues(Vector2D & tValueMin, Vector2D & tValueMax) const {  // line 1980
    tValueMin = ptArrayNodes[0].tValueMin;
    tValueMax = ptArrayNodes[0].tValueMax;

    for (int32 i = 1; i < tHeader.nNodeCount; i++) {
        tValueMin.x = Math::Min(tValueMin.x, ptArrayNodes[i].tValueMin.x);
        tValueMax.x = Math::Max(tValueMax.x, ptArrayNodes[i].tValueMax.x);
        tValueMin.y = Math::Min(tValueMin.y, ptArrayNodes[i].tValueMin.y);
        tValueMax.y = Math::Max(tValueMax.y, ptArrayNodes[i].tValueMax.y);
    }
}

template <>
void InterpolationPath<Vector3D>::GetMinMaxValues(Vector3D & tValueMin, Vector3D & tValueMax) const {  // line 2000
    tValueMin = ptArrayNodes[0].tValueMin;
    tValueMax = ptArrayNodes[0].tValueMax;

    for (int32 i = 1; i < tHeader.nNodeCount; i++) {
        tValueMin.x = Math::Min(tValueMin.x, ptArrayNodes[i].tValueMin.x);
        tValueMax.x = Math::Max(tValueMax.x, ptArrayNodes[i].tValueMax.x);

        tValueMin.y = Math::Min(tValueMin.y, ptArrayNodes[i].tValueMin.y);
        tValueMax.y = Math::Max(tValueMax.y, ptArrayNodes[i].tValueMax.y);

        tValueMin.z = Math::Min(tValueMin.z, ptArrayNodes[i].tValueMin.z);
        tValueMax.z = Math::Max(tValueMax.z, ptArrayNodes[i].tValueMax.z);
    }
}

void InterpTime::Init() {  // line 2025
    nTimeType = TICK_TIME_NON_LOOPING;
    flTime = 0.0f;
    flScale = 1.0f;
    flRatio = 0.0f;
    flOutro = 0.0f;
    tCachedScalarParams.Init();
}

void InterpTickTime::Init() {  // line 2039
    nTimeType = TICK_TIME_NON_LOOPING;
    tTickTime = 0;
    tScale = 1;
    flRatio = 0.0f;
    flOutro = 0.0f;
    tCachedScalarParams.Init();
}

template <typename TYPE>
void InterpolationPath<TYPE>::Resolve(float rangeInterpolant, ResolvedInterpolationPath<TYPE> & tResolvedInterpolationPath) const {  // line 2054
    if (tResolvedInterpolationPath.nMaxNodeCount < tHeader.nNodeCount) {
        tResolvedInterpolationPath.nMaxNodeCount = -1;
        uint32 dwSize = tHeader.nNodeCount * sizeof(ResolvedInterpolationNode<TYPE>);
        tResolvedInterpolationPath.ptArrayNodes = (ResolvedInterpolationNode<TYPE> *)SigmaMemoryNew(dwSize, 4, NULL, FALSE);
    }

    tResolvedInterpolationPath.tHeader = tHeader;

    for (int32 i = 0; i < tHeader.nNodeCount; i++) {
        tResolvedInterpolationPath.ptArrayNodes[i].tTime = ptArrayNodes[i].tTime;
        Math::Lerp(ptArrayNodes[i].tValueMin, ptArrayNodes[i].tValueMax, rangeInterpolant, tResolvedInterpolationPath.ptArrayNodes[i].tValue);
    }
}

template class InterpolationPath<float>;
template class InterpolationPath<int>;
template class InterpolationPath<RGBAColor>;
template class InterpolationPath<Vector3D>;
template class InterpolationPath<Vector2D>;
template class ResolvedInterpolationPath<float>;
template class ResolvedInterpolationPath<RGBAColor>;
