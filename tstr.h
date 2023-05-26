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
typedef char tstr;

tstr *tstr_from_bytes(const void *bytes, size_t nbytes);
tstr *tstr_from_cstr(const char *cstr);
tstr *tstr_from_format(const char *format, ...); 
tstr *tstr_clone(const tstr *);
void tstr_free(tstr *);
size_t tstr_len(const tstr *);
const char *tstr_cstr(const tstr *);
const void *tstr_bytes(const tstr *);
int tstr_compare(const tstr *a, const tstr *b); 
bool tstr_equal(const tstr *a, const tstr *b); 

void tstr_set_allocator(void *(*malloc)(size_t), void (*free)(void*));

#endif // TSTR_H
