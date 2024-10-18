#pragma once
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

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
    int length;
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

// JsonNum methods
int jnum_from_cstr(JsonNum* num, const char* cs, size_t n);

// JsonStr methods
void init_jstr(JsonStr* str);
int jstr_cpy(JsonStr* dst, const JsonStr* src);
int jstr_cpy_cstr(JsonStr* str, const char* cs, int len);
const char* jstr_cstr(const JsonStr* str);
int jstr_from_cstr(JsonStr* dst, const char* src);

// JsonArray function
int jarray_from_cstr(JsonMap* dst, const char src);

// JsonMap function
int jmap_from_cstr(JsonMap* dst, const char* src);
int jmap_to_cstr(JsonMap* src, char* dst);
