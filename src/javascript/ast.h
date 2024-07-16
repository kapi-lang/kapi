#ifndef javascript_ast_h
#define javascript_ast_h

#include <json.h>

#include "parser.h"

json_object *ast_program(ParserOutput *parser);

#endif
