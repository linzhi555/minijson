#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minijson.h"
#include "minilexer.h"
#include "miniutils.h"

const int INIT_JSTR_LEN = 200;

static int parse_base_obj(JsonBaseObj *obj, Lexer *l, char *err);

// parse the char* to JsonNum
int jnum_from_cstr(JsonNum *num, const char *cs, size_t n) {
    int64_t ipart = 0;  // interger part 11.000203 's ipart is 11
    int64_t fpart = 0;  // float part , 11.000203 's fpart is 203
    int64_t dot0s = 0;  // zero nums after dot, 11.000203 's dot0s is 3

    char state = 's';  // s : start, i : interger , d: dot, f:float
    size_t i;
    for (i = 0; i < n; i++) {
        char c = cs[i];
        if (state == 's') {
            if (isspace(c)) continue;
            if (!isdigit(c)) goto fail;
            ipart = c - '0';
            state = 'i';
            continue;
        }

        else if (state == 'i') {
            if (c == '.') {
                state = 'd';
                continue;
            }
            if (!isdigit(c)) break;
            ipart *= 10;
            ipart += c - '0';
            continue;
        }

        else if (state == 'd') {
            if (c == '0') {
                dot0s++;
                continue;
            } else if (isdigit(c)) {
                state = 'f';
                fpart = c - '0';
                continue;
            }
            break;
        }

        else if (state == 'f') {
            if (!isdigit(c)) break;
            fpart *= 10;
            fpart += c - '0';
            continue;
        }
    }

    if (state == 'i') {
        num->isInt = true;
        num->Int64 = ipart;
    } else if (state == 'f' || (state == 'd' && dot0s > 0)) {
        num->isInt = false;
        num->Double = ipart;
        int fpartlen = 0;
        for (size_t temp = fpart; temp != 0; temp /= 10) fpartlen++;
        double res = fpart;
        for (int i = 0; i < dot0s + fpartlen; i++) {
            res /= 10;
        }
        num->Double += res;
    } else {
        goto fail;
    }
    return i;
fail:
    return 0;
}

void init_jstr(JsonStr *str) {
    assert(str != NULL);
    str->len = 0;
    str->cap = INIT_JSTR_LEN + 1;
    char *temp = malloc(str->cap * sizeof(char));
    str->data = temp;
}

void free_jstr(JsonStr *str) {
    free(str->data);
}

int jstr_cpy(JsonStr *dst, const JsonStr *src) {
    memcpy(dst, src, sizeof(JsonStr));
    dst->data = malloc(src->cap * sizeof(char));
    memcpy(dst->data, src->data, src->cap);
    return dst->len;
}

int jstr_cpy_cstr(JsonStr *str, const char *cs, int len) {
    assert(str != NULL);
    assert(cs != NULL);
    if (str->cap <= len + 1) {
        str->cap = (len + 1) * 2;

        if (str->cap != 0) free(str->data);
        str->data = malloc(sizeof(char) * str->cap);
    }

    strncpy(str->data, cs, len);
    str->data[len] = '\0';
    str->len = len;

    return len;
}

const char *jstr_cstr(const JsonStr *str) {
    return str->data;
}

int jstr_from_cstr(JsonStr *dst, const char *src) {
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

static int parse_obj_field(JsonMap *resObj, Lexer *l, char *err) {
    int old = l->cursor;
    JsonStr strObj;
    JsonBaseObj fieldObj;

    if (!lexer_peek_expect(l, TK_STR)) {
        snprintf(err, ERR_MAX_LEN, "expect 'str'     %10s", l->curStr);
        goto fail;
    }
    jstr_cpy(&strObj, &lexer_peek(l)->jstr);
    lexer_next(l);

    if (!lexer_peek_expect(l, TK_COLON)) {
        snprintf(err, ERR_MAX_LEN, "expect ':'       %10s", l->curStr);
        goto fail;
    }
    lexer_next(l);

    if (parse_base_obj(&fieldObj, l, err) == 0) goto fail;

    jmap_set(resObj, jstr_cstr(&strObj), fieldObj);
    free_jstr(&strObj);
    return l->cursor - old;

fail:
    lexer_set(l, old);
    return 0;
}

static int parse_map(JsonMap *dst, Lexer *l, char *err) {
    int old = l->cursor;
    JsonMap obj;
    init_jmap(&obj);

    if (!lexer_peek_expect(l, TK_LBRACE)) {
        LOG("len: %ld contents %.10s", strlen(l->curStr), l->curStr);
        snprintf(err, ERR_MAX_LEN, "expect {       %10s", l->curStr);
        goto fail;
    }
    lexer_next(l);

    while (parse_obj_field(&obj, l, err) != 0) {
        if (lexer_peek_expect(l, TK_COMMA)) {
            lexer_next(l);
        } else {
            break;
        }
    }

    if (!lexer_peek_expect(l, TK_RBRACE)) {
        snprintf(err, ERR_MAX_LEN, "expect '}' at %d      %10s", l->cursor, l->curStr);
        goto fail;
    }
    lexer_next(l);

    *dst = obj;
    return l->cursor - old;

fail:
    free_jmap(&obj);
    lexer_set(l, old);
    return 0;
}

static int parse_array(JsonArray *dst, Lexer *l, char *err) {
    int old = l->cursor;
    JsonArray array;
    init_jarray(&array);
    if (!lexer_peek_expect(l, TK_LBRACKET)) {
        snprintf(err, ERR_MAX_LEN, "expect [       %10s", l->curStr);
        goto fail;
    }
    lexer_next(l);

    JsonBaseObj obj;

    while (parse_base_obj(&obj, l, err) != 0) {
        jarray_append(&array, obj);
        if (lexer_peek_expect(l, TK_COMMA)) {
            lexer_next(l);
        } else {
            break;
        }
    }

    if (!lexer_peek_expect(l, TK_RBRACKET)) {
        snprintf(err, ERR_MAX_LEN, "expect ']' at %d      %10s", l->cursor, l->curStr);
        goto fail;
    }
    lexer_next(l);

    // success:
    *dst = array;
    return l->cursor - old;
fail:
    free_jarry(&array);
    lexer_set(l, old);
    return 0;
}

static int parse_base_obj(JsonBaseObj *obj, Lexer *l, char *err) {
    int offset = 0;
    char nouse[ERR_MAX_LEN];
    offset = parse_map(&obj->jsonMap, l, nouse);
    if (offset != 0) {
        obj->type = JMAP;
        return offset;
    }

    offset = parse_array(&obj->jsonArray, l, nouse);
    if (offset != 0) {
        obj->type = JARRAY;
        return offset;
    }

    const Token *tk = lexer_peek(l);
    if (tk == NULL) {
        goto fail;
    }

    if (tk != NULL) {
        switch (tk->type) {
        case TK_STR:
            obj->type = JSTR;
            jstr_cpy(&obj->jsonStr, &tk->jstr);
            break;
        case TK_NUM:
            obj->type = JNUM;
            obj->jsonNum = tk->jnum;
            break;
        case TK_TRUE:
            obj->type = JBOOL;
            obj->jsonBool.data = true;
            break;
        case TK_FALSE:
            obj->type = JBOOL;
            obj->jsonBool.data = false;
            break;
        case TK_NULL:
            obj->type = JNULL;
            break;
        default:
            goto fail;
        }
    }
    int temp = l->curLen;
    lexer_next(l);
    return temp;

fail:
    snprintf(err, ERR_MAX_LEN, "can not parse base obj at %10s", l->curStr);
    return 0;
}

const char *minijson_version() {
    return "0.0.1";
}

int minijson_parse_str(JsonMap *res, const char *src, char *err) {
    Lexer l;
    init_lexer(&l, src);

    int len = parse_map(res, &l, err);
    free_lexer(&l);
    if (len == 0) {
        return -1;
    }
    return 0;
}

void minijson_to_str();
