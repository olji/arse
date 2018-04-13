#include <stdlib.h>
#include <string.h>
#include "arse.h"
#include "table.h"
#include "array.h"

#include "debug.h"
#include "panic.h"

struct arse *arse_create(char *str, int file){
  struct arse *a = malloc(sizeof(struct arse));
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
  free(a->lines);
  free(a);
}
int arse_insert(struct arse *a, int index, char *str){
  int table = 0;
  size_t length = a->lines[table]->length;
  while(length < index && table < a->lines_count){
    ++table;
    length += a->lines[table]->length;
  }
  table_insert(a->lines[table], index, str);
  return 0;
}
int arse_remove(struct arse *a, int index, int length){
  table_remove(a->lines[0], index, length);
  return 0;
}
void arse_undo(struct arse *a){
  table_undo(a->lines[0]);
}
void arse_redo(struct arse *a){
  table_redo(a->lines[0]);
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
    strcpy(buffer + total_length, t_buffer);
    total_length += a->lines[i]->length;
    buffer[total_length++] = '\n';
    free(t_buffer);
  }
  buffer[total_length-1] = 0;
  return buffer;
}
