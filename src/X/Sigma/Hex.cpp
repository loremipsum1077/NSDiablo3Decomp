#include "SigmaTypes.h"
#include "Unicode.h"

#include <stdio.h>

uint32 atoh(LPCSTR sz) {
    uint32 dwValue;
    if (sz == NULL || sz[0] == '\0')
        return 0;
    int32 nRetval = sscanf(sz, "%x", &dwValue);
    (void)nRetval;
    return dwValue;
}

uint32 atoh(LPCSTR sz, BOOL& bSuccess) {
    uint32 dwValue;
    if (sz == NULL || sz[0] == '\0')
        return 0;
    int32 nRetval = sscanf(sz, "%x", &dwValue);
    if (nRetval != 1) {
        bSuccess = FALSE;
        return 0;
    }
    bSuccess = TRUE;
    return dwValue;
}
