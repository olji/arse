#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "arse.h"

#include "testing.h"
#include "tests.h"

int exec_command(struct arse *editor, char mode);
int main(){
  int total=0,passing=0;

  test(&string_load_and_get);
  fprintf(stderr, "====Insertion test====\n");
  test(&insert_start);
  test(&insert_middle);
  test(&insert_inside_piece);
  test(&insert_end);

  fprintf(stderr, "====Undo/redo test====\n");
  test(&undo_first);
  test(&undo_second);
  test(&undo_redo);
  test(&redo_undo);

  fprintf(stderr, "====Removal test====\n");
  test(&delete_nothing);
  test(&delete_inside_piece);
  test(&delete_whole_piece);
  test(&delete_across_piece);

  fprintf(stderr, "====Subeditor test====\n");
  test(&arsify_piece);

  print_test_result();

  // Multi-line stuff,
  /*
  // Arsify
  arse_piece_to_arse(editor, 0, 0, 5, true);
  assert(editor, "firstOn first line\nsecoOn second linend");
  arse_piece_to_arse(editor, 0, 8, 5, true);
  assert(editor, "firstOn first line\nsecoOn second linend");
  //arse_piece_to_arse(editor, 0, 6, 1, true);
  //assert(editor, "firstOn first line\nsecoOn second linend");
  arse_insert_at_line(editor, 0, 1, "Hello");
  assert(editor, "fHelloirstOn fHelloirst line\nsecoOn second linend");
  arse_remove_at_line(editor, 0, 2, 2);
  assert(editor, "fHloirstOn fHloirst line\nsecoOn second linend");
  arse_insert_at_line(editor, 0, 14, "Inside second arse");
  assert(editor, "fHlInside second arseoirstOn fHlInside second arseoirst line\nsecoOn second linend");

  arse_delete(editor);
  */
  return 0;
}
