#include <string.h>
#include "stdlib.h"
#include "array.h"
void *array_increase(void *block, size_t typesize, int amount){
  return 0;
}
void *array_double(void *block, size_t typesize){
  return 0;
}
size_t array_size(void *block, size_t typesize){
  size_t i = 0;
  while(block + (typesize * i) != 0){
    ++i;
  }
  return i;
}
size_t strinst(char *str, char character){
  size_t occurences = 1;
  str = strchr(str, character);
  while(str != NULL){
    ++occurences;
    str = strchr(str+1, character);
  }
  return occurences;
}
