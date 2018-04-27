#ifndef ARSE_H
#define ARSE_H
#include <stdio.h>
struct table;
struct table_stack;
struct arse{
  struct table **lines;
  struct table_stack *action_history;
  struct table_stack *action_future;
  size_t lines_count;
  char *filename;
  FILE *fp;
};
struct arse *arse_create(char *str, int file);
void arse_delete(struct arse *a);
int arse_insert(struct arse *a, size_t index, char *str);
int arse_insert_at_line(struct arse *a, size_t line, size_t index, char *str);
int arse_remove(struct arse *a, size_t index, size_t length);
void arse_undo(struct arse *a);
void arse_redo(struct arse *a);
void arse_undo_line(struct arse *a, size_t line);
void arse_redo_line(struct arse *a, size_t line);
char *arse_get_line(struct arse *a, size_t line);
int arse_save(struct arse *a);
void arse_backup(struct arse *a);
char *arse_buffer(struct arse *a);
#endif /* ARSE_H */
