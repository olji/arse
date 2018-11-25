#ifndef STACK_H
#define STACK_H
#include <stddef.h>
struct part;
struct table;
struct part_stack{
  struct part **stack;
  size_t length;
  size_t pointer;
};
struct table_stack{
  struct table **stack;
  size_t length;
  size_t pointer;
};
struct part_stack *part_stack_create();
void part_stack_delete(struct part_stack *s);
struct part *part_stack_pop(struct part_stack *s);
struct part *part_stack_top(struct part_stack *s);
int part_stack_push(struct part_stack *s, struct part *p);
void part_stack_clean(struct part_stack *s);
void part_stack_print(struct part_stack *s);
struct table_stack *table_stack_create();
void table_stack_delete(struct table_stack *s);
struct table *table_stack_pop(struct table_stack *s);
struct table *table_stack_pop_instance(struct table_stack *s, struct table *t);
struct table *table_stack_top(struct table_stack *s);
int table_stack_push(struct table_stack *s, struct table *t);
void table_stack_clean(struct table_stack *s);
void table_stack_clean_instance(struct table_stack *s, struct table *t);
#endif /* STACK_H */
