// Template checks for bdReference<T>. The binary has two out-of-line members, both
// destructors: bdReference<Console::Online::GameConnection> and <Console::Online::Party>.
#include "Contrib/Contrib/DemonWare/bdCore/bdReference/bdReferencable.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"

// Stand-ins: only "derives from bdReferencable at offset 0" matters to these functions.
// (try_file also reports Console::Online::GameConnection::~GameConnection D0 as OK; that is
// the stand-in's implicit destructor and says nothing about the real class.)
namespace Console {
namespace GameNetwork { class Connection : public bdReferencable { public: int a; }; }
namespace Online {
class GameConnection : public GameNetwork::Connection {};
class Party : public bdReferencable {};
}
}

template class bdReference<Console::Online::GameConnection>;
template class bdReference<Console::Online::Party>;

// Compile-only uses of the members that have no out-of-line copy (not in functions.csv).
void use_reference(bdReference<Console::Online::GameConnection>& a,
                   bdReference<Console::GameNetwork::Connection>& b,
                   Console::Online::GameConnection* p) {
    bdReference<Console::GameNetwork::Connection> c(a);
    bdReference<Console::Online::GameConnection> d(b);
    b = a;
    a = p;
    a = d;
    if (a == d || a != d || !a || a.isNull() || c.notNull())
        (*a).a = a->a + (*c).a;
    Console::Online::GameConnection* q = reference_cast<Console::Online::GameConnection, Console::GameNetwork::Connection>(b);
    a = q;
    a = bdMakeReference<Console::Online::GameConnection>();
    q = a;
    delete new bdReference<Console::Online::Party>();
}
