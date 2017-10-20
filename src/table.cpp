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
    begin->prev = begin;
    end->prev = p;
    end->next = end;
}
table::~table(){
    piece_delete_chain(begin, end);
    while(!history.empty()){
        part *p = history.top();
        part_delete(p);
        history.pop();
    }
    while(!future.empty()){
        part *p = future.top();
        part_delete(p);
        future.pop();
    }
}
void table::insert(int index, int start, int length){
    piece *p = begin;
    piece *n = new piece();
    int distance = p->length;
    while(distance < index || p == end){
        p = p->next;
        distance += p->length;
    }
    index = index - (distance - p->length);
    n->length = length;
    n->start = start;
    n->buffer = CHANGE;
    bool even = p->length == index;
    part *range;
    if(even){
        range = part_create(p, p);
    } else {
        range = part_create(p, p->next);
    }
    history.push(range);

    if(!even){
        piece_split(p, index);
    }
    piece_insert(p, n);
    p->length = index;
}
void table::remove(int from, int length){
    piece *first = begin;
    int distance = first->length;
    while(distance < from || first == end){
        first = first->next;
        distance += first->length;
    }
    from = from - (distance - first->length);
    piece *last = first;
    distance = last->length - from;
    while(distance < length || last == end){
        last = last->next;
        distance += last->length;
    }
    std::cout << "Length: " << length << ", distance: " << distance << ", last->length: " << last->length << std::endl;
    piece *part_start, *part_end;
    part_start = (from == first->length) ? first->next : first;
    int len = (distance - last->length);
    if(len < 0){
        len = 0;
    } else {
        len = length - len;
    }
    part_end = (len == last->length) ? last->next : last;
    piece_print(part_start);
    piece_print(part_end);
    history.push(part_create(part_start, part_end));
    piece *del_start, *del_end;
    del_start = (part_start == first) ? piece_split(first, from) : part_start;
    if(part_start == part_end){
        std::cout << "Same" << std::endl;
        piece_print(del_start->next);
        del_end = piece_split(del_start, len)->prev;
    } else {
        std::cout << "Diff" << std::endl;
        del_end = ((part_end == last) ? piece_split(last, len) : part_end)->prev;
    }
    piece_print(del_start);
    piece_print(del_end);
    piece_print(del_end->next);
    del_start->prev->next = del_end->next;
    del_end->next->prev = del_start->prev;
    piece_print(del_start->prev->next);
    piece_print(del_end->next->prev);
    piece_delete_chain(del_start, del_end);
}
std::vector<piece*> table::get_pieces(){
    std::vector<piece*> pieces;
    piece *p = begin;
    while(p != end){
        pieces.push_back(p);
        p = p->next;
    }
    pieces.push_back(end);
    return pieces;
}
