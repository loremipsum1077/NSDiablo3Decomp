// Original file: X/Common/Map/MinimapCommon.cpp
//
// The minimap icon resolver shared by the client and the server.

#include "../ActorCommon.h"
#include "../ActorCommonData.h"
#include "../Attributes/FastAttributeStructs.h"
#include "../Definitions/ActorDefinition.h"
#include "../Game.h"
#include "../GizmoSwig.h"
#include "../SNO/SNOFiles.h"
#include "../TagMaps/ActorTags.h"
#include "../TagMaps/MarkerTags.h"
#include "../TagMaps/Tags.h"
#include "../../Client/Gfx/GfxCommon.h"
#include "../../Sigma/RefString.h"

// Declared locally until their headers are adopted; each comment names the file that defines
// the function or type.
typedef int32 CrafterType;
enum {
    CRAFTER_TYPE_INVALID = -1,  // X/Common/Crafting/ItemCraftingSwig.h:9
};
CrafterType NPCGetCrafterType(const ActorCommonData & tACD);  // X/Common/NPC.cpp:355

typedef int32 ConversationIcon;
enum {
    CONV_ICON_INVALID = -1,  // X/Common/Definitions/ConversationDefinition.h:182
    CONV_ICON_NONE = 0,
    CONV_ICON_EXCLAMATION = 1,
    CONV_ICON_QUESTION = 2,
    CONV_ICON_ASTERISK = 3,
    CONV_ICON_GOSSIP = 4,
    CONV_ICON_EXCLAMATION_BONUS = 5,
    NUM_CONVERSATION_ICONS = 6,
};

FastAttribKey MakeAttribKey(Attrib eAttrib, int32 nParam);  // X/Common/Attributes/FastAttribute.cpp:2073
BOOL GameIsOpenWorld();  // X/Common/GameCommonData.cpp:311

namespace Minimap {

void ResolveIconForActor(const ActorCommonData & tACD, const SNO snoActor, const MinimapMarkerSetInfo & tMarkerOwner,
                         const int32 nPlayerIndex, SNO & snoRet, CRefString & szRet)
{
    snoRet = SNO_NONE;
    if ((tACD.dwFlags & (1 << ACD_QUEST_OBJECTIVE)) || tACD.AttributesGetInt(ATTRIBUTE_QUEST_MONSTER)) {
        ImageTextureFrame tImageTexFrame;
        if (tACD.IsMonster() && tACD.IsAlive()) {
            if (GameIsOpenWorld()) {
                szRet = "MarkerHostilePlayer";
            } else {
                szRet = "MarkerHostileMonster";
            }
        } else if (tACD.IsGizmo()) {
            szRet = "MarkerQuestDot";
        }
        return;
    }

    BOOL bTryFallBackToBaseIcon = NPCGetCrafterType(tACD) != CRAFTER_TYPE_INVALID;
    if (tACD.IsNPC() && nPlayerIndex != -1) {
        ConversationIcon eConvIcon = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_CONVERSATION_ICON, nPlayerIndex));
        ImageTextureFrame tImageTexFrame;
        switch (eConvIcon) {
        case CONV_ICON_EXCLAMATION:
            szRet = "MarkerExclamation";
            bTryFallBackToBaseIcon = FALSE;
            break;
        case CONV_ICON_EXCLAMATION_BONUS:
            szRet = "MarkerExclamation_Blue";
            bTryFallBackToBaseIcon = FALSE;
            break;
        case CONV_ICON_QUESTION:
            szRet = "MarkerQuestion";
            bTryFallBackToBaseIcon = FALSE;
            break;
        case CONV_ICON_ASTERISK:
            szRet = "MarkerAsterisk";
            break;
        }
        if (szRet != "" && !bTryFallBackToBaseIcon) {
            return;
        }
    }

    snoRet = tACD.AttributesGetInt(ATTRIBUTE_MINIMAP_ICON_OVERRIDE);
    if (!SNOFileExists(TEXTURE_SNO, snoRet)) {
        if (tACD.IsGizmo() && tACD.GetGizmoType() == ACTORGIZMO_MAILBOX && GameIsSeasonal()) {
            return;
        }
        if (tACD.IsGizmo() && !tACD.IsOperatableGizmo(ACDID_INVALID, TRUE)) {
            ScopedActorDefinitionLockR pdefActor(snoActor);
            snoRet = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_ICON_GIZMO_USED);
        }
        if (snoRet == SNO_NONE) {
            if (GameIsOpenWorld()) {
                ScopedActorDefinitionLockR pdefActor(snoActor);
                snoRet = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_ICON_OPENWORLD);
            }
            if (snoRet == SNO_NONE) {
                snoRet = ActorGetInheritedTagMapValueSNO(snoActor, tMarkerOwner, TAG_ACTOR_MINIMAP_ICON,
                                                         TAG_MARKER_MINIMAP_ICON);
            }
        }
    }
    if (snoRet != SNO_NONE) {
        szRet = "";
    }
}

}  // namespace Minimap
