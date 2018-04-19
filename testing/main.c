#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arse.h"

#include "testing.h"

void exec_command(struct arse *editor, char mode);
int main(int argc, char **argv){
  if(argc < 2){
    char *c = "Testing";
    char *str = malloc(sizeof(char) * strlen(c) + 1);
    strcpy(str, c);
    struct arse *editor = arse_create(str, 0);
    fprintf(stderr, "###SINGLE LINE TESTS###\n");
    fprintf(stderr, "====Insertion test====\n");
    arse_insert(editor, 4, "<SOMETHING>");
    assert(arse_buffer(editor), "Test<SOMETHING>ing");
    arse_insert(editor, 0, "<Pere>");
    assert(arse_buffer(editor), "<Pere>Test<SOMETHING>ing");
    arse_insert(editor, 2, "<Hello>");
    assert(arse_buffer(editor), "<P<Hello>ere>Test<SOMETHING>ing");
    arse_insert(editor, 4, "<Last one>");
    assert(arse_buffer(editor), "<P<H<Last one>ello>ere>Test<SOMETHING>ing");
    arse_insert(editor, 11, "<Kidding>");
    assert(arse_buffer(editor), "<P<H<Last o<Kidding>ne>ello>ere>Test<SOMETHING>ing");

    fprintf(stderr, "====Undo/redo test====\n");
    arse_undo(editor);
    assert(arse_buffer(editor), "<P<H<Last one>ello>ere>Test<SOMETHING>ing");
    arse_undo(editor);
    assert(arse_buffer(editor), "<P<Hello>ere>Test<SOMETHING>ing");
    arse_redo(editor);
    assert(arse_buffer(editor), "<P<H<Last one>ello>ere>Test<SOMETHING>ing");
    arse_undo(editor);
    assert(arse_buffer(editor), "<P<Hello>ere>Test<SOMETHING>ing");

    fprintf(stderr, "====Removal test====\n");
    /* Delete across pieces */
    arse_remove(editor, 4, 10);
    assert(arse_buffer(editor), "<P<Hest<SOMETHING>ing");
    /* Delete whole piece */
    arse_remove(editor, 0, 2);
    assert(arse_buffer(editor), "<Hest<SOMETHING>ing");
    /* Delete nothing */
    arse_remove(editor, 6, 0);
    assert(arse_buffer(editor), "<Hest<SOMETHING>ing");
    /* Delete in same piece */
    arse_remove(editor, 6, 2);
    assert(arse_buffer(editor), "<Hest<METHING>ing");
    /* TODO: Multiline support and test */
    arse_delete(editor);

    fprintf(stderr, "###MULTILINE TESTS###\n");
    c = "First\nSecond";
    str = malloc(sizeof(char) * strlen(c) + 1);
    strcpy(str, c);
    editor = arse_create(str, 0);
    assert(arse_buffer(editor), "First\nSecond");
    /* Insert on second line through index */
    arse_insert(editor, 9, "On second line");
    /* Since we don't store newlines explicitly, they're not counted in the index */
    assert(arse_buffer(editor), "First\nSecoOn second linend");
    /* Insert on first line even though index is larger */
    arse_insert_at_line(editor, 0, 15, "On first line");
    assert(arse_buffer(editor), "FirstOn first line\nSecoOn second linend");

    arse_undo(editor);
    assert(arse_buffer(editor), "First\nSecoOn second linend");
    arse_redo(editor);
    assert(arse_buffer(editor), "FirstOn first line\nSecoOn second linend");
    arse_undo_line(editor, 1);
    assert(arse_buffer(editor), "FirstOn first line\nSecond");
    arse_redo_line(editor, 1);
    assert(arse_buffer(editor), "FirstOn first line\nSecoOn second linend");
    arse_undo(editor);
    assert(arse_buffer(editor), "FirstOn first line\nSecond");
    arse_undo(editor);
    assert(arse_buffer(editor), "First\nSecond");
    arse_redo_line(editor, 1);
    assert(arse_buffer(editor), "First\nSecoOn second linend");
    arse_redo_line(editor, 0);
    assert(arse_buffer(editor), "FirstOn first line\nSecoOn second linend");

    arse_delete(editor);
    return 0;

  } else {
    struct arse *editor = arse_create(argv[1], 1);
    /* char mode; */
    printf("Not yet implemented");
    /* do{ */
    /*   std::cin >> mode; */
    /*   exec_command(editor, mode); */
    /*   printf("%s\n", arse_buffer(editor)); */
    /* } while(mode != 'q'); */
    arse_delete(editor);
  }
}
void exec_command(struct arse *editor, char mode){
  /* int index; */
  /* std::string text; */
  /* switch(mode){ */
  /* case 'i': */
  /*   std::cin >> index; */
  /*   std::cin >> text; */
  /*   editor->insert(index, text); */
  /*   break; */
  /* case 'u': */
  /*   editor->undo(); */
  /*   break; */
  /* default: */
  /*   std::cout << "Not a command" << std::endl; */
  /* } */
}