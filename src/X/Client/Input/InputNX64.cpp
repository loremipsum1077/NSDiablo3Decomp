// Original file: X/Client/Input/InputNX64.cpp
// All 40 functions are written; 39 match. The nn::util::BitFlagSet / nn::TimeSpanType /
// ControllerSupportArg::SetDefault inline bodies and the nn::hid declarations below are
// reconstructed from the line table and import table (sUpdateNpad, InputPlatformUpdate and
// sCSAThreadProc match with them).
// AdjustUIButtonIconForSwitch is NON_MATCHING: same size and control flow, register use and block
// order differ in the reverse-remap tail. The 15-iteration bound of sReverseRemapSingleJoyconButton
// (the original checks NPAD_SINGLEJOY_REMAP[0..14] only) is written as MAX_BUTTON - 1; the
// original spelling is unknown.
// Bootstrap recovers the controller map assignments from InputGfxInit.


#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../NintendoSDK/Include/nn/os/os_SystemEventTypes.h"
#include "../../../NintendoSDK/Include/nn/nn_Result.h"
#include "../../../NintendoSDK/Include/nn/os/os_SystemEvent.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_minmax.h"
#include "../Console/GamerProfile.h"
#include "../Console/GamerProfileInternal.h"
#include "../CGame.h"
#include "../CPlayer.h"
#include <math.h>
#include "Input.h"
#include "../UserInterface/UI.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Sigma/Math/MathGeneral.h"
#include <strings.h>
#include "../Gfx/GfxVB.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/Unicode.h"
#include <stddef.h>
#include <stdint.h>
#include <type_traits>

// The nn::util / nn::hid SDK headers are not in src/ yet; the types below are copied from the
// DWARF of nn/util/util_BitFlagSet.h and nn/hid/hid_*.h (data members only).
namespace nn { namespace util {
struct Float3 { float x, y, z; };  // nn/util/util_MathTypes.h
namespace detail {  // bodies reconstructed from the line table (util_BitFlagSet.h:26, 44, 66)
template <typename T> inline void AndEqual( T * dest, const T * source, int count ) { for ( int i = 0; i < count; i++ ) dest[i] &= source[i]; }
template <typename T> inline void OrEqual( T * dest, const T * source, int count ) { for ( int i = 0; i < count; i++ ) dest[i] |= source[i]; }
template <typename T> inline bool IsAnyOn( const T * _storage, int count ) { for ( int i = 0; i < count; i++ ) { if ( _storage[i] ) return true; } return false; }
}
template <int N, typename Tag> struct BitFlagSet {
    typedef typename std::conditional<( N <= 32 ), uint32_t, uint64_t>::type StorageT;
    static const int StorageBitCount = sizeof( StorageT ) * 8;
    static const int StorageCount = ( N + StorageBitCount - 1 ) / StorageBitCount;
    StorageT _storage[StorageCount];
    template <int BitIndex> struct Flag {
        static const int Index = BitIndex;
        static const BitFlagSet Mask;
        static const int StorageIndex = BitIndex / StorageBitCount;
        static const StorageT StorageMask = static_cast<StorageT>( 1 ) << ( BitIndex % StorageBitCount );
    };
    BitFlagSet operator&( const BitFlagSet & other ) const { BitFlagSet value = *this; value &= other; return value; }
    BitFlagSet operator|( const BitFlagSet & other ) const { BitFlagSet value = *this; value |= other; return value; }
    BitFlagSet & operator&=( const BitFlagSet & other ) { detail::AndEqual( _storage, other._storage, StorageCount ); return *this; }
    BitFlagSet & operator|=( const BitFlagSet & other ) { detail::OrEqual( _storage, other._storage, StorageCount ); return *this; }
    bool IsAnyOn() const { return detail::IsAnyOn( _storage, StorageCount ); }
    template <typename FlagT> BitFlagSet & Set( bool isOn = true ) { return SetImpl( FlagT::StorageIndex, FlagT::StorageMask, isOn ); }
    template <typename FlagT> bool Test() const { return TestImpl( FlagT::StorageIndex, FlagT::StorageMask ); }
    bool Test( int index ) const { return TestImpl( index / StorageBitCount, static_cast<StorageT>( 1 ) << ( index % StorageBitCount ) ); }
    BitFlagSet & SetImpl( int storageIndex, StorageT storageMask, bool isOn ) { if ( isOn ) { _storage[storageIndex] |= storageMask; } else { _storage[storageIndex] &= ~storageMask; } return *this; }
    bool TestImpl( int storageIndex, StorageT storageMask ) const { return ( _storage[storageIndex] & storageMask ) != 0; }
};
// Every set used here has one storage word.
template <int N, typename Tag> template <int BitIndex> const BitFlagSet<N, Tag> BitFlagSet<N, Tag>::Flag<BitIndex>::Mask = { { static_cast<typename BitFlagSet<N, Tag>::StorageT>( 1 ) << BitIndex } };
struct Unorm8x4 { uint8_t v[4]; };  // nn/util/util_MathTypes.h
typedef Unorm8x4 Color4u8Type;
} }
namespace nn {
struct TimeSpanType {  // nn/nn_TimeSpan.h; inline bodies (defined below) reconstructed from the line table (201-203, 402-429) and the DWARF locals
    int64_t _nanoSeconds;
    int64_t GetMilliSeconds() const { return DivideNanoSeconds( _nanoSeconds, INT64_C( 0x431BDE82D7B634DB ), 18 ); }
    static int64_t DivideNanoSeconds( const int64_t nanoSeconds, const int64_t magic, const int32_t rightShift );
    static int64_t MultiplyRightShift( const int64_t x, const int64_t y );
};
namespace hid {
typedef uint32_t NpadIdType;
struct NpadButton {};
typedef nn::util::BitFlagSet<64, NpadButton> NpadButtonSet;
struct NpadAttribute {};
typedef nn::util::BitFlagSet<32, NpadAttribute> NpadAttributesSet;
struct NpadStyleTag {};
typedef nn::util::BitFlagSet<32, NpadStyleTag> NpadStyleSet;
struct AnalogStickState {  // size 0x8
    int32_t x;  // 0x00
    int32_t y;  // 0x04
};
struct DebugPadButton {};
struct DebugPadAttribute {};
typedef nn::util::BitFlagSet<32, DebugPadButton> DebugPadButtonSet;
typedef nn::util::BitFlagSet<32, DebugPadAttribute> DebugPadAttributeSet;
struct DebugPadState {  // size 0x20
    int64_t samplingNumber;  // 0x00
    DebugPadAttributeSet attributes;  // 0x08
    DebugPadButtonSet buttons;  // 0x0c
    AnalogStickState analogStickR;  // 0x10
    AnalogStickState analogStickL;  // 0x18
};
struct SixAxisSensorHandle {  // size 0x4
    int32_t _storage;  // 0x00
};
struct DirectionState {  // size 0x24
    nn::util::Float3 x;  // 0x00
    nn::util::Float3 y;  // 0x0c
    nn::util::Float3 z;  // 0x18
};
struct SixAxisSensorAttribute {};
typedef nn::util::BitFlagSet<32, SixAxisSensorAttribute> SixAxisSensorAttributeSet;
struct SixAxisSensorState {  // size 0x60
    nn::TimeSpanType deltaTime;  // 0x00
    int64_t samplingNumber;  // 0x08
    nn::util::Float3 acceleration;  // 0x10
    nn::util::Float3 angularVelocity;  // 0x1c
    nn::util::Float3 angle;  // 0x28
    DirectionState direction;  // 0x34
    SixAxisSensorAttributeSet attributes;  // 0x58
};
struct VibrationDeviceHandle {  // size 0x4
    uint32_t _storage;  // 0x00
};
enum VibrationDeviceType {
    VibrationDeviceType_Unknown = 0,
    VibrationDeviceType_LinearResonantActuator = 1,
};
enum VibrationDevicePosition {
    VibrationDevicePosition_None = 0,
    VibrationDevicePosition_Left = 1,
    VibrationDevicePosition_Right = 2,
};
struct VibrationDeviceInfo {  // size 0x8
    VibrationDeviceType deviceType;  // 0x00
    VibrationDevicePosition position;  // 0x04
};
struct VibrationValue {  // size 0x10 (nn/hid/hid_VibrationValueTypes.h)
    float amplitudeLow;  // 0x00
    float frequencyLow;  // 0x04
    float amplitudeHigh;  // 0x08
    float frequencyHigh;  // 0x0c
};
void SendVibrationValue( const VibrationDeviceHandle &, const VibrationValue & );
} }
extern const int32 INPUT_WHEEL_DELTA = 120;  // line 50
// X/Client/Input/InputNX64.h
enum NpadType {
    TYPE_NONE = 0,
    TYPE_HANDHELD = 1,
    TYPE_FULLKEY = 2,
    TYPE_JOYDUAL = 3,
    TYPE_JOYLEFT = 4,
    TYPE_JOYRIGHT = 5,
    TYPE_INVALID = 6,
};

enum MapType {
    MAP_GAME = 0,
    MAP_UI = 1,
    MAP_MAX = 2,
};











struct NpadButtonMap {  // size 0x38
    nn::hid::NpadButtonSet tNpadMask;  // 0x00
    nn::hid::NpadButtonSet tNpadTest;  // 0x08
    uint32 uButtonIndex;  // 0x10
    LPCSTR sIconName;  // 0x18
    CRefString sIconString;  // 0x20
};
static_assert(sizeof(NpadButtonMap) == 0x38, "NpadButtonMap");

struct Npad {  // size 0x3180
    typedef nn::hid::NpadIdType Id;
    typedef nn::hid::NpadStyleSet Style;
    typedef nn::os::SystemEventType StyleSetUpdateEventType;
    struct Gesture {  // size 0x1
    };
    typedef nn::util::BitFlagSet<32, Npad::Gesture> GestureSet;
    struct State {  // size 0x30
        nn::hid::AnalogStickState analogStickL;  // 0x00
        nn::hid::AnalogStickState analogStickR;  // 0x08
        nn::hid::NpadAttributesSet attributes;  // 0x10
        nn::hid::NpadButtonSet buttons;  // 0x18
        int64_t samplingNumber;  // 0x20
        Npad::GestureSet gestures;  // 0x28
    };
    struct Vibrator {  // size 0xc
        nn::hid::VibrationDeviceHandle tHandle;  // 0x00
        nn::hid::VibrationDeviceInfo tInfo;  // 0x04
    };
    struct SixAxisState {  // size 0x30e8
        struct GestureState {  // size 0x18
            int64_t anLastProcessedSamplingNumber[1];  // 0x00
            int64_t anTimeSinceLastActivation[1];  // 0x08
            int32_t anActiveSampleCount[1];  // 0x10
        };
        int32_t nHandleCount;  // 0x00
        nn::hid::SixAxisSensorHandle atHandles[8];  // 0x04
        nn::hid::SixAxisSensorState atSensorStates[8][16];  // 0x28
        Npad::SixAxisState::GestureState atGestureStates[8];  // 0x3028
    };
    Npad::Id uId;  // 0x00
    bool bConnected;  // 0x04
    bool bWasConnected;  // 0x05
    int32_t nDisconnectDelayFrames;  // 0x08
    Npad::Style tStyle;  // 0x0c
    NpadType eType;  // 0x10
    Npad::State tState;  // 0x18
    Npad::StyleSetUpdateEventType tStyleSetUpdateEvent;  // 0x48
    static const uint32 MAX_VIBRATORS = 2;
    Npad::Vibrator atVibrator[2];  // 0x78
    uint32 uVibratorCount;  // 0x90
    Npad::SixAxisState tSixAxis;  // 0x98
};
static_assert(sizeof(Npad) == 0x3180, "Npad");

struct DebugPad {  // size 0x28
    nn::hid::DebugPadState tState;  // 0x00
    bool bIsInitialized;  // 0x20
};
static_assert(sizeof(DebugPad) == 0x28, "DebugPad");

struct InputNX64Globals {  // size 0xf7c0
    Blizzard::Time::Second tLastButtonActivityTime;  // 0x00
    Npad tNpadHandheld;  // 0x08
    int32_t nPrimaryPlayerControllerIndex;  // 0x3188
    int32_t nUpdatingControllerType;  // 0x318c
    Npad atNpad[4];  // 0x3190
    DebugPad tDebugPad;  // 0xf790
    bool bWasControllerAppShown;  // 0xf7b8
    bool bProgramHasResumed;  // 0xf7b9
};
static_assert(sizeof(InputNX64Globals) == 0xf7c0, "InputNX64Globals");

struct CSAGlobals {  // size 0x28
    Blizzard::Thread::Thread tThread;  // 0x00
    Blizzard::Lock::Event tShowEvent;  // 0x08
    Blizzard::Lock::Event tDoneEvent;  // 0x14
    bool bActive;  // 0x20
    bool bAlive;  // 0x21
    bool bSingleMode;  // 0x22
};
static_assert(sizeof(CSAGlobals) == 0x28, "CSAGlobals");
extern CSAGlobals sCSAGlobals;

void InputClearRemappings();
namespace ControllerDisconnectedHandler { namespace Console { void ControllerDisconnected( int32 nControllerIndex ); } }  // ControllerDisconnectedHandler.h

static void sBuildButtonMaps();
BOOL InputAreInHandheldMode();
Npad * InputGetNpad( int32_t nControllerIndex );

static void sCSADispose();
inline int64_t nn::TimeSpanType::DivideNanoSeconds( const int64_t nanoSeconds, const int64_t magic, const int32_t rightShift )
{
    int64_t n = MultiplyRightShift( nanoSeconds, magic );
    if ( magic < 0 )
    {
        n += nanoSeconds;
    }
    n >>= rightShift;
    return n + ( static_cast<uint64_t>( nanoSeconds ) >> 63 );
}
inline int64_t nn::TimeSpanType::MultiplyRightShift( const int64_t x, const int64_t y )
{
    const uint64_t x_lo = x & 0xffffffff;
    const int64_t x_hi = x >> 32;

    const uint64_t y_lo = y & 0xffffffff;
    const int64_t y_hi = y >> 32;

    const int64_t z = x_hi * y_lo + ( ( x_lo * y_lo ) >> 32 );
    const int64_t z_lo = z & 0xffffffff;
    const int64_t z_hi = z >> 32;

    return x_hi * y_hi + z_hi + ( static_cast<int64_t>( x_lo * y_hi + z_lo ) >> 32 );
}
namespace nn { namespace hid {
// nn/hid/hid_NpadCommonTypes.h, hid_Npad*.h, hid_SixAxisSensor.h, hid_VibrationDeviceApi.h, hid_ControllerSupport*.h:
// declarations from the import table of the image; state structs and enumerators from the DWARF.
typedef NpadStyleSet::Flag<0> NpadStyleFullKey;
typedef NpadStyleSet::Flag<1> NpadStyleHandheld;
typedef NpadStyleSet::Flag<2> NpadStyleJoyDual;
typedef NpadStyleSet::Flag<3> NpadStyleJoyLeft;
typedef NpadStyleSet::Flag<4> NpadStyleJoyRight;
enum NpadJoyHoldType { NpadJoyHoldType_Vertical = 0, NpadJoyHoldType_Horizontal = 1 };
struct NpadFullKeyState { int64_t samplingNumber; NpadButtonSet buttons; AnalogStickState analogStickL; AnalogStickState analogStickR; NpadAttributesSet attributes; };
struct NpadHandheldState { int64_t samplingNumber; NpadButtonSet buttons; AnalogStickState analogStickL; AnalogStickState analogStickR; NpadAttributesSet attributes; };
struct NpadJoyDualState { int64_t samplingNumber; NpadButtonSet buttons; AnalogStickState analogStickL; AnalogStickState analogStickR; NpadAttributesSet attributes; };
struct NpadJoyLeftState { int64_t samplingNumber; NpadButtonSet buttons; AnalogStickState analogStickL; AnalogStickState analogStickR; NpadAttributesSet attributes; };
struct NpadJoyRightState { int64_t samplingNumber; NpadButtonSet buttons; AnalogStickState analogStickL; AnalogStickState analogStickR; NpadAttributesSet attributes; };
struct ControllerSupportArg {  // size 0x21c
    int8_t playerCountMin;  // 0x00
    int8_t playerCountMax;  // 0x01
    bool enableTakeOverConnection;  // 0x02
    bool enableLeftJustify;  // 0x03
    bool enablePermitJoyDual;  // 0x04
    bool enableSingleMode;  // 0x05
    bool enableIdentificationColor;  // 0x06
    nn::util::Color4u8Type identificationColor[4];  // 0x07
    bool enableExplainText;  // 0x17
    char explainText[4][129];  // 0x18
    void SetDefault()  // body from the line table (hid_ControllerSupportArg.h:96-105)
    {
        playerCountMin = 0;
        playerCountMax = 4;
        enableTakeOverConnection = true;
        enableLeftJustify = false;
        enablePermitJoyDual = true;
        enableSingleMode = false;
        enableIdentificationColor = false;
        enableExplainText = false;
    }
};
void InitializeNpad();
void SetSupportedNpadStyleSet( NpadStyleSet );
void SetSupportedNpadIdType( const NpadIdType *, size_t );
void SetNpadJoyHoldType( NpadJoyHoldType );
void BindNpadStyleSetUpdateEvent( const NpadIdType &, nn::os::SystemEventType *, nn::os::EventClearMode );
NpadStyleSet GetNpadStyleSet( const NpadIdType & );
int GetNpadStates( NpadFullKeyState *, int, const NpadIdType & );
int GetNpadStates( NpadHandheldState *, int, const NpadIdType & );
int GetNpadStates( NpadJoyDualState *, int, const NpadIdType & );
int GetNpadStates( NpadJoyLeftState *, int, const NpadIdType & );
int GetNpadStates( NpadJoyRightState *, int, const NpadIdType & );
int GetSixAxisSensorHandles( SixAxisSensorHandle *, int, const NpadIdType &, NpadStyleSet );
void StartSixAxisSensor( const SixAxisSensorHandle & );
void StopSixAxisSensor( const SixAxisSensorHandle & );
int GetSixAxisSensorStates( SixAxisSensorState *, int, const SixAxisSensorHandle & );
int GetVibrationDeviceHandles( VibrationDeviceHandle *, int, const NpadIdType &, NpadStyleSet );
void InitializeVibrationDevice( const VibrationDeviceHandle & );
void GetVibrationDeviceInfo( VibrationDeviceInfo *, const VibrationDeviceHandle & );
nn::Result ShowControllerSupport( const ControllerSupportArg & );
} }
static void sInitNpad( Npad & tNpad, const Npad::Id uId );
static void sUpdateNpad( Npad & tNpad );
static void sCSAInitialize();
static uint sCSAThreadProc( void * pParam );
static bool sNpadIsConnectedWithDelay( const Npad & tNpad ) { return tNpad.bConnected || tNpad.nDisconnectDelayFrames > 0; }
static bool sNpadWasConnectedWithDelay( const Npad & tNpad ) { return tNpad.bWasConnected || tNpad.nDisconnectDelayFrames >= 0; }
static void sNpadClearDisconnectionDelay( Npad & tNpad ) { tNpad.nDisconnectDelayFrames = -1; }
namespace nn { namespace os { void ClearSystemEvent( SystemEventType * ); } }  // nn/os/os_SystemEvent.h
void ConfigureCSAThread( Blizzard::Thread::Thread tThread );  // X/Common/Shell/MainNX64.cpp
namespace ControllerDisconnectedHandler { namespace Console { void ControllerConnected( int32 nControllerIndex ); } }  // ControllerDisconnectedHandler.h
static void sUpdateControllerState( const Npad & tNpad, ControllerState & tControllerState );
static void sGenerateRandomInput( int i, ControllerState & tState );
static bool sCSAIsActive();
static void sCSAActivate();
#include "../UserInterface/Console/ConsoleScreenManager.h"
#include "../Console/LocalPlayerClamping.h"
void InputSetControllerPresent( BOOL bPresent );  // Input.cpp
namespace nn { namespace util { template <int N, typename Tag> inline bool operator==( const BitFlagSet<N, Tag> & lhs, const BitFlagSet<N, Tag> & rhs ) { for ( int i = 0; i < BitFlagSet<N, Tag>::StorageCount; i++ ) { if ( lhs._storage[i] != rhs._storage[i] ) return false; } return true; } } }  // util_BitFlagSet.h (body reconstructed)








































































































static const int MAX_BUTTON = 16;
static NpadButtonMap NPAD_JOY_FULL_MAP[MAP_MAX][MAX_BUTTON];
static NpadButtonMap NPAD_JOY_LEFT_MAP[MAP_MAX][MAX_BUTTON];
static NpadButtonMap NPAD_JOY_RIGHT_MAP[MAP_MAX][MAX_BUTTON];

static InputButton NPAD_SINGLEJOY_REMAP[MAX_BUTTON];

static NpadButtonMap ControllerMappings[3][MAP_MAX][18];

static const Npad::Id NPAD_IDS[5] = { 0, 1, 2, 3, 32 };
static const Npad::Id HANDHELD_NPAD_IDS[2] = { 32, 0 };

int32 s_nNX64TriggerSlack = 10;
int32 s_nNX64MaxSampledStates = 8;
BOOL s_bNX64LogDroppedSamples = FALSE;
float s_flFlickAccelerationThreshold = 0.3f;
float s_flFlickAngularVelocityThreshold = 0.6f;
int32 s_nFlickActiveFramesNeeded = 6;
int32 s_nFlickCooldownTimeMs = 250;
int32 s_nDisconnectDelayFrames = 25;

static InputNX64Globals sgtNX64Globals;

CRefString InputButtonFindActionIconName( InputButton eButton, BOOL fLabelVersion, const Player * ptPlayer )
{
    Npad & npad = InputAreInHandheldMode() ? sgtNX64Globals.tNpadHandheld : sgtNX64Globals.atNpad[ptPlayer->nControllerIndex];
    int foundButtonIndex = -1;
    const NpadButtonMap * ptMap = NULL;
    switch ( npad.eType )
    {
    case TYPE_HANDHELD:
    case TYPE_FULLKEY:
    case TYPE_JOYDUAL:
        ptMap = NPAD_JOY_FULL_MAP[MAP_GAME];
        break;
    case TYPE_JOYLEFT:
        ptMap = NPAD_JOY_LEFT_MAP[MAP_GAME];
        break;
    case TYPE_JOYRIGHT:
        ptMap = NPAD_JOY_RIGHT_MAP[MAP_GAME];
        break;
    default:
        return CRefString();
    }

    for ( size_t i = 0; i < MAX_BUTTON; i++ )
    {



        if ( ptMap[i].uButtonIndex == eButton )
        {
            foundButtonIndex = i;
            break;
        }
    }

    if ( foundButtonIndex == -1 )
        return CRefString();

    return ptMap[foundButtonIndex].sIconString;
}





void InputGfxInit()
{


    sBuildButtonMaps();

}





void InputGfxDispose()
{
}







BOOL InputPlatformInitialize()
{
    nn::hid::InitializeNpad();

    nn::hid::NpadStyleSet styles = nn::hid::NpadStyleFullKey::Mask |
        nn::hid::NpadStyleHandheld::Mask |
        nn::hid::NpadStyleJoyDual::Mask |
        nn::hid::NpadStyleJoyLeft::Mask |
        nn::hid::NpadStyleJoyRight::Mask;



    nn::hid::SetSupportedNpadStyleSet( styles );
    nn::hid::SetSupportedNpadIdType( NPAD_IDS, 5 );
    nn::hid::SetNpadJoyHoldType( nn::hid::NpadJoyHoldType_Horizontal );

    sgtNX64Globals.tLastButtonActivityTime = 0;
    sgtNX64Globals.bWasControllerAppShown = false;
    sgtNX64Globals.bProgramHasResumed = false;


    sInitNpad( sgtNX64Globals.tNpadHandheld, 32 );
    sgtNX64Globals.nPrimaryPlayerControllerIndex = 0;
    sgtNX64Globals.nUpdatingControllerType = -1;



    for ( int i = 0; i < 4; i++ )
        sInitNpad( sgtNX64Globals.atNpad[i], NPAD_IDS[i] );












    sCSAInitialize();

    return TRUE;
}

void InputForceHandheldSinglePlayer()
{
    bool bInHandheldMode = InputAreInHandheldMode();
    const bool bIsTesting = CmdLineGetParams()->eTestType != 0;
    if ( bInHandheldMode && !bIsTesting )
    {
        int32 nPrimaryUserIndex = Console::GamerProfile::GetPrimaryProfileUserIndex();
        for ( int32 nControllerIndex = 0; nControllerIndex < 4; nControllerIndex++ )
        {
            if ( nControllerIndex != nPrimaryUserIndex )
            {
                if ( Console::GamerProfile::IsSignedIn( nControllerIndex ) )
                {
                    Console::GamerProfile::Internal::HandleUserSignedOut( nControllerIndex );
                }
            }
        }
    }
}

void InputFixupPostCSA()
{
    if ( PlayersAreAllLocal() )
    {
        GamePause( FALSE );
    }









    sUpdateNpad( sgtNX64Globals.tNpadHandheld );
    BOOL bHandheldConnected = sgtNX64Globals.tNpadHandheld.bConnected;
    BOOL bHandheldWasConnected = sgtNX64Globals.tNpadHandheld.bWasConnected;
    if ( bHandheldConnected )
    {
        if ( !bHandheldWasConnected )
        {

            nn::hid::SetSupportedNpadIdType( HANDHELD_NPAD_IDS, 2 );
        }
    }
    else if ( bHandheldWasConnected )
    {

        nn::hid::SetSupportedNpadIdType( NPAD_IDS, 5 );
    }


    for ( int i = 0; i < 4; i++ )
    {
        Npad & tNpad = sgtNX64Globals.atNpad[i];
        sUpdateNpad( tNpad );
    }

    InputForceHandheldSinglePlayer();



    if ( sgtNX64Globals.tNpadHandheld.bConnected || sgtNX64Globals.atNpad[sgtNX64Globals.nPrimaryPlayerControllerIndex].bConnected )
    {

        if ( !sgtNX64Globals.tNpadHandheld.bWasConnected && !sgtNX64Globals.atNpad[sgtNX64Globals.nPrimaryPlayerControllerIndex].bWasConnected )
        {
            ControllerDisconnectedHandler::Console::ControllerConnected( sgtNX64Globals.nPrimaryPlayerControllerIndex );
        }
    }

    else if ( sNpadWasConnectedWithDelay( sgtNX64Globals.tNpadHandheld ) || sNpadWasConnectedWithDelay( sgtNX64Globals.atNpad[sgtNX64Globals.nPrimaryPlayerControllerIndex] ) )
    {

        ControllerDisconnectedHandler::Console::ControllerDisconnected( sgtNX64Globals.nPrimaryPlayerControllerIndex );
    }


    for ( int i = 0; i < 4; i++ )
    {
        if ( i == sgtNX64Globals.nPrimaryPlayerControllerIndex )
            continue;
        Npad & tNpad = sgtNX64Globals.atNpad[i];
        if ( tNpad.bConnected )
        {
            if ( !tNpad.bWasConnected )
            {
                ControllerDisconnectedHandler::Console::ControllerConnected( i );
            }
        }

        else if ( sNpadWasConnectedWithDelay( tNpad ) && !bHandheldConnected )
        {
            ControllerDisconnectedHandler::Console::ControllerDisconnected( i );
        }
    }



    sNpadClearDisconnectionDelay( sgtNX64Globals.tNpadHandheld );

    for ( int i = 0; i < 4; i++ )
    {   Npad & tNpad = sgtNX64Globals.atNpad[i];
        sNpadClearDisconnectionDelay( tNpad );
    }
}




void InputPlatformUpdate(
    InputState & tInputState )
{

    if ( sCSAIsActive() )
        return;




    if ( sgtNX64Globals.bWasControllerAppShown )
    {
        InputFixupPostCSA();
        sgtNX64Globals.bWasControllerAppShown = false;
    }






    BOOL bPresent = FALSE;
    BOOL bShowControllerApp = FALSE;
    const BOOL bFakeInput = CmdLineGetParams()->tBitArray.Get( CMDLINE_FAKE_INPUT_BIT );


    int32_t primaryUser = Console::GamerProfile::GetPrimaryProfileUserIndex();
    sgtNX64Globals.nPrimaryPlayerControllerIndex = primaryUser < 0 ? 0 : primaryUser;




    sUpdateNpad( sgtNX64Globals.tNpadHandheld );
    BOOL bHandheldConnected = sgtNX64Globals.tNpadHandheld.bConnected;
    BOOL bHandheldWasConnected = sgtNX64Globals.tNpadHandheld.bWasConnected;
    if ( bHandheldConnected )
    {

        if ( !bHandheldWasConnected )
        {

            nn::hid::SetSupportedNpadIdType( HANDHELD_NPAD_IDS, 2 );


            if ( sgtNX64Globals.atNpad[0].bConnected )
            {
                bShowControllerApp = !bFakeInput;
            }
        }
    }

    else if ( bHandheldWasConnected )
    {

        nn::hid::SetSupportedNpadIdType( NPAD_IDS, 5 );



        sgtNX64Globals.atNpad[sgtNX64Globals.nPrimaryPlayerControllerIndex].bConnected = bHandheldWasConnected;
    }


    for ( int i = 0; i < 4; i++ )
    {
        Npad & tNpad = sgtNX64Globals.atNpad[i];
        sUpdateNpad( tNpad );
    }


    Npad * atNpads[4] = { NULL };


    if ( bHandheldConnected )
    {

        if ( sgtNX64Globals.atNpad[0].bConnected ) atNpads[sgtNX64Globals.nPrimaryPlayerControllerIndex] = &sgtNX64Globals.atNpad[0]; else atNpads[sgtNX64Globals.nPrimaryPlayerControllerIndex] = &sgtNX64Globals.tNpadHandheld;
    }
    else
    {










        for ( int i = 0; i < 4; i++ )
        {
            atNpads[i] = &sgtNX64Globals.atNpad[i];
        }
    }










    const int32 nFakePlayers = CmdLineGetParams()->nTestPlayerNum;
    const bool bIsTesting = CmdLineGetParams()->eTestType != 0;
    for ( int32 i = 0; i < 4; i++ )
    {
        Npad * tNpad = atNpads[i];

        ControllerState & tControllerState = tInputState.tControllerState[i];


        bool bIsFakePlayer = bIsTesting && i < nFakePlayers;

        if ( tNpad == NULL && !bIsFakePlayer )
        {

            SigmaMemoryClear( &tControllerState.tCurrentFrame, sizeof( tControllerState.tCurrentFrame ) );
            continue;
        }


        if ( tNpad )
        {

            sUpdateControllerState( *tNpad, tControllerState );
        }
        else
        {

            SigmaMemoryClear( &tControllerState.tCurrentFrame, sizeof( tControllerState.tCurrentFrame ) );
        }

        if ( bIsFakePlayer )
        {
            tControllerState.tCurrentFrame.bIsConnected = TRUE;

            if ( bFakeInput )
            {

                sGenerateRandomInput( i, tControllerState );
            }
        }











        if ( bHandheldConnected )
        {
            if ( sgtNX64Globals.atNpad[0].bConnected != sgtNX64Globals.atNpad[0].bWasConnected )
            {
                bShowControllerApp |= !bFakeInput && !bIsFakePlayer;
            }
        }
        else if ( tControllerState.tCurrentFrame.bIsConnected )
        {

            if ( !tControllerState.tPrevFrame.bIsConnected )
            {
                bShowControllerApp |= !bFakeInput && !bIsFakePlayer;
                ControllerDisconnectedHandler::Console::ControllerConnected( i );
            }
            bPresent = TRUE;
        }
        else if ( tControllerState.tPrevFrame.bIsConnected )
        {
            bShowControllerApp |= !bFakeInput && !bIsFakePlayer;

            ControllerDisconnectedHandler::Console::ControllerDisconnected( i );
        }
    }





































    if ( sgtNX64Globals.bProgramHasResumed )
    {

        bShowControllerApp = TRUE; sgtNX64Globals.bProgramHasResumed = false;
    }


    if ( bShowControllerApp && !sgtNX64Globals.bWasControllerAppShown && !bIsTesting )
    {
        sCSAActivate();
        sgtNX64Globals.bWasControllerAppShown = true;
        if ( PlayersAreAllLocal() )
        {
            GamePause( TRUE );
        }
    }


    LocalPlayerClamping::Update();

    InputSetControllerPresent( bPresent || bHandheldConnected );
}




void InputPlatformDispose()
{

    sCSADispose();
}






BOOL InputPlatformAppHasCapture()
{
    return TRUE;
}




void InputPlatformCursorSet( CursorTypes nType )
{
}






int32 InputPlatformGetControllerTriggerSlack()
{
    return s_nNX64TriggerSlack;
}




BOOL InputPlatformAcceptButtonIsSwapped()
{
    return TRUE;
}






Key InputPlatformKeyConvert( const uint16 wPlatformKey )
{
    return KEY_NONE;
}
















BOOL InputPlatformCapsLockIsDown()
{
    return FALSE;
}






BOOL InputPlatformIsCursorInWindow()
{
    return FALSE;
}






void InputPlatformShowPlatformCursor( const BOOL fShow, const BOOL fIgnoreState )
{
}









BOOL InputPlatformSetClipboard( const CHAR * puzString )
{
    return FALSE;
}







BOOL InputPlatformGetClipboard( CHAR * puzString, int32 nMaxLength )
{
    return FALSE;
}









const char * InputIMEGetCompositionString( int32 * nInsertionIndex )
{
    return NULL;
}







const char * InputIMEGetResultString()
{
    return NULL;
}




IVector2D InputPlatformGetTouchPadResolution( int32 nControllerIndex )
{
    return IVector2D( 0, 0 );
}




void InputSetPlayerController( Player * ptPlayer, int32 nControllerIndex )
{

    ptPlayer->nControllerIndex = nControllerIndex;
    bool bIsFakePlayer = CmdLineGetParams()->eTestType != 0 && nControllerIndex < CmdLineGetParams()->nTestPlayerNum;

    if ( nControllerIndex >= 0 && nControllerIndex < 4 )
    {
        if ( !InputGetCurrentState( INPUTCONTEXT_DEFAULT ).tControllerState[nControllerIndex].tCurrentFrame.bIsConnected && !bIsFakePlayer )
        {
            ControllerDisconnectedHandler::Console::ControllerDisconnected( nControllerIndex );
        }
    }
}














Npad * InputGetNpad( int32_t nControllerIndex )
{
    Npad * ptNpad = NULL;

    if ( nControllerIndex >= 0 && nControllerIndex < 4 )
    {
        ptNpad = &sgtNX64Globals.atNpad[nControllerIndex];

        if ( InputAreInHandheldMode() )
        {


            ptNpad = sgtNX64Globals.atNpad[0].bConnected ? &sgtNX64Globals.atNpad[0] : &sgtNX64Globals.tNpadHandheld;
        }
    }
    return ptNpad;
}








void InputSetVibration( const int32 nControllerIndex, const float flSpeed )
{


    if ( nControllerIndex < 0 || nControllerIndex >= 4 )
        return;




    Npad * ptNpad = InputGetNpad( nControllerIndex );
    if ( !ptNpad->bConnected )
        return;









    float Amplitude = flSpeed * 0.5f;
    const float MinFrequencyScale = 0.5f;




    float FrequencyScale = Math::Max( MinFrequencyScale, flSpeed );

    nn::hid::VibrationValue tValue;
    tValue.amplitudeLow = Amplitude;
    tValue.frequencyLow = FrequencyScale * 160.0f;
    tValue.amplitudeHigh = Amplitude;
    tValue.frequencyHigh = FrequencyScale * 320.0f;


    for ( uint32 i = 0; i < ptNpad->uVibratorCount; i++ )
    {
        Npad::Vibrator & vibrator = ptNpad->atVibrator[i];
        nn::hid::SendVibrationValue( vibrator.tHandle, tValue );
    }
}

int32_t InputGetControllerType( int32_t nControllerIndex )
{
    int32 nType = -1;
    Npad * npad = InputGetNpad( nControllerIndex );
    if ( npad )
        nType = npad->eType;
    return nType;
}




bool InputIsSingleJoycon( int32_t nControllerIndex )
{
    int32_t type = InputGetControllerType( nControllerIndex );
    return type == TYPE_JOYLEFT || type == TYPE_JOYRIGHT;
}


BOOL InputAreInHandheldMode()
{
    return sgtNX64Globals.tNpadHandheld.bConnected;
}






static void sInitNpad( Npad & tNpad, const Npad::Id uId )
{
    tNpad.uId = uId;
    tNpad.bConnected = false; tNpad.bWasConnected = false;
    tNpad.tStyle._storage[0] = 0;
    sNpadClearDisconnectionDelay( tNpad );

    SigmaMemoryClear( &tNpad.tState, sizeof( tNpad.tState ) );
    SigmaMemoryClear( tNpad.atVibrator, sizeof( tNpad.atVibrator ) );
    tNpad.uVibratorCount = 0;
    tNpad.tSixAxis.nHandleCount = 0;
    nn::hid::BindNpadStyleSetUpdateEvent( tNpad.uId, &tNpad.tStyleSetUpdateEvent, nn::os::EventClearMode_ManualClear );
}



















































































static void sUpdateControllerStick( const nn::hid::AnalogStickState & tNpadState, Vector2D & tControllerState )
{
    float x = (float)tNpadState.x / 32767.0f;
    float y = (float)tNpadState.y / 32767.0f;
    tControllerState.Set( x, y );
}

static void sUpdateControllerTrigger( const nn::hid::NpadButtonSet & tNpadState, const int iNpadButton, int32 & iControllerState )
{
    iControllerState = tNpadState.Test( iNpadButton ) ? 255 : 0;
}








static void sUpdateControllerButtons( const nn::hid::NpadButtonSet & tNpadState, uint32 & uControllerState, const NpadButtonMap * atMap, const InputButton * aeRemap, const uint32 uMapCount )
{

    uControllerState &= ~( 1 << 14 );
    uControllerState &= ~( 1 << 15 );

    bool bIgnoreList[MAX_BUTTON] = { false };

    for ( uint32 i = 0; i < uMapCount; i++ )
    {
        if ( bIgnoreList[i] ) continue;

        const NpadButtonMap tMap = atMap[i];
        const bool bActive = tMap.tNpadTest == ( tMap.tNpadMask & tNpadState );
        if ( aeRemap && aeRemap[tMap.uButtonIndex] != -1 )
        {
            if ( bActive ) uControllerState |= ( 1 << aeRemap[tMap.uButtonIndex] ); else uControllerState &= ~( 1 << aeRemap[tMap.uButtonIndex] );
            bIgnoreList[aeRemap[tMap.uButtonIndex]] = true;
        }
        else
        {
            if ( bActive ) uControllerState |= ( 1 << tMap.uButtonIndex ); else uControllerState &= ~( 1 << tMap.uButtonIndex );
        }
    }
}







































static void sUpdateControllerState( const Npad & tNpad, ControllerState & tControllerState )
{
    MapType type;
    type = Console::Screens::IsScreenSetOpen() ? MAP_UI : MAP_GAME;




    switch ( tNpad.eType )
    {
    case TYPE_JOYDUAL:


        tControllerState.tCurrentFrame.tGestures.bFlick = tNpad.tState.gestures.Test<Npad::GestureSet::Flag<0> >();

    case TYPE_HANDHELD:
    case TYPE_FULLKEY:


        sUpdateControllerStick( tNpad.tState.analogStickL, tControllerState.tCurrentFrame.vecLeftStick );
        sUpdateControllerStick( tNpad.tState.analogStickR, tControllerState.tCurrentFrame.vecRightStick );

        sUpdateControllerTrigger( tNpad.tState.buttons, 8, tControllerState.tCurrentFrame.nLeftTrigger );
        sUpdateControllerTrigger( tNpad.tState.buttons, 9, tControllerState.tCurrentFrame.nRightTrigger );

        sUpdateControllerButtons( tNpad.tState.buttons, tControllerState.tCurrentFrame.uButtons, NPAD_JOY_FULL_MAP[type], NULL, MAX_BUTTON );
        break;

    case TYPE_JOYLEFT:


        sUpdateControllerStick( tNpad.tState.analogStickL, tControllerState.tCurrentFrame.vecLeftStick );
        tControllerState.tCurrentFrame.vecLeftStick.Set( -tControllerState.tCurrentFrame.vecLeftStick.y, tControllerState.tCurrentFrame.vecLeftStick.x );
        tControllerState.tCurrentFrame.vecRightStick.Set( 0.0f, 0.0f );


        tControllerState.tCurrentFrame.tGestures.bFlick = tNpad.tState.gestures.Test<Npad::GestureSet::Flag<0> >();


        if ( type == MAP_UI )
        {
            sUpdateControllerTrigger( tNpad.tState.buttons, 8, tControllerState.tCurrentFrame.nLeftTrigger );
            sUpdateControllerTrigger( tNpad.tState.buttons, 6, tControllerState.tCurrentFrame.nRightTrigger );
        }
        else
        {
            sUpdateControllerTrigger( tNpad.tState.buttons, 24, tControllerState.tCurrentFrame.nLeftTrigger );
            sUpdateControllerTrigger( tNpad.tState.buttons, 25, tControllerState.tCurrentFrame.nRightTrigger );
        }

        sUpdateControllerButtons( tNpad.tState.buttons, tControllerState.tCurrentFrame.uButtons, NPAD_JOY_LEFT_MAP[type], NPAD_SINGLEJOY_REMAP, 14 );
        break;

    case TYPE_JOYRIGHT:



        sUpdateControllerStick( tNpad.tState.analogStickR, tControllerState.tCurrentFrame.vecLeftStick );
        tControllerState.tCurrentFrame.vecLeftStick.Set( tControllerState.tCurrentFrame.vecLeftStick.y, -tControllerState.tCurrentFrame.vecLeftStick.x );
        tControllerState.tCurrentFrame.vecRightStick.Set( 0.0f, 0.0f );


        tControllerState.tCurrentFrame.tGestures.bFlick = tNpad.tState.gestures.Test<Npad::GestureSet::Flag<0> >();


        if ( type == MAP_UI )
        {
            sUpdateControllerTrigger( tNpad.tState.buttons, 9, tControllerState.tCurrentFrame.nLeftTrigger );
            sUpdateControllerTrigger( tNpad.tState.buttons, 7, tControllerState.tCurrentFrame.nRightTrigger );
        }
        else
        {
            sUpdateControllerTrigger( tNpad.tState.buttons, 26, tControllerState.tCurrentFrame.nLeftTrigger );
            sUpdateControllerTrigger( tNpad.tState.buttons, 27, tControllerState.tCurrentFrame.nRightTrigger );
        }

        sUpdateControllerButtons( tNpad.tState.buttons, tControllerState.tCurrentFrame.uButtons, NPAD_JOY_RIGHT_MAP[type], NPAD_SINGLEJOY_REMAP, 14 );
        break;

    default:


        SigmaMemoryClear( &tControllerState.tCurrentFrame, sizeof( tControllerState.tCurrentFrame ) );
        break;
    }




    tControllerState.tCurrentFrame.bIsConnected = sNpadIsConnectedWithDelay( tNpad );




    tControllerState.tPrevFrame.bIsConnected = sNpadWasConnectedWithDelay( tNpad );


    if ( tControllerState.tCurrentFrame.uButtons != 0 )
    {
        sgtNX64Globals.tLastButtonActivityTime = Blizzard::Time::Seconds();
    }
}



















template <typename T> static void sUpdateNpadState( const Npad::Id uId, Npad::State & tNpadState, Npad::SixAxisState & tSixAxis )
{
    {


        const int64_t nPreviousSample = tNpadState.samplingNumber;
        int nStates = blz::clamp( s_nNX64MaxSampledStates, 1, 16 );
        T atStates[16];
        nStates = nn::hid::GetNpadStates( atStates, nStates, uId );
        if ( nStates < 1 )
        {
            return;
        }






        tNpadState.analogStickL = atStates[0].analogStickL;
        tNpadState.analogStickR = atStates[0].analogStickR;
        tNpadState.attributes = atStates[0].attributes;
        tNpadState.buttons = atStates[0].buttons;
        tNpadState.samplingNumber = atStates[0].samplingNumber;


        for ( int i = 1; i < nStates && nPreviousSample < atStates[i].samplingNumber; i++ )
        {
            tNpadState.buttons |= atStates[i].buttons;
        }
        // The DWARF also records nLastSample (line 1564) and nDroppedSamples (line 1567, nested
        // block): dropped-sample logging (s_bNX64LogDroppedSamples) that leaves no code.
    }











    {
        const int32_t nGestureIndex = 0;
        const float_t flAccelerationThresholdSqr = s_flFlickAccelerationThreshold * 7.0f * s_flFlickAccelerationThreshold * 7.0f;
        const float_t flAngularVelocityThresholdSqr = s_flFlickAngularVelocityThreshold * 5.0f * s_flFlickAngularVelocityThreshold * 5.0f;
        bool bActivateFlick = false;


        for ( int nSensorIndex = 0; nSensorIndex < tSixAxis.nHandleCount; nSensorIndex++ )
        {

            int64_t nLastProcessedSamplingNumber = tSixAxis.atGestureStates[nSensorIndex].anLastProcessedSamplingNumber[nGestureIndex];
            int64_t nTimeSinceLastActivationMs = tSixAxis.atGestureStates[nSensorIndex].anTimeSinceLastActivation[nGestureIndex];
            int32_t nActiveSampleCount = tSixAxis.atGestureStates[nSensorIndex].anActiveSampleCount[nGestureIndex];


            nn::hid::SixAxisSensorState * atSensorStates = tSixAxis.atSensorStates[nSensorIndex];
            const int32_t nSampleCount = nn::hid::GetSixAxisSensorStates( atSensorStates, 16, tSixAxis.atHandles[nSensorIndex] );




            int32_t nOldestUnprocessedSampleIndex = nSampleCount - 1;
            while ( nOldestUnprocessedSampleIndex >= 0 )
            {
                const nn::hid::SixAxisSensorState & tFrameSensorState = atSensorStates[nOldestUnprocessedSampleIndex];
                if ( tFrameSensorState.samplingNumber > nLastProcessedSamplingNumber )
                {
                    break;
                }
                nOldestUnprocessedSampleIndex--;
            }


            if ( nOldestUnprocessedSampleIndex < 0 )
            {
                continue;
            }









            if ( nOldestUnprocessedSampleIndex == nSampleCount - 1 )
            {
                nTimeSinceLastActivationMs = s_nFlickCooldownTimeMs;
            }



            for ( int32_t nSampleIndex = nOldestUnprocessedSampleIndex; nSampleIndex >= 0; nSampleIndex-- )
            {
                const nn::hid::SixAxisSensorState & tFrameSensorState = atSensorStates[nSampleIndex];

                nTimeSinceLastActivationMs += tFrameSensorState.deltaTime.GetMilliSeconds();
                nLastProcessedSamplingNumber = tFrameSensorState.samplingNumber;


                if ( nTimeSinceLastActivationMs < s_nFlickCooldownTimeMs )
                {
                    nActiveSampleCount = 0;
                    continue;
                }

                nTimeSinceLastActivationMs = s_nFlickCooldownTimeMs;





                nn::util::Float3 tAcceleration = tFrameSensorState.acceleration;
                tAcceleration.z += 1.0f;
                nn::util::Float3 tAngularVelocity = tFrameSensorState.angularVelocity;


                float_t flAccelerationMagSqr = tAcceleration.x * tAcceleration.x + tAcceleration.y * tAcceleration.y + tAcceleration.z * tAcceleration.z;
                float_t flAngularVelocityMagSqr = tAngularVelocity.x * tAngularVelocity.x + tAngularVelocity.y * tAngularVelocity.y + tAngularVelocity.z * tAngularVelocity.z;
                if ( flAccelerationMagSqr >= flAccelerationThresholdSqr )
                {
                    nActiveSampleCount++;
                }


                else if ( flAngularVelocityMagSqr >= flAngularVelocityThresholdSqr )
                {
                    nActiveSampleCount++;
                }
                else
                {
                    nActiveSampleCount = 0;
                }


                if ( nActiveSampleCount >= s_nFlickActiveFramesNeeded )
                {
                    bActivateFlick = true;
                    nActiveSampleCount = 0;
                    nTimeSinceLastActivationMs = 0;
                }
            }





            tSixAxis.atGestureStates[nSensorIndex].anLastProcessedSamplingNumber[nGestureIndex] = nLastProcessedSamplingNumber;
            tSixAxis.atGestureStates[nSensorIndex].anTimeSinceLastActivation[nGestureIndex] = nTimeSinceLastActivationMs;
            tSixAxis.atGestureStates[nSensorIndex].anActiveSampleCount[nGestureIndex] = nActiveSampleCount;
        }

        tNpadState.gestures.template Set<Npad::GestureSet::Flag<0> >( bActivateFlick );
    }
}

static void sUpdateNpad( Npad & tNpad )
{
    tNpad.bWasConnected = tNpad.bConnected;

    tNpad.tStyle = nn::hid::GetNpadStyleSet( tNpad.uId );
    tNpad.bConnected = tNpad.tStyle.IsAnyOn();



    bool bStyleSetUpdated = nn::os::TryWaitSystemEvent( &tNpad.tStyleSetUpdateEvent );
    if ( ( bStyleSetUpdated || !tNpad.bWasConnected ) && tNpad.bConnected )
    {
        if ( bStyleSetUpdated )
        {
            nn::os::ClearSystemEvent( &tNpad.tStyleSetUpdateEvent );
        }


        for ( int i = 0; i < tNpad.tSixAxis.nHandleCount; i++ )
        {

            nn::hid::StopSixAxisSensor( tNpad.tSixAxis.atHandles[i] );
        }


        if ( tNpad.tStyle.Test<nn::hid::NpadStyleJoyDual>() || tNpad.tStyle.Test<nn::hid::NpadStyleJoyLeft>() || tNpad.tStyle.Test<nn::hid::NpadStyleJoyRight>() )
        {
            tNpad.tSixAxis.nHandleCount = nn::hid::GetSixAxisSensorHandles( tNpad.tSixAxis.atHandles, 8, tNpad.uId, tNpad.tStyle );



            for ( int i = 0; i < tNpad.tSixAxis.nHandleCount; i++ )
            {

                nn::hid::StartSixAxisSensor( tNpad.tSixAxis.atHandles[i] );


                for ( int j = 0; j < 1; j++ )
                {   tNpad.tSixAxis.atGestureStates[i].anLastProcessedSamplingNumber[j] = 0;
                    tNpad.tSixAxis.atGestureStates[i].anActiveSampleCount[j] = 0;
                    tNpad.tSixAxis.atGestureStates[i].anTimeSinceLastActivation[j] = s_nFlickCooldownTimeMs;
                }
            }
        }
        else
        {

            tNpad.tSixAxis.nHandleCount = 0;
        }
    }

    if ( !tNpad.bConnected )
    {

        bool bWasJoyConFormat = tNpad.eType == TYPE_JOYLEFT || tNpad.eType == TYPE_JOYRIGHT || tNpad.eType == TYPE_JOYDUAL;

        tNpad.eType = TYPE_NONE;
        SigmaMemoryClear( &tNpad.tState, sizeof( tNpad.tState ) );
        SigmaMemoryClear( tNpad.atVibrator, sizeof( tNpad.atVibrator ) );
        tNpad.uVibratorCount = 0;




        if ( bWasJoyConFormat && tNpad.bWasConnected )
        {
            tNpad.nDisconnectDelayFrames = s_nDisconnectDelayFrames;
        }
        else if ( tNpad.nDisconnectDelayFrames >= 0 )
        {
            tNpad.nDisconnectDelayFrames--;
        }
    }
    else if ( ( tNpad.tStyle & nn::hid::NpadStyleHandheld::Mask ).IsAnyOn() )
    {
        tNpad.eType = TYPE_HANDHELD;
        sUpdateNpadState<nn::hid::NpadHandheldState>( tNpad.uId, tNpad.tState, tNpad.tSixAxis );
    }
    else if ( ( tNpad.tStyle & nn::hid::NpadStyleFullKey::Mask ).IsAnyOn() )
    {
        tNpad.eType = TYPE_FULLKEY;
        sUpdateNpadState<nn::hid::NpadFullKeyState>( tNpad.uId, tNpad.tState, tNpad.tSixAxis );
    }
    else if ( ( tNpad.tStyle & nn::hid::NpadStyleJoyDual::Mask ).IsAnyOn() )
    {
        tNpad.eType = TYPE_JOYDUAL;
        sUpdateNpadState<nn::hid::NpadJoyDualState>( tNpad.uId, tNpad.tState, tNpad.tSixAxis );
    }
    else if ( ( tNpad.tStyle & nn::hid::NpadStyleJoyLeft::Mask ).IsAnyOn() )
    {
        tNpad.eType = TYPE_JOYLEFT;
        sUpdateNpadState<nn::hid::NpadJoyLeftState>( tNpad.uId, tNpad.tState, tNpad.tSixAxis );
    }
    else if ( ( tNpad.tStyle & nn::hid::NpadStyleJoyRight::Mask ).IsAnyOn() )
    {
        tNpad.eType = TYPE_JOYRIGHT;
        sUpdateNpadState<nn::hid::NpadJoyRightState>( tNpad.uId, tNpad.tState, tNpad.tSixAxis );
    }
    else
    {
        tNpad.eType = TYPE_INVALID;
        SigmaMemoryClear( &tNpad.tState, sizeof( tNpad.tState ) );
        SigmaMemoryClear( tNpad.atVibrator, sizeof( tNpad.atVibrator ) );
        tNpad.uVibratorCount = 0;
    }



    if ( tNpad.bConnected && bStyleSetUpdated )
    {

        nn::hid::VibrationDeviceHandle atHandles[Npad::MAX_VIBRATORS];
        tNpad.uVibratorCount = nn::hid::GetVibrationDeviceHandles( atHandles, Npad::MAX_VIBRATORS, tNpad.uId, tNpad.tStyle );
        for ( uint32 i = 0; i < tNpad.uVibratorCount; i++ )
        {
            Npad::Vibrator & tVibrator = tNpad.atVibrator[i];
            tVibrator.tHandle = atHandles[i];
            nn::hid::InitializeVibrationDevice( tVibrator.tHandle );
            nn::hid::GetVibrationDeviceInfo( &tVibrator.tInfo, tVibrator.tHandle );
        }
    }


    if ( !tNpad.bWasConnected && tNpad.bConnected )
    {

        sNpadClearDisconnectionDelay( tNpad );
    }
}

static void sGenerateRandomInput( int i, ControllerState & tState )
{
    Blizzard::Time::Millisecond ms = Blizzard::Time::Milliseconds() + i * 200;

    float flFactor = (float)ms / 150.0f + (float)i * 10.0f;

    tState.tCurrentFrame.vecLeftStick.x = sinf( flFactor );
    tState.tCurrentFrame.vecLeftStick.y = cosf( flFactor );

    uint32 nButton = ( ms % 2000 ) / 400;

    if ( nButton == 0 ) tState.tCurrentFrame.uButtons |= ( 1 << 2 ); else tState.tCurrentFrame.uButtons &= ~( 1 << 2 );
    tState.tCurrentFrame.uButtons &= ~( 1 << 0 );
    if ( nButton == 1 ) tState.tCurrentFrame.uButtons |= ( 1 << 1 ); else tState.tCurrentFrame.uButtons &= ~( 1 << 1 );
    if ( nButton == 2 ) tState.tCurrentFrame.uButtons |= ( 1 << 3 ); else tState.tCurrentFrame.uButtons &= ~( 1 << 3 );
    if ( nButton == 3 ) tState.tCurrentFrame.uButtons |= ( 1 << 7 ); else tState.tCurrentFrame.uButtons &= ~( 1 << 7 );
    if ( nButton == 4 ) tState.tCurrentFrame.uButtons |= ( 1 << 5 ); else tState.tCurrentFrame.uButtons &= ~( 1 << 5 );
}

void InputOnProgramResume()
{
    sgtNX64Globals.bProgramHasResumed = true;
}




static void sBuildButtonMaps()
{














#include "../../Common/generated-game-data/button_maps.inc"
#line 1968

    InputClearRemappings();
}

void InputClearRemappings()
{
    for ( int i = 0; i < MAX_BUTTON; i++ )
        NPAD_SINGLEJOY_REMAP[i] = -1;
}
















void InputRemapSingleJoyconButton( InputButton eFrom, InputButton eTo )
{
    NPAD_SINGLEJOY_REMAP[eFrom] = eTo;
}







void SetNextControllerTypeForIconAdjustment( int32_t nControllerType )
{
    sgtNX64Globals.nUpdatingControllerType = nControllerType;
}


uint32_t InputGetConnectedNpadIds( nn::hid::NpadIdType * atNpads )
{
    uint32_t nCount = 0;


    if ( InputAreInHandheldMode() )
    {

        if ( sgtNX64Globals.atNpad[0].bConnected )
        {

            atNpads[nCount++] = sgtNX64Globals.atNpad[0].uId;
        }
        else
        {


            atNpads[nCount++] = sgtNX64Globals.tNpadHandheld.uId;
        }
    }
    else
    {
        for ( int i = 0; i < 4; i++ )
        {
            if ( sgtNX64Globals.atNpad[i].bConnected )
            {
                atNpads[i] = sgtNX64Globals.atNpad[i].uId;
                nCount++;
            }
        }
    }
    return nCount;
}

int32_t InputGetControllerIndexForNpad( nn::hid::NpadIdType tNpadId )
{
    if ( InputAreInHandheldMode() )
    {
        if ( tNpadId == 32 || tNpadId == 0 )
            return sgtNX64Globals.nPrimaryPlayerControllerIndex;
        return -1;
    }
    if ( tNpadId < 4 )
        return tNpadId;
    return -1;
}




































static int32 sReverseRemapSingleJoyconButton( const int32 nButton )
{
    for ( int i = 0; i < MAX_BUTTON - 1; i++ )
    {
        if ( NPAD_SINGLEJOY_REMAP[i] == nButton )
            return i;
    }
    return nButton;
}

// NON_MATCHING: same size; 182/384 insns differ in the remap tail (original keeps the index in w9 and x8, branch not csel). See top comment.

LPCSTR AdjustUIButtonIconForSwitch( LPCSTR szDefaultIconName )
{
    LPCSTR adjustedIcon = szDefaultIconName;


    int32_t controllerType = InputGetControllerType( UI_GetPrimaryUIController() );


    if ( sgtNX64Globals.nUpdatingControllerType != -1 )
    {
        controllerType = sgtNX64Globals.nUpdatingControllerType;
    }
    else if ( controllerType == TYPE_INVALID || controllerType == TYPE_NONE )
    {
        controllerType = sgtNX64Globals.atNpad[sgtNX64Globals.nPrimaryPlayerControllerIndex].eType;
    }

    bool bSingleJoyconMode = controllerType == TYPE_JOYLEFT || controllerType == TYPE_JOYRIGHT;


    const NpadButtonMap * fullMap = NPAD_JOY_FULL_MAP[MAP_UI];
    const NpadButtonMap * ptMap = NULL;

    if ( controllerType == TYPE_JOYLEFT )
    {
        ptMap = NPAD_JOY_LEFT_MAP[MAP_UI];
    }
    else if ( controllerType == TYPE_JOYRIGHT )
    {
        ptMap = NPAD_JOY_RIGHT_MAP[MAP_UI];
    }
    else
    {

        if ( !strcasecmp( szDefaultIconName, "button1" ) )
        {
            adjustedIcon = "button2";
        }
        else if ( !strcasecmp( szDefaultIconName, "button2" ) )
        {
            adjustedIcon = "button1";
        }
    }


    if ( !strcasecmp( adjustedIcon, "back" ) || !strcasecmp( adjustedIcon, "view" ) )
    {
        adjustedIcon = "plus";
    }
    else if ( !strcasecmp( adjustedIcon, "menu" ) || !strcasecmp( adjustedIcon, "start" ) )
    {
        adjustedIcon = "minus";
    }


    else if ( !strcasecmp( adjustedIcon, "r2h" ) )
    {



        return bSingleJoyconMode ? "r4" : "r2";
    }







    else if ( bSingleJoyconMode )
    {
        if ( !strcasecmp( adjustedIcon, "Dpad_H_Button" ) )
        {
            adjustedIcon = "joycon_H_button";
        }
        else if ( !strcasecmp( adjustedIcon, "Dpad_V_Button" ) )
        {
            adjustedIcon = "joycon_V_button";
        }
        else if ( !strcasecmp( adjustedIcon, "l_stick_H_Button" ) )
        {
            adjustedIcon = "joycon_neutral_stick_scroll_horizontal";
        }
        else if ( !strcasecmp( adjustedIcon, "l_stick_V_Button" ) )
        {
            adjustedIcon = "joycon_neutral_stick_scroll_vertical";
        }
        else if ( !strcasecmp( adjustedIcon, "l_stick" ) )
        {
            adjustedIcon = "joycon_neutral_stick";
        }
    }


    if ( ptMap )
    {
        for ( int i = 0; i < MAX_BUTTON; i++ )
        {
            if ( !strcasecmp( fullMap[i].sIconName, adjustedIcon ) )
            {

                int32 uButton = sReverseRemapSingleJoyconButton( i );
                return ptMap[uButton].sIconName;
            }
        }
    }










    return adjustedIcon;
}










CSAGlobals sCSAGlobals;

static uint sCSAThreadProc( void * pParam )
{


    sCSAGlobals.tShowEvent.WaitForever();

    while ( sCSAGlobals.bAlive )
    {




        nn::hid::ControllerSupportArg arg;
        arg.SetDefault();
        arg.enableSingleMode = sCSAGlobals.bSingleMode;

        nn::Result result = nn::hid::ShowControllerSupport( arg );




        sCSAGlobals.tDoneEvent.Set();
        sCSAGlobals.tShowEvent.WaitForever();
    }
    return 0;
}

static void sCSAInitialize()
{
    sCSAGlobals.bActive = false; sCSAGlobals.bAlive = true;

    sCSAGlobals.tShowEvent.Create();
    sCSAGlobals.tDoneEvent.Create();
    Blizzard::Thread::Create( sCSAThreadProc, NULL, &sCSAGlobals.tThread, "CSAThread", 1, 0x8000 );

    Blizzard::Thread::SetPriority( sCSAGlobals.tThread, Blizzard::Thread::PRIORITY_LOWEST );
    ConfigureCSAThread( sCSAGlobals.tThread );
    Blizzard::Thread::Resume( sCSAGlobals.tThread );
}



static void sCSADispose()
{
    if ( sCSAGlobals.bActive )
    {
        sCSAGlobals.tDoneEvent.WaitForever();
        sCSAGlobals.bActive = false;
    }

    sCSAGlobals.bAlive = false;
    sCSAGlobals.tShowEvent.Set();
    Blizzard::Thread::WaitForever( sCSAGlobals.tThread );

    Blizzard::Thread::Close( sCSAGlobals.tThread );
    sCSAGlobals.tShowEvent.Destroy();
    sCSAGlobals.tDoneEvent.Destroy();
}

static void sCSAActivate()
{

    sCSAGlobals.bActive = true;
    sCSAGlobals.bSingleMode = sgtNX64Globals.tNpadHandheld.bConnected;
    sCSAGlobals.tShowEvent.Set();
}

static bool sCSAIsActive()
{
    if ( sCSAGlobals.bActive )
    {




        if ( sCSAGlobals.tDoneEvent.Wait( 0 ) )
        {
            sCSAGlobals.bActive = false;
        }
    }
    return sCSAGlobals.bActive;
}
bool InputCSAIsActive()
{
    return sCSAGlobals.bActive;
}
