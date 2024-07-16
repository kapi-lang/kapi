#include <string.h>

#include "javascript/ast.h"
#include "macros.h"
#include "parser.h"
#include "token.h"

static json_object *ast_generate_statement(const Statement *stmt);
static json_object *ast_generate_expression(const Expression *expression);

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

  json_object_object_add(jexpr, "left",
                         ast_generate_expression(expression->left));

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

  json_object_object_add(jexpr, "right",
                         ast_generate_expression(expression->right));

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
                         ast_generate_expression(expression->argument));

  return jexpr;
}

static json_object *ast_generate_expression(const Expression *expression) {
  switch (expression->type) {
  case EXPRESSION_MEMBER: {
    json_object *jexpr = ast_object_typed("MemberExpression");

    json_object_object_add(
        jexpr, "object",
        ast_generate_expression(expression->as.member->object));
    json_object_object_add(
        jexpr, "property",
        ast_generate_expression(expression->as.member->property));

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

    json_object_object_add(
        jexpr, "left", ast_generate_expression(expression->as.logical->left));

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

    json_object_object_add(
        jexpr, "right", ast_generate_expression(expression->as.logical->right));

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

      json_object_object_add(jprop, "key", ast_identifier(prop->key.name));
      json_object_object_add(jprop, "computed", json_object_new_boolean(false));
      json_object_object_add(jprop, "method", json_object_new_boolean(false));
      json_object_object_add(jprop, "method", json_object_new_boolean(false));
      json_object_object_add(jprop, "value",
                             ast_generate_expression(prop->value));
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

    json_object_object_add(
        jexpr, "argument",
        ast_generate_expression(expression->as.unary->right));

    return jexpr;
  }

  case EXPRESSION_BINARY:
    return ast_binary_expression(expression->as.binary);

  case EXPRESSION_GROUP: {
    json_object *jexpr = ast_object_typed("ParenthesizedExpression");

    json_object_object_add(
        jexpr, "expression",
        ast_generate_expression(expression->as.group->expression));

    return jexpr;
  }

  case EXPRESSION_ASSIGNMENT: {
    json_object *jexpr = ast_object_typed("AssignmentExpression");

    json_object_object_add(
        jexpr, "left",
        ast_generate_expression(expression->as.assignment->left));

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

    json_object_object_add(
        jexpr, "right",
        ast_generate_expression(expression->as.assignment->right));

    return jexpr;
  }

  case EXPRESSION_CALL: {
    json_object *jexpr = ast_object_typed("CallExpression");

    if (memcmp(expression->as.call->callee->as.identifier->name, "print", 5) ==
        0) {
      json_object_object_add(jexpr, "callee", ast_identifier("console.log"));
    } else {
      json_object_object_add(
          jexpr, "callee",
          ast_generate_expression(expression->as.call->callee));
    }

    json_object *jargs = json_object_new_array();
    json_object_object_add(jexpr, "arguments", jargs);

    FOR_I(expression->as.call->length, {
      json_object_array_put_idx(
          jargs, i, ast_generate_expression(expression->as.call->arguments[i]));
    });
    return jexpr;
  }

  case EXPRESSION_LITERAL: {
    switch (expression->as.literal->type) {
    case TOKEN_NIL:
#if defined(USE_UNDEFINED)
      return ast_identifier("undefined");
#else
      return ast_null_literal();
#endif

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

  json_object_object_add(jval_decl, "id", ast_identifier(declaration->id.name));

  if (declaration->init) {
    json_object_object_add(jval_decl, "init",
                           ast_generate_expression(declaration->init));
  } else {
    json_object_object_add(jval_decl, "init", NULL);
  }

  return jdeclaration;
}

static json_object *
ast_generate_declaration(const DeclarationStatement *declaration) {
  switch (declaration->type) {
#ifdef USE_INTERFACE
  case DECLARATION_INTERFACE: {
    json_object *jdeclaration = ast_object_typed("TSInterfaceDeclaration");
    json_object_object_add(jdeclaration, "id",
                           ast_identifier(declaration->as.interface->id.name));

    json_object *jbody = json_object_new_object();
    json_object_object_add(jdeclaration, "body", jbody);
    json_object_object_add(jbody, "type",
                           json_object_new_string("TSInterfaceBody"));

    json_object *jbody_arr = json_object_new_array();
    json_object_object_add(jbody, "body", jbody_arr);

    FOR_I(declaration->as.interface->length, {
      const PropertySignature *prop = declaration->as.interface->body[i];

      json_object *jprop = json_object_new_object();

      json_object_object_add(jprop, "type",
                             json_object_new_string("TSPropertySignature"));
      json_object_object_add(jprop, "key", ast_identifier(prop->key.name));
      json_object_object_add(jprop, "computed", json_object_new_boolean(false));
      json_object_object_add(jprop, "optional",
                             json_object_new_boolean(prop->optional));

      json_object *jtype = json_object_new_object();
      json_object_object_add(jprop, "typeAnnotation", jtype);

      json_object_object_add(jtype, "type",
                             json_object_new_string("TSTypeAnnotation"));

      json_object *jtype_annotation = json_object_new_object();
      json_object_object_add(jtype, "typeAnnotation", jtype_annotation);

      switch (prop->typeAnnotation->type) {
      case TYPE_ANNOTATION_INTEGER:
      case TYPE_ANNOTATION_FLOAT:
      case TYPE_ANNOTATION_NUMBER:
        json_object_object_add(jtype_annotation, "type",
                               json_object_new_string("TSNumberKeyword"));
        break;
      case TYPE_ANNOTATION_BOOLEAN:
        json_object_object_add(jtype_annotation, "type",
                               json_object_new_string("TSBooleanKeyword"));
        break;
      case TYPE_ANNOTATION_STRING:
        json_object_object_add(jtype_annotation, "type",
                               json_object_new_string("TSStringKeyword"));
        break;
      case TYPE_ANNOTATION_REFERENCE:
        json_object_object_add(jtype_annotation, "type",
                               json_object_new_string("TSTypeReference"));
        json_object_object_add(
            jtype_annotation, "typeName",
            ast_identifier(prop->typeAnnotation->typeName.name));
        break;
      }

      json_object_array_put_idx(jbody_arr, i, jprop);
    });

    return jdeclaration;
  }
#else
  case DECLARATION_STRUCT:
    return NULL;
#endif

  case DECLARATION_VARIABLE:
    return ast_variable_declaration(declaration->as.variable);

  case DECLARATION_FUNCTION: {
    json_object *jdeclaration = ast_object_typed("FunctionDeclaration");
    json_object_object_add(jdeclaration, "id",
                           ast_identifier(declaration->as.function->id.name));

    json_object *jparams = json_object_new_array();
    json_object_object_add(jdeclaration, "params", jparams);

    FOR_I(declaration->as.function->length, {
      const Identifier *param = declaration->as.function->params[i];

      json_object_array_put_idx(jparams, i, ast_identifier(param->name));
    });

    json_object_object_add(
        jdeclaration, "body",
        ast_generate_statement(declaration->as.function->body));

    return jdeclaration;
  }
  }
}

static json_object *ast_generate_statement(const Statement *stmt) {
  switch (stmt->type) {

  case STATEMENT_RETURN: {
    json_object *jstmt = ast_object_typed("ReturnStatement");

    json_object_object_add(jstmt, "argument",
                           ast_generate_expression(stmt->as.returns->argument));

    return jstmt;
  }

  case STATEMENT_FOR: {
    VariableDeclaration decl = (VariableDeclaration){
        .id = stmt->as.for_loop->left,
        .init = NULL,
    };

    if (stmt->as.for_loop->right->type == EXPRESSION_IDENTIFIER) {
      decl.id = (Identifier){
          .name = "i",
      };

      json_object *jstmt = ast_object_typed("ForStatement");

      Expression *init_expr = NEW(Expression);
      init_expr->type = EXPRESSION_LITERAL;
      init_expr->as.literal = NEW(Literal);
      init_expr->as.literal->type = TOKEN_INTEGER;
      init_expr->as.literal->as.number = 0;

      decl.init = init_expr;

      json_object_object_add(jstmt, "init", ast_variable_declaration(&decl));

      Expression *id_expr = NEW(Expression);
      id_expr->type = EXPRESSION_IDENTIFIER;
      id_expr->as.identifier = NEW(Identifier);
      id_expr->as.identifier->name = "i";

      Expression *length_expr = NEW(Expression);
      length_expr->type = EXPRESSION_MEMBER;
      length_expr->as.member = NEW(MemberExpression);
      length_expr->as.member->object = stmt->as.for_loop->right;
      length_expr->as.member->property = NEW(Expression);
      length_expr->as.member->property->type = EXPRESSION_IDENTIFIER;
      length_expr->as.member->property->as.identifier = NEW(Identifier);
      length_expr->as.member->property->as.identifier->name = "length";

      BinaryExpression *test_exp = NEW(BinaryExpression);
      test_exp->left = id_expr;
      test_exp->operator= TOKEN_LESS;
      test_exp->right = length_expr;

      json_object_object_add(jstmt, "test", ast_binary_expression(test_exp));

      UpdateExpression *update_exp = NEW(UpdateExpression);
      update_exp->prefix = false;
      update_exp->operator= TOKEN_INCREMENT;
      update_exp->argument = id_expr;

      json_object_object_add(jstmt, "update",
                             ast_update_expression(update_exp));

      BlockStatement *next_block = NEW(BlockStatement);
      next_block->length = 0;
      next_block->body = NULL;

      for (size_t i = 0; i < stmt->as.for_loop->body->as.block->length + 1;
           i++) {
        const size_t index = next_block->length++;

        next_block->body =
            ARRAY_RESIZE(Statement *, next_block->body, next_block->length);

        if (i == 0) {
          Statement *item_decl = NEW(Statement);

          item_decl->type = STATEMENT_DECLARATION;
          item_decl->as.declaration = NEW(DeclarationStatement);
          item_decl->as.declaration->type = DECLARATION_VARIABLE;
          item_decl->as.declaration->as.variable = NEW(VariableDeclaration);
          item_decl->as.declaration->as.variable->id = stmt->as.for_loop->left;

          item_decl->as.declaration->as.variable->init = NEW(Expression);
          item_decl->as.declaration->as.variable->init->type =
              EXPRESSION_MEMBER;
          item_decl->as.declaration->as.variable->init->as.member =
              NEW(MemberExpression);
          item_decl->as.declaration->as.variable->init->as.member->object =
              stmt->as.for_loop->right;
          item_decl->as.declaration->as.variable->init->as.member->property =
              id_expr;
          item_decl->as.declaration->as.variable->init->as.member->computed =
              true;

          next_block->body[index] = item_decl;
        } else {
          next_block->body[index] =
              stmt->as.for_loop->body->as.block->body[index - 1];
        }
      }

      stmt->as.for_loop->body->as.block = next_block;

      json_object_object_add(jstmt, "body",
                             ast_generate_statement(stmt->as.for_loop->body));

      return jstmt;
    } else if (stmt->as.for_loop->right->type == EXPRESSION_BINARY) {
      json_object *jstmt = ast_object_typed("ForStatement");

      decl.init = stmt->as.for_loop->right->as.binary->left;

      json_object_object_add(jstmt, "init", ast_variable_declaration(&decl));

      Expression *literal_expr = NEW(Expression);
      literal_expr->type = EXPRESSION_IDENTIFIER;
      literal_expr->as.identifier = NEW(Identifier);
      literal_expr->as.identifier->name = stmt->as.for_loop->left.name;

      BinaryExpression *test_exp = NEW(BinaryExpression);
      test_exp->left = literal_expr;
      test_exp->operator= stmt->as.for_loop->reverse ? TOKEN_GREATER_EQUAL
                                                     : TOKEN_LESS_EQUAL;
      test_exp->right = stmt->as.for_loop->right->as.binary->right;

      json_object_object_add(jstmt, "test", ast_binary_expression(test_exp));

      UpdateExpression *update_exp = NEW(UpdateExpression);
      update_exp->prefix = false;
      update_exp->operator= stmt->as.for_loop->reverse ? TOKEN_DECREMENT
                                                       : TOKEN_INCREMENT;
      update_exp->argument = literal_expr;

      json_object_object_add(jstmt, "update",
                             ast_update_expression(update_exp));

      json_object_object_add(jstmt, "body",
                             ast_generate_statement(stmt->as.for_loop->body));

      free(update_exp);
      free(test_exp);
      free(literal_expr->as.identifier);
      free(literal_expr);

      return jstmt;
    }

    return NULL;
  }

  case STATEMENT_IF: {
    json_object *jstmt = ast_object_typed("IfStatement");

    json_object_object_add(jstmt, "test",
                           ast_generate_expression(stmt->as.condition->test));
    json_object_object_add(
        jstmt, "consequent",
        ast_generate_statement(stmt->as.condition->consequent));

    if (stmt->as.condition->alternate) {
      json_object_object_add(
          jstmt, "alternate",
          ast_generate_statement(stmt->as.condition->alternate));
    } else {
      json_object_object_add(jstmt, "alternate", json_object_new_null());
    }

    return jstmt;
  }

  case STATEMENT_WHILE: {
    json_object *jstmt = ast_object_typed("WhileStatement");

    json_object_object_add(jstmt, "test",
                           ast_generate_expression(stmt->as.while_loop->test));
    json_object_object_add(jstmt, "body",
                           ast_generate_statement(stmt->as.while_loop->body));

    return jstmt;
  }

  case STATEMENT_BLOCK: {
    json_object *jstmt = ast_object_typed("BlockStatement");

    json_object *jbody = json_object_new_array();
    json_object_object_add(jstmt, "body", jbody);

    size_t index = 0;
    FOR_I(stmt->as.block->length, {
      const Statement *inner_stmt = stmt->as.block->body[i];

      json_object *jitem = ast_generate_statement(inner_stmt);

      if (jitem) {
        json_object_array_put_idx(jbody, index++, jitem);
      }
    });

    return jstmt;
  }

  case STATEMENT_DECLARATION: {
    return ast_generate_declaration(stmt->as.declaration);
  }

  case STATEMENT_EMPTY:
    return ast_object_typed("EmptyStatement");

  case STATEMENT_BREAK:
    return ast_object_typed("BreakStatement");

  case STATEMENT_CONTINUE:
    return ast_object_typed("ContinueStatement");

  case STATEMENT_EXPRESSION: {
    json_object *jstmt = ast_object_typed("ExpressionStatement");
    json_object_object_add(
        jstmt, "expression",
        ast_generate_expression(stmt->as.expression->expression));
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

    json_object *jitem = ast_generate_statement(stmt);

    if (jitem) {
      json_object_array_put_idx(jbody, index++, jitem);
    }
  });

  return jprogram;
}
