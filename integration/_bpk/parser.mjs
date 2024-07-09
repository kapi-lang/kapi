import { TokenType } from "./token.mjs";
import { error_create } from "./tools.mjs";
import { expression_create } from "./expression.mjs";
import { declaration_create } from "./declaration.mjs";

/**
 * @typedef {import("./declaration.mjs").Declaration | import("./expression.mjs").Expression} Statement
 */

let types = [];

/** @type {Statement[]} */
let statements = [];
let cursor = 0;

/** @type {import('./token.mjs').Token[]} */
let $tokens = [];

function parser_error(msg) {
  const token = parser_peek();

  return error_create(`[parser] ${msg}`, token);
}

export function parser_ast_from(tokens) {
  types = [], statements = [], cursor = 0;
  $tokens = tokens;

  while (!parser_at_end()) {
    switch (parser_peek().type) {

      case TokenType.struct:
        parser_advance();
        types.push(parser_struct());
        break;

      case TokenType.variable:
        parser_advance();
        statements.push(parser_variable());
        break;

      case TokenType.print:
        parser_advance();
        statements.push(parser_print());
        break;

      default:
        statements.push(parser_expression_statement());
        break;
    }
  }

  return { types, statements };
}

function parser_at_end() {
  return parser_peek().type === TokenType.eof;
}

function parser_peek(offset = 0) {
  return $tokens[cursor + offset];
}

function parser_advance() {
  return $tokens[cursor++];
}

function parser_consume(type, msg) {
  if (parser_check(type)) return parser_advance();

  throw parser_error(msg);
}

function parser_check(type, offset) {
  return !parser_at_end() && parser_peek(offset).type === type;
}

/**
 * @return {Statement}
 */
function parser_print() {
  const name = parser_peek(-1);

  parser_consume(TokenType.left_paren, "expect '(' before call.");

  const args = [];

  do {
    if (args.length >= 255) {
      throw parser_error('cannot have more than 255 arguments.');
    }

    args.push(parser_expression());
  } while (parser_check(TokenType.comma) && parser_advance());

  parser_consume(TokenType.right_paren, "expect ')' after call.");

  parser_consume(TokenType.semicolon, "expect ';' after call.");

  return expression_create('CallExpression', name, args);
}

/**
 * @return {Statement}
 */
function parser_variable() {
  const name = parser_consume(TokenType.identifier, "expect variable name");

  if (!name) {
    throw parser_error("invalid variable name");
  }

  parser_consume(TokenType.assign, "expect '=' after variable name");

  return declaration_create(
    'VariableDeclaration',
    name,
    parser_expression_statement(),
  );
}

/**
 * @return {Statement}
 */
function parser_expression_statement() {
  const expression = parser_expression();

  parser_consume(TokenType.semicolon, "expect ';' after value");

  return expression;
}

/**
 * @return {Statement}
 */
function parser_expression() {
  return parser_assignment();
}

/**
 * @return {Statement}
 */
function parser_assignment() {
  const expression = parser_or();

  if (parser_check(TokenType.assign)) {
    const operator = parser_advance();
    const right = parser_assignment();

    if (expression.type === 'VariableExpression') {
      return expression_create('AssignmentExpression', expression.name, operator, right);
    }

    throw parser_error("invalid assignment target (op: " + operator.lexeme + ")");
  }

  return expression;
}

/**
 * @return {Statement}
 */
function parser_or() {
  let expression = parser_and();

  while (parser_check(TokenType.or)) {
    parser_advance();
    const operator = parser_peek(-1);
    const right = parser_and();

    expression = expression_create('BinaryExpression', expression, operator, right);
  }

  return expression;
}

/**
 * @return {Statement}
 */
function parser_and() {
  let expression = parser_equality();

  while (parser_check(TokenType.and)) {
    parser_advance();
    const operator = parser_peek(-1);
    const right = parser_equality();

    expression = expression_create('BinaryExpression', expression, operator, right);
  }

  return expression;
}

/**
 * @return {Statement}
 */
function parser_equality() {
  let expression = parser_comparison();

  while (parser_check(TokenType.not) || parser_check(TokenType.eq)) {
    parser_advance();
    const operator = parser_peek(-1);
    const right = parser_comparison();

    expression = expression_create('BinaryExpression', expression, operator, right);
  }

  return expression;
}

/**
 * @return {Statement}
 */
function parser_comparison() {
  let expression = parser_term();

  while (parser_check(TokenType.gt) || parser_check(TokenType.gte) || parser_check(TokenType.lt) || parser_check(TokenType.lte)) {
    parser_advance();
    const operator = parser_peek(-1);
    const right = parser_term();

    expression = expression_create('BinaryExpression', expression, operator, right);
  }

  return expression;
}

/**
 * @return {Statement}
 */
function parser_term() {
  let expression = parser_factor();

  while (parser_check(TokenType.minus) || parser_check(TokenType.plus)) {
    parser_advance();
    const operator = parser_peek(-1);
    const right = parser_factor();

    expression = expression_create('BinaryExpression', expression, operator, right);
  }

  return expression;
}

/**
 * @return {Statement}
 */
function parser_factor() {
  let expression = parser_unary();

  while (parser_check(TokenType.slash) || parser_check(TokenType.star)) {
    parser_advance();
    const operator = parser_peek(-1);
    const right = parser_unary();

    expression = expression_create('BinaryExpression', expression, operator, right);
  }

  return expression;
}

/**
 * @return {Statement}
 */
function parser_unary() {
  while (parser_check(TokenType.minus)) {
    parser_advance();
    const operator = parser_peek(-1);
    const right = parser_unary();

    return expression_create('UnaryExpression', operator, right);
  }

  return parser_call();
}

/**
 * @return {Statement}
 */
function parser_call() {
  const expression = parser_primary();


  if (parser_check(TokenType.left_paren)) {
    parser_advance();

    const args = [];

    do {
      if (args.length >= 255) {
        throw parser_error('cannot have more than 255 arguments.');
      }

      args.push(parser_expression());
    } while (parser_check(TokenType.comma) && parser_advance());

    parser_consume(TokenType.right_paren, "expect ')' after call.");

    return expression_create(
      'CallExpression',
      /** @type {import("./expression.mjs").VariableExpression} */(expression).name,
      args
    );
  }

  return expression;
}

/**
 * @return {Statement}
 */
function parser_primary() {
  switch (parser_peek().type) {
    case TokenType.integer: {
      const value = parser_consume(TokenType.integer, "expect 'integer' after variable assing");
      return expression_create('LiteralExpression', TokenType.integer, value);
    }

    case TokenType.float: {
      const value = parser_consume(TokenType.float, "expect 'float' after variable assing");
      return expression_create('LiteralExpression', TokenType.float, value);
    }


    case TokenType.true: {
      const value = parser_consume(TokenType.true, "expect 'true' after variable assing");
      return expression_create('LiteralExpression', TokenType.bool, value);
    }

    case TokenType.false: {
      const value = parser_consume(TokenType.false, "expect 'false' after variable assing");
      return expression_create('LiteralExpression', TokenType.bool, value);
    }

    case TokenType.string: {
      const value = parser_consume(TokenType.string, "expect 'string' after variable assing");
      return expression_create('LiteralExpression', TokenType.string, value);
    }

    case TokenType.nil: {
      const value = parser_consume(TokenType.nil, "expect 'nil' after variable assing");
      return expression_create('LiteralExpression', TokenType.nil, value);
    }

    case TokenType.identifier: {
      const name = parser_consume(TokenType.identifier, "expect 'identifier' before variable assing");

      if (parser_check(TokenType.left_brace)) {
        parser_advance();

        const properties = [];

        do {
          const key = parser_consume(TokenType.identifier, "expect 'identifier' after struct initialization");
          parser_consume(TokenType.assign, "expect '=' after struct key");
          const value = parser_expression();
          parser_consume(TokenType.comma, "expect ',' after struct value");

          properties.push({ key, value });

        } while (!parser_check(TokenType.right_brace));


        parser_consume(TokenType.right_brace, "expect '}' after struct initialization");

        return expression_create('ObjectExpression', name, properties);
      }

      return expression_create('VariableExpression', name);
    }

    case TokenType.left_paren: {
      parser_consume(TokenType.left_paren, "expect '(' after function call");

      const expression = parser_expression();

      parser_consume(TokenType.right_paren, "expect ')' after function call");

      return expression_create('GroupExpression', expression);
    }

  }

  throw parser_error('invalid primary expression type (' + parser_peek().type + ')');
}

function parser_struct() {
  const name = parser_consume(TokenType.identifier, "expect struct name");

  const struct = {
    kind: 'StructType',
    name,
    /** @type {{name: import("./token.mjs").Token, type: string; optional: boolean;}[]} */
    attributes: [],
  };

  parser_consume(TokenType.left_brace, "expect '{' after struct name");

  do {
    const name = parser_consume(TokenType.identifier, "expect attribute name");

    let optional = false;
    if (parser_check(TokenType.question_mark)) {
      optional = true;
      parser_advance();
    }

    parser_consume(TokenType.colon, "expect ':' after attribute name.");

    let type = parser_consume(TokenType.identifier, "expect attribute value").lexeme;
    type = TokenType[type] || type;

    parser_consume(TokenType.semicolon, "expect ';' after attribute declaration.");

    struct.attributes.push({
      name,
      type,
      optional,
    });

  } while (!parser_check(TokenType.right_brace));

  parser_consume(TokenType.right_brace, "expect '}' after struct declaration");

  return struct;
}
