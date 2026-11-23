// Element types for the blz::vector template checks (tests/blz/vector*.cpp).
// Layouts come from build/dwarf/headers; only what the instantiated members
// touch is declared (size, member order, which special members are
// user-provided and how the inline ones are written).
#pragma once

#include "Contrib/Contrib/blz/include/blz/util_pair.h"
#include "Contrib/Contrib/blz/include/blz/string.h"

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned long uint64;
typedef long int64;
typedef int BOOL;
typedef int SNO;
typedef int GBID;
typedef const char* LPCSTR;

// ---------------------------------------------------------------- Sigma types
typedef int SNOGroup;

struct SNOName {  // size 0x8
    SNOGroup snoGroup;  // 0x00
    SNO snoHandle;      // 0x04
    SNOName(const SNOName& snoname) { Set(snoname.snoGroup, snoname.snoHandle); }
    void Set(const SNOGroup _snoGroup, const SNO _snoHandle) {
        snoGroup = _snoGroup;
        snoHandle = _snoHandle;
    }
};

struct RGBAColor {  // size 0x4
    uint8 r, g, b, a;
    RGBAColor(const RGBAColor& rgba) { Set(rgba); }
    void Set(const RGBAColor& rgba) { *reinterpret_cast<uint32*>(this) = *reinterpret_cast<const uint32*>(&rgba); }
};

struct Vector2D {  // size 0x8
    float x, y;
    Vector2D& operator=(const Vector2D& wpSrc) {
        x = wpSrc.x;
        y = wpSrc.y;
        return *this;
    }
};

struct Vector3D {  // size 0xc
    float x, y, z;
    Vector3D(const Vector3D& wpSrc) : x(wpSrc.x), y(wpSrc.y), z(wpSrc.z) {}
};

struct VertexColor { uint32 val; };
typedef RGBAColor UByte4;

struct Quaternion { Vector3D wvImag; float wdReal; };
struct PRTransform { Quaternion q; Vector3D wp; };
struct AxialCylinder { Vector3D wpBase; float wdHeight; float wdRadius; };
struct OBB { PRTransform transform; Vector3D wvExt; };
struct Rect2D {
    float left, top, right, bottom;
    Rect2D& operator=(const Rect2D& rect) {
        left = rect.left;
        top = rect.top;
        right = rect.right;
        bottom = rect.bottom;
        return *this;
    }
};
namespace Shape { enum Type { SHAPE_0 }; }

typedef uint32 ACDID_t;
struct ACDID { uint32 id; };
typedef uint32 SWorldID;
struct WorldPlace { Vector3D wp; SWorldID idSWorld; };
enum TargetType { TARGET_0 };
class Target {
public:
    TargetType eType;
    ACDID idACDTarget;
    WorldPlace tPlace;
    BOOL bForcedActorTarget;
};

class CRefStringData;
class CRefString {  // size 0x18
public:
    CRefString(const CRefString& szString);
    ~CRefString();
    CRefString& operator=(const CRefString& szString);
    CRefString& operator=(LPCSTR szString);
    CRefStringData* m_pData;
    char* m_pszString;
    BOOL m_bForceAppLevelMemoryPool;
};

struct UIID {  // size 0x8
    UIID(const UIID& other) { SetInternal(other.nIDHash, 0); }
    UIID& operator=(const UIID& arg) {
        if (this != &arg)
            SetInternal(arg.nIDHash, 0);
        return *this;
    }
    uint64 nIDHash;
    void SetInternal(uint64, LPCSTR);
};

struct ImageTextureFrame { SNO snoTexture; int32 nFrame; };

// ------------------------------------------------------------ element types
namespace Console { namespace Online { struct SeasonInfo { int a[8]; }; } }
enum SeverityLevel { SEVERITY_0 };
enum OutputStreamType { OUTPUT_0 };
class Particle;
struct FormattedTextDelta;
struct UIStyleChildSpec;
class UIControl;

enum ChromaEffectType { CHROMA_0 };
struct ChromaEffect {  // size 0x10
    ChromaEffectType eType;
    uint32 dwStartTime;
    float flDurationInSecs;
    RGBAColor rgbaColor;
};

struct BuffColor {  // size 0x8
    BuffColor(LPCSTR, RGBAColor);
    SNO snoPower;
    RGBAColor rgbaColor;
};

namespace AsyncRender {
typedef void (*AsyncRenderCallback)(void*, void*);
struct AsyncRenderCallbackItem {  // size 0x18
    AsyncRenderCallback callback;
    void* pParam1;
    void* pParam2;
};
}  // namespace AsyncRender

enum SpriteType { SPRITE_0 };
enum RenderContext { RC_0 };
struct SpritePointerList;
struct SpriteBucket {  // size 0x28
    uint32 dwKey;
    float flZ;
    SpriteType eType;
    RenderContext eContext;
    SpritePointerList* ptSpritePtrs;
    uint32 dwStartIndex;
    uint32 dwMinVertexIndex;
    uint32 dwMaxVertexIndex;
};

struct DebugText {  // size 0x128
    char uszText[256];
    Vector2D vecOriginalPosUIC;
    float flWidthUIC;
    float flHeightUIC;
    Rect2D rectUIC;
    BOOL fDraw;
    RenderContext eContext;
};

struct VertexNoTexL {  // size 0x10
    Vector3D wpPos;
    VertexColor vcDiffuse;
};

struct Vertex1TexL {  // size 0x1c
    Vector3D wpPos;
    UByte4 wvpNormal;
    VertexColor vcDiffuse;
    VertexColor vcAuxColor;
    UByte4 vpUV0;
};

struct MapTarget {  // size 0x18
    Vector2D vecPosition;
    int32 nIndex;
    UIID id;
};

struct WeatherVolume {  // size 0x54
    Vector3D wpPos;
    Shape::Type eType;
    AxialCylinder cyl;
    OBB obb;
    SNO snoWeather;
    SNO snoQuestRange;
};

struct QueuedFlippyInfo {  // size 0x30
    Vector3D wpDest;
    SNO snoParticle;
    PRTransform transform;
    SNO snoActorOverride;
};

enum AIQueuedActionType { AIQ_0 };
struct AIQueuedPower {  // size 0x58
    SNO snoPower;
    Target tTarget;
    uint32 uAnimTag;
    uint32 uNewIdleAnimTag;
    uint32 uTicks;
    int32 uData;
    float flData;
    char strData[32];
    AIQueuedActionType eType;
};

struct UIControlItem {  // size 0x18
    UIID id;
    UIControl* ptControl;
    UIControl* ptActiveControl;
};

class ActorCommonData;
enum TransmuteRecipeIngredientType { TRI_0 };
struct TransmuteInput {  // size 0x20
    GBID gbidCurrency;
    ActorCommonData* ptACDItem;
    int64 nStackCount;
    BOOL bUsedInRecipe;
    TransmuteRecipeIngredientType eIngredientType;
};

namespace Blizzard { namespace Time { struct Timestamp { uint64 m_microsecondsSinceUnixEpoch; }; } }

namespace OnlineService {
typedef int32 ContentLicenseId;
struct ContentLicense {  // size 0x18
    ContentLicense();
    ContentLicenseId licenseId;
    Blizzard::Time::Timestamp expireTime;
    uint32 quantity;
    uint32 propertyFlags;
};
typedef uint32 GameTagId;
struct GameTag {  // size 0x20
    GameTagId m_tagId;
    CRefString m_tag;
};
}  // namespace OnlineService

namespace OnlineServiceServer {
typedef uint32 LicenseId;
struct License {  // size 0x28
    License();
    LicenseId id;
    Blizzard::Time::Timestamp expireTime;
    uint64 transactionId;
    bool consumed;
    bool has_consumed;
    bool revoked;
    uint64 instanceId;
};
}  // namespace OnlineServiceServer

namespace Console {
struct AnalogStickInputFilter {
    struct Rule {  // size 0x14
        typedef int32 ID;
        ID id;
        Vector2D vecMinStickPos;
        Vector2D vecMaxStickPos;
    };
};
}  // namespace Console

namespace UICharacterDetails { namespace Console {
struct StatListEntry {  // size 0x50
    StatListEntry();
    int32 nIndex;
    CRefString strDesc;
    CRefString strValue;
    CRefString strTip;
};
} }  // namespace UICharacterDetails::Console

namespace UIConsoleDesignerMenu {
class ConsoleCommandOption {  // size 0x40
public:
    ConsoleCommandOption();
    CRefString m_displayText;
    CRefString m_command;
    BOOL m_bCloseOnSelect;
    BOOL bToggledOn;
    BOOL bHasBeenSelected;
};
class QuestMenuOption {  // size 0x38
public:
    QuestMenuOption();
    SNO m_sno;
    CRefString m_displayName;
    CRefString m_data;
};
}  // namespace UIConsoleDesignerMenu

namespace UIEventQueue { namespace Console {
struct CriticalMessage {  // size 0x38
    CRefString sMessage;
    CRefString sMessage2;
    ImageTextureFrame tImage;
};
} }  // namespace UIEventQueue::Console

namespace UIOptions { namespace Console {
enum SelectionType { SEL_0 };
enum OptionID { OPT_0 };
class Selection {  // size 0x48
public:
    Selection();
    SelectionType m_eType;
    CRefString m_sDisplayText;
    CRefString m_sToolTip;
    BOOL m_bSelected;
    OptionID m_eOptionID;
    BOOL m_bChecked;
    BOOL m_bWrap;
};
} }  // namespace UIOptions::Console

namespace UIPause { namespace Console {
enum PauseOptionID { PO_0 };
struct PauseOption {  // size 0x28
    PauseOptionID eId;
    CRefString sDisplayText;
    BOOL bShowLeaf;
};
} }  // namespace UIPause::Console

namespace UIEnchant {
struct AffixInfo {  // size 0x20
    GBID gbidAffix;
    BOOL bIsInherentAffix;
    CRefString sAffixTooltipText;
    AffixInfo();
};
}  // namespace UIEnchant

enum DRLGCommandType { DC_0 };
enum DRLGErrorResolution { DE_0 };
enum DRLGTileType { DT_0 };
enum DRLGStage { DS_0 };
struct DRLGTestEvent {  // size 0x38
    enum Status { STATUS_NONE = 0 };
    DRLGTestEvent();
    uint32 m_dwSeed;
    DRLGCommandType m_eCommandType;
    Status m_eStatus;
    DRLGErrorResolution m_eResolution;
    DRLGTileType m_eTileType;
    int32 m_nCommandID;
    CRefString m_sFailureReason;
    DRLGStage m_eDRLGStage;
};


class Player;
namespace UIBuffs {
struct SortedBuff {  // size 0x28
    SNO snoPower;
    int32 nBuffIdx;
    const Player* ptPlayer;
    int32 nDuration;
    int32 nStackCount;
    BOOL bPlayerCanCancel;
    BOOL fIsDebuff;
    BOOL fIsHidden;
};
}  // namespace UIBuffs

namespace UICharacterDetails { namespace Console {
struct BuffData {  // size 0x38
    UIID idBuff;
    UIID idBuffText;
    UIBuffs::SortedBuff tBuff;
};
} }  // namespace UICharacterDetails::Console

class XFourCC {  // size 0x4
public:
    XFourCC();
    XFourCC(const XFourCC& f) : m_value(f.m_value) {}
    XFourCC& operator=(const XFourCC& f);
private:
    uint32 m_value;
};

namespace OnlineService {
typedef uint32 GameAccountId;
typedef uint64 HeroId;
struct GameAccountHandle {  // size 0xc
    GameAccountHandle();
    GameAccountId m_id;
    XFourCC m_program;
    uint32 m_region;
};
}  // namespace OnlineService

struct PvpPlayerResult {  // size 0x68
    OnlineService::GameAccountHandle tAccountId;
    OnlineService::HeroId uHeroId;
    uint32 gbidClass;
    uint64 nTeamId;
    int eTeam;
    float teamRating;
    float teamRatingVariance;
    float individualRating;
    float individualRatingVariance;
    bool win;
    uint32 arena;
    uint32 score;
    int numTakedowns;
    int nDamage;
    int nRank;
    int nTitle;
    int nBadge;
    int nGlory;
    bool isFemale;
};

namespace google { namespace protobuf {
class Message {  // size 0x8 (vtable only)
public:
    virtual ~Message();
};
} }  // namespace google::protobuf

namespace D3 { namespace Leaderboard {
class Score : public google::protobuf::Message {  // size 0x50
public:
    Score(const Score&);
    virtual ~Score();
    Score& operator=(const Score& from);
    char _fields_[0x48];
};
} }  // namespace D3::Leaderboard

namespace D3 { namespace Items {
class Generator : public google::protobuf::Message {  // size 0xd8
public:
    Generator(const Generator&);
    virtual ~Generator();
    Generator& operator=(const Generator& from);
    char _fields_[0xd0];
};
} }  // namespace D3::Items

namespace UISeasonStash { namespace Console { struct SeasonStashItemProvider {
struct StashItemData {  // size 0xe8
    ACDID idItem;
    int32 nIndex;
    D3::Items::Generator tGenerator;
    Blizzard::Time::Timestamp nExpiration;
    StashItemData();
};
}; } }  // namespace UISeasonStash::Console

struct FormattedTextDelta;
// The original instantiates reserve in FormattedText.cpp, where the
// destructor is inline (Cleanup / ClearDeltas, lines 958-1448 there).
void sDeleteFormattedTextDelta(FormattedTextDelta*& pDelta);
class FormattedText {  // size 0x58
public:
    FormattedText(const FormattedText&);
    FormattedText& operator=(const FormattedText& tFormattedText);
    FormattedText(FormattedText&&);
    FormattedText& operator=(FormattedText&& tFormattedText);
    ~FormattedText() { Cleanup(); }
    void Cleanup() {
        m_sOriginalText = "";
        m_szOriginalText = 0;
        m_nOriginalLength = 0;
        ClearDeltas();
    }
    void ClearDeltas() {
        if (m_arTextDeltas.size() > 0) {
            for (uint32 i = 0; i < m_arTextDeltas.size(); ++i)
                sDeleteFormattedTextDelta(m_arTextDeltas[i]);
            m_arTextDeltas.clear();
            ComputeHash();
        }
    }
    void ComputeHash();
    LPCSTR m_szOriginalText;
    CRefString m_sOriginalText;
    uint32 m_nOriginalLength;
    blz::vector<FormattedTextDelta*> m_arTextDeltas;
    uint64 m_dwHash;
    BOOL m_fPassword;
    BOOL m_fContainsBreakHints;
    BOOL m_fContainsIcons;
};

class XVarBase;
class XVariant {  // size 0x30
public:
    XVariant(const XVariant&);
    XVariant(XVariant&&);
    ~XVariant();
    XVariant& operator=(const XVariant& v);
    XVariant& operator=(XVariant&& v);
    int m_type;
    union Value {
        bool m_bool;
        int64 m_int;
        uint64 m_uint;
        double m_float;
        char m_data[40];
    } m_value;
};

class XVarCollection {
public:
    struct TrackedXVar {  // size 0x38
        TrackedXVar(XVarBase* ptVarLocal, const XVariant& tVariantLocal)
            : ptVar(ptVarLocal), tDefaultValue(tVariantLocal) {}
        XVarBase* ptVar;
        XVariant tDefaultValue;
    };
};

namespace OnlineService {
// Ref-counted functor (X/Common/OnlineService/Callback/Callback_Impl.h).
// Only the inline copy / destroy paths are written out.
template <typename FT>
class Callback {
public:
    struct node_t {
        void* vtable;
        void (*destroy)(void*);
        int refs;
        int pad;
        char data[8];
    };
    Callback(const Callback& other) : m_impl(other.m_impl) { AddRef(m_impl); }
    ~Callback() { Release(m_impl); }
    Callback& operator=(const Callback&);
    static void Free(node_t* node) {
        node->destroy(node->data);
        delete[] reinterpret_cast<char*>(node);
    }
    static void AddRef(node_t* node) {
        if (node)
            __atomic_add_fetch(&node->refs, 1, __ATOMIC_SEQ_CST);
    }
    static void Release(node_t*& node) {
        if (node) {
            if (__atomic_sub_fetch(&node->refs, 1, __ATOMIC_SEQ_CST) == 0)
                Free(node);
            node = 0;
        }
    }
    node_t* m_impl;
};
}  // namespace OnlineService

namespace OnlineServiceServer {
typedef OnlineService::Callback<void(int, const google::protobuf::Message*)> ProtoMessageCallback;
}

struct ConsumableSaveRecord {  // size 0x20
    ConsumableSaveRecord();
    OnlineService::GameAccountHandle account_id;
    uint64 transaction_id;
    OnlineServiceServer::ProtoMessageCallback cb;
};

struct ErrorManagerBlizzardErrorMetaData {  // size 0x50
    blz::string metadata_key;
    blz::string metadata_value;
};

namespace google { namespace protobuf { namespace {
struct OptionsToInterpret {  // size 0x60
    OptionsToInterpret(const blz::string&, const blz::string&, const google::protobuf::Message*, google::protobuf::Message*);
    blz::string name_scope;
    blz::string element_name;
    const google::protobuf::Message* original_options;
    google::protobuf::Message* options;
};
} } }  // namespace google::protobuf::(anonymous)

namespace OnlineService { enum StoreEscrowState { ESCROW_0 }; }
struct EscrowSaveData {  // size 0x78
    EscrowSaveData();
    uint64 account_id;
    uint64 bnet_account_id;
    blz::string transaction_id;
    int64 amount;
    blz::string order_id;
    OnlineService::StoreEscrowState state;
    OnlineServiceServer::ProtoMessageCallback cb;
};

namespace D3 {
namespace Account { class SavedDefinition : public google::protobuf::Message {  // size 0xf8
public:
    SavedDefinition(const SavedDefinition&);
    virtual ~SavedDefinition();
    SavedDefinition& operator=(const SavedDefinition&);
    char _fields_[0xf0];
}; }
namespace Hero { class SavedDefinition : public google::protobuf::Message {  // size 0x88
public:
    SavedDefinition(const SavedDefinition&);
    virtual ~SavedDefinition();
    SavedDefinition& operator=(const SavedDefinition&);
    char _fields_[0x80];
}; }
namespace Items { class Mail : public google::protobuf::Message {  // size 0x68
public:
    Mail(const Mail&);
    virtual ~Mail();
    Mail& operator=(const Mail&);
    char _fields_[0x60];
}; }
namespace ChallengeRifts { class AccountData : public google::protobuf::Message {  // size 0x28
public:
    AccountData(const AccountData&);
    virtual ~AccountData();
    AccountData& operator=(const AccountData&);
    char _fields_[0x20];
}; }
}  // namespace D3

namespace SHero {
struct PlayerSaveData {  // size 0x1e8
    PlayerSaveData();
    OnlineService::GameAccountHandle idGameAccount;
    blz::string tHeroFilename;
    blz::string tAccountFilename;
    D3::Account::SavedDefinition tAccountDefinition;
    D3::Hero::SavedDefinition tHeroDefinition;
    uint32 uHeroSaveOptions;
};
struct ChallengeRiftPlayerSaveData {  // size 0xc8
    ChallengeRiftPlayerSaveData();
    OnlineService::GameAccountHandle idGameAccount;
    D3::Items::Mail tRewardMail;
    blz::string tSeenTutorials;
    D3::ChallengeRifts::AccountData tAccountData;
};
}  // namespace SHero
