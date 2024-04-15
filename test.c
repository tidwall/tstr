#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "tstr.h"

tstr *tstr_alloc(size_t nbytes);
tstr *tstr_from_zeros(size_t nbytes);
int tstr_compare_cstr(tstr *str, const char *cstr);

int main(void) {
    tstr *str1 = tstr_from_bytes("HELLO", 3);
    assert(tstr_len(str1) == 3);
    assert(strcmp(str1, "HEL") == 0);

    tstr *str2 = tstr_clone(str1);
    tstr *str3 = tstr_from_format("%s %d", "HI", 76);
    tstr *str4 = tstr_from_format("%s %d", "HI", 77);
    tstr *str5 = tstr_from_format("%s %d", "HI", 78);
    tstr *str6 = tstr_from_zeros(10);
    
    assert(tstr_equal(str1, str2)); 
    assert(!tstr_equal(str1, str3)); 
    assert(!tstr_equal(str4, str3)); 

    assert(tstr_cmp(str4, str3) == 1);
    assert(tstr_cmp(str4, str4) == 0);
    assert(tstr_cmp(str4, str5) == -1);

    assert(tstr_cmp_cstr(str4, "HI 76") == 1);
    assert(tstr_cmp_cstr(str4, "HI 77") == 0);
    assert(tstr_cmp_cstr(str4, "HI 78") == -1);

    assert(tstr_casecmp_cstr(str4, "Hi 76") == 1);
    assert(tstr_casecmp_cstr(str4, "Hi 77") == 0);
    assert(tstr_casecmp_cstr(str4, "Hi 78") == -1);

    assert(tstr_len(str6) == 10);

    for (size_t i = 0; i < tstr_len(str6); i++) {
        assert(str6[i] == 0);
    }
    tstr_free(str6);
    tstr_free(str5);
    tstr_free(str4);
    tstr_free(str3);
    tstr_free(str2);
    assert(tstr_len(str1) == 3);
    assert(strcmp(str1, "HEL") == 0);
    tstr_free(str1);

    printf("PASSED\n");
    return 0;
}