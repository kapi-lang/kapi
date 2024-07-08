#ifndef parser_h
#define parser_h

#include <stdbool.h>
#include <stddef.h>

#include "lexer.h"
#include "token.h"

//// Expression

typedef struct Expression Expression;

typedef struct LiteralExpression {
  TokenType type;
  union {
    bool boolean;
    double number;
    const char *string;
  } as;
} LiteralExpression;

typedef struct UnaryExpression {
  Token operator;
  Expression *right;
} UnaryExpression;

typedef struct BinaryExpression {
  Expression *left;
  Token operator;
  Expression *right;
} BinaryExpression;

typedef struct AssignmentExpression {
  Token left;
  Expression *right;
} AssignmentExpression;

typedef struct GroupExpression {
  Expression *expression;
} GroupExpression;

typedef struct CallExpression {
  Token callee;
  Expression **arguments;
  size_t length;
} CallExpression;

typedef enum ExpressionType {
  EXPRESSION_LITERAL,
  EXPRESSION_UNARY,
  EXPRESSION_BINARY,
  EXPRESSION_ASSIGNMENT,
  EXPRESSION_GROUP,
  EXPRESSION_CALL,
} ExpressionType;

struct Expression {
  ExpressionType type;
  union {
    LiteralExpression *literal;
    UnaryExpression *unary;
    BinaryExpression *binary;
    GroupExpression *group;
    AssignmentExpression *assignment;
    CallExpression *call;
  } as;
};

//// Statement

typedef struct Statement Statement;

typedef struct ExpressionStatement {
  Expression *expression;
} ExpressionStatement;

typedef enum StatementType {
  STATEMENT_EXPRESSION,
} StatementType;

struct Statement {
  StatementType type;
  union {
    ExpressionStatement *expression;
  } as;
};

//// Declaration

typedef struct Declaration Declaration;

typedef struct VariableDeclaration {
  Token id;
  Expression *init;
} VariableDeclaration;

typedef struct FunctionDeclaration FunctionDeclaration;

typedef enum DeclarationType {
  DECLARATION_VARIABLE,
  DECLARATION_FUNCTION,
} DeclarationType;

struct Declaration {
  DeclarationType type;
  union {
    VariableDeclaration *variable;
    FunctionDeclaration *function;
  } as;
};

//// Program

typedef enum ProgramBodyType {
  PROGRAM_BODY_DECLARATION,
  PROGRAM_BODY_STATEMENT,
} ProgramBodyType;

typedef struct ProgramBody {
  ProgramBodyType type;
  union {
    Declaration *declaration;
    Statement *statement;
  } as;
} ProgramBody;

typedef struct ParserOutput {
  ProgramBody *body;
  size_t length;
} ParserOutput;

ParserOutput *parser_run(LexerOutput *tokens);

#endif
