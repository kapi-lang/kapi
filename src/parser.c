#include <string.h>

#include "lexer.h"
#include "macros.h"
#include "parser.h"
#include "token.h"
#include "type/string.h"

typedef struct ParserState {
  Token *start;
  Token *cursor;
} ParserState;

ParserState parser;

static Expression *parser_assignment();
static Expression *parser_expression();
static Statement *parser_statement();

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

  EXCEPTION("[parser] %s | found: (%d) %s at %lu:%lu \n", message, token.type,
            token.lexeme, token.line, token.column);
}

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

    char *number_string = string_remove_all(token.lexeme, '_');

    literal_expr->as.number = strtod(number_string, NULL);

    free(number_string);

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

  case TOKEN_LEFT_BRACE: {
    (void)parser_consume(TOKEN_LEFT_BRACE,
                         "expect '{' before an struct initialization");

    ObjectExpression *obj_expr = NEW(ObjectExpression);
    obj_expr->properties = NULL;
    obj_expr->length = 0;

    do {
      ObjectProperty *prop = NEW(ObjectProperty);

      const Token prop_name = parser_consume(
          TOKEN_IDENTIFIER, "expect 'identifier' to define a struct property");

      prop->key = (Identifier){.name = prop_name.lexeme};

      (void)parser_consume(TOKEN_ASSIGN,
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

    Expression *expr = NEW(Expression);
    expr->type = EXPRESSION_OBJECT;
    expr->as.object = obj_expr;

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

  EXCEPTION("[parser] expression type not supported (%d = '%s') at %lu:%lu\n",
            token.type, token.lexeme, token.line, token.column);
}

static Expression *parser_call() {
  Expression *expr = parser_primary();

  while (parser_check(TOKEN_DOT)) {
    (void)parser_advance();

    MemberExpression *member_expr = NEW(MemberExpression);
    member_expr->object = expr;
    member_expr->property = parser_primary();
    member_expr->computed = false;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_MEMBER;
    next_expr->as.member = member_expr;

    expr = next_expr;
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

        const size_t index = call_expr->length++;

        call_expr->arguments =
            ARRAY_RESIZE(Expression *, call_expr->arguments, call_expr->length);

        call_expr->arguments[index] = parser_expression();
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

    UpdateExpression *update_expr = NEW(UpdateExpression);
    update_expr->operator= operator.type;
    update_expr->prefix = false;
    update_expr->argument = expr;

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_UPDATE;
    next_expr->as.update = update_expr;

    return next_expr;
  }

  return expr;
}

static Expression *parser_unary() {
  while (parser_check(TOKEN_INCREMENT) || parser_check(TOKEN_DECREMENT)) {
    const Token operator= parser_advance();

    UpdateExpression *update_expr = NEW(UpdateExpression);
    update_expr->operator= operator.type;
    update_expr->prefix = true;
    update_expr->argument = parser_unary();

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_UPDATE;
    next_expr->as.update = update_expr;

    return next_expr;
  }

  while (parser_check(TOKEN_MINUS) || parser_check(TOKEN_BITWISE_NOT)) {
    const Token operator= parser_advance();

    UnaryExpression *unary_expr = NEW(UnaryExpression);
    unary_expr->operator= operator.type;
    unary_expr->right = parser_unary();
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

  while (parser_check(TOKEN_SLASH) || parser_check(TOKEN_STAR) ||
         parser_check(TOKEN_REMAINDER) || parser_check(TOKEN_EXPONENTIATION) ||
         parser_check(TOKEN_BITWISE_AND) || parser_check(TOKEN_BITWISE_NOT) ||
         parser_check(TOKEN_BITWISE_OR) || parser_check(TOKEN_BITWISE_XOR) ||
         parser_check(TOKEN_BITWISE_LEFT_SHIFT) ||
         parser_check(TOKEN_BITWISE_RIGHT_SHIFT) ||
         parser_check(TOKEN_BITWISE_UNSIGNED_RIGHT_SHIFT)) {
    const Token operator= parser_advance();

    BinaryExpression *bin_expr = NEW(BinaryExpression);
    bin_expr->left = expr;
    bin_expr->operator= operator.type;
    bin_expr->right = parser_unary();

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_BINARY;
    next_expr->as.binary = bin_expr;

    expr = next_expr;
  }

  return expr;
}

static Expression *parser_term() {
  Expression *expr = parser_factor();

  while (parser_check(TOKEN_MINUS) || parser_check(TOKEN_PLUS)) {
    const Token operator= parser_advance();

    BinaryExpression *bin_expr = NEW(BinaryExpression);
    bin_expr->left = expr;
    bin_expr->operator= operator.type;
    bin_expr->right = parser_factor();

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_BINARY;
    next_expr->as.binary = bin_expr;

    expr = next_expr;
  }

  return expr;
}

static Expression *parser_comparison() {
  Expression *expr = parser_term();

  while (parser_check(TOKEN_GREATER) || parser_check(TOKEN_GREATER_EQUAL) ||
         parser_check(TOKEN_LESS) || parser_check(TOKEN_LESS_EQUAL)) {
    const Token operator= parser_advance();

    BinaryExpression *bin_expr = NEW(BinaryExpression);
    bin_expr->left = expr;
    bin_expr->operator= operator.type;
    bin_expr->right = parser_term();

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_BINARY;
    next_expr->as.binary = bin_expr;

    expr = next_expr;
  }

  return expr;
}

static Expression *parser_equality() {
  Expression *expr = parser_comparison();

  while (parser_check(TOKEN_NOT) || parser_check(TOKEN_EQUAL)) {
    const Token operator= parser_advance();

    BinaryExpression *bin_expr = NEW(BinaryExpression);
    bin_expr->left = expr;
    bin_expr->operator= operator.type;
    bin_expr->right = parser_comparison();

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_BINARY;
    next_expr->as.binary = bin_expr;

    expr = next_expr;
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

    expr = next_expr;
  }

  return expr;
}

static Expression *parser_or() {
  Expression *expr = parser_and();

  while (parser_check(TOKEN_OR)) {
    const Token operator= parser_advance();

    LogicalExpression *logical_expr = NEW(LogicalExpression);
    logical_expr->left = expr;
    logical_expr->operator= operator.type;
    logical_expr->right = parser_and();

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_LOGICAL;
    next_expr->as.logical = logical_expr;

    expr = next_expr;
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

    AssignmentExpression *assign_expr = NEW(AssignmentExpression);
    assign_expr->left = expr;
    assign_expr->operator= operator.type;
    assign_expr->right = parser_assignment();

    Expression *next_expr = NEW(Expression);
    next_expr->type = EXPRESSION_ASSIGNMENT;
    next_expr->as.assignment = assign_expr;

    return next_expr;
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

  Statement *decl = NEW(Statement);
  decl->type = STATEMENT_DECLARATION;
  decl->as.declaration = NEW(DeclarationStatement);
  decl->as.declaration->type = DECLARATION_VARIABLE;
  VariableDeclaration *var_decl = NEW(VariableDeclaration);
  decl->as.declaration->as.variable = var_decl;

  var_decl->id = (Identifier){
      .name = name.lexeme,
      .typeAnnotation = NEW(TypeAnnotation),
  };

  var_decl->id.typeAnnotation->optional = false;

  (void)parser_consume(TOKEN_COLON, "expect ':' after variable name");

  const Token var_type =
      parser_consume(TOKEN_IDENTIFIER, "expect 'identifier' in variable type");

  if (string_equal(var_type.lexeme, "String")) {
    var_decl->id.typeAnnotation->type = TOKEN_TYPE_STRING;
  } else if (string_equal(var_type.lexeme, "Bool")) {
    var_decl->id.typeAnnotation->type = TOKEN_TYPE_BOOLEAN;
  } else if (string_equal(var_type.lexeme, "Int")) {
    var_decl->id.typeAnnotation->type = TOKEN_TYPE_INTEGER;
  } else if (string_equal(var_type.lexeme, "Float")) {
    var_decl->id.typeAnnotation->type = TOKEN_TYPE_FLOAT;
  } else {
    var_decl->id.typeAnnotation->type = TOKEN_TYPE_STRUCT;
    var_decl->id.typeAnnotation->typeName = var_type.lexeme;
  }

  if (parser_check(TOKEN_QUESTION_MARK)) {
    (void)parser_advance();
    var_decl->id.typeAnnotation->optional = true;
  }

  if (!var_decl->id.typeAnnotation->optional) {
    (void)parser_consume(TOKEN_ASSIGN, "expect '=' after variable declaration");

    var_decl->init = parser_expression();
  }

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

  StructDeclaration *struct_decl = NEW(StructDeclaration);

  struct_decl->id = (Identifier){.name = name.lexeme};
  struct_decl->body = NULL;
  struct_decl->length = 0;

  do {
    Identifier *prop = NEW(Identifier);

    const Token prop_name = parser_consume(
        TOKEN_IDENTIFIER, "expect 'identifier' to define a struct property");

    prop->name = prop_name.lexeme;
    prop->typeAnnotation = NEW(TypeAnnotation);

    (void)parser_consume(TOKEN_COLON, "expect ':' after struct property name");

    const Token prop_type = parser_consume(
        TOKEN_IDENTIFIER, "expect 'identifier' to define a struct property");

    if (string_equal(prop_type.lexeme, "String")) {
      prop->typeAnnotation->type = TOKEN_TYPE_STRING;
    } else if (string_equal(prop_type.lexeme, "Bool")) {
      prop->typeAnnotation->type = TOKEN_TYPE_BOOLEAN;
    } else if (string_equal(prop_type.lexeme, "Int")) {
      prop->typeAnnotation->type = TOKEN_TYPE_INTEGER;
    } else if (string_equal(prop_type.lexeme, "Float")) {
      prop->typeAnnotation->type = TOKEN_TYPE_FLOAT;
    } else {
      prop->typeAnnotation->type = TOKEN_TYPE_STRUCT;
      prop->typeAnnotation->typeName = prop_type.lexeme;
    }

    if (parser_check(TOKEN_QUESTION_MARK)) {
      (void)parser_advance();

      prop->typeAnnotation->optional = true;
    }

    (void)parser_consume(TOKEN_SEMICOLON,
                         "expect ';' after struct property definition");

    const size_t index = struct_decl->length++;
    struct_decl->body =
        ARRAY_RESIZE(Identifier *, struct_decl->body, struct_decl->length);
    struct_decl->body[index] = prop;
  } while (!parser_check(TOKEN_RIGHT_BRACE));

  (void)parser_consume(TOKEN_RIGHT_BRACE, "expect '}' after struct body");

  Statement *decl = NEW(Statement);

  decl->type = STATEMENT_DECLARATION;
  decl->as.declaration = NEW(DeclarationStatement);
  decl->as.declaration->type = DECLARATION_STRUCT;
  decl->as.declaration->as.structure = struct_decl;

  return decl;
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

    const size_t index = stmt->as.block->length++;

    stmt->as.block->body =
        ARRAY_RESIZE(Statement *, stmt->as.block->body, stmt->as.block->length);

    stmt->as.block->body[index] = parser_statement();
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

  stmt->as.for_loop->left = (Identifier){
      .name = for_item.lexeme,
  };

  (void)parser_consume(TOKEN_IN, "expect 'in' after 'identifier' keyword");

  if (parser_check(TOKEN_IDENTIFIER)) {
    stmt->as.for_loop->right = parser_primary();
  } else {
    Expression *right_expr = NEW(Expression);
    BinaryExpression *bin_expr = NEW(BinaryExpression);

    bin_expr->left = parser_primary();

    if (parser_check(TOKEN_TO)) {
      bin_expr
          ->operator= parser_consume(TOKEN_TO, "expect 'to' after 'in' keyword")
          .type;
    } else if (parser_check(TOKEN_DOWNTO)) {
      stmt->as.for_loop->reverse = true;
      bin_expr
          ->operator=
              parser_consume(TOKEN_DOWNTO, "expect 'downto' after 'in' keyword")
          .type;
    } else {
      EXCEPTION("unknown keyword\n");
    }

    bin_expr->right = parser_primary();

    right_expr->type = EXPRESSION_BINARY;
    right_expr->as.binary = bin_expr;

    stmt->as.for_loop->right = right_expr;
  }

  (void)parser_consume(TOKEN_RIGHT_PAREN,
                       "expect ')' after while test expression");

  stmt->as.for_loop->body = parser_block();

  return stmt;
}

static Statement *parser_function() {
  (void)parser_advance();

  FunctionDeclaration *fn_decl = NEW(FunctionDeclaration);

  fn_decl->length = 0;
  fn_decl->params = NULL;
  fn_decl->returnType = NEW(TypeAnnotation);

  fn_decl->id = (Identifier){
      .name = parser_consume(TOKEN_IDENTIFIER,
                             "expect 'identifier' to name a function")
                  .lexeme,
  };

  (void)parser_consume(TOKEN_LEFT_PAREN, "expect '(' after function name");

  do {
    if (parser_check(TOKEN_COMMA)) {
      (void)parser_advance();
    }

    const size_t index = fn_decl->length++;

    fn_decl->params =
        ARRAY_RESIZE(Identifier *, fn_decl->params, fn_decl->length);

    Identifier *param = NEW(Identifier);
    param->typeAnnotation = NEW(TypeAnnotation);

    switch (parser_peek().type) {
    case TOKEN_TYPE_VOID:
    case TOKEN_TYPE_BOOLEAN:
    case TOKEN_TYPE_NUMBER:
    case TOKEN_TYPE_INTEGER:
    case TOKEN_TYPE_FLOAT:
    case TOKEN_TYPE_STRING:
      param->typeAnnotation->type = parser_advance().type;
      break;
    case TOKEN_IDENTIFIER:
      param->typeAnnotation->type = TOKEN_TYPE_STRUCT;
      param->typeAnnotation->typeName = parser_advance().lexeme;
      break;
    default:
      EXCEPTION("invalid type for function parameter\n");
      break;
    }

    param->name =
        parser_consume(TOKEN_IDENTIFIER,
                       "expect 'identifier' for function parameter name")
            .lexeme;

    fn_decl->params[index] = param;
  } while (parser_check(TOKEN_COMMA));

  (void)parser_consume(TOKEN_RIGHT_PAREN, "expect ')' before function body");

  switch (parser_peek().type) {
  case TOKEN_TYPE_VOID:
  case TOKEN_TYPE_BOOLEAN:
  case TOKEN_TYPE_NUMBER:
  case TOKEN_TYPE_INTEGER:
  case TOKEN_TYPE_FLOAT:
  case TOKEN_TYPE_STRING:
    fn_decl->returnType->type = parser_advance().type;
    break;
  case TOKEN_IDENTIFIER:
    fn_decl->returnType->type = TOKEN_TYPE_STRUCT;
    fn_decl->returnType->typeName = parser_advance().lexeme;
    break;
  default:
    EXCEPTION("invalid type for function parameter\n");
    break;
  }

  fn_decl->body = parser_block();

  Statement *decl = NEW(Statement);

  decl->type = STATEMENT_DECLARATION;
  decl->as.declaration = NEW(DeclarationStatement);
  decl->as.declaration->type = DECLARATION_FUNCTION;
  decl->as.declaration->as.function = fn_decl;

  return decl;
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
    parser_add_statement(output, parser_statement());
  }

  return output;
}
