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

 function: modified discrete cosine transform prototypes
 last mod: $Id: mdct.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#ifndef _OGG_mdct_H_
#define _OGG_mdct_H_

#include "vorbis/codec.h"





/*#define MDCT_INTEGERIZED  <- be warned there could be some hurt left here*/
#ifdef MDCT_INTEGERIZED

#define DATA_TYPE int
#define REG_TYPE  register int
#define TRIGBITS 14
#define cPI3_8 6270
#define cPI2_8 11585
#define cPI1_8 15137

#define FLOAT_CONV(x) ((int)((x)*(1<<TRIGBITS)+.5))
#define MULT_NORM(x) ((x)>>TRIGBITS)
#define HALVE(x) ((x)>>1)

#else

#define DATA_TYPE float
#define REG_TYPE  float
#define cPI3_8 .38268343236508977175F
#define cPI2_8 .70710678118654752441F
#define cPI1_8 .92387953251128675613F

#define FLOAT_CONV(x) (x)
#define MULT_NORM(x) (x)
#define HALVE(x) ((x)*.5f)

#endif


typedef struct {
  int n;
  int log2n;

  DATA_TYPE *trig;
  int       *bitrev;

  DATA_TYPE scale;
} mdct_lookup;

extern void mdct_init(mdct_lookup *lookup,int n);
extern void mdct_clear(mdct_lookup *l);
extern void mdct_forward(mdct_lookup *init, DATA_TYPE *in, DATA_TYPE *out);
extern void mdct_backward(mdct_lookup *init, DATA_TYPE *in, DATA_TYPE *out);


// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(mdct_lookup) == 0x20, "mdct_lookup");
_Static_assert(offsetof(mdct_lookup, n) == 0x0, "mdct_lookup.n");
_Static_assert(offsetof(mdct_lookup, log2n) == 0x4, "mdct_lookup.log2n");
_Static_assert(offsetof(mdct_lookup, trig) == 0x8, "mdct_lookup.trig");
_Static_assert(offsetof(mdct_lookup, bitrev) == 0x10, "mdct_lookup.bitrev");
_Static_assert(offsetof(mdct_lookup, scale) == 0x18, "mdct_lookup.scale");

#endif
