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

  output->body = ARRAY_RESIZE(Statement *, output->body, output->length);

  output->body[index] = statement;
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

  EXCEPTION("[parser] %s | found: (%d) %s \n", message, token.type,
            token.lexeme);
}

static Expression *parser_expression();

static Expression *parser_primary() {
  const Token token = parser_peek();

  switch (token.type) {
  case TOKEN_NIL: {
    Expression *expr = NEW(Expression);
    Literal *literal_expr = NEW(Literal);

    literal_expr->type = TOKEN_NIL;

    expr->type = EXPRESSION_LITERAL;
    expr->as.literal = literal_expr;

    (void)parser_advance();

    return expr;
  }

  case TOKEN_INTEGER: {
    Expression *expr = NEW(Expression);
    Literal *literal_expr = NEW(Literal);

    literal_expr->type = TOKEN_INTEGER;
    literal_expr->as.number = strtod(token.lexeme, NULL);

    expr->type = EXPRESSION_LITERAL;
    expr->as.literal = literal_expr;

    (void)parser_advance();

    return expr;
  }

  case TOKEN_FLOAT: {
    Expression *expr = NEW(Expression);
    Literal *literal_expr = NEW(Literal);

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
    Literal *literal_expr = NEW(Literal);

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
    Literal *literal_expr = NEW(Literal);

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
    Expression *expr = NEW(Expression);
    Identifier *identifier_expr = NEW(Identifier);

    expr->type = EXPRESSION_IDENTIFIER;
    expr->as.identifier = identifier_expr;
    identifier_expr->name = token.lexeme;

    (void)parser_advance();

    return expr;
  }

  default:
    break;
  }

  EXCEPTION("[parser] expression type not supported (%d = '%s')\n", token.type,
            token.lexeme);
}

static Expression *parser_call() {
  Expression *expr = parser_primary();

  if (expr->type == EXPRESSION_IDENTIFIER) {
    if (parser_check(TOKEN_PLUS) && parser_next().type == TOKEN_PLUS) {
      (void)parser_advance();
      (void)parser_advance();

      UpdateExpression *update_expr = NEW(UpdateExpression);
      update_expr->operator= "++";
      update_expr->prefix = false;
      update_expr->argument = expr;

      Expression *next_expr = NEW(Expression);
      next_expr->type = EXPRESSION_UPDATE;
      next_expr->as.update = update_expr;

      return next_expr;
    } else if (parser_check(TOKEN_MINUS) && parser_next().type == TOKEN_MINUS) {
      (void)parser_advance();
      (void)parser_advance();

      UpdateExpression *update_expr = NEW(UpdateExpression);
      update_expr->operator= "--";
      update_expr->prefix = false;
      update_expr->argument = expr;

      Expression *next_expr = NEW(Expression);
      next_expr->type = EXPRESSION_UPDATE;
      next_expr->as.update = update_expr;

      return next_expr;
    }
  }

  return expr;
}

static Expression *parser_unary() {
  while (parser_check(TOKEN_PLUS) && parser_next().type == TOKEN_PLUS) {
    (void)parser_advance();
    (void)parser_advance();

    UpdateExpression *update_expr = NEW(UpdateExpression);
    update_expr->operator= "++";
    update_expr->prefix = true;
    update_expr->argument = parser_call();

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_UPDATE;
    next_expr->as.update = update_expr;

    return next_expr;
  }

  while (parser_check(TOKEN_MINUS) && parser_next().type == TOKEN_MINUS) {
    (void)parser_advance();
    (void)parser_advance();

    UpdateExpression *update_expr = NEW(UpdateExpression);
    update_expr->operator= "--";
    update_expr->prefix = true;
    update_expr->argument = parser_call();

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_UPDATE;
    next_expr->as.update = update_expr;

    return next_expr;
  }

  while (parser_check(TOKEN_MINUS)) {
    const Token operator= parser_advance();

    Expression *right = parser_unary();

    UnaryExpression *unary_expr = NEW(UnaryExpression);
    unary_expr->operator= operator.lexeme;
    unary_expr->right = right;
    unary_expr->prefix = true;

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
    bin_expr->operator= operator.lexeme;
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
    bin_expr->operator= operator.lexeme;
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
    bin_expr->operator= operator.lexeme;
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
    bin_expr->operator= operator.lexeme;
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

    LogicalExpression *logical_expr = NEW(LogicalExpression);
    logical_expr->left = expr;
    logical_expr->operator= operator.lexeme;
    logical_expr->right = right;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_LOGICAL;
    next_expr->as.logical = logical_expr;

    return next_expr;
  }

  return expr;
}

static Expression *parser_or() {
  Expression *expr = parser_and();

  while (parser_check(TOKEN_OR)) {
    const Token operator= parser_advance();
    Expression *right = parser_and();

    LogicalExpression *logical_expr = NEW(LogicalExpression);
    logical_expr->left = expr;
    logical_expr->operator= operator.lexeme;
    logical_expr->right = right;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_LOGICAL;
    next_expr->as.logical = logical_expr;

    return next_expr;
  }

  return expr;
}

static Expression *parser_assignment() {
  Expression *expr = parser_or();

  if (parser_peek().type == TOKEN_LEFT_BRACE) {
    (void)parser_consume(TOKEN_LEFT_BRACE,
                         "expect '{' before an struct initialization");

    ObjectExpression *obj_expr = NEW(ObjectExpression);
    obj_expr->properties = NULL;
    obj_expr->length = 0;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_OBJECT;
    next_expr->as.object = obj_expr;

    do {
      ObjectProperty *prop = NEW(ObjectProperty);

      const Token prop_name = parser_consume(
          TOKEN_IDENTIFIER, "expect 'identifier' to define a struct property");

      prop->key = (Identifier){.name = prop_name.lexeme};

      (void)parser_consume(TOKEN_ASSING,
                           "expect '=' after struct property assignment");

      prop->value = parser_assignment();

      (void)parser_consume(TOKEN_COMMA,
                           "expect ',' after struct property assignment");

      const size_t index = obj_expr->length++;
      obj_expr->properties = ARRAY_RESIZE(
          ObjectProperty *, obj_expr->properties, obj_expr->length);
      obj_expr->properties[index] = prop;

    } while (!parser_check(TOKEN_RIGHT_BRACE));

    (void)parser_consume(TOKEN_RIGHT_BRACE,
                         "expect '}' after an struct initialization");

    return next_expr;
  } else if (parser_check(TOKEN_ASSING)) {
    (void)parser_consume(TOKEN_ASSING,
                         "expect '=' before an variable assignment");
    Expression *right = parser_assignment();

    AssignmentExpression *assign_expr = NEW(AssignmentExpression);
    assign_expr->left = expr;
    assign_expr->right = right;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_ASSIGNMENT;
    next_expr->as.assignment = assign_expr;

    return next_expr;
  }

  return expr;
}

static Expression *parser_expression() { return parser_assignment(); }

static Statement *parser_statement();

static Statement *parser_expression_statement() {
  Statement *stmt = NEW(Statement);

  if (parser_check(TOKEN_SEMICOLON)) {
    stmt->type = STATEMENT_EMPTY;

    (void)parser_consume(TOKEN_SEMICOLON, "expect ';' after expression");
  } else if (parser_check(TOKEN_LEFT_BRACE)) {
    (void)parser_consume(TOKEN_LEFT_BRACE, "expect '{' before block");

    stmt->type = STATEMENT_BLOCK;
    stmt->as.block = NEW(BlockStatement);
    stmt->as.block->length = 0;
    stmt->as.block->body = NULL;

    do {
      if (parser_is_eof()) {
        EXCEPTION("unterminated block\n");
      }

      const size_t index = stmt->as.block->length++;

      stmt->as.block->body = ARRAY_RESIZE(Statement *, stmt->as.block->body,
                                          stmt->as.block->length);

      stmt->as.block->body[index] = parser_statement();

    } while (!parser_check(TOKEN_RIGHT_BRACE));

    (void)parser_consume(TOKEN_RIGHT_BRACE, "expect '}' after block");
  } else {

    LOG_DEBUG(">> %d %s\n", parser_peek().type, parser_peek().lexeme);

    stmt->type = STATEMENT_EXPRESSION;
    stmt->as.expression = NEW(ExpressionStatement);
    stmt->as.expression->expression = parser_expression();

    (void)parser_consume(TOKEN_SEMICOLON, "expect ';' after expression");
  }

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

  call_expr->callee = (Identifier){
      .name = name.lexeme,
  };
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

static Statement *parser_var() {
  (void)parser_consume(TOKEN_VAR, "expect 'var' before variable declaration");

  const Token name = parser_consume(TOKEN_IDENTIFIER,
                                    "expect 'identifier' after 'var' keyword");

  Statement *decl = NEW(Statement);
  decl->type = STATEMENT_DECLARATION;
  decl->as.declaration = NEW(DeclarationStatement);
  decl->as.declaration->type = DECLARATION_VARIABLE;
  VariableDeclaration *val_decl = NEW(VariableDeclaration);
  decl->as.declaration->as.variable = val_decl;

  const size_t index = val_decl->length++;

  val_decl->declarations = ARRAY_RESIZE(
      VariableDeclarator *, val_decl->declarations, val_decl->length);

  VariableDeclarator *var_decl = NEW(VariableDeclarator);

  var_decl->id = (Identifier){
      .name = name.lexeme,
  };

  if (parser_check(TOKEN_ASSING)) {
    (void)parser_consume(TOKEN_ASSING, "expect '=' after variable declaration");

    var_decl->init = parser_expression();
  }

  val_decl->declarations[index] = var_decl;

  (void)parser_consume(TOKEN_SEMICOLON,
                       "expect ';' after variable declaration");

  return decl;
}

static Statement *parser_struct() {
  (void)parser_consume(TOKEN_STRUCT,
                       "expect 'struct' before struct declaration");

  const Token name = parser_consume(
      TOKEN_IDENTIFIER, "expect 'identifier' after 'struct' keyword");

  (void)parser_consume(TOKEN_LEFT_BRACE, "expect '{' after struct name");

  Statement *decl = NEW(Statement);
  decl->type = STATEMENT_DECLARATION;
  decl->as.declaration = NEW(DeclarationStatement);
  decl->as.declaration->type = DECLARATION_INTERFACE;
  InterfaceDeclaration *struct_decl = NEW(InterfaceDeclaration);
  decl->as.declaration->as.interface = struct_decl;

  struct_decl->id = (Identifier){
      .name = name.lexeme,
  };
  struct_decl->body = NULL;
  struct_decl->length = 0;

  do {
    PropertySignature *prop = NEW(PropertySignature);

    const Token prop_name = parser_consume(
        TOKEN_IDENTIFIER, "expect 'identifier' to define a struct property");

    prop->key = (Identifier){.name = prop_name.lexeme};
    prop->optional = false;

    if (parser_check(TOKEN_QUESTION_MARK)) {
      (void)parser_advance();

      prop->optional = true;
    }

    prop->typeAnnotation = NEW(TypeAnnotation);

    (void)parser_consume(TOKEN_COLON, "expect ':' after struct property name");

    switch (parser_peek().type) {
    case TOKEN_IDENTIFIER: {
      prop->typeAnnotation->type = TYPE_ANNOTATION_REFERENCE;
      prop->typeAnnotation->typeName =
          (Identifier){.name = parser_advance().lexeme};

      break;
    }

    case TOKEN_INTEGER:
    case TOKEN_FLOAT: {
      prop->typeAnnotation->type = TYPE_ANNOTATION_NUMBER;

      (void)parser_advance();

      break;
    }

    case TOKEN_BOOLEAN: {
      prop->typeAnnotation->type = TYPE_ANNOTATION_BOOLEAN;

      (void)parser_advance();

      break;
    }

    case TOKEN_STRING: {
      prop->typeAnnotation->type = TYPE_ANNOTATION_STRING;

      (void)parser_advance();

      break;
    }

    default:
      EXCEPTION("unknown struct property type\n");
      break;
    }

    (void)parser_consume(TOKEN_SEMICOLON,
                         "expect ';' after struct property definition");

    const size_t index = struct_decl->length++;
    struct_decl->body = ARRAY_RESIZE(PropertySignature *, struct_decl->body,
                                     struct_decl->length);
    struct_decl->body[index] = prop;
  } while (!parser_check(TOKEN_RIGHT_BRACE));

  (void)parser_consume(TOKEN_RIGHT_BRACE, "expect '}' after struct body");

  return decl;
}

static Statement *parser_statement() {
  const Token token = parser_peek();

  switch (token.type) {
  case TOKEN_STRUCT:
    return parser_struct();
    break;

  case TOKEN_VAR:
    return parser_var();

  case TOKEN_PRINT:
    return parser_print();

  default:
    return parser_expression_statement();
  }
}

ParserOutput *parser_run(LexerOutput *tokens) {
  ParserOutput *output = NEW(ParserOutput);

  output->body = NULL;
  output->length = 0;

  parser.start = parser.cursor = tokens->tokens;

  while (!parser_is_eof()) {
    parser_add_statement(output, parser_statement());
  }

  return output;
}
