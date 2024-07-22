#include "lexer.h"
#include "macros.h"
#include "parser.h"
#include "system/file.h"
#include "type-checker.h"
#include "type/string.h"

#include "javascript/generator.h"

/**
 * kapi javascript <filename>
 * kapi brightscript <filename>
 */
int main(int argc, char const *argv[]) {
  const char *platform = argv[1];
  const char *filename = argv[2];

  LOG_INFO("platform: %s\n", platform);
  LOG_INFO("file: %s\n", filename);

  const char *source = file_read(filename);

  LexerOutput *tokens = lexer_run(source);
  ParserOutput *ast = parser_run(tokens);

  // TypeCheckerOutput *errors = typechecker_run(ast);

  // FOR_I(errors->length, {
  //   const TypeCheckerError *error = errors->errors[i];

  //   LOG_ERROR("[Error] %s at %lu:%lu\n", error->message, error->line,
  //             error->column);
  // })

  // if (errors->length > 0) {
  //   return 1;
  // }

  if (string_equal(platform, "javascript")) {
    javascript_generate(ast);
  }

  return 0;
}
