#pragma once

#include "XChainedFixedBlockAllocator.h"




















template <typename TYPE>
class XMemoryPool : public XChainedFixedBlockAllocator<TYPE> {
};
