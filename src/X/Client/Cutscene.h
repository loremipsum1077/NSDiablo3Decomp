// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Client/Cutscene.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/Unicode.h"

enum {
    CUTSCENEID_INVALID = -1,
};

typedef int32 CutsceneID;

typedef void (*CutsceneCallback)();

typedef int32 PlayerClass;

struct CutsceneParams {  // size 0x120
    CutsceneParams();
    CutsceneParams(LPCSTR, LPCSTR);
    CHAR m_szPath[128];  // 0x00
    CHAR m_szSubtitlePath[128];  // 0x80
    BOOL m_fFullscreen;  // 0x100
    BOOL m_fCanBeCancelled;  // 0x104
    BOOL m_fFadeIn;  // 0x108
    int32 m_nAudioTrack;  // 0x10c
    BOOL m_bAudioMuted;  // 0x110
    SNO m_snoAlphaMask;  // 0x114
    CutsceneCallback m_pfnOnFinishedCallback;  // 0x118
};
static_assert(sizeof(CutsceneParams) == 0x120, "CutsceneParams");

inline CutsceneParams::CutsceneParams(LPCSTR szPath, LPCSTR szSubtitlePath)  // line 37
{
    Blizzard::String::Copy(m_szPath, szPath, sizeof(m_szPath));
    Blizzard::String::Copy(m_szSubtitlePath, szSubtitlePath, sizeof(m_szSubtitlePath));
    m_fFullscreen = TRUE;
    m_fCanBeCancelled = TRUE;
    m_fFadeIn = FALSE;
    m_nAudioTrack = 0;
    m_bAudioMuted = FALSE;
    m_snoAlphaMask = SNO_NONE;
    m_pfnOnFinishedCallback = NULL;
}

class CRefString;
struct IVector2D;

// Cutscene.cpp
void CutsceneStop(const CutsceneID idCutscene, const BOOL fStoppingAll);  // line 2197
SNO CutsceneGetTexture(const CutsceneID idCutscene);  // line 1513
BOOL CutsceneIsPlaying(CutsceneID idCutscene);  // line 1527
BOOL CutsceneGetFrameSize(const CutsceneID idCutscene, IVector2D & tSize);  // line 1546
CutsceneID CutscenePlay(const CutsceneParams & tParams);  // line 2099
CRefString CutsceneGetSubtitleName(const CRefString & sFilename, int32 nAudioTrack);  // line 2935

// Functions defined in this file (bodies are not in the DWARF):
//   line    37  CutsceneParams::CutsceneParams(LPCSTR szPath, LPCSTR szSubtitlePath)  [inlined into 5 functions]
