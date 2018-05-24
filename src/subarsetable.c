#include <stdlib.h>
#include <string.h>
#include "arse.h"
#include "subarsetable.h"

#include "debug.h"

struct subarse_table *subarse_table_create(size_t length, size_t (*f)(char*,size_t)){
  struct subarse_table *st = malloc(sizeof(struct subarse_table));
  st->table = calloc(length, sizeof(struct subarse_node*));
  st->length = length;
  st->hashfunc = f;
  return st;
}
struct subarse_node *subarse_node_create(struct arse *a){
  struct subarse_node *sn = malloc(sizeof(struct subarse_node));
  sn->editor = a;
  sn->next = NULL;
  struct arse_buffer *buf = arse_get_buffer(a);
  sn->str = buf->buffer;
  free(buf);
  return sn;
}
void subarse_table_delete(struct subarse_table *st){
  for(size_t i = 0; i < st->length; ++i){
    if(st->table[i] != NULL){
      subarse_node_delete(st->table[i]);
    }
  }
  free(st->table);
  free(st);
}
void subarse_node_delete(struct subarse_node *sn){
  if(sn->next != NULL){
    subarse_node_delete(sn);
  }
  arse_delete(sn->editor);
  free(sn->str);
  free(sn);
}
struct arse *subarse_table_find(struct subarse_table *st, char *s){
  struct subarse_node *index = st->table[st->hashfunc(s, st->length)];
  if(index == NULL){
    return NULL;
  }
  while(index != NULL){
    if(strcmp(index->str, s) == 0){
      debug("Found arse\n");
      return index->editor;
    }
    index = index->next;
  }
  return NULL;
}
void subarse_table_insert(struct subarse_table *st, struct arse *a){
  struct subarse_node *node = subarse_node_create(a);
  struct subarse_node *index = st->table[st->hashfunc(node->str, st->length)];
  if(index == NULL){
    st->table[st->hashfunc(node->str, st->length)] = node;
  } else {
    /* TODO: Handle shared editors */
    while(index->next != NULL){
      index = index->next;
    }
    index->next = node;
  }
}
