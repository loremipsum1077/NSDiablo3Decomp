// Hand-written subset; the DWARF has no type for the math descriptor.
// Original file: Contrib/Contrib/DemonWare/external/libtomcrypt/src/headers/tomcrypt_math.h

#pragma once

// The descriptor is copied as a whole (ltc_mp = ltm_desc) and is 0x168 bytes in the image.
// Leading members follow the upstream descriptor (count_bits is called through +0x68);
// the remaining function pointers are not declared.
typedef struct {
    const char * name;  // 0x00
    int bits_per_digit;  // 0x08
    int (*init)(void ** a);  // 0x10
    int (*init_copy)(void ** dst, void * src);  // 0x18
    void (*deinit)(void * a);  // 0x20
    int (*neg)(void * src, void * dst);  // 0x28
    int (*copy)(void * src, void * dst);  // 0x30
    int (*set_int)(void * a, unsigned long n);  // 0x38
    unsigned long (*get_int)(void * a);  // 0x40
    unsigned long (*get_digit)(void * a, int n);  // 0x48
    int (*get_digit_count)(void * a);  // 0x50
    int (*compare)(void * a, void * b);  // 0x58
    int (*compare_d)(void * a, unsigned long n);  // 0x60
    int (*count_bits)(void * a);  // 0x68
    void * _unadopted[(0x168 - 0x70) / sizeof(void *)];  // 0x70
} ltc_math_descriptor;
static_assert(sizeof(ltc_math_descriptor) == 0x168, "ltc_math_descriptor");

extern "C" {
extern ltc_math_descriptor ltc_mp;
extern const ltc_math_descriptor ltm_desc;
}
