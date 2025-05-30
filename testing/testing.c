#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "testing.h"
#include "arse.h"
static int test_executed = 0;
static int test_passed = 0;
int assert(struct arse *a, char *expected){
  int ret = 0;
  struct arse_string *input = arse_get_string(a);
  if(strcmp(input->string, expected)){
    fprintf(stderr, "\033[0;31mExpected: \"%s\", Actual: \"%s\"\033[0m\n", expected, input->string);
  } else {
    fprintf(stderr, "\033[0;32mPassed\033[0m\n");
    ret = 1;
  }
  arse_string_delete(input);
  return ret;
}
int check(struct arse *a, char *expected){
  int ret = 0;
  struct arse_string *input = arse_get_string(a);
  int result = strcmp(input->string, expected);
  if(result){
    fprintf(stderr, "\033[0;31mTest failed during setup, other functionality is broken\033[0m\n");
  }
  arse_string_delete(input);
  return result;
}

void test(int (*fn)()){
  ++test_executed;
  test_passed += fn();
}
void print_test_result(){
  if(test_passed != test_executed){
    fprintf(stderr, "\033[0;31m");
  } else {
    fprintf(stderr, "\033[0;32m");
  }
  fprintf(stderr, "\n%d/%d tests passed\033[0m\n", test_passed, test_executed);
}
struct arse* test_init_empty(){
  struct arse *editor = malloc(sizeof(struct arse));
  arse_init(editor);
  return editor;
}
struct arse* test_init_loaded(char *str){
  struct arse *editor = malloc(sizeof(struct arse));
  arse_init(editor);
  arse_load_string(editor, str);
  return editor;
}
void test_cleanup(struct arse *a){
  arse_delete(a);
}
