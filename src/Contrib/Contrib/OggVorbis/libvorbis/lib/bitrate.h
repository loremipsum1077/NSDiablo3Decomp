/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: bitrate tracking and management
 last mod: $Id: bitrate.h 13293 2007-07-24 00:09:47Z xiphmont $

 ********************************************************************/

#ifndef _V_BITRATE_H_
#define _V_BITRATE_H_

#include "vorbis/codec.h"
#include "codec_internal.h"
#include "os.h"

/* encode side bitrate tracking */
typedef struct bitrate_manager_state {
  int            managed;

  long           avg_reservoir;
  long           minmax_reservoir;
  long           avg_bitsper;
  long           min_bitsper;
  long           max_bitsper;

  long           short_per_long;
  double         avgfloat;

  vorbis_block  *vb;
  int            choice;
} bitrate_manager_state;

typedef struct bitrate_manager_info{
  long           avg_rate;
  long           min_rate;
  long           max_rate;
  long           reservoir_bits;
  double         reservoir_bias;

  double         slew_damp;

} bitrate_manager_info;

extern void vorbis_bitrate_init(vorbis_info *vi,bitrate_manager_state *bs);
extern void vorbis_bitrate_clear(bitrate_manager_state *bs);
extern int vorbis_bitrate_managed(vorbis_block *vb);
extern int vorbis_bitrate_addblock(vorbis_block *vb);
extern int vorbis_bitrate_flushpacket(vorbis_dsp_state *vd, ogg_packet *op);


// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(struct bitrate_manager_state) == 0x50, "bitrate_manager_state");
_Static_assert(offsetof(struct bitrate_manager_state, managed) == 0x0, "bitrate_manager_state.managed");
_Static_assert(offsetof(struct bitrate_manager_state, avg_reservoir) == 0x8, "bitrate_manager_state.avg_reservoir");
_Static_assert(offsetof(struct bitrate_manager_state, minmax_reservoir) == 0x10, "bitrate_manager_state.minmax_reservoir");
_Static_assert(offsetof(struct bitrate_manager_state, avg_bitsper) == 0x18, "bitrate_manager_state.avg_bitsper");
_Static_assert(offsetof(struct bitrate_manager_state, min_bitsper) == 0x20, "bitrate_manager_state.min_bitsper");
_Static_assert(offsetof(struct bitrate_manager_state, max_bitsper) == 0x28, "bitrate_manager_state.max_bitsper");
_Static_assert(offsetof(struct bitrate_manager_state, short_per_long) == 0x30, "bitrate_manager_state.short_per_long");
_Static_assert(offsetof(struct bitrate_manager_state, avgfloat) == 0x38, "bitrate_manager_state.avgfloat");
_Static_assert(offsetof(struct bitrate_manager_state, vb) == 0x40, "bitrate_manager_state.vb");
_Static_assert(offsetof(struct bitrate_manager_state, choice) == 0x48, "bitrate_manager_state.choice");
_Static_assert(sizeof(struct bitrate_manager_info) == 0x30, "bitrate_manager_info");
_Static_assert(offsetof(struct bitrate_manager_info, avg_rate) == 0x0, "bitrate_manager_info.avg_rate");
_Static_assert(offsetof(struct bitrate_manager_info, min_rate) == 0x8, "bitrate_manager_info.min_rate");
_Static_assert(offsetof(struct bitrate_manager_info, max_rate) == 0x10, "bitrate_manager_info.max_rate");
_Static_assert(offsetof(struct bitrate_manager_info, reservoir_bits) == 0x18, "bitrate_manager_info.reservoir_bits");
_Static_assert(offsetof(struct bitrate_manager_info, reservoir_bias) == 0x20, "bitrate_manager_info.reservoir_bias");
_Static_assert(offsetof(struct bitrate_manager_info, slew_damp) == 0x28, "bitrate_manager_info.slew_damp");

#endif
