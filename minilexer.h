#include <stdbool.h>
#include "minijson.h"
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

struct Lexer {
    const char *src;     // the raw input str
    int cursor;          // current postion of lexer
    const char *curStr;  // current str (src+cursor)
    bool isFinished;     // if the lexer can not be forward
    Token curToken;      // the current token
    int curLen;          // how many char belong to this token
};

typedef struct Lexer Lexer;

int init_lexer(Lexer *l, const char *src);
const Token *lexer_peek(Lexer *l);
bool lexer_peek_expect(Lexer *l, TokenType expect);
void free_lexer(Lexer *l);
int lexer_set(Lexer *l, int cursor);
int lexer_next(Lexer *l);
