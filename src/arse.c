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
  a->fp = NULL;
  a->action_history = table_stack_create();
  a->action_future = table_stack_create();
  if(file){
    a->filename = str;
    a->fp = fopen(str, "r+");
    if(a->fp){
      fseek(a->fp, 0, SEEK_END);
      int file_len = ftell(a->fp);
      debug("file_len: %d\n", file_len);
      fseek(a->fp, 0, SEEK_SET);
      str = malloc(sizeof(char) * file_len);
      fread(&str[0], 1, file_len, a->fp);
      debug("read: %s\n", str);
      debug("hello");
    }
  }
  a->lines_count = strinst(str, '\n');
  a->lines = malloc(sizeof(struct table*) * (a->lines_count + 1));
  int i = 0;
  char *token = strtok(str,"\n");
  while(token != NULL){
    a->lines[i++] = table_create(token);
    token = strtok(NULL,"\n");
  }
  if(a->lines_count > i){
    --a->lines_count;
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
  if(a->fp != NULL){
    fclose(a->fp);
  }
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
  /* Non-line specific operations aside from undo/redo feels obsolete at this point */
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
  if(t != NULL){
    table_stack_push(a->action_future, t);
    table_undo(t);
  }
}
void arse_redo(struct arse *a){
  struct table *t = table_stack_pop(a->action_future);
  if(t != NULL){
    table_stack_push(a->action_history, t);
    table_redo(t);
  }
}
void arse_undo_line(struct arse *a, size_t line){
  table_stack_pop_instance(a->action_history, a->lines[line]);
  table_stack_push(a->action_future, a->lines[line]);
  table_undo(a->lines[line]);
}
void arse_redo_line(struct arse *a, size_t line){
  table_stack_push(a->action_history, a->lines[line]);
  table_redo(a->lines[line]);
}
int arse_save(struct arse *a, char *filename){
  FILE *fp = NULL;
  if(strcmp(filename, a->filename) == 0){
    fp = a->fp;
  } else {

  }
  fseek(fp, 0, SEEK_SET);
  struct arse_buffer *buf = arse_get_buffer(a);
  fwrite(buf->buffer, buf->length, 1, fp);
  fflush(fp);
  return 0;
}
void arse_backup(struct arse *a){
}
char *arse_get_line(struct arse *a, size_t line){
  return table_buffer(a->lines[line]);
}
struct arse_buffer *arse_get_buffer(struct arse *a){
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
    strncpy(buffer + total_length, t_buffer, a->lines[i]->length);
    total_length += a->lines[i]->length;
    buffer[total_length++] = '\n';
    free(t_buffer);
  }
  buffer[total_length-1] = 0;
  struct arse_buffer *result = malloc(sizeof(struct arse_buffer));
  result->buffer = buffer;
  result->length = total_length;
  return result;
}
}
