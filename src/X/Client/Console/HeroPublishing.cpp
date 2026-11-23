// Original file: X/Client/Console/HeroPublishing.cpp

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../Common/Protobuf/Account.pb.h"
#include "../../Common/Protobuf/Hero.pb.h"
#include "Online.h"
#include <utility>

// Declared here because their headers do not carry them yet.
namespace Console { namespace Online {
BOOL LobbyIsConnected(int32 nUserIndex);  // X/Client/Console/LobbyService.cpp
void StoragePublishUserFile(int32 nUserIndex, const char * szContext, const char * szFilename, blz::string szData, StoragePublishUserFileCallback pfnCallback);  // X/Client/Console/LobbyService.cpp
} }



static const Blizzard::Time::Millisecond MINIMUM_PUBLISH_INTERVAL = 10 * 60 * 1000;

namespace Console { namespace HeroPublishing {
void sPublishProtobuf(int32 nControllerIndex, const char * szFilename, const google::protobuf::MessageLite & tMessage);







Blizzard::Time::Millisecond suPublishInterval;
Blizzard::Time::Millisecond suLastPublishTime[4];






void InitializeForGame()

{



    for (int32 i = 0; i < 4; i++)
    {
        suLastPublishTime[i] = Blizzard::Time::Milliseconds();
    }
}


void DisposeForGame()
{

}
} }



void sPublishCallback(Console::Online::StorageResult eResult)
{











} namespace Console { namespace HeroPublishing {

void sPublishProtobuf(int32 nControllerIndex,
                      const char * szFilename,
                      const google::protobuf::MessageLite & tMessage)

{
    blz::string sData;

    if (tMessage.SerializeToString(&sData))
    {




        Console::Online::StoragePublishUserFile(nControllerIndex, NULL, szFilename, std::move(sData), sPublishCallback);
    }
}



void Publish(int32 nControllerIndex,
             D3::Account::SavedDefinition & tAccountDefinition,
             D3::Hero::SavedDefinition & tHeroDefinition,
             BOOL bForcePublish)

{

    if (nControllerIndex < 0 || suPublishInterval < MINIMUM_PUBLISH_INTERVAL || nControllerIndex >= sizeof(suLastPublishTime) / sizeof(suLastPublishTime[0]))
        return;
    if (!Console::Online::LobbyIsConnected(nControllerIndex))
        return;
    Blizzard::Time::Millisecond & uLastPublishTime = suLastPublishTime[nControllerIndex];



    Blizzard::Time::Millisecond now = Blizzard::Time::Milliseconds();
    if (bForcePublish ||
        Blizzard::Time::GetTimeElapsed(uLastPublishTime, now) >= suPublishInterval)
    {
        sPublishProtobuf(nControllerIndex, "account", tAccountDefinition);

        char szFilename[32];
        Blizzard::String::Format(szFilename, sizeof(szFilename), "hero-%016llx", tHeroDefinition.digest().hero_id().id_low());
        sPublishProtobuf(nControllerIndex, szFilename, tHeroDefinition);

        suLastPublishTime[nControllerIndex] = now;
    }
}




void SetPublishIntervalInMinutes(int32 nPublishInterval)

{
    Blizzard::Time::Millisecond uNewTime = nPublishInterval * 60 * 1000;
    if (uNewTime < MINIMUM_PUBLISH_INTERVAL)
    {
        uNewTime = 0;
    }

    suPublishInterval = uNewTime;
}
} }
