#ifndef TABLE_H
#define TABLE_H

#include <stdbool.h>

#include "buffertype.h"

struct part_stack;
struct part;
struct piece;

/// @brief Basic meta-information of contents in table
struct table_info{
  size_t line_count;
  size_t *line_lengths;
  size_t arse_count;
  struct piece **arse_nodes;
};

/// @brief Piece table structure with undo/redo and text information
struct table{
  struct piece *begin;
  struct piece *end;
  char* buffers[2];
  size_t length;
  struct table_info state;

  struct part_stack *history;
  struct part_stack *future;
};

/// @brief Create a instance of table
///
/// @param input Initial string which instance will represent
/// @param static_str If true it will not try to clean up memory of the input string
///
/// @return Newly created instance of table
struct table *table_create(char *input, bool static_str);

/// @brief Clean up memory used by table instance 
///
/// @param t Instance to clean up
void table_delete(struct table *t, int free_input);

/// @brief Insert text into piece chain
///
/// @param t Table instance
/// @param index Index to start insertion
/// @param str String to insert
///
/// @return Unused, always NULL
struct piece *table_insert(struct table *t, size_t index, char *str);
struct piece *table_insert_line(struct table *t, size_t index, char *str);

/// @brief Remove continuous text section from piece chain
///
/// @param t Table instance
/// @param from Index of first character to remove
/// @param length Amount of characters to remove in total
void table_remove(struct table *t, size_t from, size_t length);

/// @brief Undo last operation
///
/// @param t Table instance
void table_undo(struct table *t);

/// @brief Redo last undone operation
///
/// @param t Table instance
void table_redo(struct table *t);

struct table *table_split(struct table *t, size_t index);

size_t table_length(struct table *t);

/// @brief Get string which table instance is representing
///
/// @param t Table instance
///
/// @return Text buffer constructed from piece chain
char *table_buffer(struct table *t);

/// @brief DEBUG METHOD: prints series of pieces from piece chain
///
/// @param t Table instance
/// @param begin Start piece
/// @param end End piece
void table_print_series(struct table *t, struct piece *begin, struct piece *end);

/// @brief DEBUG METHOD: Print piece information from piece chain
///
/// @param t Table instance
/// @param p Piece
void table_print_piece(struct table *t, struct piece *p);

/// @brief Update table_info member in table instance
///
/// @param t Table instance
///
/// @return Zero if finished without errors
int table_state_update(struct table *t);

/// @brief Get container of a section of piece chain
///
/// @param t Table instance
/// @param from Index to start container from, split piece if necessary
/// @param length Length to cover, split piece if necessary
///
/// @return Part instance which covers the range specified
struct part *table_get_pieces(struct table *t, size_t from, size_t length);

/// @brief Get representation of pieces in piece chain section
///
/// @param t Table instance
/// @param start Beginning piece
/// @param end Last piece
/// @param result Char array to write results to
void table_get_string(struct table *t, struct piece *start, struct piece *end, char *result);

/// @brief Replace series of pieces with one piece instance
///
/// @param t Table instance
/// @param start Start of piece series to be replaced
/// @param end End of piece series to be replaced
/// @param replacement Piece to replace piece series with
void table_replace_pieces(struct table *t, struct piece *start, struct piece *end, struct piece *replacement);

/// @brief Turn piece containing index character to an arse instance
///
/// @param t Table instance
/// @param index Index of string of the piece chain
///
/// @return 
int table_piece_to_arse(struct table *t, size_t index);

#endif /* TABLE_H */
