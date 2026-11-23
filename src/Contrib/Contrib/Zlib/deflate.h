/* deflate.h -- internal compression state
 * Copyright (C) 1995-2004 Jean-loup Gailly
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* @(#) $Id$ */

#ifndef DEFLATE_H
#define DEFLATE_H

#include "zutil.h"

/* define NO_GZIP when compiling if you want to disable gzip header and
   trailer creation by deflate().  NO_GZIP would be used to avoid linking in
   the crc code when it is not needed.  For shared libraries, gzip encoding
   should be left enabled. */
#ifndef NO_GZIP
#  define GZIP
#endif

/* ===========================================================================
 * Internal compression state.
 */

#define LENGTH_CODES 29
/* number of length codes, not counting the special END_BLOCK code */

#define LITERALS  256
/* number of literal bytes 0..255 */

#define L_CODES (LITERALS+1+LENGTH_CODES)
/* number of Literal or Length codes, including the END_BLOCK code */

#define D_CODES   30
/* number of distance codes */

#define BL_CODES  19
/* number of codes used to transfer the bit lengths */

#define HEAP_SIZE (2*L_CODES+1)
/* maximum heap size */

#define MAX_BITS 15
/* All codes must not exceed MAX_BITS bits */

#define INIT_STATE    42
#define EXTRA_STATE   69
#define NAME_STATE    73
#define COMMENT_STATE 91
#define HCRC_STATE   103
#define BUSY_STATE   113
#define FINISH_STATE 666
/* Stream status */


/* Data structure describing a single value and its code string. */
typedef struct ct_data_s {
    union {
        ush  freq;       /* frequency count */
        ush  code;       /* bit string */
    } fc;
    union {
        ush  dad;        /* father node in Huffman tree */
        ush  len;        /* length of bit string */
    } dl;
} FAR ct_data;

#define Freq fc.freq
#define Code fc.code
#define Dad  dl.dad
#define Len  dl.len

typedef struct static_tree_desc_s  static_tree_desc;

typedef struct tree_desc_s {
    ct_data *dyn_tree;           /* the dynamic tree */
    int     max_code;            /* largest code with non zero frequency */
    static_tree_desc *stat_desc; /* the corresponding static tree */
} FAR tree_desc;

typedef ush Pos;
typedef Pos FAR Posf;
typedef unsigned IPos;

/* A Pos is an index in the character window. We use short instead of int to
 * save space in the various tables. IPos is used only for parameter passing.
 */

typedef struct internal_state {
    z_streamp strm;      /* pointer back to this zlib stream */
    int   status;        /* as the name implies */
    Bytef *pending_buf;  /* output still pending */
    ulg   pending_buf_size; /* size of pending_buf */
    Bytef *pending_out;  /* next pending byte to output to the stream */
    uInt   pending;      /* nb of bytes in the pending buffer */
    int   wrap;          /* bit 0 true for zlib, bit 1 true for gzip */
    gz_headerp  gzhead;  /* gzip header information to write */
    uInt   gzindex;      /* where in extra, name, or comment */
    Byte  method;        /* STORED (for zip only) or DEFLATED */
    int   last_flush;    /* value of flush param for previous deflate call */

                /* used by deflate.c: */

    uInt  w_size;        /* LZ77 window size (32K by default) */
    uInt  w_bits;        /* log2(w_size)  (8..16) */
    uInt  w_mask;        /* w_size - 1 */

    Bytef *window;
    /* Sliding window. Input bytes are read into the second half of the window,
     * and move to the first half later to keep a dictionary of at least wSize
     * bytes. With this organization, matches are limited to a distance of
     * wSize-MAX_MATCH bytes, but this ensures that IO is always
     * performed with a length multiple of the block size. Also, it limits
     * the window size to 64K, which is quite useful on MSDOS.
     * To do: use the user input buffer as sliding window.
     */

    ulg window_size;
    /* Actual size of window: 2*wSize, except when the user input buffer
     * is directly used as sliding window.
     */

    Posf *prev;
    /* Link to older string with same hash index. To limit the size of this
     * array to 64K, this link is maintained only for the last 32K strings.
     * An index in this array is thus a window index modulo 32K.
     */

    Posf *head; /* Heads of the hash chains or NIL. */

    uInt  ins_h;          /* hash index of string to be inserted */
    uInt  hash_size;      /* number of elements in hash table */
    uInt  hash_bits;      /* log2(hash_size) */
    uInt  hash_mask;      /* hash_size-1 */

    uInt  hash_shift;
    /* Number of bits by which ins_h must be shifted at each input
     * step. It must be such that after MIN_MATCH steps, the oldest
     * byte no longer takes part in the hash key, that is:
     *   hash_shift * MIN_MATCH >= hash_bits
     */

    long block_start;
    /* Window position at the beginning of the current output block. Gets
     * negative when the window is moved backwards.
     */

    uInt match_length;           /* length of best match */
    IPos prev_match;             /* previous match */
    int match_available;         /* set if previous match exists */
    uInt strstart;               /* start of string to insert */
    uInt match_start;            /* start of matching string */
    uInt lookahead;              /* number of valid bytes ahead in window */

    uInt prev_length;
    /* Length of the best match at previous step. Matches not greater than this
     * are discarded. This is used in the lazy match evaluation.
     */

    uInt max_chain_length;
    /* To speed up deflation, hash chains are never searched beyond this
     * length.  A higher limit improves compression ratio but degrades the
     * speed.
     */

    uInt max_lazy_match;
    /* Attempt to find a better match only when the current match is strictly
     * smaller than this value. This mechanism is used only for compression
     * levels >= 4.
     */
#   define max_insert_length  max_lazy_match
    /* Insert new strings in the hash table only if the match length is not
     * greater than this length. This saves time but degrades compression.
     * max_insert_length is used only for compression levels <= 3.
     */

    int level;    /* compression level (1..9) */
    int strategy; /* favor or force Huffman coding*/

    uInt good_match;
    /* Use a faster search when the previous match is longer than this */

    int nice_match; /* Stop searching when current match exceeds this */

                /* used by trees.c: */
    /* Didn't use ct_data typedef below to supress compiler warning */
    struct ct_data_s dyn_ltree[HEAP_SIZE];   /* literal and length tree */
    struct ct_data_s dyn_dtree[2*D_CODES+1]; /* distance tree */
    struct ct_data_s bl_tree[2*BL_CODES+1];  /* Huffman tree for bit lengths */

    struct tree_desc_s l_desc;               /* desc. for literal tree */
    struct tree_desc_s d_desc;               /* desc. for distance tree */
    struct tree_desc_s bl_desc;              /* desc. for bit length tree */

    ush bl_count[MAX_BITS+1];
    /* number of codes at each bit length for an optimal tree */

    int heap[2*L_CODES+1];      /* heap used to build the Huffman trees */
    int heap_len;               /* number of elements in the heap */
    int heap_max;               /* element of largest frequency */
    /* The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not used.
     * The same heap array is used to build all trees.
     */

    uch depth[2*L_CODES+1];
    /* Depth of each subtree used as tie breaker for trees of equal frequency
     */

    uchf *l_buf;          /* buffer for literals or lengths */

    uInt  lit_bufsize;
    /* Size of match buffer for literals/lengths.  There are 4 reasons for
     * limiting lit_bufsize to 64K:
     *   - frequencies can be kept in 16 bit counters
     *   - if compression is not successful for the first block, all input
     *     data is still in the window so we can still emit a stored block even
     *     when input comes from standard input.  (This can also be done for
     *     all blocks if lit_bufsize is not greater than 32K.)
     *   - if compression is not successful for a file smaller than 64K, we can
     *     even emit a stored file instead of a stored block (saving 5 bytes).
     *     This is applicable only for zip (not gzip or zlib).
     *   - creating new Huffman trees less frequently may not provide fast
     *     adaptation to changes in the input data statistics. (Take for
     *     example a binary file with poorly compressible code followed by
     *     a highly compressible string table.) Smaller buffer sizes give
     *     fast adaptation but have of course the overhead of transmitting
     *     trees more frequently.
     *   - I can't count above 4
     */

    uInt last_lit;      /* running index in l_buf */

    ushf *d_buf;
    /* Buffer for distances. To simplify the code, d_buf and l_buf have
     * the same number of elements. To use different lengths, an extra flag
     * array would be necessary.
     */

    ulg opt_len;        /* bit length of current block with optimal trees */
    ulg static_len;     /* bit length of current block with static trees */
    uInt matches;       /* number of string matches in current block */
    int last_eob_len;   /* bit length of EOB code for last block */

#ifdef DEBUG
    ulg compressed_len; /* total bit length of compressed file mod 2^32 */
    ulg bits_sent;      /* bit length of compressed data sent mod 2^32 */
#endif

    ush bi_buf;
    /* Output buffer. bits are inserted starting at the bottom (least
     * significant bits).
     */
    int bi_valid;
    /* Number of valid bits in bi_buf.  All bits above the last valid bit
     * are always zero.
     */

} FAR deflate_state;

/* Output a byte on the stream.
 * IN assertion: there is enough room in pending_buf.
 */
#define put_byte(s, c) {s->pending_buf[s->pending++] = (c);}


#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */

#define MAX_DIST(s)  ((s)->w_size-MIN_LOOKAHEAD)
/* In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to MAX_DIST instead of WSIZE.
 */

        /* in trees.c */
void _tr_init         OF((deflate_state *s));
int  _tr_tally        OF((deflate_state *s, unsigned dist, unsigned lc));
void _tr_flush_block  OF((deflate_state *s, charf *buf, ulg stored_len,
                          int eof));
void _tr_align        OF((deflate_state *s));
void _tr_stored_block OF((deflate_state *s, charf *buf, ulg stored_len,
                          int eof));

#define d_code(dist) \
   ((dist) < 256 ? _dist_code[dist] : _dist_code[256+((dist)>>7)])
/* Mapping from a distance to a distance code. dist is the distance - 1 and
 * must not have side effects. _dist_code[256] and _dist_code[257] are never
 * used.
 */

#ifndef DEBUG
/* Inline versions of _tr_tally for speed: */

#if defined(GEN_TREES_H) || !defined(STDC)
  extern uch _length_code[];
  extern uch _dist_code[];
#else
  extern const uch _length_code[];
  extern const uch _dist_code[];
#endif

# define _tr_tally_lit(s, c, flush) \
  { uch cc = (c); \
    s->d_buf[s->last_lit] = 0; \
    s->l_buf[s->last_lit++] = cc; \
    s->dyn_ltree[cc].Freq++; \
    flush = (s->last_lit == s->lit_bufsize-1); \
   }
# define _tr_tally_dist(s, distance, length, flush) \
  { uch len = (length); \
    ush dist = (distance); \
    s->d_buf[s->last_lit] = dist; \
    s->l_buf[s->last_lit++] = len; \
    dist--; \
    s->dyn_ltree[_length_code[len]+LITERALS+1].Freq++; \
    s->dyn_dtree[d_code(dist)].Freq++; \
    flush = (s->last_lit == s->lit_bufsize-1); \
  }
#else
# define _tr_tally_lit(s, c, flush) flush = _tr_tally(s, 0, c)
# define _tr_tally_dist(s, distance, length, flush) \
              flush = _tr_tally(s, distance, length)
#endif


// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(struct ct_data_s) == 0x4, "ct_data_s");
_Static_assert(sizeof(struct tree_desc_s) == 0x18, "tree_desc_s");
_Static_assert(offsetof(struct tree_desc_s, dyn_tree) == 0x0, "tree_desc_s.dyn_tree");
_Static_assert(offsetof(struct tree_desc_s, max_code) == 0x8, "tree_desc_s.max_code");
_Static_assert(offsetof(struct tree_desc_s, stat_desc) == 0x10, "tree_desc_s.stat_desc");
_Static_assert(sizeof(struct internal_state) == 0x1728, "internal_state");
_Static_assert(offsetof(struct internal_state, strm) == 0x0, "internal_state.strm");
_Static_assert(offsetof(struct internal_state, status) == 0x8, "internal_state.status");
_Static_assert(offsetof(struct internal_state, pending_buf) == 0x10, "internal_state.pending_buf");
_Static_assert(offsetof(struct internal_state, pending_buf_size) == 0x18, "internal_state.pending_buf_size");
_Static_assert(offsetof(struct internal_state, pending_out) == 0x20, "internal_state.pending_out");
_Static_assert(offsetof(struct internal_state, pending) == 0x28, "internal_state.pending");
_Static_assert(offsetof(struct internal_state, wrap) == 0x2c, "internal_state.wrap");
_Static_assert(offsetof(struct internal_state, gzhead) == 0x30, "internal_state.gzhead");
_Static_assert(offsetof(struct internal_state, gzindex) == 0x38, "internal_state.gzindex");
_Static_assert(offsetof(struct internal_state, method) == 0x3c, "internal_state.method");
_Static_assert(offsetof(struct internal_state, last_flush) == 0x40, "internal_state.last_flush");
_Static_assert(offsetof(struct internal_state, w_size) == 0x44, "internal_state.w_size");
_Static_assert(offsetof(struct internal_state, w_bits) == 0x48, "internal_state.w_bits");
_Static_assert(offsetof(struct internal_state, w_mask) == 0x4c, "internal_state.w_mask");
_Static_assert(offsetof(struct internal_state, window) == 0x50, "internal_state.window");
_Static_assert(offsetof(struct internal_state, window_size) == 0x58, "internal_state.window_size");
_Static_assert(offsetof(struct internal_state, prev) == 0x60, "internal_state.prev");
_Static_assert(offsetof(struct internal_state, head) == 0x68, "internal_state.head");
_Static_assert(offsetof(struct internal_state, ins_h) == 0x70, "internal_state.ins_h");
_Static_assert(offsetof(struct internal_state, hash_size) == 0x74, "internal_state.hash_size");
_Static_assert(offsetof(struct internal_state, hash_bits) == 0x78, "internal_state.hash_bits");
_Static_assert(offsetof(struct internal_state, hash_mask) == 0x7c, "internal_state.hash_mask");
_Static_assert(offsetof(struct internal_state, hash_shift) == 0x80, "internal_state.hash_shift");
_Static_assert(offsetof(struct internal_state, block_start) == 0x88, "internal_state.block_start");
_Static_assert(offsetof(struct internal_state, match_length) == 0x90, "internal_state.match_length");
_Static_assert(offsetof(struct internal_state, prev_match) == 0x94, "internal_state.prev_match");
_Static_assert(offsetof(struct internal_state, match_available) == 0x98, "internal_state.match_available");
_Static_assert(offsetof(struct internal_state, strstart) == 0x9c, "internal_state.strstart");
_Static_assert(offsetof(struct internal_state, match_start) == 0xa0, "internal_state.match_start");
_Static_assert(offsetof(struct internal_state, lookahead) == 0xa4, "internal_state.lookahead");
_Static_assert(offsetof(struct internal_state, prev_length) == 0xa8, "internal_state.prev_length");
_Static_assert(offsetof(struct internal_state, max_chain_length) == 0xac, "internal_state.max_chain_length");
_Static_assert(offsetof(struct internal_state, max_lazy_match) == 0xb0, "internal_state.max_lazy_match");
_Static_assert(offsetof(struct internal_state, level) == 0xb4, "internal_state.level");
_Static_assert(offsetof(struct internal_state, strategy) == 0xb8, "internal_state.strategy");
_Static_assert(offsetof(struct internal_state, good_match) == 0xbc, "internal_state.good_match");
_Static_assert(offsetof(struct internal_state, nice_match) == 0xc0, "internal_state.nice_match");
_Static_assert(offsetof(struct internal_state, dyn_ltree) == 0xc4, "internal_state.dyn_ltree");
_Static_assert(offsetof(struct internal_state, dyn_dtree) == 0x9b8, "internal_state.dyn_dtree");
_Static_assert(offsetof(struct internal_state, bl_tree) == 0xaac, "internal_state.bl_tree");
_Static_assert(offsetof(struct internal_state, l_desc) == 0xb48, "internal_state.l_desc");
_Static_assert(offsetof(struct internal_state, d_desc) == 0xb60, "internal_state.d_desc");
_Static_assert(offsetof(struct internal_state, bl_desc) == 0xb78, "internal_state.bl_desc");
_Static_assert(offsetof(struct internal_state, bl_count) == 0xb90, "internal_state.bl_count");
_Static_assert(offsetof(struct internal_state, heap) == 0xbb0, "internal_state.heap");
_Static_assert(offsetof(struct internal_state, heap_len) == 0x14a4, "internal_state.heap_len");
_Static_assert(offsetof(struct internal_state, heap_max) == 0x14a8, "internal_state.heap_max");
_Static_assert(offsetof(struct internal_state, depth) == 0x14ac, "internal_state.depth");
_Static_assert(offsetof(struct internal_state, l_buf) == 0x16f0, "internal_state.l_buf");
_Static_assert(offsetof(struct internal_state, lit_bufsize) == 0x16f8, "internal_state.lit_bufsize");
_Static_assert(offsetof(struct internal_state, last_lit) == 0x16fc, "internal_state.last_lit");
_Static_assert(offsetof(struct internal_state, d_buf) == 0x1700, "internal_state.d_buf");
_Static_assert(offsetof(struct internal_state, opt_len) == 0x1708, "internal_state.opt_len");
_Static_assert(offsetof(struct internal_state, static_len) == 0x1710, "internal_state.static_len");
_Static_assert(offsetof(struct internal_state, matches) == 0x1718, "internal_state.matches");
_Static_assert(offsetof(struct internal_state, last_eob_len) == 0x171c, "internal_state.last_eob_len");
_Static_assert(offsetof(struct internal_state, bi_buf) == 0x1720, "internal_state.bi_buf");
_Static_assert(offsetof(struct internal_state, bi_valid) == 0x1724, "internal_state.bi_valid");

#endif /* DEFLATE_H */
