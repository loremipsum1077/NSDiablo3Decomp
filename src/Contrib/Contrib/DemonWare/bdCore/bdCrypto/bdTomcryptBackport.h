// Adopted from DWARF reference; original padding enumerators.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdTomcryptBackport.h

#pragma once

enum ltc_pkcs_1_paddings {
    LTC_PKCS_1_V1_5 = 1,
    LTC_PKCS_1_OAEP = 2,
    LTC_PKCS_1_PSS = 3,
};

enum ltc_pkcs_1_v1_5_blocks {
    LTC_PKCS_1_EMSA = 1,
    LTC_PKCS_1_EME = 2,
};

