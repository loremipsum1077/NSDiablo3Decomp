// Original file: X/Common/PortalCommon.cpp

#include "PortalCommon.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Server/Actors/Actors.h"
#include "../Server/SGame.h"
#include "../Server/SGameManager.h"
#include "../Server/SPlayer.h"
#include "../Server/SWorld.h"
#include "ActorCommonData.h"
#include "Definitions/ActDefinition.h"
#include "Game.h"
#include "GameCommonData.h"
#include "GameTemplate.h"
#include "Player.h"
#include "World.h"
#include <new>
#include <stddef.h>

struct PortalGlobals {  // size 0x30
    XChainedFixedBlockAllocator<ResolvedPortalDestination> poolPortalSpecifier;  // 0x00
};

enum {
    MAX_PORTAL_SPECIFIERS = 64,
};

// Declared here because their headers do not carry them yet.
void SGameACDChangeWorld(const ACDID idACD, const SNO snoWorld, const uint32 uEntranceGUID, PlayerWarpReason eReason, const float flWarpFadeInSecs, const Player * ptReferencePlayer);  // X/Server/SGame.cpp:7358
void SGameGetOpenWorldHubWorldForPlayer(const Player & tPlayer, ResolvedPortalDestination & tDest);  // X/Server/SGame.cpp:6756
void ActGetCurrentStartLocation(ResolvedPortalDestination & tDest);  // X/Common/Act.cpp:260
SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);  // X/Common/World.cpp:605












static const TypeDescriptorField PortalSpecifierMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PortalSpecifierMessage, tHeader), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(PortalSpecifierMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_RESOLVED_PORTAL_DESTINATION, offsetof(PortalSpecifierMessage, tResolvedDest), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(PortalSpecifierMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PortalSpecifierMessage("PortalSpecifierMessage", 0, PortalSpecifierMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PORTAL_SPECIFIER_MESSAGE = &TypeDescriptor_PortalSpecifierMessage;





void PortalsInitializeForGame()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();

    XMemoryAllocatorInterface * ptMemInterface = AppGetCurrentMemoryInterface();

    tGCD.ptPortalGlobals = (PortalGlobals *)SigmaMemoryNew(sizeof(PortalGlobals), 4, ptMemInterface, TRUE);
    tGCD.ptPortalGlobals->poolPortalSpecifier.Initialize();

    tGCD.ptPortalGlobals->poolPortalSpecifier.SetMemoryInterface(ptMemInterface);
    tGCD.ptPortalGlobals->poolPortalSpecifier.SetBlockCount_Tracked(MAX_PORTAL_SPECIFIERS);
}




void PortalsDisposeForGame()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();

    XMemoryAllocatorInterface * ptMemInterface = AppGetCurrentMemoryInterface();

    tGCD.ptPortalGlobals->~PortalGlobals();

    SigmaMemoryFree(tGCD.ptPortalGlobals, ptMemInterface); tGCD.ptPortalGlobals = NULL;
}




static ResolvedPortalDestination * sAllocPortalSpecifier()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ResolvedPortalDestination * ptPortalSpecifier = tGCD.ptPortalGlobals->poolPortalSpecifier.Alloc_Tracked();
    *ptPortalSpecifier = ResolvedPortalDestination();
    return ptPortalSpecifier;
}



static void sFreePortalSpecifier(ResolvedPortalDestination *& ptPortalSpecifier)
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (ptPortalSpecifier)
    {
        tGCD.ptPortalGlobals->poolPortalSpecifier.Free(ptPortalSpecifier);



        ptPortalSpecifier = NULL;
    }
}





void PortalSpecifierDispose(ACDID idACD)
{
    ActorCommonData & tACD = ACDGet(idACD);


    if (tACD.ptResolvedDest)
    {
        sFreePortalSpecifier(tACD.ptResolvedDest);
    }
}





void CPortalSpecifierInitialize(ACDID idACD, const ResolvedPortalDestination & tSpecifier)

{
    ActorCommonData & tACD = ACDGet(idACD);

    sFreePortalSpecifier(tACD.ptResolvedDest);
    tACD.ptResolvedDest = sAllocPortalSpecifier();

    SigmaMemoryCopy(tACD.ptResolvedDest, &tSpecifier, sizeof(ResolvedPortalDestination));
}







void SPortalSpecifierInitialize(Actor * ptActor, const ResolvedPortalDestination & tDest)

{










    if (tDest.snoWorld == SNO_NONE)
    {
        return;
    }



    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    tACD.ptResolvedDest = sAllocPortalSpecifier();
    SigmaMemoryCopy(tACD.ptResolvedDest, &tDest, sizeof(ResolvedPortalDestination));

    tACD.ModifiedMark((ACDModifiedType)7);
}




void SPortalMovePlayerToTeamSpawnPoint(const Player & tPlayer)

{

    SWorld * ptSWorld = SWorldGet(ACDGet(tPlayer.idPlayerACD).GetLocation().tWorldLocation.idSWorld);

    SGameACDChangeWorld(tPlayer.idPlayerACD, ptSWorld->tWCD.snoWorld, 0, 0, 0.0f, NULL);
}




void SPortalMovePlayerToActSpawnPoint(const Player & tPlayer)

{
    ResolvedPortalDestination tDest;


    if (GameIsOpenWorld())
    {
        SGameGetOpenWorldHubWorldForPlayer(tPlayer, tDest);
    }
    else if (GameIsStoryMode())
    {
        ActGetCurrentStartLocation(tDest);
    }
    else
    {
        tDest.snoWorld = GetSNOWorldFromGameType(GameGetGameType());
        tDest.uEntranceGUID = (uint32)-1;
    }






    if (WorldCommonFindBySNO(tDest.snoWorld, TRUE) == SWORLDID_INVALID)
    {
        SWorldCreate(tDest.snoWorld, TRUE, SPlayerIsLocal(tPlayer), NULL, ACDID_INVALID, ACDID_INVALID, tDest.uEntranceGUID, 0, FALSE, 0);
    }



    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayersAreLocalToEachOther(tPlayer, *ptPlayer))
        {
            SGameACDChangeWorld(ptPlayer->idPlayerACD, tDest.snoWorld, tDest.uEntranceGUID, 0, 0.0f, NULL);
        }
    }
}
