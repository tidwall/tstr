# tstr

A safe immutable string format for C.

- Supports binary as well as null-terminated cstr.
- Backwards compatible with the "char *" C-string.
- Provides a clone method for sharing the same data without new allocations.

## Example

```C
#include "tstr.h"

// Create a tstr.
tstr *str = tstr_from_cstr("fantastic words");
if (!str) ... // check for out of memory

// Make a clone of str. 
// The result shares the same memory as the original.
tstr *str2 = tstr_clone(str);

// Print the string "fantastic words".
printf("%s\n", str2);

// Free both the clone and original
tstr_free(str);
tstr_free(str2);
```

## API

```C
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
```

## Structure

The internal structure is:

```C
struct {
    atomic_int rc;  // reference counter for cloning
    size_t len;     // length of cstr. Does not include the null character.
    char cstr[];    // raw string binary. Always null-terminated.
};
```

A complete tstr allocation will always be a little larger than the
original data in order to include the reference counter, length, and the
null-terminator character.

The actual tstr pointer (`tstr *`) starts at the first byte of the `cstr`
field, which ensures that the tstr works like a C-string and can be used
by all `string.h` functions as well as `tstr.h` functions.
