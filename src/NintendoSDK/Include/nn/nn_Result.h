// Adopted from DWARF; hand-maintained.
// Original file: NintendoSDK/Include/nn/nn_Result.h

#pragma once

#include "result/result_ResultBase.h"

namespace nn {
class ResultSuccess;
// result/result_ErrorResult.h line 23; forward-declared so Result can befriend it.
namespace result {
namespace detail {
template <int Module_, int Description_> class ErrorResultBase;
}
}
class Result : public nn::result::detail::ResultBase<nn::Result> {  // size 0x4
public:
    Result();
    // Original line 50; recovered from the copy inlined into NavigateTo.
    bool IsSuccess() const { return m_Value == InnerSuccessValue; }
    nn::result::detail::ResultBase<nn::Result>::InnerType GetInnerValueForDebug() const;
    operator ResultSuccess() const;
    static bool CanAccept(nn::Result);
    bool IsFailure() const { return !IsSuccess(); }
    // Original line 111. Inferred body: the module is the low 9 bits (UpdatePlayerStateError
    // masks with 0x1ff and compares with module 115).
    int GetModule() const { return m_Value & 0x1FF; }
    int GetDescription() const { return (m_Value >> 9) & 0x1FFF; }
private:
    nn::result::detail::ResultBase<nn::Result>::InnerType m_Value;  // 0x00
    // Original line 130 (private, explicit). Inferred body.
    explicit Result(nn::result::detail::ResultBase<nn::Result>::InnerType tValue) : m_Value(tValue) {}
    // Inferred: ErrorResultBase::operator Result (result_ErrorResult.h:32) builds a Result from
    // its InnerValue; DWARF does not record friends.
    template <int Module_, int Description_> friend class nn::result::detail::ErrorResultBase;
};
static_assert(sizeof(Result) == 0x4, "Result");
static_assert(sizeof(result::detail::ResultBase<Result>) == 0x1, "ResultBase<Result>");

class ResultSuccess : public nn::result::detail::ResultBase<nn::ResultSuccess> {  // size 0x1
public:
    operator Result() const;
    bool IsSuccess() const;
    static bool CanAccept(nn::Result);
    nn::result::detail::ResultBase<nn::ResultSuccess>::InnerType GetInnerValueForDebug() const;
    int GetModule() const;
    int GetDescription() const;
};
static_assert(sizeof(ResultSuccess) == 0x1, "ResultSuccess");

}  // namespace nn

// Functions defined in this file (bodies are not in the DWARF):
//   line    50  bool nn::Result::IsSuccess() const  [inlined into 11 functions]
//   line   111  int nn::Result::GetModule() const  [inlined into 1 functions]
//   line   123  int nn::Result::GetDescription() const  [inlined into 1 functions]

// Bodies of the inline members NX64NFP.cpp needs. Result() leaves the value unset: the copies
// inlined into TagAccessorBase::TagAccessorBase store nothing for m_tLastResult.
inline nn::Result::Result() {}
inline nn::result::detail::ResultBase<nn::Result>::InnerType nn::Result::GetInnerValueForDebug() const { return m_Value; }
inline nn::ResultSuccess::operator nn::Result() const
{
    nn::Result tResult;
    *reinterpret_cast<nn::result::detail::ResultBase<nn::Result>::InnerType *>(&tResult) = nn::Result::InnerSuccessValue;
    return tResult;
}
