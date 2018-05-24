#ifndef SUBARSETABLE_H
#define SUBARSETABLE_H
struct arse;
struct subarse_node{
  struct arse *editor;
  struct subarse_node *next;
  char *str;
};
struct subarse_table{
  struct subarse_node **table;
  size_t length;
  size_t (*hashfunc)(char*,size_t);
};

struct subarse_node *subarse_node_create(struct arse *a);
struct subarse_table *subarse_table_create(size_t length, size_t (*f)(char*,size_t));
void subarse_table_delete(struct subarse_table *st);
void subarse_node_delete(struct subarse_node *sn);
struct arse *subarse_table_find(struct subarse_table *st, char *s);
void subarse_table_insert(struct subarse_table *st, struct arse *a);
#endif /* SUBARSETABLE_H */
