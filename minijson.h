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

typedef struct JsonValue JsonValue;

typedef struct {
    int len;
    int cap;
    JsonValue* list;
} JsonArray;

typedef struct {
    int len;
    int cap;
    JsonStr* keyList;
    JsonValue* valueList;
} JsonMap;

struct JsonValue {
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
// TODO: use minijsong instead
int minijson_parse_str(JsonMap* res, const char* src, JsonStr* err);

// TODO: implement
void minijson_to_str();

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
int init_jmap(JsonMap* dst);
int free_jmap(JsonMap* dst);
int jmap_from_cstr(JsonMap* dst, const char* src, JsonStr* err);
int jmap_to_cstr(const JsonMap* src, char* dst);
int jmap_set(JsonMap* map, const char* key, JsonValue val);
int jmap_set_str(JsonMap* map, const char* key, const char* val);
int jmap_set_int(JsonMap* map, const char* key, size_t val);
int jmap_set_float(JsonMap* map, const char* key, double val);
int jmap_set_bool(JsonMap* map, const char* key, bool val);
JsonValue* jmap_get(JsonMap* map, const char* key);
int* jmap_delete(JsonMap* map, const char* key);
void jmap_output(JsonStr* dist, const JsonMap* src, int indent);

// JsonValue methods
void jvalue_output(JsonStr* dist, const JsonValue* v, int indent);
