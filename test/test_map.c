#include <assert.h>
#include <stdio.h>
#include "minijson.h"

int main() {
    JsonMap map;
    init_jmap(&map);
    jmap_set_str(&map, "name", "lll");

    JsonStr out;
    init_jstr(&out);

    jmap_output(&out, &map, 0);
    printf("%s\n", jstr_cstr(&out));

    free_jstr(&out);
    free_jmap(&map);
}
