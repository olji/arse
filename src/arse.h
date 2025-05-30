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

/// @brief Main editor and sub-editor structure
struct arse{
  // Not entirely clear what this is tracking, seems like hosts[i]->master? But haven't found assignment for that
  //struct table **masters;
  //size_t masters_count;
  // Host pieces that are referring to this as a sub-editor
  //struct piece **hosts;
  //struct table_stack *action_history;
  //struct table_stack *action_future;
  //struct subarse_table *slaves;
  //size_t hosts_count;
  //char *filename;
  char *buffers[2]; // buffertype ARSE_ORIGINAL and ARSE_CHANGE
  struct piece *begin;
  struct piece *end;
  //size_t length;
  //struct table_info state;

  //struct part_stack *history;
  //struct part_stack *future;
};

/// @brief Initialise an arse instance at supplied memory space
///
/// @param a Memory address to initialise
void arse_init(struct arse *a);

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

/// @brief Remove continuous sequence of character
///
/// @param a Arse instance
/// @param index Index to start removal from
/// @param length Length of sequence
///
/// @return Zero on success, non-zero on failure
int arse_remove(struct arse *a, size_t index, size_t length);

/// @brief Undo last action
///
/// This will remove the last action of the most recently modified
/// table
///
/// @param a Arse instance
void arse_undo(struct arse *a);

/// @brief Redo last action
///
/// This will redo the last undone action of the most recently
/// undone table
///
/// @param a Arse instance
void arse_redo(struct arse *a);

/// @brief Convert piece sequence for text sequence into a sub-editor arse instance
///
/// @param a Main arse instance
/// @param line Line index of table to modify
/// @param index Text index start point of conversion
/// @param length Length of text sequence
/// @param unique Force creation of new instance if duplicate is found, instead of sharing same instance
///
/// @return Zero on success, non-zero on failure
int arse_piece_to_arse(struct arse *a, size_t index, size_t length, bool unique);

/// @brief Save backup file
///
/// @param a Arse instance
void arse_backup(struct arse *a);

/// @brief Clean up arse string structure
///
/// @param a Arse string instance
void arse_string_delete(struct arse_string *a);

/// @brief Get plain string managed by arse instance
///
/// @param a Arse instance
///
/// @return Current string handled by arse instance
struct arse_string *arse_get_string(struct arse *a);
#endif /* ARSE_H */
