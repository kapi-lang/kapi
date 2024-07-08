#include <stdbool.h>
#include <string.h>

#include "lexer.h"
#include "macros.h"
#include "token.h"

typedef struct LexerState {
  int line;
  int column;
  const char *start;
  const char *cursor;
} LexerState;

LexerState lexer;

static void lexer_commit() { lexer.start = lexer.cursor; }

static void lexer_next_line() {
  lexer.line++;
  lexer.column = 0;
}

static char lexer_peek() { return *lexer.cursor; }

static char lexer_next() { return lexer.cursor[1]; }

static char lexer_is_eof() { return *lexer.cursor == '\0'; }

static char lexer_advance() {
  lexer.cursor++;
  lexer.column++;

  return lexer.cursor[-1];
}

static bool char_is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool char_is_digit(char c) { return c >= '0' && c <= '9'; }

void lexer_add_token(LexerOutput *output, Token token) {
  const size_t index = output->length++;

  output->tokens = ARRAY_RESIZE(Token, output->tokens, output->length);

  output->tokens[index] = token;

  LOG_DEBUG("[lexer] token: (%d) '%s' \n", token.type, token.lexeme);

  lexer_commit();
}

static Token lexer_string() {
  while (lexer_peek() != '"' && !lexer_is_eof()) {
    if (lexer_peek() == '\n') {
      lexer.line++;
    }

    lexer_advance();
  }

  if (lexer_is_eof()) {
    EXCEPTION("unterminated string\n");
  }

  /* The closing quote. */
  lexer_advance();

  const size_t length = (size_t)(lexer.cursor - lexer.start);

  char *lexeme = STRING_NEW(char, length);

  (void)memcpy(lexeme, lexer.start, length);

  lexeme[length] = '\0';

  return token_create(TOKEN_STRING, lexeme);
}

static Token lexer_number() {
  TokenType type = TOKEN_INTEGER;

  while (char_is_digit(lexer_peek()))
    lexer_advance();

  if (lexer_peek() == '.' && char_is_digit(lexer_next())) {
    type = TOKEN_FLOAT;

    lexer_advance();

    while (char_is_digit(lexer_peek()))
      lexer_advance();
  }

  const size_t length = (size_t)(lexer.cursor - lexer.start);

  char *lexeme = STRING_NEW(char, length);

  (void)memcpy(lexeme, lexer.start, length);

  lexeme[length] = '\0';

  return token_create(type, lexeme);
}

static TokenType lexer_check_identifier(int start, int length, const char *rest,
                                        TokenType type) {
  if (lexer.cursor - lexer.start == start + length &&
      memcmp(lexer.start + start, rest, length) == 0) {
    return type;
  }

  return TOKEN_IDENTIFIER;
}

static TokenType lexer_identifier_type() {
  switch (lexer.start[0]) {
  case 'a':
    return lexer_check_identifier(1, 2, "nd", TOKEN_AND);

  case 'e':
    return lexer_check_identifier(1, 1, "q", TOKEN_EQUAL);

  case 't':
    return lexer_check_identifier(1, 3, "rue", TOKEN_TRUE);

  case 'f':
    return lexer_check_identifier(1, 4, "alse", TOKEN_FALSE);

  case 'v':
    return lexer_check_identifier(1, 2, "ar", TOKEN_VAR);

  case 'p':
    return lexer_check_identifier(1, 4, "rint", TOKEN_PRINT);

  case 'n': {
    switch (lexer.start[1]) {
    case 'i':
      return lexer_check_identifier(1, 2, "il", TOKEN_NIL);
    case 'o':
      return lexer_check_identifier(1, 2, "ot", TOKEN_NOT);
    }

    break;
  }

  case 'g': {
    if (lexer.cursor - lexer.start == 2) {
      return lexer_check_identifier(1, 1, "t", TOKEN_GREATER);
    } else if (lexer.cursor - lexer.start == 3) {
      return lexer_check_identifier(1, 2, "te", TOKEN_GREATER_EQUAL);
    }
    break;
  }

  case 'l': {
    if (lexer.cursor - lexer.start == 2) {
      return lexer_check_identifier(1, 1, "t", TOKEN_LESS);
    } else if (lexer.cursor - lexer.start == 3) {
      return lexer_check_identifier(1, 2, "te", TOKEN_LESS_EQUAL);
    }
    break;
  }
  }

  return TOKEN_IDENTIFIER;
}

static Token lexer_identifier() {
  while (char_is_alpha(lexer_peek()))
    lexer_advance();

  const size_t length = (size_t)(lexer.cursor - lexer.start);

  char *lexeme = STRING_NEW(char, length);

  (void)memcpy(lexeme, lexer.start, length);

  lexeme[length] = '\0';

  return token_create(lexer_identifier_type(), lexeme);
}

LexerOutput *lexer_run(const char *source) {
  lexer.line = 1;
  lexer.column = 0;
  lexer.start = lexer.cursor = source;

  LexerOutput *output = NEW(LexerOutput);

  while (!lexer_is_eof()) {
    const char c = lexer_advance();

    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      lexer_commit();
      break;
    case '\n':
      lexer_next_line();
      lexer_commit();
      break;
    case '#':
      /* A comment goes until the end of the line. */
      while (lexer_peek() != '\n' && !lexer_is_eof())
        lexer_advance();
      lexer_commit();
      break;

    case '"':
      lexer_add_token(output, lexer_string());
      break;

    case '+':
      lexer_add_token(output, token_create(TOKEN_PLUS, "+"));
      break;
    case '-':
      lexer_add_token(output, token_create(TOKEN_MINUS, "-"));
      break;
    case '/':
      lexer_add_token(output, token_create(TOKEN_SLASH, "/"));
      break;
    case '*':
      lexer_add_token(output, token_create(TOKEN_STAR, "*"));
      break;
    case ';':
      lexer_add_token(output, token_create(TOKEN_SEMICOLON, ";"));
      break;
    case ',':
      lexer_add_token(output, token_create(TOKEN_COMMA, ","));
      break;
    case ':':
      lexer_add_token(output, token_create(TOKEN_COLON, ":"));
      break;
    case '.':
      lexer_add_token(output, token_create(TOKEN_DOT, "."));
      break;
    case '=':
      lexer_add_token(output, token_create(TOKEN_ASSING, "="));
      break;
    case '?':
      lexer_add_token(output, token_create(TOKEN_QUESTION_MARK, "?"));
      break;
    case '|':
      lexer_add_token(output, token_create(TOKEN_PIPE, "|"));
      break;
    case '(':
      lexer_add_token(output, token_create(TOKEN_LEFT_PAREN, "("));
      break;
    case ')':
      lexer_add_token(output, token_create(TOKEN_RIGHT_PAREN, ")"));
      break;
    case '{':
      lexer_add_token(output, token_create(TOKEN_LEFT_BRACE, "{"));
      break;
    case '}':
      lexer_add_token(output, token_create(TOKEN_RIGHT_BRACE, "}"));
      break;
    case '[':
      lexer_add_token(output, token_create(TOKEN_LEFT_BRACKET, "["));
      break;
    case ']':
      lexer_add_token(output, token_create(TOKEN_RIGHT_BRACKET, "]"));
      break;

    default:
      if (char_is_digit(c)) {
        lexer_add_token(output, lexer_number());
      } else if (char_is_alpha(c)) {
        lexer_add_token(output, lexer_identifier());
      }
      break;
    }
  }

  lexer_add_token(output, token_create(TOKEN_EOF, "EOF"));

  return output;
}
