#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minijson.h"
const int INIT_JSTR_LEN = 200;

static int jstr_ensure_cap(JsonStr *str, int newcap) {
    if (str->cap >= newcap) return str->cap;

    str->cap = newcap * 2;
    str->data = realloc(str->data, str->cap * sizeof(char));
    return str->cap;
}

void init_jstr(JsonStr *str) {
    assert(str != NULL);
    str->len = 0;
    str->cap = 0;
    str->data = NULL;
}

void free_jstr(JsonStr *str) {
    free(str->data);
}

int jstr_cpy(JsonStr *dst, const JsonStr *src) {
    assert(dst != NULL);
    assert(src != NULL);

    memcpy(dst, src, sizeof(JsonStr));
    dst->data = malloc(src->cap * sizeof(char));
    memcpy(dst->data, src->data, src->cap);
    return dst->len;
}

int jstr_sprintf(JsonStr *str, const char *format, ...) {
    va_list args, args_copy;
    va_start(args, format);
    va_copy(args_copy, args);

    int len = vsnprintf(NULL, 0, format, args) + 1;
    char *temp = calloc(len + 1, sizeof(char));
    vsnprintf(temp, len + 1, format, args_copy);
    str->cap = len + 1;
    str->len = len;
    free(str->data);
    str->data = temp;

    va_end(args_copy);
    va_end(args);

    return str->len;
}

int jstr_cpy_cstr(JsonStr *str, const char *cs, int len) {
    assert(str != NULL);
    assert(cs != NULL);

    jstr_ensure_cap(str, len + 1);
    for (int i = 0; i < len; i++) {
        str->data[i] = cs[i];
    }

    str->data[len] = '\0';
    str->len = len;

    return len;
}

int jstr_append_cstr(JsonStr *str, const char *cs, int len) {
    assert(str != NULL);
    assert(cs != NULL);

    jstr_ensure_cap(str, str->len + len + 1);
    for (int i = 0; i < len; i++) {
        str->data[len + 1] = cs[i];
    }

    str->data[str->len] = '\0';
    str->len += len;

    return len;
}

const char *jstr_cstr(const JsonStr *str) {
    assert(str != NULL);
    return str->data;
}

int jstr_from_cstr(JsonStr *dst, const char *src) {
    assert(dst != NULL);
    assert(dst != NULL);
    if (src[0] != '\"') return 0;
    for (int i = 1; src[i] != '\0'; i++) {
        if (src[i] == '\\' && src[i + 1] != '\0') {
            i++;
            continue;
        }
        if (src[i] == '"') {
            jstr_cpy_cstr(dst, src + 1, i - 1);
            return i + 1;
        }
    }
    return 0;
}
