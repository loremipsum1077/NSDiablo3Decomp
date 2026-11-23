#include "Hub.h"

// X/Client/CGame.h:145. Only the member read here is declared; the rest is not adopted.
class UIGlobals;
class CGame {
public:
    uint8 _unadopted[0xb80];
    UIGlobals * ptUIGlobals;  // 0xb80
};
#include "../../Common/Game.h"




extern AppGlobals sgtAppGlobals;
// X/Client/CGame.h:773.
inline BOOL CGameIsValid()
{
    return sgtAppGlobals.ptCGameActive != NULL;
}

// X/Client/Battlenet/Subsystems/ScreenManager.h:66, LightBoxManager.h:15 and
// ModalNotificationManager.h:71/:114. Only the sizes and the members called here are declared.
class ScreenManager {  // size 0x498
public:
    ScreenManager();
    ~ScreenManager();
private:
    uint8 _unadopted[0x498];
};
#include "Subsystems/LightBoxManager.h"






class ModalNotificationManagerBase {  // size 0x70
public:
    ModalNotificationManagerBase();
    virtual ~ModalNotificationManagerBase();
private:
    uint8 _unadopted[0x68];
};
class ModalNotificationManager : public ModalNotificationManagerBase {  // size 0x70
    friend class Hub;  // not in the DWARF; Hub creates and destroys the private instance
private:
    ModalNotificationManager() {}
    virtual ~ModalNotificationManager();
};

static Hub * sgpHub;

void Hub::Initialize()
{
    m_pModalNotificationManager = new ModalNotificationManager;
    m_pContextMenuManager = NULL;















    m_pToastManager = NULL;
    m_pFanfareManager = NULL;
    m_pClientService = NULL;
    m_pGameIconManager = NULL;
    m_pSimpleCheckoutManager = NULL;


    m_pScreenManager = new ScreenManager;
    m_pLightBoxManager = new LightBoxManager;
}




Hub::~Hub()
{
    // Global delete: the original calls the virtual complete-object destructor and then
    // ::operator delete on the most-derived pointer, not the deleting destructor.










    ::delete m_pLightBoxManager;
    ::delete m_pScreenManager;
    ::delete m_pModalNotificationManager;
}





Hub & HubGet()
{
    return *sgpHub;
}



BOOL Hub::IsUIInitialized() const
{

    if (CGameIsValid())
        return sgtAppGlobals.ptCGameActive->ptUIGlobals != NULL;
    return FALSE;
}

void Hub::InitializeForApp()
{


    sgpHub = new Hub;
    sgpHub->Initialize();
}

void Hub::DisposeForApp()
{

    delete sgpHub;
    sgpHub = NULL;
}

void Hub::Process()
{
}
