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

 function: random psychoacoustics (not including preecho)
 last mod: $Id: psy.h 16946 2010-03-03 16:12:40Z xiphmont $

 ********************************************************************/

#ifndef _V_PSY_H_
#define _V_PSY_H_
#include "smallft.h"

#include "backends.h"
#include "envelope.h"

#ifndef EHMER_MAX
#define EHMER_MAX 56
#endif

/* psychoacoustic setup ********************************************/
#define P_BANDS 17      /* 62Hz to 16kHz */
#define P_LEVELS 8      /* 30dB to 100dB */
#define P_LEVEL_0 30.    /* 30 dB */
#define P_NOISECURVES 3

#define NOISE_COMPAND_LEVELS 40
typedef struct vorbis_info_psy{
  int   blockflag;

  float ath_adjatt;
  float ath_maxatt;

  float tone_masteratt[P_NOISECURVES];
  float tone_centerboost;
  float tone_decay;
  float tone_abs_limit;
  float toneatt[P_BANDS];

  int noisemaskp;
  float noisemaxsupp;
  float noisewindowlo;
  float noisewindowhi;
  int   noisewindowlomin;
  int   noisewindowhimin;
  int   noisewindowfixed;
  float noiseoff[P_NOISECURVES][P_BANDS];
  float noisecompand[NOISE_COMPAND_LEVELS];

  float max_curve_dB;

  int normal_p;
  int normal_start;
  int normal_partition;
  double normal_thresh;
} vorbis_info_psy;

typedef struct{
  int   eighth_octave_lines;

  /* for block long/short tuning; encode only */
  float preecho_thresh[VE_BANDS];
  float postecho_thresh[VE_BANDS];
  float stretch_penalty;
  float preecho_minenergy;

  float ampmax_att_per_sec;

  /* channel coupling config */
  int   coupling_pkHz[PACKETBLOBS];
  int   coupling_pointlimit[2][PACKETBLOBS];
  int   coupling_prepointamp[PACKETBLOBS];
  int   coupling_postpointamp[PACKETBLOBS];
  int   sliding_lowpass[2][PACKETBLOBS];

} vorbis_info_psy_global;

typedef struct {
  float ampmax;
  int   channels;

  vorbis_info_psy_global *gi;
  int   coupling_pointlimit[2][P_NOISECURVES];
} vorbis_look_psy_global;


typedef struct {
  int n;
  struct vorbis_info_psy *vi;

  float ***tonecurves;
  float **noiseoffset;

  float *ath;
  long  *octave;             /* in n.ocshift format */
  long  *bark;

  long  firstoc;
  long  shiftoc;
  int   eighth_octave_lines; /* power of two, please */
  int   total_octave_lines;
  long  rate; /* cache it */

  float m_val; /* Masking compensation value */

} vorbis_look_psy;

extern void   _vp_psy_init(vorbis_look_psy *p,vorbis_info_psy *vi,
                           vorbis_info_psy_global *gi,int n,long rate);
extern void   _vp_psy_clear(vorbis_look_psy *p);
extern void  *_vi_psy_dup(void *source);

extern void   _vi_psy_free(vorbis_info_psy *i);
extern vorbis_info_psy *_vi_psy_copy(vorbis_info_psy *i);

extern void _vp_noisemask(vorbis_look_psy *p,
                          float *logmdct,
                          float *logmask);

extern void _vp_tonemask(vorbis_look_psy *p,
                         float *logfft,
                         float *logmask,
                         float global_specmax,
                         float local_specmax);

extern void _vp_offset_and_mix(vorbis_look_psy *p,
                               float *noise,
                               float *tone,
                               int offset_select,
                               float *logmask,
                               float *mdct,
                               float *logmdct);

extern float _vp_ampmax_decay(float amp,vorbis_dsp_state *vd);

extern void _vp_couple_quantize_normalize(int blobno,
                                          vorbis_info_psy_global *g,
                                          vorbis_look_psy *p,
                                          vorbis_info_mapping0 *vi,
                                          float **mdct,
                                          int   **iwork,
                                          int    *nonzero,
                                          int     sliding_lowpass,
                                          int     ch);


// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(struct vorbis_info_psy) == 0x208, "vorbis_info_psy");
_Static_assert(offsetof(struct vorbis_info_psy, blockflag) == 0x0, "vorbis_info_psy.blockflag");
_Static_assert(offsetof(struct vorbis_info_psy, ath_adjatt) == 0x4, "vorbis_info_psy.ath_adjatt");
_Static_assert(offsetof(struct vorbis_info_psy, ath_maxatt) == 0x8, "vorbis_info_psy.ath_maxatt");
_Static_assert(offsetof(struct vorbis_info_psy, tone_masteratt) == 0xc, "vorbis_info_psy.tone_masteratt");
_Static_assert(offsetof(struct vorbis_info_psy, tone_centerboost) == 0x18, "vorbis_info_psy.tone_centerboost");
_Static_assert(offsetof(struct vorbis_info_psy, tone_decay) == 0x1c, "vorbis_info_psy.tone_decay");
_Static_assert(offsetof(struct vorbis_info_psy, tone_abs_limit) == 0x20, "vorbis_info_psy.tone_abs_limit");
_Static_assert(offsetof(struct vorbis_info_psy, toneatt) == 0x24, "vorbis_info_psy.toneatt");
_Static_assert(offsetof(struct vorbis_info_psy, noisemaskp) == 0x68, "vorbis_info_psy.noisemaskp");
_Static_assert(offsetof(struct vorbis_info_psy, noisemaxsupp) == 0x6c, "vorbis_info_psy.noisemaxsupp");
_Static_assert(offsetof(struct vorbis_info_psy, noisewindowlo) == 0x70, "vorbis_info_psy.noisewindowlo");
_Static_assert(offsetof(struct vorbis_info_psy, noisewindowhi) == 0x74, "vorbis_info_psy.noisewindowhi");
_Static_assert(offsetof(struct vorbis_info_psy, noisewindowlomin) == 0x78, "vorbis_info_psy.noisewindowlomin");
_Static_assert(offsetof(struct vorbis_info_psy, noisewindowhimin) == 0x7c, "vorbis_info_psy.noisewindowhimin");
_Static_assert(offsetof(struct vorbis_info_psy, noisewindowfixed) == 0x80, "vorbis_info_psy.noisewindowfixed");
_Static_assert(offsetof(struct vorbis_info_psy, noiseoff) == 0x84, "vorbis_info_psy.noiseoff");
_Static_assert(offsetof(struct vorbis_info_psy, noisecompand) == 0x150, "vorbis_info_psy.noisecompand");
_Static_assert(offsetof(struct vorbis_info_psy, max_curve_dB) == 0x1f0, "vorbis_info_psy.max_curve_dB");
_Static_assert(offsetof(struct vorbis_info_psy, normal_p) == 0x1f4, "vorbis_info_psy.normal_p");
_Static_assert(offsetof(struct vorbis_info_psy, normal_start) == 0x1f8, "vorbis_info_psy.normal_start");
_Static_assert(offsetof(struct vorbis_info_psy, normal_partition) == 0x1fc, "vorbis_info_psy.normal_partition");
_Static_assert(offsetof(struct vorbis_info_psy, normal_thresh) == 0x200, "vorbis_info_psy.normal_thresh");
_Static_assert(sizeof(vorbis_info_psy_global) == 0x1ec, "vorbis_info_psy_global");
_Static_assert(offsetof(vorbis_info_psy_global, eighth_octave_lines) == 0x0, "vorbis_info_psy_global.eighth_octave_lines");
_Static_assert(offsetof(vorbis_info_psy_global, preecho_thresh) == 0x4, "vorbis_info_psy_global.preecho_thresh");
_Static_assert(offsetof(vorbis_info_psy_global, postecho_thresh) == 0x20, "vorbis_info_psy_global.postecho_thresh");
_Static_assert(offsetof(vorbis_info_psy_global, stretch_penalty) == 0x3c, "vorbis_info_psy_global.stretch_penalty");
_Static_assert(offsetof(vorbis_info_psy_global, preecho_minenergy) == 0x40, "vorbis_info_psy_global.preecho_minenergy");
_Static_assert(offsetof(vorbis_info_psy_global, ampmax_att_per_sec) == 0x44, "vorbis_info_psy_global.ampmax_att_per_sec");
_Static_assert(offsetof(vorbis_info_psy_global, coupling_pkHz) == 0x48, "vorbis_info_psy_global.coupling_pkHz");
_Static_assert(offsetof(vorbis_info_psy_global, coupling_pointlimit) == 0x84, "vorbis_info_psy_global.coupling_pointlimit");
_Static_assert(offsetof(vorbis_info_psy_global, coupling_prepointamp) == 0xfc, "vorbis_info_psy_global.coupling_prepointamp");
_Static_assert(offsetof(vorbis_info_psy_global, coupling_postpointamp) == 0x138, "vorbis_info_psy_global.coupling_postpointamp");
_Static_assert(offsetof(vorbis_info_psy_global, sliding_lowpass) == 0x174, "vorbis_info_psy_global.sliding_lowpass");
_Static_assert(sizeof(vorbis_look_psy_global) == 0x28, "vorbis_look_psy_global");
_Static_assert(offsetof(vorbis_look_psy_global, ampmax) == 0x0, "vorbis_look_psy_global.ampmax");
_Static_assert(offsetof(vorbis_look_psy_global, channels) == 0x4, "vorbis_look_psy_global.channels");
_Static_assert(offsetof(vorbis_look_psy_global, gi) == 0x8, "vorbis_look_psy_global.gi");
_Static_assert(offsetof(vorbis_look_psy_global, coupling_pointlimit) == 0x10, "vorbis_look_psy_global.coupling_pointlimit");
_Static_assert(sizeof(vorbis_look_psy) == 0x60, "vorbis_look_psy");
_Static_assert(offsetof(vorbis_look_psy, n) == 0x0, "vorbis_look_psy.n");
_Static_assert(offsetof(vorbis_look_psy, vi) == 0x8, "vorbis_look_psy.vi");
_Static_assert(offsetof(vorbis_look_psy, tonecurves) == 0x10, "vorbis_look_psy.tonecurves");
_Static_assert(offsetof(vorbis_look_psy, noiseoffset) == 0x18, "vorbis_look_psy.noiseoffset");
_Static_assert(offsetof(vorbis_look_psy, ath) == 0x20, "vorbis_look_psy.ath");
_Static_assert(offsetof(vorbis_look_psy, octave) == 0x28, "vorbis_look_psy.octave");
_Static_assert(offsetof(vorbis_look_psy, bark) == 0x30, "vorbis_look_psy.bark");
_Static_assert(offsetof(vorbis_look_psy, firstoc) == 0x38, "vorbis_look_psy.firstoc");
_Static_assert(offsetof(vorbis_look_psy, shiftoc) == 0x40, "vorbis_look_psy.shiftoc");
_Static_assert(offsetof(vorbis_look_psy, eighth_octave_lines) == 0x48, "vorbis_look_psy.eighth_octave_lines");
_Static_assert(offsetof(vorbis_look_psy, total_octave_lines) == 0x4c, "vorbis_look_psy.total_octave_lines");
_Static_assert(offsetof(vorbis_look_psy, rate) == 0x50, "vorbis_look_psy.rate");
_Static_assert(offsetof(vorbis_look_psy, m_val) == 0x58, "vorbis_look_psy.m_val");

#endif
