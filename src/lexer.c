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

static void lexer_add_token(LexerOutput *output, Token token) {
  const size_t index = output->length++;

  output->tokens = ARRAY_RESIZE(Token, output->tokens, output->length);

  output->tokens[index] = token;

  lexer_commit();

  LOG_DEBUG("[lexer::add_token] (%d) %s\n", token.type, token.lexeme);
}

static Token lexer_string() {
  (void)lexer_commit();

  while (lexer_peek() != '"' && !lexer_is_eof()) {
    if (lexer_peek() == '\n') {
      lexer.line++;
    }

    lexer_advance();
  }

  if (lexer_is_eof()) {
    EXCEPTION("unterminated string\n");
  }

  const size_t length = (size_t)(lexer.cursor - lexer.start);

  /* The closing quote. */
  (void)lexer_advance();

  char *lexeme = STRING_NEW(char, length);

  (void)memcpy(lexeme, lexer.start, length);

  lexeme[length] = '\0';

  return token_create(TOKEN_STRING, lexeme, lexer.line, lexer.column);
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

  return token_create(type, lexeme, lexer.line, lexer.column);
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
  case 'N':
    return lexer_check_identifier(1, 5, "umber", TOKEN_TYPE_NUMBER);

  case 'V':
    return lexer_check_identifier(1, 3, "oid", TOKEN_TYPE_VOID);

  case 'I':
    return lexer_check_identifier(1, 6, "nteger", TOKEN_TYPE_INTEGER);

  case 'F':
    return lexer_check_identifier(1, 4, "loat", TOKEN_TYPE_FLOAT);

  case 'B':
    return lexer_check_identifier(1, 6, "oolean", TOKEN_TYPE_BOOLEAN);

  case 'S':
    return lexer_check_identifier(1, 5, "tring", TOKEN_TYPE_STRING);

  case 'a':
    return lexer_check_identifier(1, 2, "nd", TOKEN_AND);

  case 'b':
    return lexer_check_identifier(1, 4, "reak", TOKEN_BREAK);

  case 'c':
    return lexer_check_identifier(1, 7, "ontinue", TOKEN_CONTINUE);

  case 'r':
    return lexer_check_identifier(1, 5, "eturn", TOKEN_RETURN);

  case 'o': {
    if (lexer.cursor - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'f':
        return lexer_check_identifier(1, 1, "f", TOKEN_OF);
      case 'r':
        return lexer_check_identifier(1, 1, "r", TOKEN_OR);
      }
    }
    break;
  }

  case 'e': {
    if (lexer.cursor - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'q':
        return lexer_check_identifier(1, 1, "q", TOKEN_EQUAL);
      case 'l':
        return lexer_check_identifier(1, 3, "lse", TOKEN_ELSE);
      }
    }
    break;
  }

  case 't': {
    if (lexer.cursor - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'o':
        return lexer_check_identifier(1, 1, "o", TOKEN_TO);
      case 'r':
        return lexer_check_identifier(1, 3, "rue", TOKEN_TRUE);
      }
    }
    break;
  }

  case 'i':
    return lexer_check_identifier(1, 1, "f", TOKEN_IF);

  case 'f': {
    if (lexer.cursor - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'a':
        return lexer_check_identifier(1, 4, "alse", TOKEN_FALSE);
      case 'o':
        return lexer_check_identifier(1, 2, "or", TOKEN_FOR);
      case 'u':
        return lexer_check_identifier(1, 7, "unction", TOKEN_FUNCTION);
      }
    }
    break;
  }

  case 'v':
    return lexer_check_identifier(1, 2, "ar", TOKEN_VAR);

  case 'p':
    return lexer_check_identifier(1, 4, "rint", TOKEN_PRINT);

  case 's':
    return lexer_check_identifier(1, 5, "truct", TOKEN_STRUCT);

  case 'n': {
    if (lexer.cursor - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'i':
        return lexer_check_identifier(1, 2, "il", TOKEN_NIL);
      case 'o':
        return lexer_check_identifier(1, 2, "ot", TOKEN_NOT);
      }
    }
    break;
  }

  case 'g': {
    if (lexer.cursor - lexer.start == 3) {
      return lexer_check_identifier(1, 2, "te", TOKEN_GREATER_EQUAL);
    } else if (lexer.cursor - lexer.start == 2) {
      return lexer_check_identifier(1, 1, "t", TOKEN_GREATER);
    }
    break;
  }

  case 'l': {
    if (lexer.cursor - lexer.start == 3) {
      return lexer_check_identifier(1, 2, "te", TOKEN_LESS_EQUAL);
    } else if (lexer.cursor - lexer.start == 2) {
      return lexer_check_identifier(1, 1, "t", TOKEN_LESS);
    }
    break;
  }

  case 'w':
    return lexer_check_identifier(1, 4, "hile", TOKEN_WHILE);

  case 'd':
    return lexer_check_identifier(1, 5, "ownto", TOKEN_DOWNTO);
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

  return token_create(lexer_identifier_type(), lexeme, lexer.line,
                      lexer.column);
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
        (void)lexer_advance();
      lexer_commit();
      break;

    case '"':
      lexer_add_token(output, lexer_string());
      break;

    case '~':
      lexer_add_token(output, token_create(TOKEN_BITWISE_NOT, "~", lexer.line,
                                           lexer.column));
      break;

    case '%':
      switch (lexer_peek()) {
      case '=': {
        (void)lexer_advance();
        lexer_add_token(output, token_create(TOKEN_ASSIGN_MODULUS,
                                             "%=", lexer.line, lexer.column));
        break;
      }

      default:

        lexer_add_token(
            output, token_create(TOKEN_MODULUS, "%", lexer.line, lexer.column));
        break;
      }
      break;

    case '&':
      switch (lexer_peek()) {
      case '=': {
        (void)lexer_advance();
        lexer_add_token(output, token_create(TOKEN_ASSIGN_BITWISE_AND,
                                             "&=", lexer.line, lexer.column));
        break;
      }

      default:
        lexer_add_token(output, token_create(TOKEN_BITWISE_AND, "&", lexer.line,
                                             lexer.column));
        break;
      }
      break;

    case '|':
      switch (lexer_peek()) {
      case '=': {
        (void)lexer_advance();
        lexer_add_token(output, token_create(TOKEN_ASSIGN_BITWISE_OR,
                                             "|=", lexer.line, lexer.column));
        break;
      }

      default:
        lexer_add_token(output, token_create(TOKEN_BITWISE_OR, "|", lexer.line,
                                             lexer.column));
        break;
      }
      break;

    case '^':
      switch (lexer_peek()) {
      case '=': {
        (void)lexer_advance();
        lexer_add_token(output, token_create(TOKEN_ASSIGN_BITWISE_XOR,
                                             "^=", lexer.line, lexer.column));
        break;
      }

      default:
        lexer_add_token(output, token_create(TOKEN_BITWISE_XOR, "^", lexer.line,
                                             lexer.column));
        break;
      }
      break;

    case '<': {
      switch (lexer_peek()) {
      case '<':
        (void)lexer_advance();

        switch (lexer_peek()) {
        case '=':
          (void)lexer_advance();
          lexer_add_token(output,
                          token_create(TOKEN_ASSIGN_BITWISE_LEFT_SHIFT,
                                       "<<=", lexer.line, lexer.column));
          break;

        default:
          lexer_add_token(output, token_create(TOKEN_BITWISE_LEFT_SHIFT, "<<",
                                               lexer.line, lexer.column));
          break;
        }
        break;
      }
      break;
    }

    case '>': {
      switch (lexer_peek()) {
      case '>':
        (void)lexer_advance();

        switch (lexer_peek()) {
        case '=': {
          (void)lexer_advance();
          lexer_add_token(output,
                          token_create(TOKEN_ASSIGN_BITWISE_RIGHT_SHIFT,
                                       ">>=", lexer.line, lexer.column));

          break;
        }

        case '>': {
          (void)lexer_advance();
          switch (lexer_peek()) {
          case '=':
            (void)lexer_advance();
            lexer_add_token(
                output, token_create(TOKEN_ASSIGN_BITWISE_UNSIGNED_RIGHT_SHIFT,
                                     ">>>=", lexer.line, lexer.column));
            break;
          default:
            lexer_add_token(output,
                            token_create(TOKEN_BITWISE_UNSIGNED_RIGHT_SHIFT,
                                         ">>>", lexer.line, lexer.column));
            break;
          }
          break;
        }

        default: {
          lexer_add_token(output, token_create(TOKEN_BITWISE_RIGHT_SHIFT, ">>",
                                               lexer.line, lexer.column));
          break;
        }
        }
        break;
      }
      break;
    }

    case '+': {
      switch (lexer_peek()) {
      case '+': {
        (void)lexer_advance();
        lexer_add_token(output, token_create(TOKEN_INCREMENT, "++", lexer.line,
                                             lexer.column));
        break;
      }

      case '=': {
        (void)lexer_advance();
        lexer_add_token(output, token_create(TOKEN_ASSIGN_INCREMENT,
                                             "+=", lexer.line, lexer.column));
        break;
      }

      default:
        lexer_add_token(
            output, token_create(TOKEN_PLUS, "+", lexer.line, lexer.column));
        break;
      }
      break;
    }

    case '-': {
      switch (lexer_peek()) {
      case '-': {
        (void)lexer_advance();
        lexer_add_token(output, token_create(TOKEN_DECREMENT, "-", lexer.line,
                                             lexer.column));
        break;
      }

      case '=': {
        (void)lexer_advance();
        lexer_add_token(output, token_create(TOKEN_ASSIGN_DECREMENT,
                                             "-=", lexer.line, lexer.column));
        break;
      }

      default:
        lexer_add_token(
            output, token_create(TOKEN_MINUS, "-", lexer.line, lexer.column));
        break;
      }
      break;
    }

    case '/': {
      switch (lexer_peek()) {
      case '=': {
        (void)lexer_advance();
        lexer_add_token(output, token_create(TOKEN_ASSIGN_DIVIDE,
                                             "/=", lexer.line, lexer.column));
        break;
      }

      default:
        lexer_add_token(
            output, token_create(TOKEN_SLASH, "/", lexer.line, lexer.column));
        break;
      }
      break;
    }

    case '*': {
      switch (lexer_peek()) {
      case '=': {
        (void)lexer_advance();
        lexer_add_token(output, token_create(TOKEN_ASSIGN_MULTIPLY,
                                             "*=", lexer.line, lexer.column));
        break;
      }

      case '*': {
        (void)lexer_advance();
        switch (lexer_peek()) {
        case '=':
          (void)lexer_advance();
          lexer_add_token(output,
                          token_create(TOKEN_ASSIGN_EXPONENTIATION,
                                       "**=", lexer.line, lexer.column));
          break;
        default:
          lexer_add_token(output, token_create(TOKEN_EXPONENTIATION, "**",
                                               lexer.line, lexer.column));
          break;
        }
        break;
      }

      default:
        lexer_add_token(
            output, token_create(TOKEN_STAR, "*", lexer.line, lexer.column));
        break;
      }
      break;
    }

    case ';':
      lexer_add_token(
          output, token_create(TOKEN_SEMICOLON, ";", lexer.line, lexer.column));
      break;

    case ',':
      lexer_add_token(output,
                      token_create(TOKEN_COMMA, ",", lexer.line, lexer.column));
      break;

    case ':':
      lexer_add_token(output,
                      token_create(TOKEN_COLON, ":", lexer.line, lexer.column));
      break;

    case '.':
      lexer_add_token(output,
                      token_create(TOKEN_DOT, ".", lexer.line, lexer.column));
      break;

    case '=':
      lexer_add_token(
          output, token_create(TOKEN_ASSIGN, "=", lexer.line, lexer.column));
      break;

    case '?':
      lexer_add_token(output, token_create(TOKEN_QUESTION_MARK, "?", lexer.line,
                                           lexer.column));
      break;

    case '(':
      lexer_add_token(output, token_create(TOKEN_LEFT_PAREN, "(", lexer.line,
                                           lexer.column));
      break;

    case ')':
      lexer_add_token(output, token_create(TOKEN_RIGHT_PAREN, ")", lexer.line,
                                           lexer.column));
      break;

    case '{':
      lexer_add_token(output, token_create(TOKEN_LEFT_BRACE, "{", lexer.line,
                                           lexer.column));
      break;

    case '}':
      lexer_add_token(output, token_create(TOKEN_RIGHT_BRACE, "}", lexer.line,
                                           lexer.column));
      break;

    case '[':
      lexer_add_token(output, token_create(TOKEN_LEFT_BRACKET, "[", lexer.line,
                                           lexer.column));
      break;

    case ']':
      lexer_add_token(output, token_create(TOKEN_RIGHT_BRACKET, "]", lexer.line,
                                           lexer.column));
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

  lexer_add_token(output,
                  token_create(TOKEN_EOF, "EOF", lexer.line, lexer.column));

  return output;
}
