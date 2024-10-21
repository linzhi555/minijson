#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "minijson.h"
#include "miniutils.h"
void init_jarray(JsonArray* dst) {
    dst->cap = 0;
    dst->len = 0;
    dst->list = NULL;
}

void free_jarry(JsonArray* dst) {
    if (dst->cap != 0) {
        free(dst->list);
    }
    dst->len = 0;
    dst->cap = 0;
}

int ensure_cap(JsonArray* array, int newcap) {
    if (array->cap >= newcap) return array->cap;
    array->cap = newcap * 2;
    array->list = realloc(array->list, array->cap * sizeof(JsonBaseObj));
    return array->cap;
}

int jarray_append(JsonArray* array, JsonBaseObj newobj) {
    ensure_cap(array, array->len + 1);
    array->list[array->len] = newobj;
    array->len++;
    return array->len;
}

JsonBaseObj* jarray_get(JsonArray* array, int index) {
    if (array->len - 1 < index) return NULL;
    return &array->list[index];
}

int jarray_delete(JsonArray* array, int index) {
    if (array->len - 1 < index) return -1;
    array->list[index] = array->list[array->len - 1];
    array->len--;
    return 0;
}

void jarray_debug(const JsonArray* array, int indent) {
    printf("%s[\n", nspace(2 * indent));
    for (int i = 0; i < array->len; i++) {
        printf("%s", nspace(2 * (indent + 1)));
        jbaseobj_debug(&array->list[i], indent);
        printf("\n");
    }
    printf("%s]\n", nspace(2 * indent));
}

// int jarray_insert(JsonArray* array, JsonBaseObj newobj) {
//     assert("not impleted!" == NULL);
// }
