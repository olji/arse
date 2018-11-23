#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "arse.h"

#include "debug.h"

struct piece *piece_create(int start, int length, enum buffertype buffer){
  struct piece *p = malloc(sizeof(struct piece));
  p->start = start;
  p->length = length;
  p->buffer = buffer;
  p->arse = NULL;
  return p;
}
void piece_delete(struct piece *p){
  /* Arse is deleted when cleaning subarse table */
  free(p);
}
struct piece *piece_copy(struct piece *p){
  /* TODO: Should editor be copied or shared? */
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
size_t piece_chain_length(struct piece *start, struct piece *end){
  if(start == end){
    return start->length;
  }
  return start->length + piece_chain_length(start->next, end);
}
struct piece *piece_split(struct piece *p, int index){
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
struct arse *piece_to_arse(struct piece *p, char *content){
  if(p->buffer != ARSE_EDITOR){
    p->arse = arse_create(content, 0);
    p->buffer = ARSE_EDITOR;
    if(p->length != strlen(content)){
      fprintf(stderr, "WARNING: Arsified node had mismatching lengths\n");
    }
    p->length = strlen(content);
  }
  return p->arse;
}
