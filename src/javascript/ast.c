#include <string.h>

#include "javascript/ast.h"
#include "macros.h"
#include "parser.h"
#include "token.h"
#include "type/string.h"

static json_object *ast_statement(const Statement *stmt);
static json_object *ast_expression(const Expression *expression);

static json_object *ast_object_typed(const char *value) {
  json_object *jroot = json_object_new_object();

  json_object_object_add(jroot, "type", json_object_new_string(value));

  return jroot;
}

static json_object *ast_identifier(const char *value) {
  json_object *jroot = ast_object_typed("Identifier");

  json_object_object_add(jroot, "name", json_object_new_string(value));

  return jroot;
}

static json_object *ast_null_literal() {
  json_object *jroot = ast_object_typed("NullLiteral");

  json_object_object_add(jroot, "value", NULL);

  return jroot;
}

static json_object *ast_numeric_literal(double value, TokenType type) {
  json_object *jroot = ast_object_typed("NumericLiteral");

  if (type == TOKEN_INTEGER) {
    json_object_object_add(jroot, "value", json_object_new_int64(value));
  } else if (type == TOKEN_FLOAT) {
    json_object_object_add(jroot, "value", json_object_new_double(value));
  }

  return jroot;
}

static json_object *ast_boolean_literal(bool value) {
  json_object *jroot = ast_object_typed("BooleanLiteral");

  json_object_object_add(jroot, "value", json_object_new_boolean(value));

  return jroot;
}

static json_object *ast_string_literal(const char *value) {
  json_object *jroot = ast_object_typed("StringLiteral");

  json_object_object_add(jroot, "value", json_object_new_string(value));

  return jroot;
}

static json_object *ast_binary_expression(const BinaryExpression *expression) {
  json_object *jexpr = ast_object_typed("BinaryExpression");

  json_object_object_add(jexpr, "left", ast_expression(expression->left));

  switch (expression->operator) {
  case TOKEN_EQUAL:
    json_object_object_add(jexpr, "operator", json_object_new_string("==="));
    break;

  case TOKEN_NOT:
    json_object_object_add(jexpr, "operator", json_object_new_string("!=="));
    break;

  case TOKEN_GREATER:
    json_object_object_add(jexpr, "operator", json_object_new_string(">"));
    break;

  case TOKEN_GREATER_EQUAL:
    json_object_object_add(jexpr, "operator", json_object_new_string(">="));
    break;

  case TOKEN_LESS:
    json_object_object_add(jexpr, "operator", json_object_new_string("<"));
    break;

  case TOKEN_LESS_EQUAL:
    json_object_object_add(jexpr, "operator", json_object_new_string("<="));
    break;

  case TOKEN_PLUS:
    json_object_object_add(jexpr, "operator", json_object_new_string("+"));
    break;

  case TOKEN_MINUS:
    json_object_object_add(jexpr, "operator", json_object_new_string("-"));
    break;

  case TOKEN_STAR:
    json_object_object_add(jexpr, "operator", json_object_new_string("*"));
    break;

  case TOKEN_SLASH:
    json_object_object_add(jexpr, "operator", json_object_new_string("/"));
    break;

  case TOKEN_EXPONENTIATION:
    json_object_object_add(jexpr, "operator", json_object_new_string("**"));
    break;

  case TOKEN_REMAINDER:
    json_object_object_add(jexpr, "operator", json_object_new_string("%"));
    break;

  case TOKEN_BITWISE_AND:
    json_object_object_add(jexpr, "operator", json_object_new_string("&"));
    break;

  case TOKEN_BITWISE_OR:
    json_object_object_add(jexpr, "operator", json_object_new_string("|"));
    break;

  case TOKEN_BITWISE_XOR:
    json_object_object_add(jexpr, "operator", json_object_new_string("^"));
    break;

  case TOKEN_BITWISE_LEFT_SHIFT:
    json_object_object_add(jexpr, "operator", json_object_new_string("<<"));
    break;

  case TOKEN_BITWISE_RIGHT_SHIFT:
    json_object_object_add(jexpr, "operator", json_object_new_string(">>"));
    break;

  case TOKEN_BITWISE_UNSIGNED_RIGHT_SHIFT:
    json_object_object_add(jexpr, "operator", json_object_new_string(">>>"));
    break;

  default:
    EXCEPTION("operator (%d) not supported for BinaryExpression\n",
              expression->operator);
    break;
  }

  json_object_object_add(jexpr, "right", ast_expression(expression->right));

  return jexpr;
}

static json_object *ast_update_expression(const UpdateExpression *expression) {
  json_object *jexpr = ast_object_typed("UpdateExpression");

  json_object_object_add(jexpr, "prefix",
                         json_object_new_boolean(expression->prefix));

  switch (expression->operator) {
  case TOKEN_INCREMENT:
    json_object_object_add(jexpr, "operator", json_object_new_string("++"));
    break;

  case TOKEN_DECREMENT:
    json_object_object_add(jexpr, "operator", json_object_new_string("--"));
    break;

  default:
    EXCEPTION("operator (%d) not supported for UpdateExpression\n",
              expression->operator);
    break;
  }

  json_object_object_add(jexpr, "argument",
                         ast_expression(expression->argument));

  return jexpr;
}

static json_object *ast_expression(const Expression *expression) {
  switch (expression->type) {
  case EXPRESSION_ARRAY: {
    json_object *jexpr = ast_object_typed("ArrayExpression");

    json_object *jelements = json_object_new_array();
    json_object_object_add(jexpr, "elements", jelements);

    FOR_I(expression->as.array->length, {
      const Expression *element = expression->as.array->elements[i];

      json_object_array_put_idx(jelements, i, ast_expression(element));
    });

    return jexpr;
  }

  case EXPRESSION_MEMBER: {
    json_object *jexpr = ast_object_typed("MemberExpression");

    json_object_object_add(jexpr, "object",
                           ast_expression(expression->as.member->object));
    json_object_object_add(jexpr, "property",
                           ast_expression(expression->as.member->property));

    json_object_object_add(
        jexpr, "computed",
        json_object_new_boolean(expression->as.member->computed));

    return jexpr;
  }

  case EXPRESSION_UPDATE: {
    return ast_update_expression(expression->as.update);
  }

  case EXPRESSION_LOGICAL: {
    json_object *jexpr = ast_object_typed("LogicalExpression");

    json_object_object_add(jexpr, "left",
                           ast_expression(expression->as.logical->left));

    switch (expression->as.logical->operator) {
    case TOKEN_AND:
      json_object_object_add(jexpr, "operator", json_object_new_string("&&"));
      break;

    case TOKEN_OR:
      json_object_object_add(jexpr, "operator", json_object_new_string("||"));
      break;

    default:
      EXCEPTION("operator (%d) not supported for LogicalExpression\n",
                expression->as.logical->operator);
      break;
    }

    json_object_object_add(jexpr, "right",
                           ast_expression(expression->as.logical->right));

    return jexpr;
  }

  case EXPRESSION_OBJECT: {
    json_object *jexpr = ast_object_typed("ObjectExpression");

    json_object *jprops = json_object_new_array();
    json_object_object_add(jexpr, "properties", jprops);

    FOR_I(expression->as.object->length, {
      const ObjectProperty *prop = expression->as.object->properties[i];

      json_object *jprop = ast_object_typed("ObjectProperty");
      json_object_array_put_idx(jprops, i, jprop);

      json_object_object_add(jprop, "key", ast_identifier(prop->key->name));
      json_object_object_add(jprop, "computed", json_object_new_boolean(false));
      json_object_object_add(jprop, "method", json_object_new_boolean(false));
      json_object_object_add(jprop, "method", json_object_new_boolean(false));
      json_object_object_add(jprop, "value", ast_expression(prop->value));
    });

    return jexpr;
  }

  case EXPRESSION_IDENTIFIER:
    return ast_identifier(expression->as.identifier->name);

  case EXPRESSION_UNARY: {
    json_object *jexpr = ast_object_typed("UnaryExpression");

    json_object_object_add(
        jexpr, "prefix", json_object_new_boolean(expression->as.unary->prefix));

    switch (expression->as.unary->operator) {
    case TOKEN_MINUS:
      json_object_object_add(jexpr, "operator", json_object_new_string("-"));
      break;

    case TOKEN_BITWISE_NOT:
      json_object_object_add(jexpr, "operator", json_object_new_string("~"));
      break;

    default:
      EXCEPTION("operator (%d) not supported for UnaryExpression\n",
                expression->as.unary->operator);
      break;
    }

    json_object_object_add(jexpr, "argument",
                           ast_expression(expression->as.unary->right));

    return jexpr;
  }

  case EXPRESSION_BINARY:
    return ast_binary_expression(expression->as.binary);

  case EXPRESSION_GROUP: {
    json_object *jexpr = ast_object_typed("ParenthesizedExpression");

    json_object_object_add(jexpr, "expression",
                           ast_expression(expression->as.group->expression));

    return jexpr;
  }

  case EXPRESSION_ASSIGNMENT: {
    json_object *jexpr = ast_object_typed("AssignmentExpression");

    json_object_object_add(jexpr, "left",
                           ast_expression(expression->as.assignment->left));

    switch (expression->as.assignment->operator) {
    case TOKEN_ASSIGN:
      json_object_object_add(jexpr, "operator", json_object_new_string("="));
      break;

    case TOKEN_ASSIGN_INCREMENT:
      json_object_object_add(jexpr, "operator", json_object_new_string("+="));
      break;

    case TOKEN_ASSIGN_DECREMENT:
      json_object_object_add(jexpr, "operator", json_object_new_string("-="));
      break;

    case TOKEN_ASSIGN_DIVIDE:
      json_object_object_add(jexpr, "operator", json_object_new_string("/="));
      break;

    case TOKEN_ASSIGN_MULTIPLY:
      json_object_object_add(jexpr, "operator", json_object_new_string("*="));
      break;

    case TOKEN_ASSIGN_EXPONENTIATION:
      json_object_object_add(jexpr, "operator", json_object_new_string("**="));
      break;

    case TOKEN_ASSIGN_REMAINDER:
      json_object_object_add(jexpr, "operator", json_object_new_string("%="));
      break;

    case TOKEN_ASSIGN_BITWISE_AND:
      json_object_object_add(jexpr, "operator", json_object_new_string("&="));
      break;

    case TOKEN_ASSIGN_BITWISE_OR:
      json_object_object_add(jexpr, "operator", json_object_new_string("|="));
      break;

    case TOKEN_ASSIGN_BITWISE_XOR:
      json_object_object_add(jexpr, "operator", json_object_new_string("^="));
      break;

    case TOKEN_ASSIGN_BITWISE_LEFT_SHIFT:
      json_object_object_add(jexpr, "operator", json_object_new_string("<<="));
      break;

    case TOKEN_ASSIGN_BITWISE_RIGHT_SHIFT:
      json_object_object_add(jexpr, "operator", json_object_new_string(">>="));
      break;

    case TOKEN_ASSIGN_BITWISE_UNSIGNED_RIGHT_SHIFT:
      json_object_object_add(jexpr, "operator", json_object_new_string(">>>="));
      break;

    default:
      EXCEPTION("operator (%d) not supported for AssignmentExpression\n",
                expression->as.assignment->operator);
      break;
    }

    json_object_object_add(jexpr, "right",
                           ast_expression(expression->as.assignment->right));

    return jexpr;
  }

  case EXPRESSION_CALL: {
    json_object *jexpr = ast_object_typed("CallExpression");

    if (string_equal(expression->as.call->callee->as.identifier->name,
                     "print")) {
      json_object_object_add(jexpr, "callee", ast_identifier("console.log"));
    } else {
      json_object_object_add(jexpr, "callee",
                             ast_expression(expression->as.call->callee));
    }

    json_object *jargs = json_object_new_array();
    json_object_object_add(jexpr, "arguments", jargs);

    FOR_I(expression->as.call->length, {
      json_object_array_put_idx(
          jargs, i, ast_expression(expression->as.call->arguments[i]));
    });

    return jexpr;
  }

  case EXPRESSION_LITERAL: {
    switch (expression->as.literal->type) {
    case TOKEN_NIL:
      return ast_null_literal();

    case TOKEN_INTEGER:
    case TOKEN_FLOAT:
      return ast_numeric_literal(expression->as.literal->as.number,
                                 expression->as.literal->type);

    case TOKEN_BOOLEAN:
      return ast_boolean_literal(expression->as.literal->as.boolean);

    case TOKEN_STRING:
      return ast_string_literal(expression->as.literal->as.string);

    default:
      break;
    }

    break;
  }
  }

  LOG_DEBUG("not supported expression (%d)\n", expression->type);

  return NULL;
}

static json_object *ast_variable_declaration(VariableDeclaration *declaration) {
  json_object *jdeclaration = ast_object_typed("VariableDeclaration");
  json_object_object_add(jdeclaration, "kind", json_object_new_string("var"));

  json_object *jdecls = json_object_new_array();
  json_object_object_add(jdeclaration, "declarations", jdecls);

  json_object *jval_decl = json_object_new_object();
  json_object_object_add(jval_decl, "type",
                         json_object_new_string("VariableDeclarator"));
  json_object_array_put_idx(jdecls, 0, jval_decl);

  json_object_object_add(jval_decl, "id",
                         ast_identifier(declaration->id->name));

  if (declaration->init) {
    json_object_object_add(jval_decl, "init",
                           ast_expression(declaration->init));
  } else {
    json_object_object_add(jval_decl, "init", NULL);
  }

  return jdeclaration;
}

static json_object *ast_declaration(const DeclarationStatement *declaration) {
  switch (declaration->type) {
  case DECLARATION_STRUCT:
    return NULL;

  case DECLARATION_VARIABLE:
    return ast_variable_declaration(declaration->as.variable);

  case DECLARATION_FUNCTION: {
    json_object *jdeclaration = ast_object_typed("FunctionDeclaration");
    json_object_object_add(jdeclaration, "id",
                           ast_identifier(declaration->as.function->id->name));

    json_object *jparams = json_object_new_array();
    json_object_object_add(jdeclaration, "params", jparams);

    FOR_I(declaration->as.function->length, {
      const Identifier *param = declaration->as.function->params[i];

      json_object_array_put_idx(jparams, i, ast_identifier(param->name));
    });

    json_object_object_add(jdeclaration, "body",
                           ast_statement(declaration->as.function->body));

    return jdeclaration;
  }
  }
}

static json_object *ast_statement(const Statement *stmt) {
  switch (stmt->type) {

  case STATEMENT_RETURN: {
    json_object *jstmt = ast_object_typed("ReturnStatement");

    json_object_object_add(jstmt, "argument",
                           ast_expression(stmt->as.returns->argument));

    return jstmt;
  }

  case STATEMENT_FOR: {
    if (stmt->as.for_loop->right->type == EXPRESSION_IDENTIFIER) {
      json_object *jstmt = ast_object_typed("ForStatement");

      Expression *id_expr;
      {
        char *index_name = string_from(stmt->as.for_loop->left->name);
        string_concat(&index_name, "_$idx");
        id_expr = parser_identifier_expr(index_name);
      }

      DeclarationStatement *var_decl = parser_variable_decl(
          id_expr->as.identifier, parser_literal_expr_number(TOKEN_INTEGER, 0));

      json_object_object_add(jstmt, "init", ast_declaration(var_decl));

      Expression *test_expr = parser_binary_expr(
          id_expr, TOKEN_LESS,
          parser_member_expr(stmt->as.for_loop->right,
                             parser_identifier_expr("length"), false));

      json_object_object_add(jstmt, "test", ast_expression(test_expr));

      Expression *update_expr =
          parser_update_expr(TOKEN_INCREMENT, id_expr, false);

      json_object_object_add(jstmt, "update", ast_expression(update_expr));

      BlockStatement *next_block = NEW(BlockStatement);
      next_block->length = 0;
      next_block->body = NULL;

      FOR_I(stmt->as.for_loop->body->as.block->length + 1, {
        if (i == 0) {
          Expression *item_expr =
              parser_member_expr(stmt->as.for_loop->right, id_expr, true);

          Statement *item_decl = parser_declaration_stmt(
              parser_variable_decl(stmt->as.for_loop->left, item_expr));

          ARRAY_PUSH(Statement *, next_block->body, next_block->length,
                     item_decl);
        } else {
          ARRAY_PUSH(Statement *, next_block->body, next_block->length,
                     stmt->as.for_loop->body->as.block->body[index - 1]);
        }
      });

      stmt->as.for_loop->body->as.block = next_block;

      json_object_object_add(jstmt, "body",
                             ast_statement(stmt->as.for_loop->body));

      return jstmt;
    } else if (stmt->as.for_loop->right->type == EXPRESSION_BINARY) {
      json_object *jstmt = ast_object_typed("ForStatement");

      Expression *id_expr =
          parser_identifier_expr(stmt->as.for_loop->left->name);

      DeclarationStatement *var_decl = parser_variable_decl(
          id_expr->as.identifier, stmt->as.for_loop->right->as.binary->left);

      json_object_object_add(jstmt, "init", ast_declaration(var_decl));

      Expression *test_expr = parser_binary_expr(
          id_expr,
          stmt->as.for_loop->reverse ? TOKEN_GREATER_EQUAL : TOKEN_LESS_EQUAL,
          stmt->as.for_loop->right->as.binary->right);

      json_object_object_add(jstmt, "test", ast_expression(test_expr));

      Expression *update_expr = parser_update_expr(
          stmt->as.for_loop->reverse ? TOKEN_DECREMENT : TOKEN_INCREMENT,
          id_expr, false);

      json_object_object_add(jstmt, "update", ast_expression(update_expr));

      json_object_object_add(jstmt, "body",
                             ast_statement(stmt->as.for_loop->body));

      return jstmt;
    }

    return NULL;
  }

  case STATEMENT_IF: {
    json_object *jstmt = ast_object_typed("IfStatement");

    json_object_object_add(jstmt, "test",
                           ast_expression(stmt->as.condition->test));
    json_object_object_add(jstmt, "consequent",
                           ast_statement(stmt->as.condition->consequent));

    if (stmt->as.condition->alternate) {
      json_object_object_add(jstmt, "alternate",
                             ast_statement(stmt->as.condition->alternate));
    } else {
      json_object_object_add(jstmt, "alternate", json_object_new_null());
    }

    return jstmt;
  }

  case STATEMENT_WHILE: {
    json_object *jstmt = ast_object_typed("WhileStatement");

    json_object_object_add(jstmt, "test",
                           ast_expression(stmt->as.while_loop->test));
    json_object_object_add(jstmt, "body",
                           ast_statement(stmt->as.while_loop->body));

    return jstmt;
  }

  case STATEMENT_BLOCK: {
    json_object *jstmt = ast_object_typed("BlockStatement");

    json_object *jbody = json_object_new_array();
    json_object_object_add(jstmt, "body", jbody);

    size_t index = 0;
    FOR_I(stmt->as.block->length, {
      const Statement *inner_stmt = stmt->as.block->body[i];

      json_object *jitem = ast_statement(inner_stmt);

      if (jitem) {
        json_object_array_put_idx(jbody, index++, jitem);
      }
    });

    return jstmt;
  }

  case STATEMENT_DECLARATION: {
    return ast_declaration(stmt->as.declaration);
  }

  case STATEMENT_EMPTY:
    return ast_object_typed("EmptyStatement");

  case STATEMENT_BREAK:
    return ast_object_typed("BreakStatement");

  case STATEMENT_CONTINUE:
    return ast_object_typed("ContinueStatement");

  case STATEMENT_EXPRESSION: {
    json_object *jstmt = ast_object_typed("ExpressionStatement");

    json_object_object_add(jstmt, "expression",
                           ast_expression(stmt->as.expression->expression));
    return jstmt;
  }
  }
}

json_object *ast_program(ParserOutput *ast) {
  json_object *jprogram = ast_object_typed("Program");

  json_object *jbody = json_object_new_array();
  json_object_object_add(jprogram, "body", jbody);

  size_t index = 0;
  FOR_I(ast->length, {
    const Statement *stmt = ast->body[i];

    json_object *jitem = ast_statement(stmt);

    if (jitem) {
      json_object_array_put_idx(jbody, index++, jitem);
    }
  });

  return jprogram;
}
