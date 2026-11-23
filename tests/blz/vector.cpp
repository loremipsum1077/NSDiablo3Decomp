// Template checks for blz::vector: destructors, assignment, insert_raw,
// resize/reserve. Instantiations that share an inline callee (reserve,
// _transfer, ...) go in separate files: a second caller in the same TU
// changes clang's inlining decisions.
#include "Contrib/Contrib/blz/include/blz/vector.h"
#include "vector_types.h"

// Destructors.
template blz::vector<unsigned int>::~vector();
template blz::vector<ACDID>::~vector();
template blz::vector<UIID>::~vector();
template blz::vector<Console::Online::SeasonInfo>::~vector();
template blz::vector<void (*)(const char*, int, SeverityLevel, OutputStreamType, const char*, const char*)>::~vector();

// operator=.
template blz::vector<unsigned int>& blz::vector<unsigned int>::operator=(const blz::vector<unsigned int>&);
template blz::vector<blz::pair<unsigned long, unsigned long> >& blz::vector<blz::pair<unsigned long, unsigned long> >::operator=(const blz::vector<blz::pair<unsigned long, unsigned long> >&);
template blz::vector<UIID>& blz::vector<UIID>::operator=(const blz::vector<UIID>&);
template blz::vector<TransmuteInput>& blz::vector<TransmuteInput>::operator=(const blz::vector<TransmuteInput>&);
template blz::vector<OnlineService::ContentLicense>& blz::vector<OnlineService::ContentLicense>::operator=(const blz::vector<OnlineService::ContentLicense>&);
template blz::vector<OnlineServiceServer::License>& blz::vector<OnlineServiceServer::License>::operator=(const blz::vector<OnlineServiceServer::License>&);
template blz::vector<Console::AnalogStickInputFilter::Rule>& blz::vector<Console::AnalogStickInputFilter::Rule>::operator=(const blz::vector<Console::AnalogStickInputFilter::Rule>&);

// insert_raw.
template blz::vector<int>::insert_raw_ranges blz::vector<int>::insert_raw(const int*, size_t);
template blz::vector<Particle*>::insert_raw_ranges blz::vector<Particle*>::insert_raw(Particle* const*, size_t);
template blz::vector<FormattedTextDelta*>::insert_raw_ranges blz::vector<FormattedTextDelta*>::insert_raw(FormattedTextDelta* const*, size_t);
template blz::vector<UIStyleChildSpec*>::insert_raw_ranges blz::vector<UIStyleChildSpec*>::insert_raw(UIStyleChildSpec* const*, size_t);
template blz::vector<UIControlItem>::insert_raw_ranges blz::vector<UIControlItem>::insert_raw(const UIControlItem*, size_t);

// resize / reserve.
template void blz::vector<AsyncRender::AsyncRenderCallbackItem>::resize(size_t);
template void blz::vector<SpriteBucket>::resize(size_t);
template void blz::vector<QueuedFlippyInfo>::reserve(size_t);
template void blz::vector<QueuedFlippyInfo>::push_back(const QueuedFlippyInfo&);
template void blz::vector<AIQueuedPower>::reserve(size_t);
