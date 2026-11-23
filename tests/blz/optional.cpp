// Checks blz::optional through Console::Online::PartyMember::Copy
// (X/Client/Console/Online.h:685), which assigns two optional<Digest>.
// The Digest types are protobuf messages (virtual destructor); stand-ins with
// the sizes from the DWARF (0xa0 and 0x78).
#include "Contrib/Contrib/blz/include/blz/optional.h"
#include <string.h>

namespace D3 { namespace Hero {
class Digest { public: Digest(const Digest&); virtual ~Digest(); private: char m_data[0xa0 - 8]; };
} }
namespace D3 { namespace Account {
class Digest { public: Digest(const Digest&); virtual ~Digest(); private: char m_data[0x78 - 8]; };
} }

namespace Console { namespace Online {
struct PartyMember {
    void Copy(PartyMember& tOther, int bDigests) const;
    unsigned char idPlayer;
    unsigned long idOnlineUser;
    char szName[128];
    int bIsLocal;
    int bIsPrimary;
    int bIsGuest;
    int bIsInGame;
    int nControllerIndex;
    blz::optional<D3::Hero::Digest> tHeroDigest;
    blz::optional<D3::Account::Digest> tAccountDigest;
};
static_assert(sizeof(PartyMember) == 0x1d0, "PartyMember");

void PartyMember::Copy(PartyMember& tOther, int bDigests) const {
    tOther.idPlayer = idPlayer;
    tOther.idOnlineUser = idOnlineUser;
    strncpy(tOther.szName, szName, 128);
    tOther.szName[127] = 0;
    tOther.bIsLocal = bIsLocal;
    tOther.bIsPrimary = bIsPrimary;
    tOther.bIsGuest = bIsGuest;
    tOther.bIsInGame = bIsInGame;
    tOther.nControllerIndex = nControllerIndex;
    if (bDigests) {
        tOther.tHeroDigest = tHeroDigest;
        tOther.tAccountDigest = tAccountDigest;
    }
}
} }
