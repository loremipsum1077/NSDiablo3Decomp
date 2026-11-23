// X/Console/BuildLocker.cpp
// Build fingerprint ("build locker"): reads the signature stamped into the data files, checks the
// binary and text copies agree, and draws the signature as an on-screen watermark.

#include "../../BlizzardCore/Include/Blizzard/File.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Client/CGame.h"
#include "../Client/Font/Font.h"
#include "../Client/Font/FormattedText.h"
#include "../Common/GlobalSNO.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/RGBAColor.h"
#include "../Sigma/StringManip.h"
#include "../Sigma/buildver.h"
#include <stdlib.h>
#include <string.h>

struct Signature {
    char szName[64];
    char szComment[64];
    uint8 nMonth;
    uint8 nDay;
    uint8 nYear;
};
static_assert(sizeof(Signature) == 0x83, "Signature");

static Signature s_tSignature;
static BOOL s_bWatermarkEnabled;  // DWARF variable DIE 0x7e39: type BOOL (typedef int32, SigmaTypes.h:16); original accesses it as a byte (ldrb/strb [x8,#0x470]) but the DWARF type is kept









extern const RGBAColor crgbaTextShadow;  // RGBAColor.cpp (no header declares it)

// NON_MATCHING: 988 vs 980 B. Drop-shadow colour starts from crgbaBlack (original GOT slot +0x120, line 59). With the late-GlobalOpt build the two statics are now separate symbols as in the original (s_bWatermarkEnabled shrunk to one byte, ldrb [x8,#0x470]). What is left: the Vector2D stores of the later draws are str/str in ours where the original pairs them (stp), plus scheduling around the colour strh (tried member-wise assignment, no change).
void BuildLockerDrawWatermark()
{
    if (s_bWatermarkEnabled == TRUE)
    {
        CRefString sMsg;
        sMsg.Format("%s\n%s\n%02d-%02d-%02d", s_tSignature.szName, s_tSignature.szComment, s_tSignature.nMonth, s_tSignature.nDay, s_tSignature.nYear);
        Rect2D rectViewportUIC;
        CGameGetSafeViewportUIC(rectViewportUIC);
        TextRenderProperties tProps(GlobalSNOGet(637), 21, 1.0f, 1.0f);
        FormattedText tFormattedText(sMsg, 5);
        Vector2D vecSizeUIC;
        FontStringGetRenderedSizeUIC(tProps, tFormattedText, vecSizeUIC);
        RGBAColor tColor = crgbaWhite;
        RGBAColor tDropShadowColor = crgbaBlack;
        tDropShadowColor.a = 40;
        tColor.r = 192;
        tColor.a = 80;
        Vector2D vecUIC = Vector2D((rectViewportUIC.right - vecSizeUIC.x) - 15.0f, (rectViewportUIC.bottom - vecSizeUIC.y) - 15.0f);
        FontStringDrawUIC(tProps, tFormattedText, vecUIC, tColor, &tDropShadowColor, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
        vecUIC = Vector2D(rectViewportUIC.left - 15.0f, rectViewportUIC.top + 15.0f);
        tColor.r = 255;
        tColor.g = 192;
        FontStringDrawUIC(tProps, tFormattedText, vecUIC, tColor, &tDropShadowColor, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
        vecUIC = Vector2D(rectViewportUIC.left - 15.0f, (rectViewportUIC.bottom - vecSizeUIC.y) - 15.0f);
        tColor.g = 255;
        tColor.b = 192;
        FontStringDrawUIC(tProps, tFormattedText, vecUIC, tColor, &tDropShadowColor, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
        vecUIC = Vector2D((rectViewportUIC.right - vecSizeUIC.x) - 15.0f, rectViewportUIC.top + 15.0f);
        tColor.r = 192;
        FontStringDrawUIC(tProps, tFormattedText, vecUIC, tColor, &tDropShadowColor, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
        vecUIC = Vector2D((rectViewportUIC.right + rectViewportUIC.left) * 0.5f - vecSizeUIC.x * 0.5f, (rectViewportUIC.bottom + rectViewportUIC.top) * 0.5f - vecSizeUIC.y * 0.5f);
        tColor.r = 255;
        tColor.g = 255;
        tColor.b = 255;
        tColor.a = 48;
        tDropShadowColor.a = 24;
        FontStringDrawUIC(tProps, tFormattedText, vecUIC, tColor, &tDropShadowColor, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
        sMsg = "This build has a unique fingerprint - do not distribute.";
        tFormattedText = FormattedText(sMsg, 5);
        tProps = TextRenderProperties(GlobalSNOGet(637), 13, 1.0f, 1.0f);
        FontStringGetRenderedSizeUIC(tProps, tFormattedText, vecSizeUIC);
        vecUIC = Vector2D((rectViewportUIC.right + rectViewportUIC.left) * 0.5f - vecSizeUIC.x * 0.5f, (rectViewportUIC.bottom - vecSizeUIC.y) - 15.0f);
        FontStringDrawUIC(tProps, tFormattedText, vecUIC, crgbaWhite, &crgbaTextShadow, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
    }
}

















uint32 sGenerateKey();
BOOL sReadBinarySignature(Signature & tSignature, uint32 nKey);
BOOL sReadTextSignature(Signature & tSignature);

void BuildLockerValidate()
{
    SigmaMemoryFill(&s_tSignature, 0, sizeof(Signature));
    Signature tTextSignature;
    SigmaMemoryFill(&tTextSignature, 0, sizeof(Signature));
    uint32 nKey = sGenerateKey();
    BOOL bBinary = sReadBinarySignature(s_tSignature, nKey);
    BOOL bText = sReadTextSignature(tTextSignature);
    if (bBinary || bText)
    {







        if (bBinary != bText) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }

        if (memcmp(&s_tSignature, &tTextSignature, sizeof(Signature)) != 0) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
        s_bWatermarkEnabled = TRUE;
    }
}


uint32 sGenerateKey()
{
    int32 buildNum = D3BuildNum();  // INFERENCE: signed (csel-based % 0x10000 at 0x1430-0x1444); decompiler names it uint32
    return (buildNum % 0x10000) * 0x10001;
}


BOOL sReadBinarySignature(Signature & tSignature, uint32 nKey)
{
    Blizzard::File::Stream tStream;
    char szPath[256];
    uint8 tBuffer[2048];
    int32 nAmount;  // INFERENCE: declaration order tStream,szPath,tBuffer,nAmount,keyBytes from the stack offsets
    uint8 keyBytes[4];
    Blizzard::String::Format(szPath, sizeof(szPath), "%s\\CPKs\\PVPCommon.cpk", SNOFilesGetConsoleDataPathOverride());
    if (Blizzard::File::Exists(szPath) != Blizzard::File::missing)
    {
        if (Blizzard::File::Open(szPath, 1, tStream))
        {
            nAmount = sizeof(tBuffer);
            bool bRes = Blizzard::File::Read(tStream, tBuffer, 0xe5fc0, &nAmount, 0);
            Blizzard::File::Close(tStream);











            if (!bRes) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            keyBytes[0] = nKey;
            keyBytes[1] = nKey >> 8;
            keyBytes[2] = nKey >> 16;
            keyBytes[3] = nKey >> 24;
            for (int i = 0; i < nAmount; i++)
            {
                tBuffer[i] = tBuffer[i] ^ keyBytes[i % 4];
            }
            Blizzard::String::Copy(tSignature.szName, (const char *)tBuffer, sizeof(tSignature.szName));
            int32 nNameLen = strlen(tSignature.szName);








            if (nNameLen < sizeof(tSignature.szName)) {} else  /* INFERENCE: positive compare (cmp x8,#0x40; b.hs at 0x1570s) so the assert is an else arm */ { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            if (nNameLen >= nAmount) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            Blizzard::String::Copy(tSignature.szComment, (const char *)(tBuffer + nNameLen + 1), sizeof(tSignature.szComment));
            int32 nCommentLen = strlen(tSignature.szComment);


            if (nCommentLen < sizeof(tSignature.szComment)) {} else { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            if (nNameLen + nCommentLen >= nAmount) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }

            if (nNameLen + nCommentLen + 5 != nAmount) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            tSignature.nMonth = tBuffer[nNameLen + 1 + nCommentLen + 1];
            tSignature.nDay = tBuffer[nNameLen + 1 + nCommentLen + 2];
            tSignature.nYear = tBuffer[nNameLen + 1 + nCommentLen + 3];
            return TRUE;
        }
    }
    return FALSE;
}

BOOL sReadTextSignature(Signature & tSignature)
{
    Blizzard::File::Stream tStream;
    char szPath[256];
    uint8 tBuffer[2048];
    int32 nAmount;
    Blizzard::String::Format(szPath, sizeof(szPath), "%s\\build.dat", SNOFilesGetConsoleDataPathOverride());
    if (Blizzard::File::Exists(szPath) != Blizzard::File::missing)
    {
        if (Blizzard::File::Open(szPath, 1, tStream))
        {
            nAmount = sizeof(tBuffer);
            bool bRes = Blizzard::File::Read(tStream, tBuffer, 0, &nAmount, 0);
            Blizzard::File::Close(tStream);











            if (!bRes) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            StringTokenizer tTokenizer((const char *)tBuffer, "\n");
            char tToken[256];






            if (!tTokenizer.GetNextToken(tToken, sizeof(tToken))) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            Blizzard::String::Copy(tSignature.szName, tToken, sizeof(tSignature.szName));

            if (!tTokenizer.GetNextToken(tToken, sizeof(tToken))) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            Blizzard::String::Copy(tSignature.szComment, tToken, sizeof(tSignature.szComment));

            if (!tTokenizer.GetNextToken(tToken, sizeof(tToken))) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            StringTokenizer tDateTokenizer(tToken, "-");
            char tDateToken[32];


            if (!tDateTokenizer.GetNextToken(tDateToken, sizeof(tDateToken))) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            tSignature.nMonth = atoi(tDateToken);
            if (!tDateTokenizer.GetNextToken(tDateToken, sizeof(tDateToken))) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            tSignature.nDay = atoi(tDateToken);
            if (!tDateTokenizer.GetNextToken(tDateToken, sizeof(tDateToken))) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }
            tSignature.nYear = atoi(tDateToken);
            return TRUE;
        }
    }
    return FALSE;
}
