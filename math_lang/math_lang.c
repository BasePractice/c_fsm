#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "math_lang.h"

struct LexerContext {
    const char *content;
    size_t content_size;
    size_t it;
    size_t line_no;
    size_t line_it;
};

bool lexer_init_string(struct LexerContext **ctx, const char *text) {
    if (ctx == 0)
        return false;
    if (text == 0)
        return false;
    (*ctx) = calloc(1, sizeof(struct LexerContext));
    (*ctx)->content = strdup(text);
    (*ctx)->content_size = strlen(text);
    (*ctx)->line_no = 1;
    (*ctx)->line_it = 0;
    return true;
}

bool lexer_init_file(struct LexerContext **ctx, const char *filename) {
    FILE *fd;
    if (ctx == 0)
        return false;
    if (filename == 0)
        return false;

    fd = fopen(filename, "rt");
    if (fd == 0)
        return false;
    (*ctx) = calloc(1, sizeof(struct LexerContext));
    fseek(fd, 0, SEEK_END);
    (*ctx)->content_size = (size_t) ftell(fd);
    fseek(fd, 0, SEEK_SET);
    (*ctx)->content = calloc((*ctx)->content_size, 1);
    fread((void *) (*ctx)->content, (*ctx)->content_size, 1, fd);
    fclose(fd);
    (*ctx)->line_no = 1;
    return true;
}

void lexer_destroy(struct LexerContext **ctx) {
    if (ctx != 0 && (*ctx) != 0) {
        free((void *) (*ctx)->content);
        free((*ctx));
        (*ctx) = 0;
    }
}

static bool lexer_symbol_next(struct LexerContext *ctx) {
    if (ctx != 0 && ctx->it < ctx->content_size) {
        ctx->it++;
        ctx->line_it++;
        return true;
    }
    return false;
}

inline static char lexer_symbol_peek(struct LexerContext *ctx) {
    assert(ctx->it < ctx->content_size);
    return ctx->content[ctx->it];
}

inline static bool

lexer_symbol_is(struct LexerContext *ctx, char original) {
    return lexer_symbol_peek(ctx) == original;
}

inline static bool

lexer_symbol_is_digit(struct LexerContext *ctx) {
    char symbol = lexer_symbol_peek(ctx);
    return symbol >= '0' && symbol <= '9';
}

inline static bool

lexer_symbol_is_dot(struct LexerContext *ctx) {
    return lexer_symbol_is(ctx, '.');
}

inline static bool

lexer_symbol_is_lpar(struct LexerContext *ctx) {
    return lexer_symbol_is(ctx, '(');
}

inline static bool

lexer_symbol_is_rpar(struct LexerContext *ctx) {
    return lexer_symbol_is(ctx, ')');
}

inline static bool

lexer_symbol_is_mul(struct LexerContext *ctx) {
    return lexer_symbol_is(ctx, '*');
}

inline static bool

lexer_symbol_is_plus(struct LexerContext *ctx) {
    return lexer_symbol_is(ctx, '+');
}

inline static bool

lexer_symbol_is_pol(struct LexerContext *ctx) {
    return lexer_symbol_is(ctx, '^');
}

inline static bool

lexer_symbol_is_minus(struct LexerContext *ctx) {
    return lexer_symbol_is(ctx, '-');
}

inline static bool

lexer_symbol_is_div(struct LexerContext *ctx) {
    return lexer_symbol_is(ctx, '/');
}

inline static bool

lexer_symbol_is_alpha(struct LexerContext *ctx) {
    char symbol = lexer_symbol_peek(ctx);
    return (symbol >= 'A' && symbol <= 'Z') ||
           (symbol >= 'a' && symbol <= 'z');
}

inline static bool

lexer_symbol_is_space(struct LexerContext *ctx) {
    char symbol = lexer_symbol_peek(ctx);
    return symbol == ' ' || symbol == '\t';
}

inline static bool

lexer_symbol_is_nl(struct LexerContext *ctx) {
    char symbol = lexer_symbol_peek(ctx);
    return symbol == '\r' || symbol == '\n';
}

inline static bool

lexer_symbol_parse_numeric(struct LexerContext *ctx, struct LexerToken *tok) {
    bool ret = true;
    tok->p = ctx->content + ctx->it;
    tok->it_start = ctx->it;
    tok->type = TokenInt;
    while (lexer_symbol_next(ctx) && !lexer_eof(ctx)) {
        if (lexer_symbol_is_digit(ctx))
            continue;
        if (lexer_symbol_is_dot(ctx)) {
            if (tok->type == TokenReal) {
                tok->error = "Duplicate dot present on input sequences";
                ret = false;
                break;
            }
            tok->type = TokenReal;
            continue;
        }
        break;
    }
    tok->it_end = ctx->it;
    return ret;
}

inline static bool

lexer_symbol_parse_id(struct LexerContext *ctx, struct LexerToken *tok) {
    tok->type = TokenId;
    tok->p = ctx->content + ctx->it;
    tok->it_start = ctx->it;
    while (lexer_symbol_next(ctx) && !lexer_eof(ctx)) {
        if (lexer_symbol_is_alpha(ctx))
            continue;
        if (lexer_symbol_is_digit(ctx))
            continue;
        if (lexer_symbol_is(ctx, '_'))
            continue;
        break;
    }
    tok->it_end = ctx->it;
    return true;
}

inline static bool

lexer_symbol_skip(struct LexerContext *ctx) {
    while (!lexer_eof(ctx) && (lexer_symbol_is_space(ctx) || lexer_symbol_is_nl(ctx))) {
        if (lexer_symbol_is_nl(ctx)) {
            ctx->line_no++;
            ctx->line_it = 0;
        }
        lexer_symbol_next(ctx);
    }
    return !lexer_eof(ctx);
}

bool lexer_eof(struct LexerContext *ctx) {
    if (ctx != 0 && ctx->it < ctx->content_size) {
        return false;
    }
    return true;
}

static bool lexer_sync(struct LexerContext *ctx, struct LexerToken *token) {
    ctx->it = token->it_start;
    ctx->line_it = token->line_no;
    return true;
}

bool lexer_peek(struct LexerContext *ctx, struct LexerToken *token) {
    size_t it = ctx->it, line_no = ctx->line_no, line_it = ctx->line_it;
    bool ret = lexer_next(ctx, token);
    ctx->it = it;
    ctx->line_no = line_no;
    ctx->line_it = line_it;
    return ret;
}

bool lexer_next(struct LexerContext *ctx, struct LexerToken *token) {
    if (token == 0)
        return false;
    token->type = End;
    if (lexer_eof(ctx))
        return false;
    if (!lexer_symbol_skip(ctx))
        return false;
    token->p = ctx->content + ctx->it;
    token->it_start = ctx->it;
    token->it_end = ctx->it;
    token->line_no = ctx->line_no;
    token->error = 0;
    if (lexer_symbol_is_plus(ctx)) {
        token->type = TokenPlus;
        lexer_symbol_next(ctx);
        return true;
    } else if (lexer_symbol_is_minus(ctx)) {
        token->type = TokenMinus;
        lexer_symbol_next(ctx);
        return true;
    } else if (lexer_symbol_is_div(ctx)) {
        token->type = TokenDiv;
        lexer_symbol_next(ctx);
        return true;
    } else if (lexer_symbol_is_mul(ctx)) {
        token->type = TokenMul;
        lexer_symbol_next(ctx);
        return true;
    } else if (lexer_symbol_is_lpar(ctx)) {
        token->type = TokenLPar;
        lexer_symbol_next(ctx);
        return true;
    } else if (lexer_symbol_is_rpar(ctx)) {
        token->type = TokenRPar;
        lexer_symbol_next(ctx);
        return true;
    } else if (lexer_symbol_is_rpar(ctx)) {
        token->type = TokenMul;
        lexer_symbol_next(ctx);
        return true;
    } else if (lexer_symbol_is_pol(ctx)) {
        token->type = TokenPol;
        lexer_symbol_next(ctx);
        return true;
    } else if (lexer_symbol_is_digit(ctx)) {
        return lexer_symbol_parse_numeric(ctx, token);
    } else if (lexer_symbol_is_alpha(ctx)) {
        return lexer_symbol_parse_id(ctx, token);
    }
    token->error = "Unknown input token";
    return false;
}

enum NodeType {
    RealValue,
    IntValue,
    Subroutine,
    MathDiv,
    MathMul,
    MathSub,
    MathAdd,
    MathPol,
    Constant,
};

struct Node {
    enum NodeType type;
    union {
        struct BinaryOperation {
            struct Node *left;
            struct Node *right;
        } binary;
        struct Node *root;
    } un;
    union {
        double d_value;
        long i_value;
        char *id;
    } value;
};

static void node_destroy(struct Node **node) {
    if (0 != node && 0 != (*node)) {
        free((*node));
        (*node) = 0;
    }
}

struct ParserContext {
    struct LexerContext *lexer;
    struct LexerToken token;
    struct Node *root;
};

bool parser_init(struct ParserContext **pparser) {
    (*pparser) = calloc(1, sizeof(struct ParserContext));
    (*pparser)->lexer = 0;
    memset(&(*pparser)->token, 0, sizeof(struct LexerToken));
    return true;
}

void parser_destroy(struct ParserContext **pparser) {
    if (pparser && 0 != (*pparser)) {
        if (0 != (*pparser)->lexer) {
            lexer_destroy(&(*pparser)->lexer);
        }
        free((*pparser));
    }
}

/*
 * BNF
 *
  Program: Statement

  Statement: SimpleStmt '-' SimpleStmt |
             SimpleStmt '+' SimpleStmt |
             SimpleStmt '/' SimpleStmt |
             SimpleStmt '*' SimpleStmt |
             SimpleStmt '^' SimpleStmt |
             SimpleStmt

  Subroutine: Id '(' Statement ')'

  SimpleStmt: Value |
              '(' Statement ')' |
              Subroutine

  Value:  Digit+ | Const

  Const:  Id
 *
 */

#define LEXER_NEXT lexer_next(ctx->lexer, &ctx->token)
#define LEXER_PEEK lexer_peek(ctx->lexer, &ctx->token)

static struct Node *parse_statement(struct ParserContext *ctx);

static struct Node *parse_value(struct ParserContext *ctx) {
    char n_buf[127];
    struct Node *root = 0;
    if (ctx->token.type == TokenReal) {
        root = calloc(1, sizeof(struct Node));
        root->type = RealValue;
        memset(n_buf, 0, sizeof(n_buf));
        memcpy(n_buf, ctx->token.p, ctx->token.it_end - ctx->token.it_start);
        root->value.d_value = strtod(n_buf, 0);
        LEXER_NEXT;
    } else if (ctx->token.type == TokenInt) {
        root = calloc(1, sizeof(struct Node));
        root->type = IntValue;
        memset(n_buf, 0, sizeof(n_buf));
        memcpy(n_buf, ctx->token.p, ctx->token.it_end - ctx->token.it_start);
        root->value.i_value = strtol(n_buf, 0, 10);
        LEXER_NEXT;
    } else if (ctx->token.type == TokenId) {
        root = calloc(1, sizeof(struct Node));
        root->type = Subroutine;
        memset(n_buf, 0, sizeof(n_buf));
        memcpy(n_buf, ctx->token.p, ctx->token.it_end - ctx->token.it_start);
        root->value.id = strdup(n_buf);
        LEXER_NEXT;
    }
    return root;
}

static struct Node *parse_simple_stmt(struct ParserContext *ctx) {
    char n_buf[127];
    struct Node *root = 0;

    if (ctx->token.type == TokenId) {
        root = calloc(1, sizeof(struct Node));
        LEXER_PEEK;
        if (ctx->token.type == TokenLPar) {
            LEXER_NEXT;
            root->type = Subroutine;
            memset(n_buf, 0, sizeof(n_buf));
            memcpy(n_buf, ctx->token.p, ctx->token.it_end - ctx->token.it_start);
            root->value.id = strdup(n_buf);
            root->un.root = parse_statement(ctx);
        } else {
            root->type = Constant;
            memset(n_buf, 0, sizeof(n_buf));
            memcpy(n_buf, ctx->token.p, ctx->token.it_end - ctx->token.it_start);
            root->value.id = strdup(n_buf);
        }
    } else if (ctx->token.type == TokenLPar) {
        root = parse_statement(ctx);
        if (LEXER_PEEK && ctx->token.type == TokenRPar) {
            LEXER_NEXT;
        } else {
            node_destroy(&root);
        };
    } else {
        return parse_value(ctx);
    }
    return root;
}


static struct Node *parse_statement(struct ParserContext *ctx) {
    struct Node *root = 0;

    LEXER_PEEK;
    root = parse_simple_stmt(ctx);
    if (root) {
        LEXER_PEEK;
        switch (ctx->token.type) {
            case TokenDiv: {
                struct Node *d = calloc(1, sizeof(struct Node));

                LEXER_NEXT;
                LEXER_NEXT;
                d->type = MathDiv;
                d->un.binary.left = root;
                d->un.binary.right = parse_simple_stmt(ctx);
                return root;
            }
            case TokenMul: {
                struct Node *d = calloc(1, sizeof(struct Node));

                LEXER_NEXT;
                LEXER_NEXT;
                d->type = MathMul;
                d->un.binary.left = root;
                d->un.binary.right = parse_simple_stmt(ctx);
                return root;
            }
            case TokenMinus: {
                struct Node *d = calloc(1, sizeof(struct Node));

                LEXER_NEXT;
                LEXER_NEXT;
                d->type = MathSub;
                d->un.binary.left = root;
                d->un.binary.right = parse_simple_stmt(ctx);
                return root;
            }
            case TokenPlus: {
                struct Node *d = calloc(1, sizeof(struct Node));

                LEXER_NEXT;
                LEXER_NEXT;
                d->type = MathAdd;
                d->un.binary.left = root;
                d->un.binary.right = parse_simple_stmt(ctx);
                return root;
            }
            case TokenPol: {
                struct Node *d = calloc(1, sizeof(struct Node));

                LEXER_NEXT;
                LEXER_NEXT;
                d->type = MathPol;
                d->un.binary.left = root;
                d->un.binary.right = parse_simple_stmt(ctx);
                return root;
            }
            default: {
                fprintf(stderr, "Expected math operation, but found %.*s\n",
                        (int) (ctx->token.it_end - ctx->token.it_start),
                        ctx->token.p);
                node_destroy(&root);
                break;
            }
        }
    }
    return root;
}

static bool parser_parse(struct ParserContext *ctx) {
    ctx->root = parse_statement(ctx);
    return 0 != ctx->root;
}

bool parser_parse_string(struct ParserContext *ctx, const char *text) {
    if (0 != ctx) {
        lexer_init_string(&(ctx->lexer), text);
        return parser_parse(ctx);
    }
    return false;
}

bool parser_parse_file(struct ParserContext *ctx, const char *filename) {
    if (0 != ctx && 0 != ctx->lexer) {
        lexer_init_file(&(ctx->lexer), filename);
        return parser_parse(ctx);
    }
    return false;
}
