#include <vector>

#include "piece.hpp"

void piece_insert(piece *p, piece *n){
    n->next = p->next;
    n->next->prev = n;
    n->prev = p;
    p->next = n;
}
void piece_delete_id(piece *start, int id){
    if(start->next != 0){
        piece_delete_id(start->next, id);
    }
    if(start->piece_id == id){
        start->prev->next = start->next;
        start->next->prev = start->prev;
        delete start;
    }
}
void piece_delete_chain(piece *start, piece *end){
    if(start != end){
        piece_delete_chain(start->next, end);
    }
    delete start;
}
piece *piece_split(piece* p, int index){
    piece *n = new piece();
    piece_insert(p, n);
    n->buffer = p->buffer;
    n->start = index;
    n->length = p->length - index;
    n->start = p->start + index;
    p->length = index;
    n->piece_id = p->piece_id;
}
part::part(){
    length = 1;
}
part::~part(){
    piece_delete_chain(pieces[0], pieces[length-1]);
    delete pieces;
}
bool insert(piece *p);
piece *part::next(){}
piece *part::prev(){}
piece *part::buffer(){}
piece *part::split(piece* p, int index){
    piece *n = new piece();
    piece_insert(p, n);
    n->buffer = p->buffer;
    n->start = index;
    n->length = p->length - index;
    n->start = p->start + index;
    p->length = index;
    ++length;
}
