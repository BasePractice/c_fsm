#include <string.h>
#include <assert.h>
#include "math_lang.h"

int main() {
    struct LexerContext *ctx = 0;
    struct LexerToken token = {};
    assert(lexer_init_string(&ctx, "(3 * 8)/6 + 1 - sin(1.5667)"));
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenLPar);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenInt);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenMul);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenInt);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenRPar);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenDiv);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenInt);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenPlus);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenInt);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenMinus);
    assert(lexer_next(ctx, &token));
    assert(token.type == TokenId);
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
    return EXIT_SUCCESS;
}

