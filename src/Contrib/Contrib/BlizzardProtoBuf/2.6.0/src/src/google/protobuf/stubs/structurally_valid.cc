// Copyright 2005-2008 Google Inc. All Rights Reserved.
// Author: jrm@google.com (Jim Meehan)
// UTF-8 lookup tables from Google Protocol Buffers 2.6.0 (COPYING.txt).
// Layouts reconstructed from DWARF.
// Original file: Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/stubs/structurally_valid.cc

#include "common.h"

namespace google {
namespace protobuf {
namespace internal {
struct RemapEntry {  // size 0x4
    google::protobuf::uint8 delete_bytes;  // 0x00
    google::protobuf::uint8 add_bytes;  // 0x01
    google::protobuf::uint16 bytes_offset;  // 0x02
};
static_assert(sizeof(RemapEntry) == 0x4, "RemapEntry");

enum {
    kExitDstSpaceFull = 239,
    kExitIllegalStructure = 240,
    kExitOK = 241,
    kExitReject = 242,
    kExitReplace1 = 243,
    kExitReplace2 = 244,
    kExitReplace3 = 245,
    kExitReplace21 = 246,
    kExitReplace31 = 247,
    kExitReplace32 = 248,
    kExitReplaceOffset1 = 249,
    kExitReplaceOffset2 = 250,
    kExitReplace1S0 = 251,
    kExitSpecial = 252,
    kExitDoAgain = 253,
    kExitRejectAlt = 254,
    kExitNone = 255,
};

typedef struct {  // size 0x40
    const google::protobuf::uint32 state0;  // 0x00
    const google::protobuf::uint32 state0_size;  // 0x04
    const google::protobuf::uint32 total_size;  // 0x08
    const int max_expand;  // 0x0c
    const int entry_shift;  // 0x10
    const int bytes_per_entry;  // 0x14
    const google::protobuf::uint32 losub;  // 0x18
    const google::protobuf::uint32 hiadd;  // 0x1c
    const google::protobuf::uint8 * state_table;  // 0x20
    const google::protobuf::internal::RemapEntry * remap_base;  // 0x28
    const google::protobuf::uint8 * remap_string;  // 0x30
    const google::protobuf::uint8 * fast_state;  // 0x38
} UTF8StateMachineObj;

typedef google::protobuf::internal::UTF8StateMachineObj UTF8ScanObj;


#define X__ (kExitIllegalStructure)
#define RJ_ (kExitReject)
#line 94
static const uint8 utf8acceptnonsurrogates[] = {
// state[0] 0x000000 Byte 1
  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,

  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__,   1,   1,   1,   1,   1,   1,    1,   1,   1,   1,   1,   1,   1,   1,
  1,   1,   1,   1,   1,   1,   1,   1,    1,   1,   1,   1,   1,   1,   1,   1,
  2,   3,   3,   3,   3,   3,   3,   3,    3,   3,   3,   3,   3,   7,   3,   3,
  4,   5,   5,   5,   6, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

// state[1] 0x000080 Byte 2 of 2
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

// state[2] 0x000000 Byte 2 of 3
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
  1,   1,   1,   1,   1,   1,   1,   1,    1,   1,   1,   1,   1,   1,   1,   1,
  1,   1,   1,   1,   1,   1,   1,   1,    1,   1,   1,   1,   1,   1,   1,   1,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

// state[3] 0x001000 Byte 2 of 3
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

  1,   1,   1,   1,   1,   1,   1,   1,    1,   1,   1,   1,   1,   1,   1,   1,
  1,   1,   1,   1,   1,   1,   1,   1,    1,   1,   1,   1,   1,   1,   1,   1,
  1,   1,   1,   1,   1,   1,   1,   1,    1,   1,   1,   1,   1,   1,   1,   1,
  1,   1,   1,   1,   1,   1,   1,   1,    1,   1,   1,   1,   1,   1,   1,   1,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

// state[4] 0x000000 Byte 2 of 4
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
  3,   3,   3,   3,   3,   3,   3,   3,    3,   3,   3,   3,   3,   3,   3,   3,
  3,   3,   3,   3,   3,   3,   3,   3,    3,   3,   3,   3,   3,   3,   3,   3,
  3,   3,   3,   3,   3,   3,   3,   3,    3,   3,   3,   3,   3,   3,   3,   3,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

// state[5] 0x040000 Byte 2 of 4
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

  3,   3,   3,   3,   3,   3,   3,   3,    3,   3,   3,   3,   3,   3,   3,   3,
  3,   3,   3,   3,   3,   3,   3,   3,    3,   3,   3,   3,   3,   3,   3,   3,
  3,   3,   3,   3,   3,   3,   3,   3,    3,   3,   3,   3,   3,   3,   3,   3,
  3,   3,   3,   3,   3,   3,   3,   3,    3,   3,   3,   3,   3,   3,   3,   3,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

// state[6] 0x100000 Byte 2 of 4
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

  3,   3,   3,   3,   3,   3,   3,   3,    3,   3,   3,   3,   3,   3,   3,   3,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

// state[7] 0x00d000 Byte 2 of 3
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

  1,   1,   1,   1,   1,   1,   1,   1,    1,   1,   1,   1,   1,   1,   1,   1,
  1,   1,   1,   1,   1,   1,   1,   1,    1,   1,   1,   1,   1,   1,   1,   1,
  8,   8,   8,   8,   8,   8,   8,   8,    8,   8,   8,   8,   8,   8,   8,   8,
  8,   8,   8,   8,   8,   8,   8,   8,    8,   8,   8,   8,   8,   8,   8,   8,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

// state[8] 0x00d800 Byte 3 of 3
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,

RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_,  RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_,
RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_,  RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_,
RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_,  RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_,
RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_,  RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_, RJ_,

X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
X__, X__, X__, X__, X__, X__, X__, X__,  X__, X__, X__, X__, X__, X__, X__, X__,
};

#undef X__
#undef RJ_
#line 293
static const unsigned char utf8acceptnonsurrogates_fast[256] = {
0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,

1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,
};

#line 286
static const RemapEntry utf8acceptnonsurrogates_remap_base[1] = {
    {0, 0, 0}
};
#line 290
static const unsigned char utf8acceptnonsurrogates_remap_string[1] = {0};

#line 315
static const UTF8ScanObj utf8acceptnonsurrogates_obj = {
    0, 256, 2304, 0, 8, 1, 0x20202020, 0,
    utf8acceptnonsurrogates, utf8acceptnonsurrogates_remap_base,
    utf8acceptnonsurrogates_remap_string, utf8acceptnonsurrogates_fast
};
#line 348
static bool InStateZero(const UTF8ScanObj* st, const uint8* Tbl) {
  const uint8* Tbl0 = &st->state_table[st->state0];
  return (static_cast<uint32>(Tbl - Tbl0) < st->state0_size);
}
#line 355
// NON_MATCHING: 528 vs 544; identical through the 8-byte loop; the byte-at-a-time loop keeps src-1 as an extra induction variable in the original and the back-up loops are rotated differently; loop spellings tried give identical IR.
int UTF8GenericScan(const UTF8ScanObj* st,
                    const char * str,
                    int str_length,
                    int* bytes_consumed) {
  *bytes_consumed = 0;
  if (str_length == 0) return kExitOK;

  int eshift = st->entry_shift;
  const uint8* isrc = reinterpret_cast<const uint8*>(str);
  const uint8* src = isrc;
  const uint8* srclimit = isrc + str_length;
  const uint8* srclimit8 = srclimit - 7;
  const uint8* Tbl_0 = &st->state_table[st->state0];

 DoAgain:
  // Do state-table scan
  int e = 0;
  uint8 c;
  const uint8* Tbl2 = &st->fast_state[0];
  const uint32 losub = st->losub;
  const uint32 hiadd = st->hiadd;
  // Check initial few bytes one at a time until 8-byte aligned

  while ((((uintptr_t)src & 0x07) != 0) &&
         (src < srclimit) &&
         Tbl2[src[0]] == 0) {
    src++;
  }
  if (((uintptr_t)src & 0x07) == 0) {
    // Do fast for groups of 8 identity bytes.



    while (src < srclimit8) {
      uint32 s0123 = (reinterpret_cast<const uint32 *>(src))[0];
      uint32 s4567 = (reinterpret_cast<const uint32 *>(src))[1];
      src += 8;
      // This is a fast range check for all bytes in [lowsub..0x80-hiadd)
      uint32 temp = (s0123 - losub) | (s0123 + hiadd) |
                    (s4567 - losub) | (s4567 + hiadd);
      if ((temp & 0x80808080) != 0) {
        // We typically end up here on cr/lf/ht; src was incremented
        int e0123 = (Tbl2[src[-8]] | Tbl2[src[-7]]) |
                    (Tbl2[src[-6]] | Tbl2[src[-5]]);
        if (e0123 != 0) {
          src -= 8;
          break;
        }    // Exit on Non-interchange
        e0123 = (Tbl2[src[-4]] | Tbl2[src[-3]]) |
                (Tbl2[src[-2]] | Tbl2[src[-1]]);
        if (e0123 != 0) {
          src -= 4;
          break;
        }    // Exit on Non-interchange
        // Else OK, go around again
      }
    }
  }


  // Byte-at-a-time scan

  const uint8* Tbl = Tbl_0;
  while (src < srclimit) {
    c = *src;
    e = Tbl[c];
    src++;
    if (e >= kExitIllegalStructure) {break;}
    Tbl = &Tbl_0[e << eshift];
  }











  if (e >= kExitIllegalStructure) {
    // Back up over exit-causing byte
    src--;
    // Back up over last character if partial
    if (!InStateZero(st, Tbl)) {
      // Not in state0, back up over last char
      src--;
      while ((src > isrc) && ((src[0] & 0xc0) == 0x80)) { src--; }
    }
  } else if (InStateZero(st, Tbl)) {
    e = kExitOK;
  } else {
    // Exit at end of input, not in state0: back up over the partial character
    src--;
    while ((src > isrc) && ((src[0] & 0xc0) == 0x80)) { src--; }
    e = kExitIllegalStructure;
  }

  // 457 col 7 is the start of the test, and its branch goes back to the line-379 scan

  if (e == kExitDoAgain) {
    goto DoAgain;
  }


  *bytes_consumed = src - isrc;
  return e;
}
#line 466
int UTF8GenericScanFastAscii(const UTF8ScanObj* st,
                    const char * str,
                    int str_length,
                    int* bytes_consumed) {
  *bytes_consumed = 0;
  if (str_length == 0) return kExitOK;

  const uint8* isrc =  reinterpret_cast<const uint8*>(str);
  const uint8* src = isrc;
  const uint8* srclimit = isrc + str_length;
  const uint8* srclimit8 = srclimit - 7;
  int n;
  int rest_consumed;
  int exit_reason;
  do {
    // Check initial few bytes one at a time until 8-byte aligned
    while ((((uintptr_t)src & 0x07) != 0) &&
           (src < srclimit) && (src[0] < 0x80)) {
      src++;
    }
    if (((uintptr_t)src & 0x07) == 0) {
      while ((src < srclimit8) &&
             (((reinterpret_cast<const uint32*>(src)[0] |
                reinterpret_cast<const uint32*>(src)[1]) & 0x80808080) == 0)) {
        src += 8;
      }
    }
    while ((src < srclimit) && (src[0] < 0x80)) {
      src++;
    }
    // Run state table on the rest
    n = src - isrc;
    exit_reason = UTF8GenericScan(st, str + n, str_length - n, &rest_consumed);
    src += rest_consumed;
  } while ( exit_reason == kExitDoAgain );

  *bytes_consumed = src - isrc;
  return exit_reason;
}

#line 525
bool IsStructurallyValidUTF8(const char* buf, int len) {


    int bytes_consumed;
    UTF8GenericScanFastAscii(&utf8acceptnonsurrogates_obj, buf, len, &bytes_consumed);

    return bytes_consumed == len;
}
}
}
}
