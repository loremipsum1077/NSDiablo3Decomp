// Original file: X/Client/UserInterface/Console/UIConsoleCinematics.cpp
// Local declarations stand in for headers not adopted yet; each names its defining file.
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../../../Common/GlobalSNO.h"
#include "../../CGame.h"
#include "ControllerActions.h"
#include "../../../Common/Definitions/HeroDefinition.h"
#include "../UIUtils.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundHeroManager.h"
#include "../../../Sigma/Math/Vector.h"
#include "../Controls/UIRActorControl.h"
#include "../../../Common/Player.h"
#include "../Controls/UICutsceneControl.h"
#include "../../../Common/Protobuf/Hero.pb.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIStoryboard.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../Console/GamerProfile.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Sigma/RefString.h"
#include "ConsoleScreenManager.h"
extern UIID UIWND_CONSOLE_CINEMATICS, UILIST_CONSOLE_CINEMATICS, UIWND_CONSOLE_CINEMATICS_ARROW_UP, UIWND_CONSOLE_CINEMATICS_ARROW_DOWN, UIVID_CONSOLE_CINEMATICS_PREVIEW, UIVID_CONSOLE_CINEMATICS_PREVIEW_WIDE;  // UIIDs.h
extern UIID UIRACTOR_CONSOLE_CINEMATICS_HIGHLIGHT, UITEXT_CONSOLE_CINEMATICS_TITLE, UITEXT_CONSOLE_CINEMATICS_SUMMARY, UISTORYBOARD_CONSOLE_CINEMATICS_TITLE_FADE;  // UIIDs.h
int32 CountSetBits(const uint8 * lpb, const int32 nLen);  // X/Sigma/BitManip.cpp:87
typedef void (*CutsceneCallback)(); void CutscenePlayActCutscene(const Act eAct, const BOOL fUseNoLogoVersion, CutsceneCallback pfnOnFinishedCallback, int32 nControllerIndex); void CutscenePlayFinalCinematic(const BOOL fExpansion, const PlayerClass ePlayerClass, const BOOL fIsMale, CutsceneCallback pfnOnFinishedCallback);  // X/Client/Cutscene.h
void CutscenePlayInterstitialCutscene(const Act eAct, const int32 nIndex, const PlayerClass ePlayerClass, const BOOL fIsMale, int32 nControllerIndex, CutsceneCallback pfnOnFinishedCallback); void CutsceneTryToCancel(const UIID &);  // X/Client/Cutscene.h
CRefString CutsceneGetActCutscenePath(Act eAct, BOOL fUseNoLogoVersion); CRefString CutsceneGetFinalCinematicPath(const BOOL fExpansionOutro, const PlayerClass ePlayerClass, const BOOL fIsMale); CRefString CutsceneGetInterstitialCutscenePath(int32 & nAudioTrack, const Act eAct, const int32 nIndex, const PlayerClass ePlayerClass, const BOOL fIsMale);  // X/Client/Cutscene.h
SNO GetActBackgroundSNO(Act eAct); void UIStartup_SetBackgroundWorld(SNO snoWorld);  // Battlenet/UI/Util/UIUtil.h, UIStartup.h
UIRActor & UI_GetRActorFromID(const UIID & id); UICutscene & UI_GetCutsceneFromID(const UIID & id); UIStoryboard & UI_GetStoryboardFromID(const UIID & id);  // UI.h
// Inferred: "(void)" parameter lists fill the line between each DWARF decl line and the body line; literals 100 (Act step, cf. ACT_2 = 100), 2 (female player flag),
// 4 (expansion act index) and action ids 0/1 have no named constant in this CU's DWARF; IndexToCinematic's loop shape follows its DWARF locations (nCurrentActCinematics decremented).

namespace UICinematics {
namespace Console {

        struct Cinematic
        {
            int32 nAct;
            int32 nCinematicIndex;
        };


        Cinematic IndexToCinematic(
            int32 nIndex)
        {
            uint32 nDataLen;
            const uint8 * pbPlayedCutscene = Prefs::GetPlayedCutScenes(nDataLen);
            int32 nTotalActCinematics = 0;
            for (uint32 i = 0; i < nDataLen; ++i)
            {
                int32 nCurrentActCinematics = CountSetBits(&pbPlayedCutscene[i], 1);
                if (nIndex < nTotalActCinematics + nCurrentActCinematics)
                {
                    Cinematic tCin;
                    tCin.nAct = i;
                    tCin.nCinematicIndex = 0;

                    int32 nTrueTotalActCinematics = nTotalActCinematics + nCurrentActCinematics;

                    for (uint32 j = 0; j < 8 && nIndex >= nTrueTotalActCinematics - nCurrentActCinematics; ++j)
                    {
                        if (pbPlayedCutscene[i] & (1 << j))
                        {
                            tCin.nCinematicIndex = j;
                            --nCurrentActCinematics;
                        }
                    }

                    return tCin;
                }

                nTotalActCinematics += nCurrentActCinematics;
            }



            Cinematic tEmpty;
            tEmpty.nAct = 0;
            tEmpty.nCinematicIndex = 0;
            return tEmpty;
        }


        CRefString GetCinematicTitle(
            const Cinematic & tCinematic)
        {
            CRefString sLabel;
            if (tCinematic.nAct == 3 && tCinematic.nCinematicIndex == 7)
            {
                sLabel.Format("Bnet_Cinematics:Cinematic_5");
            }
            else if (tCinematic.nAct == 4 && tCinematic.nCinematicIndex == 7)
            {
                sLabel.Format("Bnet_Cinematics:Cinematic_7");
            }
            else if (tCinematic.nCinematicIndex == 0)
            {
                int32 nIndex = (tCinematic.nAct == 4) ? 6 : tCinematic.nAct + 1;
                sLabel.Format("Bnet_Cinematics:Cinematic_%d", nIndex);
            }
            else
            {
                sLabel.Format("Bnet_Cinematics:Interstitial_Act%d_%02d", tCinematic.nAct + 1, tCinematic.nCinematicIndex - 1);
            }
            return LanguageRules::ApplyLanguageRules(StringListFetch(sLabel).str());
        }


        CRefString GetCinematicSummary(
            const Cinematic & tCinematic)
        {
            CRefString sLabel;
            if (tCinematic.nAct == 3 && tCinematic.nCinematicIndex == 7)
            {
                sLabel.Format("Bnet_Cinematics:Cinematic_5_Summary");
            }
            else if (tCinematic.nAct == 4 && tCinematic.nCinematicIndex == 7)
            {
                sLabel.Format("Bnet_Cinematics:Cinematic_7_Summary");
            }
            else if (tCinematic.nCinematicIndex == 0)
            {
                int32 nIndex = (tCinematic.nAct == 4) ? 6 : tCinematic.nAct + 1;
                sLabel.Format("Bnet_Cinematics:Cinematic_%d_Summary", nIndex);
            }
            else
            {
                sLabel.Format("Bnet_Cinematics:Interstitial_Act%d_%02d_Summary", tCinematic.nAct + 1, tCinematic.nCinematicIndex - 1);
            }
            return LanguageRules::ApplyLanguageRules(StringListFetch(sLabel).str());
        }


        class CinematicsDataSource : public ConsoleListBoxDataSource
        {
        public:
            CinematicsDataSource() {}


            virtual int32 GetNumItems(
                void) const
            {
                uint32 nDataLen;
                const uint8 * pbPlayedCutscene = Prefs::GetPlayedCutScenes(nDataLen);
                return CountSetBits(pbPlayedCutscene, nDataLen);
            }



            virtual UIControlSourceKey GetSourceKeyForItem(
                const int32) const
            {
                return UIControlSourceKey::Parse("ConsoleCinematics:ConsoleCinematics_main.menuItem", -1, NULL, NULL);
            }



            virtual void InitializeItemUI(
                const UIConsoleListBox &,
                const UIID &,
                const int32)
            {

            }



            virtual void UpdateItemUI(
                const UIConsoleListBox & tListBox,
                const UIID & idItemUI,
                const int32 nIndex)
            {
                UIID idText = UIIDCreateFromParent(idItemUI, ".text");

                Cinematic tCinematic = IndexToCinematic(nIndex);
                CRefString sLabel = GetCinematicTitle(tCinematic);
                BOOL bSelected = (tListBox.GetSelectedIndex() == nIndex);

                UIText & tText = UI_GetTextFromID(idText);
                tText.SetText(sLabel, FALSE);
                RGBAColor rgbaTextColor = bSelected ? crgbaWhite : crgbaGold;
                tText.SetTextColor(rgbaTextColor);

                if (bSelected)
                {
                    UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_CINEMATICS_HIGHLIGHT);


                    Vector2D vecFinalTL;
                    Rect2D rectFilagree = tFilagree.GetWindowRect();
                    UIWindow & tItem = UI_GetWindowFromID(idItemUI);
                    Rect2D rectItem = tItem.GetWindowRect();
                    AlignRect(
                        Vector2D(rectItem.left, rectItem.top),
                        Vector2D(rectItem.Width(), rectItem.Height()),
                        ALIGN_CENTER_VERTICAL,
                        rectFilagree.left,
                        Vector2D(rectFilagree.Width(), rectFilagree.Height()),
                        vecFinalTL);
                    tFilagree.MoveTo(vecFinalTL);
                }
            }
        };



        struct UICinematicsData
        {
            CinematicsDataSource tCinematicsData;
            BOOL bIsPlayingFullScreenCutscene;
        };



        UICinematicsData sgtCinematicsData;


        void RefreshHighlightRActor(
            void)
        {
            UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_CINEMATICS_HIGHLIGHT);
            PRTransform transform(cgqIdentity, cgwpNull);
            if (tFilagree.GetRActor() == RACTORID_INVALID)
            {
                RActorID idRActor = RActorCreate(
                    GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_MENU_FILAGREE),
                    1.0f,
                    -1.0f,
                    transform,
                    0,
                    tFilagree.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(),
                    -1);
                tFilagree.SetRActor(idRActor);
            }
        }


        void UpdateScrollArrows(
            void)
        {
            UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_CINEMATICS);
            UI_GetWindowFromID(UIWND_CONSOLE_CINEMATICS_ARROW_UP).SetActive(tListBox.GetFirstVisibleItemIndex() > 0);
            UI_GetWindowFromID(UIWND_CONSOLE_CINEMATICS_ARROW_DOWN).SetActive(tListBox.GetFirstVisibleItemIndex() + tListBox.GetNumVisibleItems() < tListBox.GetNumItems());
        }




















        void OpenForController(
            int32 nControllerIndex)
        {
            UI_GetWindowFromID(UIWND_CONSOLE_CINEMATICS).SetActive(TRUE);
            ::Console::Screens::GetFrontendBackgroundHeroManager().ClearAllBackgroundHeros();
            UIConsoleListBox & tCutsceneList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_CINEMATICS);
            tCutsceneList.SetDataSource(sgtCinematicsData.tCinematicsData);
            tCutsceneList.ResetAllItems();
            UpdateScrollArrows();
            UI_GetCutsceneFromID(UIVID_CONSOLE_CINEMATICS_PREVIEW).SetLooping(TRUE);
            UI_GetCutsceneFromID(UIVID_CONSOLE_CINEMATICS_PREVIEW_WIDE).SetLooping(TRUE);
            ::Console::Screens::GetFrontendBackgroundHeroManager().ClearBackgroundBanner();
            RefreshHighlightRActor();

            ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
            tActionManager.ClearAllActions();
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("Console:PlayCinematic"));
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("Console:Back"));
            ::Console::Screens::UpdateButtonLegendText();

            sgtCinematicsData.bIsPlayingFullScreenCutscene = FALSE;




        }



        void Close(
            void)
        {




            UI_GetWindowFromID(UIWND_CONSOLE_CINEMATICS).SetActive(FALSE);
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_CINEMATICS).SetSelectedIndex(-1);
            UI_GetCutsceneFromID(UIVID_CONSOLE_CINEMATICS_PREVIEW).Stop();
            UI_GetCutsceneFromID(UIVID_CONSOLE_CINEMATICS_PREVIEW_WIDE).Stop();

            UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_CINEMATICS_HIGHLIGHT);
            tFilagree.Cleanup();

            CutsceneTryToCancel(GetUIIDNone());
        }



        void OnUpdate(
            const UIID &)
        {
            UpdateScrollArrows();
        }



        void OnCutsceneFinished(
            void)
        {
            sgtCinematicsData.bIsPlayingFullScreenCutscene = FALSE;
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_CINEMATICS).ForceCallToSelectionChange();
        }



        void OnAcceptPressed(
            const UIID &,
            int32 nControllerIndex)
        {
            int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_CINEMATICS).GetSelectedIndex();
            if (nSelectedIndex != -1)
            {
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);

                UI_GetCutsceneFromID(UIVID_CONSOLE_CINEMATICS_PREVIEW).Stop();
                UI_GetCutsceneFromID(UIVID_CONSOLE_CINEMATICS_PREVIEW_WIDE).Stop();
                const D3::Hero::SavedDefinition & tHero = ::Console::GamerProfile::GetHero(UI_GetPrimaryUIController());

                BOOL bIsMale = !(tHero.digest().player_flags() & 2);
                Cinematic tCinematic = IndexToCinematic(nSelectedIndex);
                Act eAct = static_cast<Act>(tCinematic.nAct * 100);

                switch (tCinematic.nCinematicIndex)
                {
                case 0:
                    CutscenePlayActCutscene(eAct, FALSE, OnCutsceneFinished, nControllerIndex);
                    sgtCinematicsData.bIsPlayingFullScreenCutscene = TRUE;
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                    CutscenePlayInterstitialCutscene(
                        eAct,
                        tCinematic.nCinematicIndex - 1,
                        PlayerClassFromGBID(tHero.digest().gbid_class()),
                        bIsMale,
                        nControllerIndex,
                        OnCutsceneFinished);
                    sgtCinematicsData.bIsPlayingFullScreenCutscene = TRUE;
                    break;
                case 7:
                    CutscenePlayFinalCinematic(tCinematic.nAct == 4, PlayerClassFromGBID(tHero.digest().gbid_class()), bIsMale, OnCutsceneFinished);
                    sgtCinematicsData.bIsPlayingFullScreenCutscene = TRUE;
                    break;
                default:
                    break;
                }
            }
        }



        void OnCancelPressed(
            const UIID & id,
            int32 nControllerIndex)
        {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);

            ::Console::Screens::GoToPreviousScreenSet(nControllerIndex, FALSE);
        }


        // NON_MATCHING: 960 vs 964 bytes; only difference: ours keeps the GOT address of UIVID_CONSOLE_CINEMATICS_PREVIEW_WIDE in x19 from the line-462 assignment, the original reloads it (adrp+ldr) for the second Stop(); everything else identical.
        void OnCutsceneListChanged(
            const UIID & id)
        {
            if (id == GetUIIDNone())
            {
                return;
            }

            UIConsoleListBox & tCutsceneList = UI_GetConsoleListBoxFromID(id);
            int32 nSelectedIndex = tCutsceneList.GetSelectedIndex();
            if (nSelectedIndex != -1)
            {
                const D3::Hero::SavedDefinition & tHero = ::Console::GamerProfile::GetHero(UI_GetPrimaryUIController());

                BOOL bIsMale = !(tHero.digest().player_flags() & 2);
                Cinematic tCinematic = IndexToCinematic(nSelectedIndex);
                LanguageRules::SetClientGender(bIsMale);

                CRefString sCutscenePath;
                Act eAct = static_cast<Act>(tCinematic.nAct * 100);

                SNO snoWorld = GetActBackgroundSNO(eAct);
                UIStartup_SetBackgroundWorld(snoWorld);
                RefreshHighlightRActor();

                PlayerClass ePlayerClass = PlayerClassFromGBID(tHero.digest().gbid_class());


                switch (tCinematic.nCinematicIndex)
                {
                case 0:
                    sCutscenePath = CutsceneGetActCutscenePath(eAct, TRUE);
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                    {
                        int32 nAudioTrackIgnored = 0;
                        sCutscenePath = CutsceneGetInterstitialCutscenePath(nAudioTrackIgnored, eAct, tCinematic.nCinematicIndex - 1, ePlayerClass, bIsMale);
                    }
                    break;





                case 7:
                    sCutscenePath = CutsceneGetFinalCinematicPath(tCinematic.nAct == 4, ePlayerClass, bIsMale);
                    break;
                }

                UIID idPrimaryCutscene;
                // Lines 456-461 leave no code; the original assigns the wide preview on line 462
                // (inlined UIID::operator= at 462 in the DWARF).




                idPrimaryCutscene = UIVID_CONSOLE_CINEMATICS_PREVIEW_WIDE;


                UI_GetCutsceneFromID(UIVID_CONSOLE_CINEMATICS_PREVIEW).Stop();
                UI_GetCutsceneFromID(UIVID_CONSOLE_CINEMATICS_PREVIEW_WIDE).Stop();
                UI_GetCutsceneFromID(idPrimaryCutscene).Play(sCutscenePath, TRUE);

                UI_GetTextFromID(UITEXT_CONSOLE_CINEMATICS_TITLE).SetText(
                    GetCinematicTitle(tCinematic), FALSE);

                UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_CINEMATICS_TITLE_FADE).Stop();
                UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_CINEMATICS_TITLE_FADE).Play(FALSE, 0.0f);

                UI_GetTextFromID(UITEXT_CONSOLE_CINEMATICS_SUMMARY).SetText(
                    GetCinematicSummary(tCinematic), FALSE);
            }
        }

}  // namespace Console
}  // namespace UICinematics
