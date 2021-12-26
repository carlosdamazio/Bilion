#define LEX_CAP 1024

enum token_sentinels {
    // file related tokens
    TOK_EOF               = -1,
    TOK_NEWLINE           = -2,

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

// Token to reset delimiters
static const Token empty_tok;

void free_tokens(Token *tokens);
Token* lex(char *line, int lineno);
