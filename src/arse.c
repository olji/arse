#include <stdlib.h>
#include <string.h>
#include "arse.h"
#include "subarsetable.h"
#include "table.h"
#include "array.h"
#include "stack.h"
#include "part.h"
#include "piece.h"

#include "debug.h"
#include "panic.h"

size_t default_hashfunc(char *str, size_t mod){
  size_t pos = 0;
  for (int i=0; str[i] != '\0'; ++i){
    pos += str[i];
    pos += pos << 2;
    pos -= pos >> 3;
    pos ^= pos << 4;
  }
  return pos % mod;
}

struct arse *arse_create(char *str, int file){
  struct arse *a = malloc(sizeof(struct arse));
  a->fp = NULL;
  a->masters = NULL;
  a->hosts = NULL;
  a->masters_count = 0;
  a->hosts_count = 0;
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
  char *start, *end, *strend = str + strlen(str) - 1;
  start = end = str;
  /* Control for single line input */
  while(end != strend){
    end = strchr(start, '\n');
    if(end == NULL){
      end = strend;
    }
    char *token = calloc(end - start + 2, sizeof(char));
    strncpy(token, start, end - start + 1);
    a->lines[i++] = table_create(token);
    start = end + 1;
  }
  if(a->lines_count > i){
    --a->lines_count;
  }
  a->anaas = subarse_table_create(16, &default_hashfunc);
  return a;
}
void arse_delete(struct arse *a){
  for(int i = 0; i < a->lines_count; ++i){
    table_delete(a->lines[i], i == 0);
  }
  table_stack_delete(a->action_history);
  table_stack_delete(a->action_future);
  subarse_table_delete(a->anaas);
  free(a->lines);
  if(a->masters != NULL){
    free(a->masters);
  }
  if(a->hosts != NULL){
    free(a->hosts);
  }
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
    length += a->lines[table++]->length;
  }
  if(table >= a->lines_count){
    debug("Failure\n");
    return 1;
  }
  table_stack_clean_instance(a->action_future, a->lines[table]);
  table_stack_push(a->action_history, a->lines[table]);
  /* TODO: table_insert should return old string content if node affected was an editor so position in arsetable can be updated */
  table_insert(a->lines[table], index, str);
  return 0;
}
int arse_insert_at_line(struct arse *a, size_t line, size_t index, char *str){
  size_t table = 0;
  size_t length = 0;
  if(index > a->lines[line]->length){
    index = a->lines[line]->length;
  }
  while(table < line){
    length += a->lines[table++]->length;
  }
  return arse_insert(a, index + length, str);
}
int arse_remove(struct arse *a, size_t index, size_t length){
  size_t table = 0;
  size_t len = a->lines[table]->length;
  while(len < index && table < a->lines_count){
    index -= a->lines[table]->length;
    len += a->lines[table++]->length;
  }
  if(table >= a->lines_count){
    return 1;
  }
  table_stack_clean_instance(a->action_future, a->lines[table]);
  table_stack_push(a->action_history, a->lines[table]);
  /* TODO: table_remove should return old string content if node affected was an editor so position in arsetable can be updated */
  table_remove(a->lines[table], index, length);
  return 0;
}
int arse_remove_at_line(struct arse *a, size_t line, size_t index, size_t length){
  size_t table = 0;
  size_t len = 0;
  if(index > a->lines[line]->length){
    index = a->lines[line]->length;
  }
  while(table < line){
    len += a->lines[table++]->length;
  }
  return arse_remove(a, index + len, length);
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
  struct arse_string *str = arse_get_string(a);
  fwrite(str->string, str->length, 1, fp);
  fflush(fp);
  return 0;
}
void arse_backup(struct arse *a){
}
char *arse_get_line(struct arse *a, size_t line){
  return table_buffer(a->lines[line]);
}
struct arse_string *arse_get_string(struct arse *a){
  char *buffer;
  size_t total_length = 0;
  for(int i = 0; i < a->lines_count; ++i){
    total_length += a->lines[i]->length;
  }
  buffer = calloc(total_length + 1, sizeof(char));
  total_length = 0;
  for(int i = 0; i < a->lines_count; ++i){
    char *t_buffer = table_buffer(a->lines[i]);
    debug("##buffer given: %s\n", t_buffer);
    strncpy(buffer + total_length, t_buffer, strlen(t_buffer));
    total_length += strlen(t_buffer);
    free(t_buffer);
  }
  buffer[total_length] = 0;
  struct arse_string *result = malloc(sizeof(struct arse_string));
  result->string = buffer;
  result->length = total_length;
  return result;
}
struct arse_buffer *arse_get_buffer(struct arse *a){
  struct arse_buffer *result = malloc(sizeof(struct arse_buffer));
  result->lines_count = a->lines_count;
  result->lines = malloc(sizeof(char*) * a->lines_count);
  result->line_lengths = malloc(sizeof(size_t*) * a->lines_count);
  for(int i = 0; i < a->lines_count; ++i){
    char *t_buffer = table_buffer(a->lines[i]);
    result->line_lengths[i] = strlen(t_buffer);
    result->lines[i] = malloc(strlen(t_buffer) * sizeof(char));
    debug("##buffer given: %s\n", t_buffer);
    strcpy(result->lines[i], t_buffer);
    free(t_buffer);
  }
  return result;
}
int arse_piece_to_arse(struct arse *a, size_t line, size_t index, size_t length, bool force){
  struct part *p = table_get_pieces(a->lines[line], index, length); 
  char *string = calloc(length + 1, sizeof(string));
  table_get_string(a->lines[line], p->first, p->last, string);
  debug("arse string: [%s]\n", string);
  struct arse *ed = subarse_table_find(a->anaas, string);
  if(ed == NULL){
    ed = arse_create(string, false);
    subarse_table_insert(a->anaas, ed);
  } else {
    free(string);
  }
  struct piece *new = piece_create(0,length,ARSE_EDITOR);
  if(ed->masters == NULL){
    ed->masters = malloc(sizeof(struct table*));
    ed->masters_count = 1;
  } else {
    struct table** tmp = realloc(ed->masters, sizeof(struct table*)*(++ed->masters_count));
    if(tmp != NULL){
      ed->masters = tmp;
    } else {
      return 1;
    }
  }
  ed->masters[ed->masters_count - 1] = a->lines[line];
  if(ed->hosts == NULL){
    ed->hosts = malloc(sizeof(struct piece*));
    ed->hosts_count = 1;
  } else {
    struct piece** tmp = realloc(ed->hosts, sizeof(struct piece*)*(++ed->hosts_count));
    if(tmp != NULL){
      ed->hosts = tmp;
    } else {
      return 2;
    }
  }
  ed->hosts[ed->hosts_count - 1] = new;
  new->arse = ed;
  new->master = a->lines[line];
  table_replace_pieces(a->lines[line], p->first, p->last, new);
  part_delete(p, false);
  return 0;
}

void arse_buffer_delete(struct arse_buffer *a){
  for(int i = 0; i < a->lines_count; ++i){
    free(a->lines[i]);
  }
  free(a->lines);
  free(a->line_lengths);
  free(a);
}
void arse_string_delete(struct arse_string *a){
  free(a->string);
  free(a);
}
