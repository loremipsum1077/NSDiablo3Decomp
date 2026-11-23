#pragma once

#include <stdarg.h>
#include <stddef.h>

#include "Types.h"

namespace Blizzard {
namespace String {
size_t Copy(char* dest, const char* source, size_t length);
size_t Append(char* dest, const char* source, size_t length);
int Compare(const char* s1, const char* s2);
int Compare(const char* s1, const char* s2, size_t length);
int CompareI(const char* s1, const char* s2);
bool Equal(const char* s1, const char* s2);
bool Equal(const char* s1, const char* s2, size_t length);
bool EqualI(const char* s1, const char* s2);
bool EqualI(const char* s1, const char* s2, size_t length);
size_t Length(const char* buf);
const char* Find(const char* string, const char* search, size_t stringLength = 0x7FFFFFFF);
const char* Find(const char* string, char search, size_t stringLength = 0x7FFFFFFF);
const char* FindI(const char* string, const char* search, size_t stringLength = 0x7FFFFFFF);
uint32 Format(char* dest, uint32 maxchars, const char* format, ...);
uint32 VFormat(char* dest, uint32 maxchars, const char* format, va_list arglist);
void* MemCopy(void* dest, const void* source, size_t size);
void MemFill(void* buf, size_t size, uint8 val);
void ToLower(char* buf);
bool EndsWith(const char* string, const char* suffix);
const char* FindPathSeparator(const char* buf, size_t bufLength = 0x7FFFFFFF);
// Path.cpp:156-286.
void SplitPath(const char* path, char* dir, int dirMax, char* file, int fileMax);
const char* MakeUnivPath(const char* rawpath, char* fixedpath, int fixedpathMax);
const char* MakeNativePath(const char* rawpath, char* fixedpath, int fixedpathMax);
void ForceTrailingSeparator(char* buf, int bufMax, char preferredSlashType);
void JoinPath(char* path, int pathLen, const char* dir, const char* filename);
const char* FindFilename(const char* buf);
char* FindFilename(char* buf);
const char* FindExtensionWithLeadingDot(const char* buf);
char* FindExtensionWithLeadingDot(char* buf);

// String.h:147. The body is not in the DWARF; the one out-of-line copy (QuickFormat<1024>) passes
// the variadic arguments straight to VFormat.
template <int k_bufferSize>
class QuickFormat {
public:
    QuickFormat(const char* format, ...) {
        va_list arglist;
        va_start(arglist, format);
        VFormat(m_buffer, k_bufferSize, format, arglist);
        va_end(arglist);
    }
    operator const char*() const { return m_buffer; }

protected:
    char m_buffer[k_bufferSize];
};

typedef QuickFormat<1024> QFormat;
}  // namespace String
}  // namespace Blizzard

namespace Blizzard {
namespace Unicode {
class GetUTF8FromCodepoint {  // size 0x8
public:
    GetUTF8FromCodepoint(unsigned int);
    operator const char*() const { return m_buffer; }
    const char* c_str() const { return m_buffer; }

protected:
    char m_buffer[8];  // 0x00
};
static_assert(sizeof(GetUTF8FromCodepoint) == 0x8, "GetUTF8FromCodepoint");

bool CountUTF8Chars(const char* s, size_t numBytes, size_t& numChars);
unsigned int GetCodepointFromUTF8(const char** src);
size_t GetUTF8CharWidth(const char* s);
int ConvertUTF8to16(utf16* dst, uint32 dstMaxChars, const char* src, uint32 srcMaxChars, uint32* dstChars,
                    uint32* srcChars);
int ConvertUTF16to8(char* dst, uint32 dstMaxChars, const utf16* src, uint32 srcMaxChars, uint32* dstChars,
                    uint32* srcChars);
}  // namespace Unicode
}  // namespace Blizzard
