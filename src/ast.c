#include <json.h>
#include <string.h>

#include "ast.h"
#include "json_object.h"
#include "json_types.h"
#include "macros.h"
#include "parser.h"

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

static json_object *ast_numeric_literal(double value) {
  json_object *jroot = ast_object_typed("NumericLiteral");

  json_object_object_add(jroot, "value", json_object_new_double(value));

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

static json_object *ast_generate_expression(const Expression *expression) {
  switch (expression->type) {
  case EXPRESSION_UPDATE: {
    json_object *jexpr = ast_object_typed("UpdateExpression");

    json_object_object_add(
        jexpr, "prefix",
        json_object_new_boolean(expression->as.update->prefix));
    json_object_object_add(
        jexpr, "operator",
        json_object_new_string(expression->as.update->operator));
    json_object_object_add(
        jexpr, "argument",
        ast_generate_expression(expression->as.update->argument));

    return jexpr;
  }

  case EXPRESSION_LOGICAL: {
    json_object *jexpr = ast_object_typed("LogicalExpression");

    json_object_object_add(
        jexpr, "left", ast_generate_expression(expression->as.binary->left));

    char *op = "";

    if (memcmp(expression->as.binary->operator, "and", 3) == 0) {
      op = "&&";
    } else if (memcmp(expression->as.binary->operator, "or", 2) == 0) {
      op = "||";
    }

    json_object_object_add(jexpr, "operator", json_object_new_string(op));
    json_object_object_add(
        jexpr, "right", ast_generate_expression(expression->as.binary->right));

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
    json_object_object_add(
        jexpr, "operator",
        json_object_new_string(expression->as.unary->operator));
    json_object_object_add(
        jexpr, "argument",
        ast_generate_expression(expression->as.unary->right));

    return jexpr;
  }

  case EXPRESSION_BINARY: {
    json_object *jexpr = ast_object_typed("BinaryExpression");

    json_object_object_add(
        jexpr, "left", ast_generate_expression(expression->as.binary->left));

    char *op = "";

    if (memcmp(expression->as.binary->operator, "eq", 2) == 0) {
      op = "===";
    } else if (memcmp(expression->as.binary->operator, "not", 3) == 0) {
      op = "!==";
    } else if (memcmp(expression->as.binary->operator, "gte", 3) == 0) {
      op = ">=";
    } else if (memcmp(expression->as.binary->operator, "gt", 2) == 0) {
      op = ">";
    } else if (memcmp(expression->as.binary->operator, "lte", 3) == 0) {
      op = "<=";
    } else if (memcmp(expression->as.binary->operator, "lt", 2) == 0) {
      op = "<";
    } else {
      op = (char *)expression->as.binary->operator;
    }

    json_object_object_add(jexpr, "operator", json_object_new_string(op));
    json_object_object_add(
        jexpr, "right", ast_generate_expression(expression->as.binary->right));

    return jexpr;
  }

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
    json_object_object_add(jexpr, "operator", json_object_new_string("="));
    json_object_object_add(
        jexpr, "right",
        ast_generate_expression(expression->as.assignment->right));

    return jexpr;
  }

  case EXPRESSION_CALL: {
    json_object *jexpr = ast_object_typed("CallExpression");
    json_object_object_add(jexpr, "callee",
                           ast_identifier(expression->as.call->callee.name));

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
#ifdef USE_UNDEFINED
      return ast_identifier("undefined");
#else
      return ast_null_literal();
#endif

    case TOKEN_INTEGER:
    case TOKEN_FLOAT:
      return ast_numeric_literal(expression->as.literal->as.number);

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
  case DECLARATION_INTERFACE:
    return NULL;
#endif

  case DECLARATION_VARIABLE: {
    json_object *jdeclaration = ast_object_typed("VariableDeclaration");
    json_object_object_add(jdeclaration, "kind", json_object_new_string("var"));

    json_object *jdecls = json_object_new_array();
    json_object_object_add(jdeclaration, "declarations", jdecls);

    FOR_I(declaration->as.variable->length, {
      const VariableDeclarator *val_decl =
          declaration->as.variable->declarations[i];

      json_object *jval_decl = json_object_new_object();
      json_object_object_add(jval_decl, "type",
                             json_object_new_string("VariableDeclarator"));
      json_object_array_put_idx(jdecls, i, jval_decl);

      json_object_object_add(jval_decl, "id",
                             ast_identifier(val_decl->id.name));

      if (val_decl->init) {
        json_object_object_add(jval_decl, "init",
                               ast_generate_expression(val_decl->init));
      } else {
        json_object_object_add(jval_decl, "init", NULL);
      }
    });

    return jdeclaration;
  }

  case DECLARATION_FUNCTION:
    return NULL;
  }
}

static json_object *ast_generate_statement(const Statement *stmt) {
  switch (stmt->type) {
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

  case STATEMENT_EMPTY: {
    json_object *jstmt = ast_object_typed("EmptyStatement");
    return jstmt;
  }

  case STATEMENT_EXPRESSION: {
    json_object *jstmt = ast_object_typed("ExpressionStatement");
    json_object_object_add(
        jstmt, "expression",
        ast_generate_expression(stmt->as.expression->expression));
    return jstmt;
  }
  }
}

void ast_generate(ParserOutput *ast) {
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

  printf("JSON:\n\n%s\n\n",
         json_object_to_json_string_ext(jprogram, JSON_C_TO_STRING_PRETTY |
                                                      JSON_C_TO_STRING_COLOR));

  if (json_object_to_file("ast.json", jprogram)) {
    EXCEPTION("[ast] error to generate file");
  }

  json_object_put(jprogram);
}
