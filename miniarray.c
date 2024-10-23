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

static int jarray_ensure_cap(JsonArray* array, int newcap) {
    if (array->cap >= newcap) return array->cap;
    array->cap = newcap * 2;
    array->list = realloc(array->list, array->cap * sizeof(JsonValue));
    return array->cap;
}

int jarray_append(JsonArray* array, JsonValue newobj) {
    jarray_ensure_cap(array, array->len + 1);
    array->list[array->len] = newobj;
    array->len++;
    return array->len;
}

JsonValue* jarray_get(JsonArray* array, int index) {
    if (array->len - 1 < index) return NULL;
    return &array->list[index];
}

int jarray_delete(JsonArray* array, int index) {
    if (array->len - 1 < index) return -1;
    array->list[index] = array->list[array->len - 1];
    array->len--;
    return 0;
}

void jarray_output(JsonStr* dist, const JsonArray* array, int indent) {
    jstr_sprintf_back(dist, "%s[\n", nspace(2 * indent));
    for (int i = 0; i < array->len; i++) {
        jstr_sprintf_back(dist, "%s", nspace(2 * (indent + 1)));
        jvalue_output(dist, &array->list[i], indent);
        jstr_sprintf_back(dist, "\n");
    }
    jstr_sprintf_back(dist, "%s]\n", nspace(2 * indent));
}

// int jarray_insert(JsonArray* array, JsonValue newobj) {
//     assert("not impleted!" == NULL);
// }
