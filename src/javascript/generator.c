#include <string.h>

#include "javascript/ast.h"
#include "javascript/generator.h"
#include "macros.h"
#include "system/file.h"
#include "system/folder.h"

#define ROOT_DIR ".kapi"
#define FILENAME "javascript.ast.json"
#define OUTPUT_PATH (ROOT_DIR "/" FILENAME)

void javascript_generate(ParserOutput *ast) {
  json_object *jprogram = ast_program(ast);

  printf("JSON:\n\n%s\n\n",
         json_object_to_json_string_ext(jprogram, JSON_C_TO_STRING_PRETTY |
                                                      JSON_C_TO_STRING_COLOR));

  if (!folder_create(ROOT_DIR)) {
    EXCEPTION("[ast] error to create folder .kapi");
  }

  const char *content =
      json_object_to_json_string_ext(jprogram, JSON_C_TO_STRING_PLAIN);

  FILE *file = file_open(OUTPUT_PATH);

  file_write(file, content, sizeof(char), strlen(content));

  if (!file_close(file)) {
    EXCEPTION("[ast] error to generate file");
  }

  json_object_put(jprogram);
}
