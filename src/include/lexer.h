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
    Token  *tokens;
    char   *stack_trace;
} LexResult;

// Token to reset delimiters
static const Token empty_tok;
// Functions prototypes of lexer.c
void        free_tokens(Token *tokens);
void        free_lex_result(LexResult *result);
LexResult*  lex(FileInfo *fi, char *line);
