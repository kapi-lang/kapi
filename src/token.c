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

const char *token_to_string(TokenType type, const char *extra) {
  switch (type) {
  case TOKEN_STRING:
  case TOKEN_TYPE_STRING:
    return "String";

  case TOKEN_BOOLEAN:
  case TOKEN_TYPE_BOOLEAN:
    return "Boolean";

  case TOKEN_FLOAT:
  case TOKEN_TYPE_FLOAT:
    return "Float";

  case TOKEN_INTEGER:
  case TOKEN_TYPE_INTEGER:
    return "Integer";

  case TOKEN_TYPE_NUMBER:
    return "Number";

  case TOKEN_TYPE_VOID:
    return "Void";

  case TOKEN_TYPE_STRUCT:
    return extra;

  default:
    return "?";
  }
}
