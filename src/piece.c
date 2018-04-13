#include <stdio.h>
#include <stdlib.h>
#include "piece.h"

#include "debug.h"

struct piece *piece_create(int start, int length, enum buffertype buffer){
  struct piece *p = malloc(sizeof(struct piece));
  p->start = start;
  p->length = length;
  p->buffer = buffer;
  debug("New piece(s-l-b): %d - %d - %d\n", start, length, buffer);
  return p;
}
struct piece *piece_copy(struct piece *p){
  debug("piece_copy created ");
  struct piece *new = piece_create(p->start, p->length, p->buffer);
  new->next = p->next;
  new->previous = p->previous;
  return new;
}
void piece_insert_after(struct piece *before, struct piece *p){
  p->next = before->next;
  p->previous = before;
  before->next->previous = p;
  before->next = p;
}
void piece_insert_before(struct piece *after, struct piece *p){
  p->next = after;
  p->previous = after->previous;
  after->previous->next = p;
  after->previous = p;
}
struct piece *piece_split(struct piece *p, int index){
  debug("piece_split created ");
  struct piece *new = piece_create(p->start + index, p->length - index, p->buffer);
  p->length = p->length - new->length;
  piece_insert_after(p, new);
  return new;
}
void piece_delete_to(struct piece *from, struct piece *to){
  while(from != to){
    struct piece *del = from;
    from = from->next;
    free(del);
  }
  free(to);
}
