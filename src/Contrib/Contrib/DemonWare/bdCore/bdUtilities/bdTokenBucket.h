// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdTokenBucket.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdTokenBucket {  // size 0x20
public:
    bdTokenBucket(bdUInt32);
    void fillBucket();
    bdUInt32 getAvailTokens(bdUInt32 maxTokens) const;
    void removeTokens(bdUInt32 numTokens);
    void setTokensRestricted(bdBool isRestricted);
    void setRates(bdUInt32 tokensPerMs, bdUInt32 bucketSize);
    void setMinFragment(bdUInt32 minFragmentTokens);
protected:
    bdUInt32 m_tokensPerMs;  // 0x00
    bdUInt32 m_capacity;  // 0x04
    bdUInt32 m_minFragment;  // 0x08
    bdUInt32 m_tokens;  // 0x0c
    bdUInt64 m_lastUpdateTime;  // 0x10
    bdBool m_isInfinite;  // 0x18
};
static_assert(sizeof(bdTokenBucket) == 0x20, "bdTokenBucket");

