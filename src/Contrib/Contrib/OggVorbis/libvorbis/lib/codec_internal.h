/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2009             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: libvorbis codec headers
 last mod: $Id: codec_internal.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#ifndef _V_CODECI_H_
#define _V_CODECI_H_

#include "envelope.h"
#include "codebook.h"

#define BLOCKTYPE_IMPULSE    0
#define BLOCKTYPE_PADDING    1
#define BLOCKTYPE_TRANSITION 0
#define BLOCKTYPE_LONG       1

#define PACKETBLOBS 15

typedef struct vorbis_block_internal{
  float  **pcmdelay;  /* this is a pointer into local storage */
  float  ampmax;
  int    blocktype;

  oggpack_buffer *packetblob[PACKETBLOBS]; /* initialized, must be freed;
                                              blob [PACKETBLOBS/2] points to
                                              the oggpack_buffer in the
                                              main vorbis_block */
} vorbis_block_internal;

typedef void vorbis_look_floor;
typedef void vorbis_look_residue;
typedef void vorbis_look_transform;

/* mode ************************************************************/
typedef struct {
  int blockflag;
  int windowtype;
  int transformtype;
  int mapping;
} vorbis_info_mode;

typedef void vorbis_info_floor;
typedef void vorbis_info_residue;
typedef void vorbis_info_mapping;

#include "psy.h"
#include "bitrate.h"

typedef struct private_state {
  /* local lookup storage */
  envelope_lookup        *ve; /* envelope lookup */
  int                     window[2];
  vorbis_look_transform **transform[2];    /* block, type */
  drft_lookup             fft_look[2];

  int                     modebits;
  vorbis_look_floor     **flr;
  vorbis_look_residue   **residue;
  vorbis_look_psy        *psy;
  vorbis_look_psy_global *psy_g_look;

  /* local storage, only used on the encoding side.  This way the
     application does not need to worry about freeing some packets'
     memory and not others'; packet storage is always tracked.
     Cleared next call to a _dsp_ function */
  unsigned char *header;
  unsigned char *header1;
  unsigned char *header2;

  bitrate_manager_state bms;

  ogg_int64_t sample_count;
} private_state;

/* codec_setup_info contains all the setup information specific to the
   specific compression/decompression mode in progress (eg,
   psychoacoustic settings, channel setup, options, codebook
   etc).
*********************************************************************/

#include "highlevel.h"
typedef struct codec_setup_info {

  /* Vorbis supports only short and long blocks, but allows the
     encoder to choose the sizes */

  long blocksizes[2];

  /* modes are the primary means of supporting on-the-fly different
     blocksizes, different channel mappings (LR or M/A),
     different residue backends, etc.  Each mode consists of a
     blocksize flag and a mapping (along with the mapping setup */

  int        modes;
  int        maps;
  int        floors;
  int        residues;
  int        books;
  int        psys;     /* encode only */

  vorbis_info_mode       *mode_param[64];
  int                     map_type[64];
  vorbis_info_mapping    *map_param[64];
  int                     floor_type[64];
  vorbis_info_floor      *floor_param[64];
  int                     residue_type[64];
  vorbis_info_residue    *residue_param[64];
  static_codebook        *book_param[256];
  codebook               *fullbooks;

  vorbis_info_psy        *psy_param[4]; /* encode only */
  vorbis_info_psy_global psy_g_param;

  bitrate_manager_info   bi;
  highlevel_encode_setup hi; /* used only by vorbisenc.c.  It's a
                                highly redundant structure, but
                                improves clarity of program flow. */
  int         halfrate_flag; /* painless downsample for decode */
} codec_setup_info;

extern vorbis_look_psy_global *_vp_global_look(vorbis_info *vi);
extern void _vp_global_free(vorbis_look_psy_global *look);



typedef struct {
  int sorted_index[VIF_POSIT+2];
  int forward_index[VIF_POSIT+2];
  int reverse_index[VIF_POSIT+2];

  int hineighbor[VIF_POSIT];
  int loneighbor[VIF_POSIT];
  int posts;

  int n;
  int quant_q;
  vorbis_info_floor1 *vi;

  long phrasebits;
  long postbits;
  long frames;
} vorbis_look_floor1;



extern int *floor1_fit(vorbis_block *vb,vorbis_look_floor1 *look,
                          const float *logmdct,   /* in */
                          const float *logmask);
extern int *floor1_interpolate_fit(vorbis_block *vb,vorbis_look_floor1 *look,
                          int *A,int *B,
                          int del);
extern int floor1_encode(oggpack_buffer *opb,vorbis_block *vb,
                  vorbis_look_floor1 *look,
                  int *post,int *ilogmask);

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(struct vorbis_block_internal) == 0x88, "vorbis_block_internal");
_Static_assert(offsetof(struct vorbis_block_internal, pcmdelay) == 0x0, "vorbis_block_internal.pcmdelay");
_Static_assert(offsetof(struct vorbis_block_internal, ampmax) == 0x8, "vorbis_block_internal.ampmax");
_Static_assert(offsetof(struct vorbis_block_internal, blocktype) == 0xc, "vorbis_block_internal.blocktype");
_Static_assert(offsetof(struct vorbis_block_internal, packetblob) == 0x10, "vorbis_block_internal.packetblob");
_Static_assert(sizeof(vorbis_info_mode) == 0x10, "vorbis_info_mode");
_Static_assert(offsetof(vorbis_info_mode, blockflag) == 0x0, "vorbis_info_mode.blockflag");
_Static_assert(offsetof(vorbis_info_mode, windowtype) == 0x4, "vorbis_info_mode.windowtype");
_Static_assert(offsetof(vorbis_info_mode, transformtype) == 0x8, "vorbis_info_mode.transformtype");
_Static_assert(offsetof(vorbis_info_mode, mapping) == 0xc, "vorbis_info_mode.mapping");
_Static_assert(sizeof(struct private_state) == 0xe8, "private_state");
_Static_assert(offsetof(struct private_state, ve) == 0x0, "private_state.ve");
_Static_assert(offsetof(struct private_state, window) == 0x8, "private_state.window");
_Static_assert(offsetof(struct private_state, transform) == 0x10, "private_state.transform");
_Static_assert(offsetof(struct private_state, fft_look) == 0x20, "private_state.fft_look");
_Static_assert(offsetof(struct private_state, modebits) == 0x50, "private_state.modebits");
_Static_assert(offsetof(struct private_state, flr) == 0x58, "private_state.flr");
_Static_assert(offsetof(struct private_state, residue) == 0x60, "private_state.residue");
_Static_assert(offsetof(struct private_state, psy) == 0x68, "private_state.psy");
_Static_assert(offsetof(struct private_state, psy_g_look) == 0x70, "private_state.psy_g_look");
_Static_assert(offsetof(struct private_state, header) == 0x78, "private_state.header");
_Static_assert(offsetof(struct private_state, header1) == 0x80, "private_state.header1");
_Static_assert(offsetof(struct private_state, header2) == 0x88, "private_state.header2");
_Static_assert(offsetof(struct private_state, bms) == 0x90, "private_state.bms");
_Static_assert(offsetof(struct private_state, sample_count) == 0xe0, "private_state.sample_count");
_Static_assert(sizeof(struct codec_setup_info) == 0x1698, "codec_setup_info");
_Static_assert(offsetof(struct codec_setup_info, blocksizes) == 0x0, "codec_setup_info.blocksizes");
_Static_assert(offsetof(struct codec_setup_info, modes) == 0x10, "codec_setup_info.modes");
_Static_assert(offsetof(struct codec_setup_info, maps) == 0x14, "codec_setup_info.maps");
_Static_assert(offsetof(struct codec_setup_info, floors) == 0x18, "codec_setup_info.floors");
_Static_assert(offsetof(struct codec_setup_info, residues) == 0x1c, "codec_setup_info.residues");
_Static_assert(offsetof(struct codec_setup_info, books) == 0x20, "codec_setup_info.books");
_Static_assert(offsetof(struct codec_setup_info, psys) == 0x24, "codec_setup_info.psys");
_Static_assert(offsetof(struct codec_setup_info, mode_param) == 0x28, "codec_setup_info.mode_param");
_Static_assert(offsetof(struct codec_setup_info, map_type) == 0x228, "codec_setup_info.map_type");
_Static_assert(offsetof(struct codec_setup_info, map_param) == 0x328, "codec_setup_info.map_param");
_Static_assert(offsetof(struct codec_setup_info, floor_type) == 0x528, "codec_setup_info.floor_type");
_Static_assert(offsetof(struct codec_setup_info, floor_param) == 0x628, "codec_setup_info.floor_param");
_Static_assert(offsetof(struct codec_setup_info, residue_type) == 0x828, "codec_setup_info.residue_type");
_Static_assert(offsetof(struct codec_setup_info, residue_param) == 0x928, "codec_setup_info.residue_param");
_Static_assert(offsetof(struct codec_setup_info, book_param) == 0xb28, "codec_setup_info.book_param");
_Static_assert(offsetof(struct codec_setup_info, fullbooks) == 0x1328, "codec_setup_info.fullbooks");
_Static_assert(offsetof(struct codec_setup_info, psy_param) == 0x1330, "codec_setup_info.psy_param");
_Static_assert(offsetof(struct codec_setup_info, psy_g_param) == 0x1350, "codec_setup_info.psy_g_param");
_Static_assert(offsetof(struct codec_setup_info, bi) == 0x1540, "codec_setup_info.bi");
_Static_assert(offsetof(struct codec_setup_info, hi) == 0x1570, "codec_setup_info.hi");
_Static_assert(offsetof(struct codec_setup_info, halfrate_flag) == 0x1690, "codec_setup_info.halfrate_flag");
_Static_assert(sizeof(vorbis_look_floor1) == 0x530, "vorbis_look_floor1");
_Static_assert(offsetof(vorbis_look_floor1, sorted_index) == 0x0, "vorbis_look_floor1.sorted_index");
_Static_assert(offsetof(vorbis_look_floor1, forward_index) == 0x104, "vorbis_look_floor1.forward_index");
_Static_assert(offsetof(vorbis_look_floor1, reverse_index) == 0x208, "vorbis_look_floor1.reverse_index");
_Static_assert(offsetof(vorbis_look_floor1, hineighbor) == 0x30c, "vorbis_look_floor1.hineighbor");
_Static_assert(offsetof(vorbis_look_floor1, loneighbor) == 0x408, "vorbis_look_floor1.loneighbor");
_Static_assert(offsetof(vorbis_look_floor1, posts) == 0x504, "vorbis_look_floor1.posts");
_Static_assert(offsetof(vorbis_look_floor1, n) == 0x508, "vorbis_look_floor1.n");
_Static_assert(offsetof(vorbis_look_floor1, quant_q) == 0x50c, "vorbis_look_floor1.quant_q");
_Static_assert(offsetof(vorbis_look_floor1, vi) == 0x510, "vorbis_look_floor1.vi");
_Static_assert(offsetof(vorbis_look_floor1, phrasebits) == 0x518, "vorbis_look_floor1.phrasebits");
_Static_assert(offsetof(vorbis_look_floor1, postbits) == 0x520, "vorbis_look_floor1.postbits");
_Static_assert(offsetof(vorbis_look_floor1, frames) == 0x528, "vorbis_look_floor1.frames");

#endif
