#ifndef UNDO_HPP
#define UNDO_HPP
#include <stack>
struct part;

class history{
    protected:
        std::stack<part*> history;
        std::stack<part*> future;
    public:
        void undo();
        void redo();
};
#endif
