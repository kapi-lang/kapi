#ifndef type_checker_h
#define type_checker_h

#include <stddef.h>

#include "parser.h"

typedef struct TypeCheckerError {
  const char *message;
  size_t line;
  size_t column;
} TypeCheckerError;

typedef struct TypeCheckerOutput {
  TypeCheckerError **errors;
  size_t length;
} TypeCheckerOutput;

TypeCheckerOutput *typechecker_run(ParserOutput *ast);

#endif
