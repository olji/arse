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

/// @brief A string with length supplied
struct arse_string{
  char *string;
  size_t length;
};

/// @brief All lines handled by arse instance with meta-information
struct arse_buffer{
  char **lines;
  size_t *line_lengths;
  size_t lines_count;
};

/// @brief Main editor and sub-editor structure
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

/// @brief Initialise an arse instance at supplied memory space
///
/// @param a Memory address to initialise
void arse_init(struct arse *a);

/// @brief Open and load content from file into arse instance
///
/// @param a Arse instance
/// @param path Path to file
///
/// @return Zero on success, non-zero on failure
int arse_open_file(struct arse *a, char *path);

/// @brief Load arse instance with a basic string
///
/// @param a Arse instance
/// @param str Input string
void arse_load_string(struct arse *a, char *str);

/// @brief Clean up memory space of arse instance
///
/// @param a Arse instance
void arse_delete(struct arse *a);

/// @brief Insert string at index
///
/// @param a Arse instance
/// @param index Character index to start insertion at
/// @param str String to insert
///
/// @return Zero on success, non-zero on failure
int arse_insert(struct arse *a, size_t index, char *str);

/// @brief Insert string at index on line
///
/// @param a Arse instance
/// @param line Line to insert in
/// @param index Character index at specificed line
/// @param str String to insert
///
/// @return Zero on success, non-zero on failure
int arse_insert_at_line(struct arse *a, size_t line, size_t index, char *str);

/// @brief Remove continuous sequence of character
///
/// @param a Arse instance
/// @param index Index to start removal from
/// @param length Length of sequence
///
/// @return Zero on success, non-zero on failure
int arse_remove(struct arse *a, size_t index, size_t length);

/// @brief Remove continuous sequence of character at line
///
/// @param a Arse instance
/// @param line Line to remove from
/// @param index Index to start removal from at specified line
/// @param length Length of sequence
///
/// @return Zero on success, non-zero on failure
int arse_remove_at_line(struct arse *a, size_t line, size_t index, size_t length);

/// @brief Create a new, empty line
///
/// @param a Arse instance
/// @param line Position to add line at
void arse_new_line(struct arse *a, size_t line);

/// @brief Undo last action
///
/// This will remove the last action of the most recently modified
/// table
///
/// @param a Arse instance
void arse_undo(struct arse *a);

/// @brief Get absolute index (starting at 0 from first line)
///
/// @param a Arse instance
/// @param line Line which index is relative to
/// @param index Relative index to line
///
/// @return Absolute index
size_t arse_index_for_line(struct arse *a, size_t line, size_t index);

/// @brief Redo last action
///
/// This will redo the last undone action of the most recently
/// undone table
///
/// @param a Arse instance
void arse_redo(struct arse *a);

/// @brief Undo specific line
///
/// Undos the last action on specific line
///
/// @param a Arse instance
/// @param line Line to undo
void arse_undo_line(struct arse *a, size_t line);

/// @brief Redo specific line
///
/// Redo the last undone action on specific line
///
/// @param a Arse instance
/// @param line Line to redo
void arse_redo_line(struct arse *a, size_t line);

/// @brief Get string of specific line
///
/// @param a Arse instance
/// @param line Line to fetch string from
///
/// @return String found at given line index
char *arse_get_line(struct arse *a, size_t line);

/// @brief Convert piece sequence for text sequence into a sub-editor arse instance
///
/// @param a Main arse instance
/// @param line Line index of table to modify
/// @param index Text index start point of conversion
/// @param length Length of text sequence
/// @param unique Force creation of new instance if duplicate is found, instead of sharing same instance
///
/// @return 
int arse_piece_to_arse(struct arse *a, size_t line, size_t index, size_t length, bool unique);

/// @brief Write buffer to file
///
/// @param a Arse instance
/// @param filename Path to target file
///
/// @return Zero on success
int arse_save(struct arse *a, char *filename);

/// @brief Save backup file
///
/// @param a Arse instance
void arse_backup(struct arse *a);

/// @brief Clean up arse buffer structure
///
/// @param a Arse buffer instance
void arse_buffer_delete(struct arse_buffer *a);

/// @brief Clean up arse string structure
///
/// @param a Arse string instance
void arse_string_delete(struct arse_string *a);

/// @brief Get buffer information of arse instance
///
/// @param a Arse instance
///
/// @return Arse buffer structure containing buffer information
struct arse_buffer *arse_get_buffer(struct arse *a);

/// @brief Get plain string managed by arse instance
///
/// @param a Arse instance
///
/// @return Current string handled by arse instance
struct arse_string *arse_get_string(struct arse *a);
#endif /* ARSE_H */
