#ifndef ARRAY_H
#define ARRAY_H
void *array_increase(void *block, size_t typesize, int amount);
void *array_double(void *block, size_t typesize);
size_t array_size(void *block, size_t typesize);
size_t strinst(char *str, char character);
#endif /* ARRAY_H */
