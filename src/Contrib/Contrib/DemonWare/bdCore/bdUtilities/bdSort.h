// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdSort.h
// Hand-written: the DWARF has only the .inl (comb sort, instantiated by
// X/Client/Console/MailService.cpp). The header name follows the DemonWare .h/.inl pattern.

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

template <typename T, typename Compare>
void bdCombSort(T *const data, const bdUInt size, Compare compare);

inline void bdNewGap(bdUInt & gap);

#include "bdSort.inl"
