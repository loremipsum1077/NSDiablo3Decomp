#include "bdTomcryptBackport.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_pk.h"
#include <stddef.h>
#include <string.h>
// Use the bundled upstream math descriptor rather than the shared partial declaration.
#define MRSA
#define MECC
typedef Prng_state prng_state;
extern "C" {
#include "../../external/libtomcrypt/upstream-headers/tomcrypt_math.h"
union Hash_state;
typedef Hash_state hash_state;
extern  struct ltc_hash_descriptor {
    /** name of hash */
    char *name;
    /** internal ID */
    unsigned char ID;
    /** Size of digest in octets */
    unsigned long hashsize;
    /** Input block size in octets */
    unsigned long blocksize;
    /** ASN.1 OID */
    unsigned long OID[16];
    /** Length of DER encoding */
    unsigned long OIDlen;

    /** Init a hash state
      @param hash   The hash to initialize
      @return CRYPT_OK if successful
    */
    int (*init)(hash_state *hash);
    /** Process a block of data 
      @param hash   The hash state
      @param in     The data to hash
      @param inlen  The length of the data (octets)
      @return CRYPT_OK if successful
    */
    int (*process)(hash_state *hash, const unsigned char *in, unsigned long inlen);
    /** Produce the digest and store it
      @param hash   The hash state
      @param out    [out] The destination of the digest
      @return CRYPT_OK if successful
    */
    int (*done)(hash_state *hash, unsigned char *out);
    /** Self-test
      @return CRYPT_OK if successful, CRYPT_NOP if self-tests have been disabled
    */
    int (*test)(void);

    /* accelerated hmac callback: if you need to-do multiple packets just use the generic hmac_memory and provide a hash callback */
    int  (*hmac_block)(const unsigned char *key, unsigned long  keylen,
                       const unsigned char *in,  unsigned long  inlen, 
                             unsigned char *out, unsigned long *outlen);

} hash_descriptor[];
int hash_is_valid(int);
void* libTomCryptMalloc(size_t);
void libTomCryptFree(void*);
int der_decode_sequence_ex(const unsigned char*, unsigned long, ltc_asn1_list*, unsigned long, int);
int der_length_sequence(ltc_asn1_list*, unsigned long, unsigned long*);
int pkcs_1_pss_decode(const unsigned char*, unsigned long, const unsigned char*, unsigned long, unsigned long, int, unsigned long, int*);
}

static_assert(sizeof(ltc_math_descriptor) == 0x168, "ltc_math_descriptor");
static_assert(offsetof(ltc_math_descriptor, unsigned_size) == 0x88, "unsigned_size");
static_assert(offsetof(ltc_math_descriptor, rsa_me) == 0x160, "rsa_me");
static_assert(sizeof(ltc_hash_descriptor) == 0xd0, "ltc_hash_descriptor");
static_assert(offsetof(ltc_hash_descriptor, OIDlen) == 0xa0, "OIDlen");

// libtomcrypt helper macros used by the original (tomcrypt_custom.h / tomcrypt_pk.h); this build's
// XMALLOC/XFREE are libTomCryptMalloc/libTomCryptFree (calls in the original), XMEM_NEQ is mem_neq.
#define XMALLOC libTomCryptMalloc
#define XFREE libTomCryptFree
#define XMEMCPY memcpy
#define XMEM_NEQ mem_neq
#define der_decode_sequence(in, inlen, list, outlen) der_decode_sequence_ex(in, inlen, list, outlen, 1)
#define LTC_SET_ASN1(list, index, Type, Data, Size)  \
   do {                                              \
      int LTC_MACRO_temp            = (index);       \
      ltc_asn1_list *LTC_MACRO_list = (list);        \
      LTC_MACRO_list[LTC_MACRO_temp].type = (Type);  \
      LTC_MACRO_list[LTC_MACRO_temp].data = (void*)(Data);  \
      LTC_MACRO_list[LTC_MACRO_temp].size = (Size);  \
      LTC_MACRO_list[LTC_MACRO_temp].used = 0;       \
      } while (0);
#line 506
int mem_neq(const void *a, const void *b, size_t len)
{
   unsigned char ret = 0;
   const unsigned char* pa;
   const unsigned char* pb;




   pa = static_cast<const unsigned char*>(a);
   pb = static_cast<const unsigned char*>(b);

   while (len-- > 0) {
      ret |= *pa ^ *pb;
      ++pa;
      ++pb;
   }

   ret |= ret >> 4;
   ret |= ret >> 2;
   ret |= ret >> 1;
   ret &= 1;

   return ret;
}

#line 161
int pkcs_1_v1_5_decode(const unsigned char *msg,
                             unsigned long  msglen,
                                       int  block_type,
                             unsigned long  modulus_bitlen,
                             unsigned char *out,
                             unsigned long *outlen,
                                       int *is_valid)
{
  unsigned long modulus_len, ps_len, i;
  int result;

  /* default to invalid packet */
  *is_valid = 0;

  modulus_len = (modulus_bitlen >> 3) + (modulus_bitlen & 7 ? 1 : 0);

  /* test message size */

  if ((msglen > modulus_len) || (modulus_len < 11)) {
    return CRYPT_PK_INVALID_SIZE;
  }

  result = CRYPT_OK;

  /* separate encoded message */

  if ((msg[0] != 0x00) || (msg[1] != (unsigned char)block_type)) {
    result = CRYPT_INVALID_PACKET;
  }

  if (block_type == LTC_PKCS_1_EME) {
    for (i = 2; i < modulus_len; i++) {
      /* separator */
      if (msg[i] == 0x00) { break; }
    }
    ps_len = i++ - 2;

    if (i > modulus_len) {


      result = CRYPT_INVALID_PACKET;
    }
  } else {
    for (i = 2; i < modulus_len - 1; i++) {
       if (msg[i] != 0xFF) { break; }
    }

    /* separator check */
    if (msg[i] != 0) {

      result = CRYPT_INVALID_PACKET;
    }

    ps_len = i - 2;
  }

  if (ps_len < 8)
  {


    result = CRYPT_INVALID_PACKET;
  }

  if (*outlen < (msglen - (2 + ps_len + 1))) {
    result = CRYPT_INVALID_PACKET;
  }

  if (result == CRYPT_OK) {
     *outlen = (msglen - (2 + ps_len + 1));
     XMEMCPY(out, &msg[2 + ps_len + 1], *outlen);

     /* valid packet */
     *is_valid = 1;
  }

  return result;
}

#line 351
// libtomcrypt rsa_verify_hash.c layout (DWARF decl lines 352-360, 424-427; LTC_SET_ASN1 locals 457-460)
int rsa_verify_hash_ex(const unsigned char *sig,      unsigned long siglen,
                       const unsigned char *hash,     unsigned long hashlen,
                             int            padding,
                             int            hash_idx, unsigned long saltlen,
                             int           *stat,     rsa_key      *key)
{
  unsigned long modulus_bitlen, modulus_bytelen, x;
  int           err;
  unsigned char *tmpbuf;






  /* default to invalid */
  *stat = 0;

  /* valid padding? */

  if ((padding != LTC_PKCS_1_V1_5) &&
      (padding != LTC_PKCS_1_PSS)) {
    return CRYPT_ERROR;
  }

  if (padding == LTC_PKCS_1_PSS) {
    /* valid hash ? */
    if ((err = hash_is_valid(hash_idx)) != CRYPT_OK) {
       return err;
    }
  }

  /* get modulus len in bits */
  modulus_bitlen = ltc_mp.count_bits( (key->N));

  /* outlen must be at least the size of the modulus */
  modulus_bytelen = ltc_mp.unsigned_size( (key->N));
  if (modulus_bytelen != siglen) {
     return CRYPT_INVALID_PACKET;
  }

  /* allocate temp buffer for decoded sig */
  tmpbuf = (unsigned char*)XMALLOC(siglen);
  if (tmpbuf == NULL) {
     return CRYPT_MEM;
  }

  /* RSA decode it  */
  x = siglen;
  if ((err = ltc_mp.rsa_me(sig, siglen, tmpbuf, &x, PK_PUBLIC, key)) != CRYPT_OK) {
     XFREE(tmpbuf);
     return err;
  }

  /* make sure the output is the right size */
  if (x != siglen) {
     XFREE(tmpbuf);
     return CRYPT_INVALID_PACKET;
  }

  if (padding == LTC_PKCS_1_PSS) {
    /* PSS decode and verify it */

    if(modulus_bitlen%8 == 1){
      err = pkcs_1_pss_decode(hash, hashlen, tmpbuf+1, x-1, saltlen, hash_idx, modulus_bitlen, stat);
    }
    else{
      err = pkcs_1_pss_decode(hash, hashlen, tmpbuf, x, saltlen, hash_idx, modulus_bitlen, stat);
    }

  } else {
    /* PKCS #1 v1.5 decode it */
    unsigned char *out;
    unsigned long outlen, loid[16], reallen;
    int           decoded;
    ltc_asn1_list digestinfo[2], siginfo[2];

    /* not all hashes have OIDs... so sad */
    if (hash_descriptor[hash_idx].OIDlen == 0) {
       err = CRYPT_INVALID_ARG;
       goto bail_2;
    }

    /* allocate temp buffer for decoded hash */
    outlen = ((modulus_bitlen >> 3) + (modulus_bitlen & 7 ? 1 : 0)) - 3;
    out    = (unsigned char*)XMALLOC(outlen);
    if (out == NULL) {
      err = CRYPT_MEM;
      goto bail_2;
    }

    if ((err = pkcs_1_v1_5_decode(tmpbuf, x, LTC_PKCS_1_EMSA, modulus_bitlen, out, &outlen, &decoded)) != CRYPT_OK) {
      XFREE(out);
      goto bail_2;
    }

    /* now we must decode out[0...outlen-1] using ASN.1, test the OID and then test the hash */








    LTC_SET_ASN1(digestinfo, 0, LTC_ASN1_OBJECT_IDENTIFIER, loid, sizeof(loid)/sizeof(loid[0]));
    LTC_SET_ASN1(digestinfo, 1, LTC_ASN1_NULL,              NULL,                          0);
    LTC_SET_ASN1(siginfo,    0, LTC_ASN1_SEQUENCE,          digestinfo,                    2);
    LTC_SET_ASN1(siginfo,    1, LTC_ASN1_OCTET_STRING,      tmpbuf,                        siglen);

    if ((err = der_decode_sequence(out, outlen, siginfo, 2)) != CRYPT_OK) {
       XFREE(out);
       goto bail_2;
    }

    if ((err = der_length_sequence(siginfo, 2, &reallen)) != CRYPT_OK) {
       XFREE(out);
       goto bail_2;
    }

    /* test OID */
    if ((reallen == outlen) &&
        (digestinfo[0].size == hash_descriptor[hash_idx].OIDlen) &&
        (XMEM_NEQ(digestinfo[0].data, hash_descriptor[hash_idx].OID, sizeof(unsigned long) * hash_descriptor[hash_idx].OIDlen) == 0) &&
        (siginfo[1].size == hashlen) &&
        (XMEM_NEQ(siginfo[1].data, hash, hashlen) == 0)) {
       *stat = 1;
    }




    XFREE(out);
  }

bail_2:

  XFREE(tmpbuf);
  return err;
}
