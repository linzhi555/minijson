#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minijson.h"
#include "miniutils.h"

int init_jmap(JsonMap *map) {
    map->cap = 0;
    map->len = 0;
    map->keyList = NULL;
    map->valueList = NULL;
    return 0;
}

int free_jmap(JsonMap *map) {
    for (int i = 0; i < map->len; i++) {
        JsonStr *k = &map->keyList[i];
        JsonValue *v = &map->valueList[i];
        free_jstr(k);
        switch (v->type) {
        case JMAP:
            free_jmap(&v->jsonMap);
            break;
        case JSTR:
            free_jstr(&v->jsonStr);
            break;
        case JARRAY:
            free_jarry(&v->jsonArray);
            break;
        default:
            break;
        }
    }
    map->keyList = NULL;
    map->valueList = NULL;

    map->cap = 0;
    map->len = 0;
    return 0;
}

static int jmap_ensure_cap(JsonMap *map, int cap) {
    if (map->cap >= cap) {
        return map->cap;
    }

    map->keyList = realloc(map->keyList, cap * 2 * sizeof(JsonStr));
    map->valueList = realloc(map->valueList, cap * 2 * sizeof(JsonValue));

    assert(map->keyList);
    assert(map->valueList);

    for (int i = map->cap; i < cap; i++) {
        init_jstr(&map->keyList[i]);
    }

    map->cap = cap;
    return map->cap;
}

int jmap_set(JsonMap *map, const char *key, JsonValue val) {
    assert(key != NULL);
    assert(map != NULL);
    JsonValue *target = NULL;
    for (int i = 0; i < map->len; i++) {
        JsonValue *v = &map->valueList[i];
        JsonStr *k = &map->keyList[i];
        if (strcmp(key, jstr_cstr(k)) == 0) {
            target = v;
        }
    }

    if (target == NULL) {
        jmap_ensure_cap(map, map->len + 1);
        jstr_cpy_cstr(map->keyList + map->len, key, strlen(key));
        target = &map->valueList[map->len];
        map->len++;
    }

    *target = val;

    LOG("set %s %d , now map len:%d\n", key, val.type, map->len);
    return 0;
}

int jmap_set_str(JsonMap *map, const char *key, const char *val);
int jmap_set_int(JsonMap *map, const char *key, size_t val);
int jmap_set_float(JsonMap *map, const char *key, double val);
int jmap_set_bool(JsonMap *map, const char *key, bool val);

void jmap_output(JsonStr *dist, const JsonMap *map, int indent) {
    assert(dist != NULL);
    assert(map != NULL);

    jstr_sprintf_back(dist, "{\n");
    for (int i = 0; i < map->len; i++) {
        const JsonValue *v = &map->valueList[i];
        const JsonStr *k = &map->keyList[i];
        jstr_sprintf_back(dist, "%skey: %s ", nspace(2 * (indent + 1)), jstr_cstr(k));
        jstr_sprintf_back(dist, "%svalue: ", nspace(2 * (indent + 1)));
        jvalue_output(dist, v, indent);
        jstr_sprintf_back(dist, "\n");
    }
    jstr_sprintf_back(dist, "%s}\n", nspace(2 * indent));
}

void jvalue_output(JsonStr *dist, const JsonValue *v, int indent) {
    switch (v->type) {
    case JSTR:
        jstr_sprintf_back(dist, "%s", jstr_cstr(&v->jsonStr));
        break;
    case JNULL:
        jstr_sprintf_back(dist, "null");
        break;
    case JNUM:
        if (v->jsonNum.isInt) {
            jstr_sprintf_back(dist, "%ld i", v->jsonNum.Int64);
        } else {
            jstr_sprintf_back(dist, "%lf f", v->jsonNum.Double);
        }

        break;
    case JBOOL:
        if (v->jsonBool.data) {
            jstr_sprintf_back(dist, "true");
        } else {
            jstr_sprintf_back(dist, "false");
        }
        break;
    case JARRAY:
        jarray_output(dist, &v->jsonArray, indent + 1);
        break;
    case JMAP:
        jmap_output(dist, &v->jsonMap, indent + 1);
        break;
    }
}
