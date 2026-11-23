// Layout check for X/Client/Camera/Components/CameraComponent.h against the DWARF (byte_size
// 0x38, members at 0x08..0x24), and a replay of SpringFollowComponent::OnFirstTick
// (X/Client/Camera/Components/SpringFollowComponent.cpp:235, 0x71003e7c10), which inlines the
// in-class OnFirstTick and GetCentroidLocation. SpringFollowComponent is a stand-in that only
// places the WorldPlace the function writes (at 0x34, in the base's tail padding); its name
// is invented.
#include "X/Client/Camera/Components/CameraComponent.h"

#include <cstddef>

struct CameraComponentProbe : CameraComponent {
    static void Check() {
        static_assert(sizeof(CameraComponent) == 0x38, "CameraComponent");
        static_assert(offsetof(CameraComponentProbe, snoObserver) == 0x08, "snoObserver");
        static_assert(offsetof(CameraComponentProbe, bDisabled) == 0x0c, "bDisabled");
        static_assert(offsetof(CameraComponentProbe, bForceAllPlayersForCentroid) == 0x10, "bForceAllPlayersForCentroid");
        static_assert(offsetof(CameraComponentProbe, m_nActivePlayers) == 0x14, "m_nActivePlayers");
        static_assert(offsetof(CameraComponentProbe, m_nMovingPlayers) == 0x18, "m_nMovingPlayers");
    }
};

class SpringFollowComponent : public CameraComponent {
public:
    virtual void OnFirstTick();
private:
    WorldPlace m_wpLastCentroid;  // 0x34 (invented name)
};

void SpringFollowComponent::OnFirstTick()
{
    CameraComponent::OnFirstTick();
    m_wpLastCentroid = GetCentroidLocation();
}
