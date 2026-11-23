// Original file: X/Client/Battlenet/UI/Util/UIUtil.cpp
#include "UIUtil.h"
#include "../../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../../Common/Definitions/StringListDefinition.h"
#include "../../../../Common/Game.h"
#include "../../../../Common/GlobalSNO.h"
#include "../../../../Sigma/Math/MathGeneral.h"
#include "../../../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../../../Sigma/UnicodeText.h"
#include "../../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../../../Contrib/Contrib/blz/include/blz/string.h"
#include <ctype.h>

// Declared here because their headers do not declare them yet.
BOOL SNOPackChunkLoadAct(Act eAct);  // X/Common/SNO/SNOFiles.cpp:11554
Act ActGetOpenWorldDefaultAct();  // X/Common/Act.cpp:550
extern const SymbolicEntry gHandicapNames[];  // X/Common/SNO/SymbolicValue.cpp:5074






































const Blizzard::Time::Timestamp sctOneMillisecond = 1000000;
const Blizzard::Time::Timestamp sctOneSecond = sctOneMillisecond * 1000;
const Blizzard::Time::Timestamp sctOneMinute = sctOneSecond * 60;
const Blizzard::Time::Timestamp sctOneHour = sctOneMinute * 60;
const Blizzard::Time::Timestamp sctOneDay = sctOneHour * 24;
const Blizzard::Time::Timestamp sctOneYear = sctOneDay * 365;

















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































SNO GetActBackgroundSNO(Act eAct)
{
    GlobalSNO eGlobalSNO = GLOBALSNO_WORLD_BATTLENET_MAINSCREEN;
    if (SNOPackChunkLoadAct(eAct))
    {





        switch (eAct)
        {
        case ACT_1:
            eGlobalSNO = GLOBALSNO_WORLD_BATTLENET_ACT1SCREEN;
            break;
        case ACT_2:
            eGlobalSNO = GLOBALSNO_WORLD_BATTLENET_ACT2SCREEN;
            break;
        case ACT_3:
            eGlobalSNO = GLOBALSNO_WORLD_BATTLENET_ACT3SCREEN;
            break;
        case ACT_4:
            eGlobalSNO = GLOBALSNO_WORLD_BATTLENET_ACT4SCREEN;
            break;
        case ACT_5:
            eGlobalSNO = GLOBALSNO_WORLD_BATTLENET_ACT5SCREEN;
            break;
        default:
            eGlobalSNO = GLOBALSNO_WORLD_BATTLENET_MAINSCREEN;
            break;
        case ACT_OPEN_WORLD:



            return GetActBackgroundSNO(ActGetOpenWorldDefaultAct());
        }
    }

    return GlobalSNOGet(eGlobalSNO);
}






SNO GetFrontendCharacterSNO(PlayerClass eClass, BOOL fIsFemale)
{
    GlobalSNO eGlobalSNO = GLOBALSNO_INVALID;


    switch (eClass)
    {

    case PLAYER_CLASS_DEMONHUNTER:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_FRONTEND_DHUNTER_FEMALE : GLOBALSNO_ACTOR_FRONTEND_DHUNTER_MALE;
        break;


    case PLAYER_CLASS_BARBARIAN:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_FRONTEND_BARBARIAN_FEMALE : GLOBALSNO_ACTOR_FRONTEND_BARBARIAN_MALE;
        break;


    case PLAYER_CLASS_WIZARD:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_FRONTEND_WIZARD_FEMALE : GLOBALSNO_ACTOR_FRONTEND_WIZARD_MALE;
        break;


    case PLAYER_CLASS_WITCHDOCTOR:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_FRONTEND_WITCHDOCTOR_FEMALE : GLOBALSNO_ACTOR_FRONTEND_WITCHDOCTOR_MALE;
        break;


    case PLAYER_CLASS_MONK:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_FRONTEND_MONK_FEMALE : GLOBALSNO_ACTOR_FRONTEND_MONK_MALE;
        break;


    case PLAYER_CLASS_CRUSADER:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_FRONTEND_CRUSADER_FEMALE : GLOBALSNO_ACTOR_FRONTEND_CRUSADER_MALE;
        break;


    case PLAYER_CLASS_NECROMANCER:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_FRONTEND_NECROMANCER_FEMALE : GLOBALSNO_ACTOR_FRONTEND_NECROMANCER_MALE;
        break;
    }





    if (eGlobalSNO != GLOBALSNO_INVALID)
    {
        return GlobalSNOGet(eGlobalSNO);
    }
    return SNO_NONE;
}























SNO GetHeroCreateCharacterSNO(PlayerClass eClass, BOOL fIsFemale)
{
    GlobalSNO eGlobalSNO = GLOBALSNO_INVALID;


    switch (eClass)
    {

    case PLAYER_CLASS_DEMONHUNTER:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_STARTUP_DHUNTER_FEMALE : GLOBALSNO_ACTOR_STARTUP_DHUNTER_MALE;
        break;


    case PLAYER_CLASS_BARBARIAN:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_STARTUP_BARBARIAN_FEMALE : GLOBALSNO_ACTOR_STARTUP_BARBARIAN_MALE;
        break;


    case PLAYER_CLASS_WIZARD:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_STARTUP_WIZARD_FEMALE : GLOBALSNO_ACTOR_STARTUP_WIZARD_MALE;
        break;


    case PLAYER_CLASS_WITCHDOCTOR:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_STARTUP_WITCHDOCTOR_FEMALE : GLOBALSNO_ACTOR_STARTUP_WITCHDOCTOR_MALE;
        break;


    case PLAYER_CLASS_MONK:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_STARTUP_MONK_FEMALE : GLOBALSNO_ACTOR_STARTUP_MONK_MALE;
        break;


    case PLAYER_CLASS_CRUSADER:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_STARTUP_CRUSADER_FEMALE : GLOBALSNO_ACTOR_STARTUP_CRUSADER_MALE;
        break;


    case PLAYER_CLASS_NECROMANCER:
        eGlobalSNO = fIsFemale ? GLOBALSNO_ACTOR_STARTUP_NECROMANCER_FEMALE : GLOBALSNO_ACTOR_STARTUP_NECROMANCER_MALE;
        break;
    }




    if (eGlobalSNO != GLOBALSNO_INVALID)
    {
        return GlobalSNOGet(eGlobalSNO);
    }
    return SNO_NONE;
}


BOOL HasOnlySpaces(const CRefString & szText)
{
    for (size_t i = 0; i < szText.Length(); i++)
    {
        if (((LPCSTR)szText)[i] != ' ')
        {
            return FALSE;
        }
    }


    return TRUE;
}













TimeSpan GetTimeSpan(const Blizzard::Time::Timestamp & tStart, const Blizzard::Time::Timestamp & tEnd)
{
    Blizzard::Time::Timestamp tElapsedTime;

    TimeSpan span;
    if (tEnd > tStart)
    {
        tElapsedTime = tEnd - tStart;





        span.years = tElapsedTime / sctOneYear;
        tElapsedTime %= sctOneYear;

        span.days = tElapsedTime / sctOneDay;
        tElapsedTime %= sctOneDay;

        span.hours = tElapsedTime / sctOneHour;
        tElapsedTime %= sctOneHour;

        span.minutes = tElapsedTime / sctOneMinute;
        tElapsedTime %= sctOneMinute;

        span.seconds = tElapsedTime / sctOneSecond;
        tElapsedTime %= sctOneSecond;

        span.milliseconds = tElapsedTime / sctOneMillisecond;
    }

    return span;
}















































CRefString DisplayElapsedTime(const TimeSpan & span, const ElapsedTimeDisplayOptions & tElapsedTimeDisplayOptions)


{
    CRefString sTime;

    if (span.TotalDays() > 0)
    {
        CRefString sDayLabel;
        if (tElapsedTimeDisplayOptions.fUseAltGermanCase)
        {

            sDayLabel = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "StoreDays").str(), UnicodeText::FormatNumber(span.days, TRUE, FNS_NORMAL, 0).str(), NULL);
        }
        else
        {



            sDayLabel = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralDays").str(), UnicodeText::FormatNumber(span.days, TRUE, FNS_NORMAL, 0).str(), NULL);
        }



        if (tElapsedTimeDisplayOptions.fAlwaysShowSeconds)
        {
            if (tElapsedTimeDisplayOptions.fOnlyDisplayNumber)
            {
                sTime.Format("%lld:%02lld", span.days, span.hours);
            }
            else
            {
                CRefString sHourLabel = LocalizedSprintf(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralHours").str(),
                    UnicodeText::FormatNumber(span.hours, TRUE, FNS_NORMAL, 0).str(), NULL);

                CRefString sMinuteLabel = LocalizedSprintf(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralMinutes").str(),
                    UnicodeText::FormatNumber(span.minutes, TRUE, FNS_NORMAL, 0).str(), NULL);

                CRefString sSecondLabel = LocalizedSprintf(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralSeconds").str(),
                    UnicodeText::FormatNumber(span.seconds, TRUE, FNS_NORMAL, 0).str(), NULL);

                sTime.Format("%s %s %s %s",

                             sDayLabel.str(),
                             sHourLabel.str(),
                             sMinuteLabel.str(),
                             sSecondLabel.str());
            }
        }
        else
        {
            int64 nFixedHours = span.hours;
            if (!tElapsedTimeDisplayOptions.fCountUp)
            {
                if (!tElapsedTimeDisplayOptions.fRoundDown)
                {




                    if (span.minutes > 0) { nFixedHours++; }
                }
            }
            if (tElapsedTimeDisplayOptions.fOnlyDisplayNumber)
            {
                sTime.Format("%lld:%02lld", span.days, nFixedHours);
            }
            else if (nFixedHours > 0)
            {
                CRefString sHourLabel = LocalizedSprintf(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralHours").str(),
                    UnicodeText::FormatNumber(nFixedHours, TRUE, FNS_NORMAL, 0).str(), NULL);

                sTime.Format("%s %s",

                             sDayLabel.str(),
                             sHourLabel.str());
            }
            else
            {
                return sDayLabel;
            }
        }
    }

    else if (span.TotalHours() > 0)
    {

        CRefString sHourLabel = LocalizedSprintf(
            StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralHours").str(),
            UnicodeText::FormatNumber(span.hours, TRUE, FNS_NORMAL, 0).str(), NULL);

        if (tElapsedTimeDisplayOptions.fAlwaysShowSeconds)
        {
            if (tElapsedTimeDisplayOptions.fOnlyDisplayNumber)
            {
                sTime.Format("%lld:%02lld", span.hours, span.minutes);
            }
            else
            {
                CRefString sMinuteLabel = LocalizedSprintf(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralMinutes").str(),
                    UnicodeText::FormatNumber(span.minutes, TRUE, FNS_NORMAL, 0).str(), NULL);

                CRefString sSecondLabel = LocalizedSprintf(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralSeconds").str(),
                    UnicodeText::FormatNumber(span.seconds, TRUE, FNS_NORMAL, 0).str(), NULL);

                sTime.Format("%s %s %s",

                             sHourLabel.str(),
                             sMinuteLabel.str(),
                             sSecondLabel.str());
            }
        }
        else
        {
            int64 nFixedMinutes = span.minutes;
            if (!tElapsedTimeDisplayOptions.fCountUp)
            {
                if (!tElapsedTimeDisplayOptions.fRoundDown)
                {




                    if (span.seconds > 0) { nFixedMinutes++; }
                }
            }
            if (tElapsedTimeDisplayOptions.fOnlyDisplayNumber)
            {
                sTime.Format("%lld:%02lld", span.hours, nFixedMinutes);
            }
            else
            {
                CRefString sMinuteLabel = LocalizedSprintf(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralMinutes").str(),
                    UnicodeText::FormatNumber(nFixedMinutes, TRUE, FNS_NORMAL, 0).str(), NULL);

                sTime.Format("%s %s",

                             sHourLabel.str(),
                             sMinuteLabel.str());
            }
        }
    }


    else if (span.TotalMinutes() > 0)
    {
        CRefString sMinuteLabel = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralMinutes");
        if (tElapsedTimeDisplayOptions.fAlwaysShowSeconds)
        {
            if (tElapsedTimeDisplayOptions.fOnlyDisplayNumber)
            {
                sTime.Format("%lld:%02lld", span.minutes, span.seconds);

                if (tElapsedTimeDisplayOptions.fDisplayMilliseconds)
                {
                    float flMillisecondsTime = span.milliseconds / 1000.0;
                    CRefString sMillisecondsTime = UnicodeText::FormatNumber(flMillisecondsTime, FALSE, FNS_NORMAL, 3, 3, 0);
                    LPCSTR szMillisecondsTime = sMillisecondsTime;


                    sTime.Format("%s%s", sTime.str(), szMillisecondsTime + 1);
                }
            }
            else
            {
                CRefString sMinuteLabelScoped = LocalizedSprintf(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralMinutes").str(),
                    UnicodeText::FormatNumber(span.minutes, TRUE, FNS_NORMAL, 0).str(), NULL);

                CRefString sSecondLabel = LocalizedSprintf(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralSeconds").str(),
                    UnicodeText::FormatNumber(span.seconds, TRUE, FNS_NORMAL, 0).str(), NULL);

                sTime.Format("%s %s",

                             sMinuteLabelScoped.str(),
                             sSecondLabel.str());
            }
        }
        else
        {






            int64 nFixedMinutes = span.minutes;
            if (!tElapsedTimeDisplayOptions.fCountUp)
            {
                if (!tElapsedTimeDisplayOptions.fRoundDown)
                {




                    if (span.seconds > 0) { nFixedMinutes++; }
                }
            }
            if (tElapsedTimeDisplayOptions.fOnlyDisplayNumber)
            {
                sTime.Format("%lld", nFixedMinutes);

                if (tElapsedTimeDisplayOptions.fDisplayMilliseconds)
                {
                    float flMillisecondsTime = span.milliseconds / 1000.0;
                    CRefString sMillisecondsTime = UnicodeText::FormatNumber(flMillisecondsTime, FALSE, FNS_NORMAL, 3, 3, 0);
                    LPCSTR szMillisecondsTime = sMillisecondsTime;


                    sTime.Format("%s:%02lld%s", sTime.str(), span.seconds, szMillisecondsTime + 1);
                }
            }
            else
            {
                CRefString sMinuteLabelScoped = LocalizedSprintf(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralMinutes").str(),
                    UnicodeText::FormatNumber(nFixedMinutes, TRUE, FNS_NORMAL, 0).str(), NULL);

                sTime = sMinuteLabelScoped;
            }
        }
    }


    else if (span.TotalSeconds() > 0)
    {
        if (tElapsedTimeDisplayOptions.fOnlyDisplaySeconds)
        {
            sTime.Format("%02lld", span.seconds);
        }
        else if (tElapsedTimeDisplayOptions.fOnlyDisplayNumber)
        {
            sTime.Format("0:%02lld", span.seconds);

            if (tElapsedTimeDisplayOptions.fDisplayMilliseconds)
            {
                float flMillisecondsTime = span.milliseconds / 1000.0;
                CRefString sMillisecondsTime = UnicodeText::FormatNumber(flMillisecondsTime, FALSE, FNS_NORMAL, 3, 3, 0);
                LPCSTR szMillisecondsTime = sMillisecondsTime;


                sTime.Format("%s%s", sTime.str(), szMillisecondsTime + 1);
            }
        }
        else
        {
            CRefString sSecondLabel = LocalizedSprintf(
                StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralSeconds").str(),
                UnicodeText::FormatNumber(span.seconds, TRUE, FNS_NORMAL, 0).str(), NULL);

            sTime = sSecondLabel;
        }
    }
    else
    {


        if (tElapsedTimeDisplayOptions.fOnlyDisplayNumber && tElapsedTimeDisplayOptions.fDisplayMilliseconds)
        {
            float flMillisecondsTime = span.milliseconds / 1000.0;
            CRefString sMillisecondsTime = UnicodeText::FormatNumber(flMillisecondsTime, FALSE, FNS_NORMAL, 3, 3, 0);
            LPCSTR szMillisecondsTime = sMillisecondsTime;


            sTime.Format("0:%02lld%s", span.seconds, szMillisecondsTime + 1);
        }
    }

    return sTime;
}



CRefString DisplayElapsedTime(const Blizzard::Time::Timestamp & tStart, const Blizzard::Time::Timestamp & tEnd, const ElapsedTimeDisplayOptions & tElapsedTimeDisplayOptions)



{
    TimeSpan span = GetTimeSpan(tStart, tEnd);
    return DisplayElapsedTime(span, tElapsedTimeDisplayOptions);
}















CRefString DisplayElapsedTime(const uint32 dwStartTicks, const uint32 dwEndTicks, const ElapsedTimeDisplayOptions & tElapsedTimeDisplayOptions)



{
    Blizzard::Time::Timestamp tStart = (uint64)dwStartTicks * sctOneSecond / 60;
    Blizzard::Time::Timestamp tEnd = (uint64)dwEndTicks * sctOneSecond / 60;

    return DisplayElapsedTime(tStart, tEnd, tElapsedTimeDisplayOptions);
}

















































































































































































































































































CRefString GetUnknownHyperlinkText()
{
    static blz::string unknownString = blz::make_stringf("[%s]", StringListFetch("Bnet_Strings:Unknown").str());
    return unknownString.c_str();
}



































































































































































































































































































































































































































































































CRefString DisplayActName(Act eAct)

{

    CRefString sActName;
    switch (eAct)
    {
    case ACT_1:
        sActName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "Act1");
        break;
    case ACT_2:
        sActName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "Act2");
        break;
    case ACT_3:
        sActName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "Act3");
        break;
    case ACT_4:
        sActName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "Act4");
        break;
    case ACT_5:
        sActName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "Act5");
        break;
    case ACT_OPEN_WORLD:
        sActName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "AdventureMode");
        break;

    case ACT_INVALID:
        sActName = "";
        break;
    }

    return sActName;
}



CRefString DisplayHandicap(int32 nHandicap)
{
    return DisplayFormattedHandicap(nHandicap);
}









CRefString DisplayFormattedHandicap(int32 nHandicap)

{
    CRefString sHandicapLabel(SymbolToName(nHandicap, gHandicapNames));
    CRefString sHandicap = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), sHandicapLabel);
    return sHandicap;
}



CRefString DisplayHandicapDescription(int32 nHandicap)

{

    CRefString sDescriptionLabel;
    sDescriptionLabel.Format("DifficultySelector_Description_%d", Math::Pin(nHandicap, 0, 4));
    float flGoldFindFromHandicap = 0.0f;
    float flXPBonusFromHandicap = 0.0f;
    if (nHandicap >= 0)
    {
        HandicapLevel * ptLevel = GameGetInfoForHandicapLevel(nHandicap);
        if (ptLevel)
        {
            flXPBonusFromHandicap = ptLevel->tHandicapValues.flXPBonus;
            flGoldFindFromHandicap = ptLevel->tHandicapValues.flGoldFind;
        }
    }
    CRefString sDescription = LocalizedSprintf(



        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_GAME_SETTINGS), sDescriptionLabel),

        UnicodeText::FormatNumber(flGoldFindFromHandicap, TRUE, FNS_PERCENT, 0, 0, 0).str(),
        UnicodeText::FormatNumber(flXPBonusFromHandicap, TRUE, FNS_PERCENT, 0, 0, 0).str(), NULL);

    return sDescription;
}




CRefString StripWhitespace(const CRefString & sUtf8Input)

{
    CRefString sOutput(sUtf8Input);
    size_t numChars = 0;
    bool isValidUtf8 = Blizzard::Unicode::CountUTF8Chars(sUtf8Input.str(), sUtf8Input.Length(), numChars);
    if (isValidUtf8 && numChars > 0)
    {
        LPCSTR szCurPos;
        LPCSTR szNonSpaceStart;

        LPCSTR szUtf8Input = sUtf8Input.str();
        bool hasNonSpaceAlready = false;
        LPCSTR szLastContinuousSpaceStart = NULL;
        szNonSpaceStart = szUtf8Input;

        for (szCurPos = szUtf8Input; szCurPos < szUtf8Input + sUtf8Input.Length(); )
        {
            size_t width = Blizzard::Unicode::GetUTF8CharWidth(szCurPos);
            if (width == 1 && isspace(*szCurPos))
            {

                if (!hasNonSpaceAlready)
                {
                    szNonSpaceStart = szCurPos + 1;
                }

                if (!szLastContinuousSpaceStart)
                {
                    szLastContinuousSpaceStart = szCurPos;
                }
            }
            else
            {
                hasNonSpaceAlready = true;
                szLastContinuousSpaceStart = NULL;
            }

            szCurPos += width;
        }
        if (szNonSpaceStart != szUtf8Input || szLastContinuousSpaceStart)
        {
            if (szLastContinuousSpaceStart)
            {
                if (szLastContinuousSpaceStart <= szNonSpaceStart)
                {

                    sOutput = "";
                }
                else
                {

                    sOutput = CRefString(szNonSpaceStart, szLastContinuousSpaceStart - szNonSpaceStart);
                }
            }
            else
            {

                sOutput = CRefString(szNonSpaceStart);
            }
        }
    }

    return sOutput;
}



CRefString GetPortraitImageName(const PlayerClass ePlayerClass, const BOOL fIsMale)


{
    CRefString sImageName("Portrait_");
    switch (ePlayerClass)
    {
    case PLAYER_CLASS_DEMONHUNTER:
        sImageName += "Demonhunter";
        break;
    case PLAYER_CLASS_BARBARIAN:
        sImageName += "Barbarian";
        break;
    case PLAYER_CLASS_WIZARD:
        sImageName += "Wizard";
        break;
    case PLAYER_CLASS_WITCHDOCTOR:
        sImageName += "Witchdoctor";
        break;
    case PLAYER_CLASS_MONK:
        sImageName += "Monk";
        break;
    case PLAYER_CLASS_CRUSADER:
        sImageName += "Crusader";
        break;
    case PLAYER_CLASS_NECROMANCER:
        sImageName += "Necromancer";
        break;
    default:
        sImageName = "PlayerClassUnknown";
        break;
    }




    if (fIsMale)
    {
        sImageName += "_Male";
    }
    else
    {
        sImageName += "_Female";
    }

    return sImageName;
}
