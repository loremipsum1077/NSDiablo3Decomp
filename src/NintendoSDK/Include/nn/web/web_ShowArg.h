// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/web/web_ShowArg.h

#pragma once

#include "web_Types.h"
#include <stddef.h>
#include <stdint.h>

namespace nn { namespace account { struct Uid; } }

namespace nn {
namespace web {
class ShowWebPageArg {  // size 0x2000
public:
    static const size_t RequestUrlMaxLength = 3072;
    static const size_t CallbackUrlMaxLength = 1024;
    static const size_t CallbackableUrlMaxLength = 1024;
    static const size_t UserAgentAdditionalStringMaxLength = 128;
    explicit ShowWebPageArg(const char *);
    void SetBootAsMediaPlayer(bool);
    void SetCallbackUrl(const char *);
    void SetCallbackableUrl(const char *);
    void SetPointerEnabled(const bool);
    void SetLeftStickMode(const nn::web::WebLeftStickMode);
    void SetUid(const nn::account::Uid &);
    void SetFooterFixedKind(const nn::web::WebFooterFixedKind);
    void SetBackgroundKind(const nn::web::WebBackgroundKind);
    void SetBootDisplayKind(const nn::web::WebBootDisplayKind);
    void SetJsExtensionEnabled(const bool);
    void SetDisplayUrlKind(const nn::web::WebDisplayUrlKind);
    void SetTouchEnabledOnContents(const bool);
    void SetUserAgentAdditionalString(const char *);
    void SetMediaPlayerAutoCloseEnabled(const bool);
    void SetPageCacheEnabled(const bool);
    void SetPageFadeEnabled(const bool);
    void SetWebAudioEnabled(const bool);
    void SetPageScrollIndicatorEnabled(const bool);
    uint8_t * Data();
protected:
    static const size_t DataSize = 8192;
    uint8_t m_Data[8192];  // 0x00
private:
    ShowWebPageArg();
};
static_assert(sizeof(ShowWebPageArg) == 0x2000, "ShowWebPageArg");

class WebPageReturnValue {  // size 0x1010
public:
    static const size_t LastUrlMaxLength = 4096;
    WebPageReturnValue();
    nn::web::WebExitReason GetWebExitReason() const;
    const char * GetLastUrl() const;
    size_t GetLastUrlSize() const;
protected:
    uint64_t m_ExitReason;  // 0x00
    char m_LastUrl[4096];  // 0x08
    uint64_t m_LastUrlSize;  // 0x1008
};
static_assert(sizeof(WebPageReturnValue) == 0x1010, "WebPageReturnValue");

}  // namespace web
}  // namespace nn
