/* Original file: Contrib/Contrib/PCRE/pcre_tables.c (PCRE 8.35). Values from the original image;
_pcre_utf8_table3 (line 90) was dropped by the linker. */

#ifndef PCRE_INCLUDED
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pcre_internal.h"
#endif
















































const pcre_uint8 PRIV(OP_lengths)[] = { OP_LENGTHS };




const pcre_uint32 PRIV(hspace_list)[] = { HSPACE_LIST };
const pcre_uint32 PRIV(vspace_list)[] = { VSPACE_LIST };















const int PRIV(utf8_table1)[] =
  { 0x7f, 0x7ff, 0xffff, 0x1fffff, 0x3ffffff, 0x7fffffff};

const int PRIV(utf8_table1_size) = sizeof(PRIV(utf8_table1)) / sizeof(int);




const int PRIV(utf8_table2)[] = { 0,    0xc0, 0xe0, 0xf0, 0xf8, 0xfc};





const pcre_uint8 PRIV(utf8_table4)[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5 };







const pcre_uint32 PRIV(ucp_gentype)[] = {
  ucp_C, ucp_C, ucp_C, ucp_C, ucp_C, ucp_L,
  ucp_L, ucp_L, ucp_L, ucp_L, ucp_M, ucp_M,
  ucp_M, ucp_N, ucp_N, ucp_N, ucp_P, ucp_P,
  ucp_P, ucp_P, ucp_P, ucp_P, ucp_P, ucp_S,
  ucp_S, ucp_S, ucp_S, ucp_Z, ucp_Z, ucp_Z,
};






































const pcre_uint32 PRIV(ucp_gbtable[]) = {
   (1<<ucp_gbLF),  /* ucp_gbCR */
   0,  /* ucp_gbLF */
   0,  /* ucp_gbControl */
   (1<<ucp_gbExtend)|(1<<ucp_gbSpacingMark),  /* ucp_gbExtend */
   (1<<ucp_gbExtend)|(1<<ucp_gbPrepend)|(1<<ucp_gbSpacingMark)|(1<<ucp_gbL)|(1<<ucp_gbV)|(1<<ucp_gbT)|(1<<ucp_gbLV)|(1<<ucp_gbLVT)|(1<<ucp_gbOther),  /* ucp_gbPrepend */
   (1<<ucp_gbExtend)|(1<<ucp_gbSpacingMark),  /* ucp_gbSpacingMark */
   (1<<ucp_gbExtend)|(1<<ucp_gbSpacingMark)|(1<<ucp_gbL)|(1<<ucp_gbV)|(1<<ucp_gbLV)|(1<<ucp_gbLVT),  /* ucp_gbL */
   (1<<ucp_gbExtend)|(1<<ucp_gbSpacingMark)|(1<<ucp_gbV)|(1<<ucp_gbT),  /* ucp_gbV */
   (1<<ucp_gbExtend)|(1<<ucp_gbSpacingMark)|(1<<ucp_gbT),  /* ucp_gbT */
   (1<<ucp_gbExtend)|(1<<ucp_gbSpacingMark)|(1<<ucp_gbV)|(1<<ucp_gbT),  /* ucp_gbLV */
   (1<<ucp_gbExtend)|(1<<ucp_gbSpacingMark)|(1<<ucp_gbT),  /* ucp_gbLVT */
   (1<<ucp_gbRegionalIndicator),  /* ucp_gbRegionalIndicator */
   (1<<ucp_gbExtend)|(1<<ucp_gbSpacingMark),  /* ucp_gbOther */
};






























































































































































































const char PRIV(utt_names)[] =
  "Any\0"
  "Arabic\0"
  "Armenian\0"
  "Avestan\0"
  "Balinese\0"
  "Bamum\0"
  "Batak\0"
  "Bengali\0"
  "Bopomofo\0"
  "Brahmi\0"
  "Braille\0"
  "Buginese\0"
  "Buhid\0"
  "C\0"
  "Canadian_Aboriginal\0"
  "Carian\0"
  "Cc\0"
  "Cf\0"
  "Chakma\0"
  "Cham\0"
  "Cherokee\0"
  "Cn\0"
  "Co\0"
  "Common\0"
  "Coptic\0"
  "Cs\0"
  "Cuneiform\0"
  "Cypriot\0"
  "Cyrillic\0"
  "Deseret\0"
  "Devanagari\0"
  "Egyptian_Hieroglyphs\0"
  "Ethiopic\0"
  "Georgian\0"
  "Glagolitic\0"
  "Gothic\0"
  "Greek\0"
  "Gujarati\0"
  "Gurmukhi\0"
  "Han\0"
  "Hangul\0"
  "Hanunoo\0"
  "Hebrew\0"
  "Hiragana\0"
  "Imperial_Aramaic\0"
  "Inherited\0"
  "Inscriptional_Pahlavi\0"
  "Inscriptional_Parthian\0"
  "Javanese\0"
  "Kaithi\0"
  "Kannada\0"
  "Katakana\0"
  "Kayah_Li\0"
  "Kharoshthi\0"
  "Khmer\0"
  "L\0"
  "L&\0"
  "Lao\0"
  "Latin\0"
  "Lepcha\0"
  "Limbu\0"
  "Linear_B\0"
  "Lisu\0"
  "Ll\0"
  "Lm\0"
  "Lo\0"
  "Lt\0"
  "Lu\0"
  "Lycian\0"
  "Lydian\0"
  "M\0"
  "Malayalam\0"
  "Mandaic\0"
  "Mc\0"
  "Me\0"
  "Meetei_Mayek\0"
  "Meroitic_Cursive\0"
  "Meroitic_Hieroglyphs\0"
  "Miao\0"
  "Mn\0"
  "Mongolian\0"
  "Myanmar\0"
  "N\0"
  "Nd\0"
  "New_Tai_Lue\0"
  "Nko\0"
  "Nl\0"
  "No\0"
  "Ogham\0"
  "Ol_Chiki\0"
  "Old_Italic\0"
  "Old_Persian\0"
  "Old_South_Arabian\0"
  "Old_Turkic\0"
  "Oriya\0"
  "Osmanya\0"
  "P\0"
  "Pc\0"
  "Pd\0"
  "Pe\0"
  "Pf\0"
  "Phags_Pa\0"
  "Phoenician\0"
  "Pi\0"
  "Po\0"
  "Ps\0"
  "Rejang\0"
  "Runic\0"
  "S\0"
  "Samaritan\0"
  "Saurashtra\0"
  "Sc\0"
  "Sharada\0"
  "Shavian\0"
  "Sinhala\0"
  "Sk\0"
  "Sm\0"
  "So\0"
  "Sora_Sompeng\0"
  "Sundanese\0"
  "Syloti_Nagri\0"
  "Syriac\0"
  "Tagalog\0"
  "Tagbanwa\0"
  "Tai_Le\0"
  "Tai_Tham\0"
  "Tai_Viet\0"
  "Takri\0"
  "Tamil\0"
  "Telugu\0"
  "Thaana\0"
  "Thai\0"
  "Tibetan\0"
  "Tifinagh\0"
  "Ugaritic\0"
  "Vai\0"
  "Xan\0"
  "Xps\0"
  "Xsp\0"
  "Xuc\0"
  "Xwd\0"
  "Yi\0"
  "Z\0"
  "Zl\0"
  "Zp\0"
  "Zs\0";

const ucp_type_table PRIV(utt)[] = {
  {    0, PT_ANY, 0 },  /* Any */
  {    4, PT_SC, ucp_Arabic },  /* Arabic */
  {   11, PT_SC, ucp_Armenian },  /* Armenian */
  {   20, PT_SC, ucp_Avestan },  /* Avestan */
  {   28, PT_SC, ucp_Balinese },  /* Balinese */
  {   37, PT_SC, ucp_Bamum },  /* Bamum */
  {   43, PT_SC, ucp_Batak },  /* Batak */
  {   49, PT_SC, ucp_Bengali },  /* Bengali */
  {   57, PT_SC, ucp_Bopomofo },  /* Bopomofo */
  {   66, PT_SC, ucp_Brahmi },  /* Brahmi */
  {   73, PT_SC, ucp_Braille },  /* Braille */
  {   81, PT_SC, ucp_Buginese },  /* Buginese */
  {   90, PT_SC, ucp_Buhid },  /* Buhid */
  {   96, PT_GC, ucp_C },  /* C */
  {   98, PT_SC, ucp_Canadian_Aboriginal },  /* Canadian_Aboriginal */
  {  118, PT_SC, ucp_Carian },  /* Carian */
  {  125, PT_PC, ucp_Cc },  /* Cc */
  {  128, PT_PC, ucp_Cf },  /* Cf */
  {  131, PT_SC, ucp_Chakma },  /* Chakma */
  {  138, PT_SC, ucp_Cham },  /* Cham */
  {  143, PT_SC, ucp_Cherokee },  /* Cherokee */
  {  152, PT_PC, ucp_Cn },  /* Cn */
  {  155, PT_PC, ucp_Co },  /* Co */
  {  158, PT_SC, ucp_Common },  /* Common */
  {  165, PT_SC, ucp_Coptic },  /* Coptic */
  {  172, PT_PC, ucp_Cs },  /* Cs */
  {  175, PT_SC, ucp_Cuneiform },  /* Cuneiform */
  {  185, PT_SC, ucp_Cypriot },  /* Cypriot */
  {  193, PT_SC, ucp_Cyrillic },  /* Cyrillic */
  {  202, PT_SC, ucp_Deseret },  /* Deseret */
  {  210, PT_SC, ucp_Devanagari },  /* Devanagari */
  {  221, PT_SC, ucp_Egyptian_Hieroglyphs },  /* Egyptian_Hieroglyphs */
  {  242, PT_SC, ucp_Ethiopic },  /* Ethiopic */
  {  251, PT_SC, ucp_Georgian },  /* Georgian */
  {  260, PT_SC, ucp_Glagolitic },  /* Glagolitic */
  {  271, PT_SC, ucp_Gothic },  /* Gothic */
  {  278, PT_SC, ucp_Greek },  /* Greek */
  {  284, PT_SC, ucp_Gujarati },  /* Gujarati */
  {  293, PT_SC, ucp_Gurmukhi },  /* Gurmukhi */
  {  302, PT_SC, ucp_Han },  /* Han */
  {  306, PT_SC, ucp_Hangul },  /* Hangul */
  {  313, PT_SC, ucp_Hanunoo },  /* Hanunoo */
  {  321, PT_SC, ucp_Hebrew },  /* Hebrew */
  {  328, PT_SC, ucp_Hiragana },  /* Hiragana */
  {  337, PT_SC, ucp_Imperial_Aramaic },  /* Imperial_Aramaic */
  {  354, PT_SC, ucp_Inherited },  /* Inherited */
  {  364, PT_SC, ucp_Inscriptional_Pahlavi },  /* Inscriptional_Pahlavi */
  {  386, PT_SC, ucp_Inscriptional_Parthian },  /* Inscriptional_Parthian */
  {  409, PT_SC, ucp_Javanese },  /* Javanese */
  {  418, PT_SC, ucp_Kaithi },  /* Kaithi */
  {  425, PT_SC, ucp_Kannada },  /* Kannada */
  {  433, PT_SC, ucp_Katakana },  /* Katakana */
  {  442, PT_SC, ucp_Kayah_Li },  /* Kayah_Li */
  {  451, PT_SC, ucp_Kharoshthi },  /* Kharoshthi */
  {  462, PT_SC, ucp_Khmer },  /* Khmer */
  {  468, PT_GC, ucp_L },  /* L */
  {  470, PT_LAMP, 0 },  /* L& */
  {  473, PT_SC, ucp_Lao },  /* Lao */
  {  477, PT_SC, ucp_Latin },  /* Latin */
  {  483, PT_SC, ucp_Lepcha },  /* Lepcha */
  {  490, PT_SC, ucp_Limbu },  /* Limbu */
  {  496, PT_SC, ucp_Linear_B },  /* Linear_B */
  {  505, PT_SC, ucp_Lisu },  /* Lisu */
  {  510, PT_PC, ucp_Ll },  /* Ll */
  {  513, PT_PC, ucp_Lm },  /* Lm */
  {  516, PT_PC, ucp_Lo },  /* Lo */
  {  519, PT_PC, ucp_Lt },  /* Lt */
  {  522, PT_PC, ucp_Lu },  /* Lu */
  {  525, PT_SC, ucp_Lycian },  /* Lycian */
  {  532, PT_SC, ucp_Lydian },  /* Lydian */
  {  539, PT_GC, ucp_M },  /* M */
  {  541, PT_SC, ucp_Malayalam },  /* Malayalam */
  {  551, PT_SC, ucp_Mandaic },  /* Mandaic */
  {  559, PT_PC, ucp_Mc },  /* Mc */
  {  562, PT_PC, ucp_Me },  /* Me */
  {  565, PT_SC, ucp_Meetei_Mayek },  /* Meetei_Mayek */
  {  578, PT_SC, ucp_Meroitic_Cursive },  /* Meroitic_Cursive */
  {  595, PT_SC, ucp_Meroitic_Hieroglyphs },  /* Meroitic_Hieroglyphs */
  {  616, PT_SC, ucp_Miao },  /* Miao */
  {  621, PT_PC, ucp_Mn },  /* Mn */
  {  624, PT_SC, ucp_Mongolian },  /* Mongolian */
  {  634, PT_SC, ucp_Myanmar },  /* Myanmar */
  {  642, PT_GC, ucp_N },  /* N */
  {  644, PT_PC, ucp_Nd },  /* Nd */
  {  647, PT_SC, ucp_New_Tai_Lue },  /* New_Tai_Lue */
  {  659, PT_SC, ucp_Nko },  /* Nko */
  {  663, PT_PC, ucp_Nl },  /* Nl */
  {  666, PT_PC, ucp_No },  /* No */
  {  669, PT_SC, ucp_Ogham },  /* Ogham */
  {  675, PT_SC, ucp_Ol_Chiki },  /* Ol_Chiki */
  {  684, PT_SC, ucp_Old_Italic },  /* Old_Italic */
  {  695, PT_SC, ucp_Old_Persian },  /* Old_Persian */
  {  707, PT_SC, ucp_Old_South_Arabian },  /* Old_South_Arabian */
  {  725, PT_SC, ucp_Old_Turkic },  /* Old_Turkic */
  {  736, PT_SC, ucp_Oriya },  /* Oriya */
  {  742, PT_SC, ucp_Osmanya },  /* Osmanya */
  {  750, PT_GC, ucp_P },  /* P */
  {  752, PT_PC, ucp_Pc },  /* Pc */
  {  755, PT_PC, ucp_Pd },  /* Pd */
  {  758, PT_PC, ucp_Pe },  /* Pe */
  {  761, PT_PC, ucp_Pf },  /* Pf */
  {  764, PT_SC, ucp_Phags_Pa },  /* Phags_Pa */
  {  773, PT_SC, ucp_Phoenician },  /* Phoenician */
  {  784, PT_PC, ucp_Pi },  /* Pi */
  {  787, PT_PC, ucp_Po },  /* Po */
  {  790, PT_PC, ucp_Ps },  /* Ps */
  {  793, PT_SC, ucp_Rejang },  /* Rejang */
  {  800, PT_SC, ucp_Runic },  /* Runic */
  {  806, PT_GC, ucp_S },  /* S */
  {  808, PT_SC, ucp_Samaritan },  /* Samaritan */
  {  818, PT_SC, ucp_Saurashtra },  /* Saurashtra */
  {  829, PT_PC, ucp_Sc },  /* Sc */
  {  832, PT_SC, ucp_Sharada },  /* Sharada */
  {  840, PT_SC, ucp_Shavian },  /* Shavian */
  {  848, PT_SC, ucp_Sinhala },  /* Sinhala */
  {  856, PT_PC, ucp_Sk },  /* Sk */
  {  859, PT_PC, ucp_Sm },  /* Sm */
  {  862, PT_PC, ucp_So },  /* So */
  {  865, PT_SC, ucp_Sora_Sompeng },  /* Sora_Sompeng */
  {  878, PT_SC, ucp_Sundanese },  /* Sundanese */
  {  888, PT_SC, ucp_Syloti_Nagri },  /* Syloti_Nagri */
  {  901, PT_SC, ucp_Syriac },  /* Syriac */
  {  908, PT_SC, ucp_Tagalog },  /* Tagalog */
  {  916, PT_SC, ucp_Tagbanwa },  /* Tagbanwa */
  {  925, PT_SC, ucp_Tai_Le },  /* Tai_Le */
  {  932, PT_SC, ucp_Tai_Tham },  /* Tai_Tham */
  {  941, PT_SC, ucp_Tai_Viet },  /* Tai_Viet */
  {  950, PT_SC, ucp_Takri },  /* Takri */
  {  956, PT_SC, ucp_Tamil },  /* Tamil */
  {  962, PT_SC, ucp_Telugu },  /* Telugu */
  {  969, PT_SC, ucp_Thaana },  /* Thaana */
  {  976, PT_SC, ucp_Thai },  /* Thai */
  {  981, PT_SC, ucp_Tibetan },  /* Tibetan */
  {  989, PT_SC, ucp_Tifinagh },  /* Tifinagh */
  {  998, PT_SC, ucp_Ugaritic },  /* Ugaritic */
  { 1007, PT_SC, ucp_Vai },  /* Vai */
  { 1011, PT_ALNUM, 0 },  /* Xan */
  { 1015, PT_PXSPACE, 0 },  /* Xps */
  { 1019, PT_SPACE, 0 },  /* Xsp */
  { 1023, PT_UCNC, 0 },  /* Xuc */
  { 1027, PT_WORD, 0 },  /* Xwd */
  { 1031, PT_SC, ucp_Yi },  /* Yi */
  { 1034, PT_GC, ucp_Z },  /* Z */
  { 1036, PT_PC, ucp_Zl },  /* Zl */
  { 1039, PT_PC, ucp_Zp },  /* Zp */
  { 1042, PT_PC, ucp_Zs }   /* Zs */
};

const int PRIV(utt_size) = sizeof(PRIV(utt)) / sizeof(ucp_type_table);
