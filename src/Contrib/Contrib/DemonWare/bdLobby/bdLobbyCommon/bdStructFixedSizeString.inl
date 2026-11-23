// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdStructFixedSizeString.inl
// Bodies are not in the DWARF; the constructor's inlined bdStrlcpy (line 12) and the two
// getBuffer accessors (lines 65, 71) are what the surviving callers show.
#pragma once

#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"

template <bdUWord MaxLength>
inline bdStructFixedSizeString<MaxLength>::bdStructFixedSizeString()
{
    bdStrlcpy(m_buffer, "", BufferSize);
}

template <bdUWord MaxLength>
inline bdStructFixedSizeString<MaxLength>::~bdStructFixedSizeString()
{
    // Not in the DWARF. The owning class's implicit destructor still stores its vtable pointers,
    // which clang only does when a member's destructor body is not literally empty; a statement
    // that compiles to nothing (a stripped assert or log) is the smallest thing that explains it.
    ;
}

template <bdUWord MaxLength>
inline const bdNChar8 * bdStructFixedSizeString<MaxLength>::getBuffer() const
{
    return m_buffer;
}

template <bdUWord MaxLength>
inline bdNChar8 * bdStructFixedSizeString<MaxLength>::getBuffer()
{
    return m_buffer;
}

template <bdUWord MaxLength>
inline bdUWord bdStructFixedSizeString<MaxLength>::getBufferSize() const
{
    return BufferSize;
}
