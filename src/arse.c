#include <stdlib.h>
#include <string.h>
#include "arse.h"
#include "table.h"
#include "array.h"
#include "stack.h"

#include "debug.h"
#include "panic.h"

struct arse *arse_create(char *str, int file){
  struct arse *a = malloc(sizeof(struct arse));
  a->action_history = table_stack_create();
  a->action_future = table_stack_create();
  if(file){
    printf("Not implemented");
    panic();
    /* fp = fopen(str.c_str(), "r"); */
    /* if(fp){ */
    /*   std::string input; */
    /*   fseek(fp, 0, SEEK_END); */
    /*   int file_len = std::ftell(fp); */
    /*   fseek(fp, 0, SEEK_SET); */
    /*   input.resize(file_len); */
    /*   fread(&input[0], 1, file_len, fp); */
    /* } */
  } else {
    a->lines_count = strinst(str, '\n');
    a->lines = malloc(sizeof(struct table*) * (a->lines_count + 1));
    int i = 0;
    char *token = strtok(str,"\n");
    while(token != NULL){
      a->lines[i++] = table_create(token);
      token = strtok(NULL,"\n");
    }
  }
  return a;
}
void arse_delete(struct arse *a){
  for(int i = 0; i < a->lines_count; ++i){
    table_delete(a->lines[i], i == 0);
  }
  table_stack_delete(a->action_history);
  table_stack_delete(a->action_future);
  free(a->lines);
  free(a);
}
int arse_insert(struct arse *a, size_t index, char *str){
  size_t table = 0;
  size_t length = a->lines[table]->length;
  while(length < index && table < a->lines_count){
    index -= a->lines[table]->length;
    ++table;
    length += a->lines[table]->length;
  }
  table_stack_clean(a->action_future);
  table_stack_push(a->action_history, a->lines[table]);
  table_insert(a->lines[table], index, str);
  return 0;
}
int arse_insert_at_line(struct arse *a, size_t line, size_t index, char *str){
  table_stack_clean_instance(a->action_future, a->lines[line]);
  table_stack_push(a->action_history, a->lines[line]);
  table_insert(a->lines[line], index, str);
  return 0;
}
int arse_remove(struct arse *a, size_t index, size_t length){
  table_stack_clean(a->action_future);
  table_stack_push(a->action_history, a->lines[0]);
  table_remove(a->lines[0], index, length);
  return 0;
}
int arse_remove_at_line(struct arse *a, size_t line, size_t index, size_t length){
  table_stack_clean_instance(a->action_future, a->lines[line]);
  table_stack_push(a->action_history, a->lines[line]);
  table_remove(a->lines[line], index, length);
  return 0;
}
void arse_undo(struct arse *a){
  struct table *t = table_stack_pop(a->action_history);
  table_stack_push(a->action_future, t);
  table_undo(t);
}
void arse_redo(struct arse *a){
  struct table *t = table_stack_pop(a->action_future);
  table_stack_push(a->action_history, t);
  table_redo(t);
}
void arse_undo_line(struct arse *a, size_t line){
  table_stack_pop_instance(a->action_history, a->lines[line]);
  table_undo(a->lines[line]);
}
void arse_redo_line(struct arse *a, size_t line){
  table_stack_push(a->action_history, a->lines[line]);
  table_redo(a->lines[line]);
}
int arse_save(struct arse *a){
  return 0;
}
void arse_backup(struct arse *a){
}
char *arse_buffer(struct arse *a){
  char *buffer;
  size_t total_length = 0;
  for(int i = 0; i < a->lines_count; ++i){
    total_length += a->lines[i]->length;
  }
  buffer = calloc(total_length + a->lines_count, sizeof(char));
  total_length = 0;
  for(int i = 0; i < a->lines_count; ++i){
    char *t_buffer = table_buffer(a->lines[i]);
    debug("##buffer given: %s\n", t_buffer);
    strcpy(buffer + total_length, t_buffer);
    total_length += a->lines[i]->length;
    buffer[total_length++] = '\n';
    free(t_buffer);
  }
  buffer[total_length-1] = 0;
  return buffer;
}
