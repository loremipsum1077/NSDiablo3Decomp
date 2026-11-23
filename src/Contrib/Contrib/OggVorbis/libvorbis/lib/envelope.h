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

 function: PCM data envelope analysis and manipulation
 last mod: $Id: envelope.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#ifndef _V_ENVELOPE_
#define _V_ENVELOPE_

#include "mdct.h"

#define VE_PRE    16
#define VE_WIN    4
#define VE_POST   2
#define VE_AMP    (VE_PRE+VE_POST-1)

#define VE_BANDS  7
#define VE_NEARDC 15

#define VE_MINSTRETCH 2   /* a bit less than short block */
#define VE_MAXSTRETCH 12  /* one-third full block */

typedef struct {
  float ampbuf[VE_AMP];
  int   ampptr;

  float nearDC[VE_NEARDC];
  float nearDC_acc;
  float nearDC_partialacc;
  int   nearptr;

} envelope_filter_state;

typedef struct {
  int begin;
  int end;
  float *window;
  float total;
} envelope_band;

typedef struct {
  int ch;
  int winlength;
  int searchstep;
  float minenergy;

  mdct_lookup  mdct;
  float       *mdct_win;

  envelope_band          band[VE_BANDS];
  envelope_filter_state *filter;
  int   stretch;

  int                   *mark;

  long storage;
  long current;
  long curmark;
  long cursor;
} envelope_lookup;

extern void _ve_envelope_init(envelope_lookup *e,vorbis_info *vi);
extern void _ve_envelope_clear(envelope_lookup *e);
extern long _ve_envelope_search(vorbis_dsp_state *v);
extern void _ve_envelope_shift(envelope_lookup *e,long shift);
extern int  _ve_envelope_mark(vorbis_dsp_state *v);



// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(envelope_filter_state) == 0x90, "envelope_filter_state");
_Static_assert(offsetof(envelope_filter_state, ampbuf) == 0x0, "envelope_filter_state.ampbuf");
_Static_assert(offsetof(envelope_filter_state, ampptr) == 0x44, "envelope_filter_state.ampptr");
_Static_assert(offsetof(envelope_filter_state, nearDC) == 0x48, "envelope_filter_state.nearDC");
_Static_assert(offsetof(envelope_filter_state, nearDC_acc) == 0x84, "envelope_filter_state.nearDC_acc");
_Static_assert(offsetof(envelope_filter_state, nearDC_partialacc) == 0x88, "envelope_filter_state.nearDC_partialacc");
_Static_assert(offsetof(envelope_filter_state, nearptr) == 0x8c, "envelope_filter_state.nearptr");
_Static_assert(sizeof(envelope_band) == 0x18, "envelope_band");
_Static_assert(offsetof(envelope_band, begin) == 0x0, "envelope_band.begin");
_Static_assert(offsetof(envelope_band, end) == 0x4, "envelope_band.end");
_Static_assert(offsetof(envelope_band, window) == 0x8, "envelope_band.window");
_Static_assert(offsetof(envelope_band, total) == 0x10, "envelope_band.total");
_Static_assert(sizeof(envelope_lookup) == 0x118, "envelope_lookup");
_Static_assert(offsetof(envelope_lookup, ch) == 0x0, "envelope_lookup.ch");
_Static_assert(offsetof(envelope_lookup, winlength) == 0x4, "envelope_lookup.winlength");
_Static_assert(offsetof(envelope_lookup, searchstep) == 0x8, "envelope_lookup.searchstep");
_Static_assert(offsetof(envelope_lookup, minenergy) == 0xc, "envelope_lookup.minenergy");
_Static_assert(offsetof(envelope_lookup, mdct) == 0x10, "envelope_lookup.mdct");
_Static_assert(offsetof(envelope_lookup, mdct_win) == 0x30, "envelope_lookup.mdct_win");
_Static_assert(offsetof(envelope_lookup, band) == 0x38, "envelope_lookup.band");
_Static_assert(offsetof(envelope_lookup, filter) == 0xe0, "envelope_lookup.filter");
_Static_assert(offsetof(envelope_lookup, stretch) == 0xe8, "envelope_lookup.stretch");
_Static_assert(offsetof(envelope_lookup, mark) == 0xf0, "envelope_lookup.mark");
_Static_assert(offsetof(envelope_lookup, storage) == 0xf8, "envelope_lookup.storage");
_Static_assert(offsetof(envelope_lookup, current) == 0x100, "envelope_lookup.current");
_Static_assert(offsetof(envelope_lookup, curmark) == 0x108, "envelope_lookup.curmark");
_Static_assert(offsetof(envelope_lookup, cursor) == 0x110, "envelope_lookup.cursor");

#endif
