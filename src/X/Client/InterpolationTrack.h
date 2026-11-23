// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/InterpolationTrack.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "InterpolationScalar.h"
#include "../Sigma/DataStructures/XPtr64.h"
#include "../Sigma/Files/XSerializeData.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/RGBAColor.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/Memory/SigmaMemory.h"

struct RNG;

typedef float PathTime;

typedef int32 TickTime;

enum TickTimeType {
    TICK_TIME_NON_LOOPING = 0,
    TICK_TIME_LOOPING = 1,
    TICK_TIME_OUTRO_CROSSFADE = 2,
};

enum DistributionFunction {
    DISTRIBUTION_FUNCTION_UNIFORM = 0,
    DISTRIBUTION_FUNCTION_LOW = 1,
    DISTRIBUTION_FUNCTION_MID = 2,
    DISTRIBUTION_FUNCTION_HIGH = 3,
    DISTRIBUTION_FUNCTION_LOW_AND_HIGH = 4,
    DISTRIBUTION_FUNCTION_SMOOTH_LOW = 5,
    DISTRIBUTION_FUNCTION_SMOOTH_MID = 6,
    DISTRIBUTION_FUNCTION_SMOOTH_HIGH = 7,
    DISTRIBUTION_FUNCTION_SMOOTH_LOW_AND_HIGH = 8,
};

struct InterpTime {  // size 0x2c
    InterpTime();
    InterpTime(const float, const float);
    void Init();
    TickTimeType nTimeType;  // 0x00
    float flTime;  // 0x04
    float flScale;  // 0x08
    float flRatio;  // 0x0c
    float flOutro;  // 0x10
    InterpScalarParams tCachedScalarParams;  // 0x14
};
static_assert(sizeof(InterpTime) == 0x2c, "InterpTime");

// Line 50. Only the member constructor runs (Wind::Update inlines it: InterpScalarParams::Init
// and nothing else before its own InterpTime::Init call).
inline InterpTime::InterpTime() {}

struct InterpTickTime {  // size 0x2c
    InterpTickTime();
    InterpTickTime(const TickTime, const TickTime);
    void Init();
    TickTimeType nTimeType;  // 0x00
    TickTime tTickTime;  // 0x04
    TickTime tScale;  // 0x08
    float flRatio;  // 0x0c
    float flOutro;  // 0x10
    InterpScalarParams tCachedScalarParams;  // 0x14
};
static_assert(sizeof(InterpTickTime) == 0x2c, "InterpTickTime");

// Line 79 (inline_helpers.csv). sUpdateARope's three inlined copies run the member
// constructor (InterpScalarParams::Init) and then call Init from line 81.
inline InterpTickTime::InterpTickTime() {
    Init();
}

struct InterpolationPathHeader {  // size 0x1c
    int32 nNodeCount;  // 0x00
    PathTime tLoopPointStart;  // 0x04
    PathTime tLoopPointEnd;  // 0x08
    DistributionFunction nDistributionFunction;  // 0x0c
    InterpolationScalar tInterpScalar;  // 0x10
    InterpolationPathHeader() {}
    InterpolationPathHeader(const int32);
    BOOL operator==(const InterpolationPathHeader & OtherPathHeader) const;
    PathTime GetTimeFromInterpTickTime(const InterpTickTime & tInterpTime) const;
    PathTime GetTimeFromInterpTickTimeOutro(const InterpTickTime & tInterpTime) const;
    PathTime GetTimeFromInterpTime(const InterpTime & tInterpTime) const;
    PathTime GetTimeFromInterpTimeOutro(const InterpTime & tInterpTime) const;
    float GetInterpolantFromDistributionFunction(RNG & rng) const;
    void GetInterpolantVectorFromDistributionFunction(RNG & rng, Vector3D & vec) const;
};
static_assert(sizeof(InterpolationPathHeader) == 0x1c, "InterpolationPathHeader");

struct InterpCursor {  // size 0x8
    InterpCursor() : nLastNode(0), tLastTime(0.0f) {  // line 171
    }
    int32 nLastNode;  // 0x00
    PathTime tLastTime;  // 0x04
};
static_assert(sizeof(InterpCursor) == 0x8, "InterpCursor");


template <typename TYPE>
struct InterpolationNode {  // line 110
    TYPE tValueMin;
    TYPE tValueMax;
    PathTime tTime;
    BOOL operator==(const InterpolationNode<TYPE> & OtherNode) const;
};

template <typename TYPE>
struct ResolvedInterpolationNode {  // line 123
    TYPE tValue;
    PathTime tTime;
};

template <typename TYPE>
class ResolvedInterpolationPath {  // line 182
public:
    InterpolationPathHeader tHeader;
    ResolvedInterpolationNode<TYPE> * ptArrayNodes;
    int32 nMaxNodeCount;

    // The constructors and destructor were linker-stripped; their bodies are not recovered from
    // the binary (Resolve sets nMaxNodeCount to -1 when it allocates the nodes itself).
    ResolvedInterpolationPath() : ptArrayNodes(NULL), nMaxNodeCount(0) {}
    ResolvedInterpolationPath(int32 nMaxNodeCount_, ResolvedInterpolationNode<TYPE> * arrayNodes)  // line 198
        : ptArrayNodes(arrayNodes), nMaxNodeCount(nMaxNodeCount_) {}
    ~ResolvedInterpolationPath() {  // line 206
        if (nMaxNodeCount == -1) {
            SigmaMemoryFree(ptArrayNodes, NULL);
            ptArrayNodes = NULL; // Original TrailUpdate.cpp destructor stores after freeing.
        }
    }

    void GetInterpolatedValueByTime(const InterpTime & tInterpTime, TYPE & tResult, InterpCursor & tInterpCursor) const;
    void GetInterpolatedValueByTickTime(const InterpTickTime & tInterpTime, TYPE & tResult, InterpCursor & tInterpCursor) const;
    int32 FindNode(PathTime tTime, InterpCursor & tInterpCursor) const;
    void GetInterpolatedValueByTime(const InterpTime & tInterpTime, TYPE & tResult) const;
    void GetInterpolatedValueByTickTime(const InterpTickTime & tInterpTime, TYPE & tResult) const;
    int32 FindNode(PathTime tTime) const;

private:
    void GetInterpolatedValue(PathTime tTime, TYPE & tResult, InterpCursor & tInterpCursor) const;
    void GetInterpolatedValue(PathTime tTime, TYPE & tResult) const;
};

template <typename TYPE>
class InterpolationPath {  // line 254
public:
    InterpolationPathHeader tHeader;
    XPtr64<InterpolationNode<TYPE> > ptArrayNodes;
    SerializeData serNodes;

    InterpolationPath();
    InterpolationPath(const InterpolationPathHeader & tHeader, InterpolationNode<TYPE> * arrayNodes)  // line 269
        : tHeader(tHeader) {
        uint32 dwSize = tHeader.nNodeCount * sizeof(InterpolationNode<TYPE>);
        ptArrayNodes = (InterpolationNode<TYPE> *)SigmaMemoryNew(dwSize, 4, NULL, FALSE);
        SigmaMemoryCopy(ptArrayNodes, arrayNodes, dwSize);
        serNodes.dwSize = dwSize;
    }
    ~InterpolationPath() {  // line 280
        Dispose();
    }
    void Dispose() {  // line 285
        SigmaMemoryFree(ptArrayNodes, NULL);
    }
    BOOL operator==(const InterpolationPath<TYPE> & OtherPath) const;
    InterpolationPath<TYPE> & operator=(const InterpolationPath<TYPE> & rhs);
    int32 FindNode(PathTime tTime) const;
    void GetNode(const int32 i, InterpolationNode<TYPE> & tNode) const;
    void AddNode(PathTime tTime, const TYPE & tValueMin, const TYPE & tValueMax);
    void AddNode(PathTime tTime, const TYPE & tValue) {  // line 306
        AddNode(tTime, tValue, tValue);
    }
    void Resolve(float rangeInterpolant, ResolvedInterpolationPath<TYPE> & tResolvedInterpolationPath) const;
    void GetMinMaxValues(TYPE & tValueMin, TYPE & tValueMax) const;
    void GetIntraLoopValue(TYPE & tValue) const;
    void SetLoopPoints(PathTime tLoopPointStart, PathTime tLoopPointEnd);
    void SetConstantValue(const TYPE & tValue);
    void SetLinearPath(const TYPE & tValue0, const TYPE & tValue1);
    void RemoveAllNodes();
    void GetMinMaxValuesByTime(const InterpTime & tInterpTime, TYPE & tMin, TYPE & tMax) const;
    void GetInterpolatedValueByTime(const float rangeInterpolant, const InterpTime & tInterpTime, TYPE & tResult) const;
    void GetInterpolatedValueByTime(const uint32 dwInterpSeed, const int32 nInterp, const InterpTime & tInterpTime, TYPE & tResult) const;
    void GetMinMaxValuesByTickTime(const InterpTickTime & tInterpTime, TYPE & tMin, TYPE & tMax) const;
    void GetInterpolatedValueByTickTime(const float rangeInterpolant, const InterpTickTime & tInterpTime, TYPE & tResult) const;
    void GetInterpolatedValueByTickTime(const uint32 dwInterpSeed, const int32 nInterp, const InterpTickTime & tInterpTime, TYPE & tResult) const;

private:
    void GetInterpolatedValue(const float rangeInterpolant, PathTime tTime, TYPE & tResult) const;
    void GetInterpolatedMinMaxValues(PathTime tTime, TYPE & tMin, TYPE & tMax) const;
};

class VectorPath : public InterpolationPath<Vector3D> {  // line 384
public:
    VectorPath();
    VectorPath(const InterpolationPathHeader & tHeader, InterpolationNode<Vector3D> * arrayNodes)  // line 388
        : InterpolationPath<Vector3D>(tHeader, arrayNodes) {}
    void GetInterpolatedValueByTime(const Vector3D & rangeInterpolant, const InterpTime & tInterpTime, Vector3D & wvResult) const;
    void GetInterpolatedValueByTime(const uint32 dwInterpSeed, const int32 nInterp, const InterpTime & tInterpTime, Vector3D & tResult) const;
    void GetInterpolatedValueByTickTime(const Vector3D & rangeInterpolant, const InterpTickTime & tInterpTime, Vector3D & wvResult) const;
    void GetInterpolatedValueByTickTime(const uint32 dwInterpSeed, const int32 nInterp, const InterpTickTime & tInterpTime, Vector3D & tResult) const;

private:
    void GetInterpolatedValue(const Vector3D & rangeInterpolant, PathTime tTime, Vector3D & wvResult) const;
};
static_assert(sizeof(VectorPath) == 0x30, "VectorPath");

typedef InterpolationPath<float> FloatPath;

typedef InterpolationNode<float> FloatNode;

typedef InterpolationPath<int> IntPath;

typedef InterpolationNode<int> IntNode;

typedef InterpolationNode<Vector3D> VectorNode;

typedef InterpolationPath<RGBAColor> ColorPath;

typedef InterpolationNode<RGBAColor> ColorNode;

typedef InterpolationPath<float> AnglePath;

typedef InterpolationNode<float> AngleNode;

extern const InterpolationScalar cgInterpolationScalarDefault;
extern const FloatPath cgpathNullFloatPath;
extern const FloatPath cgpathUnitFloatPath;
extern const IntPath cgpathNullIntPath;
extern const IntPath cgpathUnitIntPath;
extern const VectorPath cgpathNullVectorPath;
extern const VectorPath cgpathZAxisVectorPath;
extern const ColorPath cgpathNullColorPath;
extern const AnglePath cgpathNullAnglePath;

class XTypeDescriptor;
extern const XTypeDescriptor * DT_FLOAT_PATH;  // InterpolationTrack.cpp:141
extern const XTypeDescriptor * DT_VECTOR_PATH;  // InterpolationTrack.cpp:215
extern const XTypeDescriptor * DT_ANGLE_PATH;  // InterpolationTrack.cpp:276
extern const XTypeDescriptor * DT_COLOR_PATH;  // InterpolationTrack.cpp:251
extern const XTypeDescriptor * DT_VELOCITY_PATH;  // InterpolationTrack.cpp:322
