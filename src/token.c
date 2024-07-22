#include "token.h"

Token token_create(TokenType type, const char *lexeme, size_t line,
                   size_t column) {
  return (Token){
      .type = type,
      .lexeme = lexeme,
      .line = line,
      .column = column,
  };
}
