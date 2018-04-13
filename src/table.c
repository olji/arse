#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "table.h"
#include "piece.h"
#include "stack.h"
#include "part.h"

#include "debug.h"

struct table *table_create(char *input){
  struct table *t = malloc(sizeof(struct table));
  t->length = strlen(input);
  t->buffers[ORIGINAL] = input;
  t->buffers[CHANGE] = malloc(sizeof(char));
  t->buffers[CHANGE][0] = 0;
  t->begin = piece_create(0,0,NONE);
  t->end = piece_create(0,0,NONE);

  struct piece *p = piece_create(0,strlen(input),ORIGINAL);

  t->begin->next = p;
  t->begin->previous = t->begin;
  p->next = t->end;
  p->previous = t->begin;
  t->end->next = t->end;
  t->end->previous = p;

  t->history = part_stack_create();
  t->future = part_stack_create();
  debug("begin: %p\n--end: %p", (void*)t->begin, (void*)t->end);
  return t;
}
size_t table_length(struct table *t){
  size_t len = 0;
  struct piece *p = t->begin;
  while(p != t->end){
    len += p->length;
    p = p->next;
  }
  return len;
}
int table_insert(struct table *t, size_t index, char *str){
  struct piece *p = t->begin;
  size_t length = strlen(str);
  size_t start = strlen(t->buffers[CHANGE]);
  struct piece *n = piece_create(start, length, CHANGE);
  char *tmp = realloc(t->buffers[CHANGE], sizeof(char) * (start + length + 1));
  if(tmp != NULL){
    tmp[start+length] = 0;
    t->buffers[CHANGE] = tmp;
  } else {
    return 1;
  }
  strcpy(t->buffers[CHANGE] + start, str);
  t->length += length;
  int distance = p->length;
  while(distance < index && p != t->end){
    p = p->next;
    distance += p->length;
  }
  index = index - (distance - p->length);
  bool even = p->length == index;
  struct part *range;
  if(even){
    range = part_create(p, p, 1);
  } else {
    range = part_create(p, p->next, 1);
  }
  part_stack_push(t->history, range);

  if(!even){
    piece_split(p, index);
  }
  piece_insert_after(p, n);
  p->length = index;
  return 0;
}
void table_remove(struct table *t, size_t from, size_t length){
  struct piece *first = t->begin;
  int distance = first->length;
  while(distance < from && first != t->end){
    first = first->next;
    distance += first->length;
  }
  from = from - (distance - first->length);
  struct piece *last = first;
  distance = last->length - from;
  while(distance < length && last != t->end){
    last = last->next;
    distance += last->length;
  }
  struct piece *part_start, *part_end;
  part_start = (from == first->length) ? first->next : first;
  int len = (distance - last->length);
  t->length -= len;
  if(len < 0){
    len = 0;
  } else {
    len = length - len;
  }
  part_end = (len == last->length) ? last->next : last;
  part_stack_push(t->history, part_create(part_start, part_end, 1));
  struct piece *del_start, *del_end;
  del_start = (part_start == first) ? piece_split(first, from) : part_start;
  if(part_start == part_end){
    del_end = piece_split(del_start, len)->previous;
  } else {
    del_end = ((part_end == last) ? piece_split(last, len) : part_end)->previous;
  }
  del_start->previous->next = del_end->next;
  del_end->next->previous = del_start->previous;
  piece_delete_to(del_start, del_end);
  return;
}
void table_delete(struct table *t){
  struct piece *p = t->begin;
  while(p != t->end){
    struct piece *d = p;
    p = p->next;
    free(d);
  }
  free(p);
  free(t->buffers[CHANGE]);
  free(t->buffers[ORIGINAL]);
  part_stack_delete(t->history);
  part_stack_delete(t->future);
  free(t);
}
char *table_buffer(struct table *t){
  debug("\n");
  table_print_series(t, t->begin, t->end);
  debug("CHANGEBUF: %s\n", t->buffers[CHANGE]);
  struct piece *p = t->begin;
  char *buffer = calloc((t->length + 1), sizeof(char));
  size_t copied = 0;
  while(p != t->end){
    if(p->buffer != NONE){
      strncpy(buffer + copied, t->buffers[p->buffer] + p->start, p->length);
      copied += p->length;
    }
    p = p->next;
  }
  return buffer;
}
void table_print_series(struct table *t, struct piece *begin, struct piece *end){
  struct piece *p = begin;
  debug("=========\nPIECE SERIES STATE\n=========\n");
  while(p != end){
    table_print_piece(t,p);
    p = p->next;
  }
  table_print_piece(t,p);
  debug("=========\nEND\n=========\n");
}
void table_print_piece(struct table *t, struct piece *p){
  char *s = calloc(sizeof(char), p->length + 1);
  strncpy(s, t->buffers[p->buffer] + p->start, p->length);
  debug("[%s](%p)\n", s, (void*)p);
  free(s);
}
void table_undo(struct table *t){
  if(t->history->pointer > 0){
    struct part *p = part_stack_pop(t->history);
    part_stack_push(t->future, part_create(p->first->previous->next, p->last->next->previous, 1));
    piece_delete_to(p->first->previous->next, p->last->next->previous);
    p->first->previous->next = p->first;
    p->last->next->previous = p->last;
    part_delete(p, 0);
  } else {
    printf("Buffer already at oldest state\n");
  }
}
void table_redo(struct table *t){
  if(t->future->pointer > 0){
    struct part *p = part_stack_pop(t->future);
    part_stack_push(t->history, part_create(p->first->previous->next, p->last->next->previous, 1));
    piece_delete_to(p->first->previous->next, p->last->next->previous);
    p->first->previous->next = p->first;
    p->last->next->previous = p->last;
    part_delete(p, 0);
  } else {
    printf("Buffer already at newest state\n");
  }
}
