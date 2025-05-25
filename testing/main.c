#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "arse.h"

#include "testing.h"
#include "tests.h"

int exec_command(struct arse *editor, char mode);
int main(int argc, char **argv){
  if(argc < 2){
    int total=0,passing=0;

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

  } else {
    struct arse *editor = malloc(sizeof(struct arse));
    arse_init(editor);
    arse_open_file(editor, argv[1]);
    int result = 0;
    while(!feof(stdin) && result != 2){
      printf(": ");
      char command = getc(stdin);
      if(command != '\n'){
        result = exec_command(editor, command);
        getc(stdin);
      } else {
        printf("\n");
      }
    }
    arse_delete(editor);
  }
}
int exec_command(struct arse *editor, char mode){
  size_t line = 0;
  size_t length = 0;
  size_t index = 0;
  char *text = malloc(sizeof(*text) * 254);
  struct arse_string *str;
  struct arse_buffer *buf;

  switch(mode){
  case 'i':
    scanf("%zu", &line);
    scanf("%zu", &index);
    scanf("%s", text);
    arse_insert_at_line(editor, line, index, text);
    break;
  case 'd':
    scanf("%zu", &line);
    scanf("%zu", &index);
    scanf("%zu", &length);
    arse_remove_at_line(editor, line, index, length);
    break;
  case 'u':
    arse_undo(editor);
    break;
  case 'p':
    buf = arse_get_buffer(editor);
    arse_buffer_delete(buf);
    str = arse_get_string(editor);
    printf("%s\n", str->string);
    break;
  case 'w':
    arse_save(editor, editor->filename);
    break;
  case 'q':
    return 2;
  default:
    printf("?\n");
    return 0;
  }
  return 1;
}
