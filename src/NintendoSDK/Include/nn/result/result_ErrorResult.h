// Original file: NintendoSDK/Include/nn/result/result_ErrorResult.h
// Subset reconstructed from the DWARF of X/Client/Console/NX64NFP.cpp (classes ErrorResultBase,
// line 23, and ErrorRange, line 51). Bodies are inferred from the code inlined into
// UpdatePlayerStateDeviceCheck / UpdatePlayerStateError.
#pragma once

#include "result_ResultBase.h"
#include "../nn_Result.h"

namespace nn {
namespace result {
namespace detail {

template <int Module_, int Description_>
class ErrorResultBase : public ResultBase<ErrorResultBase<Module_, Description_> > {  // line 23
public:
    static const int Module = Module_;  // line 28
    static const int Description = Description_;  // line 29
    // line 30; DWARF const_value: Module | Description << 9 (e.g. <115, 96> = 49267).
    static const ResultTraits::InnerType InnerValue = Module_ | (Description_ << 9);
    operator Result() const { return Result(InnerValue); }  // line 32
};

template <int Module_, int DescriptionBegin_, int DescriptionEnd_>
class ErrorRange {  // line 51
public:
    static const int Module = Module_;  // line 54
    static const int DescriptionBegin = DescriptionBegin_;  // line 55
    static const int DescriptionEnd = DescriptionEnd_;  // line 56
    // line 61; inlines Result::GetModule and Result::GetDescription at line 63.
    static bool Includes(Result tResult)
    {
        return tResult.GetModule() == Module && DescriptionBegin <= tResult.GetDescription() && tResult.GetDescription() < DescriptionEnd;
    }
    // line 65; non-template friend (mangled _ZN2nn6result6detailleENS_6ResultENS1_10ErrorRange...).
    friend bool operator<=(Result tResult, ErrorRange)
    {
        return Includes(tResult);
    }
};

static_assert(sizeof(ErrorResultBase<115, 96>) == 0x1, "ErrorResultBase<115, 96>");
static_assert(sizeof(ErrorRange<115, 64, 80>) == 0x1, "ErrorRange<115, 64, 80>");

}  // namespace detail
}  // namespace result
}  // namespace nn
