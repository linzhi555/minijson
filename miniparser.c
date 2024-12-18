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

static int parse_value(JsonValue *obj, Lexer *l, JsonStr *err);

// obj_field = key(jsonStr) + : + value(jsonValue)
static int parse_obj_field(JsonMap *resObj, Lexer *l, JsonStr *err) {
    int old = l->cursor;
    JsonStr strObj;
    init_jstr(&strObj);
    JsonValue fieldObj;
    init_jvalue_null(&fieldObj);

    if (!lexer_peek_expect(l, TK_STR)) {
        jstr_sprintf(err, "expect key when parse obj field at\n%.30s", l->curStr);
        goto fail;
    }
    jstr_cpy(&strObj, &lexer_peek(l)->jstr);
    lexer_next(l);

    if (!lexer_peek_expect(l, TK_COLON)) {
        jstr_sprintf(err, "expect ':' when parse obj field at\n%.30s", l->curStr);
        goto fail;
    }
    lexer_next(l);

    if (parse_value(&fieldObj, l, err) == 0) goto fail;

    jmap_set(resObj, jstr_cstr(&strObj), fieldObj);
    free_jstr(&strObj);
    return l->cursor - old;

fail:
    free_jstr(&strObj);
    free_jvalue(&fieldObj);
    lexer_set(l, old);
    return 0;
}

static int parse_map(JsonMap *dst, Lexer *l, JsonStr *err) {
    int old = l->cursor;
    JsonMap obj;
    init_jmap(&obj);

    if (!lexer_peek_expect(l, TK_LBRACE)) {
        LOG("len: %ld contents %.30s", strlen(l->curStr), l->curStr);
        jstr_sprintf(err, "expect { at\n%.30s", l->curStr);
        goto fail;
    }
    lexer_next(l);

    // parse map_fields of map
    // map_fileds = empty | map_field + N * ( , + map_field)
    if (parse_obj_field(&obj, l, err) == 0) {
    } else {
        while (true) {
            if (lexer_peek_expect(l, TK_COMMA)) {
                lexer_next(l);
            } else {
                break;
            }
            if (parse_obj_field(&obj, l, err) == 0) {
                goto fail;
            }
        }
    }

    if (!lexer_peek_expect(l, TK_RBRACE)) {
        jstr_sprintf(err, "expect '}' at\n %.30s", l->curStr);
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

static int parse_array(JsonArray *dst, Lexer *l, JsonStr *err) {
    int old = l->cursor;
    JsonArray array;
    init_jarray(&array);
    if (!lexer_peek_expect(l, TK_LBRACKET)) {
        jstr_sprintf(err, "expect '[' at\n%.30s", l->curStr);
        goto fail;
    }
    lexer_next(l);

    JsonValue obj;
    init_jvalue_null(&obj);

    // array elements = empty | obj + N*( , + obj)
    if (parse_value(&obj, l, err) == 0) {
    } else {
        jarray_append(&array, obj);
        while (true) {
            if (lexer_peek_expect(l, TK_COMMA)) {
                lexer_next(l);
            } else {
                break;
            }
            if (parse_value(&obj, l, err) != 0) {
                jarray_append(&array, obj);
            } else {
                jstr_sprintf(err, "expect obj when parse array after , at \n%.30s", l->curStr);
                goto fail;
            }
        }
    }

    if (!lexer_peek_expect(l, TK_RBRACKET)) {
        jstr_sprintf(err, "expect ']' at\n%.30s", l->curStr);
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

// TODO: now the parse_value just can just give the ambigous error like "fail when parse_value"
// we need more specified error
static int parse_value(JsonValue *obj, Lexer *l, JsonStr *err) {
    int offset = 0;
    JsonStr nouse;
    init_jstr(&nouse);

    offset = parse_map(&obj->jsonMap, l, &nouse);
    if (offset != 0) {
        obj->type = JMAP;
        free_jstr(&nouse);
        return offset;
    }

    offset = parse_array(&obj->jsonArray, l, &nouse);
    if (offset != 0) {
        obj->type = JARRAY;
        free_jstr(&nouse);
        return offset;
    }
    free_jstr(&nouse);

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
    jstr_sprintf(err, "can not parse base obj at\n%.30s", l->curStr);
    return 0;
}

const char *minijson_version() {
    return "0.0.1";
}

int minijson_parse_map(JsonMap *res, const char *src, JsonStr *err) {
    Lexer l;
    init_lexer(&l, src);

    int len = parse_map(res, &l, err);
    free_lexer(&l);
    if (len == 0) {
        return -1;
    }
    return 0;
}

int minijson_parse_array(JsonArray *res, const char *src, JsonStr *err) {
    Lexer l;
    init_lexer(&l, src);

    int len = parse_array(res, &l, err);
    free_lexer(&l);
    if (len == 0) {
        return -1;
    }
    return 0;
}


int minijson_parse_any(JsonValue *res, const char *src, JsonStr *err) {
    Lexer l;
    init_lexer(&l, src);

    int len = parse_value(res, &l, err);
    free_lexer(&l);
    if (len == 0) {
        return -1;
    }
    return 0;
}


void minijson_to_str();
