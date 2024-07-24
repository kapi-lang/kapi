#ifndef token_h
#define token_h

#include <stddef.h>

typedef enum TokenType {
  TOKEN_EOF,

  // Update Operators
  TOKEN_DECREMENT,
  TOKEN_INCREMENT,

  // Binary or Unary Operators
  TOKEN_MINUS,
  TOKEN_BITWISE_NOT,

  // Binary Operatiors
  TOKEN_NOT,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_EQUAL,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_LESS,
  TOKEN_LESS_EQUAL,
  TOKEN_PLUS,
  TOKEN_SLASH,
  TOKEN_STAR,
  TOKEN_REMAINDER,
  TOKEN_EXPONENTIATION,
  TOKEN_BITWISE_AND,
  TOKEN_BITWISE_OR,
  TOKEN_BITWISE_XOR,
  TOKEN_BITWISE_LEFT_SHIFT,
  TOKEN_BITWISE_RIGHT_SHIFT,
  TOKEN_BITWISE_UNSIGNED_RIGHT_SHIFT,

  // Assignment Operators
  TOKEN_ASSIGN,
  TOKEN_ASSIGN_INCREMENT,
  TOKEN_ASSIGN_DECREMENT,
  TOKEN_ASSIGN_MULTIPLY,
  TOKEN_ASSIGN_DIVIDE,
  TOKEN_ASSIGN_REMAINDER,
  TOKEN_ASSIGN_EXPONENTIATION,
  TOKEN_ASSIGN_BITWISE_AND,
  TOKEN_ASSIGN_BITWISE_OR,
  TOKEN_ASSIGN_BITWISE_XOR,
  TOKEN_ASSIGN_BITWISE_LEFT_SHIFT,
  TOKEN_ASSIGN_BITWISE_RIGHT_SHIFT,
  TOKEN_ASSIGN_BITWISE_UNSIGNED_RIGHT_SHIFT,

  // Declarations
  TOKEN_VAR,
  TOKEN_STRUCT,
  TOKEN_FUNCTION,
  TOKEN_ENUM,

  // Values
  TOKEN_IDENTIFIER,
  TOKEN_INTEGER,
  TOKEN_FLOAT,
  TOKEN_BOOLEAN,
  TOKEN_STRING,
  TOKEN_NIL,
  TOKEN_FALSE,
  TOKEN_TRUE,

  // Keywords
  TOKEN_PRINT,
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_WHILE,
  TOKEN_FOR,
  TOKEN_IN,
  TOKEN_TO,
  TOKEN_DOWNTO,
  TOKEN_BREAK,
  TOKEN_CONTINUE,
  TOKEN_RETURN,
  TOKEN_SWITCH, // TODO

  // Statements or Expressions Operatos
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_LEFT_BRACKET,
  TOKEN_RIGHT_BRACKET,
  TOKEN_COMMA,
  TOKEN_COLON,
  TOKEN_DOT,
  TOKEN_SEMICOLON,
  TOKEN_QUESTION_MARK,
} TokenType;

typedef struct Token {
  TokenType type;
  const char *lexeme;
  size_t line;
  size_t column;
} Token;

Token token_create(TokenType type, const char *lexeme, size_t line,
                   size_t column);

inline const char *token_to_string(TokenType type) {
  switch (type) {
  case TOKEN_STRING:
    return "String";

  case TOKEN_BOOLEAN:
    return "Boolean";

  case TOKEN_FLOAT:
    return "Float";

  case TOKEN_INTEGER:
    return "Integer";

  default:
    return "?";
  }
}

#endif
