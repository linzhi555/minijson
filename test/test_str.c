#include <assert.h>
#include <stdio.h>
#include "minijson.h"
int main() {
    JsonStr str;
    init_jstr(&str);

    jstr_sprintf_back(&str, "1234:%d", 11);
    printf("len:%d cap:%d\n%s\n", str.len, str.cap, jstr_cstr(&str));
    assert(str.len == 7);

    jstr_sprintf(&str, "0123456789%.4s", "abcdefg");
    printf("len:%d cap:%d\n%s\n", str.len, str.cap, jstr_cstr(&str));
    assert(str.len == 14);

    jstr_sprintf_back(&str, "1234:%d", 11);
    printf("len:%d cap:%d\n%s\n", str.len, str.cap, jstr_cstr(&str));
    assert(str.len == 21);

    jstr_sprintf_back(&str, "\naaa", 11);
    printf("len:%d cap:%d\n%s\n", str.len, str.cap, jstr_cstr(&str));
    assert(str.len == 25);

    free_jstr(&str);
}
