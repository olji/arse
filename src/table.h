#ifndef TABLE_H
#define TABLE_H

#include "buffertype.h"

struct part_stack;
struct part;
struct piece;
struct table{
  struct piece *begin;
  struct piece *end;
  char* buffers[2];
  size_t length;

  struct part_stack *history;
  struct part_stack *future;
};
struct table *table_create(char *input);
void table_delete(struct table *t, int free_input);
int table_insert(struct table *t, size_t index, char *str);
void table_remove(struct table *t, size_t from, size_t length);
void table_undo(struct table *t);
void table_redo(struct table *t);
size_t table_length(struct table *t);
char *table_buffer(struct table *t);
void table_print_series(struct table *t, struct piece *begin, struct piece *end);
void table_print_piece(struct table *t, struct piece *p);
struct part *table_get_pieces(struct table *t, size_t from, size_t length);
void table_get_string(struct table *t, struct piece *start, struct piece *end, char *result);
void table_replace_pieces(struct table *t, struct piece *start, struct piece *end, struct piece *replacement);
void table_piece_to_arse(struct table *t, size_t index);
#endif /* TABLE_H */
