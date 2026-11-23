#include "../include/blz/format.h"
#include "../include/blz/char_traits.h"
#include "../include/blz/float.h"
#include "../include/blz/log.h"
#include "../include/blz/math.h"

#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Blizzard's printf engine. core_printf parses the format into a list of
// PrintfSpecs (which also supports positional %n$ arguments), fetches the
// arguments and hands each one to __OutputValue, which formats it into an
// Output: SprintfOutput (vsprintf), SnprintfOutput (snprintf),
// SprintfCountChar (vscprintf) or PrintfOutput (printf/vprintf, stdout).
// Type, member, parameter and local names are from the DWARF; the bodies
// follow the line info of the binary.

// Not in the DWARF: the bits of PrintfSpec::m_flags (from their uses:
// add_int_spec_flags maps showpos to 2 and showbase to 0x10, padding is '0'
// only when 4 is set and 1 is not). Invented names.
enum {
    PRINTF_FLAG_MINUS = 0x01,
    PRINTF_FLAG_PLUS = 0x02,
    PRINTF_FLAG_ZERO = 0x04,
    PRINTF_FLAG_SPACE = 0x08,
    PRINTF_FLAG_HASH = 0x10,
    PRINTF_FLAG_WIDTH_ARG = 0x40,      // m_width is an argument index
    PRINTF_FLAG_PRECISION_ARG = 0x80,  // m_precision is an argument index
};










// Not in the DWARF: the diagnostics go through blz::_log with category
// "format" at log_debug. Invented name.
#define PRINTF_LOG(...) blz::_log("format", blz::log_debug, __FILE__, __LINE__, __VA_ARGS__)

// Not in the DWARF: fetching one argument of a spec's type (every
// instruction of it is attributed to the line that uses it, so it is a
// macro). Invented name. Case groups inferred: the original dispatches m_type through a jump table (1824:21: sub #1, cmp #14, br), so the switch had more than three destinations before tail merging (three groups give bit tests); grouped here by the PrintfArg member filled.
#define PRINTF_GET_ARG(arg, argType, arglist)                                                \
    switch (argType)                                                                         \
    {                                                                                        \
    case PA_TYPE_CHAR:                                                                       \
    case PA_TYPE_WCHAR:                                                                      \
    case PA_TYPE_SHORT_INT:                                                                  \
    case PA_TYPE_INT:                                                                        \
    case PA_TYPE_INT32:                                                                      \
        (arg).paIntMax = va_arg(arglist, int);                                               \
        break;                                                                               \
    case PA_TYPE_DOUBLE:                                                                     \
        (arg).paDouble = va_arg(arglist, double);                                            \
        break;                                                                               \
    case PA_TYPE_LONG_INT:                                                                   \
    case PA_TYPE_LONG_LONG_INT:                                                              \
    case PA_TYPE_INT64:                                                                      \
    case PA_TYPE_INTMAX:                                                                     \
        (arg).paIntMax = va_arg(arglist, intmax_t);                                          \
        break;                                                                               \
    case PA_TYPE_STRING:                                                                     \
    case PA_TYPE_WSTRING:                                                                    \
    case PA_TYPE_POINTER:                                                                    \
        (arg).paPointer = va_arg(arglist, void*);                                            \
        break;                                                                               \
    case PA_TYPE_POINTER_DIFF:                                                               \
    case PA_TYPE_SIZE:                                                                       \
        (arg).paSize = va_arg(arglist, size_t);                                              \
        break;                                                                               \
    default:                                                                                 \
        (arg).paIntMax = 0;                                                                  \
        break;                                                                               \
    }

















































static const uint64_t UINT64_HIGH_NIBBLE_ROUND = 0x8000000000000000ULL;


static const int ARGMAX = 12;









enum Printf_Arg_Length {
    PA_LEN_NONE = 0,
    PA_LEN_h = 1,
    PA_LEN_l = 2,
    PA_LEN_ll = 3,
    PA_LEN_L = 4,
    PA_LEN_w = 5,
    PA_LEN_I = 6,
    PA_LEN_I32 = 7,
    PA_LEN_I64 = 8,
    PA_LEN_q = 9,
    PA_LEN_hh = 10,
    PA_LEN_z = 11,
    PA_LEN_j = 12,
    PA_LEN_t = 13,
};


enum Printf_Arg_Type {
    PA_TYPE_UNKNOWN = 0,
    PA_TYPE_CHAR = 1,
    PA_TYPE_WCHAR = 2,
    PA_TYPE_SHORT_INT = 3,
    PA_TYPE_INT = 4,
    PA_TYPE_LONG_INT = 5,
    PA_TYPE_LONG_LONG_INT = 6,
    PA_TYPE_INT32 = 7,
    PA_TYPE_INT64 = 8,
    PA_TYPE_INTMAX = 9,
    PA_TYPE_DOUBLE = 10,
    PA_TYPE_STRING = 11,
    PA_TYPE_WSTRING = 12,
    PA_TYPE_POINTER = 13,
    PA_TYPE_POINTER_DIFF = 14,
    PA_TYPE_SIZE = 15,
};



static const Printf_Arg_Type s_intTypeLookup[14] = {
    PA_TYPE_INT,            // PA_LEN_NONE
    PA_TYPE_SHORT_INT,      // PA_LEN_h
    PA_TYPE_LONG_INT,       // PA_LEN_l
    PA_TYPE_LONG_LONG_INT,  // PA_LEN_ll
    PA_TYPE_DOUBLE,         // PA_LEN_L
    PA_TYPE_UNKNOWN,        // PA_LEN_w
    PA_TYPE_SIZE,           // PA_LEN_I
    PA_TYPE_INT32,          // PA_LEN_I32
    PA_TYPE_INT64,          // PA_LEN_I64
    PA_TYPE_LONG_LONG_INT,  // PA_LEN_q
    PA_TYPE_CHAR,           // PA_LEN_hh
    PA_TYPE_SIZE,           // PA_LEN_z
    PA_TYPE_INTMAX,         // PA_LEN_j
    PA_TYPE_POINTER_DIFF,   // PA_LEN_t
};


template <typename T>
class PrintfSpec {

public:
    PrintfSpec() {}
    PrintfSpec(T format, Printf_Arg_Type type) : m_width(-1), m_precision(-1), m_format(format), m_type(type), m_flags(0) {}

    void Reset()
    {
        m_nextByArg = 0;
        m_nextByOut = 0;
        m_pSource = 0;
        m_sourceLen = 0;
        m_width = -1;
        m_precision = -1;
        m_format = 0;
        m_type = PA_TYPE_UNKNOWN;
        m_length = PA_LEN_NONE;
        m_flags = 0;
        m_position = -1;
    }

    PrintfSpec<T>* m_nextByArg;  // 0x00
    PrintfSpec<T>* m_nextByOut;  // 0x08
    const T* m_pSource;  // 0x10
    ptrdiff_t m_sourceLen;  // 0x18
    int m_width;  // 0x20
    int m_precision;  // 0x24
    T m_format;  // 0x28
    Printf_Arg_Type m_type;  // 0x2c
    Printf_Arg_Length m_length;  // 0x30
    uint8_t m_flags;  // 0x34
    int8_t m_position;  // 0x35
};


union PrintfArg {
    char paChar;
    wchar_t paWchar;
    intmax_t paIntMax;
    int32_t paInt32;
    int64_t paInt64;


    unsigned char paUChar;
    unsigned short paUShortInt;
    unsigned int paUInt;
    unsigned long paULongInt;
    unsigned long long paULongLongInt;
    uintmax_t paUIntMax;
    uint32_t paUInt32;
    uint64_t paUInt64;

    float paFloat;
    double paDouble;

    const char* paString;
    const wchar_t* paWstring;
    void* paPointer;

    ptrdiff_t paPtrDiff;
    size_t paSize;
};


















































template <typename T>
class PrintfSpecList {

public:
    PrintfSpecList()
        : m_headArg(0), m_tailArg(0), m_headOut(0)
        , m_tailOut(0), m_count(0), m_argCount(0)
    {
    }





    PrintfSpec<T>* GetFirstByArg() const { return m_headArg; }
    PrintfSpec<T>* GetFirstByOut() const { return m_headOut; }
    uint32_t GetArgCount() const { return m_argCount; }

    PrintfSpec<T>* Alloc();
    bool Add(PrintfSpec<T>* newSpec);
    bool Verify();

protected:
    PrintfSpec<T> m_spec[ARGMAX];  // 0x00
    PrintfSpec<T>* m_headArg;  // 0x2a0
    PrintfSpec<T>* m_tailArg;  // 0x2a8
    PrintfSpec<T>* m_headOut;  // 0x2b0
    PrintfSpec<T>* m_tailOut;  // 0x2b8
    uint32_t m_count;  // 0x2c0
    uint32_t m_argCount;  // 0x2c4
};


template <typename T>
PrintfSpec<T>* PrintfSpecList<T>::Alloc()
{
    if (m_count >= ARGMAX)
        return 0;

    PrintfSpec<T>* spec = &m_spec[m_count++];
    spec->Reset();
    return spec;
}


template <typename T>
bool PrintfSpecList<T>::Add(PrintfSpec<T>* newSpec)
{
    if (m_headOut == 0)
        m_headOut = newSpec;
    else
        m_tailOut->m_nextByOut = newSpec;
    m_tailOut = newSpec;
    newSpec->m_nextByOut = 0;

    ++m_argCount;
    if (newSpec->m_flags & PRINTF_FLAG_WIDTH_ARG)
        ++m_argCount;
    if (newSpec->m_flags & PRINTF_FLAG_PRECISION_ARG)
        ++m_argCount;

    if (newSpec->m_format == '%')
        return false;

    if (m_headArg == 0)
    {
        m_headArg = newSpec;
        m_tailArg = newSpec;
        newSpec->m_nextByArg = 0;
        return true;
    }










    bool listIsPositional = m_headArg->m_position != -1;
    bool newHasPosition = newSpec->m_position != -1;
    if (listIsPositional != newHasPosition)
        return false;

    if (newSpec->m_position == -1)
    {
        m_tailArg->m_nextByArg = newSpec;
        m_tailArg = newSpec;
        newSpec->m_nextByArg = 0;
        return true;
    }








    {
        PrintfSpec<T>* curSpec = m_headArg;
        PrintfSpec<T>* prevSpec = 0;
        while (curSpec != 0 && curSpec->m_position < newSpec->m_position)
        {
            prevSpec = curSpec;
            curSpec = curSpec->m_nextByArg;
        }
        if (prevSpec == 0)
            m_headArg = newSpec;
        else
            prevSpec->m_nextByArg = newSpec;
        newSpec->m_nextByArg = curSpec;
        if (curSpec == 0)
            m_tailArg = newSpec;
    }
    return true;
}








template <typename T>
bool PrintfSpecList<T>::Verify()
{
    if (m_headArg == 0)
        return true;
    if (m_headArg->m_position == -1)
    {
        int8_t curPosition = 0;
        for (PrintfSpec<T>* spec = m_headArg; spec != 0; spec = spec->m_nextByArg)
        {
            if (spec->m_width == -2)
            {
                spec->m_width = curPosition++;
                spec->m_flags |= PRINTF_FLAG_WIDTH_ARG;
            }
            if (spec->m_precision == -2)
            {
                spec->m_precision = curPosition++;
                spec->m_flags |= PRINTF_FLAG_PRECISION_ARG;
            }
            spec->m_position = curPosition++;
        }
        return true;
    }

    if (m_headArg->m_position > 0)
        return false;
    for (PrintfSpec<T>* spec = m_headArg; spec->m_nextByArg != 0; spec = spec->m_nextByArg)
    {
        if (spec->m_nextByArg->m_position - spec->m_position > 1)
            return false;
    }
    return true;
}









static void __GetUnsignedIntArg(const Printf_Arg_Type argType, PrintfArg arg, uintmax_t& value)
{


    switch (argType)
    {
    case PA_TYPE_CHAR:
        value = arg.paUChar;
        break;
    case PA_TYPE_WCHAR:
        value = arg.paUInt;
        break;
    case PA_TYPE_SHORT_INT:
        value = arg.paUShortInt;
        break;
    case PA_TYPE_INT:
    case PA_TYPE_INT32:
        value = arg.paUInt;
        break;
    case PA_TYPE_LONG_INT:
    case PA_TYPE_LONG_LONG_INT:
    case PA_TYPE_INT64:
    case PA_TYPE_INTMAX:
    case PA_TYPE_POINTER:
    case PA_TYPE_POINTER_DIFF:
    case PA_TYPE_SIZE:
        value = arg.paUIntMax;
        break;
    default:
        break;
    }
}
















static void __GetSignedIntArg(const Printf_Arg_Type argType, PrintfArg arg, uintmax_t& value, bool& isNegative)
{
    (void)argType;
    isNegative = arg.paIntMax < 0;
    value = isNegative ? -arg.paIntMax : arg.paIntMax;
}


// NON_MATCHING: sizes now equal (4124/4752/3996, Sprintf/Snprintf/Printf); 15/21/15 instructions
// differ: add operand order at 813 (orig numDigits + select, ours select + numDigits; `exponent +
// numDigits` gives the same) and block order of the %a leading digit at 1001-1004 (orig places the
// value != 0 digit inline and its '1'/'2' select carries 1004:17; `!precision && ...` and a ternary merge all three).
// instructions differ. Every case is on its DWARF lines. Left: the %g precision selects (800:35
// orig folds -1 into csinv, ours csneg + sub), add operand order at 813, block order of the %a
// leading digit (1001-1004) and of the %p width test (690); tried six spellings of 800-808.

template <typename T, template <typename> class Output>
static void __OutputValue(PrintfSpec<T>* spec, PrintfArg arg, Output<T>& output)
{
    // Anonymous enum in the DWARF (format.cpp:552); decimalPointLocation holds these.
    enum { DEC_PT_NONE, DEC_PT_INLINE, DEC_PT_BEFORE, DEC_PT_AFTER };










    T buffer[32];
    T* pBuffer = &buffer[31];
    *pBuffer = 0;

    int precision;
    int width = (spec->m_width == -1) ? 0 : spec->m_width;
    int bufferLen = 0;
    int outputLength = 0;  // initialiser inferred: integer, %s, %a and nan/inf paths reach 1118 with 0
    int exponent = 0;
    int numDigits;
    int decimalPointLocation = DEC_PT_NONE;
    int decimalPointOffset = 0;
    int extraTrailingZeroes = 0;
    int extraLeadingZeroes = 0;

    T outputFormat;
    T padChar = ' ';



    // Value inferred: caseAdjust (588) is m_format & 0x20 (`and w23, w11, #0x20` at 588:35).
    const T caseOffset = 'a' - 'A';



    T caseAdjust = spec->m_format & caseOffset;

    bool signedNumericType = false;
    bool isNegative = false;
    bool wideBuffer;  // DWARF local (format.cpp:592); no instruction uses it in the char instances
    bool hexPrefix = false;

    // 596:20/596:35: the m_format load and the `| 0x20`.
    outputFormat = spec->m_format | caseOffset;

    switch (spec->m_format)
    {



    case 'c':
        output.Emit(arg.paChar, 1);
        return;

    case 'C':
        output.Emit((arg.paUInt < 0x100) ? static_cast<T>(arg.paUInt) : '*', 1);
        return;



    case 'd':
    case 'i':
        {
            uintmax_t uInteger;
            __GetSignedIntArg(spec->m_type, arg, uInteger, isNegative);
            signedNumericType = true;
            precision = (spec->m_precision == -1) ? 1 : spec->m_precision;
            // width -= has no row of its own: merged into the csel/sub at 621:17.
            if (precision > 31)
            {
                extraLeadingZeroes = precision - 31;
                width -= extraLeadingZeroes;
                precision = 31;
            }


            while (uInteger || precision)
            {
                *--pBuffer = '0' + (char)(uInteger % 10);
                if (precision)
                    --precision;
                uInteger /= 10;
            }



            padChar = ((spec->m_flags & (PRINTF_FLAG_MINUS | PRINTF_FLAG_ZERO)) == PRINTF_FLAG_ZERO) ? '0' : ' ';


            bufferLen = &buffer[31] - pBuffer;
            break;
        }

    case 'o':
    case 'u':
        {
            uintmax_t uInteger = 0;
            __GetUnsignedIntArg(spec->m_type, arg, uInteger);


            precision = (spec->m_precision == -1) ? 1 : spec->m_precision;

            if (precision > 31)
            {
                extraLeadingZeroes = precision - 31;
                width -= extraLeadingZeroes;
                precision = 31;
            }


            uint64_t base = (spec->m_format == 'u') ? 10 : 8;
            while (uInteger || precision)
            {
                *--pBuffer = '0' + (char)(uInteger % base);
                if (precision)
                    --precision;
                uInteger /= base;
            }



            padChar = ((spec->m_flags & (PRINTF_FLAG_MINUS | PRINTF_FLAG_ZERO)) == PRINTF_FLAG_ZERO) ? '0' : ' ';


            bufferLen = &buffer[31] - pBuffer;
            break;
        }

    case 'p':
    case 'P':
    case 'x':
    case 'X':
        {
            uintmax_t uInteger = 0;
            __GetUnsignedIntArg(spec->m_type, arg, uInteger);

            // Spelling inferred: the orr #0x20 / cmp 'p' at 690:40 is evaluated before the width test (690:17); only 'p' || 'P' written first gives that order and size (columns would put the width test first)
            if ((spec->m_format == 'p' || spec->m_format == 'P') && spec->m_width == -1)
            {
                width = sizeof(void*) * 2;
                spec->m_flags |= PRINTF_FLAG_ZERO;
            }


            precision = (spec->m_precision == -1) ? 1 : spec->m_precision;

            if (precision > 31)
            {
                extraLeadingZeroes = precision - 31;
                width -= extraLeadingZeroes;
                precision = 31;
            }


            while (uInteger || precision)
            {
                T c = uInteger & 0xF;
                if (c < 10)
                {
                    c = '0' + c;
                }
                else
                {
                    c = ('A' + c - 10) | caseAdjust;
                }
                *--pBuffer = c;

                if (precision)
                    --precision;
                uInteger >>= 4;
            }

            hexPrefix = (spec->m_flags & PRINTF_FLAG_HASH) != 0;




            padChar = ((spec->m_flags & (PRINTF_FLAG_MINUS | PRINTF_FLAG_ZERO)) == PRINTF_FLAG_ZERO) ? '0' : ' ';


            bufferLen = &buffer[31] - pBuffer;
            break;
        }





    case 'e':
    case 'E':
    case 'f':
    case 'F':
    case 'g':
    case 'G':
        {
            precision = (spec->m_precision == -1) ? 6 : spec->m_precision;


            double value = arg.paDouble;
            int fpClass = blz::floatclassify(value);
            signedNumericType = true;

            if (fpClass > blz::FP_CLASS_INFINITE)
            {
                pBuffer = caseAdjust ? const_cast<T*>("nan") : const_cast<T*>("NAN");
                bufferLen = 3;
                break;
            }

            // 763:33 is the `<`: the comparison sits one column past `value`, hence the parentheses
            isNegative = (value < 0.0);


            if (fpClass == blz::FP_CLASS_INFINITE)
            {
                pBuffer = caseAdjust ? const_cast<T*>("inf") : const_cast<T*>("INF");
                bufferLen = 3;
                break;
            }


            if (value < 0.0)
                value = -value;

            // Initialiser inferred: no row at 778; the value == 0 path keeps d8 (value) as significand.
            double significand = value;
            if (value == 0.0)
            {
                exponent = 0;
            }
            else
            {
                // 786:33 fcvtms is the floor (column of `floor` after `(int)`), 786:39 the log10 call
                exponent = (int)floor(log10(value));
                significand = value * pow(10, -exponent);
            }

            // 792 has no row: the constant is merged into the select at 793 (emitTrailingZeroes = forceDecimal)
            bool forceDecimal = (spec->m_flags & PRINTF_FLAG_HASH) != 0;
            bool emitTrailingZeroes = true;
            if (outputFormat == 'g')
            {
                // emitTrailingZeroes moved to 807 (795 and 807 have no row): with it after the if/else the -1 sits in both branches and folds into the 800:35 selects (orig csinv ~exponent / -1)
                if (precision == 0)
                    precision = 1;

                // 800:30 `>` -5, 800:35 `&&`, 800:47 `<`; both branches merge into the selects at 800:35
                if (exponent > -5 && exponent < precision)
                    precision -= exponent + 1;
                else
                {
                    outputFormat = 'e';
                    --precision;
                }
                emitTrailingZeroes = forceDecimal;

            }


            numDigits = precision + 1;
            if (outputFormat != 'e')
                numDigits += exponent;

            int clampedPrecision = precision;
            if (numDigits > 15)
            {
                clampedPrecision -= numDigits - 15;
                numDigits = 15;
            }



            int digitsAfterDecimalpoint = numDigits - 1 -
                                          ((outputFormat == 'f' || outputFormat == 'g') ? exponent : 0);

            significand *= pow(10, numDigits - 1);
            if (clampedPrecision <= digitsAfterDecimalpoint)
                significand += 0.5;
            uint64_t uValue = (uint64_t)significand;

            do
            {

                *--pBuffer = '0' + (char)(uValue % 10);
            } while (uValue /= 10);

            bufferLen = &buffer[31] - pBuffer;
            if (numDigits > bufferLen)
            {
                numDigits = bufferLen;
            }
            else if (numDigits < bufferLen)
            {
                exponent += bufferLen - numDigits;
                numDigits = bufferLen;
            }


            if (outputFormat == 'e')
            {
                if (precision || forceDecimal)
                {
                    decimalPointLocation = DEC_PT_INLINE;
                    ++outputLength;
                }



                if (precision >= numDigits)
                {
                    extraTrailingZeroes = precision + 1 - numDigits;
                    outputLength += extraTrailingZeroes;
                }
            }
            else
            {

                digitsAfterDecimalpoint = numDigits - 1 - exponent;
                if (!emitTrailingZeroes && digitsAfterDecimalpoint < 0)
                    extraTrailingZeroes = -digitsAfterDecimalpoint;
                else if (emitTrailingZeroes && precision > digitsAfterDecimalpoint)
                    extraTrailingZeroes = precision - (outputFormat == 'g' ? numDigits : digitsAfterDecimalpoint);
                outputLength += extraTrailingZeroes;
                // 877:59 `&&`, 877:72 `>`, 877:84 `+` fit this spelling exactly
                bool nonZeroFractionPart = (!forceDecimal && numDigits > (exponent + 1));
                if (outputFormat == 'g')
                {
                    nonZeroFractionPart = false;
                    T* lastDigit = &buffer[30];
                    for (int n = digitsAfterDecimalpoint; n; --n)
                    {
                        if (*lastDigit != '0')
                        {
                            nonZeroFractionPart = true;
                            break;
                        }
                        if (!emitTrailingZeroes)
                        {
                            *lastDigit-- = 0;
                            --bufferLen;
                        }
                    }
                    numDigits = bufferLen;
                }

                if (forceDecimal || (emitTrailingZeroes ? spec->m_precision != 0 : nonZeroFractionPart))
                {
                    // 864:34 / 901:21 / 907:38: the %e/%f/%g case accumulates outputLength itself; 1118 adds bufferLen
                    ++outputLength;

                    if (exponent < 0)
                    {
                        decimalPointLocation = DEC_PT_BEFORE;
                        extraLeadingZeroes = -exponent;
                        outputLength += extraLeadingZeroes;
                    }
                    else if (exponent >= numDigits)
                    {
                        decimalPointLocation = DEC_PT_AFTER;
                        decimalPointOffset = exponent - numDigits + 1;
                    }
                    else
                    {
                        decimalPointLocation = DEC_PT_INLINE;
                        decimalPointOffset = exponent;
                    }
                }
            }

            // 923:37 `&&`, 923:79 `||`: the emitTrailingZeroes test is written first
            if (!emitTrailingZeroes && (decimalPointLocation == DEC_PT_INLINE || decimalPointLocation == DEC_PT_BEFORE))
            {
                T* lastDigit = &buffer[30];
                T* firstDigit;
                firstDigit = &buffer[31] - numDigits;
                if (decimalPointLocation == DEC_PT_INLINE)
                    firstDigit += decimalPointOffset;
                // 931:34 first `&&`, 931:55 second `&&` (holds both remaining tests)
                while (bufferLen && *lastDigit == '0' && lastDigit >= firstDigit)
                {
                    *lastDigit-- = 0;
                    --bufferLen;
                }
            }




            padChar = ((spec->m_flags & (PRINTF_FLAG_MINUS | PRINTF_FLAG_ZERO)) == PRINTF_FLAG_ZERO) ? '0' : ' ';
            break;
        }



    case 'a':
    case 'A':
        {



            precision = (spec->m_precision == -1) ? 13 : spec->m_precision;


            double value = arg.paDouble;
            int fpClass = blz::floatclassify(value);
            signedNumericType = true;

            if (fpClass > blz::FP_CLASS_INFINITE)
            {
                pBuffer = caseAdjust ? const_cast<T*>("nan") : const_cast<T*>("NAN");
                bufferLen = 3;
                break;
            }



            blz::F64 ieee;
            ieee.f = value;
            isNegative = ieee.bits.sign;


            if (fpClass == blz::FP_CLASS_INFINITE)
            {
                pBuffer = caseAdjust ? const_cast<T*>("inf") : const_cast<T*>("INF");
                bufferLen = 3;
                break;
            }


            if (isNegative)
                value = -value;


            double significand = frexp(value, &exponent);
            ieee.f = significand;
            if (value != 0.0)
                --exponent;
            uint64_t uSignificand = ieee.ui << 12;
            if (precision)
            {
                uint64_t round = UINT64_HIGH_NIBBLE_ROUND >> (precision * 4);
                uSignificand += round;
            }

            // 999:24 / 1000:24 are the `=` of `*pBuffer++ = `: the %a digits go through pBuffer (no other pointer local)
            pBuffer = buffer;
            *pBuffer++ = '0';
            *pBuffer++ = 'X' | caseAdjust;
            if (value == 0.0)
                *pBuffer++ = '0';
            else if ((uSignificand & UINT64_HIGH_NIBBLE_ROUND) && !precision)  // 1003: the high-bit test is evaluated first (cmp x8 then ccmp); columns 23/49 suggest the reverse
                *pBuffer++ = '2';
            else
                *pBuffer++ = '1';
            if (precision)
                *pBuffer++ = '.';

            // 1011:29 is the postfix `--`
            while (precision--)
            {
                T c = T(uSignificand >> 60);

                if (c < 10)
                    c += '0';
                else
                    c = ('A' + c - 10) | caseAdjust;
                *pBuffer++ = c;

                uSignificand <<= 4;
            }


            *pBuffer++ = 'P' | caseAdjust;

            if (exponent < 0)
            {
                *pBuffer++ = '-';
                exponent = -exponent;
            }
            else
            {
                *pBuffer++ = '+';
            }
            // `'0' | digit` (1043:34, 1049:34, 1055:34, 1057:30 are orr #0x30; `+` gives add): inferred spelling
            T ch;
            int originalExponent = exponent;
            if (originalExponent >= 1000)
            {
                ch = (char)(exponent / 1000);
                exponent -= ch * 1000;
                *pBuffer++ = '0' | ch;
            }
            if (originalExponent >= 100)
            {
                ch = (char)(exponent / 100);
                exponent -= ch * 100;
                *pBuffer++ = '0' | ch;
            }
            if (originalExponent >= 10)
            {
                ch = (char)(exponent / 10);
                exponent -= ch * 10;
                *pBuffer++ = '0' | ch;
            }
            *pBuffer++ = '0' | (char)exponent;


            *pBuffer = 0;
            bufferLen = pBuffer - buffer;
            pBuffer = buffer;
            break;
        }

    case 's':
        // p (1072) is declared inside the non-null branch: 1073 (16-column indent) holds the loop
            if (arg.paString != 0)
            {
                pBuffer = const_cast<T*>(arg.paString);
                // index loop with the post-increment (orig counts with x8 and subtracts 1 at 1084:23)
                const T* p = pBuffer;
                while (*p++) ++bufferLen;

            }
            else
            {
                pBuffer = const_cast<T*>("(null)");
                bufferLen = 6;
            }


            // 1084: the m_precision != -1 test (1084:35) comes before the length compare (1084:62)
            if (spec->m_precision != -1 && bufferLen > spec->m_precision)
                bufferLen = spec->m_precision;
            break;

    default:
        output.Emit(spec->m_format, 1);
        return;
    }

























    // 1118:18 is the `+=`: integer leading zeros are not counted here (width -= extraLeadingZeroes instead)
    outputLength += bufferLen;


    if (outputFormat == 'e')
        outputLength += 5;


    T signChar = 0;
    if (signedNumericType)
    {
        if (isNegative)
            signChar = '-';


        else if (spec->m_flags & PRINTF_FLAG_PLUS)
            signChar = '+';
        else if (spec->m_flags & PRINTF_FLAG_SPACE)
            signChar = ' ';

        if (signChar)
            ++outputLength;
    }

    // 1142:18 / 1142:29: `&&` then `==`; the same test with ' ' is at 1164.
    if (signChar && padChar == '0')
        output.Emit(signChar, 1);


    if (hexPrefix)
    {
        output.Emit('0', 1);
        output.Emit('X' | caseAdjust, 1);
    }


    if (!(spec->m_flags & PRINTF_FLAG_MINUS))
    {
        int diff = width - outputLength;
        if (diff > 0)
        {
            output.Emit(padChar, diff);
            width = outputLength;
        }
    }


    if (signChar && padChar == ' ')
        output.Emit(signChar, 1);


    if (extraLeadingZeroes > 0)
    {
        output.Emit('0', 1);
        if (decimalPointLocation == DEC_PT_BEFORE)
            output.Emit('.', 1);
        output.Emit('0', extraLeadingZeroes - 1);
    }


    // 1178:13 tests == DEC_PT_INLINE and falls through into the split output
    if (decimalPointLocation == DEC_PT_INLINE)
    {




        output.Emit(pBuffer, decimalPointOffset + 1);
        output.Emit('.', 1);
        output.Emit(pBuffer + decimalPointOffset + 1, bufferLen - decimalPointOffset - 1);
    }
    else
    {
        output.Emit(pBuffer, bufferLen);
    }








    if (extraTrailingZeroes > 0)
    {
        int count = (decimalPointLocation == DEC_PT_AFTER) ? decimalPointOffset : extraTrailingZeroes;
        output.Emit('0', count);
        if (decimalPointLocation == DEC_PT_AFTER)
        {
            output.Emit('.', 1);
            output.Emit('0', extraTrailingZeroes - count);
        }
    }


    if (outputFormat == 'e')
    {
        T exponentStr[5];

        exponentStr[0] = 'E' | caseAdjust;

        if (exponent < 0)
        {
            exponentStr[1] = '-';
            exponent = -exponent;
        }
        else
        {
            exponentStr[1] = '+';
        }
        // `'0' | digit`: 1228:30, 1230:30, 1232:30 are orr #0x30 (exponent is in memory, so `+` gives add)
        exponentStr[4] = '0' | (char)(exponent % 10);
        exponent /= 10;
        exponentStr[3] = '0' | (char)(exponent % 10);
        exponent /= 10;
        exponentStr[2] = '0' | (char)(exponent % 10);
        output.Emit(exponentStr, 5);
    }


    width -= outputLength;


    // 1241:15 (m_flags) precedes 1241:46 (width): flag test first
    if ((spec->m_flags & PRINTF_FLAG_MINUS) && width > 0)
        output.Emit(' ', width);
}


template <typename T>
static bool __IsDigit(T c)
{
    return c >= '0' && c <= '9';
}




template <typename T>
static int32_t __ReadInt(const T** pstr)
{
    uint32_t retval = **pstr - '0';
    bool overflow = false;

    while (__IsDigit(*++*pstr))
    {
        if (retval > 0x0CCCCCCB)  // 1263:13 / 1263:20 are the `(retval` and `>` of an if; 1264 has no row
            overflow = true;
        retval *= 10;
        retval += **pstr - '0';
    }

    return overflow ? -1 : static_cast<int32_t>(retval);
}


enum Printf_Form_State {
    FORM_POSITION = 0,
    FORM_FLAGS = 1,
    FORM_WIDTH = 2,
    FORM_PRECISION = 3,
    FORM_LENGTH = 4,
};

// The parts of a format spec must come in order: checks that nothing which
// belongs after `state` has been seen yet.
template <typename T>
static bool __formCheck(Printf_Form_State state, const PrintfSpec<T>* spec, const T* format, bool* abortThis)
{


    bool result = true;
    const char* message;

    switch (state)
    {
    case FORM_POSITION:

        message = "position";

        if (spec->m_position != -1)
            result = false;







        if (spec->m_flags != 0)  // 1306:19 / 1306:13: the flags test sits seven lines below the position test
            result = false;

    case FORM_FLAGS:
        if (state == FORM_FLAGS)
            message = "flags";

    case FORM_WIDTH:
        if (state == FORM_WIDTH)
            message = "width";

        if (spec->m_width != -1)
            result = false;

    case FORM_PRECISION:
        if (state == FORM_PRECISION)
            message = "precision";


        if (spec->m_precision != -1)
            result = false;

    case FORM_LENGTH:
        if (state == FORM_LENGTH)
            message = "length";


        if (spec->m_length != PA_LEN_NONE)
            result = false;

    default:

        if (spec->m_format != 0)
            result = false;
    }




    if (!result)
    {
        PRINTF_LOG("Encountered poorly formed format (misplaced %s) in '%s'", message, format);
        *abortThis = true;
    }
    return result;
}


// %lc/%ls are the wide forms, %hC/%hS the narrow ones.
template <typename T>
static void FixupEquivalentTypes(PrintfSpec<T>* spec)
{
    switch (spec->m_format)
    {
    case 'c':
        if (spec->m_length == PA_LEN_w || spec->m_length == PA_LEN_l)
        {
            spec->m_format = 'C';
            spec->m_type = PA_TYPE_WCHAR;
            spec->m_length = PA_LEN_NONE;
        }
        break;
    case 'C':
        if (spec->m_length == PA_LEN_h)
        {
            spec->m_format = 'c';
            spec->m_type = PA_TYPE_CHAR;
            spec->m_length = PA_LEN_NONE;
        }
        break;
    case 's':
        if (spec->m_length == PA_LEN_w || spec->m_length == PA_LEN_l)
        {
            spec->m_format = 'S';
            spec->m_type = PA_TYPE_WSTRING;
            spec->m_length = PA_LEN_NONE;
        }
        break;
    case 'S':
        if (spec->m_length == PA_LEN_h)
        {
            spec->m_format = 's';
            spec->m_type = PA_TYPE_STRING;
            spec->m_length = PA_LEN_NONE;
        }
        break;
    }
}
// NON_MATCHING (all three instances and the copy inlined in vscprintf): frame now 0x340 as the original; left: block
// layout (loop head placed last), allowPositional spilled and registers rotated in the digit case, formCheck tests merged ((w & p) == -1), fewer shared log calls; see build/try/draft/format.txt.
template <typename T, template <typename> class Output>
static void core_printf(const T* format, va_list arglist, bool allowPositional, Output<T>& output)
{

    if (format == 0 || *format == 0)
        return;

    const T* formatPtr = format;

    PrintfSpecList<T> list;

    PrintfSpec<T>* curSpec = 0;

    bool isFormat = false;
    bool isFormatEnd = false;
    bool abortThis = false;

    T curChar;
    for (; (curChar = *formatPtr) != 0; ++formatPtr)
    {
        if (isFormat)
        {
            switch (curChar)
            {

            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
                if (curSpec->m_length == PA_LEN_L)
                    PRINTF_LOG("invalid length modifier for format '%c' in '%s' (ignored)", curChar, format);
                curSpec->m_format = curChar;
                curSpec->m_type = s_intTypeLookup[curSpec->m_length];
                isFormatEnd = true;
                break;

            case 'a':
            case 'A':
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
                if (curSpec->m_length != PA_LEN_NONE &&
                    curSpec->m_length != PA_LEN_L)
                {

                    PRINTF_LOG("invalid length modifier for format '%c' in '%s' (ignored)", curChar, format);
                }





                curSpec->m_format = curChar;
                curSpec->m_type = curSpec->m_length ? s_intTypeLookup[curSpec->m_length] : PA_TYPE_DOUBLE;
                isFormatEnd = true;
                break;

            case 's':
                if (curSpec->m_length != PA_LEN_NONE && curSpec->m_length != PA_LEN_h)  // unsigned `cmp #2; b.lo` at 1459:39, like 'C' at 1483
                    PRINTF_LOG("invalid or unsupported length modifier for format '%c' in '%s' (ignored)", 's', format);
                curSpec->m_format = curChar;
                curSpec->m_type = PA_TYPE_STRING;
                isFormatEnd = true;
                break;

            case 'c':
                if (curSpec->m_length != PA_LEN_NONE && curSpec->m_length != PA_LEN_h)  // unsigned `cmp #2; b.lo` at 1467:39, as at 1459
                    PRINTF_LOG("invalid or unsupported length modifier for format '%c' in '%s' (ignored)", 'c', format);
                curSpec->m_format = curChar;
                curSpec->m_type = PA_TYPE_CHAR;
                isFormatEnd = true;
                break;

            case 'p':
                if (curSpec->m_length != PA_LEN_NONE)
                    PRINTF_LOG("invalid length modifier for format '%c' in '%s' (ignored)", 'p', format);
                curSpec->m_format = curChar;
                curSpec->m_type = PA_TYPE_POINTER;
                isFormatEnd = true;
                break;

            case 'C':
                if (curSpec->m_length != PA_LEN_NONE && curSpec->m_length != PA_LEN_l)
                    PRINTF_LOG("invalid or unsupported length modifier for format '%c' in '%s' (ignored)", 'C', format);
                curSpec->m_format = curChar;
                curSpec->m_type = PA_TYPE_WCHAR;
                isFormatEnd = true;
                break;

            case 'S':
                if (curSpec->m_length != PA_LEN_NONE && curSpec->m_length != PA_LEN_l)
                    PRINTF_LOG("invalid or unsupported length modifier for format '%c' in '%s' (ignored)", 'S', format);
                curSpec->m_format = curChar;
                curSpec->m_type = PA_TYPE_WSTRING;
                isFormatEnd = true;
                break;

            case 'n':
                PRINTF_LOG("unsupported format '%c' in '%s' (mangled)", 'n', format);
                curSpec->Reset();
                curSpec->m_format = curChar;
                isFormatEnd = true;
                break;


            case '-':
                if (!__formCheck(FORM_FLAGS, curSpec, format, &abortThis))
                {
                    curSpec->Reset();
                    curSpec->m_format = curChar;
                    break;
                }

                curSpec->m_flags |= PRINTF_FLAG_MINUS;
                break;

            case '+':
                __formCheck(FORM_FLAGS, curSpec, format, &abortThis);
                curSpec->m_flags |= PRINTF_FLAG_PLUS;
                break;

            case ' ':
                __formCheck(FORM_FLAGS, curSpec, format, &abortThis);
                curSpec->m_flags |= PRINTF_FLAG_SPACE;
                break;

            case '#':
                __formCheck(FORM_FLAGS, curSpec, format, &abortThis);
                curSpec->m_flags |= PRINTF_FLAG_HASH;
                break;
            case '.':
                {

                    if (curSpec->m_width == -1)
                        curSpec->m_width = 0;


                    T nextChar = formatPtr[1];
                    if (!__IsDigit(nextChar) && nextChar != '*')
                        curSpec->m_precision = 0;
                }
                break;

            case '0':
                if (curSpec->m_width == -1)
                {
                    __formCheck(FORM_FLAGS, curSpec, format, &abortThis);
                    curSpec->m_flags |= PRINTF_FLAG_ZERO;
                    break;
                }
                // a '0' after the width is a digit of the width or precision
            case '*':
            case '1': case '2': case '3':
            case '4': case '5': case '6':
            case '7': case '8': case '9':
                {


                    int32_t n;
                    bool isPositional = false;
                    T nextChar;

                    if (curChar == '*')
                    {


                        nextChar = formatPtr[1];
                        if (__IsDigit(nextChar))
                        {
                            ++formatPtr;
                            n = __ReadInt(&formatPtr);
                            if (n == -1 || n > ARGMAX)
                            {
                                PRINTF_LOG("invalid argument position in '%s'", format);
                                abortThis = true;
                                break;
                            }
                            n -= 1;



                            if (*formatPtr != '$')
                            {
                                PRINTF_LOG("Encountered poorly formed format (missing $ in positional width or precision) in '%s'", format);
                                abortThis = true;
                                break;
                            }
                            if (!allowPositional)
                            {
                                PRINTF_LOG("ordered format used in an unordered format");
                                abortThis = true;
                                break;
                            }

                            isPositional = true;
                            ++formatPtr;
                        }
                        else
                        {
                            n = -2;
                            ++formatPtr;
                        }
                    }
                    else
                    {
                        n = __ReadInt(&formatPtr);
                        if (n == -1)
                        {
                            PRINTF_LOG("integer overflow in '%s'", format);
                            abortThis = true;
                            break;
                        }
                    }


                    nextChar = *formatPtr;

                    // 1620:25 carries the '.' tests too: the if chain became one switch on nextChar

                    if (nextChar == '$')
                    {
                        __formCheck(FORM_POSITION, curSpec, format, &abortThis);

                        if (!allowPositional)
                        {
                            PRINTF_LOG("ordered format used in an unordered format");
                            abortThis = true;
                            break;
                        }
                        if (n > ARGMAX)
                        {
                            PRINTF_LOG("invalid argument position in '%s'", format);
                            abortThis = true;
                            break;
                        }
                        curSpec->m_position = n - 1;
                    }
                    else if (nextChar == '.')
                    {

                        if (__formCheck(FORM_WIDTH, curSpec, format, &abortThis))
                        {
                            curSpec->m_width = n;
                            if (isPositional)
                                curSpec->m_flags |= PRINTF_FLAG_WIDTH_ARG;
                        }
                        --formatPtr;
                    }
                    else
                    {
                        --formatPtr;
                        if (curSpec->m_width != -1)
                        {
                            if (__formCheck(FORM_PRECISION, curSpec, format, &abortThis))
                            {
                                curSpec->m_precision = n;
                                if (isPositional)
                                    curSpec->m_flags |= PRINTF_FLAG_PRECISION_ARG;
                            }
                        }
                        else
                        {
                            if (__formCheck(FORM_WIDTH, curSpec, format, &abortThis))
                            {
                                curSpec->m_width = n;
                                if (isPositional)
                                    curSpec->m_flags |= PRINTF_FLAG_WIDTH_ARG;
                            }
                        }
                    }
                }
                break;

            case 'I':
                {

                    T nextChar = formatPtr[1];
                    if (__IsDigit(nextChar))
                    {
                        ++formatPtr;

                        int32_t n = __ReadInt(&formatPtr);
                        if (n == 32)
                            curSpec->m_length = PA_LEN_I32;
                        else if (n == 64)
                            curSpec->m_length = PA_LEN_I64;
                        else
                        {




                            PRINTF_LOG("Encountered poorly formed format (unknown I length variant %d) in %s", n, format);
                            return;
                        }
                        --formatPtr;
                    }
                    else
                    {


                        curSpec->m_length = PA_LEN_I;
                    }
                }
                break;


            case 'h':
                if (__formCheck(FORM_LENGTH, curSpec, format, &abortThis))
                {


                    T nextChar = formatPtr[1];
                    if (nextChar == 'h')
                    {
                        ++formatPtr;
                        curSpec->m_length = PA_LEN_hh;
                    }
                    else
                        curSpec->m_length = PA_LEN_h;
                }
                break;




            case 'l':
                if (__formCheck(FORM_LENGTH, curSpec, format, &abortThis))
                {


                    T nextChar = formatPtr[1];
                    if (nextChar == 'l')
                    {
                        ++formatPtr;
                        curSpec->m_length = PA_LEN_ll;
                    }
                    else
                        curSpec->m_length = PA_LEN_l;
                }
                break;



            case 'L':
                __formCheck(FORM_LENGTH, curSpec, format, &abortThis);
                curSpec->m_length = PA_LEN_L;
                break;

            case 'q':
                __formCheck(FORM_LENGTH, curSpec, format, &abortThis);
                curSpec->m_length = PA_LEN_q;
                break;

            case 'z':
                __formCheck(FORM_LENGTH, curSpec, format, &abortThis);
                curSpec->m_length = PA_LEN_z;
                break;

            case 'j':
                __formCheck(FORM_LENGTH, curSpec, format, &abortThis);
                curSpec->m_length = PA_LEN_j;
                break;

            case 't':
                __formCheck(FORM_LENGTH, curSpec, format, &abortThis);
                curSpec->m_length = PA_LEN_t;
                break;

            case 'w':
                __formCheck(FORM_LENGTH, curSpec, format, &abortThis);
                curSpec->m_length = PA_LEN_l;
                break;

            case '%':
                curSpec->m_sourceLen = formatPtr - curSpec->m_pSource + 1;
                if (curSpec->m_sourceLen == 2)
                {
                    curSpec->m_format = curChar;
                    curSpec->m_pSource += 1;
                    isFormatEnd = true;
                    break;
                }
                // fall through

            default:
                PRINTF_LOG("Unexpected character '%c' in '%s' (mangled)", curChar, format);
                abortThis = true;
                break;
            }




            if (abortThis)
            {
                curSpec->Reset();
                curSpec->m_format = curChar;
                isFormatEnd = true;
                abortThis = false;
            }

            if (isFormatEnd)
            {
                FixupEquivalentTypes(curSpec);


                if (!allowPositional)
                {
                    PrintfArg arg;

                    if (curSpec->m_width == -2)
                    {
                        PRINTF_GET_ARG(arg, PA_TYPE_INT, arglist);
                        curSpec->m_width = arg.paInt32;
                    }

                    if (curSpec->m_precision == -2)
                    {
                        PRINTF_GET_ARG(arg, PA_TYPE_INT, arglist);
                        curSpec->m_precision = arg.paInt32;
                    }

                    PRINTF_GET_ARG(arg, curSpec->m_type, arglist);
                    __OutputValue(curSpec, arg, output);
                }
                else
                {

                    curSpec->m_sourceLen = formatPtr + 1 - curSpec->m_pSource;
                    list.Add(curSpec);
                    curSpec = 0;

                }
                isFormatEnd = false;
                isFormat = false;
            }
        }

        else if (curChar == '%')
        {

            if (curSpec == 0)
            {
                curSpec = list.Alloc();
                if (curSpec == 0)
                {

                    PRINTF_LOG("too many positional arguments in '%s', max is %d", format, ARGMAX);
                    return;
                }
            }


            // Inferred placement (no row anywhere): abortThis is cleared in the abort block (1800), isFormatEnd at the end of the isFormatEnd block (1835): in the original neither is loop-carried (DWARF: constant ranges; abortThis tested with cbnz)
            curSpec->Reset();
                curSpec->m_pSource = formatPtr;


            isFormat = true;
        }
        else
        {
            if (!allowPositional)
                output.Emit(curChar, 1);
        }
    }




    if (!allowPositional)
        return;


    if (!list.Verify())  // 1876 is the Verify call line (inlined, 444-476); its branch has no row
        return;







    PrintfArg args[ARGMAX];
    PrintfSpec<T>* spec;
    int argPosition = 0;
    for (spec = list.GetFirstByArg(); spec != 0; spec = spec->m_nextByArg)
    {
        while (argPosition <= spec->m_position)
        {
            Printf_Arg_Type argType = (argPosition == spec->m_position) ? spec->m_type : PA_TYPE_INT;

            PrintfArg arg;
            PRINTF_GET_ARG(arg, argType, arglist);

            args[argPosition] = arg;
            argPosition++;
        }
    }
    while (static_cast<uint32_t>(argPosition) < list.GetArgCount())
    {
        PrintfArg arg;
        PRINTF_GET_ARG(arg, PA_TYPE_INT, arglist);
        args[argPosition++] = arg;
    }

    formatPtr = format;
    spec = list.GetFirstByOut();


    while (*formatPtr)
    {
        if (spec != 0)
        {

            ptrdiff_t count = spec->m_pSource - formatPtr;
            while (count--)
            {
                output.Emit(*formatPtr++, 1);
            }

            if (spec->m_position != -1)
            {

                if (spec->m_flags & PRINTF_FLAG_WIDTH_ARG)
                    spec->m_width = args[spec->m_width].paInt32;
                if (spec->m_flags & PRINTF_FLAG_PRECISION_ARG)
                    spec->m_precision = args[spec->m_precision].paInt32;
                __OutputValue(spec, args[spec->m_position], output);
            }


            formatPtr += spec->m_sourceLen;


            spec = spec->m_nextByOut;
        }
        else
        {

            output.Emit(*formatPtr++, 1);
        }
    }
}


template <typename T>
class SprintfOutput {


    T* m_buf;  // 0x00
    T* m_bufPtr;  // 0x08
    size_t m_maxlen;  // 0x10

public:
    SprintfOutput(T* buf, size_t bufLen)
        : m_buf(buf)
        , m_bufPtr(buf)
        , m_maxlen(bufLen)
    {
    }
    void Emit(T ch, size_t count)
    {
        for (; count != 0 && static_cast<size_t>(m_bufPtr - m_buf) < m_maxlen; --count)
            *m_bufPtr++ = ch;
    }

    void Emit(const T* s, size_t count)
    {
        for (; count != 0 && static_cast<size_t>(m_bufPtr - m_buf) < m_maxlen; --count)
            *m_bufPtr++ = *s++;
    }

    int Finish()
    {
        size_t len = m_bufPtr - m_buf;
        if (len >= m_maxlen)
        {
            m_bufPtr = m_buf + m_maxlen - 1;
            len = m_maxlen - 1;
        }
        *m_bufPtr = 0;
        return static_cast<int>(len);
    }
};
namespace blz {



































int vsprintf(char* __restrict dest, size_t maxlen, const char* format, va_list arglist)
{

    if (dest == 0 || format == 0 || maxlen == 0)
        return -1;

    SprintfOutput<char> output(dest, maxlen);
    core_printf(format, arglist, false, output);
    return output.Finish();
}












































// LINKER-REMOVED: DWARF subprogram at format.cpp:2081 (low_pc 0) with params
// dest, maxlen, format, arglist and local output at 2087, no code). Body is an
// inference: core_printf<char, SprintfOutput> keeps its positional code, so it
// had a caller passing allowPositional = true ("o" = ordered, see the log text).
int vsoprintf(char* __restrict dest, size_t maxlen, const char* format, va_list arglist)
{

    if (dest == 0 || format == 0 || maxlen == 0)
        return -1;

    SprintfOutput<char> output(dest, maxlen);
    core_printf(format, arglist, true, output);
    return output.Finish();
}












}  // namespace blz

template <typename T>
class SnprintfOutput {


    T* m_buf;  // 0x00
    T* m_bufPtr;  // 0x08
    size_t m_maxlen;  // 0x10
    size_t m_len;  // 0x18

public:
    SnprintfOutput(T* buf, size_t bufLen)
        : m_buf(buf)
        , m_bufPtr(buf)
        , m_maxlen(bufLen)
        , m_len(0)
    {
    }
    void Emit(T ch, size_t count)
    {
        m_len += count;  // m_len counts everything, the buffer only takes what fits
        if (m_buf)
        for (; count != 0 && static_cast<size_t>(m_bufPtr - m_buf) < m_maxlen; --count)
            *m_bufPtr++ = ch;
    }

    void Emit(const T* s, size_t count)
    {
        m_len += count;
        if (m_buf)
        for (; count != 0 && static_cast<size_t>(m_bufPtr - m_buf) < m_maxlen; --count)
            *m_bufPtr++ = *s++;
    }

    int Finish()
    {
        if (m_maxlen != 0)
        {
            if (m_len >= m_maxlen)
                m_bufPtr = m_buf + m_maxlen - 1;
            *m_bufPtr = 0;
        }

        return static_cast<int>(m_len);
    }
};
namespace blz {

int snprintf(char* __restrict dest, size_t maxlen, const char* format, ...)
{

    if (format == 0 || (dest == 0 && maxlen != 0))
        return -1;

    SnprintfOutput<char> output(dest, maxlen);

    va_list arglist;
    va_start(arglist, format);
    core_printf(format, arglist, false, output);
    va_end(arglist);
    return output.Finish();

}











































































// LINKER-REMOVED: DWARF format.cpp:2244 (low_pc 0), local output at 2250; body
// inferred as for vsoprintf.
int vsnoprintf(char* __restrict dest, size_t maxlen, const char* format, va_list arglist)
{

    if (format == 0 || (dest == 0 && maxlen != 0))
        return -1;

    SnprintfOutput<char> output(dest, maxlen);
    core_printf(format, arglist, true, output);
    return output.Finish();
}












}  // namespace blz

template <typename T>
class SprintfCountChar {


    size_t m_count;  // 0x00

public:
    SprintfCountChar() : m_count(0) {}

    void Emit(T ch, size_t count)
    {
        (void)ch;
        m_count += count;
    }

    void Emit(const T* s, size_t count)
    {
        (void)s;
        m_count += count;
    }

    int Finish()
    {
        return static_cast<int>(m_count);
    }
};
namespace blz {
// NON_MATCHING: inlines core_printf/__OutputValue, see there.
int vscprintf(const char* format, va_list arglist)
{
    SprintfCountChar<char> output;
    core_printf(format, arglist, false, output);
    return output.Finish();
}
}  // namespace blz









template <typename T>
class PrintfOutput {


    FILE* m_file;  // 0x00
    size_t m_count;  // 0x08

public:
    PrintfOutput(FILE* file) : m_file(file), m_count(0) {}

    void Emit(T ch, size_t count)
    {
        // As in the binary: count has wrapped to SIZE_MAX here, so m_count goes down by one per call.
        while (count--)
            putc(ch, m_file);
        m_count += count;
    }

    void Emit(const T* s, size_t count)
    {

        while (count--)
            putc(*s++, m_file);
        m_count += count;
    }

    int Finish()
    {
        return static_cast<int>(m_count);
    }
};
namespace blz {

int printf(const char* format, ...)
{
    PrintfOutput<char> output(stdout);

    va_list arglist;
    va_start(arglist, format);
    core_printf(format, arglist, false, output);
    va_end(arglist);

    return output.Finish();
}















int vprintf(const char* format, va_list arglist)
{
    PrintfOutput<char> output(stdout);
    core_printf(format, arglist, false, output);
    return output.Finish();
}


































// LINKER-REMOVED: DWARF format.cpp:2413 (low_pc 0), local output at 2415; body
// inferred as for vsoprintf.
int voprintf(const char* format, va_list arglist)
{
    PrintfOutput<char> output(stdout);
    core_printf(format, arglist, true, output);
    return output.Finish();
}
}  // namespace blz






























































































static char get_int_format_char(const blz::int_fmt& fmt)
{
    static const char lower[] = { 'd', 'o', 'x' };
    static const char upper[] = { 'd', 'o', 'X' };


    return (fmt.flags & blz::int_fmt::uppercase) ? upper[fmt.flags & blz::int_fmt::class_mask] : lower[fmt.flags & blz::int_fmt::class_mask];
}

static char get_uint_format_char(const blz::int_fmt& fmt)
{
    static const char lower[] = { 'u', 'o', 'x' };
    static const char upper[] = { 'u', 'o', 'X' };


    return (fmt.flags & blz::int_fmt::uppercase) ? upper[fmt.flags & blz::int_fmt::class_mask] : lower[fmt.flags & blz::int_fmt::class_mask];
}

static char get_float_format_char(const blz::float_fmt& fmt)
{
    static const char lower[] = { 'g', 'f', 'e', 'a' };
    static const char upper[] = { 'G', 'f', 'E', 'A' };


    return (fmt.flags & blz::float_fmt::uppercase) ? upper[fmt.flags & blz::float_fmt::class_mask] : lower[fmt.flags & blz::float_fmt::class_mask];
}

static inline char get_ptr_format_char(const blz::ptr_fmt& fmt)
{
    return (fmt.flags & blz::ptr_fmt::uppercase) ? 'P' : 'p';
}

template <typename T>
static void add_int_spec_flags(PrintfSpec<T>& spec, const blz::int_fmt& fmt)
{
    if (fmt.flags & blz::int_fmt::showpos)
        spec.m_flags |= PRINTF_FLAG_PLUS;
    if (fmt.flags & blz::int_fmt::showbase)
        spec.m_flags |= PRINTF_FLAG_HASH;
}

template <typename T>
static void add_float_spec_flags(PrintfSpec<T>& spec, const blz::float_fmt& fmt)
{
    if (fmt.flags & blz::float_fmt::showpos)
        spec.m_flags |= PRINTF_FLAG_PLUS;
    if (fmt.flags & blz::float_fmt::showpoint)
        spec.m_flags |= PRINTF_FLAG_HASH;
}

template <typename T>
static inline void add_ptr_spec_flags(PrintfSpec<T>& spec, const blz::ptr_fmt& fmt)
{
    if (fmt.flags & blz::ptr_fmt::showbase)
        spec.m_flags |= PRINTF_FLAG_HASH;
}
namespace blz {






















int to_string(char* dst, size_t count, const int_fmt& fmt, int n)
{
    PrintfSpec<char> spec(get_int_format_char(fmt), PA_TYPE_INT);
    add_int_spec_flags(spec, fmt);
    PrintfArg arg;
    arg.paIntMax = n;
    SprintfOutput<char> output(dst, count);
    __OutputValue(&spec, arg, output);
    return output.Finish();
}

int to_string(char* dst, size_t count, const int_fmt& fmt, unsigned int n)
{
    PrintfSpec<char> spec(get_uint_format_char(fmt), PA_TYPE_INT);
    add_int_spec_flags(spec, fmt);
    PrintfArg arg;
    arg.paUIntMax = n;
    SprintfOutput<char> output(dst, count);
    __OutputValue(&spec, arg, output);
    return output.Finish();
}

int to_string(char* dst, size_t count, const int_fmt& fmt, long n)
{
    PrintfSpec<char> spec(get_int_format_char(fmt), PA_TYPE_LONG_INT);
    add_int_spec_flags(spec, fmt);
    PrintfArg arg;
    arg.paIntMax = n;
    SprintfOutput<char> output(dst, count);
    __OutputValue(&spec, arg, output);
    return output.Finish();
}

int to_string(char* dst, size_t count, const int_fmt& fmt, unsigned long n)
{
    PrintfSpec<char> spec(get_uint_format_char(fmt), PA_TYPE_LONG_INT);
    add_int_spec_flags(spec, fmt);
    PrintfArg arg;
    arg.paUIntMax = n;
    SprintfOutput<char> output(dst, count);
    __OutputValue(&spec, arg, output);
    return output.Finish();
}























int to_string(char* dst, size_t count, const float_fmt& fmt, float f)
{
    PrintfSpec<char> spec(get_float_format_char(fmt), PA_TYPE_DOUBLE);
    add_float_spec_flags(spec, fmt);
    PrintfArg arg;
    arg.paDouble = f;
    SprintfOutput<char> output(dst, count);
    __OutputValue(&spec, arg, output);
    return output.Finish();
}


















































































































































}  // namespace blz
