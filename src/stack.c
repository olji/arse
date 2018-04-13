#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
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
    printf("SHIT\n");
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
  } else {
    //free(s->stack[s->pointer]);
  }
  s->stack[s->pointer++] = p;
  return 0;
}
