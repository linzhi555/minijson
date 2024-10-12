#include "minijson.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int INIT_JSTR_LEN = 100;

static int skip_space(const char *cs) {
    for (int i = 0; cs[i] != '\0'; i++) {
        if (!isspace(cs[i])) return i;
    }
    return 0;
}

static int find_space(const char *cs) {
    for (int i = 0; cs[i] != '\0'; i++) {
        if (isspace(cs[i])) return i;
    }
    return 0;
}

static int jnum_from_cstr(JsonNum *num, const char *cs, int n) {
    int64_t intRes;
    double floatRes;

    if (strlen(cs) <= n) {
        return 0;
    }

    int preEmpty = skip_space(cs);
    cs += preEmpty;

    char *temp = malloc(sizeof(char) * (n + 1));
    strcpy(temp, cs);

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

    free(temp);
    return 0;
}

static void init_jstr(JsonStr *str) {
    assert(str != NULL);
    str->len = 0;
    str->cap = INIT_JSTR_LEN + 1;
    str->data = malloc(str->cap * sizeof(char));
}

static int _jstr_adjust_cap(JsonStr *str, int newCap) {
    assert(str != NULL);
    if (str->len + 1 <= newCap) {
        str->data = realloc(str->data, newCap * sizeof(char));
        return newCap;
    }

    return 0;
}

static int jstr_cpy(JsonStr *dst, const JsonStr *src) {
    memcpy(dst, src, sizeof(JsonStr));
    dst->data = malloc(src->cap);
    memcpy(dst->data, src->data, src->cap);
    return dst->len;
}

static int jstr_cpy_cstr(JsonStr *str, const char *cs, int len) {
    assert(str != NULL);
    if (str->cap <= len + 1) {
        str->cap = (len + 1) * 2;

        if (str->data != NULL) free(str->data);
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

int jstr_to_jnum(const JsonStr *str, JsonNum *num) {
    return jnum_from_cstr(num, jstr_cstr(str), str->len);
}

int jstr_append(JsonStr *str, char newch) {
    if (str->cap <= str->len - 1) {
        _jstr_adjust_cap(str, str->cap * 2);
    }

    str->data[str->len] = newch;
    str->len++;
    str->data[str->len] = '\0';
    return str->len;
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
            return i;
        }
    }
    return 0;
}

typedef enum {
    TError,
    TNum,
    TStr,
    TTrue,
    TFalse,
    TNUll,
    TLBrace,
    TRBrace,
    TComma,
    TColon,
    TLBracket,
    TRBracket,
} TokenType;

typedef struct {
    TokenType type;
    JsonStr jstr;
    JsonNum jnum;
} Token;

typedef struct {
    const char *src;
    int cursor;
    bool isFinished;
    Token curToken;
    int curLen;
} Lexer;

int init_lexer(Lexer *l, const char *src) {
    l->src = src;
    l->cursor = 0;
    l->isFinished = false;
    l->curLen = 0;
    init_jstr(&l->curToken.jstr);

    return 0;
}

void free_lexer(Lexer *l) {
}

const Token *lexer_peek(Lexer *l) {
    const char *curStr = l->src + l->cursor;
    const int preEmpty = skip_space(curStr);
    const char *str = curStr + preEmpty;
    int len = 0;  // the token actual lenth(no empty chars)
    switch (str[0]) {
    case '{':
        len = 1;
        l->curToken.type = TLBrace;
        break;
    case '}':
        len = 1;
        l->curToken.type = TRBrace;
        break;
    case '[':
        len = 1;
        l->curToken.type = TLBracket;
        break;
    case ']':
        len = 1;
        l->curToken.type = TRBracket;
        break;
    case ':':
        len = 1;
        l->curToken.type = TColon;
        break;
    case ',':
        len = 1;
        l->curToken.type = TComma;
        break;
    default:
        break;
    }

    if (len != 0) goto finish;

    if (strncmp(str, "null", 4) && isspace(str[4])) {
        l->curToken.type = TNUll;
        len = 4;
    }

    if (strncmp(str, "true", 4) && isspace(str[4])) {
        l->curToken.type = TTrue;
        len = 4;
    }

    if (strncmp(str, "false", 5) && isspace(str[5])) {
        l->curToken.type = TFalse;
        len = 5;
    }

    len = jnum_from_cstr(&l->curToken.jnum, str, find_space(str) + 1);
    if (len != 0) {
        l->curToken.type = TNum;
        goto finish;
    }

    len = jstr_from_cstr(&l->curToken.jstr, str);

    if (len != 0) {
        l->curToken.type = TStr;
        goto finish;
    }

    return NULL;
finish:
    l->curLen = preEmpty + len;
    return &l->curToken;
}

bool lexer_peek_expect(Lexer *l, TokenType expect) {
    const Token *tk = lexer_peek(l);
    if (tk == NULL) return false;
    return tk->type == expect;
}

int lexer_next(Lexer *l) {
    l->cursor += l->curLen;
    lexer_peek(l);
    return l->curLen;
}

static int parse_base_obj(JsonBaseObj *obj, Lexer *l);

static int parse_null(Lexer *l) {
    int old = l->cursor;
    if (!lexer_peek_expect(l, TNUll)) return 0;
    lexer_next(l);
    return l->cursor - old;
}

static int parse_num(JsonNum *num, Lexer *l) {
    int old = l->cursor;
    if (!lexer_peek_expect(l, TNum)) return 0;
    const Token *tk = lexer_peek(l);
    *num = l->curToken.jnum;
    lexer_next(l);
    return l->cursor - old;
}

static int parse_bool(JsonBool *jbool, Lexer *l) {
    int old = l->cursor;

    if (lexer_peek_expect(l, TTrue)) {
        jbool->data = true;
    } else if (lexer_peek_expect(l, TFalse)) {
        jbool->data = false;
    } else {
        goto fail;
    }

    lexer_next(l);
    return l->cursor - old;

fail:
    l->cursor = old;
    lexer_peek(l);
    return 0;
}

static int parse_str(JsonStr *str, Lexer *l) {
    int old = l->cursor;
    if (!lexer_peek_expect(l, TStr)) {
        goto fail;
    }

    jstr_cpy(str, &l->curToken.jstr);
    lexer_next(l);

fail:
    l->cursor = old;
    return 0;
}

static int parse_array(JsonArray *resObj, Lexer *l) {
    int old = l->cursor;

fail:
    l->cursor = old;
    lexer_peek(l);
    return 0;
}

static int parse_obj_field(JsonMap *resObj, Lexer *l) {
    int old = l->cursor;
    JsonStr strObj;
    JsonBaseObj fieldObj;

    if (!lexer_peek_expect(l, TStr)) goto fail;
    Token temp;
    lexer_next(l);
    strObj = temp.jstr;

    if (!lexer_peek_expect(l, TColon)) goto fail;
    lexer_next(l);

    if (parse_base_obj(&fieldObj, l) == 0) goto fail;

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

static int parse_map(JsonMap *resObj, Lexer *l) {
    int old = l->cursor;
    JsonMap obj;
    init_json_map(&obj);

    if (!lexer_peek_expect(l, TLBrace)) goto fail;

    lexer_next(l);
    while (parse_obj_field(&obj, l) != 0)
        ;

    if (!lexer_peek_expect(l, TRBrace)) goto fail;

    lexer_next(l);
    return l->cursor - old;

fail:
    l->cursor = old;
    lexer_peek(l);
    return 0;
}

static int parse_base_obj(JsonBaseObj *obj, Lexer *l) {
    int offset = 0;

    offset = parse_map(&obj->jsonMap, l);
    if (offset != 0) {
        obj->type = JMap;
        return offset;
    }

    offset = parse_array(&obj->jsonArray, l);
    if (offset != 0) {
        obj->type = JArray;
        return offset;
    }

    offset = parse_str(&obj->jsonStr, l);
    if (offset != 0) {
        obj->type = JStr;
        return offset;
    }

    offset = parse_num(&obj->jsonNum, l);
    if (offset != 0) {
        obj->type = JNum;
        return offset;
    }

    offset = parse_bool(&obj->jsonBool, l);
    if (offset != 0) {
        obj->type = JBool;
        return offset;
    }

    offset = parse_null(l);
    if (offset != 0) {
        obj->type = JNull;
        return offset;
    }

    return 0;
}

const char *minijson_version() {
    return "0.0.1";
}

int minijson_parse_str(JsonMap *res, const char *src) {
    Lexer l;
    init_lexer(&l, src);
    int len = parse_map(res, &l);

    if (len != strlen(src)) return -1;

    free_lexer(&l);
    return len;
}

void minijson_to_str();
