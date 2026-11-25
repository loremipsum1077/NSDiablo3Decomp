#include "CombatLog.h"
#include "SNO/SNOFiles.h"
#include "../Sigma/RefString.h"
#include "ActorCommonData.h"
#include "Monsters.h"
#include "Player.h"
#include "SNO/SymbolicValue.h"
#include "../Server/PlayerList.h"
#include "Game.h"
#include "Achievements/AchievementDefines.h"
#include "../Sigma/Math/MathGeneral.h"
#include "GameCommonData.h"
#include "../Sigma/Files/XFiles.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "Networking/GameMessage.h"
// X/Sigma/Files/XFilesCore.cpp:1232
BOOL FileWriteSequential(FileReference & tFileRef, const void * lpBuf, uint32 & dwOffset, const uint32 dwSize);
// X/Sigma/Files/XFilesCore.cpp
void FileClose(FileReference & tFileRef);
// X/Client/UserInterface/UI.cpp:3860
void UI_Print(LPCSTR szText, const BOOL fEchoToConsole);
namespace CombatLog {
inline void DisableLogging();
}
extern const SymbolicEntry gDeliveryMechanismTypes[];  // X/Common/SNO/SymbolicValue.cpp
// X/Common/Achievements/AchievementDefines.cpp:158
namespace Achievement {
BOOL IsHealTypeEvaluable(uint32 eHealType);
}
extern const SymbolicEntry gHealTypes[];  // X/Common/SNO/SymbolicValue.cpp
#include "../Server/Achievements/AchievementModifierEvaluationLogic.h"
#include "../Client/Gfx/GfxVB.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
namespace CombatLog {
// X/Common/CombatLog.h:38 (declared here: CombatLog.h does not carry the message types yet).
// The DWARF also declares a Message() constructor (CombatLog.h:50) with no code; left out.
struct Message {  // size 0x24
    RequiredMessageHeader tHeader;  // 0x00
    LogEventType eEventType;  // 0x08
    ACDNetworkName idACDSource;  // 0x0c
    ACDNetworkName idACDTarget;  // 0x10
    SNO snoData;  // 0x14
    float flParam1;  // 0x18
    float flParam2;  // 0x1c
    uint32 dwParam1;  // 0x20
}; static const TypeDescriptorField Message_Fields[9] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(Message, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(Message, eEventType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(Message, idACDSource), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(Message, idACDTarget), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(Message, snoData), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Message, flParam1), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Message, flParam2), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(Message, dwParam1), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Message), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_Message("Message", 0, Message_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_COMBAT_LOG_MESSAGE = &TypeDescriptor_Message;

struct CombatLogGlobals {  // size 0x1028

    BOOL bEnabled;  // 0x00
    BOOL bSilent;  // 0x04
    BOOL bVerbose;  // 0x08
    FileReference tLogFile;  // 0x10
    uint32 dwOffset;  // 0x1020

    CombatLogGlobals()
    {
        bEnabled = FALSE;
        bSilent = FALSE;
        bVerbose = FALSE;
        dwOffset = 0;
        tLogFile.Clear();
    }
};


inline CombatLogGlobals * GetGlobals()
{
    if (!GameCommonDataExists())
        return NULL;

    return GameGetActiveGameCommonData().ptCombatLogGlobals;
}






void sEvaluateHealingAchievements(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flHealing, uint32 eHealType)
{
    if (!Achievement::IsHealTypeEvaluable(eHealType))
        return;

    if (!GameActiveGameCommonDataIsServer())
        return;

    Player * ptPlayerHealed = PlayerGetByACD(idACDTarget);
    if (ptPlayerHealed)
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(ptPlayerHealed);
        tEvaluationData.SetPower(snoPower);
        tEvaluationData.EvaluateEvent(ptPlayerHealed, Achievement::ACH_EVENT_PLAYER_HEALED, eHealType, Math::FloatRoundToInt(flHealing), (uint32)sub_71007b5e70());

        Player * ptPlayerCaster = PlayerGetByACD(idACDCaster);
        if (ptPlayerCaster && ptPlayerCaster != ptPlayerHealed)
        {
            tEvaluationData.SetPlayer(ptPlayerCaster);
            tEvaluationData.EvaluateEvent(ptPlayerCaster, Achievement::ACH_EVENT_PLAYER_HEALED_PLAYER, eHealType, Math::FloatRoundToInt(flHealing), (uint32)sub_71007b5e70());
        }
    }
}































void sGetActorName(CRefString & sName, ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        if (ACDIsPlayer(idACD))
        {
            Player * ptPlayer = PlayerGetByACD(idACD);
            if (ptPlayer)
            {
                sName = ptPlayer->usHeroName;
                return;
            }
        }

        if (ptACD->IsMonster())
            sName = MonsterGetName(ptACD, FALSE);
        else
            sName = SNOToString(ACTOR_SNO, ptACD->snoActor, FALSE);
        return;
    }
    sName = "Unknown";
}










inline CRefString GetTimeStamp()
{
    Blizzard::Time::Timestamp timeStamp = Blizzard::Time::GetTimestamp();
    Blizzard::Time::TimeRec timeRec;
    Blizzard::Time::BreakTime(timeStamp, timeRec);
    uint32 milliseconds = timeRec.nanosecond / 1000000;
    CRefString sSysTime;
    sSysTime.Format("%d:%d:%d.%03d", timeRec.hour, timeRec.minute, timeRec.second, milliseconds);
    return sSysTime;
}






void LogString(CRefString & sLogItem)
{
    if (sLogItem.Length() == 0)
        return;

    CombatLogGlobals * ptGlobals = GetGlobals();
    if (!ptGlobals || !ptGlobals->bEnabled)
        return;

    CRefString sFinal = GetTimeStamp();
    sFinal += " : ";
    sFinal += (LPCSTR)sLogItem;
    if (!ptGlobals->bSilent)
        UI_Print(sFinal.str(), TRUE);
    sFinal += "\n";
    FileWriteSequential(ptGlobals->tLogFile, sFinal.str(), ptGlobals->dwOffset, sFinal.Length());
}






















void LogPowerUse(ACDID idACDCaster, SNO snoPower, ACDID idACDTarget)
{
    CRefString sCasterName;
    CRefString sTargetName;
    CRefString sPowerName;

    sGetActorName(sCasterName, idACDCaster);
    sPowerName = SNOToString(POWER_SNO, snoPower, FALSE);
    sGetActorName(sTargetName, idACDTarget);

    CRefString logString;
    logString.Format("%s casts %s on %s", sCasterName.str(), sPowerName.str(), sTargetName.str());
    LogString(logString);
}








void LogDodge(ACDID idACDTarget, ACDID idACDCaster, SNO snoPower, float flDodge, float flDodgeRoll)
{
    CRefString sTargetName;
    CRefString sCasterName;
    CRefString sPowerName = SNOToString(POWER_SNO, snoPower, FALSE);
    sGetActorName(sTargetName, idACDTarget);
    sGetActorName(sCasterName, idACDCaster);

    CRefString logString;
    BOOL bDodge = flDodgeRoll < flDodge;
    logString.Format("%s %s(rolled %f needed less than %f) %s's %s", sTargetName.str(), bDodge ? "dodges" : "fails to dodge", flDodgeRoll, flDodge, sCasterName.str(), sPowerName.str());
    LogString(logString);
}















void LogHitRoll(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flHitChance, float flHitRoll)
{
    CRefString sTargetName;
    CRefString sCasterName;
    CRefString sPowerName;
    sGetActorName(sTargetName, idACDTarget);
    sGetActorName(sCasterName, idACDCaster);
    sPowerName = SNOToString(POWER_SNO, snoPower, FALSE);

    CRefString logString;
    BOOL bHit = flHitRoll < flHitChance;
    logString.Format("%s %s(rolled %f needed less than %f) %s's %s", sCasterName.str(), bHit ? "hits" : "misses", flHitRoll, flHitChance, sTargetName.str(), sPowerName.str());
    LogString(logString);
}

















void LogTargetInvulernable(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower)
{
    CRefString sTargetName;
    CRefString sCasterName;
    CRefString sPowerName;
    sGetActorName(sTargetName, idACDTarget);
    sGetActorName(sCasterName, idACDCaster);
    sPowerName = SNOToString(POWER_SNO, snoPower, FALSE);

    CRefString logString;
    logString.Format("%s's %s fails to damage %s(Invulnerable)", sCasterName.str(), sPowerName.str(), sTargetName.str());
    LogString(logString);
}











void LogCriticalHit(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flCritChance, float flCritRoll)
{
    CRefString sTargetName;
    CRefString sCasterName;
    CRefString sPowerName;
    sGetActorName(sTargetName, idACDTarget);
    sGetActorName(sCasterName, idACDCaster);
    sPowerName = SNOToString(POWER_SNO, snoPower, FALSE);

    CRefString logString;
    BOOL bCrit = flCritRoll < flCritChance;
    logString.Format("%s's %s %s (rolled %f, needed less then %f) %s", sCasterName.str(), sPowerName.str(), bCrit ? "crit's" : "does not crit", flCritRoll, flCritChance, sTargetName.str());
    LogString(logString);
}


















void LogCrushingBlow(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flProcChance, float flRoll)
{
    CRefString sTargetName;
    CRefString sCasterName;
    CRefString sPowerName;
    sGetActorName(sTargetName, idACDTarget);
    sGetActorName(sCasterName, idACDCaster);
    sPowerName = SNOToString(POWER_SNO, snoPower, FALSE);

    CRefString logString;
    BOOL bCrushingBlow = flRoll < flProcChance;
    logString.Format("%s's %s %s (rolled %f, needed less then %f) %s", sCasterName.str(), sPowerName.str(), bCrushingBlow ? "dealt crushing blow" : "does not deal crushing blow", flRoll, flProcChance, sTargetName.str());
    LogString(logString);
}


















void LogDamageReduction(ACDID idACDTarget, uint32 eDamageType, float flOriginalDamage, float flReducedDamage)
{
    if (flOriginalDamage - flReducedDamage < 1.0f)
        return;

    CRefString logString;
    CRefString sTargetName;
    sGetActorName(sTargetName, idACDTarget);
    logString.Format("%s applies damage reduction, %f reduced to %f (damage type:%d)", sTargetName.str(), flOriginalDamage, flReducedDamage, eDamageType);
    LogString(logString);
}















void LogBlockRoll(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flBlockChance, float flBlockRoll)
{
    CRefString sTargetName;
    CRefString sCasterName;
    CRefString sPowerName;
    sGetActorName(sTargetName, idACDTarget);
    sGetActorName(sCasterName, idACDCaster);
    sPowerName = SNOToString(POWER_SNO, snoPower, FALSE);

    CRefString logString;
    BOOL bBlock = flBlockRoll < flBlockChance;
    logString.Format("%s %s %s's  %s (rolled %f, needed less then %f)", sTargetName.str(), bBlock ? "blocks" : "fails to block", sCasterName.str(), sPowerName.str(), flBlockRoll, flBlockChance);
    LogString(logString);
}















void LogItemProcRoll(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flProcChance, float flProcRoll)
{
    BOOL bProcced = FALSE;
    CRefString sTargetName;
    CRefString sCasterName;
    CRefString sPowerName;
    sGetActorName(sTargetName, idACDTarget);
    sGetActorName(sCasterName, idACDCaster);
    sPowerName = SNOToString(POWER_SNO, snoPower, FALSE);

    CRefString logString;
    logString.Format("ItemProc Roll: %s %s %s on %s (rolled %f, needed less than %f)", sCasterName.str(), bProcced ? "Procs" : "Fails to proc", sPowerName.str(), sTargetName.str(), flProcChance, flProcRoll);
    LogString(logString);
}















void LogResistDamage(ACDID idACDCaster, ACDID idACDTarget, uint32 eDamageType, float flDamageTakenPercent, float flDamage)
{
    CRefString sTargetName;
    CRefString sCasterName;
    sGetActorName(sTargetName, idACDTarget);
    sGetActorName(sCasterName, idACDCaster);

    CRefString logString;
    logString.Format("%s resists %f percent of %s's %f damage(Type: %d)", sTargetName.str(), (1.0f - flDamageTakenPercent) * 100.0f, sCasterName.str(), flDamage, eDamageType);
    LogString(logString);
}














void LogDamageDone(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flDamage, uint32 bCrit, uint32 eDamageType, int32 eDeliveryMechanism)
{
    CRefString sTargetName;
    CRefString sCasterName;
    CRefString sPowerName;
    sGetActorName(sTargetName, idACDTarget);
    sGetActorName(sCasterName, idACDCaster);
    sPowerName = SNOToString(POWER_SNO, snoPower, FALSE);

    CRefString logString;
    logString.Format("%s's %s %s %s for %f PDM:%s %s damage", sCasterName.str(), sPowerName.str(), bCrit ? "crits" : "hits", sTargetName.str(), flDamage, SymbolToName(eDeliveryMechanism, gDeliveryMechanismTypes), SymbolToName(eDamageType, gDamageTypes));
    LogString(logString);
}


















void LogHealingDone(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flHealing, uint32 eHealType)
{
    CRefString sTargetName;
    CRefString sCasterName;
    CRefString sPowerName;
    sGetActorName(sTargetName, idACDTarget);
    sGetActorName(sCasterName, idACDCaster);
    sPowerName = SNOToString(POWER_SNO, snoPower, FALSE);

    CRefString logString;
    logString.Format("%s's %s heals %s for %f (Type: %s)", sCasterName.str(), sPowerName.str(), sTargetName.str(), flHealing, SymbolToName(eHealType, gHealTypes));
    LogString(logString);
}














BOOL ShowHealingNonVerbose(
    uint32 eHealType,
    const ACDID idACDHealer,
    const ACDID idACDTarget)
{
    switch (eHealType)  // heal-type values as the original tests them; no enumerator names in the DWARF
    {
    case 6:
    case 8:
        return FALSE;

    case 2:
    case 9:
    case 10:

        return ACDIsPlayer(idACDTarget);

    default:
        return ACDIsPlayer(idACDHealer) || ACDIsPlayer(idACDTarget);
    }

}
void LogDeath(ACDID idACDKiller, ACDID idACDVictim)
{
    CRefString sKiller;
    CRefString sVictim;
    sGetActorName(sKiller, idACDKiller);
    sGetActorName(sVictim, idACDVictim);

    CRefString logString;
    logString.Format("%s kills %s", sKiller.str(), sVictim.str());
    LogString(logString);
}








void LogEvent(
    LogEventType eType,
    ACDID idACD1,
    ACDID idACD2,
    SNO snoData,
    float flParam1,
    float flParam2,
    uint32 dwParam1)
{

    if (GameActiveGameCommonDataIsServer() && eType == LOG_EVENT_HEALING_DONE)
    {

        sEvaluateHealingAchievements(idACD1, idACD2, snoData, flParam1, dwParam1);
    }






    CombatLogGlobals * pGlobals = GetGlobals();
    if (!pGlobals->bEnabled)
        return;



    if (GameActiveGameCommonDataIsServer())
    {

        Message tMessage;
        tMessage.eEventType = eType;
        tMessage.idACDSource = ANNFromACD(idACD1);
        tMessage.idACDTarget = ANNFromACD(idACD2);
        tMessage.snoData = snoData;
        tMessage.flParam1 = flParam1;
        tMessage.flParam2 = flParam2;
        tMessage.dwParam1 = dwParam1;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
        MessageSendToClient(listDestPlayers, SMSG_COMBAT_LOG_ENTRY, &tMessage, sizeof(tMessage));

        return;
    }



    if (!ACDTryToGet(idACD1))
        return;




    switch (eType)
    {
    case LOG_EVENT_POWER_USE:
        LogPowerUse(idACD1, snoData, idACD2);
        break;

    case LOG_EVENT_DODGE:
        LogDodge(idACD1, idACD2, snoData, flParam1, flParam2);
        break;

    case LOG_EVENT_HIT_ROLL:
        if (pGlobals->bVerbose)
        {
            LogHitRoll(idACD1, idACD2, snoData, flParam1, flParam2);
        }
        break;

    case LOG_EVENT_TARGET_INVULNERABLE:
        LogTargetInvulernable(idACD1, idACD2, snoData);
        break;

    case LOG_EVENT_CRITICAL_HIT:
        LogCriticalHit(idACD1, idACD2, snoData, flParam1, flParam2);
        break;

    case LOG_EVENT_CRUSHING_BLOW:
        LogCrushingBlow(idACD1, idACD2, snoData, flParam1, flParam2);
        break;

    case LOG_EVENT_DAMAGE_REDUCTION:
        if (pGlobals->bVerbose)
        {
            LogDamageReduction(idACD1, dwParam1, flParam1, flParam2);
        }
        break;

    case LOG_EVENT_BLOCK_ROLL:
        if (pGlobals->bVerbose)
        {
            LogBlockRoll(idACD1, idACD2, snoData, flParam1, flParam2);
        }
        break;

    case LOG_EVENT_RESISTANCE_APPLIED:
        if (pGlobals->bVerbose)
        {
            LogResistDamage(idACD1, idACD2, dwParam1, flParam1, flParam2);
        }
        break;

    case LOG_EVENT_DAMAGE_DONE:
        LogDamageDone(idACD1, idACD2, snoData, flParam1, dwParam1 & 0xFF, dwParam1 >> 16, (int32)flParam2);
        break;

    case LOG_EVENT_HEALING_DONE:
        if (pGlobals->bVerbose || ShowHealingNonVerbose(dwParam1, idACD1, idACD2))
        {
            LogHealingDone(idACD1, idACD2, snoData, flParam1, dwParam1);
        }
        break;

    case LOG_EVENT_DEATH:
        LogDeath(idACD1, idACD2);
        break;

    case LOG_EVENT_ITEM_PROC_ROLL:
        if (pGlobals->bVerbose)
        {
            LogItemProcRoll(idACD1, idACD2, snoData, flParam1, flParam2);
        }
        break;
    }




}


void InitializeForGame()
{
    GameGetActiveGameCommonData().ptCombatLogGlobals = new CombatLogGlobals;
}



void DisposeForGame()
{
    GameCommonData & tData = GameGetActiveGameCommonData();
    if (tData.ptCombatLogGlobals)
    {
        DisableLogging();
        delete tData.ptCombatLogGlobals;
        tData.ptCombatLogGlobals = NULL;
    }
}



















































inline void DisableLogging()
{
    if (GameActiveGameCommonDataIsClient())
    {
        CombatLogGlobals * ptGlobals = GetGlobals();
        if (ptGlobals && ptGlobals->bEnabled)
        {
            FileClose(ptGlobals->tLogFile);
            ptGlobals->tLogFile.Clear();
            ptGlobals->bEnabled = FALSE;
        }
    }
}
}  // namespace CombatLog
