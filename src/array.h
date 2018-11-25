#ifndef ARRAY_H
#define ARRAY_H
struct table;
struct str_token{
  char *start;
  char *end;
};
struct table **table_array_insert_at(struct table **block, struct table *object, size_t index, size_t length);
size_t strinst(char *str, char character);
size_t linecount_get(char *str);
#endif /* ARRAY_H */
