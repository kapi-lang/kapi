#include <string.h>

#include "lexer.h"
#include "macros.h"
#include "parser.h"
#include "token.h"
#include "type/string.h"

Identifier *parser_identifier(const char *name) {
  Identifier *id = NEW(Identifier);

  id->name = name;
  id->typeAnnotation = NULL;

  return id;
}

Expression *parser_identifier_expr(const char *name) {
  Expression *expr = NEW(Expression);

  expr->type = EXPRESSION_IDENTIFIER;
  expr->as.identifier = parser_identifier(name);

  return expr;
}

Expression *parser_unary_expr(TokenType operator, Expression * right,
                              bool prefix) {
  Expression *expr = NEW(Expression);
  expr->type = EXPRESSION_UNARY;
  expr->as.unary = NEW(UnaryExpression);
  expr->as.unary->operator= operator;
  expr->as.unary->right = right;
  expr->as.unary->prefix = prefix;

  return expr;
}

Expression *parser_member_expr(Expression *object, Expression *property,
                               bool computed) {
  Expression *expr = NEW(Expression);

  expr->type = EXPRESSION_MEMBER;
  expr->as.member = NEW(MemberExpression);
  expr->as.member->object = object;
  expr->as.member->property = property;
  expr->as.member->computed = computed;

  return expr;
}

Expression *parser_binary_expr(Expression *left, TokenType operator,
                               Expression * right) {
  Expression *expr = NEW(Expression);

  expr->type = EXPRESSION_BINARY;
  expr->as.binary = NEW(BinaryExpression);
  expr->as.binary->left = left;
  expr->as.binary->operator= operator;
  expr->as.binary->right = right;

  return expr;
}

Expression *parser_logical_expr(Expression *left, TokenType operator,
                                Expression * right) {
  Expression *expr = NEW(Expression);

  expr->type = EXPRESSION_LOGICAL;
  expr->as.logical = NEW(LogicalExpression);
  expr->as.logical->left = left;
  expr->as.logical->operator= operator;
  expr->as.logical->right = right;

  return expr;
}

Expression *parser_assignment_expr(Expression *left, TokenType operator,
                                   Expression * right) {
  Expression *expr = NEW(Expression);

  expr->type = EXPRESSION_ASSIGNMENT;
  expr->as.assignment = NEW(AssignmentExpression);
  expr->as.assignment->left = left;
  expr->as.assignment->operator= operator;
  expr->as.assignment->right = right;

  return expr;
}

Expression *parser_update_expr(TokenType operator, Expression * right,
                               bool prefix) {
  Expression *expr = NEW(Expression);

  expr->type = EXPRESSION_UPDATE;
  expr->as.update = NEW(UpdateExpression);
  expr->as.update->operator= operator;
  expr->as.update->argument = right;
  expr->as.update->prefix = prefix;

  return expr;
}

Expression *parser_literal_expr_number(TokenType type, double value) {
  Expression *expr = NEW(Expression);

  expr->type = EXPRESSION_LITERAL;
  expr->as.literal = NEW(Literal);
  expr->as.literal->type = type;
  expr->as.literal->as.number = value;

  return expr;
}

Expression *parser_literal_expr_nil() {
  Expression *expr = NEW(Expression);

  expr->type = EXPRESSION_LITERAL;
  expr->as.literal = NEW(Literal);
  expr->as.literal->type = TOKEN_NIL;

  return expr;
}

Expression *parser_literal_expr_bool(bool value) {
  Expression *expr = NEW(Expression);

  expr->type = EXPRESSION_LITERAL;
  expr->as.literal = NEW(Literal);
  expr->as.literal->type = TOKEN_BOOLEAN;
  expr->as.literal->as.boolean = value;

  return expr;
}

Expression *parser_literal_expr_string(const char *value) {
  Expression *expr = NEW(Expression);

  expr->type = EXPRESSION_LITERAL;
  expr->as.literal = NEW(Literal);
  expr->as.literal->type = TOKEN_STRING;
  expr->as.literal->as.string = value;

  return expr;
}

DeclarationStatement *parser_variable_decl(Identifier *id, void *init) {
  DeclarationStatement *decl = NEW(DeclarationStatement);

  decl->type = DECLARATION_VARIABLE;
  decl->as.variable = NEW(VariableDeclaration);
  decl->as.variable->id = id;
  decl->as.variable->init = init;

  return decl;
}

DeclarationStatement *parser_struct_decl(Identifier *id) {
  DeclarationStatement *decl = NEW(DeclarationStatement);

  decl->type = DECLARATION_STRUCT;
  decl->as.structure = NEW(StructDeclaration);
  decl->as.structure->id = id;
  decl->as.structure->body = NULL;
  decl->as.structure->length = 0;

  return decl;
}

DeclarationStatement *parser_function_decl(Identifier *id) {
  DeclarationStatement *decl = NEW(DeclarationStatement);

  decl->type = DECLARATION_FUNCTION;
  decl->as.function = NEW(FunctionDeclaration);
  decl->as.function->id = id;
  decl->as.function->body = NULL;
  decl->as.function->params = NULL;
  decl->as.function->length = 0;
  decl->as.function->returnType = NEW(TypeAnnotation);

  return decl;
}

Statement *parser_declaration_stmt(DeclarationStatement *decl) {
  Statement *stmt = NEW(Statement);
  stmt->type = STATEMENT_DECLARATION;
  stmt->as.declaration = decl;

  return stmt;
}

typedef struct ParserState {
  Token *start;
  Token *cursor;
} ParserState;

ParserState parser;

static Expression *parser_assignment();
static Expression *parser_expression();
static Statement *parser_statement();

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

  EXCEPTION("[parser] %s | found: (%d) %s at %lu:%lu \n", message, token.type,
            token.lexeme, token.line, token.column);
}

static Expression *parser_primary() {
  const Token token = parser_peek();

  switch (token.type) {
  case TOKEN_NIL: {
    Expression *expr = parser_literal_expr_nil();

    (void)parser_advance();

    return expr;
  }

  case TOKEN_INTEGER: {
    char *number_string = string_remove_all(token.lexeme, '_');

    Expression *expr =
        parser_literal_expr_number(TOKEN_INTEGER, strtod(number_string, NULL));

    free(number_string);

    (void)parser_advance();

    return expr;
  }

  case TOKEN_FLOAT: {
    char *number_string = string_remove_all(token.lexeme, '_');

    Expression *expr =
        parser_literal_expr_number(TOKEN_FLOAT, strtod(number_string, NULL));

    free(number_string);

    (void)parser_advance();

    return expr;
  }

  case TOKEN_TRUE:
  case TOKEN_FALSE: {
    Expression *expr =
        parser_literal_expr_bool(string_equal(token.lexeme, "true"));

    (void)parser_advance();

    return expr;
  }

  case TOKEN_STRING: {
    Expression *expr = parser_literal_expr_string(token.lexeme);

    (void)parser_advance();

    return expr;
  }

  case TOKEN_LEFT_PAREN: {
    Expression *expr = NEW(Expression);
    GroupExpression *group_expr = NEW(GroupExpression);

    (void)parser_advance();

    group_expr->expression = parser_expression();

    (void)parser_consume(TOKEN_RIGHT_PAREN,
                         "expect ')' after group expression");

    expr->type = EXPRESSION_GROUP;
    expr->as.group = group_expr;

    return expr;
  }

  case TOKEN_LEFT_BRACE: {
    (void)parser_advance();

    ObjectExpression *obj_expr = NEW(ObjectExpression);
    obj_expr->properties = NULL;
    obj_expr->length = 0;

    do {
      ObjectProperty *prop = NEW(ObjectProperty);

      const Token prop_name = parser_consume(
          TOKEN_IDENTIFIER, "expect 'identifier' to define a struct property");

      prop->key = NEW(Identifier);
      prop->key->name = prop_name.lexeme;

      (void)parser_consume(TOKEN_ASSIGN,
                           "expect '=' after struct property assignment");

      prop->value = parser_assignment();

      (void)parser_consume(TOKEN_COMMA,
                           "expect ',' after struct property assignment");

      ARRAY_PUSH(ObjectProperty *, obj_expr->properties, obj_expr->length,
                 prop);
    } while (!parser_check(TOKEN_RIGHT_BRACE));

    (void)parser_consume(TOKEN_RIGHT_BRACE,
                         "expect '}' after an struct initialization");

    Expression *expr = NEW(Expression);
    expr->type = EXPRESSION_OBJECT;
    expr->as.object = obj_expr;

    return expr;
  }

  case TOKEN_LEFT_BRACKET: {
    (void)parser_advance();

    ArrayExpression *arr_expr = NEW(ArrayExpression);
    arr_expr->elements = NULL;
    arr_expr->length = 0;

    do {
      if (parser_check(TOKEN_COMMA))
        (void)parser_advance();

      Expression *elm_expr = parser_expression();

      ARRAY_PUSH(Expression *, arr_expr->elements, arr_expr->length, elm_expr);
    } while (parser_check(TOKEN_COMMA));

    (void)parser_consume(TOKEN_RIGHT_BRACKET,
                         "expect ']' after an list initialization");

    Expression *expr = NEW(Expression);
    expr->type = EXPRESSION_ARRAY;
    expr->as.object = arr_expr;

    return expr;
  }

  case TOKEN_IDENTIFIER: {
    Expression *expr = parser_identifier_expr(token.lexeme);

    (void)parser_advance();

    return expr;
  }

  default:
    break;
  }

  EXCEPTION("[parser] expression type not supported (%d = '%s') at %lu:%lu\n",
            token.type, token.lexeme, token.line, token.column);
}

static Expression *parser_call() {
  Expression *expr = parser_primary();

  while (parser_check(TOKEN_DOT)) {
    (void)parser_advance();

    expr = parser_member_expr(expr, parser_primary(), false);
  }

  while (parser_check(TOKEN_LEFT_PAREN)) {
    (void)parser_advance();

    CallExpression *call_expr = NEW(CallExpression);
    call_expr->callee = expr;
    call_expr->arguments = NULL;
    call_expr->length = 0;

    if (!parser_check(TOKEN_RIGHT_PAREN)) {
      do {
        if (parser_check(TOKEN_COMMA))
          (void)parser_advance();

        ARRAY_PUSH(Expression *, call_expr->arguments, call_expr->length,
                   parser_expression());
      } while (parser_check(TOKEN_COMMA));
    }

    (void)parser_consume(TOKEN_RIGHT_PAREN, "expect ')' after call.");

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_CALL;
    next_expr->as.call = call_expr;

    expr = next_expr;
  }

  while (parser_check(TOKEN_INCREMENT) || parser_check(TOKEN_DECREMENT)) {
    const Token operator= parser_advance();

    return parser_update_expr(operator.type, expr, false);
  }

  return expr;
}

static Expression *parser_unary() {
  while (parser_check(TOKEN_INCREMENT) || parser_check(TOKEN_DECREMENT)) {
    const Token operator= parser_advance();

    return parser_update_expr(operator.type, parser_unary(), true);
  }

  while (parser_check(TOKEN_MINUS) || parser_check(TOKEN_BITWISE_NOT)) {
    const Token operator= parser_advance();

    return parser_unary_expr(operator.type, parser_unary(), true);
  }

  return parser_call();
}

static Expression *parser_factor() {
  Expression *expr = parser_unary();

  while (parser_check(TOKEN_SLASH) || parser_check(TOKEN_STAR) ||
         parser_check(TOKEN_REMAINDER) || parser_check(TOKEN_EXPONENTIATION) ||
         parser_check(TOKEN_BITWISE_AND) || parser_check(TOKEN_BITWISE_NOT) ||
         parser_check(TOKEN_BITWISE_OR) || parser_check(TOKEN_BITWISE_XOR) ||
         parser_check(TOKEN_BITWISE_LEFT_SHIFT) ||
         parser_check(TOKEN_BITWISE_RIGHT_SHIFT) ||
         parser_check(TOKEN_BITWISE_UNSIGNED_RIGHT_SHIFT)) {
    const Token operator= parser_advance();

    expr = parser_binary_expr(expr, operator.type, parser_unary());
  }

  return expr;
}

static Expression *parser_term() {
  Expression *expr = parser_factor();

  while (parser_check(TOKEN_MINUS) || parser_check(TOKEN_PLUS)) {
    const Token operator= parser_advance();

    expr = parser_binary_expr(expr, operator.type, parser_factor());
  }

  return expr;
}

static Expression *parser_comparison() {
  Expression *expr = parser_term();

  while (parser_check(TOKEN_GREATER) || parser_check(TOKEN_GREATER_EQUAL) ||
         parser_check(TOKEN_LESS) || parser_check(TOKEN_LESS_EQUAL)) {
    const Token operator= parser_advance();

    expr = parser_binary_expr(expr, operator.type, parser_term());
  }

  return expr;
}

static Expression *parser_equality() {
  Expression *expr = parser_comparison();

  while (parser_check(TOKEN_NOT) || parser_check(TOKEN_EQUAL)) {
    const Token operator= parser_advance();

    expr = parser_binary_expr(expr, operator.type, parser_comparison());
  }

  return expr;
}

static Expression *parser_and() {
  Expression *expr = parser_equality();

  while (parser_check(TOKEN_AND)) {
    const Token operator= parser_advance();

    LogicalExpression *logical_expr = NEW(LogicalExpression);
    logical_expr->left = expr;
    logical_expr->operator= operator.type;
    logical_expr->right = parser_equality();

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_LOGICAL;
    next_expr->as.logical = logical_expr;

    expr = parser_logical_expr(expr, operator.type, parser_equality());
  }

  return expr;
}

static Expression *parser_or() {
  Expression *expr = parser_and();

  while (parser_check(TOKEN_OR)) {
    const Token operator= parser_advance();

    expr = parser_logical_expr(expr, operator.type, parser_and());
  }

  return expr;
}

static Expression *parser_assignment() {
  Expression *expr = parser_or();

  if (parser_check(TOKEN_ASSIGN) || parser_check(TOKEN_ASSIGN_INCREMENT) ||
      parser_check(TOKEN_ASSIGN_DECREMENT) ||
      parser_check(TOKEN_ASSIGN_DIVIDE) ||
      parser_check(TOKEN_ASSIGN_MULTIPLY) ||
      parser_check(TOKEN_ASSIGN_REMAINDER) ||
      parser_check(TOKEN_ASSIGN_EXPONENTIATION) ||
      parser_check(TOKEN_ASSIGN_BITWISE_AND) ||
      parser_check(TOKEN_ASSIGN_BITWISE_OR) ||
      parser_check(TOKEN_ASSIGN_BITWISE_XOR) ||
      parser_check(TOKEN_ASSIGN_BITWISE_LEFT_SHIFT) ||
      parser_check(TOKEN_ASSIGN_BITWISE_RIGHT_SHIFT) ||
      parser_check(TOKEN_ASSIGN_BITWISE_UNSIGNED_RIGHT_SHIFT)) {
    const Token operator= parser_advance();

    return parser_assignment_expr(expr, operator.type, parser_assignment());
  }

  return expr;
}

static Expression *parser_expression() { return parser_assignment(); }

static Statement *parser_expression_statement() {
  Statement *stmt = NEW(Statement);

  if (parser_check(TOKEN_SEMICOLON)) {
    stmt->type = STATEMENT_EMPTY;
  } else if (parser_check(TOKEN_BREAK)) {
    stmt->type = STATEMENT_BREAK;
    (void)parser_advance();
  } else if (parser_check(TOKEN_CONTINUE)) {
    stmt->type = STATEMENT_CONTINUE;
    (void)parser_advance();
  } else if (parser_check(TOKEN_RETURN)) {
    (void)parser_advance();
    stmt->type = STATEMENT_RETURN;
    stmt->as.returns = NEW(ReturnStatement);
    stmt->as.returns->argument = parser_expression();
  } else {
    stmt->type = STATEMENT_EXPRESSION;
    stmt->as.expression = NEW(ExpressionStatement);
    stmt->as.expression->expression = parser_expression();
  }

  (void)parser_consume(TOKEN_SEMICOLON, "expect ';' after expression");

  return stmt;
}

static Statement *parser_variable() {
  (void)parser_consume(TOKEN_VAR, "expect 'var' before variable declaration");

  const Token name = parser_consume(TOKEN_IDENTIFIER,
                                    "expect 'identifier' after 'var' keyword");

  Identifier *id = parser_identifier(name.lexeme);
  id->typeAnnotation = NEW(TypeAnnotation);
  id->typeAnnotation->optional = false;

  DeclarationStatement *decl = parser_variable_decl(id, NULL);

  (void)parser_consume(TOKEN_COLON, "expect ':' after variable name");

  const Token var_type =
      parser_consume(TOKEN_IDENTIFIER, "expect 'identifier' in variable type");

  if (string_equal(var_type.lexeme, "String")) {
    id->typeAnnotation->type = TYPE_STRING;
  } else if (string_equal(var_type.lexeme, "Bool")) {
    id->typeAnnotation->type = TYPE_BOOLEAN;
  } else if (string_equal(var_type.lexeme, "Int")) {
    id->typeAnnotation->type = TYPE_INTEGER;
  } else if (string_equal(var_type.lexeme, "Float")) {
    id->typeAnnotation->type = TYPE_FLOAT;
  } else {
    id->typeAnnotation->type = TYPE_STRUCT;
    id->typeAnnotation->as.structure = NEW(TypeAnnotationStruct);
    id->typeAnnotation->as.structure->type_name = var_type.lexeme;
  }

  if (parser_check(TOKEN_LEFT_BRACKET)) {
    (void)parser_advance();

    TypeAnnotation *next_type_annotation = NEW(TypeAnnotation);
    next_type_annotation->type = TYPE_LIST;
    next_type_annotation->as.list = NEW(TypeAnnotationList);
    next_type_annotation->as.list->element_type = id->typeAnnotation;

    id->typeAnnotation = next_type_annotation;

    (void)parser_consume(TOKEN_RIGHT_BRACKET,
                         "expect ']' after variable declaration");

    if (parser_check(TOKEN_LEFT_BRACKET)) {
      (void)parser_advance();
      (void)parser_consume(TOKEN_RIGHT_BRACKET,
                           "expect ']' after variable declaration");
    }
  }

  if (parser_check(TOKEN_QUESTION_MARK)) {
    (void)parser_advance();
    id->typeAnnotation->optional = true;
  }

  if (id->typeAnnotation->optional) {
    decl->as.variable->init = parser_literal_expr_nil();
  } else {
    (void)parser_consume(TOKEN_ASSIGN, "expect '=' after variable declaration");

    decl->as.variable->init = parser_expression();
  }

  (void)parser_consume(TOKEN_SEMICOLON,
                       "expect ';' after variable declaration");

  return parser_declaration_stmt(decl);
}

static Statement *parser_struct() {
  (void)parser_consume(TOKEN_STRUCT,
                       "expect 'struct' before struct declaration");

  const Token name = parser_consume(
      TOKEN_IDENTIFIER, "expect 'identifier' after 'struct' keyword");

  (void)parser_consume(TOKEN_LEFT_BRACE, "expect '{' after struct name");

  DeclarationStatement *decl =
      parser_struct_decl(parser_identifier(name.lexeme));

  do {
    const Token prop_name = parser_consume(
        TOKEN_IDENTIFIER, "expect 'identifier' to define a struct property");

    Identifier *prop = parser_identifier(prop_name.lexeme);
    prop->typeAnnotation = NEW(TypeAnnotation);

    (void)parser_consume(TOKEN_COLON, "expect ':' after struct property name");

    const Token prop_type = parser_consume(
        TOKEN_IDENTIFIER, "expect 'identifier' to define a struct property");

    if (string_equal(prop_type.lexeme, "String")) {
      prop->typeAnnotation->type = TYPE_STRING;
    } else if (string_equal(prop_type.lexeme, "Bool")) {
      prop->typeAnnotation->type = TYPE_BOOLEAN;
    } else if (string_equal(prop_type.lexeme, "Int")) {
      prop->typeAnnotation->type = TYPE_INTEGER;
    } else if (string_equal(prop_type.lexeme, "Float")) {
      prop->typeAnnotation->type = TYPE_FLOAT;
    } else {
      prop->typeAnnotation->type = TYPE_STRUCT;
      prop->typeAnnotation->as.structure = NEW(TypeAnnotationStruct);
      prop->typeAnnotation->as.structure->type_name = prop_type.lexeme;
    }

    if (parser_check(TOKEN_QUESTION_MARK)) {
      (void)parser_advance();

      prop->typeAnnotation->optional = true;
    }

    (void)parser_consume(TOKEN_SEMICOLON,
                         "expect ';' after struct property definition");

    ARRAY_PUSH(Identifier *, decl->as.structure->body,
               decl->as.structure->length, prop);
  } while (!parser_check(TOKEN_RIGHT_BRACE));

  (void)parser_consume(TOKEN_RIGHT_BRACE, "expect '}' after struct body");

  return parser_declaration_stmt(decl);
}

static Statement *parser_block() {
  (void)parser_consume(TOKEN_LEFT_BRACE, "expect '{' before block");

  Statement *stmt = NEW(Statement);
  stmt->type = STATEMENT_BLOCK;
  stmt->as.block = NEW(BlockStatement);
  stmt->as.block->length = 0;
  stmt->as.block->body = NULL;

  do {
    if (parser_is_eof()) {
      EXCEPTION("unterminated block\n");
    }

    ARRAY_PUSH(Statement *, stmt->as.block->body, stmt->as.block->length,
               parser_statement());
  } while (!parser_check(TOKEN_RIGHT_BRACE));

  (void)parser_consume(TOKEN_RIGHT_BRACE, "expect '}' after block");

  return stmt;
}

static Statement *parser_while() {
  (void)parser_consume(TOKEN_WHILE, "expect 'while' before while loop");
  (void)parser_consume(TOKEN_LEFT_PAREN, "expect '(' after 'while' keyword");

  Statement *stmt = NEW(Statement);
  stmt->type = STATEMENT_WHILE;
  stmt->as.while_loop = NEW(WhileStatement);
  stmt->as.while_loop->test = parser_expression();

  (void)parser_consume(TOKEN_RIGHT_PAREN,
                       "expect ')' after while test expression");

  stmt->as.while_loop->body = parser_block();

  return stmt;
}

static Statement *parser_if() {
  (void)parser_advance();
  (void)parser_consume(TOKEN_LEFT_PAREN, "expect '(' after 'if' keyword");

  Statement *stmt = NEW(Statement);
  stmt->type = STATEMENT_IF;
  stmt->as.condition = NEW(IfStatement);
  stmt->as.condition->test = parser_expression();

  (void)parser_consume(TOKEN_RIGHT_PAREN,
                       "expect ')' after if test expression");

  stmt->as.condition->consequent = parser_block();
  stmt->as.condition->alternate = NULL;

  if (parser_check(TOKEN_ELSE)) {
    (void)parser_advance();

    if (parser_check(TOKEN_IF)) {
      stmt->as.condition->alternate = parser_if();
    } else {
      stmt->as.condition->alternate = parser_block();
    }
  }

  return stmt;
}

static Statement *parser_for() {
  (void)parser_consume(TOKEN_FOR, "expect 'for' before for loop");
  (void)parser_consume(TOKEN_LEFT_PAREN, "expect '(' after 'for' keyword");

  Statement *stmt = NEW(Statement);
  stmt->type = STATEMENT_FOR;
  stmt->as.for_loop = NEW(ForStatement);
  stmt->as.for_loop->reverse = false;

  const Token for_item =
      parser_consume(TOKEN_IDENTIFIER, "expect 'identifier' after '('");

  stmt->as.for_loop->left = parser_identifier(for_item.lexeme);

  (void)parser_consume(TOKEN_IN, "expect 'in' after 'identifier' keyword");

  if (parser_check(TOKEN_IDENTIFIER)) {
    stmt->as.for_loop->right = parser_primary();
  } else {

    Expression *left = parser_primary();

    TokenType operator;
    if (parser_check(TOKEN_TO)) {
      operator= parser_consume(TOKEN_TO, "expect 'to' after 'in' keyword").type;
    } else if (parser_check(TOKEN_DOWNTO)) {
      stmt->as.for_loop->reverse = true;
      operator=
          parser_consume(TOKEN_DOWNTO, "expect 'downto' after 'in' keyword")
              .type;
    } else {
      EXCEPTION("unknown keyword\n");
    }

    Expression *right = parser_primary();

    Expression *bin_expr = parser_binary_expr(left, operator, right);

    stmt->as.for_loop->right = bin_expr;
  }

  (void)parser_consume(TOKEN_RIGHT_PAREN,
                       "expect ')' after while test expression");

  stmt->as.for_loop->body = parser_block();

  return stmt;
}

static Statement *parser_function() {
  (void)parser_advance();

  DeclarationStatement *decl = parser_function_decl(parser_identifier(
      parser_consume(TOKEN_IDENTIFIER, "expect 'identifier' to name a function")
          .lexeme));

  (void)parser_consume(TOKEN_LEFT_PAREN, "expect '(' after function name");

  do {
    if (parser_check(TOKEN_COMMA)) {
      (void)parser_advance();
    }

    Identifier *param = NEW(Identifier);
    param->typeAnnotation = NEW(TypeAnnotation);

    const Token param_type = parser_peek();

    if (string_equal(param_type.lexeme, "String")) {
      param->typeAnnotation->type = TYPE_STRING;
    } else if (string_equal(param_type.lexeme, "Bool")) {
      param->typeAnnotation->type = TYPE_BOOLEAN;
    } else if (string_equal(param_type.lexeme, "Int")) {
      param->typeAnnotation->type = TYPE_INTEGER;
    } else if (string_equal(param_type.lexeme, "Float")) {
      param->typeAnnotation->type = TYPE_FLOAT;
    } else if (param_type.type == TOKEN_IDENTIFIER) {
      param->typeAnnotation->type = TYPE_STRUCT;
      param->typeAnnotation->as.structure = NEW(TypeAnnotationStruct);
      param->typeAnnotation->as.structure->type_name = param_type.lexeme;
    } else {
      EXCEPTION("invalid type for function parameter\n");
    }

    (void)parser_advance();

    param->name =
        parser_consume(TOKEN_IDENTIFIER,
                       "expect 'identifier' for function parameter name")
            .lexeme;

    ARRAY_PUSH(Identifier *, decl->as.function->params,
               decl->as.function->length, param);
  } while (parser_check(TOKEN_COMMA));

  (void)parser_consume(TOKEN_RIGHT_PAREN, "expect ')' before function body");

  const Token return_type = parser_peek();

  if (return_type.type == TOKEN_LEFT_BRACE) {
    decl->as.function->returnType->type = TYPE_VOID;
  } else if (string_equal(return_type.lexeme, "String")) {
    decl->as.function->returnType->type = TYPE_STRING;
    (void)parser_advance();
  } else if (string_equal(return_type.lexeme, "Bool")) {
    decl->as.function->returnType->type = TYPE_BOOLEAN;
    (void)parser_advance();
  } else if (string_equal(return_type.lexeme, "Int")) {
    decl->as.function->returnType->type = TYPE_INTEGER;
    (void)parser_advance();
  } else if (string_equal(return_type.lexeme, "Float")) {
    decl->as.function->returnType->type = TYPE_FLOAT;
    (void)parser_advance();
  } else if (return_type.type == TOKEN_IDENTIFIER) {
    decl->as.function->returnType->type = TYPE_STRUCT;
    decl->as.function->returnType->as.structure = NEW(TypeAnnotationStruct);
    decl->as.function->returnType->as.structure->type_name = return_type.lexeme;

    (void)parser_advance();
  } else {
    EXCEPTION("invalid type for function return\n");
  }

  decl->as.function->body = parser_block();

  return parser_declaration_stmt(decl);
}

static Statement *parser_statement() {
  const Token token = parser_peek();

  switch (token.type) {
  case TOKEN_FUNCTION:
    return parser_function();

  case TOKEN_VAR:
    return parser_variable();

  case TOKEN_WHILE:
    return parser_while();
    break;

  case TOKEN_FOR:
    return parser_for();
    break;

  case TOKEN_IF:
    return parser_if();
    break;

  case TOKEN_LEFT_BRACE:
    return parser_block();

  case TOKEN_STRUCT:
    return parser_struct();
    break;

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
    ARRAY_PUSH(Statement *, output->body, output->length, parser_statement());
  }

  return output;
}
