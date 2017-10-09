#include <iostream>
#include "piece.hpp"
#include "undo.hpp"

void undoable::undo(){
    piece *p = history.top();
    piece_delete_id(p, p->piece_id);
    history.pop();
}
