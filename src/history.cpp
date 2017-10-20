#include <iostream>
#include "piece.hpp"
#include "history.hpp"

void history::undo(){
    if(!history.empty()){
        part *p = history.top();
        history.pop();
        future.push(part_create(p->first->prev->next, p->last->next->prev, false));
        p->first->prev->next = p->first;
        p->last->next->prev = p->last;
        part_delete(p, false);
    } else {
        std::cout << "Buffer already at oldest state" << std::endl;
    }
}
void history::redo(){
    if(!future.empty()){
        part *p = future.top();
        future.pop();
        history.push(part_create(p->first->prev->next, p->last->next->prev, false));
        p->first->prev->next = p->first;
        p->last->next->prev = p->last;
        part_delete(p, false);
    } else {
        std::cout << "Buffer already at newest state" << std::endl;
    }
}
