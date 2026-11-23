// Original file: NintendoSDK/Include/nn/result/result_ResultBase.h
// Subset reconstructed from DWARF: the empty base and its value type.
#pragma once

#include <stdint.h>

namespace nn { namespace result { namespace detail {

class ResultTraits {
public:
    typedef uint32_t InnerType;
    static const InnerType InnerSuccessValue = 0;
};
static_assert(sizeof(ResultTraits) == 0x1, "ResultTraits");

// DWARF records ResultBase<Self> as an empty class. The template definition
// is reconstructed from its instantiations; unused method bodies are omitted.
template <typename Self>
class ResultBase {
public:
    typedef ResultTraits::InnerType InnerType;
    static const InnerType InnerSuccessValue = 0;
    int GetModule() const;
    int GetDescription() const;
};

} } }  // namespace nn::result::detail
