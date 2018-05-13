#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "table.h"
#include "part.h"

struct part_stack *part_stack_create(){
  struct part_stack *s = malloc(sizeof(struct part_stack));
  s->stack = malloc(sizeof(struct part*));
  s->length = 0;
  s->pointer = 0;
  return s;
}
void part_stack_delete(struct part_stack *s){
  while(s->pointer > 0){
    part_delete(s->stack[--s->pointer],1);
  }
  free(s->stack);
  free(s);
}
struct part *part_stack_pop(struct part_stack *s){
  if(s->pointer > 0){
    return s->stack[--s->pointer];
  } else {
    return NULL;
  }
}
struct part *part_stack_top(struct part_stack *s){
  if(s->pointer > 0){
    return s->stack[s->pointer - 1];
  } else {
    return NULL;
  }
}
int part_stack_push(struct part_stack *s, struct part *p){
  if(s->pointer == s->length){
    ++s->length;
    struct part **tmp = realloc(s->stack, sizeof(struct part*) * s->length);
    if(tmp != NULL){
      s->stack = tmp;
    } else {
      return 1;
    }
  }
  s->stack[s->pointer++] = p;
  return 0;
}
void part_stack_clean(struct part_stack *s){
  free(s->stack);
  s->stack = malloc(sizeof(struct part*));
  s->length = 0;
  s->pointer = 0;
}

struct table_stack *table_stack_create(){
  struct table_stack *s = malloc(sizeof(struct table_stack));
  s->stack = malloc(sizeof(struct table*));
  s->length = 0;
  s->pointer = 0;
  return s;
}
void table_stack_delete(struct table_stack *s){
  free(s->stack);
  free(s);
}
struct table *table_stack_pop(struct table_stack *s){
  if(s->pointer > 0){
    return s->stack[--s->pointer];
  } else {
    return NULL;
  }
}
struct table *table_stack_top(struct table_stack *s){
  if(s->pointer > 0){
    return s->stack[s->pointer - 1];
  } else {
    return NULL;
  }
}
int table_stack_push(struct table_stack *s, struct table *p){
  if(s->pointer == s->length){
    ++s->length;
    struct table **tmp = realloc(s->stack, sizeof(struct table*) * s->length);
    if(tmp != NULL){
      s->stack = tmp;
    } else {
      return 1;
    }
  }
  s->stack[s->pointer++] = p;
  return 0;
}
struct table *table_stack_pop_instance(struct table_stack *s, struct table *t){
  if(s->pointer == 0){
    return NULL;
  }
  int pos = --s->pointer;
  struct table *result = NULL;
  while(s->stack[pos] != t && pos > 0){
    --pos;
  }
  if(s->stack[pos] == t){
    result = t;
  }
  if(result != NULL){
    while(pos < s->pointer){
      s->stack[pos] = s->stack[pos + 1];
      ++pos;
    }
  }
  return result;
}
void table_stack_clean(struct table_stack *s){
  free(s->stack);
  s->stack = malloc(sizeof(struct table*));
  s->length = 0;
  s->pointer = 0;
}
void table_stack_clean_instance(struct table_stack *s, struct table *t){
  size_t swap_index = 0;
  size_t pos = 0;
  while(pos < s->pointer && swap_index < s->pointer){
    if(s->stack[pos] == t){
      ++pos;
    }
    else{
      if(pos != 0){
	struct table *tmp = s->stack[pos];
	s->stack[pos] = s->stack[swap_index];
	s->stack[swap_index] = tmp;
      }
      ++swap_index;
    }
  }
  s->pointer -= swap_index;
}
