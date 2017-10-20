#ifndef PIECE_HPP
#define PIECE_HPP

enum buffertype{
    NONE=0,
    SOURCE,
    CHANGE
};
struct piece{
    piece *next;
    piece *prev;
    buffertype buffer;
    int start;
    int length;
};
struct part{
    piece* first;
    piece* last;
    int length;
};
void piece_insert(piece *p, piece *n);
piece *piece_copy(piece *p);
void piece_delete_chain(piece *start, piece *end);
piece *piece_split(piece* p, int index);
int part_length(part* p);
void piece_print(piece *p);
part *part_create(piece *from, piece *to, bool copy=true);
void part_delete(part *p, bool content = true);
#endif
