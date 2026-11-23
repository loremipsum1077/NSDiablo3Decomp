// Original DWARF layout; storage-only generic definition, no member behavior.
#pragma once
#include <type_traits>
namespace nn { namespace util {
template <typename T, unsigned long Size, unsigned long Alignment> struct TypedStorage {
 typename std::aligned_storage<Size,Alignment>::type storage;
};
}}
