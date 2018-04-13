#ifndef ARSE_H
#define ARSE_H
#include <stdio.h>
struct table;
struct arse{
  struct table **lines;
  size_t lines_count;
  char *filename;
  FILE *fp;
};
struct arse *arse_create(char *str, int file);
void arse_delete(struct arse *t);
int arse_insert(struct arse *t, int index, char *str);
int arse_remove(struct arse *t, int index, int length);
void arse_undo(struct arse *t);
void arse_redo(struct arse *t);
int arse_save(struct arse *t);
void arse_backup(struct arse *t);
char *arse_buffer(struct arse *t);
#endif /* ARSE_H */
