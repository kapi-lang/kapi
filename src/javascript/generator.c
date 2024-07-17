#include <string.h>

#include "javascript/ast.h"
#include "javascript/generator.h"
#include "macros.h"
#include "system/file.h"
#include "system/folder.h"

#define ROOT_DIR ".kapi"
#define FILENAME "javascript.ast"
#define OUTPUT_PATH (ROOT_DIR "/" FILENAME)

void javascript_generate(ParserOutput *ast) {
  json_object *jprogram = ast_program(ast);

  printf("%s\n",
         json_object_to_json_string_ext(jprogram, JSON_C_TO_STRING_PLAIN));

  if (!folder_create(ROOT_DIR)) {
    EXCEPTION("[javascript] error to create folder .kapi");
  }

  const char *content = json_object_to_json_string_ext(
      jprogram, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_SPACED);

  FILE *file = file_open(OUTPUT_PATH);

  (void)file_write(file, content, sizeof(char), strlen(content));

  if (!file_close(file)) {
    EXCEPTION("[javascript] error to write the ast");
  }

  json_object_put(jprogram);
}
