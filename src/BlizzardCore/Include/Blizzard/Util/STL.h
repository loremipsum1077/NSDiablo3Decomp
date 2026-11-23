// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Include/Blizzard/Util/STL.h

#pragma once

#include "../Sys/Allocator_STL.h"

#include <string>

namespace Blizzard {
namespace STL {
typedef std::basic_string<char, std::char_traits<char>, Blizzard::Memory::AllocSTL<char> > string;

}  // namespace STL
}  // namespace Blizzard
