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

static void parse_input_string(struct arse *a, char *input, size_t length);
static struct arse *arse_find_arse(struct arse *a, size_t line, size_t length, struct part *p);
static int arse_update_arrays(struct arse *a, struct arse *ed, size_t line);

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
void arse_init(struct arse *a){
  a->fp = NULL;
  a->masters = NULL;
  a->hosts = NULL;
  a->masters_count = 0;
  a->hosts_count = 0;
  a->action_history = table_stack_create();
  a->action_future = table_stack_create();
  a->lines_count = 0;
  a->absolute_lines_count = a->lines_count;
  a->slaves = subarse_table_create(16, &default_hashfunc);
}
int arse_open_file(struct arse *a, char *path){
  /* TODO: Optimize file load to reuse repeated string (Conf option) */
  size_t length = 0;
  a->filename = path;
  a->fp = fopen(path, "r+");
  char *str;
  if(a->fp){
    fseek(a->fp, 0, SEEK_END);
    length = ftell(a->fp);
    fseek(a->fp, 0, SEEK_SET);
    if(length == 0){
      str = calloc(2, sizeof(char));
      str[0] = 10;
    } else {
      str = calloc(length + 1, sizeof(char));
      fread(&str[0], 1, length, a->fp);
    }
    fclose(a->fp);
    a->fp = NULL;
  } else {
    return 1;
  }
  parse_input_string(a, str, length);
  return 0;
}
void arse_load_string(struct arse *a, char *str){
  parse_input_string(a, str, strlen(str));
}
void arse_delete(struct arse *a){
  for(int i = 0; i < a->lines_count; ++i){
    table_delete(a->lines[i], true);
  }
  table_stack_delete(a->action_history);
  table_stack_delete(a->action_future);
  subarse_table_delete(a->slaves);
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
  size_t length = 0;
  while(table < a->lines_count - 1 && length + a->lines[table]->length <= index){
    length += a->lines[table++]->length;
  }
  if(table >= a->lines_count){
    return 1;
  }
  table_stack_clean_instance(a->action_future, a->lines[table]);
  table_stack_push(a->action_history, a->lines[table]);
  /* TODO: table_insert should return old string content if node affected was an editor so position in arsetable can be updated */
  a->absolute_lines_count += strinst(str, '\n');
  table_insert(a->lines[table], index - length, str);
  return 0;
}
int arse_insert_at_line(struct arse *a, size_t line, size_t index, char *str){
  return arse_insert(a, arse_index_for_line(a, line, index), str);
}
int arse_remove(struct arse *a, size_t index, size_t length){
  size_t table = 0;
  size_t len = 0;
  while(table < a->lines_count - 1 && len + a->lines[table]->length < index){
    len += a->lines[table++]->length;
  }
  if(table >= a->lines_count){
    debug("Failure\n");
    return 1;
  }
  table_stack_clean_instance(a->action_future, a->lines[table]);
  table_stack_push(a->action_history, a->lines[table]);
  /* TODO: table_remove should return old string content if node affected was an editor so position in arsetable can be updated */
  table_remove(a->lines[table], index - len, length);
  return 0;
}
int arse_remove_at_line(struct arse *a, size_t line, size_t index, size_t length){
  return arse_remove(a, arse_index_for_line(a, line, index), length);
}
/* TODO: remove empty line as well */
/* TODO: Include action in undo/redo */
void arse_new_line(struct arse *a, size_t line){
  struct table *t = table_create("\n", true);
  a->lines = table_array_insert_at(a->lines, t, line);
  ++a->lines_count;
  ++a->absolute_lines_count;
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
/* TODO: Fix undo/redo for specific lines to work with tables with multiple newlines to do \n being in insertion, or just remove these functions */
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
    fp = freopen(NULL, "w", a->fp);
  } else {
    return 1;
  }
  fseek(fp, 0, SEEK_SET);
  struct arse_string *str = arse_get_string(a);
  fwrite(str->string, str->length, 1, fp);
  fflush(fp);
  a->fp = freopen(NULL, "r+", fp);
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
    debug("total_length: %zu\n", total_length);
  }
  buffer = calloc(total_length + 1, sizeof(char));
  size_t written_length = 0;
  for(int i = 0; i < a->lines_count; ++i){
    char *t_buffer = table_buffer(a->lines[i]);
    strcpy(buffer + written_length, t_buffer);
    debug("Adding %zu to written length.\n", strlen(t_buffer));
    written_length += strlen(t_buffer);
    free(t_buffer);
    /* FIXME: This free causes segfaults, check it out when insertion with newline doesn't cause undo/redo to fail */
    //free(t_buffer);
  }
  if(total_length != written_length){
    debug("Something is off: %zu -- %zu\n", total_length, written_length);
  }
  //buffer[total_length] = 0;
  struct arse_string *result = malloc(sizeof(struct arse_string));
  result->string = buffer;
  result->length = total_length;
  return result;
}
struct arse_buffer *arse_get_buffer(struct arse *a){
  struct arse_buffer *result = malloc(sizeof(struct arse_buffer));
  result->lines = calloc(a->absolute_lines_count, sizeof(char*));
  result->line_lengths = malloc(sizeof(size_t) * a->absolute_lines_count);
  for(int line = 0, abs_line = 0; line < a->lines_count; ++line){
    char *t_buffer = table_buffer(a->lines[line]);
    //struct table_info state = a->lines[line]->state;
    size_t copied = 0;
    size_t end = strlen(t_buffer);
    char *line_start = t_buffer;
    while(copied < end){
      char *line_end = strchr(line_start, '\n');
      size_t len = 0;
      if(line_end != NULL){
        line_end += 1;
        len = line_end - line_start;
      } else {
        len = strlen(line_start);
      }
      result->line_lengths[abs_line] = len;
      result->lines[abs_line] = calloc(len + 1, sizeof(char));
      if(line_end != NULL){
        strncpy(result->lines[abs_line], line_start, len);
      } else {
        strncpy(result->lines[abs_line], line_start, strlen(line_start));
      }
      copied += line_end - line_start + 1;
      line_start = line_end + 1;
      ++abs_line;
    }
    free(t_buffer);
  }
  result->lines_count = a->absolute_lines_count;
  return result;
}
static struct arse *arse_find_arse(struct arse *a, size_t line, size_t length, struct part *p){
  char *string = calloc(length + 1, sizeof(char));
  table_get_string(a->lines[line], p->first, p->last, string);
  string[length] = 0;
  struct arse *ed = subarse_table_find(a->slaves, string);
  if(ed == NULL){
    ed = malloc(sizeof(struct arse));
    arse_init(ed);
    arse_load_string(ed, string);
    subarse_table_insert(a->slaves, ed);
  } else {
    free(string);
  }
  return ed;
}
static int arse_update_arrays(struct arse *a, struct arse *ed, size_t line){
  if(ed->masters == NULL){
    ed->masters = calloc(1, sizeof(struct table*));
    ed->masters_count = 1;
  } else {
    struct table **tmp = realloc(ed->masters, ++ed->masters_count * sizeof(struct table*));
    if(tmp != NULL){
      ed->masters = tmp;
    } else {
      return 1;
    }
  }
  ed->masters[ed->masters_count - 1] = a->lines[line];
  if(ed->hosts == NULL){
    ed->hosts = calloc(1, sizeof(struct piece*));
    ed->hosts_count = 1;
  } else {
    struct piece **tmp = realloc(ed->hosts, ++ed->hosts_count * sizeof(struct piece*));
    if(tmp != NULL){
      ed->hosts = tmp;
    } else {
      return 2;
    }
  }
  return 0;
}
int arse_piece_to_arse(struct arse *a, size_t line, size_t index, size_t length, bool unique){
  struct part *p = table_get_pieces(a->lines[line], index, length); 
  struct arse *ed = arse_find_arse(a, line, length, p);
  struct piece *new = piece_create(0,length,ARSE_EDITOR);
  int ret = arse_update_arrays(a, ed, line);
  if(ret != 0){
    return ret;
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
size_t arse_index_for_line(struct arse *a, size_t line, size_t index){
  size_t table = 0;
  if(table_state_update(a->lines[table]) != 0){
    debug("Failed to update table state\n");
    return 1;
  }

  struct table_info state = a->lines[table]->state;
  size_t table_line = 0;
  size_t passed_lines = 0;
  size_t len = 0;
  while(passed_lines < line){
    len += state.line_lengths[table_line++];
    ++passed_lines;
    if(table_line >= state.line_count){
      /* Still on same line as before */
      char *str = table_buffer(a->lines[table]);
      if(str[a->lines[table]->length - 1] != '\n'){
        --passed_lines;
      }
      free(str);
      if(table_state_update(a->lines[++table]) != 0){
        return 1;
      }
      state = a->lines[table]->state;
      table_line = 0;
    }
  }
  if(index > a->lines[table]->length){
    index = a->lines[table]->length;
  }
  index = index + len;
  return index;
}
static void parse_input_string(struct arse *a, char *input, size_t length){
  //if(a->input_string != NULL){
  //  free(a->input_string);
  //}
  //a->input_string = input;
  debug("string given: %s\n", input);
  //input = malloc(strlen(input));
  //strcpy(a->input_string, input);
  a->lines_count = linecount_get(input);
  a->lines = calloc(a->lines_count, sizeof(struct table*));
  int i = 0;
  char *start, *end, *strend = input + length;
  start = end = input;
  while(end != strend){
    end = strchr(start, '\n');
    /* Control for single line input */
    if(end == NULL){
      end = strend;
    }
    /* length + newline */
    size_t len = end - start + 1;
    char *token = calloc(len + 1, sizeof(char));
    strncpy(token, start, len);
    token[len] = 0;
    a->lines[i++] = table_create(token, false);
    start = end + 1;
  }
  if(a->lines_count > i){
    --a->lines_count;
  }
  free(input);
  a->absolute_lines_count = a->lines_count;
}
