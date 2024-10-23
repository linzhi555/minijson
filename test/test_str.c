#include <stdio.h>
#include "minijson.h"
int main() {
    JsonStr str;
    init_jstr(&str);
    jstr_sprintf(&str, "this is a test %d ,contents: %3s", 11, "abcd\nefg");
    printf("%s\n",jstr_cstr(&str));
    free_jstr(&str);
}
