#include <iostream>
#include <vector>

#include "piece.hpp"
#include "table.hpp"

table::table(std::string input){
    begin = new piece();
    begin->start = 0;
    begin->length = 0;
    begin->buffer = NONE;
    end = new piece();
    end->start = 0;
    end->length = 0;
    end->buffer = NONE;
    piece *p = new piece();
    p->start = 0;
    p->buffer = SOURCE;
    p->length = input.length();
    p->prev = begin;
    p->next = end;
    begin->next = p;
    end->prev = p;
}
table::~table(){
    piece_delete_chain(begin, end);
}
void table::insert(int index, int start, int length){
    std::cout << index << " -- " << start << std::endl;
    piece *p = begin;
    piece *n = new piece();
    n->piece_id = (history.empty() ? 1 : history.top()->piece_id + 1);
    history.push(n);
    int distance = p->length;
    std::cout << "distance: " << distance << std::endl;
    while(distance < index || p == end){
        p = p->next;
        distance += p->length;
        std::cout << "distance: " << distance << std::endl;
    }
    index = index - (distance - p->length);
    n->length = length;
    n->start = start;
    n->buffer = CHANGE;
    if(p->length != index){
        piece_split(p, index);
    }
    piece_insert(p, n);
    p->length = index;
}
void table::remove(int from, int to){
}
void table::redo(){
}
std::vector<piece*> table::get_pieces(){
    std::vector<piece*> pieces;
    piece *p = begin->next;
    while(p != end){
        pieces.push_back(p);
        p = p->next;
    }
    return pieces;
}
