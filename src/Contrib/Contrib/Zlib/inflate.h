/* inflate.h -- internal inflate state definition
 * Copyright (C) 1995-2004 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* define NO_GZIP when compiling if you want to disable gzip header and
   trailer decoding by inflate().  NO_GZIP would be used to avoid linking in
   the crc code when it is not needed.  For shared libraries, gzip decoding
   should be left enabled. */
#ifndef NO_GZIP
#  define GUNZIP
#endif

/* Possible inflate modes between inflate() calls */
typedef enum {
    HEAD,       /* i: waiting for magic header */
    FLAGS,      /* i: waiting for method and flags (gzip) */
    TIME,       /* i: waiting for modification time (gzip) */
    OS,         /* i: waiting for extra flags and operating system (gzip) */
    EXLEN,      /* i: waiting for extra length (gzip) */
    EXTRA,      /* i: waiting for extra bytes (gzip) */
    NAME,       /* i: waiting for end of file name (gzip) */
    COMMENT,    /* i: waiting for end of comment (gzip) */
    HCRC,       /* i: waiting for header crc (gzip) */
    DICTID,     /* i: waiting for dictionary check value */
    DICT,       /* waiting for inflateSetDictionary() call */
        TYPE,       /* i: waiting for type bits, including last-flag bit */
        TYPEDO,     /* i: same, but skip check to exit inflate on new block */
        STORED,     /* i: waiting for stored size (length and complement) */
        COPY,       /* i/o: waiting for input or output to copy stored block */
        TABLE,      /* i: waiting for dynamic block table lengths */
        LENLENS,    /* i: waiting for code length code lengths */
        CODELENS,   /* i: waiting for length/lit and distance code lengths */
            LEN,        /* i: waiting for length/lit code */
            LENEXT,     /* i: waiting for length extra bits */
            DIST,       /* i: waiting for distance code */
            DISTEXT,    /* i: waiting for distance extra bits */
            MATCH,      /* o: waiting for output space to copy string */
            LIT,        /* o: waiting for output space to write literal */
    CHECK,      /* i: waiting for 32-bit check value */
    LENGTH,     /* i: waiting for 32-bit length (gzip) */
    DONE,       /* finished check, done -- remain here until reset */
    BAD,        /* got a data error -- remain here until reset */
    MEM,        /* got an inflate() memory error -- remain here until reset */
    SYNC        /* looking for synchronization bytes to restart inflate() */
} inflate_mode;

/*
    State transitions between above modes -

    (most modes can go to the BAD or MEM mode -- not shown for clarity)

    Process header:
        HEAD -> (gzip) or (zlib)
        (gzip) -> FLAGS -> TIME -> OS -> EXLEN -> EXTRA -> NAME
        NAME -> COMMENT -> HCRC -> TYPE
        (zlib) -> DICTID or TYPE
        DICTID -> DICT -> TYPE
    Read deflate blocks:
            TYPE -> STORED or TABLE or LEN or CHECK
            STORED -> COPY -> TYPE
            TABLE -> LENLENS -> CODELENS -> LEN
    Read deflate codes:
                LEN -> LENEXT or LIT or TYPE
                LENEXT -> DIST -> DISTEXT -> MATCH -> LEN
                LIT -> LEN
    Process trailer:
        CHECK -> LENGTH -> DONE
 */

/* state maintained between inflate() calls.  Approximately 7K bytes. */
struct inflate_state {
    inflate_mode mode;          /* current inflate mode */
    int last;                   /* true if processing last block */
    int wrap;                   /* bit 0 true for zlib, bit 1 true for gzip */
    int havedict;               /* true if dictionary provided */
    int flags;                  /* gzip header method and flags (0 if zlib) */
    unsigned dmax;              /* zlib header max distance (INFLATE_STRICT) */
    unsigned long check;        /* protected copy of check value */
    unsigned long total;        /* protected copy of output count */
    gz_headerp head;            /* where to save gzip header information */
        /* sliding window */
    unsigned wbits;             /* log base 2 of requested window size */
    unsigned wsize;             /* window size or zero if not using window */
    unsigned whave;             /* valid bytes in the window */
    unsigned write;             /* window write index */
    unsigned char FAR *window;  /* allocated sliding window, if needed */
        /* bit accumulator */
    unsigned long hold;         /* input bit accumulator */
    unsigned bits;              /* number of bits in "in" */
        /* for string and stored block copying */
    unsigned length;            /* literal or length of data to copy */
    unsigned offset;            /* distance back to copy string from */
        /* for table and code decoding */
    unsigned extra;             /* extra bits needed */
        /* fixed and dynamic code tables */
    code const FAR *lencode;    /* starting table for length/literal codes */
    code const FAR *distcode;   /* starting table for distance codes */
    unsigned lenbits;           /* index bits for lencode */
    unsigned distbits;          /* index bits for distcode */
        /* dynamic table building */
    unsigned ncode;             /* number of code length code lengths */
    unsigned nlen;              /* number of length code lengths */
    unsigned ndist;             /* number of distance code lengths */
    unsigned have;              /* number of code lengths in lens[] */
    code FAR *next;             /* next available space in codes[] */
    unsigned short lens[320];   /* temporary storage for code lengths */
    unsigned short work[288];   /* work area for code table building */
    code codes[ENOUGH];         /* space for code tables */
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(struct inflate_state) == 0x2550, "inflate_state");
_Static_assert(offsetof(struct inflate_state, mode) == 0x0, "inflate_state.mode");
_Static_assert(offsetof(struct inflate_state, last) == 0x4, "inflate_state.last");
_Static_assert(offsetof(struct inflate_state, wrap) == 0x8, "inflate_state.wrap");
_Static_assert(offsetof(struct inflate_state, havedict) == 0xc, "inflate_state.havedict");
_Static_assert(offsetof(struct inflate_state, flags) == 0x10, "inflate_state.flags");
_Static_assert(offsetof(struct inflate_state, dmax) == 0x14, "inflate_state.dmax");
_Static_assert(offsetof(struct inflate_state, check) == 0x18, "inflate_state.check");
_Static_assert(offsetof(struct inflate_state, total) == 0x20, "inflate_state.total");
_Static_assert(offsetof(struct inflate_state, head) == 0x28, "inflate_state.head");
_Static_assert(offsetof(struct inflate_state, wbits) == 0x30, "inflate_state.wbits");
_Static_assert(offsetof(struct inflate_state, wsize) == 0x34, "inflate_state.wsize");
_Static_assert(offsetof(struct inflate_state, whave) == 0x38, "inflate_state.whave");
_Static_assert(offsetof(struct inflate_state, write) == 0x3c, "inflate_state.write");
_Static_assert(offsetof(struct inflate_state, window) == 0x40, "inflate_state.window");
_Static_assert(offsetof(struct inflate_state, hold) == 0x48, "inflate_state.hold");
_Static_assert(offsetof(struct inflate_state, bits) == 0x50, "inflate_state.bits");
_Static_assert(offsetof(struct inflate_state, length) == 0x54, "inflate_state.length");
_Static_assert(offsetof(struct inflate_state, offset) == 0x58, "inflate_state.offset");
_Static_assert(offsetof(struct inflate_state, extra) == 0x5c, "inflate_state.extra");
_Static_assert(offsetof(struct inflate_state, lencode) == 0x60, "inflate_state.lencode");
_Static_assert(offsetof(struct inflate_state, distcode) == 0x68, "inflate_state.distcode");
_Static_assert(offsetof(struct inflate_state, lenbits) == 0x70, "inflate_state.lenbits");
_Static_assert(offsetof(struct inflate_state, distbits) == 0x74, "inflate_state.distbits");
_Static_assert(offsetof(struct inflate_state, ncode) == 0x78, "inflate_state.ncode");
_Static_assert(offsetof(struct inflate_state, nlen) == 0x7c, "inflate_state.nlen");
_Static_assert(offsetof(struct inflate_state, ndist) == 0x80, "inflate_state.ndist");
_Static_assert(offsetof(struct inflate_state, have) == 0x84, "inflate_state.have");
_Static_assert(offsetof(struct inflate_state, next) == 0x88, "inflate_state.next");
_Static_assert(offsetof(struct inflate_state, lens) == 0x90, "inflate_state.lens");
_Static_assert(offsetof(struct inflate_state, work) == 0x310, "inflate_state.work");
_Static_assert(offsetof(struct inflate_state, codes) == 0x550, "inflate_state.codes");
