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
    struct arse *editor = arse_create(str, 0);
    fprintf(stderr, "###SINGLE LINE TESTS###\n");
    fprintf(stderr, "====Insertion test====\n");
    arse_insert(editor, 4, "<SOMETHING>");
    assert(arse_get_string(editor), "Test<SOMETHING>ing");
    arse_insert(editor, 0, "<Pere>");
    assert(arse_get_string(editor), "<Pere>Test<SOMETHING>ing");
    arse_insert(editor, 2, "<Hello>");
    assert(arse_get_string(editor), "<P<Hello>ere>Test<SOMETHING>ing");
    arse_insert(editor, 4, "<Last one>");
    assert(arse_get_string(editor), "<P<H<Last one>ello>ere>Test<SOMETHING>ing");
    arse_insert(editor, 11, "<Kidding>");
    assert(arse_get_string(editor), "<P<H<Last o<Kidding>ne>ello>ere>Test<SOMETHING>ing");

    fprintf(stderr, "====Undo/redo test====\n");
    arse_undo(editor);
    assert(arse_get_string(editor), "<P<H<Last one>ello>ere>Test<SOMETHING>ing");
    arse_undo(editor);
    assert(arse_get_string(editor), "<P<Hello>ere>Test<SOMETHING>ing");
    arse_redo(editor);
    assert(arse_get_string(editor), "<P<H<Last one>ello>ere>Test<SOMETHING>ing");
    arse_undo(editor);
    assert(arse_get_string(editor), "<P<Hello>ere>Test<SOMETHING>ing");

    fprintf(stderr, "====Removal test====\n");
    /* Delete across pieces */
    arse_remove(editor, 4, 10);
    assert(arse_get_string(editor), "<P<Hest<SOMETHING>ing");
    /* Delete whole piece */
    arse_remove(editor, 0, 2);
    assert(arse_get_string(editor), "<Hest<SOMETHING>ing");
    /* Delete nothing */
    arse_remove(editor, 6, 0);
    assert(arse_get_string(editor), "<Hest<SOMETHING>ing");
    /* Delete in same piece */
    arse_remove(editor, 6, 2);
    assert(arse_get_string(editor), "<Hest<METHING>ing");
    arse_delete(editor);

    fprintf(stderr, "###MULTILINE TESTS###\n");
    c = "first\nsecond";
    str = malloc(sizeof(char) * strlen(c) + 1);
    strcpy(str, c);
    editor = arse_create(str, 0);
    assert(arse_get_string(editor), "first\nsecond");
    /* Insert on second line through index */
    arse_insert(editor, 10, "On second line");
    assert(arse_get_string(editor), "first\nsecoOn second linend");
    /* Insert on first line even though index is larger */
    /* As newlines are explicit now instead of implied, this test will 
     * cause the insertion to pop up at the start of the next line as
     * it's placed behind the newline */
    arse_insert_at_line(editor, 0, 15, "On first line");
    assert(arse_get_string(editor), "first\nOn first linesecoOn second linend");

    arse_undo(editor);
    assert(arse_get_string(editor), "first\nsecoOn second linend");
    arse_redo(editor);
    assert(arse_get_string(editor), "first\nOn first linesecoOn second linend");
    arse_undo_line(editor, 1);
    assert(arse_get_string(editor), "first\nOn first linesecond");
    arse_redo_line(editor, 1);
    assert(arse_get_string(editor), "first\nOn first linesecoOn second linend");
    arse_undo(editor);
    assert(arse_get_string(editor), "first\nOn first linesecond");
    arse_undo(editor);
    assert(arse_get_string(editor), "first\nsecond");
    arse_redo_line(editor, 1);
    assert(arse_get_string(editor), "first\nsecoOn second linend");
    arse_redo_line(editor, 0);
    assert(arse_get_string(editor), "first\nOn first linesecoOn second linend");

    /* Arsify */
    fprintf(stderr, "###SUBARSE TESTS###\n");
    arse_piece_to_arse(editor, 0, 0, 5, true);
    assert(arse_get_string(editor), "first\nOn first linesecoOn second linend");
    arse_piece_to_arse(editor, 0, 9, 5, true);
    assert(arse_get_string(editor), "first\nOn first linesecoOn second linend");
    arse_insert_at_line(editor, 0, 1, "Hello");
    assert(arse_get_string(editor), "fHelloirst\nOn fHelloirst linesecoOn second linend");
    arse_remove_at_line(editor, 0, 2, 2);
    assert(arse_get_string(editor), "fHloirst\nOn fHloirst linesecoOn second linend");
    arse_insert_at_line(editor, 0, 15, "Inside second arse");
    assert(arse_get_string(editor), "fHlInside second arseoirst\nOn fHlInside second arseoirst linesecoOn second linend");

    arse_delete(editor);
    return 0;

  } else {
    struct arse *editor = arse_create(argv[1], 1);
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
  size_t index = 0;
  char *text = malloc(sizeof(*text) * 254);
  struct arse_string *str;

  switch(mode){
  case 'i':
    scanf("%zu", &line);
    scanf("%zu", &index);
    scanf("%s", text);
    arse_insert_at_line(editor, line, index, text);
    break;
  case 'u':
    arse_undo(editor);
    break;
  case 'p':
    str = arse_get_string(editor);
    /* FIXME: Should probably do putchar scheme like in ed/io.c:print_line() so escaped characters are visible */
    printf("%.*s\n", str->length, str->string);
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
