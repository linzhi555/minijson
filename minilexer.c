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

int lexer_set(Lexer *l, int cursor) {
    l->cursor = cursor;
    l->curStr = l->src + l->cursor;
    lexer_peek(l);
    return l->curLen;
}

int lexer_next(Lexer *l) {
    return lexer_set(l, l->cursor + l->curLen);
}
