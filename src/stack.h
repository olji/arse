#ifndef STACK_H
#define STACK_H
#include <stddef.h>
struct part;
struct part_stack{
  struct part **stack;
  size_t length;
  size_t pointer;
};
struct part_stack *part_stack_create();
void part_stack_delete(struct part_stack *s);
struct part *part_stack_pop(struct part_stack *s);
int part_stack_push(struct part_stack *s, struct part *p);
#endif /* STACK_H */
