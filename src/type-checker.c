#include <stdbool.h>

#include "macros.h"
#include "parser.h"
#include "token.h"
#include "type-checker.h"
#include "type/hashtable.h"
#include "type/string.h"

typedef struct Scope {
  bool global;
  struct Scope *above;
  HashTable *declarations;
} Scope;

typedef struct TypeChecker {
  Scope **scopes;
} TypeChecker;

TypeChecker state;

static Scope *scope_create(bool global, Scope *above) {
  Scope *scope = NEW(Scope);
  scope->global = global;
  scope->above = above;
  scope->declarations = hashtable_create();

  return scope;
}

static void typechecker_add_error(TypeCheckerOutput *output,
                                  const char *message) {
  TypeCheckerError *error = NEW(TypeCheckerError);
  error->message = message;

  ARRAY_PUSH(TypeCheckerError *, output->errors, output->length, error);
}

TypeCheckerOutput *typechecker_run(ParserOutput *ast) {
  TypeCheckerOutput *output = NEW(TypeCheckerOutput);
  Scope *global_scope = scope_create(true, NULL);

  state.scopes = ARRAY_RESIZE(Scope *, state.scopes, 1);
  state.scopes[0] = global_scope;

  FOR_I(ast->length, {
    const Statement *stmt = ast->body[i];

    if (stmt->type == STATEMENT_DECLARATION) {
      const DeclarationStatement *decl_stmt = stmt->as.declaration;

      if (decl_stmt->type == DECLARATION_VARIABLE) {
        const VariableDeclaration *var_decl = decl_stmt->as.variable;

        if (hashtable_get(global_scope->declarations, var_decl->id->name)) {
          typechecker_add_error(output, "can't re-declare a variable");

          break;
        }

        if (var_decl->id->typeAnnotation) {
          hashtable_set(global_scope->declarations, var_decl->id->name,
                        var_decl->id->typeAnnotation);
        }
      }
    } else if (stmt->type == STATEMENT_EXPRESSION) {
      const ExpressionStatement *expr_stmt = stmt->as.expression;

      if (expr_stmt->expression->type == EXPRESSION_ASSIGNMENT) {
        const AssignmentExpression *assign_expr =
            expr_stmt->expression->as.assignment;

        const TypeAnnotation *type_annotation = hashtable_get(
            global_scope->declarations, assign_expr->left->as.identifier->name);

        if (!type_annotation) {
          typechecker_add_error(output, "can't assign an undeclared variable");

          break;
        }

        if (assign_expr->right->type == EXPRESSION_LITERAL) {
          if (type_annotation->type != assign_expr->right->as.literal->type) {
            typechecker_add_error(
                output,
                string_format(
                    "can't assign type '%s' expected '%s'",
                    token_to_string(assign_expr->right->as.literal->type, NULL),
                    token_to_string(type_annotation->type,
                                    type_annotation->typeName)));

            break;
          }
        }
      }
    }
  });

  return output;
}
