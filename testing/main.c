#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "arse.h"

#include "testing.h"

int exec_command(struct arse *editor, char mode);
int main(int argc, char **argv){
  if(argc < 2){
    char *c = "Testing";
    char *str = malloc(sizeof(char) * strlen(c) + 1);
    strcpy(str, c);

    struct arse *editor = malloc(sizeof(struct arse));
    arse_init(editor);
    arse_load_string(editor, str);
    fprintf(stderr, "###SINGLE LINE TESTS###\n");
    fprintf(stderr, "====Insertion test====\n");
    arse_insert(editor, 4, "<SOMETHING>");
    assert(editor, "Test<SOMETHING>ing");
    arse_insert(editor, 0, "<Pere>");
    assert(editor, "<Pere>Test<SOMETHING>ing");
    arse_insert(editor, 2, "<Hello>");
    assert(editor, "<P<Hello>ere>Test<SOMETHING>ing");
    arse_insert(editor, 4, "<Last one>");
    assert(editor, "<P<H<Last one>ello>ere>Test<SOMETHING>ing");
    arse_insert(editor, 11, "<Kidding>");
    assert(editor, "<P<H<Last o<Kidding>ne>ello>ere>Test<SOMETHING>ing");
    arse_insert(editor, 2, "New\nline");
    assert(editor, "<PNew\nline<H<Last o<Kidding>ne>ello>ere>Test<SOMETHING>ing");

    fprintf(stderr, "====Undo/redo test====\n");
    arse_undo(editor);
    assert(editor, "<P<H<Last o<Kidding>ne>ello>ere>Test<SOMETHING>ing");
    arse_undo(editor);
    assert(editor, "<P<H<Last one>ello>ere>Test<SOMETHING>ing");
    arse_undo(editor);
    assert(editor, "<P<Hello>ere>Test<SOMETHING>ing");
    /* FIXME: Redo behavior not working, potentially part pushed from undo that is broken */
    arse_redo(editor);
    assert(editor, "<P<H<Last one>ello>ere>Test<SOMETHING>ing");
    arse_undo(editor);
    assert(editor, "<P<Hello>ere>Test<SOMETHING>ing");

    fprintf(stderr, "====Removal test====\n");
    /* Delete across pieces */
    arse_remove(editor, 4, 10);
    assert(editor, "<P<Hest<SOMETHING>ing");
    /* Delete whole piece */
    arse_remove(editor, 0, 2);
    assert(editor, "<Hest<SOMETHING>ing");
    /* Delete nothing */
    arse_remove(editor, 6, 0);
    assert(editor, "<Hest<SOMETHING>ing");
    /* Delete in same piece */
    arse_remove(editor, 6, 2);
    assert(editor, "<Hest<METHING>ing");

    fprintf(stderr, "====Create/Remove new line tests====\n");
    arse_new_line(editor, 0);
    assert(editor, "\n<Hest<METHING>ing");
    arse_delete(editor);
    return 0;

    fprintf(stderr, "###MULTILINE TESTS###\n");
    c = "first\nsecond\n\nfourth";
    str = malloc(sizeof(char) * strlen(c) + 1);
    strcpy(str, c);
    arse_init(editor);
    arse_load_string(editor, str);
    assert(editor, "first\nsecond\n\nfourth");
    /* Insert on second line through index */
    arse_insert(editor, 10, "On second line");
    assert(editor, "first\nsecoOn second linend\n\nfourth");
    /* Insert on first line even though index is larger */
    /* As newlines are explicit now instead of implied, this test will 
     * cause the insertion to pop up at the start of the next line as
     * it's placed behind the newline */
    arse_insert_at_line(editor, 0, 15, "On first line");
    assert(editor, "first\nOn first linesecoOn second linend\n\nfourth");

    arse_undo(editor);
    assert(editor, "first\nsecoOn second linend\n\nfourth");
    arse_redo(editor);
    assert(editor, "first\nOn first linesecoOn second linend\n\nfourth");
    arse_undo(editor);
    assert(editor, "first\nsecoOn second linend\n\nfourth");
    fprintf(stderr, "####HMMM2\n");
    arse_undo(editor);
    assert(editor, "first\nsecond\n\nfourth");
    fprintf(stderr, "HMMM\n\n\n");
    arse_insert_at_line(editor, 3, 4, "On fourth line");
    assert(editor, "first\nsecond\n\nfourOn fourth lineth");
    arse_insert_at_line(editor, 3, 0, "New fourth\nNot ");
    assert(editor, "first\nsecond\n\nNew fourth\nNot fourOn fourth lineth");
    fprintf(stderr, "new line\n");
    arse_new_line(editor, 0);
    fprintf(stderr, "test\n");
    assert(editor, "\nfirst\nsecond\n\nNew fourth\nNot fourOn fourth lineth");
    arse_delete(editor);

    /* Arsify */
    c = "firstOn first line\nsecoOn second linend";
    str = malloc(sizeof(char) * strlen(c) + 1);
    strcpy(str, c);
    arse_init(editor);
    arse_load_string(editor, str);
    assert(editor, "firstOn first line\nsecoOn second linend");
    fprintf(stderr, "###SUBARSE TESTS###\n");
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
