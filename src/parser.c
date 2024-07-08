#include <string.h>

#include "lexer.h"
#include "macros.h"
#include "parser.h"
#include "token.h"

typedef struct ParserState {
  Token *start;
  Token *cursor;
} ParserState;

ParserState parser;

static void parser_add_statement(ParserOutput *output, Statement *statement) {
  const size_t index = output->length++;

  output->body = ARRAY_RESIZE(ProgramBody, output->body, output->length);

  output->body[index] = (ProgramBody){
      .type = PROGRAM_BODY_STATEMENT,
      .as.statement = statement,
  };
}

static void parser_add_declaration(ParserOutput *output,
                                   Declaration *declaration) {
  const size_t index = output->length++;

  output->body = ARRAY_RESIZE(ProgramBody, output->body, output->length);

  output->body[index] = (ProgramBody){
      .type = PROGRAM_BODY_DECLARATION,
      .as.declaration = declaration,
  };
}

static bool parser_is_eof() { return (*parser.cursor).type == TOKEN_EOF; }

static Token parser_peek() { return *parser.cursor; }

static Token parser_previous() { return parser.cursor[-1]; }

static Token parser_next() { return parser.cursor[1]; }

static Token parser_advance() {
  parser.cursor++;

  return parser.cursor[-1];
}

static bool parser_check(TokenType type) {
  return !parser_is_eof() && parser_peek().type == type;
}

static Token parser_consume(TokenType type, const char *message) {
  if (parser_check(type))
    return parser_advance();

  const Token token = parser_peek();

  EXCEPTION("[parser] %s | found: (%d) %s\n", message, token.type,
            token.lexeme);
}

static Expression *parser_expression();

static Expression *parser_primary() {
  const Token token = parser_peek();

  switch (token.type) {
  case TOKEN_NIL: {
    Expression *expr = NEW(Expression);
    LiteralExpression *literal_expr = NEW(LiteralExpression);

    literal_expr->type = TOKEN_NIL;

    expr->type = EXPRESSION_LITERAL;
    expr->as.literal = literal_expr;

    (void)parser_advance();

    return expr;
  }

  case TOKEN_INTEGER: {
    Expression *expr = NEW(Expression);
    LiteralExpression *literal_expr = NEW(LiteralExpression);

    literal_expr->type = TOKEN_INTEGER;
    literal_expr->as.number = strtod(token.lexeme, NULL);

    expr->type = EXPRESSION_LITERAL;
    expr->as.literal = literal_expr;

    (void)parser_advance();

    return expr;
  }

  case TOKEN_FLOAT: {
    Expression *expr = NEW(Expression);
    LiteralExpression *literal_expr = NEW(LiteralExpression);

    literal_expr->type = TOKEN_FLOAT;
    literal_expr->as.number = strtod(token.lexeme, NULL);

    expr->type = EXPRESSION_LITERAL;
    expr->as.literal = literal_expr;

    (void)parser_advance();

    return expr;
  }

  case TOKEN_TRUE:
  case TOKEN_FALSE: {
    Expression *expr = NEW(Expression);
    LiteralExpression *literal_expr = NEW(LiteralExpression);

    literal_expr->type = TOKEN_BOOLEAN;
    literal_expr->as.boolean =
        memcmp(token.lexeme, "true", 4) == 0 ? true : false;

    expr->type = EXPRESSION_LITERAL;
    expr->as.literal = literal_expr;

    (void)parser_advance();

    return expr;
  }

  case TOKEN_STRING: {
    Expression *expr = NEW(Expression);
    LiteralExpression *literal_expr = NEW(LiteralExpression);

    literal_expr->type = TOKEN_STRING;

    expr->type = EXPRESSION_LITERAL;
    expr->as.literal = literal_expr;
    literal_expr->as.string = token.lexeme;

    (void)parser_advance();

    return expr;
  }

  case TOKEN_LEFT_PAREN: {
    Expression *expr = NEW(Expression);
    GroupExpression *group_expr = NEW(GroupExpression);

    (void)parser_consume(TOKEN_LEFT_PAREN,
                         "expect '(' before group expression");

    group_expr->expression = parser_expression();

    (void)parser_consume(TOKEN_RIGHT_PAREN,
                         "expect ')' after group expression");

    expr->type = EXPRESSION_GROUP;
    expr->as.group = group_expr;

    return expr;
  }

  case TOKEN_IDENTIFIER: {
    if (parser_next().type == TOKEN_ASSING) {
      const Token name = parser_consume(
          TOKEN_IDENTIFIER, "expect 'identifier' before assign a variable");

      (void)parser_consume(TOKEN_ASSING, "expect '=' after a variable");

      Expression *expr = NEW(Expression);
      AssignmentExpression *assign_expr = NEW(AssignmentExpression);

      assign_expr->left = name;
      assign_expr->right = parser_expression();

      expr->type = EXPRESSION_ASSIGNMENT;
      expr->as.assignment = assign_expr;

      return expr;
    }
  }

  default:
    break;
  }

  EXCEPTION("[parser] expression type not supported (%d = '%s')\n", token.type,
            token.lexeme);
}

static Expression *parser_call() {
  Expression *expr = parser_primary();

  // if (parser_check(TOKEN_LEFT_PAREN)) {
  // }

  return expr;
}

static Expression *parser_unary() {
  while (parser_check(TOKEN_MINUS)) {
    const Token operator= parser_advance();
    Expression *right = parser_unary();

    UnaryExpression *unary_expr = NEW(UnaryExpression);
    unary_expr->operator= operator;
    unary_expr->right = right;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_UNARY;
    next_expr->as.unary = unary_expr;

    return next_expr;
  }

  return parser_call();
}

static Expression *parser_factor() {
  Expression *expr = parser_unary();

  while (parser_check(TOKEN_SLASH) || parser_check(TOKEN_STAR)) {
    const Token operator= parser_advance();
    Expression *right = parser_unary();

    BinaryExpression *bin_expr = NEW(BinaryExpression);
    bin_expr->left = expr;
    bin_expr->operator= operator;
    bin_expr->right = right;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_BINARY;
    next_expr->as.binary = bin_expr;

    return next_expr;
  }

  return expr;
}

static Expression *parser_term() {
  Expression *expr = parser_factor();

  while (parser_check(TOKEN_MINUS) || parser_check(TOKEN_PLUS)) {
    const Token operator= parser_advance();
    Expression *right = parser_factor();

    BinaryExpression *bin_expr = NEW(BinaryExpression);
    bin_expr->left = expr;
    bin_expr->operator= operator;
    bin_expr->right = right;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_BINARY;
    next_expr->as.binary = bin_expr;

    return next_expr;
  }

  return expr;
}

static Expression *parser_comparison() {
  Expression *expr = parser_term();

  while (parser_check(TOKEN_GREATER) || parser_check(TOKEN_GREATER_EQUAL) ||
         parser_check(TOKEN_LESS) || parser_check(TOKEN_LESS_EQUAL)) {
    const Token operator= parser_advance();
    Expression *right = parser_term();

    BinaryExpression *bin_expr = NEW(BinaryExpression);
    bin_expr->left = expr;
    bin_expr->operator= operator;
    bin_expr->right = right;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_BINARY;
    next_expr->as.binary = bin_expr;

    return next_expr;
  }

  return expr;
}

static Expression *parser_equality() {
  Expression *expr = parser_comparison();

  while (parser_check(TOKEN_NOT) || parser_check(TOKEN_EQUAL)) {

    const Token operator= parser_advance();
    Expression *right = parser_comparison();

    BinaryExpression *bin_expr = NEW(BinaryExpression);
    bin_expr->left = expr;
    bin_expr->operator= operator;
    bin_expr->right = right;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_BINARY;
    next_expr->as.binary = bin_expr;

    return next_expr;
  }

  return expr;
}

static Expression *parser_and() {
  Expression *expr = parser_equality();

  while (parser_check(TOKEN_AND)) {
    const Token operator= parser_advance();
    Expression *right = parser_equality();

    BinaryExpression *bin_expr = NEW(BinaryExpression);
    bin_expr->left = expr;
    bin_expr->operator= operator;
    bin_expr->right = right;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_BINARY;
    next_expr->as.binary = bin_expr;

    return next_expr;
  }

  return expr;
}

static Expression *parser_or() {
  Expression *expr = parser_and();

  while (parser_check(TOKEN_OR)) {
    const Token operator= parser_advance();
    Expression *right = parser_and();

    BinaryExpression *bin_expr = NEW(BinaryExpression);
    bin_expr->left = expr;
    bin_expr->operator= operator;
    bin_expr->right = right;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_BINARY;
    next_expr->as.binary = bin_expr;

    return next_expr;
  }

  return expr;
}

static Expression *parser_assignment() {
  Expression *expr = parser_or();

  // if(parser_check(TOKEN_ASSING)) {
  //   const Token operator = parser_advance();
  //   Expression * right = parser_assignment();
  // }

  return expr;
}

static Expression *parser_expression() { return parser_assignment(); }

static Statement *parser_expression_statement() {
  Statement *stmt = NEW(Statement);
  stmt->type = STATEMENT_EXPRESSION;
  stmt->as.expression = NEW(ExpressionStatement);
  stmt->as.expression->expression = parser_expression();

  parser_consume(TOKEN_SEMICOLON, "expect ';' after expression");

  return stmt;
}

static Statement *parser_print() {
  const Token name = parser_advance();

  (void)parser_consume(TOKEN_LEFT_PAREN, "expect '(' before call.");

  Statement *stmt = NEW(Statement);
  stmt->type = STATEMENT_EXPRESSION;
  stmt->as.expression = NEW(ExpressionStatement);

  Expression *expr = NEW(Expression);
  expr->type = EXPRESSION_CALL;
  stmt->as.expression->expression = expr;

  CallExpression *call_expr = NEW(CallExpression);
  expr->as.call = call_expr;

  call_expr->callee = name;
  call_expr->arguments = NULL;
  call_expr->length = 0;

  do {
    if (parser_check(TOKEN_COMMA))
      (void)parser_advance();

    const size_t index = call_expr->length++;

    call_expr->arguments =
        ARRAY_RESIZE(Expression *, call_expr->arguments, call_expr->length);

    call_expr->arguments[index] = parser_expression();

  } while (parser_check(TOKEN_COMMA));

  (void)parser_consume(TOKEN_RIGHT_PAREN, "expect ')' after call.");
  (void)parser_consume(TOKEN_SEMICOLON, "expect ';' after expression");

  return stmt;
}

static Declaration *parser_var() {
  (void)parser_consume(TOKEN_VAR, "expect 'var' before variable declaration");

  const Token name = parser_consume(TOKEN_IDENTIFIER,
                                    "expect 'identifier' after 'var' keyword");

  Declaration *decl = NEW(Declaration);
  decl->type = DECLARATION_VARIABLE;
  decl->as.variable = NEW(VariableDeclaration);
  decl->as.variable->id = name;
  decl->as.variable->init = NULL;

  if (parser_check(TOKEN_ASSING)) {
    (void)parser_consume(TOKEN_ASSING, "expect '=' after variable declaration");

    decl->as.variable->init = parser_expression();
  }

  (void)parser_consume(TOKEN_SEMICOLON,
                       "expect ';' after variable declaration");

  return decl;
}

ParserOutput *parser_run(LexerOutput *tokens) {
  ParserOutput *output = NEW(ParserOutput);

  output->body = NULL;
  output->length = 0;

  parser.start = parser.cursor = tokens->tokens;

  while (!parser_is_eof()) {
    const Token token = parser_peek();

    switch (token.type) {
    case TOKEN_STRUCT:
      break;

    case TOKEN_VAR:
      parser_add_declaration(output, parser_var());
      break;

    case TOKEN_PRINT:
      parser_add_statement(output, parser_print());
      break;

    default:
      parser_add_statement(output, parser_expression_statement());
      break;
    }
  }

  return output;
}
