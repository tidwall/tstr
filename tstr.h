// Copyright 2023 Joshua J Baker. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
//
// https://github.com/tidwall/tstr

#ifndef TSTR_H
#define TSTR_H

#include <stdlib.h>
#include <stdbool.h>

// tstr is string that tracks it's length, is null-terminated, is compatible
// with C strings, and can optionally store binary.
typedef const char tstr;

tstr *tstr_from_bytes(const void *bytes, size_t nbytes);
tstr *tstr_from_cstr(const char *cstr);
tstr *tstr_from_format(const char *format, ...);
tstr *tstr_clone(const tstr *);
void tstr_free(tstr *);
size_t tstr_len(const tstr *);
const char *tstr_cstr(tstr *);
const void *tstr_bytes(tstr *);
bool tstr_equal(tstr *a, tstr *b); 
int tstr_cmp(tstr *a, tstr *b);
int tstr_casecmp(tstr *a, tstr *b);
int tstr_ncmp(tstr *a, tstr *b, size_t n);
int tstr_ncasecmp(tstr *a, tstr *b, size_t n);
int tstr_cmp_cstr(tstr *str, const char *cstr);
int tstr_casecmp_cstr(tstr *str, const char *cstr);
void tstr_set_allocator(void *(*malloc)(size_t), void (*free)(void*));

// DEPRECATED
tstr *tstr_from_zeros(size_t nbytes);
int tstr_compare(tstr *a, tstr *b);

#endif // TSTR_H
