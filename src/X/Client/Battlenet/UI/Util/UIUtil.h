// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Battlenet/UI/Util/UIUtil.h
// Subset: the time-span types and the functions UIUtil.cpp defines. BasicAsynchRActorCreator and
// HeroAsynchRActorCreator (and the headers they need) are not adopted yet.

#pragma once

#include "../../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Common/GameSwig.h"
#include "../../../../Sigma/RefString.h"
#include "../../../../Common/SNO/SNOFiles.h"
#include "../../../../Sigma/SigmaTypes.h"

enum {
    TIMEFRAME_TODAY = 0,
    TIMEFRAME_YESTERDAY = 1,
    TIMEFRAME_TWO_DAYS_AGO = 2,
    TIMEFRAME_THREE_DAYS_AGO = 3,
    TIMEFRAME_EARLIER_THIS_WEEK = 4,
    TIMEFRAME_LAST_WEEK = 5,
    TIMEFRAME_TWO_WEEKS_AGO = 6,
    TIMEFRAME_EARLIER_THIS_MONTH = 7,
    TIMEFRAME_LAST_MONTH = 8,
    TIMEFRAME_MONTHS_AGO = 9,
    TIMEFRAME_LAST_YEAR = 10,
    TIMEFRAME_YEARS_AGO = 11,
    TIMEFRAME_FUTURE = 12,
    TIMEFRAME_COUNT = 13,
};

typedef int32 TimeFrame;

// The inline bodies below are not in the DWARF; they are written from the copies inlined into
// UIUtil.cpp (GetTimeSpan, DisplayElapsedTime).
struct TimeSpan {  // size 0x30
    int64 years;  // 0x00
    int64 days;  // 0x08
    int64 hours;  // 0x10
    int64 minutes;  // 0x18
    int64 seconds;  // 0x20
    int64 milliseconds;  // 0x28
    TimeSpan() : years(0), days(0), hours(0), minutes(0), seconds(0), milliseconds(0) {}  // line 341
    bool IsZero() const;
    bool IsLessThanOrEqualToZero() const;
    void SetZero();
    int64 TotalDays() const { return years * 365 + days; }  // line 348
    int64 TotalHours() const { return TotalDays() * 24 + hours; }  // line 349
    int64 TotalMinutes() const { return TotalHours() * 60 + minutes; }  // line 350
    int64 TotalSeconds() const { return TotalMinutes() * 60 + seconds; }  // line 351
    int64 TotalMilliseconds() const;
};
static_assert(sizeof(TimeSpan) == 0x30, "TimeSpan");

struct ElapsedTimeDisplayOptions {  // size 0x1c
    ElapsedTimeDisplayOptions()  // line 366; body inferred from its callers
        : fAlwaysShowSeconds(FALSE), fOnlyDisplayNumber(FALSE), fOnlyDisplaySeconds(FALSE), fCountUp(FALSE),
          fDisplayMilliseconds(FALSE), fRoundDown(FALSE), fUseAltGermanCase(FALSE) {}
    BOOL fAlwaysShowSeconds;  // 0x00
    BOOL fOnlyDisplayNumber;  // 0x04
    BOOL fOnlyDisplaySeconds;  // 0x08
    BOOL fCountUp;  // 0x0c
    BOOL fDisplayMilliseconds;  // 0x10
    BOOL fRoundDown;  // 0x14
    BOOL fUseAltGermanCase;  // 0x18
};
static_assert(sizeof(ElapsedTimeDisplayOptions) == 0x1c, "ElapsedTimeDisplayOptions");

// The functions UIUtil.cpp defines, in its line order.
SNO GetActBackgroundSNO(Act eAct);
SNO GetFrontendCharacterSNO(PlayerClass eClass, BOOL fIsFemale);
SNO GetHeroCreateCharacterSNO(PlayerClass eClass, BOOL fIsFemale);
BOOL HasOnlySpaces(const CRefString & szText);
TimeSpan GetTimeSpan(const Blizzard::Time::Timestamp & tStart, const Blizzard::Time::Timestamp & tEnd);
CRefString DisplayElapsedTime(const TimeSpan & span, const ElapsedTimeDisplayOptions & tElapsedTimeDisplayOptions);
CRefString DisplayElapsedTime(const Blizzard::Time::Timestamp & tStart, const Blizzard::Time::Timestamp & tEnd, const ElapsedTimeDisplayOptions & tElapsedTimeDisplayOptions);
CRefString DisplayElapsedTime(const uint32 dwStartTicks, const uint32 dwEndTicks, const ElapsedTimeDisplayOptions & tElapsedTimeDisplayOptions);
CRefString GetUnknownHyperlinkText();
CRefString DisplayActName(Act eAct);
CRefString DisplayHandicap(int32 nHandicap);
CRefString DisplayFormattedHandicap(int32 nHandicap);
CRefString DisplayHandicapDescription(int32 nHandicap);
CRefString StripWhitespace(const CRefString & sUtf8Input);
CRefString GetPortraitImageName(const PlayerClass ePlayerClass, const BOOL fIsMale);
