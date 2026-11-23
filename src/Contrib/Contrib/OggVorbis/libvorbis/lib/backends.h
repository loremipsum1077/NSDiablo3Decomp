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

 function: libvorbis backend and mapping structures; needed for
           static mode headers
 last mod: $Id: backends.h 16962 2010-03-11 07:30:34Z xiphmont $

 ********************************************************************/

/* this is exposed up here because we need it for static modes.
   Lookups for each backend aren't exposed because there's no reason
   to do so */

#ifndef _vorbis_backend_h_
#define _vorbis_backend_h_

#include "codec_internal.h"

/* this would all be simpler/shorter with templates, but.... */
/* Floor backend generic *****************************************/
typedef struct{
  void                   (*pack)  (vorbis_info_floor *,oggpack_buffer *);
  vorbis_info_floor     *(*unpack)(vorbis_info *,oggpack_buffer *);
  vorbis_look_floor     *(*look)  (vorbis_dsp_state *,vorbis_info_floor *);
  void (*free_info) (vorbis_info_floor *);
  void (*free_look) (vorbis_look_floor *);
  void *(*inverse1)  (struct vorbis_block *,vorbis_look_floor *);
  int   (*inverse2)  (struct vorbis_block *,vorbis_look_floor *,
                     void *buffer,float *);
} vorbis_func_floor;

typedef struct{
  int   order;
  long  rate;
  long  barkmap;

  int   ampbits;
  int   ampdB;

  int   numbooks; /* <= 16 */
  int   books[16];

  float lessthan;     /* encode-only config setting hacks for libvorbis */
  float greaterthan;  /* encode-only config setting hacks for libvorbis */

} vorbis_info_floor0;


#define VIF_POSIT 63
#define VIF_CLASS 16
#define VIF_PARTS 31
typedef struct{
  int   partitions;                /* 0 to 31 */
  int   partitionclass[VIF_PARTS]; /* 0 to 15 */

  int   class_dim[VIF_CLASS];        /* 1 to 8 */
  int   class_subs[VIF_CLASS];       /* 0,1,2,3 (bits: 1<<n poss) */
  int   class_book[VIF_CLASS];       /* subs ^ dim entries */
  int   class_subbook[VIF_CLASS][8]; /* [VIF_CLASS][subs] */


  int   mult;                      /* 1 2 3 or 4 */
  int   postlist[VIF_POSIT+2];    /* first two implicit */


  /* encode side analysis parameters */
  float maxover;
  float maxunder;
  float maxerr;

  float twofitweight;
  float twofitatten;

  int   n;

} vorbis_info_floor1;

/* Residue backend generic *****************************************/
typedef struct{
  void                 (*pack)  (vorbis_info_residue *,oggpack_buffer *);
  vorbis_info_residue *(*unpack)(vorbis_info *,oggpack_buffer *);
  vorbis_look_residue *(*look)  (vorbis_dsp_state *,
                                 vorbis_info_residue *);
  void (*free_info)    (vorbis_info_residue *);
  void (*free_look)    (vorbis_look_residue *);
  long **(*class)      (struct vorbis_block *,vorbis_look_residue *,
                        int **,int *,int);
  int  (*forward)      (oggpack_buffer *,struct vorbis_block *,
                        vorbis_look_residue *,
                        int **,int *,int,long **,int);
  int  (*inverse)      (struct vorbis_block *,vorbis_look_residue *,
                        float **,int *,int);
} vorbis_func_residue;

typedef struct vorbis_info_residue0{
/* block-partitioned VQ coded straight residue */
  long  begin;
  long  end;

  /* first stage (lossless partitioning) */
  int    grouping;         /* group n vectors per partition */
  int    partitions;       /* possible codebooks for a partition */
  int    partvals;         /* partitions ^ groupbook dim */
  int    groupbook;        /* huffbook for partitioning */
  int    secondstages[64]; /* expanded out to pointers in lookup */
  int    booklist[512];    /* list of second stage books */

  const int classmetric1[64];
  const int classmetric2[64];
} vorbis_info_residue0;

/* Mapping backend generic *****************************************/
typedef struct{
  void                 (*pack)  (vorbis_info *,vorbis_info_mapping *,
                                 oggpack_buffer *);
  vorbis_info_mapping *(*unpack)(vorbis_info *,oggpack_buffer *);
  void (*free_info)    (vorbis_info_mapping *);
  int  (*forward)      (struct vorbis_block *vb);
  int  (*inverse)      (struct vorbis_block *vb,vorbis_info_mapping *);
} vorbis_func_mapping;

typedef struct vorbis_info_mapping0{
  int   submaps;  /* <= 16 */
  int   chmuxlist[256];   /* up to 256 channels in a Vorbis stream */

  int   floorsubmap[16];   /* [mux] submap to floors */
  int   residuesubmap[16]; /* [mux] submap to residue */

  int   coupling_steps;
  int   coupling_mag[256];
  int   coupling_ang[256];

} vorbis_info_mapping0;


// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(vorbis_func_floor) == 0x38, "vorbis_func_floor");
_Static_assert(offsetof(vorbis_func_floor, pack) == 0x0, "vorbis_func_floor.pack");
_Static_assert(offsetof(vorbis_func_floor, unpack) == 0x8, "vorbis_func_floor.unpack");
_Static_assert(offsetof(vorbis_func_floor, look) == 0x10, "vorbis_func_floor.look");
_Static_assert(offsetof(vorbis_func_floor, free_info) == 0x18, "vorbis_func_floor.free_info");
_Static_assert(offsetof(vorbis_func_floor, free_look) == 0x20, "vorbis_func_floor.free_look");
_Static_assert(offsetof(vorbis_func_floor, inverse1) == 0x28, "vorbis_func_floor.inverse1");
_Static_assert(offsetof(vorbis_func_floor, inverse2) == 0x30, "vorbis_func_floor.inverse2");
_Static_assert(sizeof(vorbis_info_floor0) == 0x70, "vorbis_info_floor0");
_Static_assert(offsetof(vorbis_info_floor0, order) == 0x0, "vorbis_info_floor0.order");
_Static_assert(offsetof(vorbis_info_floor0, rate) == 0x8, "vorbis_info_floor0.rate");
_Static_assert(offsetof(vorbis_info_floor0, barkmap) == 0x10, "vorbis_info_floor0.barkmap");
_Static_assert(offsetof(vorbis_info_floor0, ampbits) == 0x18, "vorbis_info_floor0.ampbits");
_Static_assert(offsetof(vorbis_info_floor0, ampdB) == 0x1c, "vorbis_info_floor0.ampdB");
_Static_assert(offsetof(vorbis_info_floor0, numbooks) == 0x20, "vorbis_info_floor0.numbooks");
_Static_assert(offsetof(vorbis_info_floor0, books) == 0x24, "vorbis_info_floor0.books");
_Static_assert(offsetof(vorbis_info_floor0, lessthan) == 0x64, "vorbis_info_floor0.lessthan");
_Static_assert(offsetof(vorbis_info_floor0, greaterthan) == 0x68, "vorbis_info_floor0.greaterthan");
_Static_assert(sizeof(vorbis_info_floor1) == 0x460, "vorbis_info_floor1");
_Static_assert(offsetof(vorbis_info_floor1, partitions) == 0x0, "vorbis_info_floor1.partitions");
_Static_assert(offsetof(vorbis_info_floor1, partitionclass) == 0x4, "vorbis_info_floor1.partitionclass");
_Static_assert(offsetof(vorbis_info_floor1, class_dim) == 0x80, "vorbis_info_floor1.class_dim");
_Static_assert(offsetof(vorbis_info_floor1, class_subs) == 0xc0, "vorbis_info_floor1.class_subs");
_Static_assert(offsetof(vorbis_info_floor1, class_book) == 0x100, "vorbis_info_floor1.class_book");
_Static_assert(offsetof(vorbis_info_floor1, class_subbook) == 0x140, "vorbis_info_floor1.class_subbook");
_Static_assert(offsetof(vorbis_info_floor1, mult) == 0x340, "vorbis_info_floor1.mult");
_Static_assert(offsetof(vorbis_info_floor1, postlist) == 0x344, "vorbis_info_floor1.postlist");
_Static_assert(offsetof(vorbis_info_floor1, maxover) == 0x448, "vorbis_info_floor1.maxover");
_Static_assert(offsetof(vorbis_info_floor1, maxunder) == 0x44c, "vorbis_info_floor1.maxunder");
_Static_assert(offsetof(vorbis_info_floor1, maxerr) == 0x450, "vorbis_info_floor1.maxerr");
_Static_assert(offsetof(vorbis_info_floor1, twofitweight) == 0x454, "vorbis_info_floor1.twofitweight");
_Static_assert(offsetof(vorbis_info_floor1, twofitatten) == 0x458, "vorbis_info_floor1.twofitatten");
_Static_assert(offsetof(vorbis_info_floor1, n) == 0x45c, "vorbis_info_floor1.n");
_Static_assert(sizeof(vorbis_func_residue) == 0x40, "vorbis_func_residue");
_Static_assert(offsetof(vorbis_func_residue, pack) == 0x0, "vorbis_func_residue.pack");
_Static_assert(offsetof(vorbis_func_residue, unpack) == 0x8, "vorbis_func_residue.unpack");
_Static_assert(offsetof(vorbis_func_residue, look) == 0x10, "vorbis_func_residue.look");
_Static_assert(offsetof(vorbis_func_residue, free_info) == 0x18, "vorbis_func_residue.free_info");
_Static_assert(offsetof(vorbis_func_residue, free_look) == 0x20, "vorbis_func_residue.free_look");
_Static_assert(offsetof(vorbis_func_residue, class) == 0x28, "vorbis_func_residue.class");
_Static_assert(offsetof(vorbis_func_residue, forward) == 0x30, "vorbis_func_residue.forward");
_Static_assert(offsetof(vorbis_func_residue, inverse) == 0x38, "vorbis_func_residue.inverse");
_Static_assert(sizeof(struct vorbis_info_residue0) == 0xb20, "vorbis_info_residue0");
_Static_assert(offsetof(struct vorbis_info_residue0, begin) == 0x0, "vorbis_info_residue0.begin");
_Static_assert(offsetof(struct vorbis_info_residue0, end) == 0x8, "vorbis_info_residue0.end");
_Static_assert(offsetof(struct vorbis_info_residue0, grouping) == 0x10, "vorbis_info_residue0.grouping");
_Static_assert(offsetof(struct vorbis_info_residue0, partitions) == 0x14, "vorbis_info_residue0.partitions");
_Static_assert(offsetof(struct vorbis_info_residue0, partvals) == 0x18, "vorbis_info_residue0.partvals");
_Static_assert(offsetof(struct vorbis_info_residue0, groupbook) == 0x1c, "vorbis_info_residue0.groupbook");
_Static_assert(offsetof(struct vorbis_info_residue0, secondstages) == 0x20, "vorbis_info_residue0.secondstages");
_Static_assert(offsetof(struct vorbis_info_residue0, booklist) == 0x120, "vorbis_info_residue0.booklist");
_Static_assert(offsetof(struct vorbis_info_residue0, classmetric1) == 0x920, "vorbis_info_residue0.classmetric1");
_Static_assert(offsetof(struct vorbis_info_residue0, classmetric2) == 0xa20, "vorbis_info_residue0.classmetric2");
_Static_assert(sizeof(vorbis_func_mapping) == 0x28, "vorbis_func_mapping");
_Static_assert(offsetof(vorbis_func_mapping, pack) == 0x0, "vorbis_func_mapping.pack");
_Static_assert(offsetof(vorbis_func_mapping, unpack) == 0x8, "vorbis_func_mapping.unpack");
_Static_assert(offsetof(vorbis_func_mapping, free_info) == 0x10, "vorbis_func_mapping.free_info");
_Static_assert(offsetof(vorbis_func_mapping, forward) == 0x18, "vorbis_func_mapping.forward");
_Static_assert(offsetof(vorbis_func_mapping, inverse) == 0x20, "vorbis_func_mapping.inverse");
_Static_assert(sizeof(struct vorbis_info_mapping0) == 0xc88, "vorbis_info_mapping0");
_Static_assert(offsetof(struct vorbis_info_mapping0, submaps) == 0x0, "vorbis_info_mapping0.submaps");
_Static_assert(offsetof(struct vorbis_info_mapping0, chmuxlist) == 0x4, "vorbis_info_mapping0.chmuxlist");
_Static_assert(offsetof(struct vorbis_info_mapping0, floorsubmap) == 0x404, "vorbis_info_mapping0.floorsubmap");
_Static_assert(offsetof(struct vorbis_info_mapping0, residuesubmap) == 0x444, "vorbis_info_mapping0.residuesubmap");
_Static_assert(offsetof(struct vorbis_info_mapping0, coupling_steps) == 0x484, "vorbis_info_mapping0.coupling_steps");
_Static_assert(offsetof(struct vorbis_info_mapping0, coupling_mag) == 0x488, "vorbis_info_mapping0.coupling_mag");
_Static_assert(offsetof(struct vorbis_info_mapping0, coupling_ang) == 0x888, "vorbis_info_mapping0.coupling_ang");

#endif
