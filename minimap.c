#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minijson.h"
#include "miniutils.h"

#define INITIAL_HASH_SPACE 10
#define LOAD_RATIO 0.8   // when (kvLen / indexCap) > LOAD_RATIO, the map will rebuild the indexes
#define EXPAND_RATIO 10  // the new indexCap will have EXPAND_RATIO times than old

// hash the char* to (0 - N-1)
#define HASH_CHARS(chars, N) (fnv1a_32(chars, strlen(chars)) % N)

static uint32_t fnv1a_32(const char *data, size_t len) {
    uint32_t hash = 2166136261u;       // FNV-32初始哈希值
    const uint32_t prime = 16777619u;  // FNV-32质数

    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];  // 异或当前字节
        hash *= prime;    // 乘以质数
    }

    return hash;  // 返回最终哈希值
}

static void set_index(JsonMap *map, KVNode *node);

static void init_index(JsonMap *map, int cap) {
    map->indexCap = cap;
    map->indexes = calloc(sizeof(Index), map->indexCap);
    for (KVNode *p = map->head->next; p != map->tail; p = p->next) {
        set_index(map, p);
    }
}

static void free_index(JsonMap *map) {
    for (int i = 0; i < map->indexCap; i++) {
        if (map->indexes[i].next == NULL) {
            continue;
        }
        Index *p = map->indexes[i].next;
        while (p != NULL) {
            Index *needFree = p;
            p = p->next;
            free(needFree);
        }
    }
    free(map->indexes);
    map->indexes = NULL;
}

static Index *find_index(JsonMap *map, const char *key) {
    int idx = HASH_CHARS(key, map->indexCap);
    Index *p = &map->indexes[idx];
    if (p->ptr == NULL) return NULL;

    for (; p != NULL; p = p->next) {
        if (strcmp(key, p->ptr->key) == 0) {
            return p;
        }
    }
    return NULL;
}

static void set_index(JsonMap *map, KVNode *node) {
    Index *target = NULL;

    int idx = HASH_CHARS(node->key, map->indexCap);
    Index *p = &map->indexes[idx];
    if (p->ptr == NULL) {
        target = p;
        goto final;
    }
    Index *pre = NULL;
    for (; p != NULL; p = p->next) {
        if (strcmp(node->key, p->ptr->key) == 0) {
            target = p;
            goto final;
        }
        pre = p;
    }
    pre->next = calloc(1, sizeof(Index));
    target = pre->next;
    goto final;

final:
    target->ptr = node;
}

void init_jmap(JsonMap *map) {
    map->kvLen = 0;
    map->head = calloc(sizeof(KVNode), 1);
    map->tail = calloc(sizeof(KVNode), 1);
    map->head->next = map->tail;
    map->tail->pre = map->head;

    init_index(map, INITIAL_HASH_SPACE);
}

void free_jmap(JsonMap *map) {
    if (map == NULL) return;

    // free content of holds by nodes
    for (KVNode *p = map->head->next; p != map->tail; p = p->next) {
        free(p->key);
        JsonValue *v = p->value;
        free_jvalue(v);  // free jvalue if jvalue is a jstr or jmap or jarray
        free(p->value);  // free p->value itself
    }
    // free all nodes
    for (KVNode *p = map->head->next; p != map->tail;) {
        KVNode *temp = p;
        p = p->next;
        free(temp);
    }
    free(map->head);
    free(map->tail);
    map->head = NULL;
    map->tail = NULL;
    map->kvLen = 0;

    // free map indexes
    free_index(map);
}

int jmap_set(JsonMap *map, const char *key, JsonValue val) {
    assert(key != NULL);
    assert(map != NULL);
    KVNode *target = NULL;

    Index *temp = find_index(map, key);
    if (temp != NULL) {
        target = temp->ptr;
    } else {  // add kvnode there is no index

        // expand the cap of indexes firstly if the space to crowded
        if ((float) map->kvLen >= (float) map->indexCap * LOAD_RATIO) {
            free_index(map);
            init_index(map, map->indexCap * EXPAND_RATIO);
        }

        // create new node with key
        KVNode *newnode = calloc(1, sizeof(KVNode));
        newnode->key = calloc(strlen(key) + 1, sizeof(char));
        newnode->value = calloc(1, sizeof(JsonValue));
        strncpy(newnode->key, key, strlen(key) + 1);

        // add this node to kvlist
        KVNode *pre = map->tail->pre;
        newnode->pre = pre;
        newnode->next = map->tail;
        pre->next = newnode;
        map->tail->pre = newnode;

        // add the index
        set_index(map, newnode);

        target = newnode;
        map->kvLen++;
    }

    *target->value = val;
    LOG("set %s %d , now map len:%d\n", key, val.type, map->kvLen);
    return 0;
}

JsonValue jmap_get(JsonMap *map, const char *key) {
    Index *temp = find_index(map, key);
    if (temp != NULL) {
        return *temp->ptr->value;
    }

    JsonValue ret;
    init_jvalue_null(&ret);
    return ret;
}

int jmap_set_str(JsonMap *map, const char *key, const char *val) {
    JsonValue newval;
    newval.type = JSTR;
    init_jstr(&newval.jsonStr);
    jstr_cpy_cstr(&newval.jsonStr, val, strlen(val));
    return jmap_set(map, key, newval);
}
int jmap_set_int(JsonMap *map, const char *key, size_t val);
int jmap_set_float(JsonMap *map, const char *key, double val);
int jmap_set_bool(JsonMap *map, const char *key, bool val);

void jmap_output(JsonStr *dist, const JsonMap *map, int indent) {
    assert(dist != NULL);
    assert(map != NULL);

    jstr_sprintf_back(dist, "{\n");
    for (KVNode *p = map->head->next; p != map->tail; p = p->next) {
        const JsonValue *v = p->value;
        const char *k = p->key;
        jstr_sprintf_back(dist, "%s\"%s\":", nspace(2 * (indent + 1)), k);

        if (Jvalue_isbasic(v)) {
            jvalue_output(dist, v, 0);
        } else {
            jvalue_output(dist, v, indent + 1);
        }

        if (p != map->tail->pre) jstr_sprintf_back(dist, ",");  // do not add  extra comma after last elem
        jstr_sprintf_back(dist, "\n");
    }
    jstr_sprintf_back(dist, "%s}", nspace(2 * indent));
}
