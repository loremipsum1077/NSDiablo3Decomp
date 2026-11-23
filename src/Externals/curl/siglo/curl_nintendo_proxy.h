/*
 * Nintendo SDK curl glue: the system proxy setting as curl sees it
 * (curl_nintendo_proxy.h; layouts from the original DWARF).
 */
#ifndef D3_RECOVERED_EXTERNALS_CURL_SIGLO_CURL_NINTENDO_PROXY_H
#define D3_RECOVERED_EXTERNALS_CURL_SIGLO_CURL_NINTENDO_PROXY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    enum
    {
        UsernameSize = 32,
        PasswordSize = 32
    } Sizes;
    _Bool isEnabled;
    char username[UsernameSize];
    char password[PasswordSize];
} AuthenticationSetting;

typedef struct
{
    enum
    {
        ProxyNameSize = 100
    } Sizes;
    _Bool isEnabled;
    uint16_t port;
    char proxy[ProxyNameSize];
    AuthenticationSetting authentication;
} ProxySetting;

int nnnifmGetCurrentProxySetting(ProxySetting *pOutProxySetting);

#ifdef __cplusplus
}
#endif


// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(AuthenticationSetting) == 0x48, "AuthenticationSetting");
_Static_assert(offsetof(AuthenticationSetting, isEnabled) == 0x4, "AuthenticationSetting.isEnabled");
_Static_assert(offsetof(AuthenticationSetting, username) == 0x5, "AuthenticationSetting.username");
_Static_assert(offsetof(AuthenticationSetting, password) == 0x25, "AuthenticationSetting.password");
_Static_assert(sizeof(ProxySetting) == 0xb4, "ProxySetting");
_Static_assert(offsetof(ProxySetting, isEnabled) == 0x4, "ProxySetting.isEnabled");
_Static_assert(offsetof(ProxySetting, port) == 0x6, "ProxySetting.port");
_Static_assert(offsetof(ProxySetting, proxy) == 0x8, "ProxySetting.proxy");
_Static_assert(offsetof(ProxySetting, authentication) == 0x6c, "ProxySetting.authentication");

#endif /* D3_RECOVERED_EXTERNALS_CURL_SIGLO_CURL_NINTENDO_PROXY_H */
