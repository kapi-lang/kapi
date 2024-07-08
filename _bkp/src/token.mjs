/**
 * @typedef {{ type: string; lexeme: string; literal: string | number | boolean | null; line: number; column: number; start: number; end: number; }} Token
 */

export const TokenType = Object.freeze({
  // Single-character tokens.
  left_paren: '(',
  right_paren: ')',
  left_bracket: '[',
  right_bracket: ']',
  left_brace: '{',
  right_brace: '}',
  comma: ',',
  dot: '.',
  minus: '-',
  plus: '+',
  colon: ':',
  semicolon: ';',
  slash: '/',
  star: '*',
  pipe: '|',
  assign: '=',
  question_mark: '?',
  hash: '#',

  // Literals
  identifier: 'identifier',
  string: 'string',
  integer: 'integer',
  float: 'float',
  bool: 'bool',
  void: 'void',

  // Keywords.
  nil: 'nil',
  not: 'not',
  and: 'and',
  or: 'or',
  eq: 'eq',
  gt: 'gt',
  gte: 'gte',
  lt: 'lt',
  lte: 'lte',
  true: 'true',
  false: 'false',
  variable: 'var',
  if: 'if',
  else: 'else',
  print: 'print',
  return: 'return',
  struct: 'struct',

  eof: 'eof',
});

/**
 * @returns {Token}
 */
export function token_create(type, { start, end, line, column }, lexeme = '', literal = null) {
  return {
    type,
    lexeme,
    literal,
    line,
    column,
    start,
    end,
  };
}
