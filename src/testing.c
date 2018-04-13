#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testing.h"
#include "panic.h"
#include "debug.h"
int assert(char *input, char *expected){
  if(strcmp(input, expected)){
    fprintf(stderr, "Expected: \"%s\"\n", expected);
    fprintf(stderr, "Actual: \"%s\"\n", input);
    free(input);
    return 1;
  }
  free(input);
  return 0;
}
