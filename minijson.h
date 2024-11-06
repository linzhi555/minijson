/*
    MIT License

    Copyright (c) 2024 linzhi555

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum {
    JNULL,
    JBOOL,
    JNUM,
    JSTR,
    JARRAY,
    JMAP,
} JsonType;

typedef struct {
    bool data;
} JsonBool;

typedef struct {
    bool isInt;
    int64_t Int64;
    double Double;
} JsonNum;

typedef struct {
    int len;
    int cap;
    char* data;
} JsonStr;

typedef struct JsonValue JsonValue;

typedef struct {
    int len;
    int cap;
    JsonValue* list;
} JsonArray;

typedef struct MapKV {
    struct MapKV* next;
    struct MapKV* pre;
    char* key;
    JsonValue* value;
} KVNode;

typedef struct Index {
    KVNode* ptr;
    struct Index* next;
} Index;

// jsonMap all kvs stored in kvList,and has two extra headnode and tailnode,
//
// (when list empty)
// head <-> tailhead<-node
//
// (list has two actual node)
// 0<->node1<->tail
//
// we use indexes and hash function to access the list node ,the hash maybe collide,so the index is a list
//      [    0    1      2     3     ]
//         null  node1* null  node2*
//                |
//                v
//               node3*
typedef struct {
    int indexCap;
    Index* indexes;
    int kvLen;
    KVNode* head;
    KVNode* tail;
} JsonMap;

struct JsonValue {
    JsonType type;
    union {
        JsonBool jsonBool;
        JsonNum jsonNum;
        JsonStr jsonStr;
        JsonArray jsonArray;
        JsonMap jsonMap;
    };
};

// basic api
const char* minijson_version();
int minijson_parse_map(JsonMap* res, const char* src, JsonStr* err);
int minijson_parse_array(JsonArray* res, const char* src, JsonStr* err);
int minijson_parse_any(JsonValue* res, const char* src, JsonStr* err);

// JsonNull methods
bool is_null(const JsonValue* obj);

// JsonNum methods
int jnum_from_cstr(JsonNum* num, const char* cs, size_t n);

// JsonStr methods
void init_jstr(JsonStr* str);
void free_jstr(JsonStr* dst);
int jstr_sprintf(JsonStr* dst, const char* format, ...);
int jstr_sprintf_back(JsonStr* dst, const char* format, ...);
int jstr_cpy(JsonStr* dst, const JsonStr* src);
int jstr_cpy_cstr(JsonStr* str, const char* cs, int len);
const char* jstr_cstr(const JsonStr* str);
int jstr_from_cstr(JsonStr* dst, const char* src);

// JsonArray function
void init_jarray(JsonArray* dst);
void free_jarry(JsonArray* dst);
int jarray_from_cstr(JsonArray* dst, const char src, JsonStr* err);
int jarray_append(JsonArray* array, JsonValue newobj);
int jarray_insert(JsonArray* array, JsonValue newobj);
int jarray_delete(JsonArray* array, int index);
JsonValue* jarray_get(JsonArray* map, int index);
void jarray_output(JsonStr* dist, const JsonArray* array, int indent);

// JsonMap methods
void init_jmap(JsonMap* dst);
void free_jmap(JsonMap* dst);

int jmap_set(JsonMap* map, const char* key, JsonValue val);
int jmap_set_str(JsonMap* map, const char* key, const char* val);
int jmap_set_int(JsonMap* map, const char* key, int64_t val);
int jmap_set_float(JsonMap* map, const char* key, double val);
int jmap_set_bool(JsonMap* map, const char* key, bool val);

JsonValue* jmap_get_ref(const JsonMap* map, const char* key);
int jmap_get_str(const JsonMap* map, const char* key, char** val);
int jmap_get_int(const JsonMap* map, const char* key, int64_t* val);
int jmap_get_float(const JsonMap* map, const char* key, double* val);
int jmap_get_bool(const JsonMap* map, const char* key, bool* val);

int* jmap_delete(JsonMap* map, const char* key);
void jmap_output(JsonStr* dist, const JsonMap* src, int indent);

// JsonValue methods
void init_jvalue_null(JsonValue* value);
void init_jvalue_float(JsonValue* v, double f);
void init_jvalue_int(JsonValue* v, int64_t i);
void init_jvalue_bool(JsonValue* v, bool b);
void free_jvalue(JsonValue* value);
void jvalue_output(JsonStr* dist, const JsonValue* v, int indent);
bool Jvalue_isbasic(const JsonValue* v);
