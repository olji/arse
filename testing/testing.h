#ifndef TEST_H
#define TEST_H
struct arse;
int assert(struct arse *a, char *expected);
int check(struct arse *a, char *expected);
void test(int (*fn)());
void print_test_result();
struct arse* test_init_empty();
struct arse* test_init_loaded();
void test_cleanup(struct arse *a);
#endif /* TEST_H */
