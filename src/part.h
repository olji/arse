#ifndef PART_H
#define PART_H
struct piece;
struct part{
  struct piece *first;
  struct piece *last;
};
struct part *part_create(struct piece *first, struct piece *last, int deepcopy);
void part_delete(struct part *p, int deep);
int part_length(struct part *p);
#endif /* PART_H */
