#include "token.h"

Token token_create(TokenType type, const char *lexeme) {
  return (Token){
      .type = type,
      .lexeme = lexeme,
  };
}
