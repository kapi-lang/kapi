#ifndef parser_h
#define parser_h

#include <stdbool.h>
#include <stddef.h>

#include "lexer.h"
#include "token.h"

//// Expression

typedef struct Expression Expression;

typedef struct Identifier {
  const char *name;
} Identifier;

typedef struct Literal {
  TokenType type;
  union {
    bool boolean;
    double number;
    const char *string;
  } as;
} Literal;

typedef struct UpdateExpression {
  const char *operator;
  Expression *argument;
  bool prefix;
} UpdateExpression;

typedef struct UnaryExpression {
  const char *operator;
  Expression *right;
  bool prefix;
} UnaryExpression;

typedef struct LogicalExpression {
  Expression *left;
  const char *operator;
  Expression *right;
} LogicalExpression;

typedef struct BinaryExpression {
  Expression *left;
  const char *operator;
  Expression *right;
} BinaryExpression;

typedef struct AssignmentExpression {
  Expression *left;
  Expression *right;
} AssignmentExpression;

typedef struct GroupExpression {
  Expression *expression;
} GroupExpression;

typedef struct CallExpression {
  Identifier callee;
  Expression **arguments;
  size_t length;
} CallExpression;

typedef struct ObjectProperty {
  Identifier key;
  Expression *value;
} ObjectProperty;

typedef struct ObjectExpression {
  ObjectProperty **properties;
  size_t length;
} ObjectExpression;

typedef enum ExpressionType {
  EXPRESSION_IDENTIFIER,
  EXPRESSION_LITERAL,
  EXPRESSION_UNARY,
  EXPRESSION_UPDATE,
  EXPRESSION_LOGICAL,
  EXPRESSION_BINARY,
  EXPRESSION_ASSIGNMENT,
  EXPRESSION_GROUP,
  EXPRESSION_CALL,
  EXPRESSION_OBJECT,
} ExpressionType;

struct Expression {
  ExpressionType type;
  union {
    Identifier *identifier;
    Literal *literal;
    UnaryExpression *unary;
    UpdateExpression *update;
    LogicalExpression *logical;
    BinaryExpression *binary;
    GroupExpression *group;
    AssignmentExpression *assignment;
    CallExpression *call;
    ObjectExpression *object;
  } as;
};

//// Declaration

typedef struct DeclarationStatement DeclarationStatement;

typedef enum TypeAnnotationTypes {
  TYPE_ANNOTATION_REFERENCE,
  TYPE_ANNOTATION_NUMBER,
  TYPE_ANNOTATION_BOOLEAN,
  TYPE_ANNOTATION_STRING,
} TypeAnnotationTypes;

typedef struct TypeAnnotation {
  TypeAnnotationTypes type;
  Identifier typeName;
} TypeAnnotation;

typedef struct PropertySignature {
  Identifier key;
  bool optional;
  TypeAnnotation *typeAnnotation;
} PropertySignature;

typedef struct InterfaceDeclaration {
  Identifier id;
  PropertySignature **body;
  size_t length;
} InterfaceDeclaration;

typedef struct VariableDeclarator {
  Identifier id;
  Expression *init;
} VariableDeclarator;

typedef struct VariableDeclaration {
  VariableDeclarator **declarations;
  size_t length;
} VariableDeclaration;

typedef struct FunctionDeclaration FunctionDeclaration;

typedef enum DeclarationType {
  DECLARATION_INTERFACE,
  DECLARATION_VARIABLE,
  DECLARATION_FUNCTION,
} DeclarationType;

struct DeclarationStatement {
  DeclarationType type;
  union {
    InterfaceDeclaration *interface;
    VariableDeclaration *variable;
    FunctionDeclaration *function;
  } as;
};

//// Statement

typedef struct Statement Statement;

typedef struct ExpressionStatement {
  Expression *expression;
} ExpressionStatement;

typedef struct BlockStatement {
  Statement **body;
  size_t length;
} BlockStatement;

typedef enum StatementType {
  STATEMENT_EMPTY,
  STATEMENT_DECLARATION,
  STATEMENT_EXPRESSION,
  STATEMENT_BLOCK,
} StatementType;

struct Statement {
  StatementType type;
  union {
    DeclarationStatement *declaration;
    ExpressionStatement *expression;
    BlockStatement *block;
  } as;
};

//// Program

typedef struct ParserOutput {
  Statement **body;
  size_t length;
} ParserOutput;

ParserOutput *parser_run(LexerOutput *tokens);

#endif
