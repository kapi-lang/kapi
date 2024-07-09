import { TokenType, token_create } from "./token.mjs";
import { is_digit, is_alpha, is_alphanumeric, error_create } from "./tools.mjs";

const ReservedKeywords = Object.freeze({
  eq: TokenType.eq,
  not: TokenType.not,
  and: TokenType.and,
  or: TokenType.or,
  gt: TokenType.gt,
  gte: TokenType.gte,
  lt: TokenType.lt,
  lte: TokenType.lte,
  nil: TokenType.nil,
  true: TokenType.true,
  false: TokenType.false,
  if: TokenType.if,
  else: TokenType.else,
  var: TokenType.variable,
  struct: TokenType.struct,
  print: TokenType.print,
  return: TokenType.return,
});

/** @type {import('./token.mjs').Token[]} */
let tokens = [];
let start = 0, cursor = 0, line = 0, column = 0;
let $source = "";

function lexer_error(msg) {
  return error_create(`[lexer] ${msg}`, { line, column });
}

export function lexer_get_tokens(source) {
  tokens = [], start = 0, cursor = 0, line = 0, column = 0;
  $source = source;

  while (!lexer_at_end()) {
    start = cursor;

    lexer_scan_token();
  }

  tokens.push(token_create(TokenType.eof, { start, end: start, line, column }, '\0'));

  return tokens;
}

function lexer_at_end() {
  return cursor >= $source.length;
}

function lexer_scan_token() {
  const c = lexer_advance();

  switch (c) {
    case ' ':
    case '\r':
    case '\t':
      // Ignore whitespace.
      start++;
      break;
    case '\n':
      line++;
      column = 0;
      break;
    case '"':
      lexer_string();
      break;
    case TokenType.hash:
      // A comment goes until the end of the line.
      while (lexer_peek() !== '\n' && !lexer_at_end()) lexer_advance();
      break;
    case TokenType.left_paren:
      lexer_add_token(TokenType.left_paren);
      break;
    case TokenType.right_paren:
      lexer_add_token(TokenType.right_paren);
      break;
    case TokenType.left_bracket:
      lexer_add_token(TokenType.left_bracket);
      break;
    case TokenType.right_bracket:
      lexer_add_token(TokenType.right_bracket);
      break;
    case TokenType.left_brace:
      lexer_add_token(TokenType.left_brace);
      break;
    case TokenType.right_brace:
      lexer_add_token(TokenType.right_brace);
      break;
    case TokenType.comma:
      lexer_add_token(TokenType.comma);
      break;
    case TokenType.dot:
      lexer_add_token(TokenType.dot);
      break;
    case TokenType.minus:
      lexer_add_token(TokenType.minus);
      break;
    case TokenType.plus:
      lexer_add_token(TokenType.plus);
      break;
    case TokenType.slash:
      lexer_add_token(TokenType.slash);
      break;
    case TokenType.colon:
      lexer_add_token(TokenType.colon);
      break;
    case TokenType.semicolon:
      lexer_add_token(TokenType.semicolon);
      break;
    case TokenType.star:
      lexer_add_token(TokenType.star);
      break;
    case TokenType.pipe:
      lexer_add_token(TokenType.pipe);
      break;
    case TokenType.assign:
      lexer_add_token(TokenType.assign);
      break;
    case TokenType.question_mark:
      lexer_add_token(TokenType.question_mark);
      break;
    default:
      if (is_digit(c)) lexer_number();
      else if (is_alpha(c)) lexer_identifier();
      else throw lexer_error(`unexpected character(${c})`);
      break;
  }
}

function lexer_advance() {
  column++;

  return $source.charAt(cursor++);
}

function lexer_peek(offset = 0) {
  if (cursor + offset >= $source.length) return '\0';

  return $source.charAt(cursor + offset);
}

function lexer_add_token(type, literal) {
  const lexeme = $source.substring(start, cursor);

  tokens.push(token_create(type, { line, column, start, end: cursor }, lexeme, literal));
}

function lexer_string() {
  while (lexer_peek() !== '"' && !lexer_at_end()) {
    if (lexer_peek() === '\n') line++;
    lexer_advance();
  }

  if (lexer_at_end()) {
    throw lexer_error('unterminated string');
  }

  lexer_advance();

  const value = $source.substring(start + 1, cursor - 1);

  lexer_add_token(TokenType.string, value);
}

function lexer_number() {
  while (is_digit(lexer_peek())) lexer_advance();

  if (lexer_peek() === '.' && is_digit(lexer_peek(1))) {
    lexer_advance();

    while (is_digit(lexer_peek())) lexer_advance();
  }

  const value = $source.substring(start, cursor);

  if (value.includes('.')) {
    lexer_add_token(TokenType.float, Number(value));
  } else {
    lexer_add_token(TokenType.integer, Number(value));
  }
}

function lexer_identifier() {
  while (is_alphanumeric(lexer_peek())) lexer_advance();

  const value = $source.substring(start, cursor);

  const type = ReservedKeywords[value] || TokenType.identifier;

  if (type === ReservedKeywords.true) {
    lexer_add_token(type, true);
  } else if (type === ReservedKeywords.false) {
    lexer_add_token(type, false);
  } else if (type === ReservedKeywords.nil) {
    lexer_add_token(type, null);
  } else {
    lexer_add_token(type);
  }
}
