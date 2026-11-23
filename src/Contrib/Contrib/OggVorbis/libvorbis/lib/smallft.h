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

 function: fft transform
 last mod: $Id: smallft.h 13293 2007-07-24 00:09:47Z xiphmont $

 ********************************************************************/

#ifndef _V_SMFT_H_
#define _V_SMFT_H_

#include "vorbis/codec.h"

typedef struct {
  int n;
  float *trigcache;
  int *splitcache;
} drft_lookup;

extern void drft_forward(drft_lookup *l,float *data);
extern void drft_backward(drft_lookup *l,float *data);
extern void drft_init(drft_lookup *l,int n);
extern void drft_clear(drft_lookup *l);


// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(drft_lookup) == 0x18, "drft_lookup");
_Static_assert(offsetof(drft_lookup, n) == 0x0, "drft_lookup.n");
_Static_assert(offsetof(drft_lookup, trigcache) == 0x8, "drft_lookup.trigcache");
_Static_assert(offsetof(drft_lookup, splitcache) == 0x10, "drft_lookup.splitcache");

#endif
