// Original file: X/Client/UserInterface/Console/ConsoleFanfareManager.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Subsystems/FanfareManager.h"
#include "../../Battlenet/UI/Util/UIAchievementsUtil.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../UIStoryboard.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../CGame.h"
#include "../../ImageCollection.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/CosmeticItems.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/OnlineService/Achievements.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
// Declared here because their headers are not adopted yet.
UIStoryboard & UI_GetStoryboardFromID(const UIID & id); UIText & UI_GetTextFromID(const UIID & id); UIRActor & UI_GetRActorFromID(const UIID & id);  // UI.cpp
EffectGroupID EffectGroupCreateTracked(SNO snoEffectGroup, RActorID idRActorSource, RActorID idRActorTarget, int32 nUnused0, int32 nUnused1); void EffectGroupOutro(EffectGroupID idEffectGroup);  // X/Client/EffectGroup.cpp
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType, const PlayerClass eClass, const BOOL fIsMale);  // CActorInventory.cpp
namespace UIEventQueue { namespace Console { void DisplayMessageWithIconForPlayer(const CRefString & sMessage, const Player * ptPlayer, const ImageTextureFrame & tImage, CRefString * psMessageLine2, BOOL bUseLargeIcon, BOOL bCritical); } }  // UIEventQueue.cpp
#include "../Interface/UITutorials.h"
extern UITutorialManager gtTutorialMgr;  // UITutorials.h





static const int32 MAX_QUEUED_FANFARES = 20;



static void sUpdateFanfarePosition(const UIID & idFanfare)
{

    if (idFanfare != GetUIIDNone())
    {



        Vector2D vecTL, vecDim;
        UIWindow & tFanfare = UI_GetWindowFromID(idFanfare);
        tFanfare.GetRelativePosition(vecTL, vecDim);
        tFanfare.SetRelativePosition(vecTL, vecDim);
    }
}


FanfareManager::FanfareManager()

    : ToastManagerBase(1, 3.5f, FALSE)
{

}



FanfareManager::~FanfareManager()
{
}



FanfareManager::UIAnimationPair::UIAnimationPair()
    : idAnimA(GetUIIDNone())
    , idAnimB(GetUIIDNone())
{
}




FanfareManager::UIAnimationPair::UIAnimationPair(const UIID & idAnimA)

    : idAnimA(idAnimA)
    , idAnimB(GetUIIDNone())
{
}



FanfareManager::UIAnimationPair::UIAnimationPair(const UIID & idAnimA, const UIID & idAnimB)


    : idAnimA(idAnimA)
    , idAnimB(idAnimB)
{
}



void FanfareManager::Update()

{


    if (CGameGlobalsGet().idCurrentCWorld == -1)
    {
        return;
    }









    if (gtTutorialMgr.IsAnyShowing(6))
    {
        return;
    }

    ProcessToasts();

}






void sDisplaySeasonCacheReward(int32 nPlayerIndex)

{
    CRefString sTitle = StringListFetch("ConsoleSeasonJourney:SeasonCacheRewarded");
    CRefString sTextureName = CRefString().Format("Items:p4_SeasonalCache_Icon");
    UIEventQueue::Console::DisplayMessageWithIconForPlayer(sTitle, PlayerGetByPlayerIndex(nPlayerIndex), ImageTextureFrame(sTextureName), NULL, FALSE, TRUE);
}

void sDisplaySeasonJourneyReminder(int32 nPlayerIndex)

{
    CRefString sTitle = StringListFetch("ConsoleSeasonJourney:OpenSeasonJourney");
    CRefString sTextureName = CRefString().Format("UI:left");
    UIEventQueue::Console::DisplayMessageWithIconForPlayer(sTitle, PlayerGetByPlayerIndex(nPlayerIndex), ImageTextureFrame(sTextureName), NULL, FALSE, TRUE);
}

void FanfareManager::DisplayFanfareForPlayer(uint32 nPlayerIndex, OnlineService::AchievementId idAchievement)


{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    Console::Achievements * ptAchievements = tAppGlobals.ptAchievements;


    BOOL bIsSeasonJourneyObjective = ptAchievements->IsSeasonJourneyAchievementObjective(idAchievement);
    OnlineService::AchievementId idSeasonJourneyObjectiveParent = 0;
    OnlineService::AchievementId idSeasonJourneyObjectiveAchievement = 0;
    if (bIsSeasonJourneyObjective && ptAchievements->GetAchievementObjectiveParent(idAchievement, idSeasonJourneyObjectiveParent))
    {
        idSeasonJourneyObjectiveAchievement = idAchievement;
        idAchievement = idSeasonJourneyObjectiveParent;
    }

    const OnlineService::AchievementDefinition * ptDef = ptAchievements->GetAchievementDefinition(idAchievement);
    if (!ptDef)
    {
        return;
    }

    if (ptAchievements->IsSeasonJourneyAchievement(idAchievement))
    {
        CRefString sMessage;
        CRefString sTitle;
        if (bIsSeasonJourneyObjective)
        {
            const OnlineService::AchievementDefinition * ptObjectiveAchievementDefinition =
                ptAchievements->GetAchievementDefinition(idSeasonJourneyObjectiveAchievement);
            if (ptObjectiveAchievementDefinition)
            {
                sMessage = StringListFetch(ptObjectiveAchievementDefinition->m_pszNameKey).str();
            }
        }
        else
        {
            sMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_SEASON_JOURNEY), "SeasonJourneyComplete");
        }

        sTitle = LocalizedSprintf(
            StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_SEASON_JOURNEY), "SeasonJourneyToastType").str(),
            StringListFetch(ptDef->m_pszNameKey).str(), NULL);


        CRefString sTextureName = CRefString().Format("UI:%s", ptDef->m_pszIconTexture);
        UIEventQueue::Console::DisplayMessageWithIconForPlayer(sTitle, PlayerGetByPlayerIndex(nPlayerIndex), ImageTextureFrame(sTextureName), &sMessage, FALSE, TRUE);

        OnlineService::RewardIdList aridRewards;
        if (bIsSeasonJourneyObjective)
        {
            ptAchievements->GetAchievementRewards(idSeasonJourneyObjectiveAchievement, aridRewards);
        }
        else
        {
            ptAchievements->GetAchievementRewards(idAchievement, aridRewards);
        }

        for (uint32 i = 0; i < aridRewards.size(); i++)
        {
            OnlineService::RewardDefinition tReward;
            if (ptAchievements->GetRewardDefinition(aridRewards[i], tReward))
            {



                if (tReward.m_type == OnlineService::REWARD_TYPE_ITEM)
                {
                    GBID gbidItem = tReward.m_id;
                    ScopedItemDefinitionLockR pdefItem(gbidItem);

                    if (pdefItem->snoActor == GlobalSNOGet(GLOBALSNO_SEASON_CACHE))
                    {
                        sDisplaySeasonCacheReward(nPlayerIndex);
                        break;
                    }
                }
            }
        }
        sDisplaySeasonJourneyReminder(nPlayerIndex);
    }
    else
    {
        CRefString sTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS), "FanfareTypeAchievement");
        CRefString sMessage = StringListFetch(ptDef->m_pszNameKey);
        CRefString sTextureName = CRefString().Format("UI:%s", ptDef->m_pszIconTexture);

        UIEventQueue::Console::DisplayMessageWithIconForPlayer(sTitle, PlayerGetByPlayerIndex(nPlayerIndex), ImageTextureFrame(sTextureName), &sMessage, FALSE, FALSE);

    }
}





BOOL FanfareManager::IsActive() const

{
    return UI_GetWindowFromID(UIDLG_BATTLENET_FANFARE).IsActive();
}




void FanfareManager::SendFanfare(OnlineService::AchievementId idAchievement, int32 nRank, const PlatinumAchievementAwardedMessage * ptMessage)



{

    if (m_toastQueue[TOAST_MEDIUM].Count() > MAX_QUEUED_FANFARES)
    {
        return;
    }

    AppGlobals & tAppGlobals = sgtAppGlobals;
    Console::Achievements * ptAchievements = tAppGlobals.ptAchievements;


    BOOL bNeedSeasonJourneyReminder = FALSE;
    BOOL bIsSeasonJourneyObjective = ptAchievements->IsSeasonJourneyAchievementObjective(idAchievement);
    OnlineService::AchievementId idSeasonJourneyObjectiveParent = 0;
    OnlineService::AchievementId idSeasonJourneyObjectiveAchievement = 0;
    if (bIsSeasonJourneyObjective && ptAchievements->GetAchievementObjectiveParent(idAchievement, idSeasonJourneyObjectiveParent))
    {
        idSeasonJourneyObjectiveAchievement = idAchievement;
        idAchievement = idSeasonJourneyObjectiveParent;
    }

    const OnlineService::AchievementDefinition * ptDef = ptAchievements->GetAchievementDefinition(idAchievement);
    if (!ptDef)
    {
        return;
    }


    FanfareToast * ptNewToast = new FanfareToast();
    ptNewToast->sMessage = StringListFetch(ptDef->m_pszNameKey);
    CRefString sTextureName = CRefString().Format("UI:%s", ptDef->m_pszIconTexture);
    ptNewToast->tIcon = ImageTextureFrame(sTextureName);

    if (nRank > 0)
    {
        ptNewToast->sTypeMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS), "FanfareTypeConquest");
    }
    else if (ptAchievements->IsSeasonJourneyAchievement(idAchievement))
    {
        ptNewToast->sTypeMessage = LocalizedSprintf(
            StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_SEASON_JOURNEY), "SeasonJourneyToastType").str(),
            StringListFetch(ptDef->m_pszNameKey).str(), NULL);


        if (bIsSeasonJourneyObjective)
        {
            const OnlineService::AchievementDefinition * ptObjectiveAchievementDefinition =
                ptAchievements->GetAchievementDefinition(idSeasonJourneyObjectiveAchievement);
            if (ptObjectiveAchievementDefinition)
            {
                ptNewToast->sMessage = StringListFetch(ptObjectiveAchievementDefinition->m_pszNameKey).str();
            }
        }
        else
        {
            ptNewToast->sMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_SEASON_JOURNEY), "SeasonJourneyComplete");
        }

        bNeedSeasonJourneyReminder = TRUE;
    }
    else
    {
        ptNewToast->sTypeMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS), "FanfareTypeAchievement");
    }


    ptNewToast->idEffectFront = -1;
    ptNewToast->idEffectBack = -1;
    ptNewToast->nPoints = ptDef->m_pointsValue;
    ptNewToast->nRank = nRank;

    if (ptAchievements->IsSeasonJourneyAchievement(idAchievement))
    {
        if (bIsSeasonJourneyObjective)
        {
            ptNewToast->eToastType = TT_SEASON_JOURNEY_PROGRESSION;
            OnlineService::GetNumCriteriaCompleted(idAchievement,
                ptNewToast->nNumEarnedSeasonJourneyCriteria, ptNewToast->nTotalSeasonJourneyCriteria);
        }
        else
        {
            ptNewToast->eToastType = TT_SEASON_JOURNEY_COMPLETION;
        }
    }

    ptNewToast->cbOnClick.Clear();


    float flFadeInFull =
        UI_GetStoryboardFromID(UISTORYBOARD_BATTLENET_FANFARE_FADEIN_FULL).GetDuration();


    ptNewToast->listStoryboards.AddTail_Tracked(UIAnimationPair(UISTORYBOARD_BATTLENET_FANFARE_FADEIN_FULL));

    float flFadeInPartial =
        UI_GetStoryboardFromID(UISTORYBOARD_BATTLENET_FANFARE_FADEIN_PARTIAL).GetDuration();

    float flWait =
        UI_GetStoryboardFromID(UISTORYBOARD_BATTLENET_FANFARE_WAIT).GetDuration();


    ptNewToast->listStoryboards.AddTail_Tracked(UIAnimationPair(UISTORYBOARD_BATTLENET_FANFARE_WAIT));

    float flFadeOutFull =
        UI_GetStoryboardFromID(UISTORYBOARD_BATTLENET_FANFARE_FADEOUT_FULL).GetDuration();

    float flFadeOutPartial =
        UI_GetStoryboardFromID(UISTORYBOARD_BATTLENET_FANFARE_FADEOUT_PARTIAL).GetDuration();

    OnlineService::RewardIdList aridRewards;

    if (bIsSeasonJourneyObjective)
    {
        ptAchievements->GetAchievementRewards(idSeasonJourneyObjectiveAchievement, aridRewards);
    }
    else
    {
        ptAchievements->GetAchievementRewards(idAchievement, aridRewards);
    }

    int32 nRewards = 0;
    for (uint32 i = 0; i < aridRewards.size(); i++)
    {
        OnlineService::RewardDefinition tReward;
        if (ptAchievements->GetRewardDefinition(aridRewards[i], tReward))
        {



            if (tReward.m_type != OnlineService::REWARD_TYPE_ITEM)
            {
                nRewards++;
            }
            else
            {
                GBID gbidItem = tReward.m_id;
                ScopedItemDefinitionLockR pdefItem(gbidItem);

                if (pdefItem->snoActor == GlobalSNOGet(GLOBALSNO_SEASON_CACHE))
                {
                    sDisplaySeasonCacheReward(0);
                }
            }
        }
    }
    if (bNeedSeasonJourneyReminder)
    {

        sDisplaySeasonJourneyReminder(0);
    }

    if (nRewards > 0)
    {
        ptNewToast->flToastDuration = flFadeInFull + flWait + flFadeOutPartial;
        UIAnimationPair tPair(UISTORYBOARD_BATTLENET_FANFARE_FADEOUT_PARTIAL, UISTORYBOARD_BATTLENET_FANFARE_FADEOUT_POINTS);
        ptNewToast->listStoryboards.AddTail_Tracked(tPair);
    }
    else
    {
        UIAnimationPair tPair(UISTORYBOARD_BATTLENET_FANFARE_FADEOUT_FULL, UISTORYBOARD_BATTLENET_FANFARE_FADEOUT_POINTS);
        ptNewToast->flToastDuration = flFadeInFull + flWait + flFadeOutFull; ptNewToast->listStoryboards.AddTail_Tracked(tPair);
    }

    AddToastToQueue(ptNewToast, TOAST_MEDIUM);

    for (uint32 i = 0; i < aridRewards.size(); i++)
    {
        OnlineService::RewardDefinition tReward;
        if (!ptAchievements->GetRewardDefinition(aridRewards[i], tReward))
        {
            continue;
        }

        if (tReward.m_type == OnlineService::REWARD_TYPE_ITEM)
        {
            continue;
        }



        FanfareToast * ptRewardToast = new FanfareToast();
        ptRewardToast->sMessage = OnlineService::GetRewardTypeString(tReward);

        if (tReward.m_type == OnlineService::REWARD_TYPE_ITEM_TRANSMOG)
        {
            GBID gbidTransmog = tReward.m_id;
            ACDInventoryGetItemDrawInfoWithGBID(gbidTransmog, ptRewardToast->tIcon, 0, 1, TRUE);
        }
        else if (tReward.m_type == OnlineService::REWARD_TYPE_COSMETIC)
        {
            GBID gbidCosmetic = tReward.m_id;
            CosmeticItemType eCosmeticType = CosmeticItems_GetCosmeticItemTypeFromGBID(gbidCosmetic);
            switch (eCosmeticType)
            {
            case COSMETIC_ITEM_WINGS:
                ImageCollectionGetImageInfo("UI:icon_cosmetic_wings", ptRewardToast->tIcon);
                break;

            case COSMETIC_ITEM_PENNANT:
                ImageCollectionGetImageInfo("UI:icon_cosmetic_pennant", ptRewardToast->tIcon);
                break;

            case COSMETIC_ITEM_PET:
                ImageCollectionGetImageInfo("UI:icon_cosmetic_pet", ptRewardToast->tIcon);
                break;

            case COSMETIC_ITEM_FRAME:
                ImageCollectionGetImageInfo("UI:icon_cosmetic_portrait", ptRewardToast->tIcon);
                break;

            default:
                ACDInventoryGetItemDrawInfoWithGBID(gbidCosmetic, ptRewardToast->tIcon, 0, 1, TRUE);
                break;
            }
        }
        else
        {
            ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, tReward.m_hIcon, ptRewardToast->tIcon);
        }

        ptRewardToast->sTypeMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS), "FanfareTypeReward");
        ptRewardToast->idEffectFront = -1; ptRewardToast->idEffectBack = -1;

        ptRewardToast->nPoints = 0; ptRewardToast->nRank = 0;

        ptRewardToast->eToastType = TT_REWARD;
        ptNewToast->cbOnClick.Clear();

        ptRewardToast->listStoryboards.AddTail_Tracked(UIAnimationPair(UISTORYBOARD_BATTLENET_FANFARE_FADEIN_PARTIAL));
        ptRewardToast->listStoryboards.AddTail_Tracked(UIAnimationPair(UISTORYBOARD_BATTLENET_FANFARE_WAIT));

        if (i == aridRewards.size() - 1)
        {
            ptRewardToast->listStoryboards.AddTail_Tracked(UIAnimationPair(UISTORYBOARD_BATTLENET_FANFARE_FADEOUT_FULL));
            ptRewardToast->flToastDuration = flFadeInPartial + flWait + flFadeOutFull;
        }
        else
        {
            ptRewardToast->listStoryboards.AddTail_Tracked(UIAnimationPair(UISTORYBOARD_BATTLENET_FANFARE_FADEOUT_PARTIAL));
            ptRewardToast->flToastDuration = flFadeInPartial + flWait + flFadeOutPartial;
        }

        AddToastToQueue(ptRewardToast, TOAST_MEDIUM);
    }


}



const UIID FanfareManager::CreateToastVisual(int32, ToastManagerBase::Toast * ptToast)


{
    FanfareToast * ptFanfareToast = (FanfareToast *)ptToast;

    UIID idContainer = GetUIIDNone();

    if (ptFanfareToast->eToastType == TT_SEASON_JOURNEY_PROGRESSION || ptFanfareToast->eToastType == TT_SEASON_JOURNEY_COMPLETION)
    {
        idContainer = UIDLG_BATTLENET_SEASON_FANFARE;
    }
    else
    {
        idContainer = UIDLG_BATTLENET_FANFARE;
    }


    UIID idTitle = UIIDCreateFromParent(idContainer, ".Title");
    UI_GetTextFromID(idTitle).SetText(ptFanfareToast->sMessage, FALSE);

    UIID idType = UIIDCreateFromParent(idContainer, ".Type");
    UI_GetTextFromID(idType).SetText(ptFanfareToast->sTypeMessage, FALSE);

    UIID idRank = UIIDCreateFromParent(idContainer, ".RankTitle");

    UIID idPoints = UIIDCreateFromParent(idContainer, ".Points");
    if (UI_TestID(idPoints))
    {
        UIText & tPointText = UI_GetTextFromID(idPoints);
        if (ptFanfareToast->nRank > 0)
        {
            tPointText.SetText(UnicodeText::FormatNumber(ptFanfareToast->nRank, TRUE, FNS_NORMAL, 0), FALSE);
        }
        else if (ptFanfareToast->eToastType == TT_SEASON_JOURNEY_PROGRESSION)
        {
            CRefString sSeasonJourneyNumCriteriaCompleted = LocalizedSprintf(
                StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_SEASON_JOURNEY), "SeasonJourneyProgressNumbers2").str(),
                UnicodeText::FormatNumber(ptFanfareToast->nNumEarnedSeasonJourneyCriteria, TRUE, FNS_NORMAL, 0).str(),
                UnicodeText::FormatNumber(ptFanfareToast->nTotalSeasonJourneyCriteria, TRUE, FNS_NORMAL, 0).str(), NULL);

            tPointText.SetText(sSeasonJourneyNumCriteriaCompleted.str(), FALSE);
        }
        else
        {
            tPointText.SetText(UnicodeText::FormatNumber(ptFanfareToast->nPoints, TRUE, FNS_NORMAL, 0), FALSE);
        }

        UI_GetWindowFromID(idPoints).SetActive(TRUE);
    }

    UIID idIcon = UIIDCreateFromParent(idContainer, ".Icon");
    UI_GetButtonFromID(idIcon).SetAllTextureFrames(ptFanfareToast->tIcon);

    sUpdateFanfarePosition(idContainer);

    if (!ptFanfareToast->listStoryboards.IsEmpty())
    {
        const UIAnimationPair & tPair = ptFanfareToast->listStoryboards.GetHead();
        if (tPair.idAnimA != GetUIIDNone())
        {
            UIStoryboard & tFade = UI_GetStoryboardFromID(tPair.idAnimA);
            tFade.Stop();
            tFade.Play(FALSE, 0.0f);
        }
        if (tPair.idAnimB != GetUIIDNone())
        {
            UIStoryboard & tFade = UI_GetStoryboardFromID(tPair.idAnimB);
            tFade.Stop();
            tFade.Play(FALSE, 0.0f);
        }
    }

    UIID idParticlesFront = UIIDCreateFromParent(idContainer, ".ParticlesFront");
    UIID idParticlesBack = UIIDCreateFromParent(idContainer, ".ParticlesBack");
    UIRActor & tRActorFront = UI_GetRActorFromID(idParticlesFront);
    UIRActor & tRActorBack = UI_GetRActorFromID(idParticlesBack);

    RActorID idRActorFront = tRActorFront.CreateRActorForCurrentContext(
        GlobalSNOGet(GLOBALSNO_ACTOR_ACHIEVEMENT_FANFARE_EMITTER), 1.0f, -1.0f, PRTransform(cgqIdentity, cgwpNull), 0);





    RActorID idRActorBack = tRActorBack.CreateRActorForCurrentContext(
        GlobalSNOGet(GLOBALSNO_ACTOR_ACHIEVEMENT_FANFARE_EMITTER), 1.0f, -1.0f, PRTransform(cgqIdentity, cgwpNull), 0);





    switch (ptFanfareToast->eToastType)
    {
    case TT_REWARD:
        ptFanfareToast->idEffectFront = EffectGroupCreateTracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_REWARD_FANFARE_FRONT), idRActorFront, RACTORID_INVALID, 0, 0);
        ptFanfareToast->idEffectBack = EffectGroupCreateTracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_REWARD_FANFARE_BACK), idRActorBack, RACTORID_INVALID, 0, 0);
        break;
    case TT_ACHIEVEMENT:
        {
            ptFanfareToast->idEffectFront = EffectGroupCreateTracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_ACHIEVEMENT_FANFARE_FRONT), idRActorFront, RACTORID_INVALID, 0, 0);
            ptFanfareToast->idEffectBack = EffectGroupCreateTracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_ACHIEVEMENT_FANFARE_BACK), idRActorBack, RACTORID_INVALID, 0, 0);

            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_ACHIEVEMENT_FANFARE), cgtPSODefaults);

            UIID idShield = UIIDCreateFromParent(idContainer, ".Shield");
            UI_GetWindowFromID(idShield).SetActive(ptFanfareToast->nPoints != 0);
            if (UI_TestID(idRank))
            {
                UI_GetWindowFromID(idRank).SetActive(ptFanfareToast->nRank != 0);
            }
            UI_GetWindowFromID(idPoints).SetActive(ptFanfareToast->nPoints != 0);

            UIID idFOS = UIIDCreateFromParent(idContainer, ".FeatOfStrength");
            UI_GetWindowFromID(idFOS).SetActive(ptFanfareToast->nPoints == 0);
        }
        break;
    case TT_SEASON_JOURNEY_PROGRESSION:
        if (UI_TestID(idRank))
        {
            UI_GetWindowFromID(idRank).SetActive(FALSE);
        }


        ptFanfareToast->idEffectFront = -1; ptFanfareToast->idEffectBack = -1;

        tRActorFront.SetActive(FALSE);
        tRActorBack.SetActive(FALSE);
        break;
    case TT_SEASON_JOURNEY_COMPLETION:
        ptFanfareToast->idEffectFront = EffectGroupCreateTracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_SEASON_FANFARE_FRONT), idRActorFront, RACTORID_INVALID, 0, 0);
        ptFanfareToast->idEffectBack = EffectGroupCreateTracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_SEASON_FANFARE_BACK), idRActorBack, RACTORID_INVALID, 0, 0);

        tRActorFront.SetActive(TRUE);
        tRActorBack.SetActive(TRUE);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_ACHIEVEMENT_FANFARE), cgtPSODefaults);
        UI_GetWindowFromID(idRank).SetActive(ptFanfareToast->nRank != 0);
        UI_GetWindowFromID(idPoints).SetActive(ptFanfareToast->nPoints != 0);
        break;

    default:

        tRActorFront.SetActive(FALSE);
        tRActorBack.SetActive(FALSE);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_ACHIEVEMENT_FANFARE), cgtPSODefaults);
        UI_GetWindowFromID(idRank).SetActive(ptFanfareToast->nRank != 0);
        UI_GetWindowFromID(idPoints).SetActive(ptFanfareToast->nPoints != 0);
        break;
    }

    return idContainer;
}




void FanfareManager::UpdateToastVisual(int32, ToastManagerBase::Toast * ptToast, float)



{
    FanfareToast * ptFanfareToast = (FanfareToast *)ptToast;
    if (!ptFanfareToast->listStoryboards.IsEmpty())
    {
        const UIAnimationPair & tPair = ptFanfareToast->listStoryboards.GetHead();
        BOOL fAnimationADone = TRUE, fAnimationBDone = TRUE;
        if (tPair.idAnimA != GetUIIDNone())
        {
            UIStoryboard & tAnim = UI_GetStoryboardFromID(tPair.idAnimA);
            fAnimationADone = !tAnim.IsPlaying();
        }





        if (tPair.idAnimB != GetUIIDNone())
        {
            UIStoryboard & tAnim = UI_GetStoryboardFromID(tPair.idAnimB);
            fAnimationBDone = !tAnim.IsPlaying();
        }





        if (fAnimationADone && fAnimationBDone)
        {
            ptFanfareToast->listStoryboards.RemoveHead();
            if (!ptFanfareToast->listStoryboards.IsEmpty())
            {
                const UIAnimationPair & tNewPair = ptFanfareToast->listStoryboards.GetHead();
                if (tNewPair.idAnimA != GetUIIDNone())
                {
                    UI_GetStoryboardFromID(tNewPair.idAnimA).Play(FALSE, 0.0f);
                }
                if (tNewPair.idAnimB != GetUIIDNone())
                {
                    UI_GetStoryboardFromID(tNewPair.idAnimB).Play(FALSE, 0.0f);
                }
            }
        }
    }

    sUpdateFanfarePosition(ptToast->idToastWin);
}
void FanfareManager::DisposeToastVisual(ToastManagerBase::Toast * ptToast)

{
    UIID idToast = ptToast->idToastWin;
    if (idToast != GetUIIDNone())
    {



        FanfareToast * ptFanfareToast = (FanfareToast *)ptToast;
        if (!ptFanfareToast->listStoryboards.IsEmpty())
        {
            const UIAnimationPair & tPair = ptFanfareToast->listStoryboards.GetHead();
            if (tPair.idAnimA != GetUIIDNone())
            {
                UIStoryboard & tFade = UI_GetStoryboardFromID(tPair.idAnimA);
                tFade.Stop();
            }
            if (tPair.idAnimB != GetUIIDNone())
            {
                UIStoryboard & tFade = UI_GetStoryboardFromID(tPair.idAnimB);
                tFade.Stop();
            }
        }

        UI_GetControlFromID(idToast).SetActive(FALSE);
        if (ptFanfareToast->idEffectFront != -1)
        {
            EffectGroupOutro(ptFanfareToast->idEffectFront);
        }
        if (ptFanfareToast->idEffectBack != -1)
        {
            EffectGroupOutro(ptFanfareToast->idEffectBack);
        }

        UI_GetRActorFromID(UIRACTOR_BATTLENET_FANFARE_PARTICLES_FRONT).Cleanup();
        UI_GetRActorFromID(UIRACTOR_BATTLENET_FANFARE_PARTICLES_BACK).Cleanup();
    }
}



void FanfareManager::ClearFanfareQueue()

{
    m_toastQueue[TOAST_MEDIUM].RemoveAll();
}
