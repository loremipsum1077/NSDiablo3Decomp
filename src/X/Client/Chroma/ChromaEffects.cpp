#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ChromaEffectsPlatform.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/SigmaTiming.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../CPlayer.h"
#include "../Definitions/PreferencesDefinition.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
typedef XPooledList<uint32, uint32> XPooledDWORDList;  // X/Common/Attributes/FastAttribute.h:14 (no src header declares it yet)
void FastAttribGetAllWithAttrib(AttribGroupID idFastAttrib, Attrib nAttrib, XPooledDWORDList & listParams);  // X/Common/Attributes/FastAttribute.cpp:3317
// ChromaPlatformImpl: DWARF has only a declaration (vtable homed elsewhere; no platform impl on NX). Members invented from
// the calls: slot 0 complete dtor (::delete in ~ChromaEffectsGlobals), slot 2 (+0x10) takes (uint32, RGBAColor&) in Update.
class ChromaPlatformImpl { public: virtual ~ChromaPlatformImpl(); virtual void Update(const uint32 dwCurrTime, const RGBAColor & rgbaBkgrnd) = 0; };
namespace ChromaEffects {
enum { NUM_SKILL_HOTBAR_KEY_SLOTS = 4, HOTBAR_POTION_SLOT = 5, NUM_HOTBAR_KEY_SLOTS = 6 };  // X/Client/Chroma/ChromaEffects.h:17
static const float ITEM_TIME = 1.0f;  // X/Client/Chroma/ChromaEffectsPlatform.h:58
}
static const uint32 FLASH_DURATION = 1000;
static const uint32 FLASH_RATE = 250;
static const float LOW_HEALTH_PULSE_PERIOD = 500.0f;




struct BuffColor
{
    BuffColor(LPCSTR szName, RGBAColor col)
    {
        snoPower = SNOFileLookupGUID(POWER_SNO, szName);
        rgbaColor = col;
    }

    SNO snoPower;
    RGBAColor rgbaColor;
};

class ChromaEffectsGlobals
{
public:
    ChromaEffectsGlobals();
    ~ChromaEffectsGlobals();

    BOOL Initalized()
    {

        return m_ptPlatformImpl != NULL;
    }

    void Update();


    HotbarButtonState m_hotbarState[ChromaEffects::NUM_HOTBAR_KEY_SLOTS];

    blz::vector<ChromaEffect> m_listEffects;

private:

    RGBAColor sGetBackgroundColor(const uint32 dwCurrTime);


    BOOL sCheckACDForBuffColor(ActorCommonData * ptACD, RGBAColor & rgbaBuff);



    ChromaPlatformImpl * m_ptPlatformImpl;

    blz::vector<BuffColor> m_buffColors;
};

static ChromaEffectsGlobals * sgptChromaEffectsGlobals = NULL;





ChromaEffectsGlobals::ChromaEffectsGlobals()
    : m_ptPlatformImpl(NULL)
{


    if (Prefs::GetVideoPrefs().GetDisableChromaEffects())
    {
        return;
    }














    if (m_ptPlatformImpl)
    {
        m_buffColors.push_back(BuffColor("Wizard_Archon", RGBAColor(128, 38, 117)));
        m_buffColors.push_back(BuffColor("X1_DemonHunter_Vengeance", RGBAColor(0, 0, 255)));
        m_buffColors.push_back(BuffColor("Barbarian_WrathOfTheBerserker", RGBAColor(255, 61, 0)));
        m_buffColors.push_back(BuffColor("X1_Monk_Epiphany", RGBAColor(205, 151, 29)));
        m_buffColors.push_back(BuffColor("X1_Crusader_AkaratsChampion", RGBAColor(202, 145, 53)));
    }
}




ChromaEffectsGlobals::~ChromaEffectsGlobals()
{
    if (m_ptPlatformImpl)
    {
        ::delete m_ptPlatformImpl; m_ptPlatformImpl = NULL;
    }
}




void ChromaEffectsGlobals::Update()

{
    uint32 dwCurrTime = GetMachineTicks();
    RGBAColor rgbaBkgrnd = sGetBackgroundColor(dwCurrTime);

    m_ptPlatformImpl->Update(dwCurrTime, rgbaBkgrnd);
}




BOOL ChromaEffectsGlobals::sCheckACDForBuffColor(
    ActorCommonData * ptACD,
    RGBAColor & rgbaBuff)
{
    for (int32 i = ATTRIBUTE_BUFF_ICON_COUNT_0; i <= ATTRIBUTE_BUFF_ICON_COUNT_31; i++)
    {
        Attrib eAttrib = (Attrib)i;
        XPooledDWORDList listBuff(MemoryPoolsGetDWORDListPool());
        FastAttribGetAllWithAttrib(ptACD->idFastAttrib, eAttrib, listBuff);
        POS pos = listBuff.GetHeadPosition();
        while (pos)
        {
            SNO snoPower = (SNO)listBuff.GetNext(pos);
            for (BuffColor * iter = m_buffColors.begin(); iter != m_buffColors.end(); ++iter)
            {
                if (snoPower == iter->snoPower &&
                    ptACD->AttributesGetInt(MakeAttribKey(eAttrib, snoPower)) > 0)
                {
                    rgbaBuff = iter->rgbaColor;
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}




RGBAColor ChromaEffectsGlobals::sGetBackgroundColor(
    const uint32 dwCurrTime)
{
    RGBAColor rgbaBkgrnd = crgbaRed;


    ACDID idACDLocalPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDLocalPlayer = ACDTryToGet(idACDLocalPlayer);
    if (ptACDLocalPlayer)
    {
        RGBAColor rgbaBuff;
        if (sCheckACDForBuffColor(ptACDLocalPlayer, rgbaBuff))
        {
            rgbaBkgrnd = rgbaBuff;
        }

        float flValue = ptACDLocalPlayer->GetHitPointsCurrent();
        float flHPMaxTotal = ptACDLocalPlayer->AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_TOTAL);
        float flPct = flValue / flHPMaxTotal;
        if (flPct < 0.3f)
        {
            float flFreq = (float)dwCurrTime / LOW_HEALTH_PULSE_PERIOD;
            float flT = Math::Sin(flFreq * FULL_CIRCLE) * 0.5f + 0.5f;
            rgbaBkgrnd.r = (uint8)(flT * rgbaBkgrnd.r);
            rgbaBkgrnd.g = (uint8)(flT * rgbaBkgrnd.g);
            rgbaBkgrnd.b = (uint8)(flT * rgbaBkgrnd.b);
        }
    }

    return rgbaBkgrnd;
}

namespace ChromaEffects {



void InitializeForApp()

{
    sgptChromaEffectsGlobals = new ChromaEffectsGlobals;
}




void DisposeForApp()

{
    delete sgptChromaEffectsGlobals; sgptChromaEffectsGlobals = NULL;
}




void InitializeForGame()

{
}




void DisposeForGame()

{
    for (int32 i = 0; i < NUM_HOTBAR_KEY_SLOTS; i++)
    {
        sgptChromaEffectsGlobals->m_hotbarState[i].Clear();
    }
    sgptChromaEffectsGlobals->m_listEffects.clear();
}

































void Update()

{
    if (sgptChromaEffectsGlobals == NULL ||
        !sgptChromaEffectsGlobals->Initalized())
    {
        return;
    }

    sgptChromaEffectsGlobals->Update();
}




void TriggerLevelUpEffect()

{
    if (sgptChromaEffectsGlobals == NULL ||
        !sgptChromaEffectsGlobals->Initalized())
    {
        return;
    }

    ChromaEffect effect;
    effect.eType = CE_LEVEL_UP;
    effect.dwStartTime = GetMachineTicks();
    effect.flDurationInSecs = 2.0f;
    effect.rgbaColor = crgbaWhite;

    sgptChromaEffectsGlobals->m_listEffects.push_back(effect);
}




void TriggerItemDropEffect(
    const RGBAColor & rgbaColor)
{
    if (sgptChromaEffectsGlobals == NULL ||
        !sgptChromaEffectsGlobals->Initalized())
    {
        return;
    }

    ChromaEffect effect;
    effect.eType = CE_ITEM_DROP;
    effect.dwStartTime = GetMachineTicks();
    effect.flDurationInSecs = ITEM_TIME;
    effect.rgbaColor = rgbaColor;

    sgptChromaEffectsGlobals->m_listEffects.push_back(effect);
}

}  // namespace ChromaEffects
