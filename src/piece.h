#ifndef PIECE_H
#define PIECE_H

#include "buffertype.h"

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
  char* buffers[3];
  struct table *changes;
};

struct piece *piece_create(int start, int length, enum buffertype buffer);
void piece_insert_before(struct piece *after, struct piece *p);
void piece_insert_after(struct piece *before, struct piece *p);
size_t piece_chain_length(struct piece *start, struct piece *end);
struct piece *piece_copy(struct piece *p);
struct piece *piece_split(struct piece *p, int index);
void piece_delete_to(struct piece *from, struct piece *to);
#endif /* PIECE_H */
