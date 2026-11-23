// Original file: Externals/curl/siglo/curl_nintendo_proxy.cpp
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "NintendoSDK/Include/nn/nn_Result.h"

// NintendoSDK declarations (nn/nifm/nifm_TypesProxySetting.h, nifm_ApiProxy.h), layouts from the
// DWARF. GetCurrentProxySetting is an import, so nothing in the image checks its prototype.
namespace nn {
namespace nifm {
struct AuthenticationSetting {  // size 0x41
    static const size_t UsernameSize = 32;
    static const size_t PasswordSize = 32;
    bool isEnabled;  // 0x00
    char username[UsernameSize];  // 0x01
    char password[PasswordSize];  // 0x21
};

struct ProxySetting {  // size 0xaa
    static const size_t ProxyNameSize = 100;
    bool isEnabled;  // 0x00
    uint16_t port;  // 0x02
    char proxy[ProxyNameSize];  // 0x04
    AuthenticationSetting authentication;  // 0x68
};

nn::Result GetCurrentProxySetting(ProxySetting * pOutProxySetting);
}  // namespace nifm
}  // namespace nn

extern "C" {
// curl_nintendo_proxy.h: the C mirror of the nifm types.
typedef struct AuthenticationSetting {  // size 0x48
    enum {
        UsernameSize = 32,
        PasswordSize = 32
    } Sizes;  // 0x00
    bool isEnabled;  // 0x04
    char username[32];  // 0x05
    char password[32];  // 0x25
} AuthenticationSetting;

typedef struct ProxySetting {  // size 0xb4
    enum {
        ProxyNameSize = 100
    } Sizes;  // 0x00
    bool isEnabled;  // 0x04
    uint16_t port;  // 0x06
    char proxy[100];  // 0x08
    AuthenticationSetting authentication;  // 0x6c
} ProxySetting;

int curl_mfprintf(FILE * fd, const char * format, ...);

// line 28
int nnnifmGetCurrentProxySetting(ProxySetting * pOutProxySetting)
{
    nn::nifm::ProxySetting nifmProxySetting;
    memset(&nifmProxySetting, 0, sizeof(nifmProxySetting));
    nn::Result result = nn::nifm::GetCurrentProxySetting(&nifmProxySetting);
    if (result.IsFailure())
    {
        int error = result.GetDescription();
        curl_mfprintf(stderr, "\nError: nn::nifm::GetCurrentProxySetting() failed. Err Desc: %d\n\n", error);
        return error;
    }
    pOutProxySetting->isEnabled = nifmProxySetting.isEnabled;
    if (nifmProxySetting.isEnabled)
    {
        pOutProxySetting->port = nifmProxySetting.port;
        strncpy(pOutProxySetting->proxy, nifmProxySetting.proxy, ProxySetting::ProxyNameSize - 1);
        pOutProxySetting->proxy[ProxySetting::ProxyNameSize - 1] = '\0';
        pOutProxySetting->authentication.isEnabled = nifmProxySetting.authentication.isEnabled;
        if (nifmProxySetting.authentication.isEnabled)
        {
            strncpy(pOutProxySetting->authentication.username, nifmProxySetting.authentication.username,
                    AuthenticationSetting::UsernameSize - 1);
            pOutProxySetting->authentication.username[AuthenticationSetting::UsernameSize - 1] = '\0';
            strncpy(pOutProxySetting->authentication.password, nifmProxySetting.authentication.password,
                    AuthenticationSetting::PasswordSize - 1);
            pOutProxySetting->authentication.password[AuthenticationSetting::PasswordSize - 1] = '\0';
        }
    }
    return 0;
}
}  // extern "C"
