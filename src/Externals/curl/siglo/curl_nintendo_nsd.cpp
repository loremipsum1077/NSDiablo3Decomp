// Original file: Externals/curl/siglo/curl_nintendo_nsd.cpp
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "NintendoSDK/Include/nn/nn_Result.h"

// NintendoSDK declarations (nn/nsd/nsd_Common.h, nsd_ApiForMenu.h), layout from the DWARF.
// ResolveEx is an import, so nothing in the image checks its prototype.
namespace nn {
namespace nsd {
struct Fqdn {  // size 0x100
    char value[256];  // 0x00
};

nn::Result ResolveEx(Fqdn * pOut, const Fqdn & fqdn);
}  // namespace nsd
}  // namespace nn

extern "C" {
void * Curl_SigloMalloc(size_t size);
void Curl_SigloFree(void * ptr);
int curl_mfprintf(FILE * fd, const char * format, ...);

// line 30
int nnnsdResolveEx(char ** pOut, const char * pIn)
{
    int error = 0;
    nn::nsd::Fqdn * pNewFqdn;
    nn::nsd::Fqdn * pFqdn = static_cast<nn::nsd::Fqdn *>(Curl_SigloMalloc(sizeof(nn::nsd::Fqdn)));
    memset(pFqdn, 0, sizeof(nn::nsd::Fqdn));
    pNewFqdn = static_cast<nn::nsd::Fqdn *>(Curl_SigloMalloc(sizeof(nn::nsd::Fqdn)));
    memset(pNewFqdn, 0, sizeof(nn::nsd::Fqdn));
    if (pFqdn == NULL || pNewFqdn == NULL)
    {
        curl_mfprintf(stderr, "\nError: Failed to allocate memory for nn::nsd::ResolveEx().\n\n", error);
        error = -1;
    }
    else
    {
        strncpy(pFqdn->value, pIn, sizeof(pFqdn->value) - 1);
        pFqdn->value[sizeof(pFqdn->value) - 1] = '\0';
        nn::Result result = nn::nsd::ResolveEx(pNewFqdn, *pFqdn);
        if (result.IsFailure())
        {
            error = result.GetDescription();
            curl_mfprintf(stderr, "\nError: nn::nsd::ResolveEx() failed. Err Desc: %d\n\n", error);
        }
        else if (pOut != NULL && *pOut != NULL)
        {
            strncpy(*pOut, pNewFqdn->value, sizeof(pNewFqdn->value) - 1);
            (*pOut)[sizeof(pNewFqdn->value) - 1] = '\0';
        }
    }
    if (pNewFqdn != NULL)
    {
        Curl_SigloFree(pNewFqdn);
    }
    if (pFqdn != NULL)
    {
        Curl_SigloFree(pFqdn);
    }
    return error;
}
}  // extern "C"
