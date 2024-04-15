// Copyright 2023 Joshua J Baker. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
//
// https://github.com/tidwall/tstr

#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include "tstr.h"

struct tstr_internal {
    atomic_int rc;
    size_t len;
    char data[];
};

static void *(*_tstr_malloc)(size_t);
static void (*_tstr_free)(void*);

void tstr_set_allocator(void *(*malloc)(size_t), void (*free)(void*)) {
    _tstr_malloc = malloc;
    _tstr_free = free;
}

static struct tstr_internal *tstr_toistr(const tstr *str) {
    size_t dataoff = offsetof(struct tstr_internal, data);
    return (struct tstr_internal *)(((char*)str)-dataoff);
}

/// Return a newly allocated tstr that _is not_ initialized or null-terminated.
/// This is an intentionally unsafe and undocumented function used primarly to
/// to allocate a new tstr before initialization.
tstr *tstr_alloc(size_t nbytes) {
    size_t memsize = sizeof(struct tstr_internal)+nbytes+1;
    struct tstr_internal *istr = (_tstr_malloc?_tstr_malloc:malloc)(memsize);
    if (!istr) return NULL;
    istr->rc = 0;
    istr->len = nbytes;
    return (tstr*)(&istr->data[0]);
}

/// DEPRECATED
/// Use tstr_alloc(size) & memset(str, 0, size) instead.
tstr *tstr_from_zeros(size_t nbytes) {
    tstr *str = tstr_alloc(nbytes);
    if (!str) return NULL;
    memset((char*)str, 0, nbytes+1);
    return str;
}

tstr *tstr_from_format(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int nbytes = vsnprintf(NULL, 0, format, args);
    va_end(args);
    assert(nbytes >= 0);
    tstr *str = tstr_alloc(nbytes);
    if (!str) return NULL;
    va_start(args, format);
    nbytes = vsnprintf((char*)str, nbytes+1, format, args);
    va_end(args);
    assert(nbytes >= 0);
    return str;
}

tstr *tstr_from_bytes(const void *bytes, size_t nbytes) {
    tstr *str = tstr_alloc(nbytes);
    if (!str) return NULL;
    memcpy((char*)str, bytes, nbytes);
    ((char*)str)[nbytes] = '\0';
    return str;
}

tstr *tstr_from_cstr(const char *cstr) {
    if (!cstr) return NULL;
    return tstr_from_bytes(cstr, strlen(cstr));
}

tstr *tstr_clone(tstr *str) {
    if (!str) return NULL;
    struct tstr_internal *istr = tstr_toistr(str);
    atomic_fetch_add(&istr->rc, 1);
    return (tstr*)str;
}

void tstr_free(tstr *str) {
    if (!str) return;
    struct tstr_internal *istr = tstr_toistr(str);
    if (atomic_fetch_sub(&istr->rc, 1) > 0) return;
    (_tstr_free?_tstr_free:free)(istr);
}

size_t tstr_len(tstr *str) {
    if (!str) return 0;
    struct tstr_internal *istr = tstr_toistr(str);
    return istr->len;
}

const char *tstr_cstr(tstr *str) {
    if (!str) return NULL;
    struct tstr_internal *istr = tstr_toistr(str);
    return istr->data;
}

const void *tstr_bytes(tstr *str) {
    return tstr_cstr(str);
}

bool tstr_equal(tstr *a, tstr *b) {
    size_t alen = tstr_len(a);
    size_t blen = tstr_len(b);
    if (alen != blen) return false;
    if (alen == 0) return true;
    return memcmp(a, b, alen) == 0;
}

// memcmpz compares A to B.
// Works much like strcmp but for two memory segments that are not
// null-terminated, and of varying sizes.
static int memcmpz(const void *a, size_t asize, const void *b, size_t bsize, 
    bool insenstive) 
{
    size_t size = asize < bsize ? asize : bsize;
    int cmp;
    if (insenstive) {
        cmp = 0;
        for (size_t i = 0; i < size && cmp == 0; i++) {
            int ca = tolower(((unsigned char*)a)[i]);
            int cb = tolower(((unsigned char*)b)[i]);
            cmp = ca < cb ? -1 : ca > cb;
        }
    } else {
        cmp = memcmp(a, b, size);
    }
    return cmp == 0 ? asize < bsize ? -1 : asize > bsize : cmp;
}

int tstr_cmp(tstr *a, tstr *b) {
    return memcmpz(tstr_bytes(a), tstr_len(a), tstr_bytes(b), tstr_len(b), 0);
}

int tstr_casecmp(tstr *a, tstr *b) {
    return memcmpz(tstr_bytes(a), tstr_len(a), tstr_bytes(b), tstr_len(b), 1);
}

int tstr_cmp_cstr(tstr *str, const char *cstr) {
    return memcmpz(tstr_bytes(str), tstr_len(str), cstr, strlen(cstr), 0);
}

int tstr_casecmp_cstr(tstr *str, const char *cstr) {
    return memcmpz(tstr_bytes(str), tstr_len(str), cstr, strlen(cstr), 1);
}

static int tstr_ncmp0(tstr *a, tstr *b, size_t n, bool ci) {
    size_t na = tstr_len(a);
    size_t nb = tstr_len(b);
    na = na < n ? na : n;
    nb = nb < n ? nb : n;
    return memcmpz(tstr_bytes(a), na, tstr_bytes(b), nb, ci);
}

int tstr_ncmp(tstr *a, tstr *b, size_t n) {
    return tstr_ncmp0(a, b, n, 0);
}

int tstr_ncasecmp(tstr *a, tstr *b, size_t n) {
    return tstr_ncmp0(a, b, n, 1);
}

// DEPRECATED
int tstr_compare(tstr *a, tstr *b) {
    return tstr_cmp(a, b);
}

