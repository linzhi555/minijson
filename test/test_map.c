#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "minijson.h"

#define KVNUM 801

int main() {
    JsonMap map;
    init_jmap(&map);

    // add kv
    for (int i = 0; i < KVNUM; i++) {
        char k[KVNUM] = { 0 };
        char v[KVNUM] = { 0 };
        snprintf(k, KVNUM, "student%d", i);
        snprintf(v, KVNUM, "id%d", i);
        jmap_set_str(&map, k, v);
    }

    JsonStr out;
    init_jstr(&out);

    jmap_output(&out, &map, 0);
    printf("%.100s\n", jstr_cstr(&out));

    // check kvs
    for (int i = KVNUM - 1; i >= 0; i--) {
        char k[KVNUM] = { 0 };
        char v[KVNUM] = { 0 };
        snprintf(k, KVNUM, "student%d", i);
        snprintf(v, KVNUM, "id%d", i);

        JsonValue jv = jmap_get(&map, k);
        assert(jv.type == JSTR);
        assert(strcmp(jv.jsonStr.data, v) == 0);
    }
    assert(map.kvLen == KVNUM);
    printf("%d\n", map.indexCap);

    printf("test map SUCCESS\n");
    printf("=========================\n");

    free_jstr(&out);
    free_jmap(&map);
}
