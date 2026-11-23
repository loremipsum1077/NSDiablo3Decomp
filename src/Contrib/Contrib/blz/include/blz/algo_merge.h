#pragma once

// blz::merge: the usual two-range merge into `result`. Takes from the second
// range when comp(*first2, *first1) is true, otherwise from the first (so it
// is stable), then copies whatever is left of each range (blz::copy, lines 38
// and 39). Mangled template parameter order: merge<It1, It2, Out, Compare>.

#include "config.h"
#include "algo_seq.h"

namespace blz {

// The binary keeps the two loop-exit tests separate: each arm re-tests only
// its own range (matched by UIConsoleCharacterDetails.cpp's DisplayBuffs sort).
template <typename InputIt1, typename InputIt2, typename OutputIt, typename Compare>
OutputIt merge(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt result,
               Compare comp) {
    if (first1 != last1 && first2 != last2) {
        for (;;) {
            if (comp(*first2, *first1)) {
                *result++ = *first2++;
                if (first2 == last2)
                    break;
            } else {
                *result++ = *first1++;
                if (first1 == last1)
                    break;
            }
        }
    }
    result = blz::copy(first1, last1, result);
    return blz::copy(first2, last2, result);
}

}  // namespace blz
