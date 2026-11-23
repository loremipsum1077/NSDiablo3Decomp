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

 function: toplevel libogg include
 last mod: $Id: ogg.h 17571 2010-10-27 13:28:20Z xiphmont $

 ********************************************************************/
#ifndef _OGG_H
#define _OGG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <ogg/os_types.h>

typedef struct {
  void *iov_base;
  size_t iov_len;
} ogg_iovec_t;

typedef struct {
  long endbyte;
  int  endbit;

  unsigned char *buffer;
  unsigned char *ptr;
  long storage;
} oggpack_buffer;

/* ogg_page is used to encapsulate the data in one Ogg bitstream page *****/

typedef struct {
  unsigned char *header;
  long header_len;
  unsigned char *body;
  long body_len;
} ogg_page;

/* ogg_stream_state contains the current encode/decode state of a logical
   Ogg bitstream **********************************************************/

typedef struct {
  unsigned char   *body_data;    /* bytes from packet bodies */
  long    body_storage;          /* storage elements allocated */
  long    body_fill;             /* elements stored; fill mark */
  long    body_returned;         /* elements of fill returned */


  int     *lacing_vals;      /* The values that will go to the segment table */
  ogg_int64_t *granule_vals; /* granulepos values for headers. Not compact
                                this way, but it is simple coupled to the
                                lacing fifo */
  long    lacing_storage;
  long    lacing_fill;
  long    lacing_packet;
  long    lacing_returned;

  unsigned char    header[282];      /* working space for header encode */
  int              header_fill;

  int     e_o_s;          /* set when we have buffered the last packet in the
                             logical bitstream */
  int     b_o_s;          /* set after we've written the initial page
                             of a logical bitstream */
  long    serialno;
  long    pageno;
  ogg_int64_t  packetno;  /* sequence number for decode; the framing
                             knows where there's a hole in the data,
                             but we need coupling so that the codec
                             (which is in a separate abstraction
                             layer) also knows about the gap */
  ogg_int64_t   granulepos;

} ogg_stream_state;

/* ogg_packet is used to encapsulate the data and metadata belonging
   to a single raw Ogg/Vorbis packet *************************************/

typedef struct {
  unsigned char *packet;
  long  bytes;
  long  b_o_s;
  long  e_o_s;

  ogg_int64_t  granulepos;

  ogg_int64_t  packetno;     /* sequence number for decode; the framing
                                knows where there's a hole in the data,
                                but we need coupling so that the codec
                                (which is in a separate abstraction
                                layer) also knows about the gap */
} ogg_packet;

typedef struct {
  unsigned char *data;
  int storage;
  int fill;
  int returned;

  int unsynced;
  int headerbytes;
  int bodybytes;
} ogg_sync_state;

/* Ogg BITSTREAM PRIMITIVES: bitstream ************************/

extern void  oggpack_writeinit(oggpack_buffer *b);
extern int   oggpack_writecheck(oggpack_buffer *b);
extern void  oggpack_writetrunc(oggpack_buffer *b,long bits);
extern void  oggpack_writealign(oggpack_buffer *b);
extern void  oggpack_writecopy(oggpack_buffer *b,void *source,long bits);
extern void  oggpack_reset(oggpack_buffer *b);
extern void  oggpack_writeclear(oggpack_buffer *b);
extern void  oggpack_readinit(oggpack_buffer *b,unsigned char *buf,int bytes);
extern void  oggpack_write(oggpack_buffer *b,unsigned long value,int bits);
extern long  oggpack_look(oggpack_buffer *b,int bits);
extern long  oggpack_look1(oggpack_buffer *b);
extern void  oggpack_adv(oggpack_buffer *b,int bits);
extern void  oggpack_adv1(oggpack_buffer *b);
extern long  oggpack_read(oggpack_buffer *b,int bits);
extern long  oggpack_read1(oggpack_buffer *b);
extern long  oggpack_bytes(oggpack_buffer *b);
extern long  oggpack_bits(oggpack_buffer *b);
extern unsigned char *oggpack_get_buffer(oggpack_buffer *b);

extern void  oggpackB_writeinit(oggpack_buffer *b);
extern int   oggpackB_writecheck(oggpack_buffer *b);
extern void  oggpackB_writetrunc(oggpack_buffer *b,long bits);
extern void  oggpackB_writealign(oggpack_buffer *b);
extern void  oggpackB_writecopy(oggpack_buffer *b,void *source,long bits);
extern void  oggpackB_reset(oggpack_buffer *b);
extern void  oggpackB_writeclear(oggpack_buffer *b);
extern void  oggpackB_readinit(oggpack_buffer *b,unsigned char *buf,int bytes);
extern void  oggpackB_write(oggpack_buffer *b,unsigned long value,int bits);
extern long  oggpackB_look(oggpack_buffer *b,int bits);
extern long  oggpackB_look1(oggpack_buffer *b);
extern void  oggpackB_adv(oggpack_buffer *b,int bits);
extern void  oggpackB_adv1(oggpack_buffer *b);
extern long  oggpackB_read(oggpack_buffer *b,int bits);
extern long  oggpackB_read1(oggpack_buffer *b);
extern long  oggpackB_bytes(oggpack_buffer *b);
extern long  oggpackB_bits(oggpack_buffer *b);
extern unsigned char *oggpackB_get_buffer(oggpack_buffer *b);

/* Ogg BITSTREAM PRIMITIVES: encoding **************************/

extern int      ogg_stream_packetin(ogg_stream_state *os, ogg_packet *op);
extern int      ogg_stream_iovecin(ogg_stream_state *os, ogg_iovec_t *iov,
                                   int count, long e_o_s, ogg_int64_t granulepos);
extern int      ogg_stream_pageout(ogg_stream_state *os, ogg_page *og);
extern int      ogg_stream_pageout_fill(ogg_stream_state *os, ogg_page *og, int nfill);
extern int      ogg_stream_flush(ogg_stream_state *os, ogg_page *og);

/* Ogg BITSTREAM PRIMITIVES: decoding **************************/

extern int      ogg_sync_init(ogg_sync_state *oy);
extern int      ogg_sync_clear(ogg_sync_state *oy);
extern int      ogg_sync_reset(ogg_sync_state *oy);
extern int      ogg_sync_destroy(ogg_sync_state *oy);
extern int      ogg_sync_check(ogg_sync_state *oy);

extern char    *ogg_sync_buffer(ogg_sync_state *oy, long size);
extern int      ogg_sync_wrote(ogg_sync_state *oy, long bytes);
extern long     ogg_sync_pageseek(ogg_sync_state *oy,ogg_page *og);
extern int      ogg_sync_pageout(ogg_sync_state *oy, ogg_page *og);
extern int      ogg_stream_pagein(ogg_stream_state *os, ogg_page *og);
extern int      ogg_stream_packetout(ogg_stream_state *os,ogg_packet *op);
extern int      ogg_stream_packetpeek(ogg_stream_state *os,ogg_packet *op);

/* Ogg BITSTREAM PRIMITIVES: general ***************************/

extern int      ogg_stream_init(ogg_stream_state *os,int serialno);
extern int      ogg_stream_clear(ogg_stream_state *os);
extern int      ogg_stream_reset(ogg_stream_state *os);
extern int      ogg_stream_reset_serialno(ogg_stream_state *os,int serialno);
extern int      ogg_stream_destroy(ogg_stream_state *os);
extern int      ogg_stream_check(ogg_stream_state *os);
extern int      ogg_stream_eos(ogg_stream_state *os);

extern void     ogg_page_checksum_set(ogg_page *og);

extern int      ogg_page_version(const ogg_page *og);
extern int      ogg_page_continued(const ogg_page *og);
extern int      ogg_page_bos(const ogg_page *og);
extern int      ogg_page_eos(const ogg_page *og);
extern ogg_int64_t  ogg_page_granulepos(const ogg_page *og);
extern int      ogg_page_serialno(const ogg_page *og);
extern long     ogg_page_pageno(const ogg_page *og);
extern int      ogg_page_packets(const ogg_page *og);

extern void     ogg_packet_clear(ogg_packet *op);


#ifdef __cplusplus
}
#endif


// Layout checks generated from the DWARF by tools/layout_asserts.py.
_Static_assert(sizeof(ogg_iovec_t) == 0x10, "ogg_iovec_t");
_Static_assert(offsetof(ogg_iovec_t, iov_base) == 0x0, "ogg_iovec_t.iov_base");
_Static_assert(offsetof(ogg_iovec_t, iov_len) == 0x8, "ogg_iovec_t.iov_len");
_Static_assert(sizeof(oggpack_buffer) == 0x28, "oggpack_buffer");
_Static_assert(offsetof(oggpack_buffer, endbyte) == 0x0, "oggpack_buffer.endbyte");
_Static_assert(offsetof(oggpack_buffer, endbit) == 0x8, "oggpack_buffer.endbit");
_Static_assert(offsetof(oggpack_buffer, buffer) == 0x10, "oggpack_buffer.buffer");
_Static_assert(offsetof(oggpack_buffer, ptr) == 0x18, "oggpack_buffer.ptr");
_Static_assert(offsetof(oggpack_buffer, storage) == 0x20, "oggpack_buffer.storage");
_Static_assert(sizeof(ogg_page) == 0x20, "ogg_page");
_Static_assert(offsetof(ogg_page, header) == 0x0, "ogg_page.header");
_Static_assert(offsetof(ogg_page, header_len) == 0x8, "ogg_page.header_len");
_Static_assert(offsetof(ogg_page, body) == 0x10, "ogg_page.body");
_Static_assert(offsetof(ogg_page, body_len) == 0x18, "ogg_page.body_len");
_Static_assert(sizeof(ogg_stream_state) == 0x198, "ogg_stream_state");
_Static_assert(offsetof(ogg_stream_state, body_data) == 0x0, "ogg_stream_state.body_data");
_Static_assert(offsetof(ogg_stream_state, body_storage) == 0x8, "ogg_stream_state.body_storage");
_Static_assert(offsetof(ogg_stream_state, body_fill) == 0x10, "ogg_stream_state.body_fill");
_Static_assert(offsetof(ogg_stream_state, body_returned) == 0x18, "ogg_stream_state.body_returned");
_Static_assert(offsetof(ogg_stream_state, lacing_vals) == 0x20, "ogg_stream_state.lacing_vals");
_Static_assert(offsetof(ogg_stream_state, granule_vals) == 0x28, "ogg_stream_state.granule_vals");
_Static_assert(offsetof(ogg_stream_state, lacing_storage) == 0x30, "ogg_stream_state.lacing_storage");
_Static_assert(offsetof(ogg_stream_state, lacing_fill) == 0x38, "ogg_stream_state.lacing_fill");
_Static_assert(offsetof(ogg_stream_state, lacing_packet) == 0x40, "ogg_stream_state.lacing_packet");
_Static_assert(offsetof(ogg_stream_state, lacing_returned) == 0x48, "ogg_stream_state.lacing_returned");
_Static_assert(offsetof(ogg_stream_state, header) == 0x50, "ogg_stream_state.header");
_Static_assert(offsetof(ogg_stream_state, header_fill) == 0x16c, "ogg_stream_state.header_fill");
_Static_assert(offsetof(ogg_stream_state, e_o_s) == 0x170, "ogg_stream_state.e_o_s");
_Static_assert(offsetof(ogg_stream_state, b_o_s) == 0x174, "ogg_stream_state.b_o_s");
_Static_assert(offsetof(ogg_stream_state, serialno) == 0x178, "ogg_stream_state.serialno");
_Static_assert(offsetof(ogg_stream_state, pageno) == 0x180, "ogg_stream_state.pageno");
_Static_assert(offsetof(ogg_stream_state, packetno) == 0x188, "ogg_stream_state.packetno");
_Static_assert(offsetof(ogg_stream_state, granulepos) == 0x190, "ogg_stream_state.granulepos");
_Static_assert(sizeof(ogg_packet) == 0x30, "ogg_packet");
_Static_assert(offsetof(ogg_packet, packet) == 0x0, "ogg_packet.packet");
_Static_assert(offsetof(ogg_packet, bytes) == 0x8, "ogg_packet.bytes");
_Static_assert(offsetof(ogg_packet, b_o_s) == 0x10, "ogg_packet.b_o_s");
_Static_assert(offsetof(ogg_packet, e_o_s) == 0x18, "ogg_packet.e_o_s");
_Static_assert(offsetof(ogg_packet, granulepos) == 0x20, "ogg_packet.granulepos");
_Static_assert(offsetof(ogg_packet, packetno) == 0x28, "ogg_packet.packetno");
_Static_assert(sizeof(ogg_sync_state) == 0x20, "ogg_sync_state");
_Static_assert(offsetof(ogg_sync_state, data) == 0x0, "ogg_sync_state.data");
_Static_assert(offsetof(ogg_sync_state, storage) == 0x8, "ogg_sync_state.storage");
_Static_assert(offsetof(ogg_sync_state, fill) == 0xc, "ogg_sync_state.fill");
_Static_assert(offsetof(ogg_sync_state, returned) == 0x10, "ogg_sync_state.returned");
_Static_assert(offsetof(ogg_sync_state, unsynced) == 0x14, "ogg_sync_state.unsynced");
_Static_assert(offsetof(ogg_sync_state, headerbytes) == 0x18, "ogg_sync_state.headerbytes");
_Static_assert(offsetof(ogg_sync_state, bodybytes) == 0x1c, "ogg_sync_state.bodybytes");

#endif  /* _OGG_H */
