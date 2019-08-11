#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testing.h"
#include "arse.h"
int assert(struct arse *a, char *expected){
  int ret = 0;
  arse_buffer_delete(arse_get_buffer(a));
  struct arse_string *input = arse_get_string(a);
  if(strcmp(input->string, expected)){
    fprintf(stderr, "Expected: \"%s\"\n", expected);
    fprintf(stderr, "Actual: \"%s\"\n", input->string);
    ret = 1;
    fprintf(stderr, "Press enter to continue...");
    getc(stdin);
  } else {
    fprintf(stderr, "Got expected: \"%s\"\n", expected);
  }
  arse_string_delete(input);
  return ret;
}
