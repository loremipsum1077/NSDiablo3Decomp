// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/UIHeroData.h

#pragma once

#include "../../Common/GameBalance.h"
#include "../../Common/OnlineService/OnlineService.h"

struct UIHeroData {  // size 0x140
    // line 17
    void Set(const char * pszHeroFile, OnlineService::HeroId uHeroId) {
        Blizzard::String::Copy(szHeroFile, pszHeroFile, sizeof(szHeroFile));
        uId = uHeroId;
    }
    char szHeroFile[256];  // 0x00
    OnlineService::HeroId uId;  // 0x100
    GBID gbidHeroClass;  // 0x108
    char szHeroName[49];  // 0x10c
};
static_assert(sizeof(UIHeroData) == 0x140, "UIHeroData");
