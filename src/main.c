#include <stdio.h>

#include "file.h"
#include "lexer.h"
#include "macros.h"
#include "parser.h"

#include "js/ast.h"

int main(int argc, char const *argv[]) {
  const char *filename = argv[1];

  LOG_INFO("file: %s\n", filename);

  const char *source = file_read(filename);

  LexerOutput *tokens = lexer_run(source);
  ParserOutput *ast = parser_run(tokens);

  ast_generate(ast);

  return 0;
}
