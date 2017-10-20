#include <iostream>
#include <exception>
#include <vector>

#include "piece.hpp"

void piece_insert(piece *p, piece *n){
    n->next = p->next;
    n->next->prev = n;
    n->prev = p;
    p->next = n;
}
void piece_delete_chain(piece *start, piece *end){
    if(!start){
        throw new std::exception();
    }
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
    return n;
}
piece *piece_copy(piece *p){
    piece *copy = new piece;
    copy->next = p->next;
    copy->prev = p->prev;
    copy->start = p->start;
    copy->length = p->length;
    copy->buffer = p->buffer;
    return copy;
}
part *part_create(piece *from, piece *to, bool copy){
    part *n = new part;
    if(copy){
        n->first = piece_copy(from);
        piece *tmp = n->first;
        while(from != to){
            tmp->next = piece_copy(tmp->next);
            tmp->next->prev = tmp;
            tmp = tmp->next;
            from = from->next;
        }
        n->last = tmp;
    } else {
        n->first = from;
        n->last = to;
    }
    n->length = part_length(n);
    return n;
}
void part_delete(part *p, bool content){
    if(content){
        piece_delete_chain(p->first, p->last);
    }
    delete p;
}
int part_length(part *p){
    piece *temp = p->first;
    int length = temp->length;
    while(temp != p->last){
        temp = temp->next;
        length + temp->length;
    }
    return length;
}
void piece_print(piece *p){
    /*
     *std::cout << "[" << p->buffer << "] -- " << p->start << " -- " << p->length << std::endl;
     */
}
