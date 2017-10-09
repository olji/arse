#ifndef UNDO_HPP
#define UNDO_HPP
#include <stack>
struct piece;

class undoable{
    protected:
        std::stack<piece*> history;
    public:
        void undo();
};
#endif
