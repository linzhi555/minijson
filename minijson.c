#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minijson.h"
#include "minilexer.h"
#include "miniutils.h"

const int INIT_JSTR_LEN = 100;

static int parse_base_obj(JsonBaseObj *obj, Lexer *l, char *err);

// parse the char* to JsonNum
int jnum_from_cstr(JsonNum *num, const char *cs, size_t n) {
    int64_t intRes;
    double floatRes;

    int preEmpty = skip_space(cs);
    cs += preEmpty;

    int res = sscanf(cs, "%ld", &intRes);
    if (res != 0) {
        num->isInt = true;
        num->Double = intRes;
        return n;
    }

    res = sscanf(cs, "%lf", &floatRes);
    if (res != 0) {
        num->isInt = false;
        num->Double = floatRes;
        return n;
    }

    return 0;
}

void init_jstr(JsonStr *str) {
    assert(str != NULL);
    str->len = 0;
    str->cap = INIT_JSTR_LEN + 1;
    printf("%s %d: malloc \n", __FILE__, __LINE__);
    char *temp = malloc(str->cap * sizeof(char));
    str->data = temp;
}

int jstr_cpy(JsonStr *dst, const JsonStr *src) {
    memcpy(dst, src, sizeof(JsonStr));
    dst->data = malloc(src->cap * sizeof(char));
    memcpy(dst->data, src->data, src->cap);
    return dst->len;
}

int jstr_cpy_cstr(JsonStr *str, const char *cs, int len) {
    assert(str != NULL);
    if (str->cap <= len + 1) {
        str->cap = (len + 1) * 2;

        if (str->data != NULL) free(str->data);
        printf("%s %d: malloc \n", __FILE__, __LINE__);
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
        sprintf(err, "expect 'str'     %10s", l->curStr);
        goto fail;
    }
    lexer_next(l);

    if (!lexer_peek_expect(l, TK_COLON)) {
        sprintf(err, "expect ':'       %10s", l->curStr);
        goto fail;
    }
    lexer_next(l);

    if (parse_base_obj(&fieldObj, l, err) == 0) goto fail;

    return l->cursor - old;

fail:
    l->cursor = old;
    lexer_peek(l);
    return 0;
}

static int init_json_map(JsonMap *map) {
    JsonMap temp = { .cap = 0, .len = 0, .keyList = NULL, .valueList = NULL };
    *map = temp;
    return 0;
}

static int parse_map(JsonMap *resObj, Lexer *l, char *err) {
    int old = l->cursor;
    JsonMap obj;
    init_json_map(&obj);

    if (!lexer_peek_expect(l, TK_LBRACE)) {
        sprintf(err, "expect '{'       %10s", l->curStr);
        goto fail;
    }
    lexer_next(l);

    bool missComma = false;
    while (parse_obj_field(&obj, l, err) != 0) {
        if (lexer_peek_expect(l, TK_COMMA)) {
            lexer_next(l);
        } else {
            missComma = true;
        }
    }

    if (missComma != true) {
        LOG("stop parse field did  not because miss comma %s", err);
        goto fail;
    }

    if (!lexer_peek_expect(l, TK_RBRACE)) {
        sprintf(err, "expect '}' at %d      %10s", l->cursor, l->curStr);
        goto fail;
    } else {
        lexer_next(l);
    }

    return l->cursor - old;

fail:
    l->cursor = old;
    lexer_peek(l);
    return 0;
}

static int parse_array(JsonArray *resObj, Lexer *l, char *err) {
    int old = l->cursor;
fail:
    l->cursor = old;
    lexer_peek(l);
    return 0;
}

static int parse_base_obj(JsonBaseObj *obj, Lexer *l, char *err) {
    int offset = 0;
    char nouse[100];
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
    sprintf(err, "can not parse base obj at %10s", l->curStr);
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
