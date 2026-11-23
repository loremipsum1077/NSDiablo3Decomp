/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2013, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include <stddef.h>
#include <string.h>

/* Original hash.h and llist.h declarations, confirmed by this owner's DWARF.
 * Kept local while this owner is recovered independently. */
typedef void (*curl_llist_dtor)(void *, void *);

struct curl_llist_element {
  void *ptr;
  struct curl_llist_element *prev;
  struct curl_llist_element *next;
};

struct curl_llist {
  struct curl_llist_element *head;
  struct curl_llist_element *tail;
  curl_llist_dtor dtor;
  size_t size;
};

typedef size_t (*hash_function)(void *, size_t, size_t);
typedef size_t (*comp_function)(void *, size_t, void *, size_t);
typedef void (*curl_hash_dtor)(void *);

struct curl_hash {
  struct curl_llist **table;
  hash_function hash_func;
  comp_function comp_func;
  curl_hash_dtor dtor;
  int slots;
  size_t size;
};

struct curl_hash_element {
  void *ptr;
  char *key;
  size_t key_len;
};

struct curl_hash_iterator {
  struct curl_hash *hash;
  int slot_index;
  struct curl_llist_element *current_element;
};

_Static_assert(sizeof(size_t) == 8, "size_t");
_Static_assert(sizeof(struct curl_llist_element) == 24, "curl_llist_element");
_Static_assert(offsetof(struct curl_llist_element, prev) == 8, "element prev");
_Static_assert(offsetof(struct curl_llist_element, next) == 16, "element next");
_Static_assert(sizeof(struct curl_llist) == 32, "curl_llist");
_Static_assert(offsetof(struct curl_llist, tail) == 8, "list tail");
_Static_assert(offsetof(struct curl_llist, dtor) == 16, "list dtor");
_Static_assert(offsetof(struct curl_llist, size) == 24, "list size");
_Static_assert(sizeof(struct curl_hash) == 48, "curl_hash");
_Static_assert(offsetof(struct curl_hash, hash_func) == 8, "hash function");
_Static_assert(offsetof(struct curl_hash, comp_func) == 16, "hash comparator");
_Static_assert(offsetof(struct curl_hash, dtor) == 24, "hash dtor");
_Static_assert(offsetof(struct curl_hash, slots) == 32, "hash slots");
_Static_assert(offsetof(struct curl_hash, size) == 40, "hash size");
_Static_assert(sizeof(struct curl_hash_element) == 24, "curl_hash_element");
_Static_assert(offsetof(struct curl_hash_element, key) == 8, "hash element key");
_Static_assert(offsetof(struct curl_hash_element, key_len) == 16,
               "hash element key length");
_Static_assert(sizeof(struct curl_hash_iterator) == 24, "curl_hash_iterator");
_Static_assert(offsetof(struct curl_hash_iterator, slot_index) == 8,
               "iterator slot index");
_Static_assert(offsetof(struct curl_hash_iterator, current_element) == 16,
               "iterator current element");

extern struct curl_llist *Curl_llist_alloc(curl_llist_dtor dtor);
extern int Curl_llist_insert_next(struct curl_llist *list,
                                  struct curl_llist_element *e,
                                  const void *p);
extern int Curl_llist_remove(struct curl_llist *list,
                             struct curl_llist_element *e, void *user);
extern void Curl_llist_destroy(struct curl_llist *list, void *user);

/* Original curl_nintendo_allocator.c declarations and allocation mappings. */
extern void *Curl_SigloMalloc(size_t sizeToAllocate);
extern void Curl_SigloFree(void *pointer);

static void
hash_element_dtor(void *user, void *element)
{
  struct curl_hash *h = (struct curl_hash *) user;
  struct curl_hash_element *e = (struct curl_hash_element *) element;

  Curl_SigloFree(e->key);
  e->key = NULL;

  if(e->ptr) {
    h->dtor(e->ptr);
    e->ptr = NULL;
  }

  e->key_len = 0;

  Curl_SigloFree(e);
}

/* return 1 on error, 0 is fine */
int
Curl_hash_init(struct curl_hash *h,
               int slots,
               hash_function hfunc,
               comp_function comparator,
               curl_hash_dtor dtor)
{
  int i;

  if(!slots || !hfunc || !comparator ||!dtor) {
    return 1; /* failure */
  }

  h->hash_func = hfunc;
  h->comp_func = comparator;
  h->dtor = dtor;
  h->size = 0;
  h->slots = slots;

  h->table = Curl_SigloMalloc(slots * sizeof(struct curl_llist *));
  if(h->table) {
    for(i = 0; i < slots; ++i) {
      h->table[i] = Curl_llist_alloc((curl_llist_dtor) hash_element_dtor);
      if(!h->table[i]) {
        while(i--) {
          Curl_llist_destroy(h->table[i], NULL);
          h->table[i] = NULL;
        }
        Curl_SigloFree(h->table);
        h->table = NULL;
        h->slots = 0;
        return 1; /* failure */
      }
    }
    return 0; /* fine */
  }
  else {
    h->slots = 0;
    return 1; /* failure */
  }
}

struct curl_hash *
Curl_hash_alloc(int slots,
                hash_function hfunc,
                comp_function comparator,
                curl_hash_dtor dtor)
{
  struct curl_hash *h;

  if(!slots || !hfunc || !comparator ||!dtor) {
    return NULL; /* failure */
  }

  h = Curl_SigloMalloc(sizeof(struct curl_hash));
  if(h) {
    if(Curl_hash_init(h, slots, hfunc, comparator, dtor)) {
      /* failure */
      Curl_SigloFree(h);
      h = NULL;
    }
  }

  return h;
}



static struct curl_hash_element *
mk_hash_element(const void *key, size_t key_len, const void *p)
{
  struct curl_hash_element *he = Curl_SigloMalloc(sizeof(struct curl_hash_element));

  if(he) {
    void *dupkey = Curl_SigloMalloc(key_len);
    if(dupkey) {
      /* copy the key */
      memcpy(dupkey, key, key_len);

      he->key = dupkey;
      he->key_len = key_len;
      he->ptr = (void *) p;
    }
    else {
      /* failed to duplicate the key, free memory and fail */
      Curl_SigloFree(he);
      he = NULL;
    }
  }
  return he;
}

#define FETCH_LIST(x,y,z) x->table[x->hash_func(y, z, x->slots)]

/* Insert the data in the hash. If there already was a match in the hash,
 * that data is replaced.
 *
 * @unittest: 1305
 */
void *
Curl_hash_add(struct curl_hash *h, void *key, size_t key_len, void *p)
{
  struct curl_hash_element  *he;
  struct curl_llist_element *le;
  struct curl_llist *l = FETCH_LIST (h, key, key_len);

  for(le = l->head; le; le = le->next) {
    he = (struct curl_hash_element *) le->ptr;
    if(h->comp_func(he->key, he->key_len, key, key_len)) {
      Curl_llist_remove(l, le, (void *)h);
      --h->size;
      break;
    }
  }

  he = mk_hash_element(key, key_len, p);
  if(he) {
    if(Curl_llist_insert_next(l, l->tail, he)) {
      ++h->size;
      return p; /* return the new entry */
    }
    /*
     * Couldn't insert it, destroy the 'he' element and the key again. We
     * don't call hash_element_dtor() since that would also call the
     * "destructor" for the actual data 'p'. When we fail, we shall not touch
     * that data.
     */
    Curl_SigloFree(he->key);
    Curl_SigloFree(he);
  }

  return NULL; /* failure */
}

/* remove the identified hash entry, returns non-zero on failure */
int Curl_hash_delete(struct curl_hash *h, void *key, size_t key_len)
{
  struct curl_llist_element *le;
  struct curl_hash_element  *he;
  struct curl_llist *l = FETCH_LIST(h, key, key_len);

  for(le = l->head; le; le = le->next) {
    he = le->ptr;
    if(h->comp_func(he->key, he->key_len, key, key_len)) {
      Curl_llist_remove(l, le, (void *) h);
      --h->size;
      return 0;
    }
  }
  return 1;
}

void *
Curl_hash_pick(struct curl_hash *h, void *key, size_t key_len)
{
  struct curl_llist_element *le;
  struct curl_hash_element  *he;
  struct curl_llist *l;

  if(h) {
    l = FETCH_LIST(h, key, key_len);
    for(le = l->head; le; le = le->next) {
      he = le->ptr;
      if(h->comp_func(he->key, he->key_len, key, key_len)) {
        return he->ptr;
      }
    }
  }

  return NULL;
}



void
Curl_hash_clean(struct curl_hash *h)
{
  int i;

  for(i = 0; i < h->slots; ++i) {
    Curl_llist_destroy(h->table[i], (void *) h);
    h->table[i] = NULL;
  }

  Curl_SigloFree(h->table);
  h->table = NULL;
  h->size = 0;
  h->slots = 0;
}

void
Curl_hash_clean_with_criterium(struct curl_hash *h, void *user,
                               int (*comp)(void *, void *))
{
  struct curl_llist_element *le;
  struct curl_llist_element *lnext;
  struct curl_llist *list;
  int i;

  if(!h)
    return;

  for(i = 0; i < h->slots; ++i) {
    list = h->table[i];
    le = list->head; /* get first list entry */
    while(le) {
      struct curl_hash_element *he = le->ptr;
      lnext = le->next;
      /* ask the callback function if we shall remove this entry or not */
      if(comp(user, he->ptr)) {
        Curl_llist_remove(list, le, (void *) h);
        --h->size; /* one less entry in the hash now */
      }
      le = lnext;
    }
  }
}

void
Curl_hash_destroy(struct curl_hash *h)
{
  if(!h)
    return;

  Curl_hash_clean(h);

  Curl_SigloFree(h);
}

size_t Curl_hash_str(void* key, size_t key_length, size_t slots_num)
{
  const char* key_str = (const char *) key;
  const char *end = key_str + key_length;
  unsigned long h = 5381;

  while(key_str < end) {
    h += h << 5;
    /* Original char is unsigned; the maintained recipe uses signed char. */
    h ^= (unsigned long) (unsigned char) *key_str++;
  }

  return (h % slots_num);
}

size_t Curl_str_key_compare(void*k1, size_t key1_len, void*k2, size_t key2_len)
{
  char *key1 = (char *)k1;
  char *key2 = (char *)k2;

  if(key1_len == key2_len &&
      *key1 == *key2 &&
      memcmp(key1, key2, key1_len) == 0) {
    return 1;
  }

  return 0;
}

void Curl_hash_start_iterate(struct curl_hash *hash,
                             struct curl_hash_iterator *iter)
{
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current_element = NULL;
}

struct curl_hash_element *
Curl_hash_next_element(struct curl_hash_iterator *iter)
{
  int i;
  struct curl_hash *h = iter->hash;

  /* Get the next element in the current list, if any */
  if(iter->current_element)
    iter->current_element = iter->current_element->next;

  /* If we have reached the end of the list, find the next one */
  if(!iter->current_element) {
    for(i = iter->slot_index;i < h->slots;i++) {
      if(h->table[i]->head) {
        iter->current_element = h->table[i]->head;
        iter->slot_index = i+1;
        break;
      }
    }
  }

  if(iter->current_element) {
    struct curl_hash_element *he = iter->current_element->ptr;
    return he;
  }
  else {
    iter->current_element = NULL;
    return NULL;
  }
}
