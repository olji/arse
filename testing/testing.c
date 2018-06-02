#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testing.h"
#include "arse.h"
int assert(struct arse_string *input, char *expected){
  int ret = 0;
  if(strcmp(input->string, expected)){
    fprintf(stderr, "Expected: \"%s\"\n", expected);
    fprintf(stderr, "Actual: \"%s\"\n", input->string);
    ret = 1;
  }
  arse_string_delete(input);
  return ret;
}
