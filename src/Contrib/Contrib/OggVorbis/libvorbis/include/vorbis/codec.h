/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2001             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *

 ********************************************************************

 function: libvorbis codec headers
 last mod: $Id: codec.h 17021 2010-03-24 09:29:41Z xiphmont $

 ********************************************************************/

#ifndef _vorbis_codec_h_
#define _vorbis_codec_h_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <ogg/ogg.h>

typedef struct vorbis_info{
  int version;
  int channels;
  long rate;

  /* The below bitrate declarations are *hints*.
     Combinations of the three values carry the following implications:

     all three set to the same value:
       implies a fixed rate bitstream
     only nominal set:
       implies a VBR stream that averages the nominal bitrate.  No hard
       upper/lower limit
     upper and or lower set:
       implies a VBR bitstream that obeys the bitrate limits. nominal
       may also be set to give a nominal rate.
     none set:
       the coder does not care to speculate.
  */

  long bitrate_upper;
  long bitrate_nominal;
  long bitrate_lower;
  long bitrate_window;

  void *codec_setup;
} vorbis_info;

/* vorbis_dsp_state buffers the current vorbis audio
   analysis/synthesis state.  The DSP state belongs to a specific
   logical bitstream ****************************************************/
typedef struct vorbis_dsp_state{
  int analysisp;
  vorbis_info *vi;

  float **pcm;
  float **pcmret;
  int      pcm_storage;
  int      pcm_current;
  int      pcm_returned;

  int  preextrapolate;
  int  eofflag;

  long lW;
  long W;
  long nW;
  long centerW;

  ogg_int64_t granulepos;
  ogg_int64_t sequence;

  ogg_int64_t glue_bits;
  ogg_int64_t time_bits;
  ogg_int64_t floor_bits;
  ogg_int64_t res_bits;

  void       *backend_state;
} vorbis_dsp_state;

typedef struct vorbis_block{
  /* necessary stream state for linking to the framing abstraction */
  float  **pcm;       /* this is a pointer into local storage */
  oggpack_buffer opb;

  long  lW;
  long  W;
  long  nW;
  int   pcmend;
  int   mode;

  int         eofflag;
  ogg_int64_t granulepos;
  ogg_int64_t sequence;
  vorbis_dsp_state *vd; /* For read-only access of configuration */

  /* local storage to avoid remallocing; it's up to the mapping to
     structure it */
  void               *localstore;
  long                localtop;
  long                localalloc;
  long                totaluse;
  struct alloc_chain *reap;

  /* bitmetrics for the frame */
  long glue_bits;
  long time_bits;
  long floor_bits;
  long res_bits;

  void *internal;

} vorbis_block;

/* vorbis_block is a single block of data to be processed as part of
the analysis/synthesis stream; it belongs to a specific logical
bitstream, but is independent from other vorbis_blocks belonging to
that logical bitstream. *************************************************/

struct alloc_chain{
  void *ptr;
  struct alloc_chain *next;
};

/* vorbis_info contains all the setup information specific to the
   specific compression/decompression mode in progress (eg,
   psychoacoustic settings, channel setup, options, codebook
   etc). vorbis_info and substructures are in backends.h.
*********************************************************************/

/* the comments are not part of vorbis_info so that vorbis_info can be
   static storage */
typedef struct vorbis_comment{
  /* unlimited user comment fields.  libvorbis writes 'libvorbis'
     whatever vendor is set to in encode */
  char **user_comments;
  int   *comment_lengths;
  int    comments;
  char  *vendor;

} vorbis_comment;


/* libvorbis encodes in two abstraction layers; first we perform DSP
   and produce a packet (see docs/analysis.txt).  The packet is then
   coded into a framed OggSquish bitstream by the second layer (see
   docs/framing.txt).  Decode is the reverse process; we sync/frame
   the bitstream and extract individual packets, then decode the
   packet back into PCM audio.

   The extra framing/packetizing is used in streaming formats, such as
   files.  Over the net (such as with UDP), the framing and
   packetization aren't necessary as they're provided by the transport
   and the streaming layer is not used */

/* Vorbis PRIMITIVES: general ***************************************/

extern void     vorbis_info_init(vorbis_info *vi);
extern void     vorbis_info_clear(vorbis_info *vi);
extern int      vorbis_info_blocksize(vorbis_info *vi,int zo);
extern void     vorbis_comment_init(vorbis_comment *vc);
extern void     vorbis_comment_add(vorbis_comment *vc, const char *comment);
extern void     vorbis_comment_add_tag(vorbis_comment *vc,
                                       const char *tag, const char *contents);
extern char    *vorbis_comment_query(vorbis_comment *vc, const char *tag, int count);
extern int      vorbis_comment_query_count(vorbis_comment *vc, const char *tag);
extern void     vorbis_comment_clear(vorbis_comment *vc);

extern int      vorbis_block_init(vorbis_dsp_state *v, vorbis_block *vb);
extern int      vorbis_block_clear(vorbis_block *vb);
extern void     vorbis_dsp_clear(vorbis_dsp_state *v);
extern double   vorbis_granule_time(vorbis_dsp_state *v,
                                    ogg_int64_t granulepos);

extern const char *vorbis_version_string(void);

/* Vorbis PRIMITIVES: analysis/DSP layer ****************************/

extern int      vorbis_analysis_init(vorbis_dsp_state *v,vorbis_info *vi);
extern int      vorbis_commentheader_out(vorbis_comment *vc, ogg_packet *op);
extern int      vorbis_analysis_headerout(vorbis_dsp_state *v,
                                          vorbis_comment *vc,
                                          ogg_packet *op,
                                          ogg_packet *op_comm,
                                          ogg_packet *op_code);
extern float  **vorbis_analysis_buffer(vorbis_dsp_state *v,int vals);
extern int      vorbis_analysis_wrote(vorbis_dsp_state *v,int vals);
extern int      vorbis_analysis_blockout(vorbis_dsp_state *v,vorbis_block *vb);
extern int      vorbis_analysis(vorbis_block *vb,ogg_packet *op);

extern int      vorbis_bitrate_addblock(vorbis_block *vb);
extern int      vorbis_bitrate_flushpacket(vorbis_dsp_state *vd,
                                           ogg_packet *op);

/* Vorbis PRIMITIVES: synthesis layer *******************************/
extern int      vorbis_synthesis_idheader(ogg_packet *op);
extern int      vorbis_synthesis_headerin(vorbis_info *vi,vorbis_comment *vc,
                                          ogg_packet *op);

extern int      vorbis_synthesis_init(vorbis_dsp_state *v,vorbis_info *vi);
extern int      vorbis_synthesis_restart(vorbis_dsp_state *v);
extern int      vorbis_synthesis(vorbis_block *vb,ogg_packet *op);
extern int      vorbis_synthesis_trackonly(vorbis_block *vb,ogg_packet *op);
extern int      vorbis_synthesis_blockin(vorbis_dsp_state *v,vorbis_block *vb);
extern int      vorbis_synthesis_pcmout(vorbis_dsp_state *v,float ***pcm);
extern int      vorbis_synthesis_lapout(vorbis_dsp_state *v,float ***pcm);
extern int      vorbis_synthesis_read(vorbis_dsp_state *v,int samples);
extern long     vorbis_packet_blocksize(vorbis_info *vi,ogg_packet *op);

extern int      vorbis_synthesis_halfrate(vorbis_info *v,int flag);
extern int      vorbis_synthesis_halfrate_p(vorbis_info *v);

/* Vorbis ERRORS and return codes ***********************************/

#define OV_FALSE      -1
#define OV_EOF        -2
#define OV_HOLE       -3

#define OV_EREAD      -128
#define OV_EFAULT     -129
#define OV_EIMPL      -130
#define OV_EINVAL     -131
#define OV_ENOTVORBIS -132
#define OV_EBADHEADER -133
#define OV_EVERSION   -134
#define OV_ENOTAUDIO  -135
#define OV_EBADPACKET -136
#define OV_EBADLINK   -137
#define OV_ENOSEEK    -138

#ifdef __cplusplus
}
#endif /* __cplusplus */


// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(struct vorbis_info) == 0x38, "vorbis_info");
_Static_assert(offsetof(struct vorbis_info, version) == 0x0, "vorbis_info.version");
_Static_assert(offsetof(struct vorbis_info, channels) == 0x4, "vorbis_info.channels");
_Static_assert(offsetof(struct vorbis_info, rate) == 0x8, "vorbis_info.rate");
_Static_assert(offsetof(struct vorbis_info, bitrate_upper) == 0x10, "vorbis_info.bitrate_upper");
_Static_assert(offsetof(struct vorbis_info, bitrate_nominal) == 0x18, "vorbis_info.bitrate_nominal");
_Static_assert(offsetof(struct vorbis_info, bitrate_lower) == 0x20, "vorbis_info.bitrate_lower");
_Static_assert(offsetof(struct vorbis_info, bitrate_window) == 0x28, "vorbis_info.bitrate_window");
_Static_assert(offsetof(struct vorbis_info, codec_setup) == 0x30, "vorbis_info.codec_setup");
_Static_assert(sizeof(struct vorbis_dsp_state) == 0x90, "vorbis_dsp_state");
_Static_assert(offsetof(struct vorbis_dsp_state, analysisp) == 0x0, "vorbis_dsp_state.analysisp");
_Static_assert(offsetof(struct vorbis_dsp_state, vi) == 0x8, "vorbis_dsp_state.vi");
_Static_assert(offsetof(struct vorbis_dsp_state, pcm) == 0x10, "vorbis_dsp_state.pcm");
_Static_assert(offsetof(struct vorbis_dsp_state, pcmret) == 0x18, "vorbis_dsp_state.pcmret");
_Static_assert(offsetof(struct vorbis_dsp_state, pcm_storage) == 0x20, "vorbis_dsp_state.pcm_storage");
_Static_assert(offsetof(struct vorbis_dsp_state, pcm_current) == 0x24, "vorbis_dsp_state.pcm_current");
_Static_assert(offsetof(struct vorbis_dsp_state, pcm_returned) == 0x28, "vorbis_dsp_state.pcm_returned");
_Static_assert(offsetof(struct vorbis_dsp_state, preextrapolate) == 0x2c, "vorbis_dsp_state.preextrapolate");
_Static_assert(offsetof(struct vorbis_dsp_state, eofflag) == 0x30, "vorbis_dsp_state.eofflag");
_Static_assert(offsetof(struct vorbis_dsp_state, lW) == 0x38, "vorbis_dsp_state.lW");
_Static_assert(offsetof(struct vorbis_dsp_state, W) == 0x40, "vorbis_dsp_state.W");
_Static_assert(offsetof(struct vorbis_dsp_state, nW) == 0x48, "vorbis_dsp_state.nW");
_Static_assert(offsetof(struct vorbis_dsp_state, centerW) == 0x50, "vorbis_dsp_state.centerW");
_Static_assert(offsetof(struct vorbis_dsp_state, granulepos) == 0x58, "vorbis_dsp_state.granulepos");
_Static_assert(offsetof(struct vorbis_dsp_state, sequence) == 0x60, "vorbis_dsp_state.sequence");
_Static_assert(offsetof(struct vorbis_dsp_state, glue_bits) == 0x68, "vorbis_dsp_state.glue_bits");
_Static_assert(offsetof(struct vorbis_dsp_state, time_bits) == 0x70, "vorbis_dsp_state.time_bits");
_Static_assert(offsetof(struct vorbis_dsp_state, floor_bits) == 0x78, "vorbis_dsp_state.floor_bits");
_Static_assert(offsetof(struct vorbis_dsp_state, res_bits) == 0x80, "vorbis_dsp_state.res_bits");
_Static_assert(offsetof(struct vorbis_dsp_state, backend_state) == 0x88, "vorbis_dsp_state.backend_state");
_Static_assert(sizeof(struct vorbis_block) == 0xc0, "vorbis_block");
_Static_assert(offsetof(struct vorbis_block, pcm) == 0x0, "vorbis_block.pcm");
_Static_assert(offsetof(struct vorbis_block, opb) == 0x8, "vorbis_block.opb");
_Static_assert(offsetof(struct vorbis_block, lW) == 0x30, "vorbis_block.lW");
_Static_assert(offsetof(struct vorbis_block, W) == 0x38, "vorbis_block.W");
_Static_assert(offsetof(struct vorbis_block, nW) == 0x40, "vorbis_block.nW");
_Static_assert(offsetof(struct vorbis_block, pcmend) == 0x48, "vorbis_block.pcmend");
_Static_assert(offsetof(struct vorbis_block, mode) == 0x4c, "vorbis_block.mode");
_Static_assert(offsetof(struct vorbis_block, eofflag) == 0x50, "vorbis_block.eofflag");
_Static_assert(offsetof(struct vorbis_block, granulepos) == 0x58, "vorbis_block.granulepos");
_Static_assert(offsetof(struct vorbis_block, sequence) == 0x60, "vorbis_block.sequence");
_Static_assert(offsetof(struct vorbis_block, vd) == 0x68, "vorbis_block.vd");
_Static_assert(offsetof(struct vorbis_block, localstore) == 0x70, "vorbis_block.localstore");
_Static_assert(offsetof(struct vorbis_block, localtop) == 0x78, "vorbis_block.localtop");
_Static_assert(offsetof(struct vorbis_block, localalloc) == 0x80, "vorbis_block.localalloc");
_Static_assert(offsetof(struct vorbis_block, totaluse) == 0x88, "vorbis_block.totaluse");
_Static_assert(offsetof(struct vorbis_block, reap) == 0x90, "vorbis_block.reap");
_Static_assert(offsetof(struct vorbis_block, glue_bits) == 0x98, "vorbis_block.glue_bits");
_Static_assert(offsetof(struct vorbis_block, time_bits) == 0xa0, "vorbis_block.time_bits");
_Static_assert(offsetof(struct vorbis_block, floor_bits) == 0xa8, "vorbis_block.floor_bits");
_Static_assert(offsetof(struct vorbis_block, res_bits) == 0xb0, "vorbis_block.res_bits");
_Static_assert(offsetof(struct vorbis_block, internal) == 0xb8, "vorbis_block.internal");
_Static_assert(sizeof(struct alloc_chain) == 0x10, "alloc_chain");
_Static_assert(offsetof(struct alloc_chain, ptr) == 0x0, "alloc_chain.ptr");
_Static_assert(offsetof(struct alloc_chain, next) == 0x8, "alloc_chain.next");
_Static_assert(sizeof(struct vorbis_comment) == 0x20, "vorbis_comment");
_Static_assert(offsetof(struct vorbis_comment, user_comments) == 0x0, "vorbis_comment.user_comments");
_Static_assert(offsetof(struct vorbis_comment, comment_lengths) == 0x8, "vorbis_comment.comment_lengths");
_Static_assert(offsetof(struct vorbis_comment, comments) == 0x10, "vorbis_comment.comments");
_Static_assert(offsetof(struct vorbis_comment, vendor) == 0x18, "vorbis_comment.vendor");

#endif

