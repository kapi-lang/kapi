#include "file.h"
#include "lexer.h"
#include "macros.h"
#include "parser.h"
#include "token.h"
#include <stdio.h>

void ast_print_expression(Expression *expression, size_t level) {
  FOR_I(level, {
    // Indent
    LOG_INFO("   ");
  });

  switch (expression->type) {
  case EXPRESSION_UNARY: {
    LOG_INFO("UnaryExpression(%s) with:\n",
             expression->as.unary->operator.lexeme);

    ast_print_expression(expression->as.unary->right, level + 1);

    LOG_INFO("\n");
    break;
  }

  case EXPRESSION_BINARY: {
    LOG_INFO("BinaryExpression(%s) with:\n",
             expression->as.binary->operator.lexeme);

    ast_print_expression(expression->as.binary->left, level + 1);
    ast_print_expression(expression->as.binary->right, level + 1);

    LOG_INFO("\n");
    break;
  }

  case EXPRESSION_GROUP: {
    LOG_INFO("GroupExpression with:\n");

    ast_print_expression(expression->as.group->expression, level + 1);

    LOG_INFO("\n");
    break;
  }

  case EXPRESSION_ASSIGNMENT: {
    LOG_INFO("AssignmentExpression(%s) with:\n",
             expression->as.assignment->left.lexeme);

    ast_print_expression(expression->as.assignment->right, level + 1);

    LOG_INFO("\n");
    break;
  }

  case EXPRESSION_CALL: {
    LOG_INFO("CallExpression(%s) with:\n", expression->as.call->callee.lexeme);

    FOR_I(expression->as.call->length, {
      ast_print_expression(expression->as.call->arguments[i], level + 1);
    });

    LOG_INFO("\n");

    break;
  }

  case EXPRESSION_LITERAL: {
    switch (expression->as.literal->type) {
    case TOKEN_NIL:
      LOG_INFO("LiteralExpression(nil)\n");
      break;

    case TOKEN_INTEGER:
    case TOKEN_FLOAT:
      LOG_INFO("LiteralExpression(%f)\n", expression->as.literal->as.number);
      break;

    case TOKEN_BOOLEAN:
      LOG_INFO("LiteralExpression(%s)\n",
               expression->as.literal->as.boolean ? "true" : "false");
      break;

    case TOKEN_STRING:
      LOG_INFO("LiteralExpression(%s)\n", expression->as.literal->as.string);
      break;

    default:
      break;
    }

    break;
  }
  }
}

void ast_print_declaration(Declaration *declaration, size_t level) {
  FOR_I(level, {
    // Indent
    LOG_INFO("   ");
  });

  switch (declaration->type) {
  case DECLARATION_VARIABLE: {
    if (declaration->as.variable->init) {
      LOG_INFO("VariableDeclaration(%s) with:\n",
               declaration->as.variable->id.lexeme);

      ast_print_expression(declaration->as.variable->init, level + 1);
    } else {
      LOG_INFO("VariableDeclaration(%s)\n",
               declaration->as.variable->id.lexeme);
    }

    LOG_INFO("\n");

    break;
  }

  case DECLARATION_FUNCTION: {
    break;
  }
  }
}

void ast_print(ParserOutput *ast) {
  FOR_I(ast->length, {
    const ProgramBody body = ast->body[i];

    if (body.type == PROGRAM_BODY_DECLARATION)
      ast_print_declaration(body.as.declaration, 0);
    else if (body.type == PROGRAM_BODY_STATEMENT)
      ast_print_expression(body.as.statement->as.expression->expression, 0);
  })
}

int main(int argc, char const *argv[]) {
  const char *filename = argv[1];

  LOG_INFO("file: %s\n", filename);

  const char *source = file_read(filename);

  LexerOutput *tokens = lexer_run(source);

  LOG_INFO("\n\nStats:\n Tokens Length = %lu\n", tokens->length);

  ParserOutput *ast = parser_run(tokens);
  LOG_INFO("\n\nStats:\n Program Body Length = %lu\n\n\nAST: \n\n",
           ast->length);
  ast_print(ast);

  return 0;
}
