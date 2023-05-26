// Copyright 2023 Joshua J Baker. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
//
// https://github.com/tidwall/tstr

#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include "tstr.h"

struct istr {
    atomic_int rc;
    size_t len;
    char data[];
};

static void *(*_malloc)(size_t);
static void (*_free)(void*);

static void *tmalloc(size_t nbytes) {
    return (_malloc ? _malloc : malloc)(nbytes);
}

static void tfree(void *ptr) {
    (_free ? _free : free)(ptr);
}

void tstr_set_allocator(void *(*malloc)(size_t), void (*free)(void*)) {
    _malloc = malloc;
    _free = free;
}

static struct istr *toistr(const tstr *str) {
    size_t dataoff = offsetof(struct istr, data);
    return (struct istr *)(((char*)str)-dataoff);
}

tstr *tstr_from_format(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int nbytes = vsnprintf(NULL, 0, format, args);
    va_end(args);
    assert(nbytes >= 0);
    struct istr *istr = tmalloc(sizeof(struct istr)+nbytes+1);
    if (!istr) return NULL;
    istr->rc = 0;
    istr->len = nbytes;
    va_start(args, format);
    nbytes = vsnprintf(istr->data, nbytes+1, format, args);
    va_end(args);
    assert(nbytes >= 0);
    return (tstr*)(&istr->data[0]);
}

tstr *tstr_from_bytes(const void *bytes, size_t nbytes) {
    if (!nbytes) return NULL;
    struct istr *istr = tmalloc(sizeof(struct istr)+nbytes+1);
    if (!istr) return NULL;
    istr->rc = 0;
    istr->len = nbytes;
    memcpy(&istr->data, bytes, nbytes);
    istr->data[nbytes] = '\0';
    return (tstr*)(&istr->data[0]);
}

tstr *tstr_from_cstr(const char *cstr) {
    if (!cstr) return NULL;
    return tstr_from_bytes(cstr, strlen(cstr));
}

tstr *tstr_clone(const tstr *str) {
    if (!str) return NULL;
    struct istr *istr = toistr(str);
    atomic_fetch_add(&istr->rc, 1);
    return (tstr*)str;
}

void tstr_free(tstr *str) {
    if (!str) return;
    struct istr *istr = toistr(str);
    if (atomic_fetch_sub(&istr->rc, 1) > 0) return;
    tfree(istr);
}

size_t tstr_len(const tstr *str) {
    if (!str) return 0;
    struct istr *istr = toistr(str);
    return istr->len;
}

const char *tstr_cstr(const tstr *str) {
    if (!str) return NULL;
    struct istr *istr = toistr(str);
    return istr->data;
}

const void *tstr_bytes(const tstr *str) {
    return tstr_cstr(str);
}

// memcmpz compares A to B.
// Works much like strcmp but for two memory segments that are not
// null-terminated, and of varying sizes.
static int memcmpz(const void *a, size_t asize, const void *b, size_t bsize) {
    size_t size = bsize;
    int scmp = 0;
    if (asize < bsize) {
        size = asize;
        scmp = -1;
    } else if (asize > bsize) {
        scmp = 1;
    }
    int cmp = memcmp(a, b, size);
    if (cmp == 0) {
        cmp = scmp;
    }
    return cmp;
}

int tstr_compare(const tstr *a, const tstr *b) {
    return memcmpz(tstr_bytes(a), tstr_len(a), tstr_bytes(b), tstr_len(b));
}

bool tstr_equal(const tstr *a, const tstr *b) {
    size_t alen = tstr_len(a);
    size_t blen = tstr_len(b);
    if (alen != blen) return false;
    if (alen == 0) return true;
    return memcmp(a, b, alen) == 0;
}
