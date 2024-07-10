#ifndef parser_h
#define parser_h

#include <stdbool.h>
#include <stddef.h>

#include "lexer.h"
#include "token.h"

typedef struct Expression Expression;
typedef struct DeclarationStatement DeclarationStatement;
typedef struct Statement Statement;

//// Expression

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
  TokenType operator;
  Expression *argument;
  bool prefix;
} UpdateExpression;

typedef struct UnaryExpression {
  TokenType operator;
  Expression *right;
  bool prefix;
} UnaryExpression;

typedef struct LogicalExpression {
  Expression *left;
  TokenType operator;
  Expression *right;
} LogicalExpression;

typedef struct BinaryExpression {
  Expression *left;
  TokenType operator;
  Expression *right;
} BinaryExpression;

typedef struct AssignmentExpression {
  Expression *left;
  TokenType operator;
  Expression *right;
} AssignmentExpression;

typedef struct GroupExpression {
  Expression *expression;
} GroupExpression;

typedef struct CallExpression {
  Expression *callee;
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

typedef struct MemberExpression {
  Expression *object;
  Expression *property;
} MemberExpression;

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
  EXPRESSION_MEMBER,
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
    MemberExpression *member;
  } as;
};

//// Declaration

typedef enum TypeAnnotationTypes {
  TYPE_ANNOTATION_REFERENCE,
  TYPE_ANNOTATION_NUMBER,
  TYPE_ANNOTATION_INTEGER,
  TYPE_ANNOTATION_FLOAT,
  TYPE_ANNOTATION_BOOLEAN,
  TYPE_ANNOTATION_STRING,
  TYPE_ANNOTATION_VOID,
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

typedef struct FunctionParamDeclaration {
  Identifier id;
  TypeAnnotation *typeAnnotation;
} FunctionParamDeclaration;

typedef struct FunctionDeclaration {
  Identifier id;
  TypeAnnotation *returnType;
  Statement *body;
  FunctionParamDeclaration **params;
  size_t length;
} FunctionDeclaration;

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

typedef struct ExpressionStatement {
  Expression *expression;
} ExpressionStatement;

typedef struct BlockStatement {
  Statement **body;
  size_t length;
} BlockStatement;

typedef struct WhileStatement {
  Expression *test;
  Statement *body;
} WhileStatement;

typedef struct IfStatement {
  Expression *test;
  Statement *consequent;
  Statement *alternate;
} IfStatement;

typedef struct ForStatement {
  Expression *left;
  Expression *right;
  Statement *body;
  bool reverse;
} ForStatement;

typedef struct ReturnStatement {
  Expression *argument;
} ReturnStatement;

typedef enum StatementType {
  STATEMENT_EMPTY,
  STATEMENT_BREAK,
  STATEMENT_CONTINUE,
  STATEMENT_DECLARATION,
  STATEMENT_EXPRESSION,
  STATEMENT_BLOCK,
  STATEMENT_WHILE,
  STATEMENT_IF,
  STATEMENT_FOR,
  STATEMENT_RETURN,
} StatementType;

struct Statement {
  StatementType type;
  union {
    ReturnStatement *returns;
    ForStatement *for_loop;
    WhileStatement *while_loop;
    IfStatement *condition;
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
