// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdStructFixedSizeString.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

// bdStructFixedSizeString (line 23): the DWARF has five instantiations (100, 1024, ...).
template <bdUWord MaxLength>
class bdStructFixedSizeString {  // size MaxLength + 1
public:
    static const bdUWord BufferSize = MaxLength + 1;
    bdStructFixedSizeString();
    ~bdStructFixedSizeString();
    const bdNChar8 * getBuffer() const;
    bdNChar8 * getBuffer();
    bdUWord getBufferSize() const;
private:
    bdNChar8 m_buffer[MaxLength + 1];  // 0x00
};

#include "bdStructFixedSizeString.inl"
