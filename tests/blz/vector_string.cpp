// Template checks for blz::vector: element types holding blz::string.
#include "Contrib/Contrib/blz/include/blz/vector.h"
#include "vector_types.h"

template void blz::vector<ErrorManagerBlizzardErrorMetaData>::push_back(const ErrorManagerBlizzardErrorMetaData&);
template void blz::vector<ErrorManagerBlizzardErrorMetaData>::_transfer(ErrorManagerBlizzardErrorMetaData*);
// OptionsToInterpret lives in an anonymous namespace, so its instantiation
// has internal linkage: keep it alive through an external function that
// takes its address.
typedef google::protobuf::OptionsToInterpret OptionsToInterpret_t;
void keep_OptionsToInterpret_push_back(void* out) {
    void (blz::vector<OptionsToInterpret_t>::*f)(OptionsToInterpret_t&&) =
        &blz::vector<OptionsToInterpret_t>::push_back;
    memcpy(out, &f, sizeof(f));
}
template void blz::vector<EscrowSaveData>::swap(blz::vector<EscrowSaveData>&);
template void blz::vector<EscrowSaveData>::_destroy();
template void blz::vector<SHero::PlayerSaveData>::push_back(SHero::PlayerSaveData&&);
template void blz::vector<SHero::PlayerSaveData>::_transfer(SHero::PlayerSaveData*);
template void blz::vector<SHero::ChallengeRiftPlayerSaveData>::push_back(SHero::ChallengeRiftPlayerSaveData&&);
template void blz::vector<SHero::ChallengeRiftPlayerSaveData>::_transfer(SHero::ChallengeRiftPlayerSaveData*);
template void blz::vector<blz::string>::reserve(size_t);
