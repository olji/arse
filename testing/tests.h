#ifndef TESTS_H
#define TESTS_H
struct arse;
int string_load_and_get();
int insert_start();
int insert_middle();
int insert_inside_piece();
int insert_end();
int undo_first();
int undo_second();
int undo_redo();
int redo_undo();
int delete_nothing();
int delete_inside_piece();
int delete_whole_piece();
int delete_across_piece();
int arsify_piece();
#endif //TESTS_H
