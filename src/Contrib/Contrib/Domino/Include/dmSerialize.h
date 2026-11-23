// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmSerialize.h

#pragma once

typedef signed char dm_int8;

typedef short dm_int16;

typedef int dm_int32;

typedef long long int dm_int64;

typedef unsigned char dm_uint8;

typedef unsigned short dm_uint16;

typedef unsigned int dm_uint32;

typedef long long unsigned int dm_uint64;

typedef float dm_float32;

struct dmFloat3 {  // size 0xc
    void Set(dm_float32, dm_float32, dm_float32);
    dm_float32 x;  // 0x00
    dm_float32 y;  // 0x04
    dm_float32 z;  // 0x08
};
static_assert(sizeof(dmFloat3) == 0xc, "dmFloat3");

struct dmFloat4 {  // size 0x10
    void Set(dm_float32, dm_float32, dm_float32, dm_float32);
    dm_float32 x;  // 0x00
    dm_float32 y;  // 0x04
    dm_float32 z;  // 0x08
    dm_float32 w;  // 0x0c
};
static_assert(sizeof(dmFloat4) == 0x10, "dmFloat4");

struct dmPlane {  // size 0x10
    dmFloat3 normal;  // 0x00
    dm_float32 offset;  // 0x0c
};
static_assert(sizeof(dmPlane) == 0x10, "dmPlane");

