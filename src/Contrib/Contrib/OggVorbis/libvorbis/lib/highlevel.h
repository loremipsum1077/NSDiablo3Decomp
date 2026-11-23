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

 function: highlevel encoder setup struct separated out for vorbisenc clarity
 last mod: $Id: highlevel.h 17195 2010-05-05 21:49:51Z giles $

 ********************************************************************/

typedef struct highlevel_byblocktype {
  double tone_mask_setting;
  double tone_peaklimit_setting;
  double noise_bias_setting;
  double noise_compand_setting;
} highlevel_byblocktype;

typedef struct highlevel_encode_setup {
  int   set_in_stone;
  const void *setup;
  double base_setting;

  double impulse_noisetune;

  /* bitrate management below all settable */
  float  req;
  int    managed;
  long   bitrate_min;
  long   bitrate_av;
  double bitrate_av_damp;
  long   bitrate_max;
  long   bitrate_reservoir;
  double bitrate_reservoir_bias;

  int impulse_block_p;
  int noise_normalize_p;
  int coupling_p;

  double stereo_point_setting;
  double lowpass_kHz;
  int    lowpass_altered;

  double ath_floating_dB;
  double ath_absolute_dB;

  double amplitude_track_dBpersec;
  double trigger_setting;

  highlevel_byblocktype block[4]; /* padding, impulse, transition, long */

} highlevel_encode_setup;

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(struct highlevel_byblocktype) == 0x20, "highlevel_byblocktype");
_Static_assert(offsetof(struct highlevel_byblocktype, tone_mask_setting) == 0x0, "highlevel_byblocktype.tone_mask_setting");
_Static_assert(offsetof(struct highlevel_byblocktype, tone_peaklimit_setting) == 0x8, "highlevel_byblocktype.tone_peaklimit_setting");
_Static_assert(offsetof(struct highlevel_byblocktype, noise_bias_setting) == 0x10, "highlevel_byblocktype.noise_bias_setting");
_Static_assert(offsetof(struct highlevel_byblocktype, noise_compand_setting) == 0x18, "highlevel_byblocktype.noise_compand_setting");
_Static_assert(sizeof(struct highlevel_encode_setup) == 0x120, "highlevel_encode_setup");
_Static_assert(offsetof(struct highlevel_encode_setup, set_in_stone) == 0x0, "highlevel_encode_setup.set_in_stone");
_Static_assert(offsetof(struct highlevel_encode_setup, setup) == 0x8, "highlevel_encode_setup.setup");
_Static_assert(offsetof(struct highlevel_encode_setup, base_setting) == 0x10, "highlevel_encode_setup.base_setting");
_Static_assert(offsetof(struct highlevel_encode_setup, impulse_noisetune) == 0x18, "highlevel_encode_setup.impulse_noisetune");
_Static_assert(offsetof(struct highlevel_encode_setup, req) == 0x20, "highlevel_encode_setup.req");
_Static_assert(offsetof(struct highlevel_encode_setup, managed) == 0x24, "highlevel_encode_setup.managed");
_Static_assert(offsetof(struct highlevel_encode_setup, bitrate_min) == 0x28, "highlevel_encode_setup.bitrate_min");
_Static_assert(offsetof(struct highlevel_encode_setup, bitrate_av) == 0x30, "highlevel_encode_setup.bitrate_av");
_Static_assert(offsetof(struct highlevel_encode_setup, bitrate_av_damp) == 0x38, "highlevel_encode_setup.bitrate_av_damp");
_Static_assert(offsetof(struct highlevel_encode_setup, bitrate_max) == 0x40, "highlevel_encode_setup.bitrate_max");
_Static_assert(offsetof(struct highlevel_encode_setup, bitrate_reservoir) == 0x48, "highlevel_encode_setup.bitrate_reservoir");
_Static_assert(offsetof(struct highlevel_encode_setup, bitrate_reservoir_bias) == 0x50, "highlevel_encode_setup.bitrate_reservoir_bias");
_Static_assert(offsetof(struct highlevel_encode_setup, impulse_block_p) == 0x58, "highlevel_encode_setup.impulse_block_p");
_Static_assert(offsetof(struct highlevel_encode_setup, noise_normalize_p) == 0x5c, "highlevel_encode_setup.noise_normalize_p");
_Static_assert(offsetof(struct highlevel_encode_setup, coupling_p) == 0x60, "highlevel_encode_setup.coupling_p");
_Static_assert(offsetof(struct highlevel_encode_setup, stereo_point_setting) == 0x68, "highlevel_encode_setup.stereo_point_setting");
_Static_assert(offsetof(struct highlevel_encode_setup, lowpass_kHz) == 0x70, "highlevel_encode_setup.lowpass_kHz");
_Static_assert(offsetof(struct highlevel_encode_setup, lowpass_altered) == 0x78, "highlevel_encode_setup.lowpass_altered");
_Static_assert(offsetof(struct highlevel_encode_setup, ath_floating_dB) == 0x80, "highlevel_encode_setup.ath_floating_dB");
_Static_assert(offsetof(struct highlevel_encode_setup, ath_absolute_dB) == 0x88, "highlevel_encode_setup.ath_absolute_dB");
_Static_assert(offsetof(struct highlevel_encode_setup, amplitude_track_dBpersec) == 0x90, "highlevel_encode_setup.amplitude_track_dBpersec");
_Static_assert(offsetof(struct highlevel_encode_setup, trigger_setting) == 0x98, "highlevel_encode_setup.trigger_setting");
_Static_assert(offsetof(struct highlevel_encode_setup, block) == 0xa0, "highlevel_encode_setup.block");
