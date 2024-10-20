#pragma once
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define ERR_MAX_LEN 100
typedef enum {
    JNULL,
    JBOOL,
    JNUM,
    JSTR,
    JARRAY,
    JMAP,
} JsonObjType;

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

typedef struct JsonBaseObj JsonBaseObj;

typedef struct {
    int len;
    int cap;
    JsonBaseObj* list;
} JsonArray;

typedef struct {
    int len;
    int cap;
    JsonStr* keyList;
    JsonBaseObj* valueList;
} JsonMap;

struct JsonBaseObj {
    JsonObjType type;
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
int minijson_parse_str(JsonMap* res, const char* src, char* err);
void minijson_to_str();

// JsonNull methods
bool is_null(const JsonBaseObj* obj);

// JsonNum methods
int jnum_from_cstr(JsonNum* num, const char* cs, size_t n);

// JsonStr methods
void init_jstr(JsonStr* str);
void free_jstr(JsonStr* dst);
int jstr_cpy(JsonStr* dst, const JsonStr* src);
int jstr_cpy_cstr(JsonStr* str, const char* cs, int len);
const char* jstr_cstr(const JsonStr* str);
int jstr_from_cstr(JsonStr* dst, const char* src);

// JsonArray function
void init_jarray(JsonArray* dst);
void free_jarry(JsonArray* dst);
void jarray_debug(const JsonArray* dst);
int jarray_from_cstr(JsonArray* dst, const char src, char* err);
int jarray_append(JsonArray* array, JsonBaseObj newobj);
int jarray_insert(JsonArray* array, JsonBaseObj newobj);
int jarray_delete(JsonArray* array, int index);
JsonBaseObj* jarray_get(JsonArray* map, int index);

// JsonMap methods
int init_jmap(JsonMap* dst);
int free_jmap(JsonMap* dst);
int jmap_from_cstr(JsonMap* dst, const char* src, char* err);
int jmap_to_cstr(const JsonMap* src, char* dst);
void jmap_debug(const JsonMap* src);
int jmap_set(JsonMap* map, const char* key, JsonBaseObj val);
int jmap_set_str(JsonMap* map, const char* key, const char* val);
int jmap_set_int(JsonMap* map, const char* key, size_t val);
int jmap_set_float(JsonMap* map, const char* key, double val);
int jmap_set_bool(JsonMap* map, const char* key, bool val);
JsonBaseObj* jmap_get(JsonMap* map, const char* key);
int* jmap_delete(JsonMap* map, const char* key);

// JsonBaseObj methods
void jbaseobj_debug(const JsonBaseObj* v);
