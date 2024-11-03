#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "minilexer.h"
#include "miniutils.h"

int init_lexer(Lexer *l, const char *src) {
    l->src = src;
    l->cursor = 0;
    l->curStr = l->src + l->cursor;
    l->isFinished = false;
    l->curLen = 0;
    init_jstr(&l->curToken.jstr);
    return 0;
}

void free_lexer(Lexer *l) {
    free(l->curToken.jstr.data);
}

const Token *lexer_peek(Lexer *l) {
    const char *curStr = l->src + l->cursor;
    const int preEmpty = skip_space(curStr);
    const char *str = curStr + preEmpty;
    int len = 0;  // the token actual lenth(no empty chars)
    switch (str[0]) {
    case '{':
        len = 1;
        l->curToken.type = TK_LBRACE;
        break;
    case '}':
        len = 1;
        l->curToken.type = TK_RBRACE;
        break;
    case '[':
        len = 1;
        l->curToken.type = TK_LBRACKET;
        break;
    case ']':
        len = 1;
        l->curToken.type = TK_RBRACKET;
        break;
    case ':':
        len = 1;
        l->curToken.type = TK_COLON;
        break;
    case ',':
        len = 1;
        l->curToken.type = TK_COMMA;
        break;
    default:
        break;
    }

    if (len != 0) goto finish;

    if (strncmp(str, "null", 4) == 0 && !isalpha(str[4]) && !isdigit(str[4])) {
        l->curToken.type = TK_NULL;
        len = 4;
        goto finish;
    }

    if (strncmp(str, "true", 4) == 0 && !isalpha(str[4]) && !isdigit(str[4])) {
        l->curToken.type = TK_TRUE;
        len = 4;
        goto finish;
    }

    if (strncmp(str, "false", 5) == 0 && !isalpha(str[5]) && !isdigit(str[5])) {
        l->curToken.type = TK_FALSE;
        len = 5;
        goto finish;
    }

    len = jnum_from_cstr(&l->curToken.jnum, str, find_no_dot_or_digit(str) + 1);
    if (len != 0) {
        l->curToken.type = TK_NUM;
        goto finish;
    }

    len = jstr_from_cstr(&l->curToken.jstr, str);

    if (len != 0) {
        l->curToken.type = TK_STR;
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

int lexer_set(Lexer *l, int cursor) {
    l->cursor = cursor;
    l->curStr = l->src + l->cursor;
    lexer_peek(l);
    return l->curLen;
}

int lexer_next(Lexer *l) {
    return lexer_set(l, l->cursor + l->curLen);
}
