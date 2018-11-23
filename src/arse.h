#ifndef ARSE_H
#define ARSE_H
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
struct piece;
struct table;
struct table_stack;
struct subarse_table;

enum backup_scheme{
  ARSE_BKUP_NONE = 0,
  ARSE_BKUP_CMD,
  ARSE_BKUP_TIME
};
struct arse_config{
  enum backup_scheme backup;
  /* Seconds or commands */
  size_t interval;
  /* Directory to put backup files */
  char *bkup_dir;
};
struct arse_string{
  char *string;
  size_t length;
};
struct arse_buffer{
  char **lines;
  size_t *line_lengths;
  size_t lines_count;
};
struct arse{
  /* TODO: some structure for keeping track of extra lines inside subarses */
  struct table **lines;
  struct table **masters;
  struct piece **hosts;
  struct table_stack *action_history;
  struct table_stack *action_future;
  struct subarse_table *slaves;
  size_t lines_count;
  size_t absolute_lines_count;
  size_t masters_count;
  size_t hosts_count;
  char *filename;
  char *input_string;
  FILE *fp;
};

void arse_init(struct arse *a);
int arse_open_file(struct arse *a, char *path);
void arse_load_string(struct arse *a, char *str);
void arse_delete(struct arse *a);
int arse_insert(struct arse *a, size_t index, char *str);
int arse_insert_at_line(struct arse *a, size_t line, size_t index, char *str);
int arse_remove(struct arse *a, size_t index, size_t length);
int arse_remove_at_line(struct arse *a, size_t line, size_t index, size_t length);
void arse_undo(struct arse *a);
void arse_redo(struct arse *a);
void arse_undo_line(struct arse *a, size_t line);
void arse_redo_line(struct arse *a, size_t line);
char *arse_get_line(struct arse *a, size_t line);
int arse_piece_to_arse(struct arse *a, size_t line, size_t index, size_t length, bool force);
int arse_save(struct arse *a, char *filename);
void arse_backup(struct arse *a);
void arse_buffer_delete(struct arse_buffer *a);
void arse_string_delete(struct arse_string *a);
struct arse_buffer *arse_get_buffer(struct arse *a);
struct arse_string *arse_get_string(struct arse *a);
#endif /* ARSE_H */
