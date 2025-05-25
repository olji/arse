#include "testing.h"
#include "tests.h"
#include "arse.h"

int insert_start(){
  fprintf(stderr, "insert_start: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_insert(editor, 0, "Start");
  int ret = assert(editor, "StartTesting");
  test_cleanup(editor);
  return ret;
}
int insert_middle(){
  fprintf(stderr, "insert_middle: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_insert(editor, 4, "Middle");
  int ret = assert(editor, "TestMiddleing");
  test_cleanup(editor);
  return ret;
}
int insert_inside_piece(){
  fprintf(stderr, "insert_inside_piece: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_insert(editor, 2, "New Piece");
  arse_insert(editor, 5, "Insertion");
  int ret = assert(editor, "TeNewInsertion Piecesting");
  test_cleanup(editor);
  return ret;
}
int insert_end(){
  fprintf(stderr, "insert_end: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_insert(editor, 7, "Appended");
  int ret = assert(editor, "TestingAppended");
  test_cleanup(editor);
  return ret;
}
int undo_first(){
  fprintf(stderr, "undo_first: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_insert(editor, 2, "One");
  arse_insert(editor, 7, "Two");
  if(check(editor, "TeOnestTwoing")){
    return 0;
  }
  arse_undo(editor);
  int ret = assert(editor, "TeOnesting");
  test_cleanup(editor);
  return ret;
}
int undo_second(){
  fprintf(stderr, "undo_second: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_insert(editor, 2, "One");
  arse_insert(editor, 7, "Two");
  if(check(editor, "TeOnestTwoing")){
    return 0;
  }
  arse_undo(editor);
  arse_undo(editor);
  int ret = assert(editor, "Testing");
  test_cleanup(editor);
  return ret;
}
int undo_redo(){
  fprintf(stderr, "undo_redo: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_insert(editor, 2, "One");
  arse_insert(editor, 7, "Two");
  if(check(editor, "TeOnestTwoing")){
    return 0;
  }
  arse_undo(editor);
  if(check(editor, "TeOnesting")){
    return 0;
  }
  arse_redo(editor);
  int ret = assert(editor, "TeOnestTwoing");
  test_cleanup(editor);
  return ret;
}
int redo_undo(){
  fprintf(stderr, "redo_undo: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_insert(editor, 2, "One");
  arse_insert(editor, 7, "Two");
  if(check(editor, "TeOnestTwoing")){
    return 0;
  }
  arse_undo(editor);
  if(check(editor, "TeOnesting")){
    return 0;
  }
  arse_redo(editor);
  if(check(editor, "TeOnestTwoing")){
    return 0;
  }
  arse_undo(editor);
  int ret = assert(editor, "TeOnesting");
  test_cleanup(editor);
  return ret;
}
int delete_nothing(){
  fprintf(stderr, "delete_nothing: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_remove(editor, 6, 0);
  int ret = assert(editor, "Testing");
  test_cleanup(editor);
  return ret;

}
int delete_inside_piece(){
  fprintf(stderr, "delete_inside_piece: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_insert(editor, 2, "One");
  if(check(editor, "TeOnesting")){
    return 0;
  }
  arse_remove(editor, 3, 1);
  int ret = assert(editor, "TeOesting");
  test_cleanup(editor);
  return ret;

}
int delete_whole_piece(){
  fprintf(stderr, "delete_whole_piece: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_insert(editor, 2, "One");
  arse_insert(editor, 7, "Two");
  if(check(editor, "TeOnestTwoing")){
    return 0;
  }
  arse_remove(editor, 2, 3);
  int ret = assert(editor, "TestTwoing");
  test_cleanup(editor);
  return ret;

}
int delete_across_piece(){
  fprintf(stderr, "delete_across_piece: ");
  struct arse *editor = test_init_loaded("Testing");
  arse_insert(editor, 2, "One");
  arse_insert(editor, 7, "Two");
  if(check(editor, "TeOnestTwoing")){
    return 0;
  }
  arse_remove(editor, 1, 7);
  int ret = assert(editor, "Twoing");
  test_cleanup(editor);
  return ret;
}
int arsify_piece(){
  fprintf(stderr, "arsify_piece: ");
  struct arse *editor = test_init_loaded("TestTest");
  arse_piece_to_arse(editor, 0, 0, 4, false);
  arse_piece_to_arse(editor, 0, 4, 4, false);
  if(check(editor, "TestTest")){
    return 0;
  }
  arse_insert(editor, 2, "One");
  int ret = assert(editor, "TeOnestTeOnest");
  test_cleanup(editor);
  return ret;
}
