// Original file: X/Client/Battlenet/Subsystems/FrontendBackgroundHeroManager.cpp
// Local declarations stand in for headers not adopted yet; each names its defining file.
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "FrontendBackgroundHeroManager.h"
#include "../../CGame.h"
#include "../../Scene.h"
#include "../../SceneAttachments.h"
#include "../../Attachments.h"
#include "../../RActors/RActors.h"
#include "../../UserInterface/Controls/UIControlControl.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Common/TagMaps/AnimTagsSwig.h"
#include "../../../Common/Definitions/AttachmentDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/Math/Transform.h"

BOOL CSceneDataArrayIterate(TDataArrayIterator<CScene> & tIterator, const CGame & tCGame, CSceneDataArrayIterateMode eIterateMode);  // X/Client/Scene.cpp:3538
float UI_GetXMLFloat32Constant(const UIControlSourceKey & tSrcKey, float flDefaultValue);  // X/Client/UserInterface/UI.cpp:5018
RActorID UI_CreateHeroUIRActor(const SNO snoActor, const SNO snoIrradianceTex, const VisualEquipment & tVisualEquipment, const VisualInventoryType eVIT, const PlayerClass ePlayerClass, const BOOL fFemale, const PRTransform & transform, const RenderContext eContext, const uint32 dwAnimTag, const float flScale, const BOOL fUseHeroLight);  // UIFunctions.cpp:3324
SNO GetFrontendCharacterSNO(PlayerClass eClass, BOOL fIsFemale);  // UIUtil.cpp:1725
SNO GetHeroCreateCharacterSNO(PlayerClass eClass, BOOL fIsFemale);  // UIUtil.cpp:1801
void CPlayerUpdateBanner(const PlayerAccountBanner & tBannerConfiguration, BannerRenderData & tBannerRenderData, BOOL fIsNamePlate);  // CPlayer.cpp:4914
void CPlayerUpdateBannerAttachments(RActor & tRActor, const BannerRenderData & tBannerRenderData);  // CPlayer.cpp:5009
Attachment * AttachmentTryToGet(const AttachmentID idAttachment);  // Attachments.cpp:106
Attachment * AttachmentFind(XPooledDataIDList & tAttachmentList, const uint32 dwID);  // Attachments.cpp:897
void AttachmentDelete(Attachment & tAttachment);  // Attachments.cpp:1058
SNO VisualInventoryGetPetSnoFromGbid(GBID gbidItem);  // VisualInventory.cpp:1302
BOOL RActorPlayAnimation(RActor * ptRActor, const uint32 dwTag, BOOL bDisableBlending, BOOL fOnlyPlayIfDifferent);  // RActorAnimation.cpp:1342
float RActorAnimationGetPercentComplete(const RActor & tRActor);  // RActorAnimation.cpp:1486

static void sShowDeadHeroEffects(BOOL fShow);


BackgroundHeroDescription::BackgroundHeroDescription()
    : m_idRActor(RACTORID_INVALID)
    , m_idPetRActor(RACTORID_INVALID)
    , m_fIsFemale(FALSE)
    , m_eClass(-1)
    , m_fIsDemo(FALSE)
    , m_fIsDead(FALSE)
{
    m_fIsHardcore = FALSE;
    m_tVisualEquipment.Init();
}



BOOL BackgroundHeroDescription::operator==(const BackgroundHeroDescription & tDesc) const
{

    if (m_fIsDemo == tDesc.m_fIsDemo)
    {



        if (m_fIsDemo)
        {
            return m_fIsFemale == tDesc.m_fIsFemale
                && m_eClass == tDesc.m_eClass;
        }


        return m_fIsFemale == tDesc.m_fIsFemale
            && m_fIsDead == tDesc.m_fIsDead
            && m_eClass == tDesc.m_eClass
            && m_fIsHardcore == tDesc.m_fIsHardcore
            && m_tVisualEquipment == tDesc.m_tVisualEquipment;
    }
    return FALSE;
}


BackgroundHeroDescription & BackgroundHeroDescription::operator=(const BackgroundHeroDescription & tDesc)
{

    if (this != &tDesc)
    {


        m_fIsDemo = tDesc.m_fIsDemo;
        m_fIsFemale = tDesc.m_fIsFemale;
        m_fIsHardcore = tDesc.m_fIsHardcore;
        m_fIsDead = tDesc.m_fIsDead;
        m_eClass = tDesc.m_eClass;
        m_tVisualEquipment = tDesc.m_tVisualEquipment;
    }
    return *this;
}


void BackgroundHeroDescription::SetDemoHero(PlayerClass eClass, BOOL fIsFemale, BOOL fIsHardcore)
{




    m_eClass = eClass;
    m_fIsFemale = fIsFemale;
    m_fIsHardcore = fIsHardcore;
    m_fIsDemo = TRUE;
    m_fIsDead = FALSE;
}


void BackgroundHeroDescription::SetPlayerHero(PlayerClass eClass, BOOL fIsFemale, BOOL fIsHardcore, BOOL fIsDead, const VisualEquipment & tVisualEquipment)
{





    m_eClass = eClass;
    m_fIsFemale = fIsFemale;
    m_fIsHardcore = fIsHardcore;
    m_tVisualEquipment = tVisualEquipment;
    m_fIsDemo = FALSE;
    m_fIsDead = fIsDead;
}



FrontendDemoHeroAnimationManager::FrontendDemoHeroAnimationManager()
    : m_idAnimatingRActor(RACTORID_INVALID)
    , m_tHeroAnimationStartTime(0)
    , m_heroEmotePercentComplete(-1.0f)
{

}



void FrontendDemoHeroAnimationManager::StartAnimation(RActorID idDemoHeroRActor)
{

    RActor * ptRActor = RActorTryToGet(idDemoHeroRActor);
    if (ptRActor)
    {
        m_idAnimatingRActor = idDemoHeroRActor;
        RActorPlayAnimation(ptRActor, TAG_ACTOR_CREATION_IDLE, TRUE, FALSE);
        m_tHeroAnimationStartTime = Blizzard::Time::GetTimestamp();
        m_heroEmotePercentComplete = -1.0f;
    }
}
















void FrontendDemoHeroAnimationManager::Update()
{

    if (m_idAnimatingRActor != RACTORID_INVALID)
    {
        RActor * ptRActor = RActorTryToGet(m_idAnimatingRActor);
        if (ptRActor)
        {
            const Blizzard::Time::Timestamp c_tOneSecond = 1000000000;
            Blizzard::Time::Timestamp elapsedTime = Blizzard::Time::GetTimestamp() - m_tHeroAnimationStartTime;

            if (m_heroEmotePercentComplete == -1.0f && elapsedTime > c_tOneSecond)
            {
                RActorPlayAnimation(ptRActor, TAG_ACTOR_CREATION_EMOTE, FALSE, TRUE);
                m_heroEmotePercentComplete = 0.0f;
            }
            else if (m_heroEmotePercentComplete >= 0.0f && ptRActor->ptActorAnim)
            {
                float percentComplete = RActorAnimationGetPercentComplete(*ptRActor);
                if (m_heroEmotePercentComplete > percentComplete
                    || percentComplete >= 0.999999f)
                {
                    RActorPlayAnimation(ptRActor, TAG_ACTOR_CREATION_IDLE, FALSE, TRUE);
                }
                m_heroEmotePercentComplete = percentComplete;
            }
        }
    }
}


FrontendBackgroundHeroManager::FrontendBackgroundHeroManager()

    : m_idBackgroundBanner(ATTACHMENTID_INVALID)
{

}



RActorID FrontendBackgroundHeroManager::GetHeroRActorId(BackgroundHeroSlot eSlot) const
{

    return m_arBackgroundHeros[eSlot].m_idRActor;
}












static PRTransform sGetHardpointTransform(BackgroundHeroSlot eSlot, bool bPet)
{


    CRefString sHardpointLabel;


    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        switch (eSlot)
        {
        case HERO_SLOT_1:
        {
            if (bPet)
            {
                sHardpointLabel = "HP_Pet_02";
            }
            else
            {
                sHardpointLabel = "HP_PartyLeader";
            }
            break;
        }
        case HERO_SLOT_2:
        {
            if (bPet)
            {
                sHardpointLabel = "HP_Pet_01";
            }
            else
            {
                sHardpointLabel = "HP_PartyMemberB";
            }
            break;
        }
        }
    }
    else
    {

        switch (eSlot)
        {
        case HERO_SLOT_1:
        {
            if (bPet)
            {
                sHardpointLabel = "HP_Pet_02";
            }
            else
            {
                sHardpointLabel = "HP_PartyLeader";
            }
            break;
        }
        case HERO_SLOT_2:
        {
            if (bPet)
            {
                sHardpointLabel = "HP_Pet_03";
            }
            else
            {
                sHardpointLabel = "HP_PartyMemberA";
            }
            break;
        }
        case HERO_SLOT_3:
        {
            if (bPet)
            {
                sHardpointLabel = "HP_Pet_01";
            }
            else
            {
                sHardpointLabel = "HP_PartyMemberB";
            }
            break;
        }
        case HERO_SLOT_4:
        {
            if (bPet)
            {
                sHardpointLabel = "HP_Pet_04";
            }
            else
            {
                sHardpointLabel = "HP_PartyMemberC";
            }
            break;
        }
        }
    }







    PRTransform transform(cgqIdentity, cgwpNull);
    CGame & tCGame = CGameGet();
    TDataArrayIterator<CScene> tIt;
    CScene * ptScene = NULL;
    int32 nCount = 0;
    while (CSceneDataArrayIterate(tIt, tCGame, SkipHolodeckScenes))
    {
        ptScene = tIt.ptElement;
        nCount++;
    }



    CSceneGetHardpointTransformByLabel(ptScene->id, HardpointHash(sHardpointLabel.str()), transform);

    return transform;
}































BOOL FrontendBackgroundHeroManager::ChangeBackgroundHero(BackgroundHeroSlot eSlot, const BackgroundHeroDescription & tHeroDescription)
{


    if (eSlot == NUM_HERO_SLOTS)
    {
        BOOL fSuccess = TRUE;
        for (int32 i = 0; i < NUM_HERO_SLOTS; i++)
        {
            fSuccess &= ChangeBackgroundHero(i, tHeroDescription);
        }
        return fSuccess;
    }



    if (m_arBackgroundHeros[eSlot].m_idRActor != RACTORID_INVALID
        && m_arBackgroundHeros[eSlot] == tHeroDescription)
    {
        m_arBackgroundHeros[eSlot].m_fIsHardcore = tHeroDescription.m_fIsHardcore;
        return TRUE;
    }

    float flHeroScale = 1.0f;
    if (!tHeroDescription.m_fIsDemo)
    {
        switch (eSlot)
        {
        case HERO_SLOT_1:
        {
            UIControlSourceKey keyConstant = UIControlSourceKey::Parse("BattleNetCampaignHeroScales:c_HeroScaleSlotLeader", -1, NULL, NULL);
            flHeroScale = UI_GetXMLFloat32Constant(keyConstant, 1.0f);
            break;
        }
        case HERO_SLOT_2:
        {
            UIControlSourceKey keyConstant = UIControlSourceKey::Parse("BattleNetCampaignHeroScales:c_HeroScaleSlotA", -1, NULL, NULL);
            flHeroScale = UI_GetXMLFloat32Constant(keyConstant, 1.0f);
            break;
        }
        case HERO_SLOT_3:
        {
            UIControlSourceKey keyConstant = UIControlSourceKey::Parse("BattleNetCampaignHeroScales:c_HeroScaleSlotB", -1, NULL, NULL);
            flHeroScale = UI_GetXMLFloat32Constant(keyConstant, 1.0f);
            break;
        }
        case HERO_SLOT_4:
        {
            UIControlSourceKey keyConstant = UIControlSourceKey::Parse("BattleNetCampaignHeroScales:c_HeroScaleSlotC", -1, NULL, NULL);
            flHeroScale = UI_GetXMLFloat32Constant(keyConstant, 1.0f);
            break;
        }
        }
    }






    PRTransform playerTransform = sGetHardpointTransform(eSlot, false);
    PRTransform petTransform = sGetHardpointTransform(eSlot, true);

    if (tHeroDescription.m_fIsDemo)
    {
        ClearBackgroundHero(eSlot);
        SNO snoActor = GetHeroCreateCharacterSNO(tHeroDescription.m_eClass, tHeroDescription.m_fIsFemale);
        m_arBackgroundHeros[eSlot].m_idRActor = RActorCreate(snoActor, flHeroScale, -1.0f, playerTransform, 0, RENDERCONTEXT_DEFAULT, -1);
        m_tDemoHeroAnimationManager.StartAnimation(m_arBackgroundHeros[eSlot].m_idRActor);
        sShowDeadHeroEffects(FALSE);
    }
    else
    {
        SNO snoActor = GetFrontendCharacterSNO(tHeroDescription.m_eClass, tHeroDescription.m_fIsFemale);
        ClearBackgroundHero(eSlot);



        VisualEquipment tDisplayEquipment = tHeroDescription.m_tVisualEquipment;
        tDisplayEquipment.tCosmeticItems[VCIS_PET].gbidItem = -1;
        m_arBackgroundHeros[eSlot].m_idRActor = UI_CreateHeroUIRActor(snoActor,
            -1,
            tDisplayEquipment,
            0,

            tHeroDescription.m_eClass,
            tHeroDescription.m_fIsFemale,
            playerTransform,
            RENDERCONTEXT_DEFAULT,
            TAG_ACTOR_SELECTION_SCREEN_IDLE,
            flHeroScale,
            TRUE);
        RActor * ptRActor = RActorGet(m_arBackgroundHeros[eSlot].m_idRActor);
        ptRActor->ePickStyle = PICKSTYLE_EXACT;


        if (tHeroDescription.m_tVisualEquipment.tCosmeticItems[VCIS_PET].gbidItem != -1)
        {
            SNO snoPetActor = VisualInventoryGetPetSnoFromGbid(tHeroDescription.m_tVisualEquipment.tCosmeticItems[VCIS_PET].gbidItem);
            if (snoPetActor != -1)
            {
                m_arBackgroundHeros[eSlot].m_idPetRActor = RActorCreate(snoPetActor, flHeroScale, -1.0f, petTransform, 0, RENDERCONTEXT_DEFAULT, -1);
            }
        }

        if (eSlot == HERO_SLOT_1)
        {
            sShowDeadHeroEffects(tHeroDescription.m_fIsDead);
        }
    }


    m_arBackgroundHeros[eSlot] = tHeroDescription;


    return TRUE;
}


BOOL FrontendBackgroundHeroManager::ChangeBackgroundBanner(const PlayerAccountBanner & tBannerConfiguration)
{

    if (m_idBackgroundBanner == ATTACHMENTID_INVALID)
    {
        GlobalSNO eGlobalBannerSNO = GLOBALSNO_ACTOR_EMOTE_BANNER_LIT;
        SNO snoBanner = GlobalSNOGet(eGlobalBannerSNO);

        CScene * ptCScene = NULL;
        TDataArrayIterator<CScene> tIterator;
        while (CSceneDataArrayIterate(tIterator, CGameGet(), SkipHolodeckScenes))
        {
            ptCScene = tIterator.ptElement;
        }




        m_idBackgroundBanner = CSceneAddAttachment(0, ptCScene->id, snoBanner, 0, HardpointHash("HP_Banner"));
    }

    Attachment * ptAttachment = AttachmentTryToGet(m_idBackgroundBanner);
    if (ptAttachment)
    {
        RActor * ptBannerRActor = RActorGet((RActorID)ptAttachment->idAttachedObject);
        ptBannerRActor->ePickStyle = PICKSTYLE_EXACT;
        if (!ptBannerRActor->ptBannerRenderData)
        {
            ptBannerRActor->ptBannerRenderData = new BannerRenderData;
        }

        CPlayerUpdateBanner(tBannerConfiguration, *ptBannerRActor->ptBannerRenderData, FALSE);
        CPlayerUpdateBannerAttachments(*ptBannerRActor, *ptBannerRActor->ptBannerRenderData);
    }

    return TRUE;
}


void FrontendBackgroundHeroManager::ClearBackgroundBanner()
{

    Attachment * ptAttachment = AttachmentTryToGet(m_idBackgroundBanner);
    if (ptAttachment)
    {
        AttachmentDelete(*ptAttachment);
    }

    m_idBackgroundBanner = ATTACHMENTID_INVALID;
}



void FrontendBackgroundHeroManager::ClearBackgroundHero(BackgroundHeroSlot eSlot)
{


    if (eSlot == NUM_HERO_SLOTS)
    {
        for (int32 i = 0; i < NUM_HERO_SLOTS; i++)
        {
            ClearBackgroundHero(i);
        }
        return;
    }

    m_arBackgroundHeros[eSlot].m_fIsDead = FALSE;
    if (m_arBackgroundHeros[eSlot].m_idRActor != RACTORID_INVALID)
    {
        RActorDelete(m_arBackgroundHeros[eSlot].m_idRActor);
        m_arBackgroundHeros[eSlot].m_idRActor = RACTORID_INVALID;
    }
    if (m_arBackgroundHeros[eSlot].m_idPetRActor != RACTORID_INVALID)
    {
        RActorDelete(m_arBackgroundHeros[eSlot].m_idPetRActor);
        m_arBackgroundHeros[eSlot].m_idPetRActor = RACTORID_INVALID;
    }

    if (eSlot == HERO_SLOT_1)
    {
        sShowDeadHeroEffects(FALSE);
    }
}



BOOL FrontendBackgroundHeroManager::IsBackgroundHeroDead(BackgroundHeroSlot eSlot) const
{

    if (eSlot < NUM_HERO_SLOTS)
    {
        return m_arBackgroundHeros[eSlot].m_fIsDead;
    }

    return FALSE;
}


void FrontendBackgroundHeroManager::Update()
{

    GameCommonData & tGCD = CGameGetGameCommonData();
    if (tGCD.GameModeGet() == GAMEMODE_MENU
        && GameIsStartUp())
    {
        if (m_arBackgroundHeros[HERO_SLOT_1].m_fIsDemo && m_arBackgroundHeros[HERO_SLOT_1].m_idRActor != RACTORID_INVALID)
        {
            m_tDemoHeroAnimationManager.Update();
        }
    }
}



static void sShowDeadHeroEffects(BOOL fShow)
{

    CGame & tCGame = CGameGet();
    CScene * ptScene = NULL;
    int32 nCount = 0;
    TDataArrayIterator<CScene> tIt;
    while (CSceneDataArrayIterate(tIt, tCGame, SkipHolodeckScenes))
    {
        ptScene = tIt.ptElement;
        nCount++;
    }

    if (nCount != 1 || !ptScene)
    {
        return;
    }



    const uint32 dwId = 3;
    Attachment * ptEffectsAttachment = AttachmentFind(ptScene->tAttachmentList, dwId);
    if (fShow && !ptEffectsAttachment)
    {

        AttachmentID idAttachment = CSceneAddAttachment(0, ptScene->id, GlobalSNOGet(GLOBALSNO_ACTOR_DEAD_HARDCORE_EFFECTS), dwId, HardpointHash("HP_FXCharacter_Ground"));

        if (idAttachment == ATTACHMENTID_INVALID)
        {
            CSceneAddAttachment(0, ptScene->id, GlobalSNOGet(GLOBALSNO_ACTOR_DEAD_HARDCORE_EFFECTS), dwId, HardpointHash("HP_PartyLeader"));
        }
    }
    else if (!fShow && ptEffectsAttachment)
    {
        AttachmentDelete(*ptEffectsAttachment);
    }
}
