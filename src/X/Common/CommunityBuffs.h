// New in 2.7.7 (no 2.6.2 counterpart): the "community buff" table, the code at
// 0x71004a5ac0..0x71004a8140. Each buff is a server-config key ("CommunityBuffXP", ...)
// with a matching "Perk" cvar ("CommunityEventXP", ...), a UI label and, for most, a
// player attribute that the community event power (Community_Event_Buff_EXPMF) grants as
// buff index <id>.
//
// The buff methods keep the binary's unnamed symbols (asm labels) so tools/try_file.py and
// tools/check compare them; constructors, destructors and the per-literal-size creation
// templates cannot carry a label and are listed with their address instead.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/string.h"
#include "../Sigma/DataStructures/XVar.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/RefString.h"
#include "Attributes/FastAttributeStructs.h"

class ActorCommonData;
typedef int32 PowerID;
typedef int32 SNO;

typedef XGrowableMap<CRefString, const CRefString &, CRefString, const CRefString &> KeyValueMap;

// The id is the buff's slot in CommunityBuffs and its buff index on the community power.
enum CommunityBuffType {
    COMMUNITY_BUFF_NONE = 0,                  // buff index 0: "a community event is running"
    COMMUNITY_BUFF_XP = 1,
    COMMUNITY_BUFF_LEGENDARY_FIND = 2,
    COMMUNITY_BUFF_GOLD_FIND = 3,
    COMMUNITY_BUFF_DOUBLE_RIFT_KEYSTONES = 4,
    COMMUNITY_BUFF_DOUBLE_BLOOD_SHARDS = 5,
    COMMUNITY_BUFF_DOUBLE_GOBLINS = 6,
    COMMUNITY_BUFF_DOUBLE_BOUNTY_BAGS = 7,
    COMMUNITY_BUFF_ROYAL_GRANDEUR = 8,
    COMMUNITY_BUFF_LEGACY_OF_NIGHTMARES = 9,
    COMMUNITY_BUFF_TRIUNES_WILL = 10,
    COMMUNITY_BUFF_PANDEMONIUM = 11,
    COMMUNITY_BUFF_KANAI_POWERS = 12,
    COMMUNITY_BUFF_TRIALS_OF_TEMPESTS = 13,
    COMMUNITY_BUFF_SHADOW_CLONES = 14,
    COMMUNITY_BUFF_FOURTH_KANAIS_CUBE_SLOT = 15,
    COMMUNITY_BUFF_ETHEREAL_ITEMS = 16,
    COMMUNITY_BUFF_SOUL_SHARDS = 17,
    COMMUNITY_BUFF_SWARM_RIFTS = 18,
    COMMUNITY_BUFF_SANCTIFIED_ITEMS = 19,
    COMMUNITY_BUFF_DARK_ALCHEMY = 20,
    COMMUNITY_BUFF_PARAGON_CAP = 21,
    COMMUNITY_BUFF_NESTING_PORTALS = 22,
    NUM_COMMUNITY_BUFFS = 23,
};

// Abstract base, vtable 0x71010ab7a0.
class CommunityBuff {
public:
    CommunityBuff(int32 nId, FastAttribKey tAttribKey, const blz::string & sConfigKey,
                  const blz::string & sLabel);  // 0x71004a5ac0
    // D2 0x71004a7b40 (the derived classes' D1/D2 alias it); D0 0x71004a7ba0 (trap).
    virtual ~CommunityBuff() {}

    // Adds buff index m_nId of the power idPower to the power's owner, with the value in the
    // buff's attribute when it has one.
    virtual BOOL Apply(PowerID idPower) = 0;
    // The toggle's own cvar; always FALSE for the scalar buffs.
    virtual BOOL IsFeatureEnabled() const = 0;
    // The parsed/loaded value (what the client shows and the server grants).
    virtual BOOL IsActive() const = 0;
    // Whether the cvar (what the game was told) has the buff on.
    virtual BOOL IsActiveInCvar() const = 0;
    virtual void Reset() = 0;
    virtual void LoadFromCvar() = 0;
    virtual blz::string GetDescription() const = 0;
    virtual void ParseValue(const blz::string & sValue) = 0;
    virtual void StoreToCvar() const = 0;

    void ReadConfig(const KeyValueMap & mapKeyValues) __asm__("sub_71004a5c80");

    int32 m_nId;                 // 0x08
    FastAttribKey m_tAttribKey;  // 0x0c, -1 when the buff sets no attribute
    blz::string m_sConfigKey;    // 0x10
    blz::string m_sLabel;        // 0x38, a StringList label
};

// An on/off buff backed by a Perk XVarBool, vtable 0x71010ab808; D0 0x71004a7aa0.
// m_bAlwaysOn buffs (Dark Alchemy, Nesting Portals) report m_nDefault whatever the config says.
class CommunityBuffToggle : public CommunityBuff {
public:
    CommunityBuffToggle(int32 nId, XVarBool * pCvar, FastAttribKey tAttribKey,
                        const blz::string & sConfigKey, const blz::string & sLabel,
                        BOOL bAlwaysOn = FALSE)
        : CommunityBuff(nId, tAttribKey, sConfigKey, sLabel),
          m_bAlwaysOn(bAlwaysOn), m_nDefault(bAlwaysOn), m_nValue(bAlwaysOn), m_pCvar(pCvar) {}

    int32 GetValue() const { return m_bAlwaysOn ? m_nDefault : m_nValue; }

    virtual BOOL Apply(PowerID idPower) __asm__("sub_71004a5d60");
    virtual BOOL IsFeatureEnabled() const __asm__("sub_71004a5e20");
    virtual BOOL IsActive() const __asm__("sub_71004a5e40");
    virtual BOOL IsActiveInCvar() const __asm__("sub_71004a5e60");
    virtual void Reset() __asm__("sub_71004a5e80");
    virtual void LoadFromCvar() __asm__("sub_71004a5e90");
    virtual blz::string GetDescription() const __asm__("sub_71004a5ec0");
    virtual void ParseValue(const blz::string & sValue) __asm__("sub_71004a5f10");
    virtual void StoreToCvar() const __asm__("sub_71004a5f40");

    BOOL m_bAlwaysOn;    // 0x60
    int32 m_nDefault;    // 0x64
    int32 m_nValue;      // 0x68
    XVarBool * m_pCvar;  // 0x70
};

// A percentage buff backed by a Perk XVarFloat, vtable 0x71010ab870; D0 0x71004a7af0.
class CommunityBuffScalar : public CommunityBuff {
public:
    // The original initializes m_flValue from m_flDefault before m_flDefault is set (the
    // constructors in 0x71004a61d0 copy the uninitialized word at +0x64 into +0x60). Every
    // path parses, loads or resets the value before it is read.
    CommunityBuffScalar(int32 nId, XVarFloat * pCvar, FastAttribKey tAttribKey,
                        const blz::string & sConfigKey, const blz::string & sLabel)
        : CommunityBuff(nId, tAttribKey, sConfigKey, sLabel),
          m_flValue(m_flDefault), m_flDefault(0.0f), m_pCvar(pCvar) {}

    virtual BOOL Apply(PowerID idPower) __asm__("sub_71004a5f70");
    virtual BOOL IsFeatureEnabled() const __asm__("sub_71004a6020");
    virtual BOOL IsActive() const __asm__("sub_71004a6030");
    virtual BOOL IsActiveInCvar() const __asm__("sub_71004a6050");
    virtual void Reset() __asm__("sub_71004a6080");
    virtual void LoadFromCvar() __asm__("sub_71004a6090");
    virtual blz::string GetDescription() const __asm__("sub_71004a60c0");
    virtual void ParseValue(const blz::string & sValue) __asm__("sub_71004a6180");
    virtual void StoreToCvar() const __asm__("sub_71004a61b0");

    float m_flValue;      // 0x60
    float m_flDefault;    // 0x64
    XVarFloat * m_pCvar;  // 0x68
};

// Owning pointer (the binary's unique_ptr-like holder). The buffs are created through
// internal-linkage temporaries in CommunityBuffs.cpp (NewCommunityBuff), templates on the two
// string-literal sizes.
class CommunityBuffPtr {
public:
    CommunityBuffPtr() : m_p(NULL) {}
    explicit CommunityBuffPtr(CommunityBuff * p) : m_p(p) {}
    ~CommunityBuffPtr() {
        if (m_p)
            delete m_p;
    }
    CommunityBuffPtr & operator=(CommunityBuffPtr && tOther) {
        CommunityBuff * p = tOther.m_p;
        tOther.m_p = NULL;
        CommunityBuff * pOld = m_p;
        m_p = p;
        if (pOld)
            delete pOld;
        return *this;
    }
    CommunityBuff * operator->() const { return m_p; }
    CommunityBuff * Get() const { return m_p; }

private:
    CommunityBuffPtr(const CommunityBuffPtr &);
    CommunityBuff * m_p;
};

// The table of all community buffs. One copy lives in LobbyService.cpp (parsed from the
// server config file and pushed into the Perk cvars), one in SPlayer.cpp (server: decides
// which perk powers a player gets), a function-local one in UIConsoleMainMenu.cpp (main menu
// text) and a temporary one in Community_Event_Buff. The destructor is implicit (inline; its
// out-of-line copy is 0x710005fce0, first emitted by LobbyService.cpp).
class CommunityBuffs {
public:
    CommunityBuffs();  // 0x71004a61d0

    void ResetAll() __asm__("sub_71004a6fa0");
    BOOL IsFeatureEnabled(int32 nType) const __asm__("sub_71004a7120");
    void Apply(PowerID idPower) const __asm__("sub_71004a7130");
    BOOL AnyActive() const __asm__("sub_71004a71f0");
    // TRUE when some buff's cvar state differs from whether snoPower's buff index <id> is on
    // tACD: the perk power must be removed and re-added.
    BOOL NeedsRefresh(const ActorCommonData & tACD, SNO snoPower) const __asm__("sub_71004a73e0");
    void LoadFromCvars() __asm__("sub_71004a7480");
    blz::string GetActiveDescriptions() const __asm__("sub_71004a7600");
    void ReadConfig(const KeyValueMap & mapKeyValues) __asm__("sub_71004a7800");
    void StoreToCvars() const __asm__("sub_71004a7920");

    CommunityBuffPtr m_apBuffs[NUM_COMMUNITY_BUFFS];  // 0x00; [0] stays empty
};
static_assert(sizeof(CommunityBuffs) == 0xb8, "CommunityBuffs");

// "Perk" cvars defined in CommunityBuffs.cpp and used elsewhere (via the GOT in 2.7.7).
extern XVarBool s_varIgrPerkEnabled;
extern XVarFloat s_varIgrPerkXP;
extern XVarBool s_varAnniversaryPerkEnabled;
extern XVarBool s_varEasterEggWorldPerkEnabled;
extern XVarBool s_varCommunityEventPerkEnabled;
extern XVarBool s_varCommunityEventPerkSeasonOnly;
