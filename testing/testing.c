#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testing.h"
#include "arse.h"
int assert(struct arse_buffer *input, char *expected){
  if(strcmp(input->buffer, expected)){
    fprintf(stderr, "Expected: \"%s\"\n", expected);
    fprintf(stderr, "Actual: \"%s\"\n", input->buffer);
    free(input);
    return 1;
  }
  free(input);
  return 0;
}
