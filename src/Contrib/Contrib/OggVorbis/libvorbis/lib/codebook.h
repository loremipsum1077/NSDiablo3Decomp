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

 function: basic shared codebook operations
 last mod: $Id: codebook.h 17030 2010-03-25 06:52:55Z xiphmont $

 ********************************************************************/

#ifndef _V_CODEBOOK_H_
#define _V_CODEBOOK_H_

#include <ogg/ogg.h>

/* This structure encapsulates huffman and VQ style encoding books; it
   doesn't do anything specific to either.

   valuelist/quantlist are nonNULL (and q_* significant) only if
   there's entry->value mapping to be done.

   If encode-side mapping must be done (and thus the entry needs to be
   hunted), the auxiliary encode pointer will point to a decision
   tree.  This is true of both VQ and huffman, but is mostly useful
   with VQ.

*/

typedef struct static_codebook{
  long   dim;            /* codebook dimensions (elements per vector) */
  long   entries;        /* codebook entries */
  long  *lengthlist;     /* codeword lengths in bits */

  /* mapping ***************************************************************/
  int    maptype;        /* 0=none
                            1=implicitly populated values from map column
                            2=listed arbitrary values */

  /* The below does a linear, single monotonic sequence mapping. */
  long     q_min;       /* packed 32 bit float; quant value 0 maps to minval */
  long     q_delta;     /* packed 32 bit float; val 1 - val 0 == delta */
  int      q_quant;     /* bits: 0 < quant <= 16 */
  int      q_sequencep; /* bitflag */

  long     *quantlist;  /* map == 1: (int)(entries^(1/dim)) element column map
                           map == 2: list of dim*entries quantized entry vals
                        */
  int allocedp;
} static_codebook;

typedef struct codebook{
  long dim;           /* codebook dimensions (elements per vector) */
  long entries;       /* codebook entries */
  long used_entries;  /* populated codebook entries */
  const static_codebook *c;

  /* for encode, the below are entry-ordered, fully populated */
  /* for decode, the below are ordered by bitreversed codeword and only
     used entries are populated */
  float        *valuelist;  /* list of dim*entries actual entry values */
  ogg_uint32_t *codelist;   /* list of bitstream codewords for each entry */

  int          *dec_index;  /* only used if sparseness collapsed */
  char         *dec_codelengths;
  ogg_uint32_t *dec_firsttable;
  int           dec_firsttablen;
  int           dec_maxlength;

  /* The current encoder uses only centered, integer-only lattice books. */
  int           quantvals;
  int           minval;
  int           delta;
} codebook;

extern void vorbis_staticbook_destroy(static_codebook *b);
extern int vorbis_book_init_encode(codebook *dest,const static_codebook *source);
extern int vorbis_book_init_decode(codebook *dest,const static_codebook *source);
extern void vorbis_book_clear(codebook *b);

extern float *_book_unquantize(const static_codebook *b,int n,int *map);
extern float *_book_logdist(const static_codebook *b,float *vals);
extern float _float32_unpack(long val);
extern long   _float32_pack(float val);
extern int  _best(codebook *book, float *a, int step);
extern int _ilog(unsigned int v);
extern long _book_maptype1_quantvals(const static_codebook *b);

extern int vorbis_book_besterror(codebook *book,float *a,int step,int addmul);
extern long vorbis_book_codeword(codebook *book,int entry);
extern long vorbis_book_codelen(codebook *book,int entry);



extern int vorbis_staticbook_pack(const static_codebook *c,oggpack_buffer *b);
extern static_codebook *vorbis_staticbook_unpack(oggpack_buffer *b);

extern int vorbis_book_encode(codebook *book, int a, oggpack_buffer *b);

extern long vorbis_book_decode(codebook *book, oggpack_buffer *b);
extern long vorbis_book_decodevs_add(codebook *book, float *a,
                                     oggpack_buffer *b,int n);
extern long vorbis_book_decodev_set(codebook *book, float *a,
                                    oggpack_buffer *b,int n);
extern long vorbis_book_decodev_add(codebook *book, float *a,
                                    oggpack_buffer *b,int n);
extern long vorbis_book_decodevv_add(codebook *book, float **a,
                                     long off,int ch,
                                    oggpack_buffer *b,int n);




// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(struct static_codebook) == 0x48, "static_codebook");
_Static_assert(offsetof(struct static_codebook, dim) == 0x0, "static_codebook.dim");
_Static_assert(offsetof(struct static_codebook, entries) == 0x8, "static_codebook.entries");
_Static_assert(offsetof(struct static_codebook, lengthlist) == 0x10, "static_codebook.lengthlist");
_Static_assert(offsetof(struct static_codebook, maptype) == 0x18, "static_codebook.maptype");
_Static_assert(offsetof(struct static_codebook, q_min) == 0x20, "static_codebook.q_min");
_Static_assert(offsetof(struct static_codebook, q_delta) == 0x28, "static_codebook.q_delta");
_Static_assert(offsetof(struct static_codebook, q_quant) == 0x30, "static_codebook.q_quant");
_Static_assert(offsetof(struct static_codebook, q_sequencep) == 0x34, "static_codebook.q_sequencep");
_Static_assert(offsetof(struct static_codebook, quantlist) == 0x38, "static_codebook.quantlist");
_Static_assert(offsetof(struct static_codebook, allocedp) == 0x40, "static_codebook.allocedp");
_Static_assert(sizeof(struct codebook) == 0x60, "codebook");
_Static_assert(offsetof(struct codebook, dim) == 0x0, "codebook.dim");
_Static_assert(offsetof(struct codebook, entries) == 0x8, "codebook.entries");
_Static_assert(offsetof(struct codebook, used_entries) == 0x10, "codebook.used_entries");
_Static_assert(offsetof(struct codebook, c) == 0x18, "codebook.c");
_Static_assert(offsetof(struct codebook, valuelist) == 0x20, "codebook.valuelist");
_Static_assert(offsetof(struct codebook, codelist) == 0x28, "codebook.codelist");
_Static_assert(offsetof(struct codebook, dec_index) == 0x30, "codebook.dec_index");
_Static_assert(offsetof(struct codebook, dec_codelengths) == 0x38, "codebook.dec_codelengths");
_Static_assert(offsetof(struct codebook, dec_firsttable) == 0x40, "codebook.dec_firsttable");
_Static_assert(offsetof(struct codebook, dec_firsttablen) == 0x48, "codebook.dec_firsttablen");
_Static_assert(offsetof(struct codebook, dec_maxlength) == 0x4c, "codebook.dec_maxlength");
_Static_assert(offsetof(struct codebook, quantvals) == 0x50, "codebook.quantvals");
_Static_assert(offsetof(struct codebook, minval) == 0x54, "codebook.minval");
_Static_assert(offsetof(struct codebook, delta) == 0x58, "codebook.delta");

#endif
