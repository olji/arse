#ifndef PIECE_H
#define PIECE_H

#include <stdbool.h>

#include "buffertype.h"

struct arse;

struct piece{
  struct piece *next;
  struct piece *previous;
  /* Index in piece table buffer */
  int start;
  /* Length of string */
  int length;
  /* Buffer */
  enum buffertype buffer;

  /* Editor piece fields, available if buffer == EDITOR */
  struct arse *arse;
  struct table *master;
};

struct piece *piece_create(int start, int length, enum buffertype buffer);
void piece_delete(struct piece *p);
void piece_insert_before(struct piece *after, struct piece *p);
void piece_insert_after(struct piece *before, struct piece *p);
size_t piece_chain_length(struct piece *start, struct piece *end);
struct arse *piece_to_arse(struct piece *p, char *content);
struct piece *piece_copy(struct piece *p);
struct piece *piece_split(struct piece *p, int index);
void piece_delete_to(struct piece *from, struct piece *to);
bool piece_equal_strict(struct piece *rhs, struct piece *lhs);
bool piece_equal_string(struct piece *r, struct piece *l);
#endif /* PIECE_H */
