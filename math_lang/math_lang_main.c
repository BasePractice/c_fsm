#include <string.h>
#include <assert.h>
#include "math_lang.h"

static void test_lexer(const char *text) {
    struct LexerContext *ctx = 0;
    struct LexerToken token = {};
    assert(lexer_init_string(&ctx, text));
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenLPar);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenInt);
    assert(strncmp("3", token.p, token.it_end - token.it_start) == 0);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenMul);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenInt);
    assert(strncmp("8", token.p, token.it_end - token.it_start) == 0);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenRPar);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenDiv);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenInt);
    assert(strncmp("6", token.p, token.it_end - token.it_start) == 0);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenPlus);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenInt);
    assert(strncmp("1", token.p, token.it_end - token.it_start) == 0);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenMinus);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenId);
    assert(strncmp("sin", token.p, token.it_end - token.it_start) == 0);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenLPar);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenReal);
    assert(strncmp("1.5667", token.p, token.it_end - token.it_start) == 0);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenRPar);
    assert(!lexer_next(ctx, &token));
    assert(token.type == End);
    lexer_destroy(&ctx);
    assert(ctx == 0);
}

static void test_parser(const char *text) {
    struct ParserContext *ctx = 0;

    assert(parser_init(&ctx));
    assert(parser_parse_string(ctx, text));
    parser_destroy(&ctx);
    assert(ctx == 0);
}

int main() {
    test_lexer("(3 * 8)/6 + 1 - sin(1.5667)");
    test_parser("(3 * 8)/6 + 1 - sin(1.5667)");
    return EXIT_SUCCESS;
}

