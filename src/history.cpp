#include <iostream>
#include "piece.hpp"
#include "history.hpp"

/* FIXME: Undo/redo causes memory leaks */
void history::undo(){
    part *p = history.top();
    history.pop();
    future.push(part_create(p->first->prev->next, p->last->next->prev));
    p->first->prev->next = p->first;
    p->last->next->prev = p->last;
    part_delete(p, false);
}
void history::redo(){
    part *p = future.top();
    future.pop();
    history.push(part_create(p->first->prev->next, p->last->next->prev));
    p->first->prev->next = p->first;
    p->last->next->prev = p->last;
    part_delete(p, false);
}
