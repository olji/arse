#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "arse.h"
#include "table.h"
#include "piece.h"
#include "stack.h"
#include "part.h"
#include "array.h"

#include "debug.h"

void table_print(struct table *t){
  table_print_series(t, t->begin, t->end);
}

struct table *table_create(char *input, bool static_str){
  struct table *t = malloc(sizeof(struct table));
  size_t length = strlen(input);
  t->length = length;
  t->buffers[ARSE_ORIGINAL] = malloc(sizeof(char) * t->length);
  strncpy(t->buffers[ARSE_ORIGINAL], input, t->length);
  t->buffers[ARSE_CHANGE] = malloc(sizeof(char));
  t->buffers[ARSE_CHANGE][0] = 0;
  t->begin = piece_create(0,0,ARSE_NONE);
  t->end = piece_create(0,0,ARSE_NONE);

  struct piece *p = piece_create(0,length,ARSE_ORIGINAL);

  t->begin->next = p;
  t->begin->previous = t->begin;
  p->next = t->end;
  p->previous = t->begin;
  t->end->next = t->end;
  t->end->previous = p;

  t->history = part_stack_create();
  t->future = part_stack_create();

  t->state.line_count = linecount_get(input);
  t->state.line_lengths = malloc(sizeof(size_t));
  /* FIXME: Fix this for multiline input */
  t->state.line_lengths[0] = length;
  t->state.arse_count = 0;
  t->state.arse_nodes = 0;

  if(!static_str){
    free(input);
  }
  
  return t;
}
void table_delete(struct table *t, int free_input){
  struct piece *p = t->begin;
  while(p != t->end){
    struct piece *d = p;
    p = p->next;
    piece_delete(d);
  }
  piece_delete(p);
  free(t->buffers[ARSE_CHANGE]);
  if(free_input){
    free(t->buffers[ARSE_ORIGINAL]);
  }
  part_stack_delete(t->history);
  part_stack_delete(t->future);
  free(t->state.line_lengths);
  if(t->state.arse_nodes != NULL){
    free(t->state.arse_nodes);
  }
  free(t);
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
struct piece *table_insert(struct table *t, size_t index, char *str){
  struct piece *p = t->begin;
  size_t distance = 0;
  size_t length = strlen(str);
  while(distance + p->length < index && p != t->end){
    distance += p->length;
    p = p->next;
  }
  if(p->buffer == ARSE_EDITOR){
    int result = arse_insert(p->arse, index - distance, str);
    if(result != 0){
      debug("Failed inserting to subarse\n");
      return NULL;
    }
    /* TODO: Verify this works */
    for(size_t i = 0; i < p->arse->masters_count; ++i){
      p->arse->masters[i]->length += length;
    }
    for(size_t i = 0; i < p->arse->hosts_count; ++i){
      p->arse->hosts[i]->length += length;
    }
  } else {
    size_t start = strlen(t->buffers[ARSE_CHANGE]);
    struct piece *n = piece_create(start, length, ARSE_CHANGE);
    char *tmp = realloc(t->buffers[ARSE_CHANGE], sizeof(char) * (start + length + 1));
    if(tmp != NULL){
      tmp[start+length] = 0;
      t->buffers[ARSE_CHANGE] = tmp;
    } else {
      return NULL;
    }
    t->length += length;
    strcpy(t->buffers[ARSE_CHANGE] + start, str);
    if(p != t->end){
      index = index - distance;
    } else {
      index = p->length;
    }
    bool even = p->length == index;
    struct part *range;
    if(even){
      range = part_create(p, p->next, 1);
    } else {
      range = part_create(p, p, 1);
    }
    part_stack_push(t->history, range);

    if(!even){
      piece_split(p, index);
    }
    /* TODO: Some nicer way to handle when inserting at end of piece chain */
    if(p != t->end){
      piece_insert_after(p, n);
    } else {
      piece_insert_before(p, n);
    }
    p->length = index;
  }
  return NULL;
}
void table_remove(struct table *t, size_t from, size_t length){
  if(length == 0){
    return;
  }
  struct piece *first = t->begin;
  int distance = first->length;
  /* Find piece to start removal */
  while(distance < from && first != t->end){
    first = first->next;
    distance += first->length;
  }
  if(first->buffer == ARSE_EDITOR){
    arse_remove(first->arse, from, length);
    for(size_t i = 0; i < first->arse->masters_count; ++i){
      first->arse->masters[i]->length -= length;
    }
    for(size_t i = 0; i < first->arse->hosts_count; ++i){
      first->arse->hosts[i]->length -= length;
    }
  } else {
    /* FIXME: Rewrite for better readability and logic */
    t->length -= length;
    from = from - (distance - first->length);
    struct piece *last = first;
    distance = last->length - from;
    while(distance < length && last != t->end){
      last = last->next;
      distance += last->length;
    }
    struct piece *part_start, *part_end;
    bool border_next = (from == first->length);
    part_start = (border_next) ? first->next : first;
    bool whole_piece = (length == part_start->length);
    int len = (distance - last->length);
    if(len < 0){
      len = 0;
    } else {
      len = length - len;
    }
    part_end = last;
    part_stack_push(t->history, part_create(part_start, part_end, 1));
    struct piece *del_start, *del_end;
    bool split_end = (part_start == part_end);
    del_start = (whole_piece) ? part_start : piece_split(first, from);

    if(whole_piece){
      del_end = del_start;
    } else if(split_end){
      del_end = piece_split(del_start, length)->previous;
    } else {
      del_end = ((part_end == last) ? piece_split(last, len) : part_end)->previous;
    }

    del_start->previous->next = del_end->next;
    del_end->next->previous = del_start->previous;
    piece_delete_to(del_start, del_end);
  }
  return;
}
/* FIXME: Invalid write of size 1 error here, likely due to piece chain not synced with table length after broken undo */
char *table_buffer(struct table *t){
  struct piece *p = t->begin;
  char *buffer = calloc((t->length + 1), sizeof(char));
  size_t copied = 0;
  while(p != t->end){
    if(p->buffer == ARSE_EDITOR){
      struct arse_string *str = arse_get_string(p->arse);
      strcpy(buffer + copied, str->string);
      copied += str->length;
      arse_string_delete(str);
    } else {
      strncpy(buffer + copied, t->buffers[p->buffer] + p->start, p->length);
      copied += p->length;
    }
    p = p->next;
  }
  buffer[t->length] = 0;
  return buffer;
}
void table_print_series(struct table *t, struct piece *begin, struct piece *end){
  struct piece *p = begin;
  debug("=========\nPIECE SERIES STATE\n=========\n");
  while(p != end){
    table_print_piece(t,p);
    p = p->next;
  }
  table_print_piece(t,end);
  debug("=========\nEND\n=========\n");
}
void table_print_piece(struct table *t, struct piece *p){
  if(p->buffer == ARSE_EDITOR){
    debug("[ARSE INSTANCE](%p)\n", (void*)p);
  } else {
    struct piece *prev = p->previous;
    struct piece *next = p->next;
    char *s = calloc(sizeof(char), p->length + 1);
    char *s_n = calloc(sizeof(char), next->length + 1);
    char *s_p = calloc(sizeof(char), prev->length + 1);
    strncpy(s, t->buffers[p->buffer] + p->start, p->length);
    strncpy(s_n, t->buffers[next->buffer] + next->start, next->length);
    strncpy(s_p, t->buffers[prev->buffer] + prev->start, prev->length);
    debug("[%s]->", s_p);
    debug("[%s](%p)", s, (void*)p);
    debug("->[%s]", s_n);
    debug("\n");
    free(s);
    free(s_n);
    free(s_p);
  }
}
void table_undo(struct table *t){
  if(t->history->pointer > 0){
    struct piece *start = t->begin;
    struct part *p = part_stack_pop(t->history);
    /* FIXME: Multiple start blocks during multiline undo tests, is it from copied parts that got included in restoration during undo/redo? */
    while(!piece_equal_string(start, p->first->previous) && start != t->end){
      start = start->next;
    }
    
    if(start == t->end){
      fprintf(stderr, "ERROR: Undo start search reached end of piece chain\n");
      return 2;
    }
    start = start->next;
    struct piece *end = start;
    while(!piece_equal_string(end, p->last->next) && end != t->end){
      end = end->next;
    }
    end = end->previous;
    t->length -= piece_chain_length(start, end);
    t->length += piece_chain_length(p->first, p->last);
    part_stack_push(t->future, part_create(start->previous->next, end->next->previous, 0));

    start->previous->next = p->first;
    end->next->previous = p->last;
    part_delete(p, 0);
  } else {
    printf("Buffer already at oldest state\n");
  }
}
void table_redo(struct table *t){
  if(t->future->pointer > 0){
    struct piece *start = t->begin;
    struct part *p = part_stack_pop(t->future);
    while(!piece_equal_string(start, p->first->previous) && start != t->end){
      start = start->next;
    }
    if(start == t->end){
      fprintf(stderr, "ERROR: Redo start search reached end of piece chain\n");
      return 2;
    }
    struct piece *end = start;
    while(!piece_equal_string(end, p->last->next) && end != t->end){
      end = end->next;
    }
    t->length -= piece_chain_length(start->next, end->previous);
    t->length += piece_chain_length(p->first, p->last);
    part_stack_push(t->history, part_create(start->next, end->previous, 0));

    start->next = p->first;
    end->previous = p->last;
    part_delete(p, 0);
  } else {
    printf("Buffer already at oldest state\n");
  }
}

struct part *table_get_pieces(struct table *t, size_t from, size_t length){
  struct piece *p = t->begin->next;
  struct piece *n;
  size_t distance = 0;
  while(distance + p->length < from && p != t->end){
    distance += p->length;
    p = p->next;
  }
  /* FIXME: Detect edges properly */
  if(distance != from){
    p = piece_split(p, from - distance);
    distance += p->previous->length;
  }
  n = p;
  while(distance + n->length < from + length && n != t->end){
    distance += n->length;
    n = n->next;
  }
  /* FIXME: Detect edges properly */
  if(distance != from + length){
    n = piece_split(n, from + length - distance)->previous;
  }
  return part_create(p, n, false);
}
void table_get_string(struct table *t, struct piece *start, struct piece *end, char *result){
  size_t copy_index = 0;
  while(start != end->next){
    if(start->buffer == ARSE_EDITOR){
      struct arse_string *str = arse_get_string(start->arse);
      strncpy(result + copy_index, str->string, str->length);
      copy_index += str->length;
      arse_string_delete(str);
    } else {
      strncpy(result + copy_index, t->buffers[start->buffer] + start->start, start->length);
      copy_index += start->length;
    }
    start = start->next;
  }
}
void table_replace_pieces(struct table *t, struct piece *start, struct piece *end, struct piece *replacement){
  struct part *p = part_create(start, end, true);
  replacement->next = end->next;
  replacement->previous = start->previous;
  start->previous->next = replacement;
  end->next->previous = replacement;
  piece_delete_to(start, end);
  part_stack_push(t->history, p);
}
int table_piece_to_arse(struct table *t, size_t index){
  struct piece *p = t->begin;
  size_t distance = 0;
  while(distance + p->length < index && p != t->end){
    distance += p->length;
    p = p->next;
  }
  if(p->buffer != ARSE_EDITOR && p->buffer != ARSE_NONE){
    ++t->state.arse_count;
    struct piece **tmp = realloc(&t->state.arse_nodes, t->state.arse_count * sizeof(struct piece*));
    if(tmp != NULL){
      t->state.arse_nodes = tmp;
    } else {
      return 1;
    }
    t->state.arse_nodes[t->state.arse_count-1] = p;

    char *content = malloc(sizeof(char) * p->length + 1);
    content[p->length] = '\0';
    strncpy(content, t->buffers[p->buffer] + p->start, p->length);
    piece_to_arse(p, content);

  }
  return 0;
}
struct table *table_split(struct table *t, size_t index){
  struct piece *start = t->begin;
  size_t distance = 0;
  while(distance + start->length < index && start != t->end){
    distance += start->length;
    start = start->next;
  }
  index -= distance;
  /* TODO: Save history */
  /* TODO: Handle piece_split inside an arse node */
  struct piece *t_begin = piece_split(start, index);
  size_t ll = t->length - index;
  char *str = calloc(ll + 1, sizeof(char));
  table_get_string(t, t_begin, t->end, str);
  piece_delete_to(t_begin, t->end->previous);
  start->next = t->end;
  t->end->previous = start;
  struct table *tmp = table_create(str, false);
  return tmp;
}
int table_state_update(struct table *t){
  char *str = table_buffer(t);
  size_t old_line_count = t->state.line_count;
  t->state.line_count = linecount_get(str);
  if(old_line_count != t->state.line_count){
  }
  if(old_line_count != t->state.line_count){
    size_t *tmp = realloc(t->state.line_lengths, t->state.line_count * sizeof(size_t));
    if(tmp != NULL){
      t->state.line_lengths = tmp;
    } else {
      return 1;
    }
  }
  char *start = str; 
  for(int i = 0;i < t->state.line_count; ++i){
    char *next = strchr(start, '\n');
    if(next == NULL){
      next = start + strlen(start) - 1;
    }
    t->state.line_lengths[i] = next - start + 1;
    start += t->state.line_lengths[i];
  }
  return 0;
}
