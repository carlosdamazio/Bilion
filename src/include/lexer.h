#ifndef LEXER_H
#define LEX_CAP 1024
#define STACK_TRACE_HEADER "==============STACK TRACE===============\n"

// Token's possible states
enum token_sentinels {
    // file related tokens
    TOK_EOF               = -1,
    TOK_INVALID           = -2,

    // keywords
    TOK_PRINT             = -3,

    // primitives
    TOK_IDENTIFIER        = -4,
    TOK_STRING            = -5,

    // delimiters
    TOK_STR_DELIM         = -6,
    TOK_PAREN_OPEN_DELIM  = -7,
    TOK_PAREN_CLOSE_DELIM = -8,
    TOK_END_EXPR_DELIM    = -9,
};

typedef struct {
    int        kind;
    int        lineno;
    int        pos;
    char       *value;
} Token;

typedef struct {
    Token     delim_stack[10];
    char      *buffer;
    int       buffer_sz;
    char      *stack_trace;
    int       stack_trace_sz;
    Token     *tokens;
    int       tokens_sz;
} Lexer;

static const Token empty_tok;

void   free_tokens(Token *tokens);
void   free_lexer(Lexer *lexer);
Lexer* lex(FileInfo *fi, char *line);
#endif
