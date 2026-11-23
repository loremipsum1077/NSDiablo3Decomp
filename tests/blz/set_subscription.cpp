// Compare original SubscriptionEvent instantiations using the production headers.
// This fixture is not part of the game build; tools/try_file.py compares its
// emitted original symbols. Achievements and party-display callers separately
// prove construction, lifetime and subscription through real source files.
#include "X/Common/OnlineService/SubscriptionEvent.h"

namespace OnlineService {
template class SubscriptionEvent<>;
template class SubscriptionEvent<int>;
}  // namespace OnlineService
