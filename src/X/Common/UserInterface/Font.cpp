// Font.cpp -- the shared half of the font module: the authored colour table
// every `{c:...}` markup run is built from, the specifier -> colour lookup,
// and the markup opener itself.  The client's own X/Client/Font/Font.cpp (the
// SNO-backed half, `FontSpecifierIsFont` and friends) is a different TU.

#include "FontDefinition.h"

#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/RefString.h"

// The authored colour table.  Its members are assigned by the constructor
// rather than aggregate-initialised so the global keeps align 4, and GlobalOpt
// folds the whole table to a static initialiser.  Values read out of main.elf
// (2.6.2 `_ZL17scgrgbaColorTable` 0x00e538a0, main.elf 0xec8fa0).
//
// Internal linkage (the `L` in the mangled name); a `FontSpecifier` below the
// table's length indexes it directly, which is exactly what
// `FontSpecifierIsColor` tests.  Every entry is opaque except that the fourth
// byte is the alpha the authored `{c:rrggbb}` run passes through.
//
// The table is exactly the authored order; do not reorder.
//
// (40 entries, 160 bytes)
static const RGBAColor scgrgbaColorTable[40] = {
#include "../generated-game-data/font_colors.inc"
};

// FontGetColorForFontSpecifier (2.6.2 0x005c5ee0, main.elf 0x0066c000) -- five
// instructions: index the packed colour table and store the word through the
// sret pointer.  The second argument is the 2.6.2 DWARF's
// `fIgnorePlayerOverrides`; no shipped instruction reads it, so the body is the
// bare table index.  The returned copy is the 32-bit
// `RGBAColor::Set(const RGBAColor&)` (RGBAColor.inl:124): `ldr w`/`str w` through the sret pointer.
RGBAColor FontGetColorForFontSpecifier(FontSpecifier eFS, BOOL fIgnorePlayerOverrides)
{
    return scgrgbaColorTable[eFS];
}

// FontSpecifierIsColor (2.6.2 0x005c5f00, main.elf 0x0066c020) -- one
// *unsigned* compare against the table's length (`cmp w0, #0x28` / `cset w0,
// lo`), so a negative specifier answers FALSE without a second test.
//
// The bound is written as the table length rather than the `FONTCOLOR_LAST`
// enumerator so the two stay in step with the table above; `FONTCOLOR_LAST` is
// 40 and the folded comparison is the same 0x28 either way.
//
// It answers TRUE for the whole `FontSpecifier` range the table covers, which
// is exactly the range `FontGetColorForFontSpecifier` may index; callers such
// as `sParseText` in X/Client/Font/FormattedText.cpp test it before every
// lookup, and fall back to the caller-supplied colour above the end.
//
// (Sibling note: the 1.0 build compares against the same 0x28, so the table
// length is unchanged between builds.)
BOOL FontSpecifierIsColor(FontSpecifier eFS)
{
    return (uint32)eFS < sizeof(scgrgbaColorTable) / sizeof(scgrgbaColorTable[0]);
}

// FontGetColorString (2.6.2 0x005c5f10) -- the opening `{c:rrggbb}` run for a
// packed colour.  One temporary `CRefString` is formatted and its characters
// are handed to the returned string's `LPCSTR` constructor, so the whole
// return statement is one expression and the temporary dies at its semicolon.
CRefString FontGetColorString(const RGBAColor &rgbaColor)
{
    UIColor color = rgbaColor.ToUIColor();
    return CRefString().Format("%s%8x%s", "{c:", color, "}");
}
