#include <stdlib.h>
#include <string.h>
#include "src/arse.h"
#include "src/subarsetable.h"
#include "src/table.h"
#include "src/piece.h"

int main(){
  char *str = "ARSEs representing strings editorially";
  char *str2 = malloc(strlen(str));
  strcpy(str2, str);
  struct arse *editor = arse_create(str2, 0);
  arse_piece_to_arse(editor, 0, 0, 4, true);
  editor->lines[0]->begin->next->arse = editor;
  printf("%s\n", arse_get_string(editor)->string);
  return 0;
}
