#include "BBE.h"
#include "BBEUtil.h"
#include "../Sigma/RefString.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Math/MathGeneral.h"
#include <ctype.h>
int32 BBEFunctionTranslate(const char * szIdentifier);  // X/Common/Definitions/GameBalanceDefinition.cpp:3771 (no DWARF declaration in this CU)
int32 BBENumOperands(int32 nFunction);  // X/Common/Definitions/GameBalanceDefinition.cpp:3793 (no DWARF declaration in this CU)
BOOL BBEIdentifierTranslate(const BBETranslationData & bbeTranslate, const char * szIdentifier, BOOL * pfConstant, BOOL * pfLong, uint8 * pbResult);  // X/Common/BBEUtil.cpp:794
BOOL BBEIdentifierToString(const int32 nIdentifier, int32 nParam0, int32 nParam1, int32 nParam2, CRefString & sIdentifier);  // X/Common/BBEUtil.cpp:839
float BBEFunctionMin(float a, float b, float c, RNG *, void * pData), BBEFunctionMax(float a, float b, float c, RNG *, void * pData), BBEFunctionPin(float a, float b, float c, RNG *, void * pData), BBEFunctionRandRangeInt(float a, float b, float c, RNG * ptRNG, void * pData);  // GameBalanceDefinition.cpp:3799-3823 (referenced by BBEFunctions only)
float BBEFunctionRandRangeIntMinMax(float a, float b, float c, RNG * ptRNG, void * pData), BBEFunctionFloor(float a, float b, float c, RNG *, void * pData), BBEFunctionDiminishing(float a, float b, float c, RNG *, void * pData), BBEFunctionPower(float a, float b, float c, RNG *, void * pData);  // GameBalanceDefinition.cpp:3888-4063
float BBEFunctionLog(float a, float, float, RNG *, void *), BBEFunctionRandRangeFloat(float a, float b, float c, RNG * ptRNG, void * pData), BBEFunctionRandRangeFloatMinMax(float a, float b, float c, RNG * ptRNG, void * pData), BBEFunctionTable(float a, float b, float c, RNG *, void * pData);  // GameBalanceDefinition.cpp:3943-4097
typedef int32 (*FunctionOperandsFunc)(int32);
#include <string.h>
const TFunctionEvaluateTable BBEFunctions[] =
{
	{ "Min", BBEFunctionMin, 2 },
	{ "Max", BBEFunctionMax, 2 },
	{ "Pin", BBEFunctionPin, 3 },
	{ "RangeInt", BBEFunctionRandRangeInt, 2 },
	{ "RandomInt", BBEFunctionRandRangeIntMinMax, 2 },
	{ "FLOOR", BBEFunctionFloor, 1 },
	{ "Dim", BBEFunctionDiminishing, 3 },
	{ "Pow", BBEFunctionPower, 2 },
	{ "Log", BBEFunctionLog, 1 },
	{ "RangeFloat", BBEFunctionRandRangeFloat, 2 },
	{ "RandomFloat", BBEFunctionRandRangeFloatMinMax, 2 },
	{ "Table", BBEFunctionTable, 2 },
	{ "", NULL, 0 },
};
// value 12 read from 0x7100e582cc: the table size less its terminating entry
const int32 NUM_BBEFUNCTIONS = (sizeof(BBEFunctions) / sizeof(BBEFunctions[0])) - 1;




















enum { BBE_MAX_OPSTACK = 64 };





enum
{
	BBE_END,
	BBE_FUNC,
	BBE_PAREN,
	BBE_COMMA,
	BBE_IDEN_FLOAT,
	BBE_IDEN_LONG,
	BBE_CONST_FLOAT,
	BBE_OP_LT,
	BBE_OP_GT,
	BBE_OP_EQ,
	BBE_OP_NEQ,
	BBE_OP_ADD,
	BBE_OP_SUB,
	BBE_OP_MUL,
	BBE_OP_DIV,
	BBE_OP_POW,
	BBE_OP_NEG,
	BBE_OP_TERNARY,
	BBE_LAST
};


union FourBytes
{
	int32 asInt;
	float asFloat;
};





template <typename T>
struct BBEState
{
	BBEState(T _defaultVal)
		: defaultVal(_defaultVal)
	{
		this->nOperands = 0;  // column 19 of the store is the '=' after "this->nOperands"

	}

	T opStack[BBE_MAX_OPSTACK];
	int32 nOperands;
	T defaultVal;

	void Push(T nOperand)
	{
		if (nOperands >= BBE_MAX_OPSTACK)
		{
			return;
		}


		opStack[nOperands++] = nOperand;
	}

	T Pop()
	{
		if (nOperands <= 0)
		{

			return defaultVal;
		}

		return opStack[--nOperands];
	}
};





BOOL BBEToString(
	uint8 * pbBuf,
	intptr nLen,
	const TFunctionEvaluateTable * ptFuncEvalTable,
	int32 nFuncEvalTable,
	CRefString & sResult)
{
	sResult = "";

	if (nLen <= 0)
	{
		return TRUE;
	}

	union Stream
	{
		uint8 * bytePtr;
		int32 * intPtr;
		float * floatPtr;
	};

	Stream current;
	current.bytePtr = pbBuf;

	BBEState<CRefString> tBBE("");
	tBBE.nOperands = 0;


	while ((current.bytePtr - pbBuf) < nLen)
	{




		uint8 bType = *current.bytePtr;
		current.bytePtr += 4;
		switch (bType)
		{
		case BBE_IDEN_LONG:
			{
				if (((current.bytePtr + 16) - pbBuf) > nLen)
				{
					return FALSE;
				}


				CRefString sIdentifier;
				if (!BBEIdentifierToString(current.intPtr[0], current.intPtr[1], current.intPtr[2], current.intPtr[3], sIdentifier))
				{
					return FALSE;
				}
				tBBE.Push(sIdentifier);
				current.bytePtr += 16;
			}
			break;

		case BBE_IDEN_FLOAT:
			{
				if (((current.bytePtr + 4) - pbBuf) > nLen)
				{
					return FALSE;
				}


				CRefString sIdentifier;
				if (!BBEIdentifierToString(*current.intPtr, 0, 0, 0, sIdentifier))
				{
					return FALSE;
				}
				tBBE.Push(sIdentifier);
				current.bytePtr += 4;
			}
			break;
		case BBE_CONST_FLOAT:
			{
				if (((current.bytePtr + 4) - pbBuf) > nLen)
				{
					return FALSE;
				}


				tBBE.Push(CRefString().Format("%f", *current.floatPtr));
				current.bytePtr += 4;
			}
			break;


		case BBE_FUNC:
			{
				int32 nFunctionIndex = *current.bytePtr;
				current.bytePtr += 4;
				if (nFunctionIndex < nFuncEvalTable)
				{





					sResult += ptFuncEvalTable[nFunctionIndex].szFunctionName;

					switch (ptFuncEvalTable[nFunctionIndex].nOperands)
					{
					case 3: {	CRefString c = tBBE.Pop();
								CRefString b = tBBE.Pop();
								CRefString a = tBBE.Pop();
						tBBE.Push(CRefString().Format("%s(%s, %s, %s)", ptFuncEvalTable[nFunctionIndex].szFunctionName, a.str(), b.str(), c.str()));

					}	break;


					case 2: {	CRefString b = tBBE.Pop();
								CRefString a = tBBE.Pop();
						tBBE.Push(CRefString().Format("%s(%s, %s)", ptFuncEvalTable[nFunctionIndex].szFunctionName, a.str(), b.str()));
					}	break;



					case 1: {	CRefString a = tBBE.Pop();
						tBBE.Push(CRefString().Format("%s(%s)", ptFuncEvalTable[nFunctionIndex].szFunctionName, a.str()));
					}	break;


					case 0:
						tBBE.Push(CRefString().Format("%s()", ptFuncEvalTable[nFunctionIndex].szFunctionName));
						break;

					default:


						tBBE.Push(CRefString(ptFuncEvalTable[nFunctionIndex].szFunctionName));
						break;
					}
				}
			}
			break;
		case BBE_OP_ADD:
			{
				CRefString b = tBBE.Pop();
				CRefString a = tBBE.Pop();
				tBBE.Push(CRefString().Format("(%s + %s)", a.str(), b.str()));
			}
			break;

		case BBE_OP_SUB:
			{
				CRefString b = tBBE.Pop();
				CRefString a = tBBE.Pop();
				tBBE.Push(CRefString().Format("(%s - %s)", a.str(), b.str()));
			}
			break;

		case BBE_OP_MUL:
			{
				CRefString b = tBBE.Pop();
				CRefString a = tBBE.Pop();
				tBBE.Push(CRefString().Format("(%s * %s)", a.str(), b.str()));
			}
			break;

		case BBE_OP_DIV:
			{
				CRefString b = tBBE.Pop();
				CRefString a = tBBE.Pop();
				tBBE.Push(CRefString().Format("(%s / %s)", a.str(), b.str()));
			}
			break;

		case BBE_OP_NEG:
			{
				CRefString a = tBBE.Pop();
				tBBE.Push(CRefString().Format("-(%s)", a.str()));
			}
			break;

		case BBE_OP_TERNARY:
			{
				CRefString num2 = tBBE.Pop();
				CRefString num1 = tBBE.Pop();
				CRefString test = tBBE.Pop();
				tBBE.Push(CRefString().Format("(%s ? %s : %s)", test.str(), num1.str(), num2.str()));
			}
			break;

		case BBE_OP_POW:
			{
				DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
			}
			break;




		case BBE_OP_LT:
			{
				CRefString b = tBBE.Pop();
				CRefString a = tBBE.Pop();
				tBBE.Push(CRefString().Format("(%s < %s)", a.str(), b.str()));
			}
			break;

		case BBE_OP_GT:
			{
				CRefString b = tBBE.Pop();
				CRefString a = tBBE.Pop();
				tBBE.Push(CRefString().Format("(%s > %s)", a.str(), b.str()));
			}
			break;

		case BBE_OP_EQ:
			{
				CRefString b = tBBE.Pop();
				CRefString a = tBBE.Pop();
				tBBE.Push(CRefString().Format("(%s == %s)", a.str(), b.str()));
			}
			break;

		case BBE_OP_NEQ:
			{
				CRefString b = tBBE.Pop();
				CRefString a = tBBE.Pop();
				tBBE.Push(CRefString().Format("(%s != %s)", a.str(), b.str()));
			}
			break;

		case BBE_END:
			{
				if (!tBBE.nOperands)
				{
					return FALSE;
				}

				sResult = tBBE.Pop();
				return TRUE;
			}

		default:
			return FALSE;
		}
	}

	return FALSE;



}





BOOL BBEEvaluate(
	uint8 * pbBuf,
	intptr nLen,
	IdentifierEvaluateFunc pfIdEvalFunc,
	const TFunctionEvaluateTable * ptFuncEvalTable,
	int32 nFuncEvalTable,
	RNG * ptRNG,
	void * callbackData,
	float & flResult)
{
	flResult = 0.0f;

	if (nLen <= 0)
	{
		return TRUE;
	}

	union Stream
	{
		uint8 * bytePtr;
		int32 * intPtr;
		float * floatPtr;
	};



	Stream current;
	current.bytePtr = pbBuf;

	BBEState<FourBytes> tBBE((FourBytes){ .asInt = 0 });
	tBBE.nOperands = 0;


	while ((current.bytePtr - pbBuf) < nLen)
	{




		uint8 bType = *current.bytePtr;
		current.bytePtr += 4;
		switch (bType)
		{
		case BBE_IDEN_LONG:
			{
				if (!pfIdEvalFunc)
				{
					return FALSE;
				}


				if (((current.bytePtr + 16) - pbBuf) > nLen)
				{
					return FALSE;
				}


				float identityLong;
				BOOL bSuccess = pfIdEvalFunc(current.intPtr[0], current.intPtr[1], current.intPtr[2], current.intPtr[3], callbackData, identityLong);
				if (!bSuccess)
				{
					return FALSE;
				}
				tBBE.Push((FourBytes){ .asFloat = identityLong });
				current.bytePtr += 16;
			}
			break;

		case BBE_IDEN_FLOAT:
			{


				if (!pfIdEvalFunc)
				{
					return FALSE;
				}


				if (((current.bytePtr + 4) - pbBuf) > nLen)
				{
					return FALSE;
				}


				float identityFloat;
				BOOL bSuccess = pfIdEvalFunc(*current.intPtr, 0, 0, 0, callbackData, identityFloat);
				if (!bSuccess)
				{
					return FALSE;
				}
				tBBE.Push((FourBytes){ .asFloat = identityFloat });
				current.bytePtr += 4;
			}
			break;

		case BBE_CONST_FLOAT:
			{

				if (((current.bytePtr + 4) - pbBuf) > nLen)
				{
					return FALSE;
				}


				tBBE.Push((FourBytes){ .asFloat = *current.floatPtr });
				current.bytePtr += 4;
			}
			break;

		case BBE_FUNC:
			{

				int32 nFunctionIndex = *current.bytePtr;
				current.bytePtr += 4;
				if (nFunctionIndex >= nFuncEvalTable)
				{

					tBBE.Push((FourBytes){ .asFloat = 0.0f });
					break;
				}

				switch (ptFuncEvalTable[nFunctionIndex].nOperands)
				{
				case 3: {	float c = tBBE.Pop().asFloat;
							float b = tBBE.Pop().asFloat;
							float a = tBBE.Pop().asFloat;
					tBBE.Push((FourBytes){ .asFloat = ptFuncEvalTable[nFunctionIndex].pfnEvalFunc(a, b, c, ptRNG, callbackData) });
				}	break;



				case 2: {	float b = tBBE.Pop().asFloat;
							float a = tBBE.Pop().asFloat;
					tBBE.Push((FourBytes){ .asFloat = ptFuncEvalTable[nFunctionIndex].pfnEvalFunc(a, b, 0.0f, ptRNG, callbackData) });
				}	break;



				case 1: {	float a = tBBE.Pop().asFloat;
					tBBE.Push((FourBytes){ .asFloat = ptFuncEvalTable[nFunctionIndex].pfnEvalFunc(a, 0.0f, 0.0f, ptRNG, callbackData) });
				}	break;


				case 0:
					tBBE.Push((FourBytes){ .asFloat = ptFuncEvalTable[nFunctionIndex].pfnEvalFunc(0.0f, 0.0f, 0.0f, ptRNG, callbackData) });
					break;

				default:
					tBBE.Push((FourBytes){ .asFloat = 0.0f });
					break;
				}
			}
			break;



		case BBE_OP_ADD:
			{
				float b = tBBE.Pop().asFloat;
				float a = tBBE.Pop().asFloat;

				tBBE.Push((FourBytes){ .asFloat = a + b });
			}
			break;

		case BBE_OP_SUB:
			{
				float b = tBBE.Pop().asFloat;
				float a = tBBE.Pop().asFloat;

				tBBE.Push((FourBytes){ .asFloat = a - b });
			}
			break;

		case BBE_OP_MUL:
			{
				float b = tBBE.Pop().asFloat;
				float a = tBBE.Pop().asFloat;

				tBBE.Push((FourBytes){ .asFloat = a * b });
			}
			break;

		case BBE_OP_DIV:
			{
				float b = tBBE.Pop().asFloat;
				float a = tBBE.Pop().asFloat;

				if (b != 0.0f)
				{
					tBBE.Push((FourBytes){ .asFloat = a / b });
				}
				else
				{

					tBBE.Push((FourBytes){ .asFloat = 0.0f });
				}
			}
			break;

		case BBE_OP_NEG:
			{
				float a = tBBE.Pop().asFloat;

				tBBE.Push((FourBytes){ .asFloat = -a });
			}
			break;

		case BBE_OP_TERNARY:
			{
				float num2 = tBBE.Pop().asFloat;
				float num1 = tBBE.Pop().asFloat;
				float test = tBBE.Pop().asFloat;

				tBBE.Push((FourBytes){ .asFloat = test > EPSILON ? num1 : num2 });
			}
			break;

		case BBE_OP_POW:
			{
				DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
			}
			break;






		case BBE_OP_LT:
			{
				float b = tBBE.Pop().asFloat;
				float a = tBBE.Pop().asFloat;

				tBBE.Push((FourBytes){ .asFloat = (float)(a < b) });
			}
			break;

		case BBE_OP_GT:
			{
				float b = tBBE.Pop().asFloat;
				float a = tBBE.Pop().asFloat;

				tBBE.Push((FourBytes){ .asFloat = (float)(a > b) });
			}
			break;

		case BBE_OP_EQ:
			{
				float b = tBBE.Pop().asFloat;
				float a = tBBE.Pop().asFloat;

				tBBE.Push((FourBytes){ .asFloat = (float)(Math::Abs(a - b) < EPSILON) });
			}
			break;

		case BBE_OP_NEQ:
			{
				float b = tBBE.Pop().asFloat;
				float a = tBBE.Pop().asFloat;

				tBBE.Push((FourBytes){ .asFloat = (float)(Math::Abs(a - b) >= EPSILON) });
			}
			break;

		case BBE_END:
			{
				if (!tBBE.nOperands)
				{
					return FALSE;
				}


				flResult = tBBE.Pop().asFloat;
				return TRUE;
			}

		default:
			return FALSE;
		}
	}

	return FALSE;




}


void BBEByteswap(
	uint8 * pbBuf,
	int32 nLen)
{
	if (nLen <= 0)
		return;

	union Stream
	{
		uint8 * bytePtr;
		int32 * intPtr;
		float * floatPtr;
	};

	Stream current;
	current.bytePtr = pbBuf;



	while ((current.bytePtr - pbBuf) < nLen)
	{
		// this build is little-endian: the original's per-token byte swaps compile to nothing
		// (SigmaByteswap.h, which src lacks, contributes only the Endian enum to the DWARF)


		uint8 bType = *current.bytePtr;
		current.intPtr++;
		switch (bType)
		{
		case BBE_END:
			return;
		case BBE_FUNC: case BBE_IDEN_FLOAT: case BBE_CONST_FLOAT: current.intPtr++;
			break;
		case BBE_IDEN_LONG:						current.intPtr += 4;
			break;
		}
	}










































}



// BBEFromString tokenizer limits and token kinds (DWARF: anonymous enums at BBE.cpp:760 and :765)

enum { MAX_IDENTIFIER_SIZE = 256 };




enum
{
	TOK_PAREN_LEFT,
	TOK_PAREN_RIGHT,
	TOK_COMMA,
	TOK_OP_ADD,
	TOK_OP_SUB,
	TOK_OP_MUL,
	TOK_OP_DIV,
	TOK_OP_QUESTION,
	TOK_OP_COLON,
	TOK_OP_POWER,
	TOK_OP_LT,
	TOK_OP_GT,
	TOK_OP_EQ,
	TOK_OP_NEQ,
	TOK_CONSTANT,
	TOK_IDENTIFIER,
	TOK_LONG_IDENTIFIER,
	TOK_FUNCTION,
};





struct TOpStack
{
	uint8 bOperator[BBE_MAX_OPSTACK];
	int32 nOperatorEx[BBE_MAX_OPSTACK];
	int32 nCount;
	int32 nOperands;
};

// LINKER-REMOVED: DWARF size 0x70; address-0 rows have no row at 827/831 and a 3-instruction select at 825:6 (cinc), so fLong picks the token in an if/else, not two returns.
void sTranslateIdentifier(
	const BBETranslationData & bbeTranslate,
	const char * szIden,
	uint8 * pbToken,
	FourBytes * value)
{
	BOOL fConstant;
	BOOL fLong;

	if (!BBEIdentifierTranslate(bbeTranslate, szIden, &fConstant, &fLong, (uint8 *)value))
	{
		// unknown identifier: evaluates as the constant 0
		// (no row of its own)
		*pbToken = TOK_CONSTANT;
		value->asFloat = 0.0f;
		return;
	}

	if (fConstant)
	{
		*pbToken = TOK_CONSTANT;
		return;
	}


	if (fLong)
	{
		*pbToken = TOK_LONG_IDENTIFIER;
	}
	else
	{
		*pbToken = TOK_IDENTIFIER;
	}

}

// sGetToken is indented with spaces in the original: columns at 850, 856, 889, 957 and 996 only fit 4-space indents


static const char * sGetToken(
    const char * txt,
    uint8 * pbToken,
    FourBytes * value,
    const BBETranslationData & bbeTranslate)
{
    char szIden[MAX_IDENTIFIER_SIZE];

    value[0].asInt = 0;

    // skip whitespace and quotes
    while (*txt && (isspace(*txt) || *txt == '"'))
        txt++;

    // a leading '.' is a number only when no identifier character follows it
    // (columns 22/30 fit isdigit(*txt))

    BOOL bIsNumber = isdigit(*txt);
    if (*txt == '.')
    {
        bIsNumber = TRUE;

        const char * szNumberCheck = txt + 1;
        while (*szNumberCheck)
        {
            if (isalpha(*szNumberCheck) || *szNumberCheck == '.' || *szNumberCheck == '_')
            {
                bIsNumber = FALSE;
                break;
            }


            if (!isdigit(*szNumberCheck))
                break;

            szNumberCheck++;
        }
    }




    if (bIsNumber)
    {
        *pbToken = TOK_CONSTANT;

        BOOL bBeforeDecimal = TRUE;
        float flValue = 0.0f;
        float flScalar = 10.0f;

        while (isdigit(*txt) || *txt == '.')
        {
            if (*txt == '.')
            {
                bBeforeDecimal = FALSE;
            }
            else
            {







                float flDigit = (float)(*txt - '0');
                if (bBeforeDecimal)
                {
                    flValue = flValue * 10.0f + flDigit;
                }
                else
                {
                    flValue = flValue + (flDigit / flScalar);
                    flScalar *= 10.0f;
                }
            }
            txt++;
        }
        value[0].asFloat = flValue;
        return txt;
    }
    else if (*txt == '\'')
    {
        // quoted identifier

        if (*(++txt) == 0)
            return NULL;

        // (no row of its own)
        // INFERRED (934-941): copy and skip each step txt; the original hoists the shared txt + 1 above the length test (line 0)

        int32 nIdenLen = 0;
        szIden[nIdenLen++] = *txt++;
        while (*txt && *txt != '\'')
        {
            if (nIdenLen < MAX_IDENTIFIER_SIZE - 1)
            {
                szIden[nIdenLen++] = *txt++;
            }
            else
            {
                txt++;
            }
        }
        if (*txt)
            txt++;

        // column 28 fits "szIden[nIdenLen++] = 0"
        szIden[nIdenLen++] = 0;

        sTranslateIdentifier(bbeTranslate, szIden, pbToken, value);
        return txt;
    }
    else if (isalpha(*txt) || *txt == '_' || *txt == '.')
    {
        int32 nIdenLen = 0;
        szIden[nIdenLen++] = *txt++;
        BOOL bInQuotes = FALSE;
        while (isalnum(*txt) || *txt == '#' || *txt == '.' || *txt == '_' || isspace(*txt) || *txt == '"')
        {
            if (!bInQuotes)
            {
                // whitespace ends the identifier outside quotes
                if (isspace(*txt))
                    break;

                // INFERRED: the toggle is set per branch (the original merges the constants 1 / 0 into one csel, line 0)
                if (*txt == '"')
                    bInQuotes = TRUE;
            }
            else if (*txt == '"')
            {
                bInQuotes = FALSE;
            }





            // column 52 is the folded "&&"
            if (nIdenLen < MAX_IDENTIFIER_SIZE - 1 && *txt != '"')
                // (no row of its own)
                szIden[nIdenLen++] = *txt;
            txt++;
        }

        // column 28 fits "szIden[nIdenLen++] = 0"


        szIden[nIdenLen++] = 0;

        while (*txt && isspace(*txt))
            txt++;

        // an identifier followed by '(' is a function call
        if (*txt == '(')
        {
            int32 func = BBEFunctionTranslate(szIden);
            if (func >= 0)
            {
                *pbToken = TOK_FUNCTION;

                value[0].asInt = func;
                return txt + 1;
            }
        }

        sTranslateIdentifier(bbeTranslate, szIden, pbToken, value);
        return txt;
    }

    // operators
    switch (*txt)
    {
        // (no rows of their own)
    case '(': *pbToken = TOK_PAREN_LEFT; return txt + 1;
    case ')': *pbToken = TOK_PAREN_RIGHT; return txt + 1;
    case '+': *pbToken = TOK_OP_ADD; return txt + 1;
    case '-': *pbToken = TOK_OP_SUB; return txt + 1;
    case '*': *pbToken = TOK_OP_MUL; return txt + 1;
    case '/': *pbToken = TOK_OP_DIV; return txt + 1;
    case '?': *pbToken = TOK_OP_QUESTION; return txt + 1;
    case ':': *pbToken = TOK_OP_COLON; return txt + 1;
    case '^': *pbToken = TOK_OP_POWER; return txt + 1;
    case ',': *pbToken = TOK_COMMA; return txt + 1;

    case '<':
        // "<=" is not supported
        if (*(++txt) == '=')
            return NULL;
        *pbToken = TOK_OP_LT;
        return txt;

    case '>':
        // ">=" is not supported




        if (*(++txt) == '=')
            return NULL;
        *pbToken = TOK_OP_GT;
        return txt;

    case '!':





        if (txt[1] == '=')
        {
            *pbToken = TOK_OP_NEQ;
            return txt + 2;
        }
        return NULL;

    case '=':




        if (txt[1] == '=')
        {
            *pbToken = TOK_OP_EQ;
            return txt + 2;
        }
        return NULL;
    }

    return NULL;
}

// operator precedence: bOp1 (on the stack) is emitted before bOp2 is pushed









static BOOL sOpCompare(
	uint8 bOp1,
	uint8 bOp2)
{
	static const uint8 bOpCompareTbl[BBE_LAST] = {
		18,	// BBE_END
		1,	// BBE_FUNC
		2,	// BBE_PAREN
		3,	// BBE_COMMA
		4,	// BBE_IDEN_FLOAT
		5,	// BBE_IDEN_LONG
		6,	// BBE_CONST_FLOAT
		10,	// BBE_OP_LT
		10,	// BBE_OP_GT
		10,	// BBE_OP_EQ
		10,	// BBE_OP_NEQ
		12,	// BBE_OP_ADD
		12,	// BBE_OP_SUB
		14,	// BBE_OP_MUL
		14,	// BBE_OP_DIV
		15,	// BBE_OP_POW
		16,	// BBE_OP_NEG
		17,	// BBE_OP_TERNARY
	};

	// functions are never popped and never pop anything (the FUNC push has no pop loop)
	if (bOp1 >= BBE_LAST || bOp1 == BBE_FUNC || bOp2 == BBE_FUNC)
	{
		return FALSE;
	}

	// bOp2 is compared directly, not through the table: the merged SUB/NEG push compares tbl[op]
	// with bOp itself (cmp w11, w8, uxtb at +0x788) and pushes the same register


	return (bOpCompareTbl[bOp1] >= bOp2);
}





static int32 sGetNumOperands(
	FunctionOperandsFunc pfFuncOperandsFunc,
	uint8 bOp,
	int32 nOpEx)
{
	switch (bOp)
	{
	// INFERRED: END, PAREN, COMMA and the operands fall to the final return 0 and TERNARY comes first:
	// the jump table at the ')' site hoists 3 (orr w0, #3 before br) and keeps 0 in a block of its own






	case BBE_OP_TERNARY:
		return 3;

	case BBE_OP_NEG:
		return 1;

	// INFERRED: without a callback a function counts as binary (the pop loops' lookup table has FUNC = 2, no call)
	case BBE_FUNC:
		if (pfFuncOperandsFunc)
			return pfFuncOperandsFunc(nOpEx);
		// fall through
	case BBE_OP_LT: case BBE_OP_GT: case BBE_OP_EQ: case BBE_OP_NEQ:
	case BBE_OP_ADD: case BBE_OP_SUB: case BBE_OP_MUL: case BBE_OP_DIV: case BBE_OP_POW:
		return 2;
	}

	return 0;
}


static uint8 * sPushOps(
	uint8 * pCur,
	uint8 * pBBE,
	int32 nMaxLen,
	TOpStack & tOp,
	uint8 bOp,
	FunctionOperandsFunc pfFuncOperandsFunc,
	int32 nOpEx)
{
	while (tOp.nCount > 0)
	{
		uint8 op = tOp.bOperator[tOp.nCount - 1];
		if (!sOpCompare(op, bOp))
			break;

		// INFERRED: no callback here (the pop loops' lookup table has FUNC = 2 and no call); nOpEx is this
		// function's own parameter (the inlined sGetNumOperands' nOpEx is DW_AT_const_value 0 at the operator pushes)
		int32 nop = sGetNumOperands(NULL, op, nOpEx);
		if (tOp.nOperands < nop)
		{
			pBBE[0] = BBE_END;
			return NULL;
		}


		if (pCur - pBBE >= nMaxLen)
		{
			pBBE[0] = BBE_END;
			return NULL;
		}


		*pCur = op;
		pCur += 4;






		tOp.nCount--;
		tOp.nOperands -= nop;
		tOp.nOperands++;
	}
	// INFERRED: COMMA and ':' (BBE_LAST) only pop; the final flush (BBE_END) writes the end marker
	if (bOp != BBE_END && bOp != BBE_COMMA && bOp != BBE_LAST)
	{
		if (tOp.nCount >= BBE_MAX_OPSTACK)
		{
			pBBE[0] = BBE_END;
			return NULL;
		}


		tOp.bOperator[tOp.nCount] = bOp;
		tOp.nOperatorEx[tOp.nCount] = nOpEx;
		tOp.nCount++;
	}
	else if (bOp == BBE_END)
	{
		// nothing to evaluate
		if (pCur == pBBE)
		{
			return NULL;
		}
		if (pCur - pBBE >= nMaxLen)
		{
			pBBE[0] = BBE_END;
			return NULL;
		}

		*pCur = BBE_END;
		pCur += 4;
	}

	return pCur;
}





static uint8 * sWriteVal(
	uint8 * pCur,
	uint8 * pBBE,
	int32 nMaxLen,
	TOpStack & tOp,
	FourBytes val)
{
	if (((pCur + 4) - pBBE) >= nMaxLen)
	{
		// buffer full
		pBBE[0] = BBE_END;
		return NULL;
	}

	*pCur = BBE_CONST_FLOAT;

	*(int32 *)(pCur + 4) = val.asInt;	pCur += 8;
	tOp.nOperands++;

	return pCur;
}




static uint8 * sWriteLong(
	uint8 * pCur,
	uint8 * pBBE,
	int32 nMaxLen,
	TOpStack & tOp,
	FourBytes * val)
{
	if (((pCur + 17) - pBBE) >= nMaxLen)
	{
		// buffer full
		pBBE[0] = BBE_END;
		return NULL;
	}

	*pCur = BBE_IDEN_LONG;

	*(int32 *)(pCur + 4) = val[0].asInt;
	*(int32 *)(pCur + 8) = val[1].asInt;
	*(int32 *)(pCur + 12) = val[2].asInt;
	*(int32 *)(pCur + 16) = val[3].asInt;	pCur += 20;
	tOp.nOperands++;

	return pCur;
}


static uint8 * sWriteIden(
	uint8 * pCur,
	uint8 * pBBE,
	int32 nMaxLen,
	TOpStack & tOp,
	FourBytes val)
{
	if (((pCur + 5) - pBBE) >= nMaxLen)
	{
		// buffer full
		pBBE[0] = BBE_END;
		return NULL;
	}

	*pCur = BBE_IDEN_FLOAT;

	*(int32 *)(pCur + 4) = val.asInt;	pCur += 8;
	tOp.nOperands++;

	return pCur;
}



// NON_MATCHING: 4440 vs 4440 bytes; calls, constants, layout and every helper match; 40 instructions differ in the
// ten operator pop loops (sPushOps 1199-1201): the original loads nOperands right after nCount (registers w9/w10/w11),
// ours after the subs (w9/w10/w9), a register-allocation / post-RA scheduling difference. Tried: -= / = - / += 1 spellings.
int32 BBEFromString(
	const char * txt,
	uint8 * pBBE,
	int32 nMaxLen,
	const BBETranslationData & bbeTranslate)
{
	FunctionOperandsFunc pfFuncOperandsFunc = BBENumOperands;

	const char * txtStart = txt;


	TOpStack tOp;
	tOp.nCount = 0;
	tOp.nOperands = 0;

	pBBE = (uint8 *)Math::Align((uintptr)pBBE, 4);

	uint8 * pCur = pBBE;
	BOOL fPrevOperand = FALSE;
	BOOL fVariable = FALSE;

	while (TRUE)	// INFERRED: exits by break / return only; the pCur test at 1498 (column 7 = two tabs) is inside the loop
	{
		uint8 token = 0;
		FourBytes val[4];


		txt = sGetToken(txt, &token, val, bbeTranslate);
		if (!txt)
			break;


		switch (token)
		{
		case TOK_PAREN_LEFT:
			if (tOp.nCount >= BBE_MAX_OPSTACK)
			{
				pBBE[0] = BBE_END;
				return 0;
			}

			tOp.bOperator[tOp.nCount++] = BBE_PAREN;
			break;
		case TOK_PAREN_RIGHT:
			{
				while (tOp.nCount > 0)
				{
					uint8 op = tOp.bOperator[tOp.nCount-1];
					if (op == BBE_PAREN)
						break;



					int32 opex = tOp.nOperatorEx[tOp.nCount-1];
					int32 nop = sGetNumOperands(pfFuncOperandsFunc, op, opex);
					if (tOp.nOperands < nop)
					{
						pBBE[0] = BBE_END;
						return 0;
					}


					if (pCur - pBBE >= nMaxLen)
					{
						pBBE[0] = BBE_END;
						return 0;
					}


					tOp.nOperands -= nop;
					tOp.nOperands++;
					*pCur = op;
					pCur += 4;
					if (op == BBE_FUNC)
					{
						if (pCur - pBBE >= nMaxLen)
						{
							pBBE[0] = BBE_END;
							return 0;
						}

						*pCur = opex;
						pCur += 4;
						break;
					}
					tOp.nCount--;
				}

				if (tOp.nCount <= 0 ||
					(tOp.bOperator[tOp.nCount-1] != BBE_PAREN && tOp.bOperator[tOp.nCount-1] != BBE_FUNC))
				{
					pBBE[0] = BBE_END;
					return 0;
				}


				tOp.nCount--;
			}
			break;

		case TOK_CONSTANT:
			// columns 48/49 fit "*val" (one character more than "val[0]")
			pCur = sWriteVal(pCur, pBBE, nMaxLen, tOp, *val);
			fPrevOperand = TRUE;
			break;

		case TOK_LONG_IDENTIFIER:

			pCur = sWriteLong(pCur, pBBE, nMaxLen, tOp, val);
			fVariable = TRUE;
			fPrevOperand = TRUE;
			break;

		case TOK_IDENTIFIER:

			pCur = sWriteIden(pCur, pBBE, nMaxLen, tOp, *val);
			fVariable = TRUE;
			fPrevOperand = TRUE;
			break;

		case TOK_FUNCTION:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_FUNC, pfFuncOperandsFunc, val[0].asInt);
			fVariable = TRUE;
			fPrevOperand = TRUE;
			break;
		case TOK_OP_ADD:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_OP_ADD, pfFuncOperandsFunc, 0);
			fPrevOperand = FALSE;
			break;
		case TOK_OP_SUB:
			if (fPrevOperand)
			{
				pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_OP_SUB, pfFuncOperandsFunc, 0);
			}
			else
			{
				pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_OP_NEG, pfFuncOperandsFunc, 0);
			}
			fPrevOperand = FALSE;
			break;
		case TOK_OP_MUL:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_OP_MUL, pfFuncOperandsFunc, 0);
			fPrevOperand = FALSE;
			break;
		case TOK_OP_DIV:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_OP_DIV, pfFuncOperandsFunc, 0);
			fPrevOperand = FALSE;
			break;
		case TOK_OP_POWER:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_OP_POW, pfFuncOperandsFunc, 0);
			fPrevOperand = FALSE;
			break;
		case TOK_OP_LT:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_OP_LT, pfFuncOperandsFunc, 0);
			fPrevOperand = FALSE;
			break;
		case TOK_OP_GT:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_OP_GT, pfFuncOperandsFunc, 0);
			fPrevOperand = FALSE;
			break;
		case TOK_OP_EQ:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_OP_EQ, pfFuncOperandsFunc, 0);
			fPrevOperand = FALSE;
			break;
		case TOK_OP_NEQ:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_OP_NEQ, pfFuncOperandsFunc, 0);
			fPrevOperand = FALSE;
			break;
		case TOK_OP_QUESTION:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_OP_TERNARY, pfFuncOperandsFunc, 0);
			break;	// INFERRED: fPrevOperand is not cleared here (the original's '?' path has no store to it)
		case TOK_OP_COLON:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_LAST, pfFuncOperandsFunc, 0);	// INFERRED: 18 pops only BBE_END entries (tbl 18), pushes nothing
			break;
		case TOK_COMMA:
			pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_COMMA, pfFuncOperandsFunc, 0);
			break;
		}

		if (!pCur)
		{
			pBBE[0] = BBE_END;
			return 0;
		}
	}
	// the final flush (BBE_END) pops every operator and writes the end marker
	pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_END, pfFuncOperandsFunc, 0);
	if (!pCur)
	{
		pBBE[0] = BBE_END;
		return 0;
	}

	// no identifiers or functions: fold the expression to one constant
	// (no row of its own)

	if (!fVariable)
	{
		// bSuccess has no location in the original (unused)
		float result;
		BOOL bSuccess = BBEEvaluate(pBBE, pCur - pBBE, NULL, NULL, 0, NULL, NULL, result);

		// rewrite the buffer as the single constant
		pCur = pBBE;

		pCur = sWriteVal(pCur, pBBE, nMaxLen, tOp, (FourBytes){ .asFloat = result });
		pCur = sPushOps(pCur, pBBE, nMaxLen, tOp, BBE_END, pfFuncOperandsFunc, 0);
	}

	// column 29 fits "(int32)(pCur - pBBE)" only with a 1-char shorter cast; kept plain

	int32 nOffset = (int32)(pCur - pBBE);
	return nOffset;
}








