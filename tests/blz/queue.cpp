// Template checks for blz/queue.h.
#include "Contrib/Contrib/blz/include/blz/queue.h"
#include "Contrib/Contrib/blz/include/blz/func_poly.h"

typedef blz::function<void()> VoidFunction;

// The binary's only out-of-line queue member is the implicit destructor. It
// is emitted when its address is taken, which a global's registration with
// __cxa_atexit does.
blz::queue<VoidFunction, blz::deque<VoidFunction> > g_queue;
