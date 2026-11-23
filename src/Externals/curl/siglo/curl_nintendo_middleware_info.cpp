// Original file: Externals/curl/siglo/curl_nintendo_middleware_info.cpp
// Nintendo's libcurl middleware tag. The SDK macro that defines the tag is not in the DWARF; the
// call it expands to is written out.

// NintendoSDK declaration (nn/util/util_ReferSymbol.h); an import, so nothing checks the prototype.
namespace nn {
namespace util {
void ReferSymbol(const void * symbol);
}  // namespace util
}  // namespace nn

extern "C" void Curl_SigloMiddlewareInfo()
{
    nn::util::ReferSymbol("SDK MW+Nintendo+NintendoSDK_libcurl-5_4_0-Release");
}
