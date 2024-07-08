#ifndef lexer_h
#define lexer_h

#include <stddef.h>

#include "token.h"

typedef struct LexerOutput {
  Token *tokens;
  size_t length;
} LexerOutput;

LexerOutput *lexer_run(const char *source);

#endif
