#include <stdlib.h>
#include <string.h>
#include "arse.h"
#include "subarsetable.h"
#include "table.h"
#include "array.h"
#include "stack.h"
#include "part.h"
#include "piece.h"

#include "debug.h"
#include "panic.h"

static void parse_input_string(struct arse *a, char *input, size_t length);
static struct arse *arse_find_arse(struct arse *a, size_t line, size_t length, struct part *p);
static int arse_update_arrays(struct arse *a, struct arse *ed, size_t line);

size_t default_hashfunc(char *str, size_t mod){
  size_t pos = 0;
  for (int i=0; str[i] != '\0'; ++i){
    pos += str[i];
    pos += pos << 2;
    pos -= pos >> 3;
    pos ^= pos << 4;
  }
  return pos % mod;
}
void arse_init(struct arse *a){
  //a->action_history = table_stack_create();
  //a->action_future = table_stack_create();
  //a->slaves = subarse_table_create(16, &default_hashfunc);
  a->buffers[ARSE_CHANGE] = calloc(1, sizeof(char));

  a->begin = piece_create(0, 0, ARSE_NONE);
  a->end = piece_create(0, 0, ARSE_NONE);

  a->begin->previous = a->begin;
  a->end->next = a->end;

  a->history = part_stack_create();
  a->future = part_stack_create();
}
void arse_load_string(struct arse *a, char *initial_str){
  parse_input_string(a, initial_str, strlen(initial_str));
}
void arse_delete(struct arse *a){
  //table_stack_delete(a->action_history);
  //table_stack_delete(a->action_future);
  //subarse_table_delete(a->slaves);
  //if(a->hosts != NULL){
  //free(a->hosts);
  //}
  struct piece *d = a->begin;
  struct piece *next = d->next;
  do{
     piece_delete(d);
     d = next;
     next = next->next;
  } while (d != a->end);
  piece_delete(a->end);
  free(a->buffers[ARSE_CHANGE]);
  free(a);
}
int arse_insert(struct arse *a, size_t index, char *str){
  /*
  size_t table = 0;
  size_t length = 0;
  size_t insertions = strinst(str, '\n') + 1;
  char *heap_str = calloc(strlen(str) + 1, sizeof(char));
  strcpy(heap_str, str);
  char **insertion_strings;
  insertion_strings = malloc(sizeof(char*) * insertions);
  if(insertions > 1){
    size_t i = 0;
    char *token = strtok(heap_str, "\n");
    while(token != NULL){
      insertion_strings[i] = calloc(strlen(token) + ((i==0)?2:1), sizeof(char));
      strncpy(insertion_strings[i], token, strlen(token));
      if(i==0){
        insertion_strings[i][strlen(token)] = '\n';
      }
      ++i;
      token = strtok(NULL, "\n");
    }
  } else {
    insertion_strings[0] = heap_str;
  }

  index = index - length;
  return 0;
  */
  struct piece *p = a->begin;
  size_t distance = 0;
  size_t length = strlen(str);
  while(distance + p->length < index && p != a->end){
    distance += p->length;
    p = p->next;
  }
  if(p->buffer == ARSE_EDITOR){
    int result = arse_insert(p->arse, index - distance, str);
    if(result != 0){
      debug("Failed inserting to subarse\n");
      return 1;
    }
    // TODO: Verify this works
    /*
    for(size_t i = 0; i < p->arse->masters_count; ++i){
      p->arse->masters[i]->length += length;
    }
    */
    //for(size_t i = 0; i < p->arse->hosts_count; ++i){
      //p->arse->hosts[i]->length += length;
    //}
  } else {
    size_t start = strlen(a->buffers[ARSE_CHANGE]);
    struct piece *n = piece_create(start, length, ARSE_CHANGE);
    char *tmp = realloc(a->buffers[ARSE_CHANGE], sizeof(char) * (start + length + 1));
    if(tmp != NULL){
      tmp[start+length] = 0;
      a->buffers[ARSE_CHANGE] = tmp;
    } else {
      return 1;
    }
    strcpy(a->buffers[ARSE_CHANGE] + start, str);
    if(p != a->end){
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
    part_stack_push(a->history, range);

    if(!even){
      piece_split(p, index);
    }
    /* TODO: Some nicer way to handle when inserting at end of piece chain */
    if(p != a->end){
      piece_insert_after(p, n);
    } else {
      piece_insert_before(p, n);
    }
    p->length = index;
  }
  return 0;
}
int arse_remove(struct arse *a, size_t index, size_t length){
  return 0;
}
void arse_undo(struct arse *a){
  /*
  struct table *t = table_stack_pop(a->action_history);
  if(t != NULL){
    table_stack_push(a->action_future, t);
    table_undo(t);
  }
  */
  if(a->history->pointer > 0){
    struct piece *start = a->begin;
    struct part *p = part_stack_pop(a->history);
    /* FIXME: Multiple start blocks during multiline undo tests, is it from copied parts that got included in restoration during undo/redo? */
    while(!piece_equal_string(start, p->first->previous) && start != a->end){
      start = start->next;
    }

    if(start == a->end){
      fprintf(stderr, "ERROR: Undo start search reached end of piece chain\n");
      return;
    }
    start = start->next;
    struct piece *end = start;
    while(!piece_equal_string(end, p->last->next) && end != a->end){
      end = end->next;
    }
    end = end->previous;
    //a->length -= piece_chain_length(start, end);
    //a->length += piece_chain_length(p->first, p->last);
    part_stack_push(a->future, part_create(start->previous->next, end->next->previous, 0));

    start->previous->next = p->first;
    end->next->previous = p->last;
    part_delete(p, 0);
  } else {
    printf("Buffer already at oldest state\n");
  }
}
void arse_redo(struct arse *a){
  /*
  struct table *t = table_stack_pop(a->action_future);
  if(t != NULL){
    table_stack_push(a->action_history, t);
    table_redo(t);
  }
  */
  if(a->future->pointer > 0){
    struct piece *start = a->begin;
    struct part *p = part_stack_pop(a->future);
    while(!piece_equal_string(start, p->first->previous) && start != a->end){
      start = start->next;
    }
    if(start == a->end){
      fprintf(stderr, "ERROR: Redo start search reached end of piece chain\n");
      return 2;
    }
    struct piece *end = start;
    while(!piece_equal_string(end, p->last->next) && end != a->end){
      end = end->next;
    }
    //a->length -= piece_chain_length(start->next, end->previous);
    //a->length += piece_chain_length(p->first, p->last);
    part_stack_push(a->history, part_create(start->next, end->previous, 0));

    start->next = p->first;
    end->previous = p->last;
    part_delete(p, 0);
  } else {
    printf("Buffer already at oldest state\n");
  }
}
void arse_backup(struct arse *a){
}
struct arse_string *arse_get_string(struct arse *a){
  size_t copy_index = 0;
  struct piece *start = a->begin, *end = a->end;
  size_t length = piece_chain_length(start, end);
  char *str = calloc(length + 1, sizeof(char));

  while(start != end->next){
    if(start->buffer == ARSE_EDITOR){
      struct arse_string *str = arse_get_string(start->arse);
      strncpy(str + copy_index, str->string, str->length);
      copy_index += str->length;
      arse_string_delete(str);
    } else {
      strncpy(str + copy_index, a->buffers[start->buffer] + start->start, start->length);
      copy_index += start->length;
    }
    start = start->next;
  }
  struct arse_string *result = malloc(sizeof(struct arse_string));
  result->string = str;
  result->length = length;
  return result;
}
static struct arse *arse_find_arse(struct arse *a, size_t line, size_t length, struct part *p){
  // TODO
  /*
  struct arse *ed = subarse_table_find(a->slaves, string);
  if(ed == NULL){
    ed = malloc(sizeof(struct arse));
    arse_init(ed);
    arse_load_string(ed, string);
    subarse_table_insert(a->slaves, ed);
  } else {
    free(string);
  }
  return ed;
  */
  return NULL;
}
static int arse_update_arrays(struct arse *a, struct arse *ed, size_t line){
  /*
  if(ed->hosts == NULL){
    ed->hosts = calloc(1, sizeof(struct piece*));
    ed->hosts_count = 1;
  } else {
    struct piece **tmp = realloc(ed->hosts, ++ed->hosts_count * sizeof(struct piece*));
    if(tmp != NULL){
      ed->hosts = tmp;
    } else {
      return 2;
    }
  }
  */
  return 0;
}
int arse_piece_to_arse(struct arse *a, size_t index, size_t length, bool unique){
  return 0;
}

void arse_string_delete(struct arse_string *a){
  free(a->string);
  free(a);
}
static void parse_input_string(struct arse *a, char *input, size_t length){
  a->buffers[ARSE_ORIGINAL] = input;
  struct piece *p = piece_create(0, length, ARSE_ORIGINAL);

  a->begin->next = p;
  a->end->previous = p;

  p->next = a->end;
  p->previous = a->begin;

  debug("string given: %s\n", input);
}
