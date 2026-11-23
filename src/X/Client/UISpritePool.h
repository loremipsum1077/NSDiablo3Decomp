// Hand-written; the reference header only lists the instantiations.
// Original file: X/Client/UISpritePool.h
// Bodies written from the UISpritePool<TimerUISprite, 128> copies in UITimerControl.cpp.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CGame.h"
#include "Sprite.h"

template <typename TYPE, int MAX_SPRITES>
class UISpritePool : public UISpritePoolInterface {
public:
    UISpritePool() {}  // line 16

    virtual ~UISpritePool()  // line 19
    {
        Dispose();
    }

    virtual void Initialize()  // line 24
    {
        for (int32 nAsyncData = 0; nAsyncData < 2; ++nAsyncData)
        {
            ptUISprites[nAsyncData] = new TYPE[MAX_SPRITES];
            nUISpriteCount[nAsyncData] = 0;
        }
    }

    virtual void Dispose()  // line 33
    {
        for (int32 nAsyncData = 0; nAsyncData < 2; ++nAsyncData)
        {
            delete[] ptUISprites[nAsyncData];
            ptUISprites[nAsyncData] = NULL;
        }
    }

    virtual void DeleteAll()  // line 42
    {
        nUISpriteCount[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DEVICE_NOT_AVAILABLE)] = 0;
    }

    TYPE * Get()  // line 47
    {
        int32 nAsyncData = CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DEVICE_AVAILABLE);

        if (nUISpriteCount[nAsyncData] >= MAX_SPRITES)
            return NULL;

        return &ptUISprites[nAsyncData][nUISpriteCount[nAsyncData]++];
    }

    int32 nUISpriteCount[2];  // 0x18
    TYPE * ptUISprites[2];  // 0x20
};
