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
    int piece_id;
};
class part: public piece{
    piece** pieces;
    int length;
    public:
    part();
    ~part();

    bool insert(piece *p);
    piece *next();
    piece *prev();
    piece *buffer();
    piece *split(piece *p, int index);
};
void piece_insert(piece *p, piece *n);
void piece_delete_id(piece *start, int id);
void piece_delete_chain(piece *start, piece *end);
piece *piece_split(piece* p, int index);
#endif
