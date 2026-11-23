// Original file: BlizzardCore/Source/System/String/System_String.cpp
// The original is a unity file that includes CanonString.cpp, Path.cpp, String.cpp and
// Unicode.cpp. Their functions are written here grouped by original file, each group in original
// line order.
#include "../../../Include/Blizzard/String.h"
#include "../../../Include/Blizzard/Types.h"

#include <algorithm>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

namespace Blizzard {
namespace String {
char * TranslateEx(const char * source, char * dest, uint32 destSize, const char * from, const char * to,
                   int bMatchCase, int bMatchWholeWord);
char * Translate(const char * source, char * dest, uint32 destSize, const char * from, const char * to);
char * FindReverse(char * string, char search, size_t stringLength);
}  // namespace String
}  // namespace Blizzard

// ---- Path.cpp ----
namespace System_String {
// line 34
template <char wrongSeparator, char rightSeparator>
void FixPath(const char * rawPath, char * fixedPath, int fixedPathMax)
{
    if (fixedPath == NULL || fixedPathMax <= 0)
    {
        return;
    }
    if (rawPath == NULL)
    {
        *fixedPath = '\0';
        return;
    }
    // Path.cpp:46 col 21 is the postfix decrement (sub/cbnz, no entry test); 47 col 16 is the
    // switch's declared c. The NUL written on running out of room replaces the last character
    // (the original stores it through the pointer saved at the top of the last iteration).
    while (fixedPathMax--)
    {
        // default first: the only case order that reproduces the original's block layout.
        switch (char c = *rawPath++)
        {
        default:
            *fixedPath++ = c;
            break;
        case wrongSeparator:
            *fixedPath++ = rightSeparator;
            break;
        case '\0':
            *fixedPath = '\0';
            return;
        }
    }
    fixedPath[-1] = '\0';
}
}  // namespace System_String

namespace Blizzard {
namespace String {
// line 71
const char * MakeUnivPath(const char * rawpath, char * fixedpath, int fixedpathMax)
{
    System_String::FixPath<'\\', '/'>(rawpath, fixedpath, fixedpathMax);
    return fixedpath;
}

// line 83
const char * MakeBackslashPath(const char * rawpath, char * fixedpath, int fixedpathMax)
{
    System_String::FixPath<'/', '\\'>(rawpath, fixedpath, fixedpathMax);
    return fixedpath;
}

// line 95
const char * MakeNativePath(const char * rawpath, char * fixedpath, int fixedpathMax)
{
    System_String::FixPath<'\\', '/'>(rawpath, fixedpath, fixedpathMax);
    return fixedpath;
}

// line 112
// Not inline: the DWARF has a concrete (linker-dropped, low_pc 0) DIE for it. Only its inlined
// copy in JoinPath survives. Inferred: the fixedPath == NULL and rawPath == NULL tests (lines
// 115/119; JoinPath passes a known non-NULL pointer, so no instruction remains for them) and the
// returned values on the early exits. The switch declares c (DWARF lexical block, line 129 col
// 16); the loop mirrors FixPath, whose count/terminator pattern the inlined code shows.
const char * MakeConsistentPath(const char * rawPath, char * fixedPath, int fixedPathMax)
{
    if (fixedPath == NULL || fixedPathMax <= 0)
    {
        return fixedPath;
    }
    if (rawPath == NULL || fixedPathMax == 1)
    {
        *fixedPath = '\0';
        return fixedPath;
    }
    char * result = fixedPath;
    // Path.cpp:128 col 21 is the postfix decrement, as in FixPath:46; the '/' store is line 136,
    // the '\\' store line 140. Inferred: those stores write the literal separator (storing c
    // back over the byte it was read from is removed when rawPath == fixedPath, as in JoinPath;
    // the original keeps both stores).
    while (fixedPathMax--)
    {
        switch (char c = *rawPath++)
        {
        case '\0':
            *fixedPath = '\0';
            return result;
        case '/':
            *fixedPath++ = '/';
            System_String::FixPath<'\\', '/'>(rawPath, fixedPath, fixedPathMax);
            return result;
        case '\\':
            *fixedPath++ = '\\';
            System_String::FixPath<'/', '\\'>(rawPath, fixedPath, fixedPathMax);
            return result;
        default:
            *fixedPath++ = c;
            break;
        }
    }
    fixedPath[-1] = '\0';
    return result;
}

// line 156
void JoinPath(char * path, int pathLen, const char * dir, const char * filename)
{
    if (path == NULL)
    {
        return;
    }
    Copy(path, dir, pathLen);
    if (path[0] != '\0')
    {
        ForceTrailingSeparator(path, pathLen, '\0');
    }
    if (filename != NULL)
    {
        while (*filename == '\\' || *filename == '/')
        {
            filename++;
        }
    }
    Append(path, filename, pathLen);
    MakeConsistentPath(path, path, pathLen);
}

// line 186



// Path.cpp:188 tests path; 190/191 and 192 (store merged, line 0) clear dir and name when it
// is NULL; 197 leaf; 200 the clamp (cols 34/52/60: the conditional, leaf - path, + 1).
void SplitPath(const char * path, char * dir, int dirlen, char * name, int namelen)
{
    if (path == NULL)
    {
        if (dir != NULL && dirlen > 0)
        {
            *dir = '\0';
        }
        if (name != NULL && namelen > 0)
        {
            *name = '\0';
        }
        return;
    }
    const char * leaf = FindFilename(path);
    if (dir != NULL)
    {
        Copy(dir, path, static_cast<int>(leaf - path) + 1 < dirlen ? static_cast<int>(leaf - path) + 1 : dirlen);
    }
    if (name != NULL)
    {
        Copy(name, leaf, namelen);
    }
}

// line 212
const char * FindFilename(const char * buf)
{
    if (buf == NULL)
    {
        return "";
    }
    const char * found = buf;
    for (;;)
    {
        switch (*buf++)
        {
        case '\\':
        case '/':
            found = buf;
            break;
        case '\0':
            return found;
        }
    }
}

// line 231
char * FindFilename(char * buf)
{
    return const_cast<char *>(FindFilename(const_cast<const char *>(buf)));
}
}  // namespace String
}  // namespace Blizzard

namespace System_String {
// line 246
const char * FindExtensionWithOffset(const char * buf, int offset)
{
    if (buf == NULL)
    {
        return "";
    }
    const char * found = NULL;
    for (;; buf++)
    {
        // Case order from the line table: the '\0' return is Path.cpp:255, the '.' store 258.
        switch (*buf)
        {
        case '\0':
            return found != NULL ? found : buf;
        case '/':
        case '\\':
            found = NULL;
            break;
        case '.':
            found = buf + offset;
            break;
        }
    }
}
}  // namespace System_String

namespace Blizzard {
namespace String {
// line 265
const char * FindExtension(const char * buf)
{
    return System_String::FindExtensionWithOffset(buf, 1);
}

// line 280
const char * FindExtensionWithLeadingDot(const char * buf)
{
    return System_String::FindExtensionWithOffset(buf, 0);
}

// line 286
char * FindExtensionWithLeadingDot(char * buf)
{
    return const_cast<char *>(System_String::FindExtensionWithOffset(buf, 0));
}

char SeparatorExemplar(const char * path);

// line 295
void ForceTrailingSeparator(char * buf, int bufMax, char preferredSlashType)
{
    if (buf == NULL)
    {
        return;
    }
    char bestSeparator = preferredSlashType;
    if (bestSeparator == '\0')
    {
        bestSeparator = SeparatorExemplar(buf);
    }
    if (bufMax >= 2)
    {
        int len = static_cast<int>(Length(buf));
        char trailing = len > 0 ? buf[len - 1] : '\0';
        if (len == 0 || trailing != bestSeparator)
        {
            if (trailing == '\\' || trailing == '/')
            {
                len--;
            }
            if (len > bufMax - 2)
            {
                len = bufMax - 2;
            }
            buf[len] = bestSeparator;
            buf[len + 1] = '\0';
        }
    }
}

// line 362
char SeparatorExemplar(const char * path)
{
    // Inferred: the NULL test (its callers pass a pointer they have already tested). The scan is
    // Path.cpp:367 (p, its own block) and 369 col 16 (the switch's c), col 22 the p++.
    if (path == NULL)
    {
        return '/';
    }
    for (const char * p = path;;)
    {
        switch (char c = *p++)
        {
        case '/':
        case '\\':
            return c;
        case '\0':
            return '/';
        }
    }
}
}  // namespace String
}  // namespace Blizzard

// ---- String.cpp ----
namespace Blizzard {
namespace String {
// line 48
size_t Copy(char * dest, const char * source, size_t length)
{
    if (dest == NULL || length == 0)
    {
        return 0;
    }
    if (source == NULL)
    {
        *dest = '\0';
        return 0;
    }
    char * d = dest;
    char * enddest = dest + length - 1;
    const char * s = source;
    while (d < enddest)
    {
        char in = *s;
        if (in == '\0')
        {
            break;
        }
        *d++ = in;
        s++;
    }
    *d = '\0';
    return d - dest;
}

// line 85
size_t Append(char * dest, const char * source, size_t length)
{
    if (dest == NULL || length == 0)
    {
        return 0;
    }
    char * d = dest;
    const char * s = source;
    char * enddest = dest + length - 1;
    while (d <= enddest && *d != '\0')
    {
        d++;
    }
    if (d <= enddest && s != NULL)
    {
        while (d < enddest)
        {
            char in = *s;
            if (in == '\0')
            {
                break;
            }
            *d++ = in;
            s++;
        }
        *d = '\0';
    }
    return d - dest;
}

// line 169
bool Equal(const char * s1, const char * s2)
{
    if (s1 == NULL || s2 == NULL)
    {
        return false;
    }
    return strcmp(s1, s2) == 0;
}

// line 174
bool Equal(const char * s1, const char * s2, size_t length)
{
    if (s1 == NULL || s2 == NULL)
    {
        return false;
    }
    return strncmp(s1, s2, length) == 0;
}

// line 185
int Compare(const char * s1, const char * s2)
{
    if (s1 == s2)
    {
        return 0;
    }
    if (s1 == NULL)
    {
        return -1;
    }
    if (s2 == NULL)
    {
        return 1;
    }
    return strcmp(s1, s2);
}

// line 195
int Compare(const char * s1, const char * s2, size_t length)
{
    if (s1 == s2)
    {
        return 0;
    }
    if (s1 == NULL)
    {
        return -1;
    }
    if (s2 == NULL)
    {
        return 1;
    }
    return strncmp(s1, s2, length);
}

// line 211
bool EqualI(const char * s1, const char * s2)
{
    if (s1 == NULL || s2 == NULL)
    {
        return false;
    }
    return strcasecmp(s1, s2) == 0;
}

// line 216
bool EqualI(const char * s1, const char * s2, size_t length)
{
    if (s1 == NULL || s2 == NULL)
    {
        return false;
    }
    return strncasecmp(s1, s2, length) == 0;
}

// line 227
int CompareI(const char * s1, const char * s2)
{
    if (s1 == s2)
    {
        return 0;
    }
    if (s1 == NULL)
    {
        return -1;
    }
    if (s2 == NULL)
    {
        return 1;
    }
    return strcasecmp(s1, s2);
}

// line 301
size_t Length(const char * buf)
{
    size_t length = 0;
    if (buf != NULL)
    {
        length = strlen(buf);
    }
    return length;
}

// line 314
void ToLower(char * buf)
{
    if (buf != NULL)
    {
        while (*buf != '\0')
        {
            char in = *buf;
            *buf++ = static_cast<char>(tolower(in));
        }
    }
}

// line 348
void StripWhitespace(char * buff)
{
    if (buff == NULL)
    {
        return;
    }
    // String.cpp:354 is the loop on *start, 356 the isspace test; 364 the all-blank store; 374
    // col 23 puts strlen inside parentheses "(strlen(buff) - 1)"; 377
    // col 10 the assignment of "*end-- = '\0'"; 384 col 28/37 "(end - start) + 2".
    char * start = buff;
    while (*start != '\0')
    {
        if (!isspace(*start))
        {
            break;
        }
        start++;
    }
    if (*start == '\0')
    {
        *buff = '\0';
        return;
    }
    char * end = buff + (strlen(buff) - 1);
    while (isspace(*end))
    {
        *end-- = '\0';
    }
    MemCopy(buff, start, (end - start) + 2);
}
}  // namespace String
}  // namespace Blizzard

namespace System_String {
// line 400
template <bool (*EqualFn)(const char *, const char *, size_t)>
const char * FindStringForward(const char * string, const char * search, size_t stringMaxLength)
{
    // The callers test string and search (Find: String.cpp:510); *search is tested here (405).
    if (*search == '\0')
    {
        return string;
    }
    size_t searchLen = Blizzard::String::Length(search);
    intptr maxLen = stringMaxLength + 1 - searchLen;
    for (char c = *string; maxLen > 0 && c != '\0'; c = *++string, maxLen--)
    {
        if (EqualFn(string, search, searchLen))
        {
            return string;
        }
    }
    return NULL;
}

// line 423
template <bool (*EqualFn)(const char *, const char *, size_t)>
const char * FindStringReverse(const char * string, const char * search, size_t stringMaxLength)
{
    // The callers test string and search (FindReverse: String.cpp:522). curr is line 431 (col 41
    // the subtraction), end 432. Inferred from the code: end is one before string (the loop exit
    // is an equality test), and the guard at 434 is a signed "curr > end".
    size_t searchLen = Blizzard::String::Length(search);
    size_t stringLength = Blizzard::String::Length(string);
    size_t stringLen = std::min(stringLength, stringMaxLength);
    const char * curr = string + stringLen - searchLen;
    const char * end = string - 1;
    if (curr > end)
    {
        for (; curr != end; curr--)
        {
            if (EqualFn(curr, search, searchLen))
            {
                return curr;
            }
        }
    }
    return NULL;
}

// inline on MatchSameChar, MatchMixedCase and MatchOnPathSeparator: MatchMixedCase (455) and
// MatchOnPathSeparator (460) have DW_AT_external, so they were not static, and none of the three
// has a concrete DIE (MatchSameChar has no DIE at all), so none was emitted out of line. This CU
// records no DW_INL_declared_inlined anywhere (std::min, which libc++ declares inline, is also
// DW_INL_inlined), so DW_INL_inlined does not rule out the keyword.
inline char MatchSameChar(char in)
{
    return in;
}

// line 455
inline char MatchMixedCase(char in)
{
    return static_cast<char>(tolower(in));
}

// line 460
inline char MatchOnPathSeparator(char in)
{
    return in == '\\' ? '/' : in;
}

// line 466
template <char (*MatchFn)(char)>
const char * FindCharForward(const char * string, char search, size_t stringMaxLength)
{
    // The callers test string and search (their own lines in the line table); the loop is
    // String.cpp:472 with local end, in at 474, both tests at 475.
    for (const char * end = string + stringMaxLength; string != end; string++)
    {
        char in = *string;
        if (in == '\0')
        {
            break;
        }
        if (MatchFn(in) == search)
        {
            return string;
        }
    }
    return NULL;
}

// line 486
template <char (*MatchFn)(char)>
const char * FindCharReverse(const char * string, char search, size_t stringMaxLength)
{
    // The callers test string and search. Loop String.cpp:493 with local end, in at 495, the
    // terminator test at 496, the match at 499.
    const char * match = NULL;
    for (const char * end = string + stringMaxLength; string != end; string++)
    {
        char in = *string;
        if (in == '\0')
        {
            break;
        }
        if (MatchFn(in) == search)
        {
            match = string;
        }
    }
    return match;
}
}  // namespace System_String

namespace Blizzard {
namespace String {
// line 508
const char * Find(const char * string, const char * search, size_t stringLength)
{
    if (string == NULL || search == NULL)
    {
        return NULL;
    }
    return System_String::FindStringForward<&Blizzard::String::Equal>(string, search, stringLength);
}

// line 514
// The string/search tests are on this function's own line 516 (line table); the helper call is
// on 517 (DWARF call_line). Same split in FindPathSeparator (558/559) and FindReverse (705/706).
const char * Find(const char * string, char search, size_t stringLength)
{
    if (string == NULL || search == '\0')
    {
        return NULL;
    }
    return System_String::FindCharForward<&System_String::MatchSameChar>(string, search, stringLength);
}

// line 520
const char * FindReverse(const char * string, const char * search, size_t stringLength = 0x7FFFFFFF)
{
    if (string == NULL || search == NULL)
    {
        return NULL;
    }
    return System_String::FindStringReverse<&Blizzard::String::Equal>(string, search, stringLength);
}

// line 532
const char * FindI(const char * string, const char * search, size_t stringLength)
{
    if (string == NULL || search == NULL)
    {
        return NULL;
    }
    return System_String::FindStringForward<&Blizzard::String::EqualI>(string, search, stringLength);
}

// line 556
const char * FindPathSeparator(const char * string, size_t stringLength)
{
    if (string == NULL)
    {
        return NULL;
    }
    return System_String::FindCharForward<&System_String::MatchOnPathSeparator>(string, '/', stringLength);
}

// NON_MATCHING (FindWholeWord and FindWholeWordI): 584 bytes against 588. Calls, lines and
// control flow agree; the original sign-extends chLeadingCharacter only at the isalnum call (its
// '_' test uses the raw byte), this one sign-extends it once after the 627 merge, which changes
// the tail-duplicated blocks of both loop copies. Four forms tried.
// line 617
// Columns: 626 col 43 is a direct strlen call (no inlined Length); 627 col 29 the
// parenthesised test; 631/632 the bools with parenthesised NUL tests; 634 col 18 the &&.
const char * FindWholeWord(const char * string, const char * search, size_t stringLength)
{
    // Inferred: no NULL test of its own (the inlined Find's tests are the first code, line 510).
    const char * start = string;
    for (;;)
    {
        const char * pResult = Find(string, search, stringLength);
        if (pResult == NULL)
        {
            return NULL;
        }
        char chTrailingCharacter = pResult[strlen(search)];
        char chLeadingCharacter = (pResult == start) ? '\0' : pResult[-1];
        if (((chLeadingCharacter | chTrailingCharacter) & 0x80) == 0)
        {
            bool bBackOkay = (chTrailingCharacter == '\0') || (!isalnum(chTrailingCharacter) && chTrailingCharacter != '_');
            bool bFrontOkay = (chLeadingCharacter == '\0') || (!isalnum(chLeadingCharacter) && chLeadingCharacter != '_');
            if (bBackOkay && bFrontOkay)
            {
                return pResult;
            }
        }
        string++;
        stringLength--;
    }
}

// line 645
const char * FindWholeWordI(const char * string, const char * search, size_t stringLength)
{
    // Inferred: no NULL test of its own, as in FindWholeWord.
    const char * start = string;
    for (;;)
    {
        const char * pResult = FindI(string, search, stringLength);
        if (pResult == NULL)
        {
            return NULL;
        }
        char chTrailingCharacter = pResult[strlen(search)];
        char chLeadingCharacter = (pResult == start) ? '\0' : pResult[-1];
        if (((chLeadingCharacter | chTrailingCharacter) & 0x80) == 0)
        {
            bool bBackOkay = (chTrailingCharacter == '\0') || (!isalnum(chTrailingCharacter) && chTrailingCharacter != '_');
            bool bFrontOkay = (chLeadingCharacter == '\0') || (!isalnum(chLeadingCharacter) && chLeadingCharacter != '_');
            if (bBackOkay && bFrontOkay)
            {
                return pResult;
            }
        }
        string++;
        stringLength--;
    }
}

// line 672
bool EndsWith(const char * string, const char * search)
{
    const char * start = FindReverse(string, search);
    return start != NULL && Length(start) == Length(search);
}

// line 703
char * FindReverse(char * string, char search, size_t stringLength)
{
    if (string == NULL || search == '\0')
    {
        return NULL;
    }
    return const_cast<char *>(
        System_String::FindCharReverse<&System_String::MatchSameChar>(string, search, stringLength));
}

// line 762
char * Translate(const char * source, char * dest, uint32 destSize, const char * from, const char * to)
{
    return TranslateEx(source, dest, destSize, from, to, 1, 0);
}

// NON_MATCHING: 864 bytes against 848. Written from the DWARF locals and their locations
// (pos = find result, source/dest = cursors, out = return value). Calls, control flow and branch
// order agree; source and out swap registers (x19/x21), and the inlined Copy of "to" (line 843)
// sets d after the length test instead of before it.
// line 776
char * TranslateEx(const char * source, char * dest, uint32 destSize, const char * from, const char * to,
                   int bMatchCase, int bMatchWholeWord)
{
    if (source == NULL || dest == NULL)
    {
        return const_cast<char *>(source);
    }
    if (from == NULL)
    {
        from = "";
    }
    uint32 fromLen = static_cast<uint32>(Length(from));
    if (to == NULL)
    {
        to = "";
    }
    uint32 toLen = static_cast<uint32>(Length(to));
    // DWARF locations: out (char *, line 795) is the returned value, starting as source; source
    // itself is the read cursor, dest the write cursor, and pos (810) the find result. The
    // const_cast is required by out's DWARF type.
    char * out = const_cast<char *>(source);
    // DWARF: FindFunction is a typedef local to TranslateEx (String.cpp:798).
    typedef const char * (*FindFunction)(const char *, const char *, size_t);
    FindFunction pfnFindInString;
    if (bMatchWholeWord)
    {
        pfnFindInString = bMatchCase ? FindWholeWord : FindWholeWordI;
    }
    else
    {
        pfnFindInString = bMatchCase ? static_cast<FindFunction>(Find) : FindI;
    }
    const char * pos;
    // 811 col 16: the call inside the assignment; 813 col 7: out; 816 col 28: the subtraction.
    while ((pos = pfnFindInString(source, from, 0x7FFFFFFF)) != NULL)
    {
        if (out == source)
        {
            out = dest;
        }
        uint32 skip = uint32(pos - source);
        // Branch order from the line table: the overflow case (818-832) precedes the normal
        // copy (838-845).
        if (skip + toLen >= destSize)
        {
            if (skip > destSize)
            {
                Copy(dest, source, destSize);
            }
            else
            {
                Copy(dest, source, skip + 1);
                dest += skip;
                Copy(dest, to, destSize - skip);
            }
            return out;
        }
        Copy(dest, source, skip + 1);
        dest += skip;
        source = pos + fromLen;
        Copy(dest, to, toLen + 1);
        dest += toLen;
        destSize -= skip + toLen;
    }
    Copy(dest, source, destSize);
    return out;
}

// line 856
uint32 Format(char * dest, uint32 maxchars, const char * format, ...)
{
    if (dest == NULL || maxchars == 0)
    {
        return 0;
    }
    if (format == NULL)
    {
        *dest = '\0';
        return 0;
    }
    va_list arglist;
    va_start(arglist, format);
    uint32 result = VFormat(dest, maxchars, format, arglist);
    va_end(arglist);
    return result;
}

// line 888
uint32 VFormat(char * dest, uint32 maxchars, const char * format, va_list arglist)
{
    char local_format[2048];
    uint32 result = 0;
    format = Translate(format, local_format, sizeof(local_format), "%I64", "%ll");
    if (format != NULL)
    {
        result = vsnprintf(dest, maxchars, format, arglist);
        dest[maxchars - 1] = '\0';
    }
    else if (maxchars != 0)
    {
        dest[0] = '\0';
    }
    return result;
}

// line 933
void * MemCopy(void * dest, const void * source, size_t size)
{
    if (static_cast<char *>(dest) < static_cast<const char *>(source) + size &&
        static_cast<const char *>(source) < static_cast<char *>(dest) + size)
    {
        memmove(dest, source, size);
    }
    else
    {
        memcpy(dest, source, size);
    }
    return dest;
}

// line 948
void MemFill(void * buf, size_t size, uint8 val)
{
    memset(buf, val, size);
}
}  // namespace String
}  // namespace Blizzard

// ---- Unicode.cpp ----
namespace System_Unicode {
typedef unsigned long UCS4;

static const UCS4 kReplacementCharacter = 0x0000FFFD;
static const UCS4 kMaximumUCS2 = 0x0000FFFF;
static const UCS4 kMaximumUTF16 = 0x0010FFFF;
static const UCS4 kMaximumUCS4 = 0x7FFFFFFF;

static const int halfShift = 10;
static const UCS4 halfBase = 0x0010000;
static const UCS4 halfMask = 0x3FF;
static const UCS4 kSurrogateHighStart = 0xD800;
static const UCS4 kSurrogateHighEnd = 0xDBFF;
static const UCS4 kSurrogateLowStart = 0xDC00;
static const UCS4 kSurrogateLowEnd = 0xDFFF;

static const UCS4 byteMask = 0xBF;
static const UCS4 byteMark = 0x80;

static const UCS4 s_offsetsFromUTF8[7] = {
    0x00000000, 0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080,
};

static const unsigned char s_bytesFromUTF8[256] = {
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6,
};

static const UCS4 s_firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
}  // namespace System_Unicode

namespace Blizzard {
namespace Unicode {
using namespace System_Unicode;

// line 129
size_t GetUTF8CharWidth(const char * c)
{
    size_t width = 0;
    uint8 x = static_cast<uint8>(*c);
    if ((x & 0x80) == 0)
    {
        if (x != 0)
        {
            return 1;
        }
    }
    else if ((x & 0xE0) == 0xC0)
    {
        return 2;
    }
    else if ((x & 0xF0) == 0xE0)
    {
        return 3;
    }
    else if ((x & 0xF8) == 0xF0)
    {
        width = 4;
    }
    return width;
}

// line 149
bool CountUTF8Chars(const char * s, size_t numBytes, size_t & numChars)
{
    numChars = 0;
    while (numBytes != 0 && s != NULL)
    {
        if (*s == '\0')
        {
            break;
        }
        size_t width = GetUTF8CharWidth(s);
        if (width == 0)
        {
            return false;
        }
        s++;
        numBytes--;
        for (size_t i = width; --i != 0;)
        {
            if (numBytes == 0 || (*s & 0xC0) != 0x80)
            {
                return false;
            }
            s++;
            numBytes--;
        }
        numChars++;
    }
    return true;
}

// line 264
int ConvertUTF16to8(char * dst, uint32 dstMaxChars, const utf16 * src, uint32 srcMaxChars, uint32 * dstChars,
                    uint32 * srcChars)
{
    const utf16 * srcStart = src;
    char * dstStart = dst;
    const utf16 * srcEnd = (srcMaxChars & 0x80000000) ? reinterpret_cast<const utf16 *>(-1) : src + srcMaxChars;
    int result = -1;
    char * dstEnd = dst + dstMaxChars;
    while (src < srcEnd)
    {
        unsigned int srcIndex = 0;
        UCS4 ch = src[srcIndex++];
        unsigned int bytes;
        if (ch >= kSurrogateHighStart && ch <= kSurrogateHighEnd)
        {
            if (src + srcIndex >= srcEnd)
            {
                break;
            }
            UCS4 ch2 = src[srcIndex];
            if (ch2 >= kSurrogateLowStart && ch2 <= kSurrogateLowEnd)
            {
                ch = ((ch - kSurrogateHighStart) << halfShift) + (ch2 - kSurrogateLowStart) + halfBase;
                srcIndex++;
            }
        }
        if (ch < 0x80)
        {
            if (ch == 0)
            {
                if (dst < dstEnd)
                {
                    *dst++ = '\0';
                    result = 0;
                }
                else
                {
                    result = 1;
                }
                break;
            }
            bytes = 1;
        }
        else if (ch < 0x800)
        {
            bytes = 2;
        }
        else if (ch < 0x10000)
        {
            bytes = 3;
        }
        else if (ch < 0x200000)
        {
            bytes = 4;
        }
        else if (ch < 0x4000000)
        {
            bytes = 5;
        }
        else if (ch <= kMaximumUCS4)
        {
            bytes = 6;
        }
        else
        {
            bytes = 2;
            ch = kReplacementCharacter;
        }
        dst += bytes;
        if (dst > dstEnd)
        {
            dst -= bytes;
            result = bytes;
            break;
        }
        switch (bytes)
        {
        case 6:
            *--dst = static_cast<char>((ch | byteMark) & byteMask);
            ch >>= 6;
        case 5:
            *--dst = static_cast<char>((ch | byteMark) & byteMask);
            ch >>= 6;
        case 4:
            *--dst = static_cast<char>((ch | byteMark) & byteMask);
            ch >>= 6;
        case 3:
            *--dst = static_cast<char>((ch | byteMark) & byteMask);
            ch >>= 6;
        case 2:
            *--dst = static_cast<char>((ch | byteMark) & byteMask);
            ch >>= 6;
        case 1:
            *--dst = static_cast<char>(ch | s_firstByteMark[bytes]);
        }
        dst += bytes;
        src += srcIndex;
    }
    if (srcChars != NULL)
    {
        *srcChars = static_cast<uint32>(src - srcStart);
    }
    if (dstChars != NULL)
    {
        *dstChars = static_cast<uint32>(dst - dstStart);
    }
    return result;
}

// line 451
unsigned int GetCodepointFromUTF8(const char ** src)
{
    const unsigned char * usrc = reinterpret_cast<const unsigned char *>(*src);
    UCS4 ch = 0;
    unsigned char in = *usrc++;
    unsigned int bytes = s_bytesFromUTF8[in];
    *src += bytes;
    switch (bytes)
    {
    case 6:
        ch += in;
        ch <<= 6;
        in = *usrc++;
        if ((in & 0xC0) != 0x80)
        {
            return kReplacementCharacter;
        }
    case 5:
        ch += in;
        ch <<= 6;
        in = *usrc++;
        if ((in & 0xC0) != 0x80)
        {
            return kReplacementCharacter;
        }
    case 4:
        ch += in;
        ch <<= 6;
        in = *usrc++;
        if ((in & 0xC0) != 0x80)
        {
            return kReplacementCharacter;
        }
    case 3:
        ch += in;
        ch <<= 6;
        in = *usrc++;
        if ((in & 0xC0) != 0x80)
        {
            return kReplacementCharacter;
        }
    case 2:
        ch += in;
        ch <<= 6;
        in = *usrc++;
        if ((in & 0xC0) != 0x80)
        {
            return kReplacementCharacter;
        }
    case 1:
        ch += in;
        ch -= s_offsetsFromUTF8[bytes];
    }
    return static_cast<unsigned int>(ch);
}

// line 481
GetUTF8FromCodepoint::GetUTF8FromCodepoint(unsigned int codepoint)
{
    {
        char * dst;
        unsigned int bytes;
        if (codepoint < 0x80)
        {
            bytes = 1;
        }
        else if (codepoint < 0x800)
        {
            bytes = 2;
        }
        else if (codepoint < 0x10000)
        {
            bytes = 3;
        }
        else if (codepoint < 0x200000)
        {
            bytes = 4;
        }
        else if (codepoint < 0x4000000)
        {
            bytes = 5;
        }
        else if (codepoint <= kMaximumUCS4)
        {
            bytes = 6;
        }
        else
        {
            bytes = 2;
            codepoint = kReplacementCharacter;
        }
        dst = m_buffer + bytes;
        *dst = '\0';
        switch (bytes)
        {
        case 6:
            *--dst = static_cast<char>((codepoint | byteMark) & byteMask);
            codepoint >>= 6;
        case 5:
            *--dst = static_cast<char>((codepoint | byteMark) & byteMask);
            codepoint >>= 6;
        case 4:
            *--dst = static_cast<char>((codepoint | byteMark) & byteMask);
            codepoint >>= 6;
        case 3:
            *--dst = static_cast<char>((codepoint | byteMark) & byteMask);
            codepoint >>= 6;
        case 2:
            *--dst = static_cast<char>((codepoint | byteMark) & byteMask);
            codepoint >>= 6;
        case 1:
            *--dst = static_cast<char>(codepoint | s_firstByteMark[bytes]);
        }
    }
}

// line 517
int ConvertUTF8to16(utf16 * dst, uint32 dstMaxChars, const char * src, uint32 srcMaxChars, uint32 * dstChars,
                    uint32 * srcChars)
{
    const char * srcStart = src;
    utf16 * dstStart = dst;
    const char * srcEnd = (srcMaxChars & 0x80000000) ? reinterpret_cast<const char *>(-1) : src + srcMaxChars;
    int result = -1;
    utf16 * dstEnd = dst + dstMaxChars;
    while (src < srcEnd)
    {
        unsigned int srcIndex = 0;
        UCS4 ch = 0;
        unsigned int bytes = s_bytesFromUTF8[static_cast<unsigned char>(*src)];
        const unsigned char * usrc = reinterpret_cast<const unsigned char *>(src);
        if (src + bytes > srcEnd)
        {
            result = -static_cast<int>(bytes);
            break;
        }
        switch (bytes)
        {
        case 6:
            ch += usrc[srcIndex++];
            ch <<= 6;
        case 5:
            ch += usrc[srcIndex++];
            ch <<= 6;
        case 4:
            ch += usrc[srcIndex++];
            ch <<= 6;
        case 3:
            ch += usrc[srcIndex++];
            ch <<= 6;
        case 2:
            ch += usrc[srcIndex++];
            ch <<= 6;
        case 1:
            ch += usrc[srcIndex++];
            ch -= s_offsetsFromUTF8[bytes];
        }
        if (dst >= dstEnd)
        {
            result = 1;
            break;
        }
        if (ch <= kMaximumUCS2)
        {
            *dst++ = static_cast<utf16>(ch);
            if (ch == 0)
            {
                result = 0;
                break;
            }
        }
        else if (ch > kMaximumUTF16)
        {
            *dst++ = static_cast<utf16>(kReplacementCharacter);
        }
        else
        {
            if (dst + 1 >= dstEnd)
            {
                result = 1;
                break;
            }
            ch -= halfBase;
            *dst++ = static_cast<utf16>((ch >> halfShift) + kSurrogateHighStart);
            *dst++ = static_cast<utf16>((ch & halfMask) + kSurrogateLowStart);
        }
        src += srcIndex;
    }
    if (srcChars != NULL)
    {
        *srcChars = static_cast<uint32>(src - srcStart);
    }
    if (dstChars != NULL)
    {
        *dstChars = static_cast<uint32>(dst - dstStart);
    }
    return result;
}
}  // namespace Unicode
}  // namespace Blizzard
