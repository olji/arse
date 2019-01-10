#include <string.h>
#include "stdlib.h"
#include "array.h"
#include "table.h"
#include "debug.h"
size_t strinst(char *str, char character){
  size_t occurences = 0;
  str = strchr(str, character);
  while(str != NULL){
    ++occurences;
    str = strchr(str+1, character);
  }
  return occurences;
}
struct table **table_array_insert_at(struct table **block, struct table *object, size_t index){
  size_t length = array_length(block);
  struct table **tmp  = realloc(block, (length + 1) * sizeof(struct table*));
  if(tmp != NULL){
    block = tmp;
  } else {
    return NULL;
  }
  for(size_t i = length; i > index; --i){
    struct table *obj = block[i-1];
    block[i] = obj;
  }
  block[index] = object;
  return block;
}
size_t linecount_get(char *str){
  return (str[strlen(str) - 1] == '\n') ? strinst(str, '\n') : strinst(str, '\n') + 1;
}
